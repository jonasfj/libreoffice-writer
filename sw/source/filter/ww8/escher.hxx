/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: escher.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:03:45 $
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#ifndef _ESCHER_HXX
#define _ESCHER_HXX

#ifndef _ESCHEREX_HXX
#include <svx/escherex.hxx>
#endif

const sal_uInt32 nInlineHack = 0x00010001;
class SwFrmFmt;
// --> OD 2005-01-06 #i30669#
class SwFmtHoriOrient;
class SwFmtVertOrient;
// <--

class WinwordAnchoring : public EscherExClientRecord_Base
{
public:
    void WriteData(EscherEx& rEx) const;
    void SetAnchoring(const SwFrmFmt& rFmt);

    /** method to perform conversion of positioning attributes with the help
        of corresponding layout information

        OD 2005-01-06 #i30669#
        Because most of the Writer object positions doesn't correspond to the
        object positions in WW8, this method converts the positioning
        attributes. For this conversion the corresponding layout information
        is needed. If no layout information exists - e.g. no layout exists - no
        conversion is performed.
        No conversion is performed for as-character anchored objects. Whose
        object positions are already treated special in method <WriteData(..)>.
        Usage of method: Used by method <SetAnchoring(..)>, nothing else

        @author OD

        @param _iorHoriOri
        input/output parameter - containing the current horizontal position
        attributes, which are converted by this method.

        @param _iorVertOri
        input/output parameter - containing the current vertical position
        attributes, which are converted by this method.

        @param _rFrmFmt
        input parameter - frame format of the anchored object

        @return boolean, indicating, if a conversion has been performed.
    */
    static bool ConvertPosition( SwFmtHoriOrient& _iorHoriOri,
                                 SwFmtVertOrient& _iorVertOri,
                                 const SwFrmFmt& _rFrmFmt );

private:
    bool mbInline;
    sal_uInt32 mnXAlign;
    sal_uInt32 mnYAlign;
    sal_uInt32 mnXRelTo;
    sal_uInt32 mnYRelTo;

};

class SwBasicEscherEx : public EscherEx
{
private:
    void Init();
protected:
    SwWW8Writer& rWrt;
    SvStream* pEscherStrm;
    SvStream* pPictStrm;
    long mnEmuMul, mnEmuDiv;

    virtual INT32 WriteFlyFrameAttr(const SwFrmFmt& rFmt, MSO_SPT eShapeType,
        EscherPropertyContainer& rPropOpt);
    void WriteBrushAttr(const SvxBrushItem &rBrush,
        EscherPropertyContainer& rPropOpt);
    void WriteOLEPicture(EscherPropertyContainer &rPropOpt,
        sal_uInt32 nShapeFlags, const Graphic &rGraphic, const SdrObject &rObj,
        sal_uInt32 nShapeId);
    void WriteGrfAttr(const SwNoTxtNode& rNd,EscherPropertyContainer& rPropOpt);

    INT32 DrawModelToEmu(INT32 nVal) const
        { return BigMulDiv(nVal, mnEmuMul, mnEmuDiv); }

    INT32 ToFract16(INT32 nVal, UINT32 nMax) const;

    SvStream* QueryPicStream();

    virtual void SetPicId(const SdrObject &, UINT32, EscherPropertyContainer &);
    SdrLayerID GetInvisibleHellId() const
        {return rWrt.pDoc->GetInvisibleHellId();}
public:
    SwBasicEscherEx(SvStream* pStrm, SwWW8Writer& rWrt, UINT32 nDrawings = 1);
    INT32 WriteGrfFlyFrame(const SwFrmFmt& rFmt, UINT32 nShapeId);
    INT32 WriteOLEFlyFrame(const SwFrmFmt& rFmt, UINT32 nShapeId);
    void WriteEmptyFlyFrame(const SwFrmFmt& rFmt, UINT32 nShapeId);
    virtual void WriteFrmExtraData(const SwFrmFmt&);
    virtual void WritePictures();
    virtual ~SwBasicEscherEx();
private:
    //No copying
    SwBasicEscherEx(const SwBasicEscherEx&);
    SwBasicEscherEx& operator=(const SwBasicEscherEx&);
};

class SwEscherEx : public SwBasicEscherEx
{
private:
    SvULongs aFollowShpIds;
    EscherExHostAppData aHostData;
    WinwordAnchoring aWinwordAnchoring;
    WW8_WrPlcTxtBoxes *pTxtBxs;

    UINT32 GetFlyShapeId(const SwFrmFmt& rFmt,
        unsigned int nHdFtIndex, DrawObjPointerVector &rPVec);
    void MakeZOrderArrAndFollowIds(std::vector<DrawObj>& rSrcArr,
        DrawObjPointerVector& rDstArr);

    INT32 WriteFlyFrm(const DrawObj &rObj, UINT32 &rShapeId,
        DrawObjPointerVector &rPVec);
    INT32 WriteTxtFlyFrame(const DrawObj &rObj, UINT32 nShapeId,
        UINT32 nTxtBox, DrawObjPointerVector &rPVec);
    void WriteOCXControl(const SwFrmFmt& rFmt,UINT32 nShapeId);
    virtual INT32 WriteFlyFrameAttr(const SwFrmFmt& rFmt, MSO_SPT eShapeType,
        EscherPropertyContainer& rPropOpt);

    virtual UINT32 QueryTextID(
        const com::sun::star::uno::Reference<
        com::sun::star::drawing::XShape > &,UINT32);
    virtual void SetPicId(const SdrObject &rSdrObj, UINT32 nShapeId,
        EscherPropertyContainer &rPropOpt);
public:
    SwEscherEx( SvStream* pStrm, SwWW8Writer& rWW8Wrt );
    virtual ~SwEscherEx();
    void FinishEscher();
    virtual void WritePictures();

    virtual void WriteFrmExtraData(const SwFrmFmt& rFmt);

    EscherExHostAppData* StartShape(const com::sun::star::uno::Reference<
        com::sun::star::drawing::XShape > &) {return &aHostData;}
private:
    //No copying
    SwEscherEx(const SwEscherEx&);
    SwEscherEx &operator=(const SwEscherEx&);
};

#endif

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
