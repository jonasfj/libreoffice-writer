 /*************************************************************************
 *
 *  $RCSfile: accpreview.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: mib $ $Date: 2002-08-15 10:25:19 $
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

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif
#ifndef _ACCESS_HRC
#include "access.hrc"
#endif
#ifndef _ACCPREVIEW_HXX
#include <accpreview.hxx>
#endif


const sal_Char sServiceName[] = "drafts.com.sun.star.text.AccessibleTextDocumentPageView";
const sal_Char sImplementationName[] = "com.sun.star.comp.Writer.SwAccessibleDocumentPageView";


// using namespace ::drafts::com::sun::star::accessibility;

using ::com::sun::star::lang::IndexOutOfBoundsException;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;




//
// SwAccessiblePreview
//

SwAccessiblePreview::SwAccessiblePreview( SwAccessibleMap *pMap ) :
    SwAccessibleDocumentBase( pMap )
{
    SetName( GetResource( STR_ACCESS_DOC_NAME ) );
}

SwAccessiblePreview::~SwAccessiblePreview()
{
}

OUString SwAccessiblePreview::getImplementationName( )
    throw( RuntimeException )
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( sImplementationName ) );
}

sal_Bool SwAccessiblePreview::supportsService( const OUString& rServiceName )
    throw( RuntimeException )
{
    return rServiceName.equalsAsciiL( 
                RTL_CONSTASCII_STRINGPARAM( sServiceName) ) ||
        rServiceName.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM( sAccessibleServiceName ) );
}

Sequence<OUString> SwAccessiblePreview::getSupportedServiceNames( )
    throw( RuntimeException )
{
    Sequence<OUString> aSeq( 2 );
    aSeq[0] = OUString( RTL_CONSTASCII_USTRINGPARAM( sServiceName ) );
    aSeq[1] = OUString( RTL_CONSTASCII_USTRINGPARAM( sAccessibleServiceName ) );
    return aSeq;
}

Sequence< sal_Int8 > SAL_CALL SwAccessiblePreview::getImplementationId() 
        throw(RuntimeException)
{
    static Sequence< sal_Int8 > aId( 16 );
    static sal_Bool bInit = sal_False;
    if(!bInit)
        rtl_createUuid( reinterpret_cast< sal_uInt8 * >(aId.getArray() ), 
                        0, sal_True );
    return aId;
}
