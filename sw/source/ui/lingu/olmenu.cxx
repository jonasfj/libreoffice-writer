/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: olmenu.cxx,v $
 * $Revision: 1.41 $
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

#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/document/XDocumentLanguages.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/linguistic2/SingleProofreadingError.hpp>
#include <com/sun/star/linguistic2/XLanguageGuessing.hpp>
#include <com/sun/star/linguistic2/XSpellChecker1.hpp>
#include <com/sun/star/uno/Any.hxx>

#include <comphelper/processfactory.hxx>

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#endif

#include <i18npool/mslangid.hxx>
#include <linguistic/lngprops.hxx>
#include <linguistic/misc.hxx>
#include <osl/file.hxx>
#include <rtl/string.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/imagemgr.hxx>
#include <sfx2/request.hxx>
#include <sfx2/sfxdlg.hxx>
#include <svl/itemset.hxx>
#include <svl/stritem.hxx>
#include <svtools/filter.hxx>
#include <svtools/langtab.hxx>
#include <svx/brshitem.hxx>
#include <svx/acorrcfg.hxx>
#include <svx/dlgutil.hxx>
#include <svx/impgrf.hxx>
#include <svx/langitem.hxx>
#include <svl/languageoptions.hxx>
#include <svx/splwrap.hxx>
#include <svx/svxacorr.hxx>
#include <svx/unolingu.hxx>
#include <unotools/lingucfg.hxx>
#include <unotools/linguprops.hxx>
#include <svl/languageoptions.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/settings.hxx>
#include <vcl/svapp.hxx>


#include "SwRewriter.hxx"
#include "chrdlg.hrc"
#include "cmdid.h"
#include "comcore.hrc"
#include "crsskip.hxx"
#include "doc.hxx"
#include "docsh.hxx"        //CheckSpellChanges
#include "edtwin.hxx"
#include "helpid.h"
#include "hintids.hxx"
#include "langhelper.hxx"
#include "ndtxt.hxx"
#include "olmenu.hrc"
#include "olmenu.hxx"
#include "swabstdlg.hxx"
#include "swmodule.hxx"
#include "swtypes.hxx"
#include "swundo.hxx"
#include "uitool.hxx"
#include "undobj.hxx"
#include "unomid.h"
#include "view.hxx"
#include "viewopt.hxx"
#include "wrtsh.hxx"
#include "wview.hxx"


#include <map>


using namespace ::com::sun::star;
using ::rtl::OUString;

extern void lcl_CharDialog( SwWrtShell &rWrtSh, BOOL bUseDialog, USHORT nSlot,const SfxItemSet *pArgs, SfxRequest *pReq );


/*--------------------------------------------------------------------------

---------------------------------------------------------------------------*/

// tries to determine the language of 'rText'
//
LanguageType lcl_CheckLanguage(
    const OUString &rText,
    uno::Reference< linguistic2::XSpellChecker1 > xSpell,
    uno::Reference< linguistic2::XLanguageGuessing > xLangGuess,
    sal_Bool bIsParaText )
{
    LanguageType  nLang = LANGUAGE_NONE;
    if (bIsParaText)	// check longer texts with language-guessing...
    {
        if (!xLangGuess.is())
            return nLang;

        lang::Locale aLocale( xLangGuess->guessPrimaryLanguage( rText, 0, rText.getLength()) );

        // get language as from "Tools/Options - Language Settings - Languages: Locale setting"
        LanguageType nTmpLang = Application::GetSettings().GetLanguage();

        // if the result from language guessing does not provide a 'Country' part
        // try to get it by looking up the locale setting of the office.
        if (aLocale.Country.getLength() == 0)
        {
            lang::Locale aTmpLocale = SvxCreateLocale( nTmpLang );
            if (aTmpLocale.Language == aLocale.Language)
                nLang = nTmpLang;
        }
        if (nLang == LANGUAGE_NONE) // language not found by looking up the system language...
            nLang = MsLangId::convertLocaleToLanguageWithFallback( aLocale );
        if (nLang == LANGUAGE_SYSTEM)
            nLang = nTmpLang;
        if (nLang == LANGUAGE_DONTKNOW)
            nLang = LANGUAGE_NONE;
    }
    else	// check single word
    {
            if (!xSpell.is())
            return nLang;

        //
        // build list of languages to check
        //
        LanguageType aLangList[4];
        const AllSettings& rSettings  = Application::GetSettings();
        SvtLinguOptions aLinguOpt;
        SvtLinguConfig().GetOptions( aLinguOpt );
        // The default document language from "Tools/Options - Language Settings - Languages: Western"
        aLangList[0] = MsLangId::resolveSystemLanguageByScriptType(aLinguOpt.nDefaultLanguage, ::com::sun::star::i18n::ScriptType::LATIN);
        // The one from "Tools/Options - Language Settings - Languages: User interface"
        aLangList[1] = rSettings.GetUILanguage();
        // The one from "Tools/Options - Language Settings - Languages: Locale setting"
        aLangList[2] = rSettings.GetLanguage();
        // en-US
        aLangList[3] = LANGUAGE_ENGLISH_US;
#ifdef DEBUG
        lang::Locale a0( SvxCreateLocale( aLangList[0] ) );
        lang::Locale a1( SvxCreateLocale( aLangList[1] ) );
        lang::Locale a2( SvxCreateLocale( aLangList[2] ) );
        lang::Locale a3( SvxCreateLocale( aLangList[3] ) );
#endif

        INT32   nCount = sizeof(aLangList) / sizeof(aLangList[0]);
        for (INT32 i = 0;  i < nCount;  i++)
        {
            INT16 nTmpLang = aLangList[i];
            if (nTmpLang != LANGUAGE_NONE  &&  nTmpLang != LANGUAGE_DONTKNOW)
            {
                if (xSpell->hasLanguage( nTmpLang ) &&
                    xSpell->isValid( rText, nTmpLang, uno::Sequence< beans::PropertyValue >() ))
                {
                    nLang = nTmpLang;
                    break;
                }
            }
        }
    }

    return nLang;
}


/// @returns : the language for the selected text that is set for the
///     specified attribute (script type).
///     If there are more than one languages used LANGUAGE_DONTKNOW will be returned.
/// @param nLangWhichId : one of
///     RES_CHRATR_LANGUAGE, RES_CHRATR_CJK_LANGUAGE, RES_CHRATR_CTL_LANGUAGE,
/// @returns: the language in use for the selected text.
///     'In use' means the language(s) matching the script type(s) of the
///     selected text. Or in other words, the language a spell checker would use.
///     If there is more than one language LANGUAGE_DONTKNOW will be returned.
// check if nScriptType includes the script type associated to nLang
inline bool lcl_checkScriptType( sal_Int16 nScriptType, LanguageType nLang )
{
    return 0 != (nScriptType & SvtLanguageOptions::GetScriptTypeOfLanguage( nLang ));
}

void SwSpellPopup::fillLangPopupMenu(
    PopupMenu *pPopupMenu,
    USHORT nLangItemIdStart,
    uno::Sequence< OUString > aSeq,
    SwWrtShell* pWrtSh,
    std::map< sal_Int16, ::rtl::OUString > &rLangTable )
{
    if (!pPopupMenu)
        return;

    SvtLanguageTable    aLanguageTable;
    
    // set of languages to be displayed in the sub menus
    std::set< OUString > aLangItems;

    OUString    aCurLang( aSeq[0] );
    USHORT      nScriptType = static_cast< sal_Int16 >(aSeq[1].toInt32());
    OUString    aKeyboardLang( aSeq[2] );
    OUString    aGuessedTextLang( aSeq[3] );

    if (aCurLang != OUString() &&
        LANGUAGE_DONTKNOW != aLanguageTable.GetType( aCurLang ))
        aLangItems.insert( aCurLang );

    //2--System
    const AllSettings& rAllSettings = Application::GetSettings();
    LanguageType rSystemLanguage = rAllSettings.GetLanguage();
    if (rSystemLanguage != LANGUAGE_DONTKNOW)
    {
        if (lcl_checkScriptType( nScriptType, rSystemLanguage ))
            aLangItems.insert( aLanguageTable.GetString(rSystemLanguage) );
    }

    //3--UI
    LanguageType rUILanguage = rAllSettings.GetUILanguage();
    if (rUILanguage != LANGUAGE_DONTKNOW)
    {
        if (lcl_checkScriptType(nScriptType, rUILanguage ))
            aLangItems.insert( aLanguageTable.GetString(rUILanguage) );
    }

    //4--guessed language
    if (aGuessedTextLang.getLength() > 0)
    {
        if (lcl_checkScriptType(nScriptType, aLanguageTable.GetType(aGuessedTextLang)))
            aLangItems.insert( aGuessedTextLang );
    }

    //5--keyboard language
    if (aKeyboardLang.getLength() > 0)
    {
        if (lcl_checkScriptType(nScriptType, aLanguageTable.GetType(aKeyboardLang)))
            aLangItems.insert( aKeyboardLang );
    }

    //6--all languages used in current document
    uno::Reference< com::sun::star::frame::XModel > xModel;
    uno::Reference< com::sun::star::frame::XController > xController( pWrtSh->GetView().GetViewFrame()->GetFrame()->GetFrameInterface()->getController(), uno::UNO_QUERY );
    if ( xController.is() )
        xModel = xController->getModel();
    uno::Reference< document::XDocumentLanguages > xDocumentLanguages( xModel, uno::UNO_QUERY );
    /*the description of nScriptType flags
      LATIN :   0x0001
      ASIAN :   0x0002
      COMPLEX:  0x0004
    */
    const sal_Int16 nMaxCount = 7;
    if (xDocumentLanguages.is())
    {
        uno::Sequence< lang::Locale > rLocales( xDocumentLanguages->getDocumentLanguages( nScriptType, nMaxCount ) );
        if (rLocales.getLength() > 0)
        {
            for (USHORT i = 0; i < rLocales.getLength(); ++i)
            {
                if (aLangItems.size() == (size_t)nMaxCount)
                    break;
                const lang::Locale& rLocale = rLocales[i];
                if (lcl_checkScriptType( nScriptType, aLanguageTable.GetType( rLocale.Language )))
                    aLangItems.insert( rLocale.Language );
            }
        }
    }


    USHORT nItemId = nLangItemIdStart;
    const OUString sAsterix(RTL_CONSTASCII_USTRINGPARAM("*"));  // multiple languages in current selection
    const OUString sEmpty;  // 'no language found' from language guessing
    std::set< OUString >::const_iterator it;
    for (it = aLangItems.begin(); it != aLangItems.end(); ++it)
    {
        OUString aEntryTxt( *it );
        if (aEntryTxt != OUString( aLanguageTable.GetString( LANGUAGE_NONE ) )&&
            aEntryTxt != sAsterix &&
            aEntryTxt != sEmpty)
        {
            DBG_ASSERT( nLangItemIdStart <= nItemId && nItemId <= nLangItemIdStart + MN_MAX_NUM_LANG,
                    "nItemId outside of expected range!" );
            pPopupMenu->InsertItem( nItemId, aEntryTxt, MIB_RADIOCHECK );
            if (aEntryTxt == aCurLang)
            {
                //make a check mark for the current language
                pPopupMenu->CheckItem( nItemId, TRUE );
            }
            rLangTable[ nItemId ] = aEntryTxt;
            ++nItemId;
        }
    }

    pPopupMenu->InsertItem( nLangItemIdStart + MN_NONE_OFFSET,  String(SW_RES( STR_LANGSTATUS_NONE )), MIB_RADIOCHECK );
    pPopupMenu->InsertItem( nLangItemIdStart + MN_RESET_OFFSET, String(SW_RES( STR_RESET_TO_DEFAULT_LANGUAGE )), MIB_RADIOCHECK );
    pPopupMenu->InsertItem( nLangItemIdStart + MN_MORE_OFFSET,  String(SW_RES( STR_LANGSTATUS_MORE )), MIB_RADIOCHECK );
}


static Image lcl_GetImageFromPngUrl( const OUString &rFileUrl )
{
    Image aRes;
    OUString aTmp;
    osl::FileBase::getSystemPathFromFileURL( rFileUrl, aTmp );
//    ::rtl::OString aPath = OString( aTmp.getStr(), aTmp.getLength(), osl_getThreadTextEncoding() );
#if defined(WNT)
//    aTmp = lcl_Win_GetShortPathName( aTmp );
#endif
    Graphic aGraphic;
    const String aFilterName( RTL_CONSTASCII_USTRINGPARAM( IMP_PNG ) );
    if( GRFILTER_OK == LoadGraphic( aTmp, aFilterName, aGraphic ) )
    {
        aRes = Image( aGraphic.GetBitmapEx() );
    }
    return aRes;
}


OUString RetrieveLabelFromCommand( const OUString& aCmdURL )
{
    OUString aLabel;
    if ( aCmdURL.getLength() )
    {
        try
        {
            uno::Reference< container::XNameAccess > xNameAccess( ::comphelper::getProcessServiceFactory()->createInstance( OUString::createFromAscii("com.sun.star.frame.UICommandDescription") ), uno::UNO_QUERY );
            if ( xNameAccess.is() )
            {
                uno::Reference< container::XNameAccess > xUICommandLabels;
                const OUString aModule( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.TextDocument" ) );
                uno::Any a = xNameAccess->getByName( aModule );
                uno::Reference< container::XNameAccess > xUICommands;
                a >>= xUICommandLabels;
                OUString aStr;
                uno::Sequence< beans::PropertyValue > aPropSeq;
                a = xUICommandLabels->getByName( aCmdURL );
                if ( a >>= aPropSeq )
                {
                    for ( sal_Int32 i = 0; i < aPropSeq.getLength(); i++ )
                    {
                        if ( aPropSeq[i].Name.equalsAscii( "Name" ))
                        {
                            aPropSeq[i].Value >>= aStr;
                            break;
                        }
                    }
                }
                aLabel = aStr;
            }
        }
        catch ( uno::Exception& )
        {
        }
    }

    return aLabel;
}


SwSpellPopup::SwSpellPopup(
        SwWrtShell* pWrtSh,
        const uno::Reference< linguistic2::XSpellAlternatives >  &xAlt,
        const String &rParaText ) :
PopupMenu( SW_RES(MN_SPELL_POPUP) ),
pSh( pWrtSh ),
xSpellAlt(xAlt),
bGrammarResults(false)
{
    DBG_ASSERT(xSpellAlt.is(), "no spelling alternatives available");

    CreateAutoMnemonics();
    nCheckedLanguage = LANGUAGE_NONE;
    if (xSpellAlt.is())
    {
        nCheckedLanguage = SvxLocaleToLanguage( xSpellAlt->getLocale() );
        aSuggestions = xSpellAlt->getAlternatives();
    }
    sal_Int16 nStringCount = static_cast< sal_Int16 >( aSuggestions.getLength() );

    SvtLinguConfig aCfg;
    const bool bHC = Application::GetSettings().GetStyleSettings().GetHighContrastMode();

    PopupMenu *pMenu = GetPopupMenu(MN_AUTOCORR);
    pMenu->SetMenuFlags(MENU_FLAG_NOAUTOMNEMONICS);
    sal_Bool bEnable = sal_False;
    if( nStringCount )
    {
        Image aImage;
        OUString aSuggestionImageUrl;
        uno::Reference< container::XNamed > xNamed( xSpellAlt, uno::UNO_QUERY );
        if (xNamed.is())
        {
            aSuggestionImageUrl = aCfg.GetSpellAndGrammarContextSuggestionImage( xNamed->getName(), bHC );
            aImage = Image( lcl_GetImageFromPngUrl( aSuggestionImageUrl ) );
        }

        InsertSeparator(0);
        bEnable = sal_True;
        USHORT nAutoCorrItemId  = MN_AUTOCORR_START;
        USHORT nItemId          = MN_SUGGESTION_START;
        for (USHORT i = 0; i < nStringCount; ++i)
        {
            const String aEntry = aSuggestions[ i ];
            InsertItem( nItemId, aEntry, 0, i );
            SetHelpId( nItemId, HID_LINGU_REPLACE);
            if (aSuggestionImageUrl.getLength() > 0)
                SetItemImage( nItemId, aImage );

            pMenu->InsertItem( nAutoCorrItemId, aEntry );
            pMenu->SetHelpId( nAutoCorrItemId, HID_LINGU_AUTOCORR);
            
            ++nAutoCorrItemId;
            ++nItemId;
        }
    }

    OUString aIgnoreSelection( String( SW_RES( STR_IGNORE_SELECTION ) ) );
    OUString aSpellingAndGrammar = RetrieveLabelFromCommand( C2U(".uno:SpellingAndGrammarDialog") );
    SetItemText( MN_SPELLING_DLG, aSpellingAndGrammar );
    USHORT nItemPos = GetItemPos( MN_IGNORE_WORD );
    InsertItem( MN_IGNORE_SELECTION, aIgnoreSelection, 0, nItemPos );
    SetHelpId( MN_IGNORE_SELECTION, HID_LINGU_IGNORE_SELECTION);

    EnableItem( MN_AUTOCORR, bEnable );

    uno::Reference< linguistic2::XLanguageGuessing > xLG = SW_MOD()->GetLanguageGuesser();
    nGuessLangWord = LANGUAGE_NONE;
    nGuessLangPara = LANGUAGE_NONE;
    if (xSpellAlt.is() && xLG.is())
    {
        nGuessLangWord = lcl_CheckLanguage( xSpellAlt->getWord(), ::GetSpellChecker(), xLG, sal_False );
        nGuessLangPara = lcl_CheckLanguage( rParaText, ::GetSpellChecker(), xLG, sal_True );
    }
    if (nGuessLangWord != LANGUAGE_NONE || nGuessLangPara != LANGUAGE_NONE)
    {
        // make sure LANGUAGE_NONE gets not used as menu entry
        if (nGuessLangWord == LANGUAGE_NONE)
            nGuessLangWord = nGuessLangPara;
        if (nGuessLangPara == LANGUAGE_NONE)
            nGuessLangPara = nGuessLangWord;
    }

    pMenu = GetPopupMenu(MN_ADD_TO_DIC);
    pMenu->CreateAutoMnemonics();
    bEnable = FALSE;    // enable MN_ADD_TO_DIC?
    uno::Reference< linguistic2::XDictionaryList >    xDicList( SvxGetDictionaryList() );
    if (xDicList.is())
    {
        // add the default positive dictionary to dic-list (if not already done).
        // This is to ensure that there is at least one dictionary to which
        // words could be added.
        uno::Reference< linguistic2::XDictionary >  xDic( SvxGetOrCreatePosDic( xDicList ) );
        if (xDic.is())
            xDic->setActive( sal_True );

        aDics = xDicList->getDictionaries();
        const uno::Reference< linguistic2::XDictionary >  *pDic = aDics.getConstArray();
        USHORT nDicCount = static_cast< USHORT >(aDics.getLength());

        USHORT nItemId = MN_DICTIONARIES_START;
        for( USHORT i = 0; i < nDicCount; i++ )
        {
            uno::Reference< linguistic2::XDictionary >  xDicTmp( pDic[i], uno::UNO_QUERY );
            if (!xDicTmp.is() || SvxGetIgnoreAllList() == xDicTmp)
                continue;

            uno::Reference< frame::XStorable > xStor( xDicTmp, uno::UNO_QUERY );
            LanguageType nActLanguage = SvxLocaleToLanguage( xDicTmp->getLocale() );
            if( xDicTmp->isActive()
                &&  xDicTmp->getDictionaryType() != linguistic2::DictionaryType_NEGATIVE
                && (nCheckedLanguage == nActLanguage || LANGUAGE_NONE == nActLanguage )
                && (!xStor.is() || !xStor->isReadonly()) )
            {
                // the extra 1 is because of the (possible) external
                // linguistic entry above
                pMenu->InsertItem( nItemId, xDicTmp->getName() );
                bEnable = sal_True;

                uno::Reference< lang::XServiceInfo > xSvcInfo( xDicTmp, uno::UNO_QUERY );
                if (xSvcInfo.is())
                {
                    OUString aDictionaryImageUrl( aCfg.GetSpellAndGrammarContextDictionaryImage(
                            xSvcInfo->getImplementationName(), bHC) );
                    if (aDictionaryImageUrl.getLength() > 0)
                    {
                        Image aImage( lcl_GetImageFromPngUrl( aDictionaryImageUrl ) );
                        pMenu->SetItemImage( nItemId, aImage );
                    }
                }

                ++nItemId;
            }
        }
    }
    EnableItem( MN_ADD_TO_DIC, bEnable );

    //ADD NEW LANGUAGE MENU ITEM
    ///////////////////////////////////////////////////////////////////////////
    String aScriptTypesInUse( String::CreateFromInt32( pWrtSh->GetScriptType() ) );
    SvtLanguageTable aLanguageTable;

    // get keyboard language
    String aKeyboardLang;
    LanguageType nLang = LANGUAGE_DONTKNOW;
    SwEditWin& rEditWin = pWrtSh->GetView().GetEditWin();
    nLang = rEditWin.GetInputLanguage();
    if (nLang != LANGUAGE_DONTKNOW && nLang != LANGUAGE_SYSTEM)
        aKeyboardLang = aLanguageTable.GetString( nLang );

    // get the language that is in use
    const String aMultipleLanguages = String::CreateFromAscii("*");
    String aCurrentLang = aMultipleLanguages;
    nLang = SwLangHelper::GetCurrentLanguage( *pWrtSh );
    if (nLang != LANGUAGE_DONTKNOW)
        aCurrentLang = aLanguageTable.GetString( nLang );

    // build sequence for status value
    uno::Sequence< OUString > aSeq( 4 );
    aSeq[0] = aCurrentLang;
    aSeq[1] = aScriptTypesInUse;
    aSeq[2] = aKeyboardLang;
    aSeq[3] = aLanguageTable.GetString(nGuessLangWord);

    pMenu = GetPopupMenu(MN_SET_LANGUAGE_SELECTION);
    fillLangPopupMenu( pMenu, MN_SET_LANGUAGE_SELECTION_START, aSeq, pWrtSh, aLangTable_Text );
    EnableItem( MN_SET_LANGUAGE_SELECTION, true );

    pMenu = GetPopupMenu(MN_SET_LANGUAGE_PARAGRAPH);
    fillLangPopupMenu( pMenu, MN_SET_LANGUAGE_PARAGRAPH_START, aSeq, pWrtSh, aLangTable_Paragraph );
    EnableItem( MN_SET_LANGUAGE_PARAGRAPH, true );
/*
    pMenu = GetPopupMenu(MN_SET_LANGUAGE_ALL_TEXT);
    fillLangPopupMenu( pMenu, MN_SET_LANGUAGE_ALL_TEXT_START, aSeq, pWrtSh, aLangTable_Document );
    EnableItem( MN_SET_LANGUAGE_ALL_TEXT, true );
*/
    uno::Reference< frame::XFrame > xFrame = pWrtSh->GetView().GetViewFrame()->GetFrame()->GetFrameInterface();
    Image rImg = ::GetImage( xFrame,
            OUString::createFromAscii(".uno:SpellingAndGrammarDialog"), sal_False,
            Application::GetSettings().GetStyleSettings().GetHighContrastMode() );
    SetItemImage( MN_SPELLING_DLG, rImg );

    //////////////////////////////////////////////////////////////////////////////////
    
    RemoveDisabledEntries( TRUE, TRUE );
    SetMenuFlags(MENU_FLAG_NOAUTOMNEMONICS);
}

/*--------------------------------------------------------------------------

---------------------------------------------------------------------------*/

SwSpellPopup::SwSpellPopup(
    SwWrtShell *pWrtSh,
    const linguistic2::ProofreadingResult &rResult,
    sal_Int32 nErrorInResult,
    const uno::Sequence< OUString > &rSuggestions,
    const String &rParaText ) :
PopupMenu( SW_RES(MN_SPELL_POPUP) ),
pSh( pWrtSh ),
aSuggestions( rSuggestions ),
bGrammarResults( true ),
aInfo16( SW_RES(IMG_INFO_16) )
{
    nCheckedLanguage = SvxLocaleToLanguage( rResult.aLocale );

    USHORT nPos = 0;
    OUString aMessageText( rResult.aErrors[ nErrorInResult ].aShortComment );
    InsertSeparator( nPos++ );
    InsertItem( MN_SHORT_COMMENT, aMessageText, MIB_NOSELECT, nPos++ );
    SetItemImage( MN_SHORT_COMMENT, aInfo16 );

    CreateAutoMnemonics();

    InsertSeparator( nPos++ );
    sal_Int32 nStringCount = aSuggestions.getLength();
    if ( nStringCount )     // suggestions available...
    {
        Image aImage;
        OUString aSuggestionImageUrl;
        uno::Reference< lang::XServiceInfo > xInfo( rResult.xProofreader, uno::UNO_QUERY );
        if (xInfo.is())
        {
            aSuggestionImageUrl = SvtLinguConfig().GetSpellAndGrammarContextSuggestionImage( xInfo->getImplementationName() );
            aImage = Image( lcl_GetImageFromPngUrl( aSuggestionImageUrl ) );
        }

        USHORT nItemId = MN_SUGGESTION_START;
        for (sal_uInt16 i = 0;  i < nStringCount;  ++i)
        {
            const String aEntry = aSuggestions[ i ];
            InsertItem( nItemId, aEntry, 0, nPos++ );
            SetHelpId( nItemId, HID_LINGU_REPLACE );
            if (aSuggestionImageUrl.getLength() > 0)
                SetItemImage( nItemId, aImage );

            ++nItemId;
        }
        InsertSeparator( nPos++ );
    }

    OUString aIgnoreSelection( String( SW_RES( STR_IGNORE_SELECTION ) ) );
    OUString aSpellingAndGrammar = RetrieveLabelFromCommand( C2U(".uno:SpellingAndGrammarDialog") );
    SetItemText( MN_SPELLING_DLG, aSpellingAndGrammar );
    USHORT nItemPos = GetItemPos( MN_IGNORE_WORD );
    InsertItem( MN_IGNORE_SELECTION, aIgnoreSelection, 0, nItemPos );
    SetHelpId( MN_IGNORE_SELECTION, HID_LINGU_IGNORE_SELECTION);

    EnableItem( MN_AUTOCORR, false );

    uno::Reference< linguistic2::XLanguageGuessing > xLG = SW_MOD()->GetLanguageGuesser();
    nGuessLangWord = LANGUAGE_NONE;
    nGuessLangPara = LANGUAGE_NONE;
    if (xLG.is())
    {
//        nGuessLangWord = lcl_CheckLanguage( xSpellAlt->getWord(), ::GetSpellChecker(), xLG, sal_False );
        nGuessLangPara = lcl_CheckLanguage( rParaText, ::GetSpellChecker(), xLG, sal_True );
    }
    if (nGuessLangWord != LANGUAGE_NONE || nGuessLangPara != LANGUAGE_NONE)
    {
        // make sure LANGUAGE_NONE gets not used as menu entry
        if (nGuessLangWord == LANGUAGE_NONE)
            nGuessLangWord = nGuessLangPara;
        if (nGuessLangPara == LANGUAGE_NONE)
            nGuessLangPara = nGuessLangWord;
    }

    EnableItem( MN_IGNORE_WORD, false );
    EnableItem( MN_ADD_TO_DIC, false );

    //ADD NEW LANGUAGE MENU ITEM
    ///////////////////////////////////////////////////////////////////////////
    String aScriptTypesInUse( String::CreateFromInt32( pWrtSh->GetScriptType() ) );
    SvtLanguageTable aLanguageTable;

    // get keyboard language
    String aKeyboardLang;
    LanguageType nLang = LANGUAGE_DONTKNOW;
    SwEditWin& rEditWin = pWrtSh->GetView().GetEditWin();
    nLang = rEditWin.GetInputLanguage();
    if (nLang != LANGUAGE_DONTKNOW && nLang != LANGUAGE_SYSTEM)
        aKeyboardLang = aLanguageTable.GetString( nLang );

    // get the language that is in use
    const String aMultipleLanguages = String::CreateFromAscii("*");
    String aCurrentLang = aMultipleLanguages;
    nLang = SwLangHelper::GetCurrentLanguage( *pWrtSh );
    if (nLang != LANGUAGE_DONTKNOW)
        aCurrentLang = aLanguageTable.GetString( nLang );

    // build sequence for status value
    uno::Sequence< OUString > aSeq( 4 );
    aSeq[0] = aCurrentLang;
    aSeq[1] = aScriptTypesInUse;
    aSeq[2] = aKeyboardLang;
    aSeq[3] = aLanguageTable.GetString(nGuessLangWord);

    PopupMenu *pMenu = GetPopupMenu(MN_SET_LANGUAGE_SELECTION);
    fillLangPopupMenu( pMenu, MN_SET_LANGUAGE_SELECTION_START, aSeq, pWrtSh, aLangTable_Text );
    EnableItem( MN_SET_LANGUAGE_SELECTION, true );

    pMenu = GetPopupMenu(MN_SET_LANGUAGE_PARAGRAPH);
    fillLangPopupMenu( pMenu, MN_SET_LANGUAGE_PARAGRAPH_START, aSeq, pWrtSh, aLangTable_Paragraph );
    EnableItem( MN_SET_LANGUAGE_PARAGRAPH, true );
/*
    pMenu = GetPopupMenu(MN_SET_LANGUAGE_ALL_TEXT);
    fillLangPopupMenu( pMenu, MN_SET_LANGUAGE_ALL_TEXT_START, aSeq, pWrtSh, aLangTable_Document );
    EnableItem( MN_SET_LANGUAGE_ALL_TEXT, true );
*/
    uno::Reference< frame::XFrame > xFrame = pWrtSh->GetView().GetViewFrame()->GetFrame()->GetFrameInterface();
    Image rImg = ::GetImage( xFrame,
            OUString::createFromAscii(".uno:SpellingAndGrammarDialog"), sal_False,
            Application::GetSettings().GetStyleSettings().GetHighContrastMode() );
    SetItemImage( MN_SPELLING_DLG, rImg );

    //////////////////////////////////////////////////////////////////////////////////

    RemoveDisabledEntries( TRUE, TRUE );
    SetMenuFlags(MENU_FLAG_NOAUTOMNEMONICS);
}

/*--------------------------------------------------------------------------

---------------------------------------------------------------------------*/
sal_uInt16  SwSpellPopup::Execute( const Rectangle& rWordPos, Window* pWin )
{
//    SetMenuFlags(MENU_FLAG_NOAUTOMNEMONICS);
    sal_uInt16 nRet = PopupMenu::Execute(pWin, pWin->LogicToPixel(rWordPos));
    Execute( nRet );
    return nRet;
}
/*-- 19.01.2006 08:15:48---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwSpellPopup::Execute( USHORT nId )
{
    if (nId == USHRT_MAX)
        return;

    if (/*bGrammarResults && */nId == MN_SHORT_COMMENT)
        return;		// nothing to do since it is the error message (short comment)

    if ((MN_SUGGESTION_START <= nId && nId <= MN_SUGGESTION_END) ||
        (MN_AUTOCORR_START <= nId && nId <= MN_AUTOCORR_END))
    {
        sal_Int32 nAltIdx = (MN_SUGGESTION_START <= nId && nId <= MN_SUGGESTION_END) ?
                nId - MN_SUGGESTION_START : nId - MN_AUTOCORR_START;
        DBG_ASSERT( 0 <= nAltIdx && nAltIdx < aSuggestions.getLength(), "index out of range" );
        if (0 <= nAltIdx && nAltIdx < aSuggestions.getLength() && (bGrammarResults || xSpellAlt.is()))
        {
            sal_Bool bOldIns = pSh->IsInsMode();
            pSh->SetInsMode( sal_True );

            String aTmp( aSuggestions[ nAltIdx ] );
            String aOrig( bGrammarResults ? OUString() : xSpellAlt->getWord() );

            // if orginal word has a trailing . (likely the end of a sentence)
            // and the replacement text hasn't, then add it to the replacement
            if (aTmp.Len() && aOrig.Len() &&
                '.' == aOrig.GetChar( aOrig.Len() - 1) && /* !IsAlphaNumeric ??*/
                '.' != aTmp.GetChar( aTmp.Len() - 1))
            {
                aTmp += '.';
            }

            // #111827#
            SwRewriter aRewriter;

            aRewriter.AddRule(UNDO_ARG1, pSh->GetCrsrDescr());
            aRewriter.AddRule(UNDO_ARG2, String(SW_RES(STR_YIELDS)));

            String aTmpStr( SW_RES(STR_START_QUOTE) );
            aTmpStr += aTmp;
            aTmpStr += String(SW_RES(STR_END_QUOTE));
            aRewriter.AddRule(UNDO_ARG3, aTmpStr);

            pSh->StartUndo(UNDO_UI_REPLACE, &aRewriter);
            pSh->StartAction();
            pSh->DelLeft();

            pSh->Insert( aTmp );
            
            /* #102505# EndAction/EndUndo moved down since insertion
               of temporary auto correction is now undoable two and
               must reside in the same undo group.*/

            // nur aufnehmen, wenn es NICHT schon in der Autokorrektur vorhanden ist
            SvxAutoCorrect* pACorr = SvxAutoCorrCfg::Get()->GetAutoCorrect();

            String aOrigWord( bGrammarResults ? OUString() : xSpellAlt->getWord() ) ;
            String aNewWord( aSuggestions[ nAltIdx ] );
            SvxPrepareAutoCorrect( aOrigWord, aNewWord );

            if (MN_AUTOCORR_START <= nId && nId <= MN_AUTOCORR_END)
                pACorr->PutText( aOrigWord, aNewWord, nCheckedLanguage );

            /* #102505# EndAction/EndUndo moved down since insertion
               of temporary auto correction is now undoable two and
               must reside in the same undo group.*/
            pSh->EndAction();
            pSh->EndUndo(UNDO_UI_REPLACE);

            pSh->SetInsMode( bOldIns );
        }
    }
    else if (nId == MN_SPELLING_DLG)
    {
        if (bGrammarResults)
        {
            SvtLinguConfig().SetProperty( A2OU( UPN_IS_GRAMMAR_INTERACTIVE ), uno::makeAny( sal_True ));
        }
        pSh->Left(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
        {
            uno::Reference<linguistic2::XDictionaryList> xDictionaryList( SvxGetDictionaryList() );
            SvxDicListChgClamp aClamp( xDictionaryList );
            pSh->GetView().GetViewFrame()->GetDispatcher()->
                Execute( FN_SPELL_GRAMMAR_DIALOG, SFX_CALLMODE_ASYNCHRON );
        }
    }    
    else if (nId == MN_IGNORE_SELECTION)
    {
        SwPaM *pPaM = pSh->GetCrsr();
        if (pPaM)
            pSh->IgnoreGrammarErrorAt( *pPaM );
    }    
    else if (nId == MN_IGNORE_WORD)
    {
        uno::Reference< linguistic2::XDictionary > xDictionary( SvxGetIgnoreAllList(), uno::UNO_QUERY );
        linguistic::AddEntryToDic( xDictionary,
                xSpellAlt->getWord(), sal_False, aEmptyStr, LANGUAGE_NONE );
    }    
    else if (MN_DICTIONARIES_START <= nId && nId <= MN_DICTIONARIES_END)
    {
            OUString aWord( xSpellAlt->getWord() );
            USHORT nDicIdx = nId - MN_DICTIONARIES_START;
            DBG_ASSERT( nDicIdx < aDics.getLength(), "dictionary index out of range" );
            
            if (nDicIdx < aDics.getLength())
            {
                uno::Reference< linguistic2::XDictionary > xDic = aDics.getConstArray()[nDicIdx];
                INT16 nAddRes = linguistic::AddEntryToDic( xDic, aWord, FALSE, aEmptyStr, LANGUAGE_NONE );
                // save modified user-dictionary if it is persistent
                uno::Reference< frame::XStorable >  xSavDic( xDic, uno::UNO_QUERY );
                if (xSavDic.is())
                    xSavDic->store();

                if (DIC_ERR_NONE != nAddRes
                    && !xDic->getEntry( aWord ).is())
                {
                    SvxDicError(
                        &pSh->GetView().GetViewFrame()->GetWindow(),
                        nAddRes );
                }
            }
    }    
    else 
    {
        // Set language for selection or for paragraph...
        
        SfxItemSet aCoreSet( pSh->GetView().GetPool(),
                    RES_CHRATR_LANGUAGE,        RES_CHRATR_LANGUAGE,
                    RES_CHRATR_CJK_LANGUAGE,    RES_CHRATR_CJK_LANGUAGE,
                    RES_CHRATR_CTL_LANGUAGE,    RES_CHRATR_CTL_LANGUAGE,
                    0 );
        String aNewLangTxt;
        
        if (MN_SET_LANGUAGE_SELECTION_START <= nId && nId <= MN_SET_LANGUAGE_SELECTION_END)
        {
            //Set language for current selection
            aNewLangTxt = aLangTable_Text[nId];
            SwLangHelper::SetLanguage( *pSh, aNewLangTxt, true, aCoreSet );
        }
        else if (nId == MN_SET_SELECTION_NONE)
        {
            //Set Language_None for current selection
            SwLangHelper::SetLanguage_None( *pSh, true, aCoreSet );
        }
        else if (nId == MN_SET_SELECTION_RESET)
        {
            //reset languages for current selection
            SwLangHelper::ResetLanguages( *pSh, true );
        }
        else if (nId == MN_SET_SELECTION_MORE)
        {
            //Open Format/Character Dialog
            lcl_CharDialog( *pSh, true, nId, 0, 0 );
        }
        else if (MN_SET_LANGUAGE_PARAGRAPH_START <= nId && nId <= MN_SET_LANGUAGE_PARAGRAPH_END)
        {
            //Set language for current paragraph
            aNewLangTxt = aLangTable_Paragraph[nId];
            pSh->Push();        // save cursor
            SwLangHelper::SelectCurrentPara( *pSh );
            SwLangHelper::SetLanguage( *pSh, aNewLangTxt, true, aCoreSet );
            pSh->Pop( FALSE );  // restore cursor
        }
        else if (nId == MN_SET_PARA_NONE)
        {
            //Set Language_None for current paragraph
            pSh->Push();        // save cursor
            SwLangHelper::SelectCurrentPara( *pSh );
            SwLangHelper::SetLanguage_None( *pSh, true, aCoreSet );
            pSh->Pop( FALSE );  // restore cursor
        }
        else if (nId == MN_SET_PARA_RESET)
        {
            //reset languages for current paragraph
            pSh->Push();        // save cursor
            SwLangHelper::SelectCurrentPara( *pSh );
            SwLangHelper::ResetLanguages( *pSh, true );
            pSh->Pop( FALSE );  // restore cursor
        }
        else if (nId == MN_SET_PARA_MORE)
        {
            pSh->Push();        // save cursor
            SwLangHelper::SelectCurrentPara( *pSh );
            //Open Format/Character Dialog
            lcl_CharDialog( *pSh, true, nId, 0, 0 );
            pSh->Pop( FALSE );  // restore cursor
        }
#if 0
        else if (nId == MN_SET_LANGUAGE_ALL_TEXT_START + nNumLanguageDocEntries - 1)
        {
            //Set Language_None as the default language
            SwLangHelper::SetLanguage_None( *pSh, false, aCoreSet );
        }
        else if (nId == MN_SET_LANGUAGE_ALL_TEXT_START + nNumLanguageDocEntries)
        {
            // open the dialog "Tools/Options/Language Settings - Language"
            SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();
            if (pFact)
            {
                VclAbstractDialog* pDlg = pFact->CreateVclDialog( pSh->GetView().GetWindow(), SID_LANGUAGE_OPTIONS );
                pDlg->Execute();
                delete pDlg;
            }
        }
#endif
    }    

    pSh->EnterStdMode();
}
