/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#ifndef _TXMSRT_HXX
#define _TXMSRT_HXX

#include <i18npool/lang.h>
#include <tox.hxx>

#include <com/sun/star/lang/Locale.hpp>


class CharClass;
class SwCntntNode;
class SwTxtNode;
class SwTxtTOXMark;
class SwIndex;
class SwFmtFld;
class IndexEntrySupplierWrapper;

enum TOXSortType
{
    TOX_SORT_INDEX,
    TOX_SORT_CUSTOM,
    TOX_SORT_CONTENT,
    TOX_SORT_PARA,
    TOX_SORT_TABLE,
    TOX_SORT_AUTHORITY
};

struct SwTOXSource
{
    const SwCntntNode* pNd;
    xub_StrLen nPos;
    BOOL bMainEntry;

    SwTOXSource() : pNd(0), nPos(0), bMainEntry(FALSE) {}
    SwTOXSource( const SwCntntNode* pNode, xub_StrLen n, BOOL bMain )
        : pNd(pNode), nPos(n), bMainEntry(bMain)
    {}
};

SV_DECL_VARARR( SwTOXSources, SwTOXSource, 0, 10 )

class SwTOXInternational
{
    IndexEntrySupplierWrapper* pIndexWrapper;
    CharClass* pCharClass;
    LanguageType eLang;
    String sSortAlgorithm;
    USHORT nOptions;

    void Init();

public:
    SwTOXInternational( LanguageType nLang, USHORT nOptions,
                        const String& rSortAlgorithm );
    SwTOXInternational( const SwTOXInternational& );
    ~SwTOXInternational();

    sal_Int32 Compare( const String& rTxt1, const String& rTxtReading1,
                       const ::com::sun::star::lang::Locale& rLocale1,
                       const String& rTxt2, const String& rTxtReading2,
                       const ::com::sun::star::lang::Locale& rLocale2 ) const;

    inline BOOL IsEqual( const String& rTxt1, const String& rTxtReading1,
                         const ::com::sun::star::lang::Locale& rLocale1,
                         const String& rTxt2, const String& rTxtReading2,
                         const ::com::sun::star::lang::Locale& rLocale2 ) const
    {
        return 0 == Compare( rTxt1, rTxtReading1, rLocale1,
                             rTxt2, rTxtReading2, rLocale2 );
    }

    inline BOOL IsLess( const String& rTxt1, const String& rTxtReading1,
                        const ::com::sun::star::lang::Locale& rLocale1,
                        const String& rTxt2, const String& rTxtReading2,
                        const ::com::sun::star::lang::Locale& rLocale2 ) const
    {
        return -1 == Compare( rTxt1, rTxtReading1, rLocale1,
                              rTxt2, rTxtReading2, rLocale2 );
    }

    String GetIndexKey( const String& rTxt, const String& rTxtReading,
                        const ::com::sun::star::lang::Locale& rLcl ) const;

    String GetFollowingText( BOOL bMorePages ) const;

    String ToUpper( const String& rStr, xub_StrLen nPos ) const;
    inline BOOL IsNumeric( const String& rStr ) const;
};

/*--------------------------------------------------------------------
     Beschreibung: Klassen fuer die Sortierung der Verzeichnisse
 --------------------------------------------------------------------*/

struct SwTOXSortTabBase
{
    SwTOXSources aTOXSources;
    ::com::sun::star::lang::Locale aLocale;
    const SwTxtNode* pTOXNd;
    const SwTxtTOXMark* pTxtMark;
    const SwTOXInternational* pTOXIntl;
    ULONG nPos;
    xub_StrLen nCntPos;
    USHORT nType;
    static USHORT nOpt;

    SwTOXSortTabBase( TOXSortType nType,
                      const SwCntntNode* pTOXSrc,
                      const SwTxtTOXMark* pTxtMark,
                      const SwTOXInternational* pIntl,
                      const ::com::sun::star::lang::Locale* pLocale = NULL );
    virtual ~SwTOXSortTabBase() {}

    USHORT	GetType() const 		{ return nType; }
    USHORT	GetOptions() const		{ return nOpt; }

    virtual void 	FillText( SwTxtNode& rNd, const SwIndex& rInsPos, USHORT nAuthField = 0) const;
    virtual USHORT  GetLevel() 	const = 0;
    virtual BOOL 	operator==( const SwTOXSortTabBase& );
    virtual BOOL 	operator<( const SwTOXSortTabBase& );

    virtual String 	GetURL() const;

    inline void GetTxt( String&, String& ) const;
    inline const ::com::sun::star::lang::Locale& GetLocale() const;

private:
    BOOL bValidTxt;
    String sSortTxt;
    String sSortTxtReading;

    virtual void _GetText( String&, String& ) = 0;
};

inline void SwTOXSortTabBase::GetTxt( String& rSortTxt,
                                      String& rSortTxtReading ) const
{
    if( !bValidTxt )
    {
        SwTOXSortTabBase* pThis = (SwTOXSortTabBase*)this;
        pThis->_GetText( pThis->sSortTxt, pThis->sSortTxtReading );
        pThis->bValidTxt = TRUE;
    }

    rSortTxt = sSortTxt;
    rSortTxtReading = sSortTxtReading;
}

inline const ::com::sun::star::lang::Locale& SwTOXSortTabBase::GetLocale() const
{
    return aLocale;
}

/*--------------------------------------------------------------------
     Beschreibung: fuer Sortierung nach Text
 --------------------------------------------------------------------*/

struct SwTOXIndex : public SwTOXSortTabBase
{
    SwTOXIndex( const SwTxtNode&, const SwTxtTOXMark*, USHORT nOptions, BYTE nKeyLevel,
                const SwTOXInternational& rIntl,
                const ::com::sun::star::lang::Locale& rLocale );
    virtual ~SwTOXIndex() {}


    virtual void 	FillText( SwTxtNode& rNd, const SwIndex& rInsPos, USHORT nAuthField = 0 ) const;
    virtual USHORT  GetLevel() const;
    virtual BOOL 	operator==( const SwTOXSortTabBase& );
    virtual BOOL 	operator<( const SwTOXSortTabBase& );

private:
    virtual void _GetText( String&, String& );

    BYTE	nKeyLevel;
};

struct SwTOXCustom : public SwTOXSortTabBase
{
    SwTOXCustom( const String& rKey, const String& rReading, USHORT nLevel,
                 const SwTOXInternational& rIntl,
                 const ::com::sun::star::lang::Locale& rLocale );
    virtual ~SwTOXCustom() {}

    virtual USHORT GetLevel() const;
    virtual BOOL   operator==( const SwTOXSortTabBase& );
    virtual BOOL   operator<( const SwTOXSortTabBase& );

private:
    virtual void _GetText( String&, String& );

    String  aKey;
    String  sReading;
    USHORT  nLev;
};

/*--------------------------------------------------------------------
     Beschreibung: fuer Sortierung nach Position
 --------------------------------------------------------------------*/

struct SwTOXContent : public SwTOXSortTabBase
{
    SwTOXContent( const SwTxtNode&, const SwTxtTOXMark*,
                const SwTOXInternational& rIntl );
    virtual ~SwTOXContent() {}

    virtual void 	FillText( SwTxtNode& rNd, const SwIndex& rInsPos, USHORT nAuthField = 0 ) const;
    virtual USHORT  GetLevel() const;
private:
    virtual void _GetText( String&, String& );

};

struct SwTOXPara : public SwTOXSortTabBase
{
    SwTOXPara( const SwCntntNode&, SwTOXElement, USHORT nLevel = FORM_ALPHA_DELIMITTER );
    virtual ~SwTOXPara() {}

    void	SetStartIndex( xub_StrLen nSet) 	{ nStartIndex = nSet;}
    void 	SetEndIndex( xub_StrLen nSet ) 		{ nEndIndex = nSet;}

    virtual void 	FillText( SwTxtNode& rNd, const SwIndex& rInsPos, USHORT nAuthField = 0 ) const;
    virtual USHORT  GetLevel() const;

    virtual String 	GetURL() const;
private:
    virtual void _GetText( String&, String& );

    SwTOXElement eType;
    USHORT m_nLevel;
    xub_StrLen nStartIndex;
    xub_StrLen nEndIndex;
};

struct SwTOXTable : public SwTOXSortTabBase
{
    SwTOXTable( const SwCntntNode& rNd );
    virtual ~SwTOXTable() {}

    void	SetLevel(USHORT nSet){nLevel = nSet;}

    virtual USHORT  GetLevel() const;

    virtual String 	GetURL() const;
private:
    virtual void _GetText( String&, String& );

    USHORT nLevel;
};

struct SwTOXAuthority : public SwTOXSortTabBase
{
private:
    SwFmtFld& m_rField;
    virtual void 	FillText( SwTxtNode& rNd, const SwIndex& rInsPos, USHORT nAuthField = 0 ) const;
    virtual void _GetText( String&, String& );

public:
    SwTOXAuthority( const SwCntntNode& rNd, SwFmtFld& rField, const SwTOXInternational& rIntl );
    virtual ~SwTOXAuthority() {}

    SwFmtFld& GetFldFmt() {return m_rField;}

    virtual BOOL 	operator==( const SwTOXSortTabBase& );
    virtual BOOL 	operator<( const SwTOXSortTabBase& );
    virtual USHORT  GetLevel() const;
};


#endif // _TXMSRT_HXX
