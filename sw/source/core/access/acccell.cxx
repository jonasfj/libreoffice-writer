 /*************************************************************************
 *
 *  $RCSfile: acccell.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: mib $ $Date: 2002-04-11 13:41:40 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEROLE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleRole.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLESTATETYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleStateType.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEEVENTID_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleEventId.hpp>
#endif

#ifndef _UTL_ACCESSIBLESTATESETHELPER_HXX_
#include <unotools/accessiblestatesethelper.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _CELLFRM_HXX
#include <cellfrm.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _CRSRSH_HXX
#include "crsrsh.hxx"
#endif
#ifndef _VISCRS_HXX
#include "viscrs.hxx"
#endif
#ifndef _ACCFRMOBJSLIST_HXX
#include <accfrmobjslist.hxx>
#endif

#ifndef _ACCMAP_HXX
#include "accmap.hxx"
#endif
#ifndef _ACCCELL_HXX
#include <acccell.hxx>
#endif

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::accessibility;
using namespace ::rtl;

const sal_Char sServiceName[] = "com.sun.star.table.AccessibleCellView";
const sal_Char sImplementationName[] = "SwAccessibleCell";

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
        SwAccessibleMap *pMap,
        const SwCellFrm *pCellFrm	) :
    SwAccessibleContext( pMap, AccessibleRole::TABLE_CELL, pCellFrm ),
    bIsSelected( sal_False )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    OUString sName( pCellFrm->GetTabBox()->GetName() );
    SetName( sName );

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
            if( rLower.IsAccessible()  )
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

    const SwFrm *pParent = GetParent( GetFrm() );
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
                aEvent.EventId = AccessibleEventId::ACCESSIBLE_SELECTION_EVENT;
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
        throw (::com::sun::star::uno::RuntimeException)
{
    return GetName();
}

OUString SAL_CALL SwAccessibleCell::getImplementationName()
        throw( RuntimeException )
{
    return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationName));
}

sal_Bool SAL_CALL SwAccessibleCell::supportsService(
        const ::rtl::OUString& sTestServiceName)
    throw (::com::sun::star::uno::RuntimeException)
{
    return sTestServiceName.equalsAsciiL( sServiceName, sizeof(sServiceName)-1 );
}

Sequence< OUString > SAL_CALL SwAccessibleCell::getSupportedServiceNames()
        throw( ::com::sun::star::uno::RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceName) );
    return aRet;
}
