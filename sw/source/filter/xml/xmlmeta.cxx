/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlmeta.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: hr $ $Date: 2007-08-02 14:22:03 $
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


#include <hintids.hxx>

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFOSUPPLIER_HPP_
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_
#include <com/sun/star/text/XTextDocument.hpp>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif

#ifndef _XMLOFF_XMLMETAI_HXX
#include <xmloff/xmlmetai.hxx>
#endif

#ifndef _XMLOFF_XMLMETAE_HXX
#include <xmloff/xmlmetae.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif

#ifndef _DOCSTAT_HXX
#include "docstat.hxx"
#endif
#ifndef _SWDOCSH_HXX
#include "docsh.hxx"
#endif
#ifndef _DOC_HXX //autogen wg. SwDoc
#include <doc.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif

#ifndef _XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _XMLEXP_HXX
#include "xmlexp.hxx"
#endif


using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::text;
using namespace ::xmloff::token;

// ---------------------------------------------------------------------

SvXMLImportContext *SwXMLImport::CreateMetaContext(
                                       const OUString& rLocalName )
{
    SvXMLImportContext *pContext = 0;

    if( !(IsStylesOnlyMode() || IsInsertMode()) )
    {
        pContext = new SfxXMLMetaContext( *this,
                                    XML_NAMESPACE_OFFICE, rLocalName,
                                    GetModel() );
    }

    if( !pContext )
        pContext = new SvXMLImportContext( *this, XML_NAMESPACE_OFFICE,
                                              rLocalName );

    return pContext;
}

// ---------------------------------------------------------------------

enum SvXMLTokenMapAttrs
{
    XML_TOK_META_STAT_TABLE = 1,
    XML_TOK_META_STAT_IMAGE = 2,
    XML_TOK_META_STAT_OLE = 4,
    XML_TOK_META_STAT_PAGE = 8,
    XML_TOK_META_STAT_PARA = 16,
    XML_TOK_META_STAT_WORD = 32,
    XML_TOK_META_STAT_CHAR = 64,
    XML_TOK_META_STAT_END=XML_TOK_UNKNOWN
};

static __FAR_DATA SvXMLTokenMapEntry aMetaStatAttrTokenMap[] =
{
    { XML_NAMESPACE_META, XML_TABLE_COUNT,      XML_TOK_META_STAT_TABLE	},
    { XML_NAMESPACE_META, XML_IMAGE_COUNT,      XML_TOK_META_STAT_IMAGE	},
    { XML_NAMESPACE_META, XML_OBJECT_COUNT,     XML_TOK_META_STAT_OLE	},
    { XML_NAMESPACE_META, XML_PARAGRAPH_COUNT,  XML_TOK_META_STAT_PARA	},
    { XML_NAMESPACE_META, XML_PAGE_COUNT,       XML_TOK_META_STAT_PAGE	},
    { XML_NAMESPACE_META, XML_WORD_COUNT,       XML_TOK_META_STAT_WORD	},
    { XML_NAMESPACE_META, XML_CHARACTER_COUNT,  XML_TOK_META_STAT_CHAR	},
    XML_TOKEN_MAP_END
};
void SwXMLImport::SetStatisticAttributes(
        const Reference< xml::sax::XAttributeList > & xAttrList)
{
    if( IsStylesOnlyMode() || IsInsertMode() )
        return;

    SvXMLImport::SetStatisticAttributes(xAttrList);

    SwDoc *pDoc = SwImport::GetDocFromXMLImport( *this );
    SwDocStat aDocStat( pDoc->GetDocStat() );

    SvXMLTokenMap aTokenMap( aMetaStatAttrTokenMap );

    sal_uInt32 nTokens = 0;
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const OUString& rValue = xAttrList->getValueByIndex( i );
        sal_Int32 nValue;
        if( !GetMM100UnitConverter().convertNumber( nValue, rValue ) )
            continue;

        const OUString& rAttrName = xAttrList->getNameByIndex( i );
        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetNamespaceMap().GetKeyByAttrName( rAttrName, &aLocalName );

        sal_uInt32 nToken = aTokenMap.Get( nPrefix, aLocalName );
        switch( nToken )
        {
        case XML_TOK_META_STAT_TABLE:
            aDocStat.nTbl = (sal_uInt16)nValue;
            break;
        case XML_TOK_META_STAT_IMAGE:
            aDocStat.nGrf = (sal_uInt16)nValue;
            break;
        case XML_TOK_META_STAT_OLE:
            aDocStat.nOLE = (sal_uInt16)nValue;
            break;
        case XML_TOK_META_STAT_PAGE:
            aDocStat.nPage = (sal_uInt32)nValue;
            break;
        case XML_TOK_META_STAT_PARA:
            aDocStat.nPara = (sal_uInt32)nValue;
            break;
        case XML_TOK_META_STAT_WORD:
            aDocStat.nWord = (sal_uInt32)nValue;
            break;
        case XML_TOK_META_STAT_CHAR:
            aDocStat.nChar = (sal_uInt32)nValue;
            break;
        default:
            nToken = 0;
        }
        nTokens |= nToken;
    }

    if( 127 == nTokens )
        aDocStat.bModified = sal_False;
    if( nTokens )
        pDoc->SetDocStat( aDocStat );

    // set progress bar reference to #paragraphs. If not available,
    // use #pages*10, or guesstimate 250 paragraphs. Additionally
    // guesstimate PROGRESS_BAR_STEPS each for meta+settings, styles,
    // and autostyles.
    sal_Int32 nProgressReference = 250;
    if( nTokens & XML_TOK_META_STAT_PARA )
        nProgressReference = (sal_Int32)aDocStat.nPara;
    else if ( nTokens & XML_TOK_META_STAT_PAGE )
        nProgressReference = 10 * (sal_Int32)aDocStat.nPage;
    ProgressBarHelper* pProgress = GetProgressBarHelper();
    pProgress->SetReference( nProgressReference + 3*PROGRESS_BAR_STEP );
    pProgress->SetValue( 0 );
}

// ---------------------------------------------------------------------

void SwXMLExport::_ExportMeta()
{
    SvXMLExport::_ExportMeta();

    if( !IsBlockMode() )
    {
        OUStringBuffer aOut(16);

        Reference < XTextDocument > xTextDoc( GetModel(), UNO_QUERY );
        Reference < XText > xText = xTextDoc->getText();
        Reference<XUnoTunnel> xTextTunnel( xText, UNO_QUERY);
        ASSERT( xTextTunnel.is(), "missing XUnoTunnel for Cursor" );
        if( !xTextTunnel.is() )
            return;

        SwXText *pText = (SwXText *)xTextTunnel->getSomething(
                                            SwXText::getUnoTunnelId() );
        ASSERT( pText, "SwXText missing" );
        if( !pText )
            return;

        SwDocStat aDocStat( pText->GetDoc()->GetDocStat() );
        aOut.append( (sal_Int32)aDocStat.nTbl );
        AddAttribute( XML_NAMESPACE_META, XML_TABLE_COUNT,
                      aOut.makeStringAndClear() );
        aOut.append( (sal_Int32)aDocStat.nGrf );
        AddAttribute( XML_NAMESPACE_META, XML_IMAGE_COUNT,
                      aOut.makeStringAndClear() );
        aOut.append( (sal_Int32)aDocStat.nOLE );
        AddAttribute( XML_NAMESPACE_META, XML_OBJECT_COUNT,
                      aOut.makeStringAndClear() );
        if( aDocStat.nPage )
        {
            aOut.append( (sal_Int32)aDocStat.nPage );
            AddAttribute( XML_NAMESPACE_META, XML_PAGE_COUNT,
                          aOut.makeStringAndClear() );
        }
        aOut.append( (sal_Int32)aDocStat.nPara );
        AddAttribute( XML_NAMESPACE_META, XML_PARAGRAPH_COUNT,
                      aOut.makeStringAndClear() );
        aOut.append( (sal_Int32)aDocStat.nWord );
        AddAttribute( XML_NAMESPACE_META, XML_WORD_COUNT,
                      aOut.makeStringAndClear() );
        aOut.append( (sal_Int32)aDocStat.nChar );
        AddAttribute( XML_NAMESPACE_META, XML_CHARACTER_COUNT,
                      aOut.makeStringAndClear() );
        SvXMLElementExport aElem( *this, XML_NAMESPACE_META,
                                  XML_DOCUMENT_STATISTIC,
                                  sal_True, sal_True );

        if( IsShowProgress() )
        {
            ProgressBarHelper *pProgress = GetProgressBarHelper();
            pProgress->SetValue( pProgress->GetValue() + 2 );
        }
    }
}

