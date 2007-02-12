/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: hintids.hxx,v $
 *
 *  $Revision: 1.32 $
 *
 *  last change: $Author: kz $ $Date: 2007-02-12 14:32:32 $
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
#ifndef _HINTIDS_HXX
#define _HINTIDS_HXX

#ifndef _SOLAR_H
#include <tools/solar.h>		// fuer __FAR_DATA
#endif
#ifndef _SAL_TYPES_H_
#include <sal/types.h>			// for sal_Unicode
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif

// fuer SwTxtHints ohne Endindex wird folgendes Zeichen eingefuegt:
//JP 24.05.00: for the new UniCode Version:
#define CH_TXTATR_BREAKWORD		((sal_Unicode)0x01)
#define CH_TXTATR_INWORD		((sal_Unicode)0x02)
#define CH_TXTATR_TAB           ((sal_Unicode)'\t')
#define CH_TXTATR_NEWLINE       ((sal_Unicode)'\n')

/*
 * Hier kommen erst mal die enums fuer die Hints
 */

#define HINT_BEGIN	1

#define POOLATTR_BEGIN		HINT_BEGIN
#define POOLATTR_END		RES_UNKNOWNATR_END

// Bereiche fuer die Id's der einzelnen Format-Attribute
// die Which-Werte fuer die Charakter-Format Attribute
enum RES_CHRATR
{
RES_CHRATR_BEGIN = HINT_BEGIN,
    RES_CHRATR_CASEMAP = RES_CHRATR_BEGIN,	//	1
    RES_CHRATR_CHARSETCOLOR,				//	2
    RES_CHRATR_COLOR,						//	3
    RES_CHRATR_CONTOUR,						//	4
    RES_CHRATR_CROSSEDOUT,					//	5
    RES_CHRATR_ESCAPEMENT,					//	6
    RES_CHRATR_FONT,						//	7
    RES_CHRATR_FONTSIZE,					//	8
    RES_CHRATR_KERNING, 					//	9
    RES_CHRATR_LANGUAGE,					// 10
    RES_CHRATR_POSTURE, 					// 11
    RES_CHRATR_PROPORTIONALFONTSIZE,		// 12
    RES_CHRATR_SHADOWED,					// 13
    RES_CHRATR_UNDERLINE,					// 14
    RES_CHRATR_WEIGHT,						// 15
    RES_CHRATR_WORDLINEMODE,				// 16
    RES_CHRATR_AUTOKERN,					// 17
    RES_CHRATR_BLINK,						// 18
    RES_CHRATR_NOHYPHEN,					// 19
    RES_CHRATR_NOLINEBREAK, 				// 20
    RES_CHRATR_BACKGROUND,					// 21
    RES_CHRATR_CJK_FONT,					// 22
    RES_CHRATR_CJK_FONTSIZE,				// 23
    RES_CHRATR_CJK_LANGUAGE,				// 24
    RES_CHRATR_CJK_POSTURE, 				// 25
    RES_CHRATR_CJK_WEIGHT,					// 26
    RES_CHRATR_CTL_FONT,					// 27
    RES_CHRATR_CTL_FONTSIZE,				// 28
    RES_CHRATR_CTL_LANGUAGE,				// 29
    RES_CHRATR_CTL_POSTURE, 				// 30
    RES_CHRATR_CTL_WEIGHT,					// 31
    RES_CHRATR_ROTATE,						// 32
    RES_CHRATR_EMPHASIS_MARK,				// 33
    RES_CHRATR_TWO_LINES, 					// 34
    RES_CHRATR_SCALEW,						// 35
    RES_CHRATR_RELIEF,						// 36
    RES_CHRATR_HIDDEN,                      // 37
RES_CHRATR_END
};

// diese Attribute stehen nur im SwpAttr-Array vom TextNode
enum RES_TXTATR
{
RES_TXTATR_BEGIN = RES_CHRATR_END,

// alle TextAttribute mit einem Ende
// JP 12.07.96: die Ids RES_TXTATR_NOLINEBREAK und RES_TXTATR_NOHYPHEN
//				wanderten in den Bereich der CharAttr. Die Ids selbst muessen
//				aber als Dummies noch erhalten bleiben. Der SwG-/Sw3 Reader
//				merkt sich die Ids immer als Offset zum Start der Gruppe!!!
// Aus dem RES_TXTATR_NOLINEBREAK ist jetzt RES_TXTATR_INETFMT geworden.
RES_TXTATR_WITHEND_BEGIN = RES_TXTATR_BEGIN ,
    RES_TXTATR_AUTOFMT = RES_TXTATR_WITHEND_BEGIN,  // 38
    RES_TXTATR_INETFMT,                             // 39
    RES_TXTATR_REFMARK, 							// 40
    RES_TXTATR_TOXMARK, 							// 41
    RES_TXTATR_CHARFMT, 							// 42
    RES_TXTATR_DUMMY5,                              // 43
    RES_TXTATR_CJK_RUBY, 							// 44
    RES_TXTATR_UNKNOWN_CONTAINER,					// 45
    RES_TXTATR_DUMMY6,								// 46
    RES_TXTATR_DUMMY7,								// 47
RES_TXTATR_WITHEND_END,

// alle TextAttribute ohne ein Ende
RES_TXTATR_NOEND_BEGIN = RES_TXTATR_WITHEND_END,
    RES_TXTATR_FIELD = RES_TXTATR_NOEND_BEGIN,		// 48
    RES_TXTATR_FLYCNT,								// 49
    RES_TXTATR_FTN, 								// 50
    RES_TXTATR_SOFTHYPH,							// 51
    RES_TXTATR_HARDBLANK,							// 52
    RES_TXTATR_DUMMY1,								// 53
    RES_TXTATR_DUMMY2,								// 54
RES_TXTATR_NOEND_END,
RES_TXTATR_END = RES_TXTATR_NOEND_END
};

enum RES_PARATR
{
RES_PARATR_BEGIN = RES_TXTATR_END,
    RES_PARATR_LINESPACING = RES_PARATR_BEGIN,		// 55
    RES_PARATR_ADJUST,								// 56
    RES_PARATR_SPLIT,								// 57
    RES_PARATR_ORPHANS, 							// 58
    RES_PARATR_WIDOWS,								// 59
    RES_PARATR_TABSTOP, 							// 60
    RES_PARATR_HYPHENZONE,							// 61
    RES_PARATR_DROP,								// 62
    RES_PARATR_REGISTER,							// 63
    RES_PARATR_NUMRULE, 							// 64
    RES_PARATR_SCRIPTSPACE,							// 65
    RES_PARATR_HANGINGPUNCTUATION,					// 66
    RES_PARATR_FORBIDDEN_RULES,						// 67
    RES_PARATR_VERTALIGN,							// 68
    RES_PARATR_SNAPTOGRID,                          // 69
    RES_PARATR_CONNECT_BORDER,                      // 70
    RES_PARATR_DUMMY5,                              // 71
    RES_PARATR_DUMMY6,                              // 72
    RES_PARATR_DUMMY7,								// 73
    RES_PARATR_DUMMY8,								// 74
RES_PARATR_END
};


enum RES_FRMATR
{
RES_FRMATR_BEGIN = RES_PARATR_END,
    RES_FILL_ORDER = RES_FRMATR_BEGIN,              // 75
    RES_FRM_SIZE,                                   // 76
    RES_PAPER_BIN,                                  // 77
    RES_LR_SPACE,                                   // 78
    RES_UL_SPACE,                                   // 79
    RES_PAGEDESC,                                   // 80
    RES_BREAK,                                      // 81
    RES_CNTNT,                                      // 82
    RES_HEADER,                                     // 83
    RES_FOOTER,                                     // 84
    RES_PRINT,                                      // 85
    RES_OPAQUE,                                     // 86
    RES_PROTECT,                                    // 87
    RES_SURROUND,                                   // 88
    RES_VERT_ORIENT,                                // 89
    RES_HORI_ORIENT,                                // 90
    RES_ANCHOR,                                     // 91
    RES_BACKGROUND,                                 // 92
    RES_BOX,                                        // 93
    RES_SHADOW,                                     // 94
    RES_FRMMACRO,                                   // 95
    RES_COL,                                        // 96
    RES_KEEP,                                       // 97
    RES_URL,                                        // 98
    RES_EDIT_IN_READONLY,                           // 99
    RES_LAYOUT_SPLIT,                               // 100
    RES_CHAIN,                                      // 101
    RES_TEXTGRID,                                   // 102
    RES_LINENUMBER  ,                               // 103
    RES_FTN_AT_TXTEND,                              // 104
    RES_END_AT_TXTEND,                              // 105
    RES_COLUMNBALANCE,                              // 106
    RES_FRAMEDIR,                                   // 107
    RES_HEADER_FOOTER_EAT_SPACING,                  // 108
    RES_ROW_SPLIT,                                  // 109
    // OD 18.09.2003 #i18732# - insert new item and 5 dummies
    RES_FOLLOW_TEXT_FLOW,                           // 110
// --> collapsing borders FME 2005-05-27 #i29550#
    RES_COLLAPSING_BORDERS,                         // 111
// <-- collapsing
    // OD 2004-05-04 #i28701# - use dummy1 for new item
    RES_WRAP_INFLUENCE_ON_OBJPOS,                   // 112
    RES_AUTO_STYLE,                                 // 113
    RES_FRMATR_STYLE_NAME,                          // 114
    RES_FRMATR_CONDITIONAL_STYLE_NAME,              // 115
RES_FRMATR_END
};

enum RES_GRFATR
{
RES_GRFATR_BEGIN = RES_FRMATR_END,
    RES_GRFATR_MIRRORGRF = RES_GRFATR_BEGIN,        // 116
    RES_GRFATR_CROPGRF,                             // 117

    RES_GRFATR_ROTATION,                            // 118
    RES_GRFATR_LUMINANCE,                           // 119
    RES_GRFATR_CONTRAST,                            // 120
    RES_GRFATR_CHANNELR,                            // 121
    RES_GRFATR_CHANNELG,                            // 122
    RES_GRFATR_CHANNELB,                            // 123
    RES_GRFATR_GAMMA,                               // 124
    RES_GRFATR_INVERT,                              // 125
    RES_GRFATR_TRANSPARENCY,                        // 126
    RES_GRFATR_DRAWMODE,                            // 127

    RES_GRFATR_DUMMY1,                              // 128
    RES_GRFATR_DUMMY2,                              // 129
    RES_GRFATR_DUMMY3,                              // 130
    RES_GRFATR_DUMMY4,                              // 131
    RES_GRFATR_DUMMY5,                              // 132
RES_GRFATR_END
};

enum RES_BOXATR
{
RES_BOXATR_BEGIN = RES_GRFATR_END,
    RES_BOXATR_FORMAT = RES_BOXATR_BEGIN,           // 133
    RES_BOXATR_FORMULA,                             // 134
    RES_BOXATR_VALUE,                               // 135
RES_BOXATR_END
};

enum RES_UNKNOWNATR
{
RES_UNKNOWNATR_BEGIN = RES_BOXATR_END,
    RES_UNKNOWNATR_CONTAINER = RES_UNKNOWNATR_BEGIN,// 136
RES_UNKNOWNATR_END
};



// ID-s fuer die Formate
enum RES_FMT
{
RES_FMT_BEGIN = RES_UNKNOWNATR_END,
    RES_CHRFMT = RES_FMT_BEGIN,
    RES_FRMFMT,
    RES_FLYFRMFMT,
    RES_TXTFMTCOLL,
    RES_GRFFMTCOLL,
    RES_DRAWFRMFMT,
    RES_CONDTXTFMTCOLL,
RES_FMT_END
};

// die ID's fuer Messages in den Formaten
enum RES_MSG
{
RES_MSG_BEGIN = RES_FMT_END,
    RES_OBJECTDYING = RES_MSG_BEGIN,
    RES_FMT_CHG,
    RES_ATTRSET_CHG,
    RES_FRM_SIZECHG,
    RES_TXTATR_FLDCHG,
    RES_TXTATR_EMPTYCHG,
    RES_INS_CHR,
    RES_INS_TXT,
    RES_DEL_CHR,
    RES_DEL_TXT,
    RES_UPDATE_ATTR,
    RES_PAGEDESC_FTNINFO,
    RES_REFMARKFLD_UPDATE,
    RES_DOCPOS_UPDATE,
    RES_TABLEFML_UPDATE,
    RES_UPDATEDDETBL,
    RES_TBLHEADLINECHG,
    RES_AUTOFMT_DOCNODE,
    RES_REPAINT,
    RES_DOC_DTOR,
    RES_SECTION_HIDDEN,
    RES_SECTION_NOT_HIDDEN,
    RES_GRAPHIC_ARRIVED,
    RES_GRAPHIC_PIECE_ARRIVED,
    RES_HIDDENPARA_PRINT,
    RES_CONDCOLL_CONDCHG,
    RES_VIRTPAGENUM_INFO,
    RES_GETNUMNODES,
    RES_GETLOWERNUMLEVEL,
    RES_RESET_FMTWRITTEN,
    RES_REMOVE_UNO_OBJECT,
    RES_GRF_REREAD_AND_INCACHE,
    RES_SECTION_RESETHIDDENFLAG,
    RES_FINDNEARESTNODE,
    RES_CONTENT_VISIBLE,
    RES_FOOTNOTE_DELETED,
    RES_REFMARK_DELETED,
    RES_TOXMARK_DELETED,
    RES_GRAPHIC_SWAPIN,
    RES_FIELD_DELETED,
    RES_NAME_CHANGED,
    RES_ALT_TEXT_CHANGED,
    RES_UNOCURSOR_LEAVES_SECTION,
RES_MSG_END
};


// eine ID fuer den RTF-Reader. Die Stylesheets werden wie
// Attribute behandelt, d.H. es gibt ein StyleSheet Attribut. Um
// nicht mit anderen Which()-Werten zu kollidieren, ist der Wert
// hier mit aufgefuehrt. (Auch das Hilfesystem definiert neue
// Attribute !!)
enum RES_FLTRATTR
{
RES_FLTRATTR_BEGIN = RES_MSG_END,
    RES_FLTR_STYLESHEET = RES_FLTRATTR_BEGIN,
    RES_FLTR_BOOKMARK,
    RES_FLTR_ANCHOR,
    RES_FLTR_BORDER,
    RES_FLTR_NUMRULE,
    RES_FLTR_NUMRULE_NUM,
    RES_FLTR_SDR_ANCHOR,
    RES_FLTR_TOX,
    RES_FLTR_SECTION,
    RES_FLTR_REDLINE,
    RES_FLTR_SCRIPTTYPE,
RES_FLTRATTR_END
};

#define RES_TBX_DUMMY RES_FLTRATTR_END + 1

#define HINT_END RES_TBX_DUMMY

// Fehler-Erkennung !!
#define INVALID_HINT HINT_END
#define RES_WHICHHINT_END HINT_END


/*
 * hole aus der statischen Default-Attribut Tabelle ueber den Which-Wert
 * das entsprechende default Attribut.
 * Ist keines vorhanden, returnt ein 0-Pointer !!!
 *
 * Diese Funktion ist im Init.CXX implementiert. Damit die Formate darauf
 * zugreifen koennen, ist sie hier als extern deklariert.
 * Im PRODUCT ist das Teil inline.
 */
class SfxPoolItem;
struct SfxItemInfo;
typedef SfxPoolItem* SwDfltAttrTab[ POOLATTR_END - POOLATTR_BEGIN  ];

extern SwDfltAttrTab __FAR_DATA aAttrTab;
extern SfxItemInfo	 __FAR_DATA aSlotTab[];


#ifndef PRODUCT
const SfxPoolItem* GetDfltAttr( USHORT nWhich );
#else
inline const SfxPoolItem* GetDfltAttr( USHORT nWhich )
{
    return aAttrTab[ nWhich - POOLATTR_BEGIN ];
}
#endif

SW_DLLPUBLIC USHORT GetWhichOfScript( USHORT nWhich, USHORT nScript );

// return for the given TextAttribut without an end the correct character.
// This function returns
//		CH_TXTATR_BREAKWORD for Textattribut which breaks a word (default)
//		CH_TXTATR_INWORD	for Textattribut which dont breaks a word
class SwTxtAttr;
sal_Unicode GetCharOfTxtAttr( const SwTxtAttr& rAttr );

// alle Sets stehen im init.cxx

// AttrSet-Range fuer die 3 Break-Attribute
extern USHORT __FAR_DATA aBreakSetRange[];
// AttrSet-Range fuer die TxtFmtColl
extern USHORT __FAR_DATA aTxtFmtCollSetRange[];
// AttrSet-Range fuer die GrfFmtColl
extern USHORT __FAR_DATA aGrfFmtCollSetRange[];
// AttrSet-Range fuer die TextNode
extern USHORT __FAR_DATA aTxtNodeSetRange[];
// AttrSet-Range fuer die NoTxtNode
extern USHORT __FAR_DATA aNoTxtNodeSetRange[];
// AttrSet-Range fuer die SwTable
extern USHORT __FAR_DATA aTableSetRange[];
// AttrSet-Range fuer die SwTableLine
extern USHORT __FAR_DATA aTableLineSetRange[];
// AttrSet-Range fuer die SwTableBox
extern USHORT __FAR_DATA aTableBoxSetRange[];
// AttrSet-Range fuer die SwFrmFmt
extern USHORT __FAR_DATA aFrmFmtSetRange[];
// AttrSet-Range fuer die SwCharFmt
extern USHORT __FAR_DATA aCharFmtSetRange[];
// AttrSet-Range fuer die character autostyles
extern USHORT __FAR_DATA aCharAutoFmtSetRange[];
// AttrSet-Range fuer die SwPageDescFmt
extern USHORT __FAR_DATA aPgFrmFmtSetRange[];

// check if ID is InRange of AttrSet-Ids
BOOL IsInRange( const USHORT* pRange, const USHORT nId );



#define ITEMID_FONT 		RES_CHRATR_FONT
#define ITEMID_POSTURE		RES_CHRATR_POSTURE
#define ITEMID_WEIGHT		RES_CHRATR_WEIGHT
#define ITEMID_SHADOWED 	RES_CHRATR_SHADOWED
#define ITEMID_AUTOKERN 	RES_CHRATR_AUTOKERN
#define ITEMID_CONTOUR		RES_CHRATR_CONTOUR
#define ITEMID_CROSSEDOUT	RES_CHRATR_CROSSEDOUT
#define ITEMID_UNDERLINE	RES_CHRATR_UNDERLINE
#define ITEMID_FONTHEIGHT	RES_CHRATR_FONTSIZE
#define ITEMID_PROPSIZE 	RES_CHRATR_PROPORTIONALFONTSIZE
#define ITEMID_COLOR		RES_CHRATR_COLOR
#define ITEMID_CHARSETCOLOR RES_CHRATR_CHARSETCOLOR
#define ITEMID_KERNING		RES_CHRATR_KERNING
#define ITEMID_CASEMAP		RES_CHRATR_CASEMAP
#define ITEMID_LANGUAGE 	RES_CHRATR_LANGUAGE
#define ITEMID_ESCAPEMENT	RES_CHRATR_ESCAPEMENT
#define ITEMID_BLINK		RES_CHRATR_BLINK
#define ITEMID_NOHYPHENHERE RES_CHRATR_NOHYPHEN
#define ITEMID_NOLINEBREAK	RES_CHRATR_NOLINEBREAK
#define ITEMID_WORDLINEMODE RES_CHRATR_WORDLINEMODE
#define ITEMID_EMPHASISMARK	RES_CHRATR_EMPHASIS_MARK
#define ITEMID_TWOLINES 	RES_CHRATR_TWO_LINES
#define ITEMID_CHARSCALE_W	RES_CHRATR_SCALEW
#define ITEMID_CHARROTATE	RES_CHRATR_ROTATE
#define ITEMID_CHARRELIEF	RES_CHRATR_RELIEF
#define ITEMID_CHARHIDDEN   RES_CHRATR_HIDDEN

//Defines fuer die ehemaligen FrmAttribute, die jetzt in frmitems.hxx
//von svx zu finden sind.
#define ITEMID_PAPERBIN 	RES_PAPER_BIN
#define ITEMID_LRSPACE		RES_LR_SPACE
#define ITEMID_ULSPACE		RES_UL_SPACE
#define ITEMID_PRINT		RES_PRINT
#define ITEMID_OPAQUE		RES_OPAQUE
#define ITEMID_PROTECT		RES_PROTECT
#define ITEMID_BRUSH		RES_BACKGROUND
#define ITEMID_BOX			RES_BOX
#define ITEMID_SHADOW		RES_SHADOW
#define ITEMID_MACRO		RES_FRMMACRO
#define ITEMID_FMTKEEP		RES_KEEP
#define ITEMID_FMTBREAK		RES_BREAK
#define ITEMID_FRAMEDIR		RES_FRAMEDIR

//Defines fuer die ehemaligen ParAttribute, die jetzt in paraitem.hxx
//bzw. tabsitem.hxx von svx zu finden sind.

#define ITEMID_LINESPACING	RES_PARATR_LINESPACING
#define ITEMID_ADJUST		RES_PARATR_ADJUST
#define ITEMID_FMTSPLIT 	RES_PARATR_SPLIT
#define ITEMID_WIDOWS		RES_PARATR_WIDOWS
#define ITEMID_ORPHANS		RES_PARATR_ORPHANS
#define ITEMID_TABSTOP		RES_PARATR_TABSTOP
#define ITEMID_HYPHENZONE	RES_PARATR_HYPHENZONE
#define ITEMID_SCRIPTSPACE	RES_PARATR_SCRIPTSPACE
#define ITEMID_HANGINGPUNCTUATION	RES_PARATR_HANGINGPUNCTUATION
#define ITEMID_FORBIDDENRULE 	RES_PARATR_FORBIDDEN_RULES
#define ITEMID_PARAVERTALIGN RES_PARATR_VERTALIGN
#define ITEMID_PARAGRID     RES_PARATR_SNAPTOGRID

//Defines fuer die ehemaligen GrfAttribute, die jetzt im svx zu finden sind.
#define ITEMID_GRF_CROP		RES_GRFATR_CROPGRF

#endif
