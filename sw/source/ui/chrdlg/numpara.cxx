/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "swtypes.hxx"
#include "hintids.hxx"
#include "globals.hrc"
#include "helpid.h"
#include <sfx2/objsh.hxx>
#include <svx/htmlmode.hxx>
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#include <svl/eitem.hxx>
#include <svl/stritem.hxx>
#include <svl/intitem.hxx>
#include <fmtline.hxx>
#include <numpara.hxx>
#include <numpara.hrc>
// --> OD 2008-04-14 #outlinelevel#
#include <app.hrc>
// <--

// Globals ******************************************************************

static USHORT __FAR_DATA aPageRg[] = {
    FN_NUMBER_NEWSTART, FN_NUMBER_NEWSTART_AT,
    0
};
/*-----------------31.01.98 08:34-------------------

--------------------------------------------------*/
SwParagraphNumTabPage::SwParagraphNumTabPage(Window* pParent,
                                                const SfxItemSet& rAttr ) :
    SfxTabPage(pParent, SW_RES(TP_NUMPARA), rAttr),
    aOutlineStartFL	        ( this, SW_RES( FL_OUTLINE_START )),
    aOutlineLvFT		    ( this, SW_RES( FT_OUTLINE_LEVEL )),
    aOutlineLvLB		    ( this, SW_RES( LB_OUTLINE_LEVEL )),
    aNumberStyleFT          ( this, SW_RES( FT_NUMBER_STYLE ) ),
    aNumberStyleLB          ( this, SW_RES( LB_NUMBER_STYLE ) ),
    aNewStartFL             ( this, SW_RES( FL_NEW_START ) ),
    aNewStartCB          	( this, SW_RES( CB_NEW_START ) ),
    aNewStartNumberCB       ( this, SW_RES( CB_NUMBER_NEW_START ) ),
    aNewStartNF          	( this, SW_RES( NF_NEW_START ) ),
    aCountParaFL            ( this, SW_RES( FL_COUNT_PARA        ) ),
    aCountParaCB            ( this, SW_RES( CB_COUNT_PARA        ) ),
    aRestartParaCountCB     ( this, SW_RES( CB_RESTART_PARACOUNT ) ),
    aRestartFT              ( this, SW_RES( FT_RESTART_NO        ) ),
    aRestartNF              ( this, SW_RES( NF_RESTART_PARA      ) ),
    // --> OD 2008-04-14 #outlinelevel#
    msOutlineNumbering( SW_RES( STR_OUTLINE_NUMBERING ) ),
    // <--
    bModified(FALSE),
    bCurNumrule(FALSE)
{
    FreeResource();

    const SfxPoolItem* pItem;
    SfxObjectShell* pObjSh;
    if(SFX_ITEM_SET == rAttr.GetItemState(SID_HTML_MODE, FALSE, &pItem) ||
        ( 0 != ( pObjSh = SfxObjectShell::Current()) &&
                    0 != (pItem = pObjSh->GetItem(SID_HTML_MODE))))
    {
        USHORT nHtmlMode = ((const SfxUInt16Item*)pItem)->GetValue();
        if(HTMLMODE_ON & nHtmlMode)
        {
            aCountParaFL        .Hide();
            aCountParaCB        .Hide();
            aRestartParaCountCB .Hide();
            aRestartFT          .Hide();
            aRestartNF          .Hide();
        }
    }
    aNewStartCB.SetClickHdl(LINK(this, SwParagraphNumTabPage, NewStartHdl_Impl));
    aNewStartNumberCB.SetClickHdl(LINK(this, SwParagraphNumTabPage, NewStartHdl_Impl));
    aNumberStyleLB.SetSelectHdl(LINK(this, SwParagraphNumTabPage, StyleHdl_Impl));
    aCountParaCB.SetClickHdl(LINK(this,
                    SwParagraphNumTabPage, LineCountHdl_Impl));
    aRestartParaCountCB.SetClickHdl(
                    LINK(this, SwParagraphNumTabPage, LineCountHdl_Impl));
}
/*-----------------31.01.98 08:34-------------------

--------------------------------------------------*/
SwParagraphNumTabPage::~SwParagraphNumTabPage()
{
}

/*-----------------31.01.98 08:38-------------------

--------------------------------------------------*/
SfxTabPage*	SwParagraphNumTabPage::Create(	Window* pParent,
                                const SfxItemSet& rSet )
{
    return new SwParagraphNumTabPage(pParent, rSet);
}

/*-----------------31.01.98 08:38-------------------

--------------------------------------------------*/
USHORT*	SwParagraphNumTabPage::GetRanges()
{
    return aPageRg;
}

/*-----------------31.01.98 08:38-------------------

--------------------------------------------------*/
BOOL	SwParagraphNumTabPage::FillItemSet( SfxItemSet& rSet )
{
    //<-#outline level, added by zhaojianwei
    if( aOutlineLvLB.GetSelectEntryPos() != aOutlineLvLB.GetSavedValue())
    {
        USHORT aOutlineLv = aOutlineLvLB.GetSelectEntryPos();
        const SfxUInt16Item* pOldOutlineLv = (const SfxUInt16Item*)GetOldItem( rSet, SID_ATTR_PARA_OUTLINE_LEVEL);
        SfxUInt16Item* pOutlineLv = (SfxUInt16Item*)pOldOutlineLv->Clone();
        pOutlineLv->SetValue( aOutlineLv );
        rSet.Put(*pOutlineLv);
        delete pOutlineLv;
        bModified = TRUE;
    }
    //<-end
    if(	aNumberStyleLB.GetSelectEntryPos() != aNumberStyleLB.GetSavedValue())
    {
        String aStyle;
        if(aNumberStyleLB.GetSelectEntryPos())
            aStyle = aNumberStyleLB.GetSelectEntry();
        const SfxStringItem* pOldRule = (const SfxStringItem*)GetOldItem( rSet, SID_ATTR_PARA_NUMRULE);
        SfxStringItem* pRule = (SfxStringItem*)pOldRule->Clone();
        pRule->SetValue(aStyle);
        rSet.Put(*pRule);
        delete pRule;
        bModified = TRUE;
    }
    if(aNewStartCB.GetState() != aNewStartCB.GetSavedValue() ||
        aNewStartNumberCB.GetState() != aNewStartNumberCB.GetSavedValue()||
        aNewStartNF.GetText() != aNewStartNF.GetSavedValue())
    {
        bModified = TRUE;
        BOOL bNewStartChecked = STATE_CHECK == aNewStartCB.GetState();
        BOOL bNumberNewStartChecked = STATE_CHECK == aNewStartNumberCB.GetState();
        rSet.Put(SfxBoolItem(FN_NUMBER_NEWSTART, bNewStartChecked));
        rSet.Put(SfxUInt16Item(FN_NUMBER_NEWSTART_AT,
                  bNumberNewStartChecked && bNewStartChecked ? (USHORT)aNewStartNF.GetValue() : USHRT_MAX));
    }

    if(aCountParaCB.GetSavedValue() != aCountParaCB.GetState() ||
        aRestartParaCountCB.GetSavedValue() != aRestartParaCountCB.GetState() ||
            aRestartNF.GetSavedValue() != aRestartNF.GetText() )
    {
        SwFmtLineNumber aFmt;
        aFmt.SetStartValue( static_cast< ULONG >(aRestartParaCountCB.GetState() == STATE_CHECK ?
                                aRestartNF.GetValue() : 0 ));
        aFmt.SetCountLines( aCountParaCB.IsChecked() );
        rSet.Put(aFmt);
        bModified = TRUE;
    }
    return bModified;
}

/*-----------------31.01.98 08:38-------------------

--------------------------------------------------*/
void	SwParagraphNumTabPage::Reset( const SfxItemSet& rSet )
{
    BOOL bHasNumberStyle = FALSE;

    SfxItemState eItemState = rSet.GetItemState( GetWhich(SID_ATTR_PARA_OUTLINE_LEVEL) );

    sal_Int16 nOutlineLv;
    if( eItemState >= SFX_ITEM_AVAILABLE )
    {
        nOutlineLv = ((const SfxUInt16Item &)rSet.Get( GetWhich(SID_ATTR_PARA_OUTLINE_LEVEL) )).GetValue();
        aOutlineLvLB.SelectEntryPos( nOutlineLv ) ;
    }
    else
    {
        aOutlineLvLB.SetNoSelection();
    }
    aOutlineLvLB.SaveValue();

    eItemState = rSet.GetItemState( GetWhich(SID_ATTR_PARA_NUMRULE) );

    String aStyle;
    if( eItemState >= SFX_ITEM_AVAILABLE )
    {
        aStyle = ((const SfxStringItem &)rSet.Get( GetWhich(SID_ATTR_PARA_NUMRULE) )).GetValue();
        if(!aStyle.Len())
            aStyle = aNumberStyleLB.GetEntry(0);

        if( aStyle.EqualsAscii("Outline"))// == String::CreateFromAscii("Outline")) //maybe need modify,zhaojianwei
        {
            aNumberStyleLB.InsertEntry( msOutlineNumbering );
            aNumberStyleLB.SelectEntry( msOutlineNumbering );
            aNumberStyleLB.RemoveEntry(msOutlineNumbering);
            aNumberStyleLB.SaveValue();
        }
        else
            aNumberStyleLB.SelectEntry( aStyle );

        bHasNumberStyle = TRUE;
    }
    else
    {
        aNumberStyleLB.SetNoSelection();
    }

    aNumberStyleLB.SaveValue();

    eItemState = rSet.GetItemState( FN_NUMBER_NEWSTART );
    if(eItemState > SFX_ITEM_AVAILABLE )
    {
        bCurNumrule = TRUE;
        const SfxBoolItem& rStart = (const SfxBoolItem&)rSet.Get(FN_NUMBER_NEWSTART);
        aNewStartCB.SetState(
            rStart.GetValue() ? 
                        STATE_CHECK : STATE_NOCHECK );
        aNewStartCB.EnableTriState(FALSE);
    }
    else
        aNewStartCB.SetState(bHasNumberStyle ? STATE_NOCHECK : STATE_DONTKNOW);
    aNewStartCB.SaveValue();

    eItemState = rSet.GetItemState( FN_NUMBER_NEWSTART_AT);
    if( eItemState > SFX_ITEM_AVAILABLE )
    {
        USHORT nNewStart = ((const SfxUInt16Item&)rSet.Get(FN_NUMBER_NEWSTART_AT)).GetValue();
        aNewStartNumberCB.Check(USHRT_MAX != nNewStart);
        if(USHRT_MAX == nNewStart)
            nNewStart = 1;

        aNewStartNF.SetValue(nNewStart);
        aNewStartNumberCB.EnableTriState(FALSE);
    }
    else
        aNewStartCB.SetState(STATE_DONTKNOW);
    NewStartHdl_Impl(&aNewStartCB);
    aNewStartNF.SaveValue();
    aNewStartNumberCB.SaveValue();
    StyleHdl_Impl(&aNumberStyleLB);
    if( SFX_ITEM_AVAILABLE <= rSet.GetItemState(RES_LINENUMBER))
    {
        SwFmtLineNumber& rNum = (SwFmtLineNumber&)rSet.Get(RES_LINENUMBER);
        ULONG nStartValue = rNum.GetStartValue();
        BOOL bCount = rNum.IsCount();
        aCountParaCB.SetState( bCount ? STATE_CHECK : STATE_NOCHECK );
        aRestartParaCountCB.SetState( 0 != nStartValue ? STATE_CHECK : STATE_NOCHECK );
        aRestartNF.SetValue(nStartValue == 0 ? 1 : nStartValue);
        LineCountHdl_Impl(&aCountParaCB);
        aCountParaCB.EnableTriState(FALSE);
        aRestartParaCountCB.EnableTriState(FALSE);
    }
    else
    {
        aCountParaCB.SetState(STATE_DONTKNOW);
        aRestartParaCountCB.SetState(STATE_DONTKNOW);
    }
    aCountParaCB.SaveValue();
    aRestartParaCountCB.SaveValue();
    aRestartNF.SaveValue();

    bModified = FALSE;
}

/*-----------------31.01.98 08:38-------------------

--------------------------------------------------*/
void SwParagraphNumTabPage::DisableOutline()
{
    aOutlineLvFT.Disable();
    aOutlineLvLB.Disable();
}
void SwParagraphNumTabPage::DisableNumbering()
{
    aNumberStyleFT.Disable();
    aNumberStyleLB.Disable();
}

/*-----------------31.01.98 08:38-------------------

--------------------------------------------------*/
void SwParagraphNumTabPage::EnableNewStart()
{
    aNewStartCB.Show();
    aNewStartNumberCB.Show();
    aNewStartNF.Show();
}

/*-----------------31.01.98 08:56-------------------

--------------------------------------------------*/
IMPL_LINK( SwParagraphNumTabPage, NewStartHdl_Impl, CheckBox*, EMPTYARG )
{
    BOOL bEnable = aNewStartCB.IsChecked();
    aNewStartNumberCB.Enable(bEnable);
    aNewStartNF.Enable(bEnable && aNewStartNumberCB.IsChecked());
    return 0;
}

/*-----------------05.02.98 10:01-------------------

--------------------------------------------------*/
IMPL_LINK( SwParagraphNumTabPage, LineCountHdl_Impl, CheckBox* , EMPTYARG)
{
    aRestartParaCountCB.Enable(aCountParaCB.IsChecked());

    BOOL bEnableRestartValue = aRestartParaCountCB.IsEnabled() &&
                                                aRestartParaCountCB.IsChecked();
    aRestartFT.Enable(bEnableRestartValue);
    aRestartNF.Enable(bEnableRestartValue);

    return 0;
}

/*-----------------05.02.98 13:39-------------------

--------------------------------------------------*/
IMPL_LINK( SwParagraphNumTabPage, StyleHdl_Impl, ListBox*, pBox )
{
//	String dd=aNumberStyleLB.GetSavedValue();
//	if(	msOutlineNumbering == dd)
    {
    }
    BOOL bEnable = bCurNumrule || pBox->GetSelectEntryPos() > 0;
    aNewStartCB.Enable(bEnable);
    NewStartHdl_Impl(&aNewStartCB);

    return 0;
}


