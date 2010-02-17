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


#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <accselectionhelper.hxx>

#ifndef _ACCCONTEXT_HXX
#include <acccontext.hxx>
#endif
#include <accmap.hxx>
#include <svx/AccessibleShape.hxx>
#include <viewsh.hxx>
#include "fesh.hxx"
#include <vcl/svapp.hxx>        // for SolarMutex
#include <tools/debug.hxx>


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

using ::com::sun::star::accessibility::XAccessible;
using ::com::sun::star::accessibility::XAccessibleContext;
using ::com::sun::star::accessibility::XAccessibleSelection;



SwAccessibleSelectionHelper::SwAccessibleSelectionHelper(
    SwAccessibleContext& rCtxt ) :
        rContext( rCtxt )
{
}

SwAccessibleSelectionHelper::~SwAccessibleSelectionHelper()
{
}

SwFEShell* SwAccessibleSelectionHelper::GetFEShell()
{
    DBG_ASSERT( rContext.GetMap() != NULL, "no map?" );
    ViewShell* pViewShell = rContext.GetMap()->GetShell();
    DBG_ASSERT( pViewShell != NULL,
                "No view shell? Then what are you looking at?" );

    SwFEShell* pFEShell = NULL;
    if( pViewShell->ISA( SwFEShell ) )
    {
        pFEShell = static_cast<SwFEShell*>( pViewShell );
    }

    return pFEShell;
}

void SwAccessibleSelectionHelper::throwIndexOutOfBoundsException()
        throw ( lang::IndexOutOfBoundsException )
{
    Reference < XAccessibleContext > xThis( &rContext );
    Reference < XAccessibleSelection >xSelThis( xThis, UNO_QUERY );
    lang::IndexOutOfBoundsException aExcept(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("index out of bounds") ),
                xSelThis );										\
    throw aExcept;
}


//=====  XAccessibleSelection  ============================================

void SwAccessibleSelectionHelper::selectAccessibleChild(
    sal_Int32 nChildIndex )
    throw ( lang::IndexOutOfBoundsException,
            RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // Get the respective child as SwFrm (also do index checking), ...
    const SwFrmOrObj aChild = rContext.GetChild( nChildIndex );
    if( !aChild.IsValid() )
        throwIndexOutOfBoundsException();

    // we can only select fly frames, so we ignore (should: return
    // false) all other attempts at child selection
    sal_Bool bRet = sal_False;
    SwFEShell* pFEShell = GetFEShell();
    if( pFEShell != NULL )
    {
        const SdrObject *pObj = aChild.GetSdrObject();
        if( pObj )
        {
            bRet = rContext.Select( const_cast< SdrObject *>( pObj ), 0==aChild.GetSwFrm());
        }
    }
    // no frame shell, or no frame, or no fly frame -> can't select

    // return bRet;
}

sal_Bool SwAccessibleSelectionHelper::isAccessibleChildSelected(
    sal_Int32 nChildIndex )
    throw ( lang::IndexOutOfBoundsException,
            RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // Get the respective child as SwFrm (also do index checking), ...
    const SwFrmOrObj aChild = rContext.GetChild( nChildIndex );
    if( !aChild.IsValid() )
        throwIndexOutOfBoundsException();

    // ... and compare to the currently selected frame
    sal_Bool bRet = sal_False;
    SwFEShell* pFEShell = GetFEShell();
    if( pFEShell )
    {
        if( aChild.GetSwFrm() != 0 )
        {
            bRet = (pFEShell->GetCurrFlyFrm() == aChild.GetSwFrm());
        }
        else
        {
            bRet = pFEShell->IsObjSelected( *aChild.GetSdrObject() );
        }
    }

    return bRet;
}

void SwAccessibleSelectionHelper::clearAccessibleSelection(  )
    throw ( RuntimeException )
{
    // return sal_False     // we can't deselect
}

void SwAccessibleSelectionHelper::selectAllAccessibleChildren(  )
    throw ( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // We can select only one. So iterate over the children to find
    // the first we can select, and select it.

    SwFEShell* pFEShell = GetFEShell();
    if( pFEShell )
    {
        ::std::list< SwFrmOrObj > aChildren;
        rContext.GetChildren( aChildren );

        ::std::list< SwFrmOrObj >::const_iterator aIter = aChildren.begin();
        ::std::list< SwFrmOrObj >::const_iterator aEndIter = aChildren.end();
        while( aIter != aEndIter )
        {
            const SwFrmOrObj& rChild = *aIter;
            const SdrObject *pObj = rChild.GetSdrObject();
            const SwFrm* pFrm = rChild.GetSwFrm();
            if( pObj && !(pFrm != 0 && pFEShell->IsObjSelected()) )
            {
                rContext.Select( const_cast< SdrObject *>( pObj ), 0==pFrm );
                if( pFrm )
                    break;
            }
            ++aIter;
        }
    }
}

sal_Int32 SwAccessibleSelectionHelper::getSelectedAccessibleChildCount(  )
    throw ( RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    sal_Int32 nCount = 0;
    // Only one frame can be selected at a time, and we only frames
    // for selectable children.
    SwFEShell* pFEShell = GetFEShell();
    if( pFEShell != 0 )
    {
        const SwFlyFrm *pFlyFrm = pFEShell->GetCurrFlyFrm();
        if( pFlyFrm )
        {
            if( rContext.GetParent(pFlyFrm, rContext.IsInPagePreview()) ==
                rContext.GetFrm() )
            {
                nCount = 1;
            }
        }
        else
        {
            sal_uInt16 nSelObjs = pFEShell->IsObjSelected();
            if( nSelObjs > 0 )
            {
                ::std::list< SwFrmOrObj > aChildren;
                rContext.GetChildren( aChildren );

                ::std::list< SwFrmOrObj >::const_iterator aIter =
                    aChildren.begin();
                ::std::list< SwFrmOrObj >::const_iterator aEndIter =
                    aChildren.end();
                while( aIter != aEndIter && nCount < nSelObjs )
                {
                    const SwFrmOrObj& rChild = *aIter;
                    if( rChild.GetSdrObject() && !rChild.GetSwFrm() &&
                        rContext.GetParent(rChild, rContext.IsInPagePreview())
                           == rContext.GetFrm() &&
                         pFEShell->IsObjSelected( *rChild.GetSdrObject() ) )
                    {
                        nCount++;
                    }
                    ++aIter;
                }
            }
        }
    }
    return nCount;
}

Reference<XAccessible> SwAccessibleSelectionHelper::getSelectedAccessibleChild(
    sal_Int32 nSelectedChildIndex )
    throw ( lang::IndexOutOfBoundsException,
            RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    // Since the index is relative to the selected children, and since
    // there can be at most one selected frame child, the index must
    // be 0, and a selection must exist, otherwise we have to throw an
    // lang::IndexOutOfBoundsException
    SwFEShell* pFEShell = GetFEShell();
    if( 0 == pFEShell )
        throwIndexOutOfBoundsException();

    SwFrmOrObj aChild;
    const SwFlyFrm *pFlyFrm = pFEShell->GetCurrFlyFrm();
    if( pFlyFrm )
    {
        if( 0 == nSelectedChildIndex &&
            rContext.GetParent(pFlyFrm, rContext.IsInPagePreview()) ==
                rContext.GetFrm() )
        {
            aChild = pFlyFrm;
        }
    }
    else
    {
        sal_uInt16 nSelObjs = pFEShell->IsObjSelected();
        if( 0 == nSelObjs || nSelectedChildIndex >= nSelObjs )
            throwIndexOutOfBoundsException();

        ::std::list< SwFrmOrObj > aChildren;
        rContext.GetChildren( aChildren );

        ::std::list< SwFrmOrObj >::const_iterator aIter = aChildren.begin();
        ::std::list< SwFrmOrObj >::const_iterator aEndIter = aChildren.end();
        while( aIter != aEndIter && !aChild.IsValid() )
        {
            const SwFrmOrObj& rChild = *aIter;
            if( rChild.GetSdrObject() && !rChild.GetSwFrm() &&
                rContext.GetParent(rChild, rContext.IsInPagePreview()) ==
                    rContext.GetFrm() &&
                pFEShell->IsObjSelected( *rChild.GetSdrObject() ) )
            {
                if( 0 == nSelectedChildIndex )
                    aChild = rChild;
                else
                    --nSelectedChildIndex;
            }
            ++aIter;
        }
    }

    if( !aChild.IsValid() )
        throwIndexOutOfBoundsException();

    DBG_ASSERT( rContext.GetMap() != NULL, "We need the map." );
    Reference< XAccessible > xChild;
    if( aChild.GetSwFrm() )
    {
        ::vos::ORef < SwAccessibleContext > xChildImpl(
                rContext.GetMap()->GetContextImpl( aChild.GetSwFrm(),
                sal_True ) );
        if( xChildImpl.isValid() )
        {
            xChildImpl->SetParent( &rContext );
            xChild = xChildImpl.getBodyPtr();
        }
    }
    else
    {
        ::vos::ORef < ::accessibility::AccessibleShape > xChildImpl(
                rContext.GetMap()->GetContextImpl( aChild.GetSdrObject(),
                                          &rContext, sal_True )  );
        if( xChildImpl.isValid() )
            xChild = xChildImpl.getBodyPtr();
    }
    return xChild;
}

// --> OD 2004-11-16 #111714# - index has to be treated as global child index.
void SwAccessibleSelectionHelper::deselectAccessibleChild(
    sal_Int32 nChildIndex )
    throw ( lang::IndexOutOfBoundsException,
            RuntimeException )
{
    // return sal_False     // we can't deselect
    if( nChildIndex < 0 ||
        nChildIndex >= rContext.GetChildCount() )
        throwIndexOutOfBoundsException();
}
