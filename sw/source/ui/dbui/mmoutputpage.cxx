/*************************************************************************
 *
 *  $RCSfile: mmoutputpage.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2005-03-01 15:27:04 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif

#pragma hdrstop
#ifndef _MAILMERGEOUTPUTPAGE_HXX
#include <mmoutputpage.hxx>
#endif
#ifndef _MAILMERGEWIZARD_HXX
#include <mailmergewizard.hxx>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif
#ifndef _SWMAILMERGECHILDWINDOW_HXX
#include <mailmergechildwindow.hxx>
#endif
#ifndef _MAILCONFIGPAGE_HXX
#include <mailconfigpage.hxx>
#endif
#include <cmdid.h>
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _SVX_SCRIPTTYPEITEM_HXX
#include <svx/scripttypeitem.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX 
#include <svx/langitem.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFX_DINFDLG_HXX
#include <sfx2/dinfdlg.hxx>
#endif
#ifndef _SFX_PRINTER_HXX
#include <sfx2/printer.hxx>
#endif
#ifndef _SFX_FCONTNR_HXX
#include <sfx2/fcontnr.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFXDISPATCH_HXX
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX 
#include <sfx2/docfilt.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _VCL_PRINT_HXX
#include <vcl/print.hxx>
#endif
#ifndef _UNOTOOLS_TEMPFILE_HXX
#include <unotools/tempfile.hxx>
#endif
#include <osl/file.hxx>
#ifndef _MAILMERGEGREETINGSPAGE_HXX
#include <mmgreetingspage.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XCOLUMN_HPP_
#include <com/sun/star/sdb/XColumn.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _BASEDLGS_HXX 
#include <sfx2/basedlgs.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _SWUNOHELPER_HXX
#include <swunohelper.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_ 
#include <vos/mutex.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _SVX_HTMLCFG_HXX
#include <svx/htmlcfg.hxx>
#endif
#ifndef _RTL_TENCINFO_H
#include <rtl/tencinfo.h>
#endif
#include <mmoutputpage.hrc>
#include <dbui.hrc>
#include <helpid.h>

using namespace svt;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

#define C2U(cChar) ::rtl::OUString::createFromAscii(cChar)
/*-- 01.07.2004 16:47:49---------------------------------------------------

  -----------------------------------------------------------------------*/
String lcl_GetExtensionForDocType(ULONG nDocType)
{
    String sExtension;
    switch( nDocType )
    {        
        case MM_DOCTYPE_OOO : sExtension = String::CreateFromAscii( "oxt" ); break;
        case MM_DOCTYPE_PDF : sExtension = String::CreateFromAscii( "pdf" ); break;
        case MM_DOCTYPE_WORD: sExtension = String::CreateFromAscii( "doc" ); break;
        case MM_DOCTYPE_HTML: sExtension = String::CreateFromAscii( "html" ); break;
        case MM_DOCTYPE_TEXT: sExtension = String::CreateFromAscii( "txt" ); break;
    }
    return sExtension;
}            
/*-- 28.06.2004 11:49:21---------------------------------------------------

  -----------------------------------------------------------------------*/
::rtl::OUString lcl_GetColumnValueOf(const ::rtl::OUString& rColumn, Reference < container::XNameAccess>& rxColAccess )
{
    ::rtl::OUString sRet;
    if(rxColAccess->hasByName(rColumn))
    {
        Any aCol = rxColAccess->getByName(rColumn);
        Reference< sdb::XColumn > xColumn;
        aCol >>= xColumn;
        if(xColumn.is())
            sRet = xColumn->getString();
    }
    return sRet;
}
/*-- 21.06.2004 14:01:13---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwSaveWarningBox_Impl : public ModalDialog
{
    FixedImage      aWarningImageIM;
    FixedInfo       aWarningFI;

    FixedText       aFileNameFT;
    Edit            aFileNameED;
    
    FixedLine       aSeparatorFL;
    OKButton        aOKPB;
    CancelButton    aCancelPB;

    DECL_LINK( ModifyHdl, Edit*);
public:
    SwSaveWarningBox_Impl(Window* pParent, const String& rFileName);
    ~SwSaveWarningBox_Impl();

    String          GetFileName() const {return aFileNameED.GetText();}
};
/*-- 02.07.2004 08:54:42---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwSendQueryBox_Impl : public ModalDialog
{
    FixedImage      aQueryImageIM;
    FixedInfo       aQueryFI;

    Edit            aTextED;
    
    FixedLine       aSeparatorFL;
    OKButton        aOKPB;
    CancelButton    aCancelPB;

    bool            bIsEmptyAllowed;
    DECL_LINK( ModifyHdl, Edit*);
public:
    SwSendQueryBox_Impl(Window* pParent, const String& rQueryText);
    ~SwSendQueryBox_Impl();

    void            SetValue(const String& rSet) 
                        {
                            aTextED.SetText(rSet);
                            ModifyHdl( &aTextED );
                        }
    String          GetValue() const {return aTextED.GetText();}

    void            SetIsEmptyTextAllowed(bool bSet) 
                        {
                            bIsEmptyAllowed = bSet;
                            ModifyHdl( &aTextED );
                        }
};

/*-- 21.06.2004 14:11:58---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSaveWarningBox_Impl::SwSaveWarningBox_Impl(Window* pParent, const String& rFileName) :
    ModalDialog(pParent, SW_RES(   DLG_MM_SAVEWARNING )),
    aWarningImageIM(this,   ResId( IM_WARNING   )),
    aWarningFI(this,        ResId( FI_WARNING   )),
    aFileNameFT(this,       ResId( FT_FILENAME  )),             
    aFileNameED(this,       ResId( ED_FILENAME  )),             
    aSeparatorFL(this,      ResId( FL_SEPARATOR )),            
    aOKPB(this,             ResId( PB_OK        )),
    aCancelPB(this,         ResId( PB_CANCEL    ))
{
    FreeResource();
    aWarningImageIM.SetImage(WarningBox::GetStandardImage());
    aFileNameED.SetText(rFileName);
    aFileNameED.SetModifyHdl(LINK(this, SwSaveWarningBox_Impl, ModifyHdl));
    ModifyHdl( &aFileNameED );
}
/*-- 21.06.2004 14:11:58---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSaveWarningBox_Impl::~SwSaveWarningBox_Impl()
{
}
/*-- 21.06.2004 14:11:58---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwSaveWarningBox_Impl, ModifyHdl, Edit*, pEdit)
{
    aOKPB.Enable(pEdit->GetText().Len() > 0);
    return 0;
}            
/*-- 02.07.2004 09:02:53---------------------------------------------------
    
  -----------------------------------------------------------------------*/
SwSendQueryBox_Impl::SwSendQueryBox_Impl(Window* pParent, const String& rText) :
    ModalDialog(pParent, SW_RES(   DLG_MM_QUERY )),
    aQueryImageIM( this,    ResId( IM_QUERY     )),
    aQueryFI( this,         ResId( FI_QUERY     )),
    aTextED( this,          ResId( ED_TEXT      )),
    aSeparatorFL(this,      ResId( FL_SEPARATOR )),            
    aOKPB(this,             ResId( PB_OK        )),
    aCancelPB(this,         ResId( PB_CANCEL    )),
    bIsEmptyAllowed(true)
{
    FreeResource();
    aQueryFI.SetText(rText);
    aQueryImageIM.SetImage(QueryBox::GetStandardImage());
    aTextED.SetModifyHdl(LINK(this, SwSendQueryBox_Impl, ModifyHdl));
    ModifyHdl( &aTextED );
}
/*-- 02.07.2004 08:58:45---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSendQueryBox_Impl::~SwSendQueryBox_Impl()
{
}
/*-- 02.07.2004 08:58:25---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwSendQueryBox_Impl, ModifyHdl, Edit*, pEdit)
{
    aOKPB.Enable(bIsEmptyAllowed  || (pEdit->GetText().Len() > 0));
    return 0;
}            

/*-- 16.04.2004 16:34:48---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwCopyToDialog : public SfxModalDialog
{
    FixedInfo       m_aDescriptionFI;
    FixedText       m_aCCFT;
    Edit            m_aCCED;
    FixedText       m_aBCCFT;
    Edit            m_aBCCED;

    FixedInfo       m_aNoteFI;
    FixedLine       m_aSeparatorFL;

    OKButton        m_aOK;
    CancelButton    m_aCancel;
    HelpButton      m_aHelp;

public: 
    SwCopyToDialog(Window* pParent);
    ~SwCopyToDialog();

    String          GetCC() {return m_aCCED.GetText();}
    void            SetCC(const String& rSet) {m_aCCED.SetText(rSet);}
    
    String          GetBCC() {return m_aBCCED.GetText();}
    void            SetBCC(const String& rSet) {m_aBCCED.SetText(rSet);}
};

/*-- 16.04.2004 16:43:18---------------------------------------------------

  -----------------------------------------------------------------------*/
SwCopyToDialog::SwCopyToDialog(Window* pParent) :
    SfxModalDialog(pParent, SW_RES(DLG_MM_COPYTO)),
#pragma warning (disable : 4355)
    m_aDescriptionFI( this, ResId(       FI_DESCRIPTION )),
    m_aCCFT( this, ResId(                FT_CC          )),
    m_aCCED( this, ResId(                ED_CC          )),
    m_aBCCFT( this, ResId(               FT_BCC         )),
    m_aBCCED( this, ResId(               ED_BCC         )),
    m_aNoteFI( this, ResId(              FI_NOTE        )),
    m_aSeparatorFL( this, ResId(         FL_SEPARATOR   )),
    m_aOK( this, ResId(                  PB_OK          )),
    m_aCancel( this, ResId(              PB_CANCEL      )),
    m_aHelp( this, ResId(                PB_HELP        ))
#pragma warning (default : 4355)
{
    FreeResource();
}    
/*-- 16.04.2004 16:43:10---------------------------------------------------

  -----------------------------------------------------------------------*/
SwCopyToDialog::~SwCopyToDialog()
{
}

/*-- 02.04.2004 13:15:54---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeOutputPage::SwMailMergeOutputPage( SwMailMergeWizard* _pParent) :
    svt::OWizardPage( _pParent, SW_RES(DLG_MM_OUTPUT_PAGE)),
#pragma warning (disable : 4355)
    m_aHeaderFI(this,           ResId(  FI_HEADER           ) ),
    m_aOptionsFI(this,          ResId(  FI_OPTIONS          ) ),
    m_aSaveStartDocRB(this,     ResId(  RB_SAVESTARTDOC     ) ),
    m_aSaveMergedDocRB(this,    ResId(  RB_SAVEMERGEDDOC    ) ),
    m_aPrintRB(this,            ResId(  RB_PRINT            ) ),
    m_aSendMailRB(this,         ResId(  RB_SENDMAIL         ) ),
    m_aSeparatorFL(this,        ResId(  FL_SEPARATOR        ) ),
    m_aSaveStartDocPB(this,     ResId(  PB_SAVESTARTDOC     ) ),
    m_aSaveAsOneRB(this,        ResId(  RB_SAVEASONE        ) ),
    m_aSaveIndividualRB(this,   ResId(  RB_SAVEINDIVIDUAL   ) ),
    m_aFromRB(this,             ResId(  RB_FROM             ) ),
    m_aFromNF(this,             ResId(  NF_FROM             ) ),
    m_aToFT(this,               ResId(  FT_TO               ) ),
    m_aToNF(this,               ResId(  NF_TO               ) ),
    m_aSaveNowPB(this,          ResId(  PB_SAVENOW          ) ),
    m_aPrinterFT(this,          ResId(  FT_PRINT            ) ),
    m_aPrinterLB(this,          ResId(  LB_PRINT            ) ),
    m_aPrinterSettingsPB(this,  ResId(  PB_PRINTERSETTINGS  ) ),
    m_aPrintAllRB(this,         ResId(  RB_PRINTALL         ) ),
    m_aPrintNowPB(this,         ResId(  PB_PRINTNOW         ) ),
    
    m_aMailToFT( this, ResId(        FT_MAILTO              ) ),
    m_aMailToLB( this, ResId(        LB_MAILTO              ) ),
    m_aCopyToPB( this, ResId(        PB_COPYTO              ) ),
    m_aSubjectFT( this, ResId(       FT_SUBJECT             ) ),
    m_aSubjectED( this, ResId(       ED_SUBJECT             ) ),
    m_aSendAsFT( this, ResId(        FT_SENDAS              ) ),
    m_aSendAsLB( this, ResId(        LB_SENDAS              ) ),
    m_aSendAsPB( this, ResId(        PB_SENDAS              ) ),
    m_aAttachmentFT( this, ResId(    FT_ATTACHMENT              ) ),
    m_aAttachmentED( this, ResId(    ED_ATTACHMENT              ) ),
    m_aSendAllRB( this, ResId(       RB_SENDALL             ) ),
    m_aSendDocumentsPB( this, ResId( PB_SENDDOCUMENTS       ) ),
    
    m_sSaveStartST(ResId(           ST_SAVESTART  ) ),
    m_sSaveMergedST(ResId(          ST_SAVEMERGED ) ),
    m_sPrintST(ResId(               ST_PRINT      ) ),
    m_sDefaultAttachmentST(ResId(   ST_DEFAULTATTACHMENT )),
    m_sNoSubjectQueryST(ResId(      ST_SUBJECTQUERY      )),
    m_sNoSubjectST(ResId(           ST_NOSUBJECT )),
    m_sNoAttachmentNameST(ResId(    ST_NOATTACHMENTNAME )),
    m_sSendMailST(ResId(            ST_SENDMAIL   ) ),
    m_sConfigureMail(ResId(         ST_CONFIGUREMAIL)),
#pragma warning (default : 4355)
    m_pWizard(_pParent),
    m_pDocumentPrinterCopy(0),
    m_pTempPrinter( 0 ) 
{
    FreeResource();
    
    Link aLink = LINK(this, SwMailMergeOutputPage, OutputTypeHdl_Impl);
    m_aSaveStartDocRB.SetClickHdl(aLink);
    m_aSaveMergedDocRB.SetClickHdl(aLink);
    m_aPrintRB.SetClickHdl(aLink);
    m_aSendMailRB.SetClickHdl(aLink);
    m_aSaveStartDocRB.Check();
    m_aPrintAllRB.Check();
    m_aSaveAsOneRB.Check();
    m_aSendAllRB.Check();

    m_aSaveStartDocPB.SetClickHdl(LINK(this, SwMailMergeOutputPage, SaveStartHdl_Impl));
    m_aSaveNowPB.SetClickHdl(LINK(this, SwMailMergeOutputPage, SaveOutputHdl_Impl));
    m_aPrinterLB.SetSelectHdl(LINK(this, SwMailMergeOutputPage, PrinterChangeHdl_Impl));
    m_aPrintNowPB.SetClickHdl(LINK(this, SwMailMergeOutputPage, PrintHdl_Impl));
    m_aPrinterSettingsPB.SetClickHdl(LINK(this, SwMailMergeOutputPage, PrinterSetupHdl_Impl));
    
    m_aSendAsPB.SetClickHdl(LINK(this, SwMailMergeOutputPage, SendAsHdl_Impl)),
    m_aSendDocumentsPB.SetClickHdl(LINK(this, SwMailMergeOutputPage, SendDocumentsHdl_Impl)),
    m_aSendAsLB.SetSelectHdl(LINK(this, SwMailMergeOutputPage, SendTypeHdl_Impl));
    
    m_nFromToRBPos = m_aFromRB.GetPosPixel().Y();
    m_nFromToFTPos = m_aToFT.GetPosPixel().Y();
    m_nFromToNFPos = m_aFromNF.GetPosPixel().Y(); 

    m_nRBOffset = m_nFromToRBPos - m_aSaveIndividualRB.GetPosPixel().Y();

    OutputTypeHdl_Impl(&m_aSaveStartDocRB);

    m_aCopyToPB.SetClickHdl(LINK(this, SwMailMergeOutputPage, CopyToHdl_Impl));

}    
/*-- 02.04.2004 13:15:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeOutputPage::~SwMailMergeOutputPage()
{
    USHORT nEntryCount = m_aPrinterLB.GetEntryCount();
    for ( USHORT i = 0; i < nEntryCount; i++ )
        delete (QueueInfo*)m_aPrinterLB.GetEntryData( i );
    delete m_pTempPrinter;
    delete m_pDocumentPrinterCopy;
}
/*-- 31.01.2005 08:38:14---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwMailMergeOutputPage::ActivatePage()
{
    //fill printer ListBox
    USHORT nCount = Printer::GetQueueCount();
    if ( nCount )
    {
        for( USHORT i = 0; i < nCount; i++ )
        {
            const QueueInfo& rInfo = Printer::GetQueueInfo( i, FALSE );
            USHORT nPos = m_aPrinterLB.InsertEntry( rInfo.GetPrinterName() );
            m_aPrinterLB.SetEntryData( nPos, new QueueInfo( rInfo ) );
        }

    }
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();

    SwView* pTargetView = rConfigItem.GetTargetView();
    DBG_ASSERT(pTargetView, "no target view exists")
    if(pTargetView)
    {
        SfxPrinter* pPrinter = pTargetView->GetWrtShell().GetPrt(TRUE);
        m_aPrinterLB.SelectEntry( pPrinter->GetName() );
        m_aToNF.SetValue( rConfigItem.GetMergedDocumentCount() );
        m_aToNF.SetMax( rConfigItem.GetMergedDocumentCount() );
        m_pDocumentPrinterCopy = pTargetView->GetWrtShell().GetPrt( TRUE )->Clone();
    }
    m_aPrinterLB.SelectEntry( rConfigItem.GetSelectedPrinter() );

    SwView* pSourceView = rConfigItem.GetSourceView();
    DBG_ASSERT(pSourceView, "no source view exists")
    if(pSourceView)
    {
        SwDocShell* pDocShell = pSourceView->GetDocShell();
        if ( pDocShell->HasName() )
        {
            INetURLObject aTmp( pDocShell->GetMedium()->GetName() );
            m_aAttachmentED.SetText(aTmp.getName( 
                    INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET ));
        }
    }
    if(!rConfigItem.IsMailAvailable())
        m_aSendMailRB.Enable(sal_False);
}
/*-- 05.07.2004 13:54:11---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwMailMergeOutputPage::determineNextButtonState()
{
    return sal_False;
}
/*-- 02.04.2004 13:15:44---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, OutputTypeHdl_Impl, RadioButton*, pButton)
{
    Control* aControls[] = 
    {
        &m_aSaveStartDocPB, 
        &m_aSaveAsOneRB, &m_aSaveIndividualRB,
        &m_aFromRB, &m_aFromNF, &m_aToFT, &m_aToNF,
        &m_aSaveNowPB, 
        &m_aPrinterFT, &m_aPrinterLB, &m_aPrinterSettingsPB, &m_aPrintAllRB, 
        &m_aPrintNowPB,
        &m_aMailToFT, &m_aMailToLB, &m_aCopyToPB, 
        &m_aSubjectFT, &m_aSubjectED, 
        &m_aSendAsFT, &m_aSendAsLB, &m_aSendAsPB, 
        &m_aAttachmentFT, &m_aAttachmentED,
        &m_aSendAllRB, &m_aSendDocumentsPB,
        0
    };
    SetUpdateMode(TRUE);
    Control** pControl = aControls;
    do
    {        
        (*pControl)->Show(sal_False);
        
    } while(*(++pControl));

    if(&m_aSaveStartDocRB == pButton)
    {        
        m_aSaveStartDocPB.Show();
        m_aSeparatorFL.SetText(m_sSaveStartST);

    }
    else if(&m_aSaveMergedDocRB == pButton)
    {
        Control* aSaveMergedControls[] = 
        {
            &m_aSaveAsOneRB, &m_aSaveIndividualRB,
            &m_aFromRB, &m_aFromNF, &m_aToFT, &m_aToNF,
            &m_aSaveNowPB, 
            0
        };
        Control** pControl = aSaveMergedControls;
        do
        {        
            (*pControl)->Show(sal_True);
        
        } while(*(++pControl));
        if(!m_aFromRB.IsChecked() && !m_aSaveAsOneRB.IsChecked())
            m_aSaveIndividualRB.Check();
        m_aSeparatorFL.SetText(m_sSaveMergedST);
        //reposition the from/to line
        if(m_aFromRB.GetPosPixel().Y() != m_nFromToRBPos)
        {        
            Point aPos(m_aFromRB.GetPosPixel()); aPos.Y() = m_nFromToRBPos; m_aFromRB.SetPosPixel(aPos);
            aPos =   m_aToFT.GetPosPixel();      aPos.Y() = m_nFromToFTPos; m_aToFT.SetPosPixel(aPos);
            aPos =   m_aFromNF.GetPosPixel();    aPos.Y() = m_nFromToNFPos; m_aFromNF.SetPosPixel(aPos);
            aPos =   m_aToNF.GetPosPixel();      aPos.Y() = m_nFromToNFPos; m_aToNF.SetPosPixel(aPos);
        }

    }        
    else if(&m_aPrintRB == pButton)
    {
        Control* aPrintControls[] = 
        {
            &m_aFromRB, &m_aFromNF, &m_aToFT, &m_aToNF,
            &m_aPrinterFT, &m_aPrinterLB, &m_aPrinterSettingsPB, &m_aPrintAllRB, 
            &m_aPrintNowPB,
            0
        };
        Control** pControl = aPrintControls;
        do
        {        
            (*pControl)->Show(sal_True);
        
        } while(*(++pControl));
        if(!m_aFromRB.IsChecked())
            m_aPrintAllRB.Check();
        
        m_aSeparatorFL.SetText(m_sPrintST);
        //reposition the from/to line
        long nRB_FT_Offset = m_nFromToRBPos - m_nFromToFTPos;
        long nRB_NF_Offset = m_nFromToRBPos - m_nFromToNFPos;
        long nNewRBXPos = m_aPrintAllRB.GetPosPixel().Y() + m_nRBOffset;
            
        Point aPos(m_aFromRB.GetPosPixel());aPos.Y() = nNewRBXPos;                 m_aFromRB.SetPosPixel(aPos);
        aPos = m_aToFT.GetPosPixel();       aPos.Y() = nNewRBXPos + nRB_FT_Offset; m_aToFT.SetPosPixel(aPos);
        aPos = m_aFromNF.GetPosPixel();     aPos.Y() = nNewRBXPos + nRB_FT_Offset; m_aFromNF.SetPosPixel(aPos);
        aPos = m_aToNF.GetPosPixel();       aPos.Y() = nNewRBXPos + nRB_FT_Offset; m_aToNF.SetPosPixel(aPos);
    }        
    else /*if(&m_aSendMailRB == pButton)*/
    {        
        Control* aMailControls[] = 
        {
            &m_aFromRB, &m_aFromNF, &m_aToFT, &m_aToNF,
            &m_aMailToFT, &m_aMailToLB, &m_aCopyToPB, 
            &m_aSubjectFT, &m_aSubjectED, 
            &m_aSendAsFT, &m_aSendAsLB, &m_aSendAsPB, 
            &m_aAttachmentFT, &m_aAttachmentED,
            &m_aSendAllRB, &m_aSendDocumentsPB, 0
        };
        Control** pControl = aMailControls;
        do
        {        
            (*pControl)->Show(sal_True);
        
        } while(*(++pControl));
        
        if(!m_aFromRB.IsChecked())
            m_aSendAllRB.Check();
        if(!m_aAttachmentED.GetText().Len())
        {        
            String sAttach( m_sDefaultAttachmentST );
            sAttach += '.';
            sAttach += lcl_GetExtensionForDocType(
                        (ULONG)m_aSendAsLB.GetEntryData(m_aSendAsLB.GetSelectEntryPos()));
            m_aAttachmentED.SetText( sAttach );

        }
        m_aSeparatorFL.SetText(m_sSendMailST);
        //fill mail address ListBox 
        if(!m_aMailToLB.GetEntryCount())
        {        
            SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
            //select first column
            uno::Reference< sdbcx::XColumnsSupplier > xColsSupp( rConfigItem.GetResultSet(), uno::UNO_QUERY);
            //get the name of the actual columns
            uno::Reference < container::XNameAccess> xColAccess = xColsSupp.is() ? xColsSupp->getColumns() : 0;
            uno::Sequence< ::rtl::OUString > aFields;
            if(xColAccess.is())
                aFields = xColAccess->getElementNames();
            const ::rtl::OUString* pFields = aFields.getConstArray();
            for(sal_Int32 nField = 0; nField < aFields.getLength(); ++nField)
                m_aMailToLB.InsertEntry(pFields[nField]);
            
            m_aMailToLB.SelectEntryPos(0);
            // then select the right one - may not be available
            const ResStringArray& rHeaders = rConfigItem.GetDefaultAddressHeaders();
            String sEMailColumn = rHeaders.GetString( MM_PART_E_MAIL );
            Sequence< ::rtl::OUString> aAssignment = 
                            rConfigItem.GetColumnAssignment( rConfigItem.GetCurrentDBData() );
            if(aAssignment.getLength() > MM_PART_E_MAIL && aAssignment[MM_PART_E_MAIL].getLength())
                sEMailColumn = aAssignment[MM_PART_E_MAIL];
            m_aMailToLB.SelectEntry(sEMailColumn);
            // HTML format pre-selected
            m_aSendAsLB.SelectEntryPos(3);
            SendTypeHdl_Impl(&m_aSendAsLB);
        }
        if(m_aSendAllRB.GetPosPixel().Y() + m_nRBOffset != m_aFromRB.GetPosPixel().Y())
        {        
            long nRB_FT_Offset = m_nFromToRBPos - m_nFromToFTPos;
            long nRB_NF_Offset = m_nFromToRBPos - m_nFromToNFPos;
            long nNewRBXPos = m_aSendAllRB.GetPosPixel().Y() + m_nRBOffset;
            
            Point aPos(m_aFromRB.GetPosPixel());aPos.Y() = nNewRBXPos;                 m_aFromRB.SetPosPixel(aPos);
            aPos = m_aToFT.GetPosPixel();       aPos.Y() = nNewRBXPos + nRB_FT_Offset; m_aToFT.SetPosPixel(aPos);
            aPos = m_aFromNF.GetPosPixel();     aPos.Y() = nNewRBXPos + nRB_FT_Offset; m_aFromNF.SetPosPixel(aPos);
            aPos = m_aToNF.GetPosPixel();       aPos.Y() = nNewRBXPos + nRB_FT_Offset; m_aToNF.SetPosPixel(aPos);
        }
    }

    SetUpdateMode(FALSE);
    return 0;
}            

/*-- 16.04.2004 16:45:10---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, CopyToHdl_Impl, PushButton*, pButton)
{
    SwCopyToDialog* pDlg = new SwCopyToDialog(pButton);
    pDlg->SetCC(m_sCC );
    pDlg->SetBCC(m_sBCC);
    if(RET_OK == pDlg->Execute())
    {
        m_sCC =     pDlg->GetCC() ;
        m_sBCC =    pDlg->GetBCC();
    }            
    delete pDlg;
    return 0;
}
/*-- 17.05.2004 13:51:02---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, SaveStartHdl_Impl, PushButton*, pButton)
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    SwView* pSourceView = rConfigItem.GetSourceView();
    DBG_ASSERT( pSourceView, "source view missing")
    if(pSourceView)
    {
        pSourceView->GetViewFrame()->GetDispatcher()->Execute(SID_SAVEDOC, SFX_CALLMODE_SYNCHRON);
        SwDocShell* pDocShell = pSourceView->GetDocShell();
        //if the document has been saved it's URL has to be stored for 
        // later use and it can be closed now
        if(pDocShell->HasName() && !pDocShell->IsModified())
        {
            INetURLObject aURL = pDocShell->GetMedium()->GetURLObject();
            //update the attachment name 
            if(!m_aAttachmentED.GetText().Len())
            {        
                SwDocShell* pDocShell = pSourceView->GetDocShell();
                if ( pDocShell->HasName() )
                {
                    m_aAttachmentED.SetText(aURL.getName( 
                            INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET ));
                }
            }
            
            rConfigItem.AddSavedDocument( 
                    aURL.GetMainURL(INetURLObject::DECODE_TO_IURI));
            pButton->Enable(FALSE);
            m_pWizard->enableButtons(WZB_FINISH, sal_True);
            pButton->Enable(FALSE);

        }            
    }
    return 0;
}
/*-- 17.05.2004 13:51:02---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, SaveOutputHdl_Impl, PushButton*, pButton)
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    SwView* pTargetView = rConfigItem.GetTargetView();
    DBG_ASSERT(pTargetView, "no target view exists")
    if(!pTargetView)
        return 0;

    if(m_aSaveAsOneRB.IsChecked())
    {
        String sFilter;
        String sPath = SwMailMergeHelper::CallSaveAsDialog(sFilter);
        if(!sPath.Len())
            return 0;
        uno::Sequence< beans::PropertyValue > aValues(1);
        beans::PropertyValue* pValues = aValues.getArray();
        pValues[0].Name = C2U("FilterName");
        pValues[0].Value <<= ::rtl::OUString(sFilter);

        uno::Reference< frame::XStorable > xStore( pTargetView->GetDocShell()->GetModel(), uno::UNO_QUERY);
        xStore->storeToURL( sPath, aValues );
    }
    else 
    {
        sal_uInt32 nBegin = 0;
        sal_uInt32 nEnd = 0;
        if(m_aSaveIndividualRB.IsChecked())
        {
            nBegin = 0;
            nEnd = rConfigItem.GetMergedDocumentCount();
        }            
        else
        {
            nBegin = m_aFromNF.GetValue() - 1;
            nEnd =   m_aToNF.GetValue();
            if(nEnd > rConfigItem.GetMergedDocumentCount())
                nEnd = rConfigItem.GetMergedDocumentCount();
        }            
        String sFilter;
        String sPath = SwMailMergeHelper::CallSaveAsDialog(sFilter);
        if(!sPath.Len())
            return 0;
        String sTargetTempURL = URIHelper::SmartRel2Abs(
            INetURLObject(), utl::TempFile::CreateTempName(),
            URIHelper::GetMaybeFileHdl());
        const SfxFilter *pSfxFlt = SwIoSystem::GetFilterOfFormat(
                String::CreateFromAscii( GetFILTER_XML() ),
                SwDocShell::Factory().GetFilterContainer() );

        uno::Sequence< beans::PropertyValue > aValues(1);
        beans::PropertyValue* pValues = aValues.getArray();
        pValues[0].Name = C2U("FilterName");
        pValues[0].Value <<= ::rtl::OUString(pSfxFlt->GetFilterName());

        uno::Reference< frame::XStorable > xStore( pTargetView->GetDocShell()->GetModel(), uno::UNO_QUERY);
        xStore->storeToURL( sTargetTempURL, aValues   );

        for(sal_uInt32 nDoc = nBegin; nDoc < nEnd; ++nDoc)
        {        
            SwDocMergeInfo& rInfo = rConfigItem.GetDocumentMergeInfo(nDoc);
            INetURLObject aURL(sPath);
            String sFile = aURL.GetBase();
            String sExtension = aURL.getExtension();
            if(!sExtension.Len())
            {
                sPath.AppendAscii(".sxw");
                sExtension.AppendAscii("sxw");
            }            
            //now extract a document from the target document
            SfxObjectShellRef xTempDocShell( new SwDocShell( SFX_CREATE_MODE_STANDARD ) );
            xTempDocShell->DoInitNew( 0 );
            SfxViewFrame* pTempFrame = SFX_APP()->CreateViewFrame( *xTempDocShell, 0, TRUE );
//            pTempFrame->GetFrame()->Appear();
            SwView* pTempView = static_cast<SwView*>( pTempFrame->GetViewShell() );
            pTargetView->GetWrtShell().StartAction();
            SwgReaderOption aOpt;
            aOpt.SetTxtFmts( sal_True );
            aOpt.SetFrmFmts( sal_True );
            aOpt.SetPageDescs( sal_True );
            aOpt.SetNumRules( sal_True );
            aOpt.SetMerge( sal_False );
            pTempView->GetDocShell()->LoadStylesFromFile( 
                    sTargetTempURL, aOpt, sal_True );
            
            pTargetView->GetWrtShell().PastePages(pTempView->GetWrtShell(), 
                    (USHORT)rInfo.nStartPageInTarget, (USHORT)rInfo.nEndPageInTarget );
            pTargetView->GetWrtShell().EndAction();
            //then save it
            String sOutPath = aURL.GetMainURL(INetURLObject::DECODE_TO_IURI);
            String sCounter('_');
            sCounter += String::CreateFromInt32(nDoc);
            sOutPath.Insert(sCounter, sPath.Len() - sExtension.Len() - 1);
            //SfxStringItem aName(SID_FILE_NAME, sOutPath);
            //SfxStringItem aFilter(SID_FILTER_NAME, sFilter);

            const SfxBoolItem* pBool = 0;
            while(true)
            {        
                //time for other slots is needed
                for(sal_Int16 r = 0; r < 10; ++r)
                    Application::Reschedule();
                bool bFailed = false;
                try
                {
                    uno::Sequence< beans::PropertyValue > aValues(1);
                    beans::PropertyValue* pValues = aValues.getArray();
                    pValues[0].Name = C2U("FilterName");
                    pValues[0].Value <<= ::rtl::OUString(sFilter);

                    uno::Reference< frame::XStorable > xStore( xTempDocShell->GetModel(), uno::UNO_QUERY);
                    xStore->storeToURL( sOutPath, aValues   );
                }
                catch( const uno::Exception& )
                {
                    bFailed = true; 
                }
                
                if(bFailed)
                {
                    SwSaveWarningBox_Impl aWarning( pButton, sOutPath );
                    if(RET_OK == aWarning.Execute())
                        sOutPath = aWarning.GetFileName();
                    else
                    {        
                        xTempDocShell->DoClose();
                        return 0;
                    }
                }
                else 
                {        
                    xTempDocShell->DoClose();
                    break;
                }
            }
        }
        ::osl::File::remove( sTargetTempURL );
    }
    m_pWizard->enableButtons(WZB_FINISH, sal_True);
    return 0;
}
/*-- 22.06.2004 11:51:30---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, PrinterChangeHdl_Impl, ListBox*, pBox)
{
    if( m_pDocumentPrinterCopy && pBox->GetSelectEntryPos() != LISTBOX_ENTRY_NOTFOUND )
    {
        const QueueInfo& rInfo = *((QueueInfo*)(pBox->GetEntryData( pBox->GetSelectEntryPos() )));

        if ( !m_pTempPrinter )
        {
            if( (m_pDocumentPrinterCopy->GetName() == rInfo.GetPrinterName()) &&
                 (m_pDocumentPrinterCopy->GetDriverName() == rInfo.GetDriver()) )
                m_pTempPrinter = new Printer( m_pDocumentPrinterCopy->GetJobSetup() );
            else
                m_pTempPrinter = new Printer( rInfo );
        }
        else
        {
            if( (m_pTempPrinter->GetName() != rInfo.GetPrinterName()) ||
                 (m_pTempPrinter->GetDriverName() != rInfo.GetDriver()) )
            {
                delete m_pTempPrinter;
                m_pTempPrinter = new Printer( rInfo );
            }
        }

        m_aPrinterSettingsPB.Enable( m_pTempPrinter->HasSupport( SUPPORT_SETUPDIALOG ) );
    }
    else
        m_aPrinterSettingsPB.Disable();
    m_pWizard->GetConfigItem().SetSelectedPrinter( pBox->GetSelectEntry() );

//    return m_pTempPrinter;
    return 0;
}

/*-- 17.05.2004 13:51:02---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, PrintHdl_Impl, PushButton*, pButton)
{
    SwView* pTargetView = m_pWizard->GetConfigItem().GetTargetView();
    DBG_ASSERT(pTargetView, "no target view exists")
    if(!pTargetView)
        return 0;
    
    sal_uInt32 nBegin = 0;
    sal_uInt32 nEnd = 0;
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();
    if(m_aPrintAllRB.IsChecked())
    {
        nBegin = 0;
        nEnd = rConfigItem.GetMergedDocumentCount();
    }            
    else
    {
        nBegin = m_aFromNF.GetValue() - 1;
        nEnd =   m_aToNF.GetValue();
        if(nEnd > rConfigItem.GetMergedDocumentCount())
            nEnd = rConfigItem.GetMergedDocumentCount();
    }            
    SfxBoolItem aSilent( SID_SILENT, sal_True);
    rConfigItem.SetPrintRange( (USHORT)nBegin, (USHORT)nEnd );
    SwWrtShell& rSh = pTargetView->GetWrtShell();
    pTargetView->SetMailMergeConfigItem(&rConfigItem, 0, sal_False);
    if(m_pTempPrinter)
    {        
        m_pDocumentPrinterCopy->SetPrinterProps(m_pTempPrinter);
        pTargetView->SetPrinter(m_pDocumentPrinterCopy->Clone());
    }
        
    rSh.GetNewDBMgr()->SetMergeType( DBMGR_MERGE_DOCUMENTS );
    SfxDispatcher *pDis = pTargetView->GetViewFrame()->GetDispatcher();
    if(m_pTempPrinter)
    {
        SfxStringItem aPrinterName(SID_PRINTER_NAME, m_pTempPrinter->GetName());
        pDis->Execute(SID_PRINTDOC,
                SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD, &aPrinterName, 0L);
    }
    else
    {
        SfxBoolItem aMergeSilent(SID_SILENT, TRUE);
        pDis->Execute(SID_PRINTDOCDIRECT,
                SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD/*, &aMergeSilent, 0L*/);
    }

    pTargetView->SetMailMergeConfigItem(0, 0, sal_False);
    m_pWizard->enableButtons(WZB_FINISH, sal_True);
    return 0;
}
/*-- 17.05.2004 13:51:02---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, PrinterSetupHdl_Impl, PushButton*, pButton)
{
    if( !m_pTempPrinter )
        PrinterChangeHdl_Impl(&m_aPrinterLB);
    if(m_pTempPrinter)
        m_pTempPrinter->Setup(pButton);
    return 0;
}
/*-- 14.06.2004 09:34:01---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, SendTypeHdl_Impl, ListBox*, pBox)
{
    ULONG nDocType = (ULONG)pBox->GetEntryData(pBox->GetSelectEntryPos());
    sal_Bool bEnable = MM_DOCTYPE_HTML != nDocType && MM_DOCTYPE_TEXT != nDocType;
    m_aSendAsPB.Enable( bEnable );
    m_aAttachmentFT.Enable( bEnable );
    m_aAttachmentED.Enable( bEnable );
    if(bEnable)
    {        
        //add the correct extension
        String sAttach(m_aAttachmentED.GetText());
        //do nothing if the user has removed the name - the warning will come early enough
        if(sAttach.Len())
        {        
            xub_StrLen nTokenCount = sAttach.GetTokenCount( '.' );
            if( 2 > nTokenCount)
            {
                sAttach += '.';
                ++nTokenCount;
            }
            sAttach.SetToken( nTokenCount - 1, '.', lcl_GetExtensionForDocType( nDocType ));
            m_aAttachmentED.SetText(sAttach);
        }
    }
    return 0;
}
/*-- 17.05.2004 13:51:02---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, SendAsHdl_Impl, PushButton*, pButton)
{
    SwMailBodyDialog* pDlg = new SwMailBodyDialog(pButton, m_pWizard);
    pDlg->SetBody(m_sBody);
    if(RET_OK == pDlg->Execute())
    {
        m_sBody = pDlg->GetBody();
    }            
    return 0;
}            
/*-- 21.05.2004 12:03:25---------------------------------------------------
    Send documents as e-mail
  -----------------------------------------------------------------------*/
IMPL_LINK(SwMailMergeOutputPage, SendDocumentsHdl_Impl, PushButton*, pButton)
{
    SwMailMergeConfigItem& rConfigItem = m_pWizard->GetConfigItem();

    //get the composed document
    SwView* pTargetView = rConfigItem.GetTargetView();
    DBG_ASSERT(pTargetView, "no target view exists")
    if(!pTargetView)
        return 0;
    
    if(!rConfigItem.GetMailServer().getLength() ||
            !SwMailMergeHelper::CheckMailAddress(rConfigItem.GetMailAddress()) )
    {
        QueryBox aQuery(pButton, WB_YES_NO_CANCEL, m_sConfigureMail);
        USHORT nRet = aQuery.Execute();
        if(RET_YES == nRet )
        {
            SfxAllItemSet aSet(pTargetView->GetPool());
            SwMailConfigDlg* pDlg = new SwMailConfigDlg(pButton, aSet);
            nRet = pDlg->Execute();
            delete pDlg;
        }
        
        if(nRet != RET_OK && nRet != RET_YES)
            return 0;
    }            
    //create the send dialog
    SwSendMailDialog* pDlg = new SwSendMailDialog( pButton, rConfigItem );
    //add the documents
    sal_uInt32 nBegin = 0;
    sal_uInt32 nEnd = 0;
    if(m_aSendAllRB.IsChecked())
    {
        nBegin = 0;
        nEnd = rConfigItem.GetMergedDocumentCount();
    }            
    else
    {
        nBegin = m_aFromNF.GetValue() - 1;
        nEnd =   m_aToNF.GetValue();
        if(nEnd > rConfigItem.GetMergedDocumentCount())
            nEnd = rConfigItem.GetMergedDocumentCount();
    }            
    bool bAsBody = false;
    bool bIsPDF = false;
    rtl_TextEncoding eEncoding = ::gsl_getSystemTextEncoding();
    SfxFilterContainer* pFilterContainer = SwDocShell::Factory().GetFilterContainer();
    const SfxFilter *pSfxFlt = 0;
    ULONG nDocType = (ULONG)m_aSendAsLB.GetEntryData(m_aSendAsLB.GetSelectEntryPos());
    String sExtension = lcl_GetExtensionForDocType(nDocType);
    switch( nDocType )
    {        
        case MM_DOCTYPE_OOO : break;
        case MM_DOCTYPE_PDF : bIsPDF = true; break;
        case MM_DOCTYPE_WORD: 
        {    
            //the method SwIOSystemGetFilterOfFormat( ) returns the template filter
            //because it uses the same user data :-(
            SfxFilterMatcher aMatcher( pFilterContainer->GetName() );
            SfxFilterMatcherIter aIter( &aMatcher );
            const SfxFilter* pFilter = aIter.First();
            String sFilterUserData( String::CreateFromAscii(GetFILTER_WW8()));
            while ( pFilter )
            {
                if( pFilter->GetUserData() == sFilterUserData   && pFilter->CanExport() )
                {        
                    pSfxFlt = pFilter;
                    break;
                }
                pFilter = aIter.Next();
            }

        }
        break;
        case MM_DOCTYPE_HTML: 
        {    
            bAsBody = true; 
            SvxHtmlOptions* pHtmlOptions = SvxHtmlOptions::Get();
            eEncoding = pHtmlOptions->GetTextEncoding();
        }
        break;
        case MM_DOCTYPE_TEXT: 
            bAsBody = true; 
        break;
    }
    if(!pSfxFlt)
        pSfxFlt = pFilterContainer->GetFilter4Extension(sExtension, SFX_FILTER_EXPORT);
    
    if(!pSfxFlt)
        return 0;
    String sMimeType = pSfxFlt->GetMimeType();
    
    if(!m_aSubjectED.GetText().Len())
    {
        SwSendQueryBox_Impl aQuery(pButton, m_sNoSubjectQueryST);
        aQuery.SetIsEmptyTextAllowed(true);
        aQuery.SetValue(m_sNoSubjectST);
        if(RET_OK == aQuery.Execute())
        {
            if(aQuery.GetValue() != m_sNoSubjectST)
                m_aSubjectED.SetText(aQuery.GetValue());
        }
        else 
            return 0;
    }            
    if(!bAsBody && !m_aAttachmentED.GetText().Len())
    {
        SwSendQueryBox_Impl aQuery(pButton, m_sNoAttachmentNameST);
        aQuery.SetIsEmptyTextAllowed(false);
        if(RET_OK == aQuery.Execute())
        {
            String sAttach(aQuery.GetValue());
            xub_StrLen nTokenCount = sAttach.GetTokenCount( '.' );
            if( 2 > nTokenCount)
            {
                sAttach += '.';
                ++nTokenCount;
            }
            sAttach.SetToken( nTokenCount - 1, '.', lcl_GetExtensionForDocType( 
                     (ULONG)m_aSendAsLB.GetEntryData(m_aSendAsLB.GetSelectEntryPos())));
            m_aAttachmentED.SetText(sAttach);
        }            
        else
            return 0;
    }            
    SfxStringItem aFilterName( SID_FILTER_NAME, pSfxFlt->GetFilterName() );
    String sEMailColumn = m_aMailToLB.GetSelectEntry();
    DBG_ASSERT( sEMailColumn.Len(), "No email column selected");
    Reference< sdbcx::XColumnsSupplier > xColsSupp( rConfigItem.GetResultSet(), UNO_QUERY);
    Reference < container::XNameAccess> xColAccess = xColsSupp.is() ? xColsSupp->getColumns() : 0;
    if(!sEMailColumn.Len() || !xColAccess.is() || !xColAccess->hasByName(sEMailColumn))
        return 0;
    
    String sFilterOptions;
    if(MM_DOCTYPE_TEXT == nDocType)
    {        
        SwAsciiOptions aOpt;
        USHORT nAppScriptType = GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage() );
        USHORT nWhich = GetWhichOfScript( RES_CHRATR_LANGUAGE, nAppScriptType);
        aOpt.SetLanguage( ((SvxLanguageItem&)pTargetView->GetWrtShell().
                            GetDefault( nWhich )).GetLanguage());
        aOpt.SetParaFlags( LINEEND_CR );
        aOpt.WriteUserData( sFilterOptions );
    }
    String sTargetTempURL = URIHelper::SmartRel2Abs(
        INetURLObject(), utl::TempFile::CreateTempName(),
        URIHelper::GetMaybeFileHdl());
    const SfxFilter *pTargetSfxFlt = SwIoSystem::GetFilterOfFormat(
            String::CreateFromAscii( GetFILTER_XML() ),
            SwDocShell::Factory().GetFilterContainer() );

    uno::Sequence< beans::PropertyValue > aValues(1);
    beans::PropertyValue* pValues = aValues.getArray();
    pValues[0].Name = C2U("FilterName");
    pValues[0].Value <<= ::rtl::OUString(pTargetSfxFlt->GetFilterName());

    uno::Reference< frame::XStorable > xStore( pTargetView->GetDocShell()->GetModel(), uno::UNO_QUERY);
    xStore->storeToURL( sTargetTempURL, aValues   );
    
    pDlg->SetDocumentCount( nEnd );
    pDlg->Show();
    //help to force painting the dialog
    for ( sal_Int16 i = 0; i < 25; i++)
        Application::Reschedule();
    for(sal_uInt32 nDoc = nBegin; nDoc < nEnd; ++nDoc)
    {        
        m_pWizard->EnterWait();
        SwDocMergeInfo& rInfo = rConfigItem.GetDocumentMergeInfo(nDoc);
            
        //now extract a document from the target document
        SfxObjectShellRef xTempDocShell( new SwDocShell( SFX_CREATE_MODE_STANDARD ) );
        xTempDocShell->DoInitNew( 0 );
        SfxViewFrame* pTempFrame = SFX_APP()->CreateViewFrame( *xTempDocShell, 0, TRUE );
//        pTempFrame->GetFrame()->Appear();
        SwView* pTempView = static_cast<SwView*>( pTempFrame->GetViewShell() );
        pTargetView->GetWrtShell().StartAction();
        SwgReaderOption aOpt;
        aOpt.SetTxtFmts( sal_True );
        aOpt.SetFrmFmts( sal_True );
        aOpt.SetPageDescs( sal_True );
        aOpt.SetNumRules( sal_True );
        aOpt.SetMerge( sal_False );
        pTempView->GetDocShell()->LoadStylesFromFile( 
                sTargetTempURL, aOpt, sal_True );
        pTargetView->GetWrtShell().PastePages(pTempView->GetWrtShell(), 
                (USHORT)rInfo.nStartPageInTarget, (USHORT)rInfo.nEndPageInTarget );
        pTargetView->GetWrtShell().EndAction();
        
        //then save it
        SfxStringItem aName(SID_FILE_NAME, 
                URIHelper::SmartRel2Abs(
                    INetURLObject(), utl::TempFile::CreateTempName(0),
                    URIHelper::GetMaybeFileHdl()) );
        const SfxStringItem* pFilterOptions = 0;
        
/*        if(bIsPDF)
        {
            SfxDispatcher* pSfxDispatcher = pTempView->GetViewFrame()->GetDispatcher();
            pSfxDispatcher->Execute(
                            SID_DIRECTEXPORTDOCASPDF,
                                  SFX_CALLMODE_SYNCHRON, &aName, &aFilterName, 0L );
        }
        else*/
        {
            uno::Sequence< beans::PropertyValue > aValues(MM_DOCTYPE_TEXT == nDocType ? 2 : 1);
            beans::PropertyValue* pValues = aValues.getArray();
            pValues[0].Name = C2U("FilterName");
            pValues[0].Value <<= ::rtl::OUString(pSfxFlt->GetFilterName());
            if(MM_DOCTYPE_TEXT == nDocType)
            {        
                pValues[1].Name = C2U("FilterOptions");
                pValues[1].Value <<= ::rtl::OUString(sFilterOptions);
            }

            uno::Reference< frame::XStorable > xStore( pTempView->GetDocShell()->GetModel(), uno::UNO_QUERY);
            xStore->storeToURL( aName.GetValue(), aValues );
        }            
        xTempDocShell->DoClose();
            
        sal_Int32 nTarget = rConfigItem.MoveResultSet(rInfo.nDBRow);
        DBG_ASSERT( nTarget == rInfo.nDBRow, "row of current document could not be selected")
        String sEMailColumn = m_aMailToLB.GetSelectEntry();
        DBG_ASSERT( sEMailColumn.Len(), "No email column selected");
        ::rtl::OUString sEMail = lcl_GetColumnValueOf(sEMailColumn, xColAccess);
        SwMailDescriptor aDesc;
        aDesc.sEMail = sEMail;
        String sBody;
        if(bAsBody)
        {
            {
                //read in the temporary file and use it as mail body 
                SfxMedium aMedium( aName.GetValue(),    STREAM_READ, TRUE);
                SvStream* pInStream = aMedium.GetInStream();
                if(pInStream)
                    pInStream->SetStreamCharSet( eEncoding );
                else
                {
                    DBG_ERROR("no output file created?")
                    continue;
                }
                ByteString sLine;
                sal_Bool bDone = pInStream->ReadLine( sLine );
                while ( bDone )
                {
                    sBody += String(sLine, eEncoding);
                    sBody += '\n';
                    bDone = pInStream->ReadLine( sLine );
                }
            }
            //remove the temporary file
            SWUnoHelper::UCB_DeleteFile( aName.GetValue() );
        }            
        else
        {
            sBody = m_sBody;  
            aDesc.sAttachmentURL = aName.GetValue();
            String sAttachment(m_aAttachmentED.GetText());
            xub_StrLen nTokenCount = sAttachment.GetTokenCount( '.' );
            if( 2 > nTokenCount)
            {
                sAttachment += '.';
                sAttachment.SetToken( nTokenCount, '.', sExtension);
            }
            else if(sAttachment.GetToken( nTokenCount - 1, '.') != sExtension)
                sAttachment += sExtension;
            aDesc.sAttachmentName = sAttachment;
            aDesc.sMimeType = sMimeType;
            
            if(rConfigItem.IsGreetingLine(sal_True))
            {        
                ::rtl::OUString sNameColumn = rConfigItem.GetAssignedColumn(MM_PART_LASTNAME);
                ::rtl::OUString sName = lcl_GetColumnValueOf(sNameColumn, xColAccess);
                String sGreeting;
                if(sName.getLength() && rConfigItem.IsIndividualGreeting(sal_True))
                {
                    ::rtl::OUString sGenderColumn = rConfigItem.GetAssignedColumn(MM_PART_GENDER);
                    const ::rtl::OUString& sFemaleValue = rConfigItem.GetFemaleGenderValue();
                    ::rtl::OUString sGenderValue = lcl_GetColumnValueOf(sGenderColumn, xColAccess);
                    SwMailMergeConfigItem::Gender eGenderType = sGenderValue == sFemaleValue ?
                        SwMailMergeConfigItem::FEMALE :
                        SwMailMergeConfigItem::MALE;

                    sGreeting = SwAddressPreview::FillData(
                        rConfigItem.GetGreetings(eGenderType)
                        [rConfigItem.GetCurrentGreeting(eGenderType)],
                            rConfigItem);
                }
                else
                {
                    sGreeting = 
                        rConfigItem.GetGreetings(SwMailMergeConfigItem::NEUTRAL)
                        [rConfigItem.GetCurrentGreeting(SwMailMergeConfigItem::NEUTRAL)];

                }    
                sGreeting += '\n';
                sBody.Insert(sGreeting, 0);
            }
        }
        aDesc.sBodyContent = sBody;
        if(MM_DOCTYPE_HTML == nDocType)
        {
            aDesc.sBodyMimeType = ::rtl::OUString::createFromAscii("text/html; charset=");
            aDesc.sBodyMimeType += ::rtl::OUString::createFromAscii( 
                                rtl_getBestMimeCharsetFromTextEncoding( eEncoding ));
        }
        else
            aDesc.sBodyMimeType =  
                ::rtl::OUString::createFromAscii("text/plain; charset=UTF-8; format=flowed"); 

        aDesc.sSubject = m_aSubjectED.GetText();
        aDesc.sCC = m_sCC;
        aDesc.sBCC = m_sBCC;
        pDlg->AddDocument( aDesc );
        //help to force painting the dialog
        for ( sal_Int16 i = 0; i < 25; i++)
            Application::Reschedule();
        //stop creating of data when dialog has been closed
        if(!pDlg->IsVisible())
        {
            m_pWizard->LeaveWait();
            break;
        }
        m_pWizard->LeaveWait();
    }
    pDlg->EnableDesctruction();
    ::osl::File::remove( sTargetTempURL );

    m_pWizard->enableButtons(WZB_FINISH, sal_True);
    //the dialog deletes itself
    //delete pDlg;
    return 0;
}
