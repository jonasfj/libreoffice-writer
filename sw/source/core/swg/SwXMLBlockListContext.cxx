/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SwXMLBlockListContext.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 16:31:50 $
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
#pragma hdrstop

#ifndef _SW_XMLBLOCKLISTCONTEXT_HXX
#include <SwXMLBlockListContext.hxx>
#endif

#ifndef _SW_XMLBLOCKIMPORT_HXX
#include <SwXMLBlockImport.hxx>
#endif

#ifndef _SW_XMLTEXTBLOCKS_HXX
#include <SwXMLTextBlocks.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif
#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;
using namespace ::xmloff::token;
using namespace ::rtl;

SwXMLBlockListContext::SwXMLBlockListContext(
   SwXMLBlockListImport& rImport,
   sal_uInt16 nPrefix,
   const OUString& rLocalName,
   const com::sun::star::uno::Reference<
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
    rLocalRef (rImport),
    SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for (sal_Int16 i=0; i < nAttrCount; i++)
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );
        OUString aLocalName;
        sal_uInt16 nPrefix = rImport.GetNamespaceMap().GetKeyByAttrName( rAttrName, &aLocalName);
        const OUString& rAttrValue = xAttrList->getValueByIndex( i );
        if ( XML_NAMESPACE_BLOCKLIST == nPrefix )
        {
            if ( IsXMLToken ( aLocalName, XML_LIST_NAME ) )
            {
                rImport.getBlockList().SetName(rAttrValue);
                break;
            }
        }
    }
}

SwXMLBlockListContext::~SwXMLBlockListContext ( void )
{
}

SvXMLImportContext *SwXMLBlockListContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;
    if (nPrefix == XML_NAMESPACE_BLOCKLIST &&
        IsXMLToken ( rLocalName, XML_BLOCK ) )
        pContext = new SwXMLBlockContext (rLocalRef, nPrefix, rLocalName, xAttrList);
    else
        pContext = new SvXMLImportContext( rLocalRef, nPrefix, rLocalName);
    return pContext;
}

SwXMLBlockContext::SwXMLBlockContext(
   SwXMLBlockListImport& rImport,
   sal_uInt16 nPrefix,
   const OUString& rLocalName,
   const com::sun::star::uno::Reference<
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
    rLocalRef(rImport),
    SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
    static const CharClass & rCC = GetAppCharClass();
    String aShort, aLong, aPackageName;
    BOOL bTextOnly = FALSE;

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for (sal_Int16 i=0; i < nAttrCount; i++)
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );
        OUString aLocalName;
        sal_uInt16 nPrefix = rImport.GetNamespaceMap().GetKeyByAttrName( rAttrName, &aLocalName);
        const OUString& rAttrValue = xAttrList->getValueByIndex( i );
        if (XML_NAMESPACE_BLOCKLIST == nPrefix)
        {
            if ( IsXMLToken ( aLocalName, XML_ABBREVIATED_NAME ) )
            {
                aShort = rCC.upper(rAttrValue);
            }
            else if ( IsXMLToken ( aLocalName, XML_NAME ) )
            {
                aLong = rAttrValue;
            }
            else if ( IsXMLToken ( aLocalName, XML_PACKAGE_NAME ) )
            {
                aPackageName = rAttrValue;
            }
            else if ( IsXMLToken ( aLocalName, XML_UNFORMATTED_TEXT ) )
            {
                if ( IsXMLToken ( rAttrValue, XML_TRUE ) )
                    bTextOnly = TRUE;
            }
        }
    }
    if (!aShort.Len() || !aLong.Len() || !aPackageName.Len())
        return;
    rImport.getBlockList().AddName( aShort, aLong, aPackageName, bTextOnly);
}

SwXMLBlockContext::~SwXMLBlockContext ( void )
{
}

SwXMLTextBlockDocumentContext::SwXMLTextBlockDocumentContext(
   SwXMLTextBlockImport& rImport,
   sal_uInt16 nPrefix,
   const OUString& rLocalName,
   const com::sun::star::uno::Reference<
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
    rLocalRef(rImport),
    SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
}

SvXMLImportContext *SwXMLTextBlockDocumentContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;
    if (nPrefix == XML_NAMESPACE_OFFICE &&
        IsXMLToken ( rLocalName, XML_BODY ) )
        pContext = new SwXMLTextBlockBodyContext (rLocalRef, nPrefix, rLocalName, xAttrList);
    else
        pContext = new SvXMLImportContext( rLocalRef, nPrefix, rLocalName);
    return pContext;
}
SwXMLTextBlockDocumentContext::~SwXMLTextBlockDocumentContext ( void )
{
}


SwXMLTextBlockTextContext::SwXMLTextBlockTextContext(
   SwXMLTextBlockImport& rImport,
   sal_uInt16 nPrefix,
   const OUString& rLocalName,
   const com::sun::star::uno::Reference<
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
    rLocalRef(rImport),
    SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
}

SvXMLImportContext *SwXMLTextBlockTextContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;
    if (nPrefix == XML_NAMESPACE_TEXT &&
        IsXMLToken ( rLocalName, XML_P ) )
        pContext = new SwXMLTextBlockParContext (rLocalRef, nPrefix, rLocalName, xAttrList);
    else
        pContext = new SvXMLImportContext( rLocalRef, nPrefix, rLocalName);
    return pContext;
}
SwXMLTextBlockTextContext::~SwXMLTextBlockTextContext ( void )
{
}


SwXMLTextBlockBodyContext::SwXMLTextBlockBodyContext(
   SwXMLTextBlockImport& rImport,
   sal_uInt16 nPrefix,
   const OUString& rLocalName,
   const com::sun::star::uno::Reference<
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
    rLocalRef(rImport),
    SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
}

SvXMLImportContext *SwXMLTextBlockBodyContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;
    if (nPrefix == XML_NAMESPACE_OFFICE &&
        IsXMLToken ( rLocalName, XML_TEXT ) )
        pContext = new SwXMLTextBlockTextContext (rLocalRef, nPrefix, rLocalName, xAttrList);
    else if (nPrefix == XML_NAMESPACE_TEXT &&
        IsXMLToken ( rLocalName, XML_P ) )
        pContext = new SwXMLTextBlockParContext (rLocalRef, nPrefix, rLocalName, xAttrList);
    else
        pContext = new SvXMLImportContext( rLocalRef, nPrefix, rLocalName);
    return pContext;
}
SwXMLTextBlockBodyContext::~SwXMLTextBlockBodyContext ( void )
{
}
SwXMLTextBlockParContext::SwXMLTextBlockParContext(
   SwXMLTextBlockImport& rImport,
   sal_uInt16 nPrefix,
   const OUString& rLocalName,
   const com::sun::star::uno::Reference<
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
    rLocalRef(rImport),
    SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
}

void SwXMLTextBlockParContext::Characters( const ::rtl::OUString& rChars )
{
    rLocalRef.m_rText.Append ( rChars.getStr());
}
SwXMLTextBlockParContext::~SwXMLTextBlockParContext ( void )
{
    if (rLocalRef.bTextOnly)
        rLocalRef.m_rText.AppendAscii( "\015" );
    else
    {
        if (rLocalRef.m_rText.GetChar ( rLocalRef.m_rText.Len()) != ' ' )
            rLocalRef.m_rText.AppendAscii( " " );
    }
}
