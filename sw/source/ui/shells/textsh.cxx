/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: textsh.cxx,v $
 * $Revision: 1.63 $
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


#define _SW_FRMVALID_HXX
#include <hintids.hxx>

#include <svtools/globalnameitem.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/lnkbase.hxx>

#ifndef __RSC //autogen
#include <tools/errinf.hxx>
#endif
#include <svx/svdview.hxx>
#include <svtools/ptitem.hxx>
#include <svtools/stritem.hxx>
#include <svtools/moduleoptions.hxx>
#include <vcl/msgbox.hxx>
#include <sfx2/fcontnr.hxx>
#include <svx/hlnkitem.hxx>
#include <svx/srchitem.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/docfile.hxx>
#include <svtools/urihelper.hxx>
#include <basic/sbxvar.hxx>
#include <svtools/whiter.hxx>
#include <sfx2/request.hxx>
#include <svx/opaqitem.hxx>
#include <svx/fontitem.hxx>
#include <svx/adjitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/sizeitem.hxx>
#include <svx/svxacorr.hxx>
#include <svx/scripttypeitem.hxx>
#include <svtools/filter.hxx>
#include <svx/htmlmode.hxx>
#include <svx/pfiledlg.hxx>
#include <svx/htmlcfg.hxx>
#include <com/sun/star/i18n/TransliterationModules.hdl>

#include <sot/clsids.hxx>
#include <svx/acorrcfg.hxx>
#include <wdocsh.hxx>
#include <fmtinfmt.hxx>
#include <fmtclds.hxx>
#include <fmtsrnd.hxx>
#include <fmtfsize.hxx>
#include <swmodule.hxx>
#include <wrtsh.hxx>
#include <view.hxx>
#include <docsh.hxx>
#include <doc.hxx>
#include <uitool.hxx>
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#include <frmmgr.hxx>
#include <textsh.hxx>
#include <frmfmt.hxx>
#include <tablemgr.hxx>
#include <swundo.hxx>		// fuer Undo-IDs
#include <shellio.hxx>
#include <frmdlg.hxx>
#include <usrpref.hxx>
#include <swtable.hxx>
#include <tblafmt.hxx>
#include <caption.hxx>
#include <idxmrk.hxx>
#include <poolfmt.hxx>
#include <breakit.hxx>
#include <crsskip.hxx>
#include <modcfg.hxx>
#include <column.hxx>
#include <edtwin.hxx>
#ifndef _SHELLS_HRC
#include <shells.hrc>
#endif
#ifndef _POPUP_HRC
#include <popup.hrc>
#endif
#ifndef _SWERROR_H
#include <swerror.h>
#endif
#include <SwAppletImpl.hxx>
#include <unochart.hxx>

#include <chartins.hxx>

#define SwTextShell
#define Paragraph
#define HyphenZone
#define TextFont
#define DropCap
#include <sfx2/msg.hxx>
#include <swslots.hxx>
#include <SwRewriter.hxx>
#include <undobj.hxx>
#ifndef _COMCORE_HRC
#include <comcore.hrc>
#endif

using namespace ::com::sun::star;
using ::rtl::OUString;

#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
#include "swabstdlg.hxx"
#include <misc.hrc>
#include <table.hrc>
#include <frmui.hrc>
#include <unomid.h>



/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SFX_IMPL_INTERFACE(SwTextShell, SwBaseShell, SW_RES(STR_SHELLNAME_TEXT))
{
    SFX_POPUPMENU_REGISTRATION(SW_RES(MN_TEXT_POPUPMENU));
    SFX_OBJECTBAR_REGISTRATION(SFX_OBJECTBAR_OBJECT, SW_RES(RID_TEXT_TOOLBOX));
    SFX_CHILDWINDOW_REGISTRATION(FN_EDIT_FORMULA);
    SFX_CHILDWINDOW_REGISTRATION(FN_INSERT_FIELD);
    SFX_CHILDWINDOW_REGISTRATION(FN_INSERT_IDX_ENTRY_DLG);
    SFX_CHILDWINDOW_REGISTRATION(FN_INSERT_AUTH_ENTRY_DLG);
    SFX_CHILDWINDOW_REGISTRATION(SID_RUBY_DIALOG);
}



TYPEINIT1(SwTextShell,SwBaseShell)



void SwTextShell::ExecInsert(SfxRequest &rReq)
{
    SwWrtShell &rSh = GetShell();

    ASSERT( !rSh.IsObjSelected() && !rSh.IsFrmSelected(),
            "Falsche Shell auf dem Dispatcher" );

    const SfxItemSet *pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem = 0;
    USHORT nSlot = rReq.GetSlot();
    if(pArgs)
        pArgs->GetItemState(nSlot, FALSE, &pItem );

    switch( nSlot )
    {
    case FN_INSERT_STRING:
        if( pItem )
            rSh.InsertByWord(((const SfxStringItem *)pItem)->GetValue());
    break;
    case FN_INSERT_SOFT_HYPHEN:
        if( CHAR_SOFTHYPHEN != rSh.SwCrsrShell::GetChar( TRUE, 0 ) &&
            CHAR_SOFTHYPHEN != rSh.SwCrsrShell::GetChar( TRUE, -1 ))
            rSh.Insert( String( CHAR_SOFTHYPHEN ) );
        break;

    case FN_INSERT_HARDHYPHEN:
    case FN_INSERT_HARD_SPACE:
        {
            sal_Unicode cIns = FN_INSERT_HARD_SPACE == nSlot ? CHAR_HARDBLANK
                                                             : CHAR_HARDHYPHEN;

            SvxAutoCorrCfg* pACfg = SvxAutoCorrCfg::Get();
            SvxAutoCorrect* pACorr = pACfg->GetAutoCorrect();
            if( pACorr && pACfg->IsAutoFmtByInput() &&
                    pACorr->IsAutoCorrFlag( CptlSttSntnc | CptlSttWrd |
                                ChgFractionSymbol | ChgOrdinalNumber |
                                ChgToEnEmDash | SetINetAttr | Autocorrect ))
                rSh.AutoCorrect( *pACorr, cIns );
            else
                rSh.Insert( String( cIns ) );
        }
        break;
    case SID_INSERT_RLM :
    case SID_INSERT_LRM :
    case SID_INSERT_ZWNBSP :
    case SID_INSERT_ZWSP:
    {
        sal_Unicode cIns = 0;
        switch(nSlot)
        {
            case SID_INSERT_RLM : cIns = CHAR_RLM ; break;
            case SID_INSERT_LRM : cIns = CHAR_LRM ; break;
            case SID_INSERT_ZWSP : cIns = CHAR_ZWSP ; break;
            case SID_INSERT_ZWNBSP: cIns = CHAR_ZWNBSP; break;
        }
        rSh.Insert( String( cIns ) );
    }
    break;
    case FN_INSERT_BREAK:
        rSh.SplitNode();
        rReq.Done();
    break;
    case FN_INSERT_PAGEBREAK:
        rSh.InsertPageBreak();
        rReq.Done();
    break;
    case FN_INSERT_LINEBREAK:
        rSh.InsertLineBreak();
        rReq.Done();
    break;
    case FN_INSERT_COLUMN_BREAK:
        rSh.InsertColumnBreak();
        rReq.Done();
    break;
    case SID_HYPERLINK_SETLINK:
        if (pItem)
            InsertHyperlink(*((const SvxHyperlinkItem *)pItem));
        rReq.Done();
        break;
    case SID_INSERT_AVMEDIA:
        rReq.SetReturnValue(SfxBoolItem(nSlot, InsertMediaDlg( rReq )));
        break;
    case  SID_INSERT_SOUND:
    case  SID_INSERT_VIDEO:
    {
        SvxPluginFileDlg aDlg( &GetView().GetViewFrame()->GetWindow(), nSlot );
        aDlg.SetContext( nSlot == SID_INSERT_SOUND? sfx2::FileDialogHelper::SW_INSERT_SOUND : sfx2::FileDialogHelper::SW_INSERT_VIDEO );

        if ( ERRCODE_NONE == aDlg.Execute() )
        {
            // URL ermitteln
            String aStrURL( aDlg.GetPath() );
            aStrURL = URIHelper::SmartRel2Abs(
                INetURLObject(), aStrURL, URIHelper::GetMaybeFileHdl() );

            INetURLObject* pURL = new INetURLObject();
            pURL->SetSmartProtocol( INET_PROT_FILE );

            if ( pURL->SetURL( aStrURL, INetURLObject::WAS_ENCODED ) )
            {
                ::rtl::OUString aName;
                comphelper::EmbeddedObjectContainer aCnt;
                svt::EmbeddedObjectRef xObj( aCnt.CreateEmbeddedObject( SvGlobalName( SO3_PLUGIN_CLASSID ).GetByteSequence(), aName ), embed::Aspects::MSOLE_CONTENT );
                if ( xObj.is() )
                {
                    svt::EmbeddedObjectRef::TryRunningState( xObj.GetObject() );

                    // set properties from dialog
                    uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
                    if ( xSet.is() )
                    {
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginURL"),
                                uno::makeAny( ::rtl::OUString( pURL->GetMainURL( INetURLObject::NO_DECODE ) ) ) );
                    }
                }

                rSh.InsertObject( xObj, 0, TRUE, nSlot);
            }
        }
    }
    break;
    case SID_INSERT_OBJECT:
    case SID_INSERT_PLUGIN:
    case SID_INSERT_APPLET:
    {
        SFX_REQUEST_ARG( rReq, pNameItem, SfxGlobalNameItem, SID_INSERT_OBJECT, sal_False );
        SvGlobalName *pName = NULL;
        SvGlobalName aName;
        if ( pNameItem )
        {
            aName = pNameItem->GetValue();
            pName = &aName;
        }

        SFX_REQUEST_ARG( rReq, pClassItem,          SfxStringItem, FN_PARAM_1, sal_False );
        SFX_REQUEST_ARG( rReq, pClassLocationItem,  SfxStringItem, FN_PARAM_2, sal_False );
        SFX_REQUEST_ARG( rReq, pCommandsItem,       SfxStringItem, FN_PARAM_3, sal_False );
        //TODO/LATER: recording currently not working, need code for Commandlist
        svt::EmbeddedObjectRef xObj;
        if((SID_INSERT_APPLET == nSlot || SID_INSERT_PLUGIN)
                && (pClassItem || pClassLocationItem || pCommandsItem))
        {
            String sClass;
            String sClassLocation;
            if(pClassItem)
                sClass = pClassItem->GetValue();
            if(pClassLocationItem)
                sClassLocation = pClassLocationItem->GetValue();

            SvCommandList aCommandList;
            if(pCommandsItem)
            {
                USHORT nTemp;
                aCommandList.AppendCommands( pCommandsItem->GetValue(), &nTemp );
            }

            if(SID_INSERT_APPLET == nSlot)
            {
                SwApplet_Impl aApplImpl( rSh.GetAttrPool(),
                                         RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
                String sBaseURL;
                SfxMedium* pMedium = GetView().GetDocShell()->GetMedium();
                if(pMedium)
                    sBaseURL = pMedium->GetURLObject().GetMainURL(INetURLObject::NO_DECODE);

                aApplImpl.CreateApplet(sClass, aEmptyStr, FALSE, sClassLocation, sBaseURL );
                aApplImpl.FinishApplet();
                xObj.Assign( aApplImpl.GetApplet(), embed::Aspects::MSOLE_CONTENT );
                if( aCommandList.Count() )
                {
                    uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
                    if ( xSet.is() )
                    {
                        uno::Sequence < beans::PropertyValue > aSeq;
                        aCommandList.FillSequence( aSeq );
                        try
                        {
                            xSet->setPropertyValue( ::rtl::OUString::createFromAscii("AppletCommands"), uno::makeAny( aSeq ) );
                        }
                        catch ( uno::Exception& )
                        {
                        }
                    }
                }
            }
            else
            {
                comphelper::EmbeddedObjectContainer aCnt;
                ::rtl::OUString sName;
                xObj.Assign( aCnt.CreateEmbeddedObject( SvGlobalName( SO3_PLUGIN_CLASSID ).GetByteSequence(), sName ),
                            embed::Aspects::MSOLE_CONTENT );
                svt::EmbeddedObjectRef::TryRunningState( xObj.GetObject() );
                uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
                if ( xSet.is() )
                {
                    try
                    {
                        if ( sClassLocation.Len() )
                            xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginURL"),
                                uno::makeAny(
                                    ::rtl::OUString(
                                        URIHelper::SmartRel2Abs(
                                            INetURLObject(), sClassLocation,
                                            URIHelper::GetMaybeFileHdl()) ) ) );
                        uno::Sequence< beans::PropertyValue > aSeq;
                        if ( aCommandList.Count() )
                        {
                            aCommandList.FillSequence( aSeq );
                            xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginCommands"), uno::makeAny( aSeq ) );
                        }
                    }
                    catch ( uno::Exception& )
                    {
                    }
                }
            }

            if(xObj.is())
                rSh.InsertOleObject( xObj );
        }
        else
        {
            DBG_ASSERT( !pNameItem || nSlot == SID_INSERT_OBJECT, "Superfluous argument!" );
            rSh.InsertObject( xObj, pName, TRUE, nSlot);
            rReq.Done();
        }
        break;
    }
    case SID_INSERT_FLOATINGFRAME:
    {
        svt::EmbeddedObjectRef xObj;
        SFX_REQUEST_ARG( rReq, pNameItem,   SfxStringItem, FN_PARAM_1, sal_False );
        SFX_REQUEST_ARG( rReq, pURLItem,    SfxStringItem, FN_PARAM_2, sal_False );
        SFX_REQUEST_ARG( rReq, pMarginItem, SvxSizeItem, FN_PARAM_3, sal_False );
        SFX_REQUEST_ARG( rReq, pScrollingItem, SfxByteItem, FN_PARAM_4, sal_False );
        SFX_REQUEST_ARG( rReq, pBorderItem, SfxBoolItem, FN_PARAM_5, sal_False );

        if(pURLItem) // URL is a _must_
        {
            comphelper::EmbeddedObjectContainer aCnt;
            ::rtl::OUString aName;
            xObj.Assign( aCnt.CreateEmbeddedObject( SvGlobalName( SO3_IFRAME_CLASSID ).GetByteSequence(), aName ),
                        embed::Aspects::MSOLE_CONTENT );
            svt::EmbeddedObjectRef::TryRunningState( xObj.GetObject() );
            uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
            if ( xSet.is() )
            {
                try
                {
                    ScrollingMode eScroll = ScrollingAuto;
                    if( pScrollingItem && pScrollingItem->GetValue() <= ScrollingAuto )
                        eScroll = (ScrollingMode) pScrollingItem->GetValue();

                    Size aMargin;
                    if ( pMarginItem )
                        aMargin = pMarginItem->GetSize();

                    if ( pURLItem )
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameURL"), uno::makeAny( ::rtl::OUString( pURLItem->GetValue() ) ) );
                    if ( pNameItem )
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameName"), uno::makeAny( ::rtl::OUString( pNameItem->GetValue() ) ) );

                    if ( eScroll == ScrollingAuto )
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsAutoScroll"),
                            uno::makeAny( sal_True ) );
                    else
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsScrollingMode"),
                            uno::makeAny( (sal_Bool) ( eScroll == ScrollingYes) ) );

                    //if ( aFrmDescr.IsFrameBorderSet() )
                    if ( pBorderItem )
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsBorder"),
                            uno::makeAny( (sal_Bool) pBorderItem->GetValue() ) );
                    /*else
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsAutoBorder"),
                            makeAny( sal_True ) );*/

                    if ( pMarginItem )
                    {
                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameMarginWidth"),
                            uno::makeAny( sal_Int32( aMargin.Width() ) ) );

                        xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameMarginHeight"),
                            uno::makeAny( sal_Int32( aMargin.Height() ) ) );
                    }
                }
                catch ( uno::Exception& )
                {
                }
            }

            rSh.InsertOleObject( xObj );
        }
        else
        {
            rSh.InsertObject( xObj, 0, TRUE, nSlot);
            rReq.Done();
        }
    }
    break;
    case SID_INSERT_DIAGRAM:
        {
            SvtModuleOptions aMOpt;
            if ( !aMOpt.IsChart() )
                break;
            if(!rReq.IsAPI())
            {
                SfxViewFrame* pVFrame = GetView().GetViewFrame();
                SwInsertChart( &GetView().GetEditWin(), &pVFrame->GetBindings() );
            }
            else
            {
                uno::Reference< chart2::data::XDataProvider > xDataProvider;
                sal_Bool bFillWithData = sal_True;
                OUString aRangeString;
                if (!GetShell().IsTblComplexForChart())
                {
                    SwFrmFmt* pTblFmt = GetShell().GetTableFmt();
                    String aCurrentTblName = pTblFmt->GetName();
        //             String aText( String::CreateFromAscii("<.>") );   // was used for UI
        //             aText.Insert( rWrtShell.GetBoxNms(), 2);
        //             aText.Insert( aCurrentTblName, 1 );
                    aRangeString = aCurrentTblName;
                    aRangeString += OUString::valueOf( sal_Unicode('.') );
                    aRangeString += GetShell().GetBoxNms();

                    // get table data provider
                    xDataProvider.set( GetView().GetDocShell()->getIDocumentChartDataProviderAccess()->GetChartDataProvider() );
                }
                else
                    bFillWithData = sal_False;  // will create chart with only it's default image

                SwTableFUNC( &rSh, FALSE ).InsertChart( xDataProvider, bFillWithData, aRangeString );
                rSh.LaunchOLEObj();

                svt::EmbeddedObjectRef& xObj = rSh.GetOLEObject();
                if(pItem && xObj.is())
                {
                    Size aSize(((SvxSizeItem*)pItem)->GetSize());
                    aSize = OutputDevice::LogicToLogic
                                    ( aSize, MapMode( MAP_TWIP ), MapMode( MAP_100TH_MM ) );

                    if(aSize.Width() > MINLAY&& aSize.Height()> MINLAY)
                    {
                        awt::Size aSz;
                        aSz.Width = aSize.Width();
                        aSz.Height = aSize.Height();
                        xObj->setVisualAreaSize( xObj.GetViewAspect(), aSz );
                    }
                }
            }
        }
        break;

    case FN_INSERT_SMA:
        {
            // #i34343# Inserting a math object into an autocompletion crashes
            // the suggestion has to be removed before
            GetView().GetEditWin().StopQuickHelp();
            SvGlobalName aGlobalName( SO3_SM_CLASSID );
            rSh.InsertObject( svt::EmbeddedObjectRef(), &aGlobalName, TRUE, 0 );
        }
        break;

    case FN_INSERT_TABLE:
        InsertTable( rReq );
        break;

    case FN_INSERT_FRAME_INTERACT_NOCOL:
    case FN_INSERT_FRAME_INTERACT:
    {
        USHORT nCols = 1;
        BOOL bModifier1 = rReq.GetModifier() == KEY_MOD1;
        if(pArgs)
        {
            if(FN_INSERT_FRAME_INTERACT_NOCOL != nSlot &&
                pArgs->GetItemState(SID_ATTR_COLUMNS, FALSE, &pItem) == SFX_ITEM_SET)
                nCols = ((SfxUInt16Item *)pItem)->GetValue();
            if(pArgs->GetItemState(SID_MODIFIER, FALSE, &pItem) == SFX_ITEM_SET)
                bModifier1 |= KEY_MOD1 == ((SfxUInt16Item *)pItem)->GetValue();
        }
        if(bModifier1 )
        {
            SwEditWin& rEdtWin = GetView().GetEditWin();
            Size aWinSize = rEdtWin.GetSizePixel();
            Point aStartPos(aWinSize.Width()/2, aWinSize.Height() / 2);
            aStartPos = rEdtWin.PixelToLogic(aStartPos);
            aStartPos.X() -= 8 * MM50;
            aStartPos.Y() -= 4 * MM50;
            Size aSize(16 * MM50, 8 * MM50);
            GetShell().LockPaint();
            GetShell().StartAllAction();
            SwFlyFrmAttrMgr aMgr( TRUE, GetShellPtr(), FRMMGR_TYPE_TEXT );
            if(nCols > 1)
            {
                SwFmtCol aCol;
                aCol.Init( nCols, aCol.GetGutterWidth(), aCol.GetWishWidth() );
                aMgr.SetCol( aCol );
            }
            aMgr.InsertFlyFrm(FLY_AT_CNTNT, aStartPos, aSize);
            GetShell().EndAllAction();
            GetShell().UnlockPaint();
        }
        else
        {
            GetView().InsFrmMode(nCols);
        }
        rReq.Ignore();
    }
    break;
    case FN_INSERT_FRAME:
    {
        BOOL bSingleCol = FALSE;
        if( 0!= dynamic_cast< SwWebDocShell*>( GetView().GetDocShell()) )
        {
            SvxHtmlOptions* pHtmlOpt = SvxHtmlOptions::Get();
            USHORT nExport = pHtmlOpt->GetExportMode();
            if( HTML_CFG_MSIE == nExport ||
                HTML_CFG_HTML32 == nExport ||
                HTML_CFG_MSIE_40 == nExport ||
                HTML_CFG_HTML32 == nExport )
            {
                bSingleCol = TRUE;
            }

        }
        // Rahmen neu anlegen
        SwFlyFrmAttrMgr aMgr( TRUE, GetShellPtr(), FRMMGR_TYPE_TEXT );
        if(pArgs)
        {
            Size aSize(aMgr.GetSize());
            aSize.Width() = GetShell().GetAnyCurRect(RECT_PAGE_PRT).Width();
            Point aPos = aMgr.GetPos();
            RndStdIds eAnchor = FLY_AT_CNTNT;
            if(pArgs->GetItemState(nSlot, FALSE, &pItem) == SFX_ITEM_SET)
                eAnchor = (RndStdIds)((SfxUInt16Item *)pItem)->GetValue();
            if(pArgs->GetItemState(FN_PARAM_1, FALSE, &pItem)  == SFX_ITEM_SET)
                aPos = ((SfxPointItem *)pItem)->GetValue();
            if(pArgs->GetItemState(FN_PARAM_2, FALSE, &pItem)  == SFX_ITEM_SET)
                aSize = ((SvxSizeItem *)pItem)->GetSize();
            if(pArgs->GetItemState(SID_ATTR_COLUMNS, FALSE, &pItem)  == SFX_ITEM_SET)
            {
                USHORT nCols = ((SfxUInt16Item *)pItem)->GetValue();
                if( !bSingleCol && 1 < nCols )
                {
                    SwFmtCol aFmtCol;
                    aFmtCol.Init( nCols , (rReq.IsAPI() ? 0
                                        : DEF_GUTTER_WIDTH), USHRT_MAX );
                    aMgr.SetCol(aFmtCol);
                }
            }

            GetShell().LockPaint();
            GetShell().StartAllAction();

            aMgr.InsertFlyFrm(eAnchor, aPos, aSize);

            GetShell().EndAllAction();
            GetShell().UnlockPaint();
        }
        else
        {
            static USHORT __READONLY_DATA aFrmAttrRange[] =
            {
                RES_FRMATR_BEGIN,       RES_FRMATR_END-1,
                SID_ATTR_BORDER_INNER,  SID_ATTR_BORDER_INNER,
                FN_GET_PRINT_AREA,      FN_GET_PRINT_AREA,
                SID_ATTR_PAGE_SIZE,     SID_ATTR_PAGE_SIZE,
                FN_SET_FRM_NAME,        FN_SET_FRM_NAME,
                SID_HTML_MODE,          SID_HTML_MODE,
                0
            };

            SfxItemSet aSet(GetPool(), aFrmAttrRange );
            aSet.Put(SfxUInt16Item(SID_HTML_MODE, ::GetHtmlMode(GetView().GetDocShell())));
            const SwRect &rPg = GetShell().GetAnyCurRect(RECT_PAGE);
            SwFmtFrmSize aFrmSize(ATT_VAR_SIZE, rPg.Width(), rPg.Height());
            aFrmSize.SetWhich(GetPool().GetWhich(SID_ATTR_PAGE_SIZE));
            aSet.Put(aFrmSize);

            const SwRect &rPr = GetShell().GetAnyCurRect(RECT_PAGE_PRT);
            SwFmtFrmSize aPrtSize(ATT_VAR_SIZE, rPr.Width(), rPr.Height());
            aPrtSize.SetWhich(GetPool().GetWhich(FN_GET_PRINT_AREA));
            aSet.Put(aPrtSize);

            aSet.Put(aMgr.GetAttrSet());
            aSet.SetParent( aMgr.GetAttrSet().GetParent() );

            // Minimalgroesse in Spalten loeschen
            SvxBoxInfoItem aBoxInfo((SvxBoxInfoItem &)aSet.Get(SID_ATTR_BORDER_INNER));
            const SvxBoxItem& rBox = (const SvxBoxItem&)aSet.Get(RES_BOX);
            aBoxInfo.SetMinDist(FALSE);
            aBoxInfo.SetDefDist(rBox.GetDistance(BOX_LINE_LEFT));
            aSet.Put(aBoxInfo);

            FieldUnit eMetric = ::GetDfltMetric(0 != PTR_CAST(SwWebDocShell, GetView().GetDocShell()));
            SW_MOD()->PutItem(SfxUInt16Item(SID_ATTR_METRIC, static_cast< UINT16 >(eMetric)));
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "Dialogdiet fail!");
            SfxAbstractTabDialog* pDlg = pFact->CreateFrmTabDialog( DLG_FRM_STD,
                                                    GetView().GetViewFrame(), &GetView().GetViewFrame()->GetWindow(), aSet, TRUE);
            DBG_ASSERT(pDlg, "Dialogdiet fail!");
            if(pDlg->Execute() && pDlg->GetOutputItemSet())
            {
                GetShell().LockPaint();
                GetShell().StartAllAction();
                GetShell().StartUndo(UNDO_INSERT);

                const SfxItemSet* pOutSet = pDlg->GetOutputItemSet();
                aMgr.SetAttrSet(*pOutSet);

                // beim ClickToEditFeld erst die Selektion loeschen
                if( GetShell().IsInClickToEdit() )
                    GetShell().DelRight();

                aMgr.InsertFlyFrm();

                uno::Reference< frame::XDispatchRecorder > xRecorder =
                        GetView().GetViewFrame()->GetBindings().GetRecorder();
                if ( xRecorder.is() )
                {
                    //FN_INSERT_FRAME
                    USHORT nAnchor = (USHORT)aMgr.GetAnchor();
                        rReq.AppendItem(SfxUInt16Item(nSlot, nAnchor));
                        rReq.AppendItem(SfxPointItem(FN_PARAM_1, GetShell().GetObjAbsPos()));
                        rReq.AppendItem(SvxSizeItem(FN_PARAM_2, GetShell().GetObjSize()));
                    rReq.Done();
                }

                GetView().AutoCaption(FRAME_CAP);

                {
                    SwRewriter aRewriter;

                    aRewriter.AddRule(UNDO_ARG1, SW_RES(STR_FRAME));

                    GetShell().EndUndo(UNDO_INSERT, &aRewriter);
                }
                GetShell().EndAllAction();
                GetShell().UnlockPaint();
            }

            DELETEZ(pDlg);
        }
        break;
    }
    case FN_INSERT_HRULER:
    {
        String sPath;
        BOOL bSimpleLine = FALSE;
        BOOL bRet = FALSE;
        Window* pParent = GetView().GetWindow();
        if ( pItem )
        {
            sPath = ((SfxStringItem*)pItem)->GetValue();
            SFX_REQUEST_ARG( rReq, pSimple, SfxBoolItem, FN_PARAM_1 , sal_False );
            if ( pSimple )
                bSimpleLine = pSimple->GetValue();
        }
        else
        {
            SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
            DBG_ASSERT(pFact, "Dialogdiet fail!");
            AbstractInsertGrfRulerDlg* pDlg = pFact->CreateInsertGrfRulerDlg( DLG_INSERT_RULER,
                                                        pParent );
            DBG_ASSERT(pDlg, "Dialogdiet fail!");
            // MessageBox fuer fehlende Grafiken
            if(!pDlg->HasImages())
                InfoBox( pParent, SW_RES(MSG_NO_RULER)).Execute();
            if(RET_OK == pDlg->Execute())
            {
                sPath = pDlg->GetGraphicName();
                bSimpleLine = pDlg->IsSimpleLine();
            }

            delete pDlg;
            rReq.AppendItem( SfxStringItem( FN_INSERT_HRULER, sPath ) );
            rReq.AppendItem( SfxBoolItem( FN_PARAM_1, bSimpleLine ) );
        }

        rSh.StartAllAction();
        rSh.StartUndo(UNDO_UI_INSERT_RULER);
        if(bSimpleLine)
        {
            if(!(rSh.IsSttOfPara() && rSh.IsEndOfPara())) // kein leerer Absatz?
                rSh.SplitNode( FALSE, FALSE ); // dann Platz schaffen
            rSh.SplitNode( FALSE, FALSE );
            rSh.Left(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
            rSh.SetTxtFmtColl( rSh.GetTxtCollFromPool( RES_POOLCOLL_HTML_HR ));
            rSh.Right(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
            bRet = TRUE;
        }
        else if(sPath.Len())
        {
            SwFlyFrmAttrMgr aFrmMgr( TRUE, &rSh, FRMMGR_TYPE_GRF );
            // am FrmMgr muessen die richtigen Parameter eingestellt werden

            aFrmMgr.SetAnchor(FLY_IN_CNTNT);

            rSh.SplitNode( FALSE, FALSE );
            rSh.SplitNode( FALSE, FALSE );
            rSh.Left(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
            rSh.SetAttr(SvxAdjustItem(SVX_ADJUST_CENTER,RES_PARATR_ADJUST ));
            if(GRFILTER_OK == GetView().InsertGraphic(sPath, aEmptyStr, TRUE, 0, 0 ))
                bRet = TRUE;
            rSh.EnterStdMode();
            rSh.Right(CRSR_SKIP_CHARS, FALSE, 1, FALSE );
        }
        rSh.EndAllAction();
        rSh.EndUndo(UNDO_UI_INSERT_RULER);
        rReq.SetReturnValue(SfxBoolItem(nSlot, bRet));
        rReq.Done();
    }
    break;
    case FN_FORMAT_COLUMN :
    {
        SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
        DBG_ASSERT(pFact, "Dialogdiet fail!");
        VclAbstractDialog* pColDlg = pFact->CreateVclAbstractDialog( GetView().GetWindow(), rSh, DLG_COLUMN);
        DBG_ASSERT(pColDlg, "Dialogdiet fail!");
        pColDlg->Execute();
        delete pColDlg;
    }
    break;

    default:
        ASSERT( !this, "falscher Dispatcher" );
        return;
    }
}

bool lcl_IsMarkInSameSection( SwWrtShell& rWrtSh, const SwSection* pSect )
{
    rWrtSh.SwapPam();
    bool bRet = pSect == rWrtSh.GetCurrSection();
    rWrtSh.SwapPam();
    return bRet;
}


void SwTextShell::StateInsert( SfxItemSet &rSet )
{
    USHORT nHtmlMode = ::GetHtmlMode(GetView().GetDocShell());
    SfxWhichIter aIter( rSet );
    SwWrtShell &rSh = GetShell();
    USHORT nWhich = aIter.FirstWhich();
    SvtModuleOptions aMOpt;
    SfxObjectCreateMode eCreateMode =
                        GetView().GetDocShell()->GetCreateMode();

    rSh.Push();
    const BOOL bCrsrInHidden = rSh.SelectHiddenRange();
    // --> OD 2009-08-05 #i103839#, #b6855246#
    // Do not call method <SwCrsrShell::Pop(..)> with 1st parameter = <FALSE>
    // in order to avoid that the view jumps to the visible cursor.
    rSh.Pop();
    // <--

    while ( nWhich )
    {
        switch ( nWhich )
        {
            case SID_INSERT_SOUND:
            case SID_INSERT_VIDEO:
                if ( GetShell().IsSelFrmMode() ||
                     !SvxPluginFileDlg::IsAvailable( nWhich ) ||
                     SFX_CREATE_MODE_EMBEDDED == eCreateMode || bCrsrInHidden )
                {
                    rSet.DisableItem( nWhich );
                }
                break;

            case SID_INSERT_DIAGRAM:
                if( !aMOpt.IsChart() || eCreateMode == SFX_CREATE_MODE_EMBEDDED || bCrsrInHidden )
                {
                    rSet.DisableItem( nWhich );
                }
                break;

            case FN_INSERT_SMA:
                if( !aMOpt.IsMath() || eCreateMode == SFX_CREATE_MODE_EMBEDDED || bCrsrInHidden )
                {
                    rSet.DisableItem( nWhich );
                }
                break;

            case SID_INSERT_FLOATINGFRAME:
            case SID_INSERT_OBJECT:
            case SID_INSERT_PLUGIN:
            case SID_INSERT_APPLET:
            {
                if(
#ifndef SOLAR_JAVA
                    nWhich == SID_INSERT_APPLET ||
#endif
                    eCreateMode == SFX_CREATE_MODE_EMBEDDED || bCrsrInHidden )
                {
                    rSet.DisableItem( nWhich );
                }
                else if( GetShell().IsSelFrmMode())
                    rSet.DisableItem( nWhich );
                else if(SID_INSERT_FLOATINGFRAME == nWhich && nHtmlMode&HTMLMODE_ON)
                {
                    SvxHtmlOptions* pHtmlOpt = SvxHtmlOptions::Get();
                    USHORT nExport = pHtmlOpt->GetExportMode();
                    if(HTML_CFG_MSIE_40 != nExport && HTML_CFG_WRITER != nExport )
                        rSet.DisableItem(nWhich);
                }
            }
            break;
            case FN_INSERT_FRAME_INTERACT_NOCOL :
            case FN_INSERT_FRAME_INTERACT:
            {
                if ( GetShell().IsSelFrmMode() ||
                    (0 != (nHtmlMode & HTMLMODE_ON) && 0 == (nHtmlMode & HTMLMODE_SOME_ABS_POS)) || bCrsrInHidden )
                    rSet.DisableItem(nWhich);
            }
            break;
            case SID_HYPERLINK_GETLINK:
                {
                    SfxItemSet aSet(GetPool(), RES_TXTATR_INETFMT, RES_TXTATR_INETFMT);
                    rSh.GetCurAttr( aSet );

                    SvxHyperlinkItem aHLinkItem;
                    const SfxPoolItem* pItem;
                    if(SFX_ITEM_SET == aSet.GetItemState(RES_TXTATR_INETFMT, FALSE, &pItem))
                    {
                        const SwFmtINetFmt* pINetFmt = (const SwFmtINetFmt*)pItem;
                        aHLinkItem.SetURL(pINetFmt->GetValue());
                        aHLinkItem.SetTargetFrame(pINetFmt->GetTargetFrame());
                        aHLinkItem.SetIntName(pINetFmt->GetName());
                        const SvxMacro *pMacro = pINetFmt->GetMacro( SFX_EVENT_MOUSEOVER_OBJECT );
                        if( pMacro )
                            aHLinkItem.SetMacro(HYPERDLG_EVENT_MOUSEOVER_OBJECT, *pMacro);

                        pMacro = pINetFmt->GetMacro( SFX_EVENT_MOUSECLICK_OBJECT );
                        if( pMacro )
                            aHLinkItem.SetMacro(HYPERDLG_EVENT_MOUSECLICK_OBJECT, *pMacro);

                        pMacro = pINetFmt->GetMacro( SFX_EVENT_MOUSEOUT_OBJECT );
                        if( pMacro )
                            aHLinkItem.SetMacro(HYPERDLG_EVENT_MOUSEOUT_OBJECT, *pMacro);

                        // Text des Links besorgen
                        rSh.StartAction();
                        rSh.CreateCrsr();
                        rSh.SwCrsrShell::SelectTxtAttr(RES_TXTATR_INETFMT,TRUE);
                        String sLinkName = rSh.GetSelTxt();
                        aHLinkItem.SetName(sLinkName);
                        aHLinkItem.SetInsertMode(HLINK_FIELD);
                        rSh.DestroyCrsr();
                        rSh.EndAction();
                    }
                    else
                    {
                        String sReturn = rSh.GetSelTxt();
                        sReturn.Erase(255);
                        sReturn.EraseTrailingChars();
                        aHLinkItem.SetName(sReturn);
                    }

                    aHLinkItem.SetInsertMode((SvxLinkInsertMode)(aHLinkItem.GetInsertMode() |
                        ((nHtmlMode & HTMLMODE_ON) != 0 ? HLINK_HTMLMODE : 0)));
                    aHLinkItem.SetMacroEvents (	HYPERDLG_EVENT_MOUSEOVER_OBJECT|
                                HYPERDLG_EVENT_MOUSECLICK_OBJECT | HYPERDLG_EVENT_MOUSEOUT_OBJECT );

                    rSet.Put(aHLinkItem);
                }
                break;

            case FN_INSERT_FRAME:
                if(rSh.IsSelFrmMode())
                {
                    const int nSel = rSh.GetSelectionType();
                    if( ((nsSelectionType::SEL_GRF | nsSelectionType::SEL_OLE ) & nSel ) || bCrsrInHidden )
                        rSet.DisableItem(nWhich);
                }
            break;
            case FN_INSERT_HRULER :
                if(rSh.IsReadOnlyAvailable() && rSh.HasReadonlySel() || bCrsrInHidden )
                    rSet.DisableItem(nWhich);
            break;
            case FN_FORMAT_COLUMN :
            {
                //#i80458# column dialog cannot work if the selection contains different page styles and different sections
                bool bDisable = true;
                if( rSh.GetFlyFrmFmt() || rSh.GetSelectedPageDescs() )
                    bDisable = false;
                if( bDisable )
                {
                    const SwSection* pCurrSection = rSh.GetCurrSection();
                    USHORT nFullSectCnt = rSh.GetFullSelectedSectionCount();
                    if( pCurrSection && ( !rSh.HasSelection() || 0 != nFullSectCnt ))
                        bDisable = false;
                    else if(
                        rSh.HasSelection() && rSh.IsInsRegionAvailable() &&
                            ( !pCurrSection || ( 1 != nFullSectCnt &&
                                lcl_IsMarkInSameSection( rSh, pCurrSection ) )))
                        bDisable = false;
                }
                if(bDisable)
                    rSet.DisableItem(nWhich);
            }
            break;
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void  SwTextShell::ExecDelete(SfxRequest &rReq)
{
    SwWrtShell &rSh = GetShell();
    switch( rReq.GetSlot() )
    {
        case FN_DELETE_SENT:
            if( rSh.IsTableMode() )
            {
                rSh.DeleteRow();
                rSh.EnterStdMode();
            }
            else
                rSh.DelToEndOfSentence();
            break;
        case FN_DELETE_BACK_SENT:
            rSh.DelToStartOfSentence();
            break;
        case FN_DELETE_WORD:
            rSh.DelNxtWord();
            break;
        case FN_DELETE_BACK_WORD:
            rSh.DelPrvWord();
            break;
        case FN_DELETE_LINE:
            rSh.DelToEndOfLine();
            break;
        case FN_DELETE_BACK_LINE:
            rSh.DelToStartOfLine();
            break;
        case FN_DELETE_PARA:
            rSh.DelToEndOfPara();
            break;
        case FN_DELETE_BACK_PARA:
            rSh.DelToStartOfPara();
            break;
        case FN_DELETE_WHOLE_LINE:
            rSh.DelLine();
            break;
        default:
            ASSERT(!this, "falscher Dispatcher");
            return;
    }
    rReq.Done();
}

void SwTextShell::ExecTransliteration( SfxRequest & rReq )
{
    using namespace ::com::sun::star::i18n;
    {
        sal_uInt32 nMode = 0;

        switch( rReq.GetSlot() )
        {
        case SID_TRANSLITERATE_UPPER:
            nMode = TransliterationModules_LOWERCASE_UPPERCASE;
            break;
        case SID_TRANSLITERATE_LOWER:
            nMode = TransliterationModules_UPPERCASE_LOWERCASE;
            break;

        case SID_TRANSLITERATE_HALFWIDTH:
            nMode = TransliterationModules_FULLWIDTH_HALFWIDTH;
            break;
        case SID_TRANSLITERATE_FULLWIDTH:
            nMode = TransliterationModules_HALFWIDTH_FULLWIDTH;
            break;

        case SID_TRANSLITERATE_HIRAGANA:
            nMode = TransliterationModules_KATAKANA_HIRAGANA;
            break;
        case SID_TRANSLITERATE_KATAGANA:
            nMode = TransliterationModules_HIRAGANA_KATAKANA;
            break;

        default:
            ASSERT(!this, "falscher Dispatcher");
        }

        if( nMode )
            GetShell().TransliterateText( nMode );
    }
}


/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



SwTextShell::SwTextShell(SwView &_rView) :
    SwBaseShell(_rView), pPostItFldMgr( 0 )
{
    SetName(String::CreateFromAscii("Text"));
    SetHelpId(SW_TEXTSHELL);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwTextShell::~SwTextShell()
{
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwTextShell::InsertSymbol( SfxRequest& rReq )
{
    const SfxItemSet *pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem = 0;
    if( pArgs )
        pArgs->GetItemState(GetPool().GetWhich(SID_CHARMAP), FALSE, &pItem);

    String aChars, aFontName;
    if ( pItem )
    {
        aChars = ((const SfxStringItem*)pItem)->GetValue();
        const SfxPoolItem* pFtItem = NULL;
        pArgs->GetItemState( GetPool().GetWhich(SID_ATTR_SPECIALCHAR), FALSE, &pFtItem);
        const SfxStringItem* pFontItem = PTR_CAST( SfxStringItem, pFtItem );
        if ( pFontItem )
            aFontName = pFontItem->GetValue();
    }

    SwWrtShell &rSh = GetShell();
    SfxItemSet aSet( GetPool(), RES_CHRATR_FONT, RES_CHRATR_FONT,
                                RES_CHRATR_CJK_FONT, RES_CHRATR_CJK_FONT,
                                RES_CHRATR_CTL_FONT, RES_CHRATR_CTL_FONT,
                                0 );
    rSh.GetCurAttr( aSet );
    USHORT nScript = rSh.GetScriptType();

    SvxFontItem aFont( RES_CHRATR_FONT );
    {
        SvxScriptSetItem aSetItem( SID_ATTR_CHAR_FONT, *aSet.GetPool() );
        aSetItem.GetItemSet().Put( aSet, FALSE );
        const SfxPoolItem* pI = aSetItem.GetItemOfScript( nScript );
        if( pI )
            aFont = *(SvxFontItem*)pI;
        else
            aFont = (SvxFontItem&)aSet.Get( GetWhichOfScript(
                        RES_CHRATR_FONT,
                        GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage() ) ));
        if (!aFontName.Len())
            aFontName = aFont.GetFamilyName();
    }

    Font aNewFont(aFontName, Size(1,1)); // Size nur wg. CTOR
    if( !aChars.Len() )
    {
        // Eingestellten Font als Default
        SfxAllItemSet aAllSet( rSh.GetAttrPool() );
        aAllSet.Put( SfxBoolItem( FN_PARAM_1, FALSE ) );

        SwViewOption aOpt(*GetShell().GetViewOptions());
        String sSymbolFont = aOpt.GetSymbolFont();
        if( !aFontName.Len() && sSymbolFont.Len() )
            aAllSet.Put( SfxStringItem( SID_FONT_NAME, sSymbolFont ) );
        else
            aAllSet.Put( SfxStringItem( SID_FONT_NAME, aFont.GetFamilyName() ) );

        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        SfxAbstractDialog* pDlg = pFact->CreateSfxDialog( GetView().GetWindow(), aAllSet,
            GetView().GetViewFrame()->GetFrame()->GetFrameInterface(), RID_SVXDLG_CHARMAP );
        if( RET_OK == pDlg->Execute() )
        {
            SFX_ITEMSET_ARG( pDlg->GetOutputItemSet(), pCItem, SfxStringItem, SID_CHARMAP, FALSE );
            SFX_ITEMSET_ARG( pDlg->GetOutputItemSet(), pFontItem, SvxFontItem, SID_ATTR_CHAR_FONT, FALSE );
            if ( pFontItem )
            {
                aNewFont.SetName( pFontItem->GetFamilyName() );
                aNewFont.SetStyleName( pFontItem->GetStyleName() );
                aNewFont.SetCharSet( pFontItem->GetCharSet() );
                aNewFont.SetPitch( pFontItem->GetPitch() );
            }

            if ( pCItem )
            {
                aChars  = pCItem->GetValue();
                aOpt.SetSymbolFont(aNewFont.GetName());
                SW_MOD()->ApplyUsrPref(aOpt, &GetView());
            }
        }

        delete pDlg;
    }

    BOOL bFontChanged = FALSE;
    if( aChars.Len() )
    {
        rSh.StartAllAction();

        // Selektierten Inhalt loeschen
        SwRewriter aRewriter;
        aRewriter.AddRule(UNDO_ARG1, SW_RES(STR_SPECIALCHAR));

        rSh.StartUndo( UNDO_INSERT, &aRewriter );
        if ( rSh.HasSelection() )
        {
            rSh.DelRight();
            aSet.ClearItem();
            rSh.GetCurAttr( aSet );

            SvxScriptSetItem aSetItem( SID_ATTR_CHAR_FONT, *aSet.GetPool() );
            aSetItem.GetItemSet().Put( aSet, FALSE );
            const SfxPoolItem* pI = aSetItem.GetItemOfScript( nScript );
            if( pI )
                aFont = *(SvxFontItem*)pI;
            else
                aFont = (SvxFontItem&)aSet.Get( GetWhichOfScript(
                            RES_CHRATR_FONT,
                            GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage() ) ));
        }

        // Zeichen einfuegen
        rSh.Insert( aChars );

        // #108876# a font attribute has to be set always due to a guessed script type
        if( aNewFont.GetName().Len() )
        {
            bFontChanged = TRUE;
            SvxFontItem aNewFontItem( aFont );
            aNewFontItem.GetFamilyName() = aNewFont.GetName();
            aNewFontItem.GetFamily()	 = aNewFont.GetFamily();
            aNewFontItem.GetPitch() 	 = aNewFont.GetPitch();
            aNewFontItem.GetCharSet()	 = aNewFont.GetCharSet();

            SfxItemSet aRestoreSet( GetPool(), RES_CHRATR_FONT, RES_CHRATR_FONT,
                                               RES_CHRATR_CJK_FONT, RES_CHRATR_CJK_FONT,
                                               RES_CHRATR_CTL_FONT, RES_CHRATR_CTL_FONT, 0 );

            nScript = pBreakIt->GetAllScriptsOfText( aChars );
            if( SCRIPTTYPE_LATIN & nScript )
            {
                aRestoreSet.Put( aSet.Get( RES_CHRATR_FONT, TRUE ) );
                aSet.Put( aNewFontItem, RES_CHRATR_FONT);
            }
            if( SCRIPTTYPE_ASIAN & nScript )
            {
                aRestoreSet.Put( aSet.Get( RES_CHRATR_CJK_FONT, TRUE ) );
                aSet.Put( aNewFontItem, RES_CHRATR_CJK_FONT );
            }
            if( SCRIPTTYPE_COMPLEX & nScript )
            {
                aRestoreSet.Put( aSet.Get( RES_CHRATR_CTL_FONT, TRUE ) );
                aSet.Put( aNewFontItem, RES_CHRATR_CTL_FONT );
            }

            rSh.SetMark();
            rSh.ExtendSelection( FALSE, aChars.Len() );
            rSh.SetAttr( aSet, nsSetAttrMode::SETATTR_DONTEXPAND | nsSetAttrMode::SETATTR_NOFORMATATTR );
            if( !rSh.IsCrsrPtAtEnd() )
                rSh.SwapPam();

            rSh.ClearMark();

            // --> FME 2007-07-09 #i75891#
            // SETATTR_DONTEXPAND does not work if there are already hard attributes.
            // Therefore we have to restore the font attributes.
            rSh.SetMark();
            rSh.SetAttr( aRestoreSet );
            rSh.ClearMark();
            // <--

            rSh.UpdateAttr();
            aFont = aNewFontItem;
        }

        rSh.EndAllAction();
        rSh.EndUndo( UNDO_INSERT );

        if ( aChars.Len() )
        {
            rReq.AppendItem( SfxStringItem( GetPool().GetWhich(SID_CHARMAP), aChars ) );
            rReq.AppendItem( SfxStringItem( SID_ATTR_SPECIALCHAR, aNewFont.GetName() ) );
            rReq.Done();
        }
    }
}


