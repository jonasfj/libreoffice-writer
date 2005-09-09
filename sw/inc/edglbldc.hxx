/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: edglbldc.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:43:11 $
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
#ifndef _EDGLBLDC_HXX
#define _EDGLBLDC_HXX

#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif

class SwSection;
class SwTOXBase;
class SwTOXBaseSection;

enum GlobalDocContentType {
    GLBLDOC_UNKNOWN,
    GLBLDOC_TOXBASE,
    GLBLDOC_SECTION
};

class SwGlblDocContent
{
    GlobalDocContentType eType;
    ULONG nDocPos;
    union {
        const SwTOXBase* pTOX;
        const SwSection* pSect;
    } PTR;

public:
    SwGlblDocContent( ULONG nPos );
    SwGlblDocContent( const SwTOXBaseSection* pTOX );
    SwGlblDocContent( const SwSection* pSect );

    // Inhalte abfragen
    GlobalDocContentType GetType() const { return eType; }
    const SwSection* GetSection() const
                            { return GLBLDOC_SECTION == eType ? PTR.pSect : 0; }
    const SwTOXBase* GetTOX() const
                            { return GLBLDOC_TOXBASE == eType ? PTR.pTOX : 0; }
    ULONG GetDocPos() const { return nDocPos; }

    // fuers Sortieren
    inline int operator==( const SwGlblDocContent& rCmp ) const
        {	return GetDocPos() == rCmp.GetDocPos(); }
    inline int operator<( const SwGlblDocContent& rCmp ) const
        {	return GetDocPos() < rCmp.GetDocPos(); }
};


typedef SwGlblDocContent* SwGlblDocContentPtr;
SV_DECL_PTRARR_SORT_DEL( SwGlblDocContents, SwGlblDocContentPtr, 10, 10 );



#endif
