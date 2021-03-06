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



#include <hintids.hxx>
#include <xmloff/xmlnmspe.hxx>

#ifndef _XMLOFF_XMLMETAI_HXX
#include <xmloff/xmlscripti.hxx>
#endif
#include "xmlimp.hxx"
#include "xmlexp.hxx"

using ::rtl::OUString;
using namespace ::com::sun::star;

// ---------------------------------------------------------------------

SvXMLImportContext *SwXMLImport::CreateScriptContext(
                                       const OUString& rLocalName )
{
    SvXMLImportContext *pContext = 0;

    if( !(IsStylesOnlyMode() || IsInsertMode()) )
    {
        pContext = new XMLScriptContext( *this,
                                    XML_NAMESPACE_OFFICE, rLocalName,
                                    GetModel() );
    }

    if( !pContext )
        pContext = new SvXMLImportContext( *this, XML_NAMESPACE_OFFICE,
                                            rLocalName );

    return pContext;
}

