/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: thints.cxx,v $
 * $Revision: 1.65.62.1 $
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


#include <hintids.hxx>
#include <sot/factory.hxx>
#include <svx/xmlcnitm.hxx>
#include <svl/whiter.hxx>
#include <svl/itemiter.hxx>
#include <svl/stylepool.hxx>
#include <svx/fontitem.hxx>
#include <svx/langitem.hxx>
#include <svx/emphitem.hxx>
#include <svx/charscaleitem.hxx>
#include <svx/charrotateitem.hxx>
// --> OD 2008-01-16 #newlistlevelattrs#
#include <svx/lrspitem.hxx>
// <--
#include <txtinet.hxx>
#include <txtflcnt.hxx>
#include <fmtfld.hxx>
#include <fmtanchr.hxx>
#include <fmtinfmt.hxx>
#include <txtatr.hxx>
#include <fchrfmt.hxx>
#include <fmtautofmt.hxx>
#include <fmtflcnt.hxx>
#include <fmtftn.hxx>
#include <txttxmrk.hxx>
#include <txtrfmrk.hxx>
#include <txtftn.hxx>
#include <txtfld.hxx>
#include <charatr.hxx>
#include <charfmt.hxx>
#include <frmfmt.hxx>
#include <ftnidx.hxx>
#include <fmtruby.hxx>
#include <fmtmeta.hxx>
#include <breakit.hxx>
#include <doc.hxx>
#include <errhdl.hxx>
#include <fldbas.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <txtfrm.hxx>
#include <rolbck.hxx>			// fuer	SwRegHistory
#include <ddefld.hxx>
#include <docufld.hxx>
#include <expfld.hxx>
#include <usrfld.hxx>
#include <poolfmt.hxx>
#include <swfont.hxx>
#include <istyleaccess.hxx>
// OD 26.06.2003 #108784#
#include <dcontact.hxx>
#include <docsh.hxx>
#include <svl/smplhint.hxx>
#include <algorithm>
#include <map>

#ifdef DBG_UTIL
#define CHECK    Check();
#else
#define CHECK
#endif

using namespace ::com::sun::star::i18n;


SwpHints::SwpHints()
    : m_pHistory(0)
    , m_bFontChange(true)
    , m_bInSplitNode(false)
    , m_bCalcHiddenParaField(false)
    , m_bHasHiddenParaField(false)
    , m_bFootnote(false)
    , m_bDDEFields(false)
{
}

struct TxtAttrDeleter
{
    SwAttrPool & m_rPool;
    TxtAttrDeleter( SwDoc & rDoc ) : m_rPool( rDoc.GetAttrPool() ) { }
    void operator() (SwTxtAttr * const pAttr)
    {
        if (RES_TXTATR_META == pAttr->Which() ||
            RES_TXTATR_METAFIELD == pAttr->Which())
        {
            static_cast<SwTxtMeta *>(pAttr)->ChgTxtNode(0); // prevents ASSERT
        }
        SwTxtAttr::Destroy( pAttr, m_rPool );
    }
};

struct TxtAttrContains
{
    xub_StrLen m_nPos;
    TxtAttrContains( const xub_StrLen nPos ) : m_nPos( nPos ) { }
    bool operator() (SwTxtAttrEnd * const pAttr)
    {
        return (*pAttr->GetStart() < m_nPos) && (m_nPos < *pAttr->GetEnd());
    }
};

// a:       |-----|
// b:
//    |---|               => valid: b before a
//    |-----|             => valid: start == end; b before a
//    |---------|         => invalid: overlap (1)
//    |-----------|       => valid: same end; b around a
//    |-----------------| => valid: b around a
//          |---|         => valid; same start; b within a
//          |-----|       => valid; same start and end; b around or within a?
//          |-----------| => valid: same start: b around a
//            |-|         => valid: b within a
//            |---|       => valid: same end; b within a
//            |---------| => invalid: overlap (2)
//                |-----| => valid: end == start; b after a
//                  |---| => valid: b after a
// ===> 2 invalid overlap cases
static
bool isOverlap(const xub_StrLen nStart1, const xub_StrLen nEnd1,
               const xub_StrLen nStart2, const xub_StrLen nEnd2)
{
    return
        ((nStart1 > nStart2) && (nStart1 < nEnd2) && (nEnd1 > nEnd2))  // (1)
     || ((nStart1 < nStart2) && (nStart2 < nEnd1) && (nEnd1 < nEnd2)); // (2)
}

/// #i106930#: now asymmetric: empty hint1 is _not_ nested, but empty hint2 is
static
bool isNestedAny(const xub_StrLen nStart1, const xub_StrLen nEnd1,
                 const xub_StrLen nStart2, const xub_StrLen nEnd2)
{
    return ((nStart1 == nStart2) || (nEnd1 == nEnd2))
        // same start/end: nested except if hint1 empty and hint2 not empty
        ? (nStart1 != nEnd1) || (nStart2 == nEnd2)
        : ((nStart1 < nStart2) ? (nEnd1 >= nEnd2) : (nEnd1 <= nEnd2));
}

static
bool isSelfNestable(const USHORT nWhich)
{
    if ((RES_TXTATR_INETFMT  == nWhich) ||
        (RES_TXTATR_CJK_RUBY == nWhich))
        return false;
    ASSERT((RES_TXTATR_META  == nWhich) ||
           (RES_TXTATR_METAFIELD  == nWhich), "???");
    return true;
}

static
bool isSplittable(const USHORT nWhich)
{
    if ((RES_TXTATR_INETFMT  == nWhich) ||
        (RES_TXTATR_CJK_RUBY == nWhich))
        return true;
    ASSERT((RES_TXTATR_META  == nWhich) ||
           (RES_TXTATR_METAFIELD  == nWhich), "???");
    return false;
}

enum Split_t { FAIL, SPLIT_NEW, SPLIT_OTHER };
/**
  Calculate splitting policy for overlapping hints, based on what kind of
  hint is inserted, and what kind of existing hint overlaps.
  */
static Split_t
splitPolicy(const USHORT nWhichNew, const USHORT nWhichOther)
{
    if (!isSplittable(nWhichOther))
    {
        if (!isSplittable(nWhichNew))
            return FAIL;
        else
            return SPLIT_NEW;
    }
    else
    {
        if ((RES_TXTATR_INETFMT  == nWhichNew) &&
            (RES_TXTATR_CJK_RUBY == nWhichOther))
            return SPLIT_NEW;
        else
            return SPLIT_OTHER;
    }
}

static void
lcl_InitINetFmt(SwTxtNode & rNode, SwTxtINetFmt * pNew)
{
    pNew->ChgTxtNode(&rNode);
    SwCharFmt * const pFmt(
         rNode.GetDoc()->GetCharFmtFromPool(RES_POOLCHR_INET_NORMAL) );
    pFmt->Add( pNew );
}

static void
lcl_InitRuby(SwTxtNode & rNode, SwTxtRuby * pNew)
{
    pNew->ChgTxtNode(&rNode);
    SwCharFmt * const pFmt(
        rNode.GetDoc()->GetCharFmtFromPool(RES_POOLCHR_RUBYTEXT) );
    pFmt->Add( pNew );
}

/**
  Create a new nesting text hint.
 */
static SwTxtAttrNesting *
MakeTxtAttrNesting(SwTxtNode & rNode, SwTxtAttrNesting & rNesting,
        const xub_StrLen nStart, const xub_StrLen nEnd)
{
    SwTxtAttr * const pNew( MakeTxtAttr(
            *rNode.GetDoc(), rNesting.GetAttr(), nStart, nEnd ) );
    switch (pNew->Which())
    {
        case RES_TXTATR_INETFMT:
        {
            lcl_InitINetFmt(rNode, static_cast<SwTxtINetFmt*>(pNew));
            break;
        }
        case RES_TXTATR_CJK_RUBY:
        {
            lcl_InitRuby(rNode, static_cast<SwTxtRuby*>(pNew));
            break;
        }
        default:
            ASSERT(false, "MakeTxtAttrNesting: what the hell is that?");
            break;
    }
    return static_cast<SwTxtAttrNesting*>(pNew);
}

typedef ::std::vector<SwTxtAttrNesting *> NestList_t;

static void
lcl_DoSplitNew(NestList_t & rSplits, SwTxtNode & rNode,
    const xub_StrLen nNewStart,
    const xub_StrLen nOtherStart, const xub_StrLen nOtherEnd, bool bOtherDummy)
{
    const bool bSplitAtStart(nNewStart < nOtherStart);
    const xub_StrLen nSplitPos( (bSplitAtStart) ? nOtherStart : nOtherEnd );
    // first find the portion that is split (not necessarily the last one!)
    NestList_t::iterator const iter(
        ::std::find_if( rSplits.begin(), rSplits.end(),
            TxtAttrContains(nSplitPos) ) );
    if (iter != rSplits.end()) // already split here?
    {
        const xub_StrLen nStartPos( // skip other's dummy character!
            (bSplitAtStart && bOtherDummy) ? nSplitPos + 1 : nSplitPos );
        SwTxtAttrNesting * const pNew( MakeTxtAttrNesting(
                rNode, **iter, nStartPos, *(*iter)->GetEnd() ) );
        *(*iter)->GetEnd() = nSplitPos;
        rSplits.insert(iter + 1, pNew);
    }
}

/**
  Insert nesting hint into the hints array. Also calls NoteInHistory.
  @param    rNewHint    the hint to be inserted (must not overlap existing!)
 */
void SwpHints::InsertNesting(SwTxtAttrNesting & rNewHint)
{
    SwpHintsArray::Insert(& rNewHint);
    NoteInHistory( & rNewHint, true );
}

/**

The following hints correspond to well-formed XML elements in ODF:
RES_TXTATR_INETFMT, RES_TXTATR_CJK_RUBY, RES_TXTATR_META, RES_TXTATR_METAFIELD

The writer core must ensure that these do not overlap; if they did,
the document would not be storable as ODF.

Also, a Hyperlink must not be nested within another Hyperlink,
and a Ruby must not be nested within another Ruby.

The ODF export in xmloff will only put a hyperlink into a ruby, never a ruby
into a hyperlink.

Unfortunately the UNO API for Hyperlink and Ruby consists of the properties
Hyperlink* and Ruby* of the css.text.CharacterProperties service.  In other
words, they are treated as formatting attributes, not as content entites.
Furthermore, for API users it is not possible to easily test whether a certain
range would be overlapping with other nested attributes, and most importantly,
<em>which ones</em>, so we can hardly refuse to insert these in cases of
overlap.

It is possible to split Hyperlink and Ruby into multiple portions, such that
the result is properly nested.

meta and meta-field must not be split, because they have xml:id.

These constraints result in the following design:

RES_TXTATR_INETFMT:
    always succeeds
    inserts n attributes split at RES_TXTATR_CJK_RUBY, RES_TXTATR_META,
        RES_TXTATR_METAFIELD
    may replace existing RES_TXTATR_INETFMT at overlap
RES_TXTATR_CJK_RUBY:
    always succeeds
    inserts n attributes split at RES_TXTATR_META, RES_TXTATR_METAFIELD
    may replace existing RES_TXTATR_CJK_RUBY at overlap
    may split existing overlapping RES_TXTATR_INETFMT
RES_TXTATR_META:
    may fail if overlapping existing RES_TXTATR_META/RES_TXTATR_METAFIELD
    may split existing overlapping RES_TXTATR_INETFMT or RES_TXTATR_CJK_RUBY
    inserts 1 attribute
RES_TXTATR_METAFIELD:
    may fail if overlapping existing RES_TXTATR_META/RES_TXTATR_METAFIELD
    may split existing overlapping RES_TXTATR_INETFMT or RES_TXTATR_CJK_RUBY
    inserts 1 attribute

The nesting is expressed by the position of the hints.
RES_TXTATR_META and RES_TXTATR_METAFIELD have a CH_TXTATR, and there can
only be one such hint starting and ending at a given position.
Only RES_TXTATR_INETFMT and RES_TXTATR_CJK_RUBY lack a CH_TXTATR.
The interpretation given is that RES_TXTATR_CJK_RUBY is always around
a RES_TXTATR_INETFMT at the same start and end position (which corresponds
with the UNO API).
Both of these are always around a nesting hint with CH_TXTATR at the same
start and end position (if they should be inside, then the start should be
after the CH_TXTATR).
It would probably be a bad idea to add another nesting hint without
CH_TXTATR; on the other hand, it would be difficult adding a CH_TXTATR to
RES_TXTATR_INETFMT and RES_TXTATR_CJK_RUBY, due to the overwriting and
splitting of exising hints that is necessary for backward compatibility.

    @param rNode    the text node
    @param rHint    the hint to be inserted
    @returns        true iff hint was successfully inserted
*/
bool
SwpHints::TryInsertNesting( SwTxtNode & rNode, SwTxtAttrNesting & rNewHint )
{
//    INVARIANT:  the nestable hints in the array are properly nested
    const USHORT nNewWhich( rNewHint.Which() );
    const xub_StrLen nNewStart( *rNewHint.GetStart() );
    const xub_StrLen nNewEnd  ( *rNewHint.GetEnd()   );
//???    const bool bNoLengthAttribute( nNewStart == nNewEnd );
    const bool bNewSelfNestable( isSelfNestable(nNewWhich) );

    ASSERT( (RES_TXTATR_INETFMT   == nNewWhich) ||
            (RES_TXTATR_CJK_RUBY  == nNewWhich) ||
            (RES_TXTATR_META      == nNewWhich) ||
            (RES_TXTATR_METAFIELD == nNewWhich),
        "TryInsertNesting: Expecting INETFMT or RUBY or META or METAFIELD" );

    NestList_t OverlappingExisting; // existing hints to be split
    NestList_t OverwrittenExisting; // existing hints to be replaced
    NestList_t SplitNew;            // new hints to be inserted

    SplitNew.push_back(& rNewHint);

    // pass 1: split the inserted hint into fragments if necessary
    for ( USHORT i = 0; i < GetEndCount(); ++i )
    {
        SwTxtAttr * const pOther = GetEnd(i);

        if (pOther->IsNesting())
        {
            const USHORT nOtherWhich( pOther->Which() );
            const xub_StrLen nOtherStart( *(pOther)->GetStart() );
            const xub_StrLen nOtherEnd  ( *(pOther)->GetEnd()   );
            if (isOverlap(nNewStart, nNewEnd, nOtherStart, nOtherEnd ))
            {
                switch (splitPolicy(nNewWhich, nOtherWhich))
                {
                    case FAIL:
                        OSL_TRACE("cannot insert hint: overlap detected");
                        ::std::for_each(SplitNew.begin(), SplitNew.end(),
                            TxtAttrDeleter(*rNode.GetDoc()));
                        return false;
                    case SPLIT_NEW:
                        lcl_DoSplitNew(SplitNew, rNode, nNewStart,
                            nOtherStart, nOtherEnd, pOther->HasDummyChar());
                        break;
                    case SPLIT_OTHER:
                        OverlappingExisting.push_back(
                            static_cast<SwTxtAttrNesting*>(pOther));
                        break;
                    default:
                        ASSERT(false, "bad code monkey");
                        break;
                }
            }
            else if (isNestedAny(nNewStart, nNewEnd, nOtherStart, nOtherEnd))
            {
                if (!bNewSelfNestable && (nNewWhich == nOtherWhich))
                {
                // ruby and hyperlink: if there is nesting, _overwrite_
                OverwrittenExisting.push_back(
                    static_cast<SwTxtAttrNesting*>(pOther));
                }
                else if ((nNewStart == nOtherStart) && pOther->HasDummyChar())
                {
                    if (rNewHint.HasDummyChar())
                    {
                        ASSERT(false,
                                "ERROR: inserting duplicate CH_TXTATR hint");
                        return false;
                    } else if (nNewEnd < nOtherEnd) {
                        // other has dummy char, new is inside other, but
                        // new contains the other's dummy char?
                        // should be corrected because it may lead to problems
                        // in SwXMeta::createEnumeration
                        // SplitNew is sorted, so this is the first split
                        xub_StrLen *const pStart(SplitNew.front()->GetStart());
                        ASSERT(*pStart == nNewStart, "how did that happen?");
                        *pStart = nNewStart + 1;
                    }
                }
            }
        }
    }

    ASSERT (isSplittable(nNewWhich) || SplitNew.size() == 1,
            "splitting the unsplittable ???");

    // pass 2: split existing hints that overlap/nest with new hint
    // do not iterate over hints array, but over remembered set of overlapping
    // hints, to keep things simple w.r.t. insertion/removal
    // N.B: if there is a hint that splits the inserted hint, then
    // that hint would also have already split any hint in OverlappingExisting
    // so any hint in OverlappingExisting can be split at most by one hint
    // in SplitNew, or even not at all (this is not true for existing hints
    // that go _around_ new hint, which is the raison d'^etre for pass 4)
    for (NestList_t::iterator itOther = OverlappingExisting.begin();
            itOther != OverlappingExisting.end(); ++itOther)
    {
        const xub_StrLen nOtherStart( *(*itOther)->GetStart() );
        const xub_StrLen nOtherEnd  ( *(*itOther)->GetEnd()   );

        for (NestList_t::iterator itNew = SplitNew.begin();
                itNew != SplitNew.end(); ++itNew)
        {
            const xub_StrLen nSplitNewStart( *(*itNew)->GetStart() );
            const xub_StrLen nSplitNewEnd  ( *(*itNew)->GetEnd()   );
            // 4 cases: within, around, overlap l, overlap r, (OTHER: no action)
            const bool bRemoveOverlap(
                !bNewSelfNestable && (nNewWhich == (*itOther)->Which()) );

            switch (ComparePosition(nSplitNewStart, nSplitNewEnd,
                                    nOtherStart,    nOtherEnd))
            {
                case POS_INSIDE:
                    {
                        ASSERT(!bRemoveOverlap,
                            "this one should be in OverwrittenExisting?");
                    }
                    break;
                case POS_OUTSIDE:
                case POS_EQUAL:
                    {
                        ASSERT(false, "existing hint inside new hint: why?");
                    }
                    break;
                case POS_OVERLAP_BEFORE:
                    {
                        Delete( *itOther ); // this also does NoteInHistory!
                        *(*itOther)->GetStart() = nSplitNewEnd;
                        InsertNesting( **itOther );
                        if (!bRemoveOverlap)
                        {
                            if ( USHRT_MAX == Count() )
                            {
                                ASSERT(false, "hints array full :-(");
                                return false;
                            }
                            SwTxtAttrNesting * const pOtherLeft(
                                MakeTxtAttrNesting( rNode, **itOther,
                                    nOtherStart, nSplitNewEnd ) );
                            InsertNesting( *pOtherLeft );
                        }
                    }
                    break;
                case POS_OVERLAP_BEHIND:
                    {
                        Delete( *itOther ); // this also does NoteInHistory!
                        *(*itOther)->GetEnd() = nSplitNewStart;
                        InsertNesting( **itOther );
                        if (!bRemoveOverlap)
                        {
                            if ( USHRT_MAX == Count() )
                            {
                                ASSERT(false, "hints array full :-(");
                                return false;
                            }
                            SwTxtAttrNesting * const pOtherRight(
                                MakeTxtAttrNesting( rNode, **itOther,
                                    nSplitNewStart, nOtherEnd ) );
                            InsertNesting( *pOtherRight );
                        }
                    }
                    break;
                default:
                    break; // overlap resolved by splitting new: nothing to do
            }
        }
    }

    if ( USHRT_MAX - SplitNew.size() <= Count() )
    {
        ASSERT(false, "hints array full :-(");
        return false;
    }

    // pass 3: insert new hints
    for (NestList_t::iterator iter = SplitNew.begin();
            iter != SplitNew.end(); ++iter)
    {
        InsertNesting(**iter);
    }

    // pass 4: handle overwritten hints
    // RES_TXTATR_INETFMT and RES_TXTATR_CJK_RUBY should displace attributes
    // of the same kind.
    for (NestList_t::iterator itOther = OverwrittenExisting.begin();
            itOther != OverwrittenExisting.end(); ++itOther)
    {
        const xub_StrLen nOtherStart( *(*itOther)->GetStart() );
        const xub_StrLen nOtherEnd  ( *(*itOther)->GetEnd()   );

        // overwritten portion is given by start/end of inserted hint
        if ((nNewStart <= nOtherStart) && (nOtherEnd <= nNewEnd))
        {
            Delete(*itOther);
            rNode.DestroyAttr( *itOther );
        }
        else
        {
            ASSERT((nOtherStart < nNewStart) && (nNewEnd < nOtherEnd), "huh?");
        // scenario: there is a RUBY, and contained within that a META;
        // now a RUBY is inserted within the META => the exising RUBY is split:
        // here it is not possible to simply insert the left/right fragment
        // of the existing RUBY because they <em>overlap</em> with the META!
            Delete( *itOther ); // this also does NoteInHistory!
            *(*itOther)->GetEnd() = nNewStart;
            bool bSuccess( TryInsertNesting(rNode, **itOther) );
            ASSERT(bSuccess, "recursive call 1 failed?");
            SwTxtAttrNesting * const pOtherRight(
                MakeTxtAttrNesting(
                    rNode, **itOther, nNewEnd, nOtherEnd ) );
            bSuccess = TryInsertNesting(rNode, *pOtherRight);
            ASSERT(bSuccess, "recursive call 2 failed?");
        }

    }

    return true;
}


// This function takes care for the following text attribute:
// RES_TXTATR_CHARFMT, RES_TXTATR_AUTOFMT
// These attributes have to be handled in a special way (Portion building).
//
// The new attribute will be split by any existing RES_TXTATR_AUTOFMT or
// RES_TXTATR_CHARFMT. The new attribute itself will
// split any existing RES_TXTATR_AUTOFMT or RES_TXTATR_CHARFMT.

void SwpHints::BuildPortions( SwTxtNode& rNode, SwTxtAttr& rNewHint,
        const SetAttrMode nMode )
{
    const USHORT nWhich = rNewHint.Which();

    const xub_StrLen nThisStart = *rNewHint.GetStart();
    const xub_StrLen nThisEnd =   *rNewHint.GetEnd();
    const bool bNoLengthAttribute = nThisStart == nThisEnd;

    std::vector<SwTxtAttr*> aInsDelHints;
    std::vector<SwTxtAttr*>::iterator aIter;

    ASSERT( RES_TXTATR_CHARFMT == rNewHint.Which() ||
            RES_TXTATR_AUTOFMT == rNewHint.Which(),
            "Expecting CHARFMT or AUTOFMT" );

    //
    // 2. Find the hints which cover the start and end position
    // of the new hint. These hints have to be split into two portions:
    //
    if ( !bNoLengthAttribute ) // nothing to do for no length attributes
    {
        for ( USHORT i = 0; i < Count(); ++i )
        {
            SwTxtAttr* pOther = GetTextHint(i);

            if ( RES_TXTATR_CHARFMT != pOther->Which() &&
                 RES_TXTATR_AUTOFMT != pOther->Which() )
                continue;

            xub_StrLen nOtherStart = *pOther->GetStart();
            const xub_StrLen nOtherEnd = *pOther->GetEnd();

            // Check if start of new attribute overlaps with pOther:
            // Split pOther if necessary:
            if ( nOtherStart < nThisStart && nThisStart < nOtherEnd )
            {
                SwTxtAttr* pNewAttr = MakeTxtAttr( *rNode.GetDoc(),
                        pOther->GetAttr(), nOtherStart, nThisStart );
                if ( RES_TXTATR_CHARFMT == pOther->Which() )
                    static_cast<SwTxtCharFmt*>(pNewAttr)->SetSortNumber( static_cast<SwTxtCharFmt*>(pOther)->GetSortNumber() );
                aInsDelHints.push_back( pNewAttr );

                NoteInHistory( pOther );
                *pOther->GetStart() = nThisStart;
                NoteInHistory( pOther, true );

                nOtherStart = nThisStart;
            }

            // Check if end of new attribute overlaps with pOther:
            // Split pOther if necessary:
            if ( nOtherStart < nThisEnd && nThisEnd < nOtherEnd )
            {
                SwTxtAttr* pNewAttr = MakeTxtAttr( *rNode.GetDoc(),
                        pOther->GetAttr(), nOtherStart, nThisEnd );
                if ( RES_TXTATR_CHARFMT == pOther->Which() )
                    static_cast<SwTxtCharFmt*>(pNewAttr)->SetSortNumber( static_cast<SwTxtCharFmt*>(pOther)->GetSortNumber() );
                aInsDelHints.push_back( pNewAttr );

                NoteInHistory( pOther );
                *pOther->GetStart() = nThisEnd;
                NoteInHistory( pOther, true );
            }
        }

        // Insert the newly created attributes:
        for ( aIter = aInsDelHints.begin(); aIter != aInsDelHints.end(); ++aIter )
        {
            SwpHintsArray::Insert( *aIter );
            NoteInHistory( *aIter, true );
        }
    }

#ifdef DBG_UTIL
    if( !rNode.GetDoc()->IsInReading() )
        CHECK;
#endif

    //
    // 4. Split rNewHint into 1 ... n new hints:
    //
    std::set<xub_StrLen> aBounds;
    aBounds.insert( nThisStart );
    aBounds.insert( nThisEnd );

    if ( !bNoLengthAttribute ) // nothing to do for no length attributes
    {
        for ( USHORT i = 0; i < Count(); ++i )
        {
            const SwTxtAttr* pOther = GetTextHint(i);

            if ( RES_TXTATR_CHARFMT != pOther->Which() &&
                 RES_TXTATR_AUTOFMT != pOther->Which() )
                continue;

            const xub_StrLen nOtherStart = *pOther->GetStart();
            const xub_StrLen nOtherEnd = *pOther->GetEnd();

            aBounds.insert( nOtherStart );
            aBounds.insert( nOtherEnd );
        }
    }

    std::set<xub_StrLen>::iterator aStartIter = aBounds.lower_bound( nThisStart );
    std::set<xub_StrLen>::iterator aEndIter = aBounds.upper_bound( nThisEnd );
    xub_StrLen nPorStart = *aStartIter;
    ++aStartIter;
    bool bDestroyHint = true;

    //
    // Insert the 1...n new parts of the new attribute:
    //
    while ( aStartIter != aEndIter || bNoLengthAttribute )
    {
        ASSERT( bNoLengthAttribute || nPorStart < *aStartIter, "AUTOSTYLES: BuildPortion trouble" )

        const xub_StrLen nPorEnd = bNoLengthAttribute ? nPorStart : *aStartIter;
        aInsDelHints.clear();

        // Get all hints that are in [nPorStart, nPorEnd[:
        for ( USHORT i = 0; i < Count(); ++i )
        {
            SwTxtAttr *pOther = GetTextHint(i);

            if ( RES_TXTATR_CHARFMT != pOther->Which() &&
                 RES_TXTATR_AUTOFMT != pOther->Which() )
                continue;

            const xub_StrLen nOtherStart = *pOther->GetStart();

            if ( nOtherStart > nPorStart )
                break;

            if ( pOther->GetEnd() && *pOther->GetEnd() == nPorEnd && nOtherStart == nPorStart )
            {
                ASSERT( *pOther->GetEnd() == nPorEnd, "AUTOSTYLES: BuildPortion trouble" )
                aInsDelHints.push_back( pOther );
            }
        }

        SwTxtAttr* pNewAttr = 0;
        if ( RES_TXTATR_CHARFMT == nWhich )
        {
            // pNewHint can be inserted after calculating the sort value.
            // This should ensure, that pNewHint comes behind the already present
            // character style
            USHORT nCharStyleCount = 0;
            aIter = aInsDelHints.begin();
            while ( aIter != aInsDelHints.end() )
            {
                if ( RES_TXTATR_CHARFMT == (*aIter)->Which() )
                {
                    // --> FME 2007-02-16 #i74589#
                    const SwFmtCharFmt& rOtherCharFmt = (*aIter)->GetCharFmt();
                    const SwFmtCharFmt& rThisCharFmt = rNewHint.GetCharFmt();
                    const bool bSameCharFmt = rOtherCharFmt.GetCharFmt() == rThisCharFmt.GetCharFmt();
                    // <--

                    // --> OD 2009-03-24 #i90311#
                    // Do not remove existing character format hint during XML import
                    if ( !rNode.GetDoc()->IsInXMLImport() &&
                         ( !( nsSetAttrMode::SETATTR_DONTREPLACE & nMode ) ||
                           bNoLengthAttribute ||
                           bSameCharFmt ) )
                    // <--
                    {
                        // Remove old hint
                        Delete( *aIter );
                        rNode.DestroyAttr( *aIter );
                    }
                    else
                        ++nCharStyleCount;
                }
                else
                {
                    // remove all attributes from auto styles, which are explicitely set in
                    // the new character format:
                    ASSERT( RES_TXTATR_AUTOFMT == (*aIter)->Which(), "AUTOSTYLES - Misc trouble" )
                    SwTxtAttr* pOther = *aIter;
                    boost::shared_ptr<SfxItemSet> pOldStyle = static_cast<const SwFmtAutoFmt&>(pOther->GetAttr()).GetStyleHandle();

                    // For each attribute in the automatic style check if it
                    // is also set the the new character style:
                    SfxItemSet aNewSet( *pOldStyle->GetPool(), RES_CHRATR_BEGIN, RES_CHRATR_END );
                    SfxItemIter aItemIter( *pOldStyle );
                    const SfxPoolItem* pItem = aItemIter.GetCurItem();
                    while( TRUE )
                    {
                        if ( !CharFmt::IsItemIncluded( pItem->Which(), &rNewHint ) )
                        {
                            aNewSet.Put( *pItem );
                        }

                        if( aItemIter.IsAtEnd() )
                            break;

                        pItem = aItemIter.NextItem();
                    }

                    // Remove old hint
                    Delete( pOther );
                    rNode.DestroyAttr( pOther );

                    // Create new AutoStyle
                    if ( aNewSet.Count() )
                    {
                        pNewAttr = MakeTxtAttr( *rNode.GetDoc(),
                                aNewSet, nPorStart, nPorEnd );
                        SwpHintsArray::Insert( pNewAttr );
                        NoteInHistory( pNewAttr, true );
                    }
                }
                ++aIter;
            }

            // If there is no current hint and start and end of rNewHint
            // is ok, we do not need to create a new txtattr.
            if ( nPorStart == nThisStart &&
                 nPorEnd == nThisEnd &&
                 !nCharStyleCount )
            {
                pNewAttr = &rNewHint;
                bDestroyHint = false;
            }
            else
            {
                pNewAttr = MakeTxtAttr( *rNode.GetDoc(), rNewHint.GetAttr(),
                        nPorStart, nPorEnd );
                static_cast<SwTxtCharFmt*>(pNewAttr)->SetSortNumber( nCharStyleCount );
            }
        }
        else
        {
            // Find the current autostyle. Mix attributes if necessary.
            SwTxtAttr* pCurrentAutoStyle = 0;
            SwTxtAttr* pCurrentCharFmt = 0;
            aIter = aInsDelHints.begin();
            while ( aIter != aInsDelHints.end() )
            {
                if ( RES_TXTATR_AUTOFMT == (*aIter)->Which() )
                    pCurrentAutoStyle = *aIter;
                else if ( RES_TXTATR_CHARFMT == (*aIter)->Which() )
                    pCurrentCharFmt = *aIter;
                ++aIter;
            }

            boost::shared_ptr<SfxItemSet> pNewStyle = static_cast<const SwFmtAutoFmt&>(rNewHint.GetAttr()).GetStyleHandle();
            if ( pCurrentAutoStyle )
            {
                boost::shared_ptr<SfxItemSet> pCurrentStyle = static_cast<const SwFmtAutoFmt&>(pCurrentAutoStyle->GetAttr()).GetStyleHandle();

                // Merge attributes
                SfxItemSet aNewSet( *pCurrentStyle );
                aNewSet.Put( *pNewStyle );

                // --> FME 2007-4-11 #i75750# Remove attributes already set at whole paragraph
                // --> FME 2007-09-24 #i81764# This should not be applied for no length attributes!!! <--
                if ( !bNoLengthAttribute && rNode.HasSwAttrSet() && aNewSet.Count() )
                {
                    SfxItemIter aIter2( aNewSet );
                    const SfxPoolItem* pItem = aIter2.GetCurItem();
                    const SfxItemSet& rWholeParaAttrSet = rNode.GetSwAttrSet();

                    do
                    {
                        const SfxPoolItem* pTmpItem = 0;
                        if ( SFX_ITEM_SET == rWholeParaAttrSet.GetItemState( pItem->Which(), FALSE, &pTmpItem ) &&
                             pTmpItem == pItem )
                        {
                            // Do not clear item if the attribute is set in a character format:
                            if ( !pCurrentCharFmt || 0 == CharFmt::GetItem( *pCurrentCharFmt, pItem->Which() ) )
                                aNewSet.ClearItem( pItem->Which() );
                        }
                    }
                    while (!aIter2.IsAtEnd() && 0 != (pItem = aIter2.NextItem()));
                }
                // <--

                // Remove old hint
                Delete( pCurrentAutoStyle );
                rNode.DestroyAttr( pCurrentAutoStyle );

                // Create new AutoStyle
                if ( aNewSet.Count() )
                    pNewAttr = MakeTxtAttr( *rNode.GetDoc(), aNewSet,
                            nPorStart, nPorEnd );
            }
            else
            {
                // Remove any attributes which are already set at the whole paragraph:
                bool bOptimizeAllowed = true;

                SfxItemSet* pNewSet = 0;
                // --> FME 2007-4-11 #i75750# Remove attributes already set at whole paragraph
                // --> FME 2007-09-24 #i81764# This should not be applied for no length attributes!!! <--
                if ( !bNoLengthAttribute && rNode.HasSwAttrSet() && pNewStyle->Count() )
                {
                    SfxItemIter aIter2( *pNewStyle );
                    const SfxPoolItem* pItem = aIter2.GetCurItem();
                    const SfxItemSet& rWholeParaAttrSet = rNode.GetSwAttrSet();

                    do
                    {
                        const SfxPoolItem* pTmpItem = 0;
                        if ( SFX_ITEM_SET == rWholeParaAttrSet.GetItemState( pItem->Which(), FALSE, &pTmpItem ) &&
                             pTmpItem == pItem )
                        {
                            // Do not clear item if the attribute is set in a character format:
                            if ( !pCurrentCharFmt || 0 == CharFmt::GetItem( *pCurrentCharFmt, pItem->Which() ) )
                            {
                                if ( !pNewSet )
                                    pNewSet = pNewStyle->Clone( TRUE );
                                pNewSet->ClearItem( pItem->Which() );
                            }
                        }
                    }
                    while (!aIter2.IsAtEnd() && 0 != (pItem = aIter2.NextItem()));

                    if ( pNewSet )
                    {
                        bOptimizeAllowed = false;
                        if ( pNewSet->Count() )
                            pNewStyle = rNode.getIDocumentStyleAccess().getAutomaticStyle( *pNewSet, IStyleAccess::AUTO_STYLE_CHAR );
                        else
                            pNewStyle.reset();

                        delete pNewSet;
                    }
                }
                // <--

                // Create new AutoStyle
                // If there is no current hint and start and end of rNewHint
                // is ok, we do not need to create a new txtattr.
                if ( bOptimizeAllowed &&
                     nPorStart == nThisStart &&
                     nPorEnd == nThisEnd )
                {
                    pNewAttr = &rNewHint;
                    bDestroyHint = false;
                }
                else if ( pNewStyle.get() )
                {
                    pNewAttr = MakeTxtAttr( *rNode.GetDoc(), *pNewStyle,
                            nPorStart, nPorEnd );
                }
            }
        }

        if ( pNewAttr )
        {
            SwpHintsArray::Insert( pNewAttr );
//            if ( bDestroyHint )
                NoteInHistory( pNewAttr, true );
        }

        if ( !bNoLengthAttribute )
        {
            nPorStart = *aStartIter;
            ++aStartIter;
        }
        else
            break;
    }

    if ( bDestroyHint )
        rNode.DestroyAttr( &rNewHint );
}

/*************************************************************************
 *						SwTxtNode::MakeTxtAttr()
 *************************************************************************/

SwTxtAttr* MakeRedlineTxtAttr( SwDoc & rDoc, SfxPoolItem & rAttr )
{
    // this is intended _only_ for special-purpose redline attributes!
    switch (rAttr.Which())
    {
        case RES_CHRATR_COLOR:
        case RES_CHRATR_WEIGHT:
        case RES_CHRATR_CJK_WEIGHT:
        case RES_CHRATR_CTL_WEIGHT:
        case RES_CHRATR_POSTURE:
        case RES_CHRATR_CJK_POSTURE:
        case RES_CHRATR_CTL_POSTURE:
        case RES_CHRATR_UNDERLINE:
        case RES_CHRATR_CROSSEDOUT:
        case RES_CHRATR_CASEMAP:
        case RES_CHRATR_BACKGROUND:
            break;
        default:
            ASSERT(false, "unsupported redline attribute");
            break;
    }

    // Put new attribute into pool
    // FIXME: this const_cast is evil!
    SfxPoolItem& rNew =
        const_cast<SfxPoolItem&>( rDoc.GetAttrPool().Put( rAttr ) );
    return new SwTxtAttrEnd( rNew, 0, 0 );
}

// create new text attribute
SwTxtAttr* MakeTxtAttr( SwDoc & rDoc, SfxPoolItem& rAttr,
                        xub_StrLen nStt, xub_StrLen nEnd )
{
    if ( isCHRATR(rAttr.Which()) )
    {
        // Somebody wants to build a SwTxtAttr for a character attribute.
        // Sorry, this is not allowed any longer.
        // You'll get a brand new autostyle attribute:
        SfxItemSet aItemSet( rDoc.GetAttrPool(),
                RES_CHRATR_BEGIN, RES_CHRATR_END );
        aItemSet.Put( rAttr );
        return MakeTxtAttr( rDoc, aItemSet, nStt, nEnd );
    }
    else if ( RES_TXTATR_AUTOFMT == rAttr.Which() &&
              static_cast<const SwFmtAutoFmt&>(rAttr).GetStyleHandle()->
                GetPool() != &rDoc.GetAttrPool() )
    {
        // If the attribute is an auto-style which refers to a pool that is
        // different from rDoc's pool, we have to correct this:
        const StylePool::SfxItemSet_Pointer_t pAutoStyle = static_cast<const SwFmtAutoFmt&>(rAttr).GetStyleHandle();
        ::std::auto_ptr<const SfxItemSet> pNewSet(
                pAutoStyle->SfxItemSet::Clone( TRUE, &rDoc.GetAttrPool() ));
        SwTxtAttr* pNew = MakeTxtAttr( rDoc, *pNewSet, nStt, nEnd );
        return pNew;
    }

    // Put new attribute into pool
    // FIXME: this const_cast is evil!
    SfxPoolItem& rNew =
        const_cast<SfxPoolItem&>( rDoc.GetAttrPool().Put( rAttr ) );

    SwTxtAttr* pNew = 0;
    switch( rNew.Which() )
    {
    case RES_TXTATR_CHARFMT:
        {
            SwFmtCharFmt &rFmtCharFmt = (SwFmtCharFmt&) rNew;
            if( !rFmtCharFmt.GetCharFmt() )
            {
                rFmtCharFmt.SetCharFmt( rDoc.GetDfltCharFmt() );
            }

            pNew = new SwTxtCharFmt( rFmtCharFmt, nStt, nEnd );
        }
        break;
    case RES_TXTATR_INETFMT:
        pNew = new SwTxtINetFmt( (SwFmtINetFmt&)rNew, nStt, nEnd );
        break;
    case RES_TXTATR_FIELD:
        pNew = new SwTxtFld( (SwFmtFld&)rNew, nStt );
        break;
    case RES_TXTATR_FLYCNT:
        {
            // erst hier wird das Frame-Format kopiert (mit Inhalt) !!
            pNew = new SwTxtFlyCnt( (SwFmtFlyCnt&)rNew, nStt );
            // Kopie von einem Text-Attribut
            if ( static_cast<const SwFmtFlyCnt &>(rAttr).GetTxtFlyCnt() )
            {
                // then the format must be copied
                static_cast<SwTxtFlyCnt *>(pNew)->CopyFlyFmt( &rDoc );
            }
        }
        break;
    case RES_TXTATR_FTN:
        pNew = new SwTxtFtn( (SwFmtFtn&)rNew, nStt );
        // ggfs. SeqNo kopieren
        if( ((SwFmtFtn&)rAttr).GetTxtFtn() )
            ((SwTxtFtn*)pNew)->SetSeqNo( ((SwFmtFtn&)rAttr).GetTxtFtn()->GetSeqRefNo() );
        break;
    case RES_TXTATR_REFMARK:
        pNew = nStt == nEnd
                ? new SwTxtRefMark( (SwFmtRefMark&)rNew, nStt )
                : new SwTxtRefMark( (SwFmtRefMark&)rNew, nStt, &nEnd );
        break;
    case RES_TXTATR_TOXMARK:
        pNew = new SwTxtTOXMark( (SwTOXMark&)rNew, nStt, &nEnd );
        break;
    case RES_TXTATR_CJK_RUBY:
        pNew = new SwTxtRuby( (SwFmtRuby&)rNew, nStt, nEnd );
        break;
    case RES_TXTATR_META:
    case RES_TXTATR_METAFIELD:
        pNew = new SwTxtMeta( static_cast<SwFmtMeta&>(rNew), nStt, nEnd );
        break;
    default:
        ASSERT(RES_TXTATR_AUTOFMT == rNew.Which(), "unknown attribute");
        pNew = new SwTxtAttrEnd( rNew, nStt, nEnd );
        break;
    }

    return pNew;
}

SwTxtAttr* MakeTxtAttr( SwDoc & rDoc, const SfxItemSet& rSet,
                        xub_StrLen nStt, xub_StrLen nEnd )
{
    IStyleAccess& rStyleAccess = rDoc.GetIStyleAccess();
    const StylePool::SfxItemSet_Pointer_t pAutoStyle = rStyleAccess.getAutomaticStyle( rSet, IStyleAccess::AUTO_STYLE_CHAR );
    SwFmtAutoFmt aNewAutoFmt;
    aNewAutoFmt.SetStyleHandle( pAutoStyle );
    SwTxtAttr* pNew = MakeTxtAttr( rDoc, aNewAutoFmt, nStt, nEnd );
    return pNew;
}


// loesche das Text-Attribut (muss beim Pool abgemeldet werden!)
void SwTxtNode::DestroyAttr( SwTxtAttr* pAttr )
{
    if( pAttr )
    {
        // einige Sachen muessen vorm Loeschen der "Format-Attribute" erfolgen
        SwDoc* pDoc = GetDoc();
        USHORT nDelMsg = 0;
        switch( pAttr->Which() )
        {
        case RES_TXTATR_FLYCNT:
            {
                // siehe auch die Anmerkung "Loeschen von Formaten
                // zeichengebundener Frames" in fesh.cxx, SwFEShell::DelFmt()
                SwFrmFmt* pFmt = pAttr->GetFlyCnt().GetFrmFmt();
                if( pFmt )		// vom Undo auf 0 gesetzt ??
                    pDoc->DelLayoutFmt( (SwFlyFrmFmt*)pFmt );
            }
            break;

        case RES_CHRATR_HIDDEN:
            SetCalcHiddenCharFlags();
            break;

        case RES_TXTATR_FTN:
            ((SwTxtFtn*)pAttr)->SetStartNode( 0 );
            nDelMsg = RES_FOOTNOTE_DELETED;
            break;

        case RES_TXTATR_FIELD:
            if( !pDoc->IsInDtor() )
            {
                // Wenn wir ein HiddenParaField sind, dann muessen wir
                // ggf. fuer eine Neuberechnung des Visible-Flags sorgen.
                const SwField* pFld = pAttr->GetFld().GetFld();

                //JP 06-08-95: DDE-Felder bilden eine Ausnahme
                ASSERT( RES_DDEFLD == pFld->GetTyp()->Which() ||
                        this == ((SwTxtFld*)pAttr)->GetpTxtNode(),
                        "Wo steht denn dieses Feld?" )

                // bestimmte Felder mussen am Doc das Calculations-Flag updaten
                switch( pFld->GetTyp()->Which() )
                {
                case RES_HIDDENPARAFLD:
                    SetCalcHiddenParaField();
                    // kein break !
                case RES_DBSETNUMBERFLD:
                case RES_GETEXPFLD:
                case RES_DBFLD:
                case RES_SETEXPFLD:
                case RES_HIDDENTXTFLD:
                case RES_DBNUMSETFLD:
                case RES_DBNEXTSETFLD:
                    if( !pDoc->IsNewFldLst() && GetNodes().IsDocNodes() )
                        pDoc->InsDelFldInFldLst( FALSE, *(SwTxtFld*)pAttr );
                    break;
                case RES_DDEFLD:
                    if( GetNodes().IsDocNodes() &&
                        ((SwTxtFld*)pAttr)->GetpTxtNode() )
                        ((SwDDEFieldType*)pFld->GetTyp())->DecRefCnt();
                    break;
                case RES_POSTITFLD:
                    {
                        const_cast<SwFmtFld&>(pAttr->GetFld()).Broadcast( SwFmtFldHint( &((SwTxtFld*)pAttr)->GetFld(), SWFMTFLD_REMOVED ) );
                        break;
                    }
                }
            }
            nDelMsg = RES_FIELD_DELETED;
            break;

        case RES_TXTATR_TOXMARK:
            static_cast<SwTOXMark&>(pAttr->GetAttr()).InvalidateTOXMark();
            break;

        case RES_TXTATR_REFMARK:
            nDelMsg = RES_REFMARK_DELETED;
            break;

        case RES_TXTATR_META:
        case RES_TXTATR_METAFIELD:
            static_cast<SwTxtMeta*>(pAttr)->ChgTxtNode(0);
            break;

        default:
            break;
        }

        if( nDelMsg && !pDoc->IsInDtor() && GetNodes().IsDocNodes() )
        {
            SwPtrMsgPoolItem aMsgHint( nDelMsg, (void*)&pAttr->GetAttr() );
            pDoc->GetUnoCallBack()->Modify( &aMsgHint, &aMsgHint );
        }

        SwTxtAttr::Destroy( pAttr, pDoc->GetAttrPool() );
    }
}

/*************************************************************************
 *						SwTxtNode::Insert()
 *************************************************************************/

SwTxtAttr*
SwTxtNode::InsertItem( SfxPoolItem& rAttr,
      const xub_StrLen nStart, const xub_StrLen nEnd, const SetAttrMode nMode )
{
   // character attributes will be inserted as automatic styles:
    ASSERT( !isCHRATR(rAttr.Which()), "AUTOSTYLES - "
        "SwTxtNode::InsertItem should not be called with character attributes");

    SwTxtAttr* const pNew = MakeTxtAttr( *GetDoc(), rAttr, nStart, nEnd );

    if ( pNew )
    {
        const bool bSuccess( InsertHint( pNew, nMode ) );
        // N.B.: also check that the hint is actually in the hints array,
        // because hints of certain types may be merged after succesful
        // insertion, and thus destroyed!
        if (!bSuccess || ( USHRT_MAX == m_pSwpHints->GetPos( pNew ) ))
        {
            return 0;
        }
    }

    return pNew;
}

// take ownership of pAttr; if insertion fails, delete pAttr
bool SwTxtNode::InsertHint( SwTxtAttr * const pAttr, const SetAttrMode nMode )
{
    BOOL bHiddenPara = FALSE;

    ASSERT( pAttr && *pAttr->GetStart() <= Len(), "StartIdx out of bounds!" );
    ASSERT( !pAttr->GetEnd() || (*pAttr->GetEnd() <= Len()),
            "EndIdx out of bounds!" );

    // translate from SetAttrMode to InsertMode (for hints with CH_TXTATR)
    const enum IDocumentContentOperations::InsertFlags nInsertFlags =
        (nMode & nsSetAttrMode::SETATTR_FORCEHINTEXPAND)
        ? static_cast<IDocumentContentOperations::InsertFlags>(
                IDocumentContentOperations::INS_FORCEHINTEXPAND |
                IDocumentContentOperations::INS_EMPTYEXPAND)
        : IDocumentContentOperations::INS_EMPTYEXPAND;

    // need this after TryInsertHint, when pAttr may be deleted
    const xub_StrLen nStart( *pAttr->GetStart() );
    const bool bDummyChar( pAttr->HasDummyChar() );
    if (bDummyChar)
    {
        USHORT nInsMode = nMode;
        switch( pAttr->Which() )
        {
            case RES_TXTATR_FLYCNT:
            {
                SwTxtFlyCnt *pFly = (SwTxtFlyCnt *)pAttr;
                SwFrmFmt* pFmt = pAttr->GetFlyCnt().GetFrmFmt();
                if( !(nsSetAttrMode::SETATTR_NOTXTATRCHR & nInsMode) )
                {
                    // Wir muessen zuerst einfuegen, da in SetAnchor()
                    // dem FlyFrm GetStart() uebermittelt wird.
                    //JP 11.05.98: falls das Anker-Attribut schon richtig
                    // gesetzt ist, dann korrigiere dieses nach dem Einfuegen
                    // des Zeichens. Sonst muesste das immer  ausserhalb
                    // erfolgen (Fehleranfaellig !)
                    const SwFmtAnchor* pAnchor = 0;
                    pFmt->GetItemState( RES_ANCHOR, FALSE,
                                            (const SfxPoolItem**)&pAnchor );

                    SwIndex aIdx( this, *pAttr->GetStart() );
                    const sal_Unicode c = GetCharOfTxtAttr(*pAttr);
                    InsertText( c, aIdx, nInsertFlags );
                    nInsMode |= nsSetAttrMode::SETATTR_NOTXTATRCHR;

                    if (pAnchor &&
                        (FLY_AS_CHAR == pAnchor->GetAnchorId()) &&
                        pAnchor->GetCntntAnchor() &&
                        pAnchor->GetCntntAnchor()->nNode == *this &&
                        pAnchor->GetCntntAnchor()->nContent == aIdx )
                    {
                        const_cast<SwIndex&>(
                            pAnchor->GetCntntAnchor()->nContent)--;
                    }
                }
                pFly->SetAnchor( this );

                // Format-Pointer kann sich im SetAnchor geaendert haben!
                // (Kopieren in andere Docs!)
                pFmt = pAttr->GetFlyCnt().GetFrmFmt();
                SwDoc *pDoc = pFmt->GetDoc();

                // OD 26.06.2003 #108784# - allow drawing objects in header/footer.
                // But don't allow control objects in header/footer
                if( RES_DRAWFRMFMT == pFmt->Which() &&
                    pDoc->IsInHeaderFooter( pFmt->GetAnchor().GetCntntAnchor()->nNode ) )
                {
                    SwDrawContact* pDrawContact =
                        static_cast<SwDrawContact*>(pFmt->FindContactObj());
                    if ( pDrawContact &&
                         pDrawContact->GetMaster() &&
                         ::CheckControlLayer( pDrawContact->GetMaster() ) )
                    {
                        // das soll nicht meoglich sein; hier verhindern
                        // Der Dtor des TxtHints loescht nicht das Zeichen.
                        // Wenn ein CH_TXTATR_.. vorliegt, dann muss man
                        // dieses explizit loeschen
                        if( nsSetAttrMode::SETATTR_NOTXTATRCHR & nInsMode )
                        {
                            // loesche das Zeichen aus dem String !
                            ASSERT( ( CH_TXTATR_BREAKWORD ==
                                        m_Text.GetChar(*pAttr->GetStart() ) ||
                                      CH_TXTATR_INWORD ==
                                        m_Text.GetChar(*pAttr->GetStart())),
                                    "where is my attribute character?" );
                            m_Text.Erase( *pAttr->GetStart(), 1 );
                            // Indizies Updaten
                            SwIndex aTmpIdx( this, *pAttr->GetStart() );
                            Update( aTmpIdx, 1, TRUE );
                        }
                        // Format loeschen nicht ins Undo aufnehmen!!
                        BOOL bUndo = pDoc->DoesUndo();
                        pDoc->DoUndo( FALSE );
                        DestroyAttr( pAttr );
                        pDoc->DoUndo( bUndo );
                        return false;
                    }
                }
                break;
            }

            case RES_TXTATR_FTN :
            {
                // Fussnoten, man kommt an alles irgendwie heran.
                // CntntNode erzeugen und in die Inserts-Section stellen
                SwDoc *pDoc = GetDoc();
                SwNodes &rNodes = pDoc->GetNodes();

                // FussNote in nicht Content-/Redline-Bereich einfuegen ??
                if( StartOfSectionIndex() < rNodes.GetEndOfAutotext().GetIndex() )
                {
                    // das soll nicht meoglich sein; hier verhindern
                    // Der Dtor des TxtHints loescht nicht das Zeichen.
                    // Wenn ein CH_TXTATR_.. vorliegt, dann muss man
                    // dieses explizit loeschen
                    if( nsSetAttrMode::SETATTR_NOTXTATRCHR & nInsMode )
                    {
                        // loesche das Zeichen aus dem String !
                        ASSERT( ( CH_TXTATR_BREAKWORD ==
                                      m_Text.GetChar(*pAttr->GetStart() ) ||
                                  CH_TXTATR_INWORD ==
                                      m_Text.GetChar(*pAttr->GetStart())),
                                "where is my attribute character?" );
                        m_Text.Erase( *pAttr->GetStart(), 1 );
                        // Indizies Updaten
                        SwIndex aTmpIdx( this, *pAttr->GetStart() );
                        Update( aTmpIdx, 1, TRUE );
                    }
                    DestroyAttr( pAttr );
                    return false;
                }

                // wird eine neue Fussnote eingefuegt ??
                BOOL bNewFtn = 0 == ((SwTxtFtn*)pAttr)->GetStartNode();
                if( bNewFtn )
                {
                    ((SwTxtFtn*)pAttr)->MakeNewTextSection( GetNodes() );
                    SwRegHistory* pHist = GetpSwpHints()
                        ? GetpSwpHints()->GetHistory() : 0;
                    if( pHist )
                        pHist->ChangeNodeIndex( GetIndex() );
                }
                else if ( !GetpSwpHints() || !GetpSwpHints()->IsInSplitNode() )
                {
                    // loesche alle Frames der Section, auf die der StartNode zeigt
                    ULONG nSttIdx =
                        ((SwTxtFtn*)pAttr)->GetStartNode()->GetIndex();
                    ULONG nEndIdx = rNodes[ nSttIdx++ ]->EndOfSectionIndex();
                    SwCntntNode* pCNd;
                    for( ; nSttIdx < nEndIdx; ++nSttIdx )
                        if( 0 != ( pCNd = rNodes[ nSttIdx ]->GetCntntNode() ))
                            pCNd->DelFrms();
                }

                if( !(nsSetAttrMode::SETATTR_NOTXTATRCHR & nInsMode) )
                {
                    // Wir muessen zuerst einfuegen, da sonst gleiche Indizes
                    // entstehen koennen und das Attribut im _SortArr_ am
                    // Dokument nicht eingetrage wird.
                    SwIndex aNdIdx( this, *pAttr->GetStart() );
                    const sal_Unicode c = GetCharOfTxtAttr(*pAttr);
                    InsertText( c, aNdIdx, nInsertFlags );
                    nInsMode |= nsSetAttrMode::SETATTR_NOTXTATRCHR;
                }

                // Wir tragen uns am FtnIdx-Array des Docs ein ...
                SwTxtFtn* pTxtFtn = 0;
                if( !bNewFtn )
                {
                    // eine alte Ftn wird umgehaengt (z.B. SplitNode)
                    for( USHORT n = 0; n < pDoc->GetFtnIdxs().Count(); ++n )
                        if( pAttr == pDoc->GetFtnIdxs()[n] )
                        {
                            // neuen Index zuweisen, dafuer aus dem SortArray
                            // loeschen und neu eintragen
                            pTxtFtn = pDoc->GetFtnIdxs()[n];
                            pDoc->GetFtnIdxs().Remove( n );
                            break;
                        }
                    // wenn ueber Undo der StartNode gesetzt wurde, kann
                    // der Index noch gar nicht in der Verwaltung stehen !!
                }
                if( !pTxtFtn )
                    pTxtFtn = (SwTxtFtn*)pAttr;

                // fuers Update der Nummern und zum Sortieren
                // muss der Node gesetzt sein.
                ((SwTxtFtn*)pAttr)->ChgTxtNode( this );

                // FussNote im Redline-Bereich NICHT ins FtnArray einfuegen!
                if( StartOfSectionIndex() > rNodes.GetEndOfRedlines().GetIndex() )
                {
#ifdef DBG_UTIL
                    const BOOL bSuccess =
#endif
                        pDoc->GetFtnIdxs().Insert( pTxtFtn );
#ifdef DBG_UTIL
                    ASSERT( bSuccess, "FtnIdx nicht eingetragen." );
#endif
                }
                SwNodeIndex aTmpIndex( *this );
                pDoc->GetFtnIdxs().UpdateFtn( aTmpIndex);
                ((SwTxtFtn*)pAttr)->SetSeqRefNo();
            }
            break;

            case RES_TXTATR_FIELD:
                {
                    // fuer HiddenParaFields Benachrichtigungsmechanismus
                    // anwerfen
                    if( RES_HIDDENPARAFLD ==
                        pAttr->GetFld().GetFld()->GetTyp()->Which() )
                    bHiddenPara = TRUE;
                }
                break;

        }
        // Fuer SwTxtHints ohne Endindex werden CH_TXTATR_..
        // eingefuegt, aStart muss danach um einen zurueckgesetzt werden.
        // Wenn wir im SwTxtNode::Copy stehen, so wurde das Zeichen bereits
        // mitkopiert. In solchem Fall ist SETATTR_NOTXTATRCHR angegeben worden.
        if( !(nsSetAttrMode::SETATTR_NOTXTATRCHR & nInsMode) )
        {
            SwIndex aIdx( this, *pAttr->GetStart() );
            InsertText( GetCharOfTxtAttr(*pAttr), aIdx, nInsertFlags );

            // adjust end of hint to account for inserted CH_TXTATR
            xub_StrLen * const pEnd(pAttr->GetEnd());
            if (pEnd)
            {
                *pEnd = *pEnd + 1;
            }
        }
    }

    GetOrCreateSwpHints();

    // 4263: AttrInsert durch TextInsert => kein Adjust
    const bool bRet = m_pSwpHints->TryInsertHint( pAttr, *this, nMode );

    if (!bRet && bDummyChar)
    {
        // undo insertion of dummy character
        // N.B. cannot insert the dummy character after inserting the hint,
        // because if the hint has no extent it will be moved in InsertText,
        // resulting in infinite recursion
        if ( !(nsSetAttrMode::SETATTR_NOTXTATRCHR & nMode) )
        {
            ASSERT( ( CH_TXTATR_BREAKWORD == m_Text.GetChar(nStart) ||
                      CH_TXTATR_INWORD    == m_Text.GetChar(nStart) ),
                    "where is my attribute character?" );
            SwIndex aIdx( this, nStart );
            EraseText( aIdx, 1 );
        }
    }

    if ( bHiddenPara )
    {
        SetCalcHiddenParaField();
    }

    return bRet;
}


/*************************************************************************
 *                        SwTxtNode::DeleteAttribute()
 *************************************************************************/

void SwTxtNode::DeleteAttribute( SwTxtAttr * const pAttr )
{
    if ( !HasHints() )
    {
        ASSERT(false, "DeleteAttribute called, but text node without hints?");
        return;
    }

    if ( pAttr->HasDummyChar() )
    {
        // Unbedingt Copy-konstruieren!
        const SwIndex aIdx( this, *pAttr->GetStart() );
        // erase the CH_TXTATR, which will also delete pAttr
        EraseText( aIdx, 1 );
    }
    else
    {
        // create MsgHint before start/end become invalid
        SwUpdateAttr aHint(
                *pAttr->GetStart(), *pAttr->GetEnd(), pAttr->Which() );
        m_pSwpHints->Delete( pAttr );
        SwTxtAttr::Destroy( pAttr, GetDoc()->GetAttrPool() );
        SwModify::Modify( 0, &aHint ); // notify Frames

        TryDeleteSwpHints();
    }
}

/*************************************************************************
 *                        SwTxtNode::DeleteAttributes()
 *************************************************************************/

//FIXME: this does NOT respect SORT NUMBER (for CHARFMT)!
void SwTxtNode::DeleteAttributes( const USHORT nWhich,
    const xub_StrLen nStart, const xub_StrLen nEnd )
{
    if ( !HasHints() )
        return;

    for ( USHORT nPos = 0; m_pSwpHints && nPos < m_pSwpHints->Count(); nPos++ )
    {
        SwTxtAttr * const pTxtHt = m_pSwpHints->GetTextHint( nPos );
        const xub_StrLen nHintStart = *(pTxtHt->GetStart());
        if (nStart < nHintStart)
        {
            break; // sorted by start
        }
        else if ( (nStart == nHintStart) && (nWhich == pTxtHt->Which()) )
        {
            if ( nWhich == RES_CHRATR_HIDDEN  )
            {
                ASSERT(false, "hey, that's a CHRATR! how did that get in?");
                SetCalcHiddenCharFlags();
            }
            else if ( nWhich == RES_TXTATR_CHARFMT )
            {
                // Check if character format contains hidden attribute:
                const SwCharFmt* pFmt = pTxtHt->GetCharFmt().GetCharFmt();
                const SfxPoolItem* pItem;
                if ( SFX_ITEM_SET == pFmt->GetItemState( RES_CHRATR_HIDDEN, TRUE, &pItem ) )
                    SetCalcHiddenCharFlags();
            }
            // --> FME 2007-03-16 #i75430# Recalc hidden flags if necessary
            else if ( nWhich == RES_TXTATR_AUTOFMT )
            {
                // Check if auto style contains hidden attribute:
                const SfxPoolItem* pHiddenItem = CharFmt::GetItem( *pTxtHt, RES_CHRATR_HIDDEN );
                if ( pHiddenItem )
                    SetCalcHiddenCharFlags();
            }
            // <--

            xub_StrLen const * const pEndIdx = pTxtHt->GetEnd();

            if ( pTxtHt->HasDummyChar() )
            {
                // Unbedingt Copy-konstruieren!
                const SwIndex aIdx( this, nStart );
                // erase the CH_TXTATR, which will also delete pTxtHt
                EraseText( aIdx, 1 );
            }
            else if( *pEndIdx == nEnd )
            {
                // den MsgHint jetzt fuettern, weil gleich sind
                // Start und End weg.
                // Das CalcVisibleFlag bei HiddenParaFields entfaellt,
                // da dies das Feld im Dtor selbst erledigt.
                SwUpdateAttr aHint( nStart, *pEndIdx, nWhich );
                m_pSwpHints->DeleteAtPos( nPos );    // gefunden, loeschen,
                SwTxtAttr::Destroy( pTxtHt, GetDoc()->GetAttrPool() );
                SwModify::Modify( 0, &aHint );	   // die Frames benachrichtigen
            }
        }
    }
    TryDeleteSwpHints();
}

/*************************************************************************
 *						SwTxtNode::DelSoftHyph()
 *************************************************************************/

void SwTxtNode::DelSoftHyph( const xub_StrLen nStt, const xub_StrLen nEnd )
{
    xub_StrLen nFndPos = nStt, nEndPos = nEnd;
    while( STRING_NOTFOUND !=
            ( nFndPos = m_Text.Search( CHAR_SOFTHYPHEN, nFndPos )) &&
            nFndPos < nEndPos )
    {
        const SwIndex aIdx( this, nFndPos );
        EraseText( aIdx, 1 );
        --nEndPos;
    }
}

// setze diese Attribute am TextNode. Wird der gesamte Bereich umspannt,
// dann setze sie nur im AutoAttrSet (SwCntntNode:: SetAttr)
BOOL SwTxtNode::SetAttr( const SfxItemSet& rSet, xub_StrLen nStt,
                         xub_StrLen nEnd, const SetAttrMode nMode )
{
    if( !rSet.Count() )
        return FALSE;

    // teil die Sets auf (fuer Selektion in Nodes)
    const SfxItemSet* pSet = &rSet;
    SfxItemSet aTxtSet( *rSet.GetPool(), RES_TXTATR_BEGIN, RES_TXTATR_END-1 );

    // gesamter Bereich
    if ( !nStt && (nEnd == m_Text.Len()) &&
         !(nMode & nsSetAttrMode::SETATTR_NOFORMATATTR ) )
    {
        // sind am Node schon Zeichenvorlagen gesetzt, muss man diese Attribute
        // (rSet) immer als TextAttribute setzen, damit sie angezeigt werden.
        int bHasCharFmts = FALSE;
        if ( HasHints() )
        {
            for ( USHORT n = 0; n < m_pSwpHints->Count(); ++n )
            {
                if ( (*m_pSwpHints)[ n ]->IsCharFmtAttr() )
                {
                    bHasCharFmts = TRUE;
                    break;
                }
            }
        }

        if( !bHasCharFmts )
        {
            aTxtSet.Put( rSet );
            // If there are any character attributes in rSet,
            // we want to set them at the paragraph:
            if( aTxtSet.Count() != rSet.Count() )
            {
                BOOL bRet = SetAttr( rSet );
                  if( !aTxtSet.Count() )
                    return bRet;
            }

            // check for auto style:
            const SfxPoolItem* pItem;
            const bool bAutoStyle = SFX_ITEM_SET == aTxtSet.GetItemState( RES_TXTATR_AUTOFMT, FALSE, &pItem );
            if ( bAutoStyle )
            {
                boost::shared_ptr<SfxItemSet> pAutoStyleSet = static_cast<const SwFmtAutoFmt*>(pItem)->GetStyleHandle();
                BOOL bRet = SetAttr( *pAutoStyleSet );
                  if( 1 == aTxtSet.Count() )
                    return bRet;
            }

            // Continue with the text attributes:
            pSet = &aTxtSet;
        }
    }

    GetOrCreateSwpHints();

    SfxItemSet aCharSet( *rSet.GetPool(), aCharAutoFmtSetRange );

    USHORT nCount = 0;
    SfxItemIter aIter( *pSet );
    const SfxPoolItem* pItem = aIter.GetCurItem();

    do
    {
        if ( pItem && (reinterpret_cast<SfxPoolItem*>(-1) != pItem))
        {
            const USHORT nWhich = pItem->Which();
            ASSERT( isCHRATR(nWhich) || isTXTATR(nWhich),
                    "SwTxtNode::SetAttr(): unknown attribute" );
            if ( isCHRATR(nWhich) || isTXTATR(nWhich) )
            {
                if ((RES_TXTATR_CHARFMT == nWhich) &&
                    (GetDoc()->GetDfltCharFmt() ==
                     static_cast<const SwFmtCharFmt*>(pItem)->GetCharFmt()))
                {
                    SwIndex aIndex( this, nStt );
                    RstAttr( aIndex, nEnd - nStt, RES_TXTATR_CHARFMT, 0 );
                    DontExpandFmt( aIndex );
                }
                else
                {
                    if (isCHRATR(nWhich) ||
                        (RES_TXTATR_UNKNOWN_CONTAINER == nWhich))
                    {
                        aCharSet.Put( *pItem );
                    }
                    else
                    {

                        SwTxtAttr *const pNew = MakeTxtAttr( *GetDoc(),
                                const_cast<SfxPoolItem&>(*pItem), nStt, nEnd );
                        if ( pNew )
                        {
                            if ( nEnd != nStt && !pNew->GetEnd() )
                            {
                                ASSERT(false,
                                    "Attribut without end, but area marked");
                                DestroyAttr( pNew ); // do not insert
                            }
                            else if ( InsertHint( pNew, nMode ) )
                            {
                                ++nCount;
                            }
                        }
                    }
                }
            }
        }
        if ( aIter.IsAtEnd() )
            break;
        pItem = aIter.NextItem();
    } while( true );

    if ( aCharSet.Count() )
    {
        SwTxtAttr* pTmpNew = MakeTxtAttr( *GetDoc(), aCharSet, nStt, nEnd );
        if ( InsertHint( pTmpNew, nMode ) )
        {
            ++nCount;
        }
    }

    TryDeleteSwpHints();

    return nCount ? TRUE : FALSE;
}

void lcl_MergeAttr( SfxItemSet& rSet, const SfxPoolItem& rAttr )
{
    if ( RES_TXTATR_AUTOFMT == rAttr.Which() )
    {
        const SfxItemSet* pCFSet = CharFmt::GetItemSet( rAttr );
        if ( !pCFSet )
            return;
        SfxWhichIter aIter( *pCFSet );
        USHORT nWhich = aIter.FirstWhich();
        while( nWhich )
        {
            if( ( nWhich < RES_CHRATR_END ||
                  RES_TXTATR_UNKNOWN_CONTAINER == nWhich ) &&
                ( SFX_ITEM_SET == pCFSet->GetItemState( nWhich, TRUE ) ) )
                rSet.Put( pCFSet->Get( nWhich ) );
            nWhich = aIter.NextWhich();
        }
    }
    else
        rSet.Put( rAttr );
}

void lcl_MergeAttr_ExpandChrFmt( SfxItemSet& rSet, const SfxPoolItem& rAttr )
{
    if( RES_TXTATR_CHARFMT == rAttr.Which() ||
        RES_TXTATR_INETFMT == rAttr.Which() ||
        RES_TXTATR_AUTOFMT == rAttr.Which() )
    {
        const SfxItemSet* pCFSet = CharFmt::GetItemSet( rAttr );

        if ( pCFSet )
        {
            SfxWhichIter aIter( *pCFSet );
            USHORT nWhich = aIter.FirstWhich();
            while( nWhich )
            {
                if( ( nWhich < RES_CHRATR_END ||
                      ( RES_TXTATR_AUTOFMT == rAttr.Which() && RES_TXTATR_UNKNOWN_CONTAINER == nWhich ) ) &&
                    ( SFX_ITEM_SET == pCFSet->GetItemState( nWhich, TRUE ) ) )
                    rSet.Put( pCFSet->Get( nWhich ) );
                nWhich = aIter.NextWhich();
            }
        }
    }

    // aufnehmen als MergeWert (falls noch nicht gesetzt neu setzen!)

/* wenn mehrere Attribute ueberlappen gewinnt der letze !!
 z.B
            1234567890123456789
              |------------| 		Font1
                 |------|			Font2
                    ^  ^
                    |--|		Abfragebereich: -> Gueltig ist Font2
*/
    rSet.Put( rAttr );
}

struct SwPoolItemEndPair
{
public:
    const SfxPoolItem* mpItem;
    xub_StrLen mnEndPos;

    SwPoolItemEndPair() : mpItem( 0 ), mnEndPos( 0 ) {};
};

// --> OD 2008-01-16 #newlistlevelattrs#
void lcl_MergeListLevelIndentAsLRSpaceItem( const SwTxtNode& rTxtNode,
                                            SfxItemSet& rSet )
{
    if ( rTxtNode.AreListLevelIndentsApplicable() )
    {
        const SwNumRule* pRule = rTxtNode.GetNumRule();
        if ( pRule && rTxtNode.GetActualListLevel() >= 0 )
        {
            const SwNumFmt& rFmt = pRule->Get(static_cast<USHORT>(rTxtNode.GetActualListLevel()));
            if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT )
            {
                SvxLRSpaceItem aLR( RES_LR_SPACE );
                aLR.SetTxtLeft( rFmt.GetIndentAt() );
                aLR.SetTxtFirstLineOfst( static_cast<short>(rFmt.GetFirstLineIndent()) );
                rSet.Put( aLR );
            }
        }
    }
}

// erfrage die Attribute vom TextNode ueber den Bereich
// --> OD 2008-01-16 #newlistlevelattrs#
BOOL SwTxtNode::GetAttr( SfxItemSet& rSet, xub_StrLen nStt, xub_StrLen nEnd,
                         BOOL bOnlyTxtAttr, BOOL bGetFromChrFmt,
                         const bool bMergeIndentValuesOfNumRule ) const
{
    if( HasHints() )
    {
        /* stelle erstmal fest, welche Text-Attribut in dem Bereich gueltig
         * sind. Dabei gibt es folgende Faelle:
         * 	UnEindeutig wenn: (wenn != Format-Attribut)
         * 		- das Attribut liegt vollstaendig im Bereich
         * 		- das Attributende liegt im Bereich
         * 		- der Attributanfang liegt im Bereich:
         * Eindeutig (im Set mergen):
         *		- das Attrib umfasst den Bereich
         * nichts tun:
         * 		das Attribut liegt ausserhalb des Bereiches
         */

        void (*fnMergeAttr)( SfxItemSet&, const SfxPoolItem& )
            = bGetFromChrFmt ? &lcl_MergeAttr_ExpandChrFmt
                             : &lcl_MergeAttr;

        // dann besorge mal die Auto-(Fmt)Attribute
        SfxItemSet aFmtSet( *rSet.GetPool(), rSet.GetRanges() );
        if( !bOnlyTxtAttr )
        {
            SwCntntNode::GetAttr( aFmtSet );
            // --> OD 2008-01-16 #newlistlevelattrs#
            if ( bMergeIndentValuesOfNumRule )
            {
                lcl_MergeListLevelIndentAsLRSpaceItem( *this, aFmtSet );
            }
            // <--
        }

        const USHORT nSize = m_pSwpHints->Count();

        if( nStt == nEnd )             // kein Bereich:
        {
            for (USHORT n = 0; n < nSize; ++n)
            {
                const SwTxtAttr* pHt = (*m_pSwpHints)[n];
                const xub_StrLen nAttrStart = *pHt->GetStart();
                if( nAttrStart > nEnd ) 		// ueber den Bereich hinaus
                    break;

                const xub_StrLen* pAttrEnd = pHt->GetEnd();
                if ( ! pAttrEnd ) // no attributes without end
                    continue;

                if( ( nAttrStart < nStt &&
                        ( pHt->DontExpand() ? nStt < *pAttrEnd
                                            : nStt <= *pAttrEnd )) ||
                    ( nStt == nAttrStart &&
                        ( nAttrStart == *pAttrEnd || !nStt )))
                    (*fnMergeAttr)( rSet, pHt->GetAttr() );
            }
        }
        else							// es ist ein Bereich definiert
        {
            // --> FME 2007-03-13 #i75299#
            ::std::auto_ptr< std::vector< SwPoolItemEndPair > > pAttrArr;
            // <--

            const USHORT coArrSz = static_cast<USHORT>(RES_TXTATR_WITHEND_END) -
                                   static_cast<USHORT>(RES_CHRATR_BEGIN);

            for (USHORT n = 0; n < nSize; ++n)
            {
                const SwTxtAttr* pHt = (*m_pSwpHints)[n];
                const xub_StrLen nAttrStart = *pHt->GetStart();
                if( nAttrStart > nEnd ) 		// ueber den Bereich hinaus
                    break;

                const xub_StrLen* pAttrEnd = pHt->GetEnd();
                if ( ! pAttrEnd ) // no attributes without end
                    continue;

                BOOL bChkInvalid = FALSE;
                if( nAttrStart <= nStt )       // vor oder genau Start
                {
                    if( *pAttrEnd <= nStt )    // liegt davor
                        continue;

                    if( nEnd <= *pAttrEnd )		// hinter oder genau Ende
                        (*fnMergeAttr)( aFmtSet, pHt->GetAttr() );
                    else
//					else if( pHt->GetAttr() != aFmtSet.Get( pHt->Which() ) )
                        // uneindeutig
                        bChkInvalid = TRUE;
                }
                else if( nAttrStart < nEnd 		// reicht in den Bereich
)//						 && pHt->GetAttr() != aFmtSet.Get( pHt->Which() ) )
                    bChkInvalid = TRUE;

                if( bChkInvalid )
                {
                    // uneindeutig ?
                    ::std::auto_ptr< SfxItemIter > pItemIter;
                    const SfxPoolItem* pItem = 0;

                    if ( RES_TXTATR_AUTOFMT == pHt->Which() )
                    {
                        const SfxItemSet* pAutoSet = CharFmt::GetItemSet( pHt->GetAttr() );
                        if ( pAutoSet )
                        {
                            pItemIter.reset( new SfxItemIter( *pAutoSet ) );
                            pItem = pItemIter->GetCurItem();
                        }
                    }
                    else
                        pItem = &pHt->GetAttr();

                    const USHORT nHintEnd = *pAttrEnd;

                    while ( pItem )
                    {
                        const USHORT nHintWhich = pItem->Which();
                        ASSERT(!isUNKNOWNATR(nHintWhich),
                                "SwTxtNode::GetAttr(): unkonwn attribute?");

                        if ( !pAttrArr.get() )
                        {
                            pAttrArr.reset(
                                new std::vector< SwPoolItemEndPair >(coArrSz));
                        }

                        std::vector< SwPoolItemEndPair >::iterator pPrev = pAttrArr->begin();
                        if (isCHRATR(nHintWhich) ||
                            isTXTATR_WITHEND(nHintWhich))
                        {
                            pPrev += nHintWhich - RES_CHRATR_BEGIN;
                        }
                        else
                        {
                            pPrev = pAttrArr->end();
                        }

#if OSL_DEBUG_LEVEL > 1
                        SwPoolItemEndPair aTmp = *pPrev;
#endif

                        if( pPrev != pAttrArr->end() )
                        {
                            if( !pPrev->mpItem )
                            {
                                if ( bOnlyTxtAttr || *pItem != aFmtSet.Get( nHintWhich ) )
                                {
                                    if( nAttrStart > nStt )
                                    {
                                        rSet.InvalidateItem( nHintWhich );
                                        pPrev->mpItem = (SfxPoolItem*)-1;
                                    }
                                    else
                                    {
                                        pPrev->mpItem = pItem;
                                        pPrev->mnEndPos = nHintEnd;
                                    }
                                }
                            }
                            else if( (SfxPoolItem*)-1 != pPrev->mpItem )
                            {
                                if( pPrev->mnEndPos == nAttrStart &&
                                    *pPrev->mpItem == *pItem )
                                {
                                    pPrev->mpItem = pItem;
                                    pPrev->mnEndPos = nHintEnd;
                                }
                                else
                                {
                                    rSet.InvalidateItem( nHintWhich );
                                    pPrev->mpItem = (SfxPoolItem*)-1;
                                }
                            }
                        }

                        pItem = ( pItemIter.get() && !pItemIter->IsAtEnd() )
                                    ? pItemIter->NextItem() : 0;
                    } // end while
                }
            }

            if ( pAttrArr.get() )
            {
                for (USHORT n = 0; n < coArrSz; ++n)
                {
                    const SwPoolItemEndPair& rItemPair = (*pAttrArr)[ n ];
                    if( (0 != rItemPair.mpItem) && ((SfxPoolItem*)-1 != rItemPair.mpItem) )
                    {
                        const USHORT nWh =
                            static_cast<USHORT>(n + RES_CHRATR_BEGIN);

                        if( nEnd <= rItemPair.mnEndPos ) // hinter oder genau Ende
                        {
                            if( *rItemPair.mpItem != aFmtSet.Get( nWh ) )
                                (*fnMergeAttr)( rSet, *rItemPair.mpItem );
                        }
                        else
                            // uneindeutig
                            rSet.InvalidateItem( nWh );
                    }
                }
            }
        }
        if( aFmtSet.Count() )
        {
            // aus dem Format-Set alle entfernen, die im TextSet auch gesetzt sind
            aFmtSet.Differentiate( rSet );
            // jetzt alle zusammen "mergen"
            rSet.Put( aFmtSet );
        }
    }
    else if( !bOnlyTxtAttr )
    {
        // dann besorge mal die Auto-(Fmt)Attribute
        SwCntntNode::GetAttr( rSet );
        // --> OD 2008-01-16 #newlistlevelattrs#
        if ( bMergeIndentValuesOfNumRule )
        {
            lcl_MergeListLevelIndentAsLRSpaceItem( *this, rSet );
        }
        // <--
    }

    return rSet.Count() ? TRUE : FALSE;
}

int lcl_IsNewAttrInSet( const SwpHints& rHints, const SfxPoolItem& rItem,
                        const xub_StrLen nEnd )
{
    int bIns = TRUE;
    for( USHORT i = 0; i < rHints.Count(); ++i )
    {
        const SwTxtAttr *pOther = rHints[ i ];
        if( *pOther->GetStart() )
            break;

        if( pOther->GetEnd() &&
            *pOther->GetEnd() == nEnd &&
            ( pOther->IsCharFmtAttr() || pOther->Which() == rItem.Which() ) )
        {
            bIns = FALSE;
            break;
        }
    }
    return bIns;
}

void SwTxtNode::FmtToTxtAttr( SwTxtNode* pNd )
{
    SfxItemSet aThisSet( GetDoc()->GetAttrPool(), aCharFmtSetRange );
    if( HasSwAttrSet() && GetpSwAttrSet()->Count() )
        aThisSet.Put( *GetpSwAttrSet() );

    GetOrCreateSwpHints();

    if( pNd == this )
    {
        if( aThisSet.Count() )
        {
            SfxItemIter aIter( aThisSet );
            const SfxPoolItem* pItem = aIter.GetCurItem();
            std::vector<USHORT> aClearWhichIds;

            while ( true )
            {
                if (lcl_IsNewAttrInSet( *m_pSwpHints, *pItem, GetTxt().Len() ))
                {
                    m_pSwpHints->SwpHintsArray::Insert(
                        MakeTxtAttr( *GetDoc(),
                            const_cast<SfxPoolItem&>(*pItem),
                            0, GetTxt().Len() ) );
                    aClearWhichIds.push_back( pItem->Which() );
                }

                if( aIter.IsAtEnd() )
                    break;
                pItem = aIter.NextItem();
            }

            ClearItemsFromAttrSet( aClearWhichIds );
        }
    }
    else
    {
        SfxItemSet aNdSet( pNd->GetDoc()->GetAttrPool(), aCharFmtSetRange );
        if( pNd->HasSwAttrSet() && pNd->GetpSwAttrSet()->Count() )
            aNdSet.Put( *pNd->GetpSwAttrSet() );

        pNd->GetOrCreateSwpHints();

        if( aThisSet.Count() )
        {
            SfxItemIter aIter( aThisSet );
            const SfxPoolItem* pItem = aIter.GetCurItem(), *pNdItem;
            std::vector<USHORT> aClearWhichIds;

            while( TRUE )
            {
                if( ( SFX_ITEM_SET != aNdSet.GetItemState( pItem->Which(), FALSE,
                    &pNdItem ) || *pItem != *pNdItem ) &&
                    lcl_IsNewAttrInSet( *m_pSwpHints, *pItem, GetTxt().Len() ) )
                {
                    m_pSwpHints->SwpHintsArray::Insert( MakeTxtAttr( *GetDoc(),
                        const_cast<SfxPoolItem&>(*pItem),
                        0, GetTxt().Len() ) );
                    aClearWhichIds.push_back( pItem->Which() );
                }
                aNdSet.ClearItem( pItem->Which() );

                if( aIter.IsAtEnd() )
                    break;
                pItem = aIter.NextItem();
            }
            ClearItemsFromAttrSet( aClearWhichIds );
        }

        if( aNdSet.Count() )
        {
            SfxItemIter aIter( aNdSet );
            const SfxPoolItem* pItem = aIter.GetCurItem();
            std::vector<USHORT> aClearWhichIds;

            while ( true )
            {
                if ( lcl_IsNewAttrInSet( *pNd->m_pSwpHints, *pItem,
                        pNd->GetTxt().Len() ) )
                {
                    pNd->m_pSwpHints->SwpHintsArray::Insert(
                            MakeTxtAttr( *pNd->GetDoc(),
                                         const_cast<SfxPoolItem&>(*pItem),
                                         0, pNd->GetTxt().Len() ) );
                }
                aClearWhichIds.push_back( pItem->Which() );

                if( aIter.IsAtEnd() )
                    break;
                pItem = aIter.NextItem();
            }

            pNd->ClearItemsFromAttrSet( aClearWhichIds );
            SwFmtChg aTmp1( pNd->GetFmtColl() );
            pNd->SwModify::Modify( &aTmp1, &aTmp1 );
        }
    }

    SetCalcHiddenCharFlags();

    pNd->TryDeleteSwpHints();
}

/*************************************************************************
 *						SwpHints::CalcFlags()
 *************************************************************************/

void SwpHints::CalcFlags()
{
    m_bDDEFields = m_bFootnote = false;
    const USHORT nSize = Count();
    const SwTxtAttr* pAttr;
    for( USHORT nPos = 0; nPos < nSize; ++nPos )
    {
        switch( ( pAttr = (*this)[ nPos ])->Which() )
        {
        case RES_TXTATR_FTN:
            m_bFootnote = true;
            if ( m_bDDEFields )
                return;
            break;
        case RES_TXTATR_FIELD:
            {
                const SwField* pFld = pAttr->GetFld().GetFld();
                if( RES_DDEFLD == pFld->GetTyp()->Which() )
                {
                    m_bDDEFields = true;
                    if ( m_bFootnote )
                        return;
                }
            }
            break;
        }
    }
}

/*************************************************************************
 *						SwpHints::CalcVisibleFlag()
 *************************************************************************/

bool SwpHints::CalcHiddenParaField()
{
    m_bCalcHiddenParaField = false;
    bool bOldHasHiddenParaField = m_bHasHiddenParaField;
    bool bNewHasHiddenParaField  = false;
    const USHORT	nSize = Count();
    const SwTxtAttr *pTxtHt;

    for( USHORT nPos = 0; nPos < nSize; ++nPos )
    {
        pTxtHt = (*this)[ nPos ];
        const USHORT nWhich = pTxtHt->Which();

        if( RES_TXTATR_FIELD == nWhich )
        {
            const SwFmtFld& rFld = pTxtHt->GetFld();
            if( RES_HIDDENPARAFLD == rFld.GetFld()->GetTyp()->Which() )
            {
                if( !((SwHiddenParaField*)rFld.GetFld())->IsHidden() )
                {
                    SetHiddenParaField(false);
                    return bOldHasHiddenParaField != bNewHasHiddenParaField;
                }
                else
                {
                    bNewHasHiddenParaField = true;
                }
            }
        }
    }
    SetHiddenParaField( bNewHasHiddenParaField );
    return bOldHasHiddenParaField != bNewHasHiddenParaField;
}


/*************************************************************************
 *						SwpHints::NoteInHistory()
 *************************************************************************/

void SwpHints::NoteInHistory( SwTxtAttr *pAttr, const bool bNew )
{
    if ( m_pHistory ) { m_pHistory->AddHint( pAttr, bNew ); }
}

/*************************************************************************
 *                      SwpHints::MergePortions( )
 *************************************************************************/

bool SwpHints::MergePortions( SwTxtNode& rNode )
{
    if ( !Count() )
        return false;

    // sort before merging
    SwpHintsArray::Resort();

    bool bRet = false;
    typedef std::multimap< int, SwTxtAttr* > PortionMap;
    PortionMap aPortionMap;
    xub_StrLen nLastPorStart = STRING_LEN;
    USHORT i = 0;
    int nKey = 0;

    // get portions by start position:
    for ( i = 0; i < Count(); ++i )
    {
        SwTxtAttr *pHt = GetTextHint( i );
        if ( RES_TXTATR_CHARFMT != pHt->Which() &&
             RES_TXTATR_AUTOFMT != pHt->Which() )
             //&&
             //RES_TXTATR_INETFMT != pHt->Which() )
            continue;

        const xub_StrLen nPorStart = *pHt->GetStart();
        if ( nPorStart != nLastPorStart && nLastPorStart != STRING_LEN )
            ++nKey;
        nLastPorStart = nPorStart;
        aPortionMap.insert( std::pair< const int, SwTxtAttr* >( nKey, pHt ) );
    }

    // check if portion i can be merged with portion i+1:
    i = 0;
    int j = i + 1;
    while ( i <= nKey )
    {
        std::pair< PortionMap::iterator, PortionMap::iterator > aRange1 = aPortionMap.equal_range( i );
        std::pair< PortionMap::iterator, PortionMap::iterator > aRange2 = aPortionMap.equal_range( j );
        PortionMap::iterator aIter1 = aRange1.first;
        PortionMap::iterator aIter2 = aRange2.first;

        bool bMerge = true;
        const USHORT nAttributesInPor1  = static_cast<USHORT>(std::distance( aRange1.first, aRange1.second ));
        const USHORT nAttributesInPor2  = static_cast<USHORT>(std::distance( aRange2.first, aRange2.second ));

        if ( nAttributesInPor1 == nAttributesInPor2 && nAttributesInPor1 != 0 )
        {
            while ( aIter1 != aRange1.second )
            {
                const SwTxtAttr* p1 = (*aIter1).second;
                const SwTxtAttr* p2 = (*aIter2).second;
                if ( *p1->GetEnd() < *p2->GetStart() || p1->Which() != p2->Which() || !(*p1 == *p2) )
                {
                    bMerge = false;
                    break;
                }
                ++aIter1;
                ++aIter2;
            }
        }
        else
        {
            bMerge = false;
        }

        if ( bMerge )
        {
            // erase all elements with key i + 1
            xub_StrLen nNewPortionEnd = 0;
            for ( aIter2 = aRange2.first; aIter2 != aRange2.second; ++aIter2 )
            {
                SwTxtAttr* p2 = (*aIter2).second;
                nNewPortionEnd = *p2->GetEnd();

                const USHORT nCountBeforeDelete = Count();
                Delete( p2 );

                // robust: check if deletion actually took place before destroying attribute:
                if ( Count() < nCountBeforeDelete )
                    rNode.DestroyAttr( p2 );
            }
            aPortionMap.erase( aRange2.first, aRange2.second );
            ++j;

            // change all attributes with key i
            aRange1 = aPortionMap.equal_range( i );
            for ( aIter1 = aRange1.first; aIter1 != aRange1.second; ++aIter1 )
            {
                SwTxtAttr* p1 = (*aIter1).second;
                NoteInHistory( p1 );
                *p1->GetEnd() = nNewPortionEnd;
                NoteInHistory( p1, true );
                bRet = true;
            }
        }
        else
        {
            ++i;
            j = i + 1;
        }
    }

    if ( bRet )
    {
        SwpHintsArray::Resort();
    }

    return bRet;
}

// check if there is already a character format and adjust the sort numbers
void lcl_CheckSortNumber( const SwpHints& rHints, SwTxtCharFmt& rNewCharFmt )
{
    const xub_StrLen nHtStart = *rNewCharFmt.GetStart();
    const xub_StrLen nHtEnd   = *rNewCharFmt.GetEnd();
    USHORT nSortNumber = 0;

    for ( USHORT i = 0; i < rHints.Count(); ++i )
    {
        const SwTxtAttr* pOtherHt = rHints[i];

        const xub_StrLen nOtherStart = *pOtherHt->GetStart();

        if ( nOtherStart > nHtStart )
            break;

        if ( RES_TXTATR_CHARFMT == pOtherHt->Which() )
        {
            const xub_StrLen nOtherEnd = *pOtherHt->GetEnd();

            if ( nOtherStart == nHtStart && nOtherEnd == nHtEnd )
            {
                const USHORT nOtherSortNum = static_cast<const SwTxtCharFmt*>(pOtherHt)->GetSortNumber();
                nSortNumber = nOtherSortNum + 1;
            }
        }
    }

    if ( nSortNumber > 0 )
        rNewCharFmt.SetSortNumber( nSortNumber );
}

/*************************************************************************
 *						SwpHints::Insert()
 *************************************************************************/

/*
 * Try to insert the new hint.
 * Depending on the type of the hint, this either always succeeds, or may fail.
 * Depending on the type of the hint, other hints may be deleted or
 * overwritten.
 * The return value indicates successful insertion.
 */
bool SwpHints::TryInsertHint( SwTxtAttr* const pHint, SwTxtNode &rNode,
        const SetAttrMode nMode )
{
    if ( USHRT_MAX == Count() ) // we're sorry, this flight is overbooked...
    {
        ASSERT(false, "hints array full :-(");
        return false;
    }

    // Felder bilden eine Ausnahme:
    // 1) Sie koennen nie ueberlappen
    // 2) Wenn zwei Felder genau aneinander liegen,
    //	  sollen sie nicht zu einem verschmolzen werden.
    // Wir koennen also auf die while-Schleife verzichten

    xub_StrLen *pHtEnd = pHint->GetEnd();
    USHORT nWhich = pHint->Which();

    switch( nWhich )
    {
    case RES_TXTATR_CHARFMT:
    {
        // Check if character format contains hidden attribute:
        const SwCharFmt* pFmt = pHint->GetCharFmt().GetCharFmt();
        const SfxPoolItem* pItem;
        if ( SFX_ITEM_SET == pFmt->GetItemState( RES_CHRATR_HIDDEN, TRUE, &pItem ) )
            rNode.SetCalcHiddenCharFlags();

        ((SwTxtCharFmt*)pHint)->ChgTxtNode( &rNode );
        break;
    }
    // --> FME 2007-03-16 #i75430# Recalc hidden flags if necessary
    case RES_TXTATR_AUTOFMT:
    {
        // Check if auto style contains hidden attribute:
        const SfxPoolItem* pHiddenItem = CharFmt::GetItem( *pHint, RES_CHRATR_HIDDEN );
        if ( pHiddenItem )
            rNode.SetCalcHiddenCharFlags();
        break;
    }
    // <--
    case RES_TXTATR_INETFMT:
        lcl_InitINetFmt(rNode, static_cast<SwTxtINetFmt*>(pHint));
        break;
    case RES_TXTATR_FIELD:
        {
            BOOL bDelFirst = 0 != ((SwTxtFld*)pHint)->GetpTxtNode();
            ((SwTxtFld*)pHint)->ChgTxtNode( &rNode );
            SwDoc* pDoc = rNode.GetDoc();
            const SwField* pFld = ((SwTxtFld*)pHint)->GetFld().GetFld();

            if( !pDoc->IsNewFldLst() )
            {
                // was fuer ein Feld ist es denn ??
                // bestimmte Felder mussen am Doc das Calculations-Flag updaten
                switch( pFld->GetTyp()->Which() )
                {
                case RES_DBFLD:
                case RES_SETEXPFLD:
                case RES_HIDDENPARAFLD:
                case RES_HIDDENTXTFLD:
                case RES_DBNUMSETFLD:
                case RES_DBNEXTSETFLD:
                    {
                        if( bDelFirst )
                            pDoc->InsDelFldInFldLst( FALSE, *(SwTxtFld*)pHint );
                        if( rNode.GetNodes().IsDocNodes() )
                            pDoc->InsDelFldInFldLst( TRUE, *(SwTxtFld*)pHint );
                    }
                    break;
                case RES_DDEFLD:
                    if( rNode.GetNodes().IsDocNodes() )
                        ((SwDDEFieldType*)pFld->GetTyp())->IncRefCnt();
                    break;
                }
            }

            // gehts ins normale Nodes-Array?
            if( rNode.GetNodes().IsDocNodes() )
            {
                BOOL bInsFldType = FALSE;
                switch( pFld->GetTyp()->Which() )
                {
                case RES_SETEXPFLD:
                    bInsFldType = ((SwSetExpFieldType*)pFld->GetTyp())->IsDeleted();
                    if( nsSwGetSetExpType::GSE_SEQ & ((SwSetExpFieldType*)pFld->GetTyp())->GetType() )
                    {
                        // bevor die ReferenzNummer gesetzt wird, sollte
                        // das Feld am richtigen FeldTypen haengen!
                        SwSetExpFieldType* pFldType = (SwSetExpFieldType*)
                                    pDoc->InsertFldType( *pFld->GetTyp() );
                        if( pFldType != pFld->GetTyp() )
                        {
                            SwFmtFld* pFmtFld = (SwFmtFld*)&((SwTxtFld*)pHint)
                                                                ->GetFld();
                            pFldType->Add( pFmtFld );          // ummelden
                            pFmtFld->GetFld()->ChgTyp( pFldType );
                        }
                        pFldType->SetSeqRefNo( *(SwSetExpField*)pFld );
                    }
                    break;
                case RES_USERFLD:
                    bInsFldType = ((SwUserFieldType*)pFld->GetTyp())->IsDeleted();
                    break;

                case RES_DDEFLD:
                    if( pDoc->IsNewFldLst() )
                        ((SwDDEFieldType*)pFld->GetTyp())->IncRefCnt();
                    bInsFldType = ((SwDDEFieldType*)pFld->GetTyp())->IsDeleted();
                    break;

                case RES_POSTITFLD:
                    if ( pDoc->GetDocShell() )
                        pDoc->GetDocShell()->Broadcast( SwFmtFldHint( &((SwTxtFld*)pHint)->GetFld(), SWFMTFLD_INSERTED ) );
                    break;
                }
                if( bInsFldType )
                    pDoc->InsDeletedFldType( *pFld->GetTyp() );
            }
        }
        break;
    case RES_TXTATR_FTN :
        ((SwTxtFtn*)pHint)->ChgTxtNode( &rNode );
        break;
    case RES_TXTATR_REFMARK:
        ((SwTxtRefMark*)pHint)->ChgTxtNode( &rNode );
        if( rNode.GetNodes().IsDocNodes() )
        {
            // search for a reference with the same name
            SwTxtAttr* pTmpHt;
            xub_StrLen *pTmpHtEnd, *pTmpHintEnd;
            for( USHORT n = 0, nEnd = Count(); n < nEnd; ++n )
            {
                if (RES_TXTATR_REFMARK == (pTmpHt = GetTextHint(n))->Which() &&
                    pHint->GetAttr() == pTmpHt->GetAttr() &&
                    0 != ( pTmpHtEnd = pTmpHt->GetEnd() ) &&
                    0 != ( pTmpHintEnd = pHint->GetEnd() ) )
                {
                    SwComparePosition eCmp = ::ComparePosition(
                            *pTmpHt->GetStart(), *pTmpHtEnd,
                            *pHint->GetStart(), *pTmpHintEnd );
                    BOOL bDelOld = TRUE, bChgStart = FALSE, bChgEnd = FALSE;
                    switch( eCmp )
                    {
                    case POS_BEFORE:
                    case POS_BEHIND:	bDelOld = FALSE; break;

                    case POS_OUTSIDE:	bChgStart = bChgEnd = TRUE; break;

                    case POS_COLLIDE_END:
                    case POS_OVERLAP_BEFORE:	bChgStart = TRUE; break;
                    case POS_COLLIDE_START:
                    case POS_OVERLAP_BEHIND:    bChgEnd = TRUE; break;
                    default: break;
                    }

                    if( bChgStart )
                        *pHint->GetStart() = *pTmpHt->GetStart();
                    if( bChgEnd )
                        *pTmpHintEnd = *pTmpHtEnd;

                    if( bDelOld )
                    {
                        NoteInHistory( pTmpHt );
                        rNode.DestroyAttr( Cut( n-- ) );
                        --nEnd;
                    }
                }
            }
        }
        break;
    case RES_TXTATR_TOXMARK:
        ((SwTxtTOXMark*)pHint)->ChgTxtNode( &rNode );
        break;

    case RES_TXTATR_CJK_RUBY:
        lcl_InitRuby(rNode, static_cast<SwTxtRuby*>(pHint));
        break;

    case RES_TXTATR_META:
    case RES_TXTATR_METAFIELD:
        static_cast<SwTxtMeta *>(pHint)->ChgTxtNode( &rNode );
        break;

    case RES_CHRATR_HIDDEN:
        rNode.SetCalcHiddenCharFlags();
        break;
    }

    if( nsSetAttrMode::SETATTR_DONTEXPAND & nMode )
        pHint->SetDontExpand( TRUE );

    // SwTxtAttrs ohne Ende werden sonderbehandelt:
    // Sie werden natuerlich in das Array insertet, aber sie werden nicht
    // in die pPrev/Next/On/Off-Verkettung aufgenommen.
    // Der Formatierer erkennt diese TxtHints an dem CH_TXTATR_.. im Text !
    xub_StrLen nHtStart = *pHint->GetStart();
    if( !pHtEnd )
    {
        SwpHintsArray::Insert( pHint );
        CalcFlags();
#ifdef DBG_UTIL
        if( !rNode.GetDoc()->IsInReading() )
            CHECK;
#endif
        // ... und die Abhaengigen benachrichtigen
        if ( rNode.GetDepends() )
        {
            SwUpdateAttr aHint( nHtStart, nHtStart, nWhich );
            rNode.Modify( 0, &aHint );
        }
        return true;
    }

    // ----------------------------------------------------------------
    // Ab hier gibt es nur noch pHint mit einem EndIdx !!!

    if( *pHtEnd < nHtStart )
    {
        ASSERT( *pHtEnd >= nHtStart,
                    "+SwpHints::Insert: invalid hint, end < start" );

        // Wir drehen den Quatsch einfach um:
        *pHint->GetStart() = *pHtEnd;
        *pHtEnd = nHtStart;
        nHtStart = *pHint->GetStart();
    }

    // I need this value later on for notification but the pointer may become invalid
    const xub_StrLen nHintEnd = *pHtEnd;
    const bool bNoHintAdjustMode = (nsSetAttrMode::SETATTR_NOHINTADJUST & nMode);

    // handle nesting attributes: inserting may fail due to overlap!
    if (pHint->IsNesting())
    {
        const bool bRet(
            TryInsertNesting(rNode, *static_cast<SwTxtAttrNesting*>(pHint)));
        if (!bRet) return false;
    }
    // Currently REFMARK and TOXMARK have OverlapAllowed set to true.
    // These attributes may be inserted directly.
    // Also attributes without length may be inserted directly.
    // SETATTR_NOHINTADJUST is set e.g., during undo.
    // Portion building in not necessary during XML import.
    else
    if ( !bNoHintAdjustMode &&
         !pHint->IsOverlapAllowedAttr() &&
         !rNode.GetDoc()->IsInXMLImport() &&
         ( RES_TXTATR_AUTOFMT == nWhich ||
           RES_TXTATR_CHARFMT == nWhich ) )
    {
        ASSERT( nWhich != RES_TXTATR_AUTOFMT ||
                static_cast<const SwFmtAutoFmt&>(pHint->GetAttr()).GetStyleHandle()->GetPool() ==
                &rNode.GetDoc()->GetAttrPool(),
                "AUTOSTYLES - Pool mismatch" )

        BuildPortions( rNode, *pHint, nMode );

        if ( nHtStart < nHintEnd ) // skip merging for 0-length attributes
            MergePortions( rNode );
    }
    else
    {
        // There may be more than one character style at the current position.
        // Take care of the sort number.
        // Special case ruby portion: During import, the ruby attribute is set
        // multiple times
        // Special case hyperlink: During import, the ruby attribute is set
        // multiple times
        // FME 2007-11-08 #i82989# in NOHINTADJUST mode, we want to insert
        // character attributes directly
        if ( ( RES_TXTATR_CHARFMT  == nWhich && !bNoHintAdjustMode ) )
        {
            BuildPortions( rNode, *pHint, nMode );
        }
        else
        {
            // --> FME 2007-11-08 #i82989# Check sort numbers in NoHintAdjustMode
            if ( RES_TXTATR_CHARFMT == nWhich )
                lcl_CheckSortNumber( *this, *static_cast<SwTxtCharFmt*>(pHint) );
            // <--

            SwpHintsArray::Insert( pHint );
            NoteInHistory( pHint, true );
        }
    }

    // ... und die Abhaengigen benachrichtigen
    if ( rNode.GetDepends() )
    {
        SwUpdateAttr aHint( nHtStart, nHtStart == nHintEnd ? nHintEnd + 1 : nHintEnd, nWhich );
        rNode.Modify( 0, &aHint );
    }

#ifdef DBG_UTIL
    if( !bNoHintAdjustMode && !rNode.GetDoc()->IsInReading() )
        CHECK;
#endif

    return true;
}

/*************************************************************************
 *						SwpHints::DeleteAtPos()
 *************************************************************************/

void SwpHints::DeleteAtPos( const USHORT nPos )
{
    SwTxtAttr *pHint = GetTextHint(nPos);
    // ChainDelete( pHint );
    NoteInHistory( pHint );
    SwpHintsArray::DeleteAtPos( nPos );

    if( RES_TXTATR_FIELD == pHint->Which() )
    {
        SwFieldType* pFldTyp = ((SwTxtFld*)pHint)->GetFld().GetFld()->GetTyp();
        if( RES_DDEFLD == pFldTyp->Which() )
        {
            const SwTxtNode* pNd = ((SwTxtFld*)pHint)->GetpTxtNode();
            if( pNd && pNd->GetNodes().IsDocNodes() )
                ((SwDDEFieldType*)pFldTyp)->DecRefCnt();
            ((SwTxtFld*)pHint)->ChgTxtNode( 0 );
        }
        else if( RES_POSTITFLD == pFldTyp->Which() )
        {
            const_cast<SwFmtFld&>(((SwTxtFld*)pHint)->GetFld()).Broadcast( SwFmtFldHint( &((SwTxtFld*)pHint)->GetFld(), SWFMTFLD_REMOVED ) );
        }
        else if ( m_bHasHiddenParaField &&
                 RES_HIDDENPARAFLD == pFldTyp->Which() )
        {
            m_bCalcHiddenParaField = true;
        }
    }

    CalcFlags();
    CHECK;
}

// Ist der Hint schon bekannt, dann suche die Position und loesche ihn.
// Ist er nicht im Array, so gibt es ein ASSERT !!

void SwpHints::Delete( SwTxtAttr* pTxtHt )
{
    // Attr 2.0: SwpHintsArr::Delete( pTxtHt );
    const USHORT nPos = GetStartOf( pTxtHt );
    ASSERT( USHRT_MAX != nPos, "Attribut nicht im Attribut-Array!" );
    if( USHRT_MAX != nPos )
        DeleteAtPos( nPos );
}

void SwTxtNode::ClearSwpHintsArr( bool bDelFields )
{
    if ( HasHints() )
    {
        USHORT nPos = 0;
        while ( nPos < m_pSwpHints->Count() )
        {
            SwTxtAttr* pDel = m_pSwpHints->GetTextHint( nPos );
            bool bDel = false;

            switch( pDel->Which() )
            {
            case RES_TXTATR_FLYCNT:
            case RES_TXTATR_FTN:
                break;

            case RES_TXTATR_FIELD:
                if( bDelFields )
                    bDel = true;
                break;
            default:
                bDel = true; break;
            }

            if( bDel )
            {
                m_pSwpHints->SwpHintsArray::DeleteAtPos( nPos );
                DestroyAttr( pDel );
            }
            else
                ++nPos;
        }
    }
}

USHORT SwTxtNode::GetLang( const xub_StrLen nBegin, const xub_StrLen nLen,
                           USHORT nScript ) const
{
    USHORT nRet = LANGUAGE_DONTKNOW;

    if ( ! nScript )
    {
        nScript = pBreakIt->GetRealScriptOfText( m_Text, nBegin );
    }

    // --> FME 2008-09-29 #i91465# hennerdrewes: Consider nScript if pSwpHints == 0
    const USHORT nWhichId = GetWhichOfScript( RES_CHRATR_LANGUAGE, nScript );
    // <--

    if ( HasHints() )
    {
        const xub_StrLen nEnd = nBegin + nLen;
        for ( USHORT i = 0, nSize = m_pSwpHints->Count(); i < nSize; ++i )
        {
            // ist der Attribut-Anfang schon groesser als der Idx ?
            const SwTxtAttr *pHt = m_pSwpHints->operator[](i);
            const xub_StrLen nAttrStart = *pHt->GetStart();
            if( nEnd < nAttrStart )
                break;

            const USHORT nWhich = pHt->Which();

            if( nWhichId == nWhich ||
                    ( ( pHt->IsCharFmtAttr() || RES_TXTATR_AUTOFMT == nWhich ) && CharFmt::IsItemIncluded( nWhichId, pHt ) ) )
            {
                const xub_StrLen *pEndIdx = pHt->GetEnd();
                // Ueberlappt das Attribut den Bereich?

                if( pEndIdx &&
                    nLen ? ( nAttrStart < nEnd && nBegin < *pEndIdx )
                         : (( nAttrStart < nBegin &&
                                ( pHt->DontExpand() ? nBegin < *pEndIdx
                                                    : nBegin <= *pEndIdx )) ||
                            ( nBegin == nAttrStart &&
                                ( nAttrStart == *pEndIdx || !nBegin ))) )
                {
                    const SfxPoolItem* pItem = CharFmt::GetItem( *pHt, nWhichId );
                    USHORT nLng = ((SvxLanguageItem*)pItem)->GetLanguage();

                    // Umfasst das Attribut den Bereich komplett?
                    if( nAttrStart <= nBegin && nEnd <= *pEndIdx )
                        nRet = nLng;
                    else if( LANGUAGE_DONTKNOW == nRet )
                        nRet = nLng; // partielle Ueberlappung, der 1. gewinnt
                }
            }
        }
    }
    if( LANGUAGE_DONTKNOW == nRet )
    {
        nRet = ((SvxLanguageItem&)GetSwAttrSet().Get( nWhichId )).GetLanguage();
        if( LANGUAGE_DONTKNOW == nRet )
            nRet = static_cast<USHORT>(GetAppLanguage());
    }
    return nRet;
}


sal_Unicode GetCharOfTxtAttr( const SwTxtAttr& rAttr )
{
    sal_Unicode cRet = CH_TXTATR_BREAKWORD;
    switch ( rAttr.Which() )
    {
        case RES_TXTATR_FTN:
        case RES_TXTATR_REFMARK:
        case RES_TXTATR_TOXMARK:
        case RES_TXTATR_META:
        case RES_TXTATR_METAFIELD:
            cRet = CH_TXTATR_INWORD;
            break;

        case RES_TXTATR_FIELD:
        case RES_TXTATR_FLYCNT:
            cRet = CH_TXTATR_BREAKWORD;
            break;

        default:
            ASSERT(false, "GetCharOfTxtAttr: unknown attr");
            break;
    }
    return cRet;
}


