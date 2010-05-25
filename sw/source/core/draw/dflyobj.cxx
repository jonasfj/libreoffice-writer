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
#include "hintids.hxx"
#include <svx/svdtrans.hxx>
#include <editeng/protitem.hxx>
#include <editeng/opaqitem.hxx>
#include <svx/svdpage.hxx>


#include <fmtclds.hxx>
#include <fmtornt.hxx>
#include <fmtfsize.hxx>
#include <fmturl.hxx>
#include "viewsh.hxx"
#include "viewimp.hxx"
#include "cntfrm.hxx"
#include "frmatr.hxx"
#include "doc.hxx"
#include "dview.hxx"
#include "dflyobj.hxx"
#include "flyfrm.hxx"
#include "frmfmt.hxx"
#include "viewopt.hxx"
#include "frmtool.hxx"
#include "flyfrms.hxx"
#include "ndnotxt.hxx"
#include "grfatr.hxx"
#include "pagefrm.hxx"


using namespace ::com::sun::star;


// --> OD 2004-11-22 #117958#
#include <svx/sdr/properties/defaultproperties.hxx>
// <--
#include <basegfx/range/b2drange.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>

// AW: For VCOfDrawVirtObj and stuff
#include <svx/sdr/contact/viewcontactofvirtobj.hxx>
#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#include <sw_primitivetypes2d.hxx>
#include <drawinglayer/primitive2d/sdrdecompositiontools2d.hxx>

using namespace ::com::sun::star;

static BOOL bInResize = FALSE;

TYPEINIT1( SwFlyDrawObj, SdrObject )
TYPEINIT1( SwVirtFlyDrawObj, SdrVirtObj )

/*************************************************************************
|*
|*	SwFlyDrawObj::Ctor
|*
|*	Ersterstellung		MA 18. Apr. 95
|*	Letzte Aenderung	MA 28. May. 96
|*
*************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        // #i95264# currently needed since createViewIndependentPrimitive2DSequence()
        // is called when RecalcBoundRect() is used. There should currently no VOCs being
        // constructed since it gets not visualized (instead the corresponding SwVirtFlyDrawObj's
        // referencing this one are visualized).
        class VCOfSwFlyDrawObj : public ViewContactOfSdrObj
        {
        protected:
            // This method is responsible for creating the graphical visualisation data
            // ONLY based on model data
            virtual drawinglayer::primitive2d::Primitive2DSequence createViewIndependentPrimitive2DSequence() const;

        public:
            // basic constructor, used from SdrObject.
            VCOfSwFlyDrawObj(SwFlyDrawObj& rObj)
            :   ViewContactOfSdrObj(rObj)
            {
            }
            virtual ~VCOfSwFlyDrawObj();
        };

        drawinglayer::primitive2d::Primitive2DSequence VCOfSwFlyDrawObj::createViewIndependentPrimitive2DSequence() const
        {
            // currently gets not visualized, return empty sequence
            return drawinglayer::primitive2d::Primitive2DSequence();
        }

        VCOfSwFlyDrawObj::~VCOfSwFlyDrawObj()
        {
        }
    } // end of namespace contact
} // end of namespace sdr

////////////////////////////////////////////////////////////////////////////////////////////////////

sdr::properties::BaseProperties* SwFlyDrawObj::CreateObjectSpecificProperties()
{
    // --> OD 2004-11-22 #117958# - create default properties
    return new sdr::properties::DefaultProperties(*this);
    // <--
}

sdr::contact::ViewContact* SwFlyDrawObj::CreateObjectSpecificViewContact()
{
    // #i95264# needs an own VC since createViewIndependentPrimitive2DSequence()
    // is called when RecalcBoundRect() is used
    return new sdr::contact::VCOfSwFlyDrawObj(*this);
}

SwFlyDrawObj::SwFlyDrawObj()
{
}

SwFlyDrawObj::~SwFlyDrawObj()
{
}

/*************************************************************************
|*
|*	SwFlyDrawObj::Factory-Methoden
|*
|*	Ersterstellung		MA 23. Feb. 95
|*	Letzte Aenderung	MA 23. Feb. 95
|*
*************************************************************************/

UINT32 __EXPORT SwFlyDrawObj::GetObjInventor() const
{
    return SWGInventor;
}


UINT16 __EXPORT SwFlyDrawObj::GetObjIdentifier()	const
{
    return SwFlyDrawObjIdentifier;
}


UINT16 __EXPORT SwFlyDrawObj::GetObjVersion() const
{
    return SwDrawFirst;
}

/*************************************************************************
|*
|*	SwVirtFlyDrawObj::CToren, Dtor
|*
|*	Ersterstellung		MA 08. Dec. 94
|*	Letzte Aenderung	MA 28. May. 96
|*
*************************************************************************/

//////////////////////////////////////////////////////////////////////////////////////
// AW: Need own primitive to get the FlyFrame paint working

namespace drawinglayer
{
    namespace primitive2d
    {
        class SwVirtFlyDrawObjPrimitive : public BufferedDecompositionPrimitive2D
        {
        private:
            const SwVirtFlyDrawObj&                 mrSwVirtFlyDrawObj;
            const basegfx::B2DRange					maOuterRange;

        protected:
            // method which is to be used to implement the local decomposition of a 2D primitive
            virtual Primitive2DSequence create2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const;

        public:
            SwVirtFlyDrawObjPrimitive(
                const SwVirtFlyDrawObj& rSwVirtFlyDrawObj,
                const basegfx::B2DRange &rOuterRange)
            :   BufferedDecompositionPrimitive2D(),
                mrSwVirtFlyDrawObj(rSwVirtFlyDrawObj),
                maOuterRange(rOuterRange)
            {
            }

            // compare operator
            virtual bool operator==(const BasePrimitive2D& rPrimitive) const;

            // get range
            virtual basegfx::B2DRange getB2DRange(const geometry::ViewInformation2D& rViewInformation) const;

            // overloaded to allow callbacks to wrap_DoPaintObject
            virtual Primitive2DSequence get2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const;

            // data read access
            const SwVirtFlyDrawObj& getSwVirtFlyDrawObj() const { return mrSwVirtFlyDrawObj; }
            const basegfx::B2DRange& getOuterRange() const { return maOuterRange; }

            // provide unique ID
            DeclPrimitrive2DIDBlock()
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer

namespace drawinglayer
{
    namespace primitive2d
    {
        Primitive2DSequence SwVirtFlyDrawObjPrimitive::create2DDecomposition(const geometry::ViewInformation2D& /*rViewInformation*/) const
        {
            Primitive2DSequence aRetval;

            if(!getOuterRange().isEmpty())
            {
                // currently this SW object has no primitive representation. As long as this is the case,
                // create invisible geometry to allow corfect HitTest and BoundRect calculations for the 
                // object. Use a filled primitive to get 'inside' as default object hit. The special cases from
                // the old SwVirtFlyDrawObj::CheckHit implementation are handled now in SwDrawView::PickObj;
                // this removed the 'hack' to get a view from inside model data or to react on null-tolerance
                // as it was done in the old implementation
                const Primitive2DReference aHitTestReference(
                    createHiddenGeometryPrimitives2D(
                        true, 
                        getOuterRange()));

                aRetval = Primitive2DSequence(&aHitTestReference, 1);
            }

            return aRetval;
        }

        bool SwVirtFlyDrawObjPrimitive::operator==(const BasePrimitive2D& rPrimitive) const
        {
            if(BufferedDecompositionPrimitive2D::operator==(rPrimitive))
            {
                const SwVirtFlyDrawObjPrimitive& rCompare = (SwVirtFlyDrawObjPrimitive&)rPrimitive;

                return (&getSwVirtFlyDrawObj() == &rCompare.getSwVirtFlyDrawObj()
                    && getOuterRange() == rCompare.getOuterRange());
            }

            return false;
        }

        basegfx::B2DRange SwVirtFlyDrawObjPrimitive::getB2DRange(const geometry::ViewInformation2D& /*rViewInformation*/) const
        {
            return getOuterRange();
        }

        Primitive2DSequence SwVirtFlyDrawObjPrimitive::get2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const
        {
            // This is the callback to keep the FlyFrame painting in SW alive as long as it
            // is not changed to primitives. This is the method which will be called by the processors
            // when they do not know this primitive (and they do not). Inside wrap_DoPaintObject
            // there needs to be a test that paint is only done during SW repaints (see there).
            // Using this mechanism guarantees the correct Z-Order of the VirtualObject-based FlyFrames.
            getSwVirtFlyDrawObj().wrap_DoPaintObject();

            // call parent
            return BufferedDecompositionPrimitive2D::get2DDecomposition(rViewInformation);
        }

        // provide unique ID
        ImplPrimitrive2DIDBlock(SwVirtFlyDrawObjPrimitive, PRIMITIVE2D_ID_SWVIRTFLYDRAWOBJPRIMITIVE2D)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////////////
// AW: own sdr::contact::ViewContact (VC) sdr::contact::ViewObjectContact (VOC) needed
// since offset is defined different from SdrVirtObj's sdr::contact::ViewContactOfVirtObj.
// For paint, that offset is used by setting at the OutputDevice; for primitives this is
// not possible since we have no OutputDevice, but define the geometry itself.

namespace sdr
{
    namespace contact
    {
        class VCOfSwVirtFlyDrawObj : public ViewContactOfVirtObj
        {
        protected:
            // This method is responsible for creating the graphical visualisation data
            // ONLY based on model data
            virtual drawinglayer::primitive2d::Primitive2DSequence createViewIndependentPrimitive2DSequence() const;

        public:
            // basic constructor, used from SdrObject.
            VCOfSwVirtFlyDrawObj(SwVirtFlyDrawObj& rObj)
            :   ViewContactOfVirtObj(rObj)
            {
            }
            virtual ~VCOfSwVirtFlyDrawObj();

            // access to SwVirtFlyDrawObj
            SwVirtFlyDrawObj& GetSwVirtFlyDrawObj() const
            {
                return (SwVirtFlyDrawObj&)mrObject;
            }
        };
    } // end of namespace contact
} // end of namespace sdr

namespace sdr
{
    namespace contact
    {
        drawinglayer::primitive2d::Primitive2DSequence VCOfSwVirtFlyDrawObj::createViewIndependentPrimitive2DSequence() const
        {
            drawinglayer::primitive2d::Primitive2DSequence xRetval;
            const SdrObject& rReferencedObject = GetSwVirtFlyDrawObj().GetReferencedObj();

            if(rReferencedObject.ISA(SwFlyDrawObj))
            {
                // create an own specialized primitive which is used as repaint callpoint and HitTest
                // for HitTest processor (see primitive implementation above)
                const basegfx::B2DRange aOuterRange(GetSwVirtFlyDrawObj().getOuterBound());

                if(!aOuterRange.isEmpty())
                {
                    const drawinglayer::primitive2d::Primitive2DReference xPrimitive(
                        new drawinglayer::primitive2d::SwVirtFlyDrawObjPrimitive(
                            GetSwVirtFlyDrawObj(),
                            aOuterRange));

                    xRetval = drawinglayer::primitive2d::Primitive2DSequence(&xPrimitive, 1);
                }
            }

            return xRetval;
        }

        VCOfSwVirtFlyDrawObj::~VCOfSwVirtFlyDrawObj()
        {
        }
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////////////

basegfx::B2DRange SwVirtFlyDrawObj::getOuterBound() const
{
    basegfx::B2DRange aOuterRange;
    const SdrObject& rReferencedObject = GetReferencedObj();

    if(rReferencedObject.ISA(SwFlyDrawObj))
    {
        const SwFlyFrm* pFlyFrame = GetFlyFrm();

        if(pFlyFrame)
        {
            const Rectangle aOuterRectangle(pFlyFrame->Frm().Pos(), pFlyFrame->Frm().SSize());

            if(!aOuterRectangle.IsEmpty()
                && RECT_EMPTY != aOuterRectangle.Right()
                && RECT_EMPTY != aOuterRectangle.Bottom())
            {
                aOuterRange.expand(basegfx::B2DTuple(aOuterRectangle.Left(), aOuterRectangle.Top()));
                aOuterRange.expand(basegfx::B2DTuple(aOuterRectangle.Right(), aOuterRectangle.Bottom()));
            }
        }
    }

    return aOuterRange;
}

basegfx::B2DRange SwVirtFlyDrawObj::getInnerBound() const
{
    basegfx::B2DRange aInnerRange;
    const SdrObject& rReferencedObject = GetReferencedObj();

    if(rReferencedObject.ISA(SwFlyDrawObj))
    {
        const SwFlyFrm* pFlyFrame = GetFlyFrm();

        if(pFlyFrame)
        {
            const Rectangle aInnerRectangle(pFlyFrame->Frm().Pos() + pFlyFrame->Prt().Pos(), pFlyFrame->Prt().SSize());

            if(!aInnerRectangle.IsEmpty()
                && RECT_EMPTY != aInnerRectangle.Right()
                && RECT_EMPTY != aInnerRectangle.Bottom())
            {
                aInnerRange.expand(basegfx::B2DTuple(aInnerRectangle.Left(), aInnerRectangle.Top()));
                aInnerRange.expand(basegfx::B2DTuple(aInnerRectangle.Right(), aInnerRectangle.Bottom()));
            }
        }
    }

    return aInnerRange;
}

sdr::contact::ViewContact* SwVirtFlyDrawObj::CreateObjectSpecificViewContact()
{
    // need an own ViewContact (VC) to allow creation of a specialized primitive
    // for being able to visualize the FlyFrames in primitive renderers
    return new sdr::contact::VCOfSwVirtFlyDrawObj(*this);
}

SwVirtFlyDrawObj::SwVirtFlyDrawObj(SdrObject& rNew, SwFlyFrm* pFly) :
    SdrVirtObj( rNew ),
    pFlyFrm( pFly )
{
    //#110094#-1
    // bNotPersistent = bNeedColorRestore = bWriterFlyFrame = TRUE;
    const SvxProtectItem &rP = pFlyFrm->GetFmt()->GetProtect();
    bMovProt = rP.IsPosProtected();
    bSizProt = rP.IsSizeProtected();
}


__EXPORT SwVirtFlyDrawObj::~SwVirtFlyDrawObj()
{
    if ( GetPage() )	//Der SdrPage die Verantwortung entziehen.
        GetPage()->RemoveObject( GetOrdNum() );
}

/*************************************************************************
|*
|*	SwVirtFlyDrawObj::GetFmt()
|*
|*	Ersterstellung		MA 08. Dec. 94
|*	Letzte Aenderung	MA 08. Dec. 94
|*
*************************************************************************/

const SwFrmFmt *SwVirtFlyDrawObj::GetFmt() const
{
    return GetFlyFrm()->GetFmt();
}


SwFrmFmt *SwVirtFlyDrawObj::GetFmt()
{
    return GetFlyFrm()->GetFmt();
}

/*************************************************************************
|*
|*	SwVirtFlyDrawObj::Paint()
|*
|*	Ersterstellung		MA 20. Dec. 94
|*	Letzte Aenderung	MA 18. Dec. 95
|*
*************************************************************************/

void SwVirtFlyDrawObj::wrap_DoPaintObject() const
{
    ViewShell* pShell = pFlyFrm->GetShell();

    // Only paint when we have a current shell and a DrawingLayer paint is in progress.
    // This avcoids evtl. problems with renderers which do processing stuff,
    // but no paints. IsPaintInProgress() depends on SW repaint, so, as long
    // as SW paints self and calls DrawLayer() for Heaven and Hell, this will
    // be correct
    if(pShell && pShell->IsDrawingLayerPaintInProgress())
    {
        sal_Bool bDrawObject(sal_True);

        if(!SwFlyFrm::IsPaint((SdrObject*)this, pShell))
        {
            bDrawObject = sal_False;
        }

        if(bDrawObject)
        {
            if(!pFlyFrm->IsFlyInCntFrm())
            {
                // it is also necessary to restore the VCL MapMode from ViewInformation since e.g.
                // the VCL PixelRenderer resets it at the used OutputDevice. Unfortunately, this
                // excludes shears and rotates which are not expressable in MapMode.
                OutputDevice* pOut = pShell->GetOut();

                pOut->Push(PUSH_MAPMODE);
                pOut->SetMapMode(pShell->getPrePostMapMode());

                // paint the FlyFrame (use standard VCL-Paint)
                pFlyFrm->Paint(GetFlyFrm()->Frm());

                pOut->Pop();
            }
        }
    }
}

/*************************************************************************
|*
|*	SwVirtFlyDrawObj::TakeObjInfo()
|*
|*	Ersterstellung		MA 03. May. 95
|*	Letzte Aenderung	MA 03. May. 95
|*
*************************************************************************/

void __EXPORT SwVirtFlyDrawObj::TakeObjInfo( SdrObjTransformInfoRec& rInfo ) const
{
    rInfo.bSelectAllowed	 = rInfo.bMoveAllowed =
    rInfo.bResizeFreeAllowed = rInfo.bResizePropAllowed = TRUE;

    rInfo.bRotateFreeAllowed = rInfo.bRotate90Allowed =
    rInfo.bMirrorFreeAllowed = rInfo.bMirror45Allowed =
    rInfo.bMirror90Allowed   = rInfo.bShearAllowed	  =
    rInfo.bCanConvToPath	 = rInfo.bCanConvToPoly	  =
    rInfo.bCanConvToPathLineToArea = rInfo.bCanConvToPolyLineToArea = FALSE;
}


/*************************************************************************
|*
|*	SwVirtFlyDrawObj::Groessenermittlung
|*
|*	Ersterstellung		MA 12. Jan. 95
|*	Letzte Aenderung	MA 10. Nov. 95
|*
*************************************************************************/

void SwVirtFlyDrawObj::SetRect() const
{
    if ( GetFlyFrm()->Frm().HasArea() )
        ((SwVirtFlyDrawObj*)this)->aOutRect = GetFlyFrm()->Frm().SVRect();
    else
        ((SwVirtFlyDrawObj*)this)->aOutRect = Rectangle();
}


const Rectangle& __EXPORT SwVirtFlyDrawObj::GetCurrentBoundRect() const
{
    SetRect();
    return aOutRect;
}

const Rectangle& __EXPORT SwVirtFlyDrawObj::GetLastBoundRect() const
{
    return GetCurrentBoundRect();
}


void __EXPORT SwVirtFlyDrawObj::RecalcBoundRect()
{
    SetRect();
}


void __EXPORT SwVirtFlyDrawObj::RecalcSnapRect()
{
    SetRect();
}


const Rectangle& __EXPORT SwVirtFlyDrawObj::GetSnapRect()  const
{
    SetRect();
    return aOutRect;
}


void __EXPORT SwVirtFlyDrawObj::SetSnapRect(const Rectangle& )
{
    Rectangle aTmp( GetLastBoundRect() );
    SetRect();
    SetChanged();
    BroadcastObjectChange();
    if (pUserCall!=NULL)
        pUserCall->Changed(*this, SDRUSERCALL_RESIZE, aTmp);
}


void __EXPORT SwVirtFlyDrawObj::NbcSetSnapRect(const Rectangle& )
{
    SetRect();
}


const Rectangle& __EXPORT SwVirtFlyDrawObj::GetLogicRect() const
{
    SetRect();
    return aOutRect;
}


void __EXPORT SwVirtFlyDrawObj::SetLogicRect(const Rectangle& )
{
    Rectangle aTmp( GetLastBoundRect() );
    SetRect();
    SetChanged();
    BroadcastObjectChange();
    if (pUserCall!=NULL)
        pUserCall->Changed(*this, SDRUSERCALL_RESIZE, aTmp);
}


void __EXPORT SwVirtFlyDrawObj::NbcSetLogicRect(const Rectangle& )
{
    SetRect();
}


::basegfx::B2DPolyPolygon SwVirtFlyDrawObj::TakeXorPoly() const
{
    const Rectangle aSourceRectangle(GetFlyFrm()->Frm().SVRect());
    const ::basegfx::B2DRange aSourceRange(aSourceRectangle.Left(), aSourceRectangle.Top(), aSourceRectangle.Right(), aSourceRectangle.Bottom());
    ::basegfx::B2DPolyPolygon aRetval;

    aRetval.append(::basegfx::tools::createPolygonFromRect(aSourceRange));

    return aRetval;
}

/*************************************************************************
|*
|*	SwVirtFlyDrawObj::Move() und Resize()
|*
|*	Ersterstellung		MA 12. Jan. 95
|*	Letzte Aenderung	MA 26. Jul. 96
|*
*************************************************************************/

void __EXPORT SwVirtFlyDrawObj::NbcMove(const Size& rSiz)
{
    MoveRect( aOutRect, rSiz );
    const Point aOldPos( GetFlyFrm()->Frm().Pos() );
    const Point aNewPos( aOutRect.TopLeft() );
    const SwRect aFlyRect( aOutRect );

    //Wenn der Fly eine automatische Ausrichtung hat (rechts oder oben),
    //so soll die Automatik erhalten bleiben
    SwFrmFmt *pFmt = GetFlyFrm()->GetFmt();
    const sal_Int16 eHori = pFmt->GetHoriOrient().GetHoriOrient();
    const sal_Int16 eVert = pFmt->GetVertOrient().GetVertOrient();
    const sal_Int16 eRelHori = pFmt->GetHoriOrient().GetRelationOrient();
    const sal_Int16 eRelVert = pFmt->GetVertOrient().GetRelationOrient();
    //Bei Absatzgebundenen Flys muss ausgehend von der neuen Position ein
    //neuer Anker gesetzt werden. Anker und neue RelPos werden vom Fly selbst
    //berechnet und gesetzt.
    if( GetFlyFrm()->IsFlyAtCntFrm() )
        ((SwFlyAtCntFrm*)GetFlyFrm())->SetAbsPos( aNewPos );
    else
    {
        const SwFrmFmt *pTmpFmt = GetFmt();
        const SwFmtVertOrient &rVert = pTmpFmt->GetVertOrient();
        const SwFmtHoriOrient &rHori = pTmpFmt->GetHoriOrient();
        long lXDiff = aNewPos.X() - aOldPos.X();
        if( rHori.IsPosToggle() && text::HoriOrientation::NONE == eHori &&
            !GetFlyFrm()->FindPageFrm()->OnRightPage() )
            lXDiff = -lXDiff;

        if( GetFlyFrm()->GetAnchorFrm()->IsRightToLeft() &&
            text::HoriOrientation::NONE == eHori )
            lXDiff = -lXDiff;

        long lYDiff = aNewPos.Y() - aOldPos.Y();
        if( GetFlyFrm()->GetAnchorFrm()->IsVertical() )
        {
            lXDiff -= rVert.GetPos();
            lYDiff += rHori.GetPos();
        }
        else
        {
            lXDiff += rHori.GetPos();
            lYDiff += rVert.GetPos();
        }

        if( GetFlyFrm()->GetAnchorFrm()->IsRightToLeft() &&
            text::HoriOrientation::NONE != eHori )
            lXDiff = GetFlyFrm()->GetAnchorFrm()->Frm().Width() -
                     aFlyRect.Width() - lXDiff;

        const Point aTmp( lXDiff, lYDiff );
        GetFlyFrm()->ChgRelPos( aTmp );
    }

    SwAttrSet aSet( pFmt->GetDoc()->GetAttrPool(),
                                            RES_VERT_ORIENT, RES_HORI_ORIENT );
    SwFmtHoriOrient aHori( pFmt->GetHoriOrient() );
    SwFmtVertOrient aVert( pFmt->GetVertOrient() );
    BOOL bPut = FALSE;

    if( !GetFlyFrm()->IsFlyLayFrm() &&
        ::GetHtmlMode(pFmt->GetDoc()->GetDocShell()) )
    {
        //Im HTML-Modus sind nur automatische Ausrichtungen erlaubt.
        //Einzig einen Snap auf Links/Rechts bzw. Linker-/Rechter-Rand koennen
        //wir versuchen.
        const SwFrm* pAnch = GetFlyFrm()->GetAnchorFrm();
        BOOL bNextLine = FALSE;

        if( !GetFlyFrm()->IsAutoPos() || text::RelOrientation::PAGE_FRAME != aHori.GetRelationOrient() )
        {
            if( text::RelOrientation::CHAR == eRelHori )
            {
                aHori.SetHoriOrient( text::HoriOrientation::LEFT );
                aHori.SetRelationOrient( text::RelOrientation::CHAR );
            }
            else
            {
                bNextLine = TRUE;
                //Horizontale Ausrichtung:
                const BOOL bLeftFrm =
                    aFlyRect.Left() < pAnch->Frm().Left() + pAnch->Prt().Left(),
                    bLeftPrt = aFlyRect.Left() + aFlyRect.Width() <
                               pAnch->Frm().Left() + pAnch->Prt().Width()/2;
                if ( bLeftFrm || bLeftPrt )
                {
                    aHori.SetHoriOrient( text::HoriOrientation::LEFT );
                    aHori.SetRelationOrient( bLeftFrm ? text::RelOrientation::FRAME : text::RelOrientation::PRINT_AREA );
                }
                else
                {
                    const BOOL bRightFrm = aFlyRect.Left() >
                                       pAnch->Frm().Left() + pAnch->Prt().Width();
                    aHori.SetHoriOrient( text::HoriOrientation::RIGHT );
                    aHori.SetRelationOrient( bRightFrm ? text::RelOrientation::FRAME : text::RelOrientation::PRINT_AREA );
                }
            }
            aSet.Put( aHori );
        }
        //Vertikale Ausrichtung bleibt grundsaetzlich schlicht erhalten,
        //nur bei nicht automatischer Ausrichtung wird umgeschaltet.
        BOOL bRelChar = text::RelOrientation::CHAR == eRelVert;
        aVert.SetVertOrient( eVert != text::VertOrientation::NONE ? eVert :
                GetFlyFrm()->IsFlyInCntFrm() ? text::VertOrientation::CHAR_CENTER :
                bRelChar && bNextLine ? text::VertOrientation::CHAR_TOP : text::VertOrientation::TOP );
        if( bRelChar )
            aVert.SetRelationOrient( text::RelOrientation::CHAR );
        else
            aVert.SetRelationOrient( text::RelOrientation::PRINT_AREA );
        aSet.Put( aVert );
        bPut = TRUE;
    }

    //Automatische Ausrichtungen wollen wir moeglichst nicht verlieren.
    if ( !bPut && bInResize )
    {
        if ( text::HoriOrientation::NONE != eHori )
        {
            aHori.SetHoriOrient( eHori );
            aHori.SetRelationOrient( eRelHori );
            aSet.Put( aHori );
            bPut = TRUE;
        }
        if ( text::VertOrientation::NONE != eVert )
        {
            aVert.SetVertOrient( eVert );
            aVert.SetRelationOrient( eRelVert );
            aSet.Put( aVert );
            bPut = TRUE;
        }
    }
    if ( bPut )
        pFmt->SetFmtAttr( aSet );
}


void __EXPORT SwVirtFlyDrawObj::NbcResize(const Point& rRef,
            const Fraction& xFact, const Fraction& yFact)
{
    ResizeRect( aOutRect, rRef, xFact, yFact );

    const SwFrm* pTmpFrm = GetFlyFrm()->GetAnchorFrm();
    if( !pTmpFrm )
        pTmpFrm = GetFlyFrm();
    const bool bVertX = pTmpFrm->IsVertical();

    const sal_Bool bRTL = pTmpFrm->IsRightToLeft();

    const Point aNewPos( bVertX || bRTL ?
                         aOutRect.Right() + 1 :
                         aOutRect.Left(),
                         aOutRect.Top() );

    Size aSz( aOutRect.Right() - aOutRect.Left() + 1,
              aOutRect.Bottom()- aOutRect.Top()  + 1 );
    if( aSz != GetFlyFrm()->Frm().SSize() )
    {
        //Die Breite darf bei Spalten nicht zu schmal werden
        if ( GetFlyFrm()->Lower() && GetFlyFrm()->Lower()->IsColumnFrm() )
        {
            SwBorderAttrAccess aAccess( SwFrm::GetCache(), GetFlyFrm() );
            const SwBorderAttrs &rAttrs = *aAccess.Get();
            long nMin = rAttrs.CalcLeftLine()+rAttrs.CalcRightLine();
            const SwFmtCol& rCol = rAttrs.GetAttrSet().GetCol();
            if ( rCol.GetColumns().Count() > 1 )
            {
                for ( USHORT i = 0; i < rCol.GetColumns().Count(); ++i )
                {
                    nMin += rCol.GetColumns()[i]->GetLeft() +
                            rCol.GetColumns()[i]->GetRight() +
                            MINFLY;
                }
                nMin -= MINFLY;
            }
            aSz.Width() = Max( aSz.Width(), nMin );
        }

        SwFrmFmt *pFmt = GetFmt();
        const SwFmtFrmSize aOldFrmSz( pFmt->GetFrmSize() );
        GetFlyFrm()->ChgSize( aSz );
        SwFmtFrmSize aFrmSz( pFmt->GetFrmSize() );
        if ( aFrmSz.GetWidthPercent() || aFrmSz.GetHeightPercent() )
        {
            long nRelWidth, nRelHeight;
            const SwFrm *pRel = GetFlyFrm()->IsFlyLayFrm() ?
                                GetFlyFrm()->GetAnchorFrm() :
                                GetFlyFrm()->GetAnchorFrm()->GetUpper();
            const ViewShell *pSh = GetFlyFrm()->GetShell();
            if ( pSh && pRel->IsBodyFrm() &&
                 pFmt->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) &&
                 pSh->VisArea().HasArea() )
            {
                nRelWidth  = pSh->GetBrowseWidth();
                nRelHeight = pSh->VisArea().Height();
                const Size aBorder = pSh->GetOut()->PixelToLogic( pSh->GetBrowseBorder() );
                nRelHeight -= 2*aBorder.Height();
            }
            else
            {
                nRelWidth  = pRel->Prt().Width();
                nRelHeight = pRel->Prt().Height();
            }
            if ( aFrmSz.GetWidthPercent() && aFrmSz.GetWidthPercent() != 0xFF &&
                 aOldFrmSz.GetWidth() != aFrmSz.GetWidth() )
                aFrmSz.SetWidthPercent( BYTE(aSz.Width() * 100L / nRelWidth + 0.5) );
            if ( aFrmSz.GetHeightPercent() && aFrmSz.GetHeightPercent() != 0xFF &&
                 aOldFrmSz.GetHeight() != aFrmSz.GetHeight() )
                aFrmSz.SetHeightPercent( BYTE(aSz.Height() * 100L / nRelHeight + 0.5) );
            pFmt->GetDoc()->SetAttr( aFrmSz, *pFmt );
        }
    }

    //Position kann auch veraendert sein!
    const Point aOldPos( bVertX || bRTL ?
                         GetFlyFrm()->Frm().TopRight() :
                         GetFlyFrm()->Frm().Pos() );

    if ( aNewPos != aOldPos )
    {
        //Kann sich durch das ChgSize veraendert haben!
        if( bVertX || bRTL )
        {
            if( aOutRect.TopRight() != aNewPos )
            {
                SwTwips nDeltaX = aNewPos.X() - aOutRect.Right();
                SwTwips nDeltaY = aNewPos.Y() - aOutRect.Top();
                MoveRect( aOutRect, Size( nDeltaX, nDeltaY ) );
            }
        }
        else if ( aOutRect.TopLeft() != aNewPos )
            aOutRect.SetPos( aNewPos );
        bInResize = TRUE;
        NbcMove( Size( 0, 0 ) );
        bInResize = FALSE;
    }
}


void __EXPORT SwVirtFlyDrawObj::Move(const Size& rSiz)
{
    NbcMove( rSiz );
    SetChanged();
    GetFmt()->GetDoc()->SetNoDrawUndoObj( TRUE );
}


void __EXPORT SwVirtFlyDrawObj::Resize(const Point& rRef,
                    const Fraction& xFact, const Fraction& yFact)
{
    NbcResize( rRef, xFact, yFact );
    SetChanged();
    GetFmt()->GetDoc()->SetNoDrawUndoObj( TRUE );
}


Pointer  __EXPORT SwVirtFlyDrawObj::GetMacroPointer(
    const SdrObjMacroHitRec& ) const
{
    return Pointer( POINTER_REFHAND );
}


FASTBOOL __EXPORT SwVirtFlyDrawObj::HasMacro() const
{
    const SwFmtURL &rURL = pFlyFrm->GetFmt()->GetURL();
    return rURL.GetMap() || rURL.GetURL().Len();
}


SdrObject* SwVirtFlyDrawObj::CheckMacroHit( const SdrObjMacroHitRec& rRec ) const
{
    const SwFmtURL &rURL = pFlyFrm->GetFmt()->GetURL();
    if( rURL.GetMap() || rURL.GetURL().Len() )
    {
        SwRect aRect;
        if ( pFlyFrm->Lower() && pFlyFrm->Lower()->IsNoTxtFrm() )
        {
            aRect = pFlyFrm->Prt();
            aRect += pFlyFrm->Frm().Pos();
        }
        else
            aRect = pFlyFrm->Frm();

        if( aRect.IsInside( rRec.aPos ) )
        {
            SwRect aActRect( aRect );
            Size aActSz( aRect.SSize() );
            aRect.Pos().X() += rRec.nTol;
            aRect.Pos().Y() += rRec.nTol;
            aRect.SSize().Height()-= 2 * rRec.nTol;
            aRect.SSize().Width() -= 2 * rRec.nTol;

            if( aRect.IsInside( rRec.aPos ) )
            {
                if( !rURL.GetMap() ||
                    pFlyFrm->GetFmt()->GetIMapObject( rRec.aPos, pFlyFrm ))
                    return (SdrObject*)this;

                return 0;
            }
        }
    }
    return SdrObject::CheckMacroHit( rRec );
}

bool SwVirtFlyDrawObj::supportsFullDrag() const
{
    // call parent
    return SdrVirtObj::supportsFullDrag();
}

SdrObject* SwVirtFlyDrawObj::getFullDragClone() const
{
    // call parent
    return SdrVirtObj::getFullDragClone();
}

// eof
