/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2010 Miklos Vajna.
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

#ifndef _RTFIMPORTFILTER_HXX_
#define _RTFIMPORTFILTER_HXX_

#include <com/sun/star/document/XFilter.hpp>
#include <com/sun/star/document/XImporter.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/implbase2.hxx>
#include <tools/stream.hxx>
#include <shellio.hxx>

/// The physical access to the RTF document (for reading).
class RtfImportFilter : public cppu::WeakImplHelper2
<
    com::sun::star::document::XFilter,
    com::sun::star::document::XImporter
>
{
protected:
    ::com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory > m_xMSF;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > m_xDstDoc;
public:
    RtfImportFilter( const ::com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& xMSF );
    virtual ~RtfImportFilter();

    // XFilter
    virtual sal_Bool SAL_CALL filter( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL cancel(  )
        throw (::com::sun::star::uno::RuntimeException);

    // XImporter
    virtual void SAL_CALL setTargetDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc )
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
};

::rtl::OUString RtfImport_getImplementationName();
::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL RtfImport_getSupportedServiceNames()
    throw();
::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL RtfImport_createInstance(
                                                                        const ::com::sun::star::uno::Reference<
                                                                        com::sun::star::lang::XMultiServiceFactory > &xMSF)
    throw( ::com::sun::star::uno::Exception );

#define IMPL_NAME_RTFIMPORT "com.sun.star.comp.Writer.RtfImport"

#endif // _RTFIMPORTFILTER_HXX_
/* vi:set shiftwidth=4 expandtab: */
