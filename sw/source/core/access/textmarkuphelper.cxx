/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: textmarkuphelper.cxx,v $
 * $Revision: 1.3 $
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

#include "precompiled_sw.hxx"

#include <textmarkuphelper.hxx>
#include <accportions.hxx>

#include <vector>
#include <algorithm>
#include <comphelper/stlunosequence.hxx>

#include "errhdl.hxx"

#include <com/sun/star/text/TextMarkupType.hpp>
#include <com/sun/star/accessibility/TextSegment.hpp>

#include <ndtxt.hxx>
#include <wrong.hxx>

using namespace com::sun::star;

// helper functions
namespace {
    const SwWrongList* getTextMarkupList( const SwTxtNode& rTxtNode,
                                          const sal_Int32 nTextMarkupType )
        throw (::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::uno::RuntimeException)
    {
        const SwWrongList* pTextMarkupList( 0 );
        switch ( nTextMarkupType )
        {
            case text::TextMarkupType::SPELLCHECK:
            {
                pTextMarkupList = rTxtNode.GetWrong();
            }
            break;
            case text::TextMarkupType::PROOFREADING:
            {
                // support not implemented yet
                pTextMarkupList = 0;
            }
            break;
            case text::TextMarkupType::SMARTTAG:
            {
                // support not implemented yet
                pTextMarkupList = 0;
            }
            break;
            default:
            {
                throw lang::IllegalArgumentException();
            }
        }

        return pTextMarkupList;
    }
}

// implementation of calss <SwTextMarkupoHelper>
SwTextMarkupHelper::SwTextMarkupHelper( const SwTxtNode& rTxtNode,
                                        const SwAccessiblePortionData& rPortionData )
    : mrTxtNode( rTxtNode ),
      mrPortionData( rPortionData )
{
}

sal_Int32 SwTextMarkupHelper::getTextMarkupCount( sal_Int32 nTextMarkupType )
        throw (::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nTextMarkupCount( 0 );

    const SwWrongList* pTextMarkupList =
                            getTextMarkupList( mrTxtNode, nTextMarkupType );
    if ( pTextMarkupList )
    {
        nTextMarkupCount = pTextMarkupList->Count();
    }

    return nTextMarkupCount;
}
::com::sun::star::accessibility::TextSegment
        SwTextMarkupHelper::getTextMarkup( sal_Int32 nTextMarkupIndex,
                                           sal_Int32 nTextMarkupType )
        throw (::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::uno::RuntimeException)
{
    if ( nTextMarkupIndex >= getTextMarkupCount( nTextMarkupType ) ||
         nTextMarkupIndex < 0 )
    {
        throw lang::IndexOutOfBoundsException();
    }

    ::com::sun::star::accessibility::TextSegment aTextMarkupSegment;
    aTextMarkupSegment.SegmentStart = -1;
    aTextMarkupSegment.SegmentEnd = -1;

    const SwWrongList* pTextMarkupList =
                            getTextMarkupList( mrTxtNode, nTextMarkupType );
    if ( pTextMarkupList )
    {
        const SwWrongArea* pTextMarkup =
                pTextMarkupList->GetElement( static_cast<USHORT>(nTextMarkupIndex) );
        if ( pTextMarkup )
        {
            const ::rtl::OUString rText = mrPortionData.GetAccessibleString();
            const sal_Int32 nStartPos =
                            mrPortionData.GetAccessiblePosition( pTextMarkup->mnPos );
            const sal_Int32 nEndPos =
                            mrPortionData.GetAccessiblePosition( pTextMarkup->mnPos + pTextMarkup->mnLen );
            aTextMarkupSegment.SegmentText = rText.copy( nStartPos, nEndPos - nStartPos );
            aTextMarkupSegment.SegmentStart = nStartPos;
            aTextMarkupSegment.SegmentEnd = nEndPos;
        }
        else
        {
            ASSERT( false,
                    "<SwTextMarkupHelper::getTextMarkup(..)> - missing <SwWrongArea> instance" );
        }
    }

    return aTextMarkupSegment;
}

::com::sun::star::uno::Sequence< ::com::sun::star::accessibility::TextSegment >
        SwTextMarkupHelper::getTextMarkupAtIndex( sal_Int32 nCharIndex,
                                                  sal_Int32 nTextMarkupType )
        throw (::com::sun::star::lang::IndexOutOfBoundsException,
               ::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::uno::RuntimeException)
{
    // assumption:
    // value of <nCharIndex> is in range [0..length of accessible text)

    const USHORT nCoreCharIndex = mrPortionData.GetModelPosition( nCharIndex );
    // Handling of portions with core length == 0 at the beginning of the
    // paragraph - e.g. numbering portion.
    if ( mrPortionData.GetAccessiblePosition( nCoreCharIndex ) > nCharIndex )
    {
        return uno::Sequence< ::com::sun::star::accessibility::TextSegment >();
    }

    ::std::vector< ::com::sun::star::accessibility::TextSegment > aTmpTextMarkups;
    const SwWrongList* pTextMarkupList =
                            getTextMarkupList( mrTxtNode, nTextMarkupType );
    if ( pTextMarkupList )
    {
        const ::rtl::OUString rText = mrPortionData.GetAccessibleString();

        const USHORT nTextMarkupCount = pTextMarkupList->Count();
        for ( USHORT nTextMarkupIdx = 0; nTextMarkupIdx < nTextMarkupCount; ++nTextMarkupIdx )
        {
            const SwWrongArea* pTextMarkup =
                    pTextMarkupList->GetElement( static_cast<USHORT>(nTextMarkupIdx) );
            ASSERT( pTextMarkup,
                    "<SwTextMarkupHelper::getTextMarkup(..)> - missing <SwWrongArea> instance" );
            if ( pTextMarkup &&
                 pTextMarkup->mnPos <= nCoreCharIndex &&
                 nCoreCharIndex < ( pTextMarkup->mnPos + pTextMarkup->mnLen ) )
            {
                const sal_Int32 nStartPos =
                    mrPortionData.GetAccessiblePosition( pTextMarkup->mnPos );
                const sal_Int32 nEndPos =
                    mrPortionData.GetAccessiblePosition( pTextMarkup->mnPos + pTextMarkup->mnLen );
                ::com::sun::star::accessibility::TextSegment aTextMarkupSegment;
                aTextMarkupSegment.SegmentText = rText.copy( nStartPos, nEndPos - nStartPos );
                aTextMarkupSegment.SegmentStart = nStartPos;
                aTextMarkupSegment.SegmentEnd = nEndPos;
                aTmpTextMarkups.push_back( aTextMarkupSegment );
            }
        }
    }

    uno::Sequence< ::com::sun::star::accessibility::TextSegment > aTextMarkups(
                                                    aTmpTextMarkups.size() );
    ::std::copy( aTmpTextMarkups.begin(), aTmpTextMarkups.end(),
                 ::comphelper::stl_begin( aTextMarkups ) );

    return aTextMarkups;
}
