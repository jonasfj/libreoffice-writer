/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xmliteme.cxx,v $
 * $Revision: 1.19 $
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
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include <tools/shl.hxx>
#include <xmloff/xmluconv.hxx>
#include "xmlexpit.hxx"
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmltabe.hxx>
#include "xmlbrshe.hxx"
#include <svx/tstpitem.hxx>
#include <svx/brshitem.hxx>
#include <vcl/fldunit.hxx>
#include <swmodule.hxx>
#include <doc.hxx>
#include "fmtornt.hxx"
#ifndef _UNOMID_H
#include <unomid.h>
#endif
#include "frmfmt.hxx"
#include "fmtfsize.hxx"
#include "swrect.hxx"
#include "xmlexp.hxx"

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::xmloff::token;

extern SvXMLItemMapEntry aXMLTableItemMap[];
extern SvXMLItemMapEntry aXMLTableRowItemMap[];
extern SvXMLItemMapEntry aXMLTableCellItemMap[];


class SwXMLTableItemMapper_Impl: public SvXMLExportItemMapper
{
    SwXMLBrushItemExport aBrushItemExport;

protected:

    sal_uInt32 nAbsWidth;

    void AddAttribute( sal_uInt16 nPrefix, enum XMLTokenEnum eLName,
                       const OUString& rValue,
                       const SvXMLNamespaceMap& rNamespaceMap,
                       SvXMLAttributeList& rAttrList ) const;

public:

    SwXMLTableItemMapper_Impl(
            SvXMLItemMapEntriesRef rMapEntries,
            SwXMLExport& rExp );

    virtual ~SwXMLTableItemMapper_Impl();

    virtual void handleSpecialItem( SvXMLAttributeList& rAttrList,
                                    const SvXMLItemMapEntry& rEntry,
                                    const SfxPoolItem& rItem,
                                    const SvXMLUnitConverter& rUnitConverter,
                                    const SvXMLNamespaceMap& rNamespaceMap,
                                    const SfxItemSet *pSet = NULL ) const;

    virtual void handleElementItem(
            SvXMLExport& rExport,
            const SvXMLItemMapEntry& rEntry,
            const SfxPoolItem& rItem,
            const SvXMLUnitConverter& rUnitConverter,
            const SfxItemSet& rSet,
            sal_uInt16 nFlags ) const;

    inline void SetAbsWidth( sal_uInt32 nAbs );
};

SwXMLTableItemMapper_Impl::SwXMLTableItemMapper_Impl(
        SvXMLItemMapEntriesRef rMapEntries,
        SwXMLExport& rExp ) :
    SvXMLExportItemMapper( rMapEntries ),
    aBrushItemExport( rExp ),
    nAbsWidth( USHRT_MAX )
{
}

SwXMLTableItemMapper_Impl::~SwXMLTableItemMapper_Impl()
{
}

void SwXMLTableItemMapper_Impl::AddAttribute( sal_uInt16 nPrefix,
        enum XMLTokenEnum eLName,
        const OUString& rValue,
        const SvXMLNamespaceMap& rNamespaceMap,
        SvXMLAttributeList& rAttrList ) const
{
    OUString sName( rNamespaceMap.GetQNameByKey( nPrefix,
                                                 GetXMLToken(eLName) ) );
    rAttrList.AddAttribute( sName, rValue );
}

void SwXMLTableItemMapper_Impl::handleSpecialItem(
        SvXMLAttributeList& rAttrList,
        const SvXMLItemMapEntry& rEntry,
        const SfxPoolItem& rItem,
        const SvXMLUnitConverter& rUnitConverter,
        const SvXMLNamespaceMap& rNamespaceMap,
        const SfxItemSet *pSet ) const
{
    switch( rEntry.nWhichId )
    {
    case RES_LR_SPACE:
        {
            const SfxPoolItem *pItem;
            if( pSet &&
                SFX_ITEM_SET == pSet->GetItemState( RES_HORI_ORIENT, sal_True,
                                                    &pItem ) )
            {
                sal_Int16 eHoriOrient =
                    ((const SwFmtHoriOrient *)pItem)->GetHoriOrient();
                sal_Bool bExport = sal_False;
                sal_uInt16 nMemberId =
                    static_cast<sal_uInt16>( rEntry.nMemberId & MID_SW_FLAG_MASK );
                switch( nMemberId )
                {
                case MID_L_MARGIN:
                    bExport = text::HoriOrientation::NONE == eHoriOrient ||
                              text::HoriOrientation::LEFT_AND_WIDTH == eHoriOrient;
                    break;
                case MID_R_MARGIN:
                    bExport = text::HoriOrientation::NONE == eHoriOrient;
                    break;
                }
                OUString sValue;
                if( bExport && SvXMLExportItemMapper::QueryXMLValue(
                    rItem, sValue, nMemberId, rUnitConverter ) )
                {
                    AddAttribute( rEntry.nNameSpace, rEntry.eLocalName, sValue,
                                  rNamespaceMap, rAttrList );
                }
            }
        }
        break;

    case RES_FRM_SIZE:
        {
            sal_uInt16 nMemberId =
                static_cast<sal_uInt16>( rEntry.nMemberId & MID_SW_FLAG_MASK );
            switch( nMemberId )
            {
            case MID_FRMSIZE_WIDTH:
                if( nAbsWidth )
                {
                    OUStringBuffer sBuffer;
                    rUnitConverter.convertMeasure( sBuffer, nAbsWidth );
                    AddAttribute( rEntry.nNameSpace, rEntry.eLocalName,
                                  sBuffer.makeStringAndClear(),
                                  rNamespaceMap, rAttrList );
                }
                break;
            case MID_FRMSIZE_REL_WIDTH:
                {
                    OUString sValue;
                    if( SvXMLExportItemMapper::QueryXMLValue(
                        rItem, sValue, nMemberId, rUnitConverter ) )
                    {
                        AddAttribute( rEntry.nNameSpace, rEntry.eLocalName,
                                      sValue, rNamespaceMap, rAttrList );
                    }
                }
                break;
            }
        }
        break;
    }
}

/** this method is called for every item that has the
    MID_SW_FLAG_ELEMENT_EXPORT flag set */
void SwXMLTableItemMapper_Impl::handleElementItem(
        SvXMLExport& /*rExport*/,
        const SvXMLItemMapEntry& rEntry,
        const SfxPoolItem& rItem,
        const SvXMLUnitConverter& /*rUnitConverter*/,
        const SfxItemSet&,
        sal_uInt16 ) const
{
    switch( rEntry.nWhichId )
    {
    case RES_BACKGROUND:
        {
            ((SwXMLTableItemMapper_Impl *)this)->aBrushItemExport.exportXML(
                                                (const SvxBrushItem&)rItem );
        }
        break;
    }
}

inline void SwXMLTableItemMapper_Impl::SetAbsWidth( sal_uInt32 nAbs )
{
    nAbsWidth = nAbs;
}

// ----------------------------------------------------------------------------

void SwXMLExport::_InitItemExport()
{
    // #110680#
    pTwipUnitConv = new SvXMLUnitConverter( MAP_TWIP,
        GetMM100UnitConverter().getXMLMeasureUnit(), getServiceFactory() );

    xTableItemMap = new SvXMLItemMapEntries( aXMLTableItemMap );
    xTableRowItemMap = new SvXMLItemMapEntries( aXMLTableRowItemMap );
    xTableCellItemMap = new SvXMLItemMapEntries( aXMLTableCellItemMap );

    pTableItemMapper = new SwXMLTableItemMapper_Impl( xTableItemMap, *this );
}

void SwXMLExport::_FinitItemExport()
{
    delete pTableItemMapper;
    delete pTwipUnitConv;
}

void SwXMLExport::ExportTableFmt( const SwFrmFmt& rFmt, sal_uInt32 nAbsWidth )
{
    ((SwXMLTableItemMapper_Impl *)pTableItemMapper)
        ->SetAbsWidth( nAbsWidth );
    ExportFmt( rFmt, XML_TABLE );
}
