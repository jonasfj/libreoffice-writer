/*************************************************************************
 *
 *  $RCSfile: accselectionhelper.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: dvo $ $Date: 2002-04-12 12:48:59 $
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
#ifndef _ACCSELECTIONHELPER_HXX_
#define _ACCSELECTIONHELPER_HXX_

class SwAccessibleContext;
class SwRootFrm;
class SwFEShell;
class SwFlyFrm;

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLESELECTION_HPP_
#include <drafts/com/sun/star/accessibility/XAccessibleSelection.hpp>
#endif

class SwAccessibleSelectionHelper
{
    /// the context on which this helper works
    SwAccessibleContext& rContext;


    /// get FE-Shell
    SwFEShell* GetFEShell();

    /// get current selected Fly-Frame, if it's a child of this frame
    const SwFlyFrm* GetSelectedChildFlyFrame();

public:

    SwAccessibleSelectionHelper( SwAccessibleContext& rContext );
    ~SwAccessibleSelectionHelper();


    //=====  XAccessibleSelection  ============================================

    void selectAccessibleChild( 
        sal_Int32 nChildIndex ) 
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException );

    sal_Bool isAccessibleChildSelected( 
        sal_Int32 nChildIndex ) 
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException );
    void clearAccessibleSelection(  ) 
        throw ( ::com::sun::star::uno::RuntimeException );
    void selectAllAccessible(  ) 
        throw ( ::com::sun::star::uno::RuntimeException );
    sal_Int32 getSelectedAccessibleChildCount(  ) 
        throw ( ::com::sun::star::uno::RuntimeException );
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible > getSelectedAccessibleChild( 
        sal_Int32 nSelectedChildIndex ) 
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException);
    void deselectSelectedAccessibleChild( 
        sal_Int32 nSelectedChildIndex ) 
        throw ( ::com::sun::star::lang::IndexOutOfBoundsException, 
                ::com::sun::star::uno::RuntimeException );
};


#endif

