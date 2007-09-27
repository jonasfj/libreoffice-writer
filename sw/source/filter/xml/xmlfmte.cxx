/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlfmte.cxx,v $
 *
 *  $Revision: 1.47 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 10:10:47 $
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


#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_
#include <com/sun/star/text/XTextDocument.hpp>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_ATTRLIST_HXX
#include <xmloff/attrlist.hxx>
#endif
#ifndef _XMLITMPR_HXX
#include "xmlexpit.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLTEXTLISTAUTOSTYLEPOOL_HXX
#include <xmloff/XMLTextListAutoStylePool.hxx>
#endif
#ifndef _XMLOFF_XMLTEXTMASTERPAGEEXPORT
#include <xmloff/XMLTextMasterPageExport.hxx>
#endif

#ifndef _XMLOFF_TXTPRMAP_HXX
#include <xmloff/txtprmap.hxx>
#endif
#ifndef _XMLOFF_XMLASTPLP_HXX
#include <xmloff/xmlaustp.hxx>
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include <xmloff/families.hxx>
#endif
#ifndef _XMLOFF_PROGRESSBARHELPER_HXX
#include <xmloff/ProgressBarHelper.hxx>
#endif

#ifndef _FORMAT_HXX //autogen wg. SwFmt
#include <format.hxx>
#endif
#ifndef _FMTPDSC_HXX
#include <fmtpdsc.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _UNOSTYLE_HXX
#include <unostyle.hxx>
#endif
#ifndef _CELLATR_HXX
#include <cellatr.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _XMLEXP_HXX
#include "xmlexp.hxx"
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif

using namespace ::rtl;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::lang;
using namespace ::xmloff::token;

void SwXMLExport::ExportFmt( const SwFmt& rFmt, enum XMLTokenEnum eFamily )
{
    // <style:style ...>
    CheckAttrList();

    // style:family="..."
    DBG_ASSERT( RES_FRMFMT==rFmt.Which(), "frame format expected" );
    if( RES_FRMFMT != rFmt.Which() )
        return;
    DBG_ASSERT( eFamily != XML_TOKEN_INVALID, "family must be specified" );
    // style:name="..."
    sal_Bool bEncoded = sal_False;
    AddAttribute( XML_NAMESPACE_STYLE, XML_NAME, EncodeStyleName(
                    rFmt.GetName(), &bEncoded ) );
    if( bEncoded )
        AddAttribute( XML_NAMESPACE_STYLE, XML_DISPLAY_NAME, rFmt.GetName() );


    if( eFamily != XML_TOKEN_INVALID )
        AddAttribute( XML_NAMESPACE_STYLE, XML_FAMILY, eFamily );

#ifndef PRODUCT
    // style:parent-style-name="..." (if its not the default only)
    const SwFmt* pParent = rFmt.DerivedFrom();
    // Parent-Namen nur uebernehmen, wenn kein Default
    ASSERT( !pParent || pParent->IsDefault(), "unexpected parent" );

    ASSERT( USHRT_MAX == rFmt.GetPoolFmtId(), "pool ids arent'supported" );
    ASSERT( USHRT_MAX == rFmt.GetPoolHelpId(), "help ids arent'supported" );
    ASSERT( USHRT_MAX == rFmt.GetPoolHelpId() ||
            UCHAR_MAX == rFmt.GetPoolHlpFileId(), "help file ids aren't supported" );
#endif

    // style:master-page-name
    if( RES_FRMFMT == rFmt.Which() && XML_TABLE == eFamily )
    {
        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET == rFmt.GetAttrSet().GetItemState( RES_PAGEDESC,
                                                            sal_False, &pItem ) )
        {
            String sName;
            const SwPageDesc *pPageDesc =
                ((const SwFmtPageDesc *)pItem)->GetPageDesc();
            if( pPageDesc )
                SwStyleNameMapper::FillProgName(
                                    pPageDesc->GetName(),
                                    sName,
                                    nsSwGetPoolIdFromName::GET_POOLID_PAGEDESC,
                                    sal_True);
            AddAttribute( XML_NAMESPACE_STYLE, XML_MASTER_PAGE_NAME,
                          EncodeStyleName( sName ) );
        }
    }

    if( XML_TABLE_CELL == eFamily )
    {
        DBG_ASSERT(RES_FRMFMT == rFmt.Which(), "only frame format");

        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET ==
            rFmt.GetAttrSet().GetItemState( RES_BOXATR_FORMAT,
                                            sal_False, &pItem ) )
        {
            sal_Int32 nFormat = (sal_Int32)
                ((const SwTblBoxNumFormat *)pItem)->GetValue();

            if ( (nFormat != -1) && (nFormat != NUMBERFORMAT_TEXT) )
            {
                // if we have a format, register and then export
                // (Careful: here we assume that data styles will be
                // written after cell styles)
                addDataStyle(nFormat);
                OUString sDataStyleName = getDataStyleName(nFormat);
                if( sDataStyleName.getLength() > 0 )
                    AddAttribute( XML_NAMESPACE_STYLE, XML_DATA_STYLE_NAME,
                                  sDataStyleName );
            }
        }
    }

    {
        SvXMLElementExport aElem( *this, XML_NAMESPACE_STYLE, XML_STYLE,
                                  sal_True, sal_True );

        SvXMLItemMapEntriesRef xItemMap;
        XMLTokenEnum ePropToken = XML_TABLE_PROPERTIES;
        if( XML_TABLE == eFamily )
        {
            xItemMap = xTableItemMap;
        }
        else if( XML_TABLE_ROW == eFamily )
        {
            xItemMap = xTableRowItemMap;
            ePropToken = XML_TABLE_ROW_PROPERTIES;
        }
        else if( XML_TABLE_CELL == eFamily )
        {
            xItemMap = xTableCellItemMap;
            ePropToken = XML_TABLE_CELL_PROPERTIES;
        }

        if( xItemMap.Is() )
        {
            SvXMLExportItemMapper& rItemMapper = GetTableItemMapper();
            rItemMapper.setMapEntries( xItemMap );

            GetTableItemMapper().exportXML( *this,
                                           rFmt.GetAttrSet(),
                                           GetTwipUnitConverter(),
                                           ePropToken,
                                           XML_EXPORT_FLAG_IGN_WS );
        }
    }
}


void SwXMLExport::_ExportStyles( sal_Bool bUsed )
{
    SvXMLExport::_ExportStyles( bUsed );

    // drawing defaults
    GetShapeExport()->ExportGraphicDefaults();

#if SUPD <628 && !defined(TEST_MIB)
    GetTextParagraphExport()->SetProgress( IsShowProgress() ? 1 : 0 );
#endif
    GetTextParagraphExport()->exportTextStyles( bUsed
#if SUPD >627 || defined(TEST_MIB)
                                             ,IsShowProgress()
#endif
                                              );
#if SUPD <628 && !defined(TEST_MIB)
    GetTextParagraphExport()->SetProgress( 0 );
#endif
}

void SwXMLExport::_ExportAutoStyles()
{
    // The order in which styles are collected *MUST* be the same as
    // the order in which they are exported. Otherwise, caching will
    // fail.

    if( (getExportFlags() & (EXPORT_MASTERSTYLES|EXPORT_CONTENT)) != 0 )
    {
        if( (getExportFlags() & EXPORT_CONTENT) == 0 )
        {
            // only master pages are exported => styles for frames bound
            // to frames (but none for frames bound to pages) need to be
            // collected.
            GetTextParagraphExport()->collectFramesBoundToFrameAutoStyles();
        }
        else
        {
            // content (and optional master pages) are exported => styles
            // for frames bound to frame or to pages need to be
            // collected.
            GetTextParagraphExport()->collectFramesBoundToPageOrFrameAutoStyles(
                                                bShowProgress );
        }

    }

    // exported in _ExportMasterStyles
    if( (getExportFlags() & EXPORT_MASTERSTYLES) != 0 )
        GetPageExport()->collectAutoStyles( sal_False );

    // if we don't export styles (i.e. in content stream only, but not
    // in single-stream case), then we can save ourselves a bit of
    // work and memory by not collecting field masters
    if( (getExportFlags() & EXPORT_STYLES ) == 0 )
        GetTextParagraphExport()->exportUsedDeclarations( sal_False );

    // exported in _ExportContent
    if( (getExportFlags() & EXPORT_CONTENT) != 0 )
    {
        GetTextParagraphExport()->exportTrackedChanges( sal_True );
        Reference < XTextDocument > xTextDoc( GetModel(), UNO_QUERY );
        Reference < XText > xText = xTextDoc->getText();

        // collect form autostyle
        // (do this before collectTextAutoStyles, 'cause the shapes need the results of the work
        // done by examineForms)
        Reference<XDrawPageSupplier> xDrawPageSupplier( GetModel(), UNO_QUERY );
        if (xDrawPageSupplier.is() && GetFormExport().is())
        {
            Reference<XDrawPage> xPage = xDrawPageSupplier->getDrawPage();
            if (xPage.is())
                GetFormExport()->examineForms(xPage);
        }

        GetTextParagraphExport()->collectTextAutoStylesOptimized( bShowProgress );
        //GetTextParagraphExport()->collectTextAutoStyles( xText, bShowProgress, sal_True, bPortions );
    }

    GetTextParagraphExport()->exportTextAutoStyles();
    GetShapeExport()->exportAutoStyles();
    if( (getExportFlags() & EXPORT_MASTERSTYLES) != 0 )
        GetPageExport()->exportAutoStyles();

    // we rely on data styles being written after cell styles in the
    // ExportFmt() method; so be careful when changing order.
    exportAutoDataStyles();

    sal_uInt16 nContentAutostyles = EXPORT_CONTENT | EXPORT_AUTOSTYLES;
    if ( ( getExportFlags() & nContentAutostyles ) == nContentAutostyles )
        GetFormExport()->exportAutoStyles();
}

XMLPageExport* SwXMLExport::CreatePageExport()
{
    return new XMLTextMasterPageExport( *this );
}

void SwXMLExport::_ExportMasterStyles()
{
    // export master styles
    GetPageExport()->exportMasterStyles( sal_False );
}

// ---------------------------------------------------------------------
class SwXMLAutoStylePoolP : public SvXMLAutoStylePoolP
{
    SvXMLExport& rExport;
    const OUString sListStyleName;
    const OUString sMasterPageName;
    const OUString sCDATA;

protected:

    virtual void exportStyleAttributes(
            SvXMLAttributeList& rAttrList,
            sal_Int32 nFamily,
            const ::std::vector< XMLPropertyState >& rProperties,
            const SvXMLExportPropertyMapper& rPropExp
            , const SvXMLUnitConverter& rUnitConverter,
            const SvXMLNamespaceMap& rNamespaceMap
            ) const;
public:

    SwXMLAutoStylePoolP( SvXMLExport& rExport );
    virtual ~SwXMLAutoStylePoolP();
};

void SwXMLAutoStylePoolP::exportStyleAttributes(
            SvXMLAttributeList& rAttrList,
            sal_Int32 nFamily,
            const ::std::vector< XMLPropertyState >& rProperties,
            const SvXMLExportPropertyMapper& rPropExp
            , const SvXMLUnitConverter& rUnitConverter,
            const SvXMLNamespaceMap& rNamespaceMap
            ) const
{
    SvXMLAutoStylePoolP::exportStyleAttributes( rAttrList, nFamily, rProperties, rPropExp, rUnitConverter, rNamespaceMap);

    if( XML_STYLE_FAMILY_TEXT_PARAGRAPH == nFamily )
    {
        for( ::std::vector< XMLPropertyState >::const_iterator
                    aProperty = rProperties.begin();
             aProperty != rProperties.end();
              aProperty++ )
        {
            if (aProperty->mnIndex != -1) // #i26762#
            {
                switch( rPropExp.getPropertySetMapper()->
                        GetEntryContextId( aProperty->mnIndex ) )
                {
                case CTF_NUMBERINGSTYLENAME:
                    {
                        OUString sStyleName;
                        aProperty->maValue >>= sStyleName;
                        if( sStyleName.getLength() )
                        {
                            OUString sTmp = rExport.GetTextParagraphExport()->GetListAutoStylePool().Find( sStyleName );
                            if( sTmp.getLength() )
                                sStyleName = sTmp;
                            GetExport().AddAttribute( XML_NAMESPACE_STYLE,
                                  sListStyleName,
                                  GetExport().EncodeStyleName( sStyleName ) );
                        }
                    }
                    break;
                case CTF_PAGEDESCNAME:
                    {
                        OUString sStyleName;
                        aProperty->maValue >>= sStyleName;
                        GetExport().AddAttribute( XML_NAMESPACE_STYLE,
                                      sMasterPageName,
                                      GetExport().EncodeStyleName( sStyleName ) );
                    }
                    break;
                }
            }
        }
    }
}

SwXMLAutoStylePoolP::SwXMLAutoStylePoolP(SvXMLExport& rExp ) :
    SvXMLAutoStylePoolP( rExp ),
    rExport( rExp ),
    sListStyleName( GetXMLToken( XML_LIST_STYLE_NAME ) ),
    sMasterPageName( GetXMLToken( XML_MASTER_PAGE_NAME ) ),
    sCDATA( GetXMLToken( XML_CDATA ) )
{
}


SwXMLAutoStylePoolP::~SwXMLAutoStylePoolP()
{
}

SvXMLAutoStylePoolP* SwXMLExport::CreateAutoStylePool()
{
    return new SwXMLAutoStylePoolP( *this );
}
