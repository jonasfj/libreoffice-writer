/*************************************************************************
 *
 *  $RCSfile: writerhelper.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2005-01-05 14:32:20 $
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
#ifndef SW_MS_MSFILTER_HXX
#   include <msfilter.hxx>
#endif

#ifndef _COM_SUN_STAR_EMBED_EMBEDSTATES_HPP_
#include <com/sun/star/embed/EmbedStates.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif

#include <algorithm>                //std::swap
#include <functional>               //std::binary_function

#ifndef _SFXITEMITER_HXX
#   include <svtools/itemiter.hxx>  //SfxItemIter
#endif
#ifndef _SVDOBJ_HXX
#   include <svx/svdobj.hxx>        //SdrObject
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
#   include <svx/tstpitem.hxx>      //SvxTabStopItem
#endif
#ifndef _SVDOBJ_HXX
#   include <svx/svdobj.hxx>        //SdrObject
#endif
#ifndef _DOC_HXX
#   include <doc.hxx>               //SwDoc
#endif
#ifndef _NDTXT_HXX
#   include <ndtxt.hxx>             //SwTxtNode
#endif
#ifndef _NDNOTXT_HXX
#    include <ndnotxt.hxx>          //SwNoTxtNode
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
#   include <SwStyleNameMapper.hxx> //SwStyleNameMapper
#endif
#ifndef _DOCARY_HXX
#   include <docary.hxx>            //SwCharFmts
#endif
#ifndef _CHARFMT_HXX
#   include <charfmt.hxx>           //SwCharFmt
#endif
#ifndef _FCHRFMT_HXX
#   include <fchrfmt.hxx>           //SwFmtCharFmt
#endif
#ifndef _UNOTOOLS_STREAMWRAP_HXX
#   include <unotools/streamwrap.hxx>
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

    //Utility to sort SwTxtFmtColl's by their outline numbering level
    class outlinecmp : public
        std::binary_function<const SwTxtFmtColl*, const SwTxtFmtColl*, bool>
    {
    public:
        bool operator()(const SwTxtFmtColl *pA, const SwTxtFmtColl *pB) const
        {
            return pA->GetOutlineLevel() < pB->GetOutlineLevel();
        }
    };

    bool IsValidSlotWhich(USHORT nSlotId, USHORT nWhichId)
    {
        return (nSlotId != 0 && nWhichId != 0 && nSlotId != nWhichId);
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
                    using sw::util::GetSwappedInSize;
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

        USHORT TransformWhichBetweenPools(const SfxItemPool &rDestPool,
            const SfxItemPool &rSrcPool, USHORT nWhich)
        {
            USHORT nSlotId = rSrcPool.GetSlotId(nWhich);
            if (IsValidSlotWhich(nSlotId, nWhich))
                nWhich = rDestPool.GetWhich(nSlotId);
            else
                nWhich = 0;
            return nWhich;
        }

        USHORT GetSetWhichFromSwDocWhich(const SfxItemSet &rSet,
            const SwDoc &rDoc, USHORT nWhich)
        {
            if (RES_WHICHHINT_END < *(rSet.GetRanges()))
            {
                nWhich = TransformWhichBetweenPools(*rSet.GetPool(),
                    rDoc.GetAttrPool(), nWhich);
            }
            return nWhich;
        }

        DrawingOLEAdaptor::DrawingOLEAdaptor(SdrOle2Obj &rObj,
            SfxObjectShell &rPers)
            : msOrigPersistName(rObj.GetPersistName()),
            mxIPRef(rObj.GetObjRef()), mrPers(rPers),
            mpGraphic( rObj.GetGraphic() )
        {
            //rObj.SetPersistName(String());
            //rObj.SetObjRef(0);
            rObj.AbandonObject();
        }

        bool DrawingOLEAdaptor::TransferToDoc( ::rtl::OUString &rName )
        {
            ASSERT(mxIPRef.is(), "Transferring invalid object to doc");
            if (!mxIPRef.is())
                return false;

            bool bSuccess = mrPers.GetEmbeddedObjectContainer().InsertEmbeddedObject( mxIPRef, rName );
            if (bSuccess)
            {
                if ( mpGraphic )
                    ::svt::EmbeddedObjectRef::SetGraphicToContainer( *mpGraphic,
                                                                    mrPers.GetEmbeddedObjectContainer(),
                                                                    rName,
                                                                    ::rtl::OUString() );

                //mxIPRef->changeState( com::sun::star::embed::EmbedStates::LOADED );
                mxIPRef = 0;
            }

            return bSuccess;
        }

        DrawingOLEAdaptor::~DrawingOLEAdaptor()
        {
            if (mxIPRef.is())
            {
                DBG_ASSERT( !mrPers.GetEmbeddedObjectContainer().HasEmbeddedObject( mxIPRef ), "Object in adaptor is inserted?!" );
                try
                {
                    com::sun::star::uno::Reference < com::sun::star::util::XCloseable > xClose( mxIPRef, com::sun::star::uno::UNO_QUERY );
                    if ( xClose.is() )
                        xClose->close(sal_True);
                }
                catch ( com::sun::star::util::CloseVetoException& )
                {
                }

                mxIPRef = 0;
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

        void DumpStream(const SvStream &rSrc, SvStream &rDest, sal_uInt32 nLen)
        {
            SvStream &rSource = const_cast<SvStream&>(rSrc);
            ULONG nOrigPos = rSource.Tell();
            if (nLen == STREAM_SEEK_TO_END)
            {
                rSource.Seek(STREAM_SEEK_TO_END);
                nLen = rSource.Tell();
            }
            if (nLen - nOrigPos)
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
        SV_IMPL_OP_PTRARR_SORT(AuthorInfos, AuthorInfo_Ptr)

        SwTwips MakeSafePositioningValue(SwTwips nIn)
        {
            if (nIn > SHRT_MAX)
                nIn = SHRT_MAX;
            else if (nIn < SHRT_MIN)
                nIn = SHRT_MIN;
            return nIn;
        }

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

        // --> OD 2004-12-13 #i38889# - by default put objects into the invisible
        // layers.
        SetLayer::SetLayer(const SwDoc &rDoc)
            : mnHeavenLayer(rDoc.GetInvisibleHeavenId()),
              mnHellLayer(rDoc.GetInvisibleHellId()),
              mnFormLayer(rDoc.GetInvisibleControlsId())
        {
        }
        // <--

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

        void GetPoolItems(const SfxItemSet &rSet, PoolItems &rItems)
        {
            if (rSet.Count())
            {
                SfxItemIter aIter(rSet);
                if (const SfxPoolItem *pItem = aIter.GetCurItem())
                {
                    do
                        rItems[pItem->Which()] = pItem;
                    while (!aIter.IsAtEnd() && (pItem = aIter.NextItem()));
                }
            }
        }

        const SfxPoolItem *SearchPoolItems(const PoolItems &rItems,
            sal_uInt16 eType)
        {
            sw::cPoolItemIter aIter = rItems.find(eType);
            if (aIter != rItems.end())
                return aIter->second;
            return 0;
        }

        void ClearOverridesFromSet(const SwFmtCharFmt &rFmt, SfxItemSet &rSet)
        {
            if (const SwCharFmt* pCharFmt = rFmt.GetCharFmt())
            {
                if (pCharFmt->GetAttrSet().Count())
                {
                    SfxItemIter aIter(pCharFmt->GetAttrSet());
                    const SfxPoolItem *pItem = aIter.GetCurItem();
                    do
                        rSet.ClearItem(pItem->Which());
                    while (!aIter.IsAtEnd() && (pItem = aIter.NextItem()));
                }
            }
        }

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

        const SwNumRule* GetNumRuleFromTxtNode(const SwTxtNode &rTxtNode)
        {
            if (const SwNumRule *pRule = GetNormalNumRuleFromTxtNode(rTxtNode))
                return pRule;
            return GetOutlineNumRuleFromTxtNode(rTxtNode);
        }

        const SwNumRule* GetOutlineNumRuleFromTxtNode(const SwTxtNode &rTxtNode)
        {
            const SwNumRule *pRet = 0;
            const SwNumRule *pRule = 0;
            const SwNodeNum* pNum;

            ASSERT(rTxtNode.GetDoc(), "No document for node?, suspicious");
            if (!rTxtNode.GetDoc())
                return 0;

            if (
                (pNum = rTxtNode.GetOutlineNum()) &&
                (MAXLEVEL > pNum->GetLevel()) &&
                (pRule = rTxtNode.GetDoc()->GetOutlineNumRule())
               )
            {
                return pRule;
            }

            return 0;
        }

        const SwNumRule* GetNormalNumRuleFromTxtNode(const SwTxtNode &rTxtNode)
        {
            const SwNumRule *pRet = 0;
            const SwNumRule *pRule = 0;
            const SwNodeNum* pNum;

            if (
                (pNum = rTxtNode.GetNum()) &&
                (MAXLEVEL > pNum->GetLevel()) &&
                (pRule = rTxtNode.GetNumRule())
               )
            {
                return pRule;
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
            if(1 == rPolyPoly.Count())
            {
                return rPolyPoly[0];
            }
            else
            {
                // This method will now just concatenate the polygons contained
                // in the given PolyPolygon. Anything else which might be thought of
                // for reducing to a single polygon will just need nore power and
                // cannot create more correct results.
                sal_uInt32 nPointCount(0L);
                sal_uInt16 a;

                for(a = 0; a < rPolyPoly.Count(); a++)
                {
                    nPointCount += (sal_uInt32)rPolyPoly[a].GetSize();
                }

                if(nPointCount > 0x0000ffff)
                {
                    DBG_ERROR("PolygonFromPolyPolygon: too many points for a single polygon (!)");
                    nPointCount = 0x0000ffff;
                }

                Polygon aRetval((sal_uInt16)nPointCount);
                sal_uInt32 nAppendIndex(0L);

                for(a = 0; a < rPolyPoly.Count(); a++)
                {
                    const Polygon& rCandidate = rPolyPoly[a];
                    
                    for(sal_uInt16 b(0); nAppendIndex <= nPointCount && b < rCandidate.GetSize(); b++)
                    {
                        aRetval[(sal_uInt16)nAppendIndex++] = rCandidate[b];
                    }
                }

                return aRetval;
            }
        }

        bool IsStarSymbol(const String &rFontName)
        {
            String sFamilyNm(GetFontToken(rFontName, 0));
            return (sFamilyNm.EqualsIgnoreCaseAscii("starsymbol") ||
                sFamilyNm.EqualsIgnoreCaseAscii("opensymbol"));
        }

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

        void RedlineStack::open(const SwPosition& rPos, const SfxPoolItem& rAttr)
        {
            ASSERT(rAttr.Which() == RES_FLTR_REDLINE, "not a redline");
            maStack.push_back(new SwFltStackEntry(rPos,rAttr.Clone()));
        }


        class SameOpenRedlineType :
            public std::unary_function<const SwFltStackEntry*, bool>
        {
        private:
            SwRedlineType meType;
        public:
            SameOpenRedlineType(SwRedlineType eType) : meType(eType) {}
            bool operator()(const SwFltStackEntry *pEntry) const
            {
                const SwFltRedline *pTest = static_cast<const SwFltRedline *>
                    (pEntry->pAttr);
                return (pEntry->bLocked && (pTest->eType == meType));
            }
        };

        void RedlineStack::close(const SwPosition& rPos, SwRedlineType eType)
        {
            //Search from end for same type
            myriter aResult = std::find_if(maStack.rbegin(), maStack.rend(),
                SameOpenRedlineType(eType));
            ASSERT(aResult != maStack.rend(), "close without open!");
            if (aResult != maStack.rend())
                (*aResult)->SetEndPos(rPos);
        }



        void RedlineStack::closeall(const SwPosition& rPos)
        {
            std::for_each(maStack.begin(), maStack.end(), CloseIfOpen(rPos));
        }


        void SetInDocAndDelete::operator()(SwFltStackEntry *pEntry)
        {
            SwPaM aRegion(pEntry->nMkNode);
            if (
                pEntry->MakeRegion(&mrDoc, aRegion, true) && 
                (*aRegion.GetPoint() != *aRegion.GetMark())
            )
            {
                mrDoc.SetRedlineMode(REDLINE_ON | REDLINE_SHOW_INSERT |
                    REDLINE_SHOW_DELETE);
                const SwFltRedline *pFltRedline = static_cast<const SwFltRedline*>
                    (pEntry->pAttr);

                if (USHRT_MAX != pFltRedline->nAutorNoPrev)
                {
                    SwRedlineData aData(pFltRedline->eTypePrev,
                        pFltRedline->nAutorNoPrev, pFltRedline->aStampPrev, aEmptyStr,
                        0);

                    mrDoc.AppendRedline(new SwRedline(aData, aRegion));
                }

                SwRedlineData aData(pFltRedline->eType, pFltRedline->nAutorNo,
                        pFltRedline->aStamp, aEmptyStr, 0);

                mrDoc.AppendRedline(new SwRedline(aData, aRegion));
                mrDoc.SetRedlineMode(REDLINE_NONE | REDLINE_SHOW_INSERT |
                    REDLINE_SHOW_DELETE );
            }
            delete pEntry;
        }


        bool CompareRedlines::operator()(const SwFltStackEntry *pOneE,
            const SwFltStackEntry *pTwoE) const
        {
            const SwFltRedline *pOne= static_cast<const SwFltRedline*>
                (pOneE->pAttr);
            const SwFltRedline *pTwo= static_cast<const SwFltRedline*>
                (pTwoE->pAttr);

            //Return the earlier time, if two have the same time, prioritize
            //inserts over deletes
            if (pOne->aStamp == pTwo->aStamp)
                return (pOne->eType == REDLINE_INSERT && pTwo->eType != REDLINE_INSERT);
            else
                return (pOne->aStamp < pTwo->aStamp) ? true : false;
        }


        RedlineStack::~RedlineStack()
        {
            std::sort(maStack.begin(), maStack.end(), CompareRedlines());
            std::for_each(maStack.begin(), maStack.end(), SetInDocAndDelete(mrDoc));
        }

        USHORT WrtRedlineAuthor::AddName( const String& rNm )
        {
            USHORT nRet;
            typedef std::vector<String>::iterator myiter;
            myiter aIter = std::find(maAuthors.begin(), maAuthors.end(), rNm);
            if (aIter != maAuthors.end())
                nRet = aIter - maAuthors.begin();
            else
            {
                nRet = maAuthors.size();
                maAuthors.push_back(rNm);
            }
            return nRet;
        }
/*
        std::vector<String> WrtRedlineAuthor::GetNames()
        {
            return maAuthors;
        }
*/
    }
}
/* vi:set tabstop=4 shiftwidth=4 expandtab: */
