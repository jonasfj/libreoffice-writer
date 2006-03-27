/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unoprnms.hxx,v $
 *
 *  $Revision: 1.114 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-27 09:22:09 $
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
#ifndef _UNOPRNMS_HXX
#define _UNOPRNMS_HXX


#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif

enum SwPropNameIds
{
/* 0001 */	SW_PROPNAME_BEGIN = 1,

/* 0001 */	UNO_NAME_FOLLOW_STYLE = SW_PROPNAME_BEGIN,
/* 0002 */	UNO_NAME_IS_PHYSICAL,
/* 0003 */	UNO_NAME_IS_AUTO_UPDATE,
/* 0004 */	UNO_NAME_DISPLAY_NAME,
/* 0005 */	UNO_NAME_PARA_GRAPHIC_URL,
/* 0006 */	UNO_NAME_PARA_GRAPHIC_FILTER,
/* 0007 */	UNO_NAME_HEADER_GRAPHIC_URL,
/* 0008 */	UNO_NAME_HEADER_GRAPHIC_FILTER,
/* 0009 */	UNO_NAME_FOOTER_GRAPHIC_URL,
/* 0010 */	UNO_NAME_FOOTER_GRAPHIC_FILTER,
/* 0011 */	UNO_NAME_BACK_GRAPHIC_URL,
/* 0012 */	UNO_NAME_BACK_GRAPHIC_FILTER,
/* 0013 */	UNO_NAME_BACK_GRAPHIC_LOCATION,
/* 0014 */	UNO_NAME_BACK_GRAPHIC_BITMAP,
/* 0015 */	UNO_NAME_GRAPHIC_URL,
/* 0016 */	UNO_NAME_GRAPHIC_FILTER,
/* 0017 */	UNO_NAME_GRAPHIC_LOCATION,
/* 0018 */	UNO_NAME_GRAPHIC_SIZE,
/* 0019 */	UNO_NAME_GRAPHIC_BITMAP,
/* 0020 */	UNO_NAME_BULLET_FONT,
/* 0021 */	UNO_NAME_PARA_GRAPHIC_LOCATION,
/* 0022 */	UNO_NAME_HEADER_GRAPHIC_LOCATION,
/* 0023 */	UNO_NAME_FOOTER_GRAPHIC_LOCATION,
/* 0024 */	UNO_NAME_PARA_LEFT_PARA_MARGIN,
/* 0025 */	UNO_NAME_PARA_RIGHT_PARA_MARGIN,
/* 0026 */	UNO_NAME_PARA_LEFT_MARGIN,
/* 0027 */	UNO_NAME_PARA_RIGHT_MARGIN,
/* 0028 */	UNO_NAME_PARA_LEFT_MARGIN_RELATIVE,
/* 0029 */	UNO_NAME_PARA_RIGHT_MARGIN_RELATIVE,
/* 0030 */	UNO_NAME_PARA_IS_AUTO_FIRST_LINE_INDENT,
/* 0031 */	UNO_NAME_PARA_FIRST_LINE_INDENT,
/* 0032 */	UNO_NAME_PARA_FIRST_LINE_INDENT_RELATIVE,
/* 0033 */	UNO_NAME_PARA_IS_HYPHENATION,
/* 0034 */	UNO_NAME_PARA_HYPHENATION_MAX_LEADING_CHARS,
/* 0035 */	UNO_NAME_PARA_HYPHENATION_MAX_TRAILING_CHARS,
/* 0036 */	UNO_NAME_PARA_HYPHENATION_MAX_HYPHENS,
/* 0037 */	UNO_NAME_LEFT_MARGIN,
/* 0038 */	UNO_NAME_RIGHT_MARGIN,
/* 0039 */	UNO_NAME_HEADER_LEFT_MARGIN,
/* 0040 */	UNO_NAME_HEADER_RIGHT_MARGIN,
/* 0041 */	UNO_NAME_FOOTER_LEFT_MARGIN,
/* 0042 */	UNO_NAME_FOOTER_RIGHT_MARGIN,
/* 0043 */	UNO_NAME_TEXT_RANGE,
/* 0044 */	UNO_NAME_NAME,
/* 0045 */	UNO_NAME_NUMBERING_ALIGNMENT,
/* 0046 */	UNO_NAME_BULLET_FONT_NAME,
/* 0047 */	UNO_NAME_BULLET_ID,
/* 0048 */	UNO_NAME_CHAR_STYLE_NAME,
/* 0049 */	UNO_NAME_ANCHOR_CHAR_STYLE_NAME,
/* 0050 */	UNO_NAME_SUFFIX,
/* 0051 */	UNO_NAME_PREFIX,
/* 0052 */	UNO_NAME_PARENT_NUMBERING,
/* 0053 */	UNO_NAME_START_WITH,
/* 0054 */	UNO_NAME_CHAR_FONT_NAME,
/* 0055 */	UNO_NAME_CHAR_FONT_STYLE_NAME,
/* 0056 */	UNO_NAME_CHAR_FONT_FAMILY,
/* 0057 */	UNO_NAME_CHAR_FONT_CHAR_SET,
/* 0058 */	UNO_NAME_CHAR_FONT_PITCH,
/* 0059 */	UNO_NAME_CHAR_POSTURE,
/* 0060 */	UNO_NAME_CHAR_WEIGHT,
/* 0061 */	UNO_NAME_CHAR_HEIGHT,
/* 0062 */	UNO_NAME_CHAR_LOCALE,
/* 0063 */	UNO_NAME_CHAR_FONT_NAME_ASIAN,
/* 0064 */	UNO_NAME_CHAR_FONT_STYLE_NAME_ASIAN,
/* 0065 */	UNO_NAME_CHAR_FONT_FAMILY_ASIAN,
/* 0066 */	UNO_NAME_CHAR_FONT_CHAR_SET_ASIAN,
/* 0067 */	UNO_NAME_CHAR_FONT_PITCH_ASIAN,
/* 0068 */	UNO_NAME_CHAR_POSTURE_ASIAN,
/* 0069 */	UNO_NAME_CHAR_WEIGHT_ASIAN,
/* 0070 */	UNO_NAME_CHAR_HEIGHT_ASIAN,
/* 0071 */	UNO_NAME_CHAR_LOCALE_ASIAN,
/* 0072 */	UNO_NAME_CHAR_FONT_NAME_COMPLEX,
/* 0073 */	UNO_NAME_CHAR_FONT_STYLE_NAME_COMPLEX,
/* 0074 */	UNO_NAME_CHAR_FONT_FAMILY_COMPLEX,
/* 0075 */	UNO_NAME_CHAR_FONT_CHAR_SET_COMPLEX,
/* 0076 */	UNO_NAME_CHAR_FONT_PITCH_COMPLEX,
/* 0077 */	UNO_NAME_CHAR_POSTURE_COMPLEX,
/* 0078 */	UNO_NAME_CHAR_WEIGHT_COMPLEX,
/* 0079 */	UNO_NAME_CHAR_HEIGHT_COMPLEX,
/* 0080 */	UNO_NAME_CHAR_LOCALE_COMPLEX,
/* 0081 */	UNO_NAME_CHAR_AUTO_KERNING,
/* 0082 */	UNO_NAME_CHAR_UNDERLINE,
/* 0083 */	UNO_NAME_CHAR_UNDERLINE_COLOR,
/* 0084 */	UNO_NAME_CHAR_UNDERLINE_HAS_COLOR,
/* 0085 */	UNO_NAME_CHAR_ESCAPEMENT,
/* 0086 */	UNO_NAME_CHAR_CASE_MAP,
/* 0087 */	UNO_NAME_CHAR_SHADOWED,
/* 0088 */	UNO_NAME_CHAR_STRIKEOUT,
/* 0089 */	UNO_NAME_CHAR_CROSSED_OUT,
/* 0090 */	UNO_NAME_CHAR_NO_HYPHENATION,
/* 0091 */	UNO_NAME_CHAR_AUTO_ESCAPEMENT,
/* 0092 */	UNO_NAME_CHAR_PROP_HEIGHT,
/* 0093 */	UNO_NAME_CHAR_DIFF_HEIGHT,
/* 0094 */	UNO_NAME_CHAR_PROP_HEIGHT_ASIAN,
/* 0095 */	UNO_NAME_CHAR_DIFF_HEIGHT_ASIAN,
/* 0096 */	UNO_NAME_CHAR_PROP_HEIGHT_COMPLEX,
/* 0097 */	UNO_NAME_CHAR_DIFF_HEIGHT_COMPLEX,
/* 0098 */	UNO_NAME_CHAR_ESCAPEMENT_HEIGHT,
/* 0099 */	UNO_NAME_CHAR_COLOR,

/* 0100 */	UNO_NAME_CHAR_FLASH,
/* 0101 */	UNO_NAME_CHAR_KERNING,
/* 0102 */	UNO_NAME_CHAR_BACK_COLOR,
/* 0103 */	UNO_NAME_CHAR_BACK_TRANSPARENT,
/* 0104 */	UNO_NAME_CHAR_COMBINE_IS_ON,
/* 0105 */	UNO_NAME_CHAR_COMBINE_PREFIX,
/* 0106 */	UNO_NAME_CHAR_COMBINE_SUFFIX,
/* 0107 */	UNO_NAME_CHAR_EMPHASIS,
/* 0108 */	UNO_NAME_PARA_LINE_SPACING,
/* 0109 */	UNO_NAME_PARA_TOP_MARGIN,
/* 0110 */	UNO_NAME_PARA_BOTTOM_MARGIN,
/* 0111 */	UNO_NAME_PARA_TOP_MARGIN_RELATIVE,
/* 0112 */	UNO_NAME_PARA_BOTTOM_MARGIN_RELATIVE,
/* 0113 */	UNO_NAME_PARA_EXPAND_SINGLE_WORD,
/* 0114 */	UNO_NAME_END_NOTICE,
/* 0115 */	UNO_NAME_EMBEDDED_OBJECTS,
/* 0116 */	UNO_NAME_ALPHABETICAL_SEPARATORS,
/* 0117 */	UNO_NAME_BACKGROUND_COLOR,
/* 0118 */	UNO_NAME_BEGIN_NOTICE,
/* 0119 */	UNO_NAME_CASE_SENSITIVE,
/* 0120 */	UNO_NAME_FRAME_STYLE_NAME,
/* 0121 */	UNO_NAME_NUMBERING_STYLE_NAME,
/* 0122 */	UNO_NAME_NUMBERING_LEVEL,
/* 0123 */	UNO_NAME_NUMBERING_START_VALUE,
/* 0124 */	UNO_NAME_NUMBERING_IS_NUMBER,
/* 0125 */	UNO_NAME_COMBINE_ENTRIES,
/* 0126 */	UNO_NAME_COUNT_LINES_IN_FRAMES,
/* 0127 */	UNO_NAME_DDE_COMMAND_TYPE,
/* 0128 */	UNO_NAME_DDE_COMMAND_FILE,
/* 0129 */	UNO_NAME_DDE_COMMAND_ELEMENT,
/* 0130 */	UNO_NAME_IS_AUTOMATIC_UPDATE,
/* 0131 */	UNO_NAME_DEFAULT_TABSTOP_DISTANCE,
/* 0132 */	UNO_NAME_DISTANCE,
/* 0133 */	UNO_NAME_DROP_CAP_FORMAT,
/* 0134 */	UNO_NAME_DROP_CAP_WHOLE_WORD,
/* 0135 */	UNO_NAME_DROP_CAP_CHAR_STYLE_NAME,
/* 0136 */	UNO_NAME_FILE_LINK,
/* 0137 */	UNO_NAME_GRAPHIC,
/* 0138 */	UNO_NAME_GRAPHICS,
/* 0139 */	UNO_NAME_IS_PROTECTED,
/* 0140 */	UNO_NAME_KEY_AS_ENTRY,
/* 0141 */	UNO_NAME_PARA_KEEP_TOGETHER,
/* 0142 */	UNO_NAME_KEEP_TOGETHER,
/* 0143 */	UNO_NAME_IS_LANDSCAPE,
/* 0144 */	UNO_NAME_SEPARATOR_TEXT,
/* 0145 */	UNO_NAME_MARKS,
/* 0146 */	UNO_NAME_NUMBER_POSITION,
/* 0147 */	UNO_NAME_OUTLINES,
/* 0148 */	UNO_NAME_PAGE_STYLE_NAME,
/* 0149 */	UNO_NAME_PAGE_STYLE_LAYOUT,
/* 0150 */	UNO_NAME_PARA_STYLES,
/* 0151 */	UNO_NAME_PARA_ADJUST,
/* 0152 */	UNO_NAME_PARA_REGISTER_MODE_ACTIVE,
/* 0153 */	UNO_NAME_PARA_STYLE_NAME,
/* 0154 */	UNO_NAME_PARA_LAST_LINE_ADJUST,
/* 0155 */	UNO_NAME_PARA_LINE_NUMBER_COUNT,
/* 0156 */	UNO_NAME_PARA_LINE_NUMBER_START_VALUE,
/* 0157 */	UNO_NAME_BACK_COLOR,
/* 0158 */	UNO_NAME_PARA_BACK_COLOR,
/* 0159 */	UNO_NAME_PARA_WIDOWS,
/* 0160 */	UNO_NAME_PARA_ORPHANS,
/* 0161 */	UNO_NAME_PARA_BACK_TRANSPARENT,
/* 0162 */	UNO_NAME_POSITION_END_OF_DOC,
/* 0163 */	UNO_NAME_POSITION_PROTECTED,
/* 0164 */	UNO_NAME_ALTERNATIVE_TEXT,
/* 0165 */	UNO_NAME_PRIMARY_KEY,
/* 0166 */	UNO_NAME_PRINT_TABLES,
/* 0167 */	UNO_NAME_PRINT_GRAPHICS,
/* 0168 */	UNO_NAME_PRINT_REVERSED,
/* 0169 */	UNO_NAME_PRINT_PROSPECT,
/* 0170 */	UNO_NAME_PRINT_CONTROLS,
/* 0171 */	UNO_NAME_PRINT_DRAWINGS,
/* 0172 */	UNO_NAME_PRINT_RIGHT_PAGES,
/* 0173 */	UNO_NAME_PRINT_LEFT_PAGES,
/* 0174 */	UNO_NAME_PRINT_BLACK_FONTS,
/* 0175 */	UNO_NAME_PRINTER_PAPER_TRAY,
/* 0176 */	UNO_NAME_PRINT_PAGE_BACKGROUND,
/* 0177 */	UNO_NAME_PRINT_ANNOTATION_MODE,
/* 0178 */	UNO_NAME_PRINT_FAX_NAME,
/* 0179 */	UNO_NAME_PRINT_PAPER_FROM_SETUP,
/* 0180 */	UNO_NAME_REGISTER_MODE_ACTIVE,
/* 0181 */	UNO_NAME_RELATIVE_WIDTH,
/* 0182 */	UNO_NAME_RELATIVE_HEIGHT,
/* 0183 */	UNO_NAME_REPEAT_HEADLINE,
/* 0184 */	UNO_NAME_SEARCH_STYLES,
/* 0185 */	UNO_NAME_SEARCH_BACKWARDS,
/* 0186 */	UNO_NAME_SEARCH_SIMILARITY,
/* 0187 */	UNO_NAME_SEARCH_IN_SELECTION,
/* 0188 */	UNO_NAME_SEARCH_CASE_SENSITIVE,
/* 0189 */	UNO_NAME_SEARCH_SIMILARITY_ADD,
/* 0190 */	UNO_NAME_SEARCH_SIMILARITY_RELAX,
/* 0191 */	UNO_NAME_SEARCH_SIMILARITY_REMOVE,
/* 0192 */	UNO_NAME_SEARCH_REGULAR_EXPRESSION,
/* 0193 */	UNO_NAME_SEARCH_SIMILARITY_EXCHANGE,
/* 0194 */	UNO_NAME_SECONDARY_KEY,
/* 0195 */	UNO_NAME_SEPARATOR_INTERVAL,
/* 0196 */	UNO_NAME_SHOW_BREAKS,
/* 0197 */	UNO_NAME_SHOW_SPACES,
/* 0198 */	UNO_NAME_SHOW_TABLES,
/* 0199 */	UNO_NAME_SHOW_GRAPHICS,

/* 0200 */	UNO_NAME_SHOW_DRAWINGS,
/* 0201 */	UNO_NAME_SHOW_TABSTOPS,
/* 0202 */	UNO_NAME_SHOW_VERT_RULER,
/* 0203 */	UNO_NAME_SHOW_PARA_BREAKS,
/* 0204 */	UNO_NAME_SHOW_HIDDEN_TEXT,
/* 0205 */	UNO_NAME_SHOW_ANNOTATIONS,
/* 0206 */	UNO_NAME_SHOW_SOFT_HYPHENS,
/* 0207 */	UNO_NAME_SHOW_VERT_SCROLL_BAR,
/* 0208 */	UNO_NAME_SHOW_HORI_SCROLL_BAR,
/* 0209 */	UNO_NAME_SHOW_FIELD_COMMANDS,
/* 0210 */	UNO_NAME_SHOW_TEXT_BOUNDARIES,
/* 0211 */	UNO_NAME_SHOW_PROTECTED_SPACES,
/* 0212 */	UNO_NAME_SHOW_TABLE_BOUNDARIES,
/* 0213 */	UNO_NAME_SHOW_HIDDEN_PARAGRAPHS,
/* 0214 */	UNO_NAME_SHOW_INDEX_MARK_BACKGROUND,
/* 0215 */	UNO_NAME_SHOW_FOOTNOTE_BACKGROUND,
/* 0216 */	UNO_NAME_SHOW_TEXT_FIELD_BACKGROUND,
/* 0217 */	UNO_NAME_IS_SYNC_WIDTH_TO_HEIGHT,
/* 0218 */	UNO_NAME_IS_SYNC_HEIGHT_TO_WIDTH,
/* 0219 */	UNO_NAME_SIZE_PROTECTED,
/* 0220 */	UNO_NAME_SMOOTH_SCROLLING,
/* 0221 */	UNO_NAME_SOLID_MARK_HANDLES,
/* 0222 */	UNO_NAME_TABLES,
/* 0223 */	UNO_NAME_TEXT_FRAMES,
/* 0224 */	UNO_NAME_TEXT_COLUMNS,
/* 0225 */	UNO_NAME_BACK_TRANSPARENT,
/* 0226 */	UNO_NAME_USE_PP,
/* 0227 */	UNO_NAME_USER_METRIC,
/* 0228 */	UNO_NAME_ANCHOR_TYPE,
/* 0229 */	UNO_NAME_ANCHOR_TYPES,
/* 0230 */	UNO_NAME_ANCHOR_PAGE_NO,
/* 0231 */	UNO_NAME_ANCHOR_FRAME,
/* 0232 */	UNO_NAME_AUTHOR,
/* 0233 */	UNO_NAME_BREAK_TYPE,
/* 0234 */	UNO_NAME_CHAIN_NEXT_NAME,
/* 0235 */	UNO_NAME_CHAIN_PREV_NAME,
/* 0236 */	UNO_NAME_CHAPTER_FORMAT,
/* 0237 */	UNO_NAME_CLIENT_MAP,
/* 0238 */	UNO_NAME_CONDITION,
/* 0239 */	UNO_NAME_CONTENT,
/* 0240 */	UNO_NAME_CHAR_CONTOURED,
/* 0241 */	UNO_NAME_CONTOUR_OUTSIDE,
/* 0242 */	UNO_NAME_CONTENT_PROTECTED,
/* 0243 */	UNO_NAME_COUNT_EMPTY_LINES,
/* 0244 */	UNO_NAME_RESTART_AT_EACH_PAGE,
/* 0245 */	UNO_NAME_DATA_BASE_NAME,
/* 0246 */	UNO_NAME_DATA_TABLE_NAME,
/* 0247 */	UNO_NAME_DATA_COMMAND_TYPE,
/* 0248 */	UNO_NAME_DATA_COLUMN_NAME,
/* 0249 */	UNO_NAME_IS_DATA_BASE_FORMAT,
/* 0250 */	UNO_NAME_DATE,
/* 0251 */	UNO_NAME_IS_DATE,
/* 0252 */	UNO_NAME_EDIT_IN_READONLY,
/* 0253 */	UNO_NAME_FALSE_CONTENT,
/* 0254 */	UNO_NAME_FILE_FORMAT,
/* 0255 */	UNO_NAME_IS_FIXED,
/* 0256 */	UNO_NAME_FOOTNOTE_COUNTING,
/* 0257 */	UNO_NAME_FORMULA,
/* 0258 */	UNO_NAME_FRAME_NAME,
/* 0259 */	UNO_NAME_GRAPHIC_NAME,
/* 0260 */	UNO_NAME_FULL_NAME,
/* 0261 */	UNO_NAME_HEIGHT,
/* 0262 */	UNO_NAME_IS_AUTO_HEIGHT,
/* 0263 */	UNO_NAME_SIZE_TYPE,
/* 0264 */	UNO_NAME_HINT,
/* 0265 */	UNO_NAME_HORI_ORIENT,
/* 0266 */	UNO_NAME_HORI_MIRRORED_ON_EVEN_PAGES,
/* 0267 */	UNO_NAME_HORI_MIRRORED_ON_ODD_PAGES,
/* 0268 */	UNO_NAME_HORI_ORIENT_RELATION,
/* 0269 */	UNO_NAME_HORI_ORIENT_POSITION,
/* 0270 */	UNO_NAME_HYPER_LINK_U_R_L,
/* 0271 */	UNO_NAME_HYPER_LINK_TARGET,
/* 0272 */	UNO_NAME_HYPER_LINK_NAME,
/* 0273 */	UNO_NAME_HYPER_LINK_EVENTS,
/* 0274 */	UNO_NAME_INFO_TYPE,
/* 0275 */	UNO_NAME_INFO_FORMAT,
/* 0276 */	UNO_NAME_IS_INPUT,
/* 0277 */	UNO_NAME_LEVEL,
/* 0278 */	UNO_NAME_INTERVAL,
/* 0279 */	UNO_NAME_LINK_REGION,
/* 0280 */	UNO_NAME_MACRO_NAME,
/* 0281 */	UNO_NAME_SPLIT,
/* 0282 */	UNO_NAME_PARA_SPLIT,
/* 0283 */	UNO_NAME_NUMBER_FORMAT,
/* 0284 */	UNO_NAME_NUMBERING_TYPE,
/* 0285 */	UNO_NAME_NUMBERING_RULES,
/* 0286 */	UNO_NAME_OFFSET,
/* 0287 */	UNO_NAME_ON,
/* 0288 */	UNO_NAME_OPAQUE,
/* 0289 */	UNO_NAME_PAGE_TOGGLE,
/* 0290 */	UNO_NAME_PAGE_DESC_NAME,
/* 0291 */	UNO_NAME_PAGE_NUMBER_OFFSET,
/* 0292 */	UNO_NAME_PLACEHOLDER,
/* 0293 */	UNO_NAME_PLACEHOLDER_TYPE,
/* 0294 */	UNO_NAME_PRINT,
/* 0295 */	UNO_NAME_REFERENCE_FIELD_PART,
/* 0296 */	UNO_NAME_REFERENCE_FIELD_SOURCE,
/* 0297 */	UNO_NAME_REGISTER_PARAGRAPH_STYLE,
/* 0298 */	UNO_NAME_SCRIPT_TYPE,
/* 0299 */	UNO_NAME_SEARCH_ALL,

/* 0300 */	UNO_NAME_SEARCH_WORDS,
/* 0301 */	UNO_NAME_SEQUENCE_VALUE,
/* 0302 */	UNO_NAME_SERVER_MAP,
/* 0303 */	UNO_NAME_IMAGE_MAP,
/* 0304 */	UNO_NAME_SET_NUMBER,
/* 0305 */	UNO_NAME_SHADOW_FORMAT,
/* 0306 */	UNO_NAME_SHOW_HORI_RULER,
/* 0307 */	UNO_NAME_SIZE,
/* 0308 */	UNO_NAME_ACTUAL_SIZE,
/* 0309 */	UNO_NAME_SOURCE_NAME,
/* 0310 */	UNO_NAME_START_AT,
/* 0311 */	UNO_NAME_STATISTIC_TYPE_ID,
/* 0312 */	UNO_NAME_SUB_TYPE,
/* 0313 */	UNO_NAME_SURROUND,
/* 0314 */	UNO_NAME_IS_EXPRESSION,
/* 0315 */	UNO_NAME_IS_SHOW_FORMULA,
/* 0316 */	UNO_NAME_TEXT_WRAP,
/* 0317 */	UNO_NAME_SURROUND_CONTOUR,
/* 0318 */	UNO_NAME_SURROUND_ANCHORONLY,
/* 0319 */	UNO_NAME_TABLE_NAME,
/* 0320 */	UNO_NAME_TABSTOPS,
/* 0321 */	UNO_NAME_TITLE,
/* 0322 */	UNO_NAME_TOP_MARGIN,
/* 0323 */	UNO_NAME_BOTTOM_MARGIN,
/* 0324 */	UNO_NAME_TRUE_CONTENT,
/* 0325 */	UNO_NAME_URL_CONTENT,
/* 0326 */	UNO_NAME_USERTEXT,
/* 0327 */	UNO_NAME_USER_DATA_TYPE,
/* 0328 */	UNO_NAME_VALUE,
/* 0329 */	UNO_NAME_VARIABLE_NAME,
/* 0330 */	UNO_NAME_VARIABLE_SUBTYPE,
/* 0331 */  UNO_NAME_VERT_ORIENT,
/* 0332 */	UNO_NAME_VERT_MIRRORED,
/* 0333 */	UNO_NAME_VERT_ORIENT_POSITION,
/* 0334 */	UNO_NAME_VERT_ORIENT_RELATION,
/* 0335 */	UNO_NAME_IS_VISIBLE,
/* 0336 */	UNO_NAME_WIDTH,
/* 0337 */	UNO_NAME_CHAR_WORD_MODE,
/* 0338 */	UNO_NAME_GRAPHIC_CROP,
/* 0339 */	UNO_NAME_CHARACTER_FORMAT_NONE,
/* 0340 */	UNO_NAME_TEXT_POSITION,
/* 0341 */	UNO_NAME_DOCUMENT_INDEX_MARK,
/* 0342 */	UNO_NAME_DOCUMENT_INDEX,
/* 0343 */	UNO_NAME_IS_GLOBAL_DOCUMENT_SECTION,
/* 0344 */	UNO_NAME_TEXT_FIELD,
/* 0345 */	UNO_NAME_BOOKMARK,
/* 0346 */	UNO_NAME_TEXT_TABLE,
/* 0347 */	UNO_NAME_CELL,
/* 0348 */	UNO_NAME_TEXT_FRAME,
/* 0349 */	UNO_NAME_REFERENCE_MARK,
/* 0350 */	UNO_NAME_TEXT_SECTION,
/* 0351 */	UNO_NAME_FOOTNOTE,
/* 0352 */	UNO_NAME_ENDNOTE,
/* 0353 */	UNO_NAME_CHART_ROW_AS_LABEL,
/* 0354 */	UNO_NAME_CHART_COLUMN_AS_LABEL,
/* 0355 */	UNO_NAME_TABLE_COLUMS,
/* 0356 */	UNO_NAME_LEFT_BORDER,
/* 0357 */	UNO_NAME_RIGHT_BORDER,
/* 0358 */	UNO_NAME_TOP_BORDER,
/* 0359 */	UNO_NAME_BOTTOM_BORDER,
/* 0360 */	UNO_NAME_BORDER_DISTANCE,
/* 0361 */	UNO_NAME_LEFT_BORDER_DISTANCE,
/* 0362 */	UNO_NAME_RIGHT_BORDER_DISTANCE,
/* 0363 */	UNO_NAME_TOP_BORDER_DISTANCE,
/* 0364 */	UNO_NAME_BOTTOM_BORDER_DISTANCE,
/* 0365 */	UNO_NAME_TABLE_BORDER,
/* 0366 */	UNO_NAME_TABLE_COLUMN_SEPARATORS,
/* 0367 */	UNO_NAME_TABLE_COLUMN_RELATIVE_SUM,
/* 0368 */	UNO_NAME_HEADER_TEXT,
/* 0369 */	UNO_NAME_HEADER_TEXT_LEFT,
/* 0370 */	UNO_NAME_HEADER_TEXT_RIGHT,
/* 0371 */	UNO_NAME_FOOTER_TEXT,
/* 0372 */	UNO_NAME_FOOTER_TEXT_LEFT,
/* 0373 */	UNO_NAME_FOOTER_TEXT_RIGHT,
/* 0374 */	UNO_NAME_HEADER_BACK_COLOR,
/* 0375 */	UNO_NAME_HEADER_GRAPHIC,
/* 0376 */	UNO_NAME_HEADER_BACK_TRANSPARENT,
/* 0377 */	UNO_NAME_HEADER_LEFT_BORDER,
/* 0378 */	UNO_NAME_HEADER_RIGHT_BORDER,
/* 0379 */	UNO_NAME_HEADER_TOP_BORDER,
/* 0380 */	UNO_NAME_HEADER_BOTTOM_BORDER,
/* 0381 */	UNO_NAME_HEADER_BORDER_DISTANCE,
/* 0382 */	UNO_NAME_HEADER_SHADOW_FORMAT,
/* 0383 */	UNO_NAME_HEADER_BODY_DISTANCE,
/* 0384 */	UNO_NAME_HEADER_IS_DYNAMIC_HEIGHT,
/* 0385 */	UNO_NAME_HEADER_IS_SHARED,
/* 0386 */	UNO_NAME_HEADER_HEIGHT,
/* 0387 */	UNO_NAME_HEADER_IS_ON,
/* 0388 */	UNO_NAME_FOOTER_BACK_COLOR,
/* 0389 */	UNO_NAME_FOOTER_GRAPHIC,
/* 0390 */	UNO_NAME_FOOTER_BACK_TRANSPARENT,
/* 0391 */	UNO_NAME_FOOTER_LEFT_BORDER,
/* 0392 */	UNO_NAME_FOOTER_RIGHT_BORDER,
/* 0393 */	UNO_NAME_FOOTER_TOP_BORDER,
/* 0394 */	UNO_NAME_FOOTER_BOTTOM_BORDER,
/* 0395 */	UNO_NAME_FOOTER_BORDER_DISTANCE,
/* 0396 */	UNO_NAME_FOOTER_SHADOW_FORMAT,
/* 0397 */	UNO_NAME_FOOTER_BODY_DISTANCE,
/* 0398 */	UNO_NAME_FOOTER_IS_DYNAMIC_HEIGHT,
/* 0399 */	UNO_NAME_FOOTER_IS_SHARED,

/* 0400 */	UNO_NAME_FOOTER_HEIGHT,
/* 0401 */	UNO_NAME_FOOTER_IS_ON,
/* 0402 */	UNO_NAME_OVERWRITE_STYLES,
/* 0403 */	UNO_NAME_LOAD_NUMBERING_STYLES,
/* 0404 */	UNO_NAME_LOAD_PAGE_STYLES,
/* 0405 */	UNO_NAME_LOAD_FRAME_STYLES,
/* 0406 */	UNO_NAME_LOAD_TEXT_STYLES,
/* 0407 */	UNO_NAME_FILE_NAME,
/* 0408 */	UNO_NAME_FILTER_NAME,
/* 0409 */	UNO_NAME_FILTER_OPTION,
/* 0410 */	UNO_NAME_PASSWORD,
/* 0411 */	UNO_NAME_COPY_COUNT,
/* 0412 */	UNO_NAME_COLLATE,
/* 0413 */	UNO_NAME_SORT,
/* 0414 */	UNO_NAME_PAGES,
/* 0415 */	UNO_NAME_FIRST_LINE_OFFSET,
/* 0416 */	UNO_NAME_SYMBOL_TEXT_DISTANCE,
/* 0417 */	UNO_NAME_USER_INDEX_NAME,
/* 0418 */	UNO_NAME_REVISION,
/* 0419 */	UNO_NAME_UNVISITED_CHAR_STYLE_NAME,
/* 0420 */	UNO_NAME_VISITED_CHAR_STYLE_NAME,
/* 0421 */	UNO_NAME_PARAGRAPH_COUNT,
/* 0422 */	UNO_NAME_WORD_COUNT,
/* 0423 */	UNO_NAME_WORD_SEPARATOR,
/* 0424 */	UNO_NAME_CHARACTER_COUNT,
/* 0425 */	UNO_NAME_ZOOM_VALUE,
/* 0426 */	UNO_NAME_ZOOM_TYPE,
/* 0427 */	UNO_NAME_CREATE_FROM_MARKS,
/* 0428 */	UNO_NAME_CREATE_FROM_OUTLINE,
/* 0429 */	UNO_NAME_PARAGRAPH_STYLE_NAMES,
/* 0430 */	UNO_NAME_CREATE_FROM_CHAPTER,
/* 0431 */	UNO_NAME_CREATE_FROM_LABELS,
/* 0432 */	UNO_NAME_USE_ALPHABETICAL_SEPARATORS,
/* 0433 */	UNO_NAME_USE_KEY_AS_ENTRY,
/* 0434 */	UNO_NAME_USE_COMBINED_ENTRIES,
/* 0435 */	UNO_NAME_IS_CASE_SENSITIVE,
/* 0436 */	UNO_NAME_USE_P_P,
/* 0437 */	UNO_NAME_USE_DASH,
/* 0438 */	UNO_NAME_USE_UPPER_CASE,
/* 0439 */	UNO_NAME_LABEL_CATEGORY,
/* 0440 */	UNO_NAME_LABEL_DISPLAY_TYPE,
/* 0441 */	UNO_NAME_USE_LEVEL_FROM_SOURCE,
/* 0442 */	UNO_NAME_LEVEL_FORMAT,
/* 0443 */	UNO_NAME_LEVEL_PARAGRAPH_STYLES,
/* 0444 */	UNO_NAME_RECALC_TAB_STOPS,
/* 0445 */	UNO_NAME_MAIN_ENTRY_CHARACTER_STYLE_NAME,
/* 0446 */	UNO_NAME_CREATE_FROM_TABLES,
/* 0447 */	UNO_NAME_CREATE_FROM_TEXT_FRAMES,
/* 0448 */	UNO_NAME_CREATE_FROM_GRAPHIC_OBJECTS,
/* 0449 */	UNO_NAME_CREATE_FROM_EMBEDDED_OBJECTS,
/* 0450 */	UNO_NAME_CREATE_FROM_STAR_MATH,
/* 0451 */	UNO_NAME_CREATE_FROM_STAR_CHART,
/* 0452 */	UNO_NAME_CREATE_FROM_STAR_CALC,
/* 0453 */	UNO_NAME_CREATE_FROM_STAR_DRAW,
/* 0454 */	UNO_NAME_CREATE_FROM_OTHER_EMBEDDED_OBJECTS,
/* 0455 */	UNO_NAME_INDEX_AUTO_MARK_FILE_U_R_L,
/* 0456 */	UNO_NAME_IS_COMMA_SEPARATED,
/* 0457 */	UNO_NAME_IS_RELATIVE_TABSTOPS,
/* 0458 */	UNO_NAME_CREATE_FROM_LEVEL_PARAGRAPH_STYLES,
/* 0459 */	UNO_NAME_SHOW_CHANGES,
/* 0460 */	UNO_NAME_RECORD_CHANGES,
/* 0461 */	UNO_LINK_DISPLAY_NAME,
/* 0462 */	UNO_LINK_DISPLAY_BITMAP,
/* 0463 */	UNO_NAME_HEADING_STYLE_NAME,
/* 0464 */	UNO_NAME_SHOW_ONLINE_LAYOUT,
/* 0465 */	UNO_NAME_USER_DEFINED_ATTRIBUTES,
/* 0466 */	UNO_NAME_TEXT_USER_DEFINED_ATTRIBUTES,
/* 0467 */	UNO_NAME_FILE_PATH,
/* 0468 */	UNO_NAME_PARA_CHAPTER_NUMBERING_LEVEL,
/* 0469 */	UNO_NAME_PARA_CONDITIONAL_STYLE_NAME,
/* 0470 */	UNO_NAME_CHAPTER_NUMBERING_LEVEL,
/* 0471 */	UNO_NAME_NUMBERING_SEPARATOR,
/* 0472 */	UNO_NAME_IS_CONTINUOUS_NUMBERING,
/* 0473 */	UNO_NAME_IS_AUTOMATIC,
/* 0474 */	UNO_NAME_IS_ABSOLUTE_MARGINS,
/* 0475 */	UNO_NAME_CATEGORY,
/* 0476 */	UNO_NAME_DEPENDENT_TEXT_FIELDS,
/* 0477 */	UNO_NAME_CURRENT_PRESENTATION,
/* 0478 */	UNO_NAME_ADJUST,
/* 0479 */	UNO_NAME_INSTANCE_NAME,
/* 0480 */	UNO_NAME_TEXT_PORTION_TYPE,
/* 0481 */	UNO_NAME_CONTROL_CHARACTER,
/* 0482 */	UNO_NAME_IS_COLLAPSED,
/* 0483 */	UNO_NAME_IS_START,
/* 0484 */	UNO_NAME_SEQUENCE_NUMBER,
/* 0485 */	UNO_NAME_REFERENCE_ID,
/* 0486 */	UNO_NAME_HEADER_LEFT_BORDER_DISTANCE,
/* 0487 */	UNO_NAME_HEADER_RIGHT_BORDER_DISTANCE,
/* 0488 */	UNO_NAME_HEADER_TOP_BORDER_DISTANCE,
/* 0489 */	UNO_NAME_HEADER_BOTTOM_BORDER_DISTANCE,
/* 0490 */	UNO_NAME_FOOTER_LEFT_BORDER_DISTANCE,
/* 0491 */	UNO_NAME_FOOTER_RIGHT_BORDER_DISTANCE,
/* 0492 */	UNO_NAME_FOOTER_TOP_BORDER_DISTANCE,
/* 0493 */	UNO_NAME_FOOTER_BOTTOM_BORDER_DISTANCE,
/* 0494 */	UNO_NAME_PARA_IS_NUMBERING_RESTART,
/* 0495 */	UNO_NAME_HIDE_FIELD_TIPS,
/* 0496 */	UNO_NAME_HEADER_USER_DEFINED_ATTRIBUTES,
/* 0497 */	UNO_NAME_FOOTER_USER_DEFINED_ATTRIBUTES,
/* 0498 */	UNO_NAME_PARA_SHADOW_FORMAT,
/* 0499 */	UNO_NAME_CONTOUR_POLY_POLYGON,

/* 0500 */	UNO_NAME_IS_PIXEL_CONTOUR,
/* 0501 */	UNO_NAME_IS_AUTOMATIC_CONTOUR,
/* 0502 */	UNO_NAME_SEPARATOR_LINE_WIDTH,
/* 0503 */	UNO_NAME_SEPARATOR_LINE_COLOR,
/* 0504 */	UNO_NAME_SEPARATOR_LINE_RELATIVE_HEIGHT,
/* 0505 */	UNO_NAME_SEPARATOR_LINE_VERTIVAL_ALIGNMENT,
/* 0506 */	UNO_NAME_SEPARATOR_LINE_IS_ON,
/* 0507 */	UNO_NAME_IS_SKIP_HIDDEN_TEXT,
/* 0508 */	UNO_NAME_IS_SKIP_PROTECTED_TEXT,
/* 0509 */	UNO_NAME_DOCUMENT_INDEX_MARKS,
/* 0510 */	UNO_NAME_FOOTNOTE_IS_COLLECT_AT_TEXT_END,
/* 0511 */	UNO_NAME_FOOTNOTE_IS_RESTART_NUMBERING,
/* 0512 */	UNO_NAME_FOOTNOTE_RESTART_NUMBERING_AT,
/* 0513 */	UNO_NAME_FOOTNOTE_IS_OWN_NUMBERING,
/* 0514 */	UNO_NAME_FOOTNOTE_NUMBERING_TYPE,
/* 0515 */	UNO_NAME_FOOTNOTE_NUMBERING_PREFIX,
/* 0516 */	UNO_NAME_FOOTNOTE_NUMBERING_SUFFIX,
/* 0517 */	UNO_NAME_ENDNOTE_IS_COLLECT_AT_TEXT_END,
/* 0518 */	UNO_NAME_ENDNOTE_IS_RESTART_NUMBERING,
/* 0519 */	UNO_NAME_ENDNOTE_RESTART_NUMBERING_AT,
/* 0520 */	UNO_NAME_ENDNOTE_IS_OWN_NUMBERING,
/* 0521 */	UNO_NAME_ENDNOTE_NUMBERING_TYPE,
/* 0522 */	UNO_NAME_ENDNOTE_NUMBERING_PREFIX,
/* 0523 */	UNO_NAME_ENDNOTE_NUMBERING_SUFFIX,
/* 0524 */	UNO_NAME_BRACKET_BEFORE,
/* 0525 */	UNO_NAME_BRACKET_AFTER,
/* 0526 */	UNO_NAME_IS_NUMBER_ENTRIES,
/* 0527 */	UNO_NAME_IS_SORT_BY_POSITION,
/* 0528 */	UNO_NAME_SORT_KEYS,
/* 0529 */	UNO_NAME_IS_SORT_ASCENDING,
/* 0530 */	UNO_NAME_SORT_KEY,
/* 0531 */	UNO_NAME_FIELDS,
/* 0532 */	UNO_NAME_DATE_TIME_VALUE,
/* 0533 */	UNO_NAME_IS_ON,
/* 0534 */	UNO_NAME_Z_ORDER,
/* 0535 */	UNO_NAME_CONTENT_SECTION,
/* 0536 */	UNO_NAME_HEADER_SECTION,
/* 0537 */	UNO_NAME_PARA_IS_HANGING_PUNCTUATION,
/* 0538 */	UNO_NAME_PARA_IS_CHARACTER_DISTANCE,
/* 0539 */	UNO_NAME_PARA_IS_FORBIDDEN_RULES,
/* 0540 */	UNO_NAME_PARA_VERT_ALIGNMENT,
/* 0541 */	UNO_NAME_IS_MAIN_ENTRY,
/* 0542 */	UNO_NAME_GRAPHIC_ROTATION,
/* 0543 */	UNO_NAME_ADJUST_LUMINANCE,
/* 0544 */	UNO_NAME_ADJUST_CONTRAST,
/* 0545 */	UNO_NAME_ADJUST_RED,
/* 0546 */	UNO_NAME_ADJUST_GREEN,
/* 0547 */	UNO_NAME_ADJUST_BLUE,
/* 0548 */	UNO_NAME_GAMMA,
/* 0549 */	UNO_NAME_GRAPHIC_IS_INVERTED,
/* 0550 */	UNO_NAME_TRANSPARENCY,
/* 0551 */	UNO_NAME_GRAPHIC_COLOR_MODE,
/* 0552 */	UNO_NAME_REDLINE_AUTHOR,
/* 0553 */	UNO_NAME_REDLINE_DATE_TIME,
/* 0554 */	UNO_NAME_REDLINE_COMMENT,
/* 0555 */	UNO_NAME_REDLINE_TYPE,
/* 0556 */	UNO_NAME_REDLINE_SUCCESSOR_DATA,
/* 0557 */	UNO_NAME_REDLINE_IDENTIFIER,
/* 0558 */	UNO_NAME_IS_IN_HEADER_FOOTER,
/* 0559 */	UNO_NAME_START_REDLINE,
/* 0560 */	UNO_NAME_END_REDLINE,
/* 0561 */	UNO_NAME_REDLINE_START,
/* 0562 */	UNO_NAME_REDLINE_END,
/* 0563 */	UNO_NAME_REDLINE_TEXT,
/* 0564 */	UNO_NAME_REDLINE_DISPLAY_TYPE,
/* 0565 */	UNO_NAME_FORBIDDEN_CHARACTERS,
/* 0566 */	UNO_NAME_RUBY_BASE_TEXT,
/* 0567 */	UNO_NAME_RUBY_TEXT,
/* 0568 */	UNO_NAME_RUBY_ADJUST,
/* 0569 */	UNO_NAME_RUBY_CHAR_STYLE_NAME,
/* 0570 */	UNO_NAME_RUBY_IS_ABOVE,
/* 0571 */	UNO_NAME_FOOTNOTE_HEIGHT,
/* 0572 */	UNO_NAME_FOOTNOTE_LINE_WEIGHT,
/* 0573 */	UNO_NAME_FOOTNOTE_LINE_COLOR,
/* 0574 */	UNO_NAME_FOOTNOTE_LINE_RELATIVE_WIDTH,
/* 0575 */	UNO_NAME_FOOTNOTE_LINE_ADJUST,
/* 0576 */	UNO_NAME_FOOTNOTE_LINE_TEXT_DISTANCE,
/* 0577 */	UNO_NAME_FOOTNOTE_LINE_DISTANCE,
/* 0578 */	UNO_NAME_CHAR_ROTATION,
/* 0579 */	UNO_NAME_CHAR_ROTATION_IS_FIT_TO_LINE,
/* 0580 */	UNO_NAME_CHAR_SCALE_WIDTH,
/* 0581 */	UNO_NAME_TAB_STOP_DISTANCE,
/* 0582 */	UNO_NAME_IS_WIDTH_RELATIVE,
/* 0583 */	UNO_NAME_CHAR_RELIEF,
/* 0584 */	UNO_NAME_IS_HIDDEN,
/* 0585 */	UNO_NAME_IS_CONDITION_TRUE,
/* 0586 */	UNO_NAME_TWO_DIGIT_YEAR,
/* 0587 */	UNO_NAME_PROTECTION_KEY,
/* 0588 */	UNO_NAME_REDLINE_PROTECTION_KEY,
/* 0589 */	UNO_NAME_AUTOMATIC_DISTANCE,
/* 0590 */	UNO_NAME_AUTOMATIC_CONTROL_FOCUS,
/* 0591 */	UNO_NAME_APPLY_FORM_DESIGN_MODE,
/* 0592 */	UNO_NAME_LOCALE,
/* 0593 */	UNO_NAME_SORT_ALGORITHM,
/* 0594 */	UNO_NAME_FRAME_HEIGHT_ABSOLUTE,
/* 0596 */	UNO_NAME_FRAME_HEIGHT_PERCENT,
/* 0597 */	UNO_NAME_FRAME_ISAUTOMATIC_HEIGHT,
/* 0598 */	UNO_NAME_FRAME_WIDTH_ABSOLUTE,
/* 0599 */	UNO_NAME_FRAME_WIDTH_PERCENT,
/* 0600 */	UNO_NAME_PARA_STYLEHEADING,
/* 0601 */	UNO_NAME_PARA_STYLELEVEL1,
/* 0602 */	UNO_NAME_PARA_STYLELEVEL10,
/* 0603 */	UNO_NAME_PARA_STYLELEVEL2,
/* 0604 */	UNO_NAME_PARA_STYLELEVEL3,
/* 0605 */	UNO_NAME_PARA_STYLELEVEL4,
/* 0606 */	UNO_NAME_PARA_STYLELEVEL5,
/* 0607 */	UNO_NAME_PARA_STYLELEVEL6,
/* 0608 */	UNO_NAME_PARA_STYLELEVEL7,
/* 0609 */	UNO_NAME_PARA_STYLELEVEL8,
/* 0610 */	UNO_NAME_PARA_STYLELEVEL9,
/* 0611 */	UNO_NAME_PARA_STYLESEPARATOR,
/* 0612 */	UNO_NAME_MACRO_LIBRARY,
/* 0613 */	UNO_NAME_CELL_NAME,
/* 0614 */  UNO_NAME_PARA_USER_DEFINED_ATTRIBUTES,
/* 0615 */  UNO_NAME_MERGE_LAST_PARA,
/* 0616 */  UNO_NAME_WRITING_MODE,
/* 0617 */  UNO_NAME_GRID_COLOR,
/* 0618 */  UNO_NAME_GRID_LINES,
/* 0619 */  UNO_NAME_GRID_BASE_HEIGHT,
/* 0620 */  UNO_NAME_GRID_RUBY_HEIGHT,
/* 0621 */  UNO_NAME_GRID_MODE,
/* 0622 */  UNO_NAME_GRID_RUBY_BELOW,
/* 0623 */  UNO_NAME_GRID_PRINT,
/* 0624 */  UNO_NAME_GRID_DISPLAY,
/* 0625 */  UNO_NAME_SNAP_TO_GRID,
/* 0626 */  UNO_NAME_DONT_BALANCE_TEXT_COLUMNS,
/* 0627 */  UNO_NAME_IS_CURRENTLY_VISIBLE,
/* 0628 */  UNO_NAME_ANCHOR_POSITION,
/* 0629 */  UNO_NAME_TEXT_READING,
/* 0630 */  UNO_NAME_PRIMARY_KEY_READING,
/* 0631 */  UNO_NAME_SECONDARY_KEY_READING,
/* 0632 */  UNO_NAME_BACK_GRAPHIC_TRANSPARENCY,
/* 0633 */  UNO_NAME_BACK_COLOR_R_G_B,
/* 0634 */  UNO_NAME_BACK_COLOR_TRANSPARENCY,
/* 0635 */  UNO_NAME_HEADER_DYNAMIC_SPACING,
/* 0636 */  UNO_NAME_FOOTER_DYNAMIC_SPACING,
/* 0637 */  UNO_NAME_BASIC_LIBRARIES,
/* 0638 */  UNO_NAME_IS_FIXED_LANGUAGE,
/* 0639 */  UNO_NAME_SELECTION,
/* 0640 */  UNO_NAME_RESULT_SET,
/* 0641 */  UNO_NAME_CONNECTION,
/* 0642 */  UNO_NAME_MODEL,
/* 0643 */  UNO_NAME_DATA_SOURCE_NAME,
/* 0644 */  UNO_NAME_DATA_COMMAND,
/* 0645 */  UNO_NAME_FILTER,
/* 0646 */  UNO_NAME_DOCUMENT_URL,
/* 0647 */  UNO_NAME_OUTPUT_URL,
/* 0648 */  UNO_NAME_OUTPUT_TYPE,
/* 0649 */  UNO_NAME_ESCAPE_PROCESSING,
/* 0650 */  UNO_NAME_SINGLE_PRINT_JOBS,
/* 0651 */  UNO_NAME_FILE_NAME_FROM_COLUMN,
/* 0652 */  UNO_NAME_FILE_NAME_PREFIX,
/* 0653 */  UNO_NAME_CHAR_STYLE_NAMES,
/* 0654 */  UNO_NAME_DAD_COMMAND,		/* DAD = DataAccessDescriptor (see com.sun.star.sdb.DataAccessDescriptor) */
/* 0655 */  UNO_NAME_DAD_COMMAND_TYPE,
/* 0656 */  UNO_NAME_CLSID,
/* 0657 */  UNO_NAME_COMPONENT,
/* 0658 */  UNO_NAME_SECT_LEFT_MARGIN,
/* 0659 */  UNO_NAME_SECT_RIGHT_MARGIN,
/* 0660 */  UNO_NAME_PARA_IS_CONNECT_BORDER,
/* 0661 */  UNO_NAME_ITEMS,
/* 0662 */  UNO_NAME_SELITEM,
/* 0663 */  UNO_NAME_IS_SPLIT_ALLOWED,
/* 0664 */  UNO_NAME_CHAR_HIDDEN,
// OD 18.09.2003 #i18732#
/* 0665 */  UNO_NAME_IS_FOLLOWING_TEXT_FLOW,
/* 0666 */  UNO_NAME_WIDTH_TYPE,
/* 0667 */  UNO_NAME_SCRIPT_URL,
/* 0668 */  UNO_NAME_RUNTIME_UID,
/* 0669 */  UNO_NAME_COLLAPSING_BORDERS,
// OD 2004-05-05 #i28701#
/* 0670 */  UNO_NAME_WRAP_INFLUENCE_ON_POSITION,
/* 0671 */  UNO_NAME_DATA_BASE_URL,
/* 0672 */  UNO_NAME_DATA_BASE_RESOURCE,
// --> OD 2004-08-06 #i28749#
/* 0673 */  UNO_NAME_TRANSFORMATION_IN_HORI_L2R,
/* 0674 */  UNO_NAME_POSITION_LAYOUT_DIR,
// <--
/* 0675 */  UNO_NAME_NUMBERING_IS_OUTLINE,
/* 0676 */  UNO_NAME_DEFAULT_OUTLINE_LEVEL,
// --> OD 2004-10-28 #i36248#
/* 0677 */  UNO_NAME_STARTPOSITION_IN_HORI_L2R,
/* 0678 */  UNO_NAME_ENDPOSITION_IN_HORI_L2R,
// <--
// --> OS 2004-12-13 #30916#
/* 0679 */  UNO_NAME_SUBJECT,
/* 0680 */  UNO_NAME_ADDRESS_FROM_COLUMN,
/* 0681 */  UNO_NAME_SEND_AS_HTML,
/* 0682 */  UNO_NAME_SEND_AS_ATTACHMENT,
/* 0683 */  UNO_NAME_MAIL_BODY,
/* 0684 */  UNO_NAME_ATTACHMENT_NAME,
/* 0685 */  UNO_NAME_ATTACHMENT_FILTER,
/* 0686 */  UNO_NAME_PRINT_OPTIONS,
/* 0687 */  UNO_NAME_SAVE_AS_SINGLE_FILE,
/* 0688 */  UNO_NAME_SAVE_FILTER,
/* 0689 */  UNO_NAME_COPIES_TO,
/* 0690 */  UNO_NAME_BLIND_COPIES_TO,
/* 0691 */  UNO_NAME_IN_SERVER_PASSWORD,
/* 0692 */  UNO_NAME_OUT_SERVER_PASSWORD,
// <--
/* 0693 */  UNO_NAME_SAVE_FILTER_OPTIONS,
/* 0694 */  UNO_NAME_SAVE_FILTER_DATA,
/* 0695 */  UNO_NAME_LOCK_UPDATES,
// --> TL 2005-02-14 #i37766#
/* 0696 */  UNO_NAME_PAGE_COUNT,
/* 0697 */  UNO_NAME_LINE_COUNT,
/* 0698 */  UNO_NAME_IS_CONSTANT_SPELLCHECK,
/* 0699 */  UNO_NAME_IS_HIDE_SPELL_MARKS,
// <--
/* 0700 */  UNO_NAME_IS_FIELD_USED,
/* 0701 */  UNO_NAME_IS_FIELD_DISPLAYED,
/* 0702 */	UNO_NAME_BUILDID,
/* 0703 */  UNO_NAME_PARA_STYLE_CONDITIONS,
// --> FME 2005-12-13 #b6354161# Print empty pages
/* 0704 */  UNO_NAME_PRINT_EMPTY_PAGES,
// <--
/* 0705 */  UNO_NAME_FIELD_CODE,
/* 0706 */  UNO_NAME_HAS_VALID_SIGNATURES,
/* 0707 */  SW_PROPNAME_END
};


struct SwPropNameLen
{
    const char* pName;
    USHORT 		nNameLen;
    operator const char*() const { return pName; }
};



typedef const SwPropNameLen SwPropNameTab[ SW_PROPNAME_END - SW_PROPNAME_BEGIN ];
extern const SwPropNameTab aPropNameTab;

// !!! function must not(!) be inline because it is called via the below  !!!
// !!! listed macros from the swui DLL and that can not access the        !!!
// !!! array aPropNameTab which is located in a different DLL.            !!!
SW_DLLPUBLIC const SwPropNameLen& GetPropName( USHORT nId );
SW_DLLPUBLIC const SwPropNameLen& SwGetPropName( USHORT nId );

#define SW_PROP_NAME(nId) \
    GetPropName( nId ).pName, GetPropName( nId ).nNameLen

#define SW_PRPNM_EQLASCI(nId) \
    GetPropName( nId ).pName, 0, GetPropName( nId ).nNameLen

#define SW_PROP_NAME_STR(nId) \
    GetPropName( nId ).pName


#endif
