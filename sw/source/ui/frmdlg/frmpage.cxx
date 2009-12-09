/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: frmpage.cxx,v $
 * $Revision: 1.67 $
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
#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif

#include <com/sun/star/embed/Aspects.hpp>
#include <com/sun/star/embed/EmbedMisc.hpp>

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _HELPID_H
#include <helpid.h>
#endif
#include <hintids.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/mnemonic.hxx>
#include <svtools/urihelper.hxx>
#include <svtools/stritem.hxx>
#include <svx/impgrf.hxx>
#include <svx/htmlmode.hxx>
#include <svx/sizeitem.hxx>
#include <svx/opaqitem.hxx>
#include <svx/protitem.hxx>
#include <svx/prntitem.hxx>
#include <svx/brshitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/frmdiritem.hxx>
#include <svx/swframevalidation.hxx>
#include <sot/clsids.hxx>

#include <sfx2/viewfrm.hxx>
#include <fmturl.hxx>
#include <fmteiro.hxx>
#include <fmtcnct.hxx>
#include <view.hxx>
#include <wrtsh.hxx>
#include <swmodule.hxx>
#include <uitool.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <viewopt.hxx>
#include <frmatr.hxx>
#include <frmdlg.hxx>
#include <frmmgr.hxx>
#include <frmpage.hxx>
#include <wrap.hxx>
#include <colmgr.hxx>
#include <grfatr.hxx>
#include <uiitems.hxx>
// OD 19.09.2003 #i18732#
#include <fmtfollowtextflow.hxx>

#ifndef _FRMUI_HRC
#include <frmui.hrc>
#endif
#ifndef _FRMPAGE_HRC
#include <frmpage.hrc>
#endif
#include <sfx2/filedlghelper.hxx>
#include "com/sun/star/ui/dialogs/TemplateDescription.hpp"
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#include <com/sun/star/ui/dialogs/ExtendedFilePickerElementIds.hpp>

using namespace ::com::sun::star;
using ::rtl::OUString;
using namespace ::sfx2;

#define SwFPos SvxSwFramePosString

struct FrmMap
{
    SvxSwFramePosString::StringId eStrId;
    SvxSwFramePosString::StringId eMirrorStrId;
    USHORT nAlign;
    ULONG  nLBRelations;
};

struct RelationMap
{
    SvxSwFramePosString::StringId eStrId;
    SvxSwFramePosString::StringId eMirrorStrId;
    ULONG  nLBRelation;
    USHORT nRelation;
};

struct StringIdPair_Impl
{
    SvxSwFramePosString::StringId eHori;
    SvxSwFramePosString::StringId eVert;
};

#define MAX_PERCENT_WIDTH   254L
#define MAX_PERCENT_HEIGHT  254L

// OD 19.09.2003 #i18732# - change order of alignments
#define LB_FRAME                0x00000001L // Textbereich des Absatzes
#define LB_PRTAREA				0x00000002L	// Textbereich des Absatzes + Einzuege
#define LB_VERT_FRAME           0x00000004L // Vertikaler Textbereich des Absatzes
#define LB_VERT_PRTAREA         0x00000008L // Vertikaler Textbereich des Absatzes + Einzuege
#define LB_REL_FRM_LEFT         0x00000010L // Linker Absatzrand
#define LB_REL_FRM_RIGHT        0x00000020L // Rechter Absatzrand

#define LB_REL_PG_LEFT          0x00000040L // Linker Seitenrand
#define LB_REL_PG_RIGHT         0x00000080L    // Rechter Seitenrand
#define LB_REL_PG_FRAME         0x00000100L // Gesamte Seite
#define LB_REL_PG_PRTAREA       0x00000200L    // Textbereich der Seite

#define LB_FLY_REL_PG_LEFT      0x00000400L    // Linker Rahmenrand
#define LB_FLY_REL_PG_RIGHT     0x00000800L    // Rechter Rahmenrand
#define LB_FLY_REL_PG_FRAME     0x00001000L    // Gesamte Rahmen
#define LB_FLY_REL_PG_PRTAREA   0x00002000L    // Rahmeninneres

#define LB_REL_BASE				0x00010000L	// Zeichenausrichtung Basis
#define LB_REL_CHAR				0x00020000L	// Zeichenausrichtung Zeichen
#define LB_REL_ROW				0x00040000L	// Zeichenausrichtung Zeile

// OD 10.11.2003 #i22305#
#define LB_FLY_VERT_FRAME       0x00100000L // vertical entire frame
#define LB_FLY_VERT_PRTAREA     0x00200000L // vertical frame text area

// OD 11.11.2003 #i22341#
#define LB_VERT_LINE            0x00400000L // vertical text line

static RelationMap __FAR_DATA aRelationMap[] =
{
    {SwFPos::FRAME,  SwFPos::FRAME, LB_FRAME, text::RelOrientation::FRAME},
    {SwFPos::PRTAREA,           SwFPos::PRTAREA,                LB_PRTAREA,             text::RelOrientation::PRINT_AREA},
    {SwFPos::REL_PG_LEFT,       SwFPos::MIR_REL_PG_LEFT,        LB_REL_PG_LEFT,         text::RelOrientation::PAGE_LEFT},
    {SwFPos::REL_PG_RIGHT,      SwFPos::MIR_REL_PG_RIGHT,       LB_REL_PG_RIGHT,        text::RelOrientation::PAGE_RIGHT},
    {SwFPos::REL_FRM_LEFT,      SwFPos::MIR_REL_FRM_LEFT,       LB_REL_FRM_LEFT,        text::RelOrientation::FRAME_LEFT},
    {SwFPos::REL_FRM_RIGHT,     SwFPos::MIR_REL_FRM_RIGHT,      LB_REL_FRM_RIGHT,       text::RelOrientation::FRAME_RIGHT},
    {SwFPos::REL_PG_FRAME,      SwFPos::REL_PG_FRAME,           LB_REL_PG_FRAME,        text::RelOrientation::PAGE_FRAME},
    {SwFPos::REL_PG_PRTAREA,    SwFPos::REL_PG_PRTAREA,         LB_REL_PG_PRTAREA,      text::RelOrientation::PAGE_PRINT_AREA},
    {SwFPos::REL_CHAR,          SwFPos::REL_CHAR,               LB_REL_CHAR,            text::RelOrientation::CHAR},

    {SwFPos::FLY_REL_PG_LEFT,       SwFPos::FLY_MIR_REL_PG_LEFT,    LB_FLY_REL_PG_LEFT,     text::RelOrientation::PAGE_LEFT},
    {SwFPos::FLY_REL_PG_RIGHT,      SwFPos::FLY_MIR_REL_PG_RIGHT,   LB_FLY_REL_PG_RIGHT,    text::RelOrientation::PAGE_RIGHT},
    {SwFPos::FLY_REL_PG_FRAME,      SwFPos::FLY_REL_PG_FRAME,       LB_FLY_REL_PG_FRAME,    text::RelOrientation::PAGE_FRAME},
    {SwFPos::FLY_REL_PG_PRTAREA,    SwFPos::FLY_REL_PG_PRTAREA,     LB_FLY_REL_PG_PRTAREA,  text::RelOrientation::PAGE_PRINT_AREA},

    {SwFPos::REL_BORDER,        SwFPos::REL_BORDER,             LB_VERT_FRAME,          text::RelOrientation::FRAME},
    {SwFPos::REL_PRTAREA,       SwFPos::REL_PRTAREA,            LB_VERT_PRTAREA,        text::RelOrientation::PRINT_AREA},

    // OD 10.11.2003 #i22305#
    {SwFPos::FLY_REL_PG_FRAME,      SwFPos::FLY_REL_PG_FRAME,   LB_FLY_VERT_FRAME,      text::RelOrientation::FRAME},
    {SwFPos::FLY_REL_PG_PRTAREA,    SwFPos::FLY_REL_PG_PRTAREA,     LB_FLY_VERT_PRTAREA,    text::RelOrientation::PRINT_AREA},

    // OD 11.11.2003 #i22341#
    {SwFPos::REL_LINE,  SwFPos::REL_LINE,   LB_VERT_LINE,   text::RelOrientation::TEXT_LINE}
};

static RelationMap __FAR_DATA aAsCharRelationMap[] =
{
    {SwFPos::REL_BASE,  SwFPos::REL_BASE,   LB_REL_BASE,    text::RelOrientation::FRAME},
    {SwFPos::REL_CHAR,   SwFPos::REL_CHAR,   LB_REL_CHAR,   text::RelOrientation::FRAME},
    {SwFPos::REL_ROW,    SwFPos::REL_ROW,   LB_REL_ROW,     text::RelOrientation::FRAME}
};

/*--------------------------------------------------------------------
    Beschreibung: Seite verankert
 --------------------------------------------------------------------*/

#define HORI_PAGE_REL	(LB_REL_PG_FRAME|LB_REL_PG_PRTAREA|LB_REL_PG_LEFT| \
                        LB_REL_PG_RIGHT)

static FrmMap __FAR_DATA aHPageMap[] =
{
    {SwFPos::LEFT,          SwFPos::MIR_LEFT,       text::HoriOrientation::LEFT,      HORI_PAGE_REL},
    {SwFPos::RIGHT,         SwFPos::MIR_RIGHT,      text::HoriOrientation::RIGHT,     HORI_PAGE_REL},
    {SwFPos::CENTER_HORI,   SwFPos::CENTER_HORI,    text::HoriOrientation::CENTER,    HORI_PAGE_REL},
    {SwFPos::FROMLEFT,      SwFPos::MIR_FROMLEFT,   text::HoriOrientation::NONE,      HORI_PAGE_REL}
};

static FrmMap __FAR_DATA aHPageHtmlMap[] =
{
    {SwFPos::FROMLEFT,      SwFPos::MIR_FROMLEFT,   text::HoriOrientation::NONE,      LB_REL_PG_FRAME}
};

#define VERT_PAGE_REL	(LB_REL_PG_FRAME|LB_REL_PG_PRTAREA)

static FrmMap __FAR_DATA aVPageMap[] =
{
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::TOP,       VERT_PAGE_REL},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::BOTTOM,    VERT_PAGE_REL},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::CENTER,    VERT_PAGE_REL},
    {SwFPos::FROMTOP,       SwFPos::FROMTOP,        text::VertOrientation::NONE,      VERT_PAGE_REL}
};

static FrmMap __FAR_DATA aVPageHtmlMap[] =
{
    {SwFPos::FROMTOP,       SwFPos::FROMTOP,        text::VertOrientation::NONE,      LB_REL_PG_FRAME}
};

/*--------------------------------------------------------------------
    Beschreibung: Rahmen verankert
 --------------------------------------------------------------------*/

#define HORI_FRAME_REL	(LB_FLY_REL_PG_FRAME|LB_FLY_REL_PG_PRTAREA| \
                        LB_FLY_REL_PG_LEFT|LB_FLY_REL_PG_RIGHT)

static FrmMap __FAR_DATA aHFrameMap[] =
{
    {SwFPos::LEFT,          SwFPos::MIR_LEFT,       text::HoriOrientation::LEFT,  HORI_FRAME_REL},
    {SwFPos::RIGHT,         SwFPos::MIR_RIGHT,      text::HoriOrientation::RIGHT,     HORI_FRAME_REL},
    {SwFPos::CENTER_HORI,   SwFPos::CENTER_HORI,    text::HoriOrientation::CENTER,    HORI_FRAME_REL},
    {SwFPos::FROMLEFT,      SwFPos::MIR_FROMLEFT,   text::HoriOrientation::NONE,      HORI_FRAME_REL}
};

static FrmMap __FAR_DATA aHFlyHtmlMap[] =
{
    {SwFPos::LEFT,          SwFPos::MIR_LEFT,       text::HoriOrientation::LEFT,      LB_FLY_REL_PG_FRAME},
    {SwFPos::FROMLEFT,      SwFPos::MIR_FROMLEFT,   text::HoriOrientation::NONE,      LB_FLY_REL_PG_FRAME}
};

// OD 19.09.2003 #i18732# - own vertical alignment map for to frame anchored objects
// OD 10.11.2003 #i22305#
#define VERT_FRAME_REL   (LB_FLY_VERT_FRAME|LB_FLY_VERT_PRTAREA)

static FrmMap __FAR_DATA aVFrameMap[] =
{
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::TOP,       VERT_FRAME_REL},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::BOTTOM,    VERT_FRAME_REL},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::CENTER,    VERT_FRAME_REL},
    {SwFPos::FROMTOP,       SwFPos::FROMTOP,        text::VertOrientation::NONE,      VERT_FRAME_REL}
};

static FrmMap __FAR_DATA aVFlyHtmlMap[] =
{
    // OD 10.11.2003 #i22305#
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::TOP,       LB_FLY_VERT_FRAME},
    {SwFPos::FROMTOP,       SwFPos::FROMTOP,        text::VertOrientation::NONE,      LB_FLY_VERT_FRAME}
};

/*--------------------------------------------------------------------
    Beschreibung: Absatz verankert
 --------------------------------------------------------------------*/

#define HORI_PARA_REL	(LB_FRAME|LB_PRTAREA|LB_REL_PG_LEFT|LB_REL_PG_RIGHT| \
                        LB_REL_PG_FRAME|LB_REL_PG_PRTAREA|LB_REL_FRM_LEFT| \
                        LB_REL_FRM_RIGHT)

static FrmMap __FAR_DATA aHParaMap[] =
{
    {SwFPos::LEFT,          SwFPos::MIR_LEFT,       text::HoriOrientation::LEFT,      HORI_PARA_REL},
    {SwFPos::RIGHT,         SwFPos::MIR_RIGHT,      text::HoriOrientation::RIGHT,     HORI_PARA_REL},
    {SwFPos::CENTER_HORI,   SwFPos::CENTER_HORI,    text::HoriOrientation::CENTER,    HORI_PARA_REL},
    {SwFPos::FROMLEFT,      SwFPos::MIR_FROMLEFT,   text::HoriOrientation::NONE,      HORI_PARA_REL}
};

#define HTML_HORI_PARA_REL	(LB_FRAME|LB_PRTAREA)

static FrmMap __FAR_DATA aHParaHtmlMap[] =
{
    {SwFPos::LEFT,  SwFPos::LEFT,   text::HoriOrientation::LEFT,      HTML_HORI_PARA_REL},
    {SwFPos::RIGHT, SwFPos::RIGHT,  text::HoriOrientation::RIGHT,     HTML_HORI_PARA_REL}
};

static FrmMap __FAR_DATA aHParaHtmlAbsMap[] =
{
    {SwFPos::LEFT,          SwFPos::MIR_LEFT,       text::HoriOrientation::LEFT,      HTML_HORI_PARA_REL},
    {SwFPos::RIGHT,         SwFPos::MIR_RIGHT,      text::HoriOrientation::RIGHT,     HTML_HORI_PARA_REL}
};

// OD 19.09.2003 #i18732# - allow vertical alignment at page areas
#define VERT_PARA_REL   (LB_VERT_FRAME|LB_VERT_PRTAREA| \
                         LB_REL_PG_FRAME|LB_REL_PG_PRTAREA)

static FrmMap __FAR_DATA aVParaMap[] =
{
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::TOP,       VERT_PARA_REL},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::BOTTOM,    VERT_PARA_REL},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::CENTER,    VERT_PARA_REL},
    {SwFPos::FROMTOP,       SwFPos::FROMTOP,        text::VertOrientation::NONE,      VERT_PARA_REL}
};

static FrmMap __FAR_DATA aVParaHtmlMap[] =
{
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::TOP,       LB_VERT_PRTAREA}
};

/*--------------------------------------------------------------------
    Beschreibung: Relativ zum Zeichen verankert
 --------------------------------------------------------------------*/

#define HORI_CHAR_REL	(LB_FRAME|LB_PRTAREA|LB_REL_PG_LEFT|LB_REL_PG_RIGHT| \
                        LB_REL_PG_FRAME|LB_REL_PG_PRTAREA|LB_REL_FRM_LEFT| \
                        LB_REL_FRM_RIGHT|LB_REL_CHAR)

static FrmMap __FAR_DATA aHCharMap[] =
{
    {SwFPos::LEFT,          SwFPos::MIR_LEFT,       text::HoriOrientation::LEFT,      HORI_CHAR_REL},
    {SwFPos::RIGHT,         SwFPos::MIR_RIGHT,      text::HoriOrientation::RIGHT,     HORI_CHAR_REL},
    {SwFPos::CENTER_HORI,   SwFPos::CENTER_HORI,    text::HoriOrientation::CENTER,    HORI_CHAR_REL},
    {SwFPos::FROMLEFT,      SwFPos::MIR_FROMLEFT,   text::HoriOrientation::NONE,      HORI_CHAR_REL}
};

#define HTML_HORI_CHAR_REL	(LB_FRAME|LB_PRTAREA|LB_REL_CHAR)

static FrmMap __FAR_DATA aHCharHtmlMap[] =
{
    {SwFPos::LEFT,          SwFPos::LEFT,           text::HoriOrientation::LEFT,      HTML_HORI_CHAR_REL},
    {SwFPos::RIGHT,         SwFPos::RIGHT,          text::HoriOrientation::RIGHT,     HTML_HORI_CHAR_REL}
};

static FrmMap __FAR_DATA aHCharHtmlAbsMap[] =
{
    {SwFPos::LEFT,          SwFPos::MIR_LEFT,       text::HoriOrientation::LEFT,      LB_PRTAREA|LB_REL_CHAR},
    {SwFPos::RIGHT,         SwFPos::MIR_RIGHT,      text::HoriOrientation::RIGHT,     LB_PRTAREA},
    {SwFPos::FROMLEFT,      SwFPos::MIR_FROMLEFT,   text::HoriOrientation::NONE,      LB_REL_PG_FRAME}
};

// OD 19.09.2003 #i18732# - allow vertical alignment at page areas
// OD 12.11.2003 #i22341# - handle <LB_REL_CHAR> on its own
#define VERT_CHAR_REL   (LB_VERT_FRAME|LB_VERT_PRTAREA| \
                         LB_REL_PG_FRAME|LB_REL_PG_PRTAREA)

static FrmMap __FAR_DATA aVCharMap[] =
{
    // OD 11.11.2003 #i22341#
    // introduce mappings for new vertical alignment at top of line <LB_VERT_LINE>
    // and correct mapping for vertical alignment at character for position <FROM_BOTTOM>
    // Note: Because of these adjustments the map becomes ambigous in its values
    //       <eStrId>/<eMirrorStrId> and <nAlign>. These ambiguities are considered
    //       in the methods <SwFrmPage::FillRelLB(..)>, <SwFrmPage::GetAlignment(..)>
    //       and <SwFrmPage::FillPosLB(..)>
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::TOP,           VERT_CHAR_REL|LB_REL_CHAR},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::BOTTOM,        VERT_CHAR_REL|LB_REL_CHAR},
    {SwFPos::BELOW,         SwFPos::BELOW,          text::VertOrientation::CHAR_BOTTOM,   LB_REL_CHAR},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::CENTER,        VERT_CHAR_REL|LB_REL_CHAR},
    {SwFPos::FROMTOP,       SwFPos::FROMTOP,        text::VertOrientation::NONE,          VERT_CHAR_REL},
    {SwFPos::FROMBOTTOM,    SwFPos::FROMBOTTOM,     text::VertOrientation::NONE,          LB_REL_CHAR|LB_VERT_LINE},
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::LINE_TOP,      LB_VERT_LINE},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::LINE_BOTTOM,   LB_VERT_LINE},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::LINE_CENTER,   LB_VERT_LINE}
};


static FrmMap __FAR_DATA aVCharHtmlMap[] =
{
    {SwFPos::BELOW,         SwFPos::BELOW,          text::VertOrientation::CHAR_BOTTOM,   LB_REL_CHAR}
};

static FrmMap __FAR_DATA aVCharHtmlAbsMap[] =
{
    {SwFPos::TOP,           SwFPos::TOP,            text::VertOrientation::TOP,           LB_REL_CHAR},
    {SwFPos::BELOW,             SwFPos::BELOW,          text::VertOrientation::CHAR_BOTTOM,   LB_REL_CHAR}
};
/*--------------------------------------------------------------------
    Beschreibung: Als Zeichen verankert
 --------------------------------------------------------------------*/

static FrmMap __FAR_DATA aVAsCharMap[] =
{
    {SwFPos::TOP,               SwFPos::TOP,            text::VertOrientation::TOP,           LB_REL_BASE},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::BOTTOM,        LB_REL_BASE},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::CENTER,        LB_REL_BASE},

    {SwFPos::TOP,               SwFPos::TOP,            text::VertOrientation::CHAR_TOP,      LB_REL_CHAR},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::CHAR_BOTTOM,   LB_REL_CHAR},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::CHAR_CENTER,   LB_REL_CHAR},

    {SwFPos::TOP,               SwFPos::TOP,            text::VertOrientation::LINE_TOP,      LB_REL_ROW},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::LINE_BOTTOM,   LB_REL_ROW},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::LINE_CENTER,   LB_REL_ROW},

    {SwFPos::FROMBOTTOM,    SwFPos::FROMBOTTOM,     text::VertOrientation::NONE,          LB_REL_BASE}
};

static FrmMap __FAR_DATA aVAsCharHtmlMap[] =
{
    {SwFPos::TOP,               SwFPos::TOP,            text::VertOrientation::TOP,           LB_REL_BASE},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::CENTER,        LB_REL_BASE},

    {SwFPos::TOP,               SwFPos::TOP,            text::VertOrientation::CHAR_TOP,      LB_REL_CHAR},

    {SwFPos::TOP,               SwFPos::TOP,            text::VertOrientation::LINE_TOP,      LB_REL_ROW},
    {SwFPos::BOTTOM,        SwFPos::BOTTOM,         text::VertOrientation::LINE_BOTTOM,   LB_REL_ROW},
    {SwFPos::CENTER_VERT,   SwFPos::CENTER_VERT,    text::VertOrientation::LINE_CENTER,   LB_REL_ROW}
};

static USHORT __FAR_DATA aPageRg[] = {
    RES_FRM_SIZE, RES_FRM_SIZE,
    RES_VERT_ORIENT, RES_ANCHOR,
    RES_COL, RES_COL,
    RES_FOLLOW_TEXT_FLOW, RES_FOLLOW_TEXT_FLOW,
    0
};
static USHORT __FAR_DATA aAddPgRg[] = {
    RES_PROTECT, 			RES_PROTECT,
    RES_PRINT, 				RES_PRINT,
    FN_SET_FRM_NAME,		FN_SET_FRM_NAME,
    FN_SET_FRM_ALT_NAME,	FN_SET_FRM_ALT_NAME,
    0
};

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

size_t lcl_GetFrmMapCount( const FrmMap* pMap)
{
    if ( pMap )
    {
        int aSizeOf = sizeof(FrmMap);
        if( pMap == aVParaHtmlMap)
            return sizeof(aVParaHtmlMap) / aSizeOf;
        if( pMap == aVAsCharHtmlMap)
            return sizeof(aVAsCharHtmlMap) / aSizeOf;
        if( pMap == aHParaHtmlMap)
            return sizeof(aHParaHtmlMap) / aSizeOf;
        if( pMap == aHParaHtmlAbsMap)
            return sizeof(aHParaHtmlAbsMap) / aSizeOf;
        if ( pMap == aVPageMap )
            return sizeof(aVPageMap) / aSizeOf;
        if ( pMap == aVPageHtmlMap )
            return sizeof(aVPageHtmlMap) / aSizeOf;
        if ( pMap == aVAsCharMap )
            return sizeof(aVAsCharMap) / aSizeOf;
        if ( pMap == aVParaMap )
            return sizeof(aVParaMap) / aSizeOf;
        if ( pMap == aHParaMap )
            return sizeof(aHParaMap) / aSizeOf;
        if ( pMap == aHFrameMap )
            return sizeof(aHFrameMap) / aSizeOf;
        // OD 19.09.2003 #i18732# - own vertical alignment map for to frame anchored objects
        if ( pMap == aVFrameMap )
            return sizeof(aVFrameMap) / aSizeOf;
        if ( pMap == aHCharMap )
            return sizeof(aHCharMap) / aSizeOf;
        if ( pMap == aHCharHtmlMap )
            return sizeof(aHCharHtmlMap) / aSizeOf;
        if ( pMap == aHCharHtmlAbsMap )
            return sizeof(aHCharHtmlAbsMap) / aSizeOf;
        if ( pMap == aVCharMap )
            return sizeof(aVCharMap) / aSizeOf;
        if ( pMap == aVCharHtmlMap )
            return sizeof(aVCharHtmlMap) / aSizeOf;
        if ( pMap == aVCharHtmlAbsMap )
            return sizeof(aVCharHtmlAbsMap) / aSizeOf;
        if ( pMap == aHPageHtmlMap )
            return sizeof(aHPageHtmlMap) / aSizeOf;
        if ( pMap == aHFlyHtmlMap )
            return sizeof(aHFlyHtmlMap) / aSizeOf;
        if ( pMap == aVFlyHtmlMap )
            return sizeof(aVFlyHtmlMap) / aSizeOf;
        return sizeof(aHPageMap) / aSizeOf;
    }
    return 0;
}
/* -----------------------------08.08.2002 14:45------------------------------

 ---------------------------------------------------------------------------*/
void lcl_InsertVectors(ListBox& rBox,
    const ::std::vector< String >& rPrev, const ::std::vector< String >& rThis,
    const ::std::vector< String >& rNext, const ::std::vector< String >& rRemain)
{
    ::std::vector< String >::const_iterator aIt;
    USHORT nEntry = 0;
    for(aIt = rPrev.begin(); aIt != rPrev.end(); aIt++)
        nEntry = rBox.InsertEntry(*aIt);
    for(aIt = rThis.begin(); aIt != rThis.end(); aIt++)
        nEntry = rBox.InsertEntry(*aIt);
    for(aIt = rNext.begin(); aIt != rNext.end(); aIt++)
        nEntry = rBox.InsertEntry(*aIt);
    rBox.SetSeparatorPos(nEntry);
    //now insert all strings sorted
    USHORT nStartPos = rBox.GetEntryCount();

    for(aIt = rPrev.begin(); aIt != rPrev.end(); aIt++)
        ::InsertStringSorted(*aIt, rBox, nStartPos );
    for(aIt = rThis.begin(); aIt != rThis.end(); aIt++)
        ::InsertStringSorted(*aIt, rBox, nStartPos );
    for(aIt = rNext.begin(); aIt != rNext.end(); aIt++)
        ::InsertStringSorted(*aIt, rBox, nStartPos );
    for(aIt = rRemain.begin(); aIt != rRemain.end(); aIt++)
        ::InsertStringSorted(*aIt, rBox, nStartPos );
}

/* -----------------------------20.08.2002 16:12------------------------------

 ---------------------------------------------------------------------------*/
SvxSwFramePosString::StringId lcl_ChangeResIdToVerticalOrRTL(SvxSwFramePosString::StringId eStringId, BOOL bVertical, BOOL bRTL)
{
    //special handling of STR_FROMLEFT
    if(SwFPos::FROMLEFT == eStringId)
    {
        eStringId = bVertical ?
            bRTL ? SwFPos::FROMBOTTOM : SwFPos::FROMTOP :
            bRTL ? SwFPos::FROMRIGHT : SwFPos::FROMLEFT;
        return eStringId;
    }
    if(bVertical)
    {
        //exchange horizontal strings with vertical strings and vice versa
        static const StringIdPair_Impl aHoriIds[] =
        {
            {SwFPos::LEFT,           SwFPos::TOP},
            {SwFPos::RIGHT,          SwFPos::BOTTOM},
            {SwFPos::CENTER_HORI,    SwFPos::CENTER_VERT},
            {SwFPos::FROMTOP,        SwFPos::FROMRIGHT},
            {SwFPos::REL_PG_LEFT,    SwFPos::REL_PG_TOP},
            {SwFPos::REL_PG_RIGHT,   SwFPos::REL_PG_BOTTOM} ,
            {SwFPos::REL_FRM_LEFT,   SwFPos::REL_FRM_TOP},
            {SwFPos::REL_FRM_RIGHT,  SwFPos::REL_FRM_BOTTOM}
        };
        static const StringIdPair_Impl aVertIds[] =
        {
            {SwFPos::TOP,            SwFPos::RIGHT},
            {SwFPos::BOTTOM,         SwFPos::LEFT },
            {SwFPos::CENTER_VERT,    SwFPos::CENTER_HORI},
            {SwFPos::FROMTOP,        SwFPos::FROMRIGHT },
            {SwFPos::REL_PG_TOP,     SwFPos::REL_PG_LEFT },
            {SwFPos::REL_PG_BOTTOM,  SwFPos::REL_PG_RIGHT } ,
            {SwFPos::REL_FRM_TOP,    SwFPos::REL_FRM_LEFT },
            {SwFPos::REL_FRM_BOTTOM, SwFPos::REL_FRM_RIGHT }
        };
        USHORT nIndex;
        for(nIndex = 0; nIndex < sizeof(aHoriIds) / sizeof(StringIdPair_Impl); ++nIndex)
        {
            if(aHoriIds[nIndex].eHori == eStringId)
            {
                eStringId = aHoriIds[nIndex].eVert;
                return eStringId;
            }
        }
        nIndex = 0;
        for(nIndex = 0; nIndex < sizeof(aVertIds) / sizeof(StringIdPair_Impl); ++nIndex)
        {
            if(aVertIds[nIndex].eHori == eStringId)
            {
                eStringId = aVertIds[nIndex].eVert;
                break;
            }
        }
    }
    return eStringId;
}

// OD 12.11.2003 #i22341# - helper method in order to determine all possible
// listbox relations in a relation map for a given relation
ULONG lcl_GetLBRelationsForRelations( const USHORT _nRel )
{
    ULONG nLBRelations = 0L;

    sal_uInt16 nRelMapSize = sizeof(aRelationMap) / sizeof(RelationMap);
    for ( sal_uInt16 nRelMapPos = 0; nRelMapPos < nRelMapSize; ++nRelMapPos )
    {
        if ( aRelationMap[nRelMapPos].nRelation == _nRel )
        {
            nLBRelations |= aRelationMap[nRelMapPos].nLBRelation;
        }
    }

    return nLBRelations;
}

// OD 14.11.2003 #i22341# - helper method on order to determine all possible
// listbox relations in a relation map for a given string ID
ULONG lcl_GetLBRelationsForStrID( const FrmMap* _pMap,
                                  const SvxSwFramePosString::StringId _eStrId,
                                  const bool _bUseMirrorStr )
{
    ULONG nLBRelations = 0L;

    size_t nRelMapSize = lcl_GetFrmMapCount( _pMap );
    for ( size_t nRelMapPos = 0; nRelMapPos < nRelMapSize; ++nRelMapPos )
    {
        if ( ( !_bUseMirrorStr && _pMap[nRelMapPos].eStrId == _eStrId ) ||
             ( _bUseMirrorStr && _pMap[nRelMapPos].eMirrorStrId == _eStrId ) )
        {
            nLBRelations |= _pMap[nRelMapPos].nLBRelations;
        }
    }

    return nLBRelations;
}

/*--------------------------------------------------------------------
    Beschreibung:	StandardRahmenTabPage
 --------------------------------------------------------------------*/

namespace
{
    void HandleAutoCB( BOOL _bChecked, FixedText& _rFT_man, FixedText& _rFT_auto )
    {
        _rFT_man.Show( !_bChecked );
        _rFT_auto.Show( _bChecked );
    }
}


SwFrmPage::SwFrmPage ( Window *pParent, const SfxItemSet &rSet ) :
    SfxTabPage		(pParent, SW_RES(TP_FRM_STD), rSet),

    aWidthFT        (this, SW_RES(FT_WIDTH)),
    aWidthAutoFT	(this, SW_RES(FT_WIDTH_AUTO)),
    aWidthED		(this, SW_RES(ED_WIDTH)),
    aRelWidthCB		(this, SW_RES(CB_REL_WIDTH)),
    aAutoWidthCB	(this, SW_RES(CB_AUTOWIDTH)),
    aHeightFT		(this, SW_RES(FT_HEIGHT)),
    aHeightAutoFT	(this, SW_RES(FT_HEIGHT_AUTO)),
    aHeightED		(this, SW_RES(ED_HEIGHT)),
    aRelHeightCB	(this, SW_RES(CB_REL_HEIGHT)),
    aAutoHeightCB	(this, SW_RES(CB_AUTOHEIGHT)),
    aFixedRatioCB	(this, SW_RES(CB_FIXEDRATIO)),
    aRealSizeBT     (this, SW_RES(BT_REALSIZE)),
    aSizeFL         (this, SW_RES(FL_SIZE)),

    aTypeFL        (this, SW_RES(FL_TYPE)),
    aTypeSepFL     (this, SW_RES(FL_TYPE_SEP)),
    aAnchorAtPageRB (this, SW_RES(RB_ANCHOR_PAGE)),
    aAnchorAtParaRB	(this, SW_RES(RB_ANCHOR_PARA)),
    aAnchorAtCharRB	(this, SW_RES(RB_ANCHOR_AT_CHAR)),
    aAnchorAsCharRB	(this, SW_RES(RB_ANCHOR_AS_CHAR)),
    aAnchorAtFrameRB(this, SW_RES(RB_ANCHOR_FRAME)),

    aHorizontalFT	(this, SW_RES(FT_HORIZONTAL)),
    aHorizontalDLB	(this, SW_RES(DLB_HORIZONTAL)),
    aAtHorzPosFT	(this, SW_RES(FT_AT_HORZ_POS)),
    aAtHorzPosED	(this, SW_RES(ED_AT_HORZ_POS)),
    aHoriRelationFT (this, SW_RES(FT_HORI_RELATION)),
    aHoriRelationLB (this, SW_RES(LB_HORI_RELATION)),
    aMirrorPagesCB  (this, SW_RES(CB_MIRROR)),
    aVerticalFT		(this, SW_RES(FT_VERTICAL)),
    aVerticalDLB	(this, SW_RES(DLB_VERTICAL)),
    aAtVertPosFT	(this, SW_RES(FT_AT_VERT_POS)),
    aAtVertPosED	(this, SW_RES(ED_AT_VERT_POS)),
    aVertRelationFT (this, SW_RES(FT_VERT_RELATION)),
    aVertRelationLB (this, SW_RES(LB_VERT_RELATION)),
    // OD 19.09.2003 #i18732# - new checkbox
    aFollowTextFlowCB(this, SW_RES(CB_FOLLOWTEXTFLOW)),
    aPositionFL     (this, SW_RES(FL_POSITION)),

    aExampleWN      (this, SW_RES(WN_BSP)),

    bAtHorzPosModified( FALSE ),
    bAtVertPosModified( FALSE ),

    bFormat(FALSE),
    bNew(TRUE),
    bNoModifyHdl(TRUE),
    bVerticalChanged(FALSE),
    bIsVerticalFrame(FALSE),
    bIsInRightToLeft(FALSE),
    bHtmlMode(FALSE),
    nHtmlMode(0),
    nDlgType(0),
    nUpperBorder(0),
    nLowerBorder(0),
    fWidthHeightRatio(1.0),
    // OD 12.11.2003 #i22341#
    mpToCharCntntPos( NULL ),

    nOldH(text::HoriOrientation::CENTER),
    nOldHRel(text::RelOrientation::FRAME),
    nOldV(text::VertOrientation::TOP),
    nOldVRel(text::RelOrientation::PRINT_AREA),
    pVMap( 0 ),
    pHMap( 0 )
{
    FreeResource();
    SetExchangeSupport();

    Link aLk = LINK(this, SwFrmPage, RangeModifyHdl);
    aWidthED.	 SetLoseFocusHdl( aLk );
    aHeightED.	 SetLoseFocusHdl( aLk );
    aAtHorzPosED.SetLoseFocusHdl( aLk );
    aAtVertPosED.SetLoseFocusHdl( aLk );
    // OD 25.09.2003 #i18732# - click handler for new checkbox
    aFollowTextFlowCB.SetClickHdl( aLk );

    aLk = LINK(this, SwFrmPage, ModifyHdl);
    aWidthED.	 SetModifyHdl( aLk );
    aHeightED.	 SetModifyHdl( aLk );
    aAtHorzPosED.SetModifyHdl( aLk );
    aAtVertPosED.SetModifyHdl( aLk );

    aLk = LINK(this, SwFrmPage, AnchorTypeHdl);
    aAnchorAtPageRB.SetClickHdl( aLk );
    aAnchorAtParaRB.SetClickHdl( aLk );
    aAnchorAtCharRB.SetClickHdl( aLk );
    aAnchorAsCharRB.SetClickHdl( aLk );
    aAnchorAtFrameRB.SetClickHdl( aLk );

    aHorizontalDLB.SetSelectHdl(LINK(this, SwFrmPage, PosHdl));
    aVerticalDLB.  SetSelectHdl(LINK(this, SwFrmPage, PosHdl));

    aHoriRelationLB.SetSelectHdl(LINK(this, SwFrmPage, RelHdl));
    aVertRelationLB.SetSelectHdl(LINK(this, SwFrmPage, RelHdl));

    aMirrorPagesCB.SetClickHdl(LINK(this, SwFrmPage, MirrorHdl));

    aLk = LINK(this, SwFrmPage, RelSizeClickHdl);
    aRelWidthCB.SetClickHdl( aLk );
    aRelHeightCB.SetClickHdl( aLk );

    aAutoWidthCB.SetClickHdl( LINK( this, SwFrmPage, AutoWidthClickHdl ) );
    aAutoHeightCB.SetClickHdl( LINK( this, SwFrmPage, AutoHeightClickHdl ) );
}

/*--------------------------------------------------------------------
    Beschreibung:	Dtor
 --------------------------------------------------------------------*/

SwFrmPage::~SwFrmPage()
{
}

SfxTabPage* SwFrmPage::Create(Window *pParent, const SfxItemSet &rSet)
{
    return new SwFrmPage( pParent, rSet );
}

namespace
{
    void MoveControl( Control& _rCtrl, long _nOffsetY )
    {
        Point aPt( _rCtrl.GetPosPixel() );
        aPt.Move( 0, _nOffsetY );
        _rCtrl.SetPosPixel( aPt );
    }
}

void SwFrmPage::EnableGraficMode( void )
{
    // #i39692# mustn't be called more than once
    if(!aRealSizeBT.IsVisible())
    {
        long nOffset1 = aRelWidthCB.GetPosPixel().Y() - aAutoWidthCB.GetPosPixel().Y();
        long nOffset2 = nOffset1 + aRelHeightCB.GetPosPixel().Y() - aAutoHeightCB.GetPosPixel().Y();

        MoveControl( aHeightFT, nOffset1 );
        MoveControl( aHeightED, nOffset1 );
        MoveControl( aRelHeightCB, nOffset1 );
        MoveControl( aFixedRatioCB, nOffset2 );

        aWidthFT.Show();
        aWidthAutoFT.Hide();
        aAutoHeightCB.Hide();

        aHeightFT.Show();
        aHeightAutoFT.Hide();
        aAutoWidthCB.Hide();

        aRealSizeBT.Show();
    }
}

void SwFrmPage::Reset( const SfxItemSet &rSet )
{

    SwWrtShell* pSh = bFormat ? ::GetActiveWrtShell() :
            ((SwFrmDlg*)GetParent()->GetParent())->GetWrtShell();

    nHtmlMode = ::GetHtmlMode(pSh->GetView().GetDocShell());
    bHtmlMode = nHtmlMode & HTMLMODE_ON ? TRUE : FALSE;

    FieldUnit aMetric = ::GetDfltMetric(bHtmlMode);
    SetMetric( aWidthED, aMetric );
    SetMetric( aHeightED, aMetric );
    SetMetric( aAtHorzPosED, aMetric );
    SetMetric( aAtVertPosED, aMetric );


    const SwFmtAnchor& rAnchor = (const SwFmtAnchor&)rSet.Get(RES_ANCHOR);

    if (bFormat)
    {
        // Bei Formaten keine Ankerbearbeitung
        aAnchorAtPageRB.Enable( FALSE );
        aAnchorAtParaRB.Enable( FALSE );
        aAnchorAtCharRB.Enable( FALSE );
        aAnchorAsCharRB.Enable( FALSE );
        aAnchorAtFrameRB.Enable( FALSE );
        aTypeFL.Enable( FALSE );
        aFixedRatioCB.Enable(FALSE);
    }
    else
    {
        if (rAnchor.GetAnchorId() != FLY_AT_FLY && !pSh->IsFlyInFly())
            aAnchorAtFrameRB.Hide();
        if(!bVerticalChanged && pSh->IsFrmVertical(TRUE, bIsInRightToLeft))
        {
            String sHLabel = aHorizontalFT.GetText();
            aHorizontalFT.SetText(aVerticalFT.GetText());
            aVerticalFT.SetText(sHLabel);
            bIsVerticalFrame = TRUE;
        }
    }

    if ( nDlgType == DLG_FRM_GRF || nDlgType == DLG_FRM_OLE )
    {
        ASSERT(pSh , "shell not found");
        const SfxPoolItem* pItem;
        //OS: nur fuer die Variante Einfuegen/Grafik/Eigenschaften
        if(SFX_ITEM_SET == rSet.GetItemState(FN_PARAM_GRF_REALSIZE, FALSE, &pItem))
            aGrfSize = ((const SvxSizeItem*)pItem)->GetSize();
        else
            pSh->GetGrfSize( aGrfSize );

        if ( !bNew )
        {
            aRealSizeBT.SetClickHdl(LINK(this, SwFrmPage, RealSizeHdl));
            EnableGraficMode();
        }
//		else
//          aTypeFL.SetSizePixel(Size(aTypeFL.GetSizePixel().Width(), aSizeFL.GetSizePixel().Height()));

        if ( nDlgType == DLG_FRM_GRF )
            aFixedRatioCB.Check( FALSE );
        else
        {
            if ( bNew )
                SetText( SW_RESSTR(STR_OLE_INSERT));
            else
                SetText( SW_RESSTR(STR_OLE_EDIT));
        }
    }
    else
    {
//      aTypeFL.SetSizePixel(Size(aTypeFL.GetSizePixel().Width(), aSizeFL.GetSizePixel().Height()));
        aGrfSize = ((const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE)).GetSize();
    }

    //Prozenteingabe ermoeglichen.
    aWidthED. SetBaseValue( aWidthED.Normalize(aGrfSize.Width()), FUNIT_TWIP );
    aHeightED.SetBaseValue( aHeightED.Normalize(aGrfSize.Height()), FUNIT_TWIP );
    //the available space is not yet known so the RefValue has to be calculated from size and relative size values
    //this is needed only if relative values are already set

    const SwFmtFrmSize& rFrmSize = (const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE);

    if (rFrmSize.GetWidthPercent() != 0xff && rFrmSize.GetWidthPercent() != 0)
    {
        //calculate the rerference value from the with and relative width values
        sal_Int32 nSpace = rFrmSize.GetWidth() * 100 / rFrmSize.GetWidthPercent();
        aWidthED. SetRefValue( nSpace );
    }

    if (rFrmSize.GetHeightPercent() != 0xff && rFrmSize.GetHeightPercent() != 0)
    {
        //calculate the rerference value from the with and relative width values
        sal_Int32 nSpace = rFrmSize.GetHeight() * 100 / rFrmSize.GetHeightPercent();
        aHeightED.SetRefValue( nSpace );
    }

    // Allgemeiner Initialisierungteil
    switch(rAnchor.GetAnchorId())
    {
        case FLY_PAGE: aAnchorAtPageRB.Check(); break;
        case FLY_AT_CNTNT: aAnchorAtParaRB.Check(); break;
        case FLY_AUTO_CNTNT: aAnchorAtCharRB.Check(); break;
        case FLY_IN_CNTNT: aAnchorAsCharRB.Check(); break;
        case FLY_AT_FLY: aAnchorAtFrameRB.Check();break;
        default:; //prevent warning
    }

    // OD 12.11.2003 #i22341# - determine content position of character
    // Note: content position can be NULL
    mpToCharCntntPos = rAnchor.GetCntntAnchor();

    // OD 19.09.2003 #i18732# - init checkbox value
    {
        const bool bFollowTextFlow =
            static_cast<const SwFmtFollowTextFlow&>(rSet.Get(RES_FOLLOW_TEXT_FLOW)).GetValue();
        aFollowTextFlowCB.Check( bFollowTextFlow );
    }

    if(bHtmlMode)
    {
        if(nDlgType == DLG_FRM_STD &&
                0 == (nHtmlMode & HTMLMODE_FULL_ABS_POS))
        {
            aHeightFT	.Enable( FALSE );
            aHeightED	.Enable( FALSE );
            aRelHeightCB.Enable( FALSE );
        }
        if( 0 == (nHtmlMode & HTMLMODE_SOME_ABS_POS))
        {
            if(GetAnchor() == FLY_PAGE)
            {
                aAnchorAtParaRB.Check();
            }
            aAnchorAtPageRB.Enable(FALSE);
        }
        aAutoHeightCB.Enable(FALSE);
        aAutoWidthCB.Enable(FALSE);
        aMirrorPagesCB.Show(FALSE);
        if(nDlgType == DLG_FRM_STD)
            aFixedRatioCB.Enable(FALSE);
        // OD 19.09.2003 #i18732# - hide checkbox in HTML mode
        aFollowTextFlowCB.Show(FALSE);
    }
    else
    {
        // OD 06.11.2003 #i18732# correct enable/disable of check box 'Mirror on..'
        aMirrorPagesCB.Enable(!aAnchorAsCharRB.IsChecked());

        // OD 06.11.2003 #i18732# - enable/disable check box 'Follow text flow'.
        // OD 10.11.2003 #i22305# - enable check box 'Follow text
        // flow' also for anchor type to-frame.
        aFollowTextFlowCB.Enable( aAnchorAtParaRB.IsChecked() ||
                                  aAnchorAtCharRB.IsChecked() ||
                                  aAnchorAtFrameRB.IsChecked() );
    }

    Init( rSet, TRUE );
    aAtVertPosED.SaveValue();
    aAtHorzPosED.SaveValue();
    // OD 19.09.2003 #i18732#
    aFollowTextFlowCB.SaveValue();

    bNoModifyHdl = FALSE;
    //lock PercentFields
    aWidthED.LockAutoCalculation(sal_True);
    aHeightED.LockAutoCalculation(sal_True);
    RangeModifyHdl(&aWidthED);  // Alle Maximalwerte initial setzen
    aHeightED.LockAutoCalculation(sal_False);
    aWidthED.LockAutoCalculation(sal_False);

    aAutoHeightCB.SaveValue();
    aAutoWidthCB.SaveValue();

    SwTwips nWidth  = static_cast< SwTwips >(aWidthED.DenormalizePercent(aWidthED.GetValue(FUNIT_TWIP)));
    SwTwips nHeight = static_cast< SwTwips >(aHeightED.DenormalizePercent(aHeightED.GetValue(FUNIT_TWIP)));
    fWidthHeightRatio = nHeight ? double(nWidth) / double(nHeight) : 1.0;
}


/*--------------------------------------------------------------------
    Beschreibung:	Attribute in den Set stopfen bei OK
 --------------------------------------------------------------------*/

BOOL SwFrmPage::FillItemSet(SfxItemSet &rSet)
{
    BOOL bRet = FALSE;
    SwWrtShell* pSh = bFormat ? ::GetActiveWrtShell()
                        : ((SwFrmDlg*)GetParent()->GetParent())->GetWrtShell();
    ASSERT( pSh , "shell not found");
    const SfxItemSet& rOldSet = GetItemSet();
    const SfxPoolItem* pOldItem = 0;

    RndStdIds eAnchorId = (RndStdIds)GetAnchor();

    if ( !bFormat )
    {
        pOldItem = GetOldItem(rSet, RES_ANCHOR);
        if (bNew || !pOldItem || eAnchorId != ((const SwFmtAnchor*)pOldItem)->GetAnchorId())
        {
            SwFmtAnchor aAnc( eAnchorId, pSh->GetPhyPageNum() );
            bRet = 0 != rSet.Put( aAnc );
        }
    }

    if ( pHMap )
    {
        SwFmtHoriOrient aHoriOrient( (const SwFmtHoriOrient&)
                                                rOldSet.Get(RES_HORI_ORIENT) );

        USHORT nMapPos = GetMapPos(pHMap, aHorizontalDLB);
        short nAlign = GetAlignment(pHMap, nMapPos, aHorizontalDLB, aHoriRelationLB);
        short nRel = GetRelation(pHMap, aHoriRelationLB);

        const sal_Int16 eHOri = nAlign;
        const sal_Int16 eRel  = nRel;

        aHoriOrient.SetHoriOrient( eHOri );
        aHoriOrient.SetRelationOrient( eRel );
        aHoriOrient.SetPosToggle(aMirrorPagesCB.IsChecked());

        BOOL bMod = aAtHorzPosED.GetText() != aAtHorzPosED.GetSavedValue();
        bMod |= aMirrorPagesCB.GetState() != aMirrorPagesCB.GetSavedValue();

        if ( eHOri == text::HoriOrientation::NONE &&
             (bNew || (bAtHorzPosModified || bMod) || nOldH != eHOri ) )
        {
            SwTwips nX = static_cast< SwTwips >(aAtHorzPosED.Denormalize(aAtHorzPosED.GetValue(FUNIT_TWIP)));
            aHoriOrient.SetPos( nX );
        }

        pOldItem = GetOldItem(rSet, FN_HORI_ORIENT);
        BOOL bSame = FALSE;
        if ((bNew == bFormat) && pOldItem)
        {
             bSame = static_cast< BOOL >(aHoriOrient == (SwFmtHoriOrient&)*pOldItem);
        }
        if ((bNew && !bFormat) || ((bAtHorzPosModified || bMod) && !bSame))
        {
            bRet |= 0 != rSet.Put( aHoriOrient );
        }
    }

    if ( pVMap )
    {
        // Ausrichtung Vertikal
        SwFmtVertOrient aVertOrient( (const SwFmtVertOrient&)
                                                rOldSet.Get(RES_VERT_ORIENT) );

        USHORT nMapPos = GetMapPos(pVMap, aVerticalDLB);
        short nAlign = GetAlignment(pVMap, nMapPos, aVerticalDLB, aVertRelationLB);
        short nRel = GetRelation(pVMap, aVertRelationLB);

        const sal_Int16     eVOri = nAlign;
        const sal_Int16     eRel  = nRel;

        aVertOrient.SetVertOrient    ( eVOri);
        aVertOrient.SetRelationOrient( eRel );

        BOOL bMod = aAtVertPosED.GetText() != aAtVertPosED.GetSavedValue();

        if ( eVOri == text::VertOrientation::NONE &&
             ( bNew || (bAtVertPosModified || bMod) || nOldV != eVOri) )
        {
            // Vertikale Position
            // fuer zeichengebundene Rahmen Offset umrechenen
            SwTwips nY = static_cast< SwTwips >(aAtVertPosED.Denormalize(aAtVertPosED.GetValue(FUNIT_TWIP)));
            if (eAnchorId == FLY_IN_CNTNT)
                nY *= -1;
            aVertOrient.SetPos( nY );
        }
        pOldItem = GetOldItem(rSet, FN_VERT_ORIENT);
        BOOL bSame = FALSE;
        if((bNew == bFormat) && pOldItem)
        {
             bSame = static_cast< BOOL >( bFormat ?
                aVertOrient.GetVertOrient() == ((SwFmtVertOrient*)pOldItem)->GetVertOrient() &&
                aVertOrient.GetRelationOrient() == ((SwFmtVertOrient*)pOldItem)->GetRelationOrient() &&
                aVertOrient.GetPos() == ((SwFmtVertOrient*)pOldItem)->GetPos()

                    : aVertOrient == (SwFmtVertOrient&)*pOldItem );
        }
        if( ( bNew && !bFormat ) || ((bAtVertPosModified || bMod) && !bSame ))
        {
            bRet |= 0 != rSet.Put( aVertOrient );
        }
    }

    //Size setzen
    //Neuer Sonderfall: Ist die Groesse von pMgr(0, 0), so werden die
    //Eigenschaften fuer eine Grafik festgelegt, die noch gar nicht geladen
    //ist. Dann wird hier kein SetSize() gemacht, wenn die
    //Groesseneinstellungen vom Anwender nicht veraendert wurden.
    const SwFmtFrmSize& rOldSize = (const SwFmtFrmSize& )rOldSet.Get(RES_FRM_SIZE);
    SwFmtFrmSize aSz( rOldSize );

/*	// Folgende Zeilen nur zum debuggen:
    BOOL bModified = aWidthED.IsValueModified();
    USHORT nOldWidth = rOldSize.GetWidth ();
    USHORT nWidth = aWidthED .GetValue();
    USHORT nMinWidth = aWidthED .GetMin();*/

    BOOL bValueModified = (aWidthED.IsValueModified() || aHeightED.IsValueModified());
    BOOL bCheckChanged = (aRelWidthCB.GetSavedValue() != aRelWidthCB.IsChecked()
                        || aRelHeightCB.GetSavedValue() != aRelHeightCB.IsChecked());

    BOOL bLegalValue = !(!rOldSize.GetWidth () && !rOldSize.GetHeight() &&
                            aWidthED .GetValue() == aWidthED .GetMin() &&
                            aHeightED.GetValue() == aHeightED.GetMin());

    if ((bNew && !bFormat) || (bValueModified || bCheckChanged) && bLegalValue)
    {
        sal_Int64 nNewWidth  = aWidthED.DenormalizePercent(aWidthED.GetRealValue(FUNIT_TWIP));
        sal_Int64 nNewHeight = aHeightED.DenormalizePercent(aHeightED.GetRealValue(FUNIT_TWIP));
        aSz.SetWidth (static_cast< SwTwips >(nNewWidth));
        aSz.SetHeight(static_cast< SwTwips >(nNewHeight));

        if (aRelWidthCB.IsChecked())
        {
            aSz.SetWidthPercent((BYTE)Min( static_cast< sal_Int64 >(MAX_PERCENT_WIDTH), aWidthED.Convert(aWidthED.NormalizePercent(nNewWidth), FUNIT_TWIP, FUNIT_CUSTOM)));
        }
        else
            aSz.SetWidthPercent(0);
        if (aRelHeightCB.IsChecked())
            aSz.SetHeightPercent((BYTE)Min(static_cast< sal_Int64 >(MAX_PERCENT_HEIGHT), aHeightED.Convert(aHeightED.NormalizePercent(nNewHeight), FUNIT_TWIP, FUNIT_CUSTOM)));
        else
            aSz.SetHeightPercent(0);

        if (aFixedRatioCB.IsChecked() && (aRelWidthCB.IsChecked() ^ aRelHeightCB.IsChecked()))
        {
            if (aRelWidthCB.IsChecked())
                aSz.SetHeightPercent(0xff);
            else
                aSz.SetWidthPercent(0xff);
        }
    }
    if( !IsInGraficMode() )
    {
        if( aAutoHeightCB.GetState() != aAutoHeightCB.GetSavedValue() )
        {
            SwFrmSize eFrmSize = (SwFrmSize) aAutoHeightCB.IsChecked()? ATT_MIN_SIZE : ATT_FIX_SIZE;
            if( eFrmSize != aSz.GetHeightSizeType() )
                aSz.SetHeightSizeType(eFrmSize);
        }
        if( aAutoWidthCB.GetState() != aAutoWidthCB.GetSavedValue() )
        {
            SwFrmSize eFrmSize = (SwFrmSize) aAutoWidthCB.IsChecked()? ATT_MIN_SIZE : ATT_FIX_SIZE;
            if( eFrmSize != aSz.GetWidthSizeType() )
                aSz.SetWidthSizeType( eFrmSize );
        }
    }
    if( !bFormat && aFixedRatioCB.GetSavedValue() != aFixedRatioCB.IsChecked())
        bRet |= 0 != rSet.Put(SfxBoolItem(FN_KEEP_ASPECT_RATIO, aFixedRatioCB.IsChecked()));

    pOldItem = GetOldItem(rSet, RES_FRM_SIZE);

    if ((pOldItem && aSz != *pOldItem) || (!pOldItem && !bFormat) ||
            (bFormat &&
                (aSz.GetWidth() > 0 || aSz.GetWidthPercent() > 0) &&
                    (aSz.GetHeight() > 0 || aSz.GetHeightPercent() > 0)))
    {
        if (aSz.GetHeightSizeType() == ATT_VAR_SIZE)    // VAR_SIZE gibts nicht bei Rahmen
            aSz.SetHeightSizeType(ATT_MIN_SIZE);        // Bug #45776 (Vorlagen ohne Breite/Hoehe)

        bRet |= 0 != rSet.Put( aSz );
    }
    // OD 19.09.2003 #i18732#
    if(aFollowTextFlowCB.IsChecked() != aFollowTextFlowCB.GetSavedValue())
    {
        bRet |= 0 != rSet.Put(SwFmtFollowTextFlow(aFollowTextFlowCB.IsChecked()));
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:	Horizontale und vertikale Pos initialisieren
 --------------------------------------------------------------------*/

void SwFrmPage::InitPos(RndStdIds eId,
                                USHORT nH,
                                USHORT nHRel,
                                USHORT nV,
                                USHORT nVRel,
                                long   nX,
                                long   nY)
{
    USHORT nPos = aVerticalDLB.GetSelectEntryPos();
    if ( nPos != LISTBOX_ENTRY_NOTFOUND && pVMap )
    {
        nOldV	 = pVMap[nPos].nAlign;

        nPos = aVertRelationLB.GetSelectEntryPos();
        if (nPos != LISTBOX_ENTRY_NOTFOUND)
            nOldVRel = ((RelationMap *)aVertRelationLB.GetEntryData(nPos))->nRelation;
    }

    nPos = aHorizontalDLB.GetSelectEntryPos();
    if ( nPos != LISTBOX_ENTRY_NOTFOUND && pHMap )
    {
        nOldH	 = pHMap[nPos].nAlign;

        nPos = aHoriRelationLB.GetSelectEntryPos();
        if (nPos != LISTBOX_ENTRY_NOTFOUND)
            nOldHRel = ((RelationMap *)aHoriRelationLB.GetEntryData(nPos))->nRelation;
    }

    BOOL bEnable = TRUE;
    if ( eId == FLY_PAGE )
    {
        pVMap = bHtmlMode ? aVPageHtmlMap : aVPageMap;
        pHMap = bHtmlMode ? aHPageHtmlMap : aHPageMap;
    }
    else if ( eId == FLY_AT_FLY )
    {
        // OD 19.09.2003 #i18732# - own vertical alignment map for to frame
        // anchored objects.
        pVMap = bHtmlMode ? aVFlyHtmlMap : aVFrameMap;
        pHMap = bHtmlMode ? aHFlyHtmlMap : aHFrameMap;
    }
    else if ( eId == FLY_AT_CNTNT )
    {
        if(bHtmlMode)
        {
            pVMap = aVParaHtmlMap;
            pHMap = nHtmlMode & HTMLMODE_SOME_ABS_POS ? aHParaHtmlAbsMap : aHParaHtmlMap;
        }
        else
        {
            pVMap = aVParaMap;
            pHMap = aHParaMap;
        }
    }
    else if ( eId == FLY_AUTO_CNTNT )
    {
        if(bHtmlMode)
        {
            pVMap = nHtmlMode & HTMLMODE_SOME_ABS_POS ? aVCharHtmlAbsMap	: aVCharHtmlMap;
            pHMap = nHtmlMode & HTMLMODE_SOME_ABS_POS ? aHCharHtmlAbsMap	: aHCharHtmlMap;
        }
        else
        {
            pVMap = aVCharMap;
            pHMap = aHCharMap;
        }
    }
    else if ( eId == FLY_IN_CNTNT )
    {
        pVMap = bHtmlMode ? aVAsCharHtmlMap 	: aVAsCharMap;
        pHMap = 0;
        bEnable = FALSE;
    }
    aHorizontalDLB.Enable( bEnable );
    aHorizontalFT.Enable( bEnable );

    // aktuelle Pos selektieren
    // Horizontal
    if ( nH == USHRT_MAX )
    {
        nH 	  = nOldH;
        nHRel = nOldHRel;
    }
    // OD 12.11.2003 #i22341# - pass <nHRel> as 3rd parameter to method <FillPosLB>
    USHORT nMapPos = FillPosLB(pHMap, nH, nHRel, aHorizontalDLB);
    FillRelLB(pHMap, nMapPos, nH, nHRel, aHoriRelationLB, aHoriRelationFT);

    // Vertikal
    if ( nV == USHRT_MAX )
    {
        nV	  = nOldV;
        nVRel = nOldVRel;
    }
    // OD 12.11.2003 #i22341# - pass <nVRel> as 3rd parameter to method <FillPosLB>
    nMapPos = FillPosLB(pVMap, nV, nVRel, aVerticalDLB);
    FillRelLB(pVMap, nMapPos, nV, nVRel, aVertRelationLB, aVertRelationFT);

    // Edits init
    bEnable = nH == text::HoriOrientation::NONE && eId != FLY_IN_CNTNT;//#61359# warum nicht in Formaten&& !bFormat;
    if (!bEnable)
    {
        aAtHorzPosED.SetValue( 0, FUNIT_TWIP );
        if (nX != LONG_MAX && bHtmlMode)
            aAtHorzPosED.SetModifyFlag();
    }
    else
    {
        if (nX != LONG_MAX)
            aAtHorzPosED.SetValue( aAtHorzPosED.Normalize(nX), FUNIT_TWIP );
    }
    aAtHorzPosFT.Enable( bEnable );
    aAtHorzPosED.Enable( bEnable );

    bEnable = nV == text::VertOrientation::NONE && !bFormat;
    if ( !bEnable )
    {
        aAtVertPosED.SetValue( 0, FUNIT_TWIP );
        if(nY != LONG_MAX && bHtmlMode)
            aAtVertPosED.SetModifyFlag();
    }
    else
    {
        if ( eId == FLY_IN_CNTNT )
        {
            if ( nY == LONG_MAX )
                nY = 0;
            else
                nY *= -1;
        }
        if ( nY != LONG_MAX )
            aAtVertPosED.SetValue( aAtVertPosED.Normalize(nY), FUNIT_TWIP );
    }
    aAtVertPosFT.Enable( bEnable );
    aAtVertPosED.Enable( bEnable );
    UpdateExample();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

USHORT SwFrmPage::FillPosLB(const FrmMap* _pMap,
                            const USHORT _nAlign,
                            const USHORT _nRel,
                            ListBox& _rLB )
{
    String sSelEntry, sOldEntry;
    sOldEntry = _rLB.GetSelectEntry();

    _rLB.Clear();

    // OD 12.11.2003 #i22341# - determine all possible listbox relations for
    // given relation for map <aVCharMap>
    const ULONG nLBRelations = (_pMap != aVCharMap)
                               ? 0L
                               : ::lcl_GetLBRelationsForRelations( _nRel );

    // Listbox fuellen
    size_t nCount = ::lcl_GetFrmMapCount(_pMap);
    for (size_t i = 0; _pMap && i < nCount; ++i)
    {
//		#61359# Warum nicht von links/von innen bzw. von oben?
//      if (!bFormat || (pMap[i].eStrId != SwFPos::FROMLEFT && pMap[i].eStrId != SwFPos::FROMTOP))
        {
            SvxSwFramePosString::StringId eStrId = aMirrorPagesCB.IsChecked() ? _pMap[i].eMirrorStrId : _pMap[i].eStrId;
            eStrId = lcl_ChangeResIdToVerticalOrRTL(eStrId, bIsVerticalFrame, bIsInRightToLeft);
            String sEntry(aFramePosString.GetString(eStrId));
            if (_rLB.GetEntryPos(sEntry) == LISTBOX_ENTRY_NOTFOUND)
            {
                // bei zeichengebundenen Rahmen keine doppelten Eintraege einfuegen
                _rLB.InsertEntry(sEntry);
            }
            // OD 12.11.2003 #i22341# - add condition to handle map <aVCharMap>
            // that is ambigous in the alignment.
            if ( _pMap[i].nAlign == _nAlign &&
                 ( !(_pMap == aVCharMap) || _pMap[i].nLBRelations & nLBRelations ) )
            {
                sSelEntry = sEntry;
            }
        }
    }

    _rLB.SelectEntry(sSelEntry);
    if (!_rLB.GetSelectEntryCount())
        _rLB.SelectEntry(sOldEntry);

    if (!_rLB.GetSelectEntryCount())
        _rLB.SelectEntryPos(0);

    PosHdl(&_rLB);

    return GetMapPos(_pMap, _rLB);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
ULONG SwFrmPage::FillRelLB( const FrmMap* _pMap,
                            const USHORT _nLBSelPos,
                            const USHORT _nAlign,
                            USHORT _nRel,
                            ListBox& _rLB,
                            FixedText& _rFT )
{
    String sSelEntry;
    ULONG  nLBRelations = 0;
    size_t nMapCount = ::lcl_GetFrmMapCount(_pMap);

    _rLB.Clear();

    if (_nLBSelPos < nMapCount)
    {
        if (_pMap == aVAsCharHtmlMap || _pMap == aVAsCharMap)
        {
            String sOldEntry(_rLB.GetSelectEntry());
            USHORT nRelCount = sizeof(aAsCharRelationMap) / sizeof(RelationMap);
            SvxSwFramePosString::StringId eStrId = _pMap[_nLBSelPos].eStrId;

            for (size_t nMapPos = 0; nMapPos < nMapCount; nMapPos++)
            {
                if (_pMap[nMapPos].eStrId == eStrId)
                {
                    nLBRelations = _pMap[nMapPos].nLBRelations;
                    for (USHORT nRelPos = 0; nRelPos < nRelCount; nRelPos++)
                    {
                        if (nLBRelations & aAsCharRelationMap[nRelPos].nLBRelation)
                        {
                            SvxSwFramePosString::StringId sStrId1 = aAsCharRelationMap[nRelPos].eStrId;

                            sStrId1 = lcl_ChangeResIdToVerticalOrRTL(sStrId1, bIsVerticalFrame, bIsInRightToLeft);
                            String sEntry = aFramePosString.GetString(sStrId1);
                            USHORT nPos = _rLB.InsertEntry(sEntry);
                            _rLB.SetEntryData(nPos, &aAsCharRelationMap[nRelPos]);
                            if (_pMap[nMapPos].nAlign == _nAlign)
                                sSelEntry = sEntry;
                            break;
                        }
                    }
                }
            }
            if (sSelEntry.Len())
                _rLB.SelectEntry(sSelEntry);
            else
            {
                _rLB.SelectEntry(sOldEntry);

                if (!_rLB.GetSelectEntryCount())
                {
                    for (USHORT i = 0; i < _rLB.GetEntryCount(); i++)
                    {
                        RelationMap *pEntry = (RelationMap *)_rLB.GetEntryData(i);
                        if (pEntry->nLBRelation == LB_REL_CHAR)	// Default
                        {
                            _rLB.SelectEntryPos(i);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            USHORT nRelCount = sizeof(aRelationMap) / sizeof(RelationMap);

            // OD 14.11.2003 #i22341# - special handling for map <aVCharMap>,
            // because its ambigous in its <eStrId>/<eMirrorStrId>.
            if ( _pMap == aVCharMap )
            {
                nLBRelations = ::lcl_GetLBRelationsForStrID( _pMap,
                                             ( aMirrorPagesCB.IsChecked()
                                               ? _pMap[_nLBSelPos].eMirrorStrId
                                               : _pMap[_nLBSelPos].eStrId),
                                             aMirrorPagesCB.IsChecked() );
            }
            else
            {
                nLBRelations = _pMap[_nLBSelPos].nLBRelations;
            }

            for (ULONG nBit = 1; nBit < 0x80000000; nBit <<= 1)
            {
                if (nLBRelations & nBit)
                {
                    for (USHORT nRelPos = 0; nRelPos < nRelCount; nRelPos++)
                    {
                        if (aRelationMap[nRelPos].nLBRelation == nBit)
                        {
                            SvxSwFramePosString::StringId eStrId1 = aMirrorPagesCB.IsChecked() ?
                                            aRelationMap[nRelPos].eMirrorStrId : aRelationMap[nRelPos].eStrId;
                            eStrId1 = lcl_ChangeResIdToVerticalOrRTL(eStrId1, bIsVerticalFrame, bIsInRightToLeft);
                            String sEntry = aFramePosString.GetString(eStrId1);
                            USHORT nPos = _rLB.InsertEntry(sEntry);
                            _rLB.SetEntryData(nPos, &aRelationMap[nRelPos]);
                            if (!sSelEntry.Len() && aRelationMap[nRelPos].nRelation == _nRel)
                                sSelEntry = sEntry;
                        }
                    }
                }
            }
            if (sSelEntry.Len())
                _rLB.SelectEntry(sSelEntry);
            else
            {
                // Warscheinlich Ankerwechsel. Daher aehnliche Relation suchen
                switch (_nRel)
                {
                    case text::RelOrientation::FRAME:           _nRel = text::RelOrientation::PAGE_FRAME;   break;
                    case text::RelOrientation::PRINT_AREA:      _nRel = text::RelOrientation::PAGE_PRINT_AREA; break;
                    case text::RelOrientation::PAGE_LEFT:       _nRel = text::RelOrientation::FRAME_LEFT;   break;
                    case text::RelOrientation::PAGE_RIGHT:      _nRel = text::RelOrientation::FRAME_RIGHT;  break;
                    case text::RelOrientation::FRAME_LEFT:      _nRel = text::RelOrientation::PAGE_LEFT;    break;
                    case text::RelOrientation::FRAME_RIGHT:     _nRel = text::RelOrientation::PAGE_RIGHT;   break;
                    case text::RelOrientation::PAGE_FRAME:      _nRel = text::RelOrientation::FRAME;          break;
                    case text::RelOrientation::PAGE_PRINT_AREA: _nRel = text::RelOrientation::PRINT_AREA;        break;

                    default:
                        if (_rLB.GetEntryCount())
                        {
                            RelationMap *pEntry = (RelationMap *)_rLB.GetEntryData(_rLB.GetEntryCount() - 1);
                            _nRel = pEntry->nRelation;
                        }
                        break;
                }

                for (USHORT i = 0; i < _rLB.GetEntryCount(); i++)
                {
                    RelationMap *pEntry = (RelationMap *)_rLB.GetEntryData(i);
                    if (pEntry->nRelation == _nRel)
                    {
                        _rLB.SelectEntryPos(i);
                        break;
                    }
                }

                if (!_rLB.GetSelectEntryCount())
                    _rLB.SelectEntryPos(0);
            }
        }
    }

    _rLB.Enable(_rLB.GetEntryCount() != 0);
    _rFT.Enable(_rLB.GetEntryCount() != 0);

    RelHdl(&_rLB);

    return nLBRelations;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

short SwFrmPage::GetRelation(FrmMap * /*pMap*/, ListBox &rRelationLB)
{
    short nRel = 0;
    USHORT nPos = rRelationLB.GetSelectEntryPos();

    if (nPos != LISTBOX_ENTRY_NOTFOUND)
    {
        RelationMap *pEntry = (RelationMap *)rRelationLB.GetEntryData(nPos);
        nRel = pEntry->nRelation;
    }

    return nRel;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

short SwFrmPage::GetAlignment(FrmMap *pMap, USHORT nMapPos,
        ListBox &/*rAlignLB*/, ListBox &rRelationLB)
{
    short nAlign = 0;

    // OD 14.11.2003 #i22341# - special handling also for map <aVCharMap>,
    // because it contains ambigous items for alignment
    if ( pMap == aVAsCharHtmlMap || pMap == aVAsCharMap ||
         pMap == aVCharMap )
    {
        if (rRelationLB.GetSelectEntryPos() != LISTBOX_ENTRY_NOTFOUND)
        {
            ULONG  nRel = ((RelationMap *)rRelationLB.GetEntryData(rRelationLB.GetSelectEntryPos()))->nLBRelation;
            size_t nMapCount = ::lcl_GetFrmMapCount(pMap);
            SvxSwFramePosString::StringId eStrId = pMap[nMapPos].eStrId;

            for (size_t i = 0; i < nMapCount; i++)
            {
                if (pMap[i].eStrId == eStrId)
                {
                    ULONG nLBRelations = pMap[i].nLBRelations;
                    if (nLBRelations & nRel)
                    {
                        nAlign = pMap[i].nAlign;
                        break;
                    }
                }
            }
        }
    }
    else if (pMap)
        nAlign = pMap[nMapPos].nAlign;

    return nAlign;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

USHORT SwFrmPage::GetMapPos( const FrmMap *pMap, ListBox &rAlignLB )
{
    USHORT nMapPos = 0;
    USHORT nLBSelPos = rAlignLB.GetSelectEntryPos();

    if (nLBSelPos != LISTBOX_ENTRY_NOTFOUND)
    {
        if (pMap == aVAsCharHtmlMap || pMap == aVAsCharMap)
        {
            size_t nMapCount = ::lcl_GetFrmMapCount(pMap);
            String sSelEntry(rAlignLB.GetSelectEntry());

            for (size_t i = 0; i < nMapCount; i++)
            {
                SvxSwFramePosString::StringId eResId = pMap[i].eStrId;

                String sEntry = aFramePosString.GetString(eResId);
                sEntry = MnemonicGenerator::EraseAllMnemonicChars( sEntry );

                if (sEntry == sSelEntry)
                {
                    nMapPos = static_cast< USHORT >(i);
                    break;
                }
            }
        }
        else
            nMapPos = nLBSelPos;
    }

    return nMapPos;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

RndStdIds SwFrmPage::GetAnchor()
{
    RndStdIds nRet = FLY_PAGE;
    if(aAnchorAtParaRB.IsChecked())
        nRet = FLY_AT_CNTNT;
    else if(aAnchorAtCharRB.IsChecked())
        nRet = FLY_AUTO_CNTNT;
    else if(aAnchorAsCharRB.IsChecked())
        nRet = FLY_IN_CNTNT;
    else if(aAnchorAtFrameRB.IsChecked())
        nRet = FLY_AT_FLY;
    return nRet;
}

/*--------------------------------------------------------------------
    Beschreibung:	Bsp - Update
 --------------------------------------------------------------------*/

void SwFrmPage::ActivatePage(const SfxItemSet& rSet)
{
    bNoModifyHdl = TRUE;
    Init(rSet);
    bNoModifyHdl = FALSE;
    //lock PercentFields
    aWidthED.LockAutoCalculation(sal_True);
    aHeightED.LockAutoCalculation(sal_True);
    RangeModifyHdl(&aWidthED);  // Alle Maximalwerte initial setzen
    aHeightED.LockAutoCalculation(sal_False);
    aWidthED.LockAutoCalculation(sal_False);
    aFollowTextFlowCB.SaveValue();
}

int SwFrmPage::DeactivatePage(SfxItemSet * _pSet)
{
    if ( _pSet )
    {
        FillItemSet( *_pSet );

        //FillItemSet setzt den Anker nicht in den Set, wenn er mit dem
        //Original uebereinstimmt. Fuer die anderen Pages brauchen wir aber
        //den aktuellen Anker.
        SwWrtShell* pSh = bFormat ? ::GetActiveWrtShell()
                            : ((SwFrmDlg*)GetParent()->GetParent())->GetWrtShell();
        RndStdIds eAnchorId = (RndStdIds)GetAnchor();
        SwFmtAnchor aAnc( eAnchorId, pSh->GetPhyPageNum() );
        _pSet->Put( aAnc );
    }

    return TRUE;
}

/*--------------------------------------------------------------------
    Beschreibung: Links/rechts mit Innen/aussen tauschen
 --------------------------------------------------------------------*/

IMPL_LINK( SwFrmPage, MirrorHdl, CheckBox *, EMPTYARG )
{
    RndStdIds eId = GetAnchor();
    InitPos( eId, USHRT_MAX, 0, USHRT_MAX, 0, LONG_MAX, LONG_MAX);

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwFrmPage, RelSizeClickHdl, CheckBox *, pBtn )
{
    if (pBtn == &aRelWidthCB)
    {
        aWidthED.ShowPercent(pBtn->IsChecked());
        if(pBtn->IsChecked())
            aWidthED.MetricField::SetMax(MAX_PERCENT_WIDTH);
    }
    else // pBtn == &aRelHeightCB
    {
        aHeightED.ShowPercent(pBtn->IsChecked());
        if(pBtn->IsChecked())
            aHeightED.MetricField::SetMax(MAX_PERCENT_HEIGHT);
    }

    if (pBtn)	// Nur wenn Handler durch Aenderung des Controllers gerufen wurde
        RangeModifyHdl(&aWidthED);	// Werte wieder korrigieren

    if (pBtn == &aRelWidthCB)
        ModifyHdl(&aWidthED);
    else // pBtn == &aRelHeightCB
        ModifyHdl(&aHeightED);

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:	Bereichspruefung
 --------------------------------------------------------------------*/

IMPL_LINK( SwFrmPage, RangeModifyHdl, Edit *, EMPTYARG )
{
    if (bNoModifyHdl)
        return 0;

    SwWrtShell* pSh = bFormat ? ::GetActiveWrtShell()
                        :((SwFrmDlg*)GetParent()->GetParent())->GetWrtShell();
    ASSERT(pSh , "shell not found");
    SwFlyFrmAttrMgr aMgr( bNew, pSh, (const SwAttrSet&)GetItemSet() );
    SvxSwFrameValidation        aVal;

    aVal.nAnchorType = static_cast< USHORT >(GetAnchor());
    aVal.bAutoHeight = aAutoHeightCB.IsChecked();
    aVal.bAutoWidth = aAutoWidthCB.IsChecked();
    aVal.bMirror = aMirrorPagesCB.IsChecked();
    // OD 18.09.2003 #i18732#
    aVal.bFollowTextFlow = aFollowTextFlowCB.IsChecked();

    if ( pHMap )
    {
        // Ausrichtung Horizontal
        USHORT nMapPos = GetMapPos(pHMap, aHorizontalDLB);
        short nAlign = GetAlignment(pHMap, nMapPos, aHorizontalDLB, aHoriRelationLB);
        short nRel = GetRelation(pHMap, aHoriRelationLB);

        aVal.nHoriOrient = nAlign;
        aVal.nHRelOrient = nRel;
    }
    else
        aVal.nHoriOrient = text::HoriOrientation::NONE;

    if ( pVMap )
    {
        // Ausrichtung Vertikal
        USHORT nMapPos = GetMapPos(pVMap, aVerticalDLB);
        short nAlign = GetAlignment(pVMap, nMapPos, aVerticalDLB, aVertRelationLB);
        short nRel = GetRelation(pVMap, aVertRelationLB);

        aVal.nVertOrient = nAlign;
        aVal.nVRelOrient = nRel;
    }
    else
        aVal.nVertOrient = text::VertOrientation::NONE;

    const long nAtHorzPosVal = static_cast< long >(
                    aAtHorzPosED.Denormalize(aAtHorzPosED.GetValue(FUNIT_TWIP)) );
    const long nAtVertPosVal = static_cast< long >(
                    aAtVertPosED.Denormalize(aAtVertPosED.GetValue(FUNIT_TWIP)) );

    aVal.nHPos = nAtHorzPosVal;
    aVal.nVPos = nAtVertPosVal;

    aMgr.ValidateMetrics(aVal, mpToCharCntntPos, TRUE);   // Einmal, um Referenzwerte fuer prozentuale Werte zu erhalten

    // Referenzwerte fuer fuer Prozentwerte setzen (100%) ...
    aWidthED.SetRefValue(aVal.aPercentSize.Width());
    aHeightED.SetRefValue(aVal.aPercentSize.Height());

    // ... und damit Breite und Hoehe korrekt umrechnen
    SwTwips nWidth  = static_cast< SwTwips >(aWidthED. DenormalizePercent(aWidthED.GetValue(FUNIT_TWIP)));
    SwTwips nHeight = static_cast< SwTwips >(aHeightED.DenormalizePercent(aHeightED.GetValue(FUNIT_TWIP)));
    aVal.nWidth  = nWidth;
    aVal.nHeight = nHeight;

    aMgr.ValidateMetrics(aVal, mpToCharCntntPos);    // Nochmal um mit korrekter Breite und Hoehe alle restlichen Werte zu ermitteln

    // alle Spalten muessen passen
    if(GetTabDialog()->GetExampleSet() &&
            SFX_ITEM_DEFAULT <= GetTabDialog()->GetExampleSet()->GetItemState(RES_COL))
    {
        const SwFmtCol& rCol = (const SwFmtCol&)GetTabDialog()->GetExampleSet()->Get(RES_COL);
        if ( rCol.GetColumns().Count() > 1 )
        {
            for ( USHORT i = 0; i < rCol.GetColumns().Count(); ++i )
            {
                aVal.nMinWidth += rCol.GetColumns()[i]->GetLeft() +
                                  rCol.GetColumns()[i]->GetRight() +
                                  MINFLY;
            }
            aVal.nMinWidth -= MINFLY;//einen hatten wir schon mit drin!
        }
    }

    nWidth = aVal.nWidth;
    nHeight = aVal.nHeight;

    // Mindestbreite auch fuer Vorlage
    aHeightED.SetMin(aHeightED.NormalizePercent(aVal.nMinHeight), FUNIT_TWIP);
    aWidthED. SetMin(aWidthED.NormalizePercent(aVal.nMinWidth), FUNIT_TWIP);

    SwTwips nMaxWidth(aVal.nMaxWidth);
    SwTwips nMaxHeight(aVal.nMaxHeight);

    if (aVal.bAutoHeight && (nDlgType == DLG_FRM_GRF || nDlgType == DLG_FRM_OLE))
    {
        SwTwips nTmp = Min(nWidth * nMaxHeight / Max(nHeight, 1L), nMaxHeight);
        aWidthED.SetMax(aWidthED.NormalizePercent(nTmp), FUNIT_TWIP);

        nTmp = Min(nHeight * nMaxWidth / Max(nWidth, 1L), nMaxWidth);
        aHeightED.SetMax(aWidthED.NormalizePercent(nTmp), FUNIT_TWIP);
    }
    else
    {
        SwTwips nTmp = static_cast< SwTwips >(aHeightED.NormalizePercent(nMaxHeight));
        aHeightED.SetMax(nTmp, FUNIT_TWIP);

        nTmp = static_cast< SwTwips >(aWidthED.NormalizePercent(nMaxWidth));
        aWidthED.SetMax(nTmp, FUNIT_TWIP);
    }

    aAtHorzPosED.SetMin(aAtHorzPosED.Normalize(aVal.nMinHPos), FUNIT_TWIP);
    aAtHorzPosED.SetMax(aAtHorzPosED.Normalize(aVal.nMaxHPos), FUNIT_TWIP);
    if ( aVal.nHPos != nAtHorzPosVal )
        aAtHorzPosED.SetValue(aAtHorzPosED.Normalize(aVal.nHPos), FUNIT_TWIP);

    SwTwips nUpperOffset = aVal.nAnchorType == FLY_IN_CNTNT ? nUpperBorder : 0;
    SwTwips nLowerOffset = aVal.nAnchorType == FLY_IN_CNTNT ? nLowerBorder : 0;

    aAtVertPosED.SetMin(aAtVertPosED.Normalize(aVal.nMinVPos + nLowerOffset + nUpperOffset), FUNIT_TWIP);
    aAtVertPosED.SetMax(aAtVertPosED.Normalize(aVal.nMaxVPos), FUNIT_TWIP);
    if ( aVal.nVPos != nAtVertPosVal )
        aAtVertPosED.SetValue(aAtVertPosED.Normalize(aVal.nVPos), FUNIT_TWIP);

    return 0;
}

IMPL_LINK( SwFrmPage, AnchorTypeHdl, RadioButton *, EMPTYARG )
{
    aMirrorPagesCB.Enable(!aAnchorAsCharRB.IsChecked());

    // OD 06.11.2003 #i18732# - enable check box 'Follow text flow' for anchor
    // type to-paragraph' and to-character
    // OD 10.11.2003 #i22305# - enable check box 'Follow text
    // flow' also for anchor type to-frame.
    aFollowTextFlowCB.Enable( aAnchorAtParaRB.IsChecked() ||
                              aAnchorAtCharRB.IsChecked() ||
                              aAnchorAtFrameRB.IsChecked() );

    RndStdIds eId = GetAnchor();

    InitPos( eId, USHRT_MAX, 0, USHRT_MAX, 0, LONG_MAX, LONG_MAX);
    RangeModifyHdl(0);

    if(bHtmlMode)
    {
        PosHdl(&aHorizontalDLB);
        PosHdl(&aVerticalDLB);
    }
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwFrmPage, PosHdl, ListBox *, pLB )
{
    BOOL bHori = pLB == &aHorizontalDLB;
    ListBox *pRelLB = bHori ? &aHoriRelationLB : &aVertRelationLB;
    FixedText *pRelFT = bHori ? &aHoriRelationFT : &aVertRelationFT;
    FrmMap *pMap = bHori ? pHMap : pVMap;

    USHORT nMapPos = GetMapPos(pMap, *pLB);
    short nAlign = GetAlignment(pMap, nMapPos, *pLB, *pRelLB);

    if (bHori)
    {
        BOOL bEnable = text::HoriOrientation::NONE == nAlign;
        aAtHorzPosED.Enable( bEnable );
        aAtHorzPosFT.Enable( bEnable );
    }
    else
    {
        BOOL bEnable = text::VertOrientation::NONE == nAlign;
        aAtVertPosED.Enable( bEnable );
        aAtVertPosFT.Enable( bEnable );
    }

    if (pLB)	// Nur wenn Handler durch Aenderung des Controllers gerufen wurde
        RangeModifyHdl( 0 );

    USHORT nRel = 0;
    if (pLB->GetSelectEntryCount())
    {

        if (pRelLB->GetSelectEntryPos() != LISTBOX_ENTRY_NOTFOUND)
            nRel = ((RelationMap *)pRelLB->GetEntryData(pRelLB->GetSelectEntryPos()))->nRelation;

        FillRelLB(pMap, nMapPos, nAlign, nRel, *pRelLB, *pRelFT);
    }
    else
        pRelLB->Clear();

    UpdateExample();

    if (bHori)
        bAtHorzPosModified = TRUE;
    else
        bAtVertPosModified = TRUE;

    // Sonderbehandlung fuer HTML-Mode mit horz-vert-Abhaengigkeiten
    if(bHtmlMode && nHtmlMode&HTMLMODE_SOME_ABS_POS &&
            FLY_AUTO_CNTNT == (RndStdIds)GetAnchor())
    {
        BOOL bSet = FALSE;
        if(bHori)
        {
            // rechts ist nur unterhalb erlaubt - von links nur oben
            // von links am Zeichen -> unterhalb
            if((text::HoriOrientation::LEFT == nAlign || text::HoriOrientation::RIGHT == nAlign) &&
                    0 == aVerticalDLB.GetSelectEntryPos())
            {
                if(text::RelOrientation::FRAME == nRel)
                    aVerticalDLB.SelectEntryPos(1);
                else
                    aVerticalDLB.SelectEntryPos(0);
                bSet = TRUE;
            }
            else if(text::HoriOrientation::LEFT == nAlign && 1 == aVerticalDLB.GetSelectEntryPos())
            {
                aVerticalDLB.SelectEntryPos(0);
                bSet = TRUE;
            }
            else if(text::HoriOrientation::NONE == nAlign && 1 == aVerticalDLB.GetSelectEntryPos())
            {
                aVerticalDLB.SelectEntryPos(0);
                bSet = TRUE;
            }
            if(bSet)
                PosHdl(&aVerticalDLB);
        }
        else
        {
            if(text::VertOrientation::TOP == nAlign)
            {
                if(1 == aHorizontalDLB.GetSelectEntryPos())
                {
                    aHorizontalDLB.SelectEntryPos(0);
                    bSet = TRUE;
                }
                aHoriRelationLB.SelectEntryPos(1);
            }
            else if(text::VertOrientation::CHAR_BOTTOM == nAlign)
            {
                if(2 == aHorizontalDLB.GetSelectEntryPos())
                {
                    aHorizontalDLB.SelectEntryPos(0);
                    bSet = TRUE;
                }
                aHoriRelationLB.SelectEntryPos(0) ;
            }
            if(bSet)
                PosHdl(&aHorizontalDLB);
        }

    }
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:	Horizontale Pos
 --------------------------------------------------------------------*/

IMPL_LINK( SwFrmPage, RelHdl, ListBox *, pLB )
{
    BOOL bHori = pLB == &aHoriRelationLB;

    UpdateExample();

    if (bHori)
        bAtHorzPosModified = TRUE;
    else
        bAtVertPosModified = TRUE;

    if(bHtmlMode  && FLY_AUTO_CNTNT == (RndStdIds)GetAnchor()) // wieder Sonderbehandlung
    {
        if(bHori)
        {
            USHORT nRel = GetRelation(pHMap, aHoriRelationLB);
            if(text::RelOrientation::PRINT_AREA == nRel && 0 == aVerticalDLB.GetSelectEntryPos())
            {
                aVerticalDLB.SelectEntryPos(1);
            }
            else if(text::RelOrientation::CHAR == nRel && 1 == aVerticalDLB.GetSelectEntryPos())
            {
                aVerticalDLB.SelectEntryPos(0);
            }
        }
    }
    if (pLB)	// Nur wenn Handler durch Aenderung des Controllers gerufen wurde
        RangeModifyHdl(0);

    return 0;
}


IMPL_LINK_INLINE_START( SwFrmPage, RealSizeHdl, Button *, EMPTYARG )
{
    aWidthED.SetUserValue( aWidthED. NormalizePercent(aGrfSize.Width() ), FUNIT_TWIP);
    aHeightED.SetUserValue(aHeightED.NormalizePercent(aGrfSize.Height()), FUNIT_TWIP);
    fWidthHeightRatio = aGrfSize.Height() ? double(aGrfSize.Width()) / double(aGrfSize.Height()) : 1.0;
    UpdateExample();
    return 0;
}
IMPL_LINK_INLINE_END( SwFrmPage, RealSizeHdl, Button *, EMPTYARG )


IMPL_LINK( SwFrmPage, AutoWidthClickHdl, void*, EMPTYARG )
{
    if( !IsInGraficMode() )
        HandleAutoCB( aAutoWidthCB.IsChecked(), aWidthFT, aWidthAutoFT );
    return 0;
}

IMPL_LINK( SwFrmPage, AutoHeightClickHdl, void*, EMPTYARG )
{
    if( !IsInGraficMode() )
        HandleAutoCB( aAutoHeightCB.IsChecked(), aHeightFT, aHeightAutoFT );
    return 0;
}

IMPL_LINK( SwFrmPage, ModifyHdl, Edit *, pEdit )
{
    SwTwips nWidth  = static_cast< SwTwips >(aWidthED.DenormalizePercent(aWidthED.GetValue(FUNIT_TWIP)));
    SwTwips nHeight = static_cast< SwTwips >(aHeightED.DenormalizePercent(aHeightED.GetValue(FUNIT_TWIP)));
    if ( aFixedRatioCB.IsChecked() )
    {
        if ( pEdit == &aWidthED )
        {
            nHeight = SwTwips((double)nWidth / fWidthHeightRatio);
            aHeightED.SetPrcntValue(aHeightED.NormalizePercent(nHeight), FUNIT_TWIP);
        }
        else if(pEdit == &aHeightED)
        {
            nWidth = SwTwips((double)nHeight * fWidthHeightRatio);
            aWidthED.SetPrcntValue(aWidthED.NormalizePercent(nWidth), FUNIT_TWIP);
        }
    }
    fWidthHeightRatio = nHeight ? double(nWidth) / double(nHeight) : 1.0;
    UpdateExample();
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwFrmPage::UpdateExample()
{
    USHORT nPos = aHorizontalDLB.GetSelectEntryPos();
    if ( pHMap && nPos != LISTBOX_ENTRY_NOTFOUND )
    {
        USHORT nMapPos = GetMapPos(pHMap, aHorizontalDLB);
        short nAlign = GetAlignment(pHMap, nMapPos, aHorizontalDLB, aHoriRelationLB);
        short nRel = GetRelation(pHMap, aHoriRelationLB);

        aExampleWN.SetHAlign(nAlign);
        aExampleWN.SetHoriRel(nRel);
    }

    nPos = aVerticalDLB.GetSelectEntryPos();
    if ( pVMap && nPos != LISTBOX_ENTRY_NOTFOUND )
    {
        USHORT nMapPos = GetMapPos(pVMap, aVerticalDLB);
        short nAlign = GetAlignment(pVMap, nMapPos, aVerticalDLB, aVertRelationLB);
        short nRel = GetRelation(pVMap, aVertRelationLB);

        aExampleWN.SetVAlign(nAlign);
        aExampleWN.SetVertRel(nRel);
    }

    // Size
    long nXPos = static_cast< long >(aAtHorzPosED.Denormalize(aAtHorzPosED.GetValue(FUNIT_TWIP)));
    long nYPos = static_cast< long >(aAtVertPosED.Denormalize(aAtVertPosED.GetValue(FUNIT_TWIP)));
    aExampleWN.SetRelPos(Point(nXPos, nYPos));

    aExampleWN.SetAnchor( static_cast< USHORT >(GetAnchor()) );
    aExampleWN.Invalidate();
}

void SwFrmPage::Init(const SfxItemSet& rSet, BOOL bReset)
{
    if(!bFormat)
    {
        SwWrtShell* pSh = ((SwFrmDlg*)GetParent()->GetParent())->GetWrtShell();

        // Size
        const BOOL bSizeFixed = pSh->IsSelObjProtected( FLYPROTECT_FIXED );

        aWidthED .Enable( !bSizeFixed );
        aHeightED.Enable( !bSizeFixed );

        // size controls for math OLE objects
        if ( DLG_FRM_OLE == nDlgType && ! bNew )
        {
            // disable width and height for math objects
            const SvGlobalName& rFactNm( pSh->GetOLEObject()->getClassID() );

            struct _GlobalNameId {
                UINT32 n1;
                USHORT n2, n3;
                BYTE b8, b9, b10, b11, b12, b13, b14, b15;
            } aGlbNmIds[4] = { { SO3_SM_CLASSID_60 }, { SO3_SM_CLASSID_50 },
                               { SO3_SM_CLASSID_40 }, { SO3_SM_CLASSID_30 } };

            for ( int i = 0; i < 4; ++i ) {
                const _GlobalNameId& rId = aGlbNmIds[ i ];

                SvGlobalName aGlbNm( rId.n1, rId.n2, rId.n3,
                                     rId.b8, rId.b9, rId.b10, rId.b11,
                                     rId.b12, rId.b13, rId.b14, rId.b15 );

                if( rFactNm == aGlbNm )
                {
                    // disable size controls for math OLE objects
                    aWidthFT.Disable();
                    aWidthED.Disable();
                    aRelWidthCB.Disable();
                    aHeightFT.Disable();
                    aHeightED.Disable();
                    aRelHeightCB.Disable();
                    aFixedRatioCB.Disable();
                    aRealSizeBT.Disable();
                    break;
                }
            }

            // TODO/LATER: get correct aspect
            if(0 != (pSh->GetOLEObject()->getStatus( embed::Aspects::MSOLE_CONTENT ) & embed::EmbedMisc::MS_EMBED_RECOMPOSEONRESIZE ) )
                aRealSizeBT.Disable();
        }
    }

    const SwFmtFrmSize& rSize = (const SwFmtFrmSize&)rSet.Get(RES_FRM_SIZE);
    sal_Int64 nWidth  = aWidthED.NormalizePercent(rSize.GetWidth());
    sal_Int64 nHeight = aHeightED.NormalizePercent(rSize.GetHeight());

    if (nWidth != aWidthED.GetValue(FUNIT_TWIP))
    {
        if(!bReset)
        {
            // Wert wurde von Umlauf-Tabpage geaendert und muss
            // mit Modify-Flag gesetzt werden
            aWidthED.SetUserValue(nWidth, FUNIT_TWIP);
        }
        else
            aWidthED.SetPrcntValue(nWidth, FUNIT_TWIP);
    }

    if (nHeight != aHeightED.GetValue(FUNIT_TWIP))
    {
        if (!bReset)
        {
            // Wert wurde von Umlauf-Tabpage geaendert und muss
            // mit Modify-Flag gesetzt werden
            aHeightED.SetUserValue(nHeight, FUNIT_TWIP);
        }
        else
            aHeightED.SetPrcntValue(nHeight, FUNIT_TWIP);
    }

    if (!IsInGraficMode())
    {
        SwFrmSize eSize = rSize.GetHeightSizeType();
        BOOL bCheck = eSize != ATT_FIX_SIZE;
        aAutoHeightCB.Check( bCheck );
        HandleAutoCB( bCheck, aHeightFT, aHeightAutoFT );
        if( eSize == ATT_VAR_SIZE )
            aHeightED.SetValue( aHeightED.GetMin(), FUNIT_NONE );

        eSize = rSize.GetWidthSizeType();
        bCheck = eSize != ATT_FIX_SIZE;
        aAutoWidthCB.Check( bCheck );
        HandleAutoCB( bCheck, aWidthFT, aWidthAutoFT );
        if( eSize == ATT_VAR_SIZE )
            aWidthED.SetValue( aWidthED.GetMin(), FUNIT_NONE );

        if ( !bFormat )
        {
            SwWrtShell* pSh = ((SwFrmDlg*)GetParent()->GetParent())->GetWrtShell();
            const SwFrmFmt* pFmt = pSh->GetFlyFrmFmt();
            if( pFmt && pFmt->GetChain().GetNext() )
                aAutoHeightCB.Enable( FALSE );
        }
    }
    else
        aAutoHeightCB.Hide();

    // Umlauf-Abstand fuer zeichengebundene Rahmen organisieren
    const SvxULSpaceItem &rUL = (const SvxULSpaceItem &)rSet.Get(RES_UL_SPACE);
    nUpperBorder = rUL.GetUpper();
    nLowerBorder = rUL.GetLower();

    if(SFX_ITEM_SET == rSet.GetItemState(FN_KEEP_ASPECT_RATIO))
    {
        aFixedRatioCB.Check(((const SfxBoolItem&)rSet.Get(FN_KEEP_ASPECT_RATIO)).GetValue());
        aFixedRatioCB.SaveValue();
    }

    // Spalten
    SwFmtCol aCol( (const SwFmtCol&)rSet.Get(RES_COL) );
    ::FitToActualSize( aCol, (USHORT)rSize.GetWidth() );

    RndStdIds eAnchorId = (RndStdIds)GetAnchor();

    if ( bNew && !bFormat )
        InitPos(eAnchorId, USHRT_MAX, 0, USHRT_MAX, USHRT_MAX, LONG_MAX, LONG_MAX);
    else
    {
        const SwFmtHoriOrient& rHori = (const SwFmtHoriOrient&)rSet.Get(RES_HORI_ORIENT);
        const SwFmtVertOrient& rVert = (const SwFmtVertOrient&)rSet.Get(RES_VERT_ORIENT);
        nOldH 	 = rHori.GetHoriOrient();
        nOldHRel = rHori.GetRelationOrient();
        nOldV 	 = rVert.GetVertOrient(),
        nOldVRel = rVert.GetRelationOrient();

        if (eAnchorId == FLY_PAGE)
        {
            if (nOldHRel == text::RelOrientation::FRAME)
                nOldHRel = text::RelOrientation::PAGE_FRAME;
            else if (nOldHRel == text::RelOrientation::PRINT_AREA)
                nOldHRel = text::RelOrientation::PAGE_PRINT_AREA;
            if (nOldVRel == text::RelOrientation::FRAME)
                nOldVRel = text::RelOrientation::PAGE_FRAME;
            else if (nOldVRel == text::RelOrientation::PRINT_AREA)
                nOldVRel = text::RelOrientation::PAGE_PRINT_AREA;
        }

        aMirrorPagesCB.Check(rHori.IsPosToggle());
        aMirrorPagesCB.SaveValue();

        InitPos(eAnchorId,
                nOldH,
                nOldHRel,
                nOldV,
                nOldVRel,
                rHori.GetPos(),
                rVert.GetPos());
    }

    // Transparent fuers Bsp
    // Umlauf fuers Bsp
    const SwFmtSurround& rSurround = (const SwFmtSurround&)rSet.Get(RES_SURROUND);
    aExampleWN.SetWrap ( static_cast< USHORT >(rSurround.GetSurround()) );

    if ( rSurround.GetSurround() == SURROUND_THROUGHT )
    {
        const SvxOpaqueItem& rOpaque = (const SvxOpaqueItem&)rSet.Get(RES_OPAQUE);
        aExampleWN.SetTransparent(!rOpaque.GetValue());
    }

    // ggf auf Prozent umschalten
    RangeModifyHdl(&aWidthED);	// Referenzwerte setzen (fuer 100%)

    if (rSize.GetWidthPercent() == 0xff || rSize.GetHeightPercent() == 0xff)
        aFixedRatioCB.Check(TRUE);
    if (rSize.GetWidthPercent() && rSize.GetWidthPercent() != 0xff &&
        !aRelWidthCB.IsChecked())
    {
        aRelWidthCB.Check(TRUE);
        RelSizeClickHdl(&aRelWidthCB);
        aWidthED.SetPrcntValue(rSize.GetWidthPercent(), FUNIT_CUSTOM);
    }
    if (rSize.GetHeightPercent() && rSize.GetHeightPercent() != 0xff &&
        !aRelHeightCB.IsChecked())
    {
        aRelHeightCB.Check(TRUE);
        RelSizeClickHdl(&aRelHeightCB);
        aHeightED.SetPrcntValue(rSize.GetHeightPercent(), FUNIT_CUSTOM);
    }
    aRelWidthCB.SaveValue();
    aRelHeightCB.SaveValue();
}

USHORT* SwFrmPage::GetRanges()
{
    return aPageRg;
}
/* -----------------------------03.11.00 10:52--------------------------------

 ---------------------------------------------------------------------------*/
void SwFrmPage::SetFormatUsed(BOOL bFmt)
{
    bFormat  	= bFmt;
    if(bFormat)
    {
        aAnchorAtPageRB.Hide();
        aAnchorAtParaRB.Hide();
        aAnchorAtCharRB.Hide();
        aAnchorAsCharRB.Hide();
        aAnchorAtFrameRB.Hide();
        aTypeFL.Hide();
        aTypeSepFL.Hide();

//        Point aSizePos = aSizeFL.GetPosPixel();
//        Size aSizeSize = aSizeFL.GetSizePixel();
//        aSizeSize.Width() = aTypeFL.GetPosPixel().X() +
//                    aTypeFL.GetSizePixel().Width() - aSizePos.X();
//        aSizeFL.SetSizePixel(aSizeSize);
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwGrfExtPage::SwGrfExtPage(Window *pParent, const SfxItemSet &rSet) :
    SfxTabPage( pParent, SW_RES(TP_GRF_EXT), rSet ),
    aMirrorFL       (this, SW_RES( FL_MIRROR )),
    aMirrorVertBox  (this, SW_RES( CB_VERT )),
    aMirrorHorzBox  (this, SW_RES( CB_HOR )),
    aAllPagesRB     (this, SW_RES( RB_MIRROR_ALL_PAGES )),
    aLeftPagesRB    (this, SW_RES( RB_MIRROR_LEFT_PAGES )),
    aRightPagesRB   (this, SW_RES( RB_MIRROR_RIGHT_PAGES )),
    aBmpWin         (this, WN_BMP, Graphic(), BitmapEx(SW_RES(BMP_EXAMPLE)), BitmapEx(SW_RES(BMP_EXAMPLE_HC)) ),
    aConnectFL      (this, SW_RES( FL_CONNECT )),
    aConnectFT      (this, SW_RES( FT_CONNECT )),
    aConnectED      (this, SW_RES( ED_CONNECT )),
    aBrowseBT       (this, SW_RES( PB_BROWSE )),
    pGrfDlg         ( 0 )
{
    FreeResource();

    SetExchangeSupport();
    aMirrorHorzBox.SetClickHdl( LINK(this, SwGrfExtPage, MirrorHdl));
    aMirrorVertBox.SetClickHdl( LINK(this, SwGrfExtPage, MirrorHdl));
    aBrowseBT.SetClickHdl	 ( LINK(this, SwGrfExtPage, BrowseHdl));
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwGrfExtPage::~SwGrfExtPage()
{
    delete pGrfDlg;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SfxTabPage* SwGrfExtPage::Create( Window *pParent, const SfxItemSet &rSet )
{
    return new SwGrfExtPage( pParent, rSet );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwGrfExtPage::Reset(const SfxItemSet &rSet)
{
    const SfxPoolItem* pItem;
    USHORT nHtmlMode = ::GetHtmlMode((const SwDocShell*)SfxObjectShell::Current());
    bHtmlMode = nHtmlMode & HTMLMODE_ON ? TRUE : FALSE;

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_GRF_CONNECT, TRUE, &pItem)
        && ((const SfxBoolItem *)pItem)->GetValue() )
    {
        aBrowseBT.Enable();
        aConnectED.SetReadOnly(FALSE);
    }

    ActivatePage(rSet);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwGrfExtPage::ActivatePage(const SfxItemSet& rSet)
{
    const SvxProtectItem& rProt = (const SvxProtectItem& )rSet.Get(RES_PROTECT);
    BOOL bProtCntnt = rProt.IsCntntProtected();

    const SfxPoolItem* pItem = 0;
    BOOL bEnable = FALSE;
    BOOL bEnableMirrorRB = FALSE;

    SfxItemState eState = rSet.GetItemState(RES_GRFATR_MIRRORGRF, TRUE, &pItem);
    if( SFX_ITEM_UNKNOWN != eState && !bProtCntnt && !bHtmlMode )
    {
        if( SFX_ITEM_SET != eState )
            pItem = &rSet.Get( RES_GRFATR_MIRRORGRF );

        bEnable = TRUE;

        MirrorGraph eMirror = static_cast< MirrorGraph >(((const SwMirrorGrf* )pItem)->GetValue());
        switch( eMirror )
        {
        case RES_MIRROR_GRAPH_DONT: break;
        case RES_MIRROR_GRAPH_VERT: aMirrorHorzBox.Check(TRUE); break;
        case RES_MIRROR_GRAPH_HOR:  aMirrorVertBox.Check(TRUE); break;
        case RES_MIRROR_GRAPH_BOTH: aMirrorHorzBox.Check(TRUE);
                                    aMirrorVertBox.Check(TRUE);
                                    break;
        default:
            ;
        }

        USHORT nPos = ((const SwMirrorGrf* )pItem)->IsGrfToggle() ? 1 : 0;
        nPos += (eMirror == RES_MIRROR_GRAPH_VERT || eMirror == RES_MIRROR_GRAPH_BOTH)
                 ? 2 : 0;

        bEnableMirrorRB = nPos != 0;

        switch (nPos)
        {
            case 1:	// Auf linken bzw geraden Seiten spiegeln
                aLeftPagesRB.Check();
                aMirrorHorzBox.Check(TRUE);
                break;
            case 2:	// Auf allen Seiten spiegeln
                aAllPagesRB.Check();
                break;
            case 3:	// Auf rechten bzw ungeraden Seiten spiegeln
                aRightPagesRB.Check();
                break;
            default:
                aAllPagesRB.Check();
                break;
        }
    }

    if( SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_GRAF_GRAPHIC, FALSE, &pItem ) )
    {
        const SvxBrushItem& rBrush = *(SvxBrushItem*)pItem;
        if( rBrush.GetGraphicLink() )
        {
            aGrfName = aNewGrfName = *rBrush.GetGraphicLink();
            aConnectED.SetText( aNewGrfName );
        }
        const Graphic* pGrf = rBrush.GetGraphic();
        if( pGrf )
            aBmpWin.SetGraphic( *pGrf );
    }

    aAllPagesRB .Enable(bEnableMirrorRB);
    aLeftPagesRB.Enable(bEnableMirrorRB);
    aRightPagesRB.Enable(bEnableMirrorRB);
    aMirrorHorzBox.Enable(bEnable);
    aMirrorVertBox.Enable(bEnable);
    aMirrorFL.Enable(bEnable);

    aAllPagesRB .SaveValue();
    aLeftPagesRB.SaveValue();
    aRightPagesRB.SaveValue();
    aMirrorHorzBox.SaveValue();
    aMirrorVertBox.SaveValue();

    aBmpWin.MirrorHorz( aMirrorVertBox.IsChecked() );
    aBmpWin.MirrorVert( aMirrorHorzBox.IsChecked() );
    aBmpWin.Invalidate();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL SwGrfExtPage::FillItemSet( SfxItemSet &rSet )
{
    BOOL bModified = FALSE;
    if ( aMirrorHorzBox.GetSavedValue() != aMirrorHorzBox.IsChecked() ||
         aMirrorVertBox.GetSavedValue() != aMirrorVertBox.IsChecked() ||
         aAllPagesRB .GetSavedValue() != aAllPagesRB .IsChecked() ||
         aLeftPagesRB.GetSavedValue() != aLeftPagesRB.IsChecked() ||
         aRightPagesRB.GetSavedValue() != aRightPagesRB.IsChecked())
    {
        bModified = TRUE;

        BOOL bHori = FALSE;

        if (aMirrorHorzBox.IsChecked() &&
                !aLeftPagesRB.IsChecked())
            bHori = TRUE;

        MirrorGraph eMirror;
        eMirror = aMirrorVertBox.IsChecked() && bHori ?
                    RES_MIRROR_GRAPH_BOTH : bHori ?
                    RES_MIRROR_GRAPH_VERT : aMirrorVertBox.IsChecked() ?
                    RES_MIRROR_GRAPH_HOR  : RES_MIRROR_GRAPH_DONT;

        BOOL bMirror = !aAllPagesRB.IsChecked();
        SwMirrorGrf aMirror( eMirror );
        aMirror.SetGrfToggle(bMirror );
        rSet.Put( aMirror );
    }

    if( aGrfName != aNewGrfName || aConnectED.IsModified() )
    {
        bModified = TRUE;
        aGrfName = aConnectED.GetText();
        rSet.Put( SvxBrushItem( aGrfName, aFilterName, GPOS_LT,
                                SID_ATTR_GRAF_GRAPHIC ));
    }
    return bModified;
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

int SwGrfExtPage::DeactivatePage(SfxItemSet *_pSet)
{
    if( _pSet )
        FillItemSet( *_pSet );
    return TRUE;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwGrfExtPage, BrowseHdl, Button *, EMPTYARG )
{
    if(!pGrfDlg)
    {
        pGrfDlg = new FileDialogHelper( SFXWB_GRAPHIC );
        pGrfDlg->SetTitle(SW_RESSTR(STR_EDIT_GRF ));
    }
    pGrfDlg->SetDisplayDirectory( aConnectED.GetText() );
    uno::Reference < ui::dialogs::XFilePicker > xFP = pGrfDlg->GetFilePicker();
    uno::Reference < ui::dialogs::XFilePickerControlAccess > xCtrlAcc(xFP, uno::UNO_QUERY);
    sal_Bool bTrue = sal_True;
    uno::Any aVal(&bTrue, ::getBooleanCppuType());
    xCtrlAcc->setValue( ui::dialogs::ExtendedFilePickerElementIds::CHECKBOX_LINK, 0, aVal);

    if ( pGrfDlg->Execute() == ERRCODE_NONE )
    {	// ausgewaehlten Filter merken
        aFilterName = pGrfDlg->GetCurrentFilter();
        aNewGrfName = INetURLObject::decode( pGrfDlg->GetPath(),
                                        INET_HEX_ESCAPE,
                                           INetURLObject::DECODE_UNAMBIGUOUS,
                                        RTL_TEXTENCODING_UTF8 );
        aConnectED.SetModifyFlag();
        aConnectED.SetText( aNewGrfName );
        //Spiegeln zuruecksetzen, da u. U. eine Bitmap
        //gegen eine andere Art von Grafik ausgetauscht worden ist,
        //die nicht gespiegelt werden kann.
        aMirrorVertBox.Check(FALSE);
        aMirrorHorzBox.Check(FALSE);
        aAllPagesRB .Enable(FALSE);
        aLeftPagesRB.Enable(FALSE);
        aRightPagesRB.Enable(FALSE);
        aBmpWin.MirrorHorz(FALSE);
        aBmpWin.MirrorVert(FALSE);

        Graphic aGraphic;
        ::LoadGraphic( pGrfDlg->GetPath(), aEmptyStr, aGraphic );
        aBmpWin.SetGraphic(aGraphic);

        BOOL bEnable = GRAPHIC_BITMAP      == aGraphic.GetType() ||
                            GRAPHIC_GDIMETAFILE == aGraphic.GetType();
        aMirrorVertBox.Enable(bEnable);
        aMirrorHorzBox.Enable(bEnable);
        aAllPagesRB .Enable(bEnable);
        aLeftPagesRB.Enable(bEnable);
        aRightPagesRB.Enable(bEnable);
    }
    return 0;
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwGrfExtPage, MirrorHdl, CheckBox *, EMPTYARG )
{
    BOOL bEnable = aMirrorHorzBox.IsChecked();

    aBmpWin.MirrorHorz( aMirrorVertBox.IsChecked() );
    aBmpWin.MirrorVert( bEnable );

    aAllPagesRB .Enable(bEnable);
    aLeftPagesRB.Enable(bEnable);
    aRightPagesRB.Enable(bEnable);

    if (!aAllPagesRB.IsChecked() && !aLeftPagesRB.IsChecked() && !aRightPagesRB.IsChecked())
        aAllPagesRB.Check();

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung: BeispielWindow
 --------------------------------------------------------------------*/

BmpWindow::BmpWindow( Window* pPar, USHORT nId,
                        const Graphic& rGraphic, const BitmapEx& rBmp, const BitmapEx& rBmpHC ) :
    Window(pPar, SW_RES(nId)),
    aGraphic(rGraphic),
    aBmp(rBmp),
    aBmpHC(rBmpHC),
    bHorz(FALSE),
    bVert(FALSE),
    bGraphic(FALSE),
    bLeftAlign(FALSE)
{
    SetBackground();
}

void BmpWindow::Paint( const Rectangle& )
{
    Point aPntPos;
    Size  aPntSz( GetSizePixel() );
    Size  aGrfSize;
    if(bGraphic)
        aGrfSize = ::GetGraphicSizeTwip(aGraphic, this);
    //it should show the default bitmap also if no graphic can be found
    if(!aGrfSize.Width() && !aGrfSize.Height())
        aGrfSize =  PixelToLogic(aBmp.GetSizePixel());

    long nRelGrf = aGrfSize.Width() * 100L / aGrfSize.Height();
    long nRelWin = aPntSz.Width() * 100L / aPntSz.Height();
    if(nRelGrf < nRelWin)
    {
        const long nWidth = aPntSz.Width();
        aPntSz.Width() = aPntSz.Height() * nRelGrf /100;
        if(!bLeftAlign)
            aPntPos.X() += nWidth - aPntSz.Width() ;
    }

    if ( bHorz )
    {
        aPntPos.Y()     += aPntSz.Height();
        aPntPos.Y() --;
        aPntSz.Height() *= -1;
    }
    if ( bVert )
    {
        aPntPos.X()     += aPntSz.Width();
        aPntPos.X()--;
        aPntSz.Width()  *= -1;
    }

    if ( bGraphic )
        aGraphic.Draw( this, aPntPos, aPntSz );
    else
    {
        bool bHC = GetSettings().GetStyleSettings().GetHighContrastMode();
        DrawBitmapEx( aPntPos, aPntSz, bHC ? aBmpHC : aBmp );
    }
}

BmpWindow::~BmpWindow()
{
}

void BmpWindow::SetGraphic(const Graphic& rGrf)
{
    aGraphic = rGrf;
    Size aGrfSize = ::GetGraphicSizeTwip(aGraphic, this);
    bGraphic = aGrfSize.Width() && aGrfSize.Height();
    Invalidate();
}

/***************************************************************************
    Beschreibung: 	URL und ImageMap an Rahmen einstellen
***************************************************************************/


SwFrmURLPage::SwFrmURLPage( Window *pParent, const SfxItemSet &rSet ) :
    SfxTabPage(pParent, 	SW_RES(TP_FRM_URL), rSet),
    aHyperLinkFL    (this, SW_RES( FL_HYPERLINK )),
    aURLFT      	(this, SW_RES( FT_URL  	 )),
    aURLED			(this, SW_RES( ED_URL 	 )),
    aSearchPB       (this, SW_RES( PB_SEARCH  )),
    aNameFT      	(this, SW_RES( FT_NAME   )),
    aNameED			(this, SW_RES( ED_NAME 	 )),
    aFrameFT        (this, SW_RES( FT_FRAME   )),
    aFrameCB        (this, SW_RES( CB_FRAME   )),

    aImageFL        (this, SW_RES( FL_IMAGE   )),
    aServerCB       (this, SW_RES( CB_SERVER  )),
    aClientCB       (this, SW_RES( CB_CLIENT  ))
{
    FreeResource();
    aSearchPB.SetClickHdl(LINK(this, SwFrmURLPage, InsertFileHdl));
}

/***************************************************************************
    Beschreibung:
***************************************************************************/

SwFrmURLPage::~SwFrmURLPage()
{
}

/***************************************************************************
    Beschreibung:
***************************************************************************/

void SwFrmURLPage::Reset( const SfxItemSet &rSet )
{
    const SfxPoolItem* pItem;
    if ( SFX_ITEM_SET == rSet.GetItemState( SID_DOCFRAME, TRUE, &pItem))
    {
        TargetList* pList = new TargetList;
        ((const SfxFrameItem*)pItem)->GetFrame()->GetTargetList(*pList);
        USHORT nCount = (USHORT)pList->Count();
        if( nCount )
        {
            USHORT i;
            for ( i = 0; i < nCount; i++ )
            {
                aFrameCB.InsertEntry(*pList->GetObject(i));
            }
            for ( i = nCount; i; i-- )
            {
                delete pList->GetObject( i - 1 );
            }
        }
        delete pList;
    }

    if ( SFX_ITEM_SET == rSet.GetItemState( RES_URL, TRUE, &pItem ) )
    {
        const SwFmtURL* pFmtURL = (const SwFmtURL*)pItem;
        aURLED.SetText( INetURLObject::decode( pFmtURL->GetURL(),
                                        INET_HEX_ESCAPE,
                                           INetURLObject::DECODE_UNAMBIGUOUS,
                                        RTL_TEXTENCODING_UTF8 ));
        aNameED.SetText( pFmtURL->GetName());

        aClientCB.Enable( pFmtURL->GetMap() != 0 );
        aClientCB.Check ( pFmtURL->GetMap() != 0 );
        aServerCB.Check ( pFmtURL->IsServerMap() );

        aFrameCB.SetText(pFmtURL->GetTargetFrameName());
        aFrameCB.SaveValue();
    }
    else
        aClientCB.Enable( FALSE );

    aServerCB.SaveValue();
    aClientCB.SaveValue();
}



/***************************************************************************
    Beschreibung:
***************************************************************************/

BOOL SwFrmURLPage::FillItemSet(SfxItemSet &rSet)
{
    BOOL bModified = FALSE;
    const SwFmtURL* pOldURL = (SwFmtURL*)GetOldItem(rSet, RES_URL);
    SwFmtURL* pFmtURL;
    if(pOldURL)
        pFmtURL = (SwFmtURL*)pOldURL->Clone();
    else
        pFmtURL = new SwFmtURL();

    {
        String sText = aURLED.GetText();

        if( pFmtURL->GetURL() != sText ||
            pFmtURL->GetName() != aNameED.GetText() ||
            aServerCB.IsChecked() != pFmtURL->IsServerMap() )
        {
            pFmtURL->SetURL( sText, aServerCB.IsChecked() );
            pFmtURL->SetName( aNameED.GetText() );
            bModified = TRUE;
        }
    }

    if(!aClientCB.IsChecked() && pFmtURL->GetMap() != 0)
    {
        pFmtURL->SetMap(0);
        bModified = TRUE;
    }

    if(pFmtURL->GetTargetFrameName() != aFrameCB.GetText())
    {
        pFmtURL->SetTargetFrameName(aFrameCB.GetText());
        bModified = TRUE;
    }
    rSet.Put(*pFmtURL);
    delete pFmtURL;
    return bModified;
}

/***************************************************************************
    Beschreibung:
***************************************************************************/

SfxTabPage* SwFrmURLPage::Create(Window *pParent, const SfxItemSet &rSet)
{
    return new SwFrmURLPage( pParent, rSet );
}

/***************************************************************************
    Beschreibung:
***************************************************************************/

IMPL_LINK( SwFrmURLPage, InsertFileHdl, PushButton *, EMPTYARG )
{
    FileDialogHelper aDlgHelper( ui::dialogs::TemplateDescription::FILEOPEN_SIMPLE, 0 );
    uno::Reference < ui::dialogs::XFilePicker > xFP = aDlgHelper.GetFilePicker();

    try
    {
        String sTemp(aURLED.GetText());
        if(sTemp.Len())
            xFP->setDisplayDirectory(sTemp);
    }
    catch( const uno::Exception& rEx )
    {
        (void) rEx;
    }
    if( aDlgHelper.Execute() == ERRCODE_NONE )
    {
        aURLED.SetText( xFP->getFiles().getConstArray()[0] );
    }

    return 0;
}

/* -----------------------------03.11.00 10:56--------------------------------

 ---------------------------------------------------------------------------*/
void lcl_Move(Window& rWin, sal_Int32 nDiff)
{
    Point aPos(rWin.GetPosPixel());
    aPos.Y() -= nDiff;
    rWin.SetPosPixel(aPos);
}

/*-----------------13.11.96 13.15-------------------

--------------------------------------------------*/

SwFrmAddPage::SwFrmAddPage(Window *pParent, const SfxItemSet &rSet ) :
    SfxTabPage(pParent, 	SW_RES(TP_FRM_ADD), rSet),

    aNameFT            (this, SW_RES(FT_NAME)),
    aNameED            (this, SW_RES(ED_NAME)),
    aAltNameFT         (this, SW_RES(FT_ALT_NAME)),
    aAltNameED         (this, SW_RES(ED_ALT_NAME)),
    aPrevFT            (this, SW_RES(FT_PREV)),
    aPrevLB            (this, SW_RES(LB_PREV)),
    aNextFT            (this, SW_RES(FT_NEXT)),
    aNextLB            (this, SW_RES(LB_NEXT)),
    aNamesFL           (this, SW_RES(FL_NAME)),

    aProtectContentCB  (this, SW_RES(CB_PROTECT_CONTENT)),
    aProtectFrameCB    (this, SW_RES(CB_PROTECT_FRAME)),
    aProtectSizeCB     (this, SW_RES(CB_PROTECT_SIZE)),
    aProtectFL         (this, SW_RES(FL_PROTECT)),

    aEditInReadonlyCB  (this, SW_RES(CB_EDIT_IN_READONLY)),
    aPrintFrameCB      (this, SW_RES(CB_PRINT_FRAME)),
    aTextFlowFT        (this, SW_RES(FT_TEXTFLOW)),
    aTextFlowLB		   (this, SW_RES(LB_TEXTFLOW)),

    aExtFL             (this, SW_RES(FL_EXT)),

    pWrtSh(0),

    nDlgType(0),
    bHtmlMode(sal_False),
    bFormat(sal_False),
    bNew(sal_False)
{
    FreeResource();

}

/*-----------------13.11.96 13.15-------------------

--------------------------------------------------*/

SwFrmAddPage::~SwFrmAddPage()
{
}

/*-----------------13.11.96 13.15-------------------

--------------------------------------------------*/

SfxTabPage* SwFrmAddPage::Create(Window *pParent, const SfxItemSet &rSet)
{
    return new SwFrmAddPage(pParent, rSet);
}

/*-----------------13.11.96 13.14-------------------

--------------------------------------------------*/

void SwFrmAddPage::Reset(const SfxItemSet &rSet )
{
    const SfxPoolItem* pItem;
    USHORT nHtmlMode = ::GetHtmlMode((const SwDocShell*)SfxObjectShell::Current());
    bHtmlMode = nHtmlMode & HTMLMODE_ON ? TRUE : FALSE;
    if(bHtmlMode)
    {
        aProtectContentCB .Hide();
        aProtectFrameCB   .Hide();
        aProtectSizeCB    .Hide();
        aEditInReadonlyCB .Hide();
        aPrintFrameCB     .Hide();
        aExtFL            .Hide();
        aProtectFL.Hide();
    }
    if ( DLG_FRM_GRF == nDlgType || DLG_FRM_OLE == nDlgType )
    {
        aEditInReadonlyCB.Hide();
        aPrintFrameCB.SetPosPixel(aEditInReadonlyCB.GetPosPixel());
    }

    if(SFX_ITEM_SET == rSet.GetItemState(FN_SET_FRM_ALT_NAME, FALSE, &pItem))
    {
        aAltNameED.SetText(((const SfxStringItem*)pItem)->GetValue());
        aAltNameED.SaveValue();
    }

    if(!bFormat)
    {
        // Grafik einfuegen - Eigenschaften
        // bNew ist nicht gesetzt, deswegen ueber Selektion erkennen
        String aTmpName1;
        if(SFX_ITEM_SET == rSet.GetItemState(FN_SET_FRM_NAME, FALSE, &pItem))
        {
            aTmpName1 = ((const SfxStringItem*)pItem)->GetValue();
        }

        DBG_ASSERT(pWrtSh, "keine Shell?");
        if( bNew || !aTmpName1.Len() )

            switch( nDlgType )
            {
                case DLG_FRM_GRF:
                    aTmpName1 = pWrtSh->GetUniqueGrfName();
                    break;
                case DLG_FRM_OLE:
                    aTmpName1 = pWrtSh->GetUniqueOLEName();
                    break;
                default:
                    aTmpName1 = pWrtSh->GetUniqueFrameName();
                    break;
            }

        aNameED.SetText( aTmpName1 );
        aNameED.SaveValue();
    }
    else
    {
        aNameED.Enable( FALSE );
        aAltNameED.Enable(FALSE);
        aNameFT.Enable( FALSE );
        aAltNameFT.Enable(FALSE);
        aNamesFL.Enable(FALSE);
    }
    if(nDlgType == DLG_FRM_STD && aAltNameFT.IsVisible())
    {
        aAltNameFT.Hide();
        aAltNameED.Hide();
        //move all controls one step up
        Window* aWindows[] =
        {
            &aPrevFT,
            &aPrevLB,
            &aNextFT,
            &aNextLB,
            &aNamesFL,
            &aProtectContentCB,
            &aProtectFrameCB,
            &aProtectSizeCB,
            &aProtectFL,
            &aEditInReadonlyCB,
            &aPrintFrameCB,
            &aTextFlowFT,
            &aTextFlowLB,
            &aExtFL,
            0
        };
        sal_Int32 nOffset = aAltNameED.GetPosPixel().Y() - aNameED.GetPosPixel().Y();
        sal_Int32 nIdx = 0;
        while(aWindows[nIdx])
        {
            lcl_Move(*aWindows[nIdx++], nOffset);
        }
    }
    else
    {
        aNameED.SetModifyHdl(LINK(this, SwFrmAddPage, EditModifyHdl));
    }

    if (!bNew)
    {
        SwFrmFmt* pFmt = pWrtSh->GetFlyFrmFmt();

        if (pFmt)
        {
            const SwFmtChain &rChain = pFmt->GetChain();
            const SwFlyFrmFmt* pFlyFmt;
            String sNextChain, sPrevChain;
            if ((pFlyFmt = rChain.GetPrev()) != 0)
            {
                sPrevChain = pFlyFmt->GetName();
            }

            if ((pFlyFmt = rChain.GetNext()) != 0)
            {
                sNextChain = pFlyFmt->GetName();
            }
            //determine chainable frames
            ::std::vector< String > aPrevPageFrames;
            ::std::vector< String > aThisPageFrames;
            ::std::vector< String > aNextPageFrames;
            ::std::vector< String > aRemainFrames;
            pWrtSh->GetConnectableFrmFmts(*pFmt, sNextChain, FALSE,
                            aPrevPageFrames, aThisPageFrames, aNextPageFrames, aRemainFrames );
            lcl_InsertVectors(aPrevLB, aPrevPageFrames, aThisPageFrames, aNextPageFrames, aRemainFrames);
            if(sPrevChain.Len())
            {
                if(LISTBOX_ENTRY_NOTFOUND == aPrevLB.GetEntryPos(sPrevChain))
                    aPrevLB.InsertEntry(sPrevChain, 1);
                aPrevLB.SelectEntry(sPrevChain);
            }
            else
                aPrevLB.SelectEntryPos(0);
            aPrevPageFrames.erase(aPrevPageFrames.begin(), aPrevPageFrames.end());
            aNextPageFrames.erase(aNextPageFrames.begin(), aNextPageFrames.end());
            aThisPageFrames.erase(aThisPageFrames.begin(), aThisPageFrames.end());
            aRemainFrames.erase(aRemainFrames.begin(), aRemainFrames.end());


            pWrtSh->GetConnectableFrmFmts(*pFmt, sPrevChain, TRUE,
                            aPrevPageFrames, aThisPageFrames, aNextPageFrames, aRemainFrames );
            lcl_InsertVectors(aNextLB, aPrevPageFrames, aThisPageFrames, aNextPageFrames, aRemainFrames);
            if(sNextChain.Len())
            {
                if(LISTBOX_ENTRY_NOTFOUND == aNextLB.GetEntryPos(sNextChain))
                    aNextLB.InsertEntry(sNextChain, 1);
                aNextLB.SelectEntry(sNextChain);
            }
            else
                aNextLB.SelectEntryPos(0);
            Link aLink(LINK(this, SwFrmAddPage, ChainModifyHdl));
            aPrevLB.SetSelectHdl(aLink);
            aNextLB.SetSelectHdl(aLink);
        }
    }
    // Pos Protected
    const SvxProtectItem& rProt = (const SvxProtectItem& )rSet.Get(RES_PROTECT);
    aProtectFrameCB.Check(rProt.IsPosProtected());
    aProtectContentCB.Check(rProt.IsCntntProtected());
    aProtectSizeCB.Check(rProt.IsSizeProtected());

    const SwFmtEditInReadonly& rEdit = (const SwFmtEditInReadonly& )rSet.Get(RES_EDIT_IN_READONLY);
    aEditInReadonlyCB.Check(rEdit.GetValue());			aEditInReadonlyCB.SaveValue();

    // drucken
    const SvxPrintItem& rPrt = (const SvxPrintItem&)rSet.Get(RES_PRINT);
    aPrintFrameCB.Check(rPrt.GetValue());				aPrintFrameCB.SaveValue();

    // textflow
    SfxItemState eState;
    if( (!bHtmlMode || (0 != (nHtmlMode&HTMLMODE_SOME_STYLES)))
            && DLG_FRM_GRF != nDlgType && DLG_FRM_OLE != nDlgType &&
        SFX_ITEM_UNKNOWN != ( eState = rSet.GetItemState(
                                        RES_FRAMEDIR, TRUE )) )
    {
        aTextFlowFT.Show();
        aTextFlowLB.Show();

        //vertical text flow is not possible in HTML
        if(bHtmlMode)
        {
            ULONG nData = FRMDIR_VERT_TOP_RIGHT;
            aTextFlowLB.RemoveEntry(aTextFlowLB.GetEntryPos((void*)nData));
        }
        sal_uInt16 nPos, nVal = ((SvxFrameDirectionItem&)rSet.Get(RES_FRAMEDIR)).GetValue();
        for( nPos = aTextFlowLB.GetEntryCount(); nPos; )
            if( (sal_uInt16)(long)aTextFlowLB.GetEntryData( --nPos ) == nVal )
                break;
        aTextFlowLB.SelectEntryPos( nPos );
        aTextFlowLB.SaveValue();
    }
    else
    {
        aTextFlowFT.Hide();
        aTextFlowLB.Hide();
    }
}

/*-----------------13.11.96 13.20-------------------

--------------------------------------------------*/

BOOL SwFrmAddPage::FillItemSet(SfxItemSet &rSet)
{
    BOOL bRet = FALSE;
    if (aNameED.GetText() != aNameED.GetSavedValue())
        bRet |= 0 != rSet.Put(SfxStringItem(FN_SET_FRM_NAME, aNameED.GetText()));
    if (aAltNameED.GetText()  != aAltNameED.GetSavedValue())
        bRet |= 0 != rSet.Put(SfxStringItem(FN_SET_FRM_ALT_NAME, aAltNameED.GetText()));

    const SfxPoolItem* pOldItem;
    SvxProtectItem aProt ( (const SvxProtectItem& )GetItemSet().Get(RES_PROTECT) );
    aProt.SetCntntProtect( aProtectContentCB.IsChecked() );
    aProt.SetSizeProtect ( aProtectSizeCB.IsChecked() );
    aProt.SetPosProtect  ( aProtectFrameCB.IsChecked() );
    if ( 0 == (pOldItem = GetOldItem(rSet, FN_SET_PROTECT)) ||
                aProt != *pOldItem )
        bRet |= 0 != rSet.Put( aProt);

    BOOL bChecked;
    if ( (bChecked = aEditInReadonlyCB.IsChecked()) != aEditInReadonlyCB.GetSavedValue() )
        bRet |= 0 != rSet.Put( SwFmtEditInReadonly( RES_EDIT_IN_READONLY, bChecked));

    if ( (bChecked = aPrintFrameCB.IsChecked()) != aPrintFrameCB.GetSavedValue() )
        bRet |= 0 != rSet.Put( SvxPrintItem( RES_PRINT, bChecked));

    // textflow
    if( aTextFlowLB.IsVisible() )
    {
        sal_uInt16 nPos = aTextFlowLB.GetSelectEntryPos();
        if( nPos != aTextFlowLB.GetSavedValue() )
        {
            nPos = (sal_uInt16)(long)aTextFlowLB.GetEntryData( nPos );
            bRet |= 0 != rSet.Put( SvxFrameDirectionItem(
                                    (SvxFrameDirection)nPos, RES_FRAMEDIR ));
        }
    }
    if(pWrtSh)
    {
        const SwFrmFmt* pFmt = pWrtSh->GetFlyFrmFmt();
        if (pFmt)
        {
            String sCurrentPrevChain, sCurrentNextChain;
            if(aPrevLB.GetSelectEntryPos())
                sCurrentPrevChain = aPrevLB.GetSelectEntry();
            if(aNextLB.GetSelectEntryPos())
                sCurrentNextChain = aNextLB.GetSelectEntry();
            const SwFmtChain &rChain = pFmt->GetChain();
            const SwFlyFrmFmt* pFlyFmt;
            String sNextChain, sPrevChain;
            if ((pFlyFmt = rChain.GetPrev()) != 0)
                sPrevChain = pFlyFmt->GetName();

            if ((pFlyFmt = rChain.GetNext()) != 0)
                sNextChain = pFlyFmt->GetName();
            if(sPrevChain != sCurrentPrevChain)
                bRet |= 0 != rSet.Put(SfxStringItem(FN_PARAM_CHAIN_PREVIOUS, sCurrentPrevChain));
            if(sNextChain != sCurrentNextChain)
                bRet |= 0 != rSet.Put(SfxStringItem(FN_PARAM_CHAIN_NEXT, sCurrentNextChain));
        }
    }
    return bRet;
}

/*-----------------13.11.96 15.05-------------------

--------------------------------------------------*/

USHORT* SwFrmAddPage::GetRanges()
{
    return aAddPgRg;
}

/*-----------------13.11.96 16.12-------------------

--------------------------------------------------*/

IMPL_LINK(SwFrmAddPage, EditModifyHdl, Edit*, EMPTYARG)
{
    BOOL bEnable = 0 != aNameED.GetText().Len();
    aAltNameED.Enable(bEnable);
    aAltNameFT.Enable(bEnable);

    return 0;
}
//-----------------------------------------------------------------------------
void	SwFrmAddPage::SetFormatUsed(BOOL bFmt)
{
    bFormat  = bFmt;
    if(bFormat)
    {
        aNameFT.Show(FALSE);
        aNameED.Show(FALSE);
        aAltNameFT.Show(FALSE);
        aAltNameED.Show(FALSE);
        aPrevFT.Show(FALSE);
        aPrevLB.Show(FALSE);
        aNextFT.Show(FALSE);
        aNextLB.Show(FALSE);
        aNamesFL.Show(FALSE);

        sal_Int32 nDiff = aExtFL.GetPosPixel().Y() - aNamesFL.GetPosPixel().Y();
        Window* aWindows[] =
        {
            &aProtectContentCB,
            &aProtectFrameCB,
            &aProtectSizeCB,
            &aProtectFL,
            &aEditInReadonlyCB,
            &aPrintFrameCB,
            &aExtFL,
            &aTextFlowFT,
            &aTextFlowLB,
            0
        };
        sal_Int32 nIdx = 0;
        while(aWindows[nIdx])
            lcl_Move(*aWindows[nIdx++], nDiff);
    }
}
/* -----------------------------08.08.2002 16:24------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SwFrmAddPage, ChainModifyHdl, ListBox*, pBox)
{
    String sCurrentPrevChain, sCurrentNextChain;
    if(aPrevLB.GetSelectEntryPos())
        sCurrentPrevChain = aPrevLB.GetSelectEntry();
    if(aNextLB.GetSelectEntryPos())
        sCurrentNextChain = aNextLB.GetSelectEntry();
    SwFrmFmt* pFmt = pWrtSh->GetFlyFrmFmt();
    if (pFmt)
    {
        BOOL bNextBox = &aNextLB == pBox;
        ListBox& rChangeLB = bNextBox ? aPrevLB : aNextLB;
        for(USHORT nEntry = rChangeLB.GetEntryCount(); nEntry > 1; nEntry--)
            rChangeLB.RemoveEntry(nEntry - 1);
        //determine chainable frames
        ::std::vector< String > aPrevPageFrames;
        ::std::vector< String > aThisPageFrames;
        ::std::vector< String > aNextPageFrames;
        ::std::vector< String > aRemainFrames;
        pWrtSh->GetConnectableFrmFmts(*pFmt, bNextBox ? sCurrentNextChain : sCurrentPrevChain, !bNextBox,
                        aPrevPageFrames, aThisPageFrames, aNextPageFrames, aRemainFrames );
        lcl_InsertVectors(rChangeLB,
                aPrevPageFrames, aThisPageFrames, aNextPageFrames, aRemainFrames);
        String sToSelect = bNextBox ? sCurrentPrevChain : sCurrentNextChain;
        if(rChangeLB.GetEntryPos(sToSelect) != LISTBOX_ENTRY_NOTFOUND)
            rChangeLB.SelectEntry(sToSelect);
        else
            rChangeLB.SelectEntryPos(0);

    }
    return 0;
}

