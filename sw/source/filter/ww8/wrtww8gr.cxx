/*************************************************************************
 *
 *  $RCSfile: wrtww8gr.cxx,v $
 *
 *  $Revision: 1.39 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 16:28:30 $
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#ifndef _FILTER_HXX //autogen
#include <svtools/filter.hxx>
#endif
#ifndef _SFXITEMITER_HXX //autogen
#include <svtools/itemiter.hxx>
#endif

#include <svtools/embedhlp.hxx>

#ifndef _VIRDEV_HXX //autogen
#include <vcl/virdev.hxx>
#endif
#ifndef _APP_HXX //autogen
#include <vcl/svapp.hxx>
#endif

#define ITEMID_BOXINFO      SID_ATTR_BORDER_INNER
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif
#ifndef _MSOLEEXP_HXX
#include <svx/msoleexp.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX
#include <svx/lrspitem.hxx> // SvxLRSpaceItem
#endif
#ifndef _SVX_ULSPITEM_HXX
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVDOOLE2_HXX
#include <svx/svdoole2.hxx>
#endif

#include <unotools/ucbstreamhelper.hxx>

#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>       // class SwFlyFrmFmt
#endif
#ifndef _GRFATR_HXX
#include <grfatr.hxx>       // class SwCropGrf
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FMTFSIZE_HXX
#include <fmtfsize.hxx>
#endif
#ifndef _FMTORNT_HXX
#include <fmtornt.hxx>
#endif

#ifndef SW_WRITERHELPER
#include "writerhelper.hxx"
#endif
#ifndef SW_WRITERWORDGLUE
#include "writerwordglue.hxx"
#endif

#ifndef _WW8STRUC_HXX
#include "ww8struc.hxx"
#endif
#ifndef _WRTWW8_HXX
#include "wrtww8.hxx"
#endif
#ifndef _WW8PAR_HXX
#include "ww8par.hxx"
#endif
#ifndef _ESCHER_HXX
#include "escher.hxx"
#endif

#include "docsh.hxx"

// Damit KA debuggen kann, ohne sich den ganzen Writer zu holen, ist
// temporaer dieses Debug gesetzt. Ist ausserdem noch das passende IniFlag
// gesetzt, dann werden in d:\ Hilfsdateien erzeugt.
// !! sollte demnaechst wieder entfernt werden !!
// #define DEBUG_KA


// ToDo:
// 5. Die MapModes, die Win nicht kann, umrechnen

// OutGrf() wird fuer jeden GrafNode im Doc gerufen. Es wird ein PicLocFc-Sprm
// eingefuegt, der statt Adresse ein Magic ULONG enthaelt. Ausserdem wird
// in der Graf-Klasse der GrfNode-Ptr gemerkt ( fuers spaetere Ausgeben der
// Grafiken und Patchen der PicLocFc-Attribute )

Writer& OutWW8_SwGrfNode( Writer& rWrt, SwCntntNode& rNode )
{
    SwWW8Writer& rWW8Wrt = (SwWW8Writer&)rWrt;
    ASSERT(rWW8Wrt.mpParentFrame, "frame not set!");
    if (rWW8Wrt.mpParentFrame)
    {
        rWW8Wrt.OutGrf(*rWW8Wrt.mpParentFrame);
        rWW8Wrt.pFib->fHasPic = 1;
    }

    return rWrt;
}

bool SwWW8Writer::TestOleNeedsGraphic(const SwAttrSet& rSet,
    SvStorageRef xOleStg, SvStorageRef xObjStg, String &rStorageName,
    SwOLENode *pOLENd)
{
#ifdef NO_OLE_SIZE_OPTIMIZE
    return true;
#else
    bool bGraphicNeeded = false;
    SfxItemIter aIter( rSet );
    const SfxPoolItem* pItem = aIter.GetCurItem();

    do {
        switch (pItem->Which())
        {
            /*
            For an inline object these properties are irrelevent because they
            will be the same as the defaults that msword applies in their
            absence, so if that is all that there is for these inline objects
            then if there turns out to be enough information in the object
            itself to regenerate the correct size and preview of the object
            then we will not need to provide an additional graphics preview in
            the data stream, which can save a lot of disk space.
            */
            case RES_FRM_SIZE:
            case RES_CNTNT:
            case RES_VERT_ORIENT:
            case RES_ANCHOR:
                break;
            default:
                bGraphicNeeded = true;
        }
    } while( !bGraphicNeeded && !aIter.IsAtEnd() &&
        0 != ( pItem = aIter.NextItem() ) );

    /*
    Now we must see if the object contains a preview itself which is equal to
    the preview that we are currently using. If the graphics are equal then we
    dont need to store another preview
    */
    GDIMetaFile aWMF;
    long nX=0,nY=0;
    if (!bGraphicNeeded && SwWW8ImplReader::ImportOleWMF(xOleStg,aWMF,nX,nY))
    {
        //TODO/LATER: this line looks strange - shouldn't it be true?!
        bGraphicNeeded = false;
        Point aTmpPoint;
        Rectangle aRect( aTmpPoint, Size( nX, nY ) );
        Graphic aGraph(aWMF);

        ErrCode nErr = ERRCODE_NONE;
        SdrOle2Obj *pRet = SvxMSDffManager::CreateSdrOLEFromStorage(
            rStorageName,xObjStg,pDoc->GetDocStorage(),aGraph,aRect,0,nErr,0);

        if (pRet)
        {
            SwOLEObj &rSObj = pOLENd->GetOLEObj();
            comphelper::EmbeddedObjectContainer aCnt( pDoc->GetDocStorage() );

            SvStream* pGraphicStream = ::utl::UcbStreamHelper::CreateStream( aCnt.GetGraphicStream( rSObj.GetOleRef() ) );
            DBG_ASSERT( pGraphicStream && !pGraphicStream->GetError(), "No graphic stream available!" );
            if ( pGraphicStream && !pGraphicStream->GetError() )
            {
                Graphic aGr1;
                GraphicFilter* pGF = GraphicFilter::GetGraphicFilter();
                String aEmptyStr;
                if( pGF->ImportGraphic( aGr1, aEmptyStr, *pGraphicStream, GRFILTER_FORMAT_DONTKNOW ) == GRFILTER_OK )
                {
                    Graphic aGr2;
                    delete pGraphicStream;
                    pGraphicStream =
                            ::utl::UcbStreamHelper::CreateStream( aCnt.GetGraphicStream( pRet->GetObjRef() ) );
                    if( pGF->ImportGraphic( aGr2, aEmptyStr, *pGraphicStream, GRFILTER_FORMAT_DONTKNOW ) == GRFILTER_OK )
                    {
                        if ( aGr1 == aGr2 )
                            bGraphicNeeded = false;
                    }
                }
            }
            else
                delete pGraphicStream;

            delete pRet;
        }
    }
    else
        bGraphicNeeded = true;
    return bGraphicNeeded;
#endif
}

Writer& OutWW8_SwOleNode( Writer& rWrt, SwCntntNode& rNode )
{
    SwWW8Writer& rWW8Wrt = (SwWW8Writer&)rWrt;
    BYTE *pSpecOLE;
    BYTE *pDataAdr;
    short nSize;
    static BYTE aSpecOLE_WW8[] = {
            0x03, 0x6a, 0, 0, 0, 0, // sprmCPicLocation
            0x0a, 0x08, 1,          // sprmCFOLE2
            0x56, 0x08, 1           // sprmCFObj
        };
    static BYTE aSpecOLE_WW6[] = {
            68, 4, 0, 0, 0, 0,      // sprmCPicLocation (len is 4)
            75, 1,                  // sprmCFOLE2
            118, 1                  // sprmCFObj
        };

    if( rWW8Wrt.bWrtWW8 )
    {
        pSpecOLE = aSpecOLE_WW8;
        nSize = sizeof( aSpecOLE_WW8 );
    }
    else
    {
        pSpecOLE = aSpecOLE_WW6;
        nSize = sizeof( aSpecOLE_WW6 );
    }
    pDataAdr = pSpecOLE + 2; //WW6 sprm is 1 but has 1 byte len as well.
    SwOLENode *pOLENd = rNode.GetOLENode();

    SvStorageRef xObjStg = rWW8Wrt.GetStorage().OpenSotStorage(
        CREATE_CONST_ASC(SL::aObjectPool), STREAM_READWRITE |
        STREAM_SHARE_DENYALL );

    if( xObjStg.Is()  )
    {
        ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject > xObj(pOLENd->GetOLEObj().GetOleRef());
        if( xObj.is() )
        {
            ::com::sun::star::embed::XEmbeddedObject *pObj = xObj.get();
            UINT32 nPictureId = (UINT32)pObj;
            Set_UInt32(pDataAdr, nPictureId);

            WW8OleMap *pMap = new WW8OleMap(nPictureId);
            bool bDuplicate = false;
            WW8OleMaps &rOleMap = rWW8Wrt.GetOLEMap();
            USHORT nPos;
            if ( rOleMap.Seek_Entry(pMap, &nPos) )
            {
                bDuplicate = true;
                delete pMap;
            }
            else if( 0 == rOleMap.Insert( pMap) )
                delete pMap;

            String sStorageName( '_' );
            sStorageName += String::CreateFromInt32( nPictureId );
            SvStorageRef xOleStg = xObjStg->OpenSotStorage( sStorageName,
                                STREAM_READWRITE| STREAM_SHARE_DENYALL );
            if( xOleStg.Is() )
            {
                /*
                If this object storage has been written already don't
                waste time rewriting it
                */
                if (!bDuplicate)
                    rWW8Wrt.GetOLEExp().ExportOLEObject( xObj, *xOleStg );

                // write as embedded field - the other things will be done
                // in the escher export
                String sServer(FieldString(ww::eEMBED));
                sServer += xOleStg->GetUserName();
                sServer += ' ';

                rWW8Wrt.OutField(0, ww::eEMBED, sServer, WRITEFIELD_START |
                    WRITEFIELD_CMD_START | WRITEFIELD_CMD_END);

                rWW8Wrt.pChpPlc->AppendFkpEntry( rWrt.Strm().Tell(),
                        nSize, pSpecOLE );

                bool bEndCR = true;
                /*
                In the word filter we only need a preview image for
                floating images, and then only (the usual case) if the
                object doesn't contain enough information to reconstruct
                what we need.

                We don't need a graphic for inline objects, so we don't
                even need the overhead of a graphic in that case.
                */
                bool bGraphicNeeded = false;

                if (rWW8Wrt.mpParentFrame)
                {
                    bGraphicNeeded = true;

                    if (rWW8Wrt.mpParentFrame->IsInline())
                    {
                        const SwAttrSet& rSet =
                            rWW8Wrt.mpParentFrame->GetFrmFmt().GetAttrSet();
                        bEndCR = false;
                        bGraphicNeeded = rWW8Wrt.TestOleNeedsGraphic(rSet,
                            xOleStg, xObjStg, sStorageName, pOLENd);
                    }
                }

                if (!bGraphicNeeded)
                    rWW8Wrt.WriteChar(0x1);
                else
                {
                    /*
                    ##897##
                    We need to insert the graphic representation of
                    this object for the inline case, otherwise word
                    has no place to find the dimensions of the ole
                    object, and will not be able to draw it
                    */
                    rWW8Wrt.OutGrf(*rWW8Wrt.mpParentFrame);
                }

                rWW8Wrt.OutField(0, ww::eEMBED, aEmptyStr,
                    WRITEFIELD_END | WRITEFIELD_CLOSE);

                if (bEndCR) //No newline in inline case
                    rWW8Wrt.WriteCR();
            }
        }
    }
    return rWrt;
}

void SwWW8Writer::OutGrf(const sw::Frame &rFrame)
{
    // GrfNode fuer spaeteres rausschreiben der Grafik merken
    pGrf->Insert(rFrame);

    pChpPlc->AppendFkpEntry( pStrm->Tell(), pO->Count(), pO->GetData() );
    pO->Remove( 0, pO->Count() );                   // leeren

    WriteChar( (char)1 );   // Grafik-Sonderzeichen in Haupttext einfuegen

    BYTE aArr[ 18 ];
    BYTE* pArr = aArr;

    const SwFrmFmt &rFlyFmt = rFrame.GetFrmFmt();
    RndStdIds eAn = rFlyFmt.GetAttrSet().GetAnchor(false).GetAnchorId();
    if( eAn == FLY_IN_CNTNT )
    {
        SwVertOrient eVert = rFlyFmt.GetVertOrient().GetVertOrient();
        if ((eVert == VERT_CHAR_CENTER) || (eVert == VERT_LINE_CENTER))
        {
            bool bVert = false;
            //The default for word in vertical text mode is to center,
            //otherwise a sub/super script hack is employed
            if (pOutFmtNode && pOutFmtNode->ISA(SwCntntNode) )
            {
                const SwTxtNode* pTxtNd = (const SwTxtNode*)pOutFmtNode;
                SwPosition aPos(*pTxtNd);
                bVert = pDoc->IsInVerticalText(aPos) ? true : false;
            }
            if (!bVert)
            {
                SwTwips nHeight = rFlyFmt.GetFrmSize().GetHeight();
                nHeight/=20; //nHeight was in twips, want it in half points, but
                             //then half of total height.
                long nFontHeight = ((const SvxFontHeightItem&)
                    GetItem(RES_CHRATR_FONTSIZE)).GetHeight();
                nHeight-=nFontHeight/20;

                if (bWrtWW8)
                    Set_UInt16( pArr, 0x4845 );
                else
                    Set_UInt8( pArr, 101 );
                Set_UInt16( pArr, -((INT16)nHeight));
            }
        }
    }

    // sprmCFSpec
    if( bWrtWW8 )
        Set_UInt16( pArr, 0x855 );
    else
        Set_UInt8( pArr, 117 );
    Set_UInt8( pArr, 1 );

    // sprmCPicLocation
    if( bWrtWW8 )
        Set_UInt16( pArr, 0x6a03 );
    else
    {
        Set_UInt8( pArr, 68 );
        Set_UInt8( pArr, 4 );
    }
    Set_UInt32( pArr, GRF_MAGIC_321 );

    // Magic variieren, damit verschiedene Grafik-Attribute nicht
    // gemerged werden
    static BYTE nAttrMagicIdx = 0;
    --pArr;
    Set_UInt8( pArr, nAttrMagicIdx++ );
    pChpPlc->AppendFkpEntry( pStrm->Tell(), pArr - aArr, aArr );

    if( ( eAn == FLY_AT_CNTNT && (bWrtWW8 || !bIsInTable )) ||
        eAn == FLY_PAGE )
    {
        WriteChar( (char)0x0d ); // umgebenden Rahmen mit CR abschliessen

        static BYTE __READONLY_DATA nSty[2] = { 0, 0 };
        pO->Insert( nSty, 2, pO->Count() );     // Style #0
        bool bOldGrf = bOutGrf;
        bOutGrf = true;

        Out_SwFmt(rFrame.GetFrmFmt(), false, false, true); // Fly-Attrs

        bOutGrf = bOldGrf;
        pPapPlc->AppendFkpEntry( pStrm->Tell(), pO->Count(), pO->GetData() );
        pO->Remove( 0, pO->Count() );                   // leeren
    }
}

GraphicDetails& GraphicDetails::operator=(const GraphicDetails &rOther)
{
    maFly = rOther.maFly;
    mnPos = rOther.mnPos;
    mnWid = rOther.mnWid;
    mnHei = rOther.mnHei;
    return *this;
}

void SwWW8WrGrf::Insert(const sw::Frame &rFly)
{
    const SwNoTxtNode *pNd =
        rFly.GetContent() ? rFly.GetContent()->GetNoTxtNode() : 0;

    UINT16 nWidth;
    UINT16 nHeight;
    if (rWrt.nFlyWidth > 0 && rWrt.nFlyHeight > 0)
    {
        nWidth = rWrt.nFlyWidth;
        nHeight = rWrt.nFlyHeight;
    }
    else
    {
        Size aSize(rFly.GetSize());
        nWidth = aSize.Width();
        nHeight = aSize.Height();
    }
    maDetails.push_back(GraphicDetails(rFly, nWidth, nHeight));
}

void SwWW8WrGrf::WritePICFHeader(SvStream& rStrm, const sw::Frame &rFly,
    UINT16 mm, UINT16 nWidth, UINT16 nHeight, const SwAttrSet* pAttrSet)
{
    INT16 nXSizeAdd = 0, nYSizeAdd = 0;
    INT16 nCropL = 0, nCropR = 0, nCropT = 0, nCropB = 0;

            // Crop-AttributInhalt in Header schreiben ( falls vorhanden )
    const SfxPoolItem* pItem;
    if (pAttrSet && (SFX_ITEM_ON
        == pAttrSet->GetItemState(RES_GRFATR_CROPGRF, false, &pItem)))
    {
        const SwCropGrf& rCr = *(SwCropGrf*)pItem;
        nCropL = (INT16)rCr.GetLeft();
        nCropR = (INT16)rCr.GetRight();
        nCropT = (INT16)rCr.GetTop();
        nCropB = (INT16)rCr.GetBottom();
        nXSizeAdd -= (INT16)( rCr.GetLeft() + rCr.GetRight() );
        nYSizeAdd -= (INT16)( rCr.GetTop() + rCr.GetBottom() );
    }

    Size aGrTwipSz(rFly.GetSize());
    bool bWrtWW8 = rWrt.bWrtWW8;
    UINT16 nHdrLen = bWrtWW8 ? 0x44 : 0x3A;

    BYTE aArr[ 0x44 ] = { 0 };

    BYTE* pArr = aArr + 0x2E;  //Do borders first

    const SwAttrSet& rAttrSet = rFly.GetFrmFmt().GetAttrSet();
    if (SFX_ITEM_ON == rAttrSet.GetItemState(RES_BOX, false, &pItem))
    {
        const SvxBoxItem* pBox = (const SvxBoxItem*)pItem;
        if( pBox )
        {
            bool bShadow = false;               // Shadow ?
            const SvxShadowItem* pSI =
                sw::util::HasItem<SvxShadowItem>(rAttrSet, RES_SHADOW);
            if (pSI)
            {
                bShadow = (pSI->GetLocation() != SVX_SHADOW_NONE) &&
                    (pSI->GetWidth() != 0);
            }

            BYTE aLnArr[4] = { BOX_LINE_TOP, BOX_LINE_LEFT,
                                BOX_LINE_BOTTOM, BOX_LINE_RIGHT };
            for( BYTE i = 0; i < 4; ++i )
            {
                const SvxBorderLine* pLn = pBox->GetLine( aLnArr[ i ] );
                WW8_BRC aBrc;
                if (pLn)
                {
                    aBrc = rWrt.TranslateBorderLine( *pLn,
                        pBox->GetDistance( aLnArr[ i ] ), bShadow );
                }

                //use importer logic to determine how large the exported
                //border will really be in word and adjust accordingly
                short nSpacing;
                short nThick = aBrc.DetermineBorderProperties(!bWrtWW8,
                    &nSpacing);
                switch (aLnArr[ i ])
                {
                    case BOX_LINE_TOP:
                    case BOX_LINE_BOTTOM:
                        nHeight -= bShadow ? nThick*2 : nThick;
                        nHeight -= nSpacing;
                        break;
                    case BOX_LINE_LEFT:
                    case BOX_LINE_RIGHT:
                    default:
                        nWidth -= bShadow ? nThick*2 : nThick;
                        nWidth -= nSpacing;
                        break;
                }
                memcpy( pArr, &aBrc.aBits1, 2);
                pArr+=2;

                if( bWrtWW8 )
                {
                    memcpy( pArr, &aBrc.aBits2, 2);
                    pArr+=2;
                }
            }
        }
    }

    pArr = aArr + 4;                                //skip lcb
    Set_UInt16( pArr, nHdrLen );                    // set cbHeader

    Set_UInt16( pArr, mm );                         // set mm

    /*
    #92494#
    Just in case our original size is too big to fit inside a ushort we can
    substitute the final size and loose on retaining the scaling factor but
    still keep the correct display size anyway.
    */
    if ( (aGrTwipSz.Width() > USHRT_MAX) || (aGrTwipSz.Height() > USHRT_MAX)
        || (aGrTwipSz.Width() < 0 ) || (aGrTwipSz.Height() < 0) )
    {
        aGrTwipSz.Width() = nWidth;
        aGrTwipSz.Height() = nHeight;
    }
    using namespace sw::types;
    // set xExt & yExt
    Set_UInt16(pArr, msword_cast<sal_uInt16>(aGrTwipSz.Width() * 254L / 144));
    Set_UInt16(pArr, msword_cast<sal_uInt16>(aGrTwipSz.Height() * 254L / 144));
    pArr += 16;
    // skip hMF & rcWinMF
    // set dxaGoal & dyaGoal
    Set_UInt16(pArr, msword_cast<sal_uInt16>(aGrTwipSz.Width()));
    Set_UInt16(pArr, msword_cast<sal_uInt16>(aGrTwipSz.Height()));

    if( aGrTwipSz.Width() + nXSizeAdd )             // set mx
    {
        double fVal = nWidth * 1000.0 / (aGrTwipSz.Width() + nXSizeAdd);
        Set_UInt16( pArr, (USHORT)::rtl::math::round(fVal) );
    }
    else
        pArr += 2;

    if( aGrTwipSz.Height() + nYSizeAdd )            // set my
    {
        double fVal = nHeight * 1000.0 / (aGrTwipSz.Height() + nYSizeAdd);
        Set_UInt16( pArr, (USHORT)::rtl::math::round(fVal) );
    }
    else
        pArr += 2;

    Set_UInt16( pArr, nCropL );                     // set dxaCropLeft
    Set_UInt16( pArr, nCropT );                     // set dyaCropTop
    Set_UInt16( pArr, nCropR );                     // set dxaCropRight
    Set_UInt16( pArr, nCropB );                     // set dyaCropBottom

    rStrm.Write( aArr, nHdrLen );
}

void SwWW8WrGrf::WriteGrfFromGrfNode(SvStream& rStrm, const SwGrfNode &rGrfNd,
    const sw::Frame &rFly, UINT16 nWidth, UINT16 nHeight)
{
    if (rGrfNd.IsLinkedFile())     // Linked File
    {
        String aFileN, aFiltN;
        UINT16 mm;
        rGrfNd.GetFileFilterNms( &aFileN, &aFiltN );

        aFileN = INetURLObject::AbsToRel( aFileN, INetURLObject::WAS_ENCODED,
                                        INetURLObject::DECODE_UNAMBIGUOUS);
        INetURLObject aUrl( aFileN );
        if( aUrl.GetProtocol() == INET_PROT_FILE )
            aFileN = aUrl.PathToFileName();

//JP 05.12.98: nach einigen tests hat sich gezeigt, das WW mit 99 nicht
//              klarkommt. Sie selbst schreiben aber bei Verknuepfunfen,
//              egal um welchen Type es sich handelt, immer den Wert 94.
//              Bug 59859
//      if ( COMPARE_EQUAL == aFiltN.ICompare( "TIF", 3 ) )
//          mm = 99;                    // 99 = TIFF
//      else
            mm = 94;                    // 94 = BMP, GIF

        WritePICFHeader(rStrm, rFly, mm, nWidth, nHeight,
            rGrfNd.GetpSwAttrSet());
        rStrm << (BYTE)aFileN.Len();    // Pascal-String schreiben
        SwWW8Writer::WriteString8(rStrm, aFileN, false,
            RTL_TEXTENCODING_MS_1252);
    }
    else                                // Embedded File oder DDE oder so was
    {
        if (rWrt.bWrtWW8)
        {
            WritePICFHeader(rStrm, rFly, 0x64, nWidth, nHeight,
                rGrfNd.GetpSwAttrSet());
            SwBasicEscherEx aInlineEscher(&rStrm, rWrt);
            aInlineEscher.WriteGrfFlyFrame(rFly.GetFrmFmt(), 0x401);
            aInlineEscher.WritePictures();
        }
        else
        {
            Graphic& rGrf = const_cast<Graphic&>(rGrfNd.GetGrf());
            bool bSwapped = rGrf.IsSwapOut() ? true : false;
            // immer ueber den Node einswappen!
            const_cast<SwGrfNode&>(rGrfNd).SwapIn();

            GDIMetaFile aMeta;
            switch (rGrf.GetType())
            {
                case GRAPHIC_BITMAP:        // Bitmap -> in Metafile abspielen
                    {
                        VirtualDevice aVirt;
                        aMeta.Record(&aVirt);
                        aVirt.DrawBitmap( Point( 0,0 ), rGrf.GetBitmap() );
                        aMeta.Stop();
                        aMeta.WindStart();
                        aMeta.SetPrefMapMode( rGrf.GetPrefMapMode());
                        aMeta.SetPrefSize( rGrf.GetPrefSize());
                    }
                    break;
                case GRAPHIC_GDIMETAFILE :      // GDI ( =SV ) Metafile
                    aMeta = rGrf.GetGDIMetaFile();
                    break;
                default:
                    return;
            }

            WritePICFHeader(rStrm, rFly, 8, nWidth, nHeight,
                rGrfNd.GetpSwAttrSet());
            WriteWindowMetafileBits(rStrm, aMeta);

            if (bSwapped)
                rGrf.SwapOut();
        }
    }
}

void SwWW8WrGrf::WriteGraphicNode(SvStream& rStrm, const GraphicDetails &rItem)
{
    UINT16 nWidth = rItem.mnWid;
    UINT16 nHeight = rItem.mnHei;
    UINT32 nPos = rStrm.Tell();         // Grafik-Anfang merken

    const sw::Frame &rFly = rItem.maFly;
    switch (rFly.GetWriterType())
    {
        case sw::Frame::eGraphic:
        {
            const SwNode *pNode = rItem.maFly.GetContent();
            const SwGrfNode *pNd = pNode ? pNode->GetGrfNode() : 0;
            ASSERT(pNd, "Impossible");
            if (pNd)
                WriteGrfFromGrfNode(rStrm, *pNd, rItem.maFly, nWidth, nHeight);
        }
        break;
        case sw::Frame::eOle:
        {
#ifdef OLE_PREVIEW_AS_EMF
            const SwNode *pNode = rItem.maFly.GetContent();
            const SwOLENode *pNd = pNode ? pNode->GetOLENode() : 0;
            ASSERT(pNd, "Impossible");
            if (!rWrt.bWrtWW8)
            {
                SwOLENode *pOleNd = const_cast<SwOLENode*>(pNd);
                ASSERT( pOleNd, " Wer hat den OleNode versteckt ?" );
                SwOLEObj&                   rSObj= pOleNd->GetOLEObj();
                ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject > rObj(  rSObj.GetOleRef() );

                comphelper::EmbeddedObjectContainer aCnt( pOleNd->GetDoc()->GetDocStorage() );

                SvStream* pGraphicStream = ::utl::UcbStreamHelper::CreateStream( aCnt.GetGraphicStream( rObj ) );
                DBG_ASSERT( pGraphicStream && !pGraphicStream->GetError(), "No graphic stream available!" );
                if ( pGraphicStream && !pGraphicStream->GetError() )
                {
                    Graphic aGr;
                    GraphicFilter* pGF = GraphicFilter::GetGraphicFilter();
                    String aEmptyStr;
                    if( pGF->ImportGraphic( aGr, aEmptyStr, *pGraphicStream, GRFILTER_FORMAT_DONTKNOW ) == GRFILTER_OK )
                    {
                        //TODO/LATER: do we really want to use GDIMetafile?!
                        GDIMetaFile aMtf;
                        aMtf = aGr.GetGDIMetaFile();
                        aMtf.WindStart();
                        aMtf.Play(Application::GetDefaultDevice(), Point(0, 0),
                            Size(2880, 2880));
                        WritePICFHeader(rStrm, rFly, 8, nWidth, nHeight,
                            pNd->GetpSwAttrSet());
                        WriteWindowMetafileBits(rStrm, aMtf);
                    }
                }
                else
                    delete pGraphicStream;
            }
            else
            {
                //Convert this ole2 preview in ww8+ to an EMF for better unicode
                //support (note that at this moment this breaks StarSymbol
                //using graphics because I need to embed starsymbol in exported
                //documents.
                WritePICFHeader(rStrm, rFly, 0x64, nWidth, nHeight,
                    pNd->GetpSwAttrSet());
                SwBasicEscherEx aInlineEscher(&rStrm, rWrt);
                aInlineEscher.WriteOLEFlyFrame(rFly.GetFrmFmt(), 0x401);
                aInlineEscher.WritePictures();
            }
#else
            // cast away const
            SwOLENode *pOleNd = const_cast<SwOLENode*>(pNd);
            ASSERT( pOleNd, " Wer hat den OleNode versteckt ?" );
            SwOLEObj&                   rSObj= pOleNd->GetOLEObj();

            // TODO/LATER: do we need to load object?
            Graphic* pGr = SdrOle2Obj::GetGraphicFromObject( pOleNd->GetDoc()->GetDocStorage(), rObj );

            //TODO/LATER: do we really want to use GDIMetafile?!
            GDIMetaFile aMtf;
            if ( pGr )
                aMtf = pGr->GetGDIMetaFile();

            Size aS(aMtf.GetPrefSize());
            aMtf.WindStart();
            aMtf.Play(Application::GetDefaultDevice(), Point(0, 0),
                Size(2880, 2880));

            WritePICFHeader(rStrm, rFly, 8, nWidth, nHeight,
                pNd->GetpSwAttrSet());
            WriteWindowMetafileBits(rStrm, aMtf);
            delete pGr;
#endif
        }
        break;
        case sw::Frame::eDrawing:
        case sw::Frame::eTxtBox:
            ASSERT(rWrt.bWrtWW8,
                "You can't try and export these in WW8 format, a filter bug");
            /*
            #i3958# We only export an empty dummy picture frame here, this is
            what word does the escher export should contain an anchored to
            character element which is drawn over this dummy and the whole
            shebang surrounded with a SHAPE field. This isn't *my* hack :-),
            its what word does.
            */
            if (rWrt.bWrtWW8)
            {
                WritePICFHeader(rStrm, rFly, 0x64, nWidth, nHeight);
                SwBasicEscherEx aInlineEscher(&rStrm, rWrt);
                aInlineEscher.WriteEmptyFlyFrame(rFly.GetFrmFmt(), 0x401);
            }
            break;
        default:
            ASSERT(!this,
           "Some inline export not implemented, remind cmc before we ship :-)");
            break;
    }

    UINT32 nPos2 = rStrm.Tell();                    // Ende merken
    rStrm.Seek( nPos );
    SVBT32 nLen;
    LongToSVBT32( nPos2 - nPos, nLen );             // Grafik-Laenge ausrechnen
    rStrm.Write( nLen, 4 );                         // im Header einpatchen
    rStrm.Seek( nPos2 );                            // Pos wiederherstellen
}

// SwWW8WrGrf::Write() wird nach dem Text gerufen. Es schreibt die alle
// Grafiken raus und merkt sich die File-Positionen der Grafiken, damit
// beim Schreiben der Attribute die Positionen in die PicLocFc-Sprms
// eingepatcht werden koennen.
// Das Suchen in den Attributen nach dem Magic ULONG und das Patchen
// passiert beim Schreiben der Attribute. Die SwWW8WrGrf-Klasse liefert
// hierfuer nur mit GetFPos() sequentiell die Positionen.
void SwWW8WrGrf::Write()
{
    SvStream& rStrm = *rWrt.pDataStrm;
    myiter aEnd = maDetails.end();
    for (myiter aIter = maDetails.begin(); aIter != aEnd; ++aIter)
    {
        UINT32 nPos = rStrm.Tell();                 // auf 4 Bytes alignen
        if( nPos & 0x3 )
            SwWW8Writer::FillCount( rStrm, 4 - ( nPos & 0x3 ) );

        bool bDuplicated = false;
        for (myiter aIter2 = maDetails.begin(); aIter2 != aIter; ++aIter2)
        {
            if (*aIter2 == *aIter)
            {
                aIter->mnPos = aIter2->mnPos;
                bDuplicated = true;
                break;
            }
        }

        if (!bDuplicated)
        {
            aIter->mnPos = rStrm.Tell();
            WriteGraphicNode(rStrm, *aIter);
        }
    }
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
