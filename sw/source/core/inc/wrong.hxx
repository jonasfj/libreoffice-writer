/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: wrong.hxx,v $
 * $Revision: 1.10 $
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

#ifndef	_WRONG_HXX
#define	_WRONG_HXX

#ifndef _COM_SUN_STAR_SMARTTAGS_XSMARTTAGPROPERTIES_HPP_
#include <com/sun/star/container/XStringKeyMap.hpp>
#endif

#include <vector>

#include <tools/string.hxx>

class SwWrongList;

// ST2
class SwWrongArea
{
public:
    rtl::OUString maType;
    com::sun::star::uno::Reference< com::sun::star::container::XStringKeyMap > mxPropertyBag;
    xub_StrLen mnPos;
    xub_StrLen mnLen;
    SwWrongList* mpSubList;

    SwWrongArea() : mnPos(0), mnLen(0), mpSubList(NULL) {}
    SwWrongArea( const rtl::OUString& rType,
                 com::sun::star::uno::Reference< com::sun::star::container::XStringKeyMap > xPropertyBag,
                 xub_StrLen nPos,
                 xub_StrLen nLen,
                 SwWrongList* pSubList )
        : maType(rType), mxPropertyBag(xPropertyBag), mnPos(nPos), mnLen(nLen), mpSubList(pSubList) {}
};

enum WrongListType
{
    WRONGLIST_SPELL,
    WRONGLIST_GRAMMAR,
    WRONGLIST_SMARTTAG,
    WRONGLIST_CHANGETRACKING
};

class SwWrongList
{
    std::vector<SwWrongArea> maList;
    WrongListType            meType;

    xub_StrLen nBeginInvalid;   // Start des ungueltigen Bereichs
    xub_StrLen nEndInvalid;     // Ende des ungueltigen Bereichs

    void ShiftLeft( xub_StrLen &rPos, xub_StrLen nStart, xub_StrLen nEnd )
    { if( rPos > nStart ) rPos = rPos > nEnd ? rPos - nEnd + nStart : nStart; }
    void ShiftRight( xub_StrLen &rPos, xub_StrLen nStart, xub_StrLen nEnd )
    { if( rPos >= nStart ) rPos += nStart - nEnd; }
    void _Invalidate( xub_StrLen nBegin, xub_StrLen nEnd );

    void Insert(USHORT nWhere, std::vector<SwWrongArea>::iterator startPos, std::vector<SwWrongArea>::iterator endPos);
    void Remove( USHORT nIdx, USHORT nLen );

    // forbidden and not implemented
    SwWrongList& operator= (const SwWrongList &);
    SwWrongList( const SwWrongList& rCpy );

public:
    SwWrongList( WrongListType eType );

    virtual ~SwWrongList();
    virtual SwWrongList* Clone();
    virtual void CopyFrom( const SwWrongList& rCopy );

    inline WrongListType GetWrongListType() const { return meType; }
    inline xub_StrLen GetBeginInv() const { return nBeginInvalid; }
    inline xub_StrLen GetEndInv() const { return nEndInvalid; }
    inline BOOL InsideInvalid( xub_StrLen nChk ) const
        { return nChk >= nBeginInvalid && nChk <= nEndInvalid; }
    void SetInvalid( xub_StrLen nBegin, xub_StrLen nEnd );
    inline void Validate(){ nBeginInvalid = STRING_LEN; }
    void Invalidate( xub_StrLen nBegin, xub_StrLen nEnd );
    BOOL InvalidateWrong();
    BOOL Fresh( xub_StrLen &rStart, xub_StrLen &rEnd, xub_StrLen nPos,
            xub_StrLen nLen, USHORT nIndex, xub_StrLen nCursorPos );
    USHORT GetWrongPos( xub_StrLen nValue ) const;

    sal_Bool Check( xub_StrLen &rChk, xub_StrLen &rLn ) const;
    sal_Bool InWrongWord( xub_StrLen &rChk, xub_StrLen &rLn ) const;
    xub_StrLen NextWrong( xub_StrLen nChk ) const;

    void Move( xub_StrLen nPos, long nDiff );
    void ClearList();

    // Divide the list into two part, the wrong words until nSplitPos will be
    // removed and transferred to a new SwWrongList.
    SwWrongList* SplitList( xub_StrLen nSplitPos );
    // Join the next SwWrongList, nInsertPos is my own text length, where
    // the other wrong list has to be inserted.
    void JoinList( SwWrongList* pNext, xub_StrLen nInsertPos );

    inline xub_StrLen Len( USHORT nIdx ) const
    {
        return nIdx < maList.size() ? maList[nIdx].mnLen : 0;
    }

    inline xub_StrLen Pos( USHORT nIdx ) const
    {
        return nIdx < maList.size() ? maList[nIdx].mnPos : 0;
    }

    inline USHORT Count() const { return (USHORT)maList.size(); }

    inline void Insert( const rtl::OUString& rType,
                        com::sun::star::uno::Reference< com::sun::star::container::XStringKeyMap > xPropertyBag,
                        xub_StrLen nNewPos, xub_StrLen nNewLen, USHORT nWhere )
    {
        std::vector<SwWrongArea>::iterator i = maList.begin();
        if ( nWhere >= maList.size() )
            i = maList.end(); // robust
        else
            i += nWhere;
        maList.insert(i, SwWrongArea( rType, xPropertyBag, nNewPos, nNewLen, 0 ) );
    }

    void Insert( const rtl::OUString& rType,
                 com::sun::star::uno::Reference< com::sun::star::container::XStringKeyMap > xPropertyBag,
                 xub_StrLen nNewPos, xub_StrLen nNewLen );

    inline SwWrongList* SubList( USHORT nIdx ) const
    {
        return nIdx < maList.size() ? maList[nIdx].mpSubList : 0;
    }

    void InsertSubList( xub_StrLen nNewPos, xub_StrLen nNewLen, USHORT nWhere, SwWrongList* pSubList );

    inline const SwWrongArea* GetElement( USHORT nIdx ) const
    {
        return nIdx < maList.size() ? &maList[nIdx] : 0;
    }
    void RemoveEntry( xub_StrLen nBegin, xub_StrLen nEnd );
    bool LookForEntry( xub_StrLen nBegin, xub_StrLen nEnd );
};

#endif
