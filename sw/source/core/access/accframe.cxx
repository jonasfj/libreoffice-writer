 /*************************************************************************
 *
 *  $RCSfile: accframe.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mib $ $Date: 2002-02-05 15:52:06 $
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

#include <hintids.hxx>
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _FLYFRM_HXX
#include <flyfrm.hxx>
#endif

#ifndef _LAYFRM_HXX
#include <layfrm.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _SECTFRM_HXX
#include <sectfrm.hxx>
#endif
#ifndef _FRMSH_HXX
#include <frmsh.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif

#ifndef _ACCFRAME_HXX
#include <accframe.hxx>
#endif

sal_Bool SwAccessibleFrame::IsAccessible( const SwFrm *pFrm )
{
    return pFrm->IsTxtFrm() || pFrm->IsRootFrm();
}

sal_Int32 SwAccessibleFrame::GetLowerCount( const Rectangle& rVisArea,
                                            const SwFrm *pFrm )
{
    sal_Int32 nCount = 0;
    const SwFrm *pLower = pFrm->GetLower();
    while( pLower )
    {
        if( pLower->PaintArea().IsOver( rVisArea ) )
        {
            if( IsAccessible( pLower ) )
                nCount++;
            else
                nCount += GetLowerCount( rVisArea, pLower );
        }
        pLower = pLower->GetNext();
    }

    return nCount;
}

const SwFrm *SwAccessibleFrame::GetDescendant( const Rectangle& rVisArea,
                                         const SwFrm *pFrm,
                                         sal_Int32& rPos )
{
    const SwFrm *pDesc = 0;

    if( rPos >= 0 )
    {
        const SwFrm *pLower = pFrm->GetLower();
        while( pLower && !pDesc )
        {
            if( pLower->PaintArea().IsOver( rVisArea ) )
            {
                if( IsAccessible( pLower ) )
                {
                    if( 0 == rPos )
                        pDesc = pLower;
                    else
                        rPos--;
                }
                else
                {
                    pDesc = GetDescendant( rVisArea, pLower, rPos );
                }
            }
            pLower = pLower->GetNext();
        }
    }

    return pDesc;
}

sal_Bool SwAccessibleFrame::GetDescendantIndex( const Rectangle& rVisArea,
                                                 const SwFrm *pFrm,
                                                  const SwFrm *pChild,
                                                 sal_Int32& rPos )
{
    const SwFrm *pLower = pFrm->GetLower();
    sal_Bool bFound = sal_False;
    while( pLower && !bFound )
    {
        if( pLower->PaintArea().IsOver( rVisArea ) )
        {
            if( IsAccessible( pLower ) )
            {
                if( pLower == pChild )
                    bFound = sal_True;
                else
                    rPos++;
            }
            else
            {
                bFound = GetDescendantIndex( rVisArea, pLower, pChild, rPos );
            }
        }
        pLower = pLower->GetNext();
    }

    return bFound;
}

const SwFrm *SwAccessibleFrame::GetDescendantAt(
                                        const Rectangle& rVisArea,
                                         const SwFrm *pFrm,
                                        const Point& rPos )
{
    const SwFrm *pLower = pFrm->GetLower();
    const SwFrm *pDesc = 0;
    while( pLower && !pDesc )
    {
        SwRect aPaintArea( pLower->PaintArea() );
        if( aPaintArea.IsOver( rVisArea ) )
        {
            if( IsAccessible( pLower ) )
            {
                if( aPaintArea.IsInside( rPos ) )
                    pDesc = pLower;
            }
            else
            {
                pDesc = GetDescendantAt( rVisArea, pLower, rPos );
            }
        }
        pLower = pLower->GetNext();
    }

    return pDesc;
}

void SwAccessibleFrame::SetVisArea( SwAccessibleFrame *pAcc,
                                    const SwFrm *pFrm,
                                    const Rectangle& rOldVisArea )
{
    const SwFrm *pLower = pFrm->GetLower();
    while( pLower )
    {
        SwRect aPaintArea( pLower->PaintArea() );
        if( IsAccessible( pLower ) )
        {
            if( aPaintArea.IsOver( pAcc->GetVisArea() ) )
            {
                if( aPaintArea.IsOver( rOldVisArea ) )
                    pAcc->LowerMoved( pLower );
                else
                    pAcc->LowerAdded( pLower );
            }
            else if( aPaintArea.IsOver( rOldVisArea ) )
            {
                pAcc->LowerRemoved( pLower );
            }
        }
        else if( aPaintArea.IsOver( rOldVisArea ) ||
                 aPaintArea.IsOver( pAcc->GetVisArea() ) )
        {
            SetVisArea( pAcc, pLower, rOldVisArea );
        }
        pLower = pLower->GetNext();
    }
}

void SwAccessibleFrame::LowerAdded( const SwFrm *pFrm )
{
}

void SwAccessibleFrame::LowerRemoved( const SwFrm *pFrm )
{
}

void SwAccessibleFrame::LowerMoved( const SwFrm *pFrm )
{
}

Rectangle SwAccessibleFrame::GetBounds( const SwFrm *pFrm )
{
    if( !pFrm )
        pFrm = GetFrm();
    SwRect aPaintArea( pFrm->PaintArea() );
    Rectangle aBounds( aPaintArea.SVRect().Intersection( aVisArea ) );
    return aBounds;
}

Window *SwAccessibleFrame::GetWindow()
{
    Window *pWin = 0;

    if( pFrm )
    {
        const ViewShell *pVSh = pFrm->GetShell();
        ASSERT( pVSh, "no view shell" );
        if( pVSh )
            pWin = pVSh->GetWin();

        ASSERT( pWin, "no window" );
    }

    return pWin;
}

sal_Bool SwAccessibleFrame::IsEditable() const
{
    const SwFrm *pFrm = GetFrm();
    if( !pFrm )
        return sal_False;

    if( !pFrm->IsRootFrm() && pFrm->IsProtected() )
        return sal_False;

    ViewShell *pVSh = pFrm->GetShell();
    ASSERT( pVSh, "no view shell" );
    if( pVSh )
    {
        const SwDoc *pDoc = pVSh->GetDoc();
        const SwDocShell *pDocSh = pDoc->GetDocShell();
        ASSERT( pDocSh, "no doc shell" );
        if( pDocSh && !pDocSh->IsReadOnly() )
            return sal_True;
    }

    return sal_False;
}

sal_Bool SwAccessibleFrame::IsOpaque() const
{
    const SwFrm *pFrm = GetFrm();
    if( !pFrm )
        return sal_False;

    ViewShell *pVSh = pFrm->GetShell();
    ASSERT( pVSh, "no view shell" );
    if( !pVSh )
        return sal_False;

    const SwViewOption *pVOpt = pVSh->GetViewOptions();
    do
    {	
        if( pFrm->IsRootFrm() )
            return sal_True;

        if( pFrm->IsPageFrm() && !pVOpt->IsPageBack() )
            return sal_False;

        const SvxBrushItem &rBack = pFrm->GetAttrSet()->GetBackground();
        if( !rBack.GetColor().GetTransparency() ||
             rBack.GetGraphicPos() != GPOS_NONE )
            return sal_True;

        if( pFrm->IsSctFrm() )
        {
            const SwSection* pSection = ((SwSectionFrm*)pFrm)->GetSection();
            if( pSection && ( TOX_HEADER_SECTION == pSection->GetType() ||
                TOX_CONTENT_SECTION == pSection->GetType() ) &&
                pVOpt->IsIndexBackground() )
                return sal_True;
        }
        if ( pFrm->IsFlyFrm() )
            pFrm = ((SwFlyFrm*)pFrm)->GetAnchor();
        else
            pFrm = pFrm->GetUpper();
    } while( pFrm && !IsAccessible( pFrm ) );

    return sal_False;
}

SwAccessibleFrame::SwAccessibleFrame( const Rectangle& rVisArea,
                                      const SwFrm *pF ) :
    aVisArea( rVisArea ),
    pFrm( pF )
{
}

SwAccessibleFrame::~SwAccessibleFrame()
{
}

const SwFrm *SwAccessibleFrame::GetUpper() const
{
    const SwLayoutFrm *pUpper = pFrm->GetUpper();
    while( pUpper && !IsAccessible( pUpper ) )
        pUpper = pUpper->GetUpper();

    return pUpper;
}

void SwAccessibleFrame::SetVisArea( const Rectangle& rNewVisArea )
{
    Rectangle aOldVisArea( aVisArea );
    aVisArea = rNewVisArea;

    SetVisArea( this, GetFrm(), aOldVisArea );
}

