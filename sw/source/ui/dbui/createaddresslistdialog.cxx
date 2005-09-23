/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: createaddresslistdialog.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-23 11:48:08 $
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

#ifndef _CREATEADDRESSLISTDIALOG_HXX
#include <createaddresslistdialog.hxx>
#endif
#ifndef _CUSTOMIZEADDRESSLISTDIALOG_HXX
#include <customizeaddresslistdialog.hxx>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif
#ifndef _SV_SCRBAR_HXX
#include <vcl/scrbar.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVT_CONTROLDIMS_HRC_
#include <svtools/controldims.hrc>
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
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
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _RTL_TEXTENC_H
#include <rtl/textenc.h>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#include <createaddresslistdialog.hrc>
#include <dbui.hrc>
#include <helpid.h>

using namespace com::sun::star::uno;
using namespace com::sun::star::ui::dialogs;
using ::rtl::OUString;

#define C2U(cChar) OUString::createFromAscii(cChar)
/*-- 19.04.2004 12:19:50---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwAddressControl_Impl : public Control
{
    ScrollBar                       m_aScrollBar;
    Window                          m_aWindow;

    ::std::vector<FixedText*>       m_aFixedTexts;
    ::std::vector<Edit*>            m_aEdits;

    SwCSVData*                      m_pData;
    Size                            m_aWinOutputSize;
    sal_Int32                       m_nLineHeight;
    sal_uInt32                      m_nCurrentDataSet;

    bool                            m_bNoDataSet;

    DECL_LINK(ScrollHdl_Impl, ScrollBar*);
    DECL_LINK(GotFocusHdl_Impl, Edit*);
    DECL_LINK(EditModifyHdl_Impl, Edit*);
    
    void                MakeVisible(const Rectangle& aRect);
    
    virtual long        PreNotify( NotifyEvent& rNEvt );
    virtual void        Command( const CommandEvent& rCEvt );
public:
    SwAddressControl_Impl(Window* pParent, const ResId& rResId );
    ~SwAddressControl_Impl();

    void        SetData(SwCSVData& rDBData);
    void        SetCurrentDataSet(sal_uInt32 nSet);
    sal_uInt32  GetCurrentDataSet() const { return m_nCurrentDataSet;}
    void        SetCursorTo(sal_uInt32 nElement);
};            

/*-- 13.04.2004 10:09:42---------------------------------------------------

  -----------------------------------------------------------------------*/
SwAddressControl_Impl::SwAddressControl_Impl(Window* pParent, const ResId& rResId ) :
    Control(pParent, rResId),
#pragma warning (disable : 4355)
    m_aScrollBar(this, ResId(SCR_1)),
    m_aWindow(this, ResId(WIN_DATA)),
#pragma warning (default : 4355)
    m_pData(0),
    m_nLineHeight(0),
    m_nCurrentDataSet(0),
    m_bNoDataSet(true),
    m_aWinOutputSize( m_aWindow.GetOutputSizePixel() )
{
    FreeResource();
    Link aScrollLink = LINK(this, SwAddressControl_Impl, ScrollHdl_Impl);
    m_aScrollBar.SetScrollHdl(aScrollLink);
    m_aScrollBar.SetEndScrollHdl(aScrollLink);
    m_aScrollBar.EnableDrag();

}
/*-- 13.04.2004 10:09:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwAddressControl_Impl::~SwAddressControl_Impl()
{
    ::std::vector<FixedText*>::iterator aTextIter;
    for(aTextIter = m_aFixedTexts.begin(); aTextIter != m_aFixedTexts.end(); ++aTextIter)
        delete *aTextIter;
    ::std::vector<Edit*>::iterator aEditIter;
    for(aEditIter = m_aEdits.begin(); aEditIter != m_aEdits.end(); ++aEditIter)
        delete *aEditIter;
}
/*-- 19.04.2004 12:22:41---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwAddressControl_Impl::SetData(SwCSVData& rDBData)
{
    m_pData = &rDBData;
    //when the address data is updated then remove the controls an build again
    if(m_aFixedTexts.size())
    {
        ::std::vector<FixedText*>::iterator aTextIter;
        for(aTextIter = m_aFixedTexts.begin(); aTextIter != m_aFixedTexts.end(); ++aTextIter)
            delete *aTextIter;
        ::std::vector<Edit*>::iterator aEditIter;
        for(aEditIter = m_aEdits.begin(); aEditIter != m_aEdits.end(); ++aEditIter)
            delete *aEditIter;
        m_aFixedTexts.clear();
        m_aEdits.clear();
        m_bNoDataSet = true;
    }            
    //now create appropriate controls 
    
    ::std::vector< OUString >::iterator  aHeaderIter;
    
    long nFTXPos = m_aWindow.LogicToPixel(Point(RSC_SP_CTRL_X, RSC_SP_CTRL_X), MAP_APPFONT).X();
    long nFTHeight = m_aWindow.LogicToPixel(Size(RSC_BS_CHARHEIGHT, RSC_BS_CHARHEIGHT), MAP_APPFONT).Height();
    long nFTWidth = 0;
    
    //determine the width of the FixedTexts
    for(aHeaderIter = m_pData->aDBColumnHeaders.begin(); 
                aHeaderIter != m_pData->aDBColumnHeaders.end(); 
                ++aHeaderIter)
    {                    
        sal_Int32 nTemp = m_aWindow.GetTextWidth(*aHeaderIter);
        if(nTemp > nFTWidth)
          nFTWidth = nTemp;
    }
    //add some pixels
    nFTWidth += 2;
    long nEDXPos = nFTWidth + nFTXPos + 
            m_aWindow.LogicToPixel(Size(RSC_SP_CTRL_DESC_X, RSC_SP_CTRL_DESC_X), MAP_APPFONT).Width();
    long nEDHeight = m_aWindow.LogicToPixel(Size(RSC_CD_TEXTBOX_HEIGHT, RSC_CD_TEXTBOX_HEIGHT), MAP_APPFONT).Height();
    long nEDWidth = m_aWinOutputSize.Width() - nEDXPos - nFTXPos;
    m_nLineHeight = nEDHeight + m_aWindow.LogicToPixel(Size(RSC_SP_CTRL_GROUP_Y, RSC_SP_CTRL_GROUP_Y), MAP_APPFONT).Height();
    
    long nEDYPos = m_aWindow.LogicToPixel(Size(RSC_SP_CTRL_DESC_Y, RSC_SP_CTRL_DESC_Y), MAP_APPFONT).Height();
    long nFTYPos = nEDYPos + nEDHeight - nFTHeight;

    Link aFocusLink = LINK(this, SwAddressControl_Impl, GotFocusHdl_Impl);
    Link aEditModifyLink = LINK(this, SwAddressControl_Impl, EditModifyHdl_Impl);
    Edit* pLastEdit = 0;
    sal_Int32 nVisibleLines = 0;
    sal_Int32 nLines = 0;
    for(aHeaderIter = m_pData->aDBColumnHeaders.begin(); 
                aHeaderIter != m_pData->aDBColumnHeaders.end(); 
                ++aHeaderIter, nEDYPos += m_nLineHeight, nFTYPos += m_nLineHeight, nLines++)
    {
        FixedText* pNewFT = new FixedText(&m_aWindow, WB_RIGHT);
        Edit* pNewED = new Edit(&m_aWindow, WB_BORDER);
        //set nLines a position identifier - used in the ModifyHdl
        pNewED->SetData((void*)nLines);
        pNewED->SetGetFocusHdl(aFocusLink);
        pNewED->SetModifyHdl(aEditModifyLink);

        pNewFT->SetPosSizePixel(Point(nFTXPos, nFTYPos), Size(nFTWidth, nFTHeight));
        pNewED->SetPosSizePixel(Point(nEDXPos, nEDYPos), Size(nEDWidth, nEDHeight));
        if(nEDYPos + nEDHeight < m_aWinOutputSize.Height())
            ++nVisibleLines;
        
        pNewFT->SetText(*aHeaderIter);
        
        pNewFT->Show();
        pNewED->Show();
        m_aFixedTexts.push_back(pNewFT);
        m_aEdits.push_back(pNewED);
        pLastEdit = pNewED;
    }                    
    //scrollbar adjustment
    if(pLastEdit)
    {
        //the m_aWindow has to be at least as high as the ScrollBar and it must include the last Edit
        sal_Int32 nContentHeight = pLastEdit->GetPosPixel().Y() + nEDHeight + 
                m_aWindow.LogicToPixel(Size(RSC_SP_CTRL_GROUP_Y, RSC_SP_CTRL_GROUP_Y), MAP_APPFONT).Height();
        if(nContentHeight < m_aScrollBar.GetSizePixel().Height())
        {        
            nContentHeight = m_aScrollBar.GetSizePixel().Height();
            m_aScrollBar.Enable(FALSE);
        }
        else
        {        
            m_aScrollBar.Enable(TRUE);
            m_aScrollBar.SetRange(Range(0, nLines));
            m_aScrollBar.SetThumbPos(0);
            m_aScrollBar.SetVisibleSize(nVisibleLines);
        }
        Size aWinOutputSize(m_aWinOutputSize);
        aWinOutputSize.Height() = nContentHeight;
        m_aWindow.SetOutputSizePixel(aWinOutputSize);

    }        
}
/*-- 21.04.2004 11:37:09---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwAddressControl_Impl::SetCurrentDataSet(sal_uInt32 nSet)
{
    if(m_bNoDataSet || m_nCurrentDataSet != nSet)
    {        
        m_bNoDataSet = false;
        m_nCurrentDataSet = nSet;
        DBG_ASSERT(m_pData->aDBData.size() > m_nCurrentDataSet, "wrong data set index")
        if(m_pData->aDBData.size() > m_nCurrentDataSet)
        {
            ::std::vector<Edit*>::iterator aEditIter;
            sal_uInt32 nIndex = 0;
            for(aEditIter = m_aEdits.begin(); aEditIter != m_aEdits.end(); ++aEditIter, ++nIndex)
            {        
                DBG_ASSERT(nIndex < m_pData->aDBData[m_nCurrentDataSet].size(), 
                            "number of colums doesn't match number of Edits")
                (*aEditIter)->SetText(m_pData->aDBData[m_nCurrentDataSet][nIndex]);
            }
        }
    }
}

/*-- 19.04.2004 14:17:50---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwAddressControl_Impl, ScrollHdl_Impl, ScrollBar*, pScroll)
{
    long nThumb = pScroll->GetThumbPos();
    m_aWindow.SetPosPixel(Point(0, - (m_nLineHeight * nThumb)));

    return 0;
}
/*-- 19.04.2004 16:16:25---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwAddressControl_Impl, GotFocusHdl_Impl, Edit*, pEdit)
{
    if(0 != (GETFOCUS_TAB & pEdit->GetGetFocusFlags()))
    {        
        Rectangle aRect(pEdit->GetPosPixel(), pEdit->GetSizePixel());
        MakeVisible(aRect);
    }
    return 0;
}
/*-- 21.04.2004 14:56:54---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwAddressControl_Impl::MakeVisible(const Rectangle & rRect)
{
    long nThumb = m_aScrollBar.GetThumbPos();
    //determine range of visible positions
    long nMinVisiblePos = - m_aWindow.GetPosPixel().Y();
    long nMaxVisiblePos = m_aScrollBar.GetSizePixel().Height() + nMinVisiblePos;
    if( rRect.TopLeft().Y() < nMinVisiblePos)
    {
        nThumb -= 1 + ((nMinVisiblePos - rRect.TopLeft().Y()) / m_nLineHeight);
    }
    else if(rRect.BottomLeft().Y() > nMaxVisiblePos)
    {
        nThumb += 1 + ((nMaxVisiblePos - rRect.BottomLeft().Y()) / m_nLineHeight);
    }            
    if(nThumb != m_aScrollBar.GetThumbPos())
    {        
        m_aScrollBar.SetThumbPos(nThumb);
        ScrollHdl_Impl(&m_aScrollBar);
    }
}
/*-- 19.04.2004 16:16:25---------------------------------------------------
    copy data changes into database
  -----------------------------------------------------------------------*/
IMPL_LINK(SwAddressControl_Impl, EditModifyHdl_Impl, Edit*, pEdit)
{
    //get the data element number of the current set
    sal_Int32 nIndex = (sal_Int32)pEdit->GetData();
    //get the index of the set
    DBG_ASSERT(m_pData->aDBData.size() > m_nCurrentDataSet, "wrong data set index" );
    if(m_pData->aDBData.size() > m_nCurrentDataSet)
    {
        m_pData->aDBData[m_nCurrentDataSet][nIndex] = pEdit->GetText();
    }
    return 0;
}    
/*-- 21.04.2004 14:51:54---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwAddressControl_Impl::SetCursorTo(sal_uInt32 nElement)
{
    if(nElement < m_aEdits.size())
    {        
        Edit* pEdit = m_aEdits[nElement];
        pEdit->GrabFocus();
        Rectangle aRect(pEdit->GetPosPixel(), pEdit->GetSizePixel());
        MakeVisible(aRect);
    }

}
/*-- 19.04.2004 16:16:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwAddressControl_Impl::Command( const CommandEvent& rCEvt )
{
    switch ( rCEvt.GetCommand() )
    {
        case COMMAND_WHEEL:
        case COMMAND_STARTAUTOSCROLL:
        case COMMAND_AUTOSCROLL:
        {
            const CommandWheelData* pWheelData = rCEvt.GetWheelData();
            if(pWheelData && !pWheelData->IsHorz() && COMMAND_WHEEL_ZOOM != pWheelData->GetMode())
            {
                HandleScrollCommand( rCEvt, 0, &m_aScrollBar );
            }
        }
        break;
        default:
            Control::Command(rCEvt);
    }
}
/*-- 19.04.2004 16:16:25---------------------------------------------------

  -----------------------------------------------------------------------*/
long SwAddressControl_Impl::PreNotify( NotifyEvent& rNEvt )
{
    if(rNEvt.GetType() == EVENT_COMMAND)
    {
        const CommandEvent* pCEvt = rNEvt.GetCommandEvent();
        USHORT nCmd = pCEvt->GetCommand();
        if( COMMAND_WHEEL == nCmd )
        {
            Command(*pCEvt);
            return 1;
        }
    }
    return Control::PreNotify(rNEvt);
}
/*-- 13.04.2004 10:08:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwCreateAddressListDialog::SwCreateAddressListDialog(
        Window* pParent, const String& rURL, SwMailMergeConfigItem& rConfig) :
    SfxModalDialog(pParent, SW_RES(DLG_MM_CREATEADDRESSLIST)),
#pragma warning (disable : 4355)
    m_aAddressInformation( this, ResId(  FI_ADDRESSINFORMATION)),
    m_pAddressControl(new SwAddressControl_Impl(this, ResId(CT_ADDRESS))),
    m_aNewPB( this, ResId(               PB_NEW)),
    m_aDeletePB( this, ResId(            PB_DELETE)),
    m_aFindPB( this, ResId(              PB_FIND)),
    m_aCustomizePB( this, ResId(         PB_CUSTOMIZE)),

    m_aViewEntriesFI( this, ResId(       FI_VIEWENTRIES)),
    m_aStartPB( this, ResId(             PB_START)),
    m_aPrevPB( this, ResId(              PB_PREV)),
    m_aSetNoNF( this, ResId(             NF_SETNO)),
    m_aNextPB( this, ResId(              PB_NEXT  )),
    m_aEndPB( this, ResId(               PB_END)),

    m_aSeparatorFL( this, ResId(         FL_SEPARATOR)),

    m_aOK( this, ResId(                  PB_OK)),
    m_aCancel( this, ResId(              PB_CANCEL)),
    m_aHelp( this, ResId(                PB_HELP)),
#pragma warning (default : 4355)
    m_sAddressListFilterName( ResId(    ST_FILTERNAME)),
    m_sURL(rURL),
    m_pCSVData( new SwCSVData ),
    m_pFindDlg(0)
{
    FreeResource();
    m_aNewPB.SetClickHdl(LINK(this, SwCreateAddressListDialog, NewHdl_Impl));
    m_aDeletePB.SetClickHdl(LINK(this, SwCreateAddressListDialog, DeleteHdl_Impl));
    m_aFindPB.SetClickHdl(LINK(this, SwCreateAddressListDialog, FindHdl_Impl));
    m_aCustomizePB.SetClickHdl(LINK(this, SwCreateAddressListDialog, CustomizeHdl_Impl));
    m_aOK.SetClickHdl(LINK(this, SwCreateAddressListDialog, OkHdl_Impl));

    Link aLk = LINK(this, SwCreateAddressListDialog, DBCursorHdl_Impl);
    m_aStartPB.SetClickHdl(aLk);
    m_aPrevPB.SetClickHdl(aLk);
    m_aSetNoNF.SetModifyHdl(LINK(this, SwCreateAddressListDialog, DBNumCursorHdl_Impl));
    m_aNextPB.SetClickHdl(aLk);
    m_aEndPB.SetClickHdl(aLk);

    if(m_sURL.Len())
    {
        //file exists, has to be loaded here
        SfxMedium aMedium( m_sURL, STREAM_READ, TRUE );
        SvStream* pStream = aMedium.GetInStream();
        if(pStream)
        {
            pStream->SetLineDelimiter( LINEEND_LF );
            pStream->SetStreamCharSet(RTL_TEXTENCODING_UTF8);
    
            OUString sSemi(';');
            OUString sQuote('"');
            String sTempMiddle(sQuote);
            sTempMiddle += sal_Unicode(9);
            OUString sMiddle(sTempMiddle); 

            String sLine;
            BOOL bRead = pStream->ReadUniOrByteStringLine( sLine, RTL_TEXTENCODING_UTF8 );

            if(bRead)
            {
                //header line
                xub_StrLen nHeaders = sLine.GetTokenCount('\t');
                xub_StrLen nIndex = 0;
                for( xub_StrLen nToken = 0; nToken < nHeaders; ++nToken)
                {        
                    String sHeader = sLine.GetToken( 0, '\t', nIndex );
                    DBG_ASSERT(sHeader.Len() > 2 && 
                            sHeader.GetChar(0) == '\"' && sHeader.GetChar(sHeader.Len() - 1) == '\"', 
                            "Wrong format of header")
                    if(sHeader.Len() > 2)
                    {
                        m_pCSVData->aDBColumnHeaders.push_back( sHeader.Copy(1, sHeader.Len() -2));
                    }
                }
            }            
            while(pStream->ReadUniOrByteStringLine( sLine, RTL_TEXTENCODING_UTF8 ))
            {
                ::std::vector<OUString> aNewData;
                //analyze data line
                xub_StrLen nDataCount = sLine.GetTokenCount('\t');
                xub_StrLen nIndex = 0;
                for( xub_StrLen nToken = 0; nToken < nDataCount; ++nToken)
                {        
                    String sData = sLine.GetToken( 0, '\t', nIndex );
                    DBG_ASSERT(sData.Len() >= 2 && 
                                sData.GetChar(0) == '\"' && sData.GetChar(sData.Len() - 1) == '\"', 
                            "Wrong format of line")
                    if(sData.Len() >= 2)
                        aNewData.push_back(sData.Copy(1, sData.Len() - 2));
                    else
                        aNewData.push_back(sData);
                }
                m_pCSVData->aDBData.push_back( aNewData );
            }
        }            
    }        
    else
    {
        //database has to be created 
        const ResStringArray& rAddressHeader = rConfig.GetDefaultAddressHeaders();
        USHORT nCount = rAddressHeader.Count();
        for(USHORT nHeader = 0; nHeader < nCount; ++nHeader)
            m_pCSVData->aDBColumnHeaders.push_back( rAddressHeader.GetString((USHORT)nHeader));
        ::std::vector<OUString> aNewData;
        String sTemp;
        aNewData.insert(aNewData.begin(), nCount, sTemp);
        m_pCSVData->aDBData.push_back(aNewData);
    }
    //now fill the address control
    m_pAddressControl->SetData(*m_pCSVData);
    m_pAddressControl->SetCurrentDataSet(0);
    m_aSetNoNF.SetMax(m_pCSVData->aDBData.size());
    UpdateButtons();
}
/*-- 13.04.2004 10:08:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwCreateAddressListDialog::~SwCreateAddressListDialog()
{
    delete m_pAddressControl;
    delete m_pCSVData;
    delete m_pFindDlg;
}
/*-- 13.04.2004 10:08:59---------------------------------------------------
    add a new data set of empty strings and set the address input control 
    to that new set
  -----------------------------------------------------------------------*/
IMPL_LINK(SwCreateAddressListDialog, NewHdl_Impl, PushButton*, pButton)
{
    sal_uInt32 nCurrent = m_pAddressControl->GetCurrentDataSet();
    ::std::vector<OUString> aNewData;
    String sTemp;
    aNewData.insert(aNewData.begin(), m_pCSVData->aDBColumnHeaders.size(), sTemp);
    m_pCSVData->aDBData.insert(m_pCSVData->aDBData.begin() + ++nCurrent, aNewData);
    m_aSetNoNF.SetMax(m_pCSVData->aDBData.size());
    //the NumericField start at 1
    m_aSetNoNF.SetValue(nCurrent + 1);
    //the address control starts at 0
    m_pAddressControl->SetCurrentDataSet(nCurrent);
    UpdateButtons();
    return 0;
}
/*-- 13.04.2004 10:09:00---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwCreateAddressListDialog, DeleteHdl_Impl, PushButton*, pButton)
{
    sal_uInt32 nCurrent = m_pAddressControl->GetCurrentDataSet();
    if(m_pCSVData->aDBData.size() > 1)
    {        
        m_pCSVData->aDBData.erase(m_pCSVData->aDBData.begin() + nCurrent);
        if(nCurrent)
            --nCurrent;
    }
    else
    {        
        // if only one set is available then clear the data
        String sTemp;
        m_pCSVData->aDBData[0].assign(m_pCSVData->aDBData[0].size(), sTemp);
        m_aDeletePB.Enable(FALSE);
    }
    m_pAddressControl->SetCurrentDataSet(nCurrent);
    m_aSetNoNF.SetMax(m_pCSVData->aDBData.size());
    UpdateButtons();
    return 0;
}
/*-- 13.04.2004 10:09:00---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwCreateAddressListDialog, FindHdl_Impl, PushButton*, pButton)
{
    if(!m_pFindDlg)
    {        
        m_pFindDlg = new SwFindEntryDialog(this);
        ListBox& rColumnBox = m_pFindDlg->GetFieldsListBox();
        ::std::vector< OUString >::iterator  aHeaderIter;
        for(aHeaderIter = m_pCSVData->aDBColumnHeaders.begin(); 
                    aHeaderIter != m_pCSVData->aDBColumnHeaders.end(); 
                    ++aHeaderIter)
            rColumnBox.InsertEntry(*aHeaderIter);
    }      
    else
        m_pFindDlg->Show(!m_pFindDlg->IsVisible());
    return 0;
}
/*-- 13.04.2004 10:09:00---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwCreateAddressListDialog, CustomizeHdl_Impl, PushButton*, pButton)
{
    SwCustomizeAddressListDialog* pDlg = new SwCustomizeAddressListDialog(pButton, *m_pCSVData);
    if(RET_OK == pDlg->Execute())
    {        
        delete m_pCSVData;
        m_pCSVData = pDlg->GetNewData();
        m_pAddressControl->SetData(*m_pCSVData);
        m_pAddressControl->SetCurrentDataSet(m_pAddressControl->GetCurrentDataSet());
    }
    delete pDlg;

    //update find dialog
    if(m_pFindDlg)
    {        
        ListBox& rColumnBox = m_pFindDlg->GetFieldsListBox();
        rColumnBox.Clear();
        ::std::vector< OUString >::iterator  aHeaderIter;
        for(aHeaderIter = m_pCSVData->aDBColumnHeaders.begin(); 
                    aHeaderIter != m_pCSVData->aDBColumnHeaders.end(); 
                    ++aHeaderIter)
            rColumnBox.InsertEntry(*aHeaderIter);
    }
    return 0;
}
/*-- 23.04.2004 09:02:51---------------------------------------------------
    writes the data into a .csv file
    encoding is UTF8, separator is tab, strings are enclosed into "
  -----------------------------------------------------------------------*/
IMPL_LINK(SwCreateAddressListDialog, OkHdl_Impl, PushButton*, pButton)
{
    if(!m_sURL.Len())
    {
        sfx2::FileDialogHelper aDlgHelper( TemplateDescription::FILESAVE_SIMPLE, 0 );
        Reference < XFilePicker > xFP = aDlgHelper.GetFilePicker();

        String sPath( SvtPathOptions().SubstituteVariable( 
                    String::CreateFromAscii("$(userurl)/database") ));
        aDlgHelper.SetDisplayDirectory( sPath );
        Reference<XFilterManager> xFltMgr(xFP, UNO_QUERY);
        ::rtl::OUString sCSV(C2U("*.csv"));
        xFltMgr->appendFilter( m_sAddressListFilterName, sCSV );
        xFltMgr->setCurrentFilter( m_sAddressListFilterName ) ;

        if( ERRCODE_NONE == aDlgHelper.Execute() )
        {        
            m_sURL = xFP->getFiles().getConstArray()[0];
            INetURLObject aResult( m_sURL );
            aResult.setExtension(String::CreateFromAscii("csv"));
            m_sURL = aResult.GetMainURL(INetURLObject::NO_DECODE);
        }
    }
    if(m_sURL.Len())
    {        
        SfxMedium aMedium( m_sURL, STREAM_READWRITE|STREAM_TRUNC, TRUE );
        SvStream* pStream = aMedium.GetOutStream();
        pStream->SetLineDelimiter( LINEEND_LF );
        pStream->SetStreamCharSet(RTL_TEXTENCODING_UTF8);
    
        OUString sSemi(';');
        OUString sQuote('"');
        String sTempMiddle(sQuote);
        sTempMiddle += sal_Unicode(9);
        OUString sMiddle(sTempMiddle); 
        sMiddle += sQuote; 

        //create a string for the header line
        OUString sLine(sQuote);
        ::std::vector< OUString >::iterator  aHeaderIter;
        for(aHeaderIter = m_pCSVData->aDBColumnHeaders.begin(); 
                    aHeaderIter != m_pCSVData->aDBColumnHeaders.end(); 
                    ++aHeaderIter)
        {
            sLine += *aHeaderIter;
            sLine += sMiddle;
        }                        
        //remove tab and quote
        sLine = sLine.copy( 0, sLine.getLength() - 2 );
        pStream->WriteUnicodeOrByteText( sLine, RTL_TEXTENCODING_UTF8 );
        endl(*pStream);

        ::std::vector< ::std::vector< OUString > >::iterator aDataIter;
        for( aDataIter = m_pCSVData->aDBData.begin(); aDataIter != m_pCSVData->aDBData.end(); ++aDataIter)
        {
            sLine = sQuote;
            ::std::vector< OUString >::iterator  aColumnIter;
            for(aColumnIter = aDataIter->begin(); aColumnIter != aDataIter->end(); ++aColumnIter)
            {
                sLine += *aColumnIter;
                sLine += sMiddle;
            }            
            //remove tab and quote
            sLine = sLine.copy( 0, sLine.getLength() - 2 );
            pStream->WriteUnicodeOrByteText( sLine, RTL_TEXTENCODING_UTF8 );
            endl(*pStream);
        }                        
        aMedium.Commit();
        EndDialog(RET_OK);
    }            
                    
    return 0;
}
/*-- 13.04.2004 10:09:01---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwCreateAddressListDialog, DBCursorHdl_Impl, PushButton*, pButton)
{
    sal_uInt32 nValue = m_aSetNoNF.GetValue();

    if(pButton == &m_aStartPB)
        nValue = 1;
    else if(pButton == &m_aPrevPB)
    {
        if(nValue > 1) 
            --nValue;
    }
    else if(pButton == &m_aNextPB)
    {
        if(nValue < (sal_uInt32)m_aSetNoNF.GetMax())
            ++nValue;
    }
    else //m_aEndPB
        nValue = m_aSetNoNF.GetMax();
    if(nValue != m_aSetNoNF.GetValue())
    {        
        m_aSetNoNF.SetValue(nValue);
        DBNumCursorHdl_Impl(&m_aSetNoNF);
    }
    return 0;
}
/*-- 21.04.2004 12:06:47---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwCreateAddressListDialog, DBNumCursorHdl_Impl, NumericField*, pField)
{
    m_pAddressControl->SetCurrentDataSet(m_aSetNoNF.GetValue() - 1);
    UpdateButtons();
    return 0;
}
/*-- 21.04.2004 13:22:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwCreateAddressListDialog::UpdateButtons()
{
    sal_uInt32 nCurrent = m_aSetNoNF.GetValue();
    sal_uInt32 nSize = (sal_uInt32 )m_pCSVData->aDBData.size();
    m_aStartPB.Enable(nCurrent != 1);
    m_aPrevPB.Enable(nCurrent != 1);
    m_aNextPB.Enable(nCurrent != nSize);
    m_aEndPB.Enable(nCurrent != nSize);
    m_aDeletePB.Enable(nSize > 0);
}
/*-- 21.04.2004 13:22:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwCreateAddressListDialog::Find(const String& rSearch, sal_Int32 nColumn)
{
    OUString sSearch = rSearch;
    sSearch.toAsciiLowerCase();
    sal_uInt32 nCurrent = m_pAddressControl->GetCurrentDataSet();
    //search forward
    bool bFound = false;
    sal_uInt32 nStart = nCurrent + 1;
    sal_uInt32 nEnd = m_pCSVData->aDBData.size();
    sal_uInt32 nElement = 0;
    sal_uInt32 nPos = 0;
    for(short nTemp = 0; nTemp < 2 && !bFound; nTemp++)
    {        
        for(nPos = nStart; nPos < nEnd; ++nPos)
        {
            ::std::vector< OUString> aData = m_pCSVData->aDBData[nPos];
            if(nColumn >=0)
                bFound = -1 != aData[(sal_uInt32)nColumn].toAsciiLowerCase().indexOf(sSearch);
            else 
            {
                for( nElement = 0; nElement <= aData.size(); ++nElement)
                {
                    bFound = aData[nElement].toAsciiLowerCase().indexOf(sSearch);
                    if(bFound)
                    {
                        nColumn = nElement;
                        break;
                    }
                }            
            }
            if(bFound)
                break;
        }                       
        nStart = 0;
        nEnd = nCurrent;
    }
    if(bFound)
    {        
        m_pAddressControl->SetCurrentDataSet(nPos);
        m_pAddressControl->SetCursorTo(nElement);
    }            
}            
/*-- 13.04.2004 13:48:38---------------------------------------------------

  -----------------------------------------------------------------------*/
SwFindEntryDialog::SwFindEntryDialog(SwCreateAddressListDialog* pParent) :
    ModelessDialog(pParent, SW_RES(DLG_MM_FIND_ENTRY)),
#pragma warning (disable : 4355)
    m_aFindFT( this, ResId(      FT_FIND      )),
    m_aFindED( this, ResId(      ED_FIND      )),
    m_aFindOnlyCB( this, ResId(  CB_FINDONLY )),
    m_aFindOnlyLB( this, ResId(  LB_FINDONLY  )),
    m_aFindPB( this, ResId(      PB_FIND)),
    m_aCancel( this, ResId(      PB_CANCEL)),
    m_aHelp( this, ResId(        PB_HELP)),         
#pragma warning (default : 4355)
    m_pParent(pParent)
{
    FreeResource();
    m_aFindPB.SetClickHdl(LINK(this, SwFindEntryDialog, FindHdl_Impl));
    m_aFindED.SetModifyHdl(LINK(this, SwFindEntryDialog, FindEnableHdl_Impl));
    m_aCancel.SetClickHdl(LINK(this, SwFindEntryDialog, CloseHdl_Impl));
}
/*-- 13.04.2004 13:48:38---------------------------------------------------

  -----------------------------------------------------------------------*/
SwFindEntryDialog::~SwFindEntryDialog()
{
}
/*-- 21.04.2004 13:37:46---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwFindEntryDialog, FindHdl_Impl, PushButton*, EMPTYARG)
{
    sal_Int32 nColumn = -1;
    if(m_aFindOnlyCB.IsChecked())
        nColumn = m_aFindOnlyLB.GetSelectEntryPos();
    m_pParent->Find(m_aFindED.GetText(), nColumn);
    return 0;
}
/*-- 21.04.2004 13:37:46---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwFindEntryDialog, FindEnableHdl_Impl, Edit*, EMPTYARG)
{
    m_aFindPB.Enable(m_aFindED.GetText().Len() > 0);
    return 0;
}
/*-- 21.04.2004 15:36:36---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwFindEntryDialog, CloseHdl_Impl, PushButton*, EMPTYARG)
{
    Show(FALSE);
    return 0;
}
