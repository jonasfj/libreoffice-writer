/*************************************************************************
 *
 *  $RCSfile: SwXMLBlockImport.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mtg $ $Date: 2001-05-02 16:40:30 $
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
#ifndef _SW_XMLBLOCKIMPORT_HXX
#define _SW_XMLBLOCKIMPORT_HXX

#ifndef _SVSTOR_HXX 
#include <so3/svstor.hxx>
#endif

#ifndef _XMLOFF_XMLICTXT_HXX
#include <xmloff/xmlictxt.hxx>
#endif

#ifndef _XMLOFF_XMLITMAP_HXX
#include <xmloff/xmlitmap.hxx>
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include <xmloff/xmlimp.hxx>
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif

#ifndef _XMLOFF_XMLKYWD_HXX
#include <xmloff/xmlkywd.hxx>
#endif

#ifndef _SW_XMLBLOCKLIST_CONTEXT_HXX
#include <SwXMLBlockListContext.hxx>
#endif

#ifndef _SW_XMLTEXTBLOCKS_HXX
#include <SwXMLTextBlocks.hxx>
#endif

class SwXMLBlockListImport : public SvXMLImport
{
private:
    SwXMLTextBlocks &rBlockList;
protected:

    // This method is called after the namespace map has been updated, but
    // before a context for the current element has been pushed.
    virtual SvXMLImportContext *CreateContext( sal_uInt16 nPrefix,
                  const ::rtl::OUString& rLocalName,
                  const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
public:
    SwXMLBlockListImport ( SwXMLTextBlocks &rBlocks );
    SwXMLTextBlocks& getBlockList ( void )
    {
        return rBlockList;
    }
    ~SwXMLBlockListImport ( void );
};
class SwXMLTextBlockImport : public SvXMLImport
{
private:
    SwXMLTextBlocks &rBlockList;
protected:

    // This method is called after the namespace map has been updated, but
    // before a context for the current element has been pushed.
    virtual SvXMLImportContext *CreateContext( sal_uInt16 nPrefix,
                  const ::rtl::OUString& rLocalName,
                  const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );
public:
    sal_Bool bTextOnly;
    String &m_rText;

    SwXMLTextBlockImport ( SwXMLTextBlocks &rBlocks, String &rNewText, sal_Bool bNewTextOnly );
    SwXMLTextBlocks& getBlockList ( void )
    {
        return rBlockList;
    }
    ~SwXMLTextBlockImport ( void );
    virtual void SAL_CALL endDocument(void)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );

};
#endif
