/*************************************************************************
 *
 *  $RCSfile: writerhelper.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2003-12-09 11:52:10 $
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

#ifndef SW_WRITERHELPER
#	include "writerhelper.hxx" 
#endif

#include <algorithm>                //std::swap
#include <functional>               //std::binary_function

#ifndef _SVDOBJ_HXX
#    include <svx/svdobj.hxx>       //SdrObject
#endif
#ifndef _SVDOOLE2_HXX
#   include <svx/svdoole2.hxx>      //SdrOle2Obj
#endif
#ifndef _SVX_FMGLOB_HXX
#   include <svx/fmglob.hxx>        //FmFormInventor
#endif
#ifndef _SVX_BRKITEM_HXX
#   include <svx/brkitem.hxx>       //SvxFmtBreakItem
#endif
#ifndef _SVX_TSPTITEM_HXX
#    include <svx/tstpitem.hxx>     //SvxTabStopItem
#endif
#ifndef _SVDOBJ_HXX
#    include <svx/svdobj.hxx>       //SdrObject
#endif
#ifndef _DOC_HXX
#    include <doc.hxx>              //SwDoc
#endif
#ifndef _NDTXT_HXX
#    include <ndtxt.hxx>            //SwTxtNode
#endif
#ifndef _NDNOTXT_HXX
#     include <ndnotxt.hxx>         //SwNoTxtNode
#endif
#ifndef _FMTCNTNT_HXX
#    include <fmtcntnt.hxx>         //SwFmtCntnt
#endif
#ifndef _NDINDEX_HXX
#    include <ndindex.hxx>          //SwNodeIndex
#endif
#ifndef _NUMRULE_HXX
#    include <numrule.hxx>          //SwNodeNum
#endif
#ifndef _SWTABLE_HXX
#    include <swtable.hxx>          //SwTable
#endif
#ifndef _FRMFMT_HXX
#    include <frmfmt.hxx>           //SwFrmFmt
#endif
#ifndef _FLYPOS_HXX
#    include <flypos.hxx>           //SwPosFlyFrms
#endif
#ifndef _FMTANCHR_HXX
#    include <fmtanchr.hxx>         //SwFmtAnchor
#endif
#ifndef _NDGRF_HXX
#    include <ndgrf.hxx>            //SwGrfNode
#endif
#ifndef _FMTFSIZE_HXX
#    include <fmtfsize.hxx>         //SwFmtFrmSize
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#   include<SwStyleNameMapper.hxx>  //SwStyleNameMapper
#endif
#ifndef _DOCARY_HXX
#   include <docary.hxx>            //SwCharFmts
#endif
#ifndef _CHARFMT_HXX
#    include <charfmt.hxx>          //SwCharFmt
#endif
#ifdef DEBUGDUMP
#   ifndef _SV_SVAPP_HXX
#       include <vcl/svapp.hxx>
#   endif
#   ifndef _TOOLS_URLOBJ_HXX
#       include <tools/urlobj.hxx>
#   endif
#   ifndef _UNOTOOLS_UCBSTREAMHELPER_HXX
#       include <unotools/ucbstreamhelper.hxx>
#   endif
#   ifndef _UNOTOOLS_LOCALFILEHELPER_HXX
#       include <unotools/localfilehelper.hxx>
#   endif
#endif

namespace 
{
    //Utility to sort SwTxtFmtColl's by their outline numbering level
    class outlinecmp : public 
        std::binary_function<const SwTxtFmtColl*, const SwTxtFmtColl*, bool>
    {
    public:
        bool operator()(const SwTxtFmtColl *pA, const SwTxtFmtColl *pB) const
        {
            return pB->GetOutlineLevel() < pB->GetOutlineLevel();
        }
    };

    /*
     Stroustroup forgets copy_if, See C++ Programming language Chp 18, pg 530
    */
    template <typename In , typename Out , typename Pred>
        Out my_copy_if(In first, In last, Out res, Pred p)
    {
        while (first != last)
        {
            if (p(*first))
                *res = *first;
            ++first;
        }
        return res;
    }

    /*
     Utility to convert a SwPosFlyFrms into a simple vector of sw::Frames

     The crucial thing is that a sw::Frame always has an anchor which 
     points to some content in the document. This is a requirement of exporting
     to Word
    */
    sw::Frames SwPosFlyFrmsToFrames(const SwPosFlyFrms &rFlys)
    {
        sw::Frames aRet;
        USHORT nEnd = rFlys.Count();
        for (USHORT nI = 0; nI < nEnd; ++nI)
        {
            const SwFrmFmt &rEntry = rFlys[nI]->GetFmt();
            if (const SwPosition* pAnchor = rEntry.GetAnchor().GetCntntAnchor())
                aRet.push_back(sw::Frame(rEntry, *pAnchor));
            else
            {
                SwPosition aPos(rFlys[nI]->GetNdIndex());
                if (SwTxtNode* pTxtNd = aPos.nNode.GetNode().GetTxtNode())
                    aPos.nContent.Assign(pTxtNd, 0);
                aRet.push_back(sw::Frame(rEntry, aPos));
            }
        }
        return aRet;
    }

    /*
     Utility to extract flyfmts from a document, potentially from a 
     selection, and with bAll off ignores the drawing objects
    */
    sw::Frames GetFrames(const SwDoc &rDoc, SwPaM *pPaM, bool bAll)
    {
        SwPosFlyFrms aFlys;
        rDoc.GetAllFlyFmts(aFlys, pPaM, true);
        sw::Frames aRet(SwPosFlyFrmsToFrames(aFlys));
        for (USHORT i = aFlys.Count(); i > 0;)
            delete aFlys[--i];
        return aRet;
    }

    //Utility to test if a frame is anchored at a given node index
    class anchoredto: public std::unary_function<const sw::Frame&, bool>
    {
    private:
        ULONG mnNode;
    public:
        anchoredto(ULONG nNode) : mnNode(nNode) {}
        bool operator()(const sw::Frame &rFrame) const
        {
            return (mnNode == rFrame.GetPosition().nNode.GetNode().GetIndex());
        }
    };

    Size GetSwappedInSize(const SwNoTxtNode& rNd)
    {
        Size aGrTwipSz(rNd.GetTwipSize());
        if ((!aGrTwipSz.Width() || !aGrTwipSz.Height()))
        {
            SwGrfNode *pGrfNode = const_cast<SwGrfNode*>(rNd.GetGrfNode());
            if (pGrfNode && (GRAPHIC_NONE != pGrfNode->GetGrf().GetType()))
            {
                bool bWasSwappedOut = pGrfNode->GetGrfObj().IsSwappedOut();
                pGrfNode->SwapIn();
                aGrTwipSz = pGrfNode->GetTwipSize();
                if (bWasSwappedOut)
                    pGrfNode->SwapOut();
            }
        }

        ASSERT(aGrTwipSz.Width() && aGrTwipSz.Height(), "0 x 0 graphic ?");
        return aGrTwipSz;
    }
}
 
namespace sw
{
    Frame::Frame(const SwFrmFmt &rFmt, const SwPosition &rPos)
        : mpFlyFrm(&rFmt), maPos(rPos), meWriterType(eTxtBox), 
        mpStartFrameContent(0)
    {
        mbIsInline = (rFmt.GetAnchor().GetAnchorId() == FLY_IN_CNTNT);
        switch (rFmt.Which())
        {
            case RES_FLYFRMFMT:
                if (const SwNodeIndex* pIdx = rFmt.GetCntnt().GetCntntIdx())
                {
                    SwNodeIndex aIdx(*pIdx, 1);
                    const SwNode &rNd = aIdx.GetNode();
                    switch (rNd.GetNodeType())
                    {
                        case ND_GRFNODE:
                            meWriterType = eGraphic;
                            maSize = GetSwappedInSize(*rNd.GetNoTxtNode());
                            break;
                        case ND_OLENODE:
                            meWriterType = eOle;
                            maSize = GetSwappedInSize(*rNd.GetNoTxtNode());
                            break;
                        default:
                            {
                                meWriterType = eTxtBox;

                                Rectangle aRect;
                                SwRect aLayRect(rFmt.FindLayoutRect());
                                /*
                                 The Object is not rendered (e.g. something in
                                 unused header/footer - so get the values from
                                 the format.
                                */
                                if (aLayRect.IsEmpty())
                                    aRect.SetSize(rFmt.GetFrmSize().GetSize());
                                else
                                    aRect = aLayRect.SVRect();

                                maSize = aRect.GetSize();
                            }
                            break;
                    }
                    mpStartFrameContent = &rNd;
                }
                else
                {
                    ASSERT(!this, "Impossible");
                    meWriterType = eTxtBox;
                }
                break;
            default:
                if (const SdrObject* pObj = rFmt.FindRealSdrObject())
                {
                    if (pObj->GetObjInventor() == FmFormInventor)
                        meWriterType = eFormControl;
                    else
                        meWriterType = eDrawing;
                    maSize = pObj->GetSnapRect().GetSize();
                }
                else
                {
                    ASSERT(!this, "Impossible");
                    meWriterType = eDrawing;
                }
                break;
        }
    }

    bool Frame::IsInline() const
    {
        return mbIsInline;
    }

    void Frame::ForceTreatAsInline()
    {
        mbIsInline = true;
    }

    namespace hack
    {
        DrawingOLEAdaptor::DrawingOLEAdaptor(SdrOle2Obj &rObj, 
            SvPersist &rPers)
            : mxIPRef(rObj.GetObjRef()), 
            msOrigPersistName(rObj.GetPersistName()), mrPers(rPers)
        {
            rObj.SetPersistName(String());
            rObj.SetObjRef(SvInPlaceObjectRef());
        }

        bool DrawingOLEAdaptor::TransferToDoc(const String &rName)
        {
            ASSERT(mxIPRef.Is(), "Transferring invalid object to doc");
            if (!mxIPRef.Is())
                return false;

            SvInfoObjectRef refObj = new SvEmbeddedInfoObject(mxIPRef, rName);
            bool bSuccess = mrPers.Move(refObj, rName);
            if (bSuccess)
            {
                SvPersist *pO = mxIPRef;
                ASSERT(!pO->IsModified(), "Not expected to be modified here");
                if (pO->IsModified())
                {
                    pO->DoSave();
                    pO->DoSaveCompleted();
                }
                mxIPRef.Clear();
                bSuccess = mrPers.Unload(pO);
            }

            return bSuccess;
        }

        DrawingOLEAdaptor::~DrawingOLEAdaptor()
        {
            if (mxIPRef.Is())
            {
                if (SvInfoObject* pInfo = mrPers.Find(msOrigPersistName))
                {
                    pInfo->SetDeleted(TRUE);
                    pInfo->SetObj(0);
                }
                mxIPRef->DoClose();
                mrPers.Remove(mxIPRef);
                mxIPRef.Clear();
            }
        }

#ifdef DEBUGDUMP
        SvStream *CreateDebuggingStream(const String &rSuffix)
        {
            SvStream* pDbgOut = 0;
            static sal_Int32 nCount;
            String aFileName(String(RTL_CONSTASCII_STRINGPARAM("wwdbg")));
            aFileName.Append(String::CreateFromInt32(++nCount));
            aFileName.Append(rSuffix);
            String aURLStr;
            if (::utl::LocalFileHelper::ConvertPhysicalNameToURL(
                Application::GetAppFileName(), aURLStr))
            {
                INetURLObject aURL(aURLStr);
                aURL.removeSegment();
                aURL.removeFinalSlash();
                aURL.Append(aFileName);

                pDbgOut = ::utl::UcbStreamHelper::CreateStream(
                    aURL.GetMainURL(INetURLObject::NO_DECODE), 
                    STREAM_TRUNC | STREAM_WRITE);
            }
            return pDbgOut;
        }

        void DumpStream(const SvStream &rSrc, SvStream &rDest)
        {
            SvStream &rSource = const_cast<SvStream&>(rSrc);
            ULONG nOrigPos = rSource.Tell();
            rSource.Seek(STREAM_SEEK_TO_END);
            if (ULONG nLen = rSource.Tell())
            {
                rSource.Seek(nOrigPos);
                sal_Char* pDat = new sal_Char[nLen];
                rSource.Read(pDat, nLen);
                rDest.Write(pDat, nLen);
                delete[] pDat;
                rSource.Seek(nOrigPos);
            }
        }
#endif

        const OutlinerParaObject* GetOutlinerParaObject(const SdrTextObj &rObj)
        {
            /*
            #i13885#
            When the object is actively being edited, that text is not set into
            the objects normal text object, but lives in a seperate object.
            */
            if (rObj.IsTextEditActive())
                return rObj.GetEditOutlinerParaObject();
            else
                return rObj.GetOutlinerParaObject();
        }
    }

    namespace util
    {
        void SetLayer::SendObjectToHell(SdrObject &rObject) const
        {
            SetObjectLayer(rObject, eHell);
        }

        void SetLayer::SendObjectToHeaven(SdrObject &rObject) const
        {
            SetObjectLayer(rObject, eHeaven);
        }
        
        void SetLayer::SetObjectLayer(SdrObject &rObject, Layer eLayer) const
        {
            if (FmFormInventor == rObject.GetObjInventor())
                rObject.SetLayer(mnFormLayer);
            else
            {
                switch (eLayer)
                {
                    case eHeaven:
                        rObject.SetLayer(mnHeavenLayer);
                        break;
                    case eHell:
                        rObject.SetLayer(mnHellLayer);
                        break;
                }
            }
        }

        //SetLayer boilerplate begin
        void SetLayer::Swap(SetLayer& rOther) throw()
        {
            std::swap(mnHeavenLayer, rOther.mnHeavenLayer);
            std::swap(mnHellLayer, rOther.mnHellLayer);
            std::swap(mnFormLayer, rOther.mnFormLayer);
        }

        SetLayer::SetLayer(const SwDoc &rDoc)
            : mnHeavenLayer(rDoc.GetHeavenId()), 
            mnHellLayer(rDoc.GetHellId()),
            mnFormLayer(rDoc.GetControlsId())
        {
        }

        SetLayer::SetLayer(const SetLayer& rOther) throw()
            : mnHeavenLayer(rOther.mnHeavenLayer),
            mnHellLayer(rOther.mnHellLayer),
            mnFormLayer(rOther.mnFormLayer)
        {
        }

        SetLayer& SetLayer::operator=(const SetLayer& rOther) throw()
        {
            SetLayer aTemp(rOther);
            Swap(aTemp);
            return *this;
        }
        //SetLayer boilerplate end

        ParaStyles GetParaStyles(const SwDoc &rDoc)
        {
            ParaStyles aStyles;
            typedef ParaStyles::size_type mysizet;

            const SwTxtFmtColls *pColls = rDoc.GetTxtFmtColls();
            mysizet nCount = pColls ? pColls->Count() : 0;
            aStyles.reserve(nCount);
            for (mysizet nI = 0; nI < nCount; ++nI)
                aStyles.push_back((*pColls)[nI]);
            return aStyles;
        }

        SwTxtFmtColl* GetParaStyle(SwDoc &rDoc, const String& rName)
        {
            // Search first in the Doc-Styles
            SwTxtFmtColl* pColl = rDoc.FindTxtFmtCollByName(rName);
            if (!pColl)
            {
                // Collection not found, try in Pool ?
                sal_uInt16 n = SwStyleNameMapper::GetPoolIdFromUIName(rName, 
                    GET_POOLID_TXTCOLL);
                if (n != SAL_MAX_UINT16)       // found or standard
                    pColl = rDoc.GetTxtCollFromPoolSimple(n, false);
            }
            return pColl;
        }

        SwCharFmt* GetCharStyle(SwDoc &rDoc, const String& rName)
        {
            SwCharFmt *pFmt = rDoc.FindCharFmtByName(rName);
            if (!pFmt)
            {
                // Collection not found, try in Pool ?
                sal_uInt16 n = SwStyleNameMapper::GetPoolIdFromUIName(rName,
                    GET_POOLID_CHRFMT);
                if (n != SAL_MAX_UINT16)       // found or standard
                    pFmt = rDoc.GetCharFmtFromPool(n);
            }
            return pFmt;
        }

        void SortByOutline(ParaStyles &rStyles)
        {
            std::sort(rStyles.begin(), rStyles.end(), outlinecmp());
        }

        Frames GetAllFrames(const SwDoc &rDoc, SwPaM *pPaM)
        {
            return GetFrames(rDoc, pPaM, true);
        }

        Frames GetNonDrawingFrames(const SwDoc &rDoc, SwPaM *pPaM)
        {
            return GetFrames(rDoc, pPaM, false);
        }

        Frames GetFramesBetweenNodes(const Frames &rFrames, 
            const SwNode &rStart, const SwNode &rEnd)
        {
            Frames aRet;
            ULONG nEnd = rEnd.GetIndex();
            for (ULONG nI = rStart.GetIndex(); nI < nEnd; ++nI)
            {
                my_copy_if(rFrames.begin(), rFrames.end(), 
                    std::back_inserter(aRet), anchoredto(nI));
            }
            return aRet;

        }

        Frames GetFramesInNode(const Frames &rFrames, const SwNode &rNode)
        {
            Frames aRet;
            my_copy_if(rFrames.begin(), rFrames.end(), 
                std::back_inserter(aRet), anchoredto(rNode.GetIndex()));
            return aRet;
        }

        const SwNumFmt* GetNumFmtFromTxtNode(const SwTxtNode &rTxtNode)
        {
            const SwNumRule *pRule = 0;
            const SwNodeNum* pNum = 0;
            if ( 
                (pNum = rTxtNode.GetNum()) &&
                (MAXLEVEL > pNum->GetLevel()) &&
                (pRule = rTxtNode.GetNumRule())
               )
            {
                return &(pRule->Get(pNum->GetLevel()));
            }

            ASSERT(rTxtNode.GetDoc(), "No document for node?, suspicious");
            if (!rTxtNode.GetDoc())
                return 0;

            if ( 
                  (pNum = rTxtNode.GetOutlineNum()) &&
                  (MAXLEVEL > pNum->GetLevel()) &&
                  (pRule = rTxtNode.GetDoc()->GetOutlineNumRule())
                )
            {
                return &(pRule->Get(pNum->GetLevel()));
            }

            return 0;
        }

        SwNoTxtNode *GetNoTxtNodeFromSwFrmFmt(const SwFrmFmt &rFmt)
        {
            const SwNodeIndex *pIndex = rFmt.GetCntnt().GetCntntIdx();
            ASSERT(pIndex, "No NodeIndex in SwFrmFmt ?, suspicious");
            if (!pIndex)
                return 0;
            SwNodeIndex aIdx(*pIndex, 1);
            return aIdx.GetNode().GetNoTxtNode();
        }

        bool HasPageBreak(const SwNode &rNd)
        {
            const SvxFmtBreakItem *pBreak = 0;
            if (rNd.IsTableNode() && rNd.GetTableNode())
            {
                const SwTable& rTable = rNd.GetTableNode()->GetTable();
                const SwFrmFmt* pApply = rTable.GetFrmFmt();
                ASSERT(pApply, "impossible");
                if (pApply)
                    pBreak = &(ItemGet<SvxFmtBreakItem>(*pApply, RES_BREAK));
            }
            else if (const SwCntntNode *pNd = rNd.GetCntntNode())
                pBreak = &(ItemGet<SvxFmtBreakItem>(*pNd, RES_BREAK));

            if (pBreak && pBreak->GetBreak() == SVX_BREAK_PAGE_BEFORE)
                return true;
            return false;
        }

        bool AdjustTabs(SvxTabStopItem &rTStop, long nSrcLeft, long nDestLeft)
        {
            bool bChanged = false;
            if (nDestLeft != nSrcLeft)
            {
                USHORT nCount = rTStop.Count();
                for (USHORT nCnt = 0; nCnt < nCount; ++nCnt)
                {
                    SvxTabStop& rTab = const_cast<SvxTabStop&>(rTStop[nCnt]);
                    if (SVX_TAB_ADJUST_DEFAULT != rTab.GetAdjustment())
                    {
                        rTab.GetTabPos() += nSrcLeft;
                        rTab.GetTabPos() -= nDestLeft;
                        bChanged = true;
                    }
                }
            }
            return bChanged;
        }

        Polygon PolygonFromPolyPolygon(const PolyPolygon &rPolyPoly)
        {
            PolyPolygon3D aPolyPoly(rPolyPoly);
            aPolyPoly.Merge();
            const PolyPolygon &rNewPolyPoly = aPolyPoly.GetPolyPolygon();
            if (!rNewPolyPoly.Count())
                return Polygon();
            else
            {
                ASSERT(rNewPolyPoly.Count() == 1, 
                    "I (cmc) must not have understood PolyPoly3D Merge");
                return rPolyPoly.GetObject(0);
            }
        }
 
    }
}
/* vi:set tabstop=4 shiftwidth=4 expandtab: */
