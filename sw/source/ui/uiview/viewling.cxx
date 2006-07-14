/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: viewling.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-14 08:34:14 $
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

#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>		// fuer ITEMID_SPELLCHECK (muss _vor_ sfxdlg.hxx!)
#endif


#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef _COM_SUN_STAR_LINGUISTIC2_XTHESAURUS_HPP_
#include <com/sun/star/linguistic2/XThesaurus.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_TEXTCONVERSIONOPTION_HPP_
#include <com/sun/star/i18n/TextConversionOption.hpp>
#endif

#ifndef _LINGUISTIC_LNGPROPS_HHX_
#include <linguistic/lngprops.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#include <toolkit/helper/vclunohelper.hxx>
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _EHDL_HXX //autogen
#include <svtools/ehdl.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVX_DLGUTIL_HXX
#include <svx/dlgutil.hxx>
#endif
#ifndef _SVX_DIALMGR_HXX //autogen
#include <svx/dialmgr.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif
#ifndef _SVXERR_HXX
#include <svx/svxerr.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _SVX_THESDLG_HXX //autogen
#include <svx/thesdlg.hxx>
#endif
#ifndef SVX_SPELL_PORTIONS_HXX
#include <svx/SpellPortions.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif
#ifndef _INITUI_HXX
#include <initui.hxx>				// fuer SpellPointer
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _BASESH_HXX
#include <basesh.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>				// CheckSpellChanges
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>				// Viewoptions
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>               // fuer Undo-Ids
#endif
#ifndef _HYP_HXX
#include <hyp.hxx>                  // Trennung
#endif
#ifndef _OLMENU_HXX
#include <olmenu.hxx>				// PopupMenu fuer OnlineSpelling
#endif
#ifndef _PAM_HXX
#include <pam.hxx>					// Spelling: Multiselektion
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _COMCORE_HRC
#include <comcore.hrc>				// STR_MULT_INTERACT_SPELL_WARN
#endif
#ifndef _VIEW_HRC
#include <view.hrc>
#endif
#ifndef _HHCWRP_HXX
#include <hhcwrp.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif

#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <cppuhelper/bootstrap.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::linguistic2;

#define C2U(cChar) rtl::OUString::createFromAscii(cChar)
/*--------------------------------------------------------------------
    Beschreibung:	Lingu-Dispatcher
 --------------------------------------------------------------------*/


void SwView::ExecLingu(SfxRequest &rReq)
{
    switch(rReq.GetSlot())
    {
        case FN_THESAURUS_DLG:
            StartThesaurus();
            rReq.Ignore();
            break;
        case SID_HANGUL_HANJA_CONVERSION:
            StartTextConversion( LANGUAGE_KOREAN, LANGUAGE_KOREAN, NULL, 
                    i18n::TextConversionOption::CHARACTER_BY_CHARACTER, sal_True );
            break;
        case SID_CHINESE_CONVERSION:
        {
            //open ChineseTranslationDialog
            Reference< XComponentContext > xContext(
                ::cppu::defaultBootstrap_InitialComponentContext() ); //@todo get context from calc if that has one
            if(xContext.is())
            {
                Reference< lang::XMultiComponentFactory > xMCF( xContext->getServiceManager() );
                if(xMCF.is())
                {
                    Reference< ui::dialogs::XExecutableDialog > xDialog(
                            xMCF->createInstanceWithContext(
                                rtl::OUString::createFromAscii("com.sun.star.linguistic2.ChineseTranslationDialog")
                                , xContext), UNO_QUERY);
                    Reference< lang::XInitialization > xInit( xDialog, UNO_QUERY );
                    if( xInit.is() )
                    {
                        //	initialize dialog
                        Reference< awt::XWindow > xDialogParentWindow(0);
                        Sequence<Any> aSeq(1);
                        Any* pArray = aSeq.getArray();
                        PropertyValue aParam;
                        aParam.Name = rtl::OUString::createFromAscii("ParentWindow");
                        aParam.Value <<= makeAny(xDialogParentWindow);
                        pArray[0] <<= makeAny(aParam);
                        xInit->initialize( aSeq );

                        //execute dialog
                        sal_Int16 nDialogRet = xDialog->execute();
                        if( RET_OK == nDialogRet )
                        {
                            //get some parameters from the dialog
                            sal_Bool bToSimplified = sal_True;
                            sal_Bool bUseVariants = sal_True;
                            sal_Bool bCommonTerms = sal_True;
                            Reference< beans::XPropertySet >  xProp( xDialog, UNO_QUERY );
                            if( xProp.is() )
                            {
                                try
                                {
                                    xProp->getPropertyValue( C2U("IsDirectionToSimplified") ) >>= bToSimplified;
                                    xProp->getPropertyValue( C2U("IsUseCharacterVariants") ) >>= bUseVariants;
                                    xProp->getPropertyValue( C2U("IsTranslateCommonTerms") ) >>= bCommonTerms;
                                }
                                catch( Exception& )
                                {
                                }
                            }

                            //execute translation
                            sal_Int16 nSourceLang = bToSimplified ? LANGUAGE_CHINESE_TRADITIONAL : LANGUAGE_CHINESE_SIMPLIFIED;
                            sal_Int16 nTargetLang = bToSimplified ? LANGUAGE_CHINESE_SIMPLIFIED : LANGUAGE_CHINESE_TRADITIONAL;
                            sal_Int32 nOptions    = bUseVariants ? i18n::TextConversionOption::USE_CHARACTER_VARIANTS : 0;
                            if( !bCommonTerms )
                                nOptions = nOptions | i18n::TextConversionOption::CHARACTER_BY_CHARACTER;
                            
                            Font aTargetFont = GetEditWin().GetDefaultFont( DEFAULTFONT_CJK_TEXT, 
                                                    nTargetLang, DEFAULTFONT_FLAGS_ONLYONE );

                            // disallow formatting, updating the view, ... while
                            // converting the document. (saves time)
                            // Also remember the current view and cursor position for later
                            pWrtShell->StartAction();

                            // remember cursor position data for later restoration of the cursor
                            const SwPosition *pPoint = pWrtShell->GetCrsr()->GetPoint();
                            sal_Bool bRestoreCursor = pPoint->nNode.GetNode().IsTxtNode();
                            const SwNodeIndex aPointNodeIndex( pPoint->nNode );
                            xub_StrLen nPointIndex = pPoint->nContent.GetIndex();;

                            // since this conversion is not interactive the whole converted
                            // document should be undone in a single undo step.
                            pWrtShell->StartUndo( UNDO_OVERWRITE );
                            
                            StartTextConversion( nSourceLang, nTargetLang, &aTargetFont, nOptions, sal_False );
                            
                            pWrtShell->EndUndo( UNDO_OVERWRITE );

                            if (bRestoreCursor)
                            {
                                SwTxtNode *pTxtNode = aPointNodeIndex.GetNode().GetTxtNode();
                                // check for unexpected error case
                                DBG_ASSERT( pTxtNode && pTxtNode->GetTxt().Len() >= nPointIndex,
                                    "text missing: corrupted node?" );
                                if (!pTxtNode || pTxtNode->GetTxt().Len() < nPointIndex)
                                    nPointIndex = 0;
                                // restore cursor to its original position
                                pWrtShell->GetCrsr()->GetPoint()->nContent.Assign( pTxtNode, nPointIndex );
                            }

                            // enable all, restore view and cursor position
                            pWrtShell->EndAction();
                        }
                    }
                    Reference< lang::XComponent > xComponent( xDialog, UNO_QUERY );
                    if( xComponent.is() )
                        xComponent->dispose();
                }
            }
            break;
        }
        case FN_HYPHENATE_OPT_DLG:
            HyphenateDocument();
            break;
        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }
}

/*--------------------------------------------------------------------
    Description: start language specific text conversion
 --------------------------------------------------------------------*/

void SwView::StartTextConversion( 
        LanguageType nSourceLang, 
        LanguageType nTargetLang,
        const Font *pTargetFont,
        sal_Int32 nOptions,
        sal_Bool bIsInteractive )
{
    // do not do text conversion if it is active elsewhere
    if (GetWrtShell().HasConvIter())
    {
//        MessBox( 0, WB_OK, String( SW_RES( STR_SPELL_TITLE ) ),
//                String( SW_RES( STR_MULT_INTERACT_SPELL_WARN ) ) ).Execute();
        return;
    }
/*
    SfxErrorContext aContext( ERRCTX_SVX_LINGU_SPELLING, aEmptyStr, pEditWin,
         RID_SVXERRCTX, DIALOG_MGR() );

    Reference< XSpellChecker1 >  xSpell = ::GetSpellChecker();
    if(!xSpell.is())
    {   // keine Arme keine Kekse
        ErrorHandler::HandleError( ERRCODE_SVX_LINGU_LINGUNOTEXISTS );
        return;
    }
*/
    SpellKontext(sal_True);

    SwViewOption* pVOpt = (SwViewOption*)pWrtShell->GetViewOptions();
    sal_Bool bOldIdle = pVOpt->IsIdle();
    pVOpt->SetIdle( sal_False );

    sal_Bool bOldIns = pWrtShell->IsInsMode();
    pWrtShell->SetInsMode( sal_True );

    
    {
        sal_Bool bSelection = ((SwCrsrShell*)pWrtShell)->HasSelection() ||
            pWrtShell->GetCrsr() != pWrtShell->GetCrsr()->GetNext();

//        sal_Bool bIsSpellSpecial = sal_True;

        sal_Bool    bStart = bSelection || pWrtShell->IsStartOfDoc();
        sal_Bool    bOther = !bSelection && !(pWrtShell->GetFrmType(0,sal_True) & FRMTYPE_BODY);

/*
        if( bOther && !bIsSpellSpecial )
        // kein Sonderbereich eingeschaltet
        {
            // Ich will auch in Sonderbereichen trennen
            QueryBox aBox( &GetEditWin(), SW_RES( DLG_SPECIAL_FORCED ) );
            if( aBox.Execute() == RET_YES  &&  xProp.is())
            {
                sal_Bool bTrue = sal_True;
                Any aTmp(&bTrue, ::getBooleanCppuType());
                xProp->setPropertyValue( C2U(UPN_IS_SPELL_SPECIAL), aTmp );
            }
            else
                return; // Nein Es wird nicht gespellt
        }
*/
        {
            const uno::Reference< lang::XMultiServiceFactory > xMgr(
                        comphelper::getProcessServiceFactory() );
            SwHHCWrapper aWrap( this, xMgr, nSourceLang, nTargetLang, pTargetFont,
                                nOptions, bIsInteractive,
                                bStart, bOther, bSelection );
            SwCrsrShell *pCrsrSh = ((SwCrsrShell*) pWrtShell);

            aWrap.Convert();
        }
    }


    pWrtShell->SetInsMode( bOldIns );
    pVOpt->SetIdle( bOldIdle );
    SpellKontext(sal_False);
}

/*--------------------------------------------------------------------
     spellcheck and text conversion related stuff
 --------------------------------------------------------------------*/

void SwView::SpellStart( SvxSpellArea eWhich,
        sal_Bool bStartDone, sal_Bool bEndDone,
        SwConversionArgs *pConvArgs )
{
    Reference< beans::XPropertySet >  xProp( ::GetLinguPropertySet() );
    sal_Bool bIsWrapReverse = (!pConvArgs && xProp.is()) ?
            *(sal_Bool*)xProp->getPropertyValue( C2U(UPN_IS_WRAP_REVERSE) ).getValue() : sal_False;

    SwDocPositions eStart = DOCPOS_START;
    SwDocPositions eEnde  = DOCPOS_END;
    SwDocPositions eCurr  = DOCPOS_CURR;
    switch ( eWhich )
    {
        case SVX_SPELL_BODY:
            if( bIsWrapReverse )
                eCurr = DOCPOS_END;
            else
                eCurr = DOCPOS_START;
            break;
        case SVX_SPELL_BODY_END:
            if( bIsWrapReverse )
            {
                if( bStartDone )
                    eStart = DOCPOS_CURR;
                eCurr = DOCPOS_END;
            }
            else if( bStartDone )
                eCurr = DOCPOS_START;
            break;
        case SVX_SPELL_BODY_START:
            if( !bIsWrapReverse )
            {
                if( bEndDone )
                    eEnde = DOCPOS_CURR;
                eCurr = DOCPOS_START;
            }
            else if( bEndDone )
                eCurr = DOCPOS_END;
            break;
        case SVX_SPELL_OTHER:
            if( bIsWrapReverse )
            {
                eStart = DOCPOS_OTHERSTART;
                eEnde  = DOCPOS_OTHEREND;
                eCurr = DOCPOS_OTHEREND;
            }
            else
            {
                eStart = DOCPOS_OTHERSTART;
                eEnde  = DOCPOS_OTHEREND;
                eCurr = DOCPOS_OTHERSTART;
            }
            break;
        default:
            ASSERT( !this, "SpellStart with unknown Area" );
    }
    pWrtShell->SpellStart( eStart, eEnde, eCurr, pConvArgs );
}

/*--------------------------------------------------------------------
    Beschreibung: Fehlermeldung beim Spelling
 --------------------------------------------------------------------*/


// Der uebergebene Pointer nLang ist selbst der Wert
IMPL_LINK( SwView, SpellError, void *, nLang )
{
#if OSL_DEBUG_LEVEL > 1
    sal_Bool bFocus = GetEditWin().HasFocus();
#endif
    sal_uInt16 nPend = 0;

    if ( pWrtShell->ActionPend() )
    {
        pWrtShell->Push();
        pWrtShell->ClearMark();
        do
        {
            pWrtShell->EndAction();
            ++nPend;
        }
        while( pWrtShell->ActionPend() );
    }
    LanguageType eLang = (LanguageType)(sal_uIntPtr)nLang;
    String aErr(::GetLanguageString( eLang ) );

    SwEditWin &rEditWin = GetEditWin();
#if OSL_DEBUG_LEVEL > 1
    bFocus = rEditWin.HasFocus();
#endif
    sal_uInt16 nWaitCnt = 0;
    while( rEditWin.IsWait() )
    {
        rEditWin.LeaveWait();
        ++nWaitCnt;
    }
    if ( LANGUAGE_NONE == eLang )
        ErrorHandler::HandleError( ERRCODE_SVX_LINGU_NOLANGUAGE );
    else
        ErrorHandler::HandleError( *new StringErrorInfo( ERRCODE_SVX_LINGU_LANGUAGENOTEXISTS, aErr ) );

    while( nWaitCnt )
    {
        rEditWin.EnterWait();
        --nWaitCnt;
    }
#if OSL_DEBUG_LEVEL > 1
    bFocus = GetEditWin().HasFocus();
#endif

    if ( nPend )
    {
        while( nPend-- )
            pWrtShell->StartAction();
        pWrtShell->Combine();
    }
#if OSL_DEBUG_LEVEL > 1
    if( !bFocus )
        GetEditWin().GrabFocus();
#endif

    return 0;
}

/*--------------------------------------------------------------------
     Beschreibung: Spelling beenden und Cursor wiederherstellen
 --------------------------------------------------------------------*/


void SwView::SpellEnd( SwConversionArgs *pConvArgs )
{
    pWrtShell->SpellEnd( pConvArgs );
    if( pWrtShell->IsExtMode() )
        pWrtShell->SetMark();
}


void SwView::HyphStart( SvxSpellArea eWhich )
{
    switch ( eWhich )
    {
        case SVX_SPELL_BODY:
            pWrtShell->HyphStart( DOCPOS_START, DOCPOS_END );
            break;
        case SVX_SPELL_BODY_END:
            pWrtShell->HyphStart( DOCPOS_CURR, DOCPOS_END );
            break;
        case SVX_SPELL_BODY_START:
            pWrtShell->HyphStart( DOCPOS_START, DOCPOS_CURR );
            break;
        case SVX_SPELL_OTHER:
            pWrtShell->HyphStart( DOCPOS_OTHERSTART, DOCPOS_OTHEREND );
            break;
        default:
            ASSERT( !this, "HyphStart with unknown Area" );
    }
}

/*--------------------------------------------------------------------
     Beschreibung: Interaktive Trennung
 --------------------------------------------------------------------*/


void SwView::HyphenateDocument()
{
    // do not hyphenate if interactive hyphenationg is active elsewhere
    if (GetWrtShell().HasHyphIter())
    {
        MessBox( 0, WB_OK, String( SW_RES( STR_HYPH_TITLE ) ),
                String( SW_RES( STR_MULT_INTERACT_HYPH_WARN ) ) ).Execute();
        return;
    }

    SfxErrorContext aContext( ERRCTX_SVX_LINGU_HYPHENATION, aEmptyStr, pEditWin,
         RID_SVXERRCTX, DIALOG_MGR() );

    Reference< XHyphenator >  xHyph( ::GetHyphenator() );
    if (!xHyph.is())
    {
        ErrorHandler::HandleError( ERRCODE_SVX_LINGU_LINGUNOTEXISTS );
        return;
    }

    if (pWrtShell->GetSelectionType() & (SwWrtShell::SEL_DRW_TXT|SwWrtShell::SEL_DRW))
    {
        // Silbentrennung in einem Draw-Objekt
        HyphenateDrawText();
    }
    else
    {
        SwViewOption* pVOpt = (SwViewOption*)pWrtShell->GetViewOptions();
        sal_Bool bOldIdle = pVOpt->IsIdle();
        pVOpt->SetIdle( sal_False );

        Reference< beans::XPropertySet >  xProp( ::GetLinguPropertySet() );


        pWrtShell->StartUndo(UNDO_INSATTR);       	// spaeter gueltig

        sal_Bool bHyphSpecial = xProp.is() ?
                *(sal_Bool*)xProp->getPropertyValue( C2U(UPN_IS_HYPH_SPECIAL) ).getValue() : sal_False;
        sal_Bool bSelection = ((SwCrsrShell*)pWrtShell)->HasSelection() ||
            pWrtShell->GetCrsr() != pWrtShell->GetCrsr()->GetNext();
        sal_Bool bOther = pWrtShell->HasOtherCnt() && bHyphSpecial && !bSelection;
        sal_Bool bStart = bSelection || ( !bOther && pWrtShell->IsStartOfDoc() );
        sal_Bool bStop = sal_False;
        if( !bOther && !(pWrtShell->GetFrmType(0,sal_True) & FRMTYPE_BODY) && !bSelection )
        // kein Sonderbereich eingeschaltet
        {
            // Ich will auch in Sonderbereichen trennen
            QueryBox aBox( &GetEditWin(), SW_RES( DLG_SPECIAL_FORCED ) );
            if( aBox.Execute() == RET_YES )
            {
                bOther = sal_True;
                if (xProp.is())
                {
                    sal_Bool bTrue = sal_True;
                    Any aTmp(&bTrue, ::getBooleanCppuType());
                    xProp->setPropertyValue( C2U(UPN_IS_HYPH_SPECIAL), aTmp );
                }
            }
            else
                bStop = sal_True; // Nein Es wird nicht getrennt
        }

        if( !bStop )
        {
            SwHyphWrapper aWrap( this, xHyph, bStart, bOther, bSelection );
            aWrap.SpellDocument();
            pWrtShell->EndUndo(UNDO_INSATTR);
        }
        pVOpt->SetIdle( bOldIdle );
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Thesaurus starten
 --------------------------------------------------------------------*/


void SwView::StartThesaurus()
{
    if(	pWrtShell->GetCrsr() != pWrtShell->GetCrsr()->GetNext() )
        return;
    sal_Bool bSelection = ((SwCrsrShell*)pWrtShell)->HasSelection();
    if( bSelection && !pWrtShell->IsSelOnePara() )
        return;

    SfxErrorContext aContext( ERRCTX_SVX_LINGU_THESAURUS, aEmptyStr, pEditWin,
         RID_SVXERRCTX, DIALOG_MGR() );

    // Sprache rausholen
    //
    LanguageType eLang = pWrtShell->GetCurLang();
    if( LANGUAGE_SYSTEM == eLang )
        eLang = GetAppLanguage();

    if( eLang == LANGUAGE_DONTKNOW || eLang == LANGUAGE_NONE )
    {
        SpellError( (void *) LANGUAGE_NONE );
        return;
    }

    SwViewOption* pVOpt = (SwViewOption*)pWrtShell->GetViewOptions();
    sal_Bool bOldIdle = pVOpt->IsIdle();
    pVOpt->SetIdle( sal_False );

#ifdef TL_NEVER
//!!! hier mu� noch was getan werden... (Umsetzung der Funktionalitaet)
    // ErrorLink setzen, alten merken
    Link aOldLnk = pSpell->ChgErrorLink(LINK(this, SwView, SpellError));
#endif


    // get initial LookUp text
    String aTmp = bSelection ?
        pWrtShell->GetSelTxt() : pWrtShell->GetCurWord();

    Reference< XThesaurus >  xThes( ::GetThesaurus() );
    SvxThesaurusDialog *pDlg = NULL;

    if ( !xThes.is() || !xThes->hasLocale( SvxCreateLocale( eLang ) ) )
    {
        SpellError( (void *) eLang );
    }
    else
    {
        // create dialog
        {	//Scope for SwWait-Object
            SwWait aWait( *GetDocShell(), sal_True );
            pDlg = new SvxThesaurusDialog( &GetEditWin(),
                                           xThes, aTmp, eLang );
        }

        {
            // Hier wird der Thesaurus-Dialog im Applikationsfenster zentriert,
            // und zwar oberhalb oder unterhalb der Cursorposition, je nachdem,
            // wo mehr Platz ist.

            // Current Word:
            SwRect aRect( pWrtShell->GetCharRect() );
            Point aTopPos = aRect.Pos();
            Point aBtmPos( aTopPos.X(), aRect.Bottom() );
            aTopPos = GetEditWin().LogicToPixel( aTopPos );
            aTopPos = GetEditWin().OutputToScreenPixel( aTopPos );
            aBtmPos = GetEditWin().LogicToPixel( aBtmPos );
            aBtmPos = GetEditWin().OutputToScreenPixel( aBtmPos );
            // ::frame::Desktop:
            Rectangle aRct = GetEditWin().GetDesktopRectPixel();
            Point aWinTop( aRct.TopLeft() );
            Point aWinBtm( aRct.BottomRight() );
            if ( aTopPos.Y() - aWinTop.Y() > aWinBtm.Y() - aBtmPos.Y() )
                aWinBtm.Y() = aTopPos.Y();
            else
                aWinTop.Y() = aBtmPos.Y();

            Size aSz = pDlg->GetSizePixel();
            if ( aWinBtm.Y() - aWinTop.Y() > aSz.Height() )
            {
                aWinTop.X() = ( aWinTop.X() + aWinBtm.X() - aSz.Width() ) / 2;
                aWinTop.Y() = ( aWinTop.Y() + aWinBtm.Y() - aSz.Height() ) / 2;
                pDlg->SetPosPixel( aWinTop );
            }
        }

        if ( pDlg->Execute()== RET_OK )
        {
            sal_Bool bOldIns = pWrtShell->IsInsMode();
            pWrtShell->SetInsMode( sal_True );

            pWrtShell->StartAllAction();
            pWrtShell->StartUndo(UNDO_DELETE);

            if( !bSelection )
            {
                if(pWrtShell->IsEndWrd())
                    pWrtShell->Left(CRSR_SKIP_CELLS, FALSE, 1, FALSE );

                pWrtShell->SelWrd();

                // make sure the selection build later from the 
                // data below does not include footnotes and other
                // "in word" character to the left and right in order
                // to preserve those. Therefore count those "in words"
                // in order to modify the selection accordingly.
                const sal_Unicode* pChar = aTmp.GetBuffer();
                xub_StrLen nLeft = 0;
                while (pChar && *pChar++ == CH_TXTATR_INWORD)
                    ++nLeft;
                pChar = aTmp.Len() ? aTmp.GetBuffer() + aTmp.Len() - 1 : 0;
                xub_StrLen nRight = 0;
                while (pChar && *pChar-- == CH_TXTATR_INWORD)
                    ++nRight;
                
                // adjust existing selection
                SwPaM *pCrsr = pWrtShell->GetCrsr();
                pCrsr->GetPoint()->nContent/*.nIndex*/ -= nRight;
                pCrsr->GetMark()->nContent/*.nIndex*/ += nLeft;
            }

            pWrtShell->Insert( pDlg->GetWord() );

            pWrtShell->EndUndo(UNDO_DELETE);
            pWrtShell->EndAllAction();

            pWrtShell->SetInsMode( bOldIns );

        }
    }

    delete pDlg;

    pVOpt->SetIdle( bOldIdle );

}

/*--------------------------------------------------------------------
    Beschreibung:	Online-Vorschlaege anbieten
 *--------------------------------------------------------------------*/


sal_Bool SwView::ExecSpellPopup(const Point& rPt)
{
    sal_Bool bRet = sal_False;
    const SwViewOption* pVOpt = pWrtShell->GetViewOptions();
    if( pVOpt->IsOnlineSpell() &&
        !pVOpt->IsHideSpell() &&
        !pWrtShell->IsSelection())
    {
        if (pWrtShell->GetSelectionType() & SwWrtShell::SEL_DRW_TXT)
            bRet = ExecDrwTxtSpellPopup(rPt);
        else if (!pWrtShell->IsSelFrmMode())
        {
            const sal_Bool bOldViewLock = pWrtShell->IsViewLocked();
            pWrtShell->LockView( sal_True );
            pWrtShell->Push();
            SwRect aToFill;
            Reference< XSpellAlternatives >  xAlt( pWrtShell->GetCorrection(&rPt, aToFill) );
            if ( xAlt.is() )
            {
                bRet = sal_True;
                pWrtShell->SttSelect();
                SwSpellPopup aPopup( pWrtShell, xAlt );
                ::com::sun::star::ui::ContextMenuExecuteEvent aEvent;
                const Point aPixPos = GetEditWin().LogicToPixel( rPt );

                aEvent.SourceWindow = VCLUnoHelper::GetInterface( pEditWin );
                aEvent.ExecutePosition.X = aPixPos.X();
                aEvent.ExecutePosition.Y = aPixPos.Y();
                Menu* pMenu = 0;

                if(TryContextMenuInterception( aPopup, pMenu, aEvent ))
                {        
                    if ( pMenu )
                    {
                        USHORT nId = ((PopupMenu*)pMenu)->Execute(pEditWin, aPixPos);
                        if(!ExecuteMenuCommand( *static_cast<PopupMenu*>(pMenu), *GetViewFrame(), nId ))
                            aPopup.Execute(nId);
                    }
                    else
                    {        
                        aPopup.Execute(
                            pEditWin,
                            aToFill.SVRect());
                    }
                }
            }

            pWrtShell->Pop( sal_False );
            pWrtShell->LockView( bOldViewLock );
        }
    }
    return bRet;
}

