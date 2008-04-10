/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: IDocumentChartDataProviderAccess.hxx,v $
 * $Revision: 1.4 $
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

 #ifndef IDOCUMENTCHARTDATAPROVIDER_HXX_INCLUDED
 #define IDOCUMENTCHARTDATAPROVIDER_HXX_INCLUDED

#include "com/sun/star/uno/Reference.hxx"

namespace com { namespace sun { namespace star { namespace frame {
    class XModel;
}}}}

class SwTable;
class SwChartDataProvider;
class SwChartLockController_Helper;


 /** Give access to the data-provider for chart
 */
 class IDocumentChartDataProviderAccess
 {
 public:

    /**
        returns or creates the data-provider for chart
    */
    virtual SwChartDataProvider * GetChartDataProvider( bool bCreate = false ) const = 0;

    /**
        calls createInternalDataProvider for all charts using the specified table
    */
    virtual void CreateChartInternalDataProviders( const SwTable *pTable ) = 0;

    /**
        returns chart controller helper that is used to lock/unlock all
        charts when relevant parts of tables are changed
    */
    virtual SwChartLockController_Helper & GetChartControllerHelper() = 0;

 protected:
    virtual ~IDocumentChartDataProviderAccess() {};
 };

 #endif // IDOCUMENTCHARTDATAPROVIDER_HXX_INCLUDED

