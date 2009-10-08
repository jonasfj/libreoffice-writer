/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: flddinf.cxx,v $
 * $Revision: 1.16 $
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

#include <sfx2/request.hxx>
#include <sfx2/frame.hxx>
#include <vcl/svapp.hxx>
#include <svtools/zforlist.hxx>

#include <helpid.h>
#include <swtypes.hxx>
#include <globals.hrc>
#include <fldbas.hxx>
#include <docufld.hxx>
#include <wrtsh.hxx>

#include <fldui.hrc>
#include <fldtdlg.hrc>
#include <flddinf.hxx>
#include <swmodule.hxx>
#include <view.hxx>
#include <svtools/zformat.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/Time.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/Date.hpp>

#define USER_DATA_VERSION_1 "1"
#define USER_DATA_VERSION USER_DATA_VERSION_1

using namespace nsSwDocInfoSubType;
using namespace com::sun::star;
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwFldDokInfPage::SwFldDokInfPage(Window* pWindow, const SfxItemSet& rCoreSet ) :
    SwFldPage( pWindow, SW_RES( TP_FLD_DOKINF ), rCoreSet ),

    aTypeFT		(this, SW_RES(FT_DOKINFTYPE)),
    aTypeTLB	(this, SW_RES(TLB_DOKINFTYPE)),
    aSelectionFT(this, SW_RES(FT_DOKINFSELECTION)),
    aSelectionLB(this, SW_RES(LB_DOKINFSELECTION)),
    aFormatFT	(this, SW_RES(FT_DOKINFFORMAT)),
    aFormatLB	(this, SW_RES(LB_DOKINFFORMAT)),
    aFixedCB	(this, SW_RES(CB_DOKINFFIXEDCONTENT)),

    pSelEntry   (0),
    aInfoStr    (SW_RES(STR_DOKINF_INFO))
{
    FreeResource();

    aTypeTLB.SetHelpId(HID_FIELD_DINF_TYPE);
    aTypeTLB.SetSelectionMode(SINGLE_SELECTION);
    aTypeTLB.SetWindowBits(WB_HASLINES|WB_CLIPCHILDREN|WB_SORT|WB_HASBUTTONS|WB_HASBUTTONSATROOT|WB_HSCROLL);
    // Font nicht setzen, damit der Font des Controls uebernommen wird!
    // Sonst bei falschem Font Bug an OV.
    aTypeTLB.SetSpaceBetweenEntries(0);

    aTypeTLB.SetNodeDefaultImages();
    //enable 'active' language selection
    aFormatLB.SetShowLanguageControl(TRUE);

    SFX_ITEMSET_ARG( &rCoreSet, pItem, SfxUnoAnyItem, SID_DOCINFO, FALSE );
    if ( pItem )
        pItem->GetValue() >>= xCustomPropertySet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

__EXPORT SwFldDokInfPage::~SwFldDokInfPage()
{
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void __EXPORT SwFldDokInfPage::Reset(const SfxItemSet& )
{
    Init();	// Allgemeine initialisierung

    // TypeListBox initialisieren
    aTypeTLB.SetUpdateMode(FALSE);
    aTypeTLB.Clear();
    pSelEntry = 0;

    // SubTypes in der TypeLB anzeigen
    USHORT nTypeId = TYP_DOCINFOFLD;
    SvLBoxEntry* pEntry = 0;

    SvLBoxEntry* pInfo = 0;

    USHORT nSubType = USHRT_MAX;
    if (IsFldEdit())
    {
        const SwField* pCurField = GetCurField();
        nSubType = ((SwDocInfoField*)pCurField)->GetSubType() & 0xff;
        if( nSubType == DI_CUSTOM )
        {
            m_sOldCustomFieldName = static_cast<const SwDocInfoField*>(pCurField)->GetName();
        }
        aFormatLB.SetAutomaticLanguage(pCurField->IsAutomaticLanguage());
        SwWrtShell *pSh = GetWrtShell();
        if(pSh)
        {
            const SvNumberformat* pFormat = pSh->GetNumberFormatter()->GetEntry(pCurField->GetFormat());
            if(pFormat)
                aFormatLB.SetLanguage(pFormat->GetLanguage());
        }
    }

    USHORT nSelEntryData = USHRT_MAX;
    String sUserData = GetUserData();
    if(sUserData.GetToken(0, ';').EqualsIgnoreCaseAscii(USER_DATA_VERSION_1))
    {
        String sVal = sUserData.GetToken(1, ';');
        nSelEntryData = static_cast< USHORT >(sVal.ToInt32());
    }

    SvStringsDtor aLst;
    GetFldMgr().GetSubTypes(nTypeId, aLst);
    for (USHORT i = 0; i < aLst.Count(); ++i)
    {
        if (!IsFldEdit() || nSubType == i)
        {
            if (DI_CUSTOM == i)
            {
                if(xCustomPropertySet.is() )
                {
                    uno::Reference< beans::XPropertySetInfo > xSetInfo = xCustomPropertySet->getPropertySetInfo();
                    const uno::Sequence< beans::Property > rProperties = xSetInfo->getProperties();
//                    uno::Sequence< ::rtl::OUString > aPropertyNames(rProperties.getLength());
//                    for (sal_Int32 i = 0; i < rProperties.getLength(); ++i) {
//                        aPropertyNames[i] = rProperties[i].Name;
//                    }
                    //if ( !IsFldEdit() )
                    if( rProperties.getLength() )
                    {
                        pInfo = aTypeTLB.InsertEntry( String(SW_RES( STR_CUSTOM )) );
                        pInfo->SetUserData(reinterpret_cast<void*>(USHRT_MAX));

                        for (sal_Int32 n=0; n < rProperties.getLength(); n++)
                        {
                            rtl::OUString sEntry = rProperties[n].Name;
                            pEntry = aTypeTLB.InsertEntry(sEntry, pInfo);
                            if(m_sOldCustomFieldName.equals( sEntry ))
                            {
                                pSelEntry = pEntry;
                                aTypeTLB.Expand( pInfo );
                            }
                            pEntry->SetUserData(reinterpret_cast<void*>(i));
                        }
                    }
                }
            }
            else
            {
                if (!(IsFldDlgHtmlMode() && (i == DI_EDIT || i == DI_THEMA || i == DI_PRINT)))
                {
                    pEntry = aTypeTLB.InsertEntry(*aLst[i]);
                    pEntry->SetUserData(reinterpret_cast<void*>(i));
                }
            }
            if(nSelEntryData == i)
                pSelEntry = pEntry;
        }
    }

    // alte Pos selektieren
    if (pSelEntry != 0)
    {
        aTypeTLB.Select(pSelEntry);
        nSubType = (USHORT)(ULONG)pSelEntry->GetUserData();
    }
    else if ( aTypeTLB.GetEntry(0) )
    {
        pSelEntry = aTypeTLB.GetEntry(0);
        nSubType = (USHORT)(ULONG)pSelEntry->GetUserData();
    }

    FillSelectionLB(nSubType);
    if ( pSelEntry )
        TypeHdl();

    aTypeTLB.SetUpdateMode(TRUE);
    aTypeTLB.SetSelectHdl(LINK(this, SwFldDokInfPage, TypeHdl));
    aTypeTLB.SetDoubleClickHdl(LINK(this, SwFldDokInfPage, InsertHdl));
    aSelectionLB.SetSelectHdl(LINK(this, SwFldDokInfPage, SubTypeHdl));
    aSelectionLB.SetDoubleClickHdl(LINK(this, SwFldDokInfPage, InsertHdl));
    aFormatLB.SetDoubleClickHdl(LINK(this, SwFldDokInfPage, InsertHdl));

    if (IsFldEdit())
    {
        nOldSel = aSelectionLB.GetSelectEntryPos();
        nOldFormat = GetCurField()->GetFormat();
        aFixedCB.SaveValue();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwFldDokInfPage, TypeHdl, ListBox *, EMPTYARG )
{
    // Alte ListBoxPos sichern
    SvLBoxEntry* pOldEntry = pSelEntry;

    // Aktuelle ListBoxPos
    pSelEntry = aTypeTLB.FirstSelected();

    if(!pSelEntry)
    {
        pSelEntry = aTypeTLB.GetEntry(0);
        aTypeTLB.Select(pSelEntry);
    }
    else

    if (pOldEntry != pSelEntry)
        FillSelectionLB((USHORT)(ULONG)pSelEntry->GetUserData());

    SubTypeHdl();

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwFldDokInfPage, SubTypeHdl, ListBox *, EMPTYARG )
{
    USHORT nSubType = (USHORT)(ULONG)pSelEntry->GetUserData();
    USHORT nPos = aSelectionLB.GetSelectEntryPos();
    USHORT nExtSubType;
    USHORT nNewType = 0;

    if (nSubType != DI_EDIT)
    {
        if (nPos == LISTBOX_ENTRY_NOTFOUND)
        {
            if (!aSelectionLB.GetEntryCount())
            {
                aFormatLB.Clear();
                aFormatLB.Enable(FALSE);
                aFormatFT.Enable(FALSE);
                if( nSubType == DI_CUSTOM )
                {
                    //find out which type the custom field has - for a start set to DATE format 
                    ::rtl::OUString sName = aTypeTLB.GetEntryText(pSelEntry);
                    try
                    {
                        uno::Any aVal = xCustomPropertySet->getPropertyValue( sName );
                        const uno::Type& rValueType = aVal.getValueType();
                        if( rValueType == ::getCppuType( (util::DateTime*)0 ))
                        {
                            nNewType = NUMBERFORMAT_DATETIME;
                        }
                        else if( rValueType == ::getCppuType( (util::Date*)0 ))
                        {
                            nNewType = NUMBERFORMAT_DATE;
                        }
                        else if( rValueType == ::getCppuType( (util::Time*)0 ))
                        {
                            nNewType = NUMBERFORMAT_TIME;
                        }
                    }
                    catch( const uno::Exception& )
                    {
                    }    
                }
                else
                    return 0;
            }
            nPos = 0;
        }

        nExtSubType = (USHORT)(ULONG)aSelectionLB.GetEntryData(nPos);
    }
    else
        nExtSubType = DI_SUB_TIME;

    USHORT nOldType = 0;
    BOOL bEnable = FALSE;
    BOOL bOneArea = FALSE;

    if (aFormatLB.IsEnabled())
        nOldType = aFormatLB.GetFormatType();

    switch (nExtSubType)
    {
        case DI_SUB_AUTHOR:
            break;

        case DI_SUB_DATE:
            nNewType = NUMBERFORMAT_DATE;
            bOneArea = TRUE;
            break;

        case DI_SUB_TIME:
            nNewType = NUMBERFORMAT_TIME;
            bOneArea = TRUE;
            break;
    }
    if (!nNewType)
    {
        aFormatLB.Clear();
    }
    else
    {
        if (nOldType != nNewType)
        {
            aFormatLB.SetFormatType(nNewType);
            aFormatLB.SetOneArea(bOneArea);
        }
        bEnable = TRUE;
    }

    ULONG nFormat = IsFldEdit() ? ((SwDocInfoField*)GetCurField())->GetFormat() : 0;

    USHORT nOldSubType = IsFldEdit() ? (((SwDocInfoField*)GetCurField())->GetSubType() & 0xff00) : 0;

    if (IsFldEdit())
    {
        nPos = aSelectionLB.GetSelectEntryPos();
        if (nPos != LISTBOX_ENTRY_NOTFOUND)
        {
            nSubType = (USHORT)(ULONG)aSelectionLB.GetEntryData(nPos);

            nOldSubType &= ~DI_SUB_FIXED;
            if (nOldSubType == nSubType)
            {
                if (!nFormat && (nNewType == NUMBERFORMAT_DATE || nNewType == NUMBERFORMAT_TIME))
                {
                    SwWrtShell *pSh = GetWrtShell();
                    if(pSh)
                    {

                        SvNumberFormatter* pFormatter = pSh->GetNumberFormatter();
                        LanguageType eLang = aFormatLB.GetCurLanguage();
                        if (nNewType == NUMBERFORMAT_DATE)
                            nFormat = pFormatter->GetFormatIndex( NF_DATE_SYSTEM_SHORT, eLang);
                        else if (nNewType == NUMBERFORMAT_TIME)
                            nFormat = pFormatter->GetFormatIndex( NF_TIME_HHMM, eLang);
                    }
                }
                aFormatLB.SetDefFormat(nFormat);
            }
        }
    }

    aFormatLB.Enable(bEnable);
    aFormatFT.Enable(bEnable);

    if (bEnable && aFormatLB.GetSelectEntryPos() == LISTBOX_ENTRY_NOTFOUND)
    {
        aFormatLB.SelectEntryPos(0);
    }

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

USHORT SwFldDokInfPage::FillSelectionLB(USHORT nSubType)
{
    // Format-Listbox fuellen
    USHORT nTypeId = TYP_DOCINFOFLD;

    EnableInsert(nSubType != USHRT_MAX);

    if (nSubType == USHRT_MAX)	// Info-Text
        nSubType = DI_SUBTYPE_BEGIN;

    aSelectionLB.Clear();

    USHORT nSize = 0;
    USHORT nSelPos = USHRT_MAX;
    USHORT nExtSubType = IsFldEdit() ? (((SwDocInfoField*)GetCurField())->GetSubType() & 0xff00) : 0;

    if (IsFldEdit())
    {
        aFixedCB.Check((nExtSubType & DI_SUB_FIXED) != 0);
        nExtSubType = ((nExtSubType & ~DI_SUB_FIXED) >> 8) - 1;
    }

    if (nSubType < DI_CREATE || nSubType == DI_DOCNO || nSubType == DI_EDIT|| nSubType == DI_CUSTOM )
    {
        // Format Box ist fuer Title und Time leer
    }
    else
    {
        nSize = GetFldMgr().GetFormatCount(nTypeId, FALSE, IsFldDlgHtmlMode());
        for (USHORT i = 0; i < nSize; i++)
        {
            USHORT nPos = aSelectionLB.InsertEntry(GetFldMgr().GetFormatStr(nTypeId, i));
            aSelectionLB.SetEntryData(nPos, reinterpret_cast<void*>(GetFldMgr().GetFormatId(nTypeId, i)));
            if (IsFldEdit() && i == nExtSubType)
                nSelPos = nPos;
        }
    }

    BOOL bEnable = nSize != 0;

    if (nSize)
    {
        if (!aSelectionLB.GetSelectEntryCount())
            aSelectionLB.SelectEntryPos(nSelPos == USHRT_MAX ? 0 : nSelPos);

        bEnable = TRUE;
    }

    aSelectionFT.Enable(bEnable);
    aSelectionLB.Enable(bEnable);

    return nSize;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL __EXPORT SwFldDokInfPage::FillItemSet(SfxItemSet& )
{
    if (!pSelEntry || (USHORT)(ULONG)pSelEntry->GetUserData() == USHRT_MAX)
        return FALSE;

    USHORT nTypeId = TYP_DOCINFOFLD;
    USHORT nSubType = (USHORT)(ULONG)pSelEntry->GetUserData();

    ULONG nFormat = 0;

    USHORT nPos = aSelectionLB.GetSelectEntryPos();

    ::rtl::OUString aName;
    if (DI_CUSTOM == nSubType)
        aName = aTypeTLB.GetEntryText(pSelEntry);

    if (nPos != LISTBOX_ENTRY_NOTFOUND)
        nSubType |= (USHORT)(ULONG)aSelectionLB.GetEntryData(nPos);

    if (aFixedCB.IsChecked())
        nSubType |= DI_SUB_FIXED;

    nPos = aFormatLB.GetSelectEntryPos();
    if(nPos != LISTBOX_ENTRY_NOTFOUND)
        nFormat = aFormatLB.GetFormat();

    if (!IsFldEdit() || nOldSel != aSelectionLB.GetSelectEntryPos() ||
        nOldFormat != nFormat || aFixedCB.GetState() != aFixedCB.GetSavedValue()
        || (DI_CUSTOM == nSubType && !aName.equals( m_sOldCustomFieldName )))
    {
        InsertFld(nTypeId, nSubType, aName, aEmptyStr, nFormat,
                ' ', aFormatLB.IsAutomaticLanguage());
    }

    return FALSE;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SfxTabPage* __EXPORT SwFldDokInfPage::Create( 	Window* pParent,
                        const SfxItemSet& rAttrSet )
{
    return ( new SwFldDokInfPage( pParent, rAttrSet ) );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

USHORT SwFldDokInfPage::GetGroup()
{
    return GRP_REG;
}
/* -----------------12.01.99 11:21-------------------
 *
 * --------------------------------------------------*/
void	SwFldDokInfPage::FillUserData()
{
    String sData( String::CreateFromAscii(
                            RTL_CONSTASCII_STRINGPARAM( USER_DATA_VERSION )));
    sData += ';';
    SvLBoxEntry* pEntry = aTypeTLB.FirstSelected();
    USHORT nTypeSel = pEntry ? sal::static_int_cast< USHORT >(reinterpret_cast< sal_uIntPtr >(pEntry->GetUserData())) : USHRT_MAX;
    sData += String::CreateFromInt32( nTypeSel );
    SetUserData(sData);
}



