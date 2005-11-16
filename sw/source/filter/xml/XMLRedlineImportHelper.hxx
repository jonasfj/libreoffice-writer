/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XMLRedlineImportHelper.hxx,v $
 *
 *  $Revision: 1.8 $
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

#ifndef _XMLREDLINEIMPORTHELPER_HXX
#define _XMLREDLINEIMPORTHELPER_HXX

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif

#ifndef _REDLINE_HXX
#include "redline.hxx"
#endif

#ifndef _REDLENUM_HXX
#include "redlenum.hxx"
#endif

#include <map>

class RedlineInfo;
class SwRedlineData;
class SwDoc;
namespace com { namespace sun { namespace star {
    namespace text { class XTextCursor; }
    namespace text { class XTextRange; }
    namespace frame { class XModel; }
} } }


typedef ::std::map< ::rtl::OUString, RedlineInfo* > RedlineMapType;

class XMLRedlineImportHelper
{
    const ::rtl::OUString sEmpty;
    const ::rtl::OUString sInsertion;
    const ::rtl::OUString sDeletion;
    const ::rtl::OUString sFormatChange;
    const ::rtl::OUString sShowChanges;
    const ::rtl::OUString sRecordChanges;
    const ::rtl::OUString sRedlineProtectionKey;

    RedlineMapType aRedlineMap;

    /// if sal_True, no redlines should be inserted into document
    /// (This typically happen when a document is loaded in 'insert'-mode.)
    sal_Bool bIgnoreRedlines;

    /// save information for saving and reconstruction of the redline mode
    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet> xModelPropertySet;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet> xImportInfoPropertySet;
    sal_Bool bShowChanges;
    sal_Bool bRecordChanges;
    ::com::sun::star::uno::Sequence<sal_Int8> aProtectionKey;

public:

    XMLRedlineImportHelper(
        sal_Bool bIgnoreRedlines,		/// ignore redlines mode

        // property sets of model + import info for saving + restoring the 
        // redline mode
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySet> & rModel,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySet> & rImportInfoSet ); 
    virtual ~XMLRedlineImportHelper();

    /// create a redline object
    /// (The redline will be inserted into the document after both start
    ///  and end cursor has been set.)
    void Add( 
        const ::rtl::OUString& rType,		/// redline type (insert, del,... )
        const ::rtl::OUString& rId,			/// use to identify this redline
        const ::rtl::OUString& rAuthor,		/// name of the author
        const ::rtl::OUString& rComment,	/// redline comment
        const ::com::sun::star::util::DateTime& rDateTime, /// date+time
        sal_Bool bMergeLastParagraph);      /// merge last paragraph?

    /// create a text section for the redline, and return an
    /// XText/XTextCursor that may be used to write into it.
    ::com::sun::star::uno::Reference< 
        ::com::sun::star::text::XTextCursor> CreateRedlineTextSection(
            ::com::sun::star::uno::Reference< 	/// needed to get the document
                    ::com::sun::star::text::XTextCursor> xOldCursor,
            const ::rtl::OUString& rId);	/// ID used to RedlineAdd() call

    /// Set start or end position for a redline in the text body.
    /// Accepts XTextRange objects.
    void SetCursor(
        const ::rtl::OUString& rId,		/// ID used in RedlineAdd() call
        sal_Bool bStart,				/// start or end Range
        ::com::sun::star::uno::Reference< 	/// the actual XTextRange
            ::com::sun::star::text::XTextRange> & rRange,
        /// text range is (from an XML view) outside of a paragraph
        /// (i.e. before a table)
        sal_Bool bIsOusideOfParagraph);	

    /**
     * Adjust the start (end) position for a redline that begins in a
     * start node. It takes the cursor positions _inside_ the redlined 
     * element (e.g. section or table).
     *
     * We will do sanity checking of the given text range: It will
     * only be considered valid if it points to the next text node
     * after the position given in a previous SetCursor */
    void AdjustStartNodeCursor(
         const ::rtl::OUString& rId,		/// ID used in RedlineAdd() call
        sal_Bool bStart,
        /// XTextRange _inside_ a table/section
        ::com::sun::star::uno::Reference<
            ::com::sun::star::text::XTextRange> & rRange);

    /// set redline mode: show changes
    void SetShowChanges( sal_Bool bShowChanges );

    /// set redline mode: record changes
    void SetRecordChanges( sal_Bool bRecordChanges );

    /// set redline protection key
    void SetProtectionKey( 
        const ::com::sun::star::uno::Sequence<sal_Int8> & rKey );

private:

    inline sal_Bool IsReady(RedlineInfo* pRedline);

    void InsertIntoDocument(RedlineInfo* pRedline);

    SwRedlineData* ConvertRedline(
        RedlineInfo* pRedline,	/// RedlineInfo to be converted
        SwDoc* pDoc);			/// document needed for Author-ID conversion

    /** save the redline mode (if rPropertySet is non-null) */
    void SaveRedlineMode(
        const ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet> & rPropertySet);

    /** don't restore the saved redline mode */
     void DontRestoreRedlineMode();

};

#endif

