/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlfly.hxx,v $
 * $Revision: 1.4 $
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

#ifndef _HTMLFLY_HXX
#define _HTMLFLY_HXX

#include <tools/solar.h>
#include <tools/string.hxx>

#ifndef _SVARRAY_H
#include <svl/svarray.hxx>
#endif

class SdrObject;
class SwFrmFmt;
class SwNodeIndex;
class SwPosFlyFrm;

// ACHTUNG: Die Werte dieses Enumgs gehen direkt in die
// Augabe Tabelle!!!
enum SwHTMLFrmType
{
    HTML_FRMTYPE_TABLE,
    HTML_FRMTYPE_TABLE_CAP,
    HTML_FRMTYPE_MULTICOL,
    HTML_FRMTYPE_EMPTY,
    HTML_FRMTYPE_TEXT,
    HTML_FRMTYPE_GRF,
    HTML_FRMTYPE_PLUGIN,
    HTML_FRMTYPE_APPLET,
    HTML_FRMTYPE_IFRAME,
    HTML_FRMTYPE_OLE,
    HTML_FRMTYPE_MARQUEE,
    HTML_FRMTYPE_CONTROL,
    HTML_FRMTYPE_DRAW,
    HTML_FRMTYPE_END
};

#define HTML_OUT_TBLNODE 	0x00
#define HTML_OUT_GRFNODE	0x01
#define HTML_OUT_OLENODE	0x02
#define HTML_OUT_DIV		0x03
#define HTML_OUT_MULTICOL	0x04
#define HTML_OUT_SPACER		0x05
#define HTML_OUT_CONTROL	0x06
#define HTML_OUT_AMARQUEE	0x07
#define HTML_OUT_MARQUEE	0x08
#define HTML_OUT_GRFFRM		0x09
#define HTML_OUT_OLEGRF		0x0a
#define HTML_OUT_SPAN		0x0b
#define HTML_OUT_MASK		0x0f

#define HTML_POS_PREFIX		0x00
#define HTML_POS_BEFORE		0x10
#define HTML_POS_INSIDE		0x20
#define HTML_POS_ANY		0x30
#define HTML_POS_MASK		0x30

#define HTML_CNTNR_NONE		0x00
#define HTML_CNTNR_SPAN		0x40
#define HTML_CNTNR_DIV		0x80
#define HTML_CNTNR_MASK		0xc0


const USHORT MAX_FRMTYPES = HTML_FRMTYPE_END;
const USHORT MAX_BROWSERS = 4;

extern BYTE aHTMLOutFrmPageFlyTable[MAX_FRMTYPES][MAX_BROWSERS];
extern BYTE aHTMLOutFrmParaFrameTable[MAX_FRMTYPES][MAX_BROWSERS];
extern BYTE aHTMLOutFrmParaPrtAreaTable[MAX_FRMTYPES][MAX_BROWSERS];
extern BYTE aHTMLOutFrmParaOtherTable[MAX_FRMTYPES][MAX_BROWSERS];
extern BYTE aHTMLOutFrmAsCharTable[MAX_FRMTYPES][MAX_BROWSERS];

class SwHTMLPosFlyFrm
{
    const SwFrmFmt 		*pFrmFmt;		// der Rahmen
    const SdrObject		*pSdrObject;	// ggf. Sdr-Objekt
    SwNodeIndex 		*pNdIdx;		// Node-Index
    UINT32 				nOrdNum;		// Aus SwPosFlyFrm
    xub_StrLen			nCntntIdx;		// seine Position im Content
    BYTE				nOutputMode;	// Ausgabe-Infos

public:

    SwHTMLPosFlyFrm( const SwPosFlyFrm& rPosFly,
                     const SdrObject *pSdrObj, BYTE nOutMode );

    BOOL operator==( const SwHTMLPosFlyFrm& ) const { return FALSE; }
    BOOL operator<( const SwHTMLPosFlyFrm& ) const;

    const SwFrmFmt& GetFmt() const { return *pFrmFmt; }
    const SdrObject *GetSdrObject() const { return pSdrObject; }

    const SwNodeIndex& GetNdIndex() const { return *pNdIdx; }

    xub_StrLen GetCntntIndex() const 	{ return nCntntIdx; }

    BYTE GetOutMode() const { return nOutputMode; }

    static BYTE GetOutFn( BYTE nMode ) { return nMode & HTML_OUT_MASK; }
    static BYTE GetOutPos( BYTE nMode ) { return nMode & HTML_POS_MASK; }
    static BYTE GetOutCntnr( BYTE nMode ) { return nMode & HTML_CNTNR_MASK; }

    BYTE GetOutFn() const { return nOutputMode & HTML_OUT_MASK; }
    BYTE GetOutPos() const { return nOutputMode & HTML_POS_MASK; }
    BYTE GetOutCntnr() const { return nOutputMode & HTML_CNTNR_MASK; }
};

typedef SwHTMLPosFlyFrm *SwHTMLPosFlyFrmPtr;
SV_DECL_PTRARR_SORT( SwHTMLPosFlyFrms, SwHTMLPosFlyFrmPtr, 10, 10 )


#endif
