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
#ifndef SW_WORD_VBA_HEADERFOOTERHELPER_HXX
#define SW_WORD_VBA_HEADERFOOTERHELPER_HXX

#include <vbahelper/vbahelper.hxx>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include <com/sun/star/text/XPageCursor.hpp>
#include <com/sun/star/style/XStyle.hpp>

class HeaderFooterHelper
{
public:
    static sal_Bool isHeader( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
    static sal_Bool isFirstPageHeader( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
    static sal_Bool isEvenPagesHeader( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
    static sal_Bool isFooter( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
    static sal_Bool isFirstPageFooter( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
    static sal_Bool isEvenPagesFooter( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
    static sal_Bool isPrimaryHeader( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
    static sal_Bool isPrimaryFooter( const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XText >& xCurrentText ) throw (css::uno::RuntimeException);
};

#endif
