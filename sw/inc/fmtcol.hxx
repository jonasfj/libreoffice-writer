/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fmtcol.hxx,v $
 * $Revision: 1.13 $
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
#ifndef _FMTCOL_HXX
#define _FMTCOL_HXX

#include <svtools/svarray.hxx>
#include "swdllapi.h"
#include <format.hxx>
#include <swtypes.hxx>		// fuer MAXLEVEL

class SwDoc;		// fuer friend

class SwFmtColl : public SwFmt
{
protected:
    SwFmtColl( SwAttrPool& rPool, const sal_Char* pFmtName,
                const USHORT* pWhichRanges, SwFmtColl* pDerFrom,
                USHORT nFmtWhich )
          : SwFmt( rPool, pFmtName, pWhichRanges, pDerFrom, nFmtWhich )
    { SetAuto( FALSE ); }

    SwFmtColl( SwAttrPool& rPool, const String &rFmtName,
                const USHORT* pWhichRanges, SwFmtColl* pDerFrom,
                USHORT nFmtWhich )
          : SwFmt( rPool, rFmtName, pWhichRanges, pDerFrom, nFmtWhich )
    { SetAuto( FALSE ); }


private:
    // erstmal wird nicht kopiert und nicht zugewiesen
    SwFmtColl(const SwFmtColl & );
    const SwFmtColl &operator=(const SwFmtColl &);
};


class SW_DLLPUBLIC SwTxtFmtColl: public SwFmtColl
{
    friend class SwDoc;

    SwTxtFmtColl(const SwTxtFmtColl & rRef);

    // --> OD 2007-01-24 #i73790#
    bool mbStayAssignedToListLevelOfOutlineStyle;
    // <--
protected:
    BYTE nOutlineLevel;
    SwTxtFmtColl *pNextTxtFmtColl;

    SwTxtFmtColl( SwAttrPool& rPool, const sal_Char* pFmtCollName,
                    SwTxtFmtColl* pDerFrom = 0,
                    USHORT nFmtWh = RES_TXTFMTCOLL )
        : SwFmtColl( rPool, pFmtCollName, aTxtFmtCollSetRange,
                        pDerFrom, nFmtWh ),
          // --> OD 2007-01-24 #i73790#
          mbStayAssignedToListLevelOfOutlineStyle( false ),
          // <--
          nOutlineLevel( NO_NUMBERING )
    { pNextTxtFmtColl = this; }

    SwTxtFmtColl( SwAttrPool& rPool, const String &rFmtCollName,
                    SwTxtFmtColl* pDerFrom = 0,
                    USHORT nFmtWh = RES_TXTFMTCOLL )
        : SwFmtColl( rPool, rFmtCollName, aTxtFmtCollSetRange,
                        pDerFrom, nFmtWh ),
          // --> OD 2007-01-24 #i73790#
          mbStayAssignedToListLevelOfOutlineStyle( false ),
          // <--
          nOutlineLevel( NO_NUMBERING )
    { pNextTxtFmtColl = this; }

public:

    // zum "abfischen" von UL-/LR-/FontHeight Aenderungen
    virtual void Modify( SfxPoolItem*, SfxPoolItem* );

    TYPEINFO();		//Bereits in Basisklasse Client drin.

    void SetOutlineLevel( BYTE );
    inline BYTE GetOutlineLevel() const { return nOutlineLevel; }

    inline void SetNextTxtFmtColl(SwTxtFmtColl& rNext);
    SwTxtFmtColl& GetNextTxtFmtColl() const { return *pNextTxtFmtColl; }

    BOOL IsAtDocNodeSet() const;

    // --> OD 2006-11-22 #i71574#
    inline const bool AssignedToListLevelOfOutlineStyle() const
    {
        return ( /*0 <= GetOutlineLevel() &&*/ GetOutlineLevel() < MAXLEVEL );
    }

    inline void DeleteAssignmentToListLevelOfOutlineStyle()
    {
        SetOutlineLevel( NO_NUMBERING );
    }
    // <--

    // --> OD 2008-03-04 #refactorlists#
    // override to recognize changes on the <SwNumRuleItem> and register/unregister
    // the paragragh style at the corresponding <SwNumRule> instance
    virtual BOOL SetFmtAttr( const SfxPoolItem& rAttr );
    virtual BOOL SetFmtAttr( const SfxItemSet& rSet );
    virtual BOOL ResetFmtAttr( USHORT nWhich1, USHORT nWhich2 = 0 );
    // <--

    // --> OD 2007-01-24 #i73790#
    // override <ResetAllFmtAttr()> to stay assigned to list level of outline style
    virtual USHORT ResetAllFmtAttr();

    inline bool StayAssignedToListLevelOfOutlineStyle() const
    {
        return mbStayAssignedToListLevelOfOutlineStyle;
    }
    // <--

    // --> OD 2008-02-13 #newlistleveattrs#
    bool AreListLevelIndentsApplicable() const;
    // <--

/*----------------- JP 09.08.94 17:36 -------------------
 wird die Funktionalitaet von Zeichenvorlagen an Absatzvorlagen
 ueberhaupt benoetigt ??

 Wenn, ja dann muessen im TextNode und hier in der TxtCollection ein 2.
 Attset fuer die Char-Attribute angelegt werden; damit die Vererbung
 und der Zugriff auf die gesetzen Attribute richtig funktioniert!!

    virtual BOOL SetDerivedFrom( SwFmtColl* pDerFrom = 0 );

    inline SwCharFmt* GetCharFmt() const;
    inline BOOL IsCharFmtSet() const;
    void SetCharFmt(SwCharFmt *);
    void ResetCharFmt();
inline BOOL SwTxtFmtColl::IsCharFmtSet() const
{
    return aCharDepend.GetRegisteredIn() ? TRUE : FALSE;
}
inline SwCharFmt* SwTxtFmtColl::GetCharFmt() const
{
    return (SwCharFmt*)aCharDepend.GetRegisteredIn();
}
--------------------------------------------------*/
};

typedef SwTxtFmtColl* SwTxtFmtCollPtr;
SV_DECL_PTRARR(SwTxtFmtColls,SwTxtFmtCollPtr,2,4)


class SwGrfFmtColl: public SwFmtColl
{
    friend class SwDoc;
protected:
    SwGrfFmtColl( SwAttrPool& rPool, const sal_Char* pFmtCollName,
                    SwGrfFmtColl* pDerFrom = 0 )
        : SwFmtColl( rPool, pFmtCollName, aGrfFmtCollSetRange,
                    pDerFrom, RES_GRFFMTCOLL )
    {}

    SwGrfFmtColl( SwAttrPool& rPool, const String &rFmtCollName,
                    SwGrfFmtColl* pDerFrom = 0 )
        : SwFmtColl( rPool, rFmtCollName, aGrfFmtCollSetRange,
                    pDerFrom, RES_GRFFMTCOLL )
    {}

public:
    TYPEINFO();		//Bereits in Basisklasse Client drin.
};

typedef SwGrfFmtColl* SwGrfFmtCollPtr;
SV_DECL_PTRARR(SwGrfFmtColls,SwGrfFmtCollPtr,2,4)



//FEATURE::CONDCOLL
// --------- Bedingte Vorlagen -------------------------------

enum Master_CollConditions
{
    PARA_IN_LIST		= 0x0001,
    PARA_IN_OUTLINE		= 0x0002,
    PARA_IN_FRAME		= 0x0004,
    PARA_IN_TABLEHEAD	= 0x0008,
    PARA_IN_TABLEBODY	= 0x0010,
    PARA_IN_SECTION		= 0x0020,
    PARA_IN_FOOTENOTE 	= 0x0040,
    PARA_IN_FOOTER 		= 0x0080,
    PARA_IN_HEADER 		= 0x0100,
    PARA_IN_ENDNOTE 	= 0x0200,
    // ...
    USRFLD_EXPRESSION	= (int)0x8000
};


class SW_DLLPUBLIC SwCollCondition : public SwClient
{
    ULONG nCondition;
    union
    {
        ULONG nSubCondition;
        String* pFldExpression;
    } aSubCondition;

public:
    TYPEINFO();		//Bereits in Basisklasse Client drin.


    SwCollCondition( SwTxtFmtColl* pColl, ULONG nMasterCond,
                    ULONG nSubCond = 0 );
    SwCollCondition( SwTxtFmtColl* pColl, ULONG nMasterCond,
                    const String& rSubExp );
    virtual ~SwCollCondition();

    // @@@ public copy ctor, but no copy assignment?
    SwCollCondition( const SwCollCondition& rCpy );
private:
    // @@@ public copy ctor, but no copy assignment?
    SwCollCondition & operator= (const SwCollCondition &);
public:

    int operator==( const SwCollCondition& rCmp ) const;
    int operator!=( const SwCollCondition& rCmp ) const
                            { return ! (*this == rCmp); }

    ULONG GetCondition() const 		{ return nCondition; }
    ULONG GetSubCondition() const	{ return aSubCondition.nSubCondition; }
    const String* GetFldExpression() const
                                    { return aSubCondition.pFldExpression; }

    void SetCondition( ULONG nCond, ULONG nSubCond );
    SwTxtFmtColl* GetTxtFmtColl() const 	{ return (SwTxtFmtColl*)GetRegisteredIn(); }
};


typedef SwCollCondition* SwCollConditionPtr;
SV_DECL_PTRARR_DEL( SwFmtCollConditions, SwCollConditionPtr, 0, 5 )

class SW_DLLPUBLIC SwConditionTxtFmtColl : public SwTxtFmtColl
{
    friend class SwDoc;
protected:
    SwFmtCollConditions aCondColls;

    SwConditionTxtFmtColl( SwAttrPool& rPool, const sal_Char* pFmtCollName,
                            SwTxtFmtColl* pDerFrom = 0 )
        : SwTxtFmtColl( rPool, pFmtCollName, pDerFrom, RES_CONDTXTFMTCOLL )
    {}
    SwConditionTxtFmtColl( SwAttrPool& rPool, const String &rFmtCollName,
                            SwTxtFmtColl* pDerFrom = 0 )
        : SwTxtFmtColl( rPool, rFmtCollName, pDerFrom, RES_CONDTXTFMTCOLL )
    {}

public:
    TYPEINFO();		//Bereits in Basisklasse Client drin.

    virtual ~SwConditionTxtFmtColl();

    // zum "abfischen" von Aenderungen
//	virtual void Modify( SfxPoolItem*, SfxPoolItem* );

    const SwCollCondition* HasCondition( const SwCollCondition& rCond ) const;
    const SwFmtCollConditions& GetCondColls() const		{ return aCondColls; }
    void InsertCondition( const SwCollCondition& rCond );
    BOOL RemoveCondition( const SwCollCondition& rCond );

    void SetConditions( const SwFmtCollConditions& );
};

//FEATURE::CONDCOLL

// ------------- Inline Implementierungen --------------------

inline void SwTxtFmtColl::SetNextTxtFmtColl( SwTxtFmtColl& rNext )
{
    pNextTxtFmtColl = &rNext;
}
#endif

