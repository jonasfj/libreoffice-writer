/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: olmenu.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 23:05:27 $
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


#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _MySVXACORR_HXX //autogen
#include <svx/svxacorr.hxx>
#endif

#ifndef _LINGUISTIC_LNGPROPS_HHX_
#include <linguistic/lngprops.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _SVX_DLGUTIL_HXX
#include <svx/dlgutil.hxx>
#endif
#ifndef _SFXITEMSET_HXX //autogen
#include <svtools/itemset.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX //autogen
#include <svx/langitem.hxx>
#endif
#ifndef _SVX_SPLWRAP_HXX
#include <svx/splwrap.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SVTOOLS_LINGUCFG_HXX_
#include <svtools/lingucfg.hxx>
#endif
#include <svx/acorrcfg.hxx>
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _HELPID_H
#include <helpid.h>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>		//CheckSpellChanges
#endif
#ifndef _OLMENU_HXX
#include <olmenu.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif

#ifndef _OLMENU_HRC
#include <olmenu.hrc>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif

// -> #111827#
#include <SwRewriter.hxx>
#include <comcore.hrc>
#include <undobj.hxx>
// <- #111827#

using namespace ::com::sun::star;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::linguistic2;
using namespace ::rtl;

#define C2U(cChar) OUString::createFromAscii(cChar)

/*--------------------------------------------------------------------------

---------------------------------------------------------------------------*/

util::Language lcl_CheckLanguage( const OUString &rWord, Reference< XSpellChecker1 >  xSpell )
{
    //
    // build list of languages to check
    //
    LanguageType aLangList[4];
    const AllSettings& rSettings  = Application::GetSettings();
    SvtLinguOptions aLinguOpt;
    SvtLinguConfig().GetOptions( aLinguOpt );
    // The default document language from "Tools/Options - Language Settings - Languages: Western"
    aLangList[0] = aLinguOpt.nDefaultLanguage;
    // The one from "Tools/Options - Language Settings - Languages: User interface"
    aLangList[1] = rSettings.GetUILanguage();
    // The one from "Tools/Options - Language Settings - Languages: Locale setting"
    aLangList[2] = rSettings.GetLanguage();
    // en-US
    aLangList[3] = LANGUAGE_ENGLISH_US;
#ifdef DEBUG
    lang::Locale a1( SvxCreateLocale( aLangList[0] ) );
    lang::Locale a2( SvxCreateLocale( aLangList[1] ) );
    lang::Locale a3( SvxCreateLocale( aLangList[2] ) );
    lang::Locale a4( SvxCreateLocale( aLangList[3] ) );
#endif

    util::Language nLang = LANGUAGE_NONE;

    INT32   nCount = sizeof(aLangList) / sizeof(aLangList[0]);
    for (INT32 i = 0;  i < nCount;  i++)
    {
        INT16 nTmpLang = aLangList[i];
        if (nTmpLang != LANGUAGE_NONE  &&  nTmpLang != LANGUAGE_DONTKNOW)
        {
            if (xSpell->hasLanguage( nTmpLang ) &&
                xSpell->isValid( rWord, nTmpLang, Sequence< PropertyValue >() ))
            {
                nLang = nTmpLang;
                break;
            }
        }
    }

    return nLang;
}


SwSpellPopup::SwSpellPopup( SwWrtShell* pWrtSh, const Reference< XSpellAlternatives >  &xAlt ) :
    PopupMenu(SW_RES(MN_SPELL_POPUP)),
    pSh	( pWrtSh ),
    xSpellAlt	(xAlt)
{
    DBG_ASSERT(xSpellAlt.is(), "no spelling alternatives available");

    CreateAutoMnemonics();
    Sequence< OUString >	aStrings;
    if (xSpellAlt.is())
        aStrings = xSpellAlt->getAlternatives();
    const OUString *pString = aStrings.getConstArray();
    sal_Int16 nStringCount = static_cast< sal_Int16 >( aStrings.getLength() );

    PopupMenu *pMenu = GetPopupMenu(MN_AUTOCORR);
    pMenu->SetMenuFlags(MENU_FLAG_NOAUTOMNEMONICS);
    sal_Bool bEnable = sal_False;
    if( nStringCount )
    {
        InsertSeparator(0);
        bEnable = sal_True;
        for( sal_uInt16 i = 0, nPos = 1, nId = MN_AUTOCORR_START + 1;
            i < nStringCount; ++i, ++nPos, ++nId )
        {
            const String aEntry = pString[ i ];
            InsertItem( nPos, aEntry, 0, i );
            SetHelpId( nPos, HID_LINGU_REPLACE);
            pMenu->InsertItem( nId, aEntry );
            pMenu->SetHelpId( nId, HID_LINGU_AUTOCORR);
        }
    }
    EnableItem( MN_AUTOCORR, bEnable );

    nCorrLang = LANGUAGE_NONE;
    if (xSpellAlt.is())
        nCorrLang = lcl_CheckLanguage( xSpellAlt->getWord(), ::GetSpellChecker() );
    if( nCorrLang != LANGUAGE_NONE )
    {
        InsertSeparator();
        String aTmp( ::GetLanguageString( nCorrLang ) );
        InsertItem( MN_LANGUAGE_WORD, String( SW_RES( STR_WORD ) ).Append(aTmp) );
        SetHelpId( MN_LANGUAGE_WORD, HID_LINGU_WORD_LANGUAGE );
        InsertItem( MN_LANGUAGE_PARA, String( SW_RES( STR_PARAGRAPH ) ).Append(aTmp) );
        SetHelpId( MN_LANGUAGE_PARA, HID_LINGU_PARA_LANGUAGE );
    }

    pMenu = GetPopupMenu(MN_INSERT);

    bEnable = FALSE;	// enable MN_INSERT?

    pMenu->CreateAutoMnemonics();
    Reference< XDictionaryList >    xDicList( SvxGetDictionaryList() );
    if (xDicList.is())
    {
        // add active, positive dictionary to dic-list (if not already done).
        // This is to ensure that there is at least on dictionary to which
        // words could be added.
        Reference< XDictionary1 >  xDic( SvxGetOrCreatePosDic( xDicList ) );
        if (xDic.is())
            xDic->setActive( sal_True );

        aDics = xDicList->getDictionaries();
        const Reference< XDictionary >  *pDic = aDics.getConstArray();
        sal_Int32 nDicCount = aDics.getLength();

        sal_Int16 nLanguage = LANGUAGE_NONE;
        if (xSpellAlt.is())
            nLanguage = SvxLocaleToLanguage( xSpellAlt->getLocale() );

        for( sal_Int32 i = 0; i < nDicCount; i++ )
        {
            Reference< XDictionary1 >  xDic( pDic[i], UNO_QUERY );
            if (!xDic.is() || SvxGetIgnoreAllList() == xDic)
                continue;

            Reference< frame::XStorable > xStor( xDic, UNO_QUERY );
            LanguageType nActLanguage = xDic->getLanguage();
            if( xDic->isActive()
                &&  xDic->getDictionaryType() != DictionaryType_NEGATIVE
                && (nLanguage == nActLanguage || LANGUAGE_NONE == nActLanguage )
                && (!xStor.is() || !xStor->isReadonly()) )
            {
                // the extra 1 is because of the (possible) external
                // linguistic entry above
                pMenu->InsertItem( MN_INSERT_START + i + 1, xDic->getName() );
                bEnable = sal_True;
            }
        }
    }
    EnableItem( MN_INSERT, bEnable );

    RemoveDisabledEntries( TRUE, TRUE );
    SetMenuFlags(MENU_FLAG_NOAUTOMNEMONICS);
}

/*--------------------------------------------------------------------------

---------------------------------------------------------------------------*/
sal_uInt16  SwSpellPopup::Execute( Window* pWin, const Rectangle& rWordPos )
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
    sal_Bool bAutoCorr = sal_False;
    if( nId > MN_AUTOCORR_START && nId != USHRT_MAX )
    {
        nId -= MN_AUTOCORR_START;
        bAutoCorr = sal_True;
    }

    if( nId && nId != USHRT_MAX)
    {
        int nAltIdx = nId - 1;
        if( xSpellAlt.is()  &&  nAltIdx < xSpellAlt->getAlternativesCount() )
        {
            sal_Bool bOldIns = pSh->IsInsMode();
            pSh->SetInsMode( sal_True );

            const Sequence< OUString > aAlts( xSpellAlt->getAlternatives() );
            const OUString *pString = aAlts.getConstArray();
            DBG_ASSERT( 0 <= nAltIdx && nAltIdx <= xSpellAlt->getAlternativesCount(),
                    "index out of range");
            String aTmp( pString[ nAltIdx ] );
            String aOrig( xSpellAlt->getWord());

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

            {
                String aTmpStr;

                aTmpStr += String(SW_RES(STR_START_QUOTE));
                aTmpStr += aTmp;
                aTmpStr += String(SW_RES(STR_END_QUOTE));
                aRewriter.AddRule(UNDO_ARG3, aTmpStr);
            }

            pSh->StartUndo(UIUNDO_REPLACE, &aRewriter);
            pSh->StartAction();
            pSh->DelLeft();

            pSh->Insert( aTmp );
            /* #102505# EndAction/EndUndo moved down since insertion
               of temporary auto correction is now undoable two and
               must reside in the same undo group.*/


            // nur aufnehmen, wenn es NICHT schon in der Autokorrektur vorhanden ist
            SvxAutoCorrect* pACorr = SvxAutoCorrCfg::Get()->GetAutoCorrect();

            LanguageType eLanguage = SvxLocaleToLanguage( xSpellAlt->getLocale() );

            String aOrigWord( xSpellAlt->getWord() ) ;
            String aNewWord;
            if( nId )
                aNewWord = pString[ nAltIdx ];
            else
                aNewWord = aOrigWord;
            SvxPrepareAutoCorrect( aOrigWord, aNewWord );

            if( bAutoCorr )
            {
                pACorr->PutText( aOrigWord, aNewWord, eLanguage );
            }

            /* #102505# EndAction/EndUndo moved down since insertion
               of temporary auto correction is now undoable two and
               must reside in the same undo group.*/
            pSh->EndAction();
            pSh->EndUndo(UIUNDO_REPLACE);

            pSh->SetInsMode( bOldIns );
        }
        else
            switch( nId )
            {
                case MN_SPELLING:
                {
                    pSh->Left(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
                    {
                        Reference<XDictionaryList> xDictionaryList( SvxGetDictionaryList() );
                        SvxDicListChgClamp aClamp( xDictionaryList );
                        pSh->GetView().GetViewFrame()->GetDispatcher()->
                            Execute( SID_SPELL_DIALOG, SFX_CALLMODE_ASYNCHRON );
                    }
                }
                break;
                case MN_IGNORE :
                {
                    Reference< XDictionary > xDictionary( SvxGetIgnoreAllList(), UNO_QUERY );
                    sal_Int16 nAddRes = SvxAddEntryToDic(
                            xDictionary,
                            xSpellAlt->getWord(), sal_False,
                            aEmptyStr, LANGUAGE_NONE );
                }
                break;
                case MN_INSERT:
                    DBG_ERROR("geht noch nicht!")
                break;
                case MN_LANGUAGE_WORD:
                case MN_LANGUAGE_PARA:
                {
                    pSh->StartAction();

                    if( MN_LANGUAGE_PARA == nId )
                    {
                        if( !pSh->IsSttPara() )
                            pSh->MovePara( fnParaCurr, fnParaStart );
                        pSh->SwapPam();
                        if( !pSh->IsEndPara() )
                            pSh->MovePara( fnParaCurr,  fnParaEnd );
                    }

                    SfxItemSet aSet(pSh->GetAttrPool(), RES_CHRATR_LANGUAGE,
                                                        RES_CHRATR_LANGUAGE);
                    aSet.Put( SvxLanguageItem( nCorrLang ) );
                    pSh->SetAttr( aSet );

                    pSh->EndAction();
                }
                break;
                default:
                    if(nId >= MN_INSERT_START )
                    {
                        OUString aWord( xSpellAlt->getWord() );
                        INT32 nDicIdx = nId - MN_INSERT_START - 1;
                        DBG_ASSERT( nDicIdx < aDics.getLength(),
                                    "dictionary index out of range" );
                        Reference< XDictionary > xDic =
                            aDics.getConstArray()[nDicIdx];
                        INT16 nAddRes = SvxAddEntryToDic( xDic,
                            aWord, FALSE, aEmptyStr, LANGUAGE_NONE );

                        if (DIC_ERR_NONE != nAddRes
                            && !xDic->getEntry( aWord ).is())
                        {
                            SvxDicError(
                                &pSh->GetView().GetViewFrame()->GetWindow(),
                                nAddRes );
                        }
                    }
            }
    }
    pSh->EnterStdMode();
}
