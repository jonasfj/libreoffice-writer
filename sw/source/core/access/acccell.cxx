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


#include <vos/mutex.hxx>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/accessibility/AccessibleEventId.hpp>
#include <unotools/accessiblestatesethelper.hxx>
#include <rtl/uuid.h>
#include <vcl/svapp.hxx>
#include <cellfrm.hxx>
#include <tabfrm.hxx>
#include <swtable.hxx>
#include "crsrsh.hxx"
#include "viscrs.hxx"
#include <accfrmobjslist.hxx>
#include "frmfmt.hxx"
#include "cellatr.hxx"
#include "accmap.hxx"
#include <acccell.hxx>

#ifndef _STLP_CFLOAT
#include <cfloat>
#endif

#include <limits.h>

using namespace ::com::sun::star;
using namespace ::com::sun::star::accessibility;
using ::rtl::OUString;

const sal_Char sServiceName[] = "com.sun.star.table.AccessibleCellView";
const sal_Char sImplementationName[] = "com.sun.star.comp.Writer.SwAccessibleCellView";

sal_Bool SwAccessibleCell::IsSelected()
{
    sal_Bool bRet = sal_False;

    DBG_ASSERT( GetMap(), "no map?" );
    const ViewShell *pVSh = GetMap()->GetShell();
    DBG_ASSERT( pVSh, "no shell?" );
    if( pVSh->ISA( SwCrsrShell ) )
    {
        const SwCrsrShell *pCSh = static_cast< const SwCrsrShell * >( pVSh );
        if( pCSh->IsTableMode() )
        {
            const SwCellFrm *pCFrm =
                static_cast< const SwCellFrm * >( GetFrm() );
            SwTableBox *pBox =
                const_cast< SwTableBox *>( pCFrm->GetTabBox() ); //SVPtrArr!
            bRet = pCSh->GetTableCrsr()->GetBoxes().Seek_Entry( pBox );
        }
    }

    return bRet;
}

void SwAccessibleCell::GetStates(
        ::utl::AccessibleStateSetHelper& rStateSet )
{
    SwAccessibleContext::GetStates( rStateSet );

    // SELECTABLE
    const ViewShell *pVSh = GetMap()->GetShell();
    DBG_ASSERT( pVSh, "no shell?" );
    if( pVSh->ISA( SwCrsrShell ) )
        rStateSet.AddState( AccessibleStateType::SELECTABLE );

    // SELECTED
    if( IsSelected() )
    {
        rStateSet.AddState( AccessibleStateType::SELECTED );
        ASSERT( bIsSelected, "bSelected out of sync" );
        ::vos::ORef < SwAccessibleContext > xThis( this );
        GetMap()->SetCursorContext( xThis );
    }
}

SwAccessibleCell::SwAccessibleCell(
        SwAccessibleMap *pInitMap,
        const SwCellFrm *pCellFrm	) :
    SwAccessibleContext( pInitMap, AccessibleRole::TABLE_CELL, pCellFrm ),
    bIsSelected( sal_False )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    OUString sBoxName( pCellFrm->GetTabBox()->GetName() );
    SetName( sBoxName );

    bIsSelected = IsSelected();
}

sal_Bool SwAccessibleCell::_InvalidateMyCursorPos()
{
    sal_Bool bNew = IsSelected();
    sal_Bool bOld;
    {
        vos::OGuard aGuard( aMutex );
        bOld = bIsSelected;
        bIsSelected = bNew;
    }
    if( bNew )
    {
        // remember that object as the one that has the caret. This is
        // neccessary to notify that object if the cursor leaves it.
        ::vos::ORef < SwAccessibleContext > xThis( this );
        GetMap()->SetCursorContext( xThis );
    }

    sal_Bool bChanged = bOld != bNew;
    if( bChanged )
        FireStateChangedEvent( AccessibleStateType::SELECTED, bNew );

    return bChanged;
}

sal_Bool SwAccessibleCell::_InvalidateChildrenCursorPos( const SwFrm *pFrm )
{
    sal_Bool bChanged = sal_False;

    const SwFrmOrObjSList aVisList( GetVisArea(), pFrm );
    SwFrmOrObjSList::const_iterator aIter( aVisList.begin() );
    while( aIter != aVisList.end() )
    {
        const SwFrmOrObj& rLower = *aIter;
        const SwFrm *pLower = rLower.GetSwFrm();
        if( pLower )
        {
            if( rLower.IsAccessible( GetMap()->GetShell()->IsPreView() )  )
            {
                ::vos::ORef< SwAccessibleContext > xAccImpl(
                    GetMap()->GetContextImpl( pLower, sal_False ) );
                if( xAccImpl.isValid() )
                {
                    ASSERT( xAccImpl->GetFrm()->IsCellFrm(),
                             "table child is not a cell frame" )
                    bChanged |= static_cast< SwAccessibleCell *>(
                            xAccImpl.getBodyPtr() )->_InvalidateMyCursorPos();
                }
                else
                    bChanged = sal_True; // If the context is not know we
                                         // don't know whether the selection
                                         // changed or not.
            }
            else
            {
                // This is a box with sub rows.
                bChanged |= _InvalidateChildrenCursorPos( pLower );
            }
        }
        ++aIter;
    }

    return bChanged;
}

void SwAccessibleCell::_InvalidateCursorPos()
{

    const SwFrm *pParent = GetParent( GetFrm(), IsInPagePreview() );
    ASSERT( pParent->IsTabFrm(), "parent is not a tab frame" );
    const SwTabFrm *pTabFrm = static_cast< const SwTabFrm * >( pParent );
    if( pTabFrm->IsFollow() )
        pTabFrm = pTabFrm->FindMaster();

    while( pTabFrm )
    {
        sal_Bool bChanged = _InvalidateChildrenCursorPos( pTabFrm );
        if( bChanged )
        {
            ::vos::ORef< SwAccessibleContext > xAccImpl(
                GetMap()->GetContextImpl( pTabFrm, sal_False ) );
            if( xAccImpl.isValid() )
            {
                AccessibleEventObject aEvent;
                aEvent.EventId = AccessibleEventId::SELECTION_CHANGED;
                xAccImpl->FireAccessibleEvent( aEvent );
            }
        }

        pTabFrm = pTabFrm->GetFollow();
    }
}

sal_Bool SwAccessibleCell::HasCursor()
{
    vos::OGuard aGuard( aMutex );
    return bIsSelected;
}

SwAccessibleCell::~SwAccessibleCell()
{
}

OUString SAL_CALL SwAccessibleCell::getAccessibleDescription (void)
        throw (uno::RuntimeException)
{
    return GetName();
}

OUString SAL_CALL SwAccessibleCell::getImplementationName()
        throw( uno::RuntimeException )
{
    return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationName));
}

sal_Bool SAL_CALL SwAccessibleCell::supportsService(
        const ::rtl::OUString& sTestServiceName)
    throw (uno::RuntimeException)
{
    return sTestServiceName.equalsAsciiL( sServiceName,
                                          sizeof(sServiceName)-1 ) ||
           sTestServiceName.equalsAsciiL( sAccessibleServiceName,
                                             sizeof(sAccessibleServiceName)-1 );
}

uno::Sequence< OUString > SAL_CALL SwAccessibleCell::getSupportedServiceNames()
        throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceName) );
    pArray[1] = OUString( RTL_CONSTASCII_USTRINGPARAM(sAccessibleServiceName) );
    return aRet;
}

void SwAccessibleCell::Dispose( sal_Bool bRecursive )
{
    const SwFrm *pParent = GetParent( GetFrm(), IsInPagePreview() );
    ::vos::ORef< SwAccessibleContext > xAccImpl(
            GetMap()->GetContextImpl( pParent, sal_False ) );
    if( xAccImpl.isValid() )
        xAccImpl->DisposeChild( GetFrm(), bRecursive );
    SwAccessibleContext::Dispose( bRecursive );
}

void SwAccessibleCell::InvalidatePosOrSize( const SwRect& rOldBox )
{
    const SwFrm *pParent = GetParent( GetFrm(), IsInPagePreview() );
    ::vos::ORef< SwAccessibleContext > xAccImpl(
            GetMap()->GetContextImpl( pParent, sal_False ) );
    if( xAccImpl.isValid() )
        xAccImpl->InvalidateChildPosOrSize( GetFrm(), rOldBox );
    SwAccessibleContext::InvalidatePosOrSize( rOldBox );
}


// =====  XAccessibleInterface  ===========================================

uno::Any SwAccessibleCell::queryInterface( const uno::Type& rType )
    throw( uno::RuntimeException )
{
    if ( rType == ::getCppuType( static_cast< uno::Reference< XAccessibleValue > * >( 0 ) ) )
    {
        uno::Reference<XAccessibleValue> xValue = this;
        uno::Any aRet;
        aRet <<= xValue;
        return aRet;
    }
    else
    {
        return SwAccessibleContext::queryInterface( rType );
    }
}

//====== XTypeProvider ====================================================
uno::Sequence< uno::Type > SAL_CALL SwAccessibleCell::getTypes()
    throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type > aTypes( SwAccessibleContext::getTypes() );

    sal_Int32 nIndex = aTypes.getLength();
    aTypes.realloc( nIndex + 1 );

    uno::Type* pTypes = aTypes.getArray();
    pTypes[nIndex] = ::getCppuType( static_cast< uno::Reference< XAccessibleValue > * >( 0 ) );

    return aTypes;
}

uno::Sequence< sal_Int8 > SAL_CALL SwAccessibleCell::getImplementationId()
        throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    static uno::Sequence< sal_Int8 > aId( 16 );
    static sal_Bool bInit = sal_False;
    if(!bInit)
    {
        rtl_createUuid( (sal_uInt8 *)(aId.getArray() ), 0, sal_True );
        bInit = sal_True;
    }
    return aId;
}

// =====  XAccessibleValue  ===============================================

SwFrmFmt* SwAccessibleCell::GetTblBoxFormat() const
{
    DBG_ASSERT( GetFrm() != NULL, "no frame?" );
    DBG_ASSERT( GetFrm()->IsCellFrm(), "no cell frame?" );

    const SwCellFrm* pCellFrm = static_cast<const SwCellFrm*>( GetFrm() );
    return pCellFrm->GetTabBox()->GetFrmFmt();
}


uno::Any SwAccessibleCell::getCurrentValue( )
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    CHECK_FOR_DEFUNC( XAccessibleValue );

    uno::Any aAny;
    aAny <<= GetTblBoxFormat()->GetTblBoxValue().GetValue();
    return aAny;
}

sal_Bool SwAccessibleCell::setCurrentValue( const uno::Any& aNumber )
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    CHECK_FOR_DEFUNC( XAccessibleValue );

    double fValue = 0;
    sal_Bool bValid = (aNumber >>= fValue);
    if( bValid )
    {
        SwTblBoxValue aValue( fValue );
        GetTblBoxFormat()->SetFmtAttr( aValue );
    }
    return bValid;
}

uno::Any SwAccessibleCell::getMaximumValue( )
    throw( uno::RuntimeException )
{
    uno::Any aAny;
    aAny <<= DBL_MAX;
    return aAny;
}

uno::Any SwAccessibleCell::getMinimumValue(  )
    throw( uno::RuntimeException )
{
    uno::Any aAny;
    aAny <<= -DBL_MAX;
    return aAny;
}
