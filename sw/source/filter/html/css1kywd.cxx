/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: css1kywd.cxx,v $
 * $Revision: 1.8.210.1 $
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


#include "css1kywd.hxx"

/*  */

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS_mimetype, "text/css" );

/*  */

// ein par allgemeine Strings

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_import, "import" );

// Feature: PrintExt
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_page, "page" );
//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_media, "media" );
// /Feature: PrintExt

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_important, "important" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_link, "link" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_visited, "visited" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_first_letter, "first-letter" );

// Feature: PrintExt
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_left, "left" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_right, "right" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_first, "first" );
// /Feature: PrintExt

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_url, "url" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_rgb, "rgb" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_pt, "pt" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_mm, "mm" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_cm, "cm" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_pc, "pc" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_inch, "in" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_px, "px" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_em, "em" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_UNIT_ex, "ex" );

/*  */

// Strings fuer Font-Properties

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_font_family, "font-family" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_serif, "serif" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_sans_serif, "sans-serif" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_cursive, "cursive" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_fantasy, "fantasy" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_monospace, "monospace" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_font_style, "font-style" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_normal, "normal" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_italic, "italic" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_oblique, "oblique" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_font_variant, "font-variant" );

//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_normal, "normal" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_small_caps, "small-caps" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_font_weight, "font-weight" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_extra_light, "extra-light" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_light, "light" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_demi_light, "demi-light" );
//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_medium, "medium" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_demi_bold, "demi-bold" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_bold, "bold" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_extra_bold, "extra-bold" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_lighter, "lighter" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_bolder, "bolder" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_font_size, "font-size" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_xx_small, "xx-small" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_x_small, "x-small" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_small, "small" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_medium, "medium" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_large, "large" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_x_large, "x-large" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_xx_large, "xx-large" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_larger, "larger" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_smaller, "smaller" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_font, "font" );



/*  */

// Strings fuer Farb- und Hintergrund-Properties

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_color, "color" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_background, "background" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_background_color, "background-color" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_transparent, "transparent" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_repeat, "repeat" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_repeat_x, "repeat-x" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_repeat_y, "repeat-y" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_no_repeat, "no-repeat" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_top, "top" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_middle, "middle" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_bottom, "bottom" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_scroll, "scroll" );


/*  */

// Strings fuer Text-Properties

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_letter_spacing, "letter-spacing" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_text_decoration, "text-decoration" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_none, "none" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_underline, "underline" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_overline, "overline" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_line_through, "line-through" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_blink, "blink" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_text_align, "text-align" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_left, "left" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_center, "center" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_right, "right" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_justify, "justify" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_text_indent, "text-indent" );


sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_line_height, "line-height" );


/*  */

// Strings fuer Box-Properties

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_margin_left, "margin-left" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_margin_right, "margin-right" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_margin_top, "margin-top" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_margin_bottom, "margin-bottom" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_margin, "margin" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_padding_top, "padding-top" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_padding_bottom, "padding-bottom" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_padding_left, "padding-left" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_padding_right, "padding-right" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_padding, "padding" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_auto, "auto" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_left_width, "border-left-width" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_right_width, "border-right-width" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_top_width, "border-top-width" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_bottom_width, "border-bottom-width" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_width, "border-width" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_color, "border-color" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_style, "border-style" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_left, "border-left" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_right, "border-right" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_top, "border-top" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border_bottom, "border-bottom" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_border, "border" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_thin, "thin" );
//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_medium, "medium" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_thick, "thick" );

//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_none, "none" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_dotted, "dotted" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_dashed, "dashed" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_solid, "solid" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_double, "double" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_groove, "groove" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_ridge, "ridge" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_inset, "inset" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_outset, "outset" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_width, "width" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_height, "height" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_float, "float" );

/*  */

// Strings fuer Positioning

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_position, "position" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_absolute, "absolute" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_relative, "relative" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_static, "static" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_left, "left" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_top, "top" );

/*  */

// Feature: PrintExt

// Strings fuer Printing Extensions

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_page_break_before, "page-break-before" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_page_break_after, "page-break-after" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_page_break_inside, "page-break-inside" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_size, "size" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_widows, "widows" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_orphans, "orphans" );
//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_marks, "marks" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_always, "always" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_avoid, "avoid" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_portrait, "portrait" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_landscape, "landscape" );

//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_crop, "crop" );
//sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_cross, "cross" );

// /Feature: PrintExt

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_class_abs_pos, "sd-abs-pos" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_so_language, "so-language" );

sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_P_direction, "direction" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_ltr, "ltr" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_rtl, "rtl" );
sal_Char __FAR_DATA CSS1_CONSTASCII_DEF( sCSS1_PV_inherit, "inherit" );
