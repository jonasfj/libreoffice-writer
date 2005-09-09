/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SwXMLSectionList.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 04:43:13 $
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

#define _SVSTDARR_STRINGSDTOR
#define _SVSTDARR_STRINGS
#include <svtools/svstdarr.hxx>
#ifndef _SW_XMLSECTIONLIST_HXX
#include <SwXMLSectionList.hxx>
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;
using namespace ::rtl;
using namespace ::xmloff::token;

sal_Char __READONLY_DATA sXML_np__office[] = "_ooffice";
sal_Char __READONLY_DATA sXML_np__text[] = "_otext";

// #110680#
SwXMLSectionList::SwXMLSectionList(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
    SvStrings & rNewSectionList)
:	SvXMLImport( xServiceFactory ),
    rSectionList ( rNewSectionList )
{
    GetNamespaceMap().Add( OUString( RTL_CONSTASCII_USTRINGPARAM ( sXML_np__office ) ),
                            GetXMLToken(XML_N_OFFICE_OOO),
                            XML_NAMESPACE_OFFICE );
    GetNamespaceMap().Add( OUString( RTL_CONSTASCII_USTRINGPARAM ( sXML_np__text ) ),
                            GetXMLToken(XML_N_TEXT_OOO),
                            XML_NAMESPACE_TEXT );
}

SwXMLSectionList::~SwXMLSectionList ( void )
    throw()
{
}

SvXMLImportContext *SwXMLSectionList::CreateContext(
        sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if  (nPrefix == XML_NAMESPACE_OFFICE && IsXMLToken ( rLocalName, XML_BODY ) ||
         nPrefix == XML_NAMESPACE_TEXT &&
        (IsXMLToken ( rLocalName, XML_P ) ||
         IsXMLToken ( rLocalName, XML_H ) ||
         IsXMLToken ( rLocalName, XML_A ) ||
         IsXMLToken ( rLocalName, XML_SPAN ) ||
         IsXMLToken ( rLocalName, XML_SECTION ) ||
         IsXMLToken ( rLocalName, XML_INDEX_BODY ) ||
         IsXMLToken ( rLocalName, XML_INDEX_TITLE )||
         IsXMLToken ( rLocalName, XML_INSERTION ) ||
         IsXMLToken ( rLocalName, XML_DELETION ) ) )
        pContext = new SvXMLSectionListContext (*this, nPrefix, rLocalName, xAttrList);
    else
        pContext = SvXMLImport::CreateContext( nPrefix, rLocalName, xAttrList );
    return pContext;
}

SvXMLSectionListContext::SvXMLSectionListContext( 
   SwXMLSectionList& rImport,
   sal_uInt16 nPrefix, 
   const OUString& rLocalName,
   const com::sun::star::uno::Reference< 
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
   rLocalRef(rImport),
   SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
}

SvXMLImportContext *SvXMLSectionListContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;
    String sName;
    
    if (nPrefix == XML_NAMESPACE_TEXT && ( IsXMLToken ( rLocalName, XML_SECTION ) ||
                                           IsXMLToken ( rLocalName, XML_BOOKMARK) ) )
    {
        sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;

        for (sal_Int16 i=0; i < nAttrCount; i++)
        {
            const OUString& rAttrName = xAttrList->getNameByIndex( i );
            OUString aLocalName;
            sal_uInt16 nPrefix = rLocalRef.GetNamespaceMap().GetKeyByAttrName( rAttrName, &aLocalName);
            const OUString& rAttrValue = xAttrList->getValueByIndex( i );
            if (XML_NAMESPACE_TEXT == nPrefix && IsXMLToken ( aLocalName, XML_NAME ) )
                sName = rAttrValue;
        }
        if ( sName.Len() )
            rLocalRef.rSectionList.Insert ( new String(sName), rLocalRef.rSectionList.Count() );
    }
    
    pContext = new SvXMLSectionListContext (rLocalRef, nPrefix, rLocalName, xAttrList);
    return pContext;
}
SvXMLSectionListContext::~SvXMLSectionListContext ( void )
{
}

SvXMLIgnoreSectionListContext::SvXMLIgnoreSectionListContext( 
   SwXMLSectionList& rImport,
   sal_uInt16 nPrefix, 
   const OUString& rLocalName,
   const com::sun::star::uno::Reference< 
   com::sun::star::xml::sax::XAttributeList > & xAttrList ) :
   rLocalRef(rImport),
   SvXMLImportContext ( rImport, nPrefix, rLocalName )
{
}

SvXMLIgnoreSectionListContext::~SvXMLIgnoreSectionListContext ( void )
{
}
SvXMLImportContext *SvXMLIgnoreSectionListContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference< xml::sax::XAttributeList > & xAttrList )
{
    return  new SvXMLIgnoreSectionListContext (rLocalRef, nPrefix, rLocalName, xAttrList);
}
