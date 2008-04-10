/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unoevtlstnr.cxx,v $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <unoevtlstnr.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/lang/XEventListener.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;

/* -----------------22.04.99 11:24-------------------
 *
 * --------------------------------------------------*/
SV_IMPL_PTRARR(SwEvtLstnrArray, XEventListenerPtr);

/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwEventListenerContainer::SwEventListenerContainer( uno::XInterface* _pxParent) :
    pListenerArr(0),
    pxParent(_pxParent)
{
}
/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
SwEventListenerContainer::~SwEventListenerContainer()
{
    if(pListenerArr && pListenerArr->Count())
    {
        pListenerArr->DeleteAndDestroy(0, pListenerArr->Count());
    }
    delete pListenerArr;
}
/*-- 22.04.99 11:24:59---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwEventListenerContainer::AddListener(const uno::Reference< lang::XEventListener > & rxListener)
{
    if(!pListenerArr)
        pListenerArr = new SwEvtLstnrArray;
    uno::Reference< lang::XEventListener > * pInsert = new uno::Reference< lang::XEventListener > ;
    *pInsert = rxListener;
    pListenerArr->Insert(pInsert, pListenerArr->Count());
}
/*-- 22.04.99 11:25:00---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool	SwEventListenerContainer::RemoveListener(const uno::Reference< lang::XEventListener > & rxListener)
{
    if(!pListenerArr)
        return sal_False;
    else
    {
         lang::XEventListener* pLeft = rxListener.get();
        for(sal_uInt16 i = 0; i < pListenerArr->Count(); i++)
        {
            XEventListenerPtr pElem = pListenerArr->GetObject(i);
             lang::XEventListener* pRight = pElem->get();
            if(pLeft == pRight)
            {
                pListenerArr->Remove(i);
                delete pElem;
                return sal_True;
            }
        }
    }
    return sal_False;
}
/*-- 22.04.99 11:25:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void	SwEventListenerContainer::Disposing()
{
    if(!pListenerArr)
        return;

    lang::EventObject aObj(pxParent);
    for(sal_uInt16 i = 0; i < pListenerArr->Count(); i++)
    {
        XEventListenerPtr pElem = pListenerArr->GetObject(i);
        (*pElem)->disposing(aObj);
    }
    pListenerArr->DeleteAndDestroy(0, pListenerArr->Count());
}


