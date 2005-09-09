/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: acchyperlink.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:51:26 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#ifndef _ACCHYPERLINK_HXX
#define _ACCHYPERLINK_HXX


#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEHYPERLINK_HPP_
#include <com/sun/star/accessibility/XAccessibleHyperlink.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif

class SwAccessibleParagraph;
class SwTxtAttr;

class SwAccessibleHyperlink : 
        public ::cppu::WeakImplHelper1<
        ::com::sun::star::accessibility::XAccessibleHyperlink >
{
    friend class SwAccessibleParagraph;
    friend class SwAccessibleHyperTextData;
    sal_uInt16 nHintPos;
    ::vos::ORef< SwAccessibleParagraph > xPara;
    sal_Int32 nStartIdx;
    sal_Int32 nEndIdx;

    SwAccessibleHyperlink( sal_uInt16 nHintPos,
                           SwAccessibleParagraph *p,
                              sal_Int32 nStt, sal_Int32 nEnd	);

    const SwTxtAttr *GetTxtAttr() const;
    void Invalidate();

public:

    // XAccessibleAction
    virtual sal_Int32 SAL_CALL getAccessibleActionCount() 
        throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL doAccessibleAction( sal_Int32 nIndex ) 
        throw (::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getAccessibleActionDescription( 
                sal_Int32 nIndex ) 
        throw (::com::sun::star::lang::IndexOutOfBoundsException,
                ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< 
            ::com::sun::star::accessibility::XAccessibleKeyBinding > SAL_CALL
               getAccessibleActionKeyBinding( sal_Int32 nIndex ) 
        throw (::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException);

    // XAccessibleHyperlink
    virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleActionAnchor( 
                sal_Int32 nIndex ) 
        throw (::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleActionObject( 
            sal_Int32 nIndex ) 
        throw (::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getStartIndex() 
        throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getEndIndex() 
        throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isValid(  ) 
        throw (::com::sun::star::uno::RuntimeException);
};

#endif

