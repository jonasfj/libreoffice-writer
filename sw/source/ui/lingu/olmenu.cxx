/*************************************************************************
 *
 *  $RCSfile: olmenu.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: hr $ $Date: 2003-06-30 15:01:57 $
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


#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif
#ifndef _OFF_APP_HXX //autogen
#include <offmgr/app.hxx>
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
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
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
    util::Language nLang = LANGUAGE_NONE;

    Reference< XSpellAlternatives >  	xAlt;
    Sequence< util::Language >	aLangs;
    if (xSpell.is())
        aLangs = xSpell->getLanguages();
    const util::Language *pLang = aLangs.getConstArray();
    INT32   nCount = aLangs.getLength();

    //! due to dieckmann (new german) spellchecker excepting many english
    //! (and other?) words as correct
    //! GERMAN and GERMAN_SWISS should be checked last.
    //! Otherwise e.g. english words might be reported as being german words!
    for (INT32 i = 0;  i < nCount;  i++)
    {
        INT16 nTmpLang = pLang[i];
        if (nTmpLang != LANGUAGE_NONE  &&
            nTmpLang != LANGUAGE_GERMAN  &&
            nTmpLang != LANGUAGE_GERMAN_SWISS)
        {
            if (xSpell->isValid( rWord, nTmpLang, Sequence< PropertyValue >() ) &&
                xSpell->hasLanguage( nTmpLang ))
            {
                nLang = nTmpLang;
                break;
            }
        }
    }
    if (nLang == LANGUAGE_NONE  &&
        xSpell->isValid( rWord, LANGUAGE_GERMAN, Sequence< PropertyValue >() ) &&
        xSpell->hasLanguage( LANGUAGE_GERMAN ))
    {
        nLang = LANGUAGE_GERMAN;
    }
    if (nLang == LANGUAGE_NONE  &&
        xSpell->isValid( rWord, LANGUAGE_GERMAN_SWISS, Sequence< PropertyValue >() ) &&
        xSpell->hasLanguage( LANGUAGE_GERMAN_SWISS ))
    {
        nLang = LANGUAGE_GERMAN_SWISS;
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
    sal_Int16 nStringCount = aStrings.getLength();

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
}

/*--------------------------------------------------------------------------

---------------------------------------------------------------------------*/
sal_uInt16	SwSpellPopup::Execute( Window* pWin, const Point& rWordPos )
{
    SetMenuFlags(MENU_FLAG_NOAUTOMNEMONICS);
    sal_uInt16 nRet = PopupMenu::Execute(pWin, pWin->LogicToPixel(rWordPos));
    sal_Bool bAutoCorr = sal_False;
    if( nRet > MN_AUTOCORR_START && nRet != USHRT_MAX )
    {
        nRet -= MN_AUTOCORR_START;
        bAutoCorr = sal_True;
    }

    if( nRet && nRet != USHRT_MAX)
    {
        int nAltIdx = nRet - 1;
        if( xSpellAlt.is()  &&  nAltIdx < xSpellAlt->getAlternativesCount() )
        {
            sal_Bool bOldIns = pSh->IsInsMode();
            pSh->SetInsMode( sal_True );

            pSh->StartUndo(UIUNDO_REPLACE);
            pSh->StartAction();
            pSh->DelLeft();

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

            pSh->Insert( aTmp );
            /* #102505# EndAction/EndUndo moved down since insertion
               of temporary auto correction is now undoable two and
               must reside in the same undo group.*/


            // nur aufnehmen, wenn es NICHT schon in der Autokorrektur vorhanden ist
            SvxAutoCorrect* pACorr = OFF_APP()->GetAutoCorrect();

            LanguageType eLanguage = SvxLocaleToLanguage( xSpellAlt->getLocale() );

            String aOrigWord( xSpellAlt->getWord() ) ;
            String aNewWord;
            if( nRet )
                aNewWord = pString[ nAltIdx ];
            else
                aNewWord = aOrigWord;
            SvxPrepareAutoCorrect( aOrigWord, aNewWord );
            
            /* #102505# Temporary auto correction is now handled by
               the document itself. Thus we no longer have to build,
               insert and remove the correction here but call the
               appropriate methods of SwDoc. */
            if( bAutoCorr )
            {
                pACorr->PutText( aOrigWord, aNewWord, eLanguage );
                pSh->GetDoc()->RemoveTmpCorr(aOrigWord);
            }
            else
            {
                SvxAutocorrWord aAWord( aOrigWord, aEmptyStr );
                if( !pACorr->GetAutocorrWordList(eLanguage)->Seek_Entry( &aAWord ))
                {
                    pSh->GetDoc()->AppendTmpCorr(aAWord.GetShort(), aNewWord);
                }
            }

            /* #102505# EndAction/EndUndo moved down since insertion
               of temporary auto correction is now undoable two and
               must reside in the same undo group.*/
            pSh->EndAction();
            pSh->EndUndo(UIUNDO_REPLACE);

            pSh->SetInsMode( bOldIns );
        }
        else
            switch( nRet )
            {
                case MN_SPELLING:
                {
                    pSh->Left(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
                    {
                        Reference<XDictionaryList> xDictionaryList( SvxGetDictionaryList() );
                        SvxDicListChgClamp aClamp( xDictionaryList );
                        pSh->GetView().GetViewFrame()->GetDispatcher()->
                            Execute( FN_SPELLING_DLG, SFX_CALLMODE_ASYNCHRON );
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

                    if( MN_LANGUAGE_PARA == nRet )
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
                    if(nRet >= MN_INSERT_START )
                    {
                        OUString aWord( xSpellAlt->getWord() );
                        INT32 nDicIdx = nRet - MN_INSERT_START - 1;
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
    return nRet;
}

