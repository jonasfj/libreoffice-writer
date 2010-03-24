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



#define _SVSTDARR_STRINGS
#include <tools/urlobj.hxx>
#include <tools/stream.hxx>
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#include <vcl/help.hxx>
#include <unotools/transliterationwrapper.hxx>
#include <unotools/tempfile.hxx>

#include <svl/svstdarr.hxx>
#include <unotools/pathoptions.hxx>
#include <swtypes.hxx>
#include <glosbib.hxx>
#include <gloshdl.hxx>
#include <actctrl.hxx>
#include <glossary.hxx>
#include <glosdoc.hxx>
#include <swunohelper.hxx>

#ifndef _GLOSBIB_HRC
#include <glosbib.hrc>
#endif
#ifndef _MISC_HRC
#include <misc.hrc>
#endif
#ifndef _HELPID_H
#include <helpid.h>
#endif


#define PATH_CASE_SENSITIVE 0x01
#define PATH_READONLY 		0x02

#define RENAME_TOKEN_DELIM		(sal_Unicode)1

/*-----------------09.06.97 13:05-------------------

--------------------------------------------------*/
SwGlossaryGroupDlg::SwGlossaryGroupDlg(Window * pParent,
                        const SvStrings* pPathArr,
                        SwGlossaryHdl *pHdl) :
    SvxStandardDialog(pParent, SW_RES(DLG_BIB_BASE)),

    aNameED( 	this, SW_RES(ED_NAME)),
    aPathLB(	this, SW_RES(LB_PATH)),
    aGroupTLB(  this, SW_RES(TLB_GROUPS)),

    aOkPB(      this, SW_RES(BT_OK)),
    aCancelPB(  this, SW_RES(BT_CANCEL)),
    aHelpPB(    this, SW_RES(BT_HELP)),
    aNewPB(     this, SW_RES(PB_NEW)),
    aDelPB(     this, SW_RES(PB_DELETE)),
    aRenamePB(  this, SW_RES(PB_RENAME)),
    aBibFT(     this, SW_RES(FT_BIB)),
    aPathFT(     this, SW_RES(FT_PATH)),
    aSelectFT(   this, SW_RES(FT_SELECT)),

    pRemovedArr(0),
    pInsertedArr(0),
    pRenamedArr(0),
    pGlosHdl(pHdl)
{
    USHORT i;

    FreeResource();

    long nTabs[] =
    {	2, // Number of Tabs
        0, 160
    };

    aGroupTLB.SetHelpId(HID_GLOS_GROUP_TREE);
    aGroupTLB.SetTabs( &nTabs[0], MAP_APPFONT );
    aGroupTLB.SetWindowBits(WB_HSCROLL|WB_CLIPCHILDREN|WB_SORT);
    aGroupTLB.SetSelectHdl(LINK(this, SwGlossaryGroupDlg, SelectHdl));
    aGroupTLB.GetModel()->SetSortMode(SortAscending);
    aNewPB.SetClickHdl(LINK(this, SwGlossaryGroupDlg, NewHdl));
    aDelPB.SetClickHdl(LINK(this, SwGlossaryGroupDlg, DeleteHdl));
    aNameED.SetModifyHdl(LINK(this, SwGlossaryGroupDlg, ModifyHdl));
    aPathLB.SetSelectHdl(LINK(this, SwGlossaryGroupDlg, ModifyHdl));
    aRenamePB.SetClickHdl(LINK(this, SwGlossaryGroupDlg, RenameHdl));
    for( i = 0; i < pPathArr->Count(); i++)
    {
        String sPath(*(*pPathArr)[i]);
        INetURLObject aTempURL(sPath);
        sPath = aTempURL.GetMainURL(INetURLObject::DECODE_WITH_CHARSET );
        aPathLB.InsertEntry(sPath);
        ULONG nCaseReadonly = 0;
        utl::TempFile aTempFile(&sPath);
        aTempFile.EnableKillingFile();
        if(!aTempFile.IsValid())
            nCaseReadonly |= PATH_READONLY;
        else if( SWUnoHelper::UCB_IsCaseSensitiveFileName( aTempFile.GetURL()))
            nCaseReadonly |= PATH_CASE_SENSITIVE;
        aPathLB.SetEntryData(i, (void*)nCaseReadonly);
    }
    aPathLB.SelectEntryPos(0);
    aPathLB.Enable(TRUE);

    const USHORT nCount = pHdl->GetGroupCnt();
    for(i = 0; i < nCount; ++i)
    {
        String sTitle;
        String sGroup = pHdl->GetGroupName(i, &sTitle);
        if(!sGroup.Len())
            continue;
        GlosBibUserData* pData = new GlosBibUserData;
        pData->sGroupName = sGroup;
        pData->sGroupTitle = sTitle;
        String sTemp(sTitle);
        //sGroup.GetToken(0, GLOS_DELIM)
        sTemp += '\t';
        pData->sPath = aPathLB.GetEntry((USHORT)sGroup.GetToken(1, GLOS_DELIM).ToInt32());
        sTemp += pData->sPath;
        SvLBoxEntry* pEntry = aGroupTLB.InsertEntry(sTemp);
        pEntry->SetUserData(pData);

    }
    aGroupTLB.GetModel()->Resort();
}

/*-----------------09.06.97 13:05-------------------

--------------------------------------------------*/
SwGlossaryGroupDlg::~SwGlossaryGroupDlg()
{

    if(pInsertedArr)
    {
        pInsertedArr->DeleteAndDestroy(0, pInsertedArr->Count());
        delete pInsertedArr;
    }
    if(pRemovedArr)
    {
        pRemovedArr->DeleteAndDestroy(0, pRemovedArr->Count());
        delete pRemovedArr;
    }
    if(pRenamedArr)
    {
        pRenamedArr->DeleteAndDestroy(0, pRenamedArr->Count());
        delete pRenamedArr;
    }

}

/*-----------------09.06.97 13:11-------------------

--------------------------------------------------*/

void __EXPORT SwGlossaryGroupDlg::Apply()
{
    if(aNewPB.IsEnabled())
        NewHdl(&aNewPB);

    String aActGroup = SwGlossaryDlg::GetCurrGroup();

    if(pRemovedArr && pRemovedArr->Count())
    {
        USHORT nCount = pRemovedArr->Count();
        for(USHORT i = 0; i < nCount; ++i)
        {
            const String* pDelEntry = (*pRemovedArr)[i];
            const String sDelGroup = pDelEntry->GetToken(0, '\t');
            if( sDelGroup == aActGroup )
            {
                //soll die aktuelle Gruppe geloescht werden, muss die akt. Gruppe
                //umgesetzt werden
                if(aGroupTLB.GetEntryCount())
                {
                    SvLBoxEntry* pFirst = aGroupTLB.First();
                    GlosBibUserData* pUserData = (GlosBibUserData*)pFirst->GetUserData();
                    pGlosHdl->SetCurGroup(pUserData->sGroupName);
                }
            }
            String sMsg(SW_RES(STR_QUERY_DELETE_GROUP1));
            String sTitle(pDelEntry->GetToken(1, '\t'));
            if(sTitle.Len())
                sMsg += sTitle;
            else
                sDelGroup.GetToken(1, GLOS_DELIM);
            sMsg += SW_RESSTR(STR_QUERY_DELETE_GROUP2);
            QueryBox aQuery(this->GetParent(), WB_YES_NO|WB_DEF_NO, sMsg );
            if(RET_YES == aQuery.Execute())
                pGlosHdl->DelGroup( sDelGroup );
        }

    }
    //erst umbenennen, falls es schon eins gab
    if(pRenamedArr && pRenamedArr->Count())
    {
        USHORT nCount = pRenamedArr->Count();
        for(USHORT i = 0; i < nCount; ++i)
        {
            String * pEntry = (*pRenamedArr)[i];
            xub_StrLen nStrSttPos = 0;
            String sOld( pEntry->GetToken(0, RENAME_TOKEN_DELIM, nStrSttPos ) );
            String sNew( pEntry->GetToken(0, RENAME_TOKEN_DELIM, nStrSttPos) );
            String sTitle( pEntry->GetToken(0, RENAME_TOKEN_DELIM, nStrSttPos) );
            pGlosHdl->RenameGroup(sOld, sNew, sTitle);
            if(!i)
                sCreatedGroup = sNew;
        }
    }
    if(pInsertedArr && pInsertedArr->Count())
    {
        USHORT nCount = pInsertedArr->Count();
        for(USHORT i = 0; i < nCount; ++i)
        {
            String sNewGroup = *(*pInsertedArr)[i];
            String sNewTitle = sNewGroup.GetToken(0, GLOS_DELIM);
            if( *(*pInsertedArr)[i] != aActGroup )
            {
                pGlosHdl->NewGroup(sNewGroup, sNewTitle);
                if(!sCreatedGroup.Len())
                    sCreatedGroup = sNewGroup;
            }
        }
    }
}
/*-----------------09.06.97 13:12-------------------

--------------------------------------------------*/
IMPL_LINK( SwGlossaryGroupDlg, SelectHdl, SvTabListBox*, EMPTYARG  )
{
    aNewPB.Enable(FALSE);
    SvLBoxEntry* pFirstEntry = aGroupTLB.FirstSelected();
    if(pFirstEntry)
    {
        GlosBibUserData* pUserData = (GlosBibUserData*)pFirstEntry->GetUserData();
        String sEntry(pUserData->sGroupName);
        String sName(aNameED.GetText());
        BOOL bExists = FALSE;
        ULONG nPos = aGroupTLB.GetEntryPos(sName, 0);
        if( 0xffffffff > nPos)
        {
            SvLBoxEntry* pEntry = aGroupTLB.GetEntry(nPos);
            GlosBibUserData* pFoundData = (GlosBibUserData*)pEntry->GetUserData();
            String sGroup = pFoundData->sGroupName;
            bExists = sGroup == sEntry;
        }

        aRenamePB.Enable(!bExists && sName.Len());
        aDelPB.Enable(IsDeleteAllowed(sEntry));
    }
    return 0;
}

/*-----------------09.06.97 13:22-------------------

--------------------------------------------------*/
IMPL_LINK( SwGlossaryGroupDlg, NewHdl, Button*, EMPTYARG )
{
    String sGroup(aNameED.GetText());
//	sGroup.ToLower();
    sGroup += GLOS_DELIM;
    sGroup += String::CreateFromInt32(aPathLB.GetSelectEntryPos());
    DBG_ASSERT(!pGlosHdl->FindGroupName(sGroup), "Gruppe bereits vorhanden!");
    if(!pInsertedArr)
        pInsertedArr = new SvStrings;
    pInsertedArr->Insert(new String(sGroup), pInsertedArr->Count());
    String sTemp(aNameED.GetText());
//	sTemp.ToLower();
    sTemp += '\t';
    sTemp += aPathLB.GetSelectEntry();
    SvLBoxEntry* pEntry = aGroupTLB.InsertEntry(sTemp);
    GlosBibUserData* pData = new GlosBibUserData;
    pData->sPath = aPathLB.GetSelectEntry();
    pData->sGroupName = sGroup;
    pData->sGroupTitle = aNameED.GetText();
    pEntry->SetUserData(pData);
    aGroupTLB.Select(pEntry);
    aGroupTLB.MakeVisible(pEntry);
    aGroupTLB.GetModel()->Resort();

    return 0;
}
/*-----------------09.06.97 13:22-------------------

--------------------------------------------------*/
IMPL_LINK( SwGlossaryGroupDlg, DeleteHdl, Button*, pButton  )
{
    SvLBoxEntry* pEntry = aGroupTLB.FirstSelected();
    if(!pEntry)
    {
        pButton->Enable(FALSE);
        return 0;
    }
    GlosBibUserData* pUserData = (GlosBibUserData*)pEntry->GetUserData();
    String sEntry(pUserData->sGroupName);
    // befindet sich der zu loeschende Name schon unter den
    // den neuen - dann weg damit
    BOOL bDelete = TRUE;
    if(pInsertedArr && pInsertedArr->Count())
    {
        USHORT nCount = pInsertedArr->Count();
        for(USHORT i = 0; i < nCount; ++i)
        {
            const String* pTemp = (*pInsertedArr)[i];
            if(*pTemp == sEntry)
            {
                pInsertedArr->Remove(i);
                bDelete = FALSE;
                break;
            }

        }
    }
    // moeglicherweise sollte es schon umbenannt werden?
    if(bDelete)
    {
        if(pRenamedArr && pRenamedArr->Count())
        {
            USHORT nCount = pRenamedArr->Count();
            for(USHORT i = 0; i < nCount; ++i)
            {
                const String* pTemp = (*pRenamedArr)[i];
                String sTemp( pTemp->GetToken(0, RENAME_TOKEN_DELIM ));
                if(sTemp == sEntry)
                {
                    pRenamedArr->Remove(i);
                    bDelete = FALSE;
                    break;
                }
            }
        }
    }
    if(bDelete)
    {
        if(!pRemovedArr)
            pRemovedArr = new SvStrings;
        String sGroupEntry(pUserData->sGroupName);
        sGroupEntry += '\t';
        sGroupEntry += pUserData->sGroupTitle;
        pRemovedArr->Insert(new String(sGroupEntry), pRemovedArr->Count());
    }
    delete pUserData;
    aGroupTLB.GetModel()->Remove(pEntry);
    if(!aGroupTLB.First())
        pButton->Enable(FALSE);
    //the content must be deleted - otherwise the new handler would be called in Apply()
    aNameED.SetText(aEmptyStr);
    return 0;
}

/* -----------------23.11.98 12:26-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwGlossaryGroupDlg, RenameHdl, Button *, EMPTYARG )
{
    SvLBoxEntry* pEntry = aGroupTLB.FirstSelected();
    GlosBibUserData* pUserData = (GlosBibUserData*)pEntry->GetUserData();
    String sEntryText(aGroupTLB.GetEntryText(pEntry));
    String sEntry(pUserData->sGroupName);

    String sNewName(aNameED.GetText());
    String sNewTitle(sNewName);

    sNewName += GLOS_DELIM;
    sNewName += String::CreateFromInt32(aPathLB.GetSelectEntryPos());
    DBG_ASSERT(!pGlosHdl->FindGroupName(sNewName), "Gruppe bereits vorhanden!");

    // befindet sich der umzubenennende Name unter den
    // den neuen - dann austauschen
    BOOL bDone = FALSE;
    if(pInsertedArr && pInsertedArr->Count())
    {
        USHORT nCount = pInsertedArr->Count();
        for(USHORT i = 0; i < nCount; ++i)
        {
            const String* pTemp = (*pInsertedArr)[i];
            if(*pTemp == sEntry)
            {
                pInsertedArr->Remove(i);
                pInsertedArr->Insert(new String(sNewName), pInsertedArr->Count());
                bDone = TRUE;
                break;
            }
        }
    }
    if(!bDone)
    {
        if(!pRenamedArr)
            pRenamedArr = new SvStrings;
        sEntry += RENAME_TOKEN_DELIM;
        sEntry += sNewName;
        sEntry += RENAME_TOKEN_DELIM;
        sEntry += sNewTitle;
        pRenamedArr->Insert(new String(sEntry), pRenamedArr->Count());
    }
    delete (GlosBibUserData*)pEntry->GetUserData();
    aGroupTLB.GetModel()->Remove(pEntry);
    String sTemp(aNameED.GetText());
//	sTemp.ToLower();
    sTemp += '\t';
    sTemp += aPathLB.GetSelectEntry();
    pEntry = aGroupTLB.InsertEntry(sTemp);
    GlosBibUserData* pData = new GlosBibUserData;
    pData->sPath = aPathLB.GetSelectEntry();
    pData->sGroupName = sNewName;
    pData->sGroupTitle = sNewTitle;
    pEntry->SetUserData(pData);
    aGroupTLB.Select(pEntry);
    aGroupTLB.MakeVisible(pEntry);
    aGroupTLB.GetModel()->Resort();
    return 0;
}
/*-----------------09.06.97 13:42-------------------

--------------------------------------------------*/
IMPL_LINK( SwGlossaryGroupDlg, ModifyHdl, Edit*, EMPTYARG )
{
    String sEntry(aNameED.GetText());
//	sEntry.ToLower();
    BOOL bEnableNew = TRUE;
    BOOL bEnableDel = FALSE;
    ULONG nCaseReadonly =
            (ULONG)aPathLB.GetEntryData(aPathLB.GetSelectEntryPos());
    BOOL bDirReadonly = 0 != (nCaseReadonly&PATH_READONLY);

    if(!sEntry.Len() || bDirReadonly)
        bEnableNew = FALSE;
    else if(sEntry.Len())
    {
        ULONG nPos = 0xffffffff;


        nPos = aGroupTLB.GetEntryPos(sEntry, 0);
        //ist es nicht case sensitive muss man selbst suchen
        if( 0xffffffff == nPos)
        {
            const ::utl::TransliterationWrapper& rSCmp = GetAppCmpStrIgnore();
            for(USHORT i = 0; i < aGroupTLB.GetEntryCount(); i++)
            {
                String sTemp = aGroupTLB.GetEntryText( i, 0 );
                nCaseReadonly = (ULONG)aPathLB.GetEntryData(
                    aPathLB.GetEntryPos(aGroupTLB.GetEntryText(i,1)));
                BOOL bCase = 0 != (nCaseReadonly & PATH_CASE_SENSITIVE);

                if( !bCase && rSCmp.isEqual( sTemp, sEntry ))
                {
                    nPos = i;
                    break;
                }
            }
        }
        if( 0xffffffff > nPos)
        {
            bEnableNew = FALSE;
            aGroupTLB.Select(aGroupTLB.GetEntry( nPos ));
            aGroupTLB.MakeVisible(aGroupTLB.GetEntry( nPos ));
        }
    }
    SvLBoxEntry* pEntry = aGroupTLB.FirstSelected();
    if(pEntry)
    {
        GlosBibUserData* pUserData = (GlosBibUserData*)pEntry->GetUserData();
        bEnableDel = IsDeleteAllowed(pUserData->sGroupName);

//		String sGroup = aGroupTLB.GetEntryText(pEntry, 0);
//		sGroup += GLOS_DELIM;
//		sGroup += String::CreateFromInt32(aPathLB.GetEntryPos(aGroupTLB.GetEntryText(pEntry, 1)));
//		bEnableDel = IsDeleteAllowed(sGroup);
    }

    aDelPB.Enable(bEnableDel);
    aNewPB.Enable(bEnableNew);
    aRenamePB.Enable(bEnableNew && pEntry);
    return 0;
}

/*------------------------------------------------------------------------
 Beschreibung:
------------------------------------------------------------------------*/

BOOL SwGlossaryGroupDlg::IsDeleteAllowed(const String &rGroup)
{
    BOOL bDel = (!pGlosHdl->IsReadOnly(&rGroup));

    // OM: befindet sich der Name unter den den neuen Bereichsnamen,
    // dann ist er auch loeschbar! Bei noch nicht existenten Bereichsnamen
    // liefert ReadOnly naemlich TRUE.

    if(pInsertedArr && pInsertedArr->Count())
    {
        USHORT nCount = pInsertedArr->Count();
        for(USHORT i = 0; i < nCount; ++i)
        {
            const String* pTemp = (*pInsertedArr)[i];
            if(*pTemp == rGroup)
            {
                bDel = TRUE;
                break;
            }
        }
    }

    return bDel;
}

/*-----------------18.07.97 19:06-------------------

--------------------------------------------------*/
void FEdit::KeyInput( const KeyEvent& rKEvent )
{
    KeyCode aCode = rKEvent.GetKeyCode();
    if( KEYGROUP_CURSOR == aCode.GetGroup() ||
        ( KEYGROUP_MISC == aCode.GetGroup() &&
          KEY_DELETE >= aCode.GetCode() ) ||
        SVT_SEARCHPATH_DELIMITER != rKEvent.GetCharCode() )
        Edit::KeyInput( rKEvent );
}
/* -----------------------------08.02.00 15:07--------------------------------

 ---------------------------------------------------------------------------*/
void 	SwGlossaryGroupTLB::RequestHelp( const HelpEvent& rHEvt )
{
    Point aPos( ScreenToOutputPixel( rHEvt.GetMousePosPixel() ));
    SvLBoxEntry* pEntry = GetEntry( aPos );
    if(pEntry)
    {
        SvLBoxTab* pTab;
        SvLBoxItem* pItem = GetItem( pEntry, aPos.X(), &pTab );
        if(pItem)
        {
            aPos = GetEntryPosition( pEntry );
            Size aSize(pItem->GetSize( this, pEntry ));
            aPos.X() = GetTabPos( pEntry, pTab );

            if((aPos.X() + aSize.Width()) > GetSizePixel().Width())
                aSize.Width() = GetSizePixel().Width() - aPos.X();
            aPos = OutputToScreenPixel(aPos);
            Rectangle aItemRect( aPos, aSize );
            String sMsg;
            GlosBibUserData* pData = (GlosBibUserData*)pEntry->GetUserData();
            sMsg = pData->sPath;
            sMsg += INET_PATH_TOKEN;
            sMsg += pData->sGroupName.GetToken(0, GLOS_DELIM);
            sMsg += SwGlossaries::GetExtension();

            Help::ShowQuickHelp( this, aItemRect, sMsg,
                        QUICKHELP_LEFT|QUICKHELP_VCENTER );
        }
    }
}
