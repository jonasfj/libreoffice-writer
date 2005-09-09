/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: wrtw8esh.cxx,v $
 *
 *  $Revision: 1.85 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:07:39 $
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

#ifndef _COM_SUN_STAR_EMBED_ASPECTS_HPP_
#include <com/sun/star/embed/Aspects.hpp>
#endif

#ifdef PCH
#include "filt_pch.hxx"
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#define _SVSTDARR_ULONGSSORT
#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>

#ifndef _SV_CVTGRF_HXX
#include <vcl/cvtgrf.hxx>
#endif
#ifndef _SV_VIRDEV_HXX
#include <vcl/virdev.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#include <sot/storage.hxx>
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
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif
#ifndef _SVDOOLE2_HXX
#include <svx/svdoole2.hxx>
#endif

#ifndef _MyEDITENG_HXX
#include <svx/editeng.hxx>
#endif
#ifndef _SVX_FLDITEM_HXX
//miserable hack to get around #98519#
#define ITEMID_FIELD            EE_FEATURE_FIELD
#include <svx/flditem.hxx>
#endif

#include <comphelper/seqstream.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <svtools/filter.hxx>

#ifndef _SVX_FMGLOB_HXX
#include <svx/fmglob.hxx>
#endif
#ifndef _SVDOUNO_HXX
#include <svx/svdouno.hxx>
#endif
#ifndef _SVX_UNOAPI_HXX_
#include <svx/unoapi.hxx>
#endif
#ifndef _SVDXCGV_HXX
#include <svx/svdxcgv.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
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
// --> OD 2005-01-06 #i30669#
#ifndef _FMTFOLLOWTEXTFLOW_HXX
#include <fmtfollowtextflow.hxx>
#endif
// <--
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
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
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
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _GRFMGR_HXX
#include <goodies/grfmgr.hxx>
#endif

#ifndef SW_WRITERHELPER
#include "writerhelper.hxx"
#endif
#ifndef SW_WRITERWORDGLUE
#include "writerwordglue.hxx"
#endif
#ifndef _WRTWW8_HXX
#include "wrtww8.hxx"
#endif
#ifndef _ESCHER_HXX
#include "escher.hxx"
#endif
#ifndef WW_FIELDS_HXX
#include "fields.hxx"
#endif

using namespace com::sun::star;
using namespace sw::util;
using namespace sw::types;
//#110185# get a part fix for this type of element
bool SwWW8Writer::MiserableFormFieldExportHack(const SwFrmFmt& rFrmFmt)
{
    ASSERT(bWrtWW8, "Not allowed");
    if (!bWrtWW8)
        return false;
    bool bHack = false;
    const SdrObject *pObject = rFrmFmt.FindRealSdrObject();
    if (pObject && pObject->GetObjInventor() == FmFormInventor)
    {
        if (SdrUnoObj *pFormObj = PTR_CAST(SdrUnoObj,pObject))
        {
            uno::Reference< awt::XControlModel > xControlModel =
                pFormObj->GetUnoControlModel();
            uno::Reference< lang::XServiceInfo > xInfo(xControlModel,
                uno::UNO_QUERY);
            uno::Reference<beans::XPropertySet> xPropSet(xControlModel, uno::UNO_QUERY);
            if (xInfo->supportsService(C2U("com.sun.star.form.component.ComboBox")))
            {
                DoComboBox(xPropSet);
                bHack = true;
            }
            else if (xInfo->supportsService(C2U("com.sun.star.form.component.CheckBox")))
            {
                DoCheckBox(xPropSet);
                bHack = true;
            }
        }
    }
    return bHack;
}


void SwWW8Writer::DoComboBox(uno::Reference<beans::XPropertySet> xPropSet)
{
    rtl::OUString sSelected;
    uno::Sequence<rtl::OUString> aListItems;
    xPropSet->getPropertyValue(C2U("StringItemList")) >>= aListItems;
    sal_Int32 nNoStrings = aListItems.getLength();
    if (nNoStrings)
    {
        uno::Any aTmp = xPropSet->getPropertyValue(C2U("DefaultText"));
        const rtl::OUString *pStr = (const rtl::OUString *)aTmp.getValue();
        if (pStr)
            sSelected = *pStr;
    }

    rtl::OUString sName;
    uno::Any aTmp = xPropSet->getPropertyValue(C2U("Name"));
    const rtl::OUString *pStr = (const rtl::OUString *)aTmp.getValue();
    if (pStr)
        sName = *pStr;

    DoComboBox(sName, sSelected, aListItems);
}

void SwWW8Writer::DoComboBox(const rtl::OUString &rName, const rtl::OUString &rSelected, uno::Sequence<rtl::OUString> &rListItems)
{
    ASSERT(bWrtWW8, "Not allowed");
    if (!bWrtWW8)
        return;
    OutField(0, ww::eFORMDROPDOWN, FieldString(ww::eFORMDROPDOWN),
        WRITEFIELD_START | WRITEFIELD_CMD_START);
    // write the refence to the "picture" structure
    ULONG nDataStt = pDataStrm->Tell();
    pChpPlc->AppendFkpEntry( Strm().Tell() );

    WriteChar( 0x01 );

    static BYTE aArr1[] =
    {
        0x03, 0x6a, 0,0,0,0,    // sprmCPicLocation
        0x06, 0x08, 0x01,       // sprmCFData
        0x55, 0x08, 0x01,       // sprmCFSpec
        0x02, 0x08, 0x01        // sprmCFFldVanish
    };
    BYTE* pDataAdr = aArr1 + 2;
    Set_UInt32( pDataAdr, nDataStt );

    pChpPlc->AppendFkpEntry(Strm().Tell(), sizeof(aArr1), aArr1);

    OutField(0, ww::eFORMDROPDOWN, FieldString(ww::eFORMDROPDOWN),
        WRITEFIELD_CLOSE);

    static const sal_uInt8 aComboData1[] =
    {
        0,0,0,0,        // len of struct
        0x44,0,         // the start of "next" data
        0,0,0,0,0,0,0,0,0,0,                // PIC-Structure!
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    //  |
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    //  |
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    //  |
        0,0,0,0,                            // /
    };
    pDataStrm->Write( aComboData1, sizeof(aComboData1) );

    static const sal_uInt8 aComboData2[] =
    {
        0xFF, 0xFF, 0xFF, 0xFF
    };
    pDataStrm->Write( aComboData2, sizeof(aComboData2) );

    sal_uInt8 nHeaderByte = 0x2;
    sal_uInt32 nNoStrings = rListItems.getLength();
    if (nNoStrings)
    {
        bool bSelectedDone = false;
        sal_uInt32 i;

        for ( i = 0; i < nNoStrings; ++i)
        {
            if (rSelected == rListItems[i])
            {
                bSelectedDone = true;
                break;
            }
        }
        if (i <= 0x3F) //only 6 bit available for selected item
            nHeaderByte |= (i << 2);
    }

    *pDataStrm << nHeaderByte;

    static const sal_uInt8 aComboData9[] =
    {
        0x80, 0x00, 0x00, 0x00, 0x00
    };
    pDataStrm->Write( aComboData9, sizeof(aComboData9) );

    sal_uInt16 nLen = msword_cast<sal_uInt16>(rName.getLength());
    *pDataStrm << nLen;
    WriteString16(*pDataStrm, rName, true);

    static const sal_uInt8 aComboData3[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
    };

    pDataStrm->Write( aComboData3, sizeof(aComboData3) );

    *pDataStrm << nNoStrings;
    if (!nNoStrings)
        *pDataStrm << sal_uInt16(0);
    else
    {
        for (sal_uInt32 i = 0; i < nNoStrings; ++i)
        {
            const rtl::OUString &rItem = rListItems[i];
            sal_uInt16 nStrLen = msword_cast<sal_uInt16>(rItem.getLength());
            *pDataStrm << nStrLen;
            WriteString16(*pDataStrm, rItem, false);
        }
    }

    SwWW8Writer::WriteLong( *pDataStrm, nDataStt,
        pDataStrm->Tell() - nDataStt );
}

void SwWW8Writer::DoCheckBox(uno::Reference<beans::XPropertySet> xPropSet)
{
    OutField(0, ww::eFORMCHECKBOX, FieldString(ww::eFORMCHECKBOX),
        WRITEFIELD_START | WRITEFIELD_CMD_START);
    // write the refence to the "picture" structure
    ULONG nDataStt = pDataStrm->Tell();
    pChpPlc->AppendFkpEntry( Strm().Tell() );

    WriteChar( 0x01 );
    static BYTE aArr1[] = {
        0x03, 0x6a, 0,0,0,0,    // sprmCPicLocation

        0x06, 0x08, 0x01,       // sprmCFData
        0x55, 0x08, 0x01,       // sprmCFSpec
        0x02, 0x08, 0x01        // sprmCFFldVanish
    };
    BYTE* pDataAdr = aArr1 + 2;
    Set_UInt32( pDataAdr, nDataStt );

    pChpPlc->AppendFkpEntry(Strm().Tell(),
                sizeof( aArr1 ), aArr1 );

    static const sal_uInt8 aComboData1[] =
    {
        0,0,0,0,        // len of struct
        0x44,0,         // the start of "next" data
        0,0,0,0,0,0,0,0,0,0,                // PIC-Structure!
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    //  |
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    //  |
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    //  |
        0,0,0,0,                            // /
    };
    pDataStrm->Write( aComboData1, sizeof(aComboData1) );

    static const sal_uInt8 aComboData2[] =
    {
        0xFF, 0xFF, 0xFF, 0xFF
    };
    pDataStrm->Write( aComboData2, sizeof(aComboData2) );

    sal_Int16 nTemp;
    xPropSet->getPropertyValue(C2U("DefaultState")) >>= nTemp;
    sal_uInt32 nIsDefaultChecked(nTemp);

    xPropSet->getPropertyValue(C2U("State")) >>= nTemp;
    sal_uInt32 nIsChecked(nTemp);
    sal_uInt8 nHeaderByte = 0x65;
    if (nIsDefaultChecked != nIsChecked)
    {
        switch (nIsChecked)
        {
            case false:
                nHeaderByte = 0x1;
                break;
            case true:
                nHeaderByte = 0x5;
                break;
            default:
                ASSERT(!this, "how did that happen");
        }
    }
    *pDataStrm << nHeaderByte;

    static const sal_uInt8 aComboData5[] =
    {
        0x00, 0x00, 0x00, 0x14, 0x00
    };
    pDataStrm->Write( aComboData5, sizeof(aComboData5) );

    uno::Any aTmp = xPropSet->getPropertyValue(C2U("Name"));
    const rtl::OUString *pStr = (const rtl::OUString *)aTmp.getValue();
    sal_uInt16 nLen = pStr ? msword_cast<sal_uInt16>(pStr->getLength()) : 0;
    *pDataStrm << nLen;
    if (pStr)
        WriteString16(*pDataStrm, String(*pStr), true);
    else
        WriteString16(*pDataStrm, aEmptyStr, true);

    *pDataStrm << nIsDefaultChecked;

    static const sal_uInt8 aComboData3[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    pDataStrm->Write( aComboData3, sizeof(aComboData3) );

    SwWW8Writer::WriteLong( *pDataStrm, nDataStt,
        pDataStrm->Tell() - nDataStt );

    OutField(0, ww::eFORMCHECKBOX, aEmptyStr, WRITEFIELD_CLOSE);
}

namespace wwUtility
{
    Graphic MakeSafeGDIMetaFile(const com::sun::star::uno::Reference < com::sun::star::embed::XEmbeddedObject >& xObj );
};


PlcDrawObj::~PlcDrawObj()
{
}

//Its irritating to have to change the RTL frames position into LTR ones
//so that word will have to place them in the right place. Doubly so that
//the SO drawings and writer frames have different ideas themselves as to
//how to be positioned when in RTL mode!
bool RTLGraphicsHack(long &rLeft, long nWidth,
    SwHoriOrient eHoriOri, SwRelationOrient eHoriRel, SwTwips nPageLeft,
    SwTwips nPageRight, SwTwips nPageSize)
{
    bool bRet = false;
    if (eHoriOri == HORI_NONE)
    {
        if (eHoriRel == REL_PG_FRAME)
        {
            rLeft = nPageSize - rLeft;
            bRet = true;
        }
        else if (
                  (eHoriRel == REL_PG_PRTAREA) ||
                  (eHoriRel == FRAME) ||
                  (eHoriRel == PRTAREA)
                )
        {
            rLeft = nPageSize - nPageLeft - nPageRight - rLeft;
            bRet = true;
        }
    }
    if (bRet)
        rLeft -= nWidth;
    return bRet;
}

bool RTLDrawingsHack(long &rLeft, long nWidth,
    SwHoriOrient eHoriOri, SwRelationOrient eHoriRel, SwTwips nPageLeft,
    SwTwips nPageRight, SwTwips nPageSize)
{
    bool bRet = false;
    if (eHoriOri == HORI_NONE)
    {
        if (eHoriRel == REL_PG_FRAME)
        {
            rLeft = nPageSize + rLeft;
            bRet = true;
        }
        else if (
                  (eHoriRel == REL_PG_PRTAREA) ||
                  (eHoriRel == FRAME) ||
                  (eHoriRel == PRTAREA)
                )
        {
            rLeft = nPageSize - nPageLeft - nPageRight + rLeft;
            bRet = true;
        }
    }
    return bRet;
}

bool SwWW8Writer::MiserableRTLFrmFmtHack(long &rLeft, long &rRight,
    const sw::Frame &rFrmFmt)
{
    //Require nasty bidi swap
    if (FRMDIR_HORI_RIGHT_TOP != pDoc->GetTextDirection(rFrmFmt.GetPosition()))
        return false;

    long nWidth = rRight - rLeft;
    SwTwips nPageLeft, nPageRight;
    SwTwips nPageSize = CurrentPageWidth(nPageLeft, nPageRight);

    const SwFmtHoriOrient& rHOr = rFrmFmt.GetFrmFmt().GetHoriOrient();

    bool bRet = false;
    sw::Frame::WriterSource eSource = rFrmFmt.GetWriterType();
    if (eSource == sw::Frame::eDrawing || eSource == sw::Frame::eFormControl)
    {
        if (RTLDrawingsHack(rLeft, nWidth, rHOr.GetHoriOrient(),
            rHOr.GetRelationOrient(), nPageLeft, nPageRight, nPageSize))
        {
            bRet = true;
        }
    }
    else
    {
        if (RTLGraphicsHack(rLeft, nWidth, rHOr.GetHoriOrient(),
            rHOr.GetRelationOrient(), nPageLeft, nPageRight, nPageSize))
        {
            bRet = true;
        }
    }
    if (bRet)
        rRight = rLeft + nWidth;
    return bRet;
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

        cDrawObjIter aEnd = maDrawObjs.end();
        cDrawObjIter aIter;

        for (aIter = maDrawObjs.begin(); aIter < aEnd; ++aIter)
            SwWW8Writer::WriteLong(*rWrt.pTableStrm, aIter->mnCp - nCpOffs);

        SwWW8Writer::WriteLong(*rWrt.pTableStrm, rFib.ccpText + rFib.ccpFtn +
            rFib.ccpHdr + rFib.ccpEdn + rFib.ccpTxbx + rFib.ccpHdrTxbx + 1);

        for (aIter = maDrawObjs.begin(); aIter < aEnd; ++aIter)
        {
            // write the fspa-struct
            const sw::Frame &rFrmFmt = aIter->maCntnt;
            const SwFrmFmt &rFmt = rFrmFmt.GetFrmFmt();
            const SdrObject* pObj = rFmt.FindRealSdrObject();

            Rectangle aRect;
            SwFmtVertOrient rVOr = rFmt.GetVertOrient();
            SwFmtHoriOrient rHOr = rFmt.GetHoriOrient();
            // --> OD 2005-01-06 #i30669# - convert the positioning attributes.
            // Most positions are converted, if layout information exists.
            const bool bPosConverted =
                 WinwordAnchoring::ConvertPosition( rHOr, rVOr, rFmt );
            // <--

            Point aObjPos;
            if (RES_FLYFRMFMT == rFmt.Which())
            {
                SwRect aLayRect(rFmt.FindLayoutRect(false, &aObjPos));
                // the Object is not visible - so get the values from
                // the format. The Position may not be correct.
                if( aLayRect.IsEmpty() )
                    aRect.SetSize( rFmt.GetFrmSize().GetSize() );
                else
                    aRect = aLayRect.SVRect();
            }
            else
            {
                ASSERT(pObj, "wo ist das SDR-Object?");
                if (pObj)
                {
                    aRect = pObj->GetSnapRect();
                }
            }

            // --> OD 2005-01-06 #i30669# - use converted position, if conversion
            // is performed. Unify position determination of Writer fly frames
            // and drawing objects.
            if ( bPosConverted )
            {
                aRect.SetPos( Point( rHOr.GetPos(), rVOr.GetPos() ) );
            }
            else
            {
                aRect -= aIter->maParentPos;
                aObjPos = aRect.TopLeft();
                if (VERT_NONE == rVOr.GetVertOrient())
                {
                    // CMC, OD 24.11.2003 #i22673#
                    SwRelationOrient eOri = rVOr.GetRelationOrient();
                    if (eOri == REL_CHAR || eOri == REL_VERT_LINE)
                        aObjPos.Y() = -rVOr.GetPos();
                    else
                        aObjPos.Y() = rVOr.GetPos();
                }
                if (HORI_NONE == rHOr.GetHoriOrient())
                    aObjPos.X() = rHOr.GetPos();
                aRect.SetPos( aObjPos );
            }
            // <--

            INT32 nThick = aIter->mnThick;

            //If we are being exported as an inline hack, set
            //corner to 0 and forget about border thickness for positioning
            if (rFrmFmt.IsInline())
            {
                aRect.SetPos(Point(0,0));
                nThick = 0;
            }

            // spid
            SwWW8Writer::WriteLong(*rWrt.pTableStrm, aIter->mnShapeId);

            sal_Int32 nLeft = aRect.Left() + nThick;
            sal_Int32 nRight = aRect.Right() - nThick;

            //Nasty swap for bidi if neccessary
            rWrt.MiserableRTLFrmFmtHack(nLeft, nRight, rFrmFmt);

            //xaLeft/yaTop/xaRight/yaBottom - rel. to anchor
            //(most of) the border is outside the graphic is word, so
            //change dimensions to fit
            SwWW8Writer::WriteLong(*rWrt.pTableStrm, nLeft);
            SwWW8Writer::WriteLong(*rWrt.pTableStrm,aRect.Top() + nThick);
            SwWW8Writer::WriteLong(*rWrt.pTableStrm, nRight);
            SwWW8Writer::WriteLong(*rWrt.pTableStrm,aRect.Bottom() - nThick);

            //fHdr/bx/by/wr/wrk/fRcaSimple/fBelowText/fAnchorLock
            USHORT nFlags=0;
            //If nFlags isn't 0x14 its overridden by the escher properties
            if( FLY_PAGE == rFmt.GetAnchor().GetAnchorId())
                nFlags = 0x0000;
            else
                nFlags = 0x0014;        // x-rel to text,  y-rel to text

            const SwFmtSurround& rSurr = rFmt.GetSurround();
            USHORT nContour = rSurr.IsContour() ? 0x0080 : 0x0040;
            SwSurround eSurround = rSurr.GetSurround();

            /*
             #i3958#
             The inline elements being export as anchored to character inside
             the shape field hack are required to be wrap through so as to flow
             over the following dummy 0x01 graphic
            */
            if (rFrmFmt.IsInline())
                eSurround = SURROUND_THROUGHT;

            switch (eSurround)
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
                default:
                    ASSERT(!this, "Unsupported surround type for export");
                    break;
            }
            if (pObj && (pObj->GetLayer() == rWrt.pDoc->GetHellId() ||
                    pObj->GetLayer() == rWrt.pDoc->GetInvisibleHellId()))
            {
                nFlags |= 0x4000;
            }

            /*
             #i3958# Required to make this inline stuff work in WordXP, not
             needed for 2003 interestingly
             */
            if (rFrmFmt.IsInline())
                nFlags |= 0x8000;

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

WW8_CP MainTxtPlcDrawObj::GetCpOffset(const WW8Fib &) const
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

DrawObj& DrawObj::operator=(const DrawObj& rOther)
{
    mnCp = rOther.mnCp;
    mnShapeId = rOther.mnShapeId;
    maCntnt = rOther.maCntnt;
    maParentPos = rOther.maParentPos;
    mnThick = rOther.mnThick;
    mnDirection = rOther.mnDirection;
    mnHdFtIndex = rOther.mnHdFtIndex;
    return *this;
}

bool PlcDrawObj::Append(SwWW8Writer& rWrt, WW8_CP nCp, const sw::Frame& rFmt,
    const Point& rNdTopLeft)
{
    bool bRet = false;
    const SwFrmFmt &rFormat = rFmt.GetFrmFmt();
    if (TXT_HDFT == rWrt.nTxtTyp || TXT_MAINTEXT == rWrt.nTxtTyp)
    {
        if (RES_FLYFRMFMT == rFormat.Which())
        {
            // check for textflyframe and if it is the first in a Chain
            if (rFormat.GetCntnt().GetCntntIdx())
                bRet = true;
        }
        else
            bRet = true;
    }

    if (bRet)
    {
        DrawObj aObj(rFmt, nCp, rNdTopLeft, rWrt.TrueFrameDirection(rFormat),
            rWrt.GetHdFtIndex());
        maDrawObjs.push_back(aObj);
    }
    return bRet;
}

void DrawObj::SetShapeDetails(UINT32 nId, INT32 nThick)
{
    mnShapeId = nId;
    mnThick = nThick;
}

bool WW8_WrPlcTxtBoxes::WriteTxt(SwWW8Writer& rWrt)
{
    bool bRet = false;
    rWrt.bInWriteEscher = true;
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
    rWrt.bInWriteEscher = false;
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

void SwWW8Writer::AppendFlyInFlys(const sw::Frame& rFrmFmt,
    const Point& rNdTopLeft)
{
    ASSERT(bWrtWW8, "this has gone horribly wrong");
    ASSERT(!pEscher, "der EscherStream wurde schon geschrieben!");
    if (pEscher)
        return ;
    PlcDrawObj *pDrwO;
    if (TXT_HDFT == nTxtTyp)
        pDrwO = pHFSdrObjs;
    else
        pDrwO = pSdrObjs;

    if (rFrmFmt.IsInline())
    {
        OutField(0, ww::eSHAPE, FieldString(ww::eSHAPE),
            WRITEFIELD_START | WRITEFIELD_CMD_START | WRITEFIELD_CMD_END);
    }

    WW8_CP nCP = Fc2Cp(Strm().Tell());
    bool bSuccess = pDrwO->Append(*this, nCP, rFrmFmt, rNdTopLeft);
    ASSERT(bSuccess, "Couldn't export a graphical element!");

    if (bSuccess)
    {
        static const sal_uInt8 aSpec8[] =
        {
            0x03, 0x6a, 0, 0, 0, 0, // sprmCObjLocation
            0x55, 0x08, 1           // sprmCFSpec
        };
                                                // fSpec-Attribut true
                            // Fuer DrawObjets muss ein Spezial-Zeichen
                            // in den Text und darum ein fSpec-Attribut
        pChpPlc->AppendFkpEntry( Strm().Tell() );
        WriteChar( 0x8 );
        pChpPlc->AppendFkpEntry( Strm().Tell(), sizeof( aSpec8 ), aSpec8 );

        const SwFrmFmt &rFmt = rFrmFmt.GetFrmFmt();

        //Need dummy picture frame
        if (rFrmFmt.IsInline())
            OutGrf(rFrmFmt);
    }

    if (rFrmFmt.IsInline())
        OutField(0, ww::eSHAPE, aEmptyStr, WRITEFIELD_CLOSE);
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
    xub_StrLen nTmpSwPos;                   // fuer HasItem()
    rtl_TextEncoding eNdChrSet;
    USHORT nScript;
    BYTE mnTyp;

    xub_StrLen SearchNext( xub_StrLen nStartPos );
    void SetCharSet(const EECharAttrib& rTxtAttr, bool bStart);

    //No copying
    WW8_SdrAttrIter(const WW8_SdrAttrIter&);
    WW8_SdrAttrIter& operator=(const WW8_SdrAttrIter&);
public:
    WW8_SdrAttrIter(SwWW8Writer& rWr, const EditTextObject& rEditObj,
        BYTE nType);
    void NextPara( USHORT nPar );
    void OutParaAttr(bool bCharAttr);
    void OutEEField(const SfxPoolItem& rHt);

    bool IsTxtAttr(xub_StrLen nSwPos);

    void NextPos() { nAktSwPos = SearchNext( nAktSwPos + 1 ); }

    void OutAttr( xub_StrLen nSwPos );
    virtual const SfxPoolItem* HasTextItem( USHORT nWhich ) const;
    virtual const SfxPoolItem& GetItem( USHORT nWhich ) const;
    bool OutAttrWithRange(xub_StrLen nPos);
    xub_StrLen WhereNext() const                { return nAktSwPos; }
    rtl_TextEncoding GetNextCharSet() const;
    rtl_TextEncoding GetNodeCharSet() const     { return eNdChrSet; }
};


WW8_SdrAttrIter::WW8_SdrAttrIter(SwWW8Writer& rWr,
    const EditTextObject& rEditObj, BYTE nTyp)
    : WW8_AttrIter( rWr ), pEditObj(&rEditObj), pEditPool(0),
    aTxtAtrArr( 0, 4 ), aChrTxtAtrArr( 0, 4 ), aChrSetArr( 0, 4 ),
    mnTyp(nTyp)
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
    eNdChrSet = ItemGet<SvxFontItem>(aSet,EE_CHAR_FONTINFO).GetCharSet();

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
        nPos = rHt.nStart;  // gibt erstes Attr-Zeichen
        if( nPos >= nStartPos && nPos <= nMinPos )
        {
            nMinPos = nPos;
            SetCharSet(rHt, true);
        }

//??        if( pHt->GetEnd() )         // Attr mit Ende
        {
            nPos = rHt.nEnd;        // gibt letztes Attr-Zeichen + 1
            if( nPos >= nStartPos && nPos < nMinPos )
            {
                nMinPos = nPos;
                SetCharSet(rHt, false);
            }
        }
/*      else
        {                                   // Attr ohne Ende
            nPos = rHt.nStart + 1;  // Laenge 1 wegen CH_TXTATR im Text
            if( nPos >= nStartPos && nPos < nMinPos )
            {
                nMinPos = nPos;
                SetCharSet(rHt, false);
            }
        }
*/
    }
    return nMinPos;
}

void WW8_SdrAttrIter::SetCharSet(const EECharAttrib& rAttr, bool bStart)
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

void WW8_SdrAttrIter::OutEEField(const SfxPoolItem& rHt)
{
    const SvxFieldItem &rField = (const SvxFieldItem &)rHt;
    const SvxFieldData *pFld = rField.GetField();
    if (pFld && pFld->ISA(SvxURLField))
    {
        BYTE nOldTxtTyp = rWrt.nTxtTyp;
        rWrt.nTxtTyp = mnTyp;
        const SvxURLField *pURL = (const SvxURLField *)pFld;
        StartURL(pURL->GetURL(), pURL->GetTargetFrame());

        const String &rStr = pURL->GetRepresentation();
        rWrt.OutSwString(rStr, 0, rStr.Len(), true, GetNodeCharSet());

        EndURL();
        rWrt.nTxtTyp = nOldTxtTyp;
    }
}

void WW8_SdrAttrIter::OutAttr( xub_StrLen nSwPos )
{
    OutParaAttr(true);

    if( aTxtAtrArr.Count() )
    {
        const SwModify* pOldMod = rWrt.pOutFmtNode;
        rWrt.pOutFmtNode = 0;

        const SfxItemPool* pSrcPool = pEditPool;
        const SfxItemPool& rDstPool = rWrt.pDoc->GetAttrPool();

        nTmpSwPos = nSwPos;
        register USHORT i, nWhich, nSlotId;
        FnAttrOut pOut;
        for( i = 0; i < aTxtAtrArr.Count(); i++ )
        {
            const EECharAttrib& rHt = aTxtAtrArr[ i ];
            if (nSwPos >= rHt.nStart && nSwPos < rHt.nEnd)
            {
                nWhich = rHt.pAttr->Which();
                if (nWhich == EE_FEATURE_FIELD)
                {
                    OutEEField(*rHt.pAttr);
                    continue;
                }
                else if (nWhich == EE_FEATURE_TAB)
                {
                    rWrt.WriteChar(0x9);
                    continue;
                }
                nSlotId = pSrcPool->GetSlotId(nWhich);

                if (nSlotId && nWhich != nSlotId)
                {
                    nWhich = rDstPool.GetWhich(nSlotId);
                    if (nWhich && nWhich != nSlotId &&
                        nWhich < RES_UNKNOWNATR_BEGIN &&
                        (pOut = aWW8AttrFnTab[nWhich - RES_CHRATR_BEGIN]))
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
                }
            }

            if( nSwPos < rHt.nStart )
                break;
        }

        nTmpSwPos = 0;      // HasTextItem nur in dem obigen Bereich erlaubt
        rWrt.pOutFmtNode = pOldMod;
    }
}

bool WW8_SdrAttrIter::IsTxtAttr(xub_StrLen nSwPos)
{
    for (USHORT i = 0; i < aTxtAtrArr.Count(); ++i)
    {
        const EECharAttrib& rHt = aTxtAtrArr[ i ];
        if (nSwPos >= rHt.nStart && nSwPos < rHt.nEnd)
        {
            if (
                 (rHt.pAttr->Which() == EE_FEATURE_FIELD) ||
                 (rHt.pAttr->Which() == EE_FEATURE_TAB)
               )
            {
                return true;
            }
        }
    }
    return false;
}

// HasItem ist fuer die Zusammenfassung des Doppel-Attributes Underline
// und WordLineMode als TextItems. OutAttr() ruft die Ausgabefunktion,
// die dann ueber HasItem() nach anderen Items an der
// Attribut-Anfangposition fragen kann.
// Es koennen nur Attribute mit Ende abgefragt werden.
// Es wird mit bDeep gesucht
const SfxPoolItem* WW8_SdrAttrIter::HasTextItem(USHORT nWhich) const
{
    const SfxPoolItem* pRet = 0;
    nWhich = sw::hack::TransformWhichBetweenPools(*pEditPool,
        rWrt.pDoc->GetAttrPool(), nWhich);
    if (nWhich)
    {
        for (USHORT i = 0; i < aTxtAtrArr.Count(); ++i)
        {
            const EECharAttrib& rHt = aTxtAtrArr[i];
            if (
                 nWhich == rHt.pAttr->Which() && nTmpSwPos >= rHt.nStart &&
                 nTmpSwPos < rHt.nEnd
               )
            {
                pRet = rHt.pAttr;   // Found
                break;
            }
            else if (nTmpSwPos < rHt.nStart)
                break;              // dann kommt da nichts mehr
        }
    }
    return pRet;
}

const SfxPoolItem& WW8_SdrAttrIter::GetItem( USHORT nWhich ) const
{
    using sw::hack::GetSetWhichFromSwDocWhich;
    const SfxPoolItem* pRet = HasTextItem(nWhich);
    if (!pRet)
    {
        SfxItemSet aSet(pEditObj->GetParaAttribs(nPara));
        nWhich = GetSetWhichFromSwDocWhich(aSet, *rWrt.pDoc, nWhich);
        ASSERT(nWhich, "Impossible, catastrophic failure imminent");
        pRet = &aSet.Get(nWhich);
    }
    return *pRet;
}

void WW8_SdrAttrIter::OutParaAttr(bool bCharAttr)
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

void SwWW8Writer::WriteSdrTextObj(const SdrObject& rObj, BYTE nTyp)
{
    const SdrTextObj* pTxtObj = PTR_CAST(SdrTextObj, &rObj);
    ASSERT(pTxtObj, "That is no SdrTextObj!");
    if (!pTxtObj)
        return;

    bool bAnyWrite = false;
    const OutlinerParaObject* pParaObj =
        sw::hack::GetOutlinerParaObject(*pTxtObj);
    if (pParaObj)
    {
        const EditTextObject& rEditObj = pParaObj->GetTextObject();
        WW8_SdrAttrIter aAttrIter( *this, rEditObj, nTyp );

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
            do {
                xub_StrLen nNextAttr = aAttrIter.WhereNext();
                rtl_TextEncoding eNextChrSet = aAttrIter.GetNextCharSet();

                if( nNextAttr > nEnd )
                    nNextAttr = nEnd;

                bool bTxtAtr = aAttrIter.IsTxtAttr( nAktPos );
                if( !bTxtAtr )
                    OutSwString( aStr, nAktPos, nNextAttr - nAktPos,
                                    true, eChrSet );

                            // Am Zeilenende werden die Attribute bis ueber das CR
                            // aufgezogen. Ausnahme: Fussnoten am Zeilenende
                if( nNextAttr == nEnd && !bTxtAtr )
                    WriteCR();              // CR danach

                                                // Ausgabe der Zeichenattribute
                aAttrIter.OutAttr( nAktPos );   // nAktPos - 1 ??
                pChpPlc->AppendFkpEntry( Strm().Tell(),
                                                pO->Count(), pO->GetData() );
                pO->Remove( 0, pO->Count() );                   // leeren

                            // Ausnahme: Fussnoten am Zeilenende
                if( nNextAttr == nEnd && bTxtAtr )
                    WriteCR();              // CR danach

                nAktPos = nNextAttr;
                eChrSet = eNextChrSet;
                aAttrIter.NextPos();
            }
            while( nAktPos < nEnd );

            ASSERT( !pO->Count(), " pO ist am ZeilenEnde nicht leer" );

            pO->Insert( bNul, pO->Count() );        // Style # as short
            pO->Insert( bNul, pO->Count() );

            aAttrIter.OutParaAttr(false);

            ULONG nPos = Strm().Tell();
            pPapPlc->AppendFkpEntry( Strm().Tell(),
                                            pO->Count(), pO->GetData() );
            pO->Remove( 0, pO->Count() );                       // leeren
            pChpPlc->AppendFkpEntry( nPos );
        }
        bAnyWrite = 0 != nPara;
    }
    if( !bAnyWrite )
        WriteStringAsPara( aEmptyStr );
}

void WinwordAnchoring::WriteData( EscherEx& rEx ) const
{
    //Toplevel groups get their winword extra data attached, and sub elements
    //use the defaults
    if (rEx.GetGroupLevel() <= 1)
    {
        SvStream& rSt = rEx.GetStream();
        //The last argument denotes the number of sub properties in this atom
        if (mbInline)
        {
            rEx.AddAtom(18, DFF_msofbtUDefProp, 3, 3); //Prop id is 0xF122
            rSt << (UINT16)0x0390 << sal_uInt32(3);
            rSt << (UINT16)0x0392 << sal_uInt32(3);
            //This sub property is required to be in the dummy inline frame as
            //well
            rSt << (UINT16)0x053F << nInlineHack;
        }
        else
        {
            rEx.AddAtom(24, DFF_msofbtUDefProp, 3, 4 ); //Prop id is 0xF122
            rSt << (UINT16)0x038F << mnXAlign;
            rSt << (UINT16)0x0390 << mnXRelTo;
            rSt << (UINT16)0x0391 << mnYAlign;
            rSt << (UINT16)0x0392 << mnYRelTo;
        }
    }
}

/*  */

void SwWW8Writer::CreateEscher()
{
    SfxItemState eBackSet =
        (const_cast<const SwDoc*>(pDoc))->GetPageDesc(0).GetMaster().
        GetItemState(RES_BACKGROUND);
    if (pHFSdrObjs->size() || pSdrObjs->size() || SFX_ITEM_SET == eBackSet)
    {
        ASSERT( !pEscher, "wer hat den Pointer nicht geloescht?" );
        SvMemoryStream* pEscherStrm = new SvMemoryStream;
        pEscherStrm->SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);
        pEscher = new SwEscherEx(pEscherStrm, *this);
    }
}

void SwWW8Writer::WriteEscher()
{
    if (pEscher)
    {
        ULONG nStart = pTableStrm->Tell();

        pEscher->WritePictures();
        pEscher->FinishEscher();

        pFib->fcDggInfo = nStart;
        pFib->lcbDggInfo = pTableStrm->Tell() - nStart;
        delete pEscher, pEscher = 0;
    }
}

void SwEscherEx::WritePictures()
{
    if (pPictStrm)
    {
        // set the blip - entries to the correct stream pos
        INT32 nEndPos = rWrt.Strm().Tell();
        SetNewBlipStreamOffset( nEndPos );

        pPictStrm->Seek( 0 );
        rWrt.Strm() << *pPictStrm;

        delete pPictStrm, pPictStrm = 0;
    }
    Flush();
}

/*  */

// Output- Routines for Escher Export

SwBasicEscherEx::SwBasicEscherEx(SvStream* pStrm, SwWW8Writer& rWW8Wrt,
    UINT32 nDrawings)
    : EscherEx(*pStrm, nDrawings), rWrt(rWW8Wrt), pEscherStrm(pStrm),
    pPictStrm(0)
{
    Init();
}

SwBasicEscherEx::~SwBasicEscherEx()
{
}

void SwBasicEscherEx::WriteFrmExtraData(const SwFrmFmt&)
{
    AddAtom(4, ESCHER_ClientAnchor);
    GetStream() << (sal_uInt32)0x80000000;
}

void SwBasicEscherEx::WriteEmptyFlyFrame(const SwFrmFmt& rFmt, UINT32 nShapeId)
{
    OpenContainer(ESCHER_SpContainer);
    AddShape(ESCHER_ShpInst_PictureFrame, 0xa00, nShapeId);
    // store anchor attribute
    WriteFrmExtraData(rFmt);

    AddAtom(6, DFF_msofbtUDefProp, 3, 1); //Prop id is 0xF122
    GetStream() << (UINT16)0x053F << nInlineHack;

    CloseContainer();   // ESCHER_SpContainer
}

UINT32 AddMirrorFlags(UINT32 nFlags, const SwMirrorGrf &rMirror)
{
    switch (rMirror.GetValue())
    {
        default:
        case RES_DONT_MIRROR_GRF:
            break;
        case RES_MIRROR_GRF_VERT:
            nFlags |= SHAPEFLAG_FLIPH;
            break;
        case RES_MIRROR_GRF_HOR:
            nFlags |= SHAPEFLAG_FLIPV;
            break;
        case RES_MIRROR_GRF_BOTH:
            nFlags |= SHAPEFLAG_FLIPH;
            nFlags |= SHAPEFLAG_FLIPV;
            break;

    }
    return nFlags;
}

INT32 SwBasicEscherEx::WriteGrfFlyFrame(const SwFrmFmt& rFmt, UINT32 nShapeId)
{
    INT32 nBorderThick=0;
    SwNoTxtNode *pNd = GetNoTxtNodeFromSwFrmFmt(rFmt);
    SwGrfNode *pGrfNd = pNd ? pNd->GetGrfNode() : 0;
    ASSERT(pGrfNd, "No SwGrfNode ?, suspicious");
    if (!pGrfNd)
        return nBorderThick;

    OpenContainer( ESCHER_SpContainer );

    const SwMirrorGrf &rMirror = pGrfNd->GetSwAttrSet().GetMirrorGrf();
    AddShape(ESCHER_ShpInst_PictureFrame, AddMirrorFlags(0xa00, rMirror),
        nShapeId);

    EscherPropertyContainer aPropOpt;

    UINT32 nFlags = ESCHER_BlipFlagDefault;

    if (pGrfNd->IsLinkedFile())
    {
        String sURL;
        pGrfNd->GetFileFilterNms( &sURL, 0 );

        WW8Bytes aBuf;
        SwWW8Writer::InsAsString16( aBuf, sURL );
        SwWW8Writer::InsUInt16( aBuf, 0 );

        USHORT nArrLen = aBuf.Count();
        BYTE* pArr = new BYTE[ nArrLen ];
        memcpy( pArr, aBuf.GetData(), nArrLen );

        aPropOpt.AddOpt(ESCHER_Prop_pibName, true, nArrLen, pArr, nArrLen);
        nFlags = ESCHER_BlipFlagLinkToFile | ESCHER_BlipFlagURL |
                    ESCHER_BlipFlagDoNotSave;
    }
    else
    {
        pGrfNd->SwapIn(true);

        Graphic         aGraphic(pGrfNd->GetGrf());
        GraphicObject   aGraphicObject( aGraphic );
        ByteString      aUniqueId = aGraphicObject.GetUniqueID();

        if ( aUniqueId.Len() )
        {
            const   MapMode aMap100mm( MAP_100TH_MM );
            Size    aSize( aGraphic.GetPrefSize() );

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
            if (nBlibId)
                aPropOpt.AddOpt(ESCHER_Prop_pib, nBlibId, sal_True);
        }
    }

    aPropOpt.AddOpt( ESCHER_Prop_pibFlags, nFlags );
    nBorderThick = WriteFlyFrameAttr(rFmt,mso_sptPictureFrame,aPropOpt);
    WriteGrfAttr(*pGrfNd, aPropOpt);

    aPropOpt.Commit( GetStream() );

    // store anchor attribute
    WriteFrmExtraData( rFmt );

    CloseContainer();   // ESCHER_SpContainer
    return nBorderThick;
}

void SwBasicEscherEx::WriteGrfAttr(const SwNoTxtNode& rNd,
    EscherPropertyContainer& rPropOpt)
{
    const SfxPoolItem* pItem;
    sal_uInt32 nMode = GRAPHICDRAWMODE_STANDARD;
    sal_Int32 nContrast = 0;
    sal_Int16 nBrightness = 0;

    if (SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState(RES_GRFATR_CONTRAST,
        true, &pItem))
    {
        nContrast = ((SfxInt16Item*)pItem)->GetValue();
    }

    if (SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState(RES_GRFATR_LUMINANCE,
        true, &pItem))
    {
        nBrightness = ((SfxInt16Item*)pItem)->GetValue();
    }


    if (SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState(RES_GRFATR_DRAWMODE,
        true, &pItem))
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

    if (SFX_ITEM_SET == rNd.GetSwAttrSet().GetItemState(RES_GRFATR_CROPGRF,
        true, &pItem))
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
}

void SwBasicEscherEx::SetPicId(const SdrObject &, UINT32,
    EscherPropertyContainer &)
{
}

void SwEscherEx::SetPicId(const SdrObject &rSdrObj, UINT32 nShapeId,
    EscherPropertyContainer &rPropOpt)
{
    pTxtBxs->Append(rSdrObj, nShapeId);
    UINT32 nPicId = pTxtBxs->Count();
    nPicId *= 0x10000;
    rPropOpt.AddOpt( ESCHER_Prop_pictureId, nPicId );
}

INT32 SwBasicEscherEx::WriteOLEFlyFrame(const SwFrmFmt& rFmt, UINT32 nShapeId)
{
    INT32 nBorderThick = 0;
    if (const SdrObject* pSdrObj = rFmt.FindRealSdrObject())
    {
        SwNodeIndex aIdx(*rFmt.GetCntnt().GetCntntIdx(), 1);
        SwOLENode& rOLENd = *aIdx.GetNode().GetOLENode();
        //com::sun::star::uno::Reference < com::sun::star::embed::XEmbeddedObject > xObj(rOLENd.GetOLEObj().GetOleRef());

        /*
        #i5970#
        Export floating ole2 .doc ver 8+ wmf ole2 previews as emf previews
        instead ==> allows unicode text to be preserved
        */
#ifdef OLE_PREVIEW_AS_EMF
        //Graphic aGraphic = wwUtility::MakeSafeGDIMetaFile(xObj);
        Graphic* pGraphic = rOLENd.GetGraphic();
#endif
        OpenContainer(ESCHER_SpContainer);

        EscherPropertyContainer aPropOpt;
        const SwMirrorGrf &rMirror = rOLENd.GetSwAttrSet().GetMirrorGrf();
        WriteOLEPicture(aPropOpt, AddMirrorFlags(0xa00 | SHAPEFLAG_OLESHAPE,
            rMirror), pGraphic ? *pGraphic : Graphic(), *pSdrObj, nShapeId);

        nBorderThick = WriteFlyFrameAttr(rFmt, mso_sptPictureFrame, aPropOpt);
        WriteGrfAttr(rOLENd, aPropOpt);
        aPropOpt.Commit(GetStream());

        // store anchor attribute
        WriteFrmExtraData( rFmt );

        CloseContainer();   // ESCHER_SpContainer
    }
    return nBorderThick;
}

void SwBasicEscherEx::WriteBrushAttr(const SvxBrushItem &rBrush,
    EscherPropertyContainer& rPropOpt)
{
    bool bSetOpacity = false;
    sal_uInt32 nOpaque = 0;
    if (const GraphicObject *pGraphicObject = rBrush.GetGraphicObject())
    {
        ByteString aUniqueId = pGraphicObject->GetUniqueID();
        if (aUniqueId.Len())
        {
            const Graphic &rGraphic = pGraphicObject->GetGraphic();
            Size aSize(rGraphic.GetPrefSize());
            const MapMode aMap100mm(MAP_100TH_MM);
            if (MAP_PIXEL == rGraphic.GetPrefMapMode().GetMapUnit())
            {
                aSize = Application::GetDefaultDevice()->PixelToLogic(
                    aSize, aMap100mm);
            }
            else
            {
                aSize = OutputDevice::LogicToLogic(aSize,
                    rGraphic.GetPrefMapMode(), aMap100mm);
            }

            Point aEmptyPoint = Point();
            Rectangle aRect(aEmptyPoint, aSize);

            sal_uInt32 nBlibId = GetBlibID(*QueryPicStream(), aUniqueId,
                aRect, 0);
            if (nBlibId)
                rPropOpt.AddOpt(ESCHER_Prop_fillBlip,nBlibId,sal_True);
        }

        if ((nOpaque = pGraphicObject->GetAttr().GetTransparency()))
            bSetOpacity = true;

        rPropOpt.AddOpt( ESCHER_Prop_fillType, ESCHER_FillPicture );
        rPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x140014 );
        rPropOpt.AddOpt( ESCHER_Prop_fillBackColor, 0 );
    }
    else
    {
        UINT32 nFillColor = GetColor(rBrush.GetColor(), false);
        rPropOpt.AddOpt( ESCHER_Prop_fillColor, nFillColor );
        rPropOpt.AddOpt( ESCHER_Prop_fillBackColor, nFillColor ^ 0xffffff );
        rPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x100010 );

        if ((nOpaque = rBrush.GetColor().GetTransparency()))
            bSetOpacity = true;
    }

    if (bSetOpacity)
    {
        nOpaque = (nOpaque * 100) / 0xFE;
        nOpaque = ((100 - nOpaque) << 16) / 100;
        rPropOpt.AddOpt(ESCHER_Prop_fillOpacity, nOpaque);
    }
}

INT32 SwBasicEscherEx::WriteFlyFrameAttr(const SwFrmFmt& rFmt,
    MSO_SPT eShapeType, EscherPropertyContainer& rPropOpt)
{
    INT32 nLineWidth=0;
    const SfxPoolItem* pItem;
    bool bFirstLine = true;
    if (SFX_ITEM_SET == rFmt.GetItemState(RES_BOX, true, &pItem))
    {
        static const UINT16 aExhperProp[4] =
        {
            ESCHER_Prop_dyTextTop,  ESCHER_Prop_dyTextBottom,
            ESCHER_Prop_dxTextLeft, ESCHER_Prop_dxTextRight
        };
        const SvxBorderLine* pLine;

        for( int n = 0; n < 4; ++n )
            if( 0 != ( pLine = ((SvxBoxItem*)pItem)->GetLine( n )) )
            {
                if( bFirstLine )
                {
                    UINT32 nLineColor = GetColor(pLine->GetColor(), false);
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
                    bFirstLine = false;
                }
                rPropOpt.AddOpt( aExhperProp[ n ], DrawModelToEmu(
                    ((SvxBoxItem*)pItem)->GetDistance( n ) ));
            }
            else
                // MM If there is no line the distance should be set to 0
                rPropOpt.AddOpt( aExhperProp[ n ], DrawModelToEmu(0));
    }
    if( bFirstLine )                // no valid line found
    {
        rPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x80000 );
        rPropOpt.AddOpt( ESCHER_Prop_dyTextTop, 0 );
        rPropOpt.AddOpt( ESCHER_Prop_dyTextBottom, 0 );
        rPropOpt.AddOpt( ESCHER_Prop_dxTextLeft, 0 );
        rPropOpt.AddOpt( ESCHER_Prop_dxTextRight, 0 );
    }

    SvxBrushItem aBrush(rWrt.TrueFrameBgBrush(rFmt));
    WriteBrushAttr(aBrush, rPropOpt);

    const SdrObject* pObj = rFmt.FindRealSdrObject();
    if( pObj && (pObj->GetLayer() == GetHellLayerId() ||
        pObj->GetLayer() == GetInvisibleHellId() ))
    {
        rPropOpt.AddOpt( ESCHER_Prop_fPrint, 0x200020 );
    }

    return nLineWidth;
}

INT32 SwEscherEx::WriteFlyFrameAttr(const SwFrmFmt& rFmt, MSO_SPT eShapeType,
    EscherPropertyContainer& rPropOpt)
{
    INT32 nLineWidth = SwBasicEscherEx::WriteFlyFrameAttr(rFmt, eShapeType,
        rPropOpt);

    /*
     These are not in SwBasicEscherEx::WriteFlyFrameAttr because inline objs
     can't do it in word and it hacks it in by stretching the graphic that
     way, perhaps we should actually draw in this space into the graphic we
     are exporting!
     */
    const SfxPoolItem* pItem;
    if (SFX_ITEM_SET == rFmt.GetItemState(RES_LR_SPACE, true, &pItem))
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

    if (SFX_ITEM_SET == rFmt.GetItemState(RES_UL_SPACE, true, &pItem))
    {
        rPropOpt.AddOpt( ESCHER_Prop_dyWrapDistTop,
                DrawModelToEmu( ((SvxULSpaceItem*)pItem)->GetUpper() ) );
        rPropOpt.AddOpt( ESCHER_Prop_dyWrapDistBottom,
                DrawModelToEmu( ((SvxULSpaceItem*)pItem)->GetLower() ) );
    }

    if (rFmt.GetSurround().IsContour())
    {
        if (const SwNoTxtNode *pNd = GetNoTxtNodeFromSwFrmFmt(rFmt))
        {
            const PolyPolygon *pPolyPoly = pNd->HasContour();
            if (pPolyPoly && pPolyPoly->Count())
            {
                Polygon aPoly(PolygonFromPolyPolygon(*pPolyPoly));
                const Size &rOrigSize = pNd->GetGraphic().GetPrefSize();
                Fraction aMapPolyX(ww::nWrap100Percent, rOrigSize.Width());
                Fraction aMapPolyY(ww::nWrap100Percent, rOrigSize.Height());
                aPoly.Scale(aMapPolyX, aMapPolyY);

                /*
                 a) stretch right bound by 15twips
                 b) shrink bottom bound to where it would have been in word
                 c) Move it to the left by 15twips

                 See the import for details
                */
                const Size &rSize = pNd->GetTwipSize();
                Fraction aMoveHack(ww::nWrap100Percent, rSize.Width());
                aMoveHack *= Fraction(15, 1);
                long nMove(aMoveHack);

                Fraction aHackX(ww::nWrap100Percent + nMove,
                        ww::nWrap100Percent);
                Fraction aHackY(ww::nWrap100Percent - nMove,
                        ww::nWrap100Percent);
                aPoly.Scale(aHackX, aHackY);

                aPoly.Move(-nMove, 0);

                SvMemoryStream aPolyDump;
                aPolyDump.SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);

                sal_uInt16 nLen = aPoly.GetSize();
                aPolyDump << nLen;
                aPolyDump << nLen;
                aPolyDump << sal_uInt16(8);
                for (sal_uInt16 nI = 0; nI < nLen; ++nI)
                {
                    aPolyDump << sal_uInt32(aPoly[nI].X());
                    aPolyDump << sal_uInt32(aPoly[nI].Y());
                }

                sal_uInt16 nArrLen = msword_cast<sal_uInt16>(aPolyDump.Tell());
                void *pArr = const_cast<void *>(aPolyDump.GetData());
                //PropOpt wants to own the buffer
                aPolyDump.ObjectOwnsMemory(false);
                rPropOpt.AddOpt(DFF_Prop_pWrapPolygonVertices, false,
                    nArrLen, static_cast<BYTE *>(pArr), nArrLen);
            }
        }
    }

    return nLineWidth;
}

void SwBasicEscherEx::Init()
{
    MapUnit eMap = MAP_TWIP;
    if (SdrModel *pModel = rWrt.pDoc->GetDrawModel())
    {
        // PPT arbeitet nur mit Einheiten zu 576DPI
        // WW hingegen verwendet twips, dh. 1440DPI.
        eMap = pModel->GetScaleUnit();
    }

    // MS-DFF-Properties sind grossteils in EMU (English Metric Units) angegeben
    // 1mm=36000emu, 1twip=635emu
    Fraction aFact(360, 1);
    aFact /= GetMapFactor(MAP_100TH_MM, eMap).X();
    // create little values
    aFact = Fraction(aFact.GetNumerator(), aFact.GetDenominator());
    mnEmuMul = aFact.GetNumerator();
    mnEmuDiv = aFact.GetDenominator();

    SetHellLayerId(rWrt.pDoc->GetHellId());
}

INT32 SwBasicEscherEx::ToFract16(INT32 nVal, UINT32 nMax) const
{
    if (nMax)
    {
        INT32 nMSVal = (nVal / 65536) * nMax;
        nMSVal += (nVal * 65536 ) / nMax;
        return nMSVal;
    }
    return 0;
}

SvStream* SwBasicEscherEx::QueryPicStream()
{
    if (!pPictStrm)
    {
        pPictStrm = new SvMemoryStream;
        pPictStrm->SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);
    }
    return pPictStrm;
}

void SwBasicEscherEx::WritePictures()
{
    ASSERT(pPictStrm, "no picture!");
    if (pPictStrm)
    {
        // set the blip - entries to the correct stream pos
        INT32 nEndPos = pPictStrm->Tell();
        WriteBlibStoreEntry(*pEscherStrm, 1, sal_True, nEndPos);

        pPictStrm->Seek(0);
        *pEscherStrm << *pPictStrm;

        delete pPictStrm, pPictStrm = 0;
    }
}

SwEscherEx::SwEscherEx(SvStream* pStrm, SwWW8Writer& rWW8Wrt)
    : SwBasicEscherEx(pStrm, rWW8Wrt, rWW8Wrt.pHFSdrObjs->size() ? 2 : 1),
    pTxtBxs(0)
{
    aHostData.SetClientData(&aWinwordAnchoring);
    OpenContainer( ESCHER_DggContainer );

    sal_uInt16 nColorCount = 4;
    *pStrm  << (sal_uInt16)( nColorCount << 4 )     // instance
            << (sal_uInt16)ESCHER_SplitMenuColors   // record type
            << (sal_uInt32)( nColorCount * 4 )      // size
            << (sal_uInt32)0x08000004
            << (sal_uInt32)0x08000001
            << (sal_uInt32)0x08000002
            << (sal_uInt32)0x100000f7;

    CloseContainer();   // ESCHER_DggContainer

    BYTE i = 2;     // for header/footer and the other
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
        DrawObjPointerVector aSorted;
        MakeZOrderArrAndFollowIds(pSdrObjs->GetObjArr(), aSorted);

        ULONG nShapeId=0;
        DrawObjPointerIter aEnd = aSorted.end();
        for (DrawObjPointerIter aIter = aSorted.begin(); aIter != aEnd; ++aIter)
        {
            INT32 nBorderThick=0;
            DrawObj *pObj = (*aIter);
            ASSERT(pObj, "impossible");
            if (!pObj)
                continue;
            const sw::Frame &rFrame = pObj->maCntnt;
            const SwFrmFmt& rFmt = rFrame.GetFrmFmt();

            switch (rFrame.GetWriterType())
            {
                case sw::Frame::eTxtBox:
                case sw::Frame::eOle:
                case sw::Frame::eGraphic:
                    nBorderThick = WriteFlyFrm(*pObj, nShapeId, aSorted);
                    break;
                case sw::Frame::eFormControl:
                    WriteOCXControl(rFmt, nShapeId=GetShapeID());
                    break;
                case sw::Frame::eDrawing:
                    aWinwordAnchoring.SetAnchoring(rFmt);
                    if (const SdrObject* pObj = rFmt.FindRealSdrObject())
                    {
                        bool bSwapInPage = false;
                        if (!pObj->GetPage())
                        {
                            if (SdrModel* pModel = rWrt.pDoc->GetDrawModel())
                            {
                                if (SdrPage *pPage = pModel->GetPage(0))
                                {
                                    bSwapInPage = true;
                                    (const_cast<SdrObject*>(pObj))->SetPage(pPage);
                                }
                            }
                        }

                        nShapeId = AddSdrObject(*pObj);

                        if (bSwapInPage)
                            (const_cast<SdrObject*>(pObj))->SetPage(0);
                    }
#ifndef PRODUCT
                    else
                        ASSERT( !this, "Where is the SDR-Object?" );
#endif
                }

            if( !nShapeId )
            {
                /*!!!*/ const SdrObject* pObj = 0;
                nShapeId = AddDummyShape( *pObj );
            }

            pObj->SetShapeDetails(nShapeId, nBorderThick);
        }

        EndSdrObjectPage();         // ???? Bugfix for 74724

        if( nSecondShapeId )
        {
            OpenContainer( ESCHER_SpContainer );

            AddShape( ESCHER_ShpInst_Rectangle, 0xe00, nSecondShapeId );

            EscherPropertyContainer aPropOpt;
            const SwFrmFmt &rFmt = const_cast<const SwDoc *>(rWrt.pDoc)->GetPageDesc(0).GetMaster();
            const SfxPoolItem* pItem = 0;
            SfxItemState eState = rFmt.GetItemState(RES_BACKGROUND, true,
                &pItem);
            if (SFX_ITEM_SET == eState && pItem)
            {
                const SvxBrushItem* pBrush = (const SvxBrushItem*)pItem;
                WriteBrushAttr(*pBrush, aPropOpt);
            }
            aPropOpt.AddOpt( ESCHER_Prop_lineColor, 0x8000001 );
            aPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x00080008 );
            aPropOpt.AddOpt( ESCHER_Prop_shadowColor, 0x8000002 );
            aPropOpt.AddOpt( ESCHER_Prop_lineWidth, 0 );

// winword defaults!
//          aPropOpt.AddOpt( ESCHER_Prop_fNoFillHitTest, 0x100000 );
//          aPropOpt.AddOpt( ESCHER_Prop_lineWidth, 0 );
//          aPropOpt.AddOpt( ESCHER_Prop_fNoLineDrawDash, 0x80000 );
//          aPropOpt.AddOpt( ESCHER_Prop_bWMode, 0x9 );
//          aPropOpt.AddOpt( ESCHER_Prop_fBackground, 0x10001 );

            aPropOpt.Commit( *pStrm );

            AddAtom( 4, ESCHER_ClientData );
            GetStream() << 1L;

            CloseContainer();   // ESCHER_SpContainer
        }
    CloseContainer();   // ESCHER_DgContainer
    }
}

SwEscherEx::~SwEscherEx()
{
}

void SwEscherEx::FinishEscher()
{
    pEscherStrm->Seek(0);
    *rWrt.pTableStrm << *pEscherStrm;
    delete pEscherStrm, pEscherStrm = 0;
}

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
bool WinwordAnchoring::ConvertPosition( SwFmtHoriOrient& _iorHoriOri,
                                         SwFmtVertOrient& _iorVertOri,
                                         const SwFrmFmt& _rFrmFmt )
{
    const RndStdIds eAnchor = _rFrmFmt.GetAnchor().GetAnchorId();

    if ( FLY_IN_CNTNT == eAnchor || FLY_AT_FLY == eAnchor )
    {
        // no conversion for as-character or at frame anchored objects
        return false;
    }

    // determine anchored object
    SwAnchoredObject* pAnchoredObj( 0L );
    {
        const SwContact* pContact = _rFrmFmt.FindContactObj();
        if ( pContact )
        {
            std::vector<SwAnchoredObject*> aAnchoredObjs;
            pContact->GetAnchoredObjs( aAnchoredObjs );
            if ( !aAnchoredObjs.empty() )
            {
                pAnchoredObj = aAnchoredObjs.front();
            }
        }
    }
    if ( !pAnchoredObj )
    {
        // no anchored object found. Thus, the needed layout information can't
        // be determined. --> no conversion
        return false;
    }

    bool bConverted( false );

    // determine value of attribute 'Follow text flow', because positions aligned
    // at page areas have to be converted, if it's set.
    const bool bFollowTextFlow = _rFrmFmt.GetFollowTextFlow().GetValue();

    // convert horizontal position, if needed
    {
        enum HoriConv { NO_CONV, CONV2PG, CONV2COL, CONV2CHAR };
        HoriConv eHoriConv( NO_CONV );

        // determine, if conversion has to be performed due to the position orientation
        bool bConvDueToOrientation( false );
        {
            const SwHoriOrient eHOri = _iorHoriOri.GetHoriOrient();
            bConvDueToOrientation = eHOri == HORI_LEFT || eHOri == HORI_RIGHT ||
                                    eHOri == HORI_INSIDE || eHOri == HORI_OUTSIDE ||
                                    ( eHOri != HORI_CENTER && _iorHoriOri.IsPosToggle() );
        }

        // determine conversion type due to the position relation
        switch ( _iorHoriOri.GetRelationOrient() )
        {
            case REL_PG_FRAME:
            case REL_PG_PRTAREA:
            {
                if ( bConvDueToOrientation || bFollowTextFlow )
                    eHoriConv = CONV2PG;
            }
            break;
            case REL_PG_LEFT:
            case REL_PG_RIGHT:
            {
                // relation not supported by WW8. Thus, conversion always needed.
                eHoriConv = CONV2PG;
            }
            break;
            case FRAME:
            {
                if ( bConvDueToOrientation )
                    eHoriConv = CONV2COL;
            }
            break;
            case PRTAREA:
            case REL_FRM_LEFT:
            case REL_FRM_RIGHT:
            {
                // relation not supported by WW8. Thus, conversion always needed.
                eHoriConv = CONV2COL;
            }
            break;
            case REL_CHAR:
            {
                if ( bConvDueToOrientation )
                    eHoriConv = CONV2CHAR;
            }
            break;
            default:
                ASSERT( false,
                        "<WinwordAnchoring::ConvertPosition(..)> - unknown horizontal relation" );
        }
        if ( eHoriConv != NO_CONV )
        {
            _iorHoriOri.SetHoriOrient( HORI_NONE );
            SwTwips nPosX( 0L );
            {
                Point aPos;
                if ( eHoriConv == CONV2PG )
                {
                    _iorHoriOri.SetRelationOrient( REL_PG_FRAME );
                    // --> OD 2005-01-27 #i33818#
                    bool bRelToTableCell( false );
                    aPos = pAnchoredObj->GetRelPosToPageFrm( bFollowTextFlow,
                                                             bRelToTableCell );
                    if ( bRelToTableCell )
                    {
                        _iorHoriOri.SetRelationOrient( REL_PG_PRTAREA );
                    }
                    // <--
                }
                else if ( eHoriConv == CONV2COL )
                {
                    _iorHoriOri.SetRelationOrient( FRAME );
                    aPos = pAnchoredObj->GetRelPosToAnchorFrm();
                }
                else if ( eHoriConv == CONV2CHAR )
                {
                    _iorHoriOri.SetRelationOrient( REL_CHAR );
                    aPos = pAnchoredObj->GetRelPosToChar();
                }
                // No distinction between layout directions, because of missing
                // information about WW8 in vertical layout.
                nPosX = aPos.X();
            }
            _iorHoriOri.SetPos( nPosX );
            bConverted = true;
        }
    }

    // convert vertical position, if needed
    {
        enum VertConv { NO_CONV, CONV2PG, CONV2PARA, CONV2LINE };
        VertConv eVertConv( NO_CONV );

        // determine, if conversion has to be performed due to the position orientation
        bool bConvDueToOrientation( false );
        {
            const SwVertOrient eVOri = _iorVertOri.GetVertOrient();
            bConvDueToOrientation = ( eVOri == VERT_TOP ||
                                      eVOri == VERT_BOTTOM ||
                                      eVOri == VERT_CHAR_TOP ||
                                      eVOri == VERT_CHAR_BOTTOM ||
                                      eVOri == VERT_CHAR_CENTER ||
                                      eVOri == VERT_LINE_TOP ||
                                      eVOri == VERT_LINE_BOTTOM ||
                                      eVOri == VERT_LINE_CENTER );
        }

        // determine conversion type due to the position relation
        switch ( _iorVertOri.GetRelationOrient() )
        {
            case REL_PG_FRAME:
            case REL_PG_PRTAREA:
            {
                if ( bConvDueToOrientation || bFollowTextFlow )
                    eVertConv = CONV2PG;
            }
            break;
            case FRAME:
            {
                if ( bConvDueToOrientation ||
                     _iorVertOri.GetVertOrient() == VERT_CENTER )
                {
                    eVertConv = CONV2PARA;
                }
            }
            break;
            case PRTAREA:
            {
                // relation not supported by WW8. Thus, conversion always needed.
                eVertConv = CONV2PARA;
            }
            break;
            case REL_CHAR:
            {
                // relation not supported by WW8. Thus, conversion always needed.
                eVertConv = CONV2PARA;
            }
            break;
            case REL_VERT_LINE:
            {
                if ( bConvDueToOrientation ||
                     _iorVertOri.GetVertOrient() == VERT_NONE )
                {
                    eVertConv = CONV2LINE;
                }
            }
            break;
            case REL_PG_LEFT:
            case REL_PG_RIGHT:
            case REL_FRM_LEFT:
            case REL_FRM_RIGHT:
            default:
                ASSERT( false,
                        "<WinwordAnchoring::ConvertPosition(..)> - unknown vertical relation" );
        }
        if ( eVertConv != NO_CONV )
        {
            _iorVertOri.SetVertOrient( VERT_NONE );
            SwTwips nPosY( 0L );
            {
                Point aPos;
                if ( eVertConv == CONV2PG )
                {
                    _iorVertOri.SetRelationOrient( REL_PG_FRAME );
                    // --> OD 2005-01-27 #i33818#
                    bool bRelToTableCell( false );
                    aPos = pAnchoredObj->GetRelPosToPageFrm( bFollowTextFlow,
                                                             bRelToTableCell );
                    if ( bRelToTableCell )
                    {
                        _iorVertOri.SetRelationOrient( REL_PG_PRTAREA );
                    }
                    // <--
                }
                else if ( eVertConv == CONV2PARA )
                {
                    _iorVertOri.SetRelationOrient( FRAME );
                    aPos = pAnchoredObj->GetRelPosToAnchorFrm();
                }
                else if ( eVertConv == CONV2LINE )
                {
                    _iorVertOri.SetRelationOrient( REL_VERT_LINE );
                    aPos = pAnchoredObj->GetRelPosToLine();
                }
                // No distinction between layout directions, because of missing
                // information about WW8 in vertical layout.
                nPosY = aPos.Y();
            }
            _iorVertOri.SetPos( nPosY );
            bConverted = true;
        }
    }

    return bConverted;
}

void WinwordAnchoring::SetAnchoring(const SwFrmFmt& rFmt)
{
    const RndStdIds eAnchor = rFmt.GetAnchor().GetAnchorId();
    mbInline = (eAnchor == FLY_IN_CNTNT);

    SwFmtHoriOrient rHoriOri = rFmt.GetHoriOrient();
    SwFmtVertOrient rVertOri = rFmt.GetVertOrient();

    // --> OD 2005-01-06 #i30669# - convert the positioning attributes.
    // Most positions are converted, if layout information exists.
    const bool bPosConverted = ConvertPosition( rHoriOri, rVertOri, rFmt );
    // <--

    const SwHoriOrient eHOri = rHoriOri.GetHoriOrient();
    // CMC, OD 24.11.2003 #i22673#
    const SwVertOrient eVOri = rVertOri.GetVertOrient();

    const SwRelationOrient eHRel = rHoriOri.GetRelationOrient();
    const SwRelationOrient eVRel = rVertOri.GetRelationOrient();

    // horizontal Adjustment
    switch (eHOri)
    {
        default:
        case HORI_NONE:
            mnXAlign = 0;
            break;
        case HORI_LEFT:
            mnXAlign = 1;
            break;
        case HORI_CENTER:
            mnXAlign = 2;
            break;
        case HORI_RIGHT:
            mnXAlign = 3;
            break;
        case HORI_INSIDE:
            mnXAlign = 4;
            break;
        case HORI_OUTSIDE:
            mnXAlign = 5;
            break;
    }

    // vertical Adjustment
    // CMC, OD 24.11.2003 #i22673#
    // When adjustment is vertically relative to line or to char
    // bottom becomes top and vice versa
    const bool bVertSwap = !bPosConverted &&
                           ( (eVRel == REL_CHAR) ||
                             (eVRel == REL_VERT_LINE) );
    switch (eVOri)
    {
        default:
        case VERT_NONE:
            mnYAlign = 0;
            break;
        case VERT_TOP:
        case VERT_LINE_TOP:
        case VERT_CHAR_TOP:
            mnYAlign = bVertSwap ? 3 : 1;
            break;
        case VERT_CENTER:
        case VERT_LINE_CENTER:
            mnYAlign = 2;
            break;
        case VERT_BOTTOM:
        case VERT_LINE_BOTTOM:
        case VERT_CHAR_BOTTOM:
            mnYAlign = bVertSwap ? 1 : 3;
            break;
    }

    // Adjustment is horizontally relative to...
    switch (eHRel)
    {
        case REL_PG_PRTAREA:
            mnXRelTo = 0;
            break;
        case REL_PG_FRAME:
        case REL_PG_LEFT:  //:-(
        case REL_PG_RIGHT: //:-(
            mnXRelTo = 1;
            break;
        case FRAME:
        case REL_FRM_LEFT: //:-(
        case REL_FRM_RIGHT: //:-(
            if (eAnchor == FLY_PAGE)
                mnXRelTo = 1;
            else
                mnXRelTo = 2;
            break;
        case PRTAREA:
            if (eAnchor == FLY_PAGE)
                mnXRelTo = 0;
            else
                mnXRelTo = 2;
            break;
        case REL_CHAR:
            mnXRelTo = 3;
            break;
    }

        // Adjustment is vertically relative to...
    switch (eVRel)
    {
        case REL_PG_PRTAREA:
            mnYRelTo = 0;
            break;
        case REL_PG_FRAME:
            mnYRelTo = 1;
            break;
        case PRTAREA:
            if (eAnchor == FLY_PAGE)
                mnYRelTo = 0;
            else
                mnYRelTo = 2;
            break;
        case FRAME:
            if (eAnchor == FLY_PAGE)
                mnYRelTo = 1;
            else
                mnYRelTo = 2;
            break;
        case REL_CHAR:
        case REL_VERT_LINE: // CMC, OD 24.11.2003 #i22673# - vertical alignment at top of line
        case REL_PG_LEFT:   //nonsense
        case REL_PG_RIGHT:  //nonsense
        case REL_FRM_LEFT:  //nonsense
        case REL_FRM_RIGHT: //nonsense
            mnYRelTo = 3;
            break;
    }
}

void SwEscherEx::WriteFrmExtraData( const SwFrmFmt& rFmt )
{
    aWinwordAnchoring.SetAnchoring(rFmt);
    aWinwordAnchoring.WriteData(*this);

    AddAtom(4, ESCHER_ClientAnchor);
    GetStream() << 0L;

    AddAtom(4, ESCHER_ClientData);
    GetStream() << 1L;
}

INT32 SwEscherEx::WriteFlyFrm(const DrawObj &rObj, UINT32 &rShapeId,
    DrawObjPointerVector &rPVec)
{
    const SwFrmFmt &rFmt = rObj.maCntnt.GetFrmFmt();
    short nDirection = rObj.mnDirection;
    unsigned int nHdFtIndex = rObj.mnHdFtIndex;

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

                rShapeId = GetFlyShapeId(rFmt, rObj.mnHdFtIndex, rPVec);
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
                        UINT32 nPrevShapeId =
                            GetFlyShapeId(*pFmt, rObj.mnHdFtIndex, rPVec);
                        pTxtBxs->Append( *pPrevObj, nPrevShapeId );
                        nTxtId = pTxtBxs->Count();
                    }
                    else
                        ++nTxtId;
                }
                nTxtId *= 0x10000;
                nTxtId += nOff;

                nBorderThick = WriteTxtFlyFrame(rObj, rShapeId, nTxtId, rPVec);
            }
        }
    }
    return nBorderThick;
}

USHORT FindPos(const SwFrmFmt &rFmt, unsigned int nHdFtIndex,
    DrawObjPointerVector &rPVec)
{
    DrawObjPointerIter aEnd = rPVec.end();
    for (DrawObjPointerIter aIter = rPVec.begin(); aIter != aEnd; ++aIter)
    {
        const DrawObj *pObj = (*aIter);
        ASSERT(pObj, "Impossible");
        if (!pObj)
            continue;
        if (
             nHdFtIndex == pObj->mnHdFtIndex &&
             &rFmt == (&pObj->maCntnt.GetFrmFmt())
           )
        {
            return aIter - rPVec.begin();
        }
    }
    return USHRT_MAX;
}

INT32 SwEscherEx::WriteTxtFlyFrame(const DrawObj &rObj, UINT32 nShapeId,
    UINT32 nTxtBox, DrawObjPointerVector &rPVec)
{
    const SwFrmFmt &rFmt = rObj.maCntnt.GetFrmFmt();
    short nDirection = rObj.mnDirection;

    INT32 nBorderThick=0;
    OpenContainer( ESCHER_SpContainer );

    AddShape( ESCHER_ShpInst_TextBox, 0xa00, nShapeId );
    EscherPropertyContainer aPropOpt;
    aPropOpt.AddOpt(ESCHER_Prop_lTxid, nTxtBox);
    if (const SwFrmFmt *pNext = rFmt.GetChain().GetNext())
    {
        USHORT nPos = FindPos(*pNext, rObj.mnHdFtIndex, rPVec);
        if (USHRT_MAX != nPos && aFollowShpIds[nPos])
            aPropOpt.AddOpt(ESCHER_Prop_hspNext, aFollowShpIds[nPos]);
    }
    nBorderThick = WriteFlyFrameAttr( rFmt, mso_sptTextBox, aPropOpt );

    MSO_TextFlow nFlow;

    switch (nDirection)
    {
        default:
            ASSERT(!this, "unknown direction type");
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

    AddAtom( 4, ESCHER_ClientTextbox ); GetStream() << nTxtBox;

    CloseContainer();   // ESCHER_SpContainer
    return nBorderThick;
}

void SwBasicEscherEx::WriteOLEPicture(EscherPropertyContainer &rPropOpt,
    sal_uInt32 nShapeFlags, const Graphic &rGraphic, const SdrObject &rObj,
    sal_uInt32 nShapeId)
{
    //nShapeFlags == 0xA00 + flips and ole active
    AddShape(ESCHER_ShpInst_PictureFrame, nShapeFlags, nShapeId);

    GraphicObject aGraphicObject(rGraphic);
    ByteString aId = aGraphicObject.GetUniqueID();
    if (aId.Len())
    {
        Rectangle aRect = rObj.GetLogicRect();
        aRect.SetPos(Point(0,0));
        aRect.Right() = DrawModelToEmu(aRect.Right());
        aRect.Bottom() = DrawModelToEmu(aRect.Bottom());
        sal_uInt32 nBlibId = GetBlibID(*QueryPicStream(), aId, aRect, 0);
        if (nBlibId)
            rPropOpt.AddOpt(ESCHER_Prop_pib, nBlibId, sal_True);
    }

    SetPicId(rObj, nShapeId, rPropOpt);
    rPropOpt.AddOpt( ESCHER_Prop_pictureActive, 0x10000 );
}

void SwEscherEx::WriteOCXControl( const SwFrmFmt& rFmt, UINT32 nShapeId )
{
    if (const SdrObject* pSdrObj = rFmt.FindRealSdrObject())
    {
        OpenContainer( ESCHER_SpContainer );

        SdrModel *pModel = rWrt.pDoc->GetDrawModel();
        OutputDevice *pDevice = Application::GetDefaultDevice();
        ASSERT(pModel && pDevice, "no model or device");
        SdrExchangeView aExchange(pModel, pDevice);
        Graphic aGraphic(aExchange.GetObjGraphic(pModel,
            const_cast<SdrObject*>(pSdrObj)));

        EscherPropertyContainer aPropOpt;
        WriteOLEPicture(aPropOpt, 0xa00 | SHAPEFLAG_OLESHAPE, aGraphic,
            *pSdrObj, nShapeId);

        WriteFlyFrameAttr( rFmt, mso_sptPictureFrame , aPropOpt );
        aPropOpt.Commit( GetStream() );

        // store anchor attribute
        WriteFrmExtraData( rFmt );

        CloseContainer();   // ESCHER_SpContainer
    }
}

/*
 I actually want to use EMF not PNG but until issue #i2192# is resolved then I
 can't risk having a math object using starsymbol exported to word as it'll
 not render correctly :-(

 Optionally perhaps detect if a GDIMetafile is using {Open|Star}Symbol and if
 it is then use PNG, and if not use EMF. That would be a very acceptable
 compromise in my view.

 But currently just use wmf as we have always done :-(
*/
Graphic wwUtility::MakeSafeGDIMetaFile( const com::sun::star::uno::Reference < com::sun::star::embed::XEmbeddedObject >& xObj )
{
#if 1
    if (xObj.is())
    {
        // TODO/LATER: not used currently, it's completely unclear if this is a problem
        //xObj->GetGDIMetaFile(aMtf);
        //return Graphic(aMtf);

        // It would be better to retrieve the last representation from the storage (it could still be the original one!)
        /*
        embed::VisualRepresentation aRep = xObj->getPreferredVisualRepresentation( embed::Aspects::MSOLE_CONTENT );
        uno::Sequence < sal_Int8 > aSeq;
        aRep.Data >>= aSeq;
        uno::Reference < io::XInputStream > xStream = new ::comphelper::SequenceInputStream( aSeq );
        SvStream* pGraphicStream = ::utl::UcbStreamHelper::CreateStream( xStream );
        if ( pGraphicStream && !pGraphicStream->GetError() )
        {
            GraphicFilter* pGF = GraphicFilter::GetGraphicFilter();
            Graphic aGraphic;
            String aEmptyStr;
            pGF->ImportGraphic( aGraphic, aEmptyStr, *pGraphicStream, GRFILTER_FORMAT_DONTKNOW );
            delete pGraphicStream;
            return aGraphic;
        }*/
    }

    return Graphic();
#else
    Graphic aGraphic;
    if (xObj.Is())
    {
        GDIMetaFile aMtf;
        xObj->GetGDIMetaFile(aMtf);
        Size aSize(xObj->GetVisArea().GetSize());
        MapUnit aUnit(xObj->GetMapUnit());
        aMtf.SetPrefSize(aSize);
        aMtf.SetPrefMapMode(aUnit);
        aGraphic = Graphic(aMtf);
    }
    return aGraphic;
#endif
}

void SwEscherEx::MakeZOrderArrAndFollowIds(
    std::vector<DrawObj>& rSrcArr, std::vector<DrawObj*>&rDstArr)
{
    USHORT n, nCnt = rSrcArr.size();
    SvULongsSort aSort( 255 < nCnt ? 255 : nCnt, 255 );
    rDstArr.clear();
    rDstArr.reserve(nCnt);
    for (n = 0; n < nCnt; ++n)
    {
        const SwFrmFmt &rFmt = rSrcArr[n].maCntnt.GetFrmFmt();
        ULONG nOrdNum = rWrt.GetSdrOrdNum(rFmt);
        USHORT nPos;
        //returns what will be the index in rDstArr of p as nPos
        aSort.Insert(nOrdNum, nPos);
        DrawObj &rObj = rSrcArr[n];
        rDstArr.insert(rDstArr.begin() + nPos, &rObj);
    }

    if (aFollowShpIds.Count())
        aFollowShpIds.Remove(0, aFollowShpIds.Count());

    for (n = 0; n < nCnt; ++n)
    {
        const SwFrmFmt &rFmt = rDstArr[n]->maCntnt.GetFrmFmt();
        bool bNeedsShapeId = false;

        if (RES_FLYFRMFMT == rFmt.Which())
        {
            const SwFmtChain &rChain = rFmt.GetChain();
            if (rChain.GetPrev() || rChain.GetNext())
                bNeedsShapeId = true;
        }

        ULONG nShapeId = bNeedsShapeId ? GetShapeID() : 0;

        aFollowShpIds.Insert(nShapeId, n);
    }
}

UINT32 SwEscherEx::GetFlyShapeId(const SwFrmFmt& rFmt,
    unsigned int nHdFtIndex, DrawObjPointerVector &rpVec)
{
    USHORT nPos = FindPos(rFmt, nHdFtIndex, rpVec);
    UINT32 nShapeId;
    if (USHRT_MAX != nPos)
    {
        if (0 == (nShapeId = aFollowShpIds[nPos]))
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

bool SwMSConvertControls::ExportControl(Writer &rWrt, const SdrObject *pObj)
{
    SwWW8Writer& rWW8Wrt = (SwWW8Writer&)rWrt;

    if (!rWW8Wrt.bWrtWW8)
        return false;

    SdrUnoObj *pFormObj = PTR_CAST(SdrUnoObj,pObj);
    uno::Reference< awt::XControlModel > xControlModel =
    pFormObj->GetUnoControlModel();

    //Why oh lord do we use so many different units ?
    //I think I painted myself into a little bit of a
    //corner by trying to use the uno interface for
    //controls export
    Rectangle aRect = pFormObj->GetLogicRect();
    aRect.SetPos(Point(0,0));
    awt::Size aSize;
    aSize.Width = TWIPS_TO_MM(aRect.Right());
    aSize.Height = TWIPS_TO_MM(aRect.Bottom());

    //Open the ObjectPool
    SvStorageRef xObjPool = rWW8Wrt.GetStorage().OpenSotStorage(
        CREATE_CONST_ASC(SL::aObjectPool), STREAM_READWRITE |
        STREAM_SHARE_DENYALL);

    //Create a destination storage for the microsoft control
    String sStorageName('_');
    sStorageName += String::CreateFromInt32((UINT32)pObj);
    SvStorageRef xOleStg = xObjPool->OpenSotStorage(sStorageName,
                 STREAM_READWRITE|STREAM_SHARE_DENYALL);

    if (!xOleStg.Is())
        return false;

    String sName;
    if (!WriteOCXStream(xOleStg,xControlModel,aSize,sName))
        return false;

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

    String sFld(FieldString(ww::eCONTROL));
    sFld.APPEND_CONST_ASC("Forms.");
    sFld += sName;
    sFld.APPEND_CONST_ASC(".1 \\s ");

    rWW8Wrt.OutField(0, ww::eCONTROL, sFld,
        WRITEFIELD_START|WRITEFIELD_CMD_START|WRITEFIELD_CMD_END);

    rWW8Wrt.pChpPlc->AppendFkpEntry(rWW8Wrt.Strm().Tell(),sizeof(aSpecOLE),
        aSpecOLE);
    rWW8Wrt.WriteChar( 0x1 );
    rWW8Wrt.OutField(0, ww::eCONTROL, aEmptyStr, WRITEFIELD_END | WRITEFIELD_CLOSE);
    return true;
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
