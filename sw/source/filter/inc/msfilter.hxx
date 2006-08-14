/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: msfilter.hxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 17:08:22 $
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
/// @HTML

#ifndef SW_MS_MSFILTER_HXX
#define SW_MS_MSFILTER_HXX

#include <set>
#include <map>
#include <vector>

#ifndef _SWTYPES_HXX
#   include <swtypes.hxx>       //SwTwips
#endif
#ifndef _STRING_HXX
#   include <tools/string.hxx>  //String
#endif
#ifndef WW_WWSTYLES_HXX
#   include "wwstyles.hxx"      //ww::sti
#endif
#ifndef _RTL_TEXTENC_H
#   include <rtl/textenc.h>     //rtl_TextEncoding
#endif
#ifndef _SV_GEN_HXX
#   include <tools/gen.hxx>     //Size
#endif
#ifndef _DATETIME_HXX 
#include <tools/datetime.hxx>
#endif
#ifndef _FLTSHELL_HXX
#include <fltshell.hxx>         // fuer den Attribut Stack
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif

#define CREATE_CONST_ASC(s) String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM(s))

class SwDoc;
class SwPaM;
class String;
class SwTableNode;
class SwNodeIndex;
class SwNoTxtNode;
class SwTxtNode;

namespace
{
    template<class C> class StyleMapperImpl;
}
class SwTxtFmtColl;
class SwCharFmt;
typedef StyleMapperImpl<SwTxtFmtColl> ParaMapper;
typedef StyleMapperImpl<SwCharFmt> CharMapper;

namespace sw
{
    namespace ms
    {
        /** MSOffice appears to set the charset of unicode fonts to MS 932

            Arial Unicode MS for example is a unicode font, but word sets 
            exported uses of it to the MS 932 charset

            @param eTextEncoding
                the OOo encoding to convert from

            @return 
                a msoffice equivalent charset identifier

            @author
                <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        sal_uInt8 rtl_TextEncodingToWinCharset(rtl_TextEncoding eTextEncoding);

        /** Import a MSWord XE field. Suitable for .doc and .rtf

            @param rDoc
                the document to insert into

            @param rPaM
                the position in the document to insert into

            @param rXE
                the arguments of the original word XE field

            @author
                <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        void ImportXE(SwDoc &rDoc, SwPaM &rPaM, const String &rXE);

        /** Convert from DTTM to Writer's DateTime

            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a
        */
        DateTime DTTM2DateTime( long lDTTM );

        /** Convert from DTTM to Writer's DateTime

            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a
        */
        long DateTime2DTTM( const DateTime& rDT );

        /** Convert from Word Date/Time field str to Writer's Date Time str

            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a
        */
        ULONG MSDateTimeFormatToSwFormat(String& rParams, SvNumberFormatter *pFormatter, USHORT &rLang, bool bHijri);

        /** Used by MSDateTimeFormatToSwFormat to identify AM time fields

            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a
        */
        bool IsNotAM(String& rParams, xub_StrLen nPos);

        /** Another function used by MSDateTimeFormatToSwFormat

            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a
        */
        void SwapQuotesInField(String &rFmt);

    }

    namespace util
    {
        struct AuthorInfo;
        typedef AuthorInfo* AuthorInfo_Ptr;

        //-----------------------------------------
        //     Redlining Authors
        //-----------------------------------------
        struct AuthorInfo
        {
            USHORT nWWAuthorId;
            USHORT nOurId;

            AuthorInfo(USHORT nWWAuthorId_, USHORT nOurId_ = 0):
                nWWAuthorId( nWWAuthorId_ ),
                nOurId(      nOurId_ )
                {}
            bool operator==(const AuthorInfo& rEntry) const
            {
                return (nWWAuthorId == rEntry.nWWAuthorId);
            }
            bool operator<(const AuthorInfo& rEntry) const
            {
                return (nWWAuthorId < rEntry.nWWAuthorId);
            }
        };

        SV_DECL_PTRARR_SORT_DEL(AuthorInfos, AuthorInfo_Ptr,16,16)

        /** Clips a value to MAX/MIN 16bit value to make it safe for use
            as a position value to give to writer. i.e. +-57.8cm. Sometimes
            we see ridiculous values for positioning in rtf and word document,
            this captures such ones and clips them to values which are 
            still outside the document, but of a value that doesn't cause 
            problems for writer's layout, e.g. see
            http://www.openoffice.org/issues/show_bug.cgi?id=i9245

            @param nIn

            @return nIn clipped to min/max 16bit value

            @author
                <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SwTwips MakeSafePositioningValue(SwTwips nIn);

        /** Knows which writer style a given word style should be imported as

            Mapping a word style to a writer style has to consider mapping
            the word builtin styles like "Normal" as the default root style
            to our default root style which is called "Default" in english,
            and "Normal" in german. 
            
            Additionally it then has to avoid name collisions such as

            a) styles "Normal" and "Default" in a single document, where
            we can use the original distinct names "Normal" and "Default" and..
            b) styles "Normal" and "Default" in a single document, where
            we can not use the original names, and must come up with an 
            alternative name for one of them..

            And it needs to report to the importer if the style being mapped to
            was already in existance, for the cut and paste/insert file mode we
            should not modify the returned style if it is already in use as it
            is does not belong to us to change.

            @author
                <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        class ParaStyleMapper
        {
        private:
            //I hate these things stupid pImpl things, but its warranted here
            ParaMapper *mpImpl;     
        public:
            ParaStyleMapper(SwDoc &rDoc);
            ~ParaStyleMapper();

            /** StyleResult
                StyleResult is a std::pair of a pointer to a style and a flag
                which is true if the style existed previously in the document.
            */
            typedef std::pair<SwTxtFmtColl*, bool> StyleResult;

            /** Get the writer style which the word style should map to

                @param rName
                The name of the word style

                @param eSti 
                The style id of the word style, we are really only interested
                in knowing if the style has either a builtin standard id, or is
                a user defined style.
            
                @return
                The equivalent writer style packaged as a StyleResult to use
                for this word style.
                
                It will only return a failure in the pathological case of
                catastropic failure elsewhere of there exist already styles
                rName and WW-rName[0..SAL_MAX_INT32], which is both unlikely
                and impossible.
            */
            StyleResult GetStyle(const String& rName, ww::sti eSti); 
        };

        /** Knows which writer style a given word style should be imported as

            Mapping a word style to a writer style has to consider mapping
            the word builtin styles like "Normal" as the default root style
            to our default root style which is called "Default" in english,
            and "Normal" in german. 
            
            Additionally it then has to avoid name collisions such as

            a) styles "Normal" and "Default" in a single document, where
            we can use the original distinct names "Normal" and "Default" and..
            b) styles "Normal" and "Default" in a single document, where
            we can not use the original names, and must come up with an 
            alternative name for one of them..

            And it needs to report to the importer if the style being mapped to
            was already in existance, for the cut and paste/insert file mode we
            should not modify the returned style if it is already in use as it
            is does not belong to us to change.

            @author
                <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        class CharStyleMapper
        {
        private:
            //I hate these things stupid pImpl things, but its warranted here
            CharMapper *mpImpl;     
        public:
            CharStyleMapper(SwDoc &rDoc);
            ~CharStyleMapper();

            /** StyleResult
                StyleResult is a std::pair of a pointer to a style and a flag
                which is true if the style existed previously in the document.
            */
            typedef std::pair<SwCharFmt*, bool> StyleResult;

            /** Get the writer style which the word style should map to

                @param rName
                The name of the word style

                @param eSti 
                The style id of the word style, we are really only interested
                in knowing if the style has either a builtin standard id, or is
                a user defined style.
            
                @return
                The equivalent writer style packaged as a StyleResult to use
                for this word style.
                
                It will only return a failure in the pathological case of
                catastropic failure elsewhere of there exist already styles
                rName and WW-rName[0..SAL_MAX_INT32], which is both unlikely
                and impossible.
            */
            StyleResult GetStyle(const String& rName, ww::sti eSti); 
        };

        /** Find suitable names for exporting this font
            
            Given a fontname description find the best primary and secondary
            fallback font to use from MSWord's persp font

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>

            @see #i10242#/#i19164# for examples
        */
        class FontMapExport
        {
        public:
            String msPrimary;
            String msSecondary;
            bool HasDistinctSecondary() const;
            FontMapExport(const String &rFontDescription);
        };

        /** Handle requirements for table formatting in insert->file mode.
            
            When inserting a table into a document which already has been
            formatted and laid out (e.g using insert->file) then tables
            must be handled in a special way, (or so previous comments and
            code in the filters leads me to believe). 

            Before the document is finalized the new tables need to have
            their layout frms deleted and recalculated. This TableManager
            detects the necessity to do this, and all tables inserted into
            a document should be registered with this manager with
            InsertTable, and before finialization DelAndMakeTblFrms should
            be called.
            
            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>

            @see #i25782# for examples
        */
        class InsertedTablesManager
        {
        public:
            typedef std::map<SwTableNode *, SwNodeIndex *> TblMap;
            typedef TblMap::iterator TblMapIter;
            void DelAndMakeTblFrms();
            void InsertTable(SwTableNode &rTableNode, SwPaM &rPaM);
            InsertedTablesManager(const SwDoc &rDoc);
        private:
            bool mbHasRoot;
            TblMap maTables;
        };

        /**
            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a>
         */
        class RedlineStack
        {
        private:
            std::vector<SwFltStackEntry *> maStack;
            typedef std::vector<SwFltStackEntry *>::reverse_iterator myriter;
            SwDoc &mrDoc;
        public:
            explicit RedlineStack(SwDoc &rDoc) : mrDoc(rDoc) {}
            void open(const SwPosition& rPos, const SfxPoolItem& rAttr);
            void close(const SwPosition& rPos, IDocumentRedlineAccess::RedlineType_t eType);
            void closeall(const SwPosition& rPos);
            ~RedlineStack();
        private:
            //No copying
            RedlineStack(const RedlineStack&);
            RedlineStack& operator=(const RedlineStack&);
        };

        /**
            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a>
         */
        class SetInDocAndDelete
        {
        private:
            SwDoc &mrDoc;
        public:
            explicit SetInDocAndDelete(SwDoc &rDoc) : mrDoc(rDoc) {}
            void operator()(SwFltStackEntry *pEntry);
        private:
            //No assignment
            SetInDocAndDelete& operator=(const SetInDocAndDelete&);
        };

        /**
            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a>
         */
        class CloseIfOpen       //Subclass from something ?
        {
        private:
            const SwPosition &mrPos;
        public:
            explicit CloseIfOpen(const SwPosition &rPos) : mrPos(rPos) {}
                void operator()(SwFltStackEntry *pEntry) const
            {
                if (pEntry->bLocked)
                    pEntry->SetEndPos(mrPos);
            }
        private:
            //No assignment
            CloseIfOpen& operator=(const CloseIfOpen&);
        };

        /**
            @author
                <a href="mailto:mmaher@openoffice.org">Martin Maher</a>
         */
        class CompareRedlines:
            public std::binary_function<const SwFltStackEntry*, const SwFltStackEntry*,
            bool>
        {
        public:
            bool operator()(const SwFltStackEntry *pOneE, const SwFltStackEntry *pTwoE)
                const;
        };

        class WrtRedlineAuthor
        {
        protected:
            std::vector<String> maAuthors;          // Array of Sw - Bookmarknames

            USHORT GetPos( const String& rNm );

            //No copying
            WrtRedlineAuthor(const WrtRedlineAuthor&);
            WrtRedlineAuthor& operator=(const WrtRedlineAuthor&);
        public:
            WrtRedlineAuthor() {}

            USHORT AddName( const String& rNm );
            virtual void Write(Writer &rWrt) = 0;
            // std::vector<String> GetNames();
        };

        /** Given a SwNoTxtNode (ole/graphic) get original size

            Get the uncropped and unscaled size of the underlying graphic or
            ole object associated with a given SwNoTxtNode. 
            
            This function will swap in the graphic if it is swapped out from
            the graphic or object cache, but will swap it out if that was the
            case, i.e.  rNd is logically unchanged before and after
            GetSwappedInSize, though not physically const

            @param rNd
                the SwNoTxtNode whose objects original size we want

            @return 
                the uncropped unscaled size of the SwNoTxtNode

            @author
                <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Size GetSwappedInSize(const SwNoTxtNode& rNd);

        struct CharRunEntry
        {
            xub_StrLen mnEndPos;
            sal_uInt16 mnScript;
            rtl_TextEncoding meCharSet;
            bool mbRTL;
            CharRunEntry(xub_StrLen nEndPos, sal_uInt16 nScript, 
                rtl_TextEncoding eCharSet, bool bRTL) 
            : mnEndPos(nEndPos), mnScript(nScript), meCharSet(eCharSet), 
            mbRTL(bRTL)
            {
            }
        };

        typedef std::vector<CharRunEntry> CharRuns;
        typedef CharRuns::const_iterator cCharRunIter;

        /** Collect the ranges of Text which share 

            Word generally requires characters which share the same direction,
            the same script, and occasionally (depending on the format) the
            same charset to be exported in independant chunks.

            So this function finds these ranges and returns a STL container
            of CharRuns

            @param rTxtNd
                The TextNode we want to ranges from

            @param nStart
                The position in the TxtNode to start processing from

            @param bSplitOnCharSet
                Set to true is we want to split on ranges of characters that
                share a plausible charset for export to e.g. WW7- or perhaps
                RTF format, not necessary for a unicode aware format like WW8+

            @return STL container of CharRuns which describe the shared
            direction, script and optionally script of the contigious sequences
            of characters
            
            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>

            @see #i22537# for example
        */
        CharRuns GetPseudoCharRuns(const SwTxtNode& rTxtNd, 
            xub_StrLen nStart = 0, bool bSplitOnCharSet = false);
    }
}

#endif
/* vi:set tabstop=4 shiftwidth=4 expandtab: */
