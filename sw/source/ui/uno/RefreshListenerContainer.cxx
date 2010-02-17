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
#include "precompiled_sw.hxx"


#include <RefreshListenerContainer.hxx>
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/util/XRefreshListener.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::util;

SwRefreshListenerContainer::SwRefreshListenerContainer( uno::XInterface* pxParent2 )
: SwEventListenerContainer ( pxParent2 )
{
}

void SwRefreshListenerContainer::Refreshed ()
{
    if(!pListenerArr)
        return;

    lang::EventObject aObj(pxParent);
    for(sal_uInt16 i = 0, nEnd = pListenerArr->Count(); i < nEnd ; i++)
    {
        Reference < XRefreshListener > xRefreshListener = Reference < XRefreshListener > ( *pListenerArr->GetObject(i), UNO_QUERY );
        xRefreshListener->refreshed(aObj);
    }
}
