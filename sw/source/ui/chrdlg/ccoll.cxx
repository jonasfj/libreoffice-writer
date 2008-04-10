/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ccoll.cxx,v $
 * $Revision: 1.15 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#include "cmdid.h"
#include <tools/list.hxx>
#include "swmodule.hxx"
#include "view.hxx"
#include "wrtsh.hxx"
#include "globals.hrc"
#include "helpid.h"


#include <sfx2/styfitem.hxx>

#include "uitool.hxx"
#include "ccoll.hxx"
#include "fmtcol.hxx"
#include "hintids.hxx"
#include "docsh.hxx"
#include "docstyle.hxx"
#include "hints.hxx"

#include "chrdlg.hrc"
#include "ccoll.hrc"
#include <vcl/svapp.hxx>

#include <unomid.h>

// ******************************************************************

//!! order of entries has to be the same as in
//!! CommandStruct SwCondCollItem::aCmds[]

const char *aCommandContext[COND_COMMAND_COUNT] =
{
    "TableHeader",
    "Table",
    "Frame",
    "Section",
    "Footnote",
    "Endnote",
    "Header",
    "Footer",
    "OutlineLevel1",
    "OutlineLevel2",
    "OutlineLevel3",
    "OutlineLevel4",
    "OutlineLevel5",
    "OutlineLevel6",
    "OutlineLevel7",
    "OutlineLevel8",
    "OutlineLevel9",
    "OutlineLevel10",
    "NumberingLevel1",
    "NumberingLevel2",
    "NumberingLevel3",
    "NumberingLevel4",
    "NumberingLevel5",
    "NumberingLevel6",
    "NumberingLevel7",
    "NumberingLevel8",
    "NumberingLevel9",
    "NumberingLevel10"
};

sal_Int16 GetCommandContextIndex( const rtl::OUString &rContextName )
{
    sal_Int16 nRes = -1;
    for (sal_Int16 i = 0;  nRes == -1 && i < COND_COMMAND_COUNT;  ++i)
    {
        if (rContextName.equalsAscii( aCommandContext[i] ))
            nRes = i;
    }
    return nRes;
}

rtl::OUString GetCommandContextByIndex( sal_Int16 nIndex )
{
    rtl::OUString aRes;
    if (0 <= nIndex  &&  nIndex < COND_COMMAND_COUNT)
    {
        aRes = C2U( aCommandContext[ nIndex ] );
    }
    return aRes;
}

// Globals ******************************************************************


CommandStruct SwCondCollItem::aCmds[] =
{
    { PARA_IN_TABLEHEAD,	0 },
    { PARA_IN_TABLEBODY, 	0 },
    { PARA_IN_FRAME, 		0 },
    { PARA_IN_SECTION, 		0 },
    { PARA_IN_FOOTENOTE,	0 },
    { PARA_IN_ENDNOTE,		0 },
    { PARA_IN_HEADER, 		0 },
    { PARA_IN_FOOTER, 		0 },
    { PARA_IN_OUTLINE, 		0 },
    { PARA_IN_OUTLINE, 		1 },
    { PARA_IN_OUTLINE, 		2 },
    { PARA_IN_OUTLINE, 		3 },
    { PARA_IN_OUTLINE, 		4 },
    { PARA_IN_OUTLINE, 		5 },
    { PARA_IN_OUTLINE, 		6 },
    { PARA_IN_OUTLINE, 		7 },
    { PARA_IN_OUTLINE, 		8 },
    { PARA_IN_OUTLINE, 		9 },
    { PARA_IN_LIST, 		0 },
    { PARA_IN_LIST, 		1 },
    { PARA_IN_LIST, 		2 },
    { PARA_IN_LIST, 		3 },
    { PARA_IN_LIST, 		4 },
    { PARA_IN_LIST, 		5 },
    { PARA_IN_LIST, 		6 },
    { PARA_IN_LIST, 		7 },
    { PARA_IN_LIST, 		8 },
    { PARA_IN_LIST, 		9 }
};


TYPEINIT1_AUTOFACTORY(SwCondCollItem, SfxPoolItem)

/****************************************************************************
    Item fuer den Transport der Bedingungstabelle
****************************************************************************/


SwCondCollItem::SwCondCollItem(USHORT _nWhich ) :
    SfxPoolItem(_nWhich)
{

}
/****************************************************************************

****************************************************************************/


SwCondCollItem::~SwCondCollItem()
{
}

/****************************************************************************

****************************************************************************/


SfxPoolItem*   SwCondCollItem::Clone( SfxItemPool * /*pPool*/ ) const
{
    return new SwCondCollItem(*this);
}

/****************************************************************************

****************************************************************************/


int SwCondCollItem::operator==( const SfxPoolItem& rItem) const
{
    DBG_ASSERT( SfxPoolItem::operator==(rItem), "unterschiedliche Typen" );
    BOOL bReturn = TRUE;
    for(USHORT i = 0; i < COND_COMMAND_COUNT; i++)
        if(sStyles[i] != ((SwCondCollItem&)rItem).sStyles[i])
        {
            bReturn = FALSE;
            break;
        }

    return bReturn;
}

/****************************************************************************

****************************************************************************/


const String&	SwCondCollItem::GetStyle(USHORT nPos) const
{
#ifndef IRIX
    return nPos < COND_COMMAND_COUNT ? sStyles[nPos] : aEmptyStr;
#else
    return nPos < COND_COMMAND_COUNT ? (String)sStyles[nPos] : aEmptyStr;
#endif
}

/****************************************************************************

****************************************************************************/


void SwCondCollItem::SetStyle(const String* pStyle, USHORT nPos)
{
    if( nPos < COND_COMMAND_COUNT )
#ifndef IRIX
        sStyles[nPos] = pStyle ? *pStyle : aEmptyStr;
#else
        sStyles[nPos] = pStyle ? (String)*pStyle : aEmptyStr;
#endif
}


/****************************************************************************

****************************************************************************/


const CommandStruct* SwCondCollItem::GetCmds()
{
    return aCmds;
}




