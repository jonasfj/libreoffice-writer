/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: hhcwrp.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 10:24:58 $
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
#ifdef PRECOMPILED
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>         // fuer Undo-Ids
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _SPLARGS_HXX
#include <splargs.hxx>
#endif


#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX
#include <svx/fontitem.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _COM_SUN_STAR_TEXT_RUBYADJUST_HPP_
#include <com/sun/star/text/RubyAdjust.hpp>
#endif

#ifndef _HHCWRP_HXX
#include <hhcwrp.hxx>
#endif
#ifndef _SDRHHCWRAP_HXX_
#include <sdrhhcwrap.hxx>
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>		// Progress
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif
#ifndef _INDEX_HXX
#include <index.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _SWCRSR_HXX
#include <swcrsr.hxx>
#endif
#ifndef _VISCRS_HXX
#include <viscrs.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FMTRUBY_HXX
#include <fmtruby.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif

#ifndef _OLMENU_HRC
#include <olmenu.hrc>
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::linguistic2;
using namespace ::com::sun::star::i18n;

#define C2U(cChar) OUString::createFromAscii(cChar)

#define CHAR_PAR_BRK    ((sal_Char) 0x0D)

//////////////////////////////////////////////////////////////////////
//     Beschreibung: Ggf. Rahmen/Objektshell abschalten

static void lcl_ActivateTextShell( SwWrtShell & rWrtSh )
{
    if( rWrtSh.IsSelFrmMode() || rWrtSh.IsObjSelected() )
        rWrtSh.EnterStdMode();
}

//////////////////////////////////////////////////////////////////////

class SwKeepConversionDirectionStateContext
{
public:
    SwKeepConversionDirectionStateContext()
    {
        //!! hack to transport the current conversion direction state settings
        //!! into the next incarnation that iterates over the drawing objets
        //!! ( see SwHHCWrapper::~SwHHCWrapper() )
        svx::HangulHanjaConversion::SetUseSavedConversionDirectionState( sal_True );
    }
    
    ~SwKeepConversionDirectionStateContext()
    {
        svx::HangulHanjaConversion::SetUseSavedConversionDirectionState( sal_False );
    }
};

//////////////////////////////////////////////////////////////////////

SwHHCWrapper::SwHHCWrapper(
        SwView* pSwView,
        const uno::Reference< lang::XMultiServiceFactory >& rxMSF,
        LanguageType nSourceLanguage,
        LanguageType nTargetLanguage,
        const Font *pTargetFont,
        sal_Int32 nConvOptions, 
        sal_Bool bIsInteractive,
        sal_Bool bStart, sal_Bool bOther, sal_Bool bSelection ) :
    svx::HangulHanjaConversion( &pSwView->GetEditWin(), rxMSF, 
                                SvxCreateLocale( nSourceLanguage ),
                                SvxCreateLocale( nTargetLanguage ),
                                pTargetFont,
                                nConvOptions,
                                bIsInteractive ),
    rWrtShell( pSwView->GetWrtShell() )
{
    pConvArgs       = 0;
    nLastPos        = 0;
    nUnitOffset     = 0;

    pView           = pSwView;
    pWin            = &pSwView->GetEditWin();
    bIsDrawObj      = sal_False;
    bIsStart        = bStart;
    bIsOtherCntnt   = bStartChk     = bOther;
    bIsConvSpecial  = sal_True;
    bIsSelection    = bSelection;
    bInfoBox        = sal_False;
    bStartDone  = bOther || bStart;
    bEndDone    = sal_False;
//    bLastRet    = sal_True;
    nPageCount  = nPageStart = 0;
}


SwHHCWrapper::~SwHHCWrapper()
{
    delete pConvArgs;

    rWrtShell.SetCareWin( NULL );

    // check for existence of a draw view which means that there are 
    // (or previously were) draw objects present in the document.
    // I.e. we like to check those too.
    if ( IsDrawObj() /*&& bLastRet*/ && pView->GetWrtShell().HasDrawView() )
    {
        Cursor *pSave = pView->GetWindow()->GetCursor();
        {
            SwKeepConversionDirectionStateContext aContext;

            SdrHHCWrapper aSdrConvWrap( pView, GetSourceLanguage(),
                    GetTargetLanguage(), GetTargetFont(), 
                    GetConversionOptions(), IsInteractive() );
            aSdrConvWrap.StartTextConversion();
        }
        pView->GetWindow()->SetCursor( pSave );
    }

    if( nPageCount )
        ::EndProgress( pView->GetDocShell() );
/*
    if( bInfoBox )
        InfoBox(&pView->GetEditWin(), String(SW_RES(STR_SPELL_OK)) ).Execute();
*/
}


void SwHHCWrapper::GetNextPortion( 
        ::rtl::OUString&    rNextPortion, 
        LanguageType&       rLangOfPortion )
{
    FindConvText_impl();
    rNextPortion    = pConvArgs->aConvText;
    rLangOfPortion  = pConvArgs->nConvTextLang;
    
    nUnitOffset  = 0;

    // build last pos from currently selected text
    SwPaM* pCrsr = rWrtShell.GetCrsr();
    nLastPos =  pCrsr->Start()->nContent.GetIndex();
}


void SwHHCWrapper::SelectNewUnit_impl( sal_Int32 nUnitStart, sal_Int32 nUnitEnd )
{
    SwPaM *pCrsr = rWrtShell.GetCrsr();
    pCrsr->GetPoint()->nContent = nLastPos;
    pCrsr->DeleteMark();

    rWrtShell.Right( CRSR_SKIP_CHARS, /*bExpand*/ sal_False,
                  (USHORT) (nUnitOffset + nUnitStart), sal_True );
    pCrsr->SetMark();
    rWrtShell.Right( CRSR_SKIP_CHARS, /*bExpand*/ sal_True,
                  (USHORT) (nUnitEnd - nUnitStart), sal_True );
    // end selection now. Otherwise SHIFT+HOME (extending the selection) 
    // won't work when the dialog is closed without any replacement.
    // (see #116346#)
    rWrtShell.EndSelect();
}


void SwHHCWrapper::HandleNewUnit(
        const sal_Int32 nUnitStart, const sal_Int32 nUnitEnd )
{
    DBG_ASSERT( nUnitStart >= 0 && nUnitEnd >= nUnitStart, "wrong arguments" );
    if (!(0 <= nUnitStart && nUnitStart <= nUnitEnd))
        return;

    lcl_ActivateTextShell( rWrtShell );

    rWrtShell.StartAllAction();

    // select current unit
    SelectNewUnit_impl( nUnitStart, nUnitEnd );

    rWrtShell.EndAllAction();
}


void SwHHCWrapper::ReplaceUnit(
         const sal_Int32 nUnitStart, const sal_Int32 nUnitEnd,
         const OUString& rReplaceWith,
         ReplacementAction eAction,
         LanguageType *pNewUnitLanguage )
{
    static OUString aBracketedStart( C2U( "(" ) );
    static OUString aBracketedEnd( C2U( ")" ) );

    DBG_ASSERT( nUnitStart >= 0 && nUnitEnd >= nUnitStart, "wrong arguments" );
    if (!(nUnitStart >= 0 && nUnitEnd >= nUnitStart))
        return;

    lcl_ActivateTextShell( rWrtShell );

    // Das aktuelle Wort austauschen
    rWrtShell.StartAllAction();

    // select current unit
    SelectNewUnit_impl( nUnitStart, nUnitEnd );

    OUString aOrigTxt( rWrtShell.GetSelTxt() );
    OUString aNewTxt( rReplaceWith );
    SwFmtRuby *pRuby = 0;
    sal_Bool bRubyBelow = sal_False;
    String  aNewOrigText;
    switch (eAction)
    {
        case eExchange :
        break;
        case eReplacementBracketed :
        {
            (((aNewTxt = aOrigTxt) += aBracketedStart) += rReplaceWith) += aBracketedEnd;
        }
        break;
        case eOriginalBracketed :
        {
            (((aNewTxt = rReplaceWith) += aBracketedStart) += aOrigTxt) += aBracketedEnd;
        }
        break;
        case eReplacementAbove  :
        {
            pRuby = new SwFmtRuby( rReplaceWith );
        }
        break;
        case eOriginalAbove :
        {
            pRuby = new SwFmtRuby( aOrigTxt );
            aNewOrigText = rReplaceWith;
        }
        break;
        case eReplacementBelow :
        {
            pRuby = new SwFmtRuby( rReplaceWith );
            bRubyBelow = sal_True;
        }
        break;
        case eOriginalBelow :
        {
            pRuby = new SwFmtRuby( aOrigTxt );
            aNewOrigText = rReplaceWith;
            bRubyBelow = sal_True;
        }
        break;
        default:
            DBG_ERROR( "unexpected case" );
    }
    nUnitOffset += nUnitStart + aNewTxt.getLength();

    if (pRuby)
    {
        rWrtShell.StartUndo( UNDO_SETRUBYATTR );
        if (aNewOrigText.Len())
        {
            rWrtShell.Delete();
            rWrtShell.Insert( aNewOrigText );

            //!! since Delete, Insert do not set the WrtShells bInSelect flag
            //!! back to false we do it now manually in order for the selection
            //!! to be done properly in the following call to Left.
            // We didn't fix it in Delete and Insert since it is currently
            // unclear if someone depends on this incorrect behvaiour
            // of the flag.
            rWrtShell.EndSelect();

            rWrtShell.Left( 0, TRUE, aNewOrigText.Len(), TRUE, TRUE );
        }

        pRuby->SetPosition( bRubyBelow );
        pRuby->SetAdjustment( RubyAdjust_CENTER );
        //!! the following seem not to be needed
        //pRuby->SetCharFmtName( const String& rNm );
        //pRuby->SetCharFmtId( USHORT nNew );
#ifdef DEBUG
        SwPaM *pPaM = rWrtShell.GetCrsr();
#endif
        rWrtShell.SetAttr(*pRuby);
        delete pRuby;
        rWrtShell.EndUndo( UNDO_SETRUBYATTR );
    }
    else
    {
        rWrtShell.StartUndo( UNDO_OVERWRITE );
         rWrtShell.Delete();
        rWrtShell.Insert( aNewTxt );

        // change language and font if necessary
        if (IsChinese( GetSourceLanguage() ))
        {
            rWrtShell.SetMark();
            rWrtShell.GetCrsr()->GetMark()->nContent -= (xub_StrLen) aNewTxt.getLength();

            DBG_ASSERT( GetTargetLanguage() == LANGUAGE_CHINESE_SIMPLIFIED || GetTargetLanguage() == LANGUAGE_CHINESE_TRADITIONAL, 
                    "SwHHCWrapper::ReplaceUnit : unexpected target language" );

            sal_uInt16 aRanges[] = {
                    RES_CHRATR_CJK_LANGUAGE, RES_CHRATR_CJK_LANGUAGE,
                    RES_CHRATR_CJK_FONT,     RES_CHRATR_CJK_FONT,
                    0, 0, 0  };

            SfxItemSet aSet( rWrtShell.GetAttrPool(), aRanges );
            if (pNewUnitLanguage)
            {
                //DBG_ASSERT(!IsSimilarChinese( *pNewUnitLanguage, nOldLang ), 
                //		"similar language should not be changed!");
                aSet.Put( SvxLanguageItem( *pNewUnitLanguage, RES_CHRATR_CJK_LANGUAGE ) );
            }

            const Font *pTargetFont = GetTargetFont();
            DBG_ASSERT( pTargetFont, "target font missing?" );
            if (pTargetFont && pNewUnitLanguage)
            {
                SvxFontItem aFontItem = (SvxFontItem&) aSet.Get( RES_CHRATR_CJK_FONT );
                aFontItem.GetFamilyName()   = pTargetFont->GetName();
                aFontItem.GetFamily()       = pTargetFont->GetFamily();
                aFontItem.GetStyleName()    = pTargetFont->GetStyleName();
                aFontItem.GetPitch()        = pTargetFont->GetPitch();
                aFontItem.GetCharSet()      = pTargetFont->GetCharSet();
                aSet.Put( aFontItem );
            }

            rWrtShell.SetAttr( aSet );

            rWrtShell.ClearMark();
        }
        
        rWrtShell.EndUndo( UNDO_OVERWRITE );
    }

    rWrtShell.EndAllAction();
}


sal_Bool SwHHCWrapper::HasRubySupport() const
{
    return sal_True;
}

    
void SwHHCWrapper::Convert()
{
    DBG_ASSERT( pConvArgs == 0, "NULL pointer expected" );
    {
        SwPaM *pCrsr = pView->GetWrtShell().GetCrsr();
        SwPosition* pSttPos = pCrsr->Start();
        SwPosition* pEndPos = pCrsr->End();
        pConvArgs = new SwConversionArgs( GetSourceLanguage(),
                            pSttPos->nNode.GetNode().GetTxtNode(), pSttPos->nContent,
                            pEndPos->nNode.GetNode().GetTxtNode(), pEndPos->nContent );

        // if it is not just a selection and we are about to begin
        // with the current conversion for the very first time 
        // we need to find the start of the current (initial)
        // convertible unit in order for the text conversion to give 
        // the correct result for that. Since it is easier to obtain 
        // the start of the word we use that though.
        if (!pCrsr->HasMark())   // is not a selection?
        {
            // since #118246 / #117803 still occurs if the cursor is placed
            // between the two chinese characters to be converted (because both 
            // of them are words on their own!) using the word boundary here does 
            // not work. Thus since chinese conversion is not interactive we start
            // at the begin of the paragraph to solve the problem, i.e. have the
            // TextConversion service get those charcters together in the same call.
            xub_StrLen nStartIdx = STRING_MAXLEN;
            if (svx::HangulHanjaConversion::IsChinese( GetSourceLanguage() ) )
                nStartIdx = 0;
            else
            {
                OUString aText( pConvArgs->pStartNode->GetTxt() );
                long     nPos = pConvArgs->pStartIdx->GetIndex();
                Boundary aBoundary( pBreakIt->GetBreakIter()->
                        getWordBoundary( aText, nPos, pBreakIt->GetLocale( pConvArgs->nConvSrcLang ),
                                WordType::DICTIONARY_WORD, sal_True ) );
                
                // valid result found?
                if (aBoundary.startPos < aText.getLength() &&
                    aBoundary.startPos != aBoundary.endPos)
                {
                    nStartIdx = static_cast< xub_StrLen >(aBoundary.startPos );
                }
            }

            if (STRING_MAXLEN != nStartIdx)
                *pConvArgs->pStartIdx = nStartIdx;
        }
    }

    if ( bIsOtherCntnt )
        ConvStart_impl( pConvArgs, SVX_SPELL_OTHER );
    else
    {
        bStartChk = sal_False;
        ConvStart_impl( pConvArgs,  SVX_SPELL_BODY_END );
    }

    ConvertDocument();

    ConvEnd_impl( pConvArgs );
}


sal_Bool SwHHCWrapper::ConvNext_impl( )
{
    //! modified version of SvxSpellWrapper::SpellNext

    // Keine Richtungsaenderung, also ist der gewuenschte Bereich ( bStartChk )
    // vollstaendig abgearbeitet.
    if( bStartChk )
        bStartDone = sal_True;
    else
        bEndDone = sal_True;

    if( bIsOtherCntnt && bStartDone && bEndDone ) // Dokument komplett geprueft?
    {
        bInfoBox = sal_True;
        return sal_False;
    }

    //ResMgr* pMgr = DIALOG_MGR();
    sal_Bool bGoOn = sal_False;

    if ( bIsOtherCntnt )
    {
        bStartChk = sal_False;
        ConvStart_impl( pConvArgs, SVX_SPELL_BODY );
        bGoOn = sal_True;
    }
    else if ( bStartDone && bEndDone )
    {
        // Bodybereich erledigt, Frage nach Sonderbereich
        if( bIsConvSpecial && HasOtherCnt_impl() )
        {
            ConvStart_impl( pConvArgs, SVX_SPELL_OTHER );
            bIsOtherCntnt = bGoOn = sal_True;
        }
        else
            bInfoBox = sal_True;
    }
    else
    {
        // Ein BODY_Bereich erledigt, Frage nach dem anderen BODY_Bereich
/*
        //pWin->LeaveWait();

        sal_uInt16 nResId = RID_SVXQB_CONTINUE;
        QueryBox aBox( pWin, ResId( nResId, pMgr ) );
        if ( aBox.Execute() != RET_YES )
        {
            // Verzicht auf den anderen Bereich, ggf. Frage nach Sonderbereich
            //pWin->EnterWait();
            bStartDone = bEndDone = sal_True;
            return SpellNext();
        }
        else
        {
*/
            bStartChk = !bStartDone;
            ConvStart_impl( pConvArgs, bStartChk ? SVX_SPELL_BODY_START : SVX_SPELL_BODY_END );
            bGoOn = sal_True;
/*
        }
        pWin->EnterWait();
*/
    }
    return bGoOn;
}


sal_Bool SwHHCWrapper::FindConvText_impl()
{
    //! modified version of SvxSpellWrapper::FindSpellError

    //ShowLanguageErrors();

    sal_Bool bFound = sal_False;

    pWin->EnterWait();
    sal_Bool bConv = sal_True;

    while ( bConv )
    {
        bFound = ConvContinue_impl( pConvArgs );
        if (bFound)
        {
            bConv = sal_False;
        }
        else
        {
            ConvEnd_impl( pConvArgs );
            bConv = ConvNext_impl();
        }
    }
    pWin->LeaveWait();
    return bFound;
}


sal_Bool SwHHCWrapper::HasOtherCnt_impl()
{
    return bIsSelection ? sal_False : rWrtShell.HasOtherCnt();
}


void SwHHCWrapper::ConvStart_impl( SwConversionArgs /* [out] */ *pConvArgs, SvxSpellArea eArea )
{
    SetDrawObj( SVX_SPELL_OTHER == eArea );
    pView->SpellStart( eArea, bStartDone, bEndDone, /* [out] */pConvArgs );
}


void SwHHCWrapper::ConvEnd_impl( SwConversionArgs *pConvArgs )
{
    pView->SpellEnd( pConvArgs );
    //ShowLanguageErrors();
}


sal_Bool SwHHCWrapper::ConvContinue_impl( SwConversionArgs *pConvArgs )
{
    sal_Bool bProgress = !bIsDrawObj && !bIsSelection;
//    bLastRet = aConvText.getLength() == 0;
    pConvArgs->aConvText = OUString();
    pConvArgs->nConvTextLang = LANGUAGE_NONE;
    uno::Any  aRet = bProgress ?
        pView->GetWrtShell().SpellContinue( &nPageCount, &nPageStart, pConvArgs ) :
        pView->GetWrtShell().SpellContinue( &nPageCount, NULL, pConvArgs );
    //aRet >>= aConvText;
    return pConvArgs->aConvText.getLength() != 0;
}

//////////////////////////////////////////////////////////////////////

