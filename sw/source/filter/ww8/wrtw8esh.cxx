/*************************************************************************
 *
 *  $RCSfile: wrtw8esh.cxx,v $
 *
 *  $Revision: 1.41 $
 *
 *  last change: $Author: cmc $ $Date: 2002-08-14 09:29:35 $
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

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#define _SVSTDARR_ULONGSSORT
#include <svtools/svstdarr.hxx>

#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SVSTOR_HXX
#include <so3/svstor.hxx>
#endif
#ifndef _IPOBJ_HXX
#include <so3/ipobj.hxx>
#endif
#ifndef _FILTER_HXX
#include <svtools/filter.hxx>
#endif
#ifndef _SFXITEMITER_HXX
#include <svtools/itemiter.hxx>
#endif
#ifndef _SVDOBJ_HXX
#include <svx/svdobj.hxx>
#endif
#ifndef _SVDOTEXT_HXX
#include <svx/svdotext.hxx>
#endif
#ifndef _SVDMODEL_HXX
#include <svx/svdmodel.hxx>
#endif
#ifndef _SVDPAGE_HXX
#include <svx/svdpage.hxx>
#endif
#ifndef _OUTLOBJ_HXX
#include <svx/outlobj.hxx>
#endif
#ifndef _EDITOBJ_HXX
#include <svx/editobj.hxx>
#endif
#ifndef _ESCHEREX_HXX
#include <svx/escherex.hxx>
#endif
#ifndef _SVX_UNOSHAPE_HXX
#include <svx/unoshape.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX
#include <svx/fontitem.hxx>
#endif
#ifndef _MyEDITENG_HXX
#include <svx/editeng.hxx>
#endif
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif

#ifndef _SVX_FMGLOB_HXX
#include <svx/fmglob.hxx>
#endif
#ifndef _SVDOUNO_HXX
#include <svx/svdouno.hxx>
#endif
#ifndef _SVX_UNOAPI_HXX_
#include <svx/unoapi.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif

#ifndef _WRTWW8_HXX
#include <wrtww8.hxx>
#endif
#ifndef _FLYPOS_HXX
#include <flypos.hxx>
#endif
#ifndef _FMTCNCT_HXX
#include <fmtcnct.hxx>
#endif
#ifndef _FMTANCHR_HXX
#include <fmtanchr.hxx>
#endif
#ifndef _FMTSRND_HXX
#include <fmtsrnd.hxx>
#endif
#ifndef _FMTORNT_HXX
#include <fmtornt.hxx>
#endif
#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _FMTCNTNT_HXX
#include <fmtcntnt.hxx>
#endif
#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _SWRECT_HXX
#include <swrect.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _GRFATR_HXX
#include <grfatr.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _UNODRAW_HXX
#include <unodraw.hxx>
#endif
#ifndef _WW8PAR_HXX
#include <ww8par.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _GRFMGR_HXX
#include <goodies/grfmgr.hxx>
#endif

using namespace ::com::sun::star;

#define sEscherStream CREATE_CONST_ASC("tempEsher")
#define sEscherPictStream 	CREATE_CONST_ASC("EsherPicts")

PlcDrawObj::~PlcDrawObj()
{
}

void PlcDrawObj::WritePlc(SwWW8Writer& rWrt) const
{
    if (8 > rWrt.pFib->nVersion)    // Cannot export drawobject in vers 7-
        return;

    sal_uInt32 nFcStart = rWrt.pTableStrm->Tell();

    if (!maDrawObjs.empty())
    {
        // write CPs
        WW8Fib& rFib = *rWrt.pFib;
        WW8_CP nCpOffs = GetCpOffset(rFib);

        typedef ::std::vector<DrawObj>::const_iterator myiter;
        myiter aEnd = maDrawObjs.end();
        myiter aIter;

        for (aIter = maDrawObjs.begin(); aIter < aEnd; ++aIter)
            SwWW8Writer::WriteLong(*rWrt.pTableStrm, aIter->mnCp - nCpOffs);

        SwWW8Writer::WriteLong(*rWrt.pTableStrm, rFib.ccpText + rFib.ccpFtn +
            rFib.ccpHdr + rFib.ccpEdn + rFib.ccpTxbx + rFib.ccpHdrTxbx + 1);

        for (aIter = maDrawObjs.begin(); aIter < aEnd; ++aIter)
        {
            // write the fspa-struct
            const SwFrmFmt& rFmt = aIter->mrCntnt;
            const SdrObject* pObj = rFmt.FindRealSdrObject();

            Rectangle aRect;
            const SwFmtVertOrient& rVOr = rFmt.GetVertOrient();
            const SwFmtHoriOrient& rHOr = rFmt.GetHoriOrient();
            if (RES_FLYFRMFMT == rFmt.Which())
            {
                Point aObjPos;
                SwRect aLayRect( rFmt.FindLayoutRect( FALSE, &aObjPos ));
                // the Object is not visible - so get the values from
                // the format. The Position may not be correct.
                if( aLayRect.IsEmpty() )
                    aRect.SetSize( rFmt.GetFrmSize().GetSize() );
                else
                    aRect = aLayRect.SVRect();

                aRect -= aIter->maParentPos;
                aObjPos = aRect.TopLeft();
                if (VERT_NONE == rVOr.GetVertOrient())
                    aObjPos.Y() = rVOr.GetPos();
                if (HORI_NONE == rHOr.GetHoriOrient())
                    aObjPos.X() = rHOr.GetPos();
                aRect.SetPos( aObjPos );
            }
            else
            {
                ASSERT(pObj, "wo ist das SDR-Object?");
                if (pObj)
                    aRect = pObj->GetSnapRect();
                aRect -= aIter->maParentPos;
            }

            // spid
            SwWW8Writer::WriteLong(*rWrt.pTableStrm, aIter->mnShapeId);

            //xaLeft/yaTop/xaRight/yaBottom - rel. to anchor
            //(most of) the border is outside the graphic is word, so
            //change dimensions to fit
            SwWW8Writer::WriteLong(*rWrt.pTableStrm,aRect.Left() + 
                aIter->mnThick);
            SwWW8Writer::WriteLong(*rWrt.pTableStrm,aRect.Top() + 
                aIter->mnThick);
            SwWW8Writer::WriteLong(*rWrt.pTableStrm,aRect.Right() - 
                aIter->mnThick);
            SwWW8Writer::WriteLong(*rWrt.pTableStrm,aRect.Bottom() - 
                aIter->mnThick);

            //fHdr/bx/by/wr/wrk/fRcaSimple/fBelowText/fAnchorLock
            USHORT nFlags=0;
            //If nFlags isn't 0x14 its overridden by the escher properties
            if( FLY_PAGE == rFmt.GetAnchor().GetAnchorId())
                nFlags = 0x0000;
            else
                nFlags = 0x0014;		// x-rel to text,  y-rel to text

            const SwFmtSurround& rSurr = rFmt.GetSurround();
            USHORT nContour = rSurr.IsContour() ? 0x0080 : 0x0040;
            switch (rSurr.GetSurround())
            {
                case SURROUND_NONE:
                    nFlags |= 0x0020;
                    break;
                case SURROUND_THROUGHT:
                    nFlags |= 0x0060;
                    break;
                case SURROUND_PARALLEL:
                    nFlags |= 0x0000 | nContour;
                    break;
                case SURROUND_IDEAL:
                    nFlags |= 0x0600 | nContour;
                    break;
                case SURROUND_LEFT:
                    nFlags |= 0x0200 | nContour;
                    break;
                case SURROUND_RIGHT:
                    nFlags |= 0x0400 | nContour;
                    break;
            }
            if( pObj && pObj->GetLayer() == rWrt.pDoc->GetHellId() )
                nFlags |= 0x4000;

            SwWW8Writer::WriteShort(*rWrt.pTableStrm, nFlags);

            // cTxbx
            SwWW8Writer::WriteLong(*rWrt.pTableStrm, 0);
        }

        RegisterWithFib(rFib, nFcStart, rWrt.pTableStrm->Tell() - nFcStart);
    }
}

void MainTxtPlcDrawObj::RegisterWithFib(WW8Fib &rFib, sal_uInt32 nStart, 
    sal_uInt32 nLen) const
{
    rFib.fcPlcfspaMom = nStart;
    rFib.lcbPlcfspaMom = nLen;
}

WW8_CP MainTxtPlcDrawObj::GetCpOffset(const WW8Fib &rFib) const
{
    return 0;
}

void HdFtPlcDrawObj::RegisterWithFib(WW8Fib &rFib, sal_uInt32 nStart, 
    sal_uInt32 nLen) const
{
    rFib.fcPlcfspaHdr = nStart;
    rFib.lcbPlcfspaHdr = nLen;
}

WW8_CP HdFtPlcDrawObj::GetCpOffset(const WW8Fib &rFib) const
{
    return rFib.ccpText + rFib.ccpFtn;
}

bool PlcDrawObj::Append(SwWW8Writer& rWrt, WW8_CP nCp, const SwFrmFmt& rFmt, 
    const Point& rNdTopLeft)
{
    bool bRet = FALSE;
    if (TXT_HDFT == rWrt.nTxtTyp || TXT_MAINTEXT == rWrt.nTxtTyp)
    {
        if (RES_FLYFRMFMT == rFmt.Which())
        {
            // check for textflyframe and if it is the first in a Chain
            if (rFmt.GetCntnt().GetCntntIdx())
                bRet = true;
        }
        else
            bRet = true;
    }

    if (bRet)
        maDrawObjs.push_back(
            DrawObj(rFmt,nCp,rNdTopLeft,rWrt.TrueFrameDirection(rFmt)));
    return bRet;
}

class HasThisFrame: public std::unary_function<const DrawObj &, bool>
{
private:
    const SwFrmFmt& mrFmt;
public:
    HasThisFrame(const SwFrmFmt& rFmt) : mrFmt(rFmt) {}
    bool operator()(const DrawObj &rIn) const
    {
        return (&rIn.mrCntnt == &mrFmt);
    }
};

void PlcDrawObj::SetShapeDetails( const SwFrmFmt& rFmt, UINT32 nId,
    INT32 nThick )
{
    typedef ::std::vector<DrawObj>::iterator myiter;
    myiter aEnd = maDrawObjs.end();
    myiter aIter = ::std::find_if(maDrawObjs.begin(), aEnd, HasThisFrame(rFmt));
    if (aIter != aEnd)
    {
        aIter->mnShapeId = nId;
        aIter->mnThick = nThick;
    }
}

/*  */

BOOL WW8_WrPlcTxtBoxes::WriteTxt( SwWW8Writer& rWrt )
{
    BOOL bRet=FALSE;
    rWrt.bInWriteEscher = TRUE;
    long& rccp=TXT_TXTBOX == nTyp ? rWrt.pFib->ccpTxbx : rWrt.pFib->ccpHdrTxbx;

    bRet = WriteGenericTxt( rWrt, nTyp, rccp );

    WW8_CP nCP = rWrt.Fc2Cp( rWrt.Strm().Tell() );
    WW8Fib& rFib = *rWrt.pFib;
    long nMyOffset = rFib.ccpText + rFib.ccpFtn + rFib.ccpHdr + rFib.ccpAtn
                            + rFib.ccpEdn;
    if( TXT_TXTBOX == nTyp )
        rWrt.pFldTxtBxs->Finish( nCP, nMyOffset );
    else
        rWrt.pFldHFTxtBxs->Finish( nCP, nMyOffset + rFib.ccpTxbx );
    rWrt.bInWriteEscher = FALSE;
    return bRet;
}

void WW8_WrPlcTxtBoxes::Append( const SdrObject& rObj, UINT32 nShapeId )
{
    void* p = (void*)&rObj;
    aCntnt.Insert( p, aCntnt.Count() );
    aShapeIds.Insert( nShapeId, aShapeIds.Count() );
}

const SvULongs* WW8_WrPlcTxtBoxes::GetShapeIdArr() const
{
    return &aShapeIds;
}

/*  */

UINT32 SwWW8Writer::GetSdrOrdNum( const SwFrmFmt& rFmt ) const
{
    UINT32 nOrdNum;
    const SdrObject* pObj = rFmt.FindRealSdrObject();
    if( pObj )
        nOrdNum = pObj->GetOrdNum();
    else
    {
        // no Layout for this format, then recalc the ordnum
        SwFrmFmt* pFmt = (SwFrmFmt*)&rFmt;
        nOrdNum = pDoc->GetSpzFrmFmts()->GetPos( pFmt );

        const SdrModel* pModel = pDoc->GetDrawModel();
        if( pModel )
            nOrdNum += pModel->GetPage( 0 )->GetObjCount();
    }
    return nOrdNum;
}

static BOOL lcl_IsFlyInFlyHere(const SwFrmFmt* pFmt, ULONG nStart, ULONG nEnd)
{
    BOOL bRet = FALSE;
    const SwFmtAnchor* pAnchor = &pFmt->GetAnchor();
    const SwPosition* pAPos;
    ULONG nIdx;
    if( ( pAnchor->GetAnchorId() == FLY_AT_CNTNT ||
        pAnchor->GetAnchorId() == FLY_AT_FLY ||
        pAnchor->GetAnchorId() == FLY_AUTO_CNTNT ) &&
        0 != ( pAPos = pAnchor->GetCntntAnchor()) &&
        nStart <= ( nIdx = pAPos->nNode.GetIndex()) &&
        nIdx < nEnd )
    {
        bRet = TRUE;
    }
    return bRet;
}

void SwWW8Writer::AppendFlyInFlys( WW8_CP& rCP, const SwFrmFmt& rFrmFmt,
                                    const Point& rNdTopLeft )
{
    ASSERT( !pEscher, "der EscherStream wurde schon geschrieben!" );
    if( pEscher )
        return ;
    BOOL bExportAsTable = FALSE;
    USHORT nArrLen = 0, nLastFmt;
    ULONG nStart, nEnd;

    if( RES_FLYFRMFMT == rFrmFmt.Which() )
    {
        const SwNodeIndex* pNdIdx = rFrmFmt.GetCntnt().GetCntntIdx();
        ASSERT( pNdIdx, "wo ist der NodeIndex geblieben?" );
        nStart = pNdIdx->GetIndex();
        nEnd = pNdIdx->GetNode().EndOfSectionIndex();
        nArrLen = pDoc->GetSpzFrmFmts()->Count();
        for( nLastFmt = 0; nLastFmt < nArrLen; ++nLastFmt )
        {
            if (lcl_IsFlyInFlyHere(
                (*pDoc->GetSpzFrmFmts())[nLastFmt],nStart,nEnd))
            {
                bExportAsTable = TRUE;
                break;
            }
        }
    }

    if (bExportAsTable)
    {
        SwTwips nTblSz = rFrmFmt.GetFrmSize().GetWidth();
        SwTwips nTblOffset=0;
        WW8Bytes aAt( 128, 128 );
        USHORT nStdAtLen = StartTableFromFrmFmt(aAt,&rFrmFmt,nTblOffset);

        static BYTE __READONLY_DATA aNullBytes[] = { 0, 0, 0, 0 };
        BYTE nWWColMax = 1;

        if( bWrtWW8 )
            SwWW8Writer::InsUInt16( aAt, 0x3404 );
        else
            aAt.Insert( 186, aAt.Count() );
        aAt.Insert( 1, aAt.Count() );

        long nHeight=0;
        const SwFmtFrmSize& rLSz = rFrmFmt.GetFrmSize();
        if( ATT_VAR_SIZE != rLSz.GetSizeType() && rLSz.GetHeight() )
            nHeight = ATT_MIN_SIZE == rLSz.GetSizeType()
                ? rLSz.GetHeight() : -rLSz.GetHeight();

        if( nHeight )
        {
            if( bWrtWW8 )
                SwWW8Writer::InsUInt16( aAt, 0x9407 );
            else
                aAt.Insert( 189, aAt.Count() );
            SwWW8Writer::InsUInt16( aAt, (USHORT)nHeight );
        }

        if( bWrtWW8 )
            SwWW8Writer::InsUInt16( aAt, 0x3403 );
        else
            aAt.Insert( 185, aAt.Count() );
        aAt.Insert( (BYTE)0, aAt.Count() );

        const SwNodeIndex* pNodeIndex = rFrmFmt.GetCntnt().GetCntntIdx();
        ULONG nNodeStt = pNodeIndex ? pNodeIndex->GetIndex()+1 : 0;
        ULONG nNodeEnd = pNodeIndex ? 
            pNodeIndex->GetNode().EndOfSectionIndex() : 0;

        WW8SaveData aSaveData(*this,nNodeStt,nNodeEnd);
        bOutTable = TRUE;
        bIsInTable= TRUE;
        WriteText();
        WriteCellEnd();
        WriteRowEnd();

        if( bWrtWW8 )
        {
            SwWW8Writer::InsUInt16( aAt, 0xD608 );
            SwWW8Writer::InsUInt16( aAt, 2 + ( nWWColMax + 1 ) * 2 +
                ( nWWColMax * 20 ));
            aAt.Insert( nWWColMax, aAt.Count() );
        }
        else
        {
            aAt.Insert( 190, aAt.Count() );
            SwWW8Writer::InsUInt16( aAt, nWWColMax * 12 + 4 );
            aAt.Insert( nWWColMax, aAt.Count() );
        }
        SwWW8Writer::InsUInt16( aAt, (USHORT)nTblOffset );

        SwTwips nCalc = rFrmFmt.GetFrmSize().GetWidth();
        SwWW8Writer::InsUInt16( aAt, (USHORT)(nTblOffset + nCalc ));

        if( bWrtWW8 )
        {
            USHORT nFlags = 0;
            SwWW8Writer::InsUInt16( aAt, nFlags );
            aAt.Insert( aNullBytes, 2, aAt.Count() );
            Out_SwFmtTableBox( aAt, rFrmFmt.GetBox() );

            static USHORT __READONLY_DATA aBorders[] =
            {
                BOX_LINE_TOP, BOX_LINE_LEFT,
                BOX_LINE_BOTTOM, BOX_LINE_RIGHT
            };
            const USHORT* pBrd = aBorders;

            for( int i = 0; i < 4; ++i, ++pBrd)
            {
                SwWW8Writer::InsUInt16(aAt, 0xD634);
                aAt.Insert( BYTE(6), aAt.Count() );
                aAt.Insert( BYTE(0), aAt.Count() );
                aAt.Insert( BYTE(3), aAt.Count() );
                aAt.Insert( BYTE(1 << i), aAt.Count() );
                aAt.Insert( BYTE(3), aAt.Count() );
                SwWW8Writer::InsUInt16(aAt,
                    rFrmFmt.GetBox().GetDistance(*pBrd));
            }
        }

        pPapPlc->AppendFkpEntry( Strm().Tell(),
                aAt.Count(), aAt.GetData() );

        if( aAt.Count() > nStdAtLen )
            aAt.Remove( nStdAtLen, aAt.Count() - nStdAtLen );
    }
    else
    {
        PlcDrawObj *pDrwO;
        if (TXT_HDFT == nTxtTyp) 
            pDrwO = pHFSdrObjs;
        else
            pDrwO = pSdrObjs;

        if (pDrwO->Append( *this, rCP, rFrmFmt, rNdTopLeft))
        {
            static BYTE __READONLY_DATA aSpec8[] = {
                0x03, 0x6a, 0, 0, 0, 0,	// sprmCObjLocation 
                0x55, 0x08, 1			// sprmCFSpec
            };
                                                    // fSpec-Attribut TRUE
                                // Fuer DrawObjets muss ein Spezial-Zeichen
                                // in den Text und darum ein fSpec-Attribut
            pChpPlc->AppendFkpEntry( Strm().Tell() );
            WriteChar( 0x8 );
            rCP += 1;		// to next charakter position
            pChpPlc->AppendFkpEntry( Strm().Tell(), sizeof( aSpec8 ), aSpec8 );

            if( RES_FLYFRMFMT == rFrmFmt.Which() )
            {
                // search all Flys/DrawObj in Flys and put it after this text
                // position. The test to change the parent fly frame into a
                // table will have left nLastFmt pointing to the first frame
                // to begin exporting
                for( ; nLastFmt < nArrLen; ++nLastFmt )
                {
                    const SwFrmFmt* pFmt = (*pDoc->GetSpzFrmFmts())[nLastFmt];
                    if (lcl_IsFlyInFlyHere(pFmt,nStart,nEnd))
                        AppendFlyInFlys( rCP, *pFmt, rNdTopLeft );
                }
            }
        }
    }
}

class WW8_SdrAttrIter : public WW8_AttrIter
{
private:
    const EditTextObject* pEditObj;
    const SfxItemPool* pEditPool;
    EECharAttribArray aTxtAtrArr;
    SvPtrarr aChrTxtAtrArr;
    SvUShorts aChrSetArr;
    USHORT nPara;
    xub_StrLen nAktSwPos;
    xub_StrLen nTmpSwPos;					// fuer HasItem()
    rtl_TextEncoding eNdChrSet;
    USHORT nScript;

    xub_StrLen SearchNext( xub_StrLen nStartPos );
    void SetCharSet( const EECharAttrib& rTxtAttr, BOOL bStart );

    //No copying
    WW8_SdrAttrIter(const WW8_SdrAttrIter&);
    WW8_SdrAttrIter& operator=(const WW8_SdrAttrIter&);
public:
    WW8_SdrAttrIter( SwWW8Writer& rWr, const EditTextObject& rEditObj );
    void NextPara( USHORT nPar );
    void OutParaAttr( BOOL bCharAttr );

    BOOL IsTxtAttr( xub_StrLen nSwPos );

    void NextPos() { nAktSwPos = SearchNext( nAktSwPos + 1 ); }

    void OutAttr( xub_StrLen nSwPos );
    virtual const SfxPoolItem* HasTextItem( USHORT nWhich ) const;
    virtual const SfxPoolItem& GetItem( USHORT nWhich ) const;
    BOOL OutAttrWithRange( xub_StrLen nPos );
    xub_StrLen WhereNext() const				{ return nAktSwPos; }
    rtl_TextEncoding GetNextCharSet() const;
    rtl_TextEncoding GetNodeCharSet() const		{ return eNdChrSet; }
};


WW8_SdrAttrIter::WW8_SdrAttrIter( SwWW8Writer& rWr,
                                    const EditTextObject& rEditObj )
    : WW8_AttrIter( rWr ), pEditObj( &rEditObj ),
    aTxtAtrArr( 0, 4 ), aChrTxtAtrArr( 0, 4 ), aChrSetArr( 0, 4 ),
    pEditPool( 0 )
{
    NextPara( 0 );
}

void WW8_SdrAttrIter::NextPara( USHORT nPar )
{
    nPara = nPar;
    // Attributwechsel an Pos 0 wird ignoriert, da davon ausgegangen
    // wird, dass am Absatzanfang sowieso die Attribute neu ausgegeben
    // werden.
    aChrTxtAtrArr.Remove( 0, aChrTxtAtrArr.Count() );
    aChrSetArr.Remove( 0, aChrSetArr.Count() );
    nAktSwPos = nTmpSwPos = 0;

    SfxItemSet aSet( pEditObj->GetParaAttribs( nPara ));
    pEditPool = aSet.GetPool();
    eNdChrSet = ((SvxFontItem&)aSet.Get( EE_CHAR_FONTINFO )).GetCharSet();

    if( pBreakIt->xBreak.is() )
        nScript = pBreakIt->xBreak->getScriptType( pEditObj->GetText(nPara), 0);
    else
        nScript = i18n::ScriptType::LATIN;

    pEditObj->GetCharAttribs( nPara, aTxtAtrArr );
    nAktSwPos = SearchNext( 1 );
}

rtl_TextEncoding WW8_SdrAttrIter::GetNextCharSet() const
{
    if( aChrSetArr.Count() )
        return (rtl_TextEncoding)aChrSetArr[ aChrSetArr.Count() - 1 ];
    return eNdChrSet;
}

// der erste Parameter in SearchNext() liefert zurueck, ob es ein TxtAtr ist.
xub_StrLen WW8_SdrAttrIter::SearchNext( xub_StrLen nStartPos )
{
    register xub_StrLen nPos;
    register xub_StrLen nMinPos = STRING_MAXLEN;
    register xub_StrLen i;

    for( i = 0; i < aTxtAtrArr.Count(); i++ )
    {
        const EECharAttrib& rHt = aTxtAtrArr[ i ];
        nPos = rHt.nStart;	// gibt erstes Attr-Zeichen
        if( nPos >= nStartPos && nPos <= nMinPos )
        {
            nMinPos = nPos;
            SetCharSet( rHt, TRUE );
        }

//??		if( pHt->GetEnd() )			// Attr mit Ende
        {
            nPos = rHt.nEnd;		// gibt letztes Attr-Zeichen + 1
            if( nPos >= nStartPos && nPos < nMinPos )
            {
                nMinPos = nPos;
                SetCharSet( rHt, FALSE );
            }
        }
/*		else
        {									// Attr ohne Ende
            nPos = rHt.nStart + 1;	// Laenge 1 wegen CH_TXTATR im Text
            if( nPos >= nStartPos && nPos < nMinPos )
            {
                nMinPos = nPos;
                SetCharSet( rHt, FALSE );
            }
        }
*/
    }
    return nMinPos;
}

void WW8_SdrAttrIter::SetCharSet( const EECharAttrib& rAttr, BOOL bStart )
{
    void* p = 0;
    rtl_TextEncoding eChrSet;
    const SfxPoolItem& rItem = *rAttr.pAttr;
    switch( rItem.Which() )
    {
    case EE_CHAR_FONTINFO:
        p = (void*)&rAttr;
        eChrSet = ((SvxFontItem&)rItem).GetCharSet();
        break;
    }

    if( p )
    {
        USHORT nPos;
        if( bStart )
        {
            nPos = aChrSetArr.Count();
            aChrSetArr.Insert( eChrSet, nPos );
            aChrTxtAtrArr.Insert( p, nPos );
        }
        else if( USHRT_MAX != ( nPos = aChrTxtAtrArr.GetPos( p )) )
        {
            aChrTxtAtrArr.Remove( nPos );
            aChrSetArr.Remove( nPos );
        }
    }
}

void WW8_SdrAttrIter::OutAttr( xub_StrLen nSwPos )
{
    OutParaAttr( TRUE );

    if( aTxtAtrArr.Count() )
    {
        const SwModify* pOldMod = rWrt.pOutFmtNode;
        rWrt.pOutFmtNode = 0;

        const SfxItemPool* pSrcPool = pEditPool,
                            * pDstPool = &rWrt.pDoc->GetAttrPool();

        nTmpSwPos = nSwPos;
        register USHORT i, nWhich, nSlotId;
        FnAttrOut pOut;
        for( i = 0; i < aTxtAtrArr.Count(); i++ )
        {
            const EECharAttrib& rHt = aTxtAtrArr[ i ];
            if( nSwPos >= rHt.nStart && nSwPos < rHt.nEnd &&
                0 != ( nSlotId = pSrcPool->GetSlotId(
                                        nWhich = rHt.pAttr->Which() )) &&
                nWhich != nSlotId &&
                0 != ( nWhich = pDstPool->GetWhich( nSlotId ) ) &&
                nWhich != nSlotId &&
                0 != ( pOut = aWW8AttrFnTab[ nWhich - RES_CHRATR_BEGIN ] ) )
            {
                if (rWrt.CollapseScriptsforWordOk(nScript,nWhich))
                {
                    // use always the SW-Which Id !
                    SfxPoolItem* pI = rHt.pAttr->Clone();
                    pI->SetWhich( nWhich );
                    (*pOut)( rWrt, *pI );
                    delete pI;
                }
            }
            else if( nSwPos < rHt.nStart )
                break;
        }

        nTmpSwPos = 0;		// HasTextItem nur in dem obigen Bereich erlaubt
        rWrt.pOutFmtNode = pOldMod;
    }
}

BOOL WW8_SdrAttrIter::IsTxtAttr(xub_StrLen )
{
// so long as the EditEngine has no Attributes, then there is only one
// position to have 
    return FALSE;
}

// HasItem ist fuer die Zusammenfassung des Doppel-Attributes Underline
// und WordLineMode als TextItems. OutAttr() ruft die Ausgabefunktion,
// die dann ueber HasItem() nach anderen Items an der
// Attribut-Anfangposition fragen kann.
// Es koennen nur Attribute mit Ende abgefragt werden.
// Es wird mit bDeep gesucht
const SfxPoolItem* WW8_SdrAttrIter::HasTextItem( USHORT nWhich ) const
{
    const SfxPoolItem* pRet = 0;
    USHORT nSlotId = rWrt.pDoc->GetAttrPool().GetSlotId( nWhich );
    if( nSlotId && nWhich != nSlotId &&
        0 != ( nWhich = pEditPool->GetWhich( nSlotId ) ) &&
        nWhich != nSlotId )
    {
        register USHORT i;
        for( i = 0; i < aTxtAtrArr.Count(); ++i )
        {
            const EECharAttrib& rHt = aTxtAtrArr[ i ];
            if( nWhich == rHt.pAttr->Which() &&
                nTmpSwPos >= rHt.nStart && nTmpSwPos < rHt.nEnd )
            {
                pRet = rHt.pAttr;		// gefunden
                break;
            }
            else if( nTmpSwPos < rHt.nStart )
                break;				// dann kommt da nichts mehr
        }
    }
    return pRet;
}

const SfxPoolItem& WW8_SdrAttrIter::GetItem( USHORT nWhich ) const
{
    const SfxPoolItem* pRet = HasTextItem( nWhich );
    if( !pRet )
    {
        SfxItemSet aSet( pEditObj->GetParaAttribs( nPara ));

        USHORT nNewW, nSlotId = rWrt.pDoc->GetAttrPool().GetSlotId( nWhich );
        if( nSlotId && nWhich != nSlotId &&
            0 != ( nNewW = aSet.GetPool()->GetWhich( nSlotId ) ) &&
            nNewW != nSlotId )
            pRet = &aSet.Get( nNewW );
    }
    return *pRet;
}

void WW8_SdrAttrIter::OutParaAttr( BOOL bCharAttr )
{
    SfxItemSet aSet( pEditObj->GetParaAttribs( nPara ));
    if( aSet.Count() )
    {
        const SfxItemSet* pOldSet = rWrt.GetCurItemSet();
        rWrt.SetCurItemSet( &aSet );

        SfxItemIter aIter( aSet );
        const SfxPoolItem* pItem = aIter.GetCurItem();
        FnAttrOut pOut;

        const SfxItemPool* pSrcPool = pEditPool,
                            * pDstPool = &rWrt.pDoc->GetAttrPool();

        do {
                USHORT nWhich = pItem->Which(),
                       nSlotId = pSrcPool->GetSlotId( nWhich );
                if( nSlotId && nWhich != nSlotId &&
                    0 != ( nWhich = pDstPool->GetWhich( nSlotId ) ) &&
                    nWhich != nSlotId &&
                    0 != ( pOut = aWW8AttrFnTab[ nWhich - RES_CHRATR_BEGIN ] )
                    && ( bCharAttr ? ( nWhich >= RES_CHRATR_BEGIN
                                      && nWhich < RES_TXTATR_END)
                                   : (nWhich >= RES_PARATR_BEGIN
                                      && nWhich < RES_FRMATR_END) ) )
                {
                    // use always the SW-Which Id !
                    SfxPoolItem* pI = pItem->Clone();
                    pI->SetWhich( nWhich );
                    if (rWrt.CollapseScriptsforWordOk(nScript,nWhich))
                        (*pOut)( rWrt, *pI );
                    delete pI;
                }

        } while( !aIter.IsAtEnd() && 0 != ( pItem = aIter.NextItem() ) );
        rWrt.SetCurItemSet( pOldSet );
    }
}

void SwWW8Writer::WriteSdrTextObj( const SdrObject& rObj )
{
    const SdrTextObj* pTxtObj = PTR_CAST( SdrTextObj, &rObj );
    ASSERT( pTxtObj, "das ist gar kein SdrTextObj!" );

    BOOL bAnyWrite = FALSE;
    const OutlinerParaObject* pParaObj = pTxtObj->GetOutlinerParaObject();
    if( pParaObj )
    {
        const EditTextObject& rEditObj = pParaObj->GetTextObject();
        WW8_SdrAttrIter aAttrIter( *this, rEditObj );

        USHORT nPara = rEditObj.GetParagraphCount();
        BYTE bNul = 0;
        for( USHORT n = 0; n < nPara; ++n )
        {
            if( n )
                aAttrIter.NextPara( n );

            rtl_TextEncoding eChrSet = aAttrIter.GetNodeCharSet();

            ASSERT( !pO->Count(), " pO ist am Zeilenanfang nicht leer" );

            String aStr( rEditObj.GetText( n ));
            xub_StrLen nAktPos = 0;
            xub_StrLen nEnd = aStr.Len();
            BOOL bUnicode = TRUE;
            do {
/*
                switch( eChrSet )
                {
        //		case CHARSET_DONTKNOW:		 ????
                case CHARSET_SYMBOL:
        //		case CHARSET_WIN_SYMBOL:
        //		case CHARSET_WIN_WINGDINGS:
        //		case CHARSET_MAC_DINGBATS:
        //		case CHARSET_MAC_SYMBOL:
        //		case CHARSET_ADOBE_SYMBOL:
        //		case CHARSET_ADOBE_DINGBATS:
        //		case CHARSET_STAR_STARBATS:
        //		case CHARSET_STAR_STARMATH:
                    bUnicode = bWrtWW8 ? TRUE : FALSE;
                    break;
                default:
                    bUnicode = FALSE;
                    break;
                }
*/
                xub_StrLen nNextAttr = aAttrIter.WhereNext();
                rtl_TextEncoding eNextChrSet = aAttrIter.GetNextCharSet();

                if( nNextAttr > nEnd )
                    nNextAttr = nEnd;

                BOOL bTxtAtr = aAttrIter.IsTxtAttr( nAktPos );
                if( !bTxtAtr )
                    OutSwString( aStr, nAktPos, nNextAttr - nAktPos,
                                    bUnicode, eChrSet );

                            // Am Zeilenende werden die Attribute bis ueber das CR
                            // aufgezogen. Ausnahme: Fussnoten am Zeilenende
                if( nNextAttr == nEnd && !bTxtAtr )
                    WriteCR();				// CR danach

                                                // Ausgabe der Zeichenattribute
                aAttrIter.OutAttr( nAktPos );	// nAktPos - 1 ??
                pChpPlc->AppendFkpEntry( Strm().Tell(),
                                                pO->Count(), pO->GetData() );
                pO->Remove( 0, pO->Count() );					// leeren

                            // Ausnahme: Fussnoten am Zeilenende
                if( nNextAttr == nEnd && bTxtAtr )
                    WriteCR();				// CR danach

                nAktPos = nNextAttr;
                eChrSet = eNextChrSet;
                aAttrIter.NextPos();
            }
            while( nAktPos < nEnd );

            ASSERT( !pO->Count(), " pO ist am ZeilenEnde nicht leer" );

            pO->Insert( bNul, pO->Count() );		// Style # as short
            pO->Insert( bNul, pO->Count() );

            aAttrIter.OutParaAttr( FALSE );

            ULONG nPos = Strm().Tell();
            pPapPlc->AppendFkpEntry( Strm().Tell(),
                                            pO->Count(), pO->GetData() );
            pO->Remove( 0, pO->Count() );						// leeren
            pChpPlc->AppendFkpEntry( nPos );
        }
        bAnyWrite = 0 != nPara;
    }
    if( !bAnyWrite )
        WriteStringAsPara( aEmptyStr );
}

/*  */
class WinwordAnchoring : public EscherExClientRecord_Base
{
public:
    void WriteData( EscherEx& rEx ) const;
    void SetAnchoring( const SwFrmFmt& rFmt, BOOL bBROKEN=FALSE );
private:
    UINT32 nXAlign;
    UINT32 nYAlign;
    UINT32 nXRelTo;
    UINT32 nYRelTo;
};

class SwEscherEx : public  EscherEx
{
private:
    SvULongs aFollowShpIds;
    std::vector<short> maDirections;
    SvPtrarr aSortFmts;
    SwWW8Writer& rWrt;
    EscherExHostAppData aHostData;
    WinwordAnchoring aWinwordAnchoring;
    WW8_WrPlcTxtBoxes *pTxtBxs;
    SvStream* pEscherStrm, *pPictStrm;
    long nEmuMul, nEmuDiv;

    void Init();
    UINT32 GetFlyShapeId( const SwFrmFmt& rFmt );
    void MakeZOrderArrAndFollowIds( const ::std::vector<DrawObj>& rSrcArr );

    INT32 ToFract16( INT32 nVal, UINT32 nMax ) const
        {
            if( nMax )
            {
                INT32 nMSVal = ( nVal / 65536) * nMax;
                nMSVal += ( nVal * 65536 ) / nMax;
                return nMSVal;
            }
            return 0;
        }
    INT32 DrawModelToEmu( INT32 nVal ) const
        { return BigMulDiv( nVal, nEmuMul, nEmuDiv ); }

    INT32 WriteFlyFrm(const SwFrmFmt& rFmt,UINT32 &rShapeId, short nDirection);
    INT32 WriteTxtFlyFrame(const SwFrmFmt& rFmt,UINT32 nShapeId,
        UINT32 nTxtBox, short nDirection);
    INT32 WriteGrfFlyFrame(const SwFrmFmt& rFmt,UINT32 nShapeId);
    INT32 WriteOLEFlyFrame(const SwFrmFmt& rFmt,UINT32 nShapeId);
    INT32 WriteFlyFrameAttr(const SwFrmFmt& rFmt,MSO_SPT eShapeType,
        EscherPropertyContainer& rPropOpt);
    void WriteGrfAttr(const SwNoTxtNode& rNd,EscherPropertyContainer& rPropOpt);
    void WriteOCXControl(const SwFrmFmt& rFmt,UINT32 nShapeId);

    virtual SvStream* QueryPicStream();
    virtual	UINT32 QueryTextID( const uno::Reference< drawing::XShape>&,UINT32);

    //No copying
    SwEscherEx(const SwEscherEx&);
    SwEscherEx &operator=(const SwEscherEx&);
public:
    SwEscherEx( SvStream* pStrm, SwWW8Writer& rWW8Wrt );
    virtual ~SwEscherEx();

    void FinishEscher();
    void WriteFrmExtraData( const SwFrmFmt& rFmt );
    void WritePictures();

    EscherExHostAppData* StartShape( const uno::Reference< drawing::XShape >& )
        {return &aHostData;}
};

void WinwordAnchoring::WriteData( EscherEx& rEx ) const
{
    //Toplevel groups get their winword extra data attached, and sub elements
    //use the defaults
    if( rEx.GetGroupLevel() <= 1 )
    {
        rEx.AddAtom(24, DFF_msofbtUDefProp, 3, 4 ); //Prop id is 0xF122
        SvStream& rSt = rEx.GetStream();
        rSt << (UINT16)0x038F << nXAlign;
        rSt << (UINT16)0x0390 << nXRelTo;
        rSt << (UINT16)0x0391 << nYAlign;
        rSt << (UINT16)0x0392 << nYRelTo;
    }
}

/*  */

void SwWW8Writer::CreateEscher()
{
    if(pHFSdrObjs->size() || pSdrObjs->size())
    {
        ASSERT( !pEscher, "wer hat den Pointer nicht geloescht?" );
        SvStream* pEscherStrm = pStg->OpenStream( sEscherStream,
                                STREAM_READWRITE | STREAM_SHARE_DENYALL );
        pEscherStrm->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );

        pEscher = new SwEscherEx( pEscherStrm, *this );
    }
}

void SwWW8Writer::WriteEscher()
{
    if( pEscher )
    {
        ULONG nStart = pTableStrm->Tell();

        pEscher->WritePictures();
        pEscher->FinishEscher();

        pFib->fcDggInfo = nStart;
        pFib->lcbDggInfo = pTableStrm->Tell() - nStart;
        delete pEscher, pEscher = 0;
    }
}


/*  */

// Output- Routines for Escher Export

SwEscherEx::SwEscherEx( SvStream* pStrm, SwWW8Writer& rWW8Wrt )
    : EscherEx( *pStrm, rWW8Wrt.pHFSdrObjs->size() ? 2 : 1 ),
    rWrt( rWW8Wrt ), pTxtBxs( 0 ), pEscherStrm( pStrm ), pPictStrm( 0 )
{
    aHostData.SetClientData(&aWinwordAnchoring);
    Init();
    OpenContainer( ESCHER_DggContainer );

    sal_uInt16 nColorCount = 4;
    *pStrm	<< (sal_uInt16)( nColorCount << 4 )		// instance
            << (sal_uInt16)ESCHER_SplitMenuColors	// record type
            << (sal_uInt32)( nColorCount * 4 )		// size
            << (sal_uInt32)0x08000004
            << (sal_uInt32)0x08000001
            << (sal_uInt32)0x08000002
            << (sal_uInt32)0x100000f7;

    CloseContainer();	// ESCHER_DggContainer

    BYTE i = 2;		// for header/footer and the other
    PlcDrawObj *pSdrObjs = rWrt.pHFSdrObjs;
    pTxtBxs = rWrt.pHFTxtBxs;

    // if no header/footer -> skip over
    if (!pSdrObjs->size())
    {
        --i;
        pSdrObjs = rWrt.pSdrObjs;
        pTxtBxs = rWrt.pTxtBxs;
    }

    for( ; i--; pSdrObjs = rWrt.pSdrObjs, pTxtBxs = rWrt.pTxtBxs )
    {
        // "dummy char" (or any Count ?) - why? This knows only M$
        GetStream() << (sal_Char)i;

        OpenContainer( ESCHER_DgContainer );

        EnterGroup( 0 );

        ULONG nSecondShapeId = pSdrObjs == rWrt.pSdrObjs ? GetShapeID() : 0;

        // write now all Writer-/DrawObjects
        MakeZOrderArrAndFollowIds( pSdrObjs->GetObjArr() );

        ULONG nShapeId=0;
        for( USHORT n = 0; n < aSortFmts.Count(); ++n )
        {
            INT32 nBorderThick=0;
            const SwFrmFmt& rFmt = *(const SwFrmFmt*)aSortFmts[n];
            if (RES_FLYFRMFMT == rFmt.Which())
                nBorderThick = WriteFlyFrm(rFmt, nShapeId, maDirections[n]);
            else if (rFmt.FindRealSdrObject()->GetObjInventor() ==
                     FmFormInventor)
            {
                WriteOCXControl(rFmt,nShapeId=GetShapeID());
            }
            else
            {
                aWinwordAnchoring.SetAnchoring(rFmt, TRUE);
                const SdrObject* pObj = rFmt.FindRealSdrObject();
                if( pObj )
                    nShapeId = AddSdrObject( *pObj );
#ifndef PRODUCT
                else
                    ASSERT( !this, "wo ist das SDR-Object?" );
#endif
            }

            if( !nShapeId )
            {
                /*!!!*/ const SdrObject* pObj = 0;
                nShapeId = AddDummyShape( *pObj );
            }

            pSdrObjs->SetShapeDetails(rFmt, nShapeId, nBorderThick);
        }

        EndSdrObjectPage();		    // ???? Bugfix for 74724

        LeaveGroup();

        if( nSecondShapeId )
        {
            OpenContainer( ESCHER_SpContainer );

            AddShape( ESCHER_ShpInst_Rectangle, 0xe00, nSecondShapeId );

            EscherPropertyContainer aPropOpt;
            // default Fuellfarbe ist das StarOffice blau7
            // ----> von DrawingLayer besorgen !!
            aPropOpt.AddOpt( ESCHER_Prop_fillColor, 0xffb800 );
            aPropOpt.AddOpt( ESCHER_Prop_fillBackColor, 0 );
            aPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x00100010 );
            aPropOpt.AddOpt( ESCHER_Prop_lineColor, 0x8000001 );
            aPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x00080008 );
            aPropOpt.AddOpt( ESCHER_Prop_shadowColor, 0x8000002 );
            aPropOpt.AddOpt( ESCHER_Prop_lineWidth, 0 );

// winword defaults!
//			aPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x100000 );
//			aPropOpt.AddOpt( ESCHER_Prop_lineWidth, 0 );
//			aPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x80000 );
//			aPropOpt.AddOpt( ESCHER_Prop_bWMode, 0x9 );
//			aPropOpt.AddOpt( ESCHER_Prop_fBackground, 0x10001 );

            aPropOpt.Commit( *pStrm );

            AddAtom( 4, ESCHER_ClientData );
            GetStream() << 1L;

            CloseContainer();	// ESCHER_SpContainer
        }
    CloseContainer();	// ESCHER_DgContainer
    }
}

SwEscherEx::~SwEscherEx()
{
}

void SwEscherEx::Init()
{
    MapUnit eMap = MAP_TWIP;
    SdrModel *pModel = rWrt.pDoc->GetDrawModel();
    if (pModel)
    {
        // PPT arbeitet nur mit Einheiten zu 576DPI
        // WW hingegen verwendet twips, dh. 1440DPI.
        eMap = pModel->GetScaleUnit();
    }

//	Fraction aFact( nAppScale );
//	aFact /= GetMapFactor( MAP_INCH, eMap ).X();
    // create little values
//	aFact = Fraction( aFact.GetNumerator(), aFact.GetDenominator() );

    // Bei 100TH_MM -> 2540/576=635/144
    // Bei Twip     -> 1440/576=5/2
//	nMapMul = aFact.GetNumerator();
//	nMapDiv = aFact.GetDenominator();
//	bNeedMap = nMapMul != nMapDiv;

    // MS-DFF-Properties sind grossteils in EMU (English Metric Units) angegeben
    // 1mm=36000emu, 1twip=635emu
    Fraction aFact( 360, 1 );
//	aFact = Fraction( 360, 1 );
    aFact /= GetMapFactor( MAP_100TH_MM, eMap ).X();
    // create little values
    aFact = Fraction( aFact.GetNumerator(), aFact.GetDenominator() );
    nEmuMul = aFact.GetNumerator();
    nEmuDiv = aFact.GetDenominator();

    // Und noch was fuer typografische Points
//	aFact = Fraction( 1, 1 );
//	aFact /= GetMapFactor(MAP_POINT,eMap).X();
//	nPntMul = aFact.GetNumerator();
//	nPntDiv = aFact.GetDenominator();

    SetHellLayerId( rWrt.pDoc->GetHellId() );
}

void SwEscherEx::WritePictures()
{
    if( pPictStrm )
    {
        // set the blip - entries to the correct stream pos
        INT32 nEndPos = rWrt.Strm().Tell();
        SetNewBlipStreamOffset( nEndPos );

        pPictStrm->Seek( 0 );
        rWrt.Strm() << *pPictStrm;

        delete pPictStrm, pPictStrm = 0;
        rWrt.GetStorage().Remove( sEscherPictStream );
    }
    Flush();
}

void SwEscherEx::FinishEscher()
{
    pEscherStrm->Seek( 0 );
    *rWrt.pTableStrm << *pEscherStrm;

    delete pEscherStrm, pEscherStrm = 0;
    rWrt.GetStorage().Remove( sEscherStream );

    /*#82587# Everytime MS 2000 creates an escher stream there is always an
     ObjectPool dir (even if empty). It turns out that if a copy of MS 2000 is
     used to open a document that contains escher graphics exported from
     StarOffice without this empty dir then *if* that copy of MS Office has
     never been used to open a MSOffice document that has escher graphics (and
     an ObjectPool dir of course) and that copy of office has not been used to
     draw escher graphics then our exported graphics do not appear. Once you
     do open a ms document with escher graphics or draw an escher graphic with
     that copy of word, then all documents from staroffice that contain escher
     work from then on. Tricky to track down, some sort of late binding
     trickery in MS where solely for first time initialization the existence
     of an ObjectPool dir is necessary for triggering some magic. cmc*/
    rWrt.GetStorage().OpenStorage(CREATE_CONST_ASC(SL::aObjectPool),
        STREAM_READWRITE | STREAM_SHARE_DENYALL);
}

extern "C"
{
    static int CompUINT32( const void *pFirst, const void *pSecond)
    {
        return( 
            (*((UINT32*)pFirst ) & 0xFFFFFF00) - 
            (*((UINT32*)pSecond) & 0xFFFFFF00) );
    }
}

void WinwordAnchoring::SetAnchoring( const SwFrmFmt& rFmt, BOOL bBROKEN )
{
    const RndStdIds eAnchor = rFmt.GetAnchor().GetAnchorId();

    const SwFmtHoriOrient&	rHoriOri = rFmt.GetHoriOrient();
    const SwFmtVertOrient&	rVertOri = rFmt.GetVertOrient();

    const SwHoriOrient eHOri = rHoriOri.GetHoriOrient();
    const SwVertOrient eVOri = rVertOri.GetVertOrient();

    SwRelationOrient   eHRel = rHoriOri.GetRelationOrient();
    SwRelationOrient   eVRel = rVertOri.GetRelationOrient();

    //There must be a problem with page anchoring and draw objects in writer
    //must be a problem somewhere.
    if (bBROKEN)
    {
        if (eHRel == PRTAREA)
            eHRel = FRAME;
        if (eVRel == PRTAREA)
            eVRel = FRAME;
    }

    UINT32 nHIndex = 0;
    UINT32 nVIndex = 0;

    switch( eAnchor )
    {
        case FLY_PAGE:
            nHIndex = 0x00000000;
            nVIndex = 0x10000000;
            // match eHRel ?
            if(      PRTAREA == eHRel ) eHRel = REL_PG_PRTAREA;
            else if( FRAME   == eHRel ) eHRel = REL_PG_FRAME;
            // match eVRel ?
            if(      REL_PG_PRTAREA == eVRel ) eVRel = PRTAREA;
            else
            if(      REL_PG_FRAME   == eVRel ) eVRel = FRAME;
            break;
        case FLY_AT_CNTNT:
            nHIndex = 0x01000000;
            nVIndex = 0x11000000;
            break;
        case FLY_AUTO_CNTNT:
            nHIndex = 0x01000000;
            nVIndex = 0x12000000;
            break;
        case FLY_IN_CNTNT:
            nHIndex = 0x02000000;
            nVIndex = 0x13000000;
            break;
        default:
            nHIndex = 0x01000000; // FLY_AT_CNTNT
            nVIndex = 0x11000000;
            break;
    }

    switch( eHRel )
    {
        case FRAME:
        //	nHIndex |= 0x00000000;
            break;
        case PRTAREA:
            nHIndex |= 0x00010000;
            break;
        case REL_PG_LEFT:
            nHIndex |= 0x00020000;
            break;
        case REL_PG_RIGHT:
            nHIndex |= 0x00030000;
            break;
        case REL_FRM_LEFT:
            nHIndex |= 0x00040000;
            break;
        case REL_FRM_RIGHT:
            nHIndex |= 0x00050000;
            break;
        case REL_PG_FRAME:
            nHIndex |= 0x00060000;
            break;
        case REL_PG_PRTAREA:
            nHIndex |= 0x00070000;
            break;
        case REL_CHAR:
            nHIndex |= 0x00080000;
            break;
        default:
            nHIndex |= 0x00090000; // PRTAREA
            break;
    }

    switch( eHOri )
    {
        case HORI_LEFT:
        //	nHIndex |= 0x00000000;
            break;
        case HORI_INSIDE:
            nHIndex |= 0x00000100;
            break;
        case HORI_RIGHT:
            nHIndex |= 0x00000200;
            break;
        case HORI_OUTSIDE:
            nHIndex |= 0x00000300;
            break;
        case HORI_CENTER:
            nHIndex |= 0x00000400;
            break;
        case HORI_NONE:
            nHIndex |= 0x00000500;
            break;
        default:
        //  nHIndex |= 0x00000000; // HORI_LEFT
            break;
    }

    switch( eVRel )
    {
        case FRAME:
        //	nVIndex |= 0x00000000;
            break;
        case PRTAREA:
            nVIndex |= 0x00010000;
            break;
        case REL_CHAR:
            nVIndex |= 0x00020000;
            break;
        default:
            nVIndex |= 0x00030000; // PRTAREA
            break;
    }

    switch( eVOri )
    {
        case VERT_TOP:
        //	nVIndex |= 0x00000000;
            break;
        case VERT_BOTTOM:
            nVIndex |= 0x00000100;
            break;
        case VERT_CENTER:
            nVIndex |= 0x00000200;
            break;
        case VERT_NONE:
            nVIndex |= 0x00000300;
            break;
        case VERT_CHAR_TOP:
            nVIndex |= 0x00000400;
            break;
        case VERT_CHAR_CENTER:
            nVIndex |= 0x00000500;
            break;
        case VERT_CHAR_BOTTOM:
            nVIndex |= 0x00000600;
            break;
        case VERT_LINE_TOP:
            nVIndex |= 0x00000700;
            break;
        case VERT_LINE_CENTER:
            nVIndex |= 0x00000800;
            break;
        case VERT_LINE_BOTTOM:
            nVIndex |= 0x00000900;
            break;
        default:
        //  nVIndex |= 0x00000000; // VERT_TOP
            break;
    }

/*
    Note: the following table MUST be sorted in ascendent order!

        nXAlign - abs. Position, Left,  Centered,  Right,  Inside, Outside
        nYAlign - abs. Position, Top,   Centered,  Bottom, Inside, Outside

        nXRelTo - Page printable area, Page,  Column,    Character
        nYRelTo - Page printable area, Page,  Paragraph, Line

        Match:  0x99 99 99 9 9
                   |  |  | | |
                   |  |  | | +-- ord of nXRelTo
                   |  |  | +---- ord of nXAlign
                   |  |  |
                   |  |  +------ SwHoriOrient
                   |  +--------- SwRelationOrient (horizontal)
                   +------------ RndStdIds
*/
    static const UINT32 aHVMatcher [] = {

//     H O R I Z O N T A L    SwHoriOrient:   HORI_LEFT, HORI_INSIDE, HORI_RIGHT, HORI_OUTSIDE, HORI_CENTER, HORI_NONE
//                                                0          1            2           3             4            5

    // RndStdIds: FLY_PAGE: 0

    // SwRelationOrient: REL_PG_LEFT: 2
    //               |
                0x00020011,  // SwHoriOrient: HORI_LEFT
                0x00020111,  //               HORI_INSIDE
                0x00020211,  //               HORI_RIGHT
                0x00020311,  //               HORI_OUTSIDE
                0x00020411,  //               HORI_CENTER
                0x00020511,  //               HORI_NONE
    // SwRelationOrient: REL_PG_RIGHT: 3
    //               |
                0x00030031,
                0x00030131,
                0x00030231,
                0x00030331,
                0x00030431,
                0x00030531,
    // SwRelationOrient: REL_PG_FRAME: 6
    //               |
                0x00060011,
                0x00060141,
                0x00060231,
                0x00060351,
                0x00060421,
                0x00060501,
    // SwRelationOrient: REL_PG_PRTAREA: 7
    //               |
                0x00070010,
                0x00070140,
                0x00070230,
                0x00070350,
                0x00070420,
                0x00070500,

    // RndStdIds: FLY_AT_CNTNT: 1
    //
    // SwRelationOrient: FRAME: 0
    //               |
                0x01000012,
                0x01000112,
                0x01000232,
                0x01000332,
                0x01000422,
                0x01000502,
    // SwRelationOrient: PRTAREA: 1
    //               |
                0x01010012,
                0x01010112,
                0x01010232,
                0x01010332,
                0x01010422,
                0x01010502,
    // SwRelationOrient: REL_PG_LEFT: 2
    //               |
                0x01020011,
                0x01020111,
                0x01020211,
                0x01020311,
                0x01020411,
                0x01020511,
    // SwRelationOrient: REL_PG_RIGHT: 3
    //               |
                0x01030031,
                0x01030131,
                0x01030231,
                0x01030331,
                0x01030431,
                0x01030531,
    // SwRelationOrient: REL_FRM_LEFT: 4
    //               |
                0x01040012,
                0x01040112,
                0x01040212,
                0x01040312,
                0x01040412,
                0x01040512,
    // SwRelationOrient: REL_FRM_RIGHT: 5
    //               |
                0x01050032,
                0x01050132,
                0x01050232,
                0x01050332,
                0x01050432,
                0x01050532,
    // SwRelationOrient: REL_PG_FRAME: 6
    //               |
                0x01060011,
                0x01060141,
                0x01060231,
                0x01060351,
                0x01060421,
                0x01060501,
    // SwRelationOrient: REL_PG_PRTAREA: 7
    //               |
                0x01070010,
                0x01070140,
                0x01070230,
                0x01070350,
                0x01070420,
                0x01070500,

    // RndStdIds: FLY_IN_CNTNT: 2
    //
    // SwRelationOrient: FRAME: 0
    //               |
                0x02000012,
                0x02000112,
                0x02000232,
                0x02000332,
                0x02000422,
                0x02000502,
    // SwRelationOrient: PRTAREA: 1
    //               |
                0x02010012,
                0x02010112,
                0x02010232,
                0x02010332,
                0x02010422,
                0x02010502,
    // SwRelationOrient: REL_PG_LEFT: 2
    //               |
                0x02020011,
                0x02020111,
                0x02020211,
                0x02020311,
                0x02020411,
                0x02020511,
    // SwRelationOrient: REL_PG_RIGHT: 3
    //               |
                0x02030031,
                0x02030131,
                0x02030231,
                0x02030331,
                0x02030431,
                0x02030531,
    // SwRelationOrient: REL_FRM_LEFT: 4
    //               |
                0x02040012,
                0x02040112,
                0x02040212,
                0x02040312,
                0x02040412,
                0x02040512,
    // SwRelationOrient: REL_FRM_RIGHT: 5
    //               |
                0x02050032,
                0x02050132,
                0x02050232,
                0x02050332,
                0x02050432,
                0x02050532,
    // SwRelationOrient: REL_PG_FRAME: 6
    //               |
                0x02060011,
                0x02060141,
                0x02060231,
                0x02060351,
                0x02060421,
                0x02060501,
    // SwRelationOrient: REL_PG_PRTAREA: 7
    //               |
                0x02070010,
                0x02070140,
                0x02070230,
                0x02070350,
                0x02070420,
                0x02070500,
    // SwRelationOrient: REL_CHAR: 8
    //               |
                0x02080013,
                0x02080113,
                0x02080233,
                0x02080333,
                0x02080423,
                0x02080503,

//     V E R T I C A L   SwVertOrient:   VERT_TOP, VERT_BOTTOM, VERT_CENTER, VERT_NONE, VERT_CHAR_TOP, VERT_CHAR_CENTER, VERT_CHAR_BOTTOM, VERT_LINE_TOP, VERT_LINE_CENTER, VERT_LINE_BOTTOM
//                                           0         1            2            3          4              5                 6 == "below"      7              8                 9

    // RndStdIds: FLY_PAGE: 0x10
    //
    // SwRelationOrient: REL_PG_FRAME (or FRAME resp.): 0
    //               |
                0x10000011,  // SwVertOrient: VERT_TOP
                0x10000131,  //               VERT_BOTTOM
                0x10000221,  //               VERT_CENTER
                0x10000301,  //               VERT_NONE
    // SwRelationOrient: REL_PG_PRTAREA (or PRTAREA resp.): 1
    //               |
                0x10010010,  // SwVertOrient: VERT_TOP
                0x10010130,  //               VERT_BOTTOM
                0x10010220,  //               VERT_CENTER
                0x10010300,  //               VERT_NONE

    // RndStdIds: FLY_AT_CNTNT: 0x11
    //
    // SwRelationOrient: FRAME: 0
    //               |
                0x11000013,  // SwVertOrient: VERT_TOP
                0x11000133,  //               VERT_BOTTOM
                0x11000223,  //               VERT_CENTER
                0x11000302,  //               VERT_NONE
    // SwRelationOrient: PRTAREA: 1
    //               |
                0x11010013,  // SwVertOrient: VERT_TOP
                0x11010133,  //               VERT_BOTTOM
                0x11010223,  //               VERT_CENTER
                0x11010302,  //               VERT_NONE

    // RndStdIds: "to character" == FLY_AUTO_CNTNT: 0x12
    //
    // SwRelationOrient: "Margin" == FRAME: 0
    //               |
                0x12000013,  // SwVertOrient: VERT_TOP
                0x12000133,  //               VERT_BOTTOM
                0x12000223,  //               VERT_CENTER
                0x12000302,  //               VERT_NONE
    // SwRelationOrient: "Textarea" == PRTAREA: 1
    //               |
                0x12010013,  // SwVertOrient: VERT_TOP
                0x12010133,  //               VERT_BOTTOM
                0x12010223,  //               VERT_CENTER
                0x12010302,  //               VERT_NONE
    // SwRelationOrient: "Character" == REL_CHAR: 2
    //               |
                0x12020013,  // SwVertOrient: VERT_TOP
                0x12020133,  //               VERT_BOTTOM
                0x12020223,  //               VERT_CENTER
                0x12020302,  //               VERT_NONE
                0x12020633,  //               VERT_CHAR_BOTTOM (value: 6)

    // RndStdIds: "as character" == FLY_IN_CNTNT: 0x13
    //
    // SwRelationOrient: FRAME: 0
    //               |
                0x13000013,  // SwVertOrient: VERT_TOP        (baseline)
                0x13000133,  //               VERT_BOTTOM
                0x13000223,  //               VERT_CENTER
                0x13000302,  //               VERT_NONE        == "from bottom"

                0x13000413,  //               VERT_CHAR_TOP   (character)
                0x13000533,  //               VERT_CHAR_CENTER
                0x13000623,  //               VERT_CHAR_BOTTOM == "below"

                0x13000713,  //               VERT_LINE_TOP   (row)
                0x13000813,  //               VERT_LINE_CENTER
                0x13000923,  //               VERT_LINE_BOTTOM
        };

    // find horizontal values
    UINT32* pFound = (UINT32*)bsearch( (const void*) &nHIndex, 
        (const void*) aHVMatcher, sizeof(aHVMatcher) / sizeof(aHVMatcher[0]),
        sizeof(aHVMatcher[0]), CompUINT32 );
    if( !pFound )
        pFound = (UINT32*)aHVMatcher; // take Element #0 if none found
    nXAlign = (*pFound & 0x000000F0) >> 4;
    nXRelTo = (*pFound & 0x0000000F);

    // find vertical values
    pFound= (UINT32*)bsearch( (const void*) &nVIndex, (void*) aHVMatcher,
        sizeof(aHVMatcher) / sizeof(aHVMatcher[0]), sizeof(aHVMatcher[0]),
        CompUINT32 );
    if( !pFound )
        pFound = (UINT32*)aHVMatcher; // take Element #0 if none found
    nYAlign = (*pFound & 0x000000F0) >> 4;
    nYRelTo = (*pFound & 0x0000000F);
}

void SwEscherEx::WriteFrmExtraData( const SwFrmFmt& rFmt )
{
    aWinwordAnchoring.SetAnchoring(rFmt );
    aWinwordAnchoring.WriteData(*this);
}

INT32 SwEscherEx::WriteFlyFrm(const SwFrmFmt& rFmt, UINT32 &rShapeId,
    short nDirection)
{
    // check for textflyframe and if it is the first in a Chain
    INT32 nBorderThick = 0;
    const SwNodeIndex* pNdIdx = rFmt.GetCntnt().GetCntntIdx();
    if( pNdIdx )
    {
        SwNodeIndex aIdx( *pNdIdx, 1 );
        switch( aIdx.GetNode().GetNodeType() )
        {
        case ND_GRFNODE:
            nBorderThick = WriteGrfFlyFrame( rFmt, rShapeId = GetShapeID() );
            break;
        case ND_OLENODE:
            nBorderThick = WriteOLEFlyFrame( rFmt, rShapeId = GetShapeID() );
            break;
        default:
            if (const SdrObject* pObj = rFmt.FindRealSdrObject())
            {
                // check for the first in a Chain
                UINT32 nTxtId;
                USHORT nOff = 0;
                const SwFrmFmt* pFmt = &rFmt, *pPrev;
                while( 0 != ( pPrev = pFmt->GetChain().GetPrev() ))
                {
                    ++nOff;
                    pFmt = pPrev;
                }

                rShapeId = GetFlyShapeId( rFmt );
                if( !nOff )
                {
                    void* p = (void*)pObj;
                    nTxtId = pTxtBxs->GetPos( p );
                    if( USHRT_MAX == nTxtId )
                    {
                        pTxtBxs->Append( *pObj, rShapeId );
                        nTxtId = pTxtBxs->Count();
                    }
                    else
                        ++nTxtId;
                }
                else
                {
                    const SdrObject* pPrevObj = pFmt->FindRealSdrObject();
                    void* p = (void*)pPrevObj;
                    nTxtId = pTxtBxs->GetPos( p );
                    if( USHRT_MAX == nTxtId )
                    {
                        UINT32 nPrevShapeId = GetFlyShapeId( *pFmt );
                        pTxtBxs->Append( *pPrevObj, nPrevShapeId );
                        nTxtId = pTxtBxs->Count();
                    }
                    else
                        ++nTxtId;
                }
                nTxtId *= 0x10000;
                nTxtId += nOff;

                nBorderThick = WriteTxtFlyFrame(rFmt, rShapeId, nTxtId, 
                    nDirection);
            }
        }
    }
    return nBorderThick;
}

INT32 SwEscherEx::WriteTxtFlyFrame(const SwFrmFmt& rFmt, UINT32 nShapeId,
    UINT32 nTxtBox, short nDirection)
{
    INT32 nBorderThick=0;
    OpenContainer( ESCHER_SpContainer );

    AddShape( ESCHER_ShpInst_TextBox, 0xa00, nShapeId );
    EscherPropertyContainer aPropOpt;
    aPropOpt.AddOpt( ESCHER_Prop_lTxid, nTxtBox );
    const VoidPtr pNext = rFmt.GetChain().GetNext();
    if( pNext )
    {
        USHORT nPos = aSortFmts.GetPos( pNext );
        if( USHRT_MAX != nPos && aFollowShpIds[ nPos ] )
            aPropOpt.AddOpt( ESCHER_Prop_hspNext, aFollowShpIds[ nPos ] );
    }
    nBorderThick = WriteFlyFrameAttr( rFmt, mso_sptTextBox, aPropOpt );

    MSO_TextFlow nFlow;

    switch (nDirection)
    {
        default:
            ASSERT(0,"unknown direction type");
        case FRMDIR_HORI_LEFT_TOP:
            nFlow=mso_txflHorzN;
        break;
        case FRMDIR_HORI_RIGHT_TOP:
            nFlow=mso_txflHorzN;
        break;
        case FRMDIR_VERT_TOP_LEFT: //not really possible in word
        case FRMDIR_VERT_TOP_RIGHT:
            nFlow=mso_txflTtoBA;
        break;
    }
    aPropOpt.AddOpt( ESCHER_Prop_txflTextFlow, nFlow );

    aPropOpt.Commit( GetStream() );

    // store anchor attribute
    WriteFrmExtraData( rFmt );

    AddAtom( 4, ESCHER_ClientAnchor );	GetStream() << 0L;
    AddAtom( 4, ESCHER_ClientData );	GetStream() << 1L;
    AddAtom( 4, ESCHER_ClientTextbox );	GetStream() << nTxtBox;

    CloseContainer();	// ESCHER_SpContainer
    return nBorderThick;
}

INT32 SwEscherEx::WriteGrfFlyFrame( const SwFrmFmt& rFmt, UINT32 nShapeId )
{
    INT32 nBorderThick=0;
    OpenContainer( ESCHER_SpContainer );

    AddShape( ESCHER_ShpInst_PictureFrame, 0xa00, nShapeId );

    EscherPropertyContainer aPropOpt;

    UINT32 nFlags = ESCHER_BlipFlagDefault;
    SwNodeIndex aIdx( *rFmt.GetCntnt().GetCntntIdx(), 1 );
    SwGrfNode& rGrfNd = *aIdx.GetNode().GetGrfNode();
    if( rGrfNd.IsLinkedFile() )
    {
        String sURL;
        rGrfNd.GetFileFilterNms( &sURL, 0 );

        WW8Bytes aBuf;
        SwWW8Writer::InsAsString16( aBuf, sURL );
        SwWW8Writer::InsUInt16( aBuf, 0 );

        USHORT nArrLen = aBuf.Count();
        BYTE* pArr = new BYTE[ nArrLen ];
        memcpy( pArr, aBuf.GetData(), nArrLen );

        aPropOpt.AddOpt( ESCHER_Prop_pibName, TRUE, nArrLen, pArr, nArrLen );
        nFlags = ESCHER_BlipFlagLinkToFile | ESCHER_BlipFlagURL |
                    ESCHER_BlipFlagDoNotSave;
    }
    else
    {
        rGrfNd.SwapIn( TRUE );

        Graphic			aGraphic( rGrfNd.GetGrf() );
        GraphicObject	aGraphicObject( aGraphic );
        ByteString		aUniqueId = aGraphicObject.GetUniqueID();

        if ( aUniqueId.Len() )
        {
             const	MapMode aMap100mm( MAP_100TH_MM );
            Size	aSize( aGraphic.GetPrefSize() );

            if ( MAP_PIXEL == aGraphic.GetPrefMapMode().GetMapUnit() )
            {
                aSize = Application::GetDefaultDevice()->PixelToLogic(
                    aSize, aMap100mm );
            }
            else
            {
                aSize = OutputDevice::LogicToLogic( aSize,
                    aGraphic.GetPrefMapMode(), aMap100mm );
            }

            Point aEmptyPoint = Point();
            Rectangle aRect( aEmptyPoint, aSize );

            sal_uInt32 nBlibId = GetBlibID( *QueryPicStream(), aUniqueId,
                aRect, 0 );
            if ( nBlibId )
            {
                aPropOpt.AddOpt( ESCHER_Prop_fillType, ESCHER_FillPicture );
                aPropOpt.AddOpt( ESCHER_Prop_pib, nBlibId, sal_True );
            }
        }
    }
    aPropOpt.AddOpt( ESCHER_Prop_pibFlags, nFlags );
    nBorderThick = WriteFlyFrameAttr(rFmt,mso_sptPictureFrame,aPropOpt);
    WriteGrfAttr( rGrfNd, aPropOpt );
    aPropOpt.Commit( GetStream() );

    // store anchor attribute
    WriteFrmExtraData( rFmt );

    AddAtom( 4, ESCHER_ClientAnchor );		GetStream() << 0L;
    AddAtom( 4, ESCHER_ClientData );		GetStream() << 1L;

    CloseContainer();	// ESCHER_SpContainer
    return nBorderThick;
}


void SwEscherEx::WriteOCXControl( const SwFrmFmt& rFmt, UINT32 nShapeId )
{
    const SdrObject* pSdrObj = rFmt.FindRealSdrObject();
    if( pSdrObj)
    {
        OpenContainer( ESCHER_SpContainer );

        AddShape( ESCHER_ShpInst_PictureFrame, 0xa10, nShapeId );

        EscherPropertyContainer aPropOpt;
        Size aSz( pSdrObj->GetLogicRect().GetSize() );
        aSz.Width() = DrawModelToEmu( aSz.Width() );
        aSz.Height() = DrawModelToEmu( aSz.Height() );
        Rectangle aRect( Point(0,0), aSz );

        pTxtBxs->Append( *pSdrObj, nShapeId );
        UINT32 nPicId = pTxtBxs->Count();
        nPicId *= 0x10000;
        aPropOpt.AddOpt( ESCHER_Prop_pictureId, nPicId );
        aPropOpt.AddOpt( ESCHER_Prop_pictureActive, 0x10000 );
        WriteFlyFrameAttr( rFmt, mso_sptPictureFrame , aPropOpt );
        aPropOpt.Commit( GetStream() );

        // store anchor attribute
        WriteFrmExtraData( rFmt );

        AddAtom( 4, ESCHER_ClientAnchor ); 		GetStream() << 0L;
        AddAtom( 4, ESCHER_ClientData );		GetStream() << 1L;

        CloseContainer();	// ESCHER_SpContainer
    }
}

INT32 SwEscherEx::WriteOLEFlyFrame( const SwFrmFmt& rFmt, UINT32 nShapeId )
{
    INT32 nBorderThick=0;
    SwNodeIndex aIdx( *rFmt.GetCntnt().GetCntntIdx(), 1 );
    SwOLENode& rOLENd = *aIdx.GetNode().GetOLENode();
    const SvInPlaceObjectRef xObj( rOLENd.GetOLEObj().GetOleRef() );

    const SdrObject* pSdrObj = rFmt.FindRealSdrObject();
    if( pSdrObj )
    {
        GDIMetaFile aMtf;
        xObj->GetGDIMetaFile( aMtf );
        OpenContainer( ESCHER_SpContainer );

        AddShape( ESCHER_ShpInst_PictureFrame, 0xa10, nShapeId );
        EscherPropertyContainer aPropOpt;

        Graphic			aGraphic( aMtf );
        GraphicObject	aGraphicObject( aGraphic );
        ByteString		aUniqueId = aGraphicObject.GetUniqueID();
        if ( aUniqueId.Len() )
        {
            Size aSz( rOLENd.GetTwipSize() );
            aSz.Width() = DrawModelToEmu( aSz.Width() );
            aSz.Height() = DrawModelToEmu( aSz.Height() );
            Rectangle aRect( Point(0,0), aSz );

            sal_uInt32 nBlibId = GetBlibID( *QueryPicStream(), aUniqueId,
                aRect, 0 );
            if ( nBlibId )
            {
                aPropOpt.AddOpt( ESCHER_Prop_fillType, ESCHER_FillPicture );
                aPropOpt.AddOpt( ESCHER_Prop_pib, nBlibId, sal_True );
            }
        }

        pTxtBxs->Append( *pSdrObj, nShapeId );
        UINT32 nPicId = pTxtBxs->Count();
        nPicId *= 0x10000;
        aPropOpt.AddOpt( ESCHER_Prop_pictureId, nPicId );
        aPropOpt.AddOpt( ESCHER_Prop_pictureActive, 0x10000 );
        nBorderThick = WriteFlyFrameAttr( rFmt, mso_sptPictureFrame, aPropOpt );
        WriteGrfAttr( rOLENd, aPropOpt );
        aPropOpt.Commit( GetStream() );

        // store anchor attribute
        WriteFrmExtraData( rFmt );

        AddAtom( 4, ESCHER_ClientAnchor );
        GetStream() << 0L;
        AddAtom( 4, ESCHER_ClientData );
        GetStream() << 1L;

        CloseContainer();	// ESCHER_SpContainer
    }
    return nBorderThick;
}


void SwEscherEx::WriteGrfAttr( const SwNoTxtNode& rNd,
    EscherPropertyContainer& rPropOpt )
{
    const SfxPoolItem* pItem;
    sal_uInt32 nMode = GRAPHICDRAWMODE_STANDARD;
    sal_Int32 nContrast = 0;
    sal_Int16 nBrightness = 0;

    if( SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState( RES_GRFATR_CONTRAST,
                                                        TRUE, &pItem ) )
    {
        nContrast = ((SfxInt16Item*)pItem)->GetValue();
    }

    if( SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState( RES_GRFATR_LUMINANCE,
                                                        TRUE, &pItem ) )
    {
        nBrightness = ((SfxInt16Item*)pItem)->GetValue();
    }


    if( SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState( RES_GRFATR_DRAWMODE,
                                                        TRUE, &pItem ) )
    {
        nMode = ((SfxEnumItem*)pItem)->GetValue();
        if (nMode == GRAPHICDRAWMODE_WATERMARK)
        {
            /*
            There is no real watermark mode in word, we must use standard
            mode and modify our ones by 70% extra brightness and 70% less
            contrast. This means that unmodified default OOo watermark
            will turn back into watermark, and modified OOo watermark will
            change into a close visual representation in standardmode
            */
            nBrightness += 70;
            if (nBrightness > 100)
                nBrightness = 100;
            nContrast -= 70;
            if (nContrast < -100)
                nContrast = -100;
            nMode = GRAPHICDRAWMODE_STANDARD;
        }
    }

    if (nMode == GRAPHICDRAWMODE_GREYS)
        nMode = 0x40004;
    else if (nMode == GRAPHICDRAWMODE_MONO)
        nMode = 0x60006;
    else
        nMode = 0;
    rPropOpt.AddOpt( ESCHER_Prop_pictureActive, nMode );

    if (nContrast != 0)
    {
        nContrast+=100;
        if (nContrast == 100)
            nContrast = 0x10000;
        else if (nContrast < 100)
        {
            nContrast *= 0x10000;
            nContrast /= 100;
        }
        else if (nContrast < 200)
            nContrast = (100 * 0x10000) / (200-nContrast);
        else
            nContrast = 0x7fffffff;
        rPropOpt.AddOpt( ESCHER_Prop_pictureContrast, nContrast);
    }

    if (nBrightness != 0)
        rPropOpt.AddOpt( ESCHER_Prop_pictureBrightness, nBrightness * 327 );

#if 0
    //gamma not seen
    if( SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState( RES_GRFATR_GAMMA,
                                                        TRUE, &pItem ) )
    {
        double fGamma = ((SwGammaGrf*)pItem)->GetValue();
        rPropOpt.AddOpt( ESCHER_Prop_pictureGamma,
            static_cast<sal_uInt32>(fGamma * 655) );
    }
#endif

    if( SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState( RES_GRFATR_CROPGRF,
                                                        TRUE, &pItem ))
    {
        const Size aSz( rNd.GetTwipSize() );
        INT32 nVal;
        if( 0 != ( nVal = ((SwCropGrf*)pItem )->GetLeft() ) )
            rPropOpt.AddOpt( ESCHER_Prop_cropFromLeft, ToFract16( nVal, aSz.Width()) );
        if( 0 != ( nVal = ((SwCropGrf*)pItem )->GetRight() ) )
            rPropOpt.AddOpt( ESCHER_Prop_cropFromRight, ToFract16( nVal, aSz.Width()));
        if( 0 != ( nVal = ((SwCropGrf*)pItem )->GetTop() ) )
            rPropOpt.AddOpt( ESCHER_Prop_cropFromTop, ToFract16( nVal, aSz.Height()));
        if( 0 != ( nVal = ((SwCropGrf*)pItem )->GetBottom() ) )
            rPropOpt.AddOpt( ESCHER_Prop_cropFromBottom, ToFract16( nVal, aSz.Height()));
    }
    // mirror ??
}

INT32 SwEscherEx::WriteFlyFrameAttr( const SwFrmFmt& rFmt, MSO_SPT eShapeType,
    EscherPropertyContainer& rPropOpt )
{
    INT32 nLineWidth=0;
    const SfxPoolItem* pItem;
    BOOL bFirstLine = TRUE;
    if( SFX_ITEM_SET == rFmt.GetItemState( RES_BOX, TRUE, &pItem ) )
    {
        static UINT16 __READONLY_DATA aExhperProp[ 4 ] = {
            ESCHER_Prop_dyTextTop,  ESCHER_Prop_dyTextBottom,
            ESCHER_Prop_dxTextLeft, ESCHER_Prop_dxTextRight
        };
        const SvxBorderLine* pLine;

        for( int n = 0; n < 4; ++n )
            if( 0 != ( pLine = ((SvxBoxItem*)pItem)->GetLine( n )) )
            {
                if( bFirstLine )
                {
                    UINT32 nLineColor = GetColor( pLine->GetColor(), FALSE );
                    rPropOpt.AddOpt( ESCHER_Prop_lineColor, nLineColor );
                    rPropOpt.AddOpt( ESCHER_Prop_lineBackColor,
                        nLineColor ^ 0xffffff );

                    MSO_LineStyle eStyle;
                    if( pLine->GetInWidth() )
                    {
                        // double line
                        nLineWidth = pLine->GetInWidth() + pLine->GetOutWidth()
                            + pLine->GetDistance();
                        if( pLine->GetInWidth() == pLine->GetOutWidth() )
                            eStyle = mso_lineDouble;
                        else if( pLine->GetInWidth() < pLine->GetOutWidth() )
                            eStyle = mso_lineThickThin;
                        else
                            eStyle = mso_lineThinThick;
                    }
                    else
                    {
                        // simple line
                        eStyle = mso_lineSimple;
                        nLineWidth = pLine->GetOutWidth();
                    }

                    rPropOpt.AddOpt( ESCHER_Prop_lineStyle, eStyle );
                    rPropOpt.AddOpt( ESCHER_Prop_lineWidth,
                        DrawModelToEmu( nLineWidth ));
                    rPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x8000E );

                    //Use import logic to determine how much of border will go
                    //outside graphic
                    nLineWidth = SwMSDffManager::GetEscherLineMatch(
                        eStyle,eShapeType,nLineWidth);
                    bFirstLine = FALSE;
                }
                rPropOpt.AddOpt( aExhperProp[ n ], DrawModelToEmu(
                    ((SvxBoxItem*)pItem)->GetDistance( n ) ));
            }
    }
    if( bFirstLine )				// no valid line found
    {
        rPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x80000 );
        rPropOpt.AddOpt( ESCHER_Prop_dyTextTop, 0 );
        rPropOpt.AddOpt( ESCHER_Prop_dyTextBottom, 0 );
        rPropOpt.AddOpt( ESCHER_Prop_dxTextLeft, 0 );
        rPropOpt.AddOpt( ESCHER_Prop_dxTextRight, 0 );
    }

    if( SFX_ITEM_SET == rFmt.GetItemState( RES_BACKGROUND, TRUE, &pItem ) )
    {
        if( ((SvxBrushItem*)pItem)->GetGraphic() )
        {
            {
                Graphic	aGraphic( *((SvxBrushItem*)pItem)->GetGraphic() );
                GraphicObject aGraphicObject( aGraphic );
                ByteString aUniqueId = aGraphicObject.GetUniqueID();

                if ( aUniqueId.Len() )
                {
                     const MapMode aMap100mm( MAP_100TH_MM );
                    Size aSize( aGraphic.GetPrefSize() );

                    if ( MAP_PIXEL == aGraphic.GetPrefMapMode().GetMapUnit() )
                    {
                        aSize = Application::GetDefaultDevice()->PixelToLogic(
                            aSize, aMap100mm );
                    }
                    else
                    {
                        aSize = OutputDevice::LogicToLogic( aSize,
                            aGraphic.GetPrefMapMode(), aMap100mm );
                    }

                    Point aEmptyPoint = Point();
                    Rectangle aRect( aEmptyPoint, aSize );

                    sal_uInt32 nBlibId = GetBlibID( *QueryPicStream(),
                        aUniqueId, aRect, 0 );
                    if ( nBlibId )
                        rPropOpt.AddOpt(ESCHER_Prop_fillBlip,nBlibId,sal_True);
                }

            }
            rPropOpt.AddOpt( ESCHER_Prop_fillType, ESCHER_FillPicture );
            rPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x140014 );
            rPropOpt.AddOpt( ESCHER_Prop_fillBackColor, 0 );
        }
        else if( !((SvxBrushItem*)pItem)->GetColor().GetTransparency() )
        {
            UINT32 nFillColor = GetColor( ((SvxBrushItem*)pItem)->
                                                    GetColor(), FALSE );
            rPropOpt.AddOpt( ESCHER_Prop_fillColor, nFillColor );
            rPropOpt.AddOpt( ESCHER_Prop_fillBackColor, nFillColor ^ 0xffffff );
            rPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x100010 );
        }
        else
            rPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x100000 );
    }
    else
        rPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x100000 );

    if( SFX_ITEM_SET == rFmt.GetItemState( RES_LR_SPACE, TRUE, &pItem ))
    {
        rPropOpt.AddOpt( ESCHER_Prop_dxWrapDistLeft,
                DrawModelToEmu( ((SvxLRSpaceItem*)pItem)->GetLeft() ) );
        rPropOpt.AddOpt( ESCHER_Prop_dxWrapDistRight,
                DrawModelToEmu( ((SvxLRSpaceItem*)pItem)->GetRight() ) );
    }
    else
    {
        rPropOpt.AddOpt( ESCHER_Prop_dxWrapDistLeft, 0 );
        rPropOpt.AddOpt( ESCHER_Prop_dxWrapDistRight, 0 );
    }

    if( SFX_ITEM_SET == rFmt.GetItemState( RES_UL_SPACE, TRUE, &pItem ))
    {
        rPropOpt.AddOpt( ESCHER_Prop_dyWrapDistTop,
                DrawModelToEmu( ((SvxULSpaceItem*)pItem)->GetUpper() ) );
        rPropOpt.AddOpt( ESCHER_Prop_dyWrapDistBottom,
                DrawModelToEmu( ((SvxULSpaceItem*)pItem)->GetLower() ) );
    }

    const SdrObject* pObj = rFmt.FindRealSdrObject();
    if( pObj && pObj->GetLayer() == GetHellLayerId() )
        rPropOpt.AddOpt( ESCHER_Prop_fPrint, 0x200020 );

    return nLineWidth;
}

void SwEscherEx::MakeZOrderArrAndFollowIds(
    const ::std::vector<DrawObj>& rSrcArr)
{
    if (aSortFmts.Count())
        aSortFmts.Remove( 0, aSortFmts.Count() );

    USHORT n, nPos, nCnt = rSrcArr.size();
    SvULongsSort aSort( 255 < nCnt ? 255 : nCnt, 255 );
    ASSERT(maDirections.empty(), "Impossible");
    maDirections.resize(nCnt);
    for( n = 0; n < nCnt; ++n )
    {
        ULONG nOrdNum = rWrt.GetSdrOrdNum(rSrcArr[n].mrCntnt);
        aSort.Insert( nOrdNum, nPos );
        void* p = (void *)(&(rSrcArr[n].mrCntnt));
        aSortFmts.Insert( p, nPos );
        maDirections[nPos] = rSrcArr[n].mnDirection;
    }

    if (aFollowShpIds.Count())
        aFollowShpIds.Remove(0, aFollowShpIds.Count());

    ULONG nShapeId;
    const SwFmtChain* pChain;
    for( n = 0; n < nCnt; ++n )
    {
        const SwFrmFmt* pFmt = (const SwFrmFmt*)aSortFmts[ n ];

        if( RES_FLYFRMFMT == pFmt->Which() &&
            ( ( pChain = &pFmt->GetChain())->GetPrev() ||
                pChain->GetNext() ) )
        {
            // the format needs a shapeid
            nShapeId = GetShapeID();
        }
        else
            nShapeId = 0;
        aFollowShpIds.Insert( nShapeId, n );
    }
}


UINT32 SwEscherEx::GetFlyShapeId( const SwFrmFmt& rFmt )
{
    const VoidPtr pFmt = (void*)&rFmt;
    USHORT nPos = aSortFmts.GetPos( pFmt );
    UINT32 nShapeId;
    if( USHRT_MAX != nPos )
    {
        if( 0 == ( nShapeId = aFollowShpIds[ nPos ] ))
        {
            nShapeId = GetShapeID();
            aFollowShpIds[ nPos ] = nShapeId;
        }
    }
    else
        nShapeId = GetShapeID();
    return nShapeId;
}

UINT32 SwEscherEx::QueryTextID( 
    const uno::Reference< drawing::XShape>& xXShapeRef, UINT32 nShapeId )
{
    UINT32 nId = 0;
    if (SdrObject* pObj = GetSdrObjectFromXShape(xXShapeRef))
    {
        pTxtBxs->Append( *pObj, nShapeId );
        nId = pTxtBxs->Count();
        nId *= 0x10000;
    }
    return nId;
}

SvStream* SwEscherEx::QueryPicStream()
{
    if( !pPictStrm )
    {
        pPictStrm = rWrt.GetStorage().OpenStream( sEscherPictStream,
                                STREAM_READWRITE | STREAM_SHARE_DENYALL );
        pPictStrm->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    }
    return pPictStrm;
}


BOOL SwMSConvertControls::ExportControl(Writer &rWrt, const SdrObject *pObj)
{
    SwWW8Writer& rWW8Wrt = (SwWW8Writer&)rWrt;

    if (!rWW8Wrt.bWrtWW8)
        return FALSE;

    SdrUnoObj *pFormObj = PTR_CAST(SdrUnoObj,pObj);
    uno::Reference< awt::XControlModel > xControlModel =
    pFormObj->GetUnoControlModel();

    //Why oh lord do we use so many different units ?
    //I think I painted myself into a little bit of a
    //corner by trying to use the uno interface for
    //controls export
    Size aTempSize=pFormObj->GetLogicRect().GetSize();
    awt::Size aSize;
    aSize.Width = TWIPS_TO_MM(aTempSize.A());
    aSize.Height = TWIPS_TO_MM(aTempSize.B());

    //Open the ObjectPool
    SvStorageRef xObjPool = rWW8Wrt.GetStorage().OpenStorage(
        CREATE_CONST_ASC(SL::aObjectPool), STREAM_READWRITE | 
        STREAM_SHARE_DENYALL);

    //Create a destination storage for the microsoft control
    String sStorageName('_');
    sStorageName += String::CreateFromInt32((UINT32)pObj);
    SvStorageRef xOleStg = xObjPool->OpenStorage(sStorageName,
                 STREAM_READWRITE|STREAM_SHARE_DENYALL);

    if (!xOleStg.Is())
        return FALSE;

    String sName;
    if (!WriteOCXStream(xOleStg,xControlModel,aSize,sName))
        return FALSE;

    BYTE aSpecOLE[] =
    {
        0x03, 0x6a, 0xFF, 0xFF, 0xFF, 0xFF, // sprmCPicLocation
        0x0a, 0x08, 1,                  // sprmCFOLE2
        0x55, 0x08, 1,                  // sprmCFSpec
        0x56, 0x08, 1                   // sprmCFObj
    };
    //Set the obj id into the sprmCPicLocation
    BYTE *pData = aSpecOLE+2;
    Set_UInt32(pData,(UINT32)pObj);

    sName.InsertAscii(" CONTROL Forms.",0);
    sName.APPEND_CONST_ASC(".1 \\s ");

    rWW8Wrt.OutField(0,87,sName,
        WRITEFIELD_START|WRITEFIELD_CMD_START|WRITEFIELD_CMD_END);

    rWW8Wrt.pChpPlc->AppendFkpEntry(rWW8Wrt.Strm().Tell(),sizeof(aSpecOLE),
        aSpecOLE);
    rWW8Wrt.WriteChar( 0x1 );
    rWW8Wrt.OutField( 0, 87, aEmptyStr, WRITEFIELD_END | WRITEFIELD_CLOSE );
    return TRUE;
}



