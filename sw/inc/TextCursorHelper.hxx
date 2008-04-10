/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: TextCursorHelper.hxx,v $
 * $Revision: 1.6 $
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
#ifndef _SW_TEXTCURSORHELPER_HXX
#define _SW_TEXTCURSORHELPER_HXX

#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <cppuhelper/implbase1.hxx>

class SwDoc;
class SwPaM;

class OTextCursorHelper : public ::cppu::ImplHelper1< ::com::sun::star::lang::XUnoTunnel>
{
public:

    OTextCursorHelper(){}
    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

    //XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

    virtual const SwPaM*		GetPaM() const	= 0;
    virtual SwPaM*				GetPaM()		= 0;
    virtual const SwDoc* 		GetDoc() const	= 0;
    virtual SwDoc* 				GetDoc()		= 0;
};

#endif // _SW_TEXTCURSORHELPER_HXX


