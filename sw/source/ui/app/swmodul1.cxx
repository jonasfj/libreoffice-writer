/*************************************************************************
 *
 *  $RCSfile: swmodul1.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: hr $ $Date: 2004-08-02 14:20:14 $
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
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif

#ifndef _SFXREQUEST_HXX
#include <sfx2/request.hxx>
#endif
#ifndef _SFXDISPATCH_HXX
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX
#include <sfx2/childwin.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCHFLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTATUSLISTENER_HPP_
#include <com/sun/star/frame/XStatusListener.hpp>
#endif
#ifndef _COM_SUN_STAR_VIEW_XSELECTIONSUPPLIER_HPP_
#include <com/sun/star/view/XSelectionSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>	// helper for implementations
#endif
#ifndef _SVX_DATACCESSDESCRIPTOR_HXX_
#include <svx/dataaccessdescriptor.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX //autogen
#include <svx/wghtitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX //autogen
#include <svx/postitem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX //autogen
#include <svx/udlnitem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX //autogen
#include <svx/crsditem.hxx>
#endif
#ifndef _SVX_CMAPITEM_HXX //autogen
#include <svx/cmapitem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX //autogen
#include <svx/colritem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif

#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _USRPREF_HXX
#include <usrpref.hxx>
#endif
#ifndef _MODCFG_HXX
#include <modcfg.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _PVIEW_HXX
#include <pview.hxx>
#endif
#ifndef _WVIEW_HXX
#include <wview.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _UINUMS_HXX
#include <uinums.hxx>
#endif
#ifndef _PRTOPT_HXX
#include <prtopt.hxx>		// fuer PrintOptions
#endif
#ifndef _NAVICFG_HXX
#include <navicfg.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _APP_HRC
#include <app.hrc>
#endif
#include "helpid.h"

using namespace ::rtl;
using namespace ::svx;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::view;
using namespace ::com::sun::star::lang;
#define C2U(char) rtl::OUString::createFromAscii(char)

/* -----------------------------05.01.00 15:14--------------------------------

 ---------------------------------------------------------------------------*/

class SwXDispatchStatusListener : public cppu::WeakImplHelper1
<
    frame::XStatusListener
>
{
    uno::Reference< frame::XStatusListener >  			xSelf;
    util::URL 						aDispURL;
    uno::Reference< frame::XDispatch > 				xDispatch;

    void			Invalidate();
public:
    SwXDispatchStatusListener(uno::Reference< frame::XDispatch > &	xDisp, const util::URL& 	aURL);
    ~SwXDispatchStatusListener();

    //XStatusListener
    virtual void SAL_CALL statusChanged(const frame::FeatureStateEvent& Event) throw( uno::RuntimeException );

    //XEventListener
    virtual void SAL_CALL disposing(const lang::EventObject& Source) throw( uno::RuntimeException );
};
/* -----------------------------05.01.00 15:18--------------------------------

 ---------------------------------------------------------------------------*/
SwXDispatchStatusListener::SwXDispatchStatusListener(
                                uno::Reference< frame::XDispatch > & xDisp, const util::URL& 	aURL) :
    xDispatch(xDisp),
    aDispURL(aURL)
{
    DBG_ASSERT(xDisp.is(),  "XDispatch not set")
    if(xDisp.is())
        xSelf = this;
}
/* -----------------------------05.01.00 15:19--------------------------------

 ---------------------------------------------------------------------------*/
SwXDispatchStatusListener::~SwXDispatchStatusListener()
{
}
/* -----------------------------05.01.00 15:21--------------------------------

 ---------------------------------------------------------------------------*/
void SwXDispatchStatusListener::disposing(const lang::EventObject& Source)
    throw( uno::RuntimeException )
{
    Invalidate();
}
/* -----------------------------05.01.00 15:22--------------------------------

 ---------------------------------------------------------------------------*/
void SwXDispatchStatusListener::statusChanged(const frame::FeatureStateEvent& rEvent)
    throw( uno::RuntimeException )
{
    if(rEvent.FeatureURL.Complete == aDispURL.Complete && rEvent.IsEnabled)
    {
        uno::Sequence <beans::PropertyValue > aArgs(0);
        xDispatch->dispatch(aDispURL, aArgs);
        Invalidate();
    }
}
/* -----------------------------05.01.00 15:45--------------------------------

 ---------------------------------------------------------------------------*/
void	SwXDispatchStatusListener::Invalidate()
{
    if(xDispatch.is())
    {
        try
        {
            xDispatch->removeStatusListener(xSelf, aDispURL);
        }
        catch(...)
        {
        }
    }
    xSelf = 0;
}
/*-----------------08/28/97 08:41pm-----------------

--------------------------------------------------*/
void lcl_SetUIPrefs(const SwViewOption* pPref, SwView* pView, ViewShell* pSh )
{
    // in FrameSets kann die tatsaechliche Sichtbarkeit von der Einstellung der ViewOptions abweichen
    sal_Bool bVScrollChanged = pPref->IsViewVScrollBar() != pSh->GetViewOptions()->IsViewVScrollBar();
    sal_Bool bHScrollChanged = pPref->IsViewHScrollBar() != pSh->GetViewOptions()->IsViewHScrollBar();
    sal_Bool bVAlignChanged = pPref->IsVRulerRight() != pSh->GetViewOptions()->IsVRulerRight();

    pSh->SetUIOptions(*pPref);
    const SwViewOption* pNewPref = pSh->GetViewOptions();

    // Scrollbars an / aus
    if(bVScrollChanged)
    {
        pView->ShowVScrollbar(pNewPref->IsViewVScrollBar());
    }
    if(bHScrollChanged)
    {
        pView->ShowHScrollbar( pNewPref->IsViewHScrollBar() || pSh->IsBrowseMode());
    }
    //if only the position of the vertical ruler has been changed initiate an update
    if(bVAlignChanged && !bHScrollChanged && !bVScrollChanged)
        pView->InvalidateBorder();

    // Lineale an / aus
    if(pNewPref->IsViewVRuler())
        pView->CreateVLineal();
    else
        pView->KillVLineal();

    // TabWindow an/aus
    if(pNewPref->IsViewHRuler())
        pView->CreateTab();
    else
        pView->KillTab();
}

/*--------------------------------------------------------------------
    Beschreibung:	Aktuelle SwWrtShell
 --------------------------------------------------------------------*/


SwWrtShell*	GetActiveWrtShell()
{
    SwView *pActive = ::GetActiveView();
    if( pActive )
        return &pActive->GetWrtShell();
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung: 	Pointer auf die aktuelle Sicht
 --------------------------------------------------------------------*/


SwView* GetActiveView()
{
    SfxViewShell* pView = SfxViewShell::Current();
    return PTR_CAST( SwView, pView );
}
/*--------------------------------------------------------------------
    Beschreibung:	Ueber Views iterieren - static
 --------------------------------------------------------------------*/

SwView* SwModule::GetFirstView()
{
    // liefert nur sichtbare SwViews
    const TypeId aTypeId = TYPE(SwView);
    SwView* pView = (SwView*)SfxViewShell::GetFirst(&aTypeId);
    return pView;
}


SwView* SwModule::GetNextView(SwView* pView)
{
    DBG_ASSERT(PTR_CAST(SwView, pView),"keine SwView uebergeben")
    const TypeId aTypeId = TYPE(SwView);
    // auf Sichtbarkeit pruefen, bis der Sfx das GetFirst/Next
    //mit bOnlyVisible implementiert hat
    SwView* pNView = (SwView*)SfxViewShell::GetNext(*pView, &aTypeId);
    while(pNView && !pNView->GetViewFrame()->IsVisible())
    {
        pNView = (SwView*)SfxViewShell::GetNext(*pNView, &aTypeId);
    }
    return pNView;
}

/*------------------------------------------------------------------------
 Beschreibung:	Liefert UsrPref mit den Einstellungen des aktuellen
                Fensters
------------------------------------------------------------------------*/


void SwModule::MakeUsrPref( SwViewOption &rToFill, sal_Bool bWeb ) const
{
        // Einstellungen Ansicht der Core setzen
    SwView* pView = ::GetActiveView();
    SwPagePreView* pPPView;

    if(pView)
    {
        rToFill = *(pView->GetWrtShell().GetViewOptions());
        SfxFrame* pTopFrame = pView->GetViewFrame()->GetTopFrame();
        if( pTopFrame->GetChildFrameCount() != 0 )
        {
            rToFill.SetViewHRuler(pUsrPref->IsViewHRuler());
            rToFill.SetViewVRuler(pUsrPref->IsViewVRuler());
        }
    }
    else if( 0 != (pPPView = PTR_CAST( SwPagePreView, SfxViewShell::Current())) )
        rToFill = *(pPPView->GetViewShell()->GetViewOptions());
    else
        rToFill = *((SwModule*)this)->GetUsrPref( bWeb );
}


/*------------------------------------------------------------------------
 Beschreibung:	Neuer Master fuer die Einstellungen wird gesetzt;
                dieser wirkt sich auf die aktuelle Sicht und alle
                folgenden aus.
------------------------------------------------------------------------*/

void SwModule::ApplyUsrPref(const SwViewOption &rUsrPref, SwView* pActView,
                            sal_uInt16 nDest )
{
    SwView* pView = pActView;
    ViewShell* pSh = pView ? &pView->GetWrtShell() : 0;

    SwMasterUsrPref* pPref = (SwMasterUsrPref*)GetUsrPref(
                                         VIEWOPT_DEST_WEB == nDest ? sal_True  :
                                         VIEWOPT_DEST_TEXT== nDest ? sal_False :
                                         pView && pView->ISA(SwWebView) );

    //per Uno soll nur die sdbcx::View, aber nicht das Module veraendert werden
    sal_Bool bViewOnly = VIEWOPT_DEST_VIEW_ONLY == nDest;
    //PreView abfruehstuecken
    SwPagePreView* pPPView;
    if( !pView && 0 != (pPPView = PTR_CAST( SwPagePreView, SfxViewShell::Current())) )
    {
        if(!bViewOnly)
            pPref->SetUIOptions( rUsrPref );
        pPPView->ShowVScrollbar(pPref->IsViewVScrollBar());
        pPPView->ShowHScrollbar(pPref->IsViewHScrollBar());
        if(!bViewOnly)
        {
            pPref->SetPagePrevRow(rUsrPref.GetPagePrevRow());
            pPref->SetPagePrevCol(rUsrPref.GetPagePrevCol());
        }
        return;
    }

    if(!bViewOnly)
    {
        pPref->SetUsrPref( rUsrPref );
        pPref->SetModified();
    }

    if( !pView )
        return;

    // Weitergabe an die CORE
    const sal_Bool bReadonly = pView->GetDocShell()->IsReadOnly();
    SwViewOption* pViewOpt;
    if(!bViewOnly)
        pViewOpt = new SwViewOption( *pPref );
    else
        pViewOpt = new SwViewOption( rUsrPref );
    pViewOpt->SetReadonly( bReadonly );
    if( !(*pSh->GetViewOptions() == *pViewOpt) )
    {
        //Ist evtl. nur eine ViewShell
        pSh->StartAction();
        pSh->ApplyViewOptions( *pViewOpt );
        ((SwWrtShell*)pSh)->SetReadOnlyAvailable(pViewOpt->IsCursorInProtectedArea());
        pSh->EndAction();
    }
    if ( pSh->GetViewOptions()->IsReadonly() != bReadonly )
        pSh->SetReadonlyOption(bReadonly);

    lcl_SetUIPrefs(pViewOpt, pView, pSh);
    // zum Schluss wird das Idle-Flag wieder gesetzt
    // #42510#
    pPref->SetIdle(sal_True);

}
/* -----------------------------28.09.00 12:36--------------------------------

 ---------------------------------------------------------------------------*/
void SwModule::ApplyUserMetric( FieldUnit eMetric, BOOL bWeb )
{
        SwMasterUsrPref* pPref;
        if(bWeb)
        {
            if(!pWebUsrPref)
                GetUsrPref(sal_True);
            pPref = pWebUsrPref;
        }
        else
        {
            if(!pUsrPref)
                GetUsrPref(sal_False);
            pPref = pUsrPref;
        }
        FieldUnit eOldMetric = pPref->GetMetric();
        if(eOldMetric != eMetric)
            pPref->SetMetric(eMetric);

        FieldUnit eHScrollMetric = pPref->IsHScrollMetric() ? pPref->GetHScrollMetric() : eMetric;
        FieldUnit eVScrollMetric = pPref->IsVScrollMetric() ? pPref->GetVScrollMetric() : eMetric;

        SwView* pTmpView = SwModule::GetFirstView();
        // fuer alle MDI-Fenster das Lineal umschalten
        while(pTmpView)
        {
            if(bWeb == (0 != PTR_CAST(SwWebView, pTmpView)))
            {
                pTmpView->ChangeVLinealMetric(eHScrollMetric);
                pTmpView->ChangeTabMetric(eVScrollMetric);
            }

            pTmpView = SwModule::GetNextView(pTmpView);
        }

}
/*-----------------13.11.96 11.57-------------------

--------------------------------------------------*/

SwNavigationConfig*  SwModule::GetNavigationConfig()
{
    if(!pNavigationConfig)
    {
        pNavigationConfig = new SwNavigationConfig;
    }
    return pNavigationConfig;
}

/*-----------------05.02.97 08.03-------------------

--------------------------------------------------*/

SwPrintOptions* 	SwModule::GetPrtOptions(sal_Bool bWeb)
{
    if(bWeb && !pWebPrtOpt)
    {
        pWebPrtOpt = new SwPrintOptions(TRUE);
    }
    else if(!bWeb && !pPrtOpt)
    {
        pPrtOpt = new SwPrintOptions(FALSE);
    }

    return bWeb ? pWebPrtOpt : pPrtOpt;
}

/*-----------------26.06.97 07.52-------------------

--------------------------------------------------*/
SwChapterNumRules*	SwModule::GetChapterNumRules()
{
    if(!pChapterNumRules)
        pChapterNumRules = new SwChapterNumRules;
    return pChapterNumRules;
}


/*--------------------------------------------------------------------
    Beschreibung: Schaut nach ob's min eine View gibt
 --------------------------------------------------------------------*/

void SwModule::StateIsView(SfxItemSet& rSet)
{
    SwView *pView = ::GetActiveView();

    TypeId aType( TYPE(SwView) );
    if( !SfxViewShell::GetFirst(&aType) )	// Ist irgendein Writer-Dok vorhanden?
        rSet.DisableItem(FN_QRY_MERGE);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwModule::ShowDBObj(SwView& rView, const SwDBData& rData, BOOL bOnlyIfAvailable)
{
    Reference<XFrame> xFrame = rView.GetViewFrame()->GetFrame()->GetFrameInterface();
    Reference<XDispatchProvider> xDP(xFrame, uno::UNO_QUERY);

    uno::Reference<frame::XFrame> xBeamerFrame = xFrame->findFrame(
                                        rtl::OUString::createFromAscii("_beamer"),
                                        FrameSearchFlag::CHILDREN);
    if (xBeamerFrame.is())
    {   // the beamer has been opened by the SfxViewFrame
        Reference<XController> xController = xBeamerFrame->getController();
        Reference<XSelectionSupplier> xControllerSelection(xController, UNO_QUERY);
        if (xControllerSelection.is())
        {

            ODataAccessDescriptor aSelection;
            aSelection.setDataSource(rData.sDataSource);
            aSelection[daCommand]       <<= rData.sCommand;
            aSelection[daCommandType]   <<= rData.nCommandType;
            xControllerSelection->select(makeAny(aSelection.createPropertyValueSequence()));
        }
        else
            DBG_ERROR("no selection supplier in the beamer!");
    }
}
/*--------------------------------------------------------------------
    Beschreibung: Redlining
 --------------------------------------------------------------------*/

sal_uInt16 SwModule::GetRedlineAuthor()
{
    if (!bAuthorInitialised)
    {
        const SvtUserOptions& rOpt = GetUserOptions();
        if( !(sActAuthor = rOpt.GetFullName()).Len() )
            sActAuthor = String( SW_RES( STR_REDLINE_UNKNOWN_AUTHOR ));
        bAuthorInitialised = sal_True;
    }
    return InsertRedlineAuthor( sActAuthor );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

const String& SwModule::GetRedlineAuthor(sal_uInt16 nPos)
{
    return *pAuthorNames->GetObject(nPos);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_uInt16 SwModule::GetRedlineAuthorCount()
{
    return pAuthorNames->Count();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_uInt16 SwModule::InsertRedlineAuthor(const String& rAuthor)
{
    sal_uInt16 nPos = 0;

    while (nPos < pAuthorNames->Count() && *pAuthorNames->GetObject(nPos) != rAuthor)
        nPos++;

    if (nPos == pAuthorNames->Count())
        pAuthorNames->Insert(new String(rAuthor), nPos);

    return nPos;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void lcl_FillAuthorAttr( sal_uInt16 nAuthor, SfxItemSet &rSet,
                        const AuthorCharAttr &rAttr )
{
    Color aCol( (ColorData)rAttr.nColor );

    if( COL_TRANSPARENT == rAttr.nColor )
    {
        // dynamische Vergabe der Attribute
        static const ColorData aColArr[] = {
                COL_LIGHTRED, 		COL_LIGHTBLUE,		COL_LIGHTMAGENTA,
                COL_GREEN,			COL_RED,			COL_BLUE,
                COL_BROWN,			COL_MAGENTA,		COL_CYAN };
        aCol.SetColor( aColArr[ nAuthor % (sizeof( aColArr ) /
                                           sizeof( aColArr[0] )) ] );
    }

    sal_Bool bBackGr = COL_NONE == rAttr.nColor;

    switch (rAttr.nItemId)
    {
    case SID_ATTR_CHAR_WEIGHT:
        {
            SvxWeightItem aW( (FontWeight)rAttr.nAttr, RES_CHRATR_WEIGHT );
            rSet.Put( aW );
            aW.SetWhich( RES_CHRATR_CJK_WEIGHT );
            rSet.Put( aW );
            aW.SetWhich( RES_CHRATR_CTL_WEIGHT );
            rSet.Put( aW );
        }
        break;

    case SID_ATTR_CHAR_POSTURE:
        {
            SvxPostureItem aP( (FontItalic)rAttr.nAttr, RES_CHRATR_POSTURE );
            rSet.Put( aP );
            aP.SetWhich( RES_CHRATR_CJK_POSTURE );
            rSet.Put( aP );
            aP.SetWhich( RES_CHRATR_CTL_POSTURE );
            rSet.Put( aP );
        }
        break;

    case SID_ATTR_CHAR_UNDERLINE:
        rSet.Put( SvxUnderlineItem( (FontUnderline)rAttr.nAttr,
                                    RES_CHRATR_UNDERLINE));
        break;

    case SID_ATTR_CHAR_STRIKEOUT:
        rSet.Put(SvxCrossedOutItem( (FontStrikeout)rAttr.nAttr,
                                    RES_CHRATR_CROSSEDOUT));
        break;

    case SID_ATTR_CHAR_CASEMAP:
        rSet.Put( SvxCaseMapItem( (SvxCaseMap)rAttr.nAttr,
                                    RES_CHRATR_CASEMAP));
        break;

    case SID_ATTR_BRUSH:
        rSet.Put( SvxBrushItem( aCol, RES_CHRATR_BACKGROUND ));
        bBackGr = sal_True;
        break;
    }

    if( !bBackGr )
        rSet.Put( SvxColorItem( aCol ) );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwModule::GetInsertAuthorAttr(sal_uInt16 nAuthor, SfxItemSet &rSet)
{
    lcl_FillAuthorAttr(nAuthor, rSet, pModuleConfig->GetInsertAuthorAttr());
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwModule::GetDeletedAuthorAttr(sal_uInt16 nAuthor, SfxItemSet &rSet)
{
    lcl_FillAuthorAttr(nAuthor, rSet, pModuleConfig->GetDeletedAuthorAttr());
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

const AuthorCharAttr& SwModule::GetInsertAuthorAttr() const
{
    return pModuleConfig->GetInsertAuthorAttr();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

const AuthorCharAttr& SwModule::GetDeletedAuthorAttr() const
{
    return pModuleConfig->GetDeletedAuthorAttr();
}

/*--------------------------------------------------------------------
    Beschreibung: Fuer zukuenftige Erweiterung:
 --------------------------------------------------------------------*/

void SwModule::GetFormatAuthorAttr( sal_uInt16 nAuthor, SfxItemSet &rSet )
{
    lcl_FillAuthorAttr( nAuthor, rSet, pModuleConfig->GetFormatAuthorAttr() );
}

const AuthorCharAttr& SwModule::GetFormatAuthorAttr() const
{
    return pModuleConfig->GetFormatAuthorAttr();
}


/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_uInt16 SwModule::GetRedlineMarkPos()
{
    return pModuleConfig->GetMarkAlignMode();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Bool SwModule::IsInsTblFormatNum(sal_Bool bHTML) const
{
    return pModuleConfig->IsInsTblFormatNum(bHTML);
}

sal_Bool SwModule::IsInsTblChangeNumFormat(sal_Bool bHTML) const
{
    return pModuleConfig->IsInsTblChangeNumFormat(bHTML);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Bool SwModule::IsInsTblAlignNum(sal_Bool bHTML) const
{
    return pModuleConfig->IsInsTblAlignNum(bHTML);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

const Color &SwModule::GetRedlineMarkColor()
{
    return pModuleConfig->GetMarkAlignColor();
}

/*-----------------03.03.98 16:47-------------------

--------------------------------------------------*/
const SwViewOption*	SwModule::GetViewOption(sal_Bool bWeb)
{
    return GetUsrPref( bWeb );
}

// returne den definierten DocStat - WordDelimiter
const String& SwModule::GetDocStatWordDelim() const
{
    return pModuleConfig->GetWordDelimiter();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
// Durchreichen der Metric von der ModuleConfig (fuer HTML-Export)
sal_uInt16 SwModule::GetMetric( sal_Bool bWeb ) const
{
    SwMasterUsrPref* pPref;
    if(bWeb)
    {
        if(!pWebUsrPref)
            GetUsrPref(sal_True);
        pPref = pWebUsrPref;
    }
    else
    {
        if(!pUsrPref)
            GetUsrPref(sal_False);
        pPref = pUsrPref;
    }
    return pPref->GetMetric();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
// Update-Stati durchreichen
sal_uInt16 SwModule::GetLinkUpdMode( sal_Bool ) const
{
    if(!pUsrPref)
        GetUsrPref(sal_False);
    return (sal_uInt16)pUsrPref->GetUpdateLinkMode();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
sal_uInt16 SwModule::GetFldUpdateFlags( sal_Bool ) const
{
    if(!pUsrPref)
        GetUsrPref(sal_False);
    return (sal_uInt16)pUsrPref->GetFldUpdateFlags();
}
/* -----------------------------28.09.00 14:18--------------------------------

 ---------------------------------------------------------------------------*/
void SwModule::ApplyFldUpdateFlags(sal_Int32 nFldFlags)
{
    if(!pUsrPref)
        GetUsrPref(sal_False);
    pUsrPref->SetFldUpdateFlags(nFldFlags);
}
/* -----------------------------28.09.00 14:18--------------------------------

 ---------------------------------------------------------------------------*/
void SwModule::ApplyLinkMode(sal_Int32 nNewLinkMode)
{
    if(!pUsrPref)
        GetUsrPref(sal_False);
    pUsrPref->SetUpdateLinkMode(nNewLinkMode);
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwModule::CheckSpellChanges( sal_Bool bOnlineSpelling,
        sal_Bool bIsSpellWrongAgain, sal_Bool bIsSpellAllAgain )
{
    sal_Bool bOnlyWrong = bIsSpellWrongAgain && !bIsSpellAllAgain;
    sal_Bool bInvalid = bOnlyWrong || bIsSpellAllAgain;
    if( bOnlineSpelling || bInvalid )
    {
        TypeId aType = TYPE(SwDocShell);
        for( SwDocShell *pDocSh = (SwDocShell*)SfxObjectShell::GetFirst(&aType);
             pDocSh;
             pDocSh = (SwDocShell*)SfxObjectShell::GetNext( *pDocSh, &aType ) )
        {
            SwDoc* pTmp = pDocSh->GetDoc();
            if ( pTmp->GetRootFrm() )
                pTmp->SpellItAgainSam( bInvalid, bOnlyWrong );
        }
//		pSpell->SetSpellWrongAgain( sal_False );
//		pSpell->SetSpellAllAgain( sal_False );
    }
}

