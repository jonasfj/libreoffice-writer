 /*************************************************************************
 *
 *  $RCSfile: accheaderfooter.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: mib $ $Date: 2002-07-10 16:53:33 $
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

#ifndef _UTL_ACCESSIBLESTATESETHELPER_HXX_
#include <unotools/accessiblestatesethelper.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _HFFRM_HXX
#include <hffrm.hxx>
#endif

#ifndef _ACCHEADERFOOTER_HXX
#include "accheaderfooter.hxx"
#endif
#ifndef _ACCESS_HRC
#include "access.hrc"
#endif

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::accessibility;
using namespace ::rtl;

const sal_Char sServiceNameHeader[] = "drafts.com.sun.star.text.AccessibleHeaderView";
const sal_Char sServiceNameFooter[] = "drafts.com.sun.star.text.AccessibleFooterView";
const sal_Char sImplementationNameHeader[] = "com.sun.star.comp.Writer.SwAccessibleHeader";
const sal_Char sImplementationNameFooter[] = "com.sun.star.comp.Writer.SwAccessibleFooter";

SwAccessibleHeaderFooter::SwAccessibleHeaderFooter( 
        SwAccessibleMap *pMap,
        const SwHeaderFrm *pHdFrm	) :
    SwAccessibleContext( pMap, AccessibleRole::HEADER, pHdFrm )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    OUString sArg( OUString::valueOf( (sal_Int32)pHdFrm->GetPhyPageNum() ) );
    SetName( GetResource( STR_ACCESS_HEADER_NAME, &sArg ) );
}

SwAccessibleHeaderFooter::SwAccessibleHeaderFooter( 
        SwAccessibleMap *pMap,
        const SwFooterFrm *pFtFrm	) :
    SwAccessibleContext( pMap, AccessibleRole::FOOTER, pFtFrm )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    OUString sArg( OUString::valueOf( (sal_Int32)pFtFrm->GetPhyPageNum() ) );
    SetName( GetResource( STR_ACCESS_FOOTER_NAME, &sArg ) );
}

SwAccessibleHeaderFooter::~SwAccessibleHeaderFooter()
{
}

OUString SAL_CALL SwAccessibleHeaderFooter::getAccessibleDescription (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleContext )

    sal_uInt16 nResId = AccessibleRole::HEADER == GetRole()
        ? STR_ACCESS_HEADER_DESC
        : STR_ACCESS_FOOTER_DESC ;

    OUString sArg( GetFormattedPageNumber() );

    return GetResource( nResId, &sArg );
}

OUString SAL_CALL SwAccessibleHeaderFooter::getImplementationName()
        throw( RuntimeException )
{
    if( AccessibleRole::HEADER == GetRole() )
        return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationNameHeader));
    else
        return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationNameFooter));
}

sal_Bool SAL_CALL SwAccessibleHeaderFooter::supportsService(
        const ::rtl::OUString& sTestServiceName)
    throw (::com::sun::star::uno::RuntimeException)
{
    if( sTestServiceName.equalsAsciiL( sAccessibleServiceName, 
                                       sizeof(sAccessibleServiceName)-1 ) )
        return sal_True;
    else if( AccessibleRole::HEADER == GetRole() )
        return sTestServiceName.equalsAsciiL( sServiceNameHeader, sizeof(sServiceNameHeader)-1 );
    else
        return sTestServiceName.equalsAsciiL( sServiceNameFooter, sizeof(sServiceNameFooter)-1 );

}

Sequence< OUString > SAL_CALL SwAccessibleHeaderFooter::getSupportedServiceNames()
        throw( ::com::sun::star::uno::RuntimeException )
{
    Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    if( AccessibleRole::HEADER == GetRole() )
        pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceNameHeader) );
    else
        pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceNameFooter) );
    pArray[1] = OUString( RTL_CONSTASCII_USTRINGPARAM(sAccessibleServiceName) );
    return aRet;
}
