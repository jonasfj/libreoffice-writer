/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: pardlg.cxx,v $
 * $Revision: 1.15 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


#include "hintids.hxx"
#include <svx/htmlmode.hxx>
#include <svl/style.hxx>

#include <svx/htmlcfg.hxx>

#ifndef _SVSTDARR_STRINGSSORTDTOR
#define _SVSTDARR_STRINGSSORTDTOR
#include <svl/svstdarr.hxx>
#endif
#include <svl/cjkoptions.hxx>
#include "docsh.hxx"
#include "wrtsh.hxx"
#include "frmatr.hxx"
#include "view.hxx"
#include "globals.hrc"
#include "swuipardlg.hxx"
#include "pagedesc.hxx"
#include "paratr.hxx"
#include "drpcps.hxx"
#include "uitool.hxx"
#include "viewopt.hxx"
#include <numpara.hxx>
#include "chrdlg.hrc"
#include "poolfmt.hrc"
#include <svx/svxids.hrc>
#include <svl/eitem.hxx>
#include <svl/intitem.hxx>
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
#include <svx/flagsdef.hxx>
// STATIC DATA -----------------------------------------------------------


SwParaDlg::SwParaDlg(Window *pParent,
                    SwView& rVw,
                    const SfxItemSet& rCoreSet,
                    BYTE nDialogMode,
                    const String *pTitle,
                    BOOL bDraw,
                    UINT16 nDefPage):

    SfxTabDialog(pParent, bDraw ? SW_RES(DLG_DRAWPARA) : SW_RES(DLG_PARA),
                    &rCoreSet,  0 != pTitle),

    rView(rVw),
    nDlgMode(nDialogMode),
    bDrawParaDlg(bDraw)

{
    FreeResource();

    nHtmlMode = ::GetHtmlMode(rVw.GetDocShell());
    BOOL bHtmlMode = static_cast< BOOL >(nHtmlMode & HTMLMODE_ON);
    if(pTitle)
    {
            // Update des Titels
        String aTmp( GetText() );
        aTmp += SW_RESSTR(STR_TEXTCOLL_HEADER);
        aTmp += *pTitle;
        aTmp += ')';
        SetText(aTmp);
    }
    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();

    DBG_ASSERT(pFact->GetTabPageCreatorFunc(RID_SVXPAGE_STD_PARAGRAPH), "GetTabPageCreatorFunc fail!");
    DBG_ASSERT(pFact->GetTabPageRangesFunc(RID_SVXPAGE_STD_PARAGRAPH), "GetTabPageRangesFunc fail!");
    AddTabPage( TP_PARA_STD,	pFact->GetTabPageCreatorFunc(RID_SVXPAGE_STD_PARAGRAPH),		pFact->GetTabPageRangesFunc(RID_SVXPAGE_STD_PARAGRAPH) );

    DBG_ASSERT(pFact->GetTabPageCreatorFunc(RID_SVXPAGE_ALIGN_PARAGRAPH), "GetTabPageCreatorFunc fail!");
    DBG_ASSERT(pFact->GetTabPageRangesFunc(RID_SVXPAGE_ALIGN_PARAGRAPH), "GetTabPageRangesFunc fail!");
    AddTabPage( TP_PARA_ALIGN,	pFact->GetTabPageCreatorFunc(RID_SVXPAGE_ALIGN_PARAGRAPH),		pFact->GetTabPageRangesFunc(RID_SVXPAGE_ALIGN_PARAGRAPH) );

    SvxHtmlOptions* pHtmlOpt = SvxHtmlOptions::Get();
    if (!bDrawParaDlg && (!bHtmlMode || pHtmlOpt->IsPrintLayoutExtension()))
    {
        DBG_ASSERT(pFact->GetTabPageCreatorFunc(RID_SVXPAGE_EXT_PARAGRAPH), "GetTabPageCreatorFunc fail!");
        DBG_ASSERT(pFact->GetTabPageRangesFunc(RID_SVXPAGE_EXT_PARAGRAPH), "GetTabPageRangesFunc fail!");
        AddTabPage( TP_PARA_EXT,	pFact->GetTabPageCreatorFunc(RID_SVXPAGE_EXT_PARAGRAPH),		pFact->GetTabPageRangesFunc(RID_SVXPAGE_EXT_PARAGRAPH) );

    }
    else
        RemoveTabPage(TP_PARA_EXT);

    SvtCJKOptions aCJKOptions;
    if(!bHtmlMode && aCJKOptions.IsAsianTypographyEnabled())
    {
        DBG_ASSERT(pFact->GetTabPageCreatorFunc(RID_SVXPAGE_PARA_ASIAN), "GetTabPageCreatorFunc fail!");
        DBG_ASSERT(pFact->GetTabPageRangesFunc(RID_SVXPAGE_PARA_ASIAN), "GetTabPageRangesFunc fail!");
        AddTabPage( TP_PARA_ASIAN,	pFact->GetTabPageCreatorFunc(RID_SVXPAGE_PARA_ASIAN),		pFact->GetTabPageRangesFunc(RID_SVXPAGE_PARA_ASIAN) );
    }
    else
        RemoveTabPage(TP_PARA_ASIAN);

    USHORT nWhich(rCoreSet.GetPool()->GetWhich(SID_ATTR_LRSPACE));
    BOOL bLRValid = SFX_ITEM_AVAILABLE <= rCoreSet.GetItemState(nWhich);
    if(bHtmlMode || !bLRValid)
        RemoveTabPage(TP_TABULATOR);
    else
    {
        DBG_ASSERT(pFact->GetTabPageCreatorFunc(RID_SVXPAGE_TABULATOR), "GetTabPageCreatorFunc fail!");
        DBG_ASSERT(pFact->GetTabPageRangesFunc(RID_SVXPAGE_TABULATOR), "GetTabPageRangesFunc fail!");
        AddTabPage( TP_TABULATOR,	pFact->GetTabPageCreatorFunc(RID_SVXPAGE_TABULATOR),		pFact->GetTabPageRangesFunc(RID_SVXPAGE_TABULATOR) );

    }
    if (!bDrawParaDlg)
    {
        if(!(nDlgMode & DLG_ENVELOP))
            AddTabPage(TP_NUMPARA,   SwParagraphNumTabPage::Create,SwParagraphNumTabPage::GetRanges);
        else
            RemoveTabPage(TP_NUMPARA);
        if(!bHtmlMode || (nHtmlMode & HTMLMODE_FULL_STYLES))
        {
            AddTabPage(TP_DROPCAPS,  SwDropCapsPage::Create, 		SwDropCapsPage::GetRanges);
        }
        else
        {
            RemoveTabPage(TP_DROPCAPS);
        }
        if(!bHtmlMode || (nHtmlMode & (HTMLMODE_SOME_STYLES|HTMLMODE_FULL_STYLES)))
        {
            DBG_ASSERT(pFact->GetTabPageCreatorFunc( RID_SVXPAGE_BACKGROUND ), "GetTabPageCreatorFunc fail!");
            DBG_ASSERT(pFact->GetTabPageRangesFunc( RID_SVXPAGE_BACKGROUND ), "GetTabPageRangesFunc fail!");
            AddTabPage(TP_BACKGROUND, pFact->GetTabPageCreatorFunc( RID_SVXPAGE_BACKGROUND ), pFact->GetTabPageRangesFunc( RID_SVXPAGE_BACKGROUND ) );
        }
        else
        {
            RemoveTabPage(TP_BACKGROUND);
        }
        if(!bHtmlMode || (nHtmlMode & HTMLMODE_PARA_BORDER))
        {
            DBG_ASSERT(pFact->GetTabPageCreatorFunc( RID_SVXPAGE_BORDER ), "GetTabPageCreatorFunc fail!");
            DBG_ASSERT(pFact->GetTabPageRangesFunc( RID_SVXPAGE_BORDER ), "GetTabPageRangesFunc fail!");
            AddTabPage(TP_BORDER, pFact->GetTabPageCreatorFunc( RID_SVXPAGE_BORDER ), pFact->GetTabPageRangesFunc( RID_SVXPAGE_BORDER ) );
        }
        else
            RemoveTabPage(TP_BORDER);
    }

    if (nDefPage)
        SetCurPageId(nDefPage);
}


__EXPORT SwParaDlg::~SwParaDlg()
{
}


void __EXPORT SwParaDlg::PageCreated(USHORT nId, SfxTabPage& rPage)
{
    SwWrtShell& rSh = rView.GetWrtShell();
    SfxAllItemSet aSet(*(GetInputSetImpl()->GetPool()));

    // Bei Tabellenumrandung kann im Writer kein Schatten eingestellt werden
    if (nId == TP_BORDER)
    {
        aSet.Put (SfxUInt16Item(SID_SWMODE_TYPE,SW_BORDER_MODE_PARA));
        rPage.PageCreated(aSet);
    }
    else if( nId == TP_PARA_STD )
    {
        aSet.Put(SfxUInt16Item(SID_SVXSTDPARAGRAPHTABPAGE_PAGEWIDTH,
                            static_cast< UINT16 >(rSh.GetAnyCurRect(RECT_PAGE_PRT).Width()) ));

        if (!bDrawParaDlg)
        {
            aSet.Put(SfxUInt32Item(SID_SVXSTDPARAGRAPHTABPAGE_FLAGSET,0x000E));
            aSet.Put(SfxUInt32Item(SID_SVXSTDPARAGRAPHTABPAGE_ABSLINEDIST, MM50/10));

        }
        rPage.PageCreated(aSet);
    }
    else if( TP_PARA_ALIGN == nId)
    {
        if (!bDrawParaDlg)
        {
            aSet.Put(SfxBoolItem(SID_SVXPARAALIGNTABPAGE_ENABLEJUSTIFYEXT,TRUE));
            rPage.PageCreated(aSet);
        }
    }
    else if( TP_PARA_EXT == nId )
    {
        // Seitenumbruch nur, wenn der Cursor im Body-Bereich und nicht in
        // einer Tabelle steht
        const USHORT eType = rSh.GetFrmType(0,TRUE);
        if( !(FRMTYPE_BODY & eType) ||
            rSh.GetSelectionType() & nsSelectionType::SEL_TBL )
        {
            aSet.Put(SfxBoolItem(SID_DISABLE_SVXEXTPARAGRAPHTABPAGE_PAGEBREAK,TRUE));
            rPage.PageCreated(aSet);
        }
    }
    else if( TP_DROPCAPS == nId )
    {
        ((SwDropCapsPage&)rPage).SetFormat(FALSE);
    }
    else if( TP_BACKGROUND == nId )
    {
      if(!( nHtmlMode & HTMLMODE_ON ) ||
        nHtmlMode & HTMLMODE_SOME_STYLES)
        {
            aSet.Put (SfxUInt32Item(SID_FLAG_TYPE, SVX_SHOW_SELECTOR));
            rPage.PageCreated(aSet);
        }
    }
    else if( TP_NUMPARA == nId)
    {
        //-->#outline level,added by zhaojianwei
        SwTxtFmtColl* pTmpColl = rSh.GetCurTxtFmtColl();
        if( pTmpColl && pTmpColl->IsAssignedToListLevelOfOutlineStyle() )
        {
            ((SwParagraphNumTabPage&)rPage).DisableOutline() ;
        }//<-end

        ((SwParagraphNumTabPage&)rPage).EnableNewStart();
        ListBox & rBox = ((SwParagraphNumTabPage&)rPage).GetStyleBox();
        SfxStyleSheetBasePool* pPool = rView.GetDocShell()->GetStyleSheetPool();
        pPool->SetSearchMask(SFX_STYLE_FAMILY_PSEUDO, SFXSTYLEBIT_ALL);
        const SfxStyleSheetBase* pBase = pPool->First();
        SvStringsSortDtor aNames;
        while(pBase)
        {
            aNames.Insert(new String(pBase->GetName()));
            pBase = pPool->Next();
        }
        for(USHORT i = 0; i < aNames.Count(); i++)
            rBox.InsertEntry(*aNames.GetObject(i));
    }

}



