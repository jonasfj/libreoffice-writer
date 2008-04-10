/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swuiccoll.cxx,v $
 * $Revision: 1.10 $
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



#include "cmdid.h"
#include <tools/list.hxx>
#include "swmodule.hxx"
#include "view.hxx"
#include "wrtsh.hxx"
#include "globals.hrc"
#include "helpid.h"


#include <sfx2/styfitem.hxx>

#include "uitool.hxx"
#include "ccoll.hxx"
#include "fmtcol.hxx"
#include "hintids.hxx"
#include "docsh.hxx"
#include "docstyle.hxx"
#include "hints.hxx"

#include "chrdlg.hrc"
#include "ccoll.hrc"
#include <vcl/svapp.hxx>

#include "swuiccoll.hxx"

static USHORT __FAR_DATA aPageRg[] = {
    FN_COND_COLL, FN_COND_COLL,
    0
};

// Achtung im Code wird dieses Array direkt (0, 1, ...) indiziert
static long nTabs[] =
    {	2, // Number of Tabs
        0, 100
    };

/****************************************************************************
Page: Ctor
****************************************************************************/


SwCondCollPage::SwCondCollPage(Window *pParent, const SfxItemSet &rSet)

    : SfxTabPage(pParent, SW_RES(TP_CONDCOLL), rSet),
    aConditionFL( this, SW_RES( FL_CONDITION )),
    aConditionCB( this, SW_RES( CB_CONDITION ) ),
    aContextFT  ( this, SW_RES( FT_CONTEXT 	) ),
    aUsedFT     ( this, SW_RES( FT_USED 		) ),
    aTbLinks( 	  this, SW_RES( TB_CONDCOLLS ) ),
    aStyleFT    ( this, SW_RES( FT_STYLE 	) ),
    aStyleLB    ( this, SW_RES( LB_STYLE 	) ),
    aFilterLB   ( this, SW_RES( LB_FILTER 	) ),
    aRemovePB   ( this, SW_RES( PB_REMOVE 	) ),
    aAssignPB   ( this, SW_RES( PB_ASSIGN 	) ),
    sNoTmpl		(		SW_RES( STR_NOTEMPL  ) ),
    aStrArr 	( 		SW_RES( STR_REGIONS  ) ),
    rSh(::GetActiveView()->GetWrtShell()),
    pCmds( SwCondCollItem::GetCmds() ),
    pFmt(0),

    bNewTemplate(FALSE)
{
    FreeResource();
    SetExchangeSupport();

    // Handler installieren
    aConditionCB.SetClickHdl(	LINK(this, SwCondCollPage, OnOffHdl));
    aTbLinks.SetDoubleClickHdl( LINK(this, SwCondCollPage, AssignRemoveHdl ));
    aStyleLB.SetDoubleClickHdl( LINK(this, SwCondCollPage, AssignRemoveHdl ));
    aRemovePB.SetClickHdl(		LINK(this, SwCondCollPage, AssignRemoveHdl ));
    aAssignPB.SetClickHdl(		LINK(this, SwCondCollPage, AssignRemoveHdl ));
    aTbLinks.SetSelectHdl(		LINK(this, SwCondCollPage, SelectHdl));
    aStyleLB.SetSelectHdl(		LINK(this, SwCondCollPage, SelectHdl));
    aFilterLB.SetSelectHdl(		LINK(this, SwCondCollPage, SelectHdl));

    aTbLinks.SetWindowBits(WB_HSCROLL|WB_CLIPCHILDREN);
    aTbLinks.SetSelectionMode( SINGLE_SELECTION );
    aTbLinks.SetTabs( &nTabs[0], MAP_APPFONT );
    aTbLinks.Resize();	// OS: Hack fuer richtige Selektion
    aTbLinks.SetSpaceBetweenEntries( 0 );
    aTbLinks.SetHelpId(HID_COND_COLL_TABLIST);

    SfxStyleFamilies aFamilies(SW_RES(DLG_STYLE_DESIGNER));
    const SfxStyleFamilyItem* pFamilyItem = 0;
    USHORT nCount = aFamilies.Count();
    USHORT i;

    for( i = 0; i < nCount; ++i)
    {
        if(SFX_STYLE_FAMILY_PARA == (USHORT)(pFamilyItem = aFamilies.GetObject(i))->GetFamily())
            break;
    }

    const SfxStyleFilter& rFilterList = pFamilyItem->GetFilterList();
    for( i = 0; i < rFilterList.Count(); ++i)
    {
        aFilterLB.InsertEntry(rFilterList.GetObject(i)->aName);
        USHORT* pFilter = new USHORT(rFilterList.GetObject(i)->nFlags);
        aFilterLB.SetEntryData(i, pFilter);
    }
    aFilterLB.SelectEntryPos(1);

    aTbLinks.Show();

}

/****************************************************************************
Page: Dtor
****************************************************************************/


__EXPORT SwCondCollPage::~SwCondCollPage()
{
    for(USHORT i = 0; i < aFilterLB.GetEntryCount(); ++i)
        delete (USHORT*)aFilterLB.GetEntryData(i);

}


int __EXPORT SwCondCollPage::DeactivatePage(SfxItemSet * _pSet)
{
    if( _pSet )
        FillItemSet(*_pSet);

    return LEAVE_PAGE;
}

/****************************************************************************
Page: Factory
****************************************************************************/


SfxTabPage* __EXPORT SwCondCollPage::Create(Window *pParent, const SfxItemSet &rSet)
{
    return new SwCondCollPage(pParent, rSet);
}

/****************************************************************************
Page: FillItemSet-Overload
****************************************************************************/


BOOL __EXPORT SwCondCollPage::FillItemSet(SfxItemSet &rSet)
{
    BOOL bModified = TRUE;
    SwCondCollItem aCondItem;
    for(USHORT i = 0; i < aStrArr.Count(); i++)
    {
        String sEntry = aTbLinks.GetEntryText(i, 1);
        aCondItem.SetStyle( &sEntry, i);
    }
    rSet.Put(aCondItem);
    return bModified;
}

/****************************************************************************
Page: Reset-Overload
****************************************************************************/


void __EXPORT SwCondCollPage::Reset(const SfxItemSet &/*rSet*/)
{
    if(bNewTemplate)
        aConditionCB.Enable();
    if(RES_CONDTXTFMTCOLL == pFmt->Which())
        aConditionCB.Check();
    OnOffHdl(&aConditionCB);

    aTbLinks.Clear();

    SfxStyleSheetBasePool* pPool = rSh.GetView().GetDocShell()->GetStyleSheetPool();
    pPool->SetSearchMask(SFX_STYLE_FAMILY_PARA, SFXSTYLEBIT_ALL);
    aStyleLB.Clear();
    const SfxStyleSheetBase* pBase = pPool->First();
    while( pBase )
    {
        if(!pFmt || pBase->GetName() != pFmt->GetName())
            aStyleLB.InsertEntry(pBase->GetName());
        pBase = pPool->Next();
    }
    aStyleLB.SelectEntryPos(0);

    for( USHORT n = 0; n < aStrArr.Count(); n++)
    {
        String aEntry( aStrArr.GetString(n) );
        aEntry += '\t';

        const SwCollCondition* pCond = 0;
        if( pFmt && RES_CONDTXTFMTCOLL == pFmt->Which() &&
            0 != ( pCond = ((SwConditionTxtFmtColl*)pFmt)->
            HasCondition( SwCollCondition( 0, pCmds[n].nCnd, pCmds[n].nSubCond ) ) )
            && pCond->GetTxtFmtColl() )
        {
            aEntry += pCond->GetTxtFmtColl()->GetName();
        }

        SvLBoxEntry* pE = aTbLinks.InsertEntryToColumn( aEntry, n );
        if(0 == n)
            aTbLinks.Select(pE);
    }

}

/****************************************************************************

****************************************************************************/


USHORT* __EXPORT SwCondCollPage::GetRanges()
{
    return aPageRg;
}


/****************************************************************************

****************************************************************************/


IMPL_LINK( SwCondCollPage, OnOffHdl, CheckBox*, pBox )
{
    const BOOL bEnable = pBox->IsChecked();
    aContextFT.Enable( bEnable );
    aUsedFT   .Enable( bEnable );
    aTbLinks  .EnableList( bEnable != FALSE );
    aStyleFT  .Enable( bEnable );
    aStyleLB  .Enable( bEnable );
    aFilterLB .Enable( bEnable );
    aRemovePB .Enable( bEnable );
    aAssignPB .Enable( bEnable );
    if( bEnable )
        SelectHdl(0);
    return 0;
}

/****************************************************************************

****************************************************************************/


IMPL_LINK( SwCondCollPage, AssignRemoveHdl, PushButton*, pBtn)
{
    SvLBoxEntry* pE = aTbLinks.FirstSelected();
    ULONG nPos;
    if( !pE || LISTBOX_ENTRY_NOTFOUND ==
        ( nPos = aTbLinks.GetModel()->GetAbsPos( pE ) ) )
    {
        ASSERT( pE, "wo kommt der leere Eintrag her?" );
        return 0;
    }

    String sSel = aStrArr.GetString( USHORT(nPos) );
    sSel += '\t';

    const BOOL bAssEnabled = pBtn != &aRemovePB && aAssignPB.IsEnabled();
    aAssignPB.Enable( !bAssEnabled );
    aRemovePB.Enable(  bAssEnabled );
    if ( bAssEnabled )
        sSel += aStyleLB.GetSelectEntry();

    aTbLinks.SetUpdateMode(FALSE);
    aTbLinks.GetModel()->Remove(pE);
    pE = aTbLinks.InsertEntryToColumn(sSel, nPos);
    aTbLinks.Select(pE);
    aTbLinks.MakeVisible(pE);
    aTbLinks.SetUpdateMode(TRUE);
    return 0;
}

/****************************************************************************

****************************************************************************/


IMPL_LINK( SwCondCollPage, SelectHdl, ListBox*, pBox)
{
    if(pBox == &aFilterLB)
    {
        aStyleLB.Clear();
        USHORT nSearchFlags = pBox->GetSelectEntryPos();
        nSearchFlags = *(USHORT*)aFilterLB.GetEntryData(nSearchFlags);
        SfxStyleSheetBasePool* pPool = rSh.GetView().GetDocShell()->GetStyleSheetPool();
        pPool->SetSearchMask(SFX_STYLE_FAMILY_PARA, nSearchFlags);
        const SfxStyleSheetBase* pBase = pPool->First();
        while( pBase )
        {
            if(!pFmt || pBase->GetName() != pFmt->GetName())
                aStyleLB.InsertEntry(pBase->GetName());
            pBase = pPool->Next();
        }
        aStyleLB.SelectEntryPos(0);
        SelectHdl(&aStyleLB);

    }
    else
    {
        String sTbEntry;
        SvLBoxEntry* pE = aTbLinks.FirstSelected();
        if(pE)
            sTbEntry = aTbLinks.GetEntryText(pE);
        sTbEntry = sTbEntry.GetToken(1, '\t');
        String sStyle = aStyleLB.GetSelectEntry();

        aAssignPB.Enable( sStyle != sTbEntry && aConditionCB.IsChecked() );

        if(pBox != &aStyleLB)
            aRemovePB.Enable( aConditionCB.IsChecked() && sTbEntry.Len() );
    }
    return 0;
}

/****************************************************************************

****************************************************************************/


void SwCondCollPage::SetCollection( SwFmt* pFormat, BOOL bNew )
{
    pFmt = pFormat;
    bNewTemplate = bNew;
}
