/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: styles.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 10:00:20 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#ifndef WW_WWSTYLES_HXX
#include "../inc/wwstyles.hxx"
#endif

#include <functional>               //std::unary_function
#include <algorithm>                //std::find_if

#ifndef _STRING_HXX
#include <tools/string.hxx>         //do we have to...
#endif

#include <tools/debug.hxx>

#ifndef WW_STATICASSERT_HXX
#   include "staticassert.hxx"      //StaticAssert
#endif

namespace
{
    class SameName: public std::unary_function<const sal_Char*, bool>
    {
    private:
        const String &mrName;
    public:
        explicit SameName(const String &rName) : mrName(rName) {}
        bool operator() (const sal_Char *pEntry) const
            { return mrName.EqualsAscii(pEntry); }
    };

    const sal_Char **GetStiNames() throw()
    {
        static const sal_Char *stiName[] = 
        {
            "Normal",
            "Heading 1",
            "Heading 2",
            "Heading 3",
            "Heading 4",
            "Heading 5",
            "Heading 6",
            "Heading 7",
            "Heading 8",
            "Heading 9",
            "Index 1",
            "Index 2",
            "Index 3",
            "Index 4",
            "Index 5",
            "Index 6",
            "Index 7",
            "Index 8",
            "Index 9",
            "TOC 1",
            "TOC 2",
            "TOC 3",
            "TOC 4",
            "TOC 5",
            "TOC 6",
            "TOC 7",
            "TOC 8",
            "TOC 9",
            "Normal Indent",
            "Footnote Text",
            "Annotation Text",
            "Header",
            "Footer",
            "Index Heading",
            "Caption",
            "Table of Figures",
            "Envelope Address",
            "Envelope Return",
            "Footnote Reference",
            "Annotation Reference",
            "Line Number",
            "Page Number",
            "Endnote Reference",
            "Endnote Text",
            "Table of Authorities",
            "Macro Text",
            "TOA Heading",
            "List",
            "List 2",
            "List 3",
            "List 4",
            "List 5",
            "List Bullet",
            "List Bullet 2",
            "List Bullet 3",
            "List Bullet 4",
            "List Bullet 5",
            "List Number",
            "List Number 2",
            "List Number 3",
            "List Number 4",
            "List Number 5",
            "Title",
            "Closing",
            "Signature",
            "Default Paragraph Font",
            "Body Text",
            "Body Text Indent",
            "List Continue",
            "List Continue 2",
            "List Continue 3",
            "List Continue 4",
            "List Continue 5",
            "Message Header",
            "Subtitle",
            "Salutation",
            "Date",
            "Body Text First Indent",
            "Body Text First Indent 2",
            "Note Heading",
            "Body Text 2",
            "Body Text 3",
            "Body Text Indent 2",
            "Body Text Indent 3",
            "Block Text",
            "Hyperlink",
            "Followed Hyperlink",
            "Strong",
            "Emphasis",
            "Document Map",
            "Plain Text"
        };

        DBG_ASSERT( (sizeof(stiName) / sizeof(stiName[0])) == ww::stiMax, "WrongSizeOfArray" );

        return stiName;
    }
}

namespace ww
{
    //Original code/idea by Takashi Ono for CJK
    sti GetCanonicalStiFromEnglishName(const String &rName) throw()
    {
        typedef const sal_Char** myIter;
        sti eRet = stiUser;
        myIter aBegin = GetStiNames();
        myIter aEnd(aBegin);
        std::advance(aEnd, stiMax);
        myIter aIter = std::find_if(aBegin, aEnd, SameName(rName));
        if (aIter != aEnd)
            eRet = static_cast<sti>(std::distance(aBegin, aIter));
        return eRet;
    }

    const sal_Char* GetEnglishNameFromSti(sti eSti) throw()
    {
        if (eSti >= stiMax)
            return 0;
        else
            return GetStiNames()[eSti];
    }

    bool StandardStiIsCharStyle(sti eSti) throw()
    {
        switch (eSti)
        {
            case stiFtnRef:
            case stiAtnRef:
            case stiLnn:
            case stiPgn:
            case stiEdnRef:
            case stiNormalChar:
                return true;
            default:
                return false;
        }
    }

    sti GetCanonicalStiFromStc(sal_uInt8 stc) throw()
    {
        if (stc == 0)
            return stiNormal;
        else if (stc < 222)
            return stiUser;
        else
        {
            static sti aMapping[] = 
            {
                stiNil, stiAtnRef, stiAtnText, stiToc8, stiToc7, stiToc6, 
                stiToc5, stiToc4, stiToc3, stiToc2, stiToc1, stiIndex7,
                stiIndex6, stiIndex5, stiIndex4, stiIndex3, stiIndex2,
                stiIndex1, stiLnn, stiIndexHeading, stiFooter, stiHeader,
                stiFtnRef, stiFtnText, stiLev9, stiLev8, stiLev7, stiLev6,
                stiLev5, stiLev4, stiLev3, stiLev2, stiLev1, stiNormIndent
            };
            return aMapping[stc-222];
        }
    }
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
