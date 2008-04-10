/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swcalwrp.hxx,v $
 * $Revision: 1.5 $
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

#ifndef _SWCALWRP_HXX
#define _SWCALWRP_HXX

#include <i18npool/lang.h>
#include <tools/string.hxx>
#include <unotools/calendarwrapper.hxx>

class SwCalendarWrapper : public CalendarWrapper
{
    String sUniqueId;
    USHORT nLang;

public:
    SwCalendarWrapper( const ::com::sun::star::uno::Reference<
                    ::com::sun::star::lang::XMultiServiceFactory > & xMSF )
        : CalendarWrapper( xMSF ), nLang( LANGUAGE_SYSTEM )
    {}

    void LoadDefaultCalendar( USHORT nLang );
};


extern SwCalendarWrapper* pCalendarWrapper;


#endif

