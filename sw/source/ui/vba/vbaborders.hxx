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
#ifndef SW_VBA_BORDERS_HXX
#define SW_VBA_BORDERS_HXX

#include <cppuhelper/implbase1.hxx>
#include <ooo/vba/word/XBorders.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <vbahelper/vbacollectionimpl.hxx>

typedef CollTestImplHelper< ov::word::XBorders > SwVbaBorders_BASE;
class VbaPalette;
class SwVbaBorders : public SwVbaBorders_BASE
{
    // XEnumerationAccess
    virtual css::uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException);
    css::uno::Reference< css::beans::XPropertySet > m_xProps;
public:
    SwVbaBorders( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::table::XCellRange >& xRange, VbaPalette& rPalette );
    virtual ~SwVbaBorders() {}

    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // SwVbaCollectionBaseImpl	
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource ); 
    
    virtual sal_Bool SAL_CALL getShadow() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setShadow( sal_Bool _shadow ) throw (css::uno::RuntimeException);

    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif //SW_VBA_BORDERS_HXX

