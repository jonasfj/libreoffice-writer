/*************************************************************************
 *
 *  $RCSfile: SwXMLBlockListContext.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-11 12:19:22 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
*
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): Martin Gallwey (gallwey@sun.com)
 *
 *
 ************************************************************************/
#ifndef _SW_XMLBLOCKLISTCONTEXT_HXX
#define _SW_XMLBLOCKLISTCONTEXT_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <xmloff/xmlictxt.hxx>
#endif

class SwXMLBlockListImport;
class SwXMLTextBlockImport;

class SwXMLBlockListContext : public SvXMLImportContext
{
private:
    SwXMLBlockListImport & rLocalRef;
public:
    SwXMLBlockListContext( SwXMLBlockListImport& rImport,
                           sal_uInt16 nPrefix, 
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    ~SwXMLBlockListContext ( void );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

};
class SwXMLBlockContext : public SvXMLImportContext
{
private:
    SwXMLBlockListImport & rLocalRef;
public:
    SwXMLBlockContext(     SwXMLBlockListImport& rImport,
                           sal_uInt16 nPrefix, 
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    ~SwXMLBlockContext ( void );
};

class SwXMLTextBlockDocumentContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;
public:
    SwXMLTextBlockDocumentContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix, 
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    ~SwXMLTextBlockDocumentContext ( void );
};
class SwXMLTextBlockBodyContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;
public:
    SwXMLTextBlockBodyContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix, 
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    ~SwXMLTextBlockBodyContext ( void );
};
class SwXMLTextBlockTextContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;
public:
    SwXMLTextBlockTextContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix, 
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    ~SwXMLTextBlockTextContext ( void );
};
class SwXMLTextBlockParContext : public SvXMLImportContext
{
private:
    SwXMLTextBlockImport & rLocalRef;
public:
    SwXMLTextBlockParContext(     SwXMLTextBlockImport& rImport,
                           sal_uInt16 nPrefix, 
                           const rtl::OUString& rLocalName,
                           const ::com::sun::star::uno::Reference< 
                           ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
    virtual void Characters( const ::rtl::OUString& rChars );
    ~SwXMLTextBlockParContext ( void );
};
#endif
