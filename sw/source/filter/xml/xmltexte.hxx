/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xmltexte.hxx,v $
 * $Revision: 1.17 $
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

#ifndef _XMLTEXTE_HXX
#define _XMLTEXTE_HXX

#include <xmloff/txtparae.hxx>
#include <tools/globname.hxx>

class SwXMLExport;
class SvXMLAutoStylePoolP;
class SwNoTxtNode;

namespace com { namespace sun { namespace star { namespace style {
                class XStyle; } } } }

class SwXMLTextParagraphExport : public XMLTextParagraphExport 
{
    const ::rtl::OUString sTextTable;
    const ::rtl::OUString sEmbeddedObjectProtocol;
    const ::rtl::OUString sGraphicObjectProtocol;

    const SvGlobalName aAppletClassId;
    const SvGlobalName aPluginClassId;
    const SvGlobalName aIFrameClassId;
    const SvGlobalName aOutplaceClassId;

    SwNoTxtNode *GetNoTxtNode( 
        const ::com::sun::star::uno::Reference <
                ::com::sun::star::beans::XPropertySet >& rPropSet ) const;

protected:
    virtual void exportStyleContent(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::style::XStyle > & rStyle );

    virtual void _collectTextEmbeddedAutoStyles(
        const ::com::sun::star::uno::Reference <
            ::com::sun::star::beans::XPropertySet > & rPropSet );
    virtual void _exportTextEmbedded(
        const ::com::sun::star::uno::Reference <
            ::com::sun::star::beans::XPropertySet > & rPropSet,
        const ::com::sun::star::uno::Reference <
            ::com::sun::star::beans::XPropertySetInfo > & rPropSetInfo );

    virtual void exportTable(
        const ::com::sun::star::uno::Reference <
            ::com::sun::star::text::XTextContent > & rTextContent,
        sal_Bool bAutoStyles, sal_Bool bProgress );

public:
    SwXMLTextParagraphExport(
        SwXMLExport& rExp,
         SvXMLAutoStylePoolP& rAutoStylePool );
    ~SwXMLTextParagraphExport();

    virtual void setTextEmbeddedGraphicURL(
        const ::com::sun::star::uno::Reference <
                ::com::sun::star::beans::XPropertySet >& rPropSet,
        ::rtl::OUString& rStreamName ) const;
};


#endif	//  _XMLTEXTE_HXX
