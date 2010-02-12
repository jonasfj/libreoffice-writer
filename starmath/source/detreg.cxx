/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_starmath.hxx"
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/uno/Sequence.h>
#include <rtl/ustring.hxx>

#include "smdetect.hxx"

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;

extern "C" {

void SAL_CALL component_getImplementationEnvironment(
        const  sal_Char**   ppEnvironmentTypeName,
        uno_Environment**  /*ppEnvironment*/           )
{
    *ppEnvironmentTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME ;
}

sal_Bool SAL_CALL component_writeInfo(  void*   /*pServiceManager*/,
                                        void*	pRegistryKey	)
{
    Reference< ::registry::XRegistryKey >
            xKey( reinterpret_cast< ::registry::XRegistryKey* >( pRegistryKey ) ) ;

    OUString aDelimiter( RTL_CONSTASCII_USTRINGPARAM("/") );
    OUString aUnoServices( RTL_CONSTASCII_USTRINGPARAM( "/UNO/SERVICES") );

    // Eigentliche Implementierung und ihre Services registrieren
    sal_Int32 i;
    Reference< ::registry::XRegistryKey >  xNewKey;

    xNewKey = xKey->createKey( aDelimiter + SmFilterDetect::impl_getStaticImplementationName() +
                               aUnoServices );

    Sequence< OUString > aServices = SmFilterDetect::impl_getStaticSupportedServiceNames();
    for(i = 0; i < aServices.getLength(); i++ )
        xNewKey->createKey( aServices.getConstArray()[i] );

    return sal_True;
}

void* SAL_CALL component_getFactory( const sal_Char* pImplementationName,
                                     void* pServiceManager,
                                     void* /*pRegistryKey*/ )
{
    // Set default return value for this operation - if it failed.
    void* pReturn = NULL ;

    if	(
            ( pImplementationName	!=	NULL ) &&
            ( pServiceManager		!=	NULL )
        )
    {
        // Define variables which are used in following macros.
        Reference< XSingleServiceFactory >   xFactory                                                                                                ;
        Reference< XMultiServiceFactory >    xServiceManager( reinterpret_cast< XMultiServiceFactory* >( pServiceManager ) ) ;

        if( SmFilterDetect::impl_getStaticImplementationName().equalsAscii( pImplementationName ) )
        {
            xFactory = ::cppu::createSingleFactory( xServiceManager,
            SmFilterDetect::impl_getStaticImplementationName(),
            SmFilterDetect::impl_createInstance,
            SmFilterDetect::impl_getStaticSupportedServiceNames() );
        }

        // Factory is valid - service was found.
        if ( xFactory.is() )
        {
            xFactory->acquire();
            pReturn = xFactory.get();
        }
    }

    // Return with result of this operation.
    return pReturn ;
}
} // extern "C"



