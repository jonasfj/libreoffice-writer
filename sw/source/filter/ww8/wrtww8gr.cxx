/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: wrtww8gr.cxx,v $
 * $Revision: 1.54 $
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#include <com/sun/star/embed/XEmbedPersist.hpp>
#include <com/sun/star/embed/Aspects.hpp>
#include <rtl/math.hxx>
#include <svtools/filter.hxx>
#include <svtools/itemiter.hxx>
#include "svtools/urihelper.hxx"

#include <svtools/embedhlp.hxx>

#include <vcl/virdev.hxx>
#include <vcl/svapp.hxx>

#include <hintids.hxx>
#include <svx/boxitem.hxx>
#include <svx/shaditem.hxx>
#include <svx/shaditem.hxx>
#include <svx/msoleexp.hxx>
#include <svx/lrspitem.hxx> // SvxLRSpaceItem
#include <svx/ulspitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/svdoole2.hxx>

#include <unotools/ucbstreamhelper.hxx>
#include <fmtanchr.hxx>
#include <ndgrf.hxx>
#include <frmfmt.hxx>       // class SwFlyFrmFmt
#include <grfatr.hxx>       // class SwCropGrf
#include <ndole.hxx>
#include <ndtxt.hxx>
#include <fmtfsize.hxx>
#include <fmtornt.hxx>

#include <writerfilter/doctok/sprmids.hxx>

#include <doc.hxx>
#include "writerhelper.hxx"
#include "writerwordglue.hxx"
#include "ww8struc.hxx"
#include "wrtww8.hxx"
#include "ww8par.hxx"
#include "escher.hxx"

#include "docsh.hxx"

using namespace ::com::sun::star;
using namespace nsFieldFlags;

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

void WW8Export::OutputGrfNode( const SwGrfNode& /*rNode*/ )
{
#if OSL_DEBUG_LEVEL > 0
    fprintf( stderr, "WW8Export::OutputGrfNode( const SwGrfNode& )\n" );
#endif
    ASSERT( mpParentFrame, "frame not set!" );
    if ( mpParentFrame )
    {
        OutGrf( *mpParentFrame );
        pFib->fHasPic = 1;
    }
}

bool WW8Export::TestOleNeedsGraphic(const SwAttrSet& rSet,
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
        // bGraphicNeeded set to true is right / fixes #i51670#.
        bGraphicNeeded = true;
        Point aTmpPoint;
        Rectangle aRect( aTmpPoint, Size( nX, nY ) );
        Graphic aGraph(aWMF);

        ErrCode nErr = ERRCODE_NONE;
        Rectangle aVisArea;
        sal_Int64 nAspect = embed::Aspects::MSOLE_CONTENT;
        if ( pOLENd )
            nAspect = pOLENd->GetAspect();
        SdrOle2Obj *pRet = SvxMSDffManager::CreateSdrOLEFromStorage(
            rStorageName,xObjStg,pDoc->GetDocStorage(),aGraph,aRect,aVisArea,0,nErr,0,nAspect);

        if (pRet)
        {
            uno::Reference< embed::XEmbeddedObject > xObj = pOLENd->GetOLEObj().GetOleRef();
            if ( xObj.is() )
            {
                SvStream* pGraphicStream = NULL;
                comphelper::EmbeddedObjectContainer aCnt( pDoc->GetDocStorage() );
                try
                {
                    uno::Reference< embed::XEmbedPersist > xPersist(
                            xObj,
                            uno::UNO_QUERY_THROW );

                    // it makes no sence to search the object in the container by reference since the object was created
                    // outside of the container and was not inserted there, only the name makes sence
                    pGraphicStream =
                            ::utl::UcbStreamHelper::CreateStream( aCnt.GetGraphicStream( xPersist->getEntryName() ) );
                }
                catch( uno::Exception& )
                {}

                DBG_ASSERT( pGraphicStream && !pGraphicStream->GetError(), "No graphic stream available!" );
                if ( pGraphicStream && !pGraphicStream->GetError() )
                {
                    Graphic aGr1;
                    GraphicFilter* pGF = GraphicFilter::GetGraphicFilter();
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
            }

            delete pRet;
        }
    }
    else
        bGraphicNeeded = true;
    return bGraphicNeeded;
#endif
}

void WW8Export::OutputOLENode( const SwOLENode& rOLENode )
{
#if OSL_DEBUG_LEVEL > 0
    fprintf( stderr, "WW8Export::OutputOLENode( const SwOLENode& rOLENode )\n" );
#endif
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

    if ( bWrtWW8 )
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

    SvStorageRef xObjStg = GetWriter().GetStorage().OpenSotStorage(
        CREATE_CONST_ASC(SL::aObjectPool), STREAM_READWRITE |
        STREAM_SHARE_DENYALL );

    if( xObjStg.Is()  )
    {
        uno::Reference < embed::XEmbeddedObject > xObj(const_cast<SwOLENode&>(rOLENode).GetOLEObj().GetOleRef());
        if( xObj.is() )
        {
            embed::XEmbeddedObject *pObj = xObj.get();
            sal_uInt32 nPictureId = (sal_uInt32)(sal_uIntPtr)pObj;
            Set_UInt32(pDataAdr, nPictureId);

            WW8OleMap *pMap = new WW8OleMap(nPictureId);
            bool bDuplicate = false;
            WW8OleMaps &rOleMap = GetOLEMap();
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
                {
                    sal_Int64 nAspect = rOLENode.GetAspect();
                    svt::EmbeddedObjectRef aObjRef( xObj, nAspect );
                    GetOLEExp().ExportOLEObject( aObjRef, *xOleStg );
                    if ( nAspect == embed::Aspects::MSOLE_ICON )
                    {
                        ::rtl::OUString aObjInfo( RTL_CONSTASCII_USTRINGPARAM( "\3ObjInfo" ) );
                        if ( !xOleStg->IsStream( aObjInfo ) )
                        {
                            const BYTE pObjInfoData[] = { 0x40, 0x00, 0x03, 0x00 };
                            SvStorageStreamRef rObjInfoStream = xOleStg->OpenSotStream( aObjInfo );
                            if ( rObjInfoStream.Is() && !rObjInfoStream->GetError() )
                            {
                                rObjInfoStream->Write( pObjInfoData, sizeof( pObjInfoData ) );
                                xOleStg->Commit();
                            }
                        }
                    }
                }

                // write as embedded field - the other things will be done
                // in the escher export
                String sServer(FieldString(ww::eEMBED));
                sServer += xOleStg->GetUserName();
                sServer += ' ';

                OutputField(0, ww::eEMBED, sServer, WRITEFIELD_START |
                    WRITEFIELD_CMD_START | WRITEFIELD_CMD_END);

                pChpPlc->AppendFkpEntry( Strm().Tell(),
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

                if (mpParentFrame)
                {
                    bGraphicNeeded = true;

                    if (mpParentFrame->IsInline())
                    {
                        const SwAttrSet& rSet =
                            mpParentFrame->GetFrmFmt().GetAttrSet();
                        bEndCR = false;
                        bGraphicNeeded = TestOleNeedsGraphic(rSet,
                            xOleStg, xObjStg, sStorageName, const_cast<SwOLENode*>(&rOLENode));
                    }
                }

                if (!bGraphicNeeded)
                    WriteChar(0x1);
                else
                {
                    /*
                    ##897##
                    We need to insert the graphic representation of
                    this object for the inline case, otherwise word
                    has no place to find the dimensions of the ole
                    object, and will not be able to draw it
                    */
                    OutGrf(*mpParentFrame);
                }

                OutputField(0, ww::eEMBED, aEmptyStr,
                    WRITEFIELD_END | WRITEFIELD_CLOSE);

                if (bEndCR) //No newline in inline case
                    WriteCR();
            }
        }
    }
}

void WW8Export::OutGrf(const sw::Frame &rFrame)
{
    // GrfNode fuer spaeteres rausschreiben der Grafik merken
    pGrf->Insert(rFrame);

    pChpPlc->AppendFkpEntry( Strm().Tell(), pO->Count(), pO->GetData() );
    pO->Remove( 0, pO->Count() );                   // leeren

    // --> OD 2007-06-06 #i29408#
    // linked, as-character anchored graphics have to be exported as fields.
    const SwGrfNode* pGrfNd = rFrame.IsInline() && rFrame.GetContent()
                              ? rFrame.GetContent()->GetGrfNode() : 0;
    if ( pGrfNd && pGrfNd->IsLinkedFile() )
    {
        String sStr( FieldString(ww::eINCLUDEPICTURE) );
        sStr.APPEND_CONST_ASC(" \"");
        {
            if ( pGrfNd )
            {
                String aFileURL;
                pGrfNd->GetFileFilterNms( &aFileURL, 0 );
                sStr += aFileURL;
            }
        }
        sStr.APPEND_CONST_ASC("\" \\d");

        OutputField( 0, ww::eINCLUDEPICTURE, sStr,
                   WRITEFIELD_START | WRITEFIELD_CMD_START | WRITEFIELD_CMD_END );
    }
    // <--

    WriteChar( (char)1 );   // Grafik-Sonderzeichen in Haupttext einfuegen

    BYTE aArr[ 18 ];
    BYTE* pArr = aArr;

    const SwFrmFmt &rFlyFmt = rFrame.GetFrmFmt();
    const RndStdIds eAn = rFlyFmt.GetAttrSet().GetAnchor(false).GetAnchorId();
    if( eAn == FLY_IN_CNTNT )
    {
        sal_Int16 eVert = rFlyFmt.GetVertOrient().GetVertOrient();
        if ((eVert == text::VertOrientation::CHAR_CENTER) || (eVert == text::VertOrientation::LINE_CENTER))
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
                    Set_UInt16( pArr, NS_sprm::LN_CHpsPos );
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
        Set_UInt16( pArr, NS_sprm::LN_CPicLocation );
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
    pChpPlc->AppendFkpEntry( Strm().Tell(), static_cast< short >(pArr - aArr), aArr );

    // --> OD 2007-04-23 #i75464#
    // Check, if graphic isn't exported as-character anchored.
    // Otherwise, an additional paragraph is exported for a graphic, which is
    // forced to be treated as inline, because it's anchored inside another frame.
    if ( !rFrame.IsInline() &&
         ( ( eAn == FLY_AT_CNTNT && ( bWrtWW8 || !bIsInTable ) ) ||
           eAn == FLY_PAGE ) )
    // <--
    {
        WriteChar( (char)0x0d ); // umgebenden Rahmen mit CR abschliessen

        static BYTE __READONLY_DATA nSty[2] = { 0, 0 };
        pO->Insert( nSty, 2, pO->Count() );     // Style #0
        bool bOldGrf = bOutGrf;
        bOutGrf = true;

        OutputFormat( rFrame.GetFrmFmt(), false, false, true ); // Fly-Attrs

        bOutGrf = bOldGrf;
        pPapPlc->AppendFkpEntry( Strm().Tell(), pO->Count(), pO->GetData() );
        pO->Remove( 0, pO->Count() );                   // leeren
    }
    // --> OD 2007-06-06 #i29408#
    // linked, as-character anchored graphics have to be exported as fields.
    else if ( pGrfNd && pGrfNd->IsLinkedFile() )
    {
        OutputField( 0, ww::eINCLUDEPICTURE, String(), WRITEFIELD_CLOSE );
    }
    // <--
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
    const Size aSize( rFly.GetLayoutSize() );
    const UINT16 nWidth = static_cast< UINT16 >(aSize.Width());
    const UINT16 nHeight = static_cast< UINT16 >(aSize.Height());
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
        nXSizeAdd = nXSizeAdd - (INT16)( rCr.GetLeft() + rCr.GetRight() );
        nYSizeAdd = nYSizeAdd - (INT16)( rCr.GetTop() + rCr.GetBottom() );
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
                        nHeight = nHeight - nSpacing;
                        break;
                    case BOX_LINE_LEFT:
                    case BOX_LINE_RIGHT:
                    default:
                        nWidth -= bShadow ? nThick*2 : nThick;
                        nWidth = nWidth - nSpacing;
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
        String aFileN;
        rGrfNd.GetFileFilterNms( &aFileN, 0 );

        // --> OD 2007-06-06 #i29408# - take the file URL as it is.
//        aFileN = URIHelper::simpleNormalizedMakeRelative(rWrt.GetBaseURL(),
//                                          aFileN);
//        INetURLObject aUrl( aFileN );
//        if( aUrl.GetProtocol() == INET_PROT_FILE )
//            aFileN = aUrl.PathToFileName();
        // <--

//JP 05.12.98: nach einigen tests hat sich gezeigt, das WW mit 99 nicht
//              klarkommt. Sie selbst schreiben aber bei Verknuepfunfen,
//              egal um welchen Type es sich handelt, immer den Wert 94.
//              Bug 59859
//      if ( COMPARE_EQUAL == aFiltN.ICompare( "TIF", 3 ) )
//          mm = 99;                    // 99 = TIFF
//      else
            UINT16 mm = 94;                    // 94 = BMP, GIF

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
                uno::Reference < embed::XEmbeddedObject > rObj(  rSObj.GetOleRef() );

                comphelper::EmbeddedObjectContainer aCnt( pOleNd->GetDoc()->GetDocStorage() );

                SvStream* pGraphicStream = ::utl::UcbStreamHelper::CreateStream( aCnt.GetGraphicStream( rObj ) );
                DBG_ASSERT( pGraphicStream && !pGraphicStream->GetError(), "No graphic stream available!" );
                if ( pGraphicStream && !pGraphicStream->GetError() )
                {
                    Graphic aGr;
                    GraphicFilter* pGF = GraphicFilter::GetGraphicFilter();
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
        case sw::Frame::eFormControl:
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
    UInt32ToSVBT32( nPos2 - nPos, nLen );             // Grafik-Laenge ausrechnen
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
