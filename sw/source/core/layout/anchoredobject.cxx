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
#include <anchoredobject.hxx>
#include <pam.hxx>
#include <frame.hxx>
#include <txtfrm.hxx>
#include <frmfmt.hxx>
#include <fmtanchr.hxx>
#include <fmtornt.hxx>
// --> OD 2004-06-29 #i28701#
#include <doc.hxx>
#include <fmtsrnd.hxx>
#include <svx/svdobj.hxx>
#include <dcontact.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/lrspitem.hxx>
#include <sortedobjs.hxx>
#include <pagefrm.hxx>
// <--
#include <frmatr.hxx>
// --> OD 2004-08-25 #i3317#
#include <colfrm.hxx>
// <--
// --> OD 2004-10-22 #i35911#
#include <layouter.hxx>


using namespace ::com::sun::star;


// <--
// ============================================================================
// --> OD 2004-06-30 #i28701# -
// implementation of helper class <SwObjPositioningInProgress>
// ============================================================================
SwObjPositioningInProgress::SwObjPositioningInProgress( SdrObject& _rSdrObj ) :
    mpAnchoredObj( 0L ),
    // --> OD 2005-08-09 #i52904#
    mbOldObjPositioningInProgress( false )
    // <--
{
    mpAnchoredObj = ::GetUserCall( &_rSdrObj )->GetAnchoredObj( &_rSdrObj );
    // --> OD 2005-08-09 #i52904#
    mbOldObjPositioningInProgress = mpAnchoredObj->IsPositioningInProgress();
    // <--
    mpAnchoredObj->SetPositioningInProgress( true );
}
SwObjPositioningInProgress::SwObjPositioningInProgress( SwAnchoredObject& _rAnchoredObj ) :
    mpAnchoredObj( &_rAnchoredObj ),
    // --> OD 2005-08-09 #i52904#
    mbOldObjPositioningInProgress( false )
    // <--
{
    // --> OD 2005-08-09 #i52904#
    mbOldObjPositioningInProgress = mpAnchoredObj->IsPositioningInProgress();
    // <--
    mpAnchoredObj->SetPositioningInProgress( true );
}

SwObjPositioningInProgress::~SwObjPositioningInProgress()
{
    if ( mpAnchoredObj )
    {
        // --> OD 2005-08-09 #i52904#
        mpAnchoredObj->SetPositioningInProgress( mbOldObjPositioningInProgress );
        // <--
    }
}

// ============================================================================

TYPEINIT0(SwAnchoredObject);

SwAnchoredObject::SwAnchoredObject() :
    mpDrawObj( 0L ),
    mpAnchorFrm( 0L ),
    // --> OD 2004-06-30 #i28701#
    mpPageFrm( 0L ),
    // <--
    maRelPos(),
    maLastCharRect(),
    mnLastTopOfLine( 0L ),
    mpVertPosOrientFrm( 0L ),
    // --> OD 2004-06-29 #i28701#
    mbPositioningInProgress( false ),
    mbConsiderForTextWrap( false ),
    mbPositionLocked( false ),
    // --> OD 2005-01-10 #i40147#
    mbKeepPositionLockedForSection( false ),
    // <--
    mbRestartLayoutProcess( false ),
    // <--
    // --> OD 2004-10-22 #i35911#
    mbClearedEnvironment( false ),
    // <--
    // --> OD 2004-08-25 #i3317#
    mbTmpConsiderWrapInfluence( false ),
    // <--
    // --> OD 2006-08-10 #i68520#
    maObjRectWithSpaces(),
    mbObjRectWithSpacesValid( false ),
    maLastObjRect()
    // <--
{
}

SwAnchoredObject::~SwAnchoredObject()
{
}

// =============================================================================
// accessors for member <mpDrawObj>
// =============================================================================
void SwAnchoredObject::SetDrawObj( SdrObject& _rDrawObj )
{
    mpDrawObj = &_rDrawObj;
}

const SdrObject* SwAnchoredObject::GetDrawObj() const
{
    return mpDrawObj;
}

SdrObject* SwAnchoredObject::DrawObj()
{
    return mpDrawObj;
}

// =============================================================================
// accessors for member <mpAnchorFrm>
// =============================================================================
const SwFrm* SwAnchoredObject::GetAnchorFrm() const
{
    return mpAnchorFrm;
}

SwFrm* SwAnchoredObject::AnchorFrm()
{
    return mpAnchorFrm;
}

void SwAnchoredObject::ChgAnchorFrm( SwFrm* _pNewAnchorFrm )
{
    mpAnchorFrm = _pNewAnchorFrm;

    if ( mpAnchorFrm )
    {
        ObjectAttachedToAnchorFrame();
    }
}

/** determine anchor frame containing the anchor position

    OD 2004-10-08 #i26945#
    the anchor frame, which is determined, is <mpAnchorFrm>
    for an at-page, at-frame or at-paragraph anchored object
    and the anchor character frame for an at-character and as-character
    anchored object.

    @author OD
*/
SwFrm* SwAnchoredObject::GetAnchorFrmContainingAnchPos()
{
    SwFrm* pAnchorFrmContainingAnchPos = FindAnchorCharFrm();
    if ( !pAnchorFrmContainingAnchPos )
    {
        pAnchorFrmContainingAnchPos = AnchorFrm();
    }

    return pAnchorFrmContainingAnchPos;
}

// =============================================================================
// OD 2004-06-30 #i28701# accessors for member <mpPageFrm>
// =============================================================================
SwPageFrm* SwAnchoredObject::GetPageFrm()
{
    return mpPageFrm;
}

const SwPageFrm* SwAnchoredObject::GetPageFrm() const
{
    return mpPageFrm;
}

void SwAnchoredObject::SetPageFrm( SwPageFrm* _pNewPageFrm )
{
    // --> OD 2006-01-02 #125977#
    if ( mpPageFrm != _pNewPageFrm )
    {
        // clear member, which denotes the layout frame at which the vertical
        // position is oriented at, if it doesn't fit to the new page frame.
        if ( GetVertPosOrientFrm() &&
             ( !_pNewPageFrm ||
               _pNewPageFrm != GetVertPosOrientFrm()->FindPageFrm() ) )
        {
            ClearVertPosOrientFrm();
        }

        // assign new page frame
        mpPageFrm = _pNewPageFrm;
    }
    // <--
}

// =============================================================================
// accessors for member <maLastCharRect>
// =============================================================================
const SwRect& SwAnchoredObject::GetLastCharRect() const
{
    return maLastCharRect;
}

SwTwips SwAnchoredObject::GetRelCharX( const SwFrm* pFrm ) const
{
    return maLastCharRect.Left() - pFrm->Frm().Left();
}

SwTwips SwAnchoredObject::GetRelCharY( const SwFrm* pFrm ) const
{
    return maLastCharRect.Bottom() - pFrm->Frm().Top();
}

void SwAnchoredObject::AddLastCharY( long nDiff )
{
    maLastCharRect.Pos().Y() += nDiff;
}

void SwAnchoredObject::ResetLastCharRectHeight()
{
    maLastCharRect.Height( 0 );
}
// =============================================================================
// accessors for member <mpVertPosOrientFrm>
// =============================================================================
void SwAnchoredObject::SetVertPosOrientFrm( const SwLayoutFrm& _rVertPosOrientFrm )
{
    mpVertPosOrientFrm = &_rVertPosOrientFrm;

    // --> OD 2004-07-02 #i28701# - take over functionality of deleted method
    // <SwFlyAtCntFrm::AssertPage()>: assure for at-paragraph and at-character
    // an anchored object, that it is registered at the correct page frame
    RegisterAtCorrectPage();
}

// =============================================================================
// accessors for member <mnLastTopOfLine>
// =============================================================================
SwTwips SwAnchoredObject::GetLastTopOfLine() const
{
    return mnLastTopOfLine;
}

// OD 2004-05-18 #i28701# - follow-up of #i22341#
void SwAnchoredObject::AddLastTopOfLineY( SwTwips _nDiff )
{
    mnLastTopOfLine += _nDiff;
}

/** check anchor character rectangle and top of line

    OD 2004-03-24 #i26791
    For to-character anchored Writer fly frames the members <maLastCharRect>
    and <maLastTopOfLine> are updated. These are checked for change and
    depending on the applied positioning, it's decided, if the Writer fly
    frame has to be invalidated.
    OD 2004-07-15 #117380#
    add parameter <_bCheckForParaPorInf>, default value <true>

    @author OD
*/
void SwAnchoredObject::CheckCharRectAndTopOfLine(
                                        const bool _bCheckForParaPorInf )
{
    if ( GetAnchorFrm() &&
         GetAnchorFrm()->IsTxtFrm() )
    {
        const SwFmtAnchor& rAnch = GetFrmFmt().GetAnchor();
        if ( (rAnch.GetAnchorId() == FLY_AT_CHAR) &&
             rAnch.GetCntntAnchor() )
        {
            // --> OD 2004-07-14 #117380# - if requested, assure that anchor frame,
            // which contains the anchor character, has a paragraph portion information.
            // The paragraph portion information is needed to determine the
            // anchor character rectangle respectively the top of the line.
            // Thus, a format of this frame is avoided to determine the
            // paragraph portion information.
            // --> OD 2004-10-04 #i26945# - use new method <FindAnchorCharFrm()>
            const SwTxtFrm& aAnchorCharFrm = *(FindAnchorCharFrm());
            // <--
            if ( !_bCheckForParaPorInf || aAnchorCharFrm.HasPara() )
            {
                _CheckCharRect( rAnch, aAnchorCharFrm );
                _CheckTopOfLine( rAnch, aAnchorCharFrm );
            }
            // <--
        }
    }
}

/** check anchor character rectangle

    OD 11.11.2003 #i22341#
    helper method for method <CheckCharRectAndTopOfLine()>
    For to-character anchored Writer fly frames the member <maLastCharRect>
    is updated. This is checked for change and depending on the applied
    positioning, it's decided, if the Writer fly frame has to be invalidated.
    OD 2004-07-14 #117380#
    improvement - add second parameter <_rAnchorCharFrm>

    @author OD
*/
void SwAnchoredObject::_CheckCharRect( const SwFmtAnchor& _rAnch,
                                       const SwTxtFrm& _rAnchorCharFrm )
{
    // determine rectangle of anchor character. If not exist, abort operation
    SwRect aCharRect;
    if ( !_rAnchorCharFrm.GetAutoPos( aCharRect, *_rAnch.GetCntntAnchor() ) )
    {
        return;
    }
    // check, if anchor character rectangle has changed
    if ( aCharRect != maLastCharRect )
    {
        // check positioning and alignment for invalidation of position
        {
            SWRECTFN( (&_rAnchorCharFrm) );
            // determine positioning and alignment
            SwFmtVertOrient aVert( GetFrmFmt().GetVertOrient() );
            SwFmtHoriOrient aHori( GetFrmFmt().GetHoriOrient() );
            // check for anchor character rectangle changes for certain
            // positionings and alignments
            // OD 07.10.2003 #110978# - add condition to invalidate position,
            // if vertical aligned at frame/page area and vertical position
            // of anchor character has changed.
            const sal_Int16 eVertRelOrient = aVert.GetRelationOrient();
            if ( ( aHori.GetRelationOrient() == text::RelOrientation::CHAR &&
                   (aCharRect.*fnRect->fnGetLeft)() !=
                        (maLastCharRect.*fnRect->fnGetLeft)() ) ||
                 ( eVertRelOrient == text::RelOrientation::CHAR &&
                   ( (aCharRect.*fnRect->fnGetTop)() !=
                        (maLastCharRect.*fnRect->fnGetTop)() ||
                     (aCharRect.*fnRect->fnGetHeight)() !=
                        (maLastCharRect.*fnRect->fnGetHeight)() ) ) ||
                 ( ( ( eVertRelOrient == text::RelOrientation::FRAME ) ||
                     ( eVertRelOrient == text::RelOrientation::PRINT_AREA ) ||
                     ( eVertRelOrient == text::RelOrientation::PAGE_FRAME ) ||
                     ( eVertRelOrient == text::RelOrientation::PAGE_PRINT_AREA ) ) &&
                   ( (aCharRect.*fnRect->fnGetTop)() !=
                        (maLastCharRect.*fnRect->fnGetTop)() ) ) )
            {
                // --> OD 2004-10-08 #i26945#, #i35911# - unlock position of
                // anchored object, if it isn't registered at the page,
                // where its anchor character frame is on.
                if ( GetPageFrm() != _rAnchorCharFrm.FindPageFrm() )
                {
                    UnlockPosition();
                }
                // <--
                InvalidateObjPos();
            }
        }
        // keep new anchor character rectangle
        maLastCharRect = aCharRect;
    }
}

/** check top of line

    OD 11.11.2003 #i22341#
    helper method for method <CheckCharRectAndTopOfLine()>
    For to-character anchored Writer fly frames the member <mnLastTopOfLine>
    is updated. This is checked for change and depending on the applied
    positioning, it's decided, if the Writer fly frame has to be invalidated.
    OD 2004-07-14 #117380#
    improvement - add second parameter <_rAnchorCharFrm>

    @author OD
*/
void SwAnchoredObject::_CheckTopOfLine( const SwFmtAnchor& _rAnch,
                                        const SwTxtFrm& _rAnchorCharFrm )
{
    SwTwips nTopOfLine = 0L;
    if ( _rAnchorCharFrm.GetTopOfLine( nTopOfLine, *_rAnch.GetCntntAnchor() ) )
    {
        if ( nTopOfLine != mnLastTopOfLine )
        {
            // check alignment for invalidation of position
            if ( GetFrmFmt().GetVertOrient().GetRelationOrient() == text::RelOrientation::TEXT_LINE )
            {
                // --> OD 2004-10-08 #i26945#, #i35911# - unlock position of
                // anchored object, if it isn't registered at the page,
                // where its anchor character frame is on.
                if ( GetPageFrm() != _rAnchorCharFrm.FindPageFrm() )
                {
                    UnlockPosition();
                }
                // <--
                InvalidateObjPos();
            }
            // keep new top of line value
            mnLastTopOfLine = nTopOfLine;
        }
    }
}

void SwAnchoredObject::ClearCharRectAndTopOfLine()
{
    maLastCharRect.Clear();
    mnLastTopOfLine = 0;
}

const Point SwAnchoredObject::GetCurrRelPos() const
{
    return maRelPos;
}
void SwAnchoredObject::SetCurrRelPos( Point _aRelPos )
{
    maRelPos = _aRelPos;
}

void SwAnchoredObject::ObjectAttachedToAnchorFrame()
{
    // default behaviour:
    // update layout direction, the anchored object is assigned to
    UpdateLayoutDir();
}

/** method update layout direction the layout direction, the anchored
    object is in

    OD 2004-07-27 #i31698#
    method has typically to be called, if the anchored object gets its
    anchor frame assigned.

    @author OD
*/
void SwAnchoredObject::UpdateLayoutDir()
{
    SwFrmFmt::tLayoutDir nLayoutDir = SwFrmFmt::HORI_L2R;
    const SwFrm* pAnchorFrm = GetAnchorFrm();
    if ( pAnchorFrm )
    {
        const bool bVert = pAnchorFrm->IsVertical();
        const bool bR2L = pAnchorFrm->IsRightToLeft();
        if ( bVert )
        {
            nLayoutDir = SwFrmFmt::VERT_R2L;
        }
        else if ( bR2L )
        {
            nLayoutDir = SwFrmFmt::HORI_R2L;
        }
    }
    GetFrmFmt().SetLayoutDir( nLayoutDir );
}

/** method to perform necessary invalidations for the positioning of
    objects, for whose the wrapping style influence has to be considered
    on the object positioning.

    OD 2004-06-30 #i28701#

    @author OD
*/
void SwAnchoredObject::InvalidateObjPosForConsiderWrapInfluence(
                                                    const bool _bNotifyBackgrd )
{
    if ( ConsiderObjWrapInfluenceOnObjPos() )
    {
        // indicate that object has not to be considered for text wrap
        SetConsiderForTextWrap( false );
        // unlock position
        UnlockPosition();
        // invalidate position
        InvalidateObjPos();
        // invalidate 'background', if requested
        if ( _bNotifyBackgrd )
        {
            NotifyBackground( GetPageFrm(), GetObjRectWithSpaces(), PREP_FLY_LEAVE );
        }
    }
}

/** method to determine, if wrapping style influence of the anchored
    object has to be considered on the object positioning

    OD 2004-06-30 #i28701#
    Note: result of this method also decides, if the booleans for the
    layout process are of relevance.

    @author OD
*/
bool SwAnchoredObject::ConsiderObjWrapInfluenceOnObjPos() const
{
    bool bRet( false );

    const SwFrmFmt& rObjFmt = GetFrmFmt();

    // --> OD 2004-08-25 #i3317# - add condition <IsTmpConsiderWrapInfluence()>
    // --> OD 2005-09-29 #i55204#
    // - correction: wrapping style influence has been considered, if condition
    //   <IsTmpConsiderWrapInfluence()> is hold, regardless of its anchor type
    //   or its wrapping style.
    if ( IsTmpConsiderWrapInfluence() )
    {
        bRet = true;
    }
    else if ( rObjFmt.getIDocumentSettingAccess()->get(IDocumentSettingAccess::CONSIDER_WRAP_ON_OBJECT_POSITION) )
    // <--
    {
        const SwFmtAnchor& rAnchor = rObjFmt.GetAnchor();
        if ( ((rAnchor.GetAnchorId() == FLY_AT_CHAR) ||
              (rAnchor.GetAnchorId() == FLY_AT_PARA)) &&
             rObjFmt.GetSurround().GetSurround() != SURROUND_THROUGHT )
        {
            // --> OD 2004-09-23 #i34520# - text also wraps around anchored
            // objects in the layer Hell - see the text formatting.
            // Thus, it hasn't to be checked here.
            bRet = true;
            // <--
        }
    }

    return bRet;
}

/** method to determine, if other anchored objects, also attached at
    to the anchor frame, have to consider its wrap influence.

    // --> OD 2005-02-22 #i43255#

    @author OD
*/
bool SwAnchoredObject::ConsiderObjWrapInfluenceOfOtherObjs() const
{
    bool bRet( false );

    const SwSortedObjs* pObjs = GetAnchorFrm()->GetDrawObjs();
    if ( pObjs->Count() > 1 )
    {
        sal_uInt32 i = 0;
        for ( ; i < pObjs->Count(); ++i )
        {
            SwAnchoredObject* pAnchoredObj = (*pObjs)[i];
            if ( pAnchoredObj != this &&
                 pAnchoredObj->ConsiderObjWrapInfluenceOnObjPos() )
            {
                bRet = true;
                break;
            }
        }
    }

    return bRet;
}

// =============================================================================
// --> OD 2004-06-29 #i28701# - accessors to booleans for layout process
// =============================================================================
bool SwAnchoredObject::ConsiderForTextWrap() const
{
    if ( ConsiderObjWrapInfluenceOnObjPos() )
        return mbConsiderForTextWrap;
    else
        return true;
}

void SwAnchoredObject::SetConsiderForTextWrap( const bool _bConsiderForTextWrap )
{
    mbConsiderForTextWrap = _bConsiderForTextWrap;
}

bool SwAnchoredObject::PositionLocked() const
{
    if ( ConsiderObjWrapInfluenceOnObjPos() )
        return mbPositionLocked;
    else
        return false;
}

bool SwAnchoredObject::RestartLayoutProcess() const
{
    if ( ConsiderObjWrapInfluenceOnObjPos() )
        return mbRestartLayoutProcess;
    else
        return false;
}

void SwAnchoredObject::SetRestartLayoutProcess( const bool _bRestartLayoutProcess )
{
    mbRestartLayoutProcess = _bRestartLayoutProcess;
}

// --> OD 2004-10-22 #i35911#
bool SwAnchoredObject::ClearedEnvironment() const
{
    if ( ConsiderObjWrapInfluenceOnObjPos() )
        return mbClearedEnvironment;
    else
        return false;
}
void SwAnchoredObject::SetClearedEnvironment( const bool _bClearedEnvironment )
{
    mbClearedEnvironment = _bClearedEnvironment;
}
// <--

/** method to determine, if due to anchored object size and wrapping
    style, its layout environment is cleared.

    OD 2004-10-22 #i35911#

    @author OD
*/
bool SwAnchoredObject::HasClearedEnvironment() const
{
    bool bHasClearedEnvironment( false );

    // --> OD 2005-03-03 #i43913# - layout frame, vertical position is orient at, has to be set.
    ASSERT( GetVertPosOrientFrm(),
            "<SwAnchoredObject::HasClearedEnvironment()> - layout frame missing, at which the vertical position is oriented at." );
    if ( GetVertPosOrientFrm() &&
         GetAnchorFrm()->IsTxtFrm() &&
         !static_cast<const SwTxtFrm*>(GetAnchorFrm())->IsFollow() &&
         static_cast<const SwTxtFrm*>(GetAnchorFrm())->FindPageFrm()->GetPhyPageNum() >=
                GetPageFrm()->GetPhyPageNum() )
    // <--
    {
        const SwFrm* pTmpFrm = GetVertPosOrientFrm()->Lower();
        while ( pTmpFrm && pTmpFrm->IsLayoutFrm() && !pTmpFrm->IsTabFrm() )
        {
            pTmpFrm = static_cast<const SwLayoutFrm*>(pTmpFrm)->Lower();
        }
        if ( !pTmpFrm )
        {
            bHasClearedEnvironment = true;
        }
        else if ( pTmpFrm->IsTxtFrm() && !pTmpFrm->GetNext() )
        {
            const SwTxtFrm* pTmpTxtFrm = static_cast<const SwTxtFrm*>(pTmpFrm);
            if ( pTmpTxtFrm->IsUndersized() ||
                 ( pTmpTxtFrm->GetFollow() &&
                   pTmpTxtFrm->GetFollow()->GetOfst() == 0 ) )
            {
                bHasClearedEnvironment = true;
            }
        }
    }

    return bHasClearedEnvironment;
}

/** method to add spacing to object area

    OD 2004-06-30 #i28701#
    OD 2006-08-10 #i68520# - return constant reference and use cache

    @author OD
*/
const SwRect& SwAnchoredObject::GetObjRectWithSpaces() const
{
    if ( mbObjRectWithSpacesValid &&
         maLastObjRect != GetObjRect() )
    {
        ASSERT( false,
                "<SwAnchoredObject::GetObjRectWithSpaces> - cache for object rectangle inclusive spaces marked as valid, but it couldn't be. Missing invalidation of cache. Please inform OD." );
        InvalidateObjRectWithSpaces();
    }
    if ( !mbObjRectWithSpacesValid )
    {
        // --> OD 2006-10-05 #i70122# - correction:
        // use bounding rectangle of anchored objects.
//        maObjRectWithSpaces = GetObjRect();
        maObjRectWithSpaces = GetObjBoundRect();
        // <--
        const SwFrmFmt& rFmt = GetFrmFmt();
        const SvxULSpaceItem& rUL = rFmt.GetULSpace();
        const SvxLRSpaceItem& rLR = rFmt.GetLRSpace();
        {
            maObjRectWithSpaces.Top ( Max( maObjRectWithSpaces.Top() - long(rUL.GetUpper()), 0L ));
            maObjRectWithSpaces.Left( Max( maObjRectWithSpaces.Left()- long(rLR.GetLeft()),  0L ));
            maObjRectWithSpaces.SSize().Height() += rUL.GetLower();
            maObjRectWithSpaces.SSize().Width()  += rLR.GetRight();
        }

        mbObjRectWithSpacesValid = true;
        maLastObjRect = GetObjRect();
    }

    return maObjRectWithSpaces;
}

// --> OD 2006-08-10 #i68520#
void SwAnchoredObject::SetObjTop( const SwTwips _nTop)
{
    const bool bTopChanged( _SetObjTop( _nTop ) );
    if ( bTopChanged )
    {
        mbObjRectWithSpacesValid = false;
    }
}

void SwAnchoredObject::SetObjLeft( const SwTwips _nLeft)
{
    const bool bLeftChanged( _SetObjLeft( _nLeft ) );
    if ( bLeftChanged )
    {
        mbObjRectWithSpacesValid = false;
    }
}
// <--

/** method to update anchored object in the <SwSortedObjs> lists

    OD 2004-07-01 #i28701#
    If document compatibility option 'Consider wrapping style influence
    on object positioning' is ON, additionally all anchored objects
    at the anchor frame and all following anchored objects on the page
    frame are invalidated.

    @author OD
*/
void SwAnchoredObject::UpdateObjInSortedList()
{
    if ( GetAnchorFrm() )
    {
        if ( GetFrmFmt().getIDocumentSettingAccess()->get(IDocumentSettingAccess::CONSIDER_WRAP_ON_OBJECT_POSITION) )
        {
            // invalidate position of all anchored objects at anchor frame
            if ( GetAnchorFrm()->GetDrawObjs() )
            {
                const SwSortedObjs* pObjs = GetAnchorFrm()->GetDrawObjs();
                // determine start index
                sal_uInt32 i = 0;
                for ( ; i < pObjs->Count(); ++i )
                {
                    SwAnchoredObject* pAnchoredObj = (*pObjs)[i];
                    if ( pAnchoredObj->ConsiderObjWrapInfluenceOnObjPos() )
                        pAnchoredObj->InvalidateObjPosForConsiderWrapInfluence( true );
                    else
                        pAnchoredObj->InvalidateObjPos();
                }
            }
            // invalidate all following anchored objects on the page frame
            if ( GetPageFrm() && GetPageFrm()->GetSortedObjs() )
            {
                const SwSortedObjs* pObjs = GetPageFrm()->GetSortedObjs();
                // determine start index
                sal_uInt32 i = pObjs->ListPosOf( *this ) + 1;
                for ( ; i < pObjs->Count(); ++i )
                {
                    SwAnchoredObject* pAnchoredObj = (*pObjs)[i];
                    if ( pAnchoredObj->ConsiderObjWrapInfluenceOnObjPos() )
                        pAnchoredObj->InvalidateObjPosForConsiderWrapInfluence( true );
                    else
                        pAnchoredObj->InvalidateObjPos();
                }
            }
        }
        // update its position in the sorted object list of its anchor frame
        AnchorFrm()->GetDrawObjs()->Update( *this );
        // update its position in the sorted object list of its page frame
        // note: as-character anchored object aren't registered at a page frame
        if ( GetFrmFmt().GetAnchor().GetAnchorId() != FLY_AS_CHAR )
        {
            GetPageFrm()->GetSortedObjs()->Update( *this );
        }
    }
}

/** method to determine, if invalidation of position is allowed

    OD 2004-07-01 #i28701#

    @author OD
*/
bool SwAnchoredObject::InvalidationOfPosAllowed() const
{
    // --> OD 2004-11-03 #114798# - Check, if page frame layout is in progress,
    // isn't needed, because of anchored object, whose are moved forward.
    return !PositionLocked();
    // <--
}

/** method to determine the page frame, on which the 'anchor' of
    the given anchored object is.

    OD 2004-07-02 #i28701#
    OD 2004-09-23 #i33751#, #i34060#
    Adjust meaning of method and thus its name: If the anchored object
    or its anchor isn't correctly inserted in the layout, no page frame
    can be found. Thus, the return type changed to be a pointer and can
    be NULL.

    @author OD
*/
SwPageFrm* SwAnchoredObject::FindPageFrmOfAnchor()
{
    SwPageFrm* pRetPageFrm = 0L;

    // --> OD 2005-03-08 #i44339# - check, if anchor frame exists.
    if ( mpAnchorFrm )
    {
        // --> OD 2004-10-08 #i26945# - use new method <GetAnchorFrmContainingAnchPos()>
        pRetPageFrm = GetAnchorFrmContainingAnchPos()->FindPageFrm();
        // <--
    }

    return pRetPageFrm;
}

/** get frame, which contains the anchor character, if the object
    is anchored at-character or as-character.

    OD 2004-10-04 #i26945#

    @author OD

    @return SwTxtFrm*
    text frame containing the anchor character. It's NULL, if the object
    isn't anchored at-character resp. as-character.
*/
SwTxtFrm* SwAnchoredObject::FindAnchorCharFrm()
{
    SwTxtFrm* pAnchorCharFrm( 0L );

    // --> OD 2005-03-08 #i44339# - check, if anchor frame exists.
    if ( mpAnchorFrm )
    {
        const SwFmtAnchor& rAnch = GetFrmFmt().GetAnchor();
        if ((rAnch.GetAnchorId() == FLY_AT_CHAR) ||
            (rAnch.GetAnchorId() == FLY_AS_CHAR))
        {
            pAnchorCharFrm = &(static_cast<SwTxtFrm*>(AnchorFrm())->
                        GetFrmAtOfst( rAnch.GetCntntAnchor()->nContent.GetIndex() ));
        }
    }
    // <--

    return pAnchorCharFrm;
}

/** method to determine, if a format on the anchored object is possible

    OD 2004-07-23 #i28701#
    A format is possible, if anchored object is in an invisible layer.
    Note: method is virtual to refine the conditions for the sub-classes.

    @author OD
*/
bool SwAnchoredObject::IsFormatPossible() const
{
    return GetFrmFmt().GetDoc()->IsVisibleLayerId( GetDrawObj()->GetLayer() );
}

// --> OD 2004-08-25 #i3317#
void SwAnchoredObject::SetTmpConsiderWrapInfluence( const bool _bTmpConsiderWrapInfluence )
{
    mbTmpConsiderWrapInfluence = _bTmpConsiderWrapInfluence;
    // --> OD 2004-10-22 #i35911#
    if ( mbTmpConsiderWrapInfluence )
    {
        SwLayouter::InsertObjForTmpConsiderWrapInfluence( *(GetFrmFmt().GetDoc()),
                                                          *this );
    }
    // <--
}

bool SwAnchoredObject::IsTmpConsiderWrapInfluence() const
{
    return mbTmpConsiderWrapInfluence;
}
// <--

// --> OD 2006-07-24 #b6449874#
void SwAnchoredObject::SetTmpConsiderWrapInfluenceOfOtherObjs( const bool bTmpConsiderWrapInfluence )
{
    const SwSortedObjs* pObjs = GetAnchorFrm()->GetDrawObjs();
    if ( pObjs->Count() > 1 )
    {
        sal_uInt32 i = 0;
        for ( ; i < pObjs->Count(); ++i )
        {
            SwAnchoredObject* pAnchoredObj = (*pObjs)[i];
            if ( pAnchoredObj != this )
            {
                pAnchoredObj->SetTmpConsiderWrapInfluence( bTmpConsiderWrapInfluence );
            }
        }
    }
}
// <--

/** method to determine, if the anchored object is overlapping with a
    previous column

    OD 2004-08-25 #i3317#
    overlapping with a previous column means, that the object overlaps
    with a column, which is a previous one of the column its anchor
    frame is in.
    Only applied for at-paragraph and at-character anchored objects.

    @author OD
*/
bool SwAnchoredObject::OverlapsPrevColumn() const
{
    bool bOverlapsPrevColumn( false );

    if ( mpAnchorFrm && mpAnchorFrm->IsTxtFrm() )
    {
        const SwFrm* pColFrm = mpAnchorFrm->FindColFrm();
        if ( pColFrm && pColFrm->GetPrev() )
        {
            const SwFrm* pTmpColFrm = pColFrm->GetPrev();
            SwRect aChkRect;
            while ( pTmpColFrm )
            {
                aChkRect.Union( pTmpColFrm->Frm() );
                pTmpColFrm = pTmpColFrm->GetPrev();
            }
            bOverlapsPrevColumn = GetObjRect().IsOver( aChkRect );
        }
    }

    return bOverlapsPrevColumn;
}

/** method to determine position of anchored object relative to
    anchor frame

    OD 2005-01-06 #i30669#
    Usage: Needed layout information for WW8 export

    @author OD
*/
Point SwAnchoredObject::GetRelPosToAnchorFrm() const
{
    Point aRelPos;

    ASSERT( GetAnchorFrm(),
            "<SwAnchoredObject::GetRelPosToAnchorFrm()> - missing anchor frame." );
    aRelPos = GetObjRect().Pos();
    aRelPos -= GetAnchorFrm()->Frm().Pos();

    return aRelPos;
}

/** method to determine position of anchored object relative to
    page frame

    OD 2005-01-06 #i30669#
    Usage: Needed layout information for WW8 export
    OD 2005-01-27 #i33818# - add parameters <_bFollowTextFlow> and
    <_obRelToTableCell>
    If <_bFollowTextFlow> is set and object is anchored inside table,
    the position relative to the table cell is determined. Output
    parameter <_obRelToTableCell> reflects this situation

    @author OD
*/
Point SwAnchoredObject::GetRelPosToPageFrm( const bool _bFollowTextFlow,
                                            bool& _obRelToTableCell ) const
{
    Point aRelPos;
    _obRelToTableCell = false;

    ASSERT( GetAnchorFrm(),
            "<SwAnchoredObject::GetRelPosToPageFrm()> - missing anchor frame." );
    ASSERT( GetAnchorFrm()->FindPageFrm(),
            "<SwAnchoredObject::GetRelPosToPageFrm()> - missing page frame." );

    aRelPos = GetObjRect().Pos();
    // --> OD 2005-01-27 #i33818# - search for cell frame, if object has to
    // follow the text flow.
    const SwFrm* pFrm( 0L );
    if ( _bFollowTextFlow && !GetAnchorFrm()->IsPageFrm() )
    {
        pFrm = GetAnchorFrm()->GetUpper();
        while ( !pFrm->IsCellFrm() && !pFrm->IsPageFrm() )
        {
            pFrm = pFrm->GetUpper();
        }
    }
    else
    {
        pFrm = GetAnchorFrm()->FindPageFrm();
    }
    if ( pFrm->IsCellFrm() )
    {
        aRelPos -= ( pFrm->Frm().Pos() + pFrm->Prt().Pos() );
        _obRelToTableCell = true;
    }
    else
    {
        aRelPos -= pFrm->Frm().Pos();
    }
    // <--

    return aRelPos;
}

/** method to determine position of anchored object relative to
    anchor character

    OD 2005-01-06 #i30669#
    Usage: Needed layout information for WW8 export

    @author OD
*/
Point SwAnchoredObject::GetRelPosToChar() const
{
    Point aRelPos;

    aRelPos = GetObjRect().Pos();
    aRelPos -= GetLastCharRect().Pos();

    return aRelPos;
}

/** method to determine position of anchored object relative to
    top of line

    OD 2005-01-06 #i30669#
    Usage: Needed layout information for WW8 export

    @author OD
*/
Point SwAnchoredObject::GetRelPosToLine() const
{
    Point aRelPos;

    aRelPos = GetObjRect().Pos();
    aRelPos.Y() -= GetLastTopOfLine();

    return aRelPos;
}
