/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: chpfld.hxx,v $
 * $Revision: 1.8 $
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
#ifndef _CHPFLD_HXX
#define _CHPFLD_HXX

#include "fldbas.hxx"

class SwFrm;
class SwTxtNode;
#include <unoobj.hxx>

enum SwChapterFormat
{
    CF_BEGIN,
    CF_NUMBER = CF_BEGIN,		// nur die Kapitelnummer
    CF_TITLE,					// nur die "Ueberschrift"
    CF_NUM_TITLE,				// Kapitelnummer und "Ueberschrift"
    CF_NUMBER_NOPREPST,			// nur die Kapitelnummer ohne Post/Prefix
    CF_NUM_NOPREPST_TITLE,		// Kapitelnummer ohne Post/Prefix und "Ueberschrift"
    CF_END
};

/*--------------------------------------------------------------------
    Beschreibung: Kapitel
 --------------------------------------------------------------------*/

class SwChapterFieldType : public SwFieldType
{
public:
    SwChapterFieldType();

    virtual SwFieldType*	Copy() const;

};



/*--------------------------------------------------------------------
    Beschreibung: Kapitelnummer
 --------------------------------------------------------------------*/
class SW_DLLPUBLIC SwChapterField : public SwField
{
    friend class SwChapterFieldType;
    BYTE nLevel;
    String sTitle, sNumber, sPre, sPost;
public:
    SwChapterField(SwChapterFieldType*, sal_uInt32 nFmt = 0);

    // --> OD 2008-02-14 #i53420#
//    void ChangeExpansion( const SwFrm*,
//                          const SwTxtNode*,
//                          BOOL bSrchNum = FALSE);
    void ChangeExpansion( const SwFrm*,
                          const SwCntntNode*,
        BOOL bSrchNum = FALSE);
    // <--
    void ChangeExpansion(const SwTxtNode &rNd, BOOL bSrchNum);

    virtual String	 Expand() const;
    virtual SwField* Copy() const;

    inline BYTE GetLevel() const;
    inline void	SetLevel(BYTE);

    inline const String& GetNumber() const;
    inline const String& GetTitle() const;
    virtual BOOL        QueryValue( com::sun::star::uno::Any& rVal, USHORT nWhich ) const;
    virtual BOOL        PutValue( const com::sun::star::uno::Any& rVal, USHORT nWhich );
};

inline BYTE SwChapterField::GetLevel() const 	{ return nLevel; }
inline void	SwChapterField::SetLevel(BYTE nLev) { nLevel = nLev; }
inline const String& SwChapterField::GetNumber() const { return sNumber; }
inline const String& SwChapterField::GetTitle() const { return sTitle; }

#endif // _CHPFLD_HXX
