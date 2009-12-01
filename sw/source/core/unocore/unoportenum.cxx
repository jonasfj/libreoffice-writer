/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unoportenum.cxx,v $
 * $Revision: 1.42 $
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


#include <unoport.hxx>
#include <IMark.hxx>
// --> OD 2007-10-23 #i81002#
#include <crossrefbookmark.hxx>
// <--
#include <doc.hxx>
#include <txatbase.hxx>
#include <txtatr.hxx>
#include <ndhints.hxx>
#include <ndtxt.hxx>
#include <unocrsr.hxx>
#include <docary.hxx>
#include <tox.hxx>
#include <unoclbck.hxx>
#include <unoobj.hxx>
#include <unoredline.hxx>
#include <unofield.hxx>
#include <unometa.hxx>
#include <fmtanchr.hxx>
#include <fmtrfmrk.hxx>
#include <unoidx.hxx>
#include <redline.hxx>
#include <crsskip.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <algorithm>
#include <stack>


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using ::rtl::OUString;
using namespace ::std;

typedef ::std::pair< TextRangeList_t * const, SwTxtAttr const * const > PortionList_t;
typedef ::std::stack< PortionList_t > PortionStack_t;

static void lcl_CreatePortions(
    TextRangeList_t & i_rPortions,
    uno::Reference< text::XText > const& i_xParentText,
    SwUnoCrsr* pUnoCrsr,
    FrameDependSortList_t & i_rFrames,
    const sal_Int32 i_nStartPos, const sal_Int32 i_nEndPos );


namespace
{
    static const BYTE BKM_TYPE_START = 0;
    static const BYTE BKM_TYPE_END = 1;
    static const BYTE BKM_TYPE_START_END = 2;

    struct SwXBookmarkPortion_Impl
    {
        Reference<XTextContent>		xBookmark;
        BYTE						nBkmType;
        const SwPosition 			aPosition;

        SwXBookmarkPortion_Impl( SwXBookmark* pXMark, BYTE nType, const SwPosition &rPosition )
        : xBookmark ( pXMark )
        , nBkmType  ( nType )
        , aPosition ( rPosition )
        {
        }
        ULONG getIndex ()
        {
            return aPosition.nContent.GetIndex();
        }
    };
    typedef boost::shared_ptr < SwXBookmarkPortion_Impl > SwXBookmarkPortion_ImplSharedPtr;
    struct BookmarkCompareStruct
    {
        bool operator () ( const SwXBookmarkPortion_ImplSharedPtr &r1,
                           const SwXBookmarkPortion_ImplSharedPtr &r2 )
        {
            // #i16896# for bookmark portions at the same position, the start should
            // always precede the end. Hence compare positions, and use bookmark type
            // as tie-breaker for same position.
            // return ( r1->nIndex   == r2->nIndex )
            //	 ? ( r1->nBkmType <  r2->nBkmType )
            //	 : ( r1->nIndex   <  r2->nIndex );

            // MTG: 25/11/05: Note that the above code does not correctly handle
            // the case when one bookmark ends, and another begins in the same
            // position. When this occurs, the above code will return the
            // the start of the 2nd bookmark BEFORE the end of the first bookmark
            // See bug #i58438# for more details. The below code is correct and
            // fixes both #i58438 and #i16896#
            return r1->aPosition < r2->aPosition;
        }
    };
    typedef std::multiset < SwXBookmarkPortion_ImplSharedPtr, BookmarkCompareStruct > SwXBookmarkPortion_ImplList;


    static void lcl_FillBookmarkArray(SwDoc& rDoc, SwUnoCrsr& rUnoCrsr, SwXBookmarkPortion_ImplList& rBkmArr)
    {
        IDocumentMarkAccess* const pMarkAccess = rDoc.getIDocumentMarkAccess();
        if(!pMarkAccess->getBookmarksCount())
            return;

        // no need to consider marks starting after aEndOfPara
        SwPosition aEndOfPara(*rUnoCrsr.GetPoint());
        aEndOfPara.nContent = aEndOfPara.nNode.GetNode().GetTxtNode()->Len();
        const IDocumentMarkAccess::const_iterator_t pCandidatesEnd = upper_bound(
            pMarkAccess->getBookmarksBegin(),
            pMarkAccess->getBookmarksEnd(),
            aEndOfPara,
            bind(&::sw::mark::IMark::StartsAfter, _2, _1)); // finds the first that starts after

        // search for all bookmarks that start or end in this paragraph
        const SwNodeIndex nOwnNode = rUnoCrsr.GetPoint()->nNode;
        for(IDocumentMarkAccess::const_iterator_t ppMark = pMarkAccess->getBookmarksBegin();
            ppMark != pCandidatesEnd;
            ++ppMark)
        {
            ::sw::mark::IMark* const pBkmk = ppMark->get();
            bool hasOther = pBkmk->IsExpanded();

            const SwPosition& rStartPos = pBkmk->GetMarkStart();
            if(rStartPos.nNode == nOwnNode)
            {
                const BYTE nType = hasOther ? BKM_TYPE_START : BKM_TYPE_START_END;
                rBkmArr.insert(SwXBookmarkPortion_ImplSharedPtr(
                    new SwXBookmarkPortion_Impl ( SwXBookmarks::GetObject(*pBkmk, &rDoc ), nType, rStartPos)));
            }

            const SwPosition& rEndPos = pBkmk->GetMarkEnd();
            if(rEndPos.nNode == nOwnNode)
            {
                auto_ptr<SwPosition> pCrossRefEndPos;
                const SwPosition* pEndPos = NULL;
                if(hasOther)
                    pEndPos = &rEndPos;
                else if(dynamic_cast< ::sw::mark::CrossRefBookmark*>(pBkmk))
                {
                    // Crossrefbookmarks only remember the start position but have to span the whole paragraph
                    pCrossRefEndPos = auto_ptr<SwPosition>(new SwPosition(rEndPos));
                    pCrossRefEndPos->nContent = pCrossRefEndPos->nNode.GetNode().GetTxtNode()->Len();
                    pEndPos = pCrossRefEndPos.get();
                }
                if(pEndPos)
                    rBkmArr.insert(SwXBookmarkPortion_ImplSharedPtr(
                        new SwXBookmarkPortion_Impl ( SwXBookmarks::GetObject(*pBkmk, &rDoc ), BKM_TYPE_END, *pEndPos)));
            }
        }
    }
}


/******************************************************************
 *	SwXTextPortionEnumeration
 ******************************************************************/
/* -----------------------------13.03.00 12:15--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXTextPortionEnumeration::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXTextPortionEnumeration::getSomething(
        const uno::Sequence< sal_Int8 >& rId )
throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >( this ) );
    }
    return 0;
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextPortionEnumeration::getImplementationName()
throw( RuntimeException )
{
    return C2U("SwXTextPortionEnumeration");
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool
SwXTextPortionEnumeration::supportsService(const OUString& rServiceName)
throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextPortionEnumeration") == rServiceName;
}
/* -----------------------------06.04.00 16:39--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextPortionEnumeration::getSupportedServiceNames()
throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextPortionEnumeration");
    return aRet;
}

/*-- 27.01.99 10:44:43---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextPortionEnumeration::SwXTextPortionEnumeration(
        SwPaM& rParaCrsr,
        uno::Reference< XText > const & xParentText,
        const sal_Int32 nStart,
        const sal_Int32 nEnd )
    : m_Portions()
{
    SwUnoCrsr* pUnoCrsr =
       rParaCrsr.GetDoc()->CreateUnoCrsr(*rParaCrsr.GetPoint(), sal_False);
    pUnoCrsr->Add(this);

    DBG_ASSERT(nEnd == -1 || (nStart <= nEnd &&
        nEnd <= pUnoCrsr->Start()->nNode.GetNode().GetTxtNode()->GetTxt().Len()),
            "start or end value invalid!");

    // find all frames, graphics and OLEs that are bound AT character in para
    FrameDependSortList_t frames;
    ::CollectFrameAtNode(*this, pUnoCrsr->GetPoint()->nNode, frames, true);
    lcl_CreatePortions(m_Portions, xParentText, pUnoCrsr, frames, nStart, nEnd);
}

SwXTextPortionEnumeration::SwXTextPortionEnumeration(
        SwPaM& rParaCrsr,
        TextRangeList_t const & rPortions )
    : m_Portions( rPortions )
{
    SwUnoCrsr* const pUnoCrsr =
       rParaCrsr.GetDoc()->CreateUnoCrsr(*rParaCrsr.GetPoint(), sal_False);
    pUnoCrsr->Add(this);
}

/*-- 27.01.99 10:44:44---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextPortionEnumeration::~SwXTextPortionEnumeration()
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    SwUnoCrsr* pUnoCrsr = GetCursor();
    delete pUnoCrsr;
}
/*-- 27.01.99 10:44:44---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextPortionEnumeration::hasMoreElements()
throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    return (m_Portions.size() > 0) ? sal_True : sal_False;
}
/*-- 27.01.99 10:44:45---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextPortionEnumeration::nextElement()
throw( container::NoSuchElementException, lang::WrappedTargetException,
       uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if (!m_Portions.size())
        throw container::NoSuchElementException();

    Any any;
    any <<= m_Portions.front();
    m_Portions.pop_front();
    return any;
}

//======================================================================

typedef ::std::deque< xub_StrLen > FieldMarks_t;

static void
lcl_FillFieldMarkArray(FieldMarks_t & rFieldMarks, SwUnoCrsr const & rUnoCrsr,
        const sal_Int32 i_nStartPos)
{
    const SwTxtNode * const pTxtNode =
        rUnoCrsr.GetPoint()->nNode.GetNode().GetTxtNode();
    if (!pTxtNode) return;

    const sal_Unicode fld[] = {
        CH_TXT_ATR_FIELDSTART, CH_TXT_ATR_FIELDEND, CH_TXT_ATR_FORMELEMENT, 0 };
    xub_StrLen pos = ::std::max(static_cast<const sal_Int32>(0), i_nStartPos);
    while ((pos = pTxtNode->GetTxt().SearchChar(fld, pos)) != STRING_NOTFOUND)
    {
        rFieldMarks.push_back(pos);
        ++pos;
    }
}

static uno::Reference<text::XTextRange>
lcl_ExportFieldMark(
        uno::Reference< text::XText > const & i_xParentText,
        SwUnoCrsr * const pUnoCrsr,
        const SwTxtNode * const pTxtNode )
{
    uno::Reference<text::XTextRange> xRef;
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    //flr: maybe its a good idea to add a special hint to the hints array and rely on the hint segmentation....
    const xub_StrLen start = pUnoCrsr->Start()->nContent.GetIndex();
    ASSERT(pUnoCrsr->End()->nContent.GetIndex() == start,
               "hmm --- why is this different");

    pUnoCrsr->Right(1, CRSR_SKIP_CHARS, FALSE, FALSE);
    if ( *pUnoCrsr->GetMark() == *pUnoCrsr->GetPoint() )
    {
        ASSERT(false, "cannot move cursor?");
        return 0;
    }

    const sal_Unicode Char = pTxtNode->GetTxt().GetChar(start);
    if (CH_TXT_ATR_FIELDSTART == Char)
    {
        ::sw::mark::IFieldmark* pFieldmark = NULL;
        if (pDoc)
        {
            pFieldmark = pDoc->getIDocumentMarkAccess()->
                getFieldmarkFor(*pUnoCrsr->GetMark());
        }
        SwXTextPortion* pPortion = new SwXTextPortion(
            pUnoCrsr, i_xParentText, PORTION_FIELD_START);
        xRef = pPortion;
        if (pPortion && pFieldmark && pDoc)
            pPortion->SetBookmark(new SwXFieldmark(false, pFieldmark, pDoc));
    }
    else if (CH_TXT_ATR_FIELDEND == Char)
    {
        ::sw::mark::IFieldmark* pFieldmark = NULL;
        if (pDoc)
        {
            pFieldmark = pDoc->getIDocumentMarkAccess()->
                getFieldmarkFor(*pUnoCrsr->GetMark());
        }
        SwXTextPortion* pPortion = new SwXTextPortion(
            pUnoCrsr, i_xParentText, PORTION_FIELD_END);
        xRef = pPortion;
        if (pPortion && pFieldmark && pDoc)
            pPortion->SetBookmark(new SwXFieldmark(false, pFieldmark, pDoc));
    }
    else if (CH_TXT_ATR_FORMELEMENT == Char)
    {
        ::sw::mark::IFieldmark* pFieldmark = NULL;
        if (pDoc)
        {
            pFieldmark = pDoc->getIDocumentMarkAccess()->
                getFieldmarkFor(*pUnoCrsr->GetMark());
        }
        SwXTextPortion* pPortion = new SwXTextPortion(
            pUnoCrsr, i_xParentText, PORTION_FIELD_START_END);
        xRef = pPortion;
        if (pPortion && pFieldmark && pDoc)
            pPortion->SetBookmark(new SwXFieldmark(true, pFieldmark, pDoc));
    }
    else
    {
        ASSERT(false, "no fieldmark found?");
    }
    return xRef;
}

/* -----------------------------31.08.00 14:28--------------------------------

 ---------------------------------------------------------------------------*/
static Reference<XTextRange>
lcl_CreateRefMarkPortion(
    Reference<XText> const& xParent,
    const SwUnoCrsr * const pUnoCrsr,
    const SwTxtAttr & rAttr, const bool bEnd)
{
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    const SwFmtRefMark& rRefMark =
        static_cast<const SwFmtRefMark&>(rAttr.GetAttr());
    Reference<XTextContent> xContent =
        static_cast<SwUnoCallBack*>(pDoc->GetUnoCallBack())
        ->GetRefMark(rRefMark);
    if (!xContent.is())
    {
        xContent = new SwXReferenceMark(pDoc, &rRefMark);
    }

    SwXTextPortion* pPortion = 0;
    if (!bEnd)
    {
        pPortion = new SwXTextPortion(pUnoCrsr, xParent, PORTION_REFMARK_START);
        pPortion->SetRefMark(xContent);
        pPortion->SetCollapsed(rAttr.GetEnd() ? false : true);
    }
    else
    {
        pPortion = new SwXTextPortion(pUnoCrsr, xParent, PORTION_REFMARK_END);
        pPortion->SetRefMark(xContent);
    }
    return pPortion;
}

//-----------------------------------------------------------------------------
static void
lcl_InsertRubyPortion(
    TextRangeList_t & rPortions,
    Reference<XText> const& xParent,
    const SwUnoCrsr * const pUnoCrsr,
    const SwTxtAttr & rAttr, const sal_Bool bEnd)
{
    SwXTextPortion* pPortion = new SwXTextPortion(pUnoCrsr,
            static_cast<const SwTxtRuby&>(rAttr), xParent, bEnd);
    rPortions.push_back(pPortion);
    pPortion->SetCollapsed(rAttr.GetEnd() ? false : true);
}

//-----------------------------------------------------------------------------
static Reference<XTextRange>
lcl_CreateTOXMarkPortion(
    Reference<XText> const& xParent,
    const SwUnoCrsr * const pUnoCrsr,
    const SwTxtAttr & rAttr, const bool bEnd)
{
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    const SwTOXMark& rTOXMark = static_cast<const SwTOXMark&>(rAttr.GetAttr());

    Reference<XTextContent> xContent =
        static_cast<SwUnoCallBack*>(pDoc->GetUnoCallBack())
        ->GetTOXMark(rTOXMark);
    if (!xContent.is())
    {
        xContent = new SwXDocumentIndexMark(rTOXMark.GetTOXType(),
                       &rTOXMark, pDoc);
    }

    SwXTextPortion* pPortion = 0;
    if (!bEnd)
    {
        pPortion = new SwXTextPortion(pUnoCrsr, xParent, PORTION_TOXMARK_START);
        pPortion->SetTOXMark(xContent);
        pPortion->SetCollapsed(rAttr.GetEnd() ? false : true);
    }
    else
    {
        pPortion = new SwXTextPortion(pUnoCrsr, xParent, PORTION_TOXMARK_END);
        pPortion->SetTOXMark(xContent);
    }
    return pPortion;
}

//-----------------------------------------------------------------------------
static void
lcl_ExportBookmark(
    TextRangeList_t & rPortions,
    Reference<XText> const& xParent,
    const SwUnoCrsr * const pUnoCrsr,
    SwXBookmarkPortion_ImplList& rBkmArr, const ULONG nIndex)
{
    for ( SwXBookmarkPortion_ImplList::iterator aIter = rBkmArr.begin(), aEnd = rBkmArr.end();
          aIter != aEnd; )
    {
        SwXBookmarkPortion_ImplSharedPtr pPtr = (*aIter);
        if ( nIndex > pPtr->getIndex() )
        {
            rBkmArr.erase( aIter++ );
            continue;
        }
        if ( nIndex < pPtr->getIndex() )
            break;

        SwXTextPortion* pPortion = 0;
        if ((BKM_TYPE_START     == pPtr->nBkmType) ||
            (BKM_TYPE_START_END == pPtr->nBkmType))
        {
            pPortion =
                new SwXTextPortion(pUnoCrsr, xParent, PORTION_BOOKMARK_START);
            rPortions.push_back(pPortion);
            pPortion->SetBookmark(pPtr->xBookmark);
            pPortion->SetCollapsed( (BKM_TYPE_START_END == pPtr->nBkmType)
                    ? true : false);

        }
        if (BKM_TYPE_END == pPtr->nBkmType)
        {
            pPortion =
                new SwXTextPortion(pUnoCrsr, xParent, PORTION_BOOKMARK_END);
            rPortions.push_back(pPortion);
            pPortion->SetBookmark(pPtr->xBookmark);
        }
        rBkmArr.erase( aIter++ );
    }
}

static void
lcl_ExportSoftPageBreak(
    TextRangeList_t & rPortions,
    Reference<XText> const& xParent,
    const SwUnoCrsr * const pUnoCrsr,
    SwSoftPageBreakList& rBreakArr, const ULONG nIndex)
{
    for ( SwSoftPageBreakList::iterator aIter = rBreakArr.begin(),
          aEnd = rBreakArr.end();
          aIter != aEnd; )
    {
        if ( nIndex > *aIter )
        {
            rBreakArr.erase( aIter++ );
            continue;
        }
        if ( nIndex < *aIter )
            break;

        rPortions.push_back(
            new SwXTextPortion(pUnoCrsr, xParent, PORTION_SOFT_PAGEBREAK) );
        rBreakArr.erase( aIter++ );
    }
}


/* -----------------------------18.12.00 14:51--------------------------------

 ---------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
#define REDLINE_PORTION_START_REMOVE 0//removed redlines are visible
#define REDLINE_PORTION_END_REMOVE   1//removed redlines are visible
#define REDLINE_PORTION_REMOVE       2//removed redlines are NOT visible
#define REDLINE_PORTION_INSERT_START 3
#define REDLINE_PORTION_INSERT_END   4

struct SwXRedlinePortion_Impl
{
    const SwRedline*    m_pRedline;
    const bool          m_bStart;

    SwXRedlinePortion_Impl ( const SwRedline* pRed, const bool bIsStart )
    : m_pRedline(pRed)
    , m_bStart(bIsStart)
    {
    }

    ULONG getRealIndex ()
    {
        return m_bStart ? m_pRedline->Start()->nContent.GetIndex()
                        : m_pRedline->End()  ->nContent.GetIndex();
    }
};

typedef boost::shared_ptr < SwXRedlinePortion_Impl >
    SwXRedlinePortion_ImplSharedPtr;

struct RedlineCompareStruct
{
    const SwPosition& getPosition ( const SwXRedlinePortion_ImplSharedPtr &r )
    {
        return *(r->m_bStart ? r->m_pRedline->Start() : r->m_pRedline->End());
    }

    bool operator () ( const SwXRedlinePortion_ImplSharedPtr &r1,
                       const SwXRedlinePortion_ImplSharedPtr &r2 )
    {
        return getPosition ( r1 ) < getPosition ( r2 );
    }
};

typedef std::multiset < SwXRedlinePortion_ImplSharedPtr, RedlineCompareStruct >
SwXRedlinePortion_ImplList;

//-----------------------------------------------------------------------------
static Reference<XTextRange>
lcl_ExportHints(
    PortionStack_t & rPortionStack,
    const Reference<XText> & xParent,
    SwUnoCrsr * const pUnoCrsr,
    SwpHints * const pHints,
    const sal_Int32 i_nStartPos,
    const sal_Int32 i_nEndPos,
    const xub_StrLen nCurrentIndex,
    const bool bRightMoveForbidden,
    bool & o_rbCursorMoved,
    sal_Int32 & o_rNextAttrPosition )
{
    // if the attribute has a dummy character, then xRef is set (except META)
    // otherwise, the portion for the attribute is inserted into rPortions!
    Reference<XTextRange> xRef;
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    //search for special text attributes - first some ends
    sal_uInt16 nEndIndex = 0;
    sal_uInt16 nNextEnd = 0;
    while(nEndIndex < pHints->GetEndCount() &&
        (!pHints->GetEnd(nEndIndex)->GetEnd() ||
        nCurrentIndex >= (nNextEnd = (*pHints->GetEnd(nEndIndex)->GetEnd()))))
    {
        if(pHints->GetEnd(nEndIndex)->GetEnd())
        {
            SwTxtAttr * const pAttr = pHints->GetEnd(nEndIndex);
            if (nNextEnd == nCurrentIndex)
            {
                const USHORT nWhich( pAttr->Which() );
                switch (nWhich)
                {
                    case RES_TXTATR_TOXMARK:
                    {
                        Reference<XTextRange> xTmp = lcl_CreateTOXMarkPortion(
                                xParent, pUnoCrsr, *pAttr, true);
                        rPortionStack.top().first->push_back(xTmp);
                    }
                    break;
                    case RES_TXTATR_REFMARK:
                    {
                        Reference<XTextRange> xTmp = lcl_CreateRefMarkPortion(
                                xParent, pUnoCrsr, *pAttr, true);
                        rPortionStack.top().first->push_back(xTmp);
                    }
                    break;
                    case RES_TXTATR_CJK_RUBY:
                       //#i91534# GetEnd() == 0 mixes the order of ruby start/end
                        if( *pAttr->GetEnd() == *pAttr->GetStart())
                        {
                            lcl_InsertRubyPortion( *rPortionStack.top().first,
                                    xParent, pUnoCrsr, *pAttr, sal_False);
                        }
                        lcl_InsertRubyPortion( *rPortionStack.top().first,
                                xParent, pUnoCrsr, *pAttr, sal_True);
                    break;
                    case RES_TXTATR_META:
                    case RES_TXTATR_METAFIELD:
                    {
                        ASSERT(*pAttr->GetStart() != *pAttr->GetEnd(),
                                "empty meta?");
                        if ((i_nStartPos > 0) &&
                            (*pAttr->GetStart() < i_nStartPos))
                        {
                            // force skip pAttr and rest of attribute ends
                            // at nCurrentIndex
                            // because they are not contained in the meta pAttr
                            // and the meta pAttr itself is outside selection!
                            // (necessary for SwXMeta::createEnumeration)
                            if (*pAttr->GetStart() + 1 == i_nStartPos)
                            {
                                nEndIndex = pHints->GetEndCount() - 1;
                            }
                            break;
                        }
                        PortionList_t Top = rPortionStack.top();
                        if (Top.second != pAttr)
                        {
                            ASSERT(false, "ExportHints: stack error" );
                        }
                        else
                        {
                            TextRangeList_t *const pCurrentPortions(Top.first);
                            rPortionStack.pop();
                            SwXTextPortion * pPortion;
                            if (RES_TXTATR_META == nWhich)
                            {
                                SwXMeta * const pMeta =
                                    new SwXMeta(pDoc, xParent,
                                        pCurrentPortions,
                                        static_cast<SwTxtMeta * const>(pAttr));
                                pPortion = new SwXTextPortion(
                                    pUnoCrsr, xParent, PORTION_META);
                                pPortion->SetMeta(pMeta);
                            }
                            else
                            {
                                SwXMetaField * const pMeta =
                                    new SwXMetaField(pDoc, xParent,
                                        pCurrentPortions,
                                        static_cast<SwTxtMeta * const>(pAttr));
                                pPortion = new SwXTextPortion(
                                    pUnoCrsr, xParent, PORTION_FIELD);
                                pPortion->SetTextField(pMeta);
                            }
                            rPortionStack.top().first->push_back(pPortion);
                        }
                    }
                    break;
                }
            }
        }
        nEndIndex++;
    }

    // then some starts
    sal_uInt16 nStartIndex = 0;
    sal_uInt16 nNextStart = 0;
    while(nStartIndex < pHints->GetStartCount() &&
        nCurrentIndex >= (nNextStart = (*pHints->GetStart(nStartIndex)->GetStart())))
    {
        const SwTxtAttr * const pAttr = pHints->GetStart(nStartIndex);
        USHORT nAttrWhich = pAttr->Which();
        if (nNextStart == nCurrentIndex)
        {
            switch( nAttrWhich )
            {
                case RES_TXTATR_FIELD:
                    if(!bRightMoveForbidden)
                    {
                        pUnoCrsr->Right(1,CRSR_SKIP_CHARS,FALSE,FALSE);
                        if( *pUnoCrsr->GetMark() == *pUnoCrsr->GetPoint() )
                            break;
                        SwXTextPortion* pPortion;
                        xRef = pPortion = new SwXTextPortion(
                                pUnoCrsr, xParent, PORTION_FIELD);
                        Reference<XTextField> xField =
                            CreateSwXTextField(*pDoc, pAttr->GetFld());
                        pPortion->SetTextField(xField);
                    }
                break;
                case RES_TXTATR_FLYCNT	 :
                    if(!bRightMoveForbidden)
                    {
                        pUnoCrsr->Right(1,CRSR_SKIP_CHARS,FALSE,FALSE);
                        if( *pUnoCrsr->GetMark() == *pUnoCrsr->GetPoint() )
                            break; // Robust #i81708 content in covered cells
                        pUnoCrsr->Exchange();
                        xRef = new SwXTextPortion(
                                pUnoCrsr, xParent, PORTION_FRAME);
                    }
                break;
                case RES_TXTATR_FTN 	 :
                {
                    if(!bRightMoveForbidden)
                    {
                        pUnoCrsr->Right(1,CRSR_SKIP_CHARS,FALSE,FALSE);
                        if( *pUnoCrsr->GetMark() == *pUnoCrsr->GetPoint() )
                            break;
                        SwXTextPortion* pPortion;
                        xRef = pPortion = new SwXTextPortion(
                                pUnoCrsr, xParent, PORTION_FOOTNOTE);
                        Reference<XFootnote> xContent =
                            SwXFootnotes::GetObject(*pDoc, pAttr->GetFtn());
                        pPortion->SetFootnote(xContent);
                    }
                }
                break;
                case RES_TXTATR_TOXMARK:
                case RES_TXTATR_REFMARK:
                {
                    bool bIsPoint = !(pAttr->GetEnd());
                    if (!bRightMoveForbidden || !bIsPoint)
                    {
                        if (bIsPoint)
                        {
                            pUnoCrsr->Right(1,CRSR_SKIP_CHARS,FALSE,FALSE);
                        }
                        Reference<XTextRange> xTmp =
                                (RES_TXTATR_REFMARK == nAttrWhich)
                            ? lcl_CreateRefMarkPortion(
                                xParent, pUnoCrsr, *pAttr, false)
                            : lcl_CreateTOXMarkPortion(
                                xParent, pUnoCrsr, *pAttr, false);
                        if (bIsPoint) // consume CH_TXTATR!
                        {
                            pUnoCrsr->Normalize(FALSE);
                            pUnoCrsr->DeleteMark();
                            xRef = xTmp;
                        }
                        else // just insert it
                        {
                            rPortionStack.top().first->push_back(xTmp);
                        }
                    }
                }
                break;
                case RES_TXTATR_CJK_RUBY:
                    //#i91534# GetEnd() == 0 mixes the order of ruby start/end
                    if(pAttr->GetEnd() && (*pAttr->GetEnd() != *pAttr->GetStart()))
                    {
                        lcl_InsertRubyPortion( *rPortionStack.top().first,
                            xParent, pUnoCrsr, *pAttr, sal_False);
                    }
                break;
                case RES_TXTATR_META:
                case RES_TXTATR_METAFIELD:
                    if (*pAttr->GetStart() != *pAttr->GetEnd())
                    {
                        if (!bRightMoveForbidden)
                        {
                            pUnoCrsr->Right(1,CRSR_SKIP_CHARS,FALSE,FALSE);
                            o_rbCursorMoved = true;
                            // only if the end is included in selection!
                            if ((i_nEndPos < 0) ||
                                (*pAttr->GetEnd() <= i_nEndPos))
                            {
                                rPortionStack.push( ::std::make_pair(
                                        new TextRangeList_t, pAttr ));
                            }
                        }
                    }
                break;
                case RES_TXTATR_AUTOFMT:
                case RES_TXTATR_INETFMT:
                case RES_TXTATR_CHARFMT:
                break; // these are handled as properties of a "Text" portion
                default:
                    DBG_ERROR("unknown attribute");
                break;
            }
        }
        nStartIndex++;
    }

    if (xRef.is()) // implies that we have moved the cursor
    {
        o_rbCursorMoved = true;
    }
    if (!o_rbCursorMoved)
    {
        // search for attribute changes behind the current cursor position
        // break up at frames, bookmarks, redlines

        nStartIndex = 0;
        nNextStart = 0;
        while(nStartIndex < pHints->GetStartCount() &&
            nCurrentIndex >= (nNextStart = (*pHints->GetStart(nStartIndex)->GetStart())))
            nStartIndex++;

        nEndIndex = 0;
        nNextEnd = 0;
        while(nEndIndex < pHints->GetEndCount() &&
            nCurrentIndex >= (nNextEnd = (*pHints->GetEnd(nEndIndex)->GetAnyEnd())))
            nEndIndex++;

        sal_Int32 nNextPos =
            ((nNextStart > nCurrentIndex) && (nNextStart < nNextEnd))
            ?   nNextStart  :   nNextEnd;
        if (nNextPos > nCurrentIndex)
        {
            o_rNextAttrPosition = nNextPos;
        }
    }
    return xRef;
}

//-----------------------------------------------------------------------------
void lcl_MoveCursor( SwUnoCrsr * const pUnoCrsr,
    const xub_StrLen nCurrentIndex,
    const sal_Int32 nNextFrameIndex, const sal_Int32 nNextPortionIndex,
    const sal_Int32 nNextAttrIndex,  const sal_Int32 nNextFieldMarkIndex,
    const sal_Int32 nEndPos )
{
    sal_Int32 nMovePos = pUnoCrsr->GetCntntNode()->Len();

    if ((nEndPos >= 0) && (nEndPos < nMovePos))
    {
        nMovePos = nEndPos;
    }

    if ((nNextFrameIndex >= 0) && (nNextFrameIndex < nMovePos))
    {
        nMovePos = nNextFrameIndex;
    }

    if ((nNextPortionIndex >= 0) && (nNextPortionIndex < nMovePos))
    {
        nMovePos = nNextPortionIndex;
    }

    if ((nNextAttrIndex >= 0) && (nNextAttrIndex < nMovePos))
    {
        nMovePos = nNextAttrIndex;
    }

    if ((nNextFieldMarkIndex >= 0) && (nNextFieldMarkIndex < nMovePos))
    {
        nMovePos = nNextFieldMarkIndex;
    }

    if (nMovePos > nCurrentIndex)
    {
//			pUnoCrsr->Right(nMovePos - nCurrentIndex);
        pUnoCrsr->GetPoint()->nContent = static_cast<USHORT>(nMovePos);
    }
}

//-----------------------------------------------------------------------------
static void
lcl_FillRedlineArray(SwDoc const & rDoc, SwUnoCrsr const & rUnoCrsr,
        SwXRedlinePortion_ImplList& rRedArr )
{
    const SwRedlineTbl& rRedTbl = rDoc.GetRedlineTbl();
    USHORT nRedTblCount = rRedTbl.Count();

    if ( nRedTblCount > 0 )
    {
        const SwPosition* pStart = rUnoCrsr.GetPoint();
        const SwNodeIndex nOwnNode = pStart->nNode;

        for(USHORT nRed = 0; nRed < nRedTblCount; nRed++)
        {
            const SwRedline* pRedline = rRedTbl[nRed];
            const SwPosition* pRedStart = pRedline->Start();
            const SwNodeIndex nRedNode = pRedStart->nNode;
            if ( nOwnNode == nRedNode )
                rRedArr.insert( SwXRedlinePortion_ImplSharedPtr (
                    new SwXRedlinePortion_Impl ( pRedline, true ) ) );
            if( pRedline->HasMark() && pRedline->End()->nNode == nOwnNode )
                rRedArr.insert( SwXRedlinePortion_ImplSharedPtr (
                    new SwXRedlinePortion_Impl ( pRedline, false) ) );
       }
    }
}

//-----------------------------------------------------------------------------
static void
lcl_FillSoftPageBreakArray(
        SwUnoCrsr const & rUnoCrsr, SwSoftPageBreakList& rBreakArr )
{
    const SwTxtNode *pTxtNode =
        rUnoCrsr.GetPoint()->nNode.GetNode().GetTxtNode();
    if( pTxtNode )
        pTxtNode->fillSoftPageBreakList( rBreakArr );
}

/* -----------------------------19.12.00 12:25--------------------------------

 ---------------------------------------------------------------------------*/
static void
lcl_ExportRedline(
    TextRangeList_t & rPortions,
    Reference<XText> const& xParent,
    const SwUnoCrsr * const pUnoCrsr,
    SwXRedlinePortion_ImplList& rRedlineArr, const ULONG nIndex)
{

    // MTG: 23/11/05: We want this loop to iterate over all red lines in this
    // array. We will only insert the ones with index matches
    for ( SwXRedlinePortion_ImplList::iterator aIter = rRedlineArr.begin(), aEnd = rRedlineArr.end();
          aIter != aEnd; )
    {
        SwXRedlinePortion_ImplSharedPtr pPtr = (*aIter );
        ULONG nRealIndex = pPtr->getRealIndex();
        // MTG: 23/11/05: If there are elements before nIndex, remove them
        if ( nIndex > nRealIndex )
            rRedlineArr.erase ( aIter++ );
        // MTG: 23/11/05: If the elements match, and them to the list
        else if ( nIndex == nRealIndex )
        {
            rPortions.push_back( new SwXRedlinePortion(
                        pPtr->m_pRedline, pUnoCrsr, xParent, pPtr->m_bStart) );
            rRedlineArr.erase ( aIter++ );
        }
        // MTG: 23/11/05: If we've iterated past nIndex, exit the loop
        else
            break;
    }
}

/* -----------------------------19.12.00 13:09--------------------------------

 ---------------------------------------------------------------------------*/
static void
lcl_ExportBkmAndRedline(
    TextRangeList_t & rPortions,
    Reference<XText> const & xParent,
    const SwUnoCrsr * const pUnoCrsr,
    SwXBookmarkPortion_ImplList& rBkmArr,
    SwXRedlinePortion_ImplList& rRedlineArr,
    SwSoftPageBreakList& rBreakArr,
    const ULONG nIndex)
{
    if (rBkmArr.size())
        lcl_ExportBookmark(rPortions, xParent, pUnoCrsr, rBkmArr, nIndex);

    if (rRedlineArr.size())
        lcl_ExportRedline(rPortions, xParent, pUnoCrsr, rRedlineArr, nIndex);

    if (rBreakArr.size())
        lcl_ExportSoftPageBreak(rPortions, xParent, pUnoCrsr, rBreakArr, nIndex);
}

//-----------------------------------------------------------------------------
static sal_Int32
lcl_ExportFrames(
    TextRangeList_t & rPortions,
    Reference<XText> const & i_xParent,
    SwUnoCrsr * const i_pUnoCrsr,
    FrameDependSortList_t & i_rFrames,
    xub_StrLen const i_nCurrentIndex)
{
    // find first Frame in (sorted) i_rFrames at current position
    while (i_rFrames.size() && (i_rFrames.front().nIndex == i_nCurrentIndex))
    // do not check for i_nEnd here; this is done implicity by lcl_MoveCursor
    {
        const SwModify * const pFrame =
            i_rFrames.front().pFrameDepend->GetRegisteredIn();
        if (pFrame) // Frame could be disposed
        {
            SwXTextPortion* pPortion = new SwXTextPortion(i_pUnoCrsr, i_xParent,
                *static_cast<SwFrmFmt*>( const_cast<SwModify*>( pFrame ) ) );
            rPortions.push_back(pPortion);
        }
        i_rFrames.pop_front();
    }

    return i_rFrames.size() ? i_rFrames.front().nIndex : -1;
}

//-----------------------------------------------------------------------------
static sal_Int32
lcl_GetNextIndex(
    SwXBookmarkPortion_ImplList const & rBkmArr,
    SwXRedlinePortion_ImplList const & rRedlineArr,
    SwSoftPageBreakList const & rBreakArr )
{
    sal_Int32 nRet = -1;
    if(rBkmArr.size())
    {
        SwXBookmarkPortion_ImplSharedPtr pPtr = (*rBkmArr.begin());
        nRet = pPtr->getIndex();
    }
    if(rRedlineArr.size())
    {
        SwXRedlinePortion_ImplSharedPtr pPtr = (*rRedlineArr.begin());
        sal_Int32 nTmp = pPtr->getRealIndex();
        if(nRet < 0 || nTmp < nRet)
            nRet = nTmp;
    }
    if(rBreakArr.size())
    {
        if(nRet < 0 || *rBreakArr.begin() < static_cast<sal_uInt32>(nRet))
            nRet = *rBreakArr.begin();
    }
    return nRet;
};

//-----------------------------------------------------------------------------
static void
lcl_CreatePortions(
        TextRangeList_t & i_rPortions,
        uno::Reference< text::XText > const & i_xParentText,
        SwUnoCrsr * const pUnoCrsr,
        FrameDependSortList_t & i_rFrames,
        const sal_Int32 i_nStartPos,
        const sal_Int32 i_nEndPos )
{
    if (!pUnoCrsr)
        return;

    // set the start if a selection should be exported
    if ((i_nStartPos > 0) &&
        (pUnoCrsr->Start()->nContent.GetIndex() != i_nStartPos))
    {
        pUnoCrsr->DeleteMark();
        DBG_ASSERT(pUnoCrsr->Start()->nNode.GetNode().GetTxtNode() &&
            (i_nStartPos <= pUnoCrsr->Start()->nNode.GetNode().GetTxtNode()->
                                GetTxt().Len()), "Incorrect start position" );
        // ??? should this be i_nStartPos - current position ?
        pUnoCrsr->Right(static_cast<xub_StrLen>(i_nStartPos),
                CRSR_SKIP_CHARS, FALSE, FALSE);
    }

    FieldMarks_t FieldMarks;
    SwXBookmarkPortion_ImplList Bookmarks;
    SwXRedlinePortion_ImplList Redlines;
    SwSoftPageBreakList SoftPageBreaks;

    SwDoc * const pDoc = pUnoCrsr->GetDoc();
    lcl_FillFieldMarkArray(FieldMarks, *pUnoCrsr, i_nStartPos);
    lcl_FillBookmarkArray(*pDoc, *pUnoCrsr, Bookmarks);
    lcl_FillRedlineArray(*pDoc, *pUnoCrsr, Redlines);
    lcl_FillSoftPageBreakArray(*pUnoCrsr, SoftPageBreaks);

    PortionStack_t PortionStack;
    PortionStack.push( PortionList_t(&i_rPortions, 0) );

    bool bAtEnd( false );
    while (!bAtEnd) // every iteration consumes at least current character!
    {
        if (pUnoCrsr->HasMark())
        {
            pUnoCrsr->Normalize(FALSE);
            pUnoCrsr->DeleteMark();
        }

        SwTxtNode * const pTxtNode = pUnoCrsr->GetNode()->GetTxtNode();
        if (!pTxtNode)
        {
            DBG_ERROR("lcl_CreatePortions: no TextNode - what now ?");
            return;
        }

        SwpHints * const pHints = pTxtNode->GetpSwpHints();
        const xub_StrLen nCurrentIndex =
            pUnoCrsr->GetPoint()->nContent.GetIndex();
        // this contains the portion which consumes the character in the
        // text at nCurrentIndex; i.e. it must be set _once_ per iteration
        uno::Reference< XTextRange > xRef;

        SwXTextCursor::SelectPam(*pUnoCrsr, sal_True); // set mark

        const sal_Int32 nFirstFrameIndex =
            lcl_ExportFrames( *PortionStack.top().first,
                i_xParentText, pUnoCrsr, i_rFrames, nCurrentIndex);

        lcl_ExportBkmAndRedline( *PortionStack.top().first, i_xParentText,
            pUnoCrsr, Bookmarks, Redlines, SoftPageBreaks, nCurrentIndex );

        bool bCursorMoved( false );
        sal_Int32 nNextAttrIndex = -1;
        // #111716# the cursor must not move right at the
        //          end position of a selection!
        bAtEnd = ((i_nEndPos >= 0) && (nCurrentIndex >= i_nEndPos))
              || (nCurrentIndex >= pTxtNode->Len());
        if (pHints)
        {
            // N.B.: side-effects nNextAttrIndex, bCursorMoved; may move cursor
            xRef = lcl_ExportHints(PortionStack, i_xParentText, pUnoCrsr,
                        pHints, i_nStartPos, i_nEndPos, nCurrentIndex, bAtEnd,
                        bCursorMoved, nNextAttrIndex);
            if (PortionStack.empty())
            {
                ASSERT(false, "CreatePortions: stack underflow");
                return;
            }
        }

        if (!xRef.is() && !bCursorMoved)
        {
            if (!bAtEnd &&
                FieldMarks.size() && (FieldMarks.front() == nCurrentIndex))
            {
                // moves cursor
                xRef = lcl_ExportFieldMark(i_xParentText, pUnoCrsr, pTxtNode);
                FieldMarks.pop_front();
            }
        }
        else
        {
            ASSERT(!FieldMarks.size() ||
                   (FieldMarks.front() != nCurrentIndex),
                   "fieldmark and hint with CH_TXTATR at same pos?");
        }

        if (!bAtEnd && !xRef.is() && !bCursorMoved)
        {
            const sal_Int32 nNextPortionIndex =
                lcl_GetNextIndex(Bookmarks, Redlines, SoftPageBreaks);
            const sal_Int32 nNextFieldMarkIndex(
                    FieldMarks.size() ? FieldMarks.front() : -1);

            lcl_MoveCursor(pUnoCrsr, nCurrentIndex,
                nFirstFrameIndex, nNextPortionIndex, nNextAttrIndex,
                nNextFieldMarkIndex,
                i_nEndPos);

            xRef = new SwXTextPortion(pUnoCrsr, i_xParentText, PORTION_TEXT);
        }
        else if (bAtEnd && !xRef.is() && !pTxtNode->Len())
        {
            // special case: for an empty paragraph, we better put out a
            // text portion because there may be a hyperlink attribute
            xRef = new SwXTextPortion(pUnoCrsr, i_xParentText, PORTION_TEXT);
        }

        if (xRef.is())
        {
            PortionStack.top().first->push_back(xRef);
        }
    }

    ASSERT((PortionStack.size() == 1) && !PortionStack.top().second,
            "CreatePortions: stack error" );
}

/*-- 27.01.99 10:44:45---------------------------------------------------

  -----------------------------------------------------------------------*/
void 	SwXTextPortionEnumeration::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
}

