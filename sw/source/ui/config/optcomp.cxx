/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif

#include "optcomp.hxx"

#include "docsh.hxx"
#include "swmodule.hxx"
#include "swtypes.hxx"
#include "uiitems.hxx"
#include "view.hxx"
#include "wrtsh.hxx"

#include "optcomp.hrc"
#include "globals.hrc"
#include <tools/urlobj.hxx>
#ifndef _UTL__HXX_
#include <unotools/configmgr.hxx>
#endif
#include <vcl/msgbox.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/fcontnr.hxx>
#include <IDocumentSettingAccess.hxx>

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::uno;
using ::rtl::OUString;
using namespace ::std;

#define ASCII_STR(s)		OUString( RTL_CONSTASCII_USTRINGPARAM( s ) )
#define DEFAULT_ENTRY		COMPATIBILITY_DEFAULT_NAME
#define USER_ENTRY			ASCII_STR( "_user" )
#define BUTTON_BORDER		2

// struct CompatibilityItem ----------------------------------------------

struct CompatibilityItem
{
    String		m_sName;
    String		m_sModule;
    bool		m_bUsePrtMetrics;
    bool		m_bAddSpacing;
    bool		m_bAddSpacingAtPages;
    bool		m_bUseOurTabStops;
    bool		m_bNoExtLeading;
    bool		m_bUseLineSpacing;
    bool		m_bAddTableSpacing;
    bool		m_bUseObjPos;
    bool		m_bUseOurTextWrapping;
    bool		m_bConsiderWrappingStyle;
    bool        m_bExpandWordSpace;
    bool		m_bIsDefault;
    bool		m_bIsUser;

    CompatibilityItem( const String& _rName, const String& _rModule,
                       bool _bUsePrtMetrics, bool _bAddSpacing, bool _bAddSpacingAtPages,
                       bool _bUseOurTabStops, bool _bNoExtLeading, bool _bUseLineSpacing,
                       bool _bAddTableSpacing, bool _bUseObjPos, bool _bUseOurTextWrapping,
                       bool _bConsiderWrappingStyle, bool _bExpandWordSpace,
                       bool _bIsDefault, bool _bIsUser ) :

        m_sName					( _rName ),
        m_sModule				( _rModule ),
        m_bUsePrtMetrics		( _bUsePrtMetrics ),
        m_bAddSpacing			( _bAddSpacing ),
        m_bAddSpacingAtPages	( _bAddSpacingAtPages ),
        m_bUseOurTabStops		( _bUseOurTabStops ),
        m_bNoExtLeading			( _bNoExtLeading ),
        m_bUseLineSpacing		( _bUseLineSpacing ),
        m_bAddTableSpacing		( _bAddTableSpacing ),
        m_bUseObjPos			( _bUseObjPos ),
        m_bUseOurTextWrapping	( _bUseOurTextWrapping ),
        m_bConsiderWrappingStyle( _bConsiderWrappingStyle ),
        m_bExpandWordSpace      ( _bExpandWordSpace ),
        m_bIsDefault			( _bIsDefault ),
        m_bIsUser				( _bIsUser ) {}
};

#include <vector>

struct SwCompatibilityOptPage_Impl
{
    typedef vector< CompatibilityItem > SwCompatibilityItemList;

    SwCompatibilityItemList		m_aList;
};

// class SwCompatibilityOptPage ------------------------------------------

SwCompatibilityOptPage::SwCompatibilityOptPage( Window* pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, SW_RES( TP_OPTCOMPATIBILITY_PAGE ), rSet ),

    m_aMainFL			( this, SW_RES( FL_MAIN ) ),
    m_aFormattingFT		( this, SW_RES( FT_FORMATTING ) ),
    m_aFormattingLB		( this, SW_RES( LB_FORMATTING ) ),
    m_aOptionsFT		( this, SW_RES( FT_OPTIONS ) ),
    m_aOptionsLB		( this, SW_RES( LB_OPTIONS ) ),
    m_aResetPB			( this, SW_RES( PB_RESET ) ),
    m_aDefaultPB		( this, SW_RES( PB_DEFAULT ) ),
    m_sUserEntry		( 		SW_RES( STR_USERENTRY ) ),
    m_sUseAsDefaultQuery( 		SW_RES( STR_QRYBOX_USEASDEFAULT ) ),
    m_pWrtShell			( NULL ),
    m_pImpl				( new SwCompatibilityOptPage_Impl ),
    m_nSavedOptions		( 0 )

{
    // init options strings with local resource ids -> so do it before FreeResource()
    for ( USHORT nResId = STR_COMP_OPTIONS_START; nResId < STR_COMP_OPTIONS_END; ++nResId )
    {
        String sEntry = String( SW_RES( nResId ) );
        if ( STR_TAB_ALIGNMENT == nResId ||
             STR_LINE_SPACING == nResId ||
             STR_USE_OBJPOSITIONING == nResId ||
             STR_USE_OURTEXTWRAPPING == nResId )
            ReplaceFormatName( sEntry );
        SvLBoxEntry* pEntry = m_aOptionsLB.SvTreeListBox::InsertEntry( sEntry );
        if ( pEntry )
        {
            m_aOptionsLB.SetCheckButtonState( pEntry, SV_BUTTON_UNCHECKED );
            pEntry->SetUserData( (void*)(ULONG)nResId );
        }
    }
    m_aOptionsLB.SetWindowBits( m_aOptionsLB.GetStyle() | WB_HSCROLL | WB_HIDESELECTION );
    m_aOptionsLB.SetHighlightRange();

    FreeResource();

    InitControls( rSet );

    // set handler
    m_aFormattingLB.SetSelectHdl( LINK( this, SwCompatibilityOptPage, SelectHdl ) );
    m_aDefaultPB.SetClickHdl( LINK( this, SwCompatibilityOptPage, UseAsDefaultHdl ) );

    // hide some controls, will be implemented later!!!
    m_aFormattingFT.Hide();
    m_aFormattingLB.Hide();
    m_aResetPB.Hide();
    // so move and resize the other controls
    Point aMovePnt = m_aFormattingFT.GetPosPixel();
    Point aNewPnt = m_aOptionsFT.GetPosPixel();
    aNewPnt.Y() = aMovePnt.Y();
    m_aOptionsFT.SetPosPixel( aNewPnt );
    aMovePnt = m_aFormattingLB.GetPosPixel();
    aNewPnt = m_aOptionsLB.GetPosPixel();
    long nDelta = aNewPnt.Y() - aMovePnt.Y();
    aNewPnt.Y() = aMovePnt.Y();
    m_aOptionsLB.SetPosPixel( aNewPnt );
    Size aNewSz = m_aOptionsLB.GetSizePixel();
    aNewSz.Height() += nDelta;
    m_aOptionsLB.SetSizePixel( aNewSz );
}

// -----------------------------------------------------------------------

SwCompatibilityOptPage::~SwCompatibilityOptPage()
{
    delete m_pImpl;
}

// -----------------------------------------------------------------------

void SwCompatibilityOptPage::ReplaceFormatName( String& rEntry )
{
    static const String sOpenOfficeName = String::CreateFromAscii("OpenOffice.org");
    static const String sAsianName = String::CreateFromAscii("StarSuite");

    Any aAny = ::utl::ConfigManager::
        GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTNAME );
    rtl::OUString sTmp;
    if ( aAny >>= sTmp )
    {
        String sFormatName = sTmp;
        String sFormatVersion;
        bool bOpenOffice = ( sOpenOfficeName == sFormatName );
        if ( bOpenOffice )
            sFormatVersion = String::CreateFromAscii("1.1");
        else
            sFormatVersion = String::CreateFromAscii("6.0/7");
        if ( !bOpenOffice && ( sAsianName != sFormatName ) )
            sFormatName = String::CreateFromAscii("StarOffice");

        rEntry.SearchAndReplace( String::CreateFromAscii("%FORMATNAME"), sFormatName );
        rEntry.SearchAndReplace( String::CreateFromAscii("%FORMATVERSION"), sFormatVersion );
    }
}

// -----------------------------------------------------------------------

ULONG convertBools2Ulong_Impl
(
    bool _bUsePrtMetrics,
    bool _bAddSpacing,
    bool _bAddSpacingAtPages,
    bool _bUseOurTabStops,
    bool _bNoExtLeading,
    bool _bUseLineSpacing,
    bool _bAddTableSpacing,
    bool _bUseObjPos,
    bool _bUseOurTextWrapping,
    bool _bConsiderWrappingStyle,
    bool _bExpandWordSpace
)
{
    ULONG nRet = 0;
    ULONG nSetBit = 1;

    if ( _bUsePrtMetrics )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bAddSpacing )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bAddSpacingAtPages )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bUseOurTabStops )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bNoExtLeading )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bUseLineSpacing )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bAddTableSpacing )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bUseObjPos )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bUseOurTextWrapping )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bConsiderWrappingStyle )
        nRet |= nSetBit;
    nSetBit = nSetBit << 1;
    if ( _bExpandWordSpace )
        nRet |= nSetBit;

    return nRet;
}

// -----------------------------------------------------------------------

void SwCompatibilityOptPage::InitControls( const SfxItemSet& rSet )
{
    // init objectshell and detect document name
    String sDocTitle;
    const SfxPoolItem* pItem = NULL;
    SfxObjectShell* pObjShell = NULL;
    if ( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_WRTSHELL, FALSE, &pItem ) )
        m_pWrtShell = (SwWrtShell*)( (const SwPtrItem*)pItem )->GetValue();
    if ( m_pWrtShell )
    {
        pObjShell = m_pWrtShell->GetView().GetDocShell();
        if ( pObjShell )
            sDocTitle = pObjShell->GetTitle( SFX_TITLE_TITLE );
    }
    else
    {
        m_aMainFL.Disable();
        m_aFormattingFT.Disable();
        m_aFormattingLB.Disable();
        m_aOptionsFT.Disable();
        m_aOptionsLB.Disable();
        m_aResetPB.Disable();
        m_aDefaultPB.Disable();
    }
    String sText = m_aMainFL.GetText();
    sText.SearchAndReplace( String::CreateFromAscii("%DOCNAME"), sDocTitle );
    m_aMainFL.SetText( sText );

    // loading file formats
    Sequence< Sequence< PropertyValue > > aList = m_aConfigItem.GetList();
    OUString sName;
    OUString sModule;
    bool bUsePrtMetrics = false;
    bool bAddSpacing = false;
    bool bAddSpacingAtPages = false;
    bool bUseOurTabStops = false;
    bool bNoExtLeading = false;
    bool bUseLineSpacing = false;
    bool bAddTableSpacing = false;
    bool bUseObjPos = false;
    bool bUseOurTextWrapping = false;
    bool bConsiderWrappingStyle = false;
    bool bExpandWordSpace = false;
    int i, j, nCount = aList.getLength();
    for ( i = 0; i < nCount; ++i )
    {
        String sNewEntry;
        const Sequence< PropertyValue >& rEntry = aList[i];
        for ( j = 0; j < rEntry.getLength(); j++ )
        {
            PropertyValue aValue = rEntry[j];
            if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_NAME )
                aValue.Value >>= sName;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_MODULE )
                aValue.Value >>= sModule;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_USEPRTMETRICS )
                aValue.Value >>= bUsePrtMetrics;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_ADDSPACING )
                aValue.Value >>= bAddSpacing;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_ADDSPACINGATPAGES )
                aValue.Value >>= bAddSpacingAtPages;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_USEOURTABSTOPS )
                aValue.Value >>= bUseOurTabStops;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_NOEXTLEADING )
                aValue.Value >>= bNoExtLeading;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_USELINESPACING )
                aValue.Value >>= bUseLineSpacing;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_ADDTABLESPACING )
                aValue.Value >>= bAddTableSpacing;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_USEOBJECTPOSITIONING )
                aValue.Value >>= bUseObjPos;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_USEOURTEXTWRAPPING )
                aValue.Value >>= bUseOurTextWrapping;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_CONSIDERWRAPPINGSTYLE )
                aValue.Value >>= bConsiderWrappingStyle;
            else if ( aValue.Name == COMPATIBILITY_PROPERTYNAME_EXPANDWORDSPACE )
                aValue.Value >>= bExpandWordSpace;
        }

        CompatibilityItem aItem(
            sName, sModule, bUsePrtMetrics, bAddSpacing,
            bAddSpacingAtPages, bUseOurTabStops, bNoExtLeading,
            bUseLineSpacing, bAddTableSpacing, bUseObjPos,
            bUseOurTextWrapping, bConsiderWrappingStyle, bExpandWordSpace,
            ( sName.equals( DEFAULT_ENTRY ) != sal_False ),
            ( sName.equals( USER_ENTRY ) != sal_False ) );
        m_pImpl->m_aList.push_back( aItem );

        if ( aItem.m_bIsDefault )
            continue;

        if ( sName.equals( USER_ENTRY ) )
            sNewEntry = m_sUserEntry;
        else if ( pObjShell && sName.getLength() > 0 )
        {
            SfxFilterContainer* pFacCont = pObjShell->GetFactory().GetFilterContainer();
            const SfxFilter* pFilter = pFacCont->GetFilter4FilterName( sName );
            if ( pFilter )
                sNewEntry = pFilter->GetUIName();
        }

        if ( sNewEntry.Len() == 0 )
            sNewEntry = sName;

        USHORT nPos = m_aFormattingLB.InsertEntry( sNewEntry );
        ULONG nOptions = convertBools2Ulong_Impl(
            bUsePrtMetrics, bAddSpacing, bAddSpacingAtPages,
            bUseOurTabStops, bNoExtLeading, bUseLineSpacing,
            bAddTableSpacing, bUseObjPos, bUseOurTextWrapping,
            bConsiderWrappingStyle, bExpandWordSpace );
        m_aFormattingLB.SetEntryData( nPos, (void*)(long)nOptions );
    }

    m_aFormattingLB.SetDropDownLineCount( m_aFormattingLB.GetEntryCount() );

    // check if the default button text is not too wide otherwise we have to stretch the button
    // and move its position and the position of the reset button
    long nTxtWidth = m_aDefaultPB.GetTextWidth( m_aDefaultPB.GetText() );
    Size aBtnSz = m_aDefaultPB.GetSizePixel();
    if ( nTxtWidth > aBtnSz.Width() )
    {
        long nDelta = nTxtWidth - aBtnSz.Width() + 2 * BUTTON_BORDER;
        aBtnSz.Width() += nDelta;
        Point aBtnPnt = m_aDefaultPB.GetPosPixel();
        aBtnPnt.X() -= nDelta;
        m_aDefaultPB.SetPosSizePixel( aBtnPnt, aBtnSz );
        aBtnPnt = m_aResetPB.GetPosPixel();
        aBtnPnt.X() -= 2 * nDelta;
        m_aResetPB.SetPosSizePixel( aBtnPnt, aBtnSz );
    }
}

// -----------------------------------------------------------------------

IMPL_LINK( SwCompatibilityOptPage, SelectHdl, ListBox*, EMPTYARG )
{
    USHORT nPos = m_aFormattingLB.GetSelectEntryPos();
    ULONG nOptions = (ULONG)(void*)m_aFormattingLB.GetEntryData( nPos );
    SetCurrentOptions( nOptions );

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SwCompatibilityOptPage, UseAsDefaultHdl, PushButton*, EMPTYARG )
{
    QueryBox aBox( this, WinBits( WB_YES_NO | WB_DEF_YES ), m_sUseAsDefaultQuery );
    if ( aBox.Execute() == RET_YES )
    {
        for ( vector< CompatibilityItem >::iterator pItem = m_pImpl->m_aList.begin();
              pItem != m_pImpl->m_aList.end(); ++pItem )
        {
            if ( pItem->m_bIsDefault )
            {
                USHORT nCount = static_cast< USHORT >( m_aOptionsLB.GetEntryCount() );
                for ( USHORT i = 0; i < nCount; ++i )
                {
                    bool bChecked = ( m_aOptionsLB.IsChecked(i) != FALSE );
                    CompatibilityOptions eOption = static_cast< CompatibilityOptions >(i);
                    switch ( eOption )
                    {
                        case COPT_USE_PRINTERDEVICE : pItem->m_bUsePrtMetrics = bChecked; break;
                        case COPT_ADD_SPACING :	pItem->m_bAddSpacing = bChecked; break;
                        case COPT_ADD_SPACING_AT_PAGES : pItem->m_bAddSpacingAtPages = bChecked; break;
                        case COPT_USE_OUR_TABSTOPS : pItem->m_bUseOurTabStops = bChecked; break;
                        case COPT_NO_EXTLEADING : pItem->m_bNoExtLeading = bChecked; break;
                        case COPT_USE_LINESPACING : pItem->m_bUseLineSpacing = bChecked; break;
                        case COPT_ADD_TABLESPACING : pItem->m_bAddTableSpacing = bChecked; break;
                        case COPT_USE_OBJECTPOSITIONING: pItem->m_bUseObjPos = bChecked; break;
                        case COPT_USE_OUR_TEXTWRAPPING: pItem->m_bUseOurTextWrapping = bChecked; break;
                        case COPT_CONSIDER_WRAPPINGSTYLE: pItem->m_bConsiderWrappingStyle = bChecked; break;
                        case COPT_EXPAND_WORDSPACE:  pItem->m_bExpandWordSpace = bChecked; break;
                        default:
                        {
                            DBG_ERRORFILE( "SwCompatibilityOptPage::UseAsDefaultHdl(): wrong option" );
                        }
                    }
                }
                break;
            }
        }

        WriteOptions();
    }

    return 0;
}

// -----------------------------------------------------------------------

void SwCompatibilityOptPage::SetCurrentOptions( ULONG nOptions )
{
    ULONG nCount = m_aOptionsLB.GetEntryCount();
    DBG_ASSERT( nCount <= 32, "SwCompatibilityOptPage::Reset(): entry overflow" );
    for ( USHORT i = 0; i < nCount; ++i )
    {
        BOOL bChecked = ( ( nOptions & 0x00000001 ) == 0x00000001 );
        m_aOptionsLB.CheckEntryPos( i, bChecked );
        nOptions = nOptions >> 1;
    }
}

// -----------------------------------------------------------------------

ULONG SwCompatibilityOptPage::GetDocumentOptions() const
{
    ULONG nRet = 0;
    if ( m_pWrtShell )
    {
        const IDocumentSettingAccess& rIDocumentSettingAccess = *m_pWrtShell->getIDocumentSettingAccess();
        nRet = convertBools2Ulong_Impl(
                rIDocumentSettingAccess.get(IDocumentSettingAccess::USE_VIRTUAL_DEVICE) == sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::PARA_SPACE_MAX) != sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::PARA_SPACE_MAX_AT_PAGES) != sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::TAB_COMPAT) == sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::ADD_EXT_LEADING) == sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::OLD_LINE_SPACING) != sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::ADD_PARA_SPACING_TO_TABLE_CELLS) != sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::USE_FORMER_OBJECT_POS) != sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::USE_FORMER_TEXT_WRAPPING) != sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::CONSIDER_WRAP_ON_OBJECT_POSITION) != sal_False,
                rIDocumentSettingAccess.get(IDocumentSettingAccess::DO_NOT_JUSTIFY_LINES_WITH_MANUAL_BREAK) != sal_True );
    }
    return nRet;
}

// -----------------------------------------------------------------------

void SwCompatibilityOptPage::WriteOptions()
{
    m_aConfigItem.Clear();
    for ( vector< CompatibilityItem >::const_iterator pItem = m_pImpl->m_aList.begin();
          pItem != m_pImpl->m_aList.end(); ++pItem )
        m_aConfigItem.AppendItem(
            pItem->m_sName, pItem->m_sModule, pItem->m_bUsePrtMetrics, pItem->m_bAddSpacing,
            pItem->m_bAddSpacingAtPages, pItem->m_bUseOurTabStops,
            pItem->m_bNoExtLeading, pItem->m_bUseLineSpacing,
            pItem->m_bAddTableSpacing, pItem->m_bUseObjPos,
            pItem->m_bUseOurTextWrapping, pItem->m_bConsiderWrappingStyle,
            pItem->m_bExpandWordSpace );
}

// -----------------------------------------------------------------------

SfxTabPage*	SwCompatibilityOptPage::Create( Window* pParent, const SfxItemSet& rAttrSet )
{
    return new SwCompatibilityOptPage( pParent, rAttrSet );
}

// -----------------------------------------------------------------------

BOOL SwCompatibilityOptPage::FillItemSet( SfxItemSet&  )
{
    BOOL bModified = FALSE;
    if ( m_pWrtShell )
    {
        ULONG nSavedOptions = m_nSavedOptions;
        ULONG nCount = m_aOptionsLB.GetEntryCount();
        DBG_ASSERT( nCount <= 32, "SwCompatibilityOptPage::Reset(): entry overflow" );

        bool bSetParaSpaceMax = false;

        for ( USHORT i = 0; i < nCount; ++i )
        {
            CompatibilityOptions nOption = static_cast< CompatibilityOptions >(i);
            BOOL bChecked = m_aOptionsLB.IsChecked(i);
            BOOL bSavedChecked = ( ( nSavedOptions & 0x00000001 ) == 0x00000001 );
            if ( bChecked != bSavedChecked )
            {
                if ( COPT_USE_PRINTERDEVICE == nOption )
                {
                    m_pWrtShell->SetUseVirDev( !bChecked );
                    bModified = TRUE;
                }
                else if ( ( COPT_ADD_SPACING == nOption || COPT_ADD_SPACING_AT_PAGES == nOption ) && !bSetParaSpaceMax )
                    bSetParaSpaceMax = true;
                else if ( COPT_USE_OUR_TABSTOPS == nOption )
                {
                    m_pWrtShell->SetTabCompat( !bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_NO_EXTLEADING == nOption )
                {
                    m_pWrtShell->SetAddExtLeading( !bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_USE_LINESPACING == nOption )
                {
                       m_pWrtShell->SetUseFormerLineSpacing( bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_ADD_TABLESPACING == nOption )
                {
                    m_pWrtShell->SetAddParaSpacingToTableCells( bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_ADD_TABLESPACING == nOption )
                {
                    m_pWrtShell->SetAddParaSpacingToTableCells( bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_USE_OBJECTPOSITIONING == nOption )
                {
                    m_pWrtShell->SetUseFormerObjectPositioning( bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_USE_OUR_TEXTWRAPPING == nOption )
                {
                    m_pWrtShell->SetUseFormerTextWrapping( bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_CONSIDER_WRAPPINGSTYLE == nOption )
                {
                    m_pWrtShell->SetConsiderWrapOnObjPos( bChecked );
                    bModified = TRUE;
                }
                else if ( COPT_EXPAND_WORDSPACE == nOption )
                {
                    m_pWrtShell->SetDoNotJustifyLinesWithManualBreak( !bChecked );
                    bModified = TRUE;
                }
            }

            nSavedOptions = nSavedOptions >> 1;
        }

        if ( bSetParaSpaceMax )
        {
            m_pWrtShell->SetParaSpaceMax( m_aOptionsLB.IsChecked( (USHORT)COPT_ADD_SPACING ) );
            m_pWrtShell->SetParaSpaceMaxAtPages( m_aOptionsLB.IsChecked( (USHORT)COPT_ADD_SPACING_AT_PAGES ) );
            bModified = TRUE;
        }
    }

    if ( bModified )
        WriteOptions();

    return bModified;
}

// -----------------------------------------------------------------------

void SwCompatibilityOptPage::Reset( const SfxItemSet&  )
{
    m_aOptionsLB.SelectEntryPos( 0 );

    ULONG nOptions = GetDocumentOptions();
    SetCurrentOptions( nOptions );
    m_nSavedOptions = nOptions;
}

