/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: vbadialogs.cxx,v $
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
#include <ooo/vba/word/XDialog.hpp>
#include "vbadialogs.hxx"
#include "vbadialog.hxx"

using namespace ::ooo::vba;
using namespace ::com::sun::star;

uno::Any
SwVbaDialogs::Item( const uno::Any &aItem ) throw (uno::RuntimeException)
{
    sal_Int32 nIndex = 0;
    aItem >>= nIndex;
    uno::Reference< word::XDialog > aDialog( new SwVbaDialog( uno::Reference< XHelperInterface >( Application(),uno::UNO_QUERY_THROW ), mxContext, m_xModel, nIndex ) );
    return uno::Any( aDialog );
}

rtl::OUString& 
SwVbaDialogs::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("SwVbaDialogs") );
    return sImplName;
}

uno::Sequence< rtl::OUString > 
SwVbaDialogs::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Dialogs" ) );
    }
    return aServiceNames;
}

