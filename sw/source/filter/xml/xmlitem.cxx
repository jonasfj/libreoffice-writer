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
#include <tools/debug.hxx>
#include <xmloff/xmlimp.hxx>
#include "xmlimpit.hxx"
#include "xmlitem.hxx"

using ::rtl::OUString;
using namespace ::com::sun::star;

SvXMLItemSetContext::SvXMLItemSetContext( SvXMLImport& rImp, USHORT nPrfx,
                                          const OUString& rLName,
                                          const uno::Reference< xml::sax::XAttributeList >& xAttrList,
                                          SfxItemSet& rISet,
                                          const SvXMLImportItemMapper& rIMap,
                                          const SvXMLUnitConverter& rUnitConverter ):
    SvXMLImportContext( rImp, nPrfx, rLName ),
    rItemSet( rISet ),
    rIMapper( rIMap ),
    rUnitConv( rUnitConverter )

{
    rIMapper.importXML( rItemSet, xAttrList, rUnitConv,
                           GetImport().GetNamespaceMap() );
}

SvXMLItemSetContext::~SvXMLItemSetContext()
{
}

SvXMLImportContext *SvXMLItemSetContext::CreateChildContext( USHORT nPrefix,
                                            const OUString& rLocalName,
                                            const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
    SvXMLItemMapEntriesRef xMapEntries = rIMapper.getMapEntries();
    SvXMLItemMapEntry* pEntry = xMapEntries->getByName( nPrefix, rLocalName );

    if( pEntry && 0 != (pEntry->nMemberId & MID_SW_FLAG_ELEMENT_ITEM_IMPORT) )
    {
        return CreateChildContext( nPrefix, rLocalName, xAttrList,
                                   rItemSet, *pEntry, rUnitConv );
    }
    else
    {
        return new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
    }
}

/** This method is called from this instance implementation of
    CreateChildContext if the element matches an entry in the
    SvXMLImportItemMapper with the mid flag MID_SW_FLAG_ELEMENT
*/
SvXMLImportContext *SvXMLItemSetContext::CreateChildContext( USHORT nPrefix,
                                   const rtl::OUString& rLocalName,
                                   const uno::Reference< xml::sax::XAttributeList >& /*xAttrList*/,
                                    SfxItemSet&  /*rItemSet*/,
                                   const SvXMLItemMapEntry& /*rEntry*/,
                                   const SvXMLUnitConverter& /*rUnitConv*/ )
{
    return new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
}


