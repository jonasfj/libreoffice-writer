/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: drwtxtex.cxx,v $
 * $Revision: 1.46.82.1 $
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
#include <hintids.hxx>

#include <tools/shl.hxx>
#include <svx/svdview.hxx>
#include <svx/spltitem.hxx>
#include <svx/orphitem.hxx>
#include <svx/brkitem.hxx>
#include <svx/widwitem.hxx>
#include <svx/kernitem.hxx>
#include <svx/escpitem.hxx>
#ifndef _SVX_PARAITEM_HXX //autogen
#include <svx/lspcitem.hxx>
#endif
#include <svx/adjitem.hxx>
#include <svx/crsditem.hxx>
#include <svx/shdditem.hxx>
#include <svx/hyznitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/fontitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/colritem.hxx>
#include <svx/wghtitem.hxx>
#ifndef _SVX_CNTRITEM_HXX //autogen
#include <svx/cntritem.hxx>
#endif
#include <svx/postitem.hxx>
#include <svx/frmdiritem.hxx>
#include <svx/svdoutl.hxx>
#include <sfx2/viewfrm.hxx>
#include <svtools/whiter.hxx>
#include <svtools/cjkoptions.hxx>
#include <svtools/ctloptions.hxx>
#include <sfx2/bindings.hxx>
#include <vcl/msgbox.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/request.hxx>
#include <svx/flditem.hxx>
#include <svx/editstat.hxx>
#include <svx/hlnkitem.hxx>
#include <svx/htmlmode.hxx>
#include <svx/langitem.hxx>
#include <svx/unolingu.hxx>
#include <svx/scripttypeitem.hxx>
#include <svx/writingmodeitem.hxx>
#include <doc.hxx>
#include <wview.hxx>
#include <viewopt.hxx>
#include <wrtsh.hxx>
#include <uitool.hxx>
#include <pardlg.hxx>
#include <swdtflvr.hxx>
#include <drwtxtsh.hxx>
#include <swmodule.hxx>
#include <initui.hxx>               // fuer SpellPointer
#include <edtwin.hxx>
#include <swwait.hxx>
#include <docstat.hxx>

#include <comphelper/processfactory.hxx>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>

#include <cmdid.h>
#include <globals.hrc>
#include <shells.hrc>
#include "swabstdlg.hxx"
#include "chrdlg.hrc"
#include "misc.hrc"

//modified on Jul. 30th
#include <svtools/languageoptions.hxx>
#include <svx/langitem.hxx>
#include <svtools/langtab.hxx>
#include <svtools/slstitm.hxx>
#include <string.h>

#include <svx/eeitem.hxx>
#include <svx/editeng.hxx>
#include <svx/editdata.hxx>
#include <svx/outliner.hxx>
#include <vcl/window.hxx>
#include <svx/editview.hxx>
#include <vcl/outdev.hxx>

#include <langhelper.hxx>

using namespace ::com::sun::star;

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwDrawTextShell::Execute( SfxRequest &rReq )
{
    SwWrtShell &rSh = GetShell();
        OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
        SfxItemSet aEditAttr(pOLV->GetAttribs());
    SfxItemSet aNewAttr(*aEditAttr.GetPool(), aEditAttr.GetRanges());

    sal_uInt16 nSlot = rReq.GetSlot();

    sal_uInt16 nWhich = GetPool().GetWhich(nSlot);
    const SfxItemSet *pNewAttrs = rReq.GetArgs();

    bool bRestoreSelection = false;
    ESelection aOldSelection;

    sal_uInt16 nEEWhich = 0;
    switch (nSlot)
    {
        case SID_LANGUAGE_STATUS:
        {
            aOldSelection = pOLV->GetSelection();
            if (!pOLV->GetEditView().HasSelection())
            {
                bRestoreSelection	= true;
                pOLV->GetEditView().SelectCurrentWord();
            }

            bRestoreSelection = SwLangHelper::SetLanguageStatus(pOLV,rReq,GetView(),rSh);
            break;
        }
        case SID_ATTR_CHAR_FONT:
        case SID_ATTR_CHAR_FONTHEIGHT:
        case SID_ATTR_CHAR_WEIGHT:
        case SID_ATTR_CHAR_POSTURE:
        {
            SfxItemPool* pPool2 = aEditAttr.GetPool()->GetSecondaryPool();
            if( !pPool2 )
                pPool2 = aEditAttr.GetPool();
            SvxScriptSetItem aSetItem( nSlot, *pPool2 );

            // #i78017 establish the same behaviour as in Writer
            USHORT nScriptTypes = SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN | SCRIPTTYPE_COMPLEX;
            if (nSlot == SID_ATTR_CHAR_FONT)
                nScriptTypes = pOLV->GetSelectedScriptType();

            aSetItem.PutItemForScriptType( nScriptTypes, pNewAttrs->Get( nWhich ) );
            aNewAttr.Put( aSetItem.GetItemSet() );
        }
        break;

        case SID_ATTR_CHAR_COLOR: nEEWhich = EE_CHAR_COLOR; break;

        case SID_ATTR_CHAR_UNDERLINE:
        {
             FontUnderline eFU = ((const SvxUnderlineItem&)aEditAttr.Get(EE_CHAR_UNDERLINE)).GetUnderline();
            aNewAttr.Put(SvxUnderlineItem(eFU == UNDERLINE_SINGLE ? UNDERLINE_NONE : UNDERLINE_SINGLE, EE_CHAR_UNDERLINE));
        }
        break;

        case SID_ATTR_CHAR_CONTOUR:     nEEWhich = EE_CHAR_OUTLINE; break;
        case SID_ATTR_CHAR_SHADOWED:    nEEWhich = EE_CHAR_SHADOW; break;
        case SID_ATTR_CHAR_STRIKEOUT:   nEEWhich = EE_CHAR_STRIKEOUT; break;
        case SID_ATTR_CHAR_WORDLINEMODE: nEEWhich = EE_CHAR_WLM; break;
        case SID_ATTR_CHAR_RELIEF      : nEEWhich = EE_CHAR_RELIEF;  break;
        case SID_ATTR_CHAR_LANGUAGE    : nEEWhich = EE_CHAR_LANGUAGE;break;
        case SID_ATTR_CHAR_KERNING     : nEEWhich = EE_CHAR_KERNING; break;
        case SID_ATTR_CHAR_SCALEWIDTH:   nEEWhich = EE_CHAR_FONTWIDTH; break;
        case SID_ATTR_CHAR_AUTOKERN  :   nEEWhich = EE_CHAR_PAIRKERNING; break;
        case SID_ATTR_CHAR_ESCAPEMENT:   nEEWhich = EE_CHAR_ESCAPEMENT; break;
        case SID_ATTR_PARA_ADJUST_LEFT:
            aNewAttr.Put(SvxAdjustItem(SVX_ADJUST_LEFT, EE_PARA_JUST));
        break;
        case SID_ATTR_PARA_ADJUST_CENTER:
            aNewAttr.Put(SvxAdjustItem(SVX_ADJUST_CENTER, EE_PARA_JUST));
        break;
        case SID_ATTR_PARA_ADJUST_RIGHT:
            aNewAttr.Put(SvxAdjustItem(SVX_ADJUST_RIGHT, EE_PARA_JUST));
        break;
        case SID_ATTR_PARA_ADJUST_BLOCK:
            aNewAttr.Put(SvxAdjustItem(SVX_ADJUST_BLOCK, EE_PARA_JUST));
        break;

        case SID_ATTR_PARA_LINESPACE_10:
        {
            SvxLineSpacingItem aItem(SVX_LINESPACE_ONE_LINE, EE_PARA_SBL);
            aItem.SetPropLineSpace(100);
            aNewAttr.Put(aItem);
        }
        break;
        case SID_ATTR_PARA_LINESPACE_15:
        {
            SvxLineSpacingItem aItem(SVX_LINESPACE_ONE_POINT_FIVE_LINES, EE_PARA_SBL);
            aItem.SetPropLineSpace(150);
            aNewAttr.Put(aItem);
        }
        break;
        case SID_ATTR_PARA_LINESPACE_20:
        {
            SvxLineSpacingItem aItem(SVX_LINESPACE_TWO_LINES, EE_PARA_SBL);
            aItem.SetPropLineSpace(200);
            aNewAttr.Put(aItem);
        }
        break;

        case FN_SET_SUPER_SCRIPT:
        {
            SvxEscapementItem aItem(EE_CHAR_ESCAPEMENT);
            SvxEscapement eEsc = (SvxEscapement ) ( (const SvxEscapementItem&)
                            aEditAttr.Get( EE_CHAR_ESCAPEMENT ) ).GetEnumValue();

            if( eEsc == SVX_ESCAPEMENT_SUPERSCRIPT )
                aItem.SetEscapement( SVX_ESCAPEMENT_OFF );
            else
                aItem.SetEscapement( SVX_ESCAPEMENT_SUPERSCRIPT );
            aNewAttr.Put( aItem, EE_CHAR_ESCAPEMENT );
        }
        break;
        case FN_SET_SUB_SCRIPT:
        {
            SvxEscapementItem aItem(EE_CHAR_ESCAPEMENT);
            SvxEscapement eEsc = (SvxEscapement ) ( (const SvxEscapementItem&)
                            aEditAttr.Get( EE_CHAR_ESCAPEMENT ) ).GetEnumValue();

            if( eEsc == SVX_ESCAPEMENT_SUBSCRIPT )
                aItem.SetEscapement( SVX_ESCAPEMENT_OFF );
            else
                aItem.SetEscapement( SVX_ESCAPEMENT_SUBSCRIPT );
            aNewAttr.Put( aItem, EE_CHAR_ESCAPEMENT );
        }
        break;

        case SID_CHAR_DLG:
        case SID_CHAR_DLG_FOR_PARAGRAPH:
        {
            const SfxItemSet* pArgs = rReq.GetArgs();

            if( !pArgs )
            {
                aOldSelection = pOLV->GetSelection();
                if (nSlot == SID_CHAR_DLG_FOR_PARAGRAPH)
                {
                    // select current paragraph (and restore selection later on...)
                    EditView & rEditView = pOLV->GetEditView();
                    SwLangHelper::SelectPara( rEditView, rEditView.GetSelection() );
                    bRestoreSelection = true;
                }

                SwView* pView = &GetView();
                FieldUnit eMetric = ::GetDfltMetric(0 != PTR_CAST(SwWebView, pView));
                SW_MOD()->PutItem(SfxUInt16Item(SID_ATTR_METRIC, static_cast< UINT16 >(eMetric)) );
                SfxItemSet aDlgAttr(GetPool(), EE_ITEMS_START, EE_ITEMS_END);

                // util::Language gibts an der EditEngine nicht! Daher nicht im Set.

                aDlgAttr.Put( aEditAttr );
                aDlgAttr.Put( SvxKerningItem(0, RES_CHRATR_KERNING) );

                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

                SfxAbstractTabDialog* pDlg = pFact->CreateSwCharDlg( pView->GetWindow(), *pView, aDlgAttr, DLG_CHAR,0, sal_True );
                DBG_ASSERT(pDlg, "Dialogdiet fail!");
                USHORT nRet = pDlg->Execute();
                if(RET_OK == nRet )
                {
                    rReq.Done( *( pDlg->GetOutputItemSet() ) );
                    aNewAttr.Put(*pDlg->GetOutputItemSet());
                }
                delete( pDlg );
                if(RET_OK != nRet)
                    return ;
            }
            else
                aNewAttr.Put(*pArgs);
        }
        break;
        case FN_FORMAT_FOOTNOTE_DLG:
        {
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

            VclAbstractDialog* pDlg = pFact->CreateSwFootNoteOptionDlg( GetView().GetWindow(), rView.GetWrtShell(), DLG_DOC_FOOTNOTE );
            DBG_ASSERT(pDlg, "Dialogdiet fail!");
            pDlg->Execute();
            delete pDlg;
            break;
        }
        case FN_NUMBERING_OUTLINE_DLG:
        {
            SfxItemSet aTmp(GetPool(), FN_PARAM_1, FN_PARAM_1);
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "Dialogdiet fail!");
            SfxAbstractTabDialog* pDlg = pFact->CreateSwTabDialog( DLG_TAB_OUTLINE,
                                                        GetView().GetWindow(), &aTmp, GetView().GetWrtShell());
            DBG_ASSERT(pDlg, "Dialogdiet fail!");
            pDlg->Execute();
            delete pDlg;
            rReq.Done();
        }
        break;
        case SID_OPEN_XML_FILTERSETTINGS:
        {
            try
            {
                uno::Reference < ui::dialogs::XExecutableDialog > xDialog(::comphelper::getProcessServiceFactory()->createInstance(rtl::OUString::createFromAscii("com.sun.star.comp.ui.XSLTFilterDialog")), uno::UNO_QUERY);
                if( xDialog.is() )
                {
                    xDialog->execute();
                }
            }
            catch( uno::Exception& )
            {
            }
            rReq.Ignore ();
        }
        break;
        case FN_WORDCOUNT_DIALOG:
        {
            SwDocStat aCurr;
            SwDocStat aDocStat( rSh.getIDocumentStatistics()->GetDocStat() );
            {
                SwWait aWait( *GetView().GetDocShell(), TRUE );
                rSh.StartAction();
                rSh.CountWords( aCurr );
                rSh.UpdateDocStat( aDocStat );
                rSh.EndAction();
            }

            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "Dialogdiet fail!");
            AbstractSwWordCountDialog* pDialog = pFact->CreateSwWordCountDialog( GetView().GetWindow() );
            pDialog->SetValues(aCurr, aDocStat );
            pDialog->Execute();
            delete pDialog;
        }
        break;
        case SID_PARA_DLG:
        {
            const SfxItemSet* pArgs = rReq.GetArgs();

            if (!pArgs)
            {
                SwView* pView = &GetView();
                FieldUnit eMetric = ::GetDfltMetric(0 != PTR_CAST(SwWebView, pView));
                SW_MOD()->PutItem(SfxUInt16Item(SID_ATTR_METRIC, static_cast< UINT16 >(eMetric)) );
                SfxItemSet aDlgAttr(GetPool(),
                                    EE_ITEMS_START, EE_ITEMS_END,
                                    SID_ATTR_PARA_HYPHENZONE, SID_ATTR_PARA_HYPHENZONE,
                                    SID_ATTR_PARA_SPLIT, SID_ATTR_PARA_SPLIT,
                                    SID_ATTR_PARA_WIDOWS, SID_ATTR_PARA_WIDOWS,
                                    SID_ATTR_PARA_ORPHANS, SID_ATTR_PARA_ORPHANS,
                                    0);

                aDlgAttr.Put(aEditAttr);

                // Die Werte sind erst einmal uebernommen worden, um den Dialog anzuzeigen.
                // Muss natuerlich noch geaendert werden
                // aDlgAttr.Put( SvxParaDlgLimitsItem( 567 * 50, 5670) );

                aDlgAttr.Put( SvxHyphenZoneItem( sal_False, RES_PARATR_HYPHENZONE) );
                aDlgAttr.Put( SvxFmtBreakItem( SVX_BREAK_NONE, RES_BREAK ) );
                aDlgAttr.Put( SvxFmtSplitItem( sal_True, RES_PARATR_SPLIT ) );
                aDlgAttr.Put( SvxWidowsItem( 0, RES_PARATR_WIDOWS ) );
                aDlgAttr.Put( SvxOrphansItem( 0, RES_PARATR_ORPHANS ) );

                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

                SfxAbstractTabDialog* pDlg = pFact->CreateSwParaDlg( GetView().GetWindow(), GetView(), aDlgAttr,DLG_STD, DLG_PARA, 0, sal_True );
                DBG_ASSERT(pDlg, "Dialogdiet fail!");
                USHORT nRet = pDlg->Execute();
                if(RET_OK == nRet)
                {
                    rReq.Done( *( pDlg->GetOutputItemSet() ) );
                    aNewAttr.Put(*pDlg->GetOutputItemSet());
                }
                delete( pDlg );
                if(RET_OK != nRet)
                    return;
            }
            else
                aNewAttr.Put(*pArgs);
        }
        break;
        case SID_AUTOSPELL_CHECK:
        {
//!! JP 16.03.2001: why??			pSdrView = rSh.GetDrawView();
//!! JP 16.03.2001: why??			pOutliner = pSdrView->GetTextEditOutliner();
            SdrOutliner * pOutliner = pSdrView->GetTextEditOutliner();
            sal_uInt32 nCtrl = pOutliner->GetControlWord();

            sal_Bool bSet = ((const SfxBoolItem&)rReq.GetArgs()->Get(
                                                    nSlot)).GetValue();
            if(bSet)
                nCtrl |= EE_CNTRL_ONLINESPELLING|EE_CNTRL_ALLOWBIGOBJS;
            else
                nCtrl &= ~EE_CNTRL_ONLINESPELLING;
            pOutliner->SetControlWord(nCtrl);

            rView.ExecuteSlot(rReq);
        }
        break;
        case SID_HYPERLINK_SETLINK:
        {
            const SfxPoolItem* pItem = 0;
            if(pNewAttrs)
                pNewAttrs->GetItemState(nSlot, sal_False, &pItem);

            if(pItem)
            {
                const SvxHyperlinkItem& rHLinkItem = *(const SvxHyperlinkItem *)pItem;
                SvxURLField aFld(rHLinkItem.GetURL(), rHLinkItem.GetName(), SVXURLFORMAT_APPDEFAULT);
                aFld.SetTargetFrame(rHLinkItem.GetTargetFrame());

                const SvxFieldItem* pFieldItem = pOLV->GetFieldAtSelection();

                if (pFieldItem && pFieldItem->GetField()->ISA(SvxURLField))
                {
                    // Feld selektieren, so dass es beim Insert geloescht wird
                    ESelection aSel = pOLV->GetSelection();
                    aSel.nEndPos++;
                    pOLV->SetSelection(aSel);
                }
                pOLV->InsertField(SvxFieldItem(aFld, EE_FEATURE_FIELD));
            }
        }
        break;

        case SID_TEXTDIRECTION_LEFT_TO_RIGHT:
        case SID_TEXTDIRECTION_TOP_TO_BOTTOM:
            // Shellwechsel!
            {
                SdrObject* pTmpObj = pSdrView->GetMarkedObjectList().GetMark(0)->GetMarkedSdrObj();
                SdrPageView* pTmpPV = pSdrView->GetSdrPageView();
                SdrView* pTmpView = pSdrView;

                pSdrView->SdrEndTextEdit(sal_True);

                SfxItemSet aAttr( *aNewAttr.GetPool(),
                            SDRATTR_TEXTDIRECTION,
                            SDRATTR_TEXTDIRECTION );

                aAttr.Put( SvxWritingModeItem(
                    nSlot == SID_TEXTDIRECTION_LEFT_TO_RIGHT ?
                        text::WritingMode_LR_TB
                        : text::WritingMode_TB_RL, SDRATTR_TEXTDIRECTION ) );
                pTmpView->SetAttributes( aAttr );

                rSh.GetView().BeginTextEdit( pTmpObj, pTmpPV, &rSh.GetView().GetEditWin(), sal_False);
                rSh.GetView().AttrChangedNotify( &rSh );
            }
            return;

        case SID_ATTR_PARA_LEFT_TO_RIGHT:
        case SID_ATTR_PARA_RIGHT_TO_LEFT:
        {
            SdrObject* pTmpObj = pSdrView->GetMarkedObjectList().GetMark(0)->GetMarkedSdrObj();
            SdrPageView* pTmpPV = pSdrView->GetSdrPageView();
            SdrView* pTmpView = pSdrView;

            pSdrView->SdrEndTextEdit(sal_True);
            sal_Bool bLeftToRight = nSlot == SID_ATTR_PARA_LEFT_TO_RIGHT;

            const SfxPoolItem* pPoolItem;
            if( pNewAttrs && SFX_ITEM_SET == pNewAttrs->GetItemState( nSlot, TRUE, &pPoolItem ) )
            {
                if( !( (SfxBoolItem*)pPoolItem)->GetValue() )
                    bLeftToRight = !bLeftToRight;
            }
            SfxItemSet aAttr( *aNewAttr.GetPool(),
                        EE_PARA_JUST, EE_PARA_JUST,
                        EE_PARA_WRITINGDIR, EE_PARA_WRITINGDIR,
                        0 );

            USHORT nAdjust = SVX_ADJUST_LEFT;
            if( SFX_ITEM_ON == aEditAttr.GetItemState(EE_PARA_JUST, TRUE, &pPoolItem ) )
                nAdjust = ( (SvxAdjustItem*)pPoolItem)->GetEnumValue();

            if( bLeftToRight )
            {
                aAttr.Put( SvxFrameDirectionItem( FRMDIR_HORI_LEFT_TOP, EE_PARA_WRITINGDIR ) );
                if( nAdjust == SVX_ADJUST_RIGHT )
                    aAttr.Put( SvxAdjustItem( SVX_ADJUST_LEFT, EE_PARA_JUST ) );
            }
            else
            {
                aAttr.Put( SvxFrameDirectionItem( FRMDIR_HORI_RIGHT_TOP, EE_PARA_WRITINGDIR ) );
                if( nAdjust == SVX_ADJUST_LEFT )
                    aAttr.Put( SvxAdjustItem( SVX_ADJUST_RIGHT, EE_PARA_JUST ) );
            }
            pTmpView->SetAttributes( aAttr );
            rSh.GetView().BeginTextEdit( pTmpObj, pTmpPV, &rSh.GetView().GetEditWin(), sal_False );
            rSh.GetView().AttrChangedNotify( &rSh );
        }
        return;
        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }
    if(nEEWhich && pNewAttrs)
        aNewAttr.Put(pNewAttrs->Get(nWhich), nEEWhich);

    SetAttrToMarked(aNewAttr);

    GetView().GetViewFrame()->GetBindings().InvalidateAll(sal_False);

    if (IsTextEdit() && pOLV->GetOutliner()->IsModified())
        rSh.SetModified();

    if (bRestoreSelection)
    {
        // restore selection
        pOLV->GetEditView().SetSelection( aOldSelection );
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwDrawTextShell::GetState(SfxItemSet& rSet)
{
    if (!IsTextEdit())	// Sonst manchmal Absturz!
        return;

    OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();

    SfxItemSet aEditAttr( pOLV->GetAttribs() );
    const SfxPoolItem *pAdjust = 0, *pLSpace = 0, *pEscItem = 0;
    int eAdjust, nLSpace, nEsc;

    while(nWhich)
    {
        USHORT nSlotId = GetPool().GetSlotId( nWhich );
        BOOL bFlag = FALSE;
        switch( nSlotId )
        {
            case SID_LANGUAGE_STATUS://20412:
            {
                nSlotId = SwLangHelper::GetLanguageStatus(pOLV,rSet);;
                break;
            }
         case SID_ATTR_PARA_ADJUST_LEFT: 	eAdjust = SVX_ADJUST_LEFT; goto ASK_ADJUST;
        case SID_ATTR_PARA_ADJUST_RIGHT:	eAdjust = SVX_ADJUST_RIGHT; goto ASK_ADJUST;
        case SID_ATTR_PARA_ADJUST_CENTER:	eAdjust = SVX_ADJUST_CENTER; goto ASK_ADJUST;
        case SID_ATTR_PARA_ADJUST_BLOCK:	eAdjust = SVX_ADJUST_BLOCK; goto ASK_ADJUST;
ASK_ADJUST:
            {
                if( !pAdjust )
                    aEditAttr.GetItemState( EE_PARA_JUST, sal_False, &pAdjust);

                if( !pAdjust || IsInvalidItem( pAdjust ))
                    rSet.InvalidateItem( nSlotId ), nSlotId = 0;
                else
                    bFlag = eAdjust == ((SvxAdjustItem*)pAdjust)->GetAdjust();
            }
            break;

        case SID_ATTR_PARA_LINESPACE_10:	nLSpace = 100;	goto ASK_LINESPACE;
        case SID_ATTR_PARA_LINESPACE_15:	nLSpace = 150;	goto ASK_LINESPACE;
        case SID_ATTR_PARA_LINESPACE_20:	nLSpace = 200;	goto ASK_LINESPACE;
ASK_LINESPACE:
            {
                if( !pLSpace )
                    aEditAttr.GetItemState( EE_PARA_SBL, sal_False, &pLSpace );

                if( !pLSpace || IsInvalidItem( pLSpace ))
                    rSet.InvalidateItem( nSlotId ), nSlotId = 0;
                else if( nLSpace == ((const SvxLineSpacingItem*)pLSpace)->
                                                GetPropLineSpace() )
                    bFlag = sal_True;
                else
                    nSlotId = 0;
            }
            break;

        case FN_SET_SUPER_SCRIPT:	nEsc = SVX_ESCAPEMENT_SUPERSCRIPT;
                                    goto ASK_ESCAPE;
        case FN_SET_SUB_SCRIPT:		nEsc = SVX_ESCAPEMENT_SUBSCRIPT;
                                    goto ASK_ESCAPE;
ASK_ESCAPE:
            {
                if( !pEscItem )
                    pEscItem = &aEditAttr.Get( EE_CHAR_ESCAPEMENT );

                if( nEsc == ((const SvxEscapementItem*)
                                                pEscItem)->GetEnumValue() )
                    bFlag = sal_True;
                else
                    nSlotId = 0;
            }
            break;

        case FN_THESAURUS_DLG:
        {
            // disable "Thesaurus" if the language is not supported
            const SfxPoolItem &rItem = GetShell().GetDoc()->GetDefault(
                            GetWhichOfScript( RES_CHRATR_LANGUAGE,
                            GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage())) );
            LanguageType nLang = ((const SvxLanguageItem &)
                                                    rItem).GetLanguage();
            //
            uno::Reference< linguistic2::XThesaurus >  xThes( ::GetThesaurus() );
            if (!xThes.is() || nLang == LANGUAGE_NONE ||
                !xThes->hasLocale( SvxCreateLocale( nLang ) ))
                rSet.DisableItem( FN_THESAURUS_DLG );
            nSlotId = 0;
        }
        break;
        case SID_HANGUL_HANJA_CONVERSION:
        case SID_CHINESE_CONVERSION:
        {
            if (!SvtCJKOptions().IsAnyEnabled())
            {
                GetView().GetViewFrame()->GetBindings().SetVisibleState( nWhich, sal_False );
                rSet.DisableItem(nWhich);
            }
            else
                GetView().GetViewFrame()->GetBindings().SetVisibleState( nWhich, sal_True );
        }
        break;

        case SID_TEXTDIRECTION_LEFT_TO_RIGHT:
        case SID_TEXTDIRECTION_TOP_TO_BOTTOM:
            if ( !SvtLanguageOptions().IsVerticalTextEnabled() )
            {
                rSet.DisableItem( nSlotId );
                nSlotId = 0;
            }
            else
            {
                SdrOutliner * pOutliner = pSdrView->GetTextEditOutliner();
                if( pOutliner )
                    bFlag = pOutliner->IsVertical() ==
                            (SID_TEXTDIRECTION_TOP_TO_BOTTOM == nSlotId);
                else
                {
                    text::WritingMode eMode = (text::WritingMode)
                                    ( (const SvxWritingModeItem&) aEditAttr.Get( SDRATTR_TEXTDIRECTION ) ).GetValue();

                    if( nSlotId == SID_TEXTDIRECTION_LEFT_TO_RIGHT )
                    {
                        bFlag = eMode == text::WritingMode_LR_TB;
                    }
                    else
                    {
                        bFlag = eMode != text::WritingMode_TB_RL;
                    }
                }
            }
            break;
        case SID_ATTR_PARA_LEFT_TO_RIGHT:
        case SID_ATTR_PARA_RIGHT_TO_LEFT:
        {
            if ( !SvtLanguageOptions().IsCTLFontEnabled() )
            {
                rSet.DisableItem( nWhich );
                nSlotId = 0;
            }
            else
            {
                SdrOutliner * pOutliner = pSdrView->GetTextEditOutliner();
                if(pOutliner && pOutliner->IsVertical())
                {
                    rSet.DisableItem( nWhich );
                    nSlotId = 0;
                }
                else
                {
                    switch( ( ( (SvxFrameDirectionItem&) aEditAttr.Get( EE_PARA_WRITINGDIR ) ) ).GetValue() )
                    {
                        case FRMDIR_HORI_LEFT_TOP:
                            bFlag = nWhich == SID_ATTR_PARA_LEFT_TO_RIGHT;
                        break;

                        case FRMDIR_HORI_RIGHT_TOP:
                            bFlag = nWhich != SID_ATTR_PARA_LEFT_TO_RIGHT;
                        break;
                    }
                }
            }
        }
        break;
        case SID_TRANSLITERATE_HALFWIDTH:
        case SID_TRANSLITERATE_FULLWIDTH:
        case SID_TRANSLITERATE_HIRAGANA:
        case SID_TRANSLITERATE_KATAGANA:
        {
            SvtCJKOptions aCJKOptions;
            if(!aCJKOptions.IsChangeCaseMapEnabled())
            {
                rSet.DisableItem(nWhich);
                GetView().GetViewFrame()->GetBindings().SetVisibleState( nWhich, sal_False );
            }
            else
                GetView().GetViewFrame()->GetBindings().SetVisibleState( nWhich, sal_True );
        }
        break;
        case SID_INSERT_RLM :
        case SID_INSERT_LRM :
        case SID_INSERT_ZWNBSP :
        case SID_INSERT_ZWSP:
        {
            SvtCTLOptions aCTLOptions;
            sal_Bool bEnabled = aCTLOptions.IsCTLFontEnabled();
            GetView().GetViewFrame()->GetBindings().SetVisibleState( nWhich, bEnabled );
            if(!bEnabled)        
                rSet.DisableItem(nWhich);        
        }	
        break;
        default:
            nSlotId = 0;			    // don't know this slot
            break;
        }

        if( nSlotId )
            rSet.Put( SfxBoolItem( nWhich, bFlag ));

        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void SwDrawTextShell::GetDrawTxtCtrlState(SfxItemSet& rSet)
{
    if (!IsTextEdit())	// Sonst Absturz!
        return;

    OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
    SfxItemSet aEditAttr(pOLV->GetAttribs());

    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();
    USHORT nScriptType = pOLV->GetSelectedScriptType();
    while(nWhich)
    {
        sal_uInt16 nEEWhich = 0;
        USHORT nSlotId = GetPool().GetSlotId( nWhich );
        switch( nSlotId )
        {
            case SID_ATTR_CHAR_FONT:
            case SID_ATTR_CHAR_FONTHEIGHT:
            case SID_ATTR_CHAR_WEIGHT:
            case SID_ATTR_CHAR_POSTURE:
            {
                SfxItemPool* pEditPool = aEditAttr.GetPool()->GetSecondaryPool();
                if( !pEditPool )
                    pEditPool = aEditAttr.GetPool();
                SvxScriptSetItem aSetItem( nSlotId, *pEditPool );
                aSetItem.GetItemSet().Put( aEditAttr, FALSE );
                const SfxPoolItem* pI = aSetItem.GetItemOfScript( nScriptType );
                if( pI )
                    rSet.Put( *pI, nWhich );
                else
                    rSet.InvalidateItem( nWhich );
            }
            break;
            case SID_ATTR_CHAR_COLOR: nEEWhich = EE_CHAR_COLOR; break;
            case SID_ATTR_CHAR_UNDERLINE: nEEWhich = EE_CHAR_UNDERLINE;break;
            case SID_ATTR_CHAR_CONTOUR: nEEWhich = EE_CHAR_OUTLINE; break;
            case SID_ATTR_CHAR_SHADOWED:  nEEWhich = EE_CHAR_SHADOW;break;
            case SID_ATTR_CHAR_STRIKEOUT: nEEWhich = EE_CHAR_STRIKEOUT;break;
            case SID_AUTOSPELL_CHECK:
            {
                const SfxPoolItem* pState = rView.GetSlotState(nWhich);
                if (pState)
                    rSet.Put(SfxBoolItem(nWhich, ((const SfxBoolItem*)pState)->GetValue()));
                else
                    rSet.DisableItem( nWhich );
                break;
            }
            case SID_ATTR_CHAR_WORDLINEMODE: nEEWhich = EE_CHAR_WLM; break;
            case SID_ATTR_CHAR_RELIEF      : nEEWhich = EE_CHAR_RELIEF;  break;
            case SID_ATTR_CHAR_LANGUAGE    : nEEWhich = EE_CHAR_LANGUAGE;break;
            case SID_ATTR_CHAR_KERNING     : nEEWhich = EE_CHAR_KERNING; break;
            case SID_ATTR_CHAR_SCALEWIDTH:   nEEWhich = EE_CHAR_FONTWIDTH;break;
            case SID_ATTR_CHAR_AUTOKERN  :   nEEWhich = EE_CHAR_PAIRKERNING; break;
            case SID_ATTR_CHAR_ESCAPEMENT:   nEEWhich = EE_CHAR_ESCAPEMENT; break;
        }
        if(nEEWhich)
            rSet.Put(aEditAttr.Get(nEEWhich, sal_True), nWhich);

        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void SwDrawTextShell::ExecClpbrd(SfxRequest &rReq)
{
    if (!IsTextEdit())	// Sonst Absturz!
        return;

    OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
    sal_uInt16 nId = rReq.GetSlot();
    switch( nId )
    {
        case SID_CUT:
            pOLV->Cut();
            return;

        case SID_COPY:
            pOLV->Copy();
            return;

        case SID_PASTE:
            pOLV->PasteSpecial();
            break;

        default:
            DBG_ERROR("falscher Dispatcher");
            return;
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	ClipBoard-Status
 --------------------------------------------------------------------*/
void SwDrawTextShell::StateClpbrd(SfxItemSet &rSet)
{
    if (!IsTextEdit())	// Sonst Absturz!
        return;

    OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
    ESelection aSel(pOLV->GetSelection());
    const sal_Bool bCopy = (aSel.nStartPara != aSel.nEndPara) ||
                           (aSel.nStartPos != aSel.nEndPos);


    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();

    while(nWhich)
    {
        switch(nWhich)
        {
            case SID_CUT:
            case SID_COPY:
                if( !bCopy )
                    rSet.DisableItem( nWhich );
                break;

            case SID_PASTE:
                {
                    TransferableDataHelper aDataHelper(
                        TransferableDataHelper::CreateFromSystemClipboard(
                                &GetView().GetEditWin() ) );

                    if( !aDataHelper.GetXTransferable().is() ||
                        !SwTransferable::IsPaste( GetShell(), aDataHelper ))
                        rSet.DisableItem( SID_PASTE );
                }
                break;

            case FN_PASTESPECIAL:
                rSet.DisableItem( FN_PASTESPECIAL );
                break;
            // --> OD 2008-06-20 #151110#
            case SID_CLIPBOARD_FORMAT_ITEMS:
                rSet.DisableItem( SID_CLIPBOARD_FORMAT_ITEMS );
                break;
            // <--
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Hyperlink-Status
 --------------------------------------------------------------------*/

void SwDrawTextShell::StateInsert(SfxItemSet &rSet)
{
    if (!IsTextEdit())	// Sonst Absturz!
        return;

    OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();

    while(nWhich)
    {
        switch(nWhich)
        {
            case SID_HYPERLINK_GETLINK:
                {
                    SvxHyperlinkItem aHLinkItem;
                    aHLinkItem.SetInsertMode(HLINK_FIELD);

                    const SvxFieldItem* pFieldItem = pOLV->GetFieldAtSelection();

                    if (pFieldItem)
                    {
                        const SvxFieldData* pField = pFieldItem->GetField();

                        if (pField->ISA(SvxURLField))
                        {
                            aHLinkItem.SetName(((const SvxURLField*) pField)->GetRepresentation());
                            aHLinkItem.SetURL(((const SvxURLField*) pField)->GetURL());
                            aHLinkItem.SetTargetFrame(((const SvxURLField*) pField)->GetTargetFrame());
                        }
                    }
                    else
                    {
                        String sSel(pOLV->GetSelected());
                        sSel.Erase(255);
                        sSel.EraseTrailingChars();
                        aHLinkItem.SetName(sSel);
                    }

                    sal_uInt16 nHtmlMode = ::GetHtmlMode(GetView().GetDocShell());
                    aHLinkItem.SetInsertMode((SvxLinkInsertMode)(aHLinkItem.GetInsertMode() |
                        ((nHtmlMode & HTMLMODE_ON) != 0 ? HLINK_HTMLMODE : 0)));

                    rSet.Put(aHLinkItem);
                }
                break;
        }
        nWhich = aIter.NextWhich();
    }
}

