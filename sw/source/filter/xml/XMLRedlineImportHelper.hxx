/*************************************************************************
 *
 *  $RCSfile: XMLRedlineImportHelper.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: dvo $ $Date: 2001-01-10 21:01:48 $
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
 *  Contributor(s): _______________________________________
 *
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
} } }


typedef ::std::map< ::rtl::OUString, RedlineInfo* > RedlineMapType;

class XMLRedlineImportHelper
{
    const ::rtl::OUString sEmpty;
    const ::rtl::OUString sInsertion;
    const ::rtl::OUString sDeletion;
    const ::rtl::OUString sFormatChange;

    RedlineMapType aRedlineMap;

public:

    XMLRedlineImportHelper();
    virtual ~XMLRedlineImportHelper();

    /// create a redline object
    /// (The redline will be inserted into the document after both start
    ///  and end cursor has been set.)
    void Add( 
        const ::rtl::OUString& rType,		/// redline type (insert, del,... )
        const ::rtl::OUString& rId,			/// use to identify this redline
        const ::rtl::OUString& rAuthor,		/// name of the author
        const ::rtl::OUString& rComment,	/// redline comment
        const ::com::sun::star::util::DateTime& rDateTime);		/// date+time

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
        const ::rtl::OUString& rId,		/// ID used to RedlineAdd() call
        sal_Bool bStart,				/// start or end Range
        ::com::sun::star::uno::Reference< 	/// the actual XTextRange
            ::com::sun::star::text::XTextRange> & rRange);	

private:

    inline sal_Bool IsReady(RedlineInfo* pRedline);

    void InsertIntoDocument(RedlineInfo* pRedline);

    SwRedlineData* ConvertRedline(
        RedlineInfo* pRedline,	/// RedlineInfo to be converted
        SwDoc* pDoc);			/// document needed for Author-ID conversion
};

#endif

