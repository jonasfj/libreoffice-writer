/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmltexti.hxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 17:25:12 $
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
#ifndef _XMLTEXTI_HXX
#define _XMLTEXTI_HXX

#ifndef _XMLOFF_TXTIMP_HXX
#include <xmloff/txtimp.hxx>
#endif
#ifndef _XMLOFF_FUNCTIONAL_HXX
#include <xmloff/functional.hxx>
#endif

class XMLRedlineImportHelper;
class SvXMLImport;

class SwXMLTextImportHelper : public XMLTextImportHelper
{
    XMLRedlineImportHelper *pRedlineHelper;

protected:
    virtual SvXMLImportContext *CreateTableChildContext(
                SvXMLImport& rImport,
                sal_uInt16 nPrefix, const ::rtl::OUString& rLocalName,
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

public:
    SwXMLTextImportHelper(
            const ::com::sun::star::uno::Reference <
                ::com::sun::star::frame::XModel>& rModel,
            SvXMLImport& rImport,
            const ::com::sun::star::uno::Reference <
                ::com::sun::star::beans::XPropertySet>& rInfoSet,
            sal_Bool bInsertM, sal_Bool bStylesOnlyM, sal_Bool bProgress,
            sal_Bool bBlockM, sal_Bool bOrganizerM,
            sal_Bool bPreserveRedlineMode );
    ~SwXMLTextImportHelper();

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet>
            createAndInsertOLEObject( SvXMLImport& rImport,
                                      const ::rtl::OUString& rHRef,
                                      const ::rtl::OUString& rStyleName,
                                      const ::rtl::OUString& rTblName,
                                         sal_Int32 nWidth, sal_Int32 nHeight );
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet>
            createAndInsertOOoLink( SvXMLImport& rImport,
                                      const ::rtl::OUString& rHRef,
                                      const ::rtl::OUString& rStyleName,
                                      const ::rtl::OUString& rTblName,
                                         sal_Int32 nWidth, sal_Int32 nHeight );
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet>
        createAndInsertApplet(
            const ::rtl::OUString &rName,
            const ::rtl::OUString &rCode,
            sal_Bool bMayScript,
            const ::rtl::OUString& rHRef,
            sal_Int32 nWidth, sal_Int32 nHeight );

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet>
        createAndInsertPlugin(
            const ::rtl::OUString &rMimeType,
            const ::rtl::OUString& rHRef,
            sal_Int32 nWidth, sal_Int32 nHeight );

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet>
        createAndInsertFloatingFrame(
            const ::rtl::OUString &rName,
            const ::rtl::OUString &rHRef,
            const ::rtl::OUString &rStyleName,
            sal_Int32 nWidth, sal_Int32 nHeight );

    virtual void endAppletOrPlugin(
        const ::com::sun::star::uno::Reference < ::com::sun::star::beans::XPropertySet > &rPropSet,
        ::std::map < const ::rtl::OUString, ::rtl::OUString, ::comphelper::UStringLess> &rParamMap);

    virtual sal_Bool IsInHeaderFooter() const;

    // redlining helper methods
    // (here is the real implementation)
    virtual void RedlineAdd(
        const ::rtl::OUString& rType,		/// redline type (insert, del,... )
        const ::rtl::OUString& rId,			/// use to identify this redline
        const ::rtl::OUString& rAuthor,		/// name of the author
        const ::rtl::OUString& rComment,	/// redline comment
        const ::com::sun::star::util::DateTime& rDateTime,	/// date+time
        sal_Bool bMergeLastPara);           /// merge last paragraph
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::text::XTextCursor> RedlineCreateText(
            ::com::sun::star::uno::Reference< 	/// needed to get the document
                    ::com::sun::star::text::XTextCursor> & rOldCursor,
            const ::rtl::OUString& rId);	/// ID used to RedlineAdd() call
    virtual void RedlineSetCursor(
        const ::rtl::OUString& rId,			/// ID used to RedlineAdd() call
        sal_Bool bStart,					/// start or end Cursor
        sal_Bool bIsOutsideOfParagraph);
    virtual void RedlineAdjustStartNodeCursor(
        sal_Bool bStart);
    virtual void SetShowChanges( sal_Bool bShowChanges );
    virtual void SetRecordChanges( sal_Bool bRecordChanges );
    virtual void SetChangesProtectionKey(
        const ::com::sun::star::uno::Sequence<sal_Int8> & rKey );
};

#endif	//  _XMLTEXTI_HXX

