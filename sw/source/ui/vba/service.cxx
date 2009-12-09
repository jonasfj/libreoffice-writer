/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: service.cxx,v $
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
#include "cppuhelper/implementationentry.hxx"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "comphelper/servicedecl.hxx"

// =============================================================================
// component exports
// =============================================================================
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace sdecl = comphelper::service_decl;

namespace globals
{
extern sdecl::ServiceDecl const serviceDecl;
}

namespace document
{
extern sdecl::ServiceDecl const serviceDecl;
}

namespace wrapformat
{
extern sdecl::ServiceDecl const serviceDecl;
}

extern "C"
{
    void SAL_CALL component_getImplementationEnvironment( 
        const sal_Char ** ppEnvTypeName, uno_Environment ** /*ppEnv*/ )
    {
        OSL_TRACE("In component_getImplementationEnv");
        *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
    }

    sal_Bool SAL_CALL component_writeInfo( 
        lang::XMultiServiceFactory * pServiceManager, registry::XRegistryKey * pRegistryKey )
    {
        OSL_TRACE("In component_writeInfo");

    // Component registration
        return component_writeInfoHelper( pServiceManager, pRegistryKey, 
        globals::serviceDecl, document::serviceDecl, wrapformat::serviceDecl  ); 
    }

    void * SAL_CALL component_getFactory( 
        const sal_Char * pImplName, lang::XMultiServiceFactory * pServiceManager,
        registry::XRegistryKey * pRegistryKey )
    {
        OSL_TRACE("In component_getFactory for %s", pImplName );
    void* pRet =  component_getFactoryHelper(
            pImplName, pServiceManager, pRegistryKey, globals::serviceDecl, document::serviceDecl, wrapformat::serviceDecl );
    OSL_TRACE("Ret is 0x%x", pRet);
    return pRet;
    }
}
