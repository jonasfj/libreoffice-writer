/*************************************************************************
 *
 *  $RCSfile: anchoreddrawobject.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 15:43:35 $
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
#ifndef _ANCHOREDDRAWOBJECT_HXX
#include <anchoreddrawobject.hxx>
#endif

#ifndef _SVDOBJ_HXX
#include <svx/svdobj.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _TOCNTNTANCHOROBJECTPOSITION_HXX
#include <tocntntanchoredobjectposition.hxx>
#endif
#ifndef _TOLAYOUTANCHOROBJECTPOSITION_HXX
#include <tolayoutanchoredobjectposition.hxx>
#endif
#ifndef _FRMTOOL_HXX
#include <frmtool.hxx>
#endif
#ifndef _FMTORNT_HXX
#include <fmtornt.hxx>
#endif
// --> OD 2004-08-12 #i32795#
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
// <--
// --> OD 2004-08-12 #i32795#
// template class <std::vector>
#include <vector>
// <--

// --> OD 2004-08-10 #i28749#
#ifndef _COM_SUN_STAR_TEXT_POSITIONLAYOUTDIR_HPP_
#include <com/sun/star/text/PositionLayoutDir.hpp>
#endif
// <--

// ============================================================================
// helper class for correct notification due to the positioning of
// the anchored drawing object
// ============================================================================
class SwPosNotify
{
    private:
        SwAnchoredDrawObject* mpAnchoredDrawObj;
        SwRect maOldObjRect;
        SwPageFrm* mpOldPageFrm;

    public:
        SwPosNotify( SwAnchoredDrawObject* _pAnchoredDrawObj );
        ~SwPosNotify();
        // --> OD 2004-08-12 #i32795#
        Point LastObjPos() const;
        // <--
};

SwPosNotify::SwPosNotify( SwAnchoredDrawObject* _pAnchoredDrawObj ) :
    mpAnchoredDrawObj( _pAnchoredDrawObj )
{
    maOldObjRect = mpAnchoredDrawObj->GetObjRect();
    // --> OD 2004-10-20 #i35640# - determine correct page frame
    mpOldPageFrm = mpAnchoredDrawObj->GetPageFrm();
    // <--
}

SwPosNotify::~SwPosNotify()
{
    if ( maOldObjRect != mpAnchoredDrawObj->GetObjRect() )
    {
        if( maOldObjRect.HasArea() && mpOldPageFrm )
        {
            mpAnchoredDrawObj->NotifyBackground( mpOldPageFrm, maOldObjRect,
                                                 PREP_FLY_LEAVE );
        }
        SwRect aNewObjRect( mpAnchoredDrawObj->GetObjRect() );
        if( aNewObjRect.HasArea() )
        {
            // --> OD 2004-10-20 #i35640# - determine correct page frame
            SwPageFrm* pNewPageFrm = mpAnchoredDrawObj->GetPageFrm();
            // <--
            if( pNewPageFrm )
                mpAnchoredDrawObj->NotifyBackground( pNewPageFrm, aNewObjRect,
                                                     PREP_FLY_ARRIVE );
        }

        ::ClrContourCache( mpAnchoredDrawObj->GetDrawObj() );

        // --> OD 2004-10-20 #i35640# - additional notify anchor text frame
        // Needed for negative positioned drawing objects
        if ( mpAnchoredDrawObj->GetAnchorFrm()->IsTxtFrm() )
        {
            mpAnchoredDrawObj->AnchorFrm()->Prepare( PREP_FLY_LEAVE );
        }
        // <--

        // indicate a restart of the layout process
        mpAnchoredDrawObj->SetRestartLayoutProcess( true );
    }
    else
    {
        // lock position
        mpAnchoredDrawObj->LockPosition();

        if ( !mpAnchoredDrawObj->ConsiderForTextWrap() )
        {
            // indicate that object has to be considered for text wrap
            mpAnchoredDrawObj->SetConsiderForTextWrap( true );
            // invalidate 'background' in order to allow its 'background'
            // to wrap around it.
            mpAnchoredDrawObj->NotifyBackground( mpAnchoredDrawObj->GetPageFrm(),
                                    mpAnchoredDrawObj->GetObjRectWithSpaces(),
                                    PREP_FLY_ARRIVE );
            // invalidate position of anchor frame in order to force
            // a re-format of the anchor frame, which also causes a
            // re-format of the invalid previous frames of the anchor frame.
            mpAnchoredDrawObj->AnchorFrm()->InvalidatePos();
        }
    }
}

// --> OD 2004-08-12 #i32795#
Point SwPosNotify::LastObjPos() const
{
    return maOldObjRect.Pos();
}
//<--

// ============================================================================
// OD 2004-08-12 #i32795#
// helper class for oscillation control on object positioning
// ============================================================================
class SwObjPosOscillationControl
{
    private:
        sal_uInt8 mnPosStackSize;

        const SwAnchoredDrawObject* mpAnchoredDrawObj;

        std::vector<Point*> maObjPositions;

    public:
        SwObjPosOscillationControl( const SwAnchoredDrawObject& _rAnchoredDrawObj );
        ~SwObjPosOscillationControl();

        bool OscillationDetected();
};

SwObjPosOscillationControl::SwObjPosOscillationControl(
                                const SwAnchoredDrawObject& _rAnchoredDrawObj )
    : mnPosStackSize( 20 ),
      mpAnchoredDrawObj( &_rAnchoredDrawObj )
{
}

SwObjPosOscillationControl::~SwObjPosOscillationControl()
{
    while ( !maObjPositions.empty() )
    {
        Point* pPos = maObjPositions.back();
        delete pPos;

        maObjPositions.pop_back();
    }
}

bool SwObjPosOscillationControl::OscillationDetected()
{
    bool bOscillationDetected = false;

    if ( maObjPositions.size() == mnPosStackSize )
    {
        // position stack is full -> oscillation
        bOscillationDetected = true;
    }
    else
    {
        Point* pNewObjPos = new Point( mpAnchoredDrawObj->GetObjRect().Pos() );
        for ( std::vector<Point*>::iterator aObjPosIter = maObjPositions.begin();
              aObjPosIter != maObjPositions.end();
              ++aObjPosIter )
        {
            if ( *(pNewObjPos) == *(*aObjPosIter) )
            {
                // position already occured -> oscillation
                bOscillationDetected = true;
                delete pNewObjPos;
                break;
            }
        }
        if ( !bOscillationDetected )
        {
            maObjPositions.push_back( pNewObjPos );
        }
    }

    return bOscillationDetected;
}

// ============================================================================
// implementation of class <SwAnchoredDrawObject>
// ============================================================================
TYPEINIT1(SwAnchoredDrawObject,SwAnchoredObject);

SwAnchoredDrawObject::SwAnchoredDrawObject() :
    SwAnchoredObject(),
    mbValidPos( false ),
    // --> OD 2004-09-29 #i34748#
    mpLastObjRect( 0L ),
    // <--
    mbNotYetAttachedToAnchorFrame( true ),
    // --> OD 2004-08-09 #i28749#
    mbNotYetPositioned( true )
    // <--
{
}

SwAnchoredDrawObject::~SwAnchoredDrawObject()
{
    // --> OD 2004-11-03 - follow-up of #i34748#
    delete mpLastObjRect;
    // <--
}

// =============================================================================
// OD 2004-03-25 #i26791# - implementation of pure virtual method declared in
// base class <SwAnchoredObject>
// =============================================================================
void SwAnchoredDrawObject::MakeObjPos()
{
    if ( IsPositioningInProgress() )
    {
        // nothind to do - positioning already in progress
        return;
    }

    if ( mbValidPos )
    {
        // nothing to do - position is valid
        return;
    }

    // --> OD 2004-08-09 #i28749# - anchored drawing object has to be attached
    // to anchor frame
    if ( NotYetAttachedToAnchorFrm() )
    {
        ASSERT( false,
                "<SwAnchoredDrawObject::MakeObjPos() - drawing object not yet attached to anchor frame -> no positioning" );
        return;
    }

    // --> OD 2004-08-09 #i28749# - if anchored drawing object hasn't been yet
    // positioned, convert its positioning attributes, if its positioning
    // attributes are given in horizontal left-to-right layout.
    // --> OD 2004-10-25 #i36010# - Note: horizontal left-to-right layout is made
    // the default layout direction for <SwDrawFrmFmt> instances. Thus, it has
    // to be adjusted manually, if no adjustment of the positioning attributes
    // have to be performed here.
    if ( mbNotYetPositioned )
    {
        mbNotYetPositioned = false;
        // --> OD 2004-09-29 #117975# - perform conversion of positioning
        // attributes only for 'master' drawing objects
        if ( !GetDrawObj()->ISA(SwDrawVirtObj) &&
             GetFrmFmt().GetPositionLayoutDir() ==
                    com::sun::star::text::PositionLayoutDir::PositionInHoriL2R )
        // <--
        {
            // --> OD 2004-10-19 #i35798# - set positioning attributes
            _SetPositioningAttr();
            // <--
        }
    }
    // <--

    // indicate that positioning is in progress
    SwObjPositioningInProgress aObjPosInProgress( *this );

    SwDrawContact* pDrawContact =
                        static_cast<SwDrawContact*>(::GetUserCall( GetDrawObj() ));

    // determine relative position of drawing object and set it
    switch ( pDrawContact->GetAnchorId() )
    {
        case FLY_IN_CNTNT:
        {
            // indicate that position will be valid after positioning is performed
            mbValidPos = true;
            // nothing to do, because as-character anchored objects are positioned
            // during the format of its anchor frame - see <SwFlyCntPortion::SetBase(..)>
        }
        break;
        case FLY_AT_CNTNT:
        case FLY_AUTO_CNTNT:
        {
            // --> OD 2004-08-12 #i32795# - move intrinsic positioning to
            // helper method <_MakeObjPosAnchoredAtPara()>
            _MakeObjPosAnchoredAtPara();
        }
        break;
        case FLY_PAGE:
        case FLY_AT_FLY:
        {
            // --> OD 2004-08-12 #i32795# - move intrinsic positioning to
            // helper method <_MakeObjPosAnchoredAtLayout()>
            _MakeObjPosAnchoredAtLayout();
        }
        break;
        default:
        {
            ASSERT( false, "<SwAnchoredDrawObject::MakeObjPos()> - unknown anchor type - please inform OD." );
        }
    }

    // keep, current object rectangle
    // --> OD 2004-09-29 #i34748# - use new method <SetLastObjRect(..)>
    SetLastObjRect( GetObjRect().SVRect() );
    // <--

    // Assure for 'master' drawing object, that it's registered at the correct page.
    // Perform check not for as-character anchored drawing objects and only if
    // the anchor frame is valid.
    if ( !GetDrawObj()->ISA(SwDrawVirtObj) &&
         !pDrawContact->ObjAnchoredAsChar() &&
         GetAnchorFrm()->IsValid() )
    {
        pDrawContact->ChkPage();
    }
}

/** method for the intrinsic positioning of a at-paragraph|at-character
    anchored drawing object

    OD 2004-08-12 #i32795# - helper method for method <MakeObjPos>

    @author OD
*/
void SwAnchoredDrawObject::_MakeObjPosAnchoredAtPara()
{
    // --> OD 2004-08-12 #i32795# - adopt positioning algorithm from Writer
    // fly frames, which are anchored at paragraph|at character

    // Determine, if anchor frame can/has to be formatted.
    // If yes, after each object positioning the anchor frame is formatted.
    // If after the anchor frame format the object position isn't valid, the
    // object is positioned again.
    const bool bFormatAnchor =
            !static_cast<const SwTxtFrm*>( GetAnchorFrm() )->IsAnyJoinLocked() &&
            !ConsiderObjWrapInfluenceOnObjPos();

    if ( bFormatAnchor )
    {
        GetAnchorFrm()->Calc();
    }

    bool bOscillationDetected = false;
    SwObjPosOscillationControl aObjPosOscCtrl( *this );
    // --> OD 2004-08-25 #i3317# - boolean, to apply temporarly the
    // 'straightforward positioning process' for the frame due to its
    // overlapping with a previous column.
    bool bConsiderWrapInfluenceDueToOverlapPrevCol( false );
    // <--
    do {
        // indicate that position will be valid after positioning is performed
        mbValidPos = true;

        // --> OD 2004-10-20 #i35640# - correct scope for <SwPosNotify> instance
        {
            // create instance of <SwPosNotify> for correct notification
            SwPosNotify aPosNotify( this );

            // determine and set position
            objectpositioning::SwToCntntAnchoredObjectPosition
                    aObjPositioning( *DrawObj() );
            aObjPositioning.CalcPosition();

            // get further needed results of the positioning algorithm
            SetVertPosOrientFrm ( aObjPositioning.GetVertPosOrientFrm() );
            _SetDrawObjAnchor();

            // check for object position oscillation, if position has changed.
            if ( GetObjRect().Pos() != aPosNotify.LastObjPos() )
            {
                bOscillationDetected = aObjPosOscCtrl.OscillationDetected();
            }
        }
        // <--
        // format anchor frame, if requested.
        // Note: the format of the anchor frame can cause the object position
        // to be invalid.
        if ( bFormatAnchor )
        {
            GetAnchorFrm()->Calc();
        }

        // --> OD 2004-08-25 #i3317#
        if ( !ConsiderObjWrapInfluenceOnObjPos() &&
             OverlapsPrevColumn() )
        {
            bConsiderWrapInfluenceDueToOverlapPrevCol = true;
        }
        // <--
    } while ( !mbValidPos && !bOscillationDetected &&
              !bConsiderWrapInfluenceDueToOverlapPrevCol );

    // --> OD 2004-08-25 #i3317# - consider a detected oscillation and overlapping
    // with previous column.
    // temporarly consider the anchored objects wrapping style influence
    if ( bOscillationDetected || bConsiderWrapInfluenceDueToOverlapPrevCol )
    {
        SetTmpConsiderWrapInfluence( true );
        SetRestartLayoutProcess( true );
    }
    // <--
}

/** method for the intrinsic positioning of a at-page|at-frame anchored
    drawing object

    OD 2004-08-12 #i32795# - helper method for method <MakeObjPos>

    @author OD
*/
void SwAnchoredDrawObject::_MakeObjPosAnchoredAtLayout()
{
    // indicate that position will be valid after positioning is performed
    mbValidPos = true;

    // create instance of <SwPosNotify> for correct notification
    SwPosNotify aPosNotify( this );

    // determine position
    objectpositioning::SwToLayoutAnchoredObjectPosition
            aObjPositioning( *DrawObj() );
    aObjPositioning.CalcPosition();

    // set position

    // --> OD 2004-07-29 #i31698#
    // --> OD 2004-10-18 #i34995# - setting anchor position needed for filters,
    // especially for the xml-filter to the OpenOffice.org file format
    {
        const Point aNewAnchorPos =
                    GetAnchorFrm()->GetFrmAnchorPos( ::HasWrap( GetDrawObj() ) );
        DrawObj()->SetAnchorPos( aNewAnchorPos );
    }
    // <--
    SetCurrRelPos( aObjPositioning.GetRelPos() );
    const SwFrm* pAnchorFrm = GetAnchorFrm();
    SWRECTFN( pAnchorFrm );
    const Point aAnchPos( (pAnchorFrm->Frm().*fnRect->fnGetPos)() );
    SetObjLeft( aAnchPos.X() + GetCurrRelPos().X() );
    SetObjTop( aAnchPos.Y() + GetCurrRelPos().Y() );
    // <--
}

void SwAnchoredDrawObject::_SetDrawObjAnchor()
{
    // new anchor position
    // --> OD 2004-07-29 #i31698# -
    Point aNewAnchorPos =
                GetAnchorFrm()->GetFrmAnchorPos( ::HasWrap( GetDrawObj() ) );
    Point aCurrAnchorPos = GetDrawObj()->GetAnchorPos();
    if ( aNewAnchorPos != aCurrAnchorPos )
    {
        // determine movement to be applied after setting the new anchor position
        Size aMove( aCurrAnchorPos.X() - aNewAnchorPos.X(),
                    aCurrAnchorPos.Y() - aNewAnchorPos.Y() );
        // set new anchor position
        DrawObj()->SetAnchorPos( aNewAnchorPos );
        // correct object position, caused by setting new anchor position
        DrawObj()->Move( aMove );
    }
}

/** method to invalidate the given page frame

    OD 2004-07-02 #i28701#

    @author OD
*/
void SwAnchoredDrawObject::_InvalidatePage( SwPageFrm* _pPageFrm )
{
    if ( _pPageFrm && !_pPageFrm->GetFmt()->GetDoc()->IsInDtor() )
    {
        if ( _pPageFrm->GetUpper() )
        {
            _pPageFrm->InvalidateFlyLayout();

            SwRootFrm* pRootFrm = static_cast<SwRootFrm*>(_pPageFrm->GetUpper());
            pRootFrm->DisallowTurbo();
            if ( pRootFrm->GetTurbo() )
            {
                const SwCntntFrm* pTmpFrm = pRootFrm->GetTurbo();
                pRootFrm->ResetTurbo();
                pTmpFrm->InvalidatePage();
            }
            pRootFrm->SetIdleFlags();
        }
    }
}

void SwAnchoredDrawObject::InvalidateObjPos()
{
    // --> OD 2004-07-01 #i28701# - check, if invalidation is allowed
    if ( mbValidPos &&
         InvalidationOfPosAllowed() )
    {
        mbValidPos = false;

        SwPageFrm* pPageFrm = AnchorFrm()->FindPageFrm();
        _InvalidatePage( pPageFrm );
        // --> OD 2004-08-12 #i32270# - also invalidate page frame, at which the
        // drawing object is registered at.
        SwPageFrm* pPageFrmRegisteredAt = GetPageFrm();
        if ( pPageFrmRegisteredAt &&
             pPageFrmRegisteredAt != pPageFrm )
        {
            _InvalidatePage( pPageFrmRegisteredAt );
        }
        // <--
        // --> OD 2004-09-23 #i33751#, #i34060# - method <GetPageFrmOfAnchor()>
        // is replaced by method <FindPageFrmOfAnchor()>. It's return value
        // have to be checked.
        SwPageFrm* pPageFrmOfAnchor = FindPageFrmOfAnchor();
        if ( pPageFrmOfAnchor &&
             pPageFrmOfAnchor != pPageFrm &&
             pPageFrmOfAnchor != pPageFrmRegisteredAt )
        // <--
        {
            _InvalidatePage( pPageFrmOfAnchor );
        }
    }
}

SwFrmFmt& SwAnchoredDrawObject::GetFrmFmt()
{
    ASSERT( static_cast<SwDrawContact*>(GetUserCall(GetDrawObj()))->GetFmt(),
            "<SwAnchoredDrawObject::GetFrmFmt()> - missing frame format -> crash." );
    return *(static_cast<SwDrawContact*>(GetUserCall(GetDrawObj()))->GetFmt());
}
const SwFrmFmt& SwAnchoredDrawObject::GetFrmFmt() const
{
    ASSERT( static_cast<SwDrawContact*>(GetUserCall(GetDrawObj()))->GetFmt(),
            "<SwAnchoredDrawObject::GetFrmFmt()> - missing frame format -> crash." );
    return *(static_cast<SwDrawContact*>(GetUserCall(GetDrawObj()))->GetFmt());
}

const SwRect SwAnchoredDrawObject::GetObjRect() const
{
    // use geometry of drawing object
    //return GetDrawObj()->GetCurrentBoundRect();
    return GetDrawObj()->GetSnapRect();
}
void SwAnchoredDrawObject::SetObjTop( const SwTwips _nTop )
{
    SwTwips nDiff = _nTop - GetObjRect().Top();
    DrawObj()->Move( Size( 0, nDiff ) );
}
void SwAnchoredDrawObject::SetObjLeft( const SwTwips _nLeft )
{
    SwTwips nDiff = _nLeft - GetObjRect().Left();
    DrawObj()->Move( Size( nDiff, 0 ) );
}

/** adjust positioning and alignment attributes for new anchor frame

    OD 2004-08-24 #i33313# - add second optional parameter <_pNewObjRect>

    @author OD
*/
void SwAnchoredDrawObject::AdjustPositioningAttr( const SwFrm* _pNewAnchorFrm,
                                                  const SwRect* _pNewObjRect )
{
    SwTwips nHoriRelPos = 0;
    SwTwips nVertRelPos = 0;
    const Point aAnchorPos = _pNewAnchorFrm->GetFrmAnchorPos( ::HasWrap( GetDrawObj() ) );
    // --> OD 2004-08-24 #i33313#
    const SwRect aObjRect( _pNewObjRect ? *_pNewObjRect : GetObjRect() );
    // <--
    const bool bVert = _pNewAnchorFrm->IsVertical();
    const bool bR2L = _pNewAnchorFrm->IsRightToLeft();
    if ( bVert )
    {
        nHoriRelPos = aObjRect.Top() - aAnchorPos.Y();
        nVertRelPos = aAnchorPos.X() - aObjRect.Right();
    }
    else if ( bR2L )
    {
        nHoriRelPos = aAnchorPos.X() - aObjRect.Right();
        nVertRelPos = aObjRect.Top() - aAnchorPos.Y();
    }
    else
    {
        nHoriRelPos = aObjRect.Left() - aAnchorPos.X();
        nVertRelPos = aObjRect.Top() - aAnchorPos.Y();
    }

    GetFrmFmt().SetAttr( SwFmtHoriOrient( nHoriRelPos, HORI_NONE, FRAME ) );
    GetFrmFmt().SetAttr( SwFmtVertOrient( nVertRelPos, VERT_NONE, FRAME ) );
}

// --> OD 2004-09-29 #i34748# - change return type
const Rectangle* SwAnchoredDrawObject::GetLastObjRect() const
{
    return mpLastObjRect;
}
// <--

// --> OD 2004-09-29 #i34748# - change return type.
// If member <mpLastObjRect> is NULL, create one.
void SwAnchoredDrawObject::SetLastObjRect( const Rectangle& _rNewLastRect )
{
    if ( !mpLastObjRect )
    {
        mpLastObjRect = new Rectangle;
    }
    *(mpLastObjRect) = _rNewLastRect;
}
// <--

void SwAnchoredDrawObject::ObjectAttachedToAnchorFrame()
{
    // --> OD 2004-07-27 #i31698#
    SwAnchoredObject::ObjectAttachedToAnchorFrame();
    // <--

    if ( mbNotYetAttachedToAnchorFrame )
    {
        mbNotYetAttachedToAnchorFrame = false;
    }
}

/** method to set positioning attributes (not for as-character anchored)

    OD 2004-10-20 #i35798#
    During load the positioning attributes aren't set.
    Thus, the positioning attributes are set by the current object geometry.
    This method is also used for the conversion for drawing objects
    (not anchored as-character) imported from OpenOffice.org file format
    once and directly before the first positioning.

    @author OD
*/
void SwAnchoredDrawObject::_SetPositioningAttr()
{
    SwDrawContact* pDrawContact =
                        static_cast<SwDrawContact*>(GetUserCall( GetDrawObj() ));

    if ( !pDrawContact->ObjAnchoredAsChar() )
    {
        SwRect aObjRect( GetObjRect() );

        SwTwips nHoriPos = aObjRect.Left();
        SwTwips nVertPos = aObjRect.Top();
        SwFrmFmt::tLayoutDir eLayoutDir = GetFrmFmt().GetLayoutDir();
        switch ( eLayoutDir )
        {
            case SwFrmFmt::HORI_L2R:
            {
                // nothing to do
            }
            break;
            case SwFrmFmt::HORI_R2L:
            {
                nHoriPos = -aObjRect.Left() - aObjRect.Width();
            }
            break;
            case SwFrmFmt::VERT_R2L:
            {
                nHoriPos = aObjRect.Top();
                nVertPos = -aObjRect.Left() - aObjRect.Width();
            }
            break;
            default:
            {
                ASSERT( false,
                        "<SwAnchoredDrawObject::_SetPositioningAttr()> - unsupported layout direction" );
            }
        }

        const SwFmtHoriOrient& rHori = GetFrmFmt().GetHoriOrient();
        GetFrmFmt().SetAttr( SwFmtHoriOrient( nHoriPos,
                                              rHori.GetHoriOrient(),
                                              rHori.GetRelationOrient() ) );

        const SwFmtVertOrient& rVert = GetFrmFmt().GetVertOrient();
        if ( rVert.GetRelationOrient() == REL_CHAR ||
             rVert.GetRelationOrient() == REL_VERT_LINE )
        {
            nVertPos = -nVertPos;
        }
        GetFrmFmt().SetAttr( SwFmtVertOrient( nVertPos,
                                              rVert.GetVertOrient(),
                                              rVert.GetRelationOrient() ) );

        // --> OD 2004-10-25 #i36010# - set layout direction of the position
        GetFrmFmt().SetPositionLayoutDir(
            com::sun::star::text::PositionLayoutDir::PositionInLayoutDirOfAnchor );
        // <--
    }
}

void SwAnchoredDrawObject::NotifyBackground( SwPageFrm* _pPageFrm,
                                             const SwRect& _rRect,
                                             PrepareHint _eHint )
{
    ::Notify_Background( GetDrawObj(), _pPageFrm, _rRect, _eHint, TRUE );
}

/** method to assure that anchored object is registered at the correct
    page frame

    OD 2004-07-02 #i28701#

    @author OD
*/
void SwAnchoredDrawObject::RegisterAtCorrectPage()
{
    SwPageFrm* pPageFrm( 0L );
    if ( GetVertPosOrientFrm() )
    {
        pPageFrm = const_cast<SwPageFrm*>(GetVertPosOrientFrm()->FindPageFrm());
    }
    if ( pPageFrm && GetPageFrm() != pPageFrm )
    {
        if ( GetPageFrm() )
            GetPageFrm()->RemoveDrawObjFromPage( *this );
        pPageFrm->AppendDrawObjToPage( *this );
    }
}

// =============================================================================
