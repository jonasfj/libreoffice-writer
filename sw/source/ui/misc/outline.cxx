/*************************************************************************
 *
 *  $RCSfile: outline.cxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-17 15:37:35 $
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

#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif
#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif

#ifndef _NUM_HXX //autogen
#define USE_NUMTABPAGES
#include <num.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
#ifndef _OUTLINE_HXX
#include <outline.hxx>
#endif
#ifndef _UINUMS_HXX
#include <uinums.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _SHELLRES_HXX //autogen
#include <shellres.hxx>
#endif
#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif
#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#ifndef _DOCSTYLE_HXX //autogen
#include <docstyle.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif

#ifndef _HELPID_H
#include <helpid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>		// fuer Vorlagenname 'keins'
#endif
#ifndef _MISC_HRC
#include <misc.hrc>
#endif
#ifndef _OUTLINE_HRC
#include <outline.hrc>
#endif

#define C2S(cChar) UniString::CreateFromAscii(cChar)
/* -----------------------------31.01.01 10:23--------------------------------

 ---------------------------------------------------------------------------*/

DBG_NAME(outlinehdl);

USHORT lcl_AdjustToPos(SvxAdjust eAdjust)
{
    USHORT nRet = 0;
    switch( eAdjust )
    {
        case SVX_ADJUST_CENTER: nRet = 1;break;
        case SVX_ADJUST_RIGHT: nRet = 2; break;
    }
    return nRet;
}
/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

class SwNumNamesDlg: public ModalDialog
{
    Edit         aFormEdit;
    ListBox      aFormBox;
    FixedLine    aFormFL;
    OKButton     aOKBtn;
    CancelButton aCancelBtn;
    HelpButton   aHelpBtn;

    DECL_LINK( ModifyHdl, Edit * );
    DECL_LINK( SelectHdl, ListBox * );
    DECL_LINK( DoubleClickHdl, ListBox * );

public:
    SwNumNamesDlg(Window *pParent);
    ~SwNumNamesDlg();
    void SetUserNames(const String *pList[]);
    String GetName() const { return aFormEdit.GetText(); }
    USHORT GetCurEntryPos() const { return aFormBox.GetSelectEntryPos(); }
};

/*------------------------------------------------------------------------
 Beschreibung:  selektierten Eintrag merken
------------------------------------------------------------------------*/


IMPL_LINK_INLINE_START( SwNumNamesDlg, SelectHdl, ListBox *, pBox )
{
    aFormEdit.SetText(pBox->GetSelectEntry());
    aFormEdit.SetSelection(Selection(0, SELECTION_MAX));
    return 0;
}
IMPL_LINK_INLINE_END( SwNumNamesDlg, SelectHdl, ListBox *, pBox )

/*------------------------------------------------------------------------
 Beschreibung:  Setzen der vom Benutzer vergebenen Namen
 Parameter:     Liste der vom Benutzer vergebenen Namen;
                nicht vom Benutzer benannte Positionen sind 0.
------------------------------------------------------------------------*/


void SwNumNamesDlg::SetUserNames(const String *pList[])
{
    USHORT nSelect = 0;
    for(USHORT i = 0; i < SwBaseNumRules::nMaxRules; ++i)
    {
        if(pList[i])
        {
            aFormBox.RemoveEntry(i);
            aFormBox.InsertEntry(*pList[i], i);
            if(i == nSelect && nSelect < SwBaseNumRules::nMaxRules)
                nSelect++;
        }
    }
    aFormBox.SelectEntryPos(nSelect);
    SelectHdl(&aFormBox);
}
/*------------------------------------------------------------------------
 Beschreibung:  OK-Button freischalten, wenn Text im Edit steht.
------------------------------------------------------------------------*/


IMPL_LINK_INLINE_START( SwNumNamesDlg, ModifyHdl, Edit *, pBox )
{
    aOKBtn.Enable(0 != pBox->GetText().Len());
    return 0;
}
IMPL_LINK_INLINE_END( SwNumNamesDlg, ModifyHdl, Edit *, pBox )
/*------------------------------------------------------------------------
 Beschreibung:  DoubleClickHdl
------------------------------------------------------------------------*/


IMPL_LINK_INLINE_START( SwNumNamesDlg, DoubleClickHdl, ListBox *, EMPTYARG )
{
    EndDialog(RET_OK);
    return 0;
}
IMPL_LINK_INLINE_END( SwNumNamesDlg, DoubleClickHdl, ListBox *, EMPTYARG )

/*--------------------------------------------------

--------------------------------------------------*/

SwNumNamesDlg::SwNumNamesDlg(Window *pParent)
    : ModalDialog(pParent, SW_RES(DLG_NUM_NAMES)),
    aFormEdit(this, SW_RES(ED_FORM)),
    aFormBox(this, SW_RES(LB_FORM)),
    aFormFL(this, SW_RES(FL_FORM)),
    aOKBtn(this, SW_RES(BT_OK)),
    aCancelBtn(this, SW_RES(BT_CANCEL)),
    aHelpBtn(this, SW_RES(BT_HELP))
{
    FreeResource();
    aFormEdit.SetModifyHdl(LINK(this, SwNumNamesDlg, ModifyHdl));
    aFormBox.SetSelectHdl(LINK(this, SwNumNamesDlg, SelectHdl));
    aFormBox.SetDoubleClickHdl(LINK(this, SwNumNamesDlg, DoubleClickHdl));
    SelectHdl(&aFormBox);
}

/*--------------------------------------------------

--------------------------------------------------*/

__EXPORT SwNumNamesDlg::~SwNumNamesDlg() {}


/* -----------------08.07.98 08:46-------------------
 *
 * --------------------------------------------------*/
USHORT lcl_BitToLevel(USHORT nActLevel)
{
    USHORT nTmp = nActLevel;
    USHORT nTmpLevel = 0;
    while( 0 != (nTmp >>= 1) )
        nTmpLevel++;
    return nTmpLevel;
}

/* -----------------07.07.98 14:13-------------------
 *
 * --------------------------------------------------*/
SwOutlineTabDialog::SwOutlineTabDialog(Window* pParent,
                    const SfxItemSet* pSwItemSet,
                    SwWrtShell &rSh) :
                                    //der UserString wird danach richtig gesetzt
        SfxTabDialog(pParent, SW_RES(DLG_TAB_OUTLINE), pSwItemSet, FALSE, &aEmptyStr),
        aFormMenu(SW_RES(MN_FORM)),
        bModified(rWrtSh.IsModified()),
        aNullStr(C2S("____")),
        nNumLevel(1),
        pChapterNumRules(SW_MOD()->GetChapterNumRules()),
        rWrtSh(rSh)
{
    PushButton* pUserBtn = GetUserButton();
    pUserBtn->SetText(SW_RES(ST_FORM));
    pUserBtn->SetHelpId(HID_OUTLINE_FORM);
    pUserBtn->SetClickHdl(LINK(this, SwOutlineTabDialog, FormHdl));

    FreeResource();
    pNumRule = new SwNumRule( *rSh.GetOutlineNumRule() );
    GetCancelButton().SetClickHdl(LINK(this, SwOutlineTabDialog, CancelHdl));

    AddTabPage(TP_NUM_POSITION	 , &SwNumPositionTabPage::Create, 0);
    AddTabPage(TP_OUTLINE_NUM	 , &SwOutlineSettingsTabPage::Create, 0);

    String sHeadline;
    for(USHORT i = 0; i < MAXLEVEL; ++i )
    {
        // wurde die Vorlage noch nicht angelegt, dann ist sie noch an dieserPosition
        if( !rWrtSh.GetParaStyle( sHeadline =
            SwStyleNameMapper::GetUIName( RES_POOLCOLL_HEADLINE1 + i,
                                                    sHeadline )) )
            aCollNames[i] = sHeadline;
    }

    // Erfragen der Gliederungsebenen der Textvorlagen
    const USHORT nCount = rWrtSh.GetTxtFmtCollCount();
    for(i = 0; i < nCount; ++i )
    {
        SwTxtFmtColl &rTxtColl = rWrtSh.GetTxtFmtColl(i);
        if(!rTxtColl.IsDefault())
        {
            BYTE nOutLevel = rTxtColl.GetOutlineLevel();
            if(nOutLevel != NO_NUMBERING)
                aCollNames[ nOutLevel ] = rTxtColl.GetName();
        }
    }
}

/* -----------------07.07.98 14:13-------------------
 *
 * --------------------------------------------------*/
SwOutlineTabDialog::~SwOutlineTabDialog()
{
    delete pNumRule;
}

/* -----------------07.07.98 14:13-------------------
 *
 * --------------------------------------------------*/
void 	SwOutlineTabDialog::PageCreated(USHORT nPageId, SfxTabPage& rPage)
{
    switch ( nPageId )
    {
        case TP_NUM_POSITION:
                ((SwNumPositionTabPage&)rPage).SetWrtShell(&rWrtSh);
                ((SwNumPositionTabPage&)rPage).SetOutlineTabDialog(this);
        break;
        case TP_OUTLINE_NUM:
                ((SwOutlineSettingsTabPage&)rPage).SetWrtShell(&rWrtSh);
        break;
    }
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineTabDialog, CancelHdl, Button *, pBtn )
{
    if (!bModified)
        rWrtSh.ResetModified();
    EndDialog(RET_CANCEL);
    return 0;
}
/* -----------------08.07.98 12:14-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineTabDialog, FormHdl, Button *, pBtn )
{
    //PopupMenu auffuellen
    for( USHORT i = 0; i < SwChapterNumRules::nMaxRules; ++i )
    {
        const SwNumRulesWithName *pRules = pChapterNumRules->GetRules(i);
        if( pRules )
            aFormMenu.SetItemText(i + MN_FORMBASE, pRules->GetName());
    }
    aFormMenu.SetSelectHdl(LINK(this, SwOutlineTabDialog, MenuSelectHdl));
    aFormMenu.Execute(pBtn, Rectangle(Point(0,0), pBtn->GetSizePixel()), POPUPMENU_EXECUTE_DOWN);
    return 0;
}

/* -----------------08.07.98 12:14-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineTabDialog, MenuSelectHdl, Menu *, pMenu )
{
    BYTE nLevelNo = 0;
    switch(pMenu->GetCurItemId())
    {
        case MN_FORM1: nLevelNo = 1;	break;
        case MN_FORM2: nLevelNo = 2;	break;
        case MN_FORM3: nLevelNo = 3;	break;
        case MN_FORM4: nLevelNo = 4;	break;
        case MN_FORM5: nLevelNo = 5;	break;
        case MN_FORM6: nLevelNo = 6;	break;
        case MN_FORM7: nLevelNo = 7;	break;
        case MN_FORM8: nLevelNo = 8;	break;
        case MN_FORM9: nLevelNo = 9;	break;

        case MN_SAVE:
        {
            SwNumNamesDlg *pDlg = new SwNumNamesDlg(this);
            const String *aStrArr[SwChapterNumRules::nMaxRules];
            for(USHORT i = 0; i < SwChapterNumRules::nMaxRules; ++i)
            {
                const SwNumRulesWithName *pRules = pChapterNumRules->GetRules(i);
                if(pRules)
                    aStrArr[i] = &pRules->GetName();
                else
                    aStrArr[i] = 0;
            }
            pDlg->SetUserNames(aStrArr);
            if(RET_OK == pDlg->Execute())
            {
                const String aName(pDlg->GetName());
                pChapterNumRules->ApplyNumRules( SwNumRulesWithName(
                        *pNumRule, aName ), pDlg->GetCurEntryPos() );
                pMenu->SetItemText(
                        pDlg->GetCurEntryPos() + MN_FORMBASE, aName);
            }
            delete pDlg;
            return 0;

        }

    }
    if( nLevelNo-- )
    {
        const SwNumRulesWithName *pRules = pChapterNumRules->GetRules( nLevelNo );
        if( pRules )
        {
            pRules->MakeNumRule( rWrtSh, *pNumRule );
            pNumRule->SetRuleType( OUTLINE_RULE );
        }
        else
            *pNumRule = *rWrtSh.GetOutlineNumRule();
    }

    USHORT	nPageId = GetCurPageId();
    SfxTabPage*	pPage = GetTabPage( nPageId );
    pPage->Reset(*GetOutputItemSet());

    return 0;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
USHORT 	SwOutlineTabDialog::GetLevel(const String &rFmtName) const
{
    for(USHORT i = 0; i < MAXLEVEL; ++i)
    {
        if(aCollNames[i] == rFmtName)
            return i;
    }
    return NO_NUMBERING;
}
/* -----------------07.07.98 16:30-------------------
 *
 * --------------------------------------------------*/
short SwOutlineTabDialog::Ok()
{
    short nRet = SfxTabDialog::Ok();
    // bei allen erzeugten Vorlagen die Ebenen setzen, muss
    // geschehen, um evtl. aufgehobene Zuordnungen
    // auch wieder zu loeschen
    USHORT i, nCount = rWrtSh.GetTxtFmtCollCount();
    for( i = 0; i < nCount; ++i )
    {
        SwTxtFmtColl &rTxtColl = rWrtSh.GetTxtFmtColl(i);
        if( !rTxtColl.IsDefault() )
            rTxtColl.SetOutlineLevel( (BYTE)GetLevel(rTxtColl.GetName()));
    }

    for( i = 0; i < MAXLEVEL; ++i )
    {
        String sHeadline;
        ::SwStyleNameMapper::FillUIName( RES_POOLCOLL_HEADLINE1 + i, sHeadline );
        SwTxtFmtColl* pColl = rWrtSh.FindTxtFmtCollByName( sHeadline );
        if( !pColl )
        {
            if( !aCollNames[i].Len() )
            {
                SwTxtFmtColl* pTxtColl = rWrtSh.GetTxtCollFromPool(
                                                    RES_POOLCOLL_HEADLINE1 + i );
                pTxtColl->SetOutlineLevel( NO_NUMBERING );
            }
            else if(aCollNames[i] != sHeadline)
            {
                SwTxtFmtColl* pTxtColl = rWrtSh.GetParaStyle(
                                aCollNames[i], SwWrtShell::GETSTYLE_CREATESOME);
                if(pTxtColl)
                    pTxtColl->SetOutlineLevel( (BYTE)i );
            }
        }
    }

    rWrtSh.SetOutlineNumRule( *pNumRule );
    return RET_OK;
}

/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
SwOutlineSettingsTabPage::SwOutlineSettingsTabPage(Window* pParent, const SfxItemSet& rSet) :
    SfxTabPage(pParent, SW_RES(TP_OUTLINE_NUM), rSet),
    aLevelFL(       this, ResId(FL_LEVEL    )),
    aLevelLB(		this, ResId(LB_LEVEL	)),
    aPreviewWIN(	this, ResId(WIN_PREVIEW	)),
    aCollLbl(this, SW_RES(FT_COLL)),
    aCollBox(this, SW_RES(LB_COLL)),
    aNumberLbl(this, SW_RES(FT_NUMBER)),
    aNumberBox(this, SW_RES(LB_NUMBER), INSERT_NUM_TYPE_NO_NUMBERING|INSERT_NUM_EXTENDED_TYPES),
    aCharFmtFT(this, SW_RES(FT_CHARFMT)),
    aCharFmtLB(this, SW_RES(LB_CHARFMT)),
    aAllLevelFT(	this, ResId(FT_ALL_LEVEL)),
    aAllLevelNF(	this, ResId(NF_ALL_LEVEL)),
    aDelim(this, SW_RES(FT_DELIM)),
    aPrefixFT(this, SW_RES(FT_PREFIX)),
    aPrefixED(this, SW_RES(ED_PREFIX)),
    aSuffixFT(this, SW_RES(FT_SUFFIX)),
    aSuffixED(this, SW_RES(ED_SUFFIX)),
    aStartLbl(this, SW_RES(FT_START)),
    aStartEdit(this, SW_RES(ED_START)),
    aNumberFL(this, SW_RES(FL_NUMBER)),
    aNoFmtName(SW_RES(ST_NO_COLL)),
    nActLevel(1),
    pCollNames(0),
    pSh(0)
{
    FreeResource();
    SetExchangeSupport();

    aCollBox.InsertEntry(aNoFmtName);
    aLevelLB.SetSelectHdl(LINK(this, 	SwOutlineSettingsTabPage, LevelHdl));
    aAllLevelNF.SetModifyHdl(LINK(this, SwOutlineSettingsTabPage, ToggleComplete));
    aCollBox.SetSelectHdl(LINK(this, 	SwOutlineSettingsTabPage, CollSelect));
    aCollBox.SetGetFocusHdl(LINK(this, 	SwOutlineSettingsTabPage, CollSelectGetFocus));
    aNumberBox.SetSelectHdl(LINK(this, 	SwOutlineSettingsTabPage, NumberSelect));
    aPrefixED.SetModifyHdl(LINK(this, 	SwOutlineSettingsTabPage, DelimModify));
    aSuffixED.SetModifyHdl(LINK(this, 	SwOutlineSettingsTabPage, DelimModify));
    aStartEdit.SetModifyHdl(LINK(this, 	SwOutlineSettingsTabPage, StartModified));
    aCharFmtLB.SetSelectHdl(LINK(this, 	SwOutlineSettingsTabPage, CharFmtHdl));

}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
void   	SwOutlineSettingsTabPage::Update()
{
        // falls eine Vorlage fuer diese Ebene bereits selektiert wurde,
        // diese in der ListBox auswaehlean
    aCollBox.Enable(USHRT_MAX != nActLevel);
    if(USHRT_MAX == nActLevel)
    {
        BOOL bSamePrefix = TRUE;
        BOOL bSameSuffix = TRUE;
        BOOL bSameType = TRUE;
        BOOL bSameComplete = TRUE;
        BOOL bSameStart = TRUE;
        BOOL bSameCharFmt = TRUE;

        const SwNumFmt* aNumFmtArr[MAXLEVEL];
        const SwCharFmt* pFirstFmt = 0;

        for(USHORT i = 0; i < MAXLEVEL; i++)
        {

            aNumFmtArr[ i ] = &pNumRule->Get(i);
            if(i == 0)
                pFirstFmt = aNumFmtArr[i]->GetCharFmt();
            else
            {
                bSameType 	&= aNumFmtArr[i]->GetNumberingType() == aNumFmtArr[0]->GetNumberingType();
                bSameStart	&= aNumFmtArr[i]->GetStart() == aNumFmtArr[0]->GetStart();
                bSamePrefix &= aNumFmtArr[i]->GetPrefix() == aNumFmtArr[0]->GetPrefix();
                bSameSuffix &= aNumFmtArr[i]->GetSuffix() == aNumFmtArr[0]->GetSuffix();
                bSameComplete &= aNumFmtArr[i]->GetIncludeUpperLevels() == aNumFmtArr[0]->GetIncludeUpperLevels();
                const SwCharFmt* pFmt = aNumFmtArr[i]->GetCharFmt();
                bSameCharFmt &= 	!pFirstFmt && !pFmt
                                    || pFirstFmt && pFmt && pFmt->GetName() == pFirstFmt->GetName();
            }
        }
        CheckForStartValue_Impl(aNumFmtArr[0]->GetNumberingType());
        if(bSameType)
            aNumberBox.SelectNumberingType( aNumFmtArr[0]->GetNumberingType() );
        else
            aNumberBox.SetNoSelection();
        if(bSameStart)
            aStartEdit.SetValue(aNumFmtArr[0]->GetStart());
        else
            aStartEdit.SetText(aEmptyStr);
        if(bSamePrefix)
            aPrefixED.SetText(aNumFmtArr[0]->GetPrefix());
        else
            aPrefixED.SetText(aEmptyStr);
        if(bSameSuffix)
            aSuffixED.SetText(aNumFmtArr[0]->GetSuffix());
        else
            aSuffixED.SetText(aEmptyStr);

        if(bSameCharFmt)
        {
            if(pFirstFmt)
                aCharFmtLB.SelectEntry(pFirstFmt->GetName());
            else
                aCharFmtLB.SelectEntry( ViewShell::GetShellRes()->aStrNone );
        }
        else
            aCharFmtLB.SetNoSelection();

        aAllLevelFT.Enable(TRUE);
        aAllLevelNF.Enable(TRUE);
        aAllLevelNF.SetMax(MAXLEVEL);
        if(bSameComplete)
        {
            aAllLevelNF.SetValue(aNumFmtArr[0]->GetIncludeUpperLevels());
        }
        else
        {
            aAllLevelNF.SetText(aEmptyStr);
        }
    }
    else
    {
        USHORT nTmpLevel = lcl_BitToLevel(nActLevel);
        String aColl(pCollNames[nTmpLevel]);
        if(aColl.Len())
            aCollBox.SelectEntry(aColl);
        else
            aCollBox.SelectEntry(aNoFmtName);
        const SwNumFmt &rFmt = pNumRule->Get(nTmpLevel);

        aNumberBox.SelectNumberingType( rFmt.GetNumberingType() );
        aPrefixED.SetText(rFmt.GetPrefix());
        aSuffixED.SetText(rFmt.GetSuffix());
        const SwCharFmt* pFmt = rFmt.GetCharFmt();
        if(pFmt)
            aCharFmtLB.SelectEntry(pFmt->GetName());
        else
            aCharFmtLB.SelectEntry( ViewShell::GetShellRes()->aStrNone );

        if(nTmpLevel)
        {
            aAllLevelFT.Enable(TRUE);
            aAllLevelNF.Enable(TRUE);
            aAllLevelNF.SetMax(nTmpLevel + 1);
            aAllLevelNF.SetValue(rFmt.GetIncludeUpperLevels());
        }
        else
        {
            aAllLevelNF.SetText(aEmptyStr);
            aAllLevelNF.Enable(FALSE);
            aAllLevelFT.Enable(FALSE);
        }
        CheckForStartValue_Impl(rFmt.GetNumberingType());
        aStartEdit.SetValue( rFmt.GetStart() );
    }
    SetModified();
}

/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineSettingsTabPage, LevelHdl, ListBox *, pBox )
{
    nActLevel = 0;
    if(pBox->IsEntryPosSelected( MAXLEVEL ))
    {
        nActLevel = 0xFFFF;
    }
    else
    {
        USHORT nMask = 1;
        for( USHORT i = 0; i < MAXLEVEL; i++ )
        {
            if(pBox->IsEntryPosSelected( i ))
                nActLevel |= nMask;
            nMask <<= 1;
        }
    }
    Update();
    return 0;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineSettingsTabPage, ToggleComplete, NumericField *, pFld )
{
    USHORT nStart = 0;
    USHORT nEnd = MAXLEVEL;
    USHORT nMask = 1;
    for(USHORT i = 0; i < MAXLEVEL; i++)
    {
        if(nActLevel & nMask)
        {
            SwNumFmt aNumFmt(pNumRule->Get(i));
            aNumFmt.SetIncludeUpperLevels( Min( (BYTE)pFld->GetValue(),
                                                (BYTE)(i + 1)) );
            pNumRule->Set(i, aNumFmt);
        }
        nMask <<= 1;
    }
    SetModified();
    return 0;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineSettingsTabPage, CollSelect, ListBox *, pBox )
{
    BYTE i;

    const String aCollName(pBox->GetSelectEntry());
    //0xFFFF darf hier nicht sein (disable)
    USHORT nTmp = nActLevel;
    USHORT nTmpLevel = lcl_BitToLevel(nActLevel);
    String sOldName( pCollNames[nTmpLevel] );

    for( i = 0; i < MAXLEVEL; ++i)
        pCollNames[i] = aSaveCollNames[i];

    if(aCollName == aNoFmtName)
        pCollNames[nTmpLevel] = aEmptyStr;
    else
    {
        pCollNames[nTmpLevel] = aCollName;
                // wird die Vorlage bereits verwendet ?
        for( i = 0; i < MAXLEVEL; ++i)
            if(i != nTmpLevel && pCollNames[i] == aCollName )
                pCollNames[i] = aEmptyStr;
    }

    // search the oldname and put it into the current entries
    if( sOldName.Len() )
        for( i = 0; i < MAXLEVEL; ++i)
            if( aSaveCollNames[ i ] == sOldName && i != nTmpLevel &&
                !pCollNames[ i ].Len() )
            {
                BYTE n;
                for( n = 0; n < MAXLEVEL; ++n )
                    if( pCollNames[ n ] == sOldName )
                        break;

                if( MAXLEVEL == n )
                    // it was a outline leveld name and the current entries is zero.
                    pCollNames[ i ] = sOldName;
            }

    SetModified();
    return 0;
}

IMPL_LINK( SwOutlineSettingsTabPage, CollSelectGetFocus, ListBox *, pBox )
{
    for( BYTE i = 0; i < MAXLEVEL; ++i)
        aSaveCollNames[i] =  pCollNames[i];
    return 0;
}

/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineSettingsTabPage, NumberSelect, SwNumberingTypeListBox *, pBox )
{
    USHORT nStart = 0;
    USHORT nEnd = MAXLEVEL;
    USHORT nMask = 1;
    sal_Int16 nNumberType = pBox->GetSelectedNumberingType();//(sal_Int16)(ULONG)pBox->GetEntryData(pBox->GetSelectEntryPos());
    for(USHORT i = 0; i < MAXLEVEL; i++)
    {
        if(nActLevel & nMask)
        {
            SwNumFmt aNumFmt(pNumRule->Get(i));
            aNumFmt.SetNumberingType(nNumberType);
            pNumRule->Set(i, aNumFmt);
            CheckForStartValue_Impl(nNumberType);
        }
        nMask <<= 1;
    }
    SetModified();
    return 0;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineSettingsTabPage, DelimModify, Edit *, pEdt )
{
    USHORT nStart = 0;
    USHORT nEnd = MAXLEVEL;
    USHORT nMask = 1;
    for(USHORT i = 0; i < MAXLEVEL; i++)
    {
        if(nActLevel & nMask)
        {
            SwNumFmt aNumFmt(pNumRule->Get(i));
            aNumFmt.SetPrefix( aPrefixED.GetText() );
            aNumFmt.SetSuffix( aSuffixED.GetText() );
            pNumRule->Set(i, aNumFmt);
        }
        nMask <<= 1;
    }
    SetModified();
    return 0;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineSettingsTabPage, StartModified, NumericField *, pFld )
{
    USHORT nStart = 0;
    USHORT nEnd = MAXLEVEL;
    USHORT nMask = 1;
    for(USHORT i = 0; i < MAXLEVEL; i++)
    {
        if(nActLevel & nMask)
        {
            SwNumFmt aNumFmt(pNumRule->Get(i));
            aNumFmt.SetStart( (USHORT)pFld->GetValue() );
            pNumRule->Set(i, aNumFmt);
        }
        nMask <<= 1;
    }
    SetModified();
    return 0;
}
/* -----------------21.09.98 12:21-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwOutlineSettingsTabPage, CharFmtHdl, ListBox *, EMPTYARG )
{
//	bAutomaticCharStyles = FALSE;
    String sEntry = aCharFmtLB.GetSelectEntry();
    USHORT nMask = 1;
    BOOL bFormatNone = sEntry == ViewShell::GetShellRes()->aStrNone;
    SwCharFmt* pFmt = 0;
    if(!bFormatNone)
    {
        USHORT nChCount = pSh->GetCharFmtCount();
        for(USHORT i = 0; i < nChCount; i++)
        {
            SwCharFmt& rChFmt = pSh->GetCharFmt(i);
            if(rChFmt.GetName() == sEntry)
            {
                pFmt = &rChFmt;
                break;
            }
        }
        if(!pFmt)
        {
            SfxStyleSheetBasePool* pPool = pSh->GetView().GetDocShell()->GetStyleSheetPool();
            SfxStyleSheetBase* pBase;
            pBase = pPool->Find(sEntry, SFX_STYLE_FAMILY_CHAR);
            if(!pBase)
                pBase = &pPool->Make(sEntry, SFX_STYLE_FAMILY_PAGE);
            pFmt = ((SwDocStyleSheet*)pBase)->GetCharFmt();

        }
    }

    for(USHORT i = 0; i < MAXLEVEL; i++)
    {
        if(nActLevel & nMask)
        {
            SwNumFmt aNumFmt(pNumRule->Get(i));
            if(bFormatNone)
                aNumFmt.SetCharFmt(0);
            else
                aNumFmt.SetCharFmt(pFmt);
            pNumRule->Set(i, aNumFmt);
        }
        nMask <<= 1;
    }
    return RET_OK;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
SwOutlineSettingsTabPage::~SwOutlineSettingsTabPage()
{
}
/* -----------------07.07.98 16:27-------------------
 *
 * --------------------------------------------------*/
void SwOutlineSettingsTabPage::SetWrtShell(SwWrtShell* pShell)
{
    pSh = pShell;
    // Erfragen der NumRules dieses Dokumentes
    pNumRule = ((SwOutlineTabDialog*)GetTabDialog())->GetNumRule();
    pCollNames = ((SwOutlineTabDialog*)GetTabDialog())->GetCollNames();

    //pNumRule = new SwNumRule( *rSh.GetOutlineNumRule() );

    aPreviewWIN.SetNumRule(pNumRule);
    aPreviewWIN.SetOutlineNames(pCollNames);
    // Startwert setzen - nActLevel muss hier 1 sein
    USHORT nTmpLevel = lcl_BitToLevel(nActLevel);
    const SwNumFmt& rNumFmt = pNumRule->Get( nTmpLevel );
    aStartEdit.SetValue( rNumFmt.GetStart() );

    // Poolformate fuer Ueberschriften anlegen
    String sStr;
    USHORT i;
    for( i = 0; i < MAXLEVEL; ++i )
    {
        aCollBox.InsertEntry( SwStyleNameMapper::GetUIName(
                                    RES_POOLCOLL_HEADLINE1 + i, aEmptyStr ));
        aLevelLB.InsertEntry( String::CreateFromInt32(i + 1) );
    }
    sStr.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "1 - " ));
    sStr += String::CreateFromInt32(MAXLEVEL);
    aLevelLB.InsertEntry( sStr );

    // Erfragen der Gliederungsebenen der Textvorlagen
    const USHORT nCount = pSh->GetTxtFmtCollCount();
    for( i = 0; i < nCount; ++i )
    {
        SwTxtFmtColl &rTxtColl = pSh->GetTxtFmtColl(i);
        if(!rTxtColl.IsDefault())
        {
            BYTE nOutLevel = rTxtColl.GetOutlineLevel();
            sStr = rTxtColl.GetName();
            if(LISTBOX_ENTRY_NOTFOUND == aCollBox.GetEntryPos( sStr ))
                aCollBox.InsertEntry( sStr );
        }
    }

    aNumberBox.SelectNumberingType(rNumFmt.GetNumberingType());
    USHORT nOutlinePos = pSh->GetOutlinePos(MAXLEVEL);
    USHORT nTmp = 0;
    if(nOutlinePos != USHRT_MAX)
    {
        nTmp = pSh->GetOutlineLevel(nOutlinePos);
    }
    aLevelLB.SelectEntryPos(nTmp);

    // Zeichenvorlagen sammeln
    aCharFmtLB.Clear();
    aCharFmtLB.InsertEntry( ViewShell::GetShellRes()->aStrNone );

    // Zeichenvorlagen
    ::FillCharStyleListBox(aCharFmtLB,
                        pSh->GetView().GetDocShell());
    Update();
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
void 	SwOutlineSettingsTabPage::ActivatePage(const SfxItemSet& rSet)
{
    nActLevel = ((SwOutlineTabDialog*)GetTabDialog())->GetActNumLevel();
    if(nActLevel != USHRT_MAX)
        aLevelLB.SelectEntryPos(lcl_BitToLevel(nActLevel));
    else
        aLevelLB.SelectEntryPos(MAXLEVEL);
    LevelHdl(&aLevelLB);
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
int 	SwOutlineSettingsTabPage::DeactivatePage(SfxItemSet *pSet)
{
    ((SwOutlineTabDialog*)GetTabDialog())->SetActNumLevel(nActLevel);
    return LEAVE_PAGE;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
BOOL 	SwOutlineSettingsTabPage::FillItemSet( SfxItemSet& rSet )
{
    return TRUE;
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
void 	SwOutlineSettingsTabPage::Reset( const SfxItemSet& rSet )
{
    ActivatePage(rSet);
}
/* -----------------07.07.98 14:19-------------------
 *
 * --------------------------------------------------*/
SfxTabPage*	SwOutlineSettingsTabPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet)
{
    return new SwOutlineSettingsTabPage(pParent, rAttrSet);
}
/* -----------------07.11.2002 15:13-----------------
 *
 * --------------------------------------------------*/
void SwOutlineSettingsTabPage::CheckForStartValue_Impl(sal_uInt16 nNumberingType)
{
    BOOL bIsNull = aStartEdit.GetValue() == 0;
    BOOL bNoZeroAllowed = nNumberingType < SVX_NUM_ARABIC ||
                        SVX_NUM_CHARS_UPPER_LETTER_N == nNumberingType ||
                        SVX_NUM_CHARS_LOWER_LETTER_N == nNumberingType;
    aStartEdit.SetMin(bNoZeroAllowed ? 1 : 0);
    if(bIsNull && bNoZeroAllowed)
        aStartEdit.GetModifyHdl().Call(&aStartEdit);
}
/*-----------------09.12.97 11:54-------------------

--------------------------------------------------*/
USHORT lcl_DrawBullet(VirtualDevice* pVDev,
            const SwNumFmt& rFmt, USHORT nXStart,
            USHORT nYStart, const Size& rSize)
{
    Font aTmpFont(pVDev->GetFont());

    Font aFont(*rFmt.GetBulletFont());
    aFont.SetSize(rSize);
    aFont.SetTransparent(TRUE);
    pVDev->SetFont( aFont );
    String aText(rFmt.GetBulletChar());
    pVDev->DrawText( Point(nXStart, nYStart), aText );
    USHORT nRet = (USHORT)pVDev->GetTextWidth(aText);

    pVDev->SetFont(aTmpFont);
    return nRet;
}
/*-----------------09.12.97 11:49-------------------

--------------------------------------------------*/
USHORT lcl_DrawGraphic(VirtualDevice* pVDev, const SwNumFmt &rFmt, USHORT nXStart,
                        USHORT nYStart, USHORT nDivision)
{
    const SvxBrushItem* pBrushItem = rFmt.GetBrush();
    USHORT nRet = 0;
    if(pBrushItem)
    {
        const Graphic* pGrf = pBrushItem->GetGraphic();
        if(pGrf)
        {
            Size aGSize( rFmt.GetGraphicSize());
            aGSize.Width() /= nDivision;
            nRet = (USHORT)aGSize.Width();
            aGSize.Height() /= nDivision;
            pGrf->Draw( pVDev, Point(nXStart,nYStart),
                    pVDev->PixelToLogic( aGSize ) );
        }
    }
    return nRet;

}
/*-----------------02.12.97 10:34-------------------
    Vorschau der Numerierung painten
--------------------------------------------------*/
void	NumberingPreview::Paint( const Rectangle& rRect )
{
    Size aSize(PixelToLogic(GetOutputSizePixel()));
    Rectangle aRect(Point(0,0), aSize);

    VirtualDevice* pVDev = new VirtualDevice(*this);
    pVDev->SetMapMode(GetMapMode());
    pVDev->SetOutputSize( aSize );

    // #101524# OJ
    pVDev->SetFillColor( GetSettings().GetStyleSettings().GetWindowColor() );
    pVDev->SetLineColor( GetSettings().GetStyleSettings().GetButtonTextColor() );
    pVDev->DrawRect(aRect);

    if(pActNum)
    {
        USHORT nWidthRelation;
        if(nPageWidth)
        {
            nWidthRelation = USHORT (nPageWidth / aSize.Width());
            if(bPosition)
                nWidthRelation = nWidthRelation * 2 / 3;
            else
                nWidthRelation = nWidthRelation / 4;
        }
        else
            nWidthRelation = 30; // Kapiteldialog

        //Hoehe pro Ebene
        USHORT nXStep = aSize.Width() / (3 * MAXLEVEL);
        if(MAXLEVEL < 10)
            nXStep /= 2;
        USHORT nYStart = 4;
        USHORT nYStep = (aSize.Height() - 6)/ MAXLEVEL;
        aStdFont = OutputDevice::GetDefaultFont(
                                    DEFAULTFONT_UI_SANS, GetAppLanguage(),
                                    DEFAULTFONT_FLAGS_ONLYONE, this );
        // #101524# OJ
        aStdFont.SetColor( SwViewOption::GetFontColor() );

        //
        USHORT nFontHeight = nYStep * 6 / 10;
        if(bPosition)
            nFontHeight = nYStep * 15 / 10;
        aStdFont.SetSize(Size( 0, nFontHeight ));

        SwNodeNum aNum( (BYTE)0 );
        USHORT nPreNum = pActNum->Get(0).GetStart();

        if(bPosition)
        {
            USHORT nLineHeight = nFontHeight * 8 / 7;
            BYTE nStart = 0;
            while( !(nActLevel & (1<<nStart)) )
            {
                nStart++;
            }
            if(nStart) // damit moeglichs Vorgaenger und Nachfolger gezeigt werden
                nStart--;
            BYTE nEnd = Min( (BYTE)(nStart + 3), MAXLEVEL );
            for( BYTE nLevel = nStart; nLevel < nEnd; ++nLevel )
            {
                const SwNumFmt &rFmt = pActNum->Get(nLevel);
                aNum.GetLevelVal()[ nLevel ] = rFmt.GetStart();
                USHORT nXStart = rFmt.GetAbsLSpace() / nWidthRelation;
                USHORT nTextOffset = rFmt.GetCharTextDistance() / nWidthRelation;
                USHORT nNumberXPos = nXStart;
                USHORT nFirstLineOffset = (-rFmt.GetFirstLineOffset()) / nWidthRelation;

                if(nFirstLineOffset <= nNumberXPos)
                    nNumberXPos -= nFirstLineOffset;
                else
                    nNumberXPos = 0;

                USHORT nBulletWidth = 0;
                if( SVX_NUM_BITMAP == rFmt.GetNumberingType() )
                {
                    nBulletWidth = lcl_DrawGraphic(pVDev, rFmt,
                                        nNumberXPos,
                                            nYStart, nWidthRelation);
                }
                else if( SVX_NUM_CHAR_SPECIAL == rFmt.GetNumberingType() )
                {
                    nBulletWidth =  lcl_DrawBullet(pVDev, rFmt, nNumberXPos, nYStart, aStdFont.GetSize());
                }
                else
                {
                    pVDev->SetFont(aStdFont);
                    aNum.SetLevel( nLevel );
                    if(pActNum->IsContinusNum())
                        aNum.GetLevelVal()[nLevel] = nPreNum;
                    String aText(pActNum->MakeNumString( aNum ));
                    pVDev->DrawText( Point(nNumberXPos, nYStart), aText );
                    nBulletWidth = (USHORT)pVDev->GetTextWidth(aText);
                    nPreNum++;
                }
                USHORT nTextXPos = nXStart;
                if(nNumberXPos + nBulletWidth + nTextOffset > nTextXPos )
                    nTextXPos = nNumberXPos + nBulletWidth + nTextOffset;
                Rectangle aRect1(Point(nTextXPos, nYStart + nFontHeight / 2), Size(aSize.Width() / 2, 2));
                pVDev->SetFillColor( GetSettings().GetStyleSettings().GetWindowColor() ); // Color( COL_BLACK ) );
                pVDev->DrawRect( aRect1 );

                Rectangle aRect2(Point(nXStart, nYStart + nLineHeight + nFontHeight / 2 ), Size(aSize.Width() / 2, 2));
                pVDev->DrawRect( aRect2 );
                nYStart += 2 * nLineHeight;
            }
        }
        else
        {
            USHORT nLineHeight = nFontHeight * 3 / 2;
            for( BYTE nLevel = 0; nLevel < MAXLEVEL;
                            ++nLevel, nYStart += nYStep )
            {
                const SwNumFmt &rFmt = pActNum->Get(nLevel);
                aNum.GetLevelVal()[ nLevel ] = rFmt.GetStart();
                USHORT nXStart = (rFmt.GetAbsLSpace() / nWidthRelation) / 2 + 2;
                USHORT nTextOffset = 2 * nXStep;
                if( SVX_NUM_BITMAP == rFmt.GetNumberingType() )
                {
                    lcl_DrawGraphic(pVDev, rFmt, nXStart, nYStart, nWidthRelation);
                    nTextOffset = nLineHeight + nXStep;
                }
                else if( SVX_NUM_CHAR_SPECIAL == rFmt.GetNumberingType() )
                {
                    nTextOffset =  lcl_DrawBullet(pVDev, rFmt, nXStart, nYStart, aStdFont.GetSize());
                    nTextOffset += nXStep;
                }
                else
                {
                    pVDev->SetFont(aStdFont);
                    aNum.SetLevel( nLevel );
                    if(pActNum->IsContinusNum())
                        aNum.GetLevelVal()[nLevel] = nPreNum;
                    String aText(pActNum->MakeNumString( aNum ));
                    pVDev->DrawText( Point(nXStart, nYStart), aText );
                    nTextOffset = (USHORT)pVDev->GetTextWidth(aText);
                    nTextOffset += nXStep;
                    nPreNum++;
                }
                pVDev->SetFont(aStdFont);

                // Changed as per BugID 79541 Branding/Configuration
                ::utl::ConfigManager* pMgr = ::utl::ConfigManager::GetConfigManager();
                UNOANY MyAny = pMgr->GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTNAME );
                UNOOUSTRING aProductName;

                MyAny >>= aProductName;
                String sMsg(C2S("%PRODUCTNAME"));
                sMsg.SearchAndReplaceAscii( "%PRODUCTNAME" , aProductName );

                if(pOutlineNames)
                    sMsg = pOutlineNames[nLevel];
                pVDev->DrawText( Point(nXStart + nTextOffset, nYStart), sMsg );
            }
        }
    }
    DrawOutDev(	Point(0,0), aSize,
                Point(0,0), aSize,
                        *pVDev );
    delete pVDev;

}

/*-----------------02.12.97 10:34-------------------

--------------------------------------------------*/
NumberingPreview::~NumberingPreview()
{
}


