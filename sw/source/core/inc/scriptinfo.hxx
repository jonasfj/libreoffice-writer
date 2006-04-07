/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scriptinfo.hxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: vg $ $Date: 2006-04-07 15:09:50 $
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
#ifndef _SCRIPTINFO_HXX
#define _SCRIPTINFO_HXX

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_SHORTS
#define _SVSTDARR_BYTES
#define _SVSTDARR_USHORTS
#define _SVSTDARR_XUB_STRLEN
#include <svtools/svstdarr.hxx>
#endif

#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif
#include <list>

class SwWrongList;
class SwTxtNode;
class Point;
class MultiSelection;
typedef std::list< xub_StrLen > PositionList;

#define SPACING_PRECISION_FACTOR 100

/*************************************************************************
 *				  class SwScanner
 * Hilfsklasse, die beim Spellen die Worte im gewuenschten Bereich
 * nacheinander zur Verfuegung stellt.
 *************************************************************************/

class SwScanner
{
    XubString aWord;
    const SwTxtNode& rNode;
    xub_StrLen nStartPos;
    xub_StrLen nEndPos;
    xub_StrLen nBegin;
    xub_StrLen nLen;
    LanguageType aCurrLang;
    USHORT nWordType;
    BOOL bStart;
    BOOL bClip;

public:
    SwScanner( const SwTxtNode& rNd, USHORT nWordType,
               xub_StrLen nStart, xub_StrLen nEnde, BOOL bClip = FALSE );


    // This next word function tries to find the language for the next word
    // It should currently _not_ be used for spell checking, and works only for
    // ! bReverse
    BOOL NextWord();

    const XubString& GetWord() const    { return aWord; }

    xub_StrLen GetBegin() const         { return nBegin; }
    xub_StrLen GetEnd() const           { return nBegin + nLen; }
    xub_StrLen GetLen() const           { return nLen; }

    LanguageType GetCurrentLanguage() const {return aCurrLang;}
};

/*************************************************************************
 *						class SwScriptInfo
 *
 * encapsultes information about script changes
 *************************************************************************/

class SwScriptInfo
{
private:
    SvXub_StrLens aScriptChg;
    SvBytes aScriptType;
    SvXub_StrLens aDirChg;
    SvBytes aDirType;
    SvXub_StrLens aKashida;
    SvXub_StrLens aCompChg;
    SvXub_StrLens aCompLen;
    SvXub_StrLens aHiddenChg;
    SvBytes aCompType;
    xub_StrLen nInvalidityPos;
    BYTE nDefaultDir;

    void UpdateBidiInfo( const String& rTxt );

public:
    enum CompType { KANA, SPECIAL_LEFT, SPECIAL_RIGHT, NONE };

    SwScriptInfo();
    ~SwScriptInfo();

    // determines script changes
    void InitScriptInfo( const SwTxtNode& rNode, sal_Bool bRTL );
    void InitScriptInfo( const SwTxtNode& rNode );

    // set/get position from which data is invalid
    inline void SetInvalidity( const xub_StrLen nPos );
    inline xub_StrLen GetInvalidity() const { return nInvalidityPos; };

    // get default direction for paragraph
    inline BYTE GetDefaultDir() const { return nDefaultDir; };

    // array operations, nCnt refers to array position
    inline USHORT CountScriptChg() const;
    inline xub_StrLen GetScriptChg( const USHORT nCnt ) const;
    inline BYTE GetScriptType( const USHORT nCnt ) const;

    inline USHORT CountDirChg() const;
    inline xub_StrLen GetDirChg( const USHORT nCnt ) const;
    inline BYTE GetDirType( const USHORT nCnt ) const;

    inline USHORT CountKashida() const;
    inline xub_StrLen GetKashida( const USHORT nCnt ) const;

    inline USHORT CountCompChg() const;
    inline xub_StrLen GetCompStart( const USHORT nCnt ) const;
    inline xub_StrLen GetCompLen( const USHORT nCnt ) const;
    inline BYTE GetCompType( const USHORT nCnt ) const;

    inline USHORT CountHiddenChg() const;
    inline xub_StrLen GetHiddenChg( const USHORT nCnt ) const;
    static void CalcHiddenRanges( const SwTxtNode& rNode,
                                                MultiSelection& rHiddenMulti );

    // "high" level operations, nPos refers to string position
    xub_StrLen NextScriptChg( const xub_StrLen nPos ) const;
    BYTE ScriptType( const xub_StrLen nPos ) const;

    // Returns the position of the next direction level change.
    // If bLevel is set, the position of the next level which is smaller
    // than the level at position nPos is returned. This is required to
    // obtain the end of a SwBidiPortion
    xub_StrLen NextDirChg( const xub_StrLen nPos,
                           const BYTE* pLevel = 0 ) const;
    BYTE DirType( const xub_StrLen nPos ) const;

#if OSL_DEBUG_LEVEL > 1
    BYTE CompType( const xub_StrLen nPos ) const;
#endif

    //
    // HIDDEN TEXT STUFF START
    //

/** Hidden text range information - static and non-version

    @descr  Determines if a given position is inside a hidden text range. The
            static version tries to obtain a valid SwScriptInfo object
            via the SwTxtNode, otherwise it calculates the values from scratch.
            The non-static version uses the internally cached informatio
            for the calculation.

    @param  rNode
                The text node.
    @param  nPos
                The given position that should be checked.
    @param  rnStartPos
                Return parameter for the start position of the hidden range.
                STRING_LEN if nPos is not inside a hidden range.
    @param  rnEndPos
                Return parameter for the end position of the hidden range.
                0 if nPos is not inside a hidden range.
    @param  rnEndPos
                Return parameter that contains all the hidden text ranges. Optional.
    @return
            returns true if there are any hidden characters in this paragraph.

*/
    static bool GetBoundsOfHiddenRange( const SwTxtNode& rNode, xub_StrLen nPos,
                                        xub_StrLen& rnStartPos, xub_StrLen& rnEndPos,
                                        PositionList* pList = 0 );
    bool GetBoundsOfHiddenRange(  xub_StrLen nPos, xub_StrLen& rnStartPos,
                                  xub_StrLen& rnEndPos, PositionList* pList = 0 ) const;

    static bool IsInHiddenRange( const SwTxtNode& rNode, xub_StrLen nPos );

/** Hidden text attribute handling

    @descr  Takes a string and either deletes the hidden ranges or sets
            a given character in place of the hidden characters.

    @param  rNode
                The text node.
    @param  nPos
                The string to modify.
    @param  cChar
                The character that should replace the hidden characters.
    @param  bDel
                If set, the hidden ranges will be deleted from the text node.
 */
    static USHORT MaskHiddenRanges( const SwTxtNode& rNode, XubString& rText,
                                    const xub_StrLen nStt, const xub_StrLen nEnd,
                                    const xub_Unicode cChar );

/** Hidden text attribute handling

    @descr  Takes a SwTxtNode and deletes the hidden ranges from the node.

    @param  rNode
                The text node.
 */
    static void DeleteHiddenRanges( SwTxtNode& rNode );

    //
    // HIDDEN TEXT STUFF END
    //

    // examines the range [ nStart, nStart + nEnd ] if there are kanas
    // returns start index of kana entry in array, otherwise USHRT_MAX
    USHORT HasKana( xub_StrLen nStart, const xub_StrLen nEnd ) const;

    // modifies the kerning array according to a given compress value
    long Compress( sal_Int32* pKernArray, xub_StrLen nIdx, xub_StrLen nLen,
                   const USHORT nCompress, const USHORT nFontHeight,
                   Point* pPoint = NULL ) const;

/** Performes a kashida justification on the kerning array

    @descr  Add some extra space for kashida justification to the
            positions in the kerning array.
    @param  pKernArray
                The printers kerning array. Optional.
    @param  pScrArray
                The screen kerning array. Optional.
    @param  nIdx
                Start referring to the paragraph.
    @param  nLen
                The number of characters to be considered.
    @param  nSpaceAdd
                The value which has to be added to a kashida opportunity.
    @return The number of kashida opportunities in the given range
*/
    USHORT KashidaJustify( sal_Int32* pKernArray ,sal_Int32* pScrArray,
                           xub_StrLen nIdx, xub_StrLen nLen,
                           long nSpaceAdd = 0 ) const;

/** Checks if language is one of the 16 Arabic languages

    @descr  Checks if language is one of the 16 Arabic languages
    @param  aLang
                The language which has to be checked.
    @return Returns if the language is an Arabic language
*/
    static BOOL IsArabicLanguage( LanguageType aLang );

/** Performes a thai justification on the kerning array

    @descr  Add some extra space for thai justification to the
            positions in the kerning array.
    @param  rTxt
                The String
    @param  pKernArray
                The printers kerning array. Optional.
    @param  pScrArray
                The screen kerning array. Optional.
    @param  nIdx
                Start referring to the paragraph.
    @param  nLen
                The number of characters to be considered.
    @param  nSpaceAdd
                The value which has to be added to the cells.
    @return The number of extra spaces in the given range
*/
    static USHORT ThaiJustify( const XubString& rTxt, sal_Int32* pKernArray,
                               sal_Int32* pScrArray, xub_StrLen nIdx,
                               xub_StrLen nLen, xub_StrLen nNumberOfBlanks = 0,
                               long nSpaceAdd = 0 );

    static SwScriptInfo* GetScriptInfo( const SwTxtNode& rNode,
                                        sal_Bool bAllowInvalid = sal_False );

    static BYTE WhichFont( xub_StrLen nIdx, const String* pTxt, const SwScriptInfo* pSI );
};

inline void SwScriptInfo::SetInvalidity( const xub_StrLen nPos )
{
    if ( nPos < nInvalidityPos )
        nInvalidityPos = nPos;
};
inline USHORT SwScriptInfo::CountScriptChg() const { return aScriptChg.Count(); }
inline xub_StrLen SwScriptInfo::GetScriptChg( const USHORT nCnt ) const
{
    ASSERT( nCnt < aScriptChg.Count(),"No ScriptChange today!");
    return aScriptChg[ nCnt ];
}
inline BYTE SwScriptInfo::GetScriptType( const xub_StrLen nCnt ) const
{
    ASSERT( nCnt < aScriptChg.Count(),"No ScriptType today!");
    return aScriptType[ nCnt ];
}

inline USHORT SwScriptInfo::CountDirChg() const { return aDirChg.Count(); }
inline xub_StrLen SwScriptInfo::GetDirChg( const USHORT nCnt ) const
{
    ASSERT( nCnt < aDirChg.Count(),"No DirChange today!");
    return aDirChg[ nCnt ];
}
inline BYTE SwScriptInfo::GetDirType( const xub_StrLen nCnt ) const
{
    ASSERT( nCnt < aDirChg.Count(),"No DirType today!");
    return aDirType[ nCnt ];
}

inline USHORT SwScriptInfo::CountKashida() const { return aKashida.Count(); }
inline xub_StrLen SwScriptInfo::GetKashida( const USHORT nCnt ) const
{
    ASSERT( nCnt < aKashida.Count(),"No Kashidas today!");
    return aKashida[ nCnt ];
}

inline USHORT SwScriptInfo::CountCompChg() const { return aCompChg.Count(); };
inline xub_StrLen SwScriptInfo::GetCompStart( const USHORT nCnt ) const
{
    ASSERT( nCnt < aCompChg.Count(),"No CompressionStart today!");
    return aCompChg[ nCnt ];
}
inline xub_StrLen SwScriptInfo::GetCompLen( const USHORT nCnt ) const
{
    ASSERT( nCnt < aCompChg.Count(),"No CompressionLen today!");
    return aCompLen[ nCnt ];
}

inline BYTE SwScriptInfo::GetCompType( const USHORT nCnt ) const
{
    ASSERT( nCnt < aCompChg.Count(),"No CompressionType today!");
    return aCompType[ nCnt ];
}

inline USHORT SwScriptInfo::CountHiddenChg() const { return aHiddenChg.Count(); };
inline xub_StrLen SwScriptInfo::GetHiddenChg( const USHORT nCnt ) const
{
    ASSERT( nCnt < aHiddenChg.Count(),"No HiddenChg today!");
    return aHiddenChg[ nCnt ];
}


#endif
