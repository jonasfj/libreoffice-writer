/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: shellres.hxx,v $
 * $Revision: 1.7 $
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
#ifndef _SHELLRES_HXX
#define _SHELLRES_HXX


#include <tools/string.hxx>

#ifndef _BITMAP_HXX //autogen
#include <vcl/bitmap.hxx>
#endif

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif

struct ShellResource : public Resource
{
    String			aPostItAuthor;
    String 			aPostItPage;
    String			aPostItLine;

    // Calc Fehlerstrings
    String			aCalc_Syntax;
    String			aCalc_ZeroDiv;
    String			aCalc_Brack;
    String			aCalc_Pow;
    String			aCalc_VarNFnd;
    String			aCalc_Overflow;
    String			aCalc_WrongTime;
    String			aCalc_Default;
    String			aCalc_Error;

    // fuers GetRefFeld - oben/unten
    String          aGetRefFld_Up;
    String          aGetRefFld_Down;
    // --> OD 2007-09-13 #i81002#
    // for GetRefField - referenced item not found
    String          aGetRefFld_RefItemNotFound;
    // <--
    // fuer dynamisches Menu - String "alle"
    String			aStrAllPageHeadFoot;
    // fuer einige Listboxen - String "keine"
    String			aStrNone;
    // fuer Felder, die Fixiert sind
    String			aFixedStr;

    //names of TOXs
    String 			aTOXIndexName;
    String          aTOXUserName;
    String          aTOXContentName;
    String          aTOXIllustrationsName;
    String          aTOXObjectsName;
    String          aTOXTablesName;
    String          aTOXAuthoritiesName;

    String          aHyperlinkClick;

    SvStringsDtor	aDocInfoLst;

    // Fly-Anker Bmps
//	Bitmap			aAnchorBmp;
//	Bitmap			aDragAnchorBmp;

    // die AutoFormat-Redline-Kommentare
    inline const SvStringsDtor&	GetAutoFmtNameLst() const;

    // returns for the specific filter the new names of pagedescs
    // This method is for the old code of the specific filters with
    // now localized names
    String GetPageDescName( USHORT nNo, BOOL bFirst = FALSE,
                                        BOOL bFollow = FALSE );

    ShellResource();
    ~ShellResource();

private:
    void _GetAutoFmtNameLst() const;
    SvStringsDtor	*pAutoFmtNameLst;
    String          sPageDescFirstName;
    String          sPageDescFollowName;
    String          sPageDescName;
};

inline const SvStringsDtor& ShellResource::GetAutoFmtNameLst() const
{
    if( !pAutoFmtNameLst )
        _GetAutoFmtNameLst();
    return *pAutoFmtNameLst;
}


#endif //_SHELLRES_HXX
