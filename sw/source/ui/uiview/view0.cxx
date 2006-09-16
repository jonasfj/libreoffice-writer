/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: view0.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 23:24:05 $
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



#include "hintids.hxx"

#ifndef _SV_GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _SVX_GALBRWS_HXX_
#include <svx/galbrws.hxx>
#endif
#ifndef _SVX_SRCHITEM_HXX
#include <svx/srchitem.hxx>
#endif
#ifndef SW_SPELL_DIALOG_CHILD_WINDOW_HXX
#include <SwSpellDialogChildWindow.hxx>
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SVTOOLS_LINGUPROPS_HXX_
#include <svtools/linguprops.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _GLOBALS_H
#include <globals.h>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SVX_SRCHDLG_HXX //autogen
#include <svx/srchdlg.hxx>
#endif
#ifndef _SFX_TEMPLDLG_HXX //autogen
#include <sfx2/templdlg.hxx>
#endif
#ifndef _UIVWIMP_HXX
#include <uivwimp.hxx>
#endif
#ifndef _AVMEDIA_MEDIAPPLAYER_HXX
#include <avmedia/mediaplayer.hxx>
#endif
#ifndef _SWLINGUCONFIG_HXX
#include <swlinguconfig.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif

#include <sfx2/objface.hxx>

#ifndef _NAVIPI_HXX //autogen
#include <navipi.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#include "view.hxx"
#include "basesh.hxx"
#include "docsh.hxx"
#include "globals.hrc"
#include "cmdid.h"		 	// FN_		 ...
#include "globdoc.hxx"
#include "wview.hxx"
#include "shells.hrc"

#define OLEObjects
#define SwView
#define SearchAttributes
#define ReplaceAttributes
#define SearchSettings
#define _ExecSearch ExecSearch
#define _StateSearch StateSearch
#define Frames
#define Graphics
#define Tables
#define Controls
#define GlobalContents
#define Text
#define Frame
#define Graphic
#define Object
#define Draw
#define TextDrawText
#define TextInTable
#define ListInText
#define ListInTable
#define WebTextInTable
#define WebListInText
#define WebListInTable
#define TextPage
#include "itemdef.hxx"
#include <svx/svxslots.hxx>
#include "swslots.hxx"

using namespace ::com::sun::star;
using namespace ::rtl;

#include <svtools/moduleoptions.hxx>

#include <IDocumentSettingAccess.hxx>

#define C2S(cChar) UniString::CreateFromAscii(cChar)

SFX_IMPL_VIEWFACTORY(SwView, SW_RES(STR_NONAME))
{
    if ( SvtModuleOptions().IsWriter() )
    {
        SFX_VIEW_REGISTRATION(SwDocShell);
        SFX_VIEW_REGISTRATION(SwGlobalDocShell);
    }
}

SFX_IMPL_INTERFACE( SwView, SfxViewShell, SW_RES(RID_TOOLS_TOOLBOX) )
{
    SFX_CHILDWINDOW_CONTEXT_REGISTRATION(SID_NAVIGATOR);
    SFX_CHILDWINDOW_REGISTRATION(SfxTemplateDialogWrapper::GetChildWindowId());
    SFX_CHILDWINDOW_REGISTRATION(SvxSearchDialogWrapper::GetChildWindowId());
    SFX_CHILDWINDOW_REGISTRATION(SwSpellDialogChildWindow::GetChildWindowId());
    SFX_CHILDWINDOW_REGISTRATION(FN_REDLINE_ACCEPT);
    SFX_CHILDWINDOW_REGISTRATION(SID_HYPERLINK_DIALOG);
    SFX_CHILDWINDOW_REGISTRATION(GalleryChildWindow::GetChildWindowId());
    SFX_CHILDWINDOW_REGISTRATION(::avmedia::MediaPlayer::GetChildWindowId());
    SFX_CHILDWINDOW_REGISTRATION(FN_INSERT_FIELD_DATA_ONLY);
        SFX_FEATURED_CHILDWINDOW_REGISTRATION(FN_SYNC_LABELS,           CHILDWIN_LABEL    );
        SFX_FEATURED_CHILDWINDOW_REGISTRATION(FN_MAILMERGE_CHILDWINDOW, CHILDWIN_MAILMERGE);
//    SFX_CHILDWINDOW_REGISTRATION(FN_MAILMERGE_SENDMAIL_CHILDWINDOW);
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_TOOLS|
                                SFX_VISIBILITY_STANDARD|SFX_VISIBILITY_SERVER,
                                SW_RES(RID_TOOLS_TOOLBOX) );
}

TYPEINIT1(SwView,SfxViewShell)

/*-----------------13.12.97 11:06-------------------

--------------------------------------------------*/
ShellModes	SwView::GetShellMode()
{
    return pViewImpl->GetShellMode();
}

/*-----------------13.12.97 11:28-------------------

--------------------------------------------------*/
view::XSelectionSupplier* SwView::GetUNOObject()
{
    return pViewImpl->GetUNOObject();
}
/* -----------------------------06.05.2002 13:18------------------------------

 ---------------------------------------------------------------------------*/
void SwView::ApplyAccessiblityOptions(SvtAccessibilityOptions& rAccessibilityOptions)
{
    pWrtShell->ApplyAccessiblityOptions(rAccessibilityOptions);
    //to enable the right state of the selection cursor in readonly documents
    if(GetDocShell()->IsReadOnly())
        pWrtShell->ShowCrsr();

}
/*-- 26.05.2004 09:14:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void   SwView::SetMailMergeConfigItem(SwMailMergeConfigItem*  pConfigItem,
                sal_uInt16 nRestart, sal_Bool bIsSource)
{
    pViewImpl->SetMailMergeConfigItem(pConfigItem, nRestart, bIsSource);
    UIFeatureChanged();
}
/*-- 26.05.2004 09:14:25---------------------------------------------------

  -----------------------------------------------------------------------*/
SwMailMergeConfigItem*  SwView::GetMailMergeConfigItem()
{
    return pViewImpl->GetMailMergeConfigItem();
}
/*-- 26.05.2004 09:14:25---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_uInt16 SwView::GetMailMergeRestartPage() const
{
    return pViewImpl->GetMailMergeRestartPage();
}
/*-- 03.09.2004 11:56:33---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwView::IsMailMergeSourceView() const
{
    return pViewImpl->IsMailMergeSourceView();
}
/*-- 12.04.2006 11:51:40---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool lcl_IsViewMarks( const SwViewOption& rVOpt )
{
    return  rVOpt.IsHardBlank() &&
            rVOpt.IsSoftHyph() &&
            SwViewOption::IsFieldShadings();
}
void lcl_SetViewMarks(SwViewOption& rVOpt, sal_Bool bOn )
{
    rVOpt.SetHardBlank(bOn);
    rVOpt.SetSoftHyph(bOn);
    SwViewOption::SetAppearanceFlag(
            VIEWOPT_FIELD_SHADINGS, bOn, TRUE);
}

void lcl_SetViewMetaChars( SwViewOption& rVOpt, sal_Bool bOn)
{
    rVOpt.SetViewMetaChars( bOn );
    if(bOn && !(rVOpt.IsParagraph()     ||
            rVOpt.IsTab()       ||
            rVOpt.IsLineBreak() ||
            rVOpt.IsShowHiddenChar() ||
            rVOpt.IsBlank()))
    {
        rVOpt.SetParagraph(bOn);
        rVOpt.SetTab(bOn);
        rVOpt.SetLineBreak(bOn);
        rVOpt.SetBlank(bOn);
        rVOpt.SetShowHiddenChar(bOn);
    }
}

/*--------------------------------------------------------------------
    State of view options
 --------------------------------------------------------------------*/
void SwView::StateViewOptions(SfxItemSet &rSet)
{
    SfxWhichIter aIter(rSet);
    sal_uInt16 nWhich = aIter.FirstWhich();
    SfxBoolItem aBool;
    const SwViewOption* pOpt = GetWrtShell().GetViewOptions();
    const IDocumentSettingAccess* pIDSA = GetDocShell()->getIDocumentSettingAccess();

    while(nWhich)
    {
        sal_Bool bReadonly = GetDocShell()->IsReadOnly();
        sal_Bool bBrowse = pIDSA ? pIDSA->get( IDocumentSettingAccess::BROWSE_MODE ) : sal_False;
        if ( bReadonly && nWhich != FN_VIEW_GRAPHIC )
        {
            rSet.DisableItem(nWhich);
            nWhich = 0;
        }
        switch(nWhich)
        {
            case FN_RULER:
            {
                if(!pOpt->IsViewHRuler(TRUE) && !pOpt->IsViewVRuler(TRUE))
                {
                    rSet.DisableItem(nWhich);
                    nWhich = 0;
                }
                else
                    aBool.SetValue( pOpt->IsViewAnyRuler());
            }
            break;
            case FN_VIEW_BOUNDS:
                aBool.SetValue( SwViewOption::IsDocBoundaries()); break;
            case FN_VIEW_GRAPHIC:
                aBool.SetValue( !pOpt->IsGraphic() ); break;
            case FN_VIEW_FIELDS:
                aBool.SetValue( SwViewOption::IsFieldShadings() ); break;
            case FN_VIEW_FIELDNAME:
                aBool.SetValue( pOpt->IsFldName() ); break;
            case FN_VIEW_MARKS:
                aBool.SetValue( lcl_IsViewMarks(*pOpt) ); break;
            case FN_VIEW_META_CHARS:
                aBool.SetValue( pOpt->IsViewMetaChars() ); break;
            case FN_VIEW_TABLEGRID:
                aBool.SetValue( SwViewOption::IsTableBoundaries() ); break;
            case FN_VIEW_HIDDEN_PARA:
                aBool.SetValue( pOpt->IsShowHiddenPara()); break;
            case SID_GRID_VISIBLE:
                aBool.SetValue( pOpt->IsGridVisible() ); break;
            case SID_GRID_USE:
                aBool.SetValue( pOpt->IsSnap() ); break;
            case SID_HELPLINES_MOVE:
                aBool.SetValue( pOpt->IsCrossHair() ); break;
            case FN_VIEW_SMOOTH_SCROLL:
                aBool.SetValue( pOpt->IsSmoothScroll()); break;
            case FN_VLINEAL:
                aBool.SetValue( StatVLineal() ); break;
            case FN_HSCROLLBAR:
                if(bBrowse)
                {
                    rSet.DisableItem(nWhich);
                    nWhich = 0;
                }
                else
                    aBool.SetValue( IsHScrollbarVisible() ); break;
            case FN_VSCROLLBAR:
                aBool.SetValue( IsVScrollbarVisible() ); break;
            case SID_AUTOSPELL_CHECK:
                aBool.SetValue( pOpt->IsOnlineSpell() );
            break;
            case SID_AUTOSPELL_MARKOFF:
                aBool.SetValue( pOpt->IsHideSpell() );
            break;
            case FN_SHADOWCURSOR:
                if (pIDSA == 0 || pIDSA->get( IDocumentSettingAccess::BROWSE_MODE ))
                {
                    rSet.DisableItem( nWhich );
                    nWhich = 0;
                }
                else
                    aBool.SetValue( pOpt->IsShadowCursor() );
            break;

        }

        if( nWhich )
        {
            aBool.SetWhich( nWhich );
            rSet.Put( aBool );
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    execute view options
 --------------------------------------------------------------------*/
void SwView::ExecViewOptions(SfxRequest &rReq)
{
    SwViewOption* pOpt = pOpt = new SwViewOption( *GetWrtShell().GetViewOptions() );
    sal_Bool bModified = GetWrtShell().IsModified();

    int eState = STATE_TOGGLE;
    sal_Bool bSet = sal_False;

    const SfxItemSet *pArgs = rReq.GetArgs();
    sal_uInt16 nSlot = rReq.GetSlot();
    const SfxPoolItem* pAttr=NULL;

    if( pArgs && SFX_ITEM_SET == pArgs->GetItemState( nSlot , sal_False, &pAttr ))
    {
        bSet = ((SfxBoolItem*)pAttr)->GetValue();
        eState = bSet ? STATE_ON : STATE_OFF;
    }

    int bFlag = STATE_ON == eState;
    uno::Reference< beans::XPropertySet >  xLngProp( ::GetLinguPropertySet() );

    switch ( nSlot )
    {
        case FN_VIEW_GRAPHIC:

                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsGraphic();
                pOpt->SetGraphic( bFlag );
                break;

        case FN_VIEW_FIELDS:
                if( STATE_TOGGLE == eState )
                    bFlag = !SwViewOption::IsFieldShadings() ;
                SwViewOption::SetAppearanceFlag(VIEWOPT_FIELD_SHADINGS, bFlag, TRUE );
                break;

        case FN_VIEW_BOUNDS:
                if( STATE_TOGGLE == eState )
                    bFlag = !SwViewOption::IsDocBoundaries();
                SwViewOption::SetAppearanceFlag(VIEWOPT_DOC_BOUNDARIES, bFlag, TRUE );
                break;

        case SID_GRID_VISIBLE:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsGridVisible();

                pOpt->SetGridVisible( bFlag );
                break;

        case SID_GRID_USE:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsSnap();

                pOpt->SetSnap( bFlag );
                break;

        case SID_HELPLINES_MOVE:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsCrossHair();

                pOpt->SetCrossHair( bFlag );
                break;

        case FN_VIEW_HIDDEN_PARA:
                if ( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsShowHiddenPara();

                pOpt->SetShowHiddenPara( bFlag );
                break;

        case FN_VIEW_SMOOTH_SCROLL:

                if ( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsSmoothScroll();

                pOpt->SetSmoothScroll( bFlag );
                break;


        case FN_VLINEAL:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsViewVRuler();

                pOpt->SetViewVRuler( bFlag );
                break;

        case FN_VSCROLLBAR:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsViewVScrollBar();

                pOpt->SetViewVScrollBar( bFlag );
                break;

        case FN_HSCROLLBAR:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsViewHScrollBar();

                pOpt->SetViewHScrollBar( bFlag );
                break;
        case FN_RULER:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsViewAnyRuler();

                pOpt->SetViewAnyRuler( bFlag );
                break;

        case FN_VIEW_TABLEGRID:
                if( STATE_TOGGLE == eState )
                    bFlag = !SwViewOption::IsTableBoundaries();
                SwViewOption::SetAppearanceFlag(VIEWOPT_TABLE_BOUNDARIES, bFlag, TRUE );
                break;

        case FN_VIEW_FIELDNAME:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsFldName() ;

                pOpt->SetFldName( bFlag );
                break;
        case FN_VIEW_MARKS:
                if( STATE_TOGGLE == eState )
                    bFlag = !lcl_IsViewMarks(*pOpt) ;

                lcl_SetViewMarks( *pOpt, bFlag );
                break;
        case FN_VIEW_META_CHARS:
                if( STATE_TOGGLE == eState )
                    bFlag = !pOpt->IsViewMetaChars();

                lcl_SetViewMetaChars( *pOpt, bFlag );
        break;
        case SID_AUTOSPELL_CHECK:
            if( STATE_TOGGLE == eState )
                bFlag = bSet = !pOpt->IsOnlineSpell();

            pOpt->SetOnlineSpell(bSet);
            {
                uno::Any aVal( &bSet, ::getCppuBooleanType() );
                String aPropName( C2S(UPN_IS_SPELL_AUTO) );

                // #107253# Replaced SvtLinguConfig with SwLinguConfig wrapper with UsageCount
                SwLinguConfig().SetProperty( aPropName, aVal );

                if (xLngProp.is())
                    xLngProp->setPropertyValue( aPropName, aVal );
            }

            if (!(STATE_TOGGLE == eState && bSet && ( pOpt->IsHideSpell() )))
                break;
        case SID_AUTOSPELL_MARKOFF:
            if( STATE_TOGGLE == eState )
                bFlag = bSet = !pOpt->IsHideSpell();

            pOpt->SetHideSpell(bSet);
            {
                uno::Any aVal( &bSet, ::getCppuBooleanType() );
                String aPropName( C2S(UPN_IS_SPELL_HIDE) );

                // #107253# Replaced SvtLinguConfig with SwLinguConfig wrapper with UsageCount
                SwLinguConfig().SetProperty( aPropName, aVal );

                if (xLngProp.is())
                    xLngProp->setPropertyValue( aPropName, aVal );
            }
        break;

        case FN_SHADOWCURSOR:
            if( STATE_TOGGLE == eState )
                bFlag = bSet = !pOpt->IsShadowCursor();

            pOpt->SetShadowCursor(bSet);
        break;

        default:
            ASSERT(sal_False, Falsche Request-Methode);
            return;
    }

    // UserPrefs setzen Request als bearbeitet kennzeichnen
    sal_Bool bWebView =  0 != dynamic_cast<const SwWebView*>(this);
    SwWrtShell &rSh = GetWrtShell();
    rSh.StartAction();
    SwModule* pModule = SW_MOD();
    if( !(*rSh.GetViewOptions() == *pOpt ))
    {
        rSh.ApplyViewOptions( *pOpt );

        //Die UsrPref muessen als Modified gekennzeichnet werden.
        //call for initialization
        pModule->GetUsrPref(bWebView);
        pModule->CheckSpellChanges( pOpt->IsOnlineSpell(), sal_False, sal_False );

    }
    //OS:   Modified wieder zuruecksetzen, weil Ansicht/Felder
    //      das Doc modified setzt.
    if( !bModified )
        rSh.ResetModified();

    pModule->ApplyUsrPref( *pOpt, this, bWebView ? VIEWOPT_DEST_WEB : VIEWOPT_DEST_TEXT );

    const BOOL bLockedView = rSh.IsViewLocked();
    rSh.LockView( TRUE );    //lock visible section
    GetWrtShell().EndAction();
    rSh.LockView( bLockedView );

    delete pOpt;
    Invalidate(rReq.GetSlot());
    if(!pArgs)
        rReq.AppendItem(SfxBoolItem(nSlot, (BOOL)bFlag));
    rReq.Done();
}

