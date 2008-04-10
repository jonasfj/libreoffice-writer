/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: XMLRangeHelper.hxx,v $
 * $Revision: 1.3 $
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

//!!
//!! This file is an exact copy of the same file in chart2 project
//!!

#ifndef XMLRANGEHELPER_HXX
#define XMLRANGEHELPER_HXX

#include <sal/types.h>
#include <rtl/ustring.hxx>

//namespace chart
//{
namespace XMLRangeHelper
{

struct Cell
{
    sal_Int32 nColumn;
    sal_Int32 nRow;
    bool bRelativeColumn;
    bool bRelativeRow;
    bool bIsEmpty;

    Cell() :
            nColumn(0),
            nRow(0),
            bRelativeColumn(false),
            bRelativeRow(false),
            bIsEmpty(true)
    {}

    inline bool empty() const { return bIsEmpty; }
};

struct CellRange
{
    Cell aUpperLeft;
    Cell aLowerRight;
    ::rtl::OUString aTableName;
};

CellRange getCellRangeFromXMLString( const ::rtl::OUString & rXMLString );

::rtl::OUString getXMLStringFromCellRange( const CellRange & rRange );


} //  namespace XMLRangeHelper
//} //  namespace chart

// XMLRANGEHELPER_HXX
#endif
