/*************************************************************************
 *
 *  $RCSfile: ascharanchoredobjectposition.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-23 13:01:13 $
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
#ifndef _ASCHARANCHOREDOBJECTPOSITION_HXX
#include <ascharanchoredobjectposition.hxx>
#endif

#ifndef _FRAME_HXX
#include <frame.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _FLYFRMS_HXX
#include <flyfrms.hxx>
#endif
#ifndef _SVX_SVDOBJ_HXX
#include <svx/svdobj.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX
#include <svx/ulspitem.hxx>
#endif
#ifndef _FMTORNT_HXX
#include <fmtornt.hxx>
#endif

using namespace objectpositioning;

/** constructor

    @author OD
*/
SwAsCharAnchoredObjectPosition::SwAsCharAnchoredObjectPosition(
                                    SdrObject& _rDrawObj,
                                    const Point&    _rProposedAnchorPos,
                                    const AsCharFlags _nFlags,
                                    const SwTwips     _nLineAscent,
                                    const SwTwips     _nLineDescent,
                                    const SwTwips     _nLineAscentInclObjs,
                                    const SwTwips     _nLineDescentInclObjs )
    : SwAnchoredObjectPosition( _rDrawObj ),
      mrProposedAnchorPos( _rProposedAnchorPos ),
      mnFlags( _nFlags ),
      mnLineAscent( _nLineAscent ),
      mnLineDescent( _nLineDescent ),
      mnLineAscentInclObjs( _nLineAscentInclObjs ),
      mnLineDescentInclObjs( _nLineDescentInclObjs ),
      maAnchorPos ( Point() ),
      mnRelPos ( 0 ),
      maObjBoundRect ( SwRect() ),
      mnLineAlignment ( 0 )
{}

/** destructor

    @author OD
*/
SwAsCharAnchoredObjectPosition::~SwAsCharAnchoredObjectPosition()
{}

/** method to cast <SwAnchoredObjectPosition::GetAnchorFrm()> to needed type

    @author OD
*/
const SwTxtFrm& SwAsCharAnchoredObjectPosition::GetAnchorTxtFrm() const
{
    ASSERT( GetAnchorFrm().ISA(SwTxtFrm),
            "SwAsCharAnchoredObjectPosition::GetAnchorTxtFrm() - wrong anchor frame type" );

    return static_cast<const SwTxtFrm&>(GetAnchorFrm());
}

/** calculate position for object

    OD 30.07.2003 #110978#
    members <maAnchorPos>, <mnRelPos>, <maObjBoundRect> and
    <mnLineAlignment> are calculated.
    calculated position is set at the given object.

    @author OD
*/
void SwAsCharAnchoredObjectPosition::CalcPosition()
{
    const SwTxtFrm& rAnchorFrm = GetAnchorTxtFrm();
    // swap anchor frame, if swapped. Note: destructor takes care of the 'undo'
    SwFrmSwapper aFrmSwapper( &rAnchorFrm, false );

    SWRECTFN( ( &rAnchorFrm ) )

    Point aAnchorPos( mrProposedAnchorPos );

    const SwFrmFmt& rFrmFmt = GetFrmFmt();

    SwRect aObjBoundRect( GetAnchoredObj().GetObjRect() );
    SwTwips nObjWidth = (aObjBoundRect.*fnRect->fnGetWidth)();

    // determine spacing values considering layout-/text-direction
    const SvxLRSpaceItem& rLRSpace = rFrmFmt.GetLRSpace();
    const SvxULSpaceItem& rULSpace = rFrmFmt.GetULSpace();
    SwTwips nLRSpaceLeft, nLRSpaceRight, nULSpaceUpper, nULSpaceLower;
    {
        if ( rAnchorFrm.IsVertical() )
        {
            // Seems to be easier to do it all the horizontal way
            // So, from now on think horizontal.
            rAnchorFrm.SwitchVerticalToHorizontal( aObjBoundRect );
            rAnchorFrm.SwitchVerticalToHorizontal( aAnchorPos );

            // convert the spacing values
            nLRSpaceLeft = rULSpace.GetUpper();
            nLRSpaceRight = rULSpace.GetLower();
            nULSpaceUpper = rLRSpace.GetRight();
            nULSpaceLower = rLRSpace.GetLeft();
        }
        else
        {
            if ( rAnchorFrm.IsRightToLeft() )
            {
                nLRSpaceLeft = rLRSpace.GetRight();
                nLRSpaceRight = rLRSpace.GetLeft();
            }
            else
            {
                nLRSpaceLeft = rLRSpace.GetLeft();
                nLRSpaceRight = rLRSpace.GetRight();
            }

            nULSpaceUpper = rULSpace.GetUpper();
            nULSpaceLower = rULSpace.GetLower();
        }
    }

    // consider left and upper spacing by adjusting anchor position.
    // left spacing is only considered, if requested.
    if( mnFlags & AS_CHAR_ULSPACE )
    {
        aAnchorPos.X() += nLRSpaceLeft;
    }
    aAnchorPos.Y() += nULSpaceUpper;

    // for drawing objects: consider difference between its bounding rectangle
    // and its snapping rectangle by adjusting anchor position.
    // left difference is only considered, if requested.
    if( !IsObjFly() )
    {
        SwRect aSnapRect = GetObject().GetSnapRect();
        if ( rAnchorFrm.IsVertical() )
        {
            rAnchorFrm.SwitchVerticalToHorizontal( aSnapRect );
        }

        if( mnFlags & AS_CHAR_ULSPACE )
        {
            aAnchorPos.X() += aSnapRect.Left() - aObjBoundRect.Left();
        }
        aAnchorPos.Y() += aSnapRect.Top() - aObjBoundRect.Top();
    }

    // enlarge bounding rectangle of object by its spacing.
    aObjBoundRect.Left( aObjBoundRect.Left() - nLRSpaceLeft );
    aObjBoundRect.Width( aObjBoundRect.Width() + nLRSpaceRight );
    aObjBoundRect.Top( aObjBoundRect.Top() - nULSpaceUpper );
    aObjBoundRect.Height( aObjBoundRect.Height() + nULSpaceLower );

    // calculate relative position to given base line.
    const SwFmtVertOrient& rVert = rFrmFmt.GetVertOrient();
    const SwTwips nObjBoundHeight = ( mnFlags & AS_CHAR_ROTATE )
                                    ? aObjBoundRect.Width()
                                    : aObjBoundRect.Height();
    const SwTwips nRelPos = _GetRelPosToBase( nObjBoundHeight, rVert );

    // for initial positioning:
    // adjust the proposed anchor position by difference between
    // calculated relative position to base line and current maximal line ascent.
    // Note: In the following line formatting the base line will be adjusted
    //       by the same difference.
    if( mnFlags & AS_CHAR_INIT && nRelPos < 0 && mnLineAscentInclObjs < -nRelPos )
    {
        if( mnFlags & AS_CHAR_ROTATE )
            aAnchorPos.X() -= mnLineAscentInclObjs + nRelPos;
        else
            aAnchorPos.Y() -= mnLineAscentInclObjs + nRelPos;
    }

    // consider BIDI-multiportion by adjusting proposed anchor position
    if( mnFlags & AS_CHAR_BIDI )
        aAnchorPos.X() -= aObjBoundRect.Width();

    // calculate relative position considering rotation and inside rotation
    // reverse direction.
    Point aRelPos;
    {
        if( mnFlags & AS_CHAR_ROTATE )
        {
            if( mnFlags & AS_CHAR_REVERSE )
                aRelPos.X() = -nRelPos - aObjBoundRect.Width();
            else
            {
                aRelPos.X() = nRelPos;
                aRelPos.Y() = -aObjBoundRect.Height();
            }
        }
        else
            aRelPos.Y() = nRelPos;
    }

    if( !IsObjFly() )
    {
        if( !( mnFlags & AS_CHAR_QUICK ) )
        {
            // save calculated Y-position value for 'automatic' vertical positioning,
            // in order to avoid a switch to 'manual' vertical positioning in
            // <SwDrawContact::_Changed(..)>.
            const SwVertOrient eVertOrient = rVert.GetVertOrient();
            if( rVert.GetPos() != nRelPos && eVertOrient != VERT_NONE )
            {
                SwFmtVertOrient aVert( rVert );
                aVert.SetPos( nRelPos );
                const_cast<SwFrmFmt&>(rFrmFmt).LockModify();
                const_cast<SwFrmFmt&>(rFrmFmt).SetAttr( aVert );
                const_cast<SwFrmFmt&>(rFrmFmt).UnlockModify();
            }

            // determine absolute anchor position considering layout directions.
            // Note: Use copy of <aAnchorPos>, because it's needed for
            //       setting relative position.
            Point aAbsAnchorPos( aAnchorPos );
            if ( rAnchorFrm.IsRightToLeft() )
            {
                rAnchorFrm.SwitchLTRtoRTL( aAbsAnchorPos );
                aAbsAnchorPos.X() -= nObjWidth;
            }
            if ( rAnchorFrm.IsVertical() )
                rAnchorFrm.SwitchHorizontalToVertical( aAbsAnchorPos );

            // set proposed anchor position at the drawing object.
            // OD 2004-04-06 #i26791# - distinction between 'master' drawing
            // object and 'virtual' drawing object no longer needed.
            GetObject().SetAnchorPos( aAbsAnchorPos );

            // move drawing object to set its correct relative position.
            {
                SwRect aSnapRect = GetObject().GetSnapRect();
                if ( rAnchorFrm.IsVertical() )
                    rAnchorFrm.SwitchVerticalToHorizontal( aSnapRect );

                Point aDiff;
                if ( rAnchorFrm.IsRightToLeft() )
                    aDiff = aRelPos + aAbsAnchorPos - aSnapRect.TopLeft();
                else
                    aDiff = aRelPos + aAnchorPos - aSnapRect.TopLeft();

                if ( rAnchorFrm.IsVertical() )
                    aDiff = Point( -aDiff.Y(), aDiff.X() );

                // OD 2004-04-06 #i26791# - distinction between 'master' drawing
                // object and 'virtual' drawing object no longer needed.
                GetObject().Move( Size( aDiff.X(), aDiff.Y() ) );
            }
        }

        // switch horizontal, LTR anchor position to absolute values.
        if ( rAnchorFrm.IsRightToLeft() )
        {
            rAnchorFrm.SwitchLTRtoRTL( aAnchorPos );
            aAnchorPos.X() -= nObjWidth;
        }
        if ( rAnchorFrm.IsVertical() )
            rAnchorFrm.SwitchHorizontalToVertical( aAnchorPos );

        // --> OD 2005-03-09 #i44347# - keep last object rectangle at anchored object
        ASSERT ( GetAnchoredObj().ISA(SwAnchoredDrawObject),
                 "<SwAsCharAnchoredObjectPosition::CalcPosition()> - wrong type of anchored object." );
        SwAnchoredDrawObject& rAnchoredDrawObj =
                        static_cast<SwAnchoredDrawObject&>( GetAnchoredObj() );
        rAnchoredDrawObj.SetLastObjRect( rAnchoredDrawObj.GetObjRect().SVRect() );
        // <--
    }
    else
    {
        // determine absolute anchor position and calculate corresponding
        // relative position and its relative position attribute.
        // Note: The relative position contains the spacing values.
        Point aRelAttr;
        if ( rAnchorFrm.IsRightToLeft() )
        {
            rAnchorFrm.SwitchLTRtoRTL( aAnchorPos );
            aAnchorPos.X() -= nObjWidth;
        }
        if ( rAnchorFrm.IsVertical() )
        {
            rAnchorFrm.SwitchHorizontalToVertical( aAnchorPos );
            aRelAttr = Point( -nRelPos, 0 );
            aRelPos = Point( -aRelPos.Y(), aRelPos.X() );
        }
        else
            aRelAttr = Point( 0, nRelPos );

        // OD 2004-03-23 #i26791#
        ASSERT( GetAnchoredObj().ISA(SwFlyInCntFrm),
                "<SwAsCharAnchoredObjectPosition::CalcPosition()> - wrong anchored object." );
        const SwFlyInCntFrm& rFlyInCntFrm =
                static_cast<const SwFlyInCntFrm&>(GetAnchoredObj());
        if ( !(mnFlags & AS_CHAR_QUICK) &&
             ( aAnchorPos != rFlyInCntFrm.GetRefPoint() ||
               aRelAttr != rFlyInCntFrm.GetCurrRelPos() ) )
        {
            // set new anchor position and relative position
            SwFlyInCntFrm* pFlyInCntFrm = &(const_cast<SwFlyInCntFrm&>(rFlyInCntFrm));
            pFlyInCntFrm->SetRefPoint( aAnchorPos, aRelAttr, aRelPos );
            if( nObjWidth != (pFlyInCntFrm->Frm().*fnRect->fnGetWidth)() )
            {
                // recalculate object bound rectangle, if object width has changed.
                aObjBoundRect = GetAnchoredObj().GetObjRect();
                aObjBoundRect.Left( aObjBoundRect.Left() - rLRSpace.GetLeft() );
                aObjBoundRect.Width( aObjBoundRect.Width() + rLRSpace.GetRight() );
                aObjBoundRect.Top( aObjBoundRect.Top() - rULSpace.GetUpper() );
                aObjBoundRect.Height( aObjBoundRect.Height() + rULSpace.GetLower() );
            }
        }
        ASSERT( (rFlyInCntFrm.Frm().*fnRect->fnGetHeight)(),
            "SwAnchoredObjectPosition::CalcPosition(..) - fly frame has an invalid height" );
    }

    // keep calculated values
    maAnchorPos = aAnchorPos;
    mnRelPos = nRelPos;
    maObjBoundRect = aObjBoundRect;
}

/** determine the relative position to base line for object position type AS_CHAR

    OD 29.07.2003 #110978#
    Note about values set at member <mnLineAlignment> -
    value gives feedback for the line formatting.
    0 - no feedback; 1|2|3 - proposed formatting of characters
    at top|at center|at bottom of line.

    @author OD
*/
SwTwips SwAsCharAnchoredObjectPosition::_GetRelPosToBase(
                                            const SwTwips _nObjBoundHeight,
                                            const SwFmtVertOrient& _rVert )
{
    SwTwips nRelPosToBase = 0;

    mnLineAlignment = 0;

    const SwVertOrient eVertOrient = _rVert.GetVertOrient();

    if ( eVertOrient == VERT_NONE )
        nRelPosToBase = _rVert.GetPos();
    else
    {
        if ( eVertOrient == VERT_CENTER )
            nRelPosToBase -= _nObjBoundHeight /  2;
        else if ( eVertOrient == VERT_TOP )
            nRelPosToBase -= _nObjBoundHeight;
        else if ( eVertOrient == VERT_BOTTOM )
            nRelPosToBase = 0;
        else if ( eVertOrient == VERT_CHAR_CENTER )
            nRelPosToBase -= ( _nObjBoundHeight + mnLineAscent - mnLineDescent ) / 2;
        else if ( eVertOrient == VERT_CHAR_TOP )
            nRelPosToBase -= mnLineAscent;
        else if ( eVertOrient == VERT_CHAR_BOTTOM )
            nRelPosToBase += mnLineDescent - _nObjBoundHeight;
        else
        {
            if( _nObjBoundHeight >= mnLineAscentInclObjs + mnLineDescentInclObjs )
            {
                // object is at least as high as the line. Thus, no more is
                // positioning necessary. Also, the max. ascent isn't changed.
                nRelPosToBase -= mnLineAscentInclObjs;
                if ( eVertOrient == VERT_LINE_CENTER )
                    mnLineAlignment = 2;
                else if ( eVertOrient == VERT_LINE_TOP )
                    mnLineAlignment = 1;
                else if ( eVertOrient == VERT_LINE_BOTTOM )
                    mnLineAlignment = 3;
            }
            else if ( eVertOrient == VERT_LINE_CENTER )
            {
                nRelPosToBase -= ( _nObjBoundHeight + mnLineAscentInclObjs - mnLineDescentInclObjs ) / 2;
                mnLineAlignment = 2;
            }
            else if ( eVertOrient == VERT_LINE_TOP )
            {
                nRelPosToBase -= mnLineAscentInclObjs;
                mnLineAlignment = 1;
            }
            else if ( eVertOrient == VERT_LINE_BOTTOM )
            {
                nRelPosToBase += mnLineDescentInclObjs - _nObjBoundHeight;
                mnLineAlignment = 3;
            }
        }
    }

    return nRelPosToBase;
}

/** calculated anchored position for object position

    @author OD
*/
Point SwAsCharAnchoredObjectPosition::GetAnchorPos() const
{
    return maAnchorPos;
}

/** calculated relative position to base line for object position

    @author OD
*/
SwTwips SwAsCharAnchoredObjectPosition::GetRelPosY() const
{
    return mnRelPos;
}

/** determined object rectangle including spacing for object

    @author OD
*/
SwRect SwAsCharAnchoredObjectPosition::GetObjBoundRectInclSpacing() const
{
    return maObjBoundRect;
}

/** determined line alignment

    @author OD
*/
sal_uInt8 SwAsCharAnchoredObjectPosition::GetLineAlignment() const
{
    return mnLineAlignment;
}
