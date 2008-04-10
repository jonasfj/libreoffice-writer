/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unocoll.cxx,v $
 * $Revision: 1.40 $
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


#include <swtypes.hxx>
#include <cmdid.h>
#include <hintids.hxx>
#ifndef _SVX_SVXIDS_HRC //autogen
#include <svx/svxids.hrc>
#endif
#include <doc.hxx>
#include <docary.hxx>
#include <fmtcol.hxx>
#include <poolfmt.hxx>
#include <unocoll.hxx>
#include <unosett.hxx>
#include <unoclbck.hxx>
#include <fmtanchr.hxx>
#include <ndtxt.hxx>
#include <section.hxx>
#include <bookmrk.hxx>
#include <ftnidx.hxx>
#include <fmtftn.hxx>
#include <txtftn.hxx>
#include <fmtpdsc.hxx>
#include <pagedesc.hxx>
#include <vos/mutex.hxx>
#include <com/sun/star/text/XTextTableCursor.hpp>
#include <com/sun/star/text/XTextTablesSupplier.hpp>
#include <com/sun/star/text/TableColumnSeparator.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <svtools/PasswordHelper.hxx>
#include <svtools/unoimap.hxx>
#include <svtools/unoevent.hxx>
#include <unotbl.hxx>
#include <unostyle.hxx>
#include <unofield.hxx>
#include <unoidx.hxx>
#include <unoframe.hxx>
#include <vcl/svapp.hxx>
#include <authfld.hxx>
#include <SwXTextDefaults.hxx>
#include <unochart.hxx>

#include "docsh.hxx"

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

/******************************************************************************
 *
 ******************************************************************************/
struct  ProvNamesId_Type
{
    const char *    pName;
    sal_uInt16  	nType;
};

// note: this thing is indexed as an array, so do not insert/remove entries!
const ProvNamesId_Type __FAR_DATA aProvNamesId[] =
{
    { "com.sun.star.text.TextTable",                          SW_SERVICE_TYPE_TEXTTABLE },
    { "com.sun.star.text.TextFrame",                          SW_SERVICE_TYPE_TEXTFRAME },
    { "com.sun.star.text.GraphicObject",                      SW_SERVICE_TYPE_GRAPHIC },
    { "com.sun.star.text.TextEmbeddedObject",                 SW_SERVICE_TYPE_OLE },
    { "com.sun.star.text.Bookmark",                           SW_SERVICE_TYPE_BOOKMARK },
    { "com.sun.star.text.Footnote",                           SW_SERVICE_TYPE_FOOTNOTE },
    { "com.sun.star.text.Endnote",                            SW_SERVICE_TYPE_ENDNOTE },
    { "com.sun.star.text.DocumentIndexMark",                  SW_SERVICE_TYPE_INDEXMARK },
    { "com.sun.star.text.DocumentIndex",                      SW_SERVICE_TYPE_INDEX },
    { "com.sun.star.text.ReferenceMark",                      SW_SERVICE_REFERENCE_MARK },
    { "com.sun.star.style.CharacterStyle",                    SW_SERVICE_STYLE_CHARACTER_STYLE },
    { "com.sun.star.style.ParagraphStyle",                    SW_SERVICE_STYLE_PARAGRAPH_STYLE },
    { "com.sun.star.style.FrameStyle",                        SW_SERVICE_STYLE_FRAME_STYLE },
    { "com.sun.star.style.PageStyle",                         SW_SERVICE_STYLE_PAGE_STYLE },
    { "com.sun.star.style.NumberingStyle",                    SW_SERVICE_STYLE_NUMBERING_STYLE },
    { "com.sun.star.text.ContentIndexMark",                   SW_SERVICE_CONTENT_INDEX_MARK },
    { "com.sun.star.text.ContentIndex",                       SW_SERVICE_CONTENT_INDEX },
    { "com.sun.star.text.UserIndexMark",                      SW_SERVICE_USER_INDEX_MARK },
    { "com.sun.star.text.UserIndex",                          SW_SERVICE_USER_INDEX },
    { "com.sun.star.text.TextSection",                        SW_SERVICE_TEXT_SECTION },
    { "com.sun.star.text.TextField.DateTime",                 SW_SERVICE_FIELDTYPE_DATETIME },
    { "com.sun.star.text.TextField.User",                     SW_SERVICE_FIELDTYPE_USER },
    { "com.sun.star.text.TextField.SetExpression",            SW_SERVICE_FIELDTYPE_SET_EXP },
    { "com.sun.star.text.TextField.GetExpression",            SW_SERVICE_FIELDTYPE_GET_EXP },
    { "com.sun.star.text.TextField.FileName",                 SW_SERVICE_FIELDTYPE_FILE_NAME },
    { "com.sun.star.text.TextField.PageNumber",               SW_SERVICE_FIELDTYPE_PAGE_NUM },
    { "com.sun.star.text.TextField.Author",                   SW_SERVICE_FIELDTYPE_AUTHOR },
    { "com.sun.star.text.TextField.Chapter",                  SW_SERVICE_FIELDTYPE_CHAPTER },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_0 },
    { "com.sun.star.text.TextField.GetReference",             SW_SERVICE_FIELDTYPE_GET_REFERENCE },
    { "com.sun.star.text.TextField.ConditionalText",          SW_SERVICE_FIELDTYPE_CONDITIONED_TEXT },
    { "com.sun.star.text.TextField.Annotation",               SW_SERVICE_FIELDTYPE_ANNOTATION },
    { "com.sun.star.text.TextField.Input",                    SW_SERVICE_FIELDTYPE_INPUT },
    { "com.sun.star.text.TextField.Macro",                    SW_SERVICE_FIELDTYPE_MACRO },
    { "com.sun.star.text.TextField.DDE",                      SW_SERVICE_FIELDTYPE_DDE },
    { "com.sun.star.text.TextField.HiddenParagraph",          SW_SERVICE_FIELDTYPE_HIDDEN_PARA },
    { "" /*com.sun.star.text.TextField.DocumentInfo"*/,       SW_SERVICE_FIELDTYPE_DOC_INFO },
    { "com.sun.star.text.TextField.TemplateName",             SW_SERVICE_FIELDTYPE_TEMPLATE_NAME },
    { "com.sun.star.text.TextField.ExtendedUser",             SW_SERVICE_FIELDTYPE_USER_EXT },
    { "com.sun.star.text.TextField.ReferencePageSet",         SW_SERVICE_FIELDTYPE_REF_PAGE_SET },
    { "com.sun.star.text.TextField.ReferencePageGet",         SW_SERVICE_FIELDTYPE_REF_PAGE_GET },
    { "com.sun.star.text.TextField.JumpEdit",                 SW_SERVICE_FIELDTYPE_JUMP_EDIT },
    { "com.sun.star.text.TextField.Script",                   SW_SERVICE_FIELDTYPE_SCRIPT },
    { "com.sun.star.text.TextField.DatabaseNextSet",          SW_SERVICE_FIELDTYPE_DATABASE_NEXT_SET },
    { "com.sun.star.text.TextField.DatabaseNumberOfSet",      SW_SERVICE_FIELDTYPE_DATABASE_NUM_SET },
    { "com.sun.star.text.TextField.DatabaseSetNumber",        SW_SERVICE_FIELDTYPE_DATABASE_SET_NUM },
    { "com.sun.star.text.TextField.Database",                 SW_SERVICE_FIELDTYPE_DATABASE },
    { "com.sun.star.text.TextField.DatabaseName",             SW_SERVICE_FIELDTYPE_DATABASE_NAME },
    { "com.sun.star.text.TextField.TableFormula",             SW_SERVICE_FIELDTYPE_TABLE_FORMULA },
    { "com.sun.star.text.TextField.PageCount",                SW_SERVICE_FIELDTYPE_PAGE_COUNT },
    { "com.sun.star.text.TextField.ParagraphCount",           SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT },
    { "com.sun.star.text.TextField.WordCount",                SW_SERVICE_FIELDTYPE_WORD_COUNT },
    { "com.sun.star.text.TextField.CharacterCount",           SW_SERVICE_FIELDTYPE_CHARACTER_COUNT },
    { "com.sun.star.text.TextField.TableCount",               SW_SERVICE_FIELDTYPE_TABLE_COUNT },
    { "com.sun.star.text.TextField.GraphicObjectCount",       SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT },
    { "com.sun.star.text.TextField.EmbeddedObjectCount",      SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT },
    { "com.sun.star.text.TextField.DocInfo.ChangeAuthor",     SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR },
    { "com.sun.star.text.TextField.DocInfo.ChangeDateTime",   SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME },
    { "com.sun.star.text.TextField.DocInfo.EditTime",         SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME },
    { "com.sun.star.text.TextField.DocInfo.Description",      SW_SERVICE_FIELDTYPE_DOCINFO_DESCRIPTION },
    { "com.sun.star.text.TextField.DocInfo.CreateAuthor",     SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_AUTHOR },
    { "com.sun.star.text.TextField.DocInfo.CreateDateTime",   SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_0 },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_1 },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_2 },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_3 },
    { "com.sun.star.text.TextField.DocInfo.Custom",           SW_SERVICE_FIELDTYPE_DOCINFO_CUSTOM },
    { "com.sun.star.text.TextField.DocInfo.PrintAuthor",      SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_AUTHOR },
    { "com.sun.star.text.TextField.DocInfo.PrintDateTime",    SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME },
    { "com.sun.star.text.TextField.DocInfo.KeyWords",         SW_SERVICE_FIELDTYPE_DOCINFO_KEY_WORDS },
    { "com.sun.star.text.TextField.DocInfo.Subject",          SW_SERVICE_FIELDTYPE_DOCINFO_SUBJECT },
    { "com.sun.star.text.TextField.DocInfo.Title",            SW_SERVICE_FIELDTYPE_DOCINFO_TITLE },
    { "com.sun.star.text.TextField.DocInfo.Revision",         SW_SERVICE_FIELDTYPE_DOCINFO_REVISION },
    { "com.sun.star.text.TextField.Bibliography",             SW_SERVICE_FIELDTYPE_BIBLIOGRAPHY },
    { "com.sun.star.text.TextField.CombinedCharacters",       SW_SERVICE_FIELDTYPE_COMBINED_CHARACTERS },
    { "com.sun.star.text.TextField.DropDown",                 SW_SERVICE_FIELDTYPE_DROPDOWN },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_4 },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_5 },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_6 },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_7 },
    { "",                                                     SW_SERVICE_FIELDTYPE_DUMMY_8 },
    { "com.sun.star.text.FieldMaster.User",                   SW_SERVICE_FIELDMASTER_USER },
    { "com.sun.star.text.FieldMaster.DDE",                    SW_SERVICE_FIELDMASTER_DDE },
    { "com.sun.star.text.FieldMaster.SetExpression",          SW_SERVICE_FIELDMASTER_SET_EXP },
    { "com.sun.star.text.FieldMaster.Database",               SW_SERVICE_FIELDMASTER_DATABASE },
    { "com.sun.star.text.FieldMaster.Bibliography",           SW_SERVICE_FIELDMASTER_BIBLIOGRAPHY },
    { "",                                                     SW_SERVICE_FIELDMASTER_DUMMY2 },
    { "",                                                     SW_SERVICE_FIELDMASTER_DUMMY3 },
    { "",                                                     SW_SERVICE_FIELDMASTER_DUMMY4 },
    { "",                                                     SW_SERVICE_FIELDMASTER_DUMMY5 },
    { "com.sun.star.text.IllustrationsIndex",                 SW_SERVICE_INDEX_ILLUSTRATIONS },
    { "com.sun.star.text.ObjectIndex",                        SW_SERVICE_INDEX_OBJECTS },
    { "com.sun.star.text.TableIndex",                         SW_SERVICE_INDEX_TABLES },
    { "com.sun.star.text.Bibliography",                       SW_SERVICE_INDEX_BIBLIOGRAPHY },
    { "com.sun.star.text.Paragraph",                          SW_SERVICE_PARAGRAPH },
    { "com.sun.star.text.TextField.InputUser",                SW_SERVICE_FIELDTYPE_INPUT_USER },
    { "com.sun.star.text.TextField.HiddenText",               SW_SERVICE_FIELDTYPE_HIDDEN_TEXT },
    { "com.sun.star.style.ConditionalParagraphStyle",         SW_SERVICE_STYLE_CONDITIONAL_PARAGRAPH_STYLE },
    { "com.sun.star.text.NumberingRules",                     SW_SERVICE_NUMBERING_RULES },
    { "com.sun.star.text.TextColumns",                        SW_SERVICE_TEXT_COLUMNS },
    { "com.sun.star.text.IndexHeaderSection",                 SW_SERVICE_INDEX_HEADER_SECTION },
    { "com.sun.star.text.Defaults",                           SW_SERVICE_DEFAULTS },
    { "com.sun.star.image.ImageMapRectangleObject",           SW_SERVICE_IMAP_RECTANGLE },
    { "com.sun.star.image.ImageMapCircleObject",              SW_SERVICE_IMAP_CIRCLE },
    { "com.sun.star.image.ImageMapPolygonObject",             SW_SERVICE_IMAP_POLYGON },
    { "com.sun.star.text.TextGraphicObject",                  SW_SERVICE_TYPE_TEXT_GRAPHIC },
    { "com.sun.star.chart2.data.DataProvider",                SW_SERVICE_CHART2_DATA_PROVIDER },

    // case-correct versions of the service names (see #i67811)
    { CSS_TEXT_TEXTFIELD_DATE_TIME,                   SW_SERVICE_FIELDTYPE_DATETIME },
    { CSS_TEXT_TEXTFIELD_USER,                        SW_SERVICE_FIELDTYPE_USER },
    { CSS_TEXT_TEXTFIELD_SET_EXPRESSION,              SW_SERVICE_FIELDTYPE_SET_EXP },
    { CSS_TEXT_TEXTFIELD_GET_EXPRESSION,              SW_SERVICE_FIELDTYPE_GET_EXP },
    { CSS_TEXT_TEXTFIELD_FILE_NAME,                   SW_SERVICE_FIELDTYPE_FILE_NAME },
    { CSS_TEXT_TEXTFIELD_PAGE_NUMBER,                 SW_SERVICE_FIELDTYPE_PAGE_NUM },
    { CSS_TEXT_TEXTFIELD_AUTHOR,                      SW_SERVICE_FIELDTYPE_AUTHOR },
    { CSS_TEXT_TEXTFIELD_CHAPTER,                     SW_SERVICE_FIELDTYPE_CHAPTER },
    { CSS_TEXT_TEXTFIELD_GET_REFERENCE,               SW_SERVICE_FIELDTYPE_GET_REFERENCE },
    { CSS_TEXT_TEXTFIELD_CONDITIONAL_TEXT,            SW_SERVICE_FIELDTYPE_CONDITIONED_TEXT },
    { CSS_TEXT_TEXTFIELD_ANNOTATION,                  SW_SERVICE_FIELDTYPE_ANNOTATION },
    { CSS_TEXT_TEXTFIELD_INPUT,                       SW_SERVICE_FIELDTYPE_INPUT },
    { CSS_TEXT_TEXTFIELD_MACRO,                       SW_SERVICE_FIELDTYPE_MACRO },
    { CSS_TEXT_TEXTFIELD_DDE,                         SW_SERVICE_FIELDTYPE_DDE },
    { CSS_TEXT_TEXTFIELD_HIDDEN_PARAGRAPH,            SW_SERVICE_FIELDTYPE_HIDDEN_PARA },
    { CSS_TEXT_TEXTFIELD_TEMPLATE_NAME,               SW_SERVICE_FIELDTYPE_TEMPLATE_NAME },
    { CSS_TEXT_TEXTFIELD_EXTENDED_USER,               SW_SERVICE_FIELDTYPE_USER_EXT },
    { CSS_TEXT_TEXTFIELD_REFERENCE_PAGE_SET,          SW_SERVICE_FIELDTYPE_REF_PAGE_SET },
    { CSS_TEXT_TEXTFIELD_REFERENCE_PAGE_GET,          SW_SERVICE_FIELDTYPE_REF_PAGE_GET },
    { CSS_TEXT_TEXTFIELD_JUMP_EDIT,                   SW_SERVICE_FIELDTYPE_JUMP_EDIT },
    { CSS_TEXT_TEXTFIELD_SCRIPT,                      SW_SERVICE_FIELDTYPE_SCRIPT },
    { CSS_TEXT_TEXTFIELD_DATABASE_NEXT_SET,           SW_SERVICE_FIELDTYPE_DATABASE_NEXT_SET },
    { CSS_TEXT_TEXTFIELD_DATABASE_NUMBER_OF_SET,      SW_SERVICE_FIELDTYPE_DATABASE_NUM_SET },
    { CSS_TEXT_TEXTFIELD_DATABASE_SET_NUMBER,         SW_SERVICE_FIELDTYPE_DATABASE_SET_NUM },
    { CSS_TEXT_TEXTFIELD_DATABASE,                    SW_SERVICE_FIELDTYPE_DATABASE },
    { CSS_TEXT_TEXTFIELD_DATABASE_NAME,               SW_SERVICE_FIELDTYPE_DATABASE_NAME },
    { CSS_TEXT_TEXTFIELD_TABLE_FORMULA,               SW_SERVICE_FIELDTYPE_TABLE_FORMULA },
    { CSS_TEXT_TEXTFIELD_PAGE_COUNT,                  SW_SERVICE_FIELDTYPE_PAGE_COUNT },
    { CSS_TEXT_TEXTFIELD_PARAGRAPH_COUNT,             SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT },
    { CSS_TEXT_TEXTFIELD_WORD_COUNT,                  SW_SERVICE_FIELDTYPE_WORD_COUNT },
    { CSS_TEXT_TEXTFIELD_CHARACTER_COUNT,             SW_SERVICE_FIELDTYPE_CHARACTER_COUNT },
    { CSS_TEXT_TEXTFIELD_TABLE_COUNT,                 SW_SERVICE_FIELDTYPE_TABLE_COUNT },
    { CSS_TEXT_TEXTFIELD_GRAPHIC_OBJECT_COUNT,        SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT },
    { CSS_TEXT_TEXTFIELD_EMBEDDED_OBJECT_COUNT,       SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT },
    { CSS_TEXT_TEXTFIELD_DOCINFO_CHANGE_AUTHOR,       SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR },
    { CSS_TEXT_TEXTFIELD_DOCINFO_CHANGE_DATE_TIME,    SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME },
    { CSS_TEXT_TEXTFIELD_DOCINFO_EDIT_TIME,           SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME },
    { CSS_TEXT_TEXTFIELD_DOCINFO_DESCRIPTION,         SW_SERVICE_FIELDTYPE_DOCINFO_DESCRIPTION },
    { CSS_TEXT_TEXTFIELD_DOCINFO_CREATE_AUTHOR,       SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_AUTHOR },
    { CSS_TEXT_TEXTFIELD_DOCINFO_CREATE_DATE_TIME,    SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME },
    { CSS_TEXT_TEXTFIELD_DOCINFO_PRINT_AUTHOR,        SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_AUTHOR },
    { CSS_TEXT_TEXTFIELD_DOCINFO_PRINT_DATE_TIME,     SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME },
    { CSS_TEXT_TEXTFIELD_DOCINFO_KEY_WORDS,           SW_SERVICE_FIELDTYPE_DOCINFO_KEY_WORDS },
    { CSS_TEXT_TEXTFIELD_DOCINFO_SUBJECT,             SW_SERVICE_FIELDTYPE_DOCINFO_SUBJECT },
    { CSS_TEXT_TEXTFIELD_DOCINFO_TITLE,               SW_SERVICE_FIELDTYPE_DOCINFO_TITLE },
    { CSS_TEXT_TEXTFIELD_DOCINFO_REVISION,            SW_SERVICE_FIELDTYPE_DOCINFO_REVISION },
    { CSS_TEXT_TEXTFIELD_DOCINFO_CUSTOM,			  SW_SERVICE_FIELDTYPE_DOCINFO_CUSTOM },
    { CSS_TEXT_TEXTFIELD_BIBLIOGRAPHY,                SW_SERVICE_FIELDTYPE_BIBLIOGRAPHY },
    { CSS_TEXT_TEXTFIELD_COMBINED_CHARACTERS,         SW_SERVICE_FIELDTYPE_COMBINED_CHARACTERS },
    { CSS_TEXT_TEXTFIELD_DROP_DOWN,                   SW_SERVICE_FIELDTYPE_DROPDOWN },
    { CSS_TEXT_TEXTFIELD_INPUT_USER,                  SW_SERVICE_FIELDTYPE_INPUT_USER },
    { CSS_TEXT_TEXTFIELD_HIDDEN_TEXT,                 SW_SERVICE_FIELDTYPE_HIDDEN_TEXT },
    { CSS_TEXT_FIELDMASTER_USER,                      SW_SERVICE_FIELDMASTER_USER },
    { CSS_TEXT_FIELDMASTER_DDE,                       SW_SERVICE_FIELDMASTER_DDE },
    { CSS_TEXT_FIELDMASTER_SET_EXPRESSION,            SW_SERVICE_FIELDMASTER_SET_EXP },
    { CSS_TEXT_FIELDMASTER_DATABASE,                  SW_SERVICE_FIELDMASTER_DATABASE },
    { CSS_TEXT_FIELDMASTER_BIBLIOGRAPHY,              SW_SERVICE_FIELDMASTER_BIBLIOGRAPHY }
};

/* -----------------------------23.03.01 13:38--------------------------------

 ---------------------------------------------------------------------------*/
const SvEventDescription* lcl_GetSupportedMacroItems()
{
    static const SvEventDescription aMacroDescriptionsImpl[] =
    {
        { SFX_EVENT_MOUSEOVER_OBJECT, "OnMouseOver" },
        { SFX_EVENT_MOUSEOUT_OBJECT, "OnMouseOut" },
        { 0, NULL }
    };

    return aMacroDescriptionsImpl;
}

/******************************************************************
 * SwXServiceProvider
 ******************************************************************/
/*-- 13.01.99 13:31:44---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString	SwXServiceProvider::GetProviderName(sal_uInt16 nObjectType)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    OUString sRet;
    sal_uInt16 nEntries = sizeof(aProvNamesId) / sizeof(aProvNamesId[0]);
    if(nObjectType < nEntries)
        sRet = C2U(aProvNamesId[nObjectType].pName);
    return sRet;
}
/* -----------------11.03.99 12:05-------------------
 *
 * --------------------------------------------------*/
uno::Sequence<OUString> 	SwXServiceProvider::GetAllServiceNames()
{
    sal_uInt16 nEntries = sizeof(aProvNamesId) / sizeof(aProvNamesId[0]);
    uno::Sequence<OUString> aRet(nEntries);
    OUString* pArray = aRet.getArray();
    sal_uInt16 n = 0;
    for(sal_uInt16 i = 0; i < nEntries; i++)
    {
        String sProv(C2U(aProvNamesId[i].pName));
        if(sProv.Len())
        {
            pArray[n] = sProv;
            n++;
        }
    }
    aRet.realloc(n);
    return aRet;

}

/*-- 13.01.99 13:31:45---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_uInt16	SwXServiceProvider::GetProviderType(const OUString& rServiceName)
{
    sal_uInt16 nEntries = sizeof(aProvNamesId) / sizeof(aProvNamesId[0]);
    for(sal_uInt16 i = 0; i < nEntries; i++ )
    {
        if( COMPARE_EQUAL == rServiceName.compareToAscii(aProvNamesId[i].pName))
            return aProvNamesId[i].nType;
    }
    return SW_SERVICE_INVALID;
}
/* -----------------13.01.99 14:37-------------------
 *
 * --------------------------------------------------*/
uno::Reference< uno::XInterface >  	SwXServiceProvider::MakeInstance(sal_uInt16 nObjectType, SwDoc* pDoc)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< uno::XInterface >  xRet;
    switch(nObjectType)
    {
        case  SW_SERVICE_TYPE_TEXTTABLE:
        {
            SwXTextTable* pTextTable = new SwXTextTable();
            xRet =  (cppu::OWeakObject*)pTextTable;
        }
        break;
        case  SW_SERVICE_TYPE_TEXTFRAME:
        {
            SwXTextFrame* pTextFrame = new SwXTextFrame( pDoc );
            xRet =  (cppu::OWeakObject*)(SwXFrame*)pTextFrame;
        }
        break;
        case  SW_SERVICE_TYPE_GRAPHIC  :
        case  SW_SERVICE_TYPE_TEXT_GRAPHIC /* #i47503# */ :
        {
            SwXTextGraphicObject* pGraphic = new SwXTextGraphicObject( pDoc );
            xRet =  (cppu::OWeakObject*)(SwXFrame*)pGraphic;

        }
        break;
        case  SW_SERVICE_TYPE_OLE      :
        {
            SwXTextEmbeddedObject* pOle = new SwXTextEmbeddedObject( pDoc );
            xRet =  (cppu::OWeakObject*)(SwXFrame*)pOle;
        }
        break;
        case  SW_SERVICE_TYPE_BOOKMARK :
        {
            SwXBookmark* pBookmark = new SwXBookmark;
            xRet =  (cppu::OWeakObject*)pBookmark;
        }
        break;
        case  SW_SERVICE_TYPE_FOOTNOTE :
            xRet =  (cppu::OWeakObject*)new SwXFootnote(sal_False);
        break;
        case  SW_SERVICE_TYPE_ENDNOTE  :
            xRet =  (cppu::OWeakObject*)new SwXFootnote(sal_True);
        break;
        case  SW_SERVICE_CONTENT_INDEX_MARK :
        case  SW_SERVICE_USER_INDEX_MARK    :
        case  SW_SERVICE_TYPE_INDEXMARK:
        {
            TOXTypes eType = TOX_INDEX;
            if(SW_SERVICE_CONTENT_INDEX_MARK== nObjectType)
                eType = TOX_CONTENT;
            else if(SW_SERVICE_USER_INDEX_MARK == nObjectType)
                eType = TOX_USER;
            xRet =  (cppu::OWeakObject*)new SwXDocumentIndexMark(eType);
        }
        break;
        case  SW_SERVICE_CONTENT_INDEX      :
        case  SW_SERVICE_USER_INDEX         :
        case  SW_SERVICE_TYPE_INDEX    :
        case SW_SERVICE_INDEX_ILLUSTRATIONS:
        case SW_SERVICE_INDEX_OBJECTS      :
        case SW_SERVICE_INDEX_TABLES:
        case SW_SERVICE_INDEX_BIBLIOGRAPHY :
        {
            TOXTypes eType = TOX_INDEX;
            if(SW_SERVICE_CONTENT_INDEX == nObjectType)
                eType = TOX_CONTENT;
            else if(SW_SERVICE_USER_INDEX == nObjectType)
                eType = TOX_USER;
            else if(SW_SERVICE_INDEX_ILLUSTRATIONS == nObjectType)
            {
                eType = TOX_ILLUSTRATIONS;
            }
            else if(SW_SERVICE_INDEX_OBJECTS       == nObjectType)
            {
                eType = TOX_OBJECTS;
            }
            else if(SW_SERVICE_INDEX_BIBLIOGRAPHY  == nObjectType)
            {
                eType = TOX_AUTHORITIES;
            }
            else if(SW_SERVICE_INDEX_TABLES == nObjectType)
            {
                eType = TOX_TABLES;
            }
            xRet =  (cppu::OWeakObject*)new SwXDocumentIndex(eType, *pDoc);
        }
        break;
        case SW_SERVICE_INDEX_HEADER_SECTION :
        case SW_SERVICE_TEXT_SECTION :
            xRet = SwXTextSectionClient::CreateXTextSection( 0, SW_SERVICE_INDEX_HEADER_SECTION == nObjectType);

        break;
        case SW_SERVICE_REFERENCE_MARK :
            xRet =  (cppu::OWeakObject*)new SwXReferenceMark(0, 0);
        break;
        case SW_SERVICE_STYLE_CHARACTER_STYLE:
        case SW_SERVICE_STYLE_PARAGRAPH_STYLE:
        case SW_SERVICE_STYLE_CONDITIONAL_PARAGRAPH_STYLE:
        case SW_SERVICE_STYLE_FRAME_STYLE:
        case SW_SERVICE_STYLE_PAGE_STYLE:
        case SW_SERVICE_STYLE_NUMBERING_STYLE:
        {
            SfxStyleFamily 	eFamily = SFX_STYLE_FAMILY_CHAR;
            switch(nObjectType)
            {
                case SW_SERVICE_STYLE_PARAGRAPH_STYLE:
                case SW_SERVICE_STYLE_CONDITIONAL_PARAGRAPH_STYLE:
                    eFamily = SFX_STYLE_FAMILY_PARA;
                break;
                case SW_SERVICE_STYLE_FRAME_STYLE:
                    eFamily = SFX_STYLE_FAMILY_FRAME;
                break;
                case SW_SERVICE_STYLE_PAGE_STYLE:
                    eFamily = SFX_STYLE_FAMILY_PAGE;
                break;
                case SW_SERVICE_STYLE_NUMBERING_STYLE:
                    eFamily = SFX_STYLE_FAMILY_PSEUDO;
                break;
            }
            SwXStyle* pNewStyle = SFX_STYLE_FAMILY_PAGE == eFamily ?
                new SwXPageStyle(pDoc->GetDocShell()) :
                    eFamily == SFX_STYLE_FAMILY_FRAME ?
                        new SwXFrameStyle ( pDoc ):
                            new SwXStyle( pDoc, eFamily, nObjectType == SW_SERVICE_STYLE_CONDITIONAL_PARAGRAPH_STYLE);
            xRet = (cppu::OWeakObject*)pNewStyle;
        }
        break;
//		SW_SERVICE_DUMMY_5
//		SW_SERVICE_DUMMY_6
//		SW_SERVICE_DUMMY_7
//		SW_SERVICE_DUMMY_8
//		SW_SERVICE_DUMMY_9
        case SW_SERVICE_FIELDTYPE_DATETIME:
        case SW_SERVICE_FIELDTYPE_USER:
        case SW_SERVICE_FIELDTYPE_SET_EXP:
        case SW_SERVICE_FIELDTYPE_GET_EXP:
        case SW_SERVICE_FIELDTYPE_FILE_NAME:
        case SW_SERVICE_FIELDTYPE_PAGE_NUM:
        case SW_SERVICE_FIELDTYPE_AUTHOR:
        case SW_SERVICE_FIELDTYPE_CHAPTER:
        case SW_SERVICE_FIELDTYPE_GET_REFERENCE:
        case SW_SERVICE_FIELDTYPE_CONDITIONED_TEXT:
        case SW_SERVICE_FIELDTYPE_INPUT:
        case SW_SERVICE_FIELDTYPE_MACRO:
        case SW_SERVICE_FIELDTYPE_DDE:
        case SW_SERVICE_FIELDTYPE_HIDDEN_PARA:
        case SW_SERVICE_FIELDTYPE_DOC_INFO:
        case SW_SERVICE_FIELDTYPE_TEMPLATE_NAME:
        case SW_SERVICE_FIELDTYPE_USER_EXT:
        case SW_SERVICE_FIELDTYPE_REF_PAGE_SET:
        case SW_SERVICE_FIELDTYPE_REF_PAGE_GET:
        case SW_SERVICE_FIELDTYPE_JUMP_EDIT:
        case SW_SERVICE_FIELDTYPE_SCRIPT:
        case SW_SERVICE_FIELDTYPE_DATABASE_NEXT_SET:
        case SW_SERVICE_FIELDTYPE_DATABASE_NUM_SET:
        case SW_SERVICE_FIELDTYPE_DATABASE_SET_NUM:
        case SW_SERVICE_FIELDTYPE_DATABASE:
        case SW_SERVICE_FIELDTYPE_DATABASE_NAME:
        case SW_SERVICE_FIELDTYPE_PAGE_COUNT      :
        case SW_SERVICE_FIELDTYPE_PARAGRAPH_COUNT :
        case SW_SERVICE_FIELDTYPE_WORD_COUNT      :
        case SW_SERVICE_FIELDTYPE_CHARACTER_COUNT :
        case SW_SERVICE_FIELDTYPE_TABLE_COUNT     :
        case SW_SERVICE_FIELDTYPE_GRAPHIC_OBJECT_COUNT    :
        case SW_SERVICE_FIELDTYPE_EMBEDDED_OBJECT_COUNT   :
        case SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_AUTHOR		:
        case SW_SERVICE_FIELDTYPE_DOCINFO_CHANGE_DATE_TIME  :
        case SW_SERVICE_FIELDTYPE_DOCINFO_EDIT_TIME         :
        case SW_SERVICE_FIELDTYPE_DOCINFO_DESCRIPTION       :
        case SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_AUTHOR     :
        case SW_SERVICE_FIELDTYPE_DOCINFO_CREATE_DATE_TIME  :
        case SW_SERVICE_FIELDTYPE_DOCINFO_CUSTOM            :
        case SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_AUTHOR      :
        case SW_SERVICE_FIELDTYPE_DOCINFO_PRINT_DATE_TIME   :
        case SW_SERVICE_FIELDTYPE_DOCINFO_KEY_WORDS         :
        case SW_SERVICE_FIELDTYPE_DOCINFO_SUBJECT           :
        case SW_SERVICE_FIELDTYPE_DOCINFO_TITLE             :
        case SW_SERVICE_FIELDTYPE_DOCINFO_REVISION          :
        case SW_SERVICE_FIELDTYPE_BIBLIOGRAPHY:
        case SW_SERVICE_FIELDTYPE_INPUT_USER				:
        case SW_SERVICE_FIELDTYPE_HIDDEN_TEXT				:
        case SW_SERVICE_FIELDTYPE_COMBINED_CHARACTERS		:
        case SW_SERVICE_FIELDTYPE_DROPDOWN                  :
        case SW_SERVICE_FIELDTYPE_TABLE_FORMULA:
            xRet = (cppu::OWeakObject*)new SwXTextField(nObjectType);
            break;
        case SW_SERVICE_FIELDTYPE_ANNOTATION:
            xRet = (cppu::OWeakObject*)new SwXTextField(nObjectType, pDoc);
            break;
        case SW_SERVICE_FIELDMASTER_USER:
        case SW_SERVICE_FIELDMASTER_DDE:
        case SW_SERVICE_FIELDMASTER_SET_EXP :
        case SW_SERVICE_FIELDMASTER_DATABASE:
        {
            sal_uInt16 nResId = USHRT_MAX;
            switch(nObjectType)
            {
                case SW_SERVICE_FIELDMASTER_USER: nResId = RES_USERFLD; break;
                case SW_SERVICE_FIELDMASTER_DDE:  nResId = RES_DDEFLD; break;
                case SW_SERVICE_FIELDMASTER_SET_EXP : nResId = RES_SETEXPFLD; break;
                case SW_SERVICE_FIELDMASTER_DATABASE: nResId = RES_DBFLD; break;
            }
            xRet =  (cppu::OWeakObject*)new SwXFieldMaster(pDoc, nResId);
        }
        break;
        case SW_SERVICE_FIELDMASTER_BIBLIOGRAPHY:
        {
            SwFieldType* pType = pDoc->GetFldType(RES_AUTHORITY, aEmptyStr, sal_True);
            if(!pType)
            {
                SwAuthorityFieldType aType(pDoc);
                pType = pDoc->InsertFldType(aType);
            }
            else
            {
                SwClientIter aIter( *pType );
                SwXFieldMaster* pMaster = (SwXFieldMaster*)aIter.First( TYPE( SwXFieldMaster ));
                if(pMaster)
                    xRet = (cppu::OWeakObject*)pMaster;
            }
            if(!xRet.is())
                xRet =  (cppu::OWeakObject*)new SwXFieldMaster(*pType, pDoc);
        }
        break;
        case SW_SERVICE_PARAGRAPH :
            xRet = (cppu::OWeakObject*)new SwXParagraph();
        break;
        case SW_SERVICE_NUMBERING_RULES :
            xRet = (cppu::OWeakObject*)new SwXNumberingRules(*pDoc);
        break;
        case SW_SERVICE_TEXT_COLUMNS :
            xRet = (cppu::OWeakObject*)new SwXTextColumns(0);
        break;
        case SW_SERVICE_DEFAULTS:
            xRet = (cppu::OWeakObject*)new SwXTextDefaults( pDoc );
        break;
        case SW_SERVICE_IMAP_RECTANGLE :
            xRet = SvUnoImageMapRectangleObject_createInstance( lcl_GetSupportedMacroItems() );
        break;
        case SW_SERVICE_IMAP_CIRCLE    :
            xRet = SvUnoImageMapCircleObject_createInstance( lcl_GetSupportedMacroItems() );
        break;
        case SW_SERVICE_IMAP_POLYGON   :
            xRet = SvUnoImageMapPolygonObject_createInstance( lcl_GetSupportedMacroItems() );
        break;
        case SW_SERVICE_CHART2_DATA_PROVIDER :
            // #i64497# If a chart is in a temporary document during clipoard
            // paste, there should be no data provider, so that own data is used
            // This should not happen during copy/paste, as this will unlink
            // charts using table data.
            OSL_ASSERT( pDoc->GetDocShell()->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED );
            if( pDoc->GetDocShell()->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
                xRet = (cppu::OWeakObject*) pDoc->GetChartDataProvider( true /* create - if not yet available */ );
        break;
        default:
            throw uno::RuntimeException();
    }
    return xRet;
}
/******************************************************************
 * SwXTextTables
 ******************************************************************/
//SMART_UNO_IMPLEMENTATION( SwXTextTables, UsrObject );

/*-- 13.01.99 12:56:24---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextTables::SwXTextTables(SwDoc* pDc) :
        SwUnoCollection(pDc)
{

}
/*-- 13.01.99 12:56:25---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextTables::~SwXTextTables()
{

}
/*-- 13.01.99 12:56:25---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXTextTables::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Int32 nRet = 0;
    if(IsValid())
        nRet = GetDoc()->GetTblFrmFmtCount(sal_True);
    return nRet;
}
/*-- 13.01.99 12:56:26---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL SwXTextTables::getByIndex(sal_Int32 nIndex)
        throw( IndexOutOfBoundsException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        if(0 <= nIndex && GetDoc()->GetTblFrmFmtCount(sal_True) > nIndex)
        {
            SwFrmFmt& rFmt = GetDoc()->GetTblFrmFmt( static_cast< xub_StrLen >(nIndex), sal_True);
            uno::Reference< XTextTable >  xTbl = SwXTextTables::GetObject(rFmt);
            aRet.setValue( &xTbl,
                ::getCppuType((uno::Reference< XTextTable>*)0));
        }
        else
            throw IndexOutOfBoundsException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 13.01.99 12:56:26---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextTables::getByName(const OUString& rItemName)
    throw( NoSuchElementException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        sal_uInt16 nCount = GetDoc()->GetTblFrmFmtCount(sal_True);
        uno::Reference< XTextTable >  xTbl;
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            String aName(rItemName);
            SwFrmFmt& rFmt = GetDoc()->GetTblFrmFmt(i, sal_True);
            if(aName == rFmt.GetName())
            {
                xTbl = SwXTextTables::GetObject(rFmt);
                aRet.setValue(&xTbl,
                    ::getCppuType(( uno::Reference< XTextTable >*)0));
                break;
            }
        }
        if(!xTbl.is())
            throw NoSuchElementException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 13.01.99 12:56:26---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXTextTables::getElementNames(void)
        throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    sal_uInt16 nCount = GetDoc()->GetTblFrmFmtCount(sal_True);
    uno::Sequence<OUString> aSeq(nCount);
    if(nCount)
    {
        OUString* pArray = aSeq.getArray();
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            SwFrmFmt& rFmt = GetDoc()->GetTblFrmFmt(i, sal_True);

            pArray[i] = OUString(rFmt.GetName());
        }
    }
    return aSeq;
}
/*-- 13.01.99 12:56:27---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextTables::hasByName(const OUString& rName)
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet= sal_False;
    if(IsValid())
    {
        sal_uInt16 nCount = GetDoc()->GetTblFrmFmtCount(sal_True);
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            String aName(rName);
            SwFrmFmt& rFmt = GetDoc()->GetTblFrmFmt(i, sal_True);
            if(aName == rFmt.GetName())
            {
                bRet = sal_True;
                break;
            }
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 13.01.99 12:56:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL
    SwXTextTables::getElementType(  )
        throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<XTextTable>*)0);
}
/*-- 13.01.99 12:56:27---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextTables::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return 0 != GetDoc()->GetTblFrmFmtCount(sal_True);
}
/* -----------------25.10.99 16:01-------------------

 --------------------------------------------------*/
OUString SwXTextTables::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SwXTextTables");
}
/* -----------------25.10.99 16:01-------------------

 --------------------------------------------------*/
sal_Bool SwXTextTables::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return rServiceName == C2U("com.sun.star.text.TextTables");
}
/* -----------------25.10.99 16:01-------------------

 --------------------------------------------------*/
uno::Sequence< OUString > SwXTextTables::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aRet(1);
    OUString* pArr = aRet.getArray();
    pArr[0] = C2U("com.sun.star.text.TextTables");
    return aRet;
}
/*-- 13.01.99 12:56:28---------------------------------------------------

  -----------------------------------------------------------------------*/
XTextTable* SwXTextTables::GetObject( SwFrmFmt& rFmt )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwXTextTable* pTbl = (SwXTextTable*)SwClientIter( rFmt ).
                                    First( TYPE( SwXTextTable ));
    if( !pTbl )
        pTbl = new SwXTextTable(rFmt);
    return pTbl ;
}
/******************************************************************
 *	SwXFrames
 ******************************************************************/
/* -----------------------------06.04.00 12:41--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXFrames::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXFrames");
}
/* -----------------------------06.04.00 12:41--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXFrames::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextFrames") == rServiceName;
}
/* -----------------------------06.04.00 12:41--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXFrames::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextFrames");
    return aRet;
}

/*-- 14.01.99 08:20:18---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFrames::SwXFrames(SwDoc* _pDoc, FlyCntType eSet) :
    SwUnoCollection(_pDoc),
    eType(eSet)
{
}

/*-- 14.01.99 08:20:18---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFrames::~SwXFrames()
{
}

/*-- 14.01.99 08:25:47---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXFrames::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Int32 nRet = 0;
    if(IsValid())
        nRet = GetDoc()->GetFlyCount(eType);
    else
        throw uno::RuntimeException();
    return nRet;
}
/*-- 14.01.99 08:25:47---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXFrames::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        sal_uInt16 nCount = GetDoc()->GetFlyCount(eType);
        if( 0 <= nIndex && nIndex < USHRT_MAX && nCount > nIndex)
        {
            SwFrmFmt* pFmt = GetDoc()->GetFlyNum((sal_uInt16) nIndex, eType);
            SwXFrame* pFrm = SwXFrames::GetObject(*pFmt, eType);
            switch(eType)
            {
                case FLYCNTTYPE_FRM:
                {
                    uno::Reference< XTextFrame >  xRef = (SwXTextFrame*)pFrm;
                    aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextFrame>*)0));
                }
                break;
                case FLYCNTTYPE_GRF:
                {
                    Reference< XTextContent >  xRef = (SwXTextGraphicObject*)pFrm;
                    aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextContent>*)0));
                }
                break;
                case FLYCNTTYPE_OLE:
                {
                    Reference< XEmbeddedObjectSupplier >  xRef = (SwXTextEmbeddedObject*)pFrm;
                    aRet.setValue(&xRef, ::getCppuType((Reference<XEmbeddedObjectSupplier>*)0));
                }
                break;

                default:
                    ;
            }
        }
        else
            throw IndexOutOfBoundsException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 14.01.99 08:25:47---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXFrames::getByName(const OUString& rName)
    throw( NoSuchElementException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwXFrame* pFrm = 0;
    if(IsValid())
    {
        sal_uInt16 nCount = GetDoc()->GetFlyCount(eType);
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            String aName(rName);
            SwFrmFmt* pFmt = GetDoc()->GetFlyNum(i, eType);
            if(aName == pFmt->GetName() )
            {
                pFrm = SwXFrames::GetObject(*pFmt, eType);
                break;
            }
        }
    }
    else
        throw uno::RuntimeException();
    if(!pFrm)
        throw NoSuchElementException();
    uno::Any aRet;
    switch(eType)
    {
        case FLYCNTTYPE_FRM:
        {
            uno::Reference< XTextFrame >  xRef = (SwXTextFrame*)pFrm;
            aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextFrame>*)0));
        }
        break;
        case FLYCNTTYPE_GRF:
        {
            uno::Reference< XTextContent >  xRef = (SwXTextGraphicObject*)pFrm;
            aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextContent>*)0));
        }
        break;
        case FLYCNTTYPE_OLE:
        {
            Reference< XEmbeddedObjectSupplier >  xRef = (SwXTextEmbeddedObject*)pFrm;
            aRet.setValue(&xRef, ::getCppuType((Reference<XEmbeddedObjectSupplier>*)0));
        }
        break;

        default:
            ;
    }
    return aRet;
}
/*-- 14.01.99 08:25:48---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXFrames::getElementNames(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    sal_uInt16 nCount = IsValid() ? GetDoc()->GetFlyCount(eType) : 0;
    uno::Sequence<OUString> aSeq(nCount);
    if(nCount)
    {
        OUString* pArray = aSeq.getArray();
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            SwFrmFmt* pFmt = GetDoc()->GetFlyNum(i, eType);
            pArray[i] = pFmt->GetName();
        }
    }
    return aSeq;
}
/*-- 14.01.99 08:25:48---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXFrames::hasByName(const OUString& rName) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();

    BYTE nNodeType = ND_TEXTNODE;
    switch( eType )
    {
        case FLYCNTTYPE_GRF:	nNodeType = ND_GRFNODE; break;
        case FLYCNTTYPE_OLE:	nNodeType = ND_OLENODE; break;
        default:
            ;
    }

    return 0 != GetDoc()->FindFlyByName( rName, nNodeType );
}

/*-- 14.01.99 08:25:48---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL SwXFrames::getElementType() throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Type aRet;
    switch(eType)
    {
        case FLYCNTTYPE_FRM:
            aRet = ::getCppuType((uno::Reference<XTextFrame>*)0);

        break;
        case FLYCNTTYPE_GRF:
            aRet = ::getCppuType((uno::Reference<XTextContent>*)0);

        break;
        case FLYCNTTYPE_OLE:
            aRet = ::getCppuType((uno::Reference<XEmbeddedObjectSupplier>*)0);
        break;
        default:
            ;
    }
    return aRet;
}
/*-- 14.01.99 08:25:48---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXFrames::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_uInt16 nRet = 0;
    if(IsValid())
        nRet = GetDoc()->GetFlyCount(eType);
    else
        throw uno::RuntimeException();
    return nRet != 0;
}
/*-- 14.01.99 08:25:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFrame* 	SwXFrames::GetObject( SwFrmFmt& rFmt, FlyCntType eType )
{
    SwXFrame* pFrm = (SwXFrame*)SwClientIter( rFmt ).
                                    First( TYPE( SwXFrame ));
    if( !pFrm )
    {
        switch(eType)
        {
            case FLYCNTTYPE_FRM:
                pFrm = new SwXTextFrame(rFmt);
            break;
            case FLYCNTTYPE_GRF:
                pFrm = new SwXTextGraphicObject(rFmt);
            break;
            case FLYCNTTYPE_OLE:
                pFrm = new SwXTextEmbeddedObject(rFmt);
            break;
            default:
                ;
        }
    }
    return pFrm;
}

/******************************************************************
 * SwXTextFrames
 ******************************************************************/
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextFrames::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextFrames");
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextFrames::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextFrames") == rServiceName;
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextFrames::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextFrames");
    return aRet;
}
/*-- 14.01.99 08:06:16---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextFrames::SwXTextFrames(SwDoc* _pDoc) :
    SwXFrames(_pDoc, FLYCNTTYPE_FRM)
{
}
/*-- 14.01.99 08:06:17---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextFrames::~SwXTextFrames()
{
}

/******************************************************************
 *	SwXTextGraphicObjects
 ******************************************************************/
//SMART_UNO_IMPLEMENTATION( SwXTextGraphicObjects, UsrObject );

/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextGraphicObjects::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextGraphicObjects");
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextGraphicObjects::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextGraphicObjects") == rServiceName;
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextGraphicObjects::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextGraphicObjects");
    return aRet;
}
/*-- 14.01.99 08:45:53---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextGraphicObjects::SwXTextGraphicObjects(SwDoc* _pDoc) :
    SwXFrames(_pDoc, FLYCNTTYPE_GRF)
{
}
/*-- 14.01.99 08:45:54---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextGraphicObjects::~SwXTextGraphicObjects()
{
}

/******************************************************************
 *	SwXTextEmbeddedObjects
 ******************************************************************/
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextEmbeddedObjects::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextEmbeddedObjects");
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextEmbeddedObjects::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextEmbeddedObjects") == rServiceName;
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextEmbeddedObjects::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextEmbeddedObjects");
    return aRet;
}
/*-- 14.01.99 08:45:13---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextEmbeddedObjects::SwXTextEmbeddedObjects(SwDoc* _pDoc) :
        SwXFrames(_pDoc, FLYCNTTYPE_OLE)
{
}
/*-- 14.01.99 08:45:31---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextEmbeddedObjects::~SwXTextEmbeddedObjects()
{
}

/******************************************************************
 *
 ******************************************************************/
#define PASSWORD_STD_TIMEOUT 1000
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXTextSections::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextSections");
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXTextSections::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextSections") == rServiceName;
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXTextSections::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextSections");
    return aRet;
}
/*-- 14.01.99 09:06:05---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextSections::SwXTextSections(SwDoc* _pDoc) :
    SwUnoCollection(_pDoc)
{
}
/*-- 14.01.99 09:06:05---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXTextSections::~SwXTextSections()
{
}
/*-- 14.01.99 09:06:05---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXTextSections::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    const SwSectionFmts& rSectFmts = GetDoc()->GetSections();
    sal_uInt16 nCount = rSectFmts.Count();
    for(sal_uInt16 i = nCount; i; i--)
    {
        if( !rSectFmts[i - 1]->IsInNodesArr())
            nCount--;
    }
    return nCount;
}
/*-- 14.01.99 09:06:06---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextSections::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference< XTextSection >  xRet;
    if(IsValid())
    {
        SwSectionFmts& rFmts = GetDoc()->GetSections();

        const SwSectionFmts& rSectFmts = GetDoc()->GetSections();
        sal_uInt16 nCount = rSectFmts.Count();
        for(sal_uInt16 i = 0; i < nCount; i++)
        {
            if( !rSectFmts[i]->IsInNodesArr())
                nIndex ++;
            else if(nIndex == i)
                break;
            if(nIndex == i)
                break;
        }
        if(nIndex >= 0 && nIndex < rFmts.Count())
        {
            SwSectionFmt* pFmt = rFmts[(sal_uInt16)nIndex];
            xRet = GetObject(*pFmt);
        }
        else
            throw IndexOutOfBoundsException();
    }
    else
        throw uno::RuntimeException();
    return makeAny(xRet);
}
/*-- 14.01.99 09:06:06---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXTextSections::getByName(const OUString& Name)
    throw( NoSuchElementException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        String aName(Name);
        SwSectionFmts& rFmts = GetDoc()->GetSections();
        uno::Reference< XTextSection >  xSect;
        for(sal_uInt16 i = 0; i < rFmts.Count(); i++)
        {
            SwSectionFmt* pFmt = rFmts[i];
            if(pFmt->IsInNodesArr() && aName == pFmt->GetSection()->GetName())
            {
                xSect = GetObject(*pFmt);
                aRet.setValue(&xSect, ::getCppuType((uno::Reference<XTextSection>*)0));
                break;
            }
        }
        if(!xSect.is())
            throw NoSuchElementException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 14.01.99 09:06:06---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXTextSections::getElementNames(void)
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    sal_uInt16 nCount = GetDoc()->GetSections().Count();
    SwSectionFmts& rSectFmts = GetDoc()->GetSections();
    for(sal_uInt16 i = nCount; i; i--)
    {
        if( !rSectFmts[i - 1]->IsInNodesArr())
            nCount--;
    }

    uno::Sequence<OUString> aSeq(nCount);
    if(nCount)
    {
        SwSectionFmts& rFmts = GetDoc()->GetSections();
        OUString* pArray = aSeq.getArray();
        sal_uInt16 nIndex = 0;
        for( sal_uInt16 i = 0; i < nCount; i++, nIndex++)
        {
            const SwSectionFmt* pFmt = rFmts[nIndex];
            while(!pFmt->IsInNodesArr())
            {
                pFmt = rFmts[++nIndex];
            }
            pArray[i] = pFmt->GetSection()->GetName();
        }
    }
    return aSeq;
}
/*-- 14.01.99 09:06:06---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextSections::hasByName(const OUString& Name)
    throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    String aName(Name);
    if(IsValid())
    {
        SwSectionFmts& rFmts = GetDoc()->GetSections();
        for(sal_uInt16 i = 0; i < rFmts.Count(); i++)
        {
            const SwSectionFmt* pFmt = rFmts[i];
            if(aName == pFmt->GetSection()->GetName())
            {
                bRet = sal_True;
                break;
            }
        }
    }
    else
    {
        //Sonderbehandlung der dbg_ - Methoden
        if( COMPARE_EQUAL != aName.CompareToAscii("dbg_", 4))
            throw uno::RuntimeException();
    }
    return bRet;
}
/*-- 14.01.99 09:06:06---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL SwXTextSections::getElementType() throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<XTextSection>*)0);
}
/*-- 14.01.99 09:06:06---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXTextSections::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_uInt16 nCount = 0;
    if(IsValid())
    {
        SwSectionFmts& rFmts = GetDoc()->GetSections();
        nCount = rFmts.Count();
    }
    else
        throw uno::RuntimeException();
    return nCount > 0;
}
/*-- 14.01.99 09:06:07---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Reference< XTextSection >  SwXTextSections::GetObject( SwSectionFmt& rFmt )
{
    SwXTextSectionClient* pClient = (SwXTextSectionClient*)SwClientIter( rFmt ).
                                    First( TYPE( SwXTextSectionClient ));
    uno::Reference< XTextSection > xRet;
    if( pClient  )
        xRet = pClient->GetXTextSection();
    // it is possible that the client is still registered but the reference is already invalid
    if( !xRet.is() )
        xRet = SwXTextSectionClient::CreateXTextSection(&rFmt);
    return xRet;
}
/******************************************************************
 *
 ******************************************************************/
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXBookmarks::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXBookmarks");
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXBookmarks::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.Bookmarks") == rServiceName;
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXBookmarks::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.Bookmarks");
    return aRet;
}
/*-- 14.01.99 09:05:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXBookmarks::SwXBookmarks(SwDoc* _pDoc) :
    SwUnoCollection(_pDoc)
{
}
/*-- 14.01.99 09:05:48---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXBookmarks::~SwXBookmarks()
{
}
/*-- 14.01.99 09:05:49---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXBookmarks::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return GetDoc()->getBookmarkCount(sal_True);
}
/*-- 14.01.99 09:05:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXBookmarks::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        if(0 <= nIndex && GetDoc()->getBookmarkCount(true) > nIndex)
        {
            SwBookmark& rBkm = GetDoc()->getBookmark((sal_uInt16) nIndex, true);
            uno::Reference< XTextContent >  xRef = GetObject(rBkm, GetDoc());
            aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextContent>*)0));
        }
        else
            throw IndexOutOfBoundsException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 14.01.99 09:05:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXBookmarks::getByName(const rtl::OUString& rName)
    throw( NoSuchElementException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        String aName(rName);
        sal_uInt16 nCount = GetDoc()->getBookmarkCount(true);
        uno::Reference< XTextContent >  xRef;
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            SwBookmark& rBkMk = GetDoc()->getBookmark( i, true );
            if(rBkMk.GetName() == aName)
            {
                xRef = SwXBookmarks::GetObject(rBkMk, GetDoc());
                aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextContent>*)0));
                break;
            }
        }
        if(!xRef.is())
            throw NoSuchElementException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 14.01.99 09:05:49---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXBookmarks::getElementNames(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    sal_uInt16 nCount = GetDoc()->getBookmarkCount(true);
    uno::Sequence<OUString> aSeq(nCount);
    if(nCount)
    {
        OUString* pArray = aSeq.getArray();
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            SwBookmark& rBkMk = GetDoc()->getBookmark( i, true );
            pArray[i] = rBkMk.GetName();
        }
    }
    return aSeq;
}
/*-- 14.01.99 09:05:49---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXBookmarks::hasByName(const OUString& rName) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    if(IsValid())
    {
        String aName(rName);
        sal_uInt16 nCount = GetDoc()->getBookmarkCount(true);
        for( sal_uInt16 i = 0; i < nCount; i++)
        {
            SwBookmark& rBkMk = GetDoc()->getBookmark( i,true);
            if(rBkMk.GetName() == aName)
            {
                bRet = sal_True;
                break;
            }
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}
/*-- 14.01.99 09:05:50---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL SwXBookmarks::getElementType() throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<XTextContent>*)0);
}
/*-- 14.01.99 09:05:50---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXBookmarks::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return GetDoc()->getBookmarkCount(true) != 0;
}
/*-- 14.01.99 09:05:50---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXBookmark* 	SwXBookmarks::GetObject( SwBookmark& rBkm, SwDoc* pDoc )
{
    SwXBookmark* pBkm = (SwXBookmark*)SwClientIter( rBkm ).
                                    First( TYPE( SwXBookmark ));
    if( !pBkm )
        pBkm = new SwXBookmark(&rBkm, pDoc);
    return pBkm;
}
/******************************************************************
 *
 ******************************************************************/

SwXNumberingRulesCollection::SwXNumberingRulesCollection( SwDoc* _pDoc ) :
    SwUnoCollection(_pDoc)
{
}

SwXNumberingRulesCollection::~SwXNumberingRulesCollection()
{
}

sal_Int32 SwXNumberingRulesCollection::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return GetDoc()->GetNumRuleTbl().Count();
}

uno::Any SwXNumberingRulesCollection::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        uno::Reference< XIndexReplace >  xRef;
        if ( nIndex < GetDoc()->GetNumRuleTbl().Count() )
        {
            xRef = new SwXNumberingRules( *GetDoc()->GetNumRuleTbl()[ static_cast< USHORT >(nIndex) ] );
            aRet.setValue(&xRef, ::getCppuType((uno::Reference<XIndexReplace>*)0));
        }

        if(!xRef.is())
            throw IndexOutOfBoundsException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}

uno::Type SAL_CALL SwXNumberingRulesCollection::getElementType() throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<XIndexReplace>*)0);
}

sal_Bool SwXNumberingRulesCollection::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return GetDoc()->GetNumRuleTbl().Count() > 0;
}

/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXFootnotes::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXFootnotes");
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXFootnotes::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.Footnotes") == rServiceName;
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXFootnotes::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.Footnotes");
    return aRet;
}
/*-- 14.01.99 09:03:52---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFootnotes::SwXFootnotes(sal_Bool bEnd, SwDoc* _pDoc) :
    SwUnoCollection(_pDoc),
    bEndnote(bEnd)
{
}
/*-- 14.01.99 09:03:52---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXFootnotes::~SwXFootnotes()
{
}
/*-- 14.01.99 09:03:53---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXFootnotes::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    sal_Int32 nCount = 0;
    sal_uInt16 n, nFtnCnt = GetDoc()->GetFtnIdxs().Count();
    SwTxtFtn* pTxtFtn;
    for( n = 0; n < nFtnCnt; ++n )
    {
        pTxtFtn = GetDoc()->GetFtnIdxs()[ n ];
        const SwFmtFtn& rFtn = pTxtFtn->GetFtn();
        if ( rFtn.IsEndNote() != bEndnote )
            continue;
        nCount++;
    }
    return nCount;
}
/*-- 14.01.99 09:03:53---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXFootnotes::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    sal_Int32 nCount = 0;
    if(IsValid())
    {
        sal_uInt16 n, nFtnCnt = GetDoc()->GetFtnIdxs().Count();
        SwTxtFtn* pTxtFtn;
        uno::Reference< XFootnote >  xRef;
        for( n = 0; n < nFtnCnt; ++n )
        {
            pTxtFtn = GetDoc()->GetFtnIdxs()[ n ];
            const SwFmtFtn& rFtn = pTxtFtn->GetFtn();
            if( rFtn.IsEndNote() != bEndnote )
                continue;

            if(nCount == nIndex)
            {
                xRef = new SwXFootnote(GetDoc(), rFtn);
                aRet.setValue(&xRef, ::getCppuType((uno::Reference<XFootnote>*)0));
                break;
            }
            nCount++;
        }
        if(!xRef.is())
            throw IndexOutOfBoundsException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 14.01.99 09:03:53---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL SwXFootnotes::getElementType() throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<XFootnote>*)0);
}
/*-- 14.01.99 09:03:54---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXFootnotes::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return GetDoc()->GetFtnIdxs().Count() > 0;
}
/* -----------------------------05.09.00 12:48--------------------------------

 ---------------------------------------------------------------------------*/
Reference<XFootnote> 	SwXFootnotes::GetObject( SwDoc& rDoc, const SwFmtFtn& rFmt )
{
    Reference<XTextContent> xContent = ((SwUnoCallBack*)rDoc.GetUnoCallBack())->
                                                            GetFootnote(rFmt);
    if(!xContent.is())
        xContent = new SwXFootnote(&rDoc, rFmt);
    Reference<XFootnote> xRet(xContent, UNO_QUERY);
    return xRet;
}

/******************************************************************
 *
 ******************************************************************/
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXReferenceMarks::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXReferenceMarks");
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXReferenceMarks::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.ReferenceMarks") == rServiceName;
}
/* -----------------------------06.04.00 12:44--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXReferenceMarks::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.ReferenceMarks");
    return aRet;
}
/*-- 14.01.99 09:03:16---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXReferenceMarks::SwXReferenceMarks(SwDoc* _pDoc) :
    SwUnoCollection(_pDoc)
{
}
/*-- 14.01.99 09:03:16---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXReferenceMarks::~SwXReferenceMarks()
{
}
/*-- 14.01.99 09:03:17---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXReferenceMarks::getCount(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return GetDoc()->GetRefMarks();
}
/*-- 14.01.99 09:03:17---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXReferenceMarks::getByIndex(sal_Int32 nIndex)
    throw( IndexOutOfBoundsException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(!IsValid())
        throw uno::RuntimeException();
    uno::Reference< XTextContent >  xRef;
    if(0 <= nIndex && nIndex < USHRT_MAX)
    {
        const SwFmtRefMark* pMark = GetDoc()->GetRefMark( (sal_uInt16) nIndex );
        if(pMark)
        {
            xRef = SwXReferenceMarks::GetObject( GetDoc(), pMark );
            aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextContent>*)0));
        }
    }
    if(!xRef.is())
        throw IndexOutOfBoundsException();
    return aRet;
}
/*-- 14.01.99 09:03:17---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXReferenceMarks::getByName(const OUString& rName)
    throw( NoSuchElementException, WrappedTargetException, uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    if(IsValid())
    {
        const SwFmtRefMark* pMark = GetDoc()->GetRefMark(rName);
        if(pMark)
        {
            uno::Reference< XTextContent >  xRef = SwXReferenceMarks::GetObject( GetDoc(), pMark );
            aRet.setValue(&xRef, ::getCppuType((uno::Reference<XTextContent>*)0));
        }
        else
            throw NoSuchElementException();
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 14.01.99 09:03:17---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXReferenceMarks::getElementNames(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Sequence<OUString> aRet;
    if(IsValid())
    {
        SvStringsDtor aStrings;
        sal_uInt16 nCount = GetDoc()->GetRefMarks( &aStrings );
        aRet.realloc(nCount);
        OUString* pNames = aRet.getArray();
        for(sal_uInt16 i = 0; i < nCount; i++)
            pNames[i] = *aStrings.GetObject(i);
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 14.01.99 09:03:17---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXReferenceMarks::hasByName(const OUString& rName) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return 0 != GetDoc()->GetRefMark( rName);
}
/*-- 14.01.99 09:03:18---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SAL_CALL SwXReferenceMarks::getElementType() throw(uno::RuntimeException)
{
    return ::getCppuType((uno::Reference<XTextContent>*)0);
}
/*-- 14.01.99 09:03:18---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXReferenceMarks::hasElements(void) throw( uno::RuntimeException )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    if(!IsValid())
        throw uno::RuntimeException();
    return 0 !=	GetDoc()->GetRefMarks();
}
/*-- 14.01.99 09:03:19---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXReferenceMark* SwXReferenceMarks::GetObject( SwDoc* pDoc, const SwFmtRefMark* pMark )
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    SwClientIter aIter( *pDoc->GetUnoCallBack() );
    SwXReferenceMark* pxMark = (SwXReferenceMark*)aIter.First( TYPE( SwXReferenceMark ));
    while(pxMark)
    {
        if(pxMark->GetMark() == pMark)
            break;
        pxMark = (SwXReferenceMark*)aIter.Next();
    }
    if( !pxMark )
        pxMark = new SwXReferenceMark(pDoc, pMark);
    return pxMark;
}
/******************************************************************
 *
 ******************************************************************/
/*-----------------11.03.98 11:18-------------------
    Gueltigkeitspruefung
--------------------------------------------------*/
void SwUnoCollection::Invalidate()
{
    bObjectValid = sal_False;
    pDoc = 0;
}

