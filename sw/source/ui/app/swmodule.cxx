/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swmodule.cxx,v $
 * $Revision: 1.67 $
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
#ifndef _SWERROR_H
#include <swerror.h>
#endif
#include <vcl/wrkwin.hxx>
#include <vcl/graph.hxx>
#include <svx/galbrws.hxx>
#include <svx/svdobj.hxx>
#include <svtools/ehdl.hxx>
#include <svx/fntszctl.hxx>
#include <svx/fntctl.hxx>
#include <svx/SmartTagCtl.hxx>
#include <svx/pszctrl.hxx>
#include <svx/insctrl.hxx>
#include <svx/selctrl.hxx>
#include <svx/linectrl.hxx>
#include <svx/tbxctl.hxx>			//z-Zt falscher includeschutz!
#include <svx/fillctrl.hxx>
#include <svx/tbcontrl.hxx>
#include <svx/verttexttbxctrl.hxx>
#include <svx/formatpaintbrushctrl.hxx>
#include <svx/contdlg.hxx>
#include <svx/layctrl.hxx>
#include <svx/fontwork.hxx>
#include <SwSpellDialogChildWindow.hxx>
#include <svx/tbxalign.hxx>
#include <svx/grafctrl.hxx>
#include <svx/tbxcolor.hxx>
#include <svx/clipboardctl.hxx>
#include <svx/lboxctrl.hxx>
#include <svx/extrusioncontrols.hxx>
#include <svx/hyprlink.hxx>
#include <svx/tbxcustomshapes.hxx>
#include <svx/fontworkgallery.hxx>
#include <svx/imapdlg.hxx>
#include <svx/srchdlg.hxx>
#include <svx/hyperdlg.hxx>
#include <com/sun/star/scanner/XScannerManager.hpp>
#include <com/sun/star/container/XSet.hpp>
#include <comphelper/processfactory.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <swmodule.hxx>
#include <swevent.hxx>
#include <swacorr.hxx>
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#include <dobjfac.hxx>
#include <init.hxx>
#include <pview.hxx>
#include <wview.hxx>
#include <wdocsh.hxx>
#include <globdoc.hxx>
#ifndef _SRCVIEW_HXX //autogen
#include <srcview.hxx>
#endif
#include <glshell.hxx>
#include <tabsh.hxx>
#include <listsh.hxx>
#include <grfsh.hxx>
#include <mediash.hxx>
#include <olesh.hxx>
#include <drawsh.hxx>
#include <wformsh.hxx>
#include <drwtxtsh.hxx>
#include <beziersh.hxx>
#include <wtextsh.hxx>
#include <wfrmsh.hxx>
#ifndef _DRFORMSH_HXX
#include <drformsh.hxx>
#endif
#ifndef _WGRFSH_HXX
#include <wgrfsh.hxx>
#endif
#ifndef _WOLESH_HXX
#include <wolesh.hxx>
#endif
#ifndef _WLISTSH_HXX
#include <wlistsh.hxx>
#endif
#ifndef _WTABSH_HXX
#include <wtabsh.hxx>
#endif
#include <navipi.hxx>
#include <chartins.hxx>
#include <inputwin.hxx>
#include <usrpref.hxx>
#include <uinums.hxx>
#include <prtopt.hxx>
#include <bookctrl.hxx>
#include <tmplctrl.hxx>
#include <viewlayoutctrl.hxx>
#include <svx/zoomsliderctrl.hxx>
#include <tblctrl.hxx>
#include <zoomctrl.hxx>
#include <workctrl.hxx>
#include <tbxanchr.hxx>
#include <fldwrap.hxx>
#ifndef _REDLNDLG_HXX
#include <redlndlg.hxx>
#endif
#ifndef _SYNCBTN_HXX
#include <syncbtn.hxx>
#endif
#include <mailmergechildwindow.hxx>
#include <modcfg.hxx>
#include <fontcfg.hxx>
#include <sfx2/evntconf.hxx>
#include <sfx2/appuno.hxx>
#include <swatrset.hxx>
#include <idxmrk.hxx>
#include <dlelstnr.hxx>
#ifndef _BARCFG_HXX
#include <barcfg.hxx>
#endif
#include <svx/rubydialog.hxx>
// OD 14.02.2003 #107424#
#include <svtools/colorcfg.hxx>

#include <svx/acorrcfg.hxx>
#include <svtools/moduleoptions.hxx>

#ifndef _AVMEDIA_MEDIAPPLAYER_HXX
#include <avmedia/mediaplayer.hxx>
#endif
#include <avmedia/mediatoolbox.hxx>

#include <annotsh.hxx>

#include <app.hrc>
#include <svx/xmlsecctrl.hxx>
ResMgr *pSwResMgr = 0;
sal_Bool 	bNoInterrupt 	= sal_False;

#include <sfx2/app.hxx>

#include <svx/svxerr.hxx>

#include <unomid.h>

using namespace com::sun::star;


TYPEINIT1( SwModule, SfxModule );

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

//************************************************************************

SwModule::SwModule( SfxObjectFactory* pWebFact,
                    SfxObjectFactory* pFact,
                    SfxObjectFactory* pGlobalFact )
    : SfxModule( SfxApplication::CreateResManager( "sw" ), sal_False, pWebFact,
                     pFact, pGlobalFact, NULL ),
    pModuleConfig(0),
    pUsrPref(0),
    pWebUsrPref(0),
    pPrtOpt(0),
    pWebPrtOpt(0),
    pChapterNumRules(0),
    pStdFontConfig(0),
    pNavigationConfig(0),
    pToolbarConfig(0),
    pWebToolbarConfig(0),
    pDBConfig(0),
    pColorConfig(0),
    pAccessibilityOptions(0),
    pCTLOptions(0),
    pUserOptions(0),
    pUndoOptions(0),
    pAttrPool(0),
    pView(0),
    bAuthorInitialised(sal_False),
    bEmbeddedLoadSave( sal_False ),
    pClipboard( 0 ),
    pDragDrop( 0 ),
    pXSelection( 0 )
{
    SetName( String::CreateFromAscii("StarWriter") );
    pSwResMgr = GetResMgr();
    SvxErrorHandler::Get();
    pErrorHdl = new SfxErrorHandler( RID_SW_ERRHDL,
                                     ERRCODE_AREA_SW,
                                     ERRCODE_AREA_SW_END,
                                     pSwResMgr );

    SfxEventConfiguration::RegisterEvent(SW_EVENT_MAIL_MERGE, SW_RES(STR_PRINT_MERGE_MACRO), String::CreateFromAscii("OnMailMerge"));
    SfxEventConfiguration::RegisterEvent(SW_EVENT_MAIL_MERGE_END, SW_RES(STR_PRINT_MERGE_MACRO), String::CreateFromAscii("OnMailMergeFinished"));
    SfxEventConfiguration::RegisterEvent(SW_EVENT_FIELD_MERGE, String(), String::CreateFromAscii("OnFieldMerge"));
    SfxEventConfiguration::RegisterEvent(SW_EVENT_FIELD_MERGE_FINISHED, String(), String::CreateFromAscii("OnFieldMergeFinished"));
    SfxEventConfiguration::RegisterEvent(SW_EVENT_PAGE_COUNT, SW_RES(STR_PAGE_COUNT_MACRO), String::CreateFromAscii("OnPageCountChange"));
    pModuleConfig = new SwModuleOptions;

    //Die brauchen wie sowieso
    pToolbarConfig = new SwToolbarConfigItem( sal_False );
    pWebToolbarConfig = new SwToolbarConfigItem( sal_True );

    pStdFontConfig = new SwStdFontConfig;

    pAuthorNames = new SvStringsDtor(5, 1);	// Alle Redlining-Autoren

    //JP 18.10.96: SvxAutocorrect gegen die SwAutocorrect austauschen
    SvxAutoCorrCfg*	pACfg = SvxAutoCorrCfg::Get();
    if( pACfg )
    {
        const SvxAutoCorrect* pOld = pACfg->GetAutoCorrect();
        pACfg->SetAutoCorrect(new SwAutoCorrect( *pOld ));
    }

    StartListening( *SFX_APP() );

    // OD 14.02.2003 #107424# - init color configuration
    // member <pColorConfig> is created and the color configuration is applied
    // at the view options.
    GetColorConfig();
}

//************************************************************************

uno::Reference< scanner::XScannerManager >
SwModule::GetScannerManager()
{
    if (!m_xScannerManager.is())
    {
        uno::Reference< lang::XMultiServiceFactory > xMgr (
            comphelper::getProcessServiceFactory() );
        if( xMgr.is() )
        {
            m_xScannerManager =
                uno::Reference< scanner::XScannerManager >(
                    xMgr->createInstance(
                        rtl::OUString::createFromAscii(
                            "com.sun.star.scanner.ScannerManager" ) ),
                    uno::UNO_QUERY );
        }
    }
    return m_xScannerManager;
}

uno::Reference< linguistic2::XLanguageGuessing > SwModule::GetLanguageGuesser()
{
    if (!m_xLanguageGuesser.is())
    {
        uno::Reference< lang::XMultiServiceFactory > xMgr ( comphelper::getProcessServiceFactory() );
        if (xMgr.is())
        {
            m_xLanguageGuesser = uno::Reference< linguistic2::XLanguageGuessing >(
                    xMgr->createInstance(
                        rtl::OUString::createFromAscii( "com.sun.star.linguistic2.LanguageGuessing" ) ),
                        uno::UNO_QUERY );
        }
    }
    return m_xLanguageGuesser;
}

//************************************************************************

SwModule::~SwModule()
{
    SetPool(0);
    delete pAttrPool;
    delete pErrorHdl;
    EndListening( *SFX_APP() );
}

//************************************************************************

void SwModule::CreateLngSvcEvtListener()
{
    if (!xLngSvcEvtListener.is())
        xLngSvcEvtListener = new SwLinguServiceEventListener;
}

//************************************************************************

void SwDLL::RegisterFactories()
{
    //Diese Id's duerfen nicht geaendert werden. Mittels der Id's wird vom
    //Sfx die View (Dokumentansicht wiederherstellen) erzeugt.
    if ( SvtModuleOptions().IsWriter() )
        SwView::RegisterFactory         ( 2 );

    SwWebView::RegisterFactory		( 5 );

    if ( SvtModuleOptions().IsWriter() )
    {
        SwSrcView::RegisterFactory      ( 6 );
        SwPagePreView::RegisterFactory  ( 7 );
    }
}

//************************************************************************


void SwDLL::RegisterInterfaces()
{
    SwModule* pMod = SW_MOD();
    SwModule::RegisterInterface( pMod );
    SwDocShell::RegisterInterface( pMod );
    SwWebDocShell::RegisterInterface( pMod );
    SwGlosDocShell::RegisterInterface( pMod );
    SwWebGlosDocShell::RegisterInterface( pMod );
    SwView::RegisterInterface( pMod );
    SwWebView::RegisterInterface( pMod );
    SwPagePreView::RegisterInterface( pMod );
    SwSrcView::RegisterInterface( pMod );


    SwBaseShell::RegisterInterface(pMod);
    SwTextShell::RegisterInterface(pMod);
    SwTableShell::RegisterInterface(pMod);
    SwListShell::RegisterInterface(pMod);
    SwFrameShell::RegisterInterface(pMod);
    SwDrawBaseShell::RegisterInterface(pMod);
    SwDrawShell::RegisterInterface(pMod);
    SwDrawFormShell::RegisterInterface(pMod);
    SwDrawTextShell::RegisterInterface(pMod);
    SwBezierShell::RegisterInterface(pMod);
    SwGrfShell::RegisterInterface(pMod);
    SwOleShell::RegisterInterface(pMod);
    SwWebTextShell::RegisterInterface(pMod);
    SwWebFrameShell::RegisterInterface(pMod);
    SwWebGrfShell::RegisterInterface(pMod);
    SwWebListShell::RegisterInterface(pMod);
    SwWebTableShell::RegisterInterface(pMod);
    SwWebDrawFormShell::RegisterInterface(pMod);
    SwWebOleShell::RegisterInterface(pMod);
    SwMediaShell::RegisterInterface(pMod);
    SwAnnotationShell::RegisterInterface(pMod);
}

//************************************************************************

void SwDLL::RegisterControls()
{
    SwModule* pMod = SW_MOD();
    SvxTbxCtlDraw::RegisterControl(SID_INSERT_DRAW, pMod );
    SvxTbxCtlAlign::RegisterControl(SID_OBJECT_ALIGN, pMod );
    SwTbxAnchor::RegisterControl(FN_TOOL_ANKER, pMod );
    SwTbxInsertCtrl::RegisterControl(FN_INSERT_CTRL, pMod );
    SwTbxInsertCtrl::RegisterControl(FN_INSERT_OBJ_CTRL, pMod );
    SwTbxAutoTextCtrl::RegisterControl(FN_INSERT_FIELD_CTRL, pMod );
    SwTbxAutoTextCtrl::RegisterControl(FN_GLOSSARY_DLG, pMod );

    SvxTbxCtlCustomShapes::RegisterControl( SID_DRAWTBX_CS_BASIC, pMod );
    SvxTbxCtlCustomShapes::RegisterControl( SID_DRAWTBX_CS_SYMBOL, pMod );
    SvxTbxCtlCustomShapes::RegisterControl( SID_DRAWTBX_CS_ARROW, pMod );
    SvxTbxCtlCustomShapes::RegisterControl( SID_DRAWTBX_CS_FLOWCHART, pMod );
    SvxTbxCtlCustomShapes::RegisterControl( SID_DRAWTBX_CS_CALLOUT, pMod );
    SvxTbxCtlCustomShapes::RegisterControl( SID_DRAWTBX_CS_STAR, pMod );

    svx::ExtrusionDepthControl::RegisterControl( SID_EXTRUSION_DEPTH_FLOATER, pMod );
    svx::ExtrusionDirectionControl::RegisterControl( SID_EXTRUSION_DIRECTION_FLOATER, pMod );
    svx::ExtrusionLightingControl::RegisterControl( SID_EXTRUSION_LIGHTING_FLOATER, pMod );
    svx::ExtrusionSurfaceControl::RegisterControl( SID_EXTRUSION_SURFACE_FLOATER, pMod );
    svx::ExtrusionColorControl::RegisterControl( SID_EXTRUSION_3D_COLOR, pMod );

    svx::FontWorkShapeTypeControl::RegisterControl( SID_FONTWORK_SHAPE_TYPE, pMod );
    svx::FontWorkAlignmentControl::RegisterControl( SID_FONTWORK_ALIGNMENT_FLOATER, pMod );
    svx::FontWorkCharacterSpacingControl::RegisterControl( SID_FONTWORK_CHARACTER_SPACING_FLOATER, pMod );

    SvxClipBoardControl::RegisterControl(SID_PASTE, pMod );
    SvxUndoRedoControl::RegisterControl(SID_UNDO, pMod );
    SvxUndoRedoControl::RegisterControl(SID_REDO, pMod );
    svx::FormatPaintBrushToolBoxControl::RegisterControl(SID_FORMATPAINTBRUSH, pMod );

    SvxFillToolBoxControl::RegisterControl(SID_ATTR_FILL_STYLE, pMod );
    SvxLineStyleToolBoxControl::RegisterControl(SID_ATTR_LINE_STYLE, pMod );
    SvxLineWidthToolBoxControl::RegisterControl(SID_ATTR_LINE_WIDTH, pMod );
    SvxLineColorToolBoxControl::RegisterControl(SID_ATTR_LINE_COLOR, pMod );
    SvxLineEndToolBoxControl::RegisterControl(SID_ATTR_LINEEND_STYLE, pMod );

    SvxFontNameToolBoxControl::RegisterControl(SID_ATTR_CHAR_FONT, pMod );
//	SvxFontHeightToolBoxControl::RegisterControl(SID_ATTR_CHAR_FONTHEIGHT, pMod );
    SvxFontColorToolBoxControl::RegisterControl(SID_ATTR_CHAR_COLOR, pMod );
    SvxFontColorExtToolBoxControl::RegisterControl(SID_ATTR_CHAR_COLOR2, pMod );
    SvxFontColorExtToolBoxControl::RegisterControl(SID_ATTR_CHAR_COLOR_BACKGROUND, pMod );
    SvxStyleToolBoxControl::RegisterControl(SID_STYLE_APPLY, pMod );
    SvxColorToolBoxControl::RegisterControl(SID_BACKGROUND_COLOR, pMod );
    SvxFrameToolBoxControl::RegisterControl(SID_ATTR_BORDER, pMod );
    SvxFrameLineStyleToolBoxControl::RegisterControl(SID_FRAME_LINESTYLE, pMod );
    SvxFrameLineColorToolBoxControl::RegisterControl(SID_FRAME_LINECOLOR, pMod );

    SvxColumnsToolBoxControl::RegisterControl(FN_INSERT_FRAME_INTERACT, pMod );
    SvxColumnsToolBoxControl::RegisterControl(FN_INSERT_FRAME, pMod );
    SvxColumnsToolBoxControl::RegisterControl(FN_INSERT_REGION, pMod );
    SvxTableToolBoxControl::RegisterControl(FN_INSERT_TABLE, pMod );
    SvxTableToolBoxControl::RegisterControl(FN_SHOW_MULTIPLE_PAGES, pMod );

    SvxFontMenuControl::RegisterControl(SID_ATTR_CHAR_FONT, pMod );
    SvxFontSizeMenuControl::RegisterControl(SID_ATTR_CHAR_FONTHEIGHT, pMod );

    SwZoomControl::RegisterControl(SID_ATTR_ZOOM, pMod );
    SwPreviewZoomControl::RegisterControl(FN_PREVIEW_ZOOM, pMod);
    SvxPosSizeStatusBarControl::RegisterControl(0, pMod );
    SvxInsertStatusBarControl::RegisterControl(SID_ATTR_INSERT, pMod );
    SvxSelectionModeControl::RegisterControl(FN_STAT_SELMODE, pMod );
    XmlSecStatusBarControl::RegisterControl( SID_SIGNATURE, pMod );

    SwBookmarkControl::RegisterControl(FN_STAT_PAGE, pMod );
    SwTemplateControl::RegisterControl(FN_STAT_TEMPLATE, pMod );
    SwViewLayoutControl::RegisterControl( SID_ATTR_VIEWLAYOUT, pMod );
    SvxZoomSliderControl::RegisterControl( SID_ATTR_ZOOMSLIDER, pMod );

    SwTableOptimizeCtrl::RegisterControl(FN_OPTIMIZE_TABLE, pMod);

    SvxIMapDlgChildWindow::RegisterChildWindow( sal_False, pMod );
    SvxSearchDialogWrapper::RegisterChildWindow( sal_False, pMod );
    SvxHlinkDlgWrapper::RegisterChildWindow( sal_False, pMod );
    SvxHyperlinkDlgWrapper::RegisterChildWindow( sal_False, pMod, SFX_CHILDWIN_FORCEDOCK );
    SvxFontWorkChildWindow::RegisterChildWindow( sal_False, pMod );
    SwFldDlgWrapper::RegisterChildWindow( sal_False, pMod );
    SwFldDataOnlyDlgWrapper::RegisterChildWindow( sal_False, pMod );
    SvxContourDlgChildWindow::RegisterChildWindow( sal_False, pMod );
    SwNavigationChild::RegisterChildWindowContext( pMod );
    SwInputChild::RegisterChildWindow( sal_False, pMod, SFX_CHILDWIN_FORCEDOCK );
    SwRedlineAcceptChild::RegisterChildWindow( sal_False, pMod );
    SwSyncChildWin::RegisterChildWindow( sal_True, pMod );
    SwMailMergeChildWindow::RegisterChildWindow( sal_False, pMod );
//    SwSendMailChildWindow::RegisterChildWindow( sal_False, pMod );
    SwInsertIdxMarkWrapper::RegisterChildWindow( sal_False, pMod );
    SwInsertAuthMarkWrapper::RegisterChildWindow( sal_False, pMod );
    SvxRubyChildWindow::RegisterChildWindow( sal_False, pMod);
    SwSpellDialogChildWindow::RegisterChildWindow(sal_False, pMod);

    SvxGrafRedToolBoxControl::RegisterControl( SID_ATTR_GRAF_RED, pMod );
    SvxGrafGreenToolBoxControl::RegisterControl( SID_ATTR_GRAF_GREEN, pMod );
    SvxGrafBlueToolBoxControl::RegisterControl( SID_ATTR_GRAF_BLUE, pMod );
    SvxGrafLuminanceToolBoxControl::RegisterControl( SID_ATTR_GRAF_LUMINANCE, pMod );
    SvxGrafContrastToolBoxControl::RegisterControl( SID_ATTR_GRAF_CONTRAST, pMod );
    SvxGrafGammaToolBoxControl::RegisterControl( SID_ATTR_GRAF_GAMMA, pMod );
    SvxGrafTransparenceToolBoxControl::RegisterControl( SID_ATTR_GRAF_TRANSPARENCE, pMod );
    SvxGrafModeToolBoxControl::RegisterControl( SID_ATTR_GRAF_MODE, pMod );
    SvxGrafFilterToolBoxControl::RegisterControl( SID_GRFFILTER, pMod );
    SvxVertTextTbxCtrl::RegisterControl(SID_TEXTDIRECTION_LEFT_TO_RIGHT, pMod);
    SvxVertTextTbxCtrl::RegisterControl(SID_TEXTDIRECTION_TOP_TO_BOTTOM, pMod);
    SvxVertTextTbxCtrl::RegisterControl(SID_DRAW_CAPTION_VERTICAL, pMod);
    SvxVertTextTbxCtrl::RegisterControl(SID_DRAW_TEXT_VERTICAL, pMod);

    SvxCTLTextTbxCtrl::RegisterControl(SID_ATTR_PARA_LEFT_TO_RIGHT, pMod);
    SvxCTLTextTbxCtrl::RegisterControl(SID_ATTR_PARA_RIGHT_TO_LEFT, pMod);

    svx::ExtrusionDepthControl::RegisterControl( SID_EXTRUSION_DEPTH_FLOATER, pMod );
    svx::ExtrusionDirectionControl::RegisterControl( SID_EXTRUSION_DIRECTION_FLOATER, pMod );
    svx::ExtrusionLightingControl::RegisterControl( SID_EXTRUSION_LIGHTING_FLOATER, pMod );
    svx::ExtrusionSurfaceControl::RegisterControl( SID_EXTRUSION_SURFACE_FLOATER, pMod );
    svx::ExtrusionColorControl::RegisterControl( SID_EXTRUSION_3D_COLOR, pMod );

    GalleryChildWindow::RegisterChildWindow(0, pMod);

    ::avmedia::MediaToolBoxControl::RegisterControl(SID_AVMEDIA_TOOLBOX, pMod);
    ::avmedia::MediaPlayer::RegisterChildWindow(0, pMod);

    SvxSmartTagsControl::RegisterControl(SID_OPEN_SMARTTAGMENU, pMod);
}



/*************************************************************************
|*
|* Modul laden (nur Attrappe fuer das Linken der DLL)
|*
\************************************************************************/

/* -----------------20.04.99 10:46-------------------
 *
 * --------------------------------------------------*/
void	SwModule::InitAttrPool()
{
    DBG_ASSERT(!pAttrPool, "Pool ist schon da!");
    pAttrPool = new SwAttrPool(0);
    SetPool(pAttrPool);
}
/* -----------------20.04.99 10:46-------------------
 *
 * --------------------------------------------------*/
void	SwModule::RemoveAttrPool()
{
    SetPool(0);
    DELETEZ(pAttrPool);
}


