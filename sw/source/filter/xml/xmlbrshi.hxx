/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlbrshi.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2005-11-16 13:54:08 $
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

#ifndef _XMLBRSHI_HXX
#define _XMLBRSHI_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif

class SvXMLImport;
class SvXMLUnitConverter;
class SvxBrushItem;

namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
    namespace io { class XOutputStream; }
} } }

class SwXMLBrushItemImportContext : public SvXMLImportContext
{
private:
    ::com::sun::star::uno::Reference < ::com::sun::star::io::XOutputStream > xBase64Stream;
    SvxBrushItem				*pItem;
    
    void ProcessAttrs(
            const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
               const SvXMLUnitConverter& rUnitConv );

public:
    TYPEINFO();

    SwXMLBrushItemImportContext(
            SvXMLImport& rImport,
            sal_uInt16 nPrfx,
            const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
            const SvXMLUnitConverter& rUnitConv,
               const SvxBrushItem& rItem	);

    SwXMLBrushItemImportContext(
            SvXMLImport& rImport,
            sal_uInt16 nPrfx,
            const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
            const SvXMLUnitConverter& rUnitConv,
            sal_uInt16 nWhich	);

    virtual ~SwXMLBrushItemImportContext();

    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                const ::rtl::OUString& rLocalName,
                 const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

    virtual void EndElement();

    const SvxBrushItem& GetItem() const { return *pItem; }
};


#endif	//  _XMLBRSHI_HXX

