/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlitemm.cxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 10:12:36 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#include <hintids.hxx>

#ifndef _SVX_UNOMID_HXX
#include <svx/unomid.hxx>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLITMAP_HXX
#include "xmlitmap.hxx"
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif

#ifndef _UNOMID_H
#include <unomid.h>
#endif

using namespace ::xmloff::token;

#define MAP_ENTRY( p, l, w, m ) \
    { XML_NAMESPACE_##p, XML_##l, w, m }
#define M_E_SI( p, l, w, m ) \
    { XML_NAMESPACE_##p, XML_##l, w, MID_SW_FLAG_SPECIAL_ITEM_IMPORT|m }
#define M_E_SE( p, l, w, m ) \
    { XML_NAMESPACE_##p, XML_##l, w, MID_SW_FLAG_SPECIAL_ITEM_EXPORT|m }

#define M_END { 0, XML_TOKEN_INVALID, 0, 0 }

SvXMLItemMapEntry aXMLTableItemMap[] =
{
    // RES_FILL_ORDER
    // not required
    // RES_FRM_SIZE
    M_E_SE( STYLE,	WIDTH,			RES_FRM_SIZE,	MID_FRMSIZE_WIDTH ),
    M_E_SE( STYLE,	REL_WIDTH,		RES_FRM_SIZE,	MID_FRMSIZE_REL_WIDTH ),
    // RES_PAPER_BIN
    // not required
    // TODO: RES_LR_SPACE
    M_E_SE( FO,	MARGIN_LEFT,		RES_LR_SPACE,	MID_L_MARGIN ),
    M_E_SE( FO,	MARGIN_RIGHT,		RES_LR_SPACE,	MID_R_MARGIN ),
    // RES_UL_SPACE
    MAP_ENTRY( FO,   MARGIN_TOP,         RES_UL_SPACE,   MID_UP_MARGIN ),
    MAP_ENTRY( FO,   MARGIN_BOTTOM,      RES_UL_SPACE,   MID_LO_MARGIN ),
    // RES_PAGEDESC
    MAP_ENTRY( STYLE,    PAGE_NUMBER,        RES_PAGEDESC,   MID_PAGEDESC_PAGENUMOFFSET),
    // RES_BREAK
    MAP_ENTRY( FO,   BREAK_BEFORE,       RES_BREAK,  MID_BREAK_BEFORE ),
    MAP_ENTRY( FO,   BREAK_AFTER,        RES_BREAK,  MID_BREAK_AFTER ),
    // RES_CNTNT
    // not required
    // RES_HEADER
    // not required
    // RES_FOOTER
    // not required
    // RES_PRINT
    // not required
    // RES_OPAQUE
    // not required
    // RES_PROTECT
    // not required
    // RES_SURROUND
    // not required
    // RES_VERT_ORIENT
    // not required
    // RES_HORI_ORIENT
    MAP_ENTRY( TABLE,    ALIGN,                  RES_HORI_ORIENT, 0 ),
    // RES_ANCHOR
    // not required
    // RES_BACKGROUND
    MAP_ENTRY( FO,   BACKGROUND_COLOR,       RES_BACKGROUND, MID_BACK_COLOR ),
    MAP_ENTRY( STYLE,    BACKGROUND_IMAGE,       RES_BACKGROUND, MID_SW_FLAG_ELEMENT_ITEM ),
    // RES_BOX
    // not required
    // RES_SHADOW
    MAP_ENTRY( STYLE,    SHADOW,             RES_SHADOW, 0 ),
    // RES_FRMMACRO
    // not required
    // RES_COL
    // not required
    // RES_KEEP
    MAP_ENTRY( FO, KEEP_WITH_NEXT,       RES_KEEP, 0 ),
    // RES_URL
    // not required
    // RES_EDIT_IN_READONLY
    // not required
    // RES_LAYOUT_SPLIT
    MAP_ENTRY( STYLE,    MAY_BREAK_BETWEEN_ROWS, RES_LAYOUT_SPLIT, 0 ),
    // RES_CHAIN
    // not required
    // RES_LINENUMBER
    // not required
    // RES_FTN_AT_TXTEND
    // not required
    // RES_END_AT_TXTEND
    // not required
    // RES_UNKNOWNATR_CONTAINER
    M_E_SE( TEXT, XMLNS, RES_UNKNOWNATR_CONTAINER, 0 ),

    // RES_FRAMEDIR
    MAP_ENTRY( STYLE, WRITING_MODE, RES_FRAMEDIR, 0 ),

    // RES_COLLAPSING_BORDERS
    MAP_ENTRY( TABLE, BORDER_MODEL, RES_COLLAPSING_BORDERS, 0 ),

    M_END
};

SvXMLItemMapEntry aXMLTableColItemMap[] =
{
    M_E_SI( STYLE,	COLUMN_WIDTH,	  RES_FRM_SIZE,	MID_FRMSIZE_COL_WIDTH ),
    MAP_ENTRY( STYLE,        REL_COLUMN_WIDTH, RES_FRM_SIZE, MID_FRMSIZE_REL_COL_WIDTH ),
    M_END
};

SvXMLItemMapEntry aXMLTableRowItemMap[] =
{
    // RES_FILL_ORDER
    // not required
    // RES_FRM_SIZE
    MAP_ENTRY( STYLE,    ROW_HEIGHT,         RES_FRM_SIZE,   MID_FRMSIZE_FIX_HEIGHT ),
    MAP_ENTRY( STYLE,    MIN_ROW_HEIGHT,     RES_FRM_SIZE,   MID_FRMSIZE_MIN_HEIGHT ),
    // RES_PAPER_BIN
    // not required
    // RES_LR_SPACE
    // not required
    // RES_UL_SPACE
    // not required
    // RES_PAGEDESC
    // not required
    // RES_BREAK
    // not required
    // RES_CNTNT
    // not required
    // RES_HEADER
    // not required
    // RES_FOOTER
    // not required
    // RES_PRINT
    // not required
    // RES_OPAQUE
    // not required
    // RES_PROTECT
    // not required
    // RES_SURROUND
    // not required
    // RES_VERT_ORIENT
    // not required
    // RES_HORI_ORIENT
    // not required
    // RES_ANCHOR
    // not required
    // RES_BACKGROUND
    MAP_ENTRY( FO,   BACKGROUND_COLOR,       RES_BACKGROUND, MID_BACK_COLOR ),
    MAP_ENTRY( STYLE,    BACKGROUND_IMAGE,       RES_BACKGROUND, MID_SW_FLAG_ELEMENT_ITEM ),
    // RES_BOX
    // not required
    // RES_ANCHOR
    // not required
    // RES_SHADOW
    // not required
    // RES_FRMMACRO
    // not required
    // RES_COL
    // not required
    // RES_KEEP
    // not required
    // RES_URL
    // not required
    // RES_EDIT_IN_READONLY
    // not required
    // RES_LAYOUT_SPLIT
    M_E_SE( STYLE, KEEP_TOGETHER, RES_ROW_SPLIT, 0 ),
    M_E_SE( FO, KEEP_TOGETHER, RES_ROW_SPLIT, 0 ),
    // RES_CHAIN
    // not required
    // RES_LINENUMBER
    // not required
    // RES_FTN_AT_TXTEND
    // not required
    // RES_END_AT_TXTEND
    // not required
    // RES_UNKNOWNATR_CONTAINER
    M_E_SE( TEXT, XMLNS, RES_UNKNOWNATR_CONTAINER, 0 ),
    M_END
};

SvXMLItemMapEntry aXMLTableCellItemMap[] =
{
    // RES_FILL_ORDER
    // not required
    // RES_FRM_SIZE
    // not required
    // RES_PAPER_BIN
    // not required
    // RES_LR_SPACE
    // not required
    // RES_UL_SPACE
    // not required
    // RES_PAGEDESC
    // not required
    // RES_BREAK
    // not required
    // RES_CNTNT
    // not required
    // RES_HEADER
    // not required
    // RES_FOOTER
    // not required
    // RES_PRINT
    // not required
    // RES_OPAQUE
    // not required
    // RES_PROTECT
    // not required
    // RES_SURROUND
    // not required
    // RES_VERT_ORIENT
    MAP_ENTRY( STYLE,    VERTICAL_ALIGN,                 RES_VERT_ORIENT, 0 ),
    // RES_HORI_ORIENT
    // not required
    // RES_ANCHOR
    // not required
    // RES_BACKGROUND
    MAP_ENTRY( FO,   BACKGROUND_COLOR,       RES_BACKGROUND, MID_BACK_COLOR ),
    MAP_ENTRY( STYLE,    BACKGROUND_IMAGE,       RES_BACKGROUND, MID_SW_FLAG_ELEMENT_ITEM ),
    // RES_BOX
    MAP_ENTRY( STYLE,    BORDER_LINE_WIDTH,        RES_BOX, ALL_BORDER_LINE_WIDTH ),
    MAP_ENTRY( STYLE,    BORDER_LINE_WIDTH_LEFT,   RES_BOX, LEFT_BORDER_LINE_WIDTH ),
    MAP_ENTRY( STYLE,    BORDER_LINE_WIDTH_RIGHT,  RES_BOX, RIGHT_BORDER_LINE_WIDTH ),
    MAP_ENTRY( STYLE,    BORDER_LINE_WIDTH_TOP,    RES_BOX, TOP_BORDER_LINE_WIDTH ),
    MAP_ENTRY( STYLE,    BORDER_LINE_WIDTH_BOTTOM, RES_BOX, BOTTOM_BORDER_LINE_WIDTH ),
    MAP_ENTRY( FO,   PADDING,                  RES_BOX, ALL_BORDER_PADDING ),
    MAP_ENTRY( FO,   PADDING_LEFT,             RES_BOX, LEFT_BORDER_PADDING ),
    MAP_ENTRY( FO,   PADDING_RIGHT,            RES_BOX, RIGHT_BORDER_PADDING ),
    MAP_ENTRY( FO,   PADDING_TOP,              RES_BOX, TOP_BORDER_PADDING ),
    MAP_ENTRY( FO,   PADDING_BOTTOM,           RES_BOX, BOTTOM_BORDER_PADDING ),
    MAP_ENTRY( FO,   BORDER,                   RES_BOX, ALL_BORDER ),
    MAP_ENTRY( FO,   BORDER_LEFT,              RES_BOX, LEFT_BORDER ),
    MAP_ENTRY( FO,   BORDER_RIGHT,             RES_BOX, RIGHT_BORDER ),
    MAP_ENTRY( FO,   BORDER_TOP,               RES_BOX, TOP_BORDER ),
    MAP_ENTRY( FO,   BORDER_BOTTOM,            RES_BOX, BOTTOM_BORDER ),
    // RES_SHADOW
    // not required
    // RES_FRMMACRO
    // not required
    // RES_COL
    // not required
    // RES_KEEP
    // not required
    // RES_URL
    // not required
    // RES_EDIT_IN_READONLY
    // not required
    // RES_LAYOUT_SPLIT
    // not required
    // RES_CHAIN
    // not required
    // RES_LINENUMBER
    // not required
    // RES_FTN_AT_TXTEND
    // not required
    // RES_END_AT_TXTEND
    // not required
    // RES_UNKNOWNATR_CONTAINER
    M_E_SE( TEXT, XMLNS, RES_UNKNOWNATR_CONTAINER, 0 ),

    // RES_FRAMEDIR
    MAP_ENTRY( STYLE, WRITING_MODE, RES_FRAMEDIR, 0 ),

    M_END
};
