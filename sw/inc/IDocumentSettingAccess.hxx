/*************************************************************************
 *
 *  $RCSfile: IDocumentSettingAccess.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 15:13:28 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef IDOCUMENTSETTINGACCESS_HXX_INCLUDED
#define IDOCUMENTSETTINGACCESS_HXX_INCLUDED

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _VOS_REF_HXX
#include <vos/ref.hxx>
#endif
#ifndef _CHCMPRSE_HXX
#include <chcmprse.hxx>
#endif

class SvxForbiddenCharactersTable;
namespace com { namespace sun { namespace star { namespace i18n { struct ForbiddenCharacters; } } } }

 /** Provides access to settings of a document
 */
 class IDocumentSettingAccess
 {
 public:
     enum DocumentSettingId
     {
         // COMPATIBILITY FLAGS START
         PARA_SPACE_MAX,
         PARA_SPACE_MAX_AT_PAGES,

         TAB_COMPAT,

         ADD_FLY_OFFSETS,

         OLD_NUMBERING,

         ADD_EXT_LEADING,
         USE_VIRTUAL_DEVICE,
         USE_HIRES_VIRTUAL_DEVICE,
         OLD_LINE_SPACING,
         ADD_PARA_SPACING_TO_TABLE_CELLS,
         USE_FORMER_OBJECT_POS,
         USE_FORMER_TEXT_WRAPPING,
         CONSIDER_WRAP_ON_OBJECT_POSITION,

         IGNORE_FIRST_LINE_INDENT_IN_NUMBERING,
         DO_NOT_JUSTIFY_LINES_WITH_MANUAL_BREAK,
         DO_NOT_RESET_PARA_ATTRS_FOR_NUM_FONT,
         OUTLINE_LEVEL_YIELDS_OUTLINE_RULE,

         DO_NOT_CAPTURE_DRAW_OBJS_ON_PAGE,
         TABLE_ROW_KEEP,
         IGNORE_TABS_AND_BLANKS_FOR_LINE_CALCULATION,
         CLIP_AS_CHARACTER_ANCHORED_WRITER_FLY_FRAME,
         // COMPATIBILITY FLAGS END

         BROWSE_MODE,
         HTML_MODE,
         GLOBAL_DOCUMENT,
         GLOBAL_DOCUMENT_SAVE_LINKS,
         LABEL_DOCUMENT,
         PURGE_OLE,
         KERN_ASIAN_PUNCTUATION
     };

 public:
    /** Return the specified document setting.

       @param id
       [in] the identifier of the document setting to be returned.
       See above for a list of valid document setting identifiers.

       @returns
       the value of the requested document setting.
    */
    virtual bool get(/*[in]*/ DocumentSettingId id) const = 0;

    /** Set the specified document setting.

        @param id
        [in] the identifier of the document setting to be set.
        See above for a list of valid document setting identifiers.

        @param value
        [in] the new value of the specified document setting.
    */
    virtual void set(/*[in]*/ DocumentSettingId id, /*[in]*/ bool value) = 0;

    /** Return the forbidden characters.

       @param nLang
       [in] the language for which the forbidden character list is returned.

       @param bLocaleData
       [in] if set and there is no user defined forbidden character list for
            language lang, the default list for language lang will be returned.

       @returns
       a list of forbidden characters.
    */
    virtual const com::sun::star::i18n::ForbiddenCharacters*
        getForbiddenCharacters(/*[in]*/ USHORT nLang, /*[in]*/ bool bLocaleData ) const = 0;

    /** Set the forbidden characters.

       @param nLang
       [in] the language for which the forbidden character list should be set.

       @param rForbiddenCharacters
       [in] the new list of forbidden characters for language lang.
    */
    virtual void setForbiddenCharacters(/*[in]*/ USHORT nLang,
                                        /*[in]*/ const com::sun::star::i18n::ForbiddenCharacters& rForbiddenCharacters ) = 0;

    /** Get the forbidden character table and creates one if necessary.

       @returns
       the forbidden characters table.
    */
    virtual vos::ORef<SvxForbiddenCharactersTable>& getForbiddenCharacterTable() = 0;

    /** Get the forbidden character table.

       @returns
       the forbidden characters table.
    */
    virtual const vos::ORef<SvxForbiddenCharactersTable>& getForbiddenCharacterTable() const = 0;

    /** Get the current link update mode.

       @param bGlobalSettings
       [in] if set, the link update mode is obtained from the module,
            if it is set to GLOBALSETTING

       @returns
       the current link update mode.
    */
    virtual sal_uInt16 getLinkUpdateMode( /*[in]*/bool bGlobalSettings ) const = 0;

    /** Set the current link update mode.

       @param nMode
       [in] the new link update mode.
    */
    virtual void setLinkUpdateMode( /*[in]*/ sal_uInt16 nMode ) = 0;

    /** Get the current field update mode.

       @param bGlobalSettings
       [in] if set, the field update mode is obtained from the module,
            if it is set to GLOBALSETTING

       @returns
       the current field update mode.
    */
    virtual sal_uInt16 getFieldUpdateFlags( /*[in]*/bool bGlobalSettings ) const = 0;

    /** Set the current field update mode.

       @param nMode
       [in] the new field update mode.
    */
    virtual void setFieldUpdateFlags( /*[in]*/ sal_uInt16 nMode )  = 0;

    /** Get the character compression type for Asian characters.

       @returns
       the current character compression mode.
    */
    virtual SwCharCompressType getCharacterCompressionType() const = 0;

    /** Set the character compression type for Asian characters.

       @param nMode
       [in] the new character compression type.
    */
    virtual void setCharacterCompressionType( /*[in]*/SwCharCompressType nType ) = 0;

protected:
    virtual ~IDocumentSettingAccess() {};
 };

 #endif // IDOCUMENTSETTINGACCESS_HXX_INCLUDED
