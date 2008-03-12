 /*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: acchyperlink.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2008-03-12 12:14:51 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifndef COMPHELPER_ACCESSIBLE_KEYBINDING_HELPER_HXX
#include <comphelper/accessiblekeybindinghelper.hxx>
#endif
#ifndef _SWURL_HXX //autogen
#include <swurl.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _TXTINET_HXX
#include <txtinet.hxx>
#endif

#ifndef _ACCPARA_HXX
#include <accpara.hxx>
#endif
#ifndef _ACCHYPERLINK_HXX
#include <acchyperlink.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::accessibility;
using ::rtl::OUString;

SwAccessibleHyperlink::SwAccessibleHyperlink( sal_uInt16 nHPos,
    SwAccessibleParagraph *p, sal_Int32 nStt, sal_Int32 nEnd ) :
    nHintPos( nHPos ),
    xPara( p ),
    nStartIdx( nStt ),
    nEndIdx( nEnd )
{
}

const SwTxtAttr *SwAccessibleHyperlink::GetTxtAttr() const
{
    const SwTxtAttr *pTxtAttr = 0;
    if( xPara.isValid() && xPara->GetMap() )
    {
        const SwTxtNode *pTxtNd = xPara->GetTxtNode();
        const SwpHints *pHints = pTxtNd->GetpSwpHints();
        if( pHints && nHintPos < pHints->Count() )
        {
            const SwTxtAttr *pHt = (*pHints)[nHintPos];
            if( RES_TXTATR_INETFMT == pHt->Which() )
                pTxtAttr = pHt;
        }
    }

    return pTxtAttr;
}


// XAccessibleAction
sal_Int32 SAL_CALL SwAccessibleHyperlink::getAccessibleActionCount() 
        throw (uno::RuntimeException)
{
     return isValid() ? 1 : 0;
}

sal_Bool SAL_CALL SwAccessibleHyperlink::doAccessibleAction( sal_Int32 nIndex ) 
        throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    sal_Bool bRet = sal_False;

    const SwTxtAttr *pTxtAttr = GetTxtAttr();
    if( pTxtAttr && 0 == nIndex )
    {
        const SwFmtINetFmt& rINetFmt = pTxtAttr->GetINetFmt();
        if( rINetFmt.GetValue().Len() )
        {
            ViewShell *pVSh = xPara->GetShell();
            if( pVSh )
            {
                LoadURL( rINetFmt.GetValue(), pVSh, URLLOAD_NOFILTER,
                         &rINetFmt.GetTargetFrame() );
                ASSERT( pTxtAttr == rINetFmt.GetTxtINetFmt(),
                         "lost my txt attr" );
                const SwTxtINetFmt* pTxtAttr2 = rINetFmt.GetTxtINetFmt();
                if( pTxtAttr2 )
                {
                    const_cast< SwTxtINetFmt* >( pTxtAttr2 )->SetVisited( sal_True );
                    const_cast< SwTxtINetFmt* >( pTxtAttr2 )->SetValidVis( sal_True );
                }
                bRet = sal_True;
            }
        }
    }

    return bRet;
}

OUString SAL_CALL SwAccessibleHyperlink::getAccessibleActionDescription(
        sal_Int32 nIndex ) 
        throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    OUString sDesc;

    const SwTxtAttr *pTxtAttr = GetTxtAttr();
    if( pTxtAttr && 0 == nIndex )
    {
        const SwFmtINetFmt& rINetFmt = pTxtAttr->GetINetFmt();
        sDesc = OUString( rINetFmt.GetValue() );
    }

    return sDesc;
}

uno::Reference< XAccessibleKeyBinding > SAL_CALL
    SwAccessibleHyperlink::getAccessibleActionKeyBinding( sal_Int32 nIndex ) 
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    uno::Reference< XAccessibleKeyBinding > xKeyBinding;

    if( isValid() && 0==nIndex )
    {
        ::comphelper::OAccessibleKeyBindingHelper* pKeyBindingHelper =
               new ::comphelper::OAccessibleKeyBindingHelper();
        xKeyBinding = pKeyBindingHelper;

        awt::KeyStroke aKeyStroke;
        aKeyStroke.Modifiers = 0;
        aKeyStroke.KeyCode = KEY_RETURN;
        aKeyStroke.KeyChar = 0;
        aKeyStroke.KeyFunc = 0;
        pKeyBindingHelper->AddKeyBinding( aKeyStroke );
    }

    return xKeyBinding;
}

// XAccessibleHyperlink
uno::Any SAL_CALL SwAccessibleHyperlink::getAccessibleActionAnchor(
        sal_Int32 /*nIndex*/ ) 
        throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    return uno::Any();
}

uno::Any SAL_CALL SwAccessibleHyperlink::getAccessibleActionObject( 
            sal_Int32 /*nIndex*/ ) 
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    return uno::Any();
}

sal_Int32 SAL_CALL SwAccessibleHyperlink::getStartIndex() 
        throw (uno::RuntimeException)
{
    return nStartIdx;
}

sal_Int32 SAL_CALL SwAccessibleHyperlink::getEndIndex() 
        throw (uno::RuntimeException)
{
    return nEndIdx;
}

sal_Bool SAL_CALL SwAccessibleHyperlink::isValid(  ) 
        throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    return xPara.isValid();
}

void SwAccessibleHyperlink::Invalidate()
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    xPara = 0;
}

