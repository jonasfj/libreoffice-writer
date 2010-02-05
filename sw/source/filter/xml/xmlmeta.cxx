/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xmlmeta.cxx,v $
 * $Revision: 1.22 $
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
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/xmlmetai.hxx>
#include <xmloff/xmlmetae.hxx>
#include <editeng/langitem.hxx>
#include <xmloff/xmluconv.hxx>
#include <xmloff/nmspmap.hxx>
#include "docstat.hxx"
#include "docsh.hxx"
#include <doc.hxx>
#include "xmlimp.hxx"
#include "xmlexp.hxx"


using ::rtl::OUString;
using ::rtl::OUStringBuffer;

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
        uno::Reference<xml::sax::XDocumentHandler> xDocBuilder(
            mxServiceFactory->createInstance(::rtl::OUString::createFromAscii(
                "com.sun.star.xml.dom.SAXDocumentBuilder")),
                uno::UNO_QUERY_THROW);
        uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
            GetModel(), UNO_QUERY_THROW);
        pContext = new SvXMLMetaDocumentContext(*this,
                        XML_NAMESPACE_OFFICE, rLocalName,
                        xDPS->getDocumentProperties(), xDocBuilder);
    }

    if( !pContext )
        pContext = new SvXMLImportContext( *this,
                        XML_NAMESPACE_OFFICE, rLocalName );

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

/*
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
*/

struct statistic {
    SvXMLTokenMapAttrs token;
    const char* name;
    USHORT SwDocStat::* target16;
    ULONG  SwDocStat::* target32; /* or 64, on LP64 platforms */
};

static const struct statistic s_stats [] = {
    { XML_TOK_META_STAT_TABLE, "TableCount",     &SwDocStat::nTbl, 0  },
    { XML_TOK_META_STAT_IMAGE, "ImageCount",     &SwDocStat::nGrf, 0  },
    { XML_TOK_META_STAT_OLE,   "ObjectCount",    &SwDocStat::nOLE, 0  },
    { XML_TOK_META_STAT_PAGE,  "PageCount",      0, &SwDocStat::nPage },
    { XML_TOK_META_STAT_PARA,  "ParagraphCount", 0, &SwDocStat::nPara },
    { XML_TOK_META_STAT_WORD,  "WordCount",      0, &SwDocStat::nWord },
    { XML_TOK_META_STAT_CHAR,  "CharacterCount", 0, &SwDocStat::nChar },
    { XML_TOK_META_STAT_END,   0,                0, 0                 }
};

void SwXMLImport::SetStatistics(
        const Sequence< beans::NamedValue > & i_rStats)
{
    if( IsStylesOnlyMode() || IsInsertMode() )
        return;

    SvXMLImport::SetStatistics(i_rStats);

    SwDoc *pDoc = SwImport::GetDocFromXMLImport( *this );
    SwDocStat aDocStat( pDoc->GetDocStat() );

    sal_uInt32 nTokens = 0;

    for (sal_Int32 i = 0; i < i_rStats.getLength(); ++i) {
        for (struct statistic const* pStat = s_stats; pStat->name != 0;
                ++pStat) {
            if (i_rStats[i].Name.equalsAscii(pStat->name)) {
                sal_Int32 val = 0;
                if (i_rStats[i].Value >>= val) {
                    if (pStat->target16 != 0) {
                        aDocStat.*(pStat->target16)
                            = static_cast<sal_uInt16> (val);
                    } else {
                        aDocStat.*(pStat->target32)
                            = static_cast<sal_uInt32> (val);
                    }
                    nTokens |= pStat->token;
                } else {
                    DBG_ERROR("SwXMLImport::SetStatistics: invalid entry");
                }
            }
        }
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

        if( IsShowProgress() )
        {
            ProgressBarHelper *pProgress = GetProgressBarHelper();
            pProgress->SetValue( pProgress->GetValue() + 2 );
        }
    }
}

