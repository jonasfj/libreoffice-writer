 /*************************************************************************
 *
 *  $RCSfile: accmap.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: mib $ $Date: 2002-03-06 08:14:51 $
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

#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _CPPUHELPER_WEAKREF_HXX_
#include <cppuhelper/weakref.hxx>
#endif

#ifndef __SGI_STL_MAP
#include <map>
#endif
#ifndef _ACCMAP_HXX
#include <accmap.hxx>
#endif
#ifndef _ACCCONTEXT_HXX
#include <acccontext.hxx>
#endif
#ifndef _ACCDOC_HXX
#include <accdoc.hxx>
#endif
#ifndef _ACCPARA_HXX
#include <accpara.hxx>
#endif
#ifndef _ACCHEADERFOOTER_HXX
#include <accheaderfooter.hxx>
#endif
#ifndef _ACCFOOTNOTE_HXX
#include <accfootnote.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _HFFRM_HXX
#include <hffrm.hxx>
#endif
#ifndef _FTNFRM_HXX
#include <ftnfrm.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::accessibility;
using namespace ::rtl;

struct SwFrmFunc
{
    sal_Bool operator()( const SwFrm * p1,
                         const SwFrm * p2) const
    {
        return p1 < p2;
    }
};

typedef ::std::map < const SwFrm *, WeakReference < XAccessible >, SwFrmFunc > _SwAccessibleMap_Impl;

class SwAccessibleMap_Impl: public _SwAccessibleMap_Impl
{
};

SwAccessibleMap::SwAccessibleMap( ViewShell *pSh ) :
    pMap( 0  ),
    pVSh( pSh ),
    nPara( 1 ),
    nFootnote( 1 ),
    nEndnote( 1 )
{
}

SwAccessibleMap::~SwAccessibleMap()
{
    vos::OGuard aGuard( aMutex );
    if( pMap )
    {
        Reference < XAccessible > xAcc;
        const SwRootFrm *pRootFrm = GetShell()->GetDoc()->GetRootFrm();
        SwAccessibleMap_Impl::iterator aIter = pMap->find( pRootFrm );
        if( aIter != pMap->end() )
            xAcc = (*aIter).second;
        if( !xAcc.is() )
            xAcc = new SwAccessibleDocument( this );
        SwAccessibleDocument *pAcc =
            static_cast< SwAccessibleDocument * >( xAcc.get() );
        pAcc->Dispose( sal_True );
    }

    delete pMap;
}

Reference< XAccessible > SwAccessibleMap::GetDocumentView()
{
    vos::OGuard aGuard( aMutex );

    Reference < XAccessible > xAcc;

    if( !pMap )
        pMap = new SwAccessibleMap_Impl;
    const SwRootFrm *pRootFrm = GetShell()->GetDoc()->GetRootFrm();
    SwAccessibleMap_Impl::iterator aIter = pMap->find( pRootFrm );
    if( aIter != pMap->end() )
        xAcc = (*aIter).second;
    if( xAcc.is() )
    {
        SwAccessibleDocument *pAcc =
            static_cast< SwAccessibleDocument * >( xAcc.get() );
        pAcc->SetVisArea( GetShell()->VisArea().SVRect() );
    }
    else
    {
        xAcc = new SwAccessibleDocument( this );
        if( aIter != pMap->end() )
        {
            (*aIter).second = xAcc;
        }
        else
        {
            SwAccessibleMap_Impl::value_type aEntry( pRootFrm, xAcc );
            pMap->insert( aEntry );
        }
    }

    return xAcc;
}

Reference< XAccessible> SwAccessibleMap::GetContext( const SwFrm *pFrm,
                                                     sal_Bool bCreate )
{
    vos::OGuard aGuard( aMutex );

    Reference < XAccessible > xAcc;

    if( !pMap && bCreate )
        pMap = new SwAccessibleMap_Impl;
    if( pMap )
    {
        SwAccessibleMap_Impl::iterator aIter = pMap->find( pFrm );
        if( aIter != pMap->end() )
            xAcc = (*aIter).second;

        if( !xAcc.is() && bCreate )
        {
            switch( pFrm->GetType() )
            {
            case FRM_TXT:
                xAcc = new SwAccessibleParagraph( this, nPara++, 
                                static_cast< const SwTxtFrm * >( pFrm ) );
                break;
            case FRM_HEADER:
                xAcc = new SwAccessibleHeaderFooter( this, 
                                static_cast< const SwHeaderFrm *>( pFrm ) );
                break;
            case FRM_FOOTER:
                xAcc = new SwAccessibleHeaderFooter( this, 
                                static_cast< const SwFooterFrm *>( pFrm ) );
                break;
            case FRM_FTN:
                {
                    const SwFtnFrm *pFtnFrm =
                        static_cast < const SwFtnFrm * >( pFrm );
                    sal_Bool bIsEndnote =
                        SwAccessibleFootnote::IsEndnote( pFtnFrm );
                    xAcc = new SwAccessibleFootnote( this, bIsEndnote,
                                (bIsEndnote ? nEndnote++ : nFootnote++),
                                  pFtnFrm );
                }
                break;
            }

            ASSERT( xAcc.is(), "unknown frame type" );
            if( xAcc.is() )
            {
                if( aIter != pMap->end() )
                {
                    (*aIter).second = xAcc;
                }
                else
                {
                    SwAccessibleMap_Impl::value_type aEntry( pFrm, xAcc );
                    pMap->insert( aEntry );
                }
            }
        }
    }

    return xAcc;
}

::vos::ORef < SwAccessibleContext > SwAccessibleMap::GetContextImpl(
            const SwFrm *pFrm,
            sal_Bool bCreate )
{
    Reference < XAccessible > xAcc( GetContext( pFrm, bCreate ) );

    ::vos::ORef < SwAccessibleContext > xAccImpl(
         static_cast< SwAccessibleContext * >( xAcc.get() ) );

    return xAccImpl;
}

void SwAccessibleMap::RemoveContext( SwAccessibleContext *pAcc )
{
    vos::OGuard aGuard( aMutex );

    if( pMap )
    {
        SwAccessibleMap_Impl::iterator aIter = pMap->find( pAcc->GetFrm() );
        if( aIter != pMap->end() )
        {
            pMap->erase( aIter );
            if( pMap->empty() )
            {
                delete pMap;
                pMap = 0;
            }
        }
    }
}

void SwAccessibleMap::DisposeFrm( const SwFrm *pFrm )
{
    if( pFrm->IsAccessibleFrm() )
    {
        vos::OGuard aGuard( aMutex );

        if( pMap )
        {
            SwAccessibleMap_Impl::iterator aIter = pMap->find( pFrm );
            if( aIter != pMap->end() )
            {
                Reference < XAccessible > xAcc = (*aIter).second;
                if( xAcc.is() )
                    static_cast< SwAccessibleContext *>( xAcc.get())->Dispose();
            }
        }
    }
}

void SwAccessibleMap::MoveFrm( const SwFrm *pFrm, const SwRect& rOldFrm )
{
    if( pFrm->IsAccessibleFrm() )
    {
        vos::OGuard aGuard( aMutex );

        if( pMap )
        {
            SwAccessibleMap_Impl::iterator aIter = pMap->find( pFrm );
            if( aIter != pMap->end() )
            {
                // If there is an accesible object already it is
                // notified directly.
                Reference < XAccessible > xAcc = (*aIter).second;
                static_cast< SwAccessibleContext * >(xAcc.get())->PosChanged();
            }
            else
            {
                // Otherwise we look if the parent is accessible.
                // If not, there is nothing to do.
                const SwLayoutFrm *pUpper = pFrm->GetUpper();
                while( pUpper && !pUpper->IsAccessibleFrm() )
                    pUpper = pUpper->GetUpper();

                if( pUpper )
                {
                    aIter = pMap->find( pUpper );
                    if( aIter != pMap->end() )
                    {
                        Reference < XAccessible > xAcc = (*aIter).second;
                        static_cast<SwAccessibleContext *>(xAcc.get())
                            ->ChildPosChanged( pFrm, rOldFrm );
                    }
                }
            }
        }
    }
}
