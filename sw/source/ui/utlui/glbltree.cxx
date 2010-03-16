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

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#endif
#include <vcl/msgbox.hxx>
#include <svl/stritem.hxx>
#include <sfx2/fcontnr.hxx>
#include <sfx2/linkmgr.hxx>
#include <sfx2/dispatch.hxx>
#include <svl/urihelper.hxx>
#include <sfx2/docfile.hxx>
#include <vcl/help.hxx>
#include <sot/filelist.hxx>
#include <svl/eitem.hxx>
#include <svl/urlbmk.hxx>
#include <svtools/filter.hxx>
#include <sfx2/docinsert.hxx>
#include <sfx2/filedlghelper.hxx>

#include <sfx2/app.hxx>
#include <swmodule.hxx>
#include <wrtsh.hxx>
#include <view.hxx>
#include <errhdl.hxx>
#include <docsh.hxx>
#include <content.hxx>
#include <edglbldc.hxx>
#include <section.hxx>
#include <tox.hxx>
#include <cnttab.hxx>
#define NAVIPI_CXX
#include <navipi.hxx>
#include <navicont.hxx>
#include <edtwin.hxx>
#include <uitool.hxx>

#include <cmdid.h>
#include <helpid.h>
#ifndef _NAVIPI_HRC
#include <navipi.hrc>
#endif
#ifndef _UTLUI_HRC
#include <utlui.hrc>
#endif
#ifndef _COMCORE_HRC
#include <comcore.hrc>
#endif
#include <globals.hrc>
#include "swabstdlg.hxx"

using namespace ::com::sun::star::uno;
using ::rtl::OUString;


// Kontextmenue fuer GlobalTree
#define CTX_INSERT_ANY_INDEX 10
//#define CTX_INSERT_CNTIDX   11
//#define CTX_INSERT_USRIDX   12
#define CTX_INSERT_FILE     11
#define CTX_INSERT_NEW_FILE 12
#define CTX_INSERT_TEXT     13

#define CTX_UPDATE_SEL		20
#define CTX_UPDATE_INDEX    21
#define CTX_UPDATE_LINK     22
#define CTX_UPDATE_ALL      23

#define CTX_UPDATE			1
#define CTX_INSERT          2
#define CTX_EDIT            3
#define CTX_DELETE          4
#define CTX_EDIT_LINK       5

#define GLOBAL_UPDATE_TIMEOUT 2000

// Flags fuer PopupMenu-enable/disable
#define ENABLE_INSERT_IDX	0x0001
#define ENABLE_INSERT_FILE  0x0002
#define ENABLE_INSERT_TEXT  0x0004
#define ENABLE_EDIT         0x0008
#define ENABLE_DELETE       0x0010
#define ENABLE_UPDATE		0x0020
#define ENABLE_UPDATE_SEL   0x0040
#define ENABLE_EDIT_LINK    0x0080

// TabPos nach links schieben
#define  GLBL_TABPOS_SUB 5

const SfxObjectShell* SwGlobalTree::pShowShell = 0;
static const USHORT __FAR_DATA aHelpForMenu[] =
{
    0, 							//
    HID_GLBLTREE_UPDATE, 		//CTX_UPDATE
    HID_GLBLTREE_INSERT,		//CTX_INSERT
    HID_GLBLTREE_EDIT,			//CTX_EDIT
    HID_GLBLTREE_DEL,			//CTX_DELETE
    HID_GLBLTREE_EDIT_LINK,     //CTX_EDIT_LINK
    0,						  //
    0,						  //
    0,						  //
    0,						  //
    HID_GLBLTREE_INS_IDX, 		//CTX_INSERT_ANY_INDEX
    HID_GLBLTREE_INS_FILE,		//CTX_INSERT_FILE
    HID_GLBLTREE_INS_NEW_FILE,	//CTX_INSERT_NEW_FILE
    HID_GLBLTREE_INS_TEXT,		//CTX_INSERT_TEXT
    0,							//
    0,							//
    0,							//
    0,							//
    0,							//
    0,							//
    HID_GLBLTREE_UPD_SEL, 		//CTX_UPDATE_SEL
    HID_GLBLTREE_UPD_IDX, 		//CTX_UPDATE_INDEX
    HID_GLBLTREE_UPD_LINK,		//CTX_UPDATE_LINK
    HID_GLBLTREEUPD_ALL			//CTX_UPDATE_ALL
};

/************************************************************************/
/*																		*/
/************************************************************************/
/* -----------------------------24.08.00 12:04--------------------------------

 ---------------------------------------------------------------------------*/
class SwGlobalFrameListener_Impl : public SfxListener
{
    BOOL bValid;
public:
    SwGlobalFrameListener_Impl(SfxViewFrame& rFrame) :
        bValid(TRUE)
        {
            StartListening(rFrame);
        }

    virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    BOOL 				IsValid() const {return bValid;}
};
/* -----------------------------24.08.00 12:05--------------------------------

 ---------------------------------------------------------------------------*/
void    SwGlobalFrameListener_Impl::Notify( SfxBroadcaster& /*rBC*/, const SfxHint& rHint )
{
    if( rHint.ISA(SfxSimpleHint) &&
            (((SfxSimpleHint&) rHint).GetId() == SFX_HINT_DYING))
        bValid = FALSE;
}

/*-----------------12.06.97 09:38-------------------

--------------------------------------------------*/
SwGlobalTree::SwGlobalTree(Window* pParent, const ResId& rResId) :

    SvTreeListBox(pParent, rResId),

    pActiveShell        ( NULL ),
    pEmphasisEntry      ( NULL ),
    pDDSource           ( NULL ),
    pSwGlblDocContents  ( NULL ),
    pDefParentWin       ( NULL ),
    pDocContent         ( NULL ),
    pDocInserter        ( NULL ),

    bIsInternalDrag         ( FALSE ),
    bLastEntryEmphasis      ( FALSE ),
    bIsImageListInitialized ( FALSE )

{
    SetDragDropMode(SV_DRAGDROP_APP_COPY  |
                    SV_DRAGDROP_CTRL_MOVE |
                    SV_DRAGDROP_ENABLE_TOP );

    aUpdateTimer.SetTimeout(GLOBAL_UPDATE_TIMEOUT);
    aUpdateTimer.SetTimeoutHdl(LINK(this, SwGlobalTree, Timeout));
    aUpdateTimer.Start();
    for(USHORT i = 0; i < GLOBAL_CONTEXT_COUNT; i++)
    {
        aContextStrings[i] = SW_RESSTR(i+ ST_GLOBAL_CONTEXT_FIRST);
    }
    SetHelpId(HID_NAVIGATOR_GLOB_TREELIST);
    SelectHdl();
    SetDoubleClickHdl(LINK(this, SwGlobalTree, DoubleClickHdl));
    EnableContextMenuHandling();
}

/*-----------------12.06.97 09:38-------------------

--------------------------------------------------*/
SwGlobalTree::~SwGlobalTree()
{
    delete pSwGlblDocContents;
    delete pDocInserter;
}

/*-----------------12.06.97 09:38-------------------

--------------------------------------------------*/
sal_Int8 SwGlobalTree::ExecuteDrop( const ExecuteDropEvent& rEvt )
{
    sal_Int8 nRet = DND_ACTION_NONE;
    SvLBoxEntry* pLast = (SvLBoxEntry*)LastVisible();
    if(pEmphasisEntry)
    {
        ImplShowTargetEmphasis( Prev(pEmphasisEntry), FALSE );
        pEmphasisEntry = 0;
    }
    else if(bLastEntryEmphasis && pLast)
    {
        ImplShowTargetEmphasis( pLast, FALSE);
    }

    SvLBoxEntry* pDropEntry = bLastEntryEmphasis ? 0 : GetEntry(rEvt.maPosPixel);
    if( bIsInternalDrag )
    {
        SvLBoxEntry* pDummy = 0;
        ULONG nInsertionPos = LIST_APPEND;
        NotifyMoving( pDropEntry, pDDSource, pDummy, nInsertionPos );
    }
    else
    {
        TransferableDataHelper aData( rEvt.maDropEvent.Transferable );

        String sFileName;
        const SwGlblDocContent* pCnt = pDropEntry ?
                    (const SwGlblDocContent*)pDropEntry->GetUserData() :
                            0;
        if( aData.HasFormat( FORMAT_FILE_LIST ))
        {
            nRet = rEvt.mnAction;
            SwGlblDocContents* pTempContents = new SwGlblDocContents;
            int nAbsContPos = pDropEntry ?
                                (int) GetModel()->GetAbsPos(pDropEntry):
                                    - 1;
            USHORT nEntryCount = (USHORT)GetEntryCount();

            // Daten holen
            FileList aFileList;
            aData.GetFileList( FORMAT_FILE_LIST, aFileList );
            for ( USHORT n = (USHORT)aFileList.Count(); n--; )
            {
                sFileName = aFileList.GetFile(n);
                InsertRegion(pCnt, &sFileName);
                // nach dem Einfuegen muss die Liste der Contents neu
                // geholt werden, um nicht auf einem alten Content zu
                // arbeiten
                if(n)
                {
                    pActiveShell->GetGlobalDocContent(*pTempContents);
                    // wenn das file erfolgreich eingefuegt wurde,
                    // dann muss auch der naechste Content geholt werden
                    if(nEntryCount < pTempContents->Count())
                    {
                        nEntryCount++;
                        nAbsContPos++;
                        pCnt = pTempContents->GetObject( static_cast< USHORT >(nAbsContPos) );
                    }
                }
            }
            delete pTempContents;
        }
        else if( 0 != (sFileName =
                        SwNavigationPI::CreateDropFileName( aData )).Len())
        {
            INetURLObject aTemp(sFileName);
            GraphicDescriptor aDesc(aTemp);
            if( !aDesc.Detect() )	// keine Grafiken annehmen
            {
                nRet = rEvt.mnAction;
                InsertRegion(pCnt, &sFileName);
            }
        }
    }
    bLastEntryEmphasis = FALSE;
    return nRet;

}
/*-----------------12.06.97 09:38-------------------

--------------------------------------------------*/
sal_Int8 SwGlobalTree::AcceptDrop( const AcceptDropEvent& rEvt )
{
    sal_Int8 nRet = rEvt.mnAction;

    //initiate scrolling
    GetDropTarget( rEvt.maPosPixel );
    SvLBoxEntry* pLast = (SvLBoxEntry*)LastVisible();
    if( rEvt.mbLeaving )
    {
        if( pEmphasisEntry )
        {
            ImplShowTargetEmphasis( Prev(pEmphasisEntry), FALSE );
            pEmphasisEntry = 0;
        }
        else if(bLastEntryEmphasis && pLast)
        {
            ImplShowTargetEmphasis( pLast, FALSE);
        }
        bLastEntryEmphasis = FALSE;
    }
    else
    {
        SvLBoxEntry* pDropEntry = GetEntry( rEvt.maPosPixel );
        if(bIsInternalDrag)
        {
            if( pDDSource != pDropEntry )
                nRet = rEvt.mnAction;
        }
        else if( IsDropFormatSupported( FORMAT_FILE ) ||
                  IsDropFormatSupported( FORMAT_STRING ) ||
                  IsDropFormatSupported( FORMAT_FILE_LIST ) ||
                  IsDropFormatSupported( SOT_FORMATSTR_ID_SOLK ) ||
                   IsDropFormatSupported( SOT_FORMATSTR_ID_NETSCAPE_BOOKMARK )||
                   IsDropFormatSupported( SOT_FORMATSTR_ID_FILECONTENT ) ||
                   IsDropFormatSupported( SOT_FORMATSTR_ID_FILEGRPDESCRIPTOR ) ||
                   IsDropFormatSupported( SOT_FORMATSTR_ID_UNIFORMRESOURCELOCATOR ) ||
                   IsDropFormatSupported( SOT_FORMATSTR_ID_FILENAME ))
                nRet = DND_ACTION_LINK;

        if(pEmphasisEntry && pEmphasisEntry != pDropEntry)
            ImplShowTargetEmphasis( Prev(pEmphasisEntry), FALSE );
        else if(pLast && bLastEntryEmphasis  && pDropEntry)
        {
            ImplShowTargetEmphasis( pLast, FALSE);
            bLastEntryEmphasis = FALSE;
        }

        if(pDropEntry)
            ImplShowTargetEmphasis( Prev(pDropEntry), DND_ACTION_NONE != nRet );
        else if(pLast)
        {
            ImplShowTargetEmphasis( pLast, DND_ACTION_NONE != nRet );
            bLastEntryEmphasis = TRUE;
        }
        pEmphasisEntry = pDropEntry;
    }
    return nRet;
}
/*-----------------12.06.97 09:38-------------------

--------------------------------------------------*/
PopupMenu* SwGlobalTree::CreateContextMenu()
{
    PopupMenu* pPop = 0;
    if(pActiveShell &&
        !pActiveShell->GetView().GetDocShell()->IsReadOnly())
    {
        USHORT nEnableFlags = GetEnableFlags();
        pPop = new PopupMenu;
        PopupMenu* pSubPop1 = new PopupMenu;
        PopupMenu* pSubPop2 = new PopupMenu;

        for (USHORT i = CTX_UPDATE_SEL; i <= CTX_UPDATE_ALL; i++)
        {
            pSubPop2->InsertItem( i, aContextStrings[ST_UPDATE_SEL - ST_GLOBAL_CONTEXT_FIRST - CTX_UPDATE_SEL+ i] );
            pSubPop2->SetHelpId(i, aHelpForMenu[i]);
        }
        pSubPop2->EnableItem(CTX_UPDATE_SEL, 0 != (nEnableFlags & ENABLE_UPDATE_SEL));

        pSubPop1->InsertItem(CTX_INSERT_ANY_INDEX, aContextStrings[ST_INDEX  - ST_GLOBAL_CONTEXT_FIRST]);
        pSubPop1->SetHelpId(CTX_INSERT_ANY_INDEX, aHelpForMenu[CTX_INSERT_ANY_INDEX]);
        pSubPop1->InsertItem(CTX_INSERT_FILE, aContextStrings[ST_FILE   - ST_GLOBAL_CONTEXT_FIRST]);
        pSubPop1->SetHelpId(CTX_INSERT_FILE, aHelpForMenu[CTX_INSERT_FILE]);
        pSubPop1->InsertItem(CTX_INSERT_NEW_FILE, aContextStrings[ST_NEW_FILE   - ST_GLOBAL_CONTEXT_FIRST]);
        pSubPop1->SetHelpId(CTX_INSERT_NEW_FILE, aHelpForMenu[CTX_INSERT_NEW_FILE]);
        pSubPop1->InsertItem(CTX_INSERT_TEXT, aContextStrings[ST_TEXT   - ST_GLOBAL_CONTEXT_FIRST]);
        pSubPop1->SetHelpId(CTX_INSERT_TEXT, aHelpForMenu[CTX_INSERT_TEXT]);


        pPop->InsertItem(CTX_UPDATE, aContextStrings[ST_UPDATE - ST_GLOBAL_CONTEXT_FIRST]);
        pPop->SetHelpId(CTX_UPDATE, aHelpForMenu[CTX_UPDATE]);
        pPop->InsertItem(CTX_EDIT, aContextStrings[ST_EDIT_CONTENT - ST_GLOBAL_CONTEXT_FIRST]);
        pPop->SetHelpId(CTX_EDIT, aHelpForMenu[CTX_EDIT]);
        if(nEnableFlags&ENABLE_EDIT_LINK)
        {
            pPop->InsertItem(CTX_EDIT_LINK, aContextStrings[ST_EDIT_LINK - ST_GLOBAL_CONTEXT_FIRST]);
            pPop->SetHelpId(CTX_EDIT_LINK, aHelpForMenu[CTX_EDIT_LINK]);
        }
        pPop->InsertItem(CTX_INSERT, aContextStrings[ST_INSERT - ST_GLOBAL_CONTEXT_FIRST]);
        pPop->SetHelpId(CTX_INSERT, aHelpForMenu[CTX_INSERT]);
        pPop->InsertSeparator() ;
        pPop->InsertItem(CTX_DELETE, aContextStrings[ST_DELETE - ST_GLOBAL_CONTEXT_FIRST]);
        pPop->SetHelpId(CTX_DELETE, aHelpForMenu[CTX_DELETE]);

        //evtl. disablen
        pSubPop1->EnableItem(CTX_INSERT_ANY_INDEX,  0 != (nEnableFlags & ENABLE_INSERT_IDX ));
        pSubPop1->EnableItem(CTX_INSERT_TEXT,       0 != (nEnableFlags & ENABLE_INSERT_TEXT));
        pSubPop1->EnableItem(CTX_INSERT_FILE,       0 != (nEnableFlags & ENABLE_INSERT_FILE));
        pSubPop1->EnableItem(CTX_INSERT_NEW_FILE,   0 != (nEnableFlags & ENABLE_INSERT_FILE));

        pPop->EnableItem(CTX_UPDATE,    0 != (nEnableFlags & ENABLE_UPDATE));
        pPop->EnableItem(CTX_INSERT,    0 != (nEnableFlags & ENABLE_INSERT_IDX));
        pPop->EnableItem(CTX_EDIT,      0 != (nEnableFlags & ENABLE_EDIT));
        pPop->EnableItem(CTX_DELETE,    0 != (nEnableFlags & ENABLE_DELETE));


        pPop->SetPopupMenu( CTX_INSERT, pSubPop1 );
        pPop->SetPopupMenu( CTX_UPDATE, pSubPop2 );
    }
    return pPop;
}
/*-----------------16.06.97 10:41-------------------

--------------------------------------------------*/
void SwGlobalTree::TbxMenuHdl(USHORT nTbxId, ToolBox* pBox)
{
    USHORT nEnableFlags = GetEnableFlags();
    if(FN_GLOBAL_OPEN == nTbxId)
    {
        PopupMenu *pMenu = new PopupMenu;
        for (USHORT i = CTX_INSERT_ANY_INDEX; i <= CTX_INSERT_TEXT; i++)
        {
            pMenu->InsertItem( i, aContextStrings[ST_INDEX  - ST_GLOBAL_CONTEXT_FIRST - CTX_INSERT_ANY_INDEX + i] );
            pMenu->SetHelpId(i, aHelpForMenu[i] );
        }
        pMenu->EnableItem(CTX_INSERT_ANY_INDEX, 0 != (nEnableFlags & ENABLE_INSERT_IDX ));
//      pMenu->EnableItem(CTX_INSERT_CNTIDX,    0 != (nEnableFlags & ENABLE_INSERT_IDX ));
//      pMenu->EnableItem(CTX_INSERT_USRIDX,    0 != (nEnableFlags & ENABLE_INSERT_IDX ));
        pMenu->EnableItem(CTX_INSERT_TEXT,      0 != (nEnableFlags & ENABLE_INSERT_TEXT));
        pMenu->EnableItem(CTX_INSERT_FILE,      0 != (nEnableFlags & ENABLE_INSERT_FILE));
        pMenu->EnableItem(CTX_INSERT_NEW_FILE,  0 != (nEnableFlags & ENABLE_INSERT_FILE));
        pMenu->SetSelectHdl(LINK(this, SwGlobalTree, PopupHdl));
        pMenu->Execute(	pBox, pBox->GetItemRect(nTbxId).BottomLeft());
        delete pMenu;
        pBox->EndSelection();
        pBox->Invalidate();
    }
    else if(FN_GLOBAL_UPDATE == nTbxId)
    {
        PopupMenu *pMenu = new PopupMenu;
        for (USHORT i = CTX_UPDATE_SEL; i <= CTX_UPDATE_ALL; i++)
        {
            pMenu->InsertItem( i, aContextStrings[ST_UPDATE_SEL - ST_GLOBAL_CONTEXT_FIRST - CTX_UPDATE_SEL+ i] );
            pMenu->SetHelpId(i, aHelpForMenu[i] );
        }
        pMenu->EnableItem(CTX_UPDATE_SEL, 0 != (nEnableFlags & ENABLE_UPDATE_SEL));
        pMenu->SetSelectHdl(LINK(this, SwGlobalTree, PopupHdl));
        pMenu->Execute(	pBox, pBox->GetItemRect(nTbxId).BottomLeft());
        delete pMenu;
        pBox->EndSelection();
        pBox->Invalidate();
    }
}
/*-----------------16.06.97 11:02-------------------

--------------------------------------------------*/
USHORT	SwGlobalTree::GetEnableFlags() const
{
    SvLBoxEntry* pEntry = FirstSelected();
    USHORT nSelCount = (USHORT)GetSelectionCount();
    USHORT nEntryCount = (USHORT)GetEntryCount();
    SvLBoxEntry* pPrevEntry = pEntry ? Prev(pEntry) : 0;

    USHORT nRet = 0;
    if(nSelCount == 1 || !nEntryCount)
        nRet |= ENABLE_INSERT_IDX|ENABLE_INSERT_FILE;
    if(nSelCount == 1)
    {
        nRet |= ENABLE_EDIT;
        if(	((SwGlblDocContent*)pEntry->GetUserData())->GetType() != GLBLDOC_UNKNOWN &&
                    (!pPrevEntry || ((SwGlblDocContent*)pPrevEntry->GetUserData())->GetType() != GLBLDOC_UNKNOWN))
            nRet |= ENABLE_INSERT_TEXT;
        if( GLBLDOC_SECTION == ((SwGlblDocContent*)pEntry->GetUserData())->GetType() )
            nRet |= ENABLE_EDIT_LINK;
    }
    else if(!nEntryCount)
    {
        nRet |= ENABLE_INSERT_TEXT;
    }
    if(nEntryCount)
        nRet |= ENABLE_UPDATE|ENABLE_DELETE;
    if(nSelCount)
        nRet |= ENABLE_UPDATE_SEL;
    return nRet;
}

/*-----------------12.06.97 09:38-------------------

--------------------------------------------------*/
void	 SwGlobalTree::RequestHelp( const HelpEvent& rHEvt )
{
    BOOL bParent = TRUE;
    Update(TRUE);
    Display(TRUE);
    if( rHEvt.GetMode() & HELPMODE_QUICK )
    {
        Point aPos( ScreenToOutputPixel( rHEvt.GetMousePosPixel() ));
        SvLBoxEntry* pEntry = GetEntry( aPos );
        const SwGlblDocContent* pCont = pEntry ?
                            (const SwGlblDocContent*)pEntry->GetUserData() : 0;
        if( pCont &&  GLBLDOC_SECTION == pCont->GetType())
        {
            bParent = FALSE;
            SvLBoxTab* pTab;
            SvLBoxItem* pItem = GetItem( pEntry, aPos.X(), &pTab );
            if(pItem && SV_ITEM_ID_LBOXSTRING == pItem->IsA())
            {
                const SwSection* pSect = pCont->GetSection();
                String sEntry = pSect->GetLinkFileName().GetToken(0, sfx2::cTokenSeperator);
                if(!pSect->IsConnectFlag())
                    sEntry.Insert(aContextStrings[ST_BROKEN_LINK - ST_GLOBAL_CONTEXT_FIRST], 0 );
                Point aEntryPos = GetEntryPosition( pEntry );

                aEntryPos.X() = GetTabPos( pEntry, pTab );
                Size aSize( pItem->GetSize( this, pEntry ) );

                if((aEntryPos.X() + aSize.Width()) > GetSizePixel().Width())
                    aSize.Width() = GetSizePixel().Width() - aEntryPos.X();

                aEntryPos = OutputToScreenPixel(aEntryPos);
                Rectangle aItemRect( aEntryPos, aSize );
                if(Help::IsBalloonHelpEnabled())
                {
                    aEntryPos.X() += aSize.Width();
                    Help::ShowBalloon( this, aEntryPos, aItemRect, sEntry );
                }
                else
                    Help::ShowQuickHelp( this, aItemRect, sEntry,
                        QUICKHELP_LEFT|QUICKHELP_VCENTER );
            }
        }
    }

    if(bParent)
        SvTreeListBox::RequestHelp(rHEvt);
}
/*-----------------16.06.97 16:15-------------------

--------------------------------------------------*/
void	 SwGlobalTree::SelectHdl()
{

    USHORT nSelCount = (USHORT)GetSelectionCount();
    SvLBoxEntry* pSel = FirstSelected();
    USHORT nAbsPos = pSel ? (USHORT)GetModel()->GetAbsPos(pSel) : 0;
    SwNavigationPI* pNavi = GetParentWindow();
    BOOL bReadonly = !pActiveShell ||
                pActiveShell->GetView().GetDocShell()->IsReadOnly();
    pNavi->aGlobalToolBox.EnableItem(FN_GLOBAL_EDIT,  nSelCount == 1 && !bReadonly);
    pNavi->aGlobalToolBox.EnableItem(FN_GLOBAL_OPEN,  nSelCount <= 1 && !bReadonly);
    pNavi->aGlobalToolBox.EnableItem(FN_GLOBAL_UPDATE,  GetEntryCount() > 0 && !bReadonly);
    pNavi->aGlobalToolBox.EnableItem(FN_ITEM_UP,
                    nSelCount == 1 && nAbsPos && !bReadonly);
    pNavi->aGlobalToolBox.EnableItem(FN_ITEM_DOWN,
                    nSelCount == 1 && nAbsPos < ((USHORT)GetEntryCount()) - 1 && !bReadonly);

}
/*-----------------16.06.97 16:15-------------------

--------------------------------------------------*/
void	 SwGlobalTree::DeselectHdl()
{
    SelectHdl();
}

/*-----------------17.06.97 13:11-------------------

--------------------------------------------------*/
DragDropMode SwGlobalTree::NotifyStartDrag( TransferDataContainer& ,
                                                SvLBoxEntry* pEntry )
{
    bIsInternalDrag = TRUE;
    pDDSource = pEntry;
    return SV_DRAGDROP_CTRL_MOVE;
}

/*-----------------21.06.97 12:44-------------------

--------------------------------------------------*/
long     SwGlobalTree::GetTabPos( SvLBoxEntry*, SvLBoxTab* pTab)
{
    return pTab->GetPos() - GLBL_TABPOS_SUB;
}

/*-----------------12.06.97 09:38-------------------

--------------------------------------------------*/
BOOL     SwGlobalTree::NotifyMoving(   SvLBoxEntry*  pTarget,
                                        SvLBoxEntry*  pSource,
                                        SvLBoxEntry*&,
                                        ULONG&
                                    )
{
    SvTreeList* _pModel = GetModel();
    USHORT nSource = (USHORT) _pModel->GetAbsPos(pSource);
    USHORT nDest   = pTarget ? (USHORT) _pModel->GetAbsPos(pTarget) : pSwGlblDocContents->Count();

    if(	pActiveShell->MoveGlobalDocContent(
            *pSwGlblDocContents, nSource, nSource + 1, nDest ) &&
            Update( FALSE ))
        Display();
    return FALSE;
}
/*-----------------12.06.97 09:39-------------------

--------------------------------------------------*/
BOOL     SwGlobalTree::NotifyCopying(  SvLBoxEntry*  /*pTarget*/,
                                        SvLBoxEntry*  /*pEntry*/,
                                        SvLBoxEntry*& /*rpNewParent*/,
                                        ULONG&        /*rNewChildPos*/
                                    )
{
    return FALSE;
}
/*-----------------12.06.97 09:39-------------------

--------------------------------------------------*/
BOOL SwGlobalTree::NotifyAcceptDrop( SvLBoxEntry* pEntry)
{
    return pEntry != 0;
}
/*-----------------12.06.97 09:39-------------------

--------------------------------------------------*/
void SwGlobalTree::StartDrag( sal_Int8 nAction, const Point& rPt )
{
    if( 1 == GetSelectionCount() )
        SvTreeListBox::StartDrag( nAction, rPt );
}
/*-----------------12.06.97 09:39-------------------

--------------------------------------------------*/
void SwGlobalTree::DragFinished( sal_Int8 nAction )
{
    SvTreeListBox::DragFinished( nAction );
    bIsInternalDrag = FALSE;
}

/***************************************************************************
    Beschreibung: 	Wird ein Ctrl+DoubleClick in einen freien Bereich ausgefuehrt,
 * 					dann soll die Basisfunktion des Controls gerufen werden
***************************************************************************/
void  SwGlobalTree::MouseButtonDown( const MouseEvent& rMEvt )
{
    Point aPos( rMEvt.GetPosPixel());
    SvLBoxEntry* pEntry = GetEntry( aPos, TRUE );
    if( !pEntry && rMEvt.IsLeft() && rMEvt.IsMod1() && (rMEvt.GetClicks() % 2) == 0)
        Control::MouseButtonDown( rMEvt );
    else
        SvTreeListBox::MouseButtonDown( rMEvt );
}

/*-----------------12.06.97 13:08-------------------

--------------------------------------------------*/
void     SwGlobalTree::GetFocus()
{
    if(Update( FALSE ))
        Display();
    SvTreeListBox::GetFocus();
}

/*-----------------12.06.97 12:34-------------------

--------------------------------------------------*/
void     SwGlobalTree::KeyInput(const KeyEvent& rKEvt)
{
    const KeyCode aCode = rKEvt.GetKeyCode();
    if(aCode.GetCode() == KEY_RETURN)
    {
        switch(aCode.GetModifier())
        {
            case KEY_MOD2:
                // Boxen umschalten
                GetParentWindow()->ToggleTree();
            break;
        }
    }
    else
        SvTreeListBox::KeyInput(rKEvt);
}

/*-----------------17.06.97 11:59-------------------

--------------------------------------------------*/
void SwGlobalTree::Clear()
{
    pEmphasisEntry = 0;
    SvTreeListBox::Clear();
}
/*-----------------12.06.97 12:38-------------------

--------------------------------------------------*/
void 	SwGlobalTree::Display(BOOL bOnlyUpdateUserData)
{
    if(!bIsImageListInitialized)
    {
        USHORT nResId = GetSettings().GetStyleSettings().GetHighContrastMode() ? IMG_NAVI_ENTRYBMPH : IMG_NAVI_ENTRYBMP;
        aEntryImages = ImageList(SW_RES(nResId));
        bIsImageListInitialized = TRUE;
    }
    USHORT nCount = pSwGlblDocContents->Count();
    if(bOnlyUpdateUserData && GetEntryCount() == pSwGlblDocContents->Count())
    {
        SvLBoxEntry* pEntry = First();
        for( USHORT i = 0; i < nCount; i++)
        {
            SwGlblDocContentPtr pCont = pSwGlblDocContents->GetObject(i);
            pEntry->SetUserData(pCont);
            pEntry = Next(pEntry);
        }
    }
    else
    {
        SetUpdateMode( FALSE );
        SvLBoxEntry* pOldSelEntry = FirstSelected();
        String sEntryName;	// Name des Eintrags
        USHORT nSelPos = USHRT_MAX;
        if(pOldSelEntry)
        {
            sEntryName = GetEntryText(pOldSelEntry);
            nSelPos = (USHORT)GetModel()->GetAbsPos(pOldSelEntry);
        }
        Clear();
        if(!pSwGlblDocContents)
            Update( FALSE );

        SvLBoxEntry* pSelEntry = 0;
        for( USHORT i = 0; i < nCount; i++)
        {
            SwGlblDocContentPtr pCont = pSwGlblDocContents->GetObject(i);
            String sEntry;
            Image aImage;
            switch( pCont->GetType()  )
            {
                case GLBLDOC_UNKNOWN:
                {
                    sEntry = aContextStrings[ST_TEXT   - ST_GLOBAL_CONTEXT_FIRST];
                    aImage = aEntryImages.GetImage(SID_SW_START + GLOBAL_CONTENT_TEXT);
                }
                break;
                case GLBLDOC_TOXBASE:
                {
                    const SwTOXBase* pBase = pCont->GetTOX();
                    sEntry = pBase->GetTitle();
                    aImage = aEntryImages.GetImage(SID_SW_START + CONTENT_TYPE_INDEX);
                }
                break;
                case GLBLDOC_SECTION:
                {
                    const SwSection* pSect = pCont->GetSection();
                    sEntry = pSect->GetName();
                    aImage = aEntryImages.GetImage(SID_SW_START + CONTENT_TYPE_REGION);
                }
                break;
            }
            SvLBoxEntry* pEntry = InsertEntry(sEntry, aImage, aImage,
                        0, FALSE, LIST_APPEND, pCont);
            if(sEntry == sEntryName)
            {
                pSelEntry = pEntry;
            }
        }
        if(pSelEntry)
        {
            Select(pSelEntry);
        }
        else if(nSelPos != USHRT_MAX && nSelPos < nCount)
        {
            Select(GetEntry(nSelPos));
        }
        else if(nCount)
            Select(First());
        else
            SelectHdl();
        SetUpdateMode( TRUE );
    }
}

/*-----------------13.06.97 10:32-------------------

--------------------------------------------------*/
void SwGlobalTree::InsertRegion( const SwGlblDocContent* pCont, const String* pFileName )
{
    Sequence< OUString > aFileNames;
    if ( !pFileName )
    {
        pDefParentWin = Application::GetDefDialogParent();
        Application::SetDefDialogParent( this );
        if ( pDocInserter )
            delete pDocInserter;
        pDocInserter = new ::sfx2::DocumentInserter( 0, String::CreateFromAscii("swriter"), true );
        pDocInserter->StartExecuteModal( LINK( this, SwGlobalTree, DialogClosedHdl ) );
    }
    else if ( pFileName->Len() )
    {
        aFileNames.realloc(1);
        INetURLObject aFileName;
        aFileName.SetSmartURL( *pFileName );
        aFileNames.getArray()[0] = aFileName.GetMainURL( INetURLObject::NO_DECODE );
        InsertRegion( pCont, aFileNames );
    }
}

/*-----------------18.06.97 12:42-------------------

--------------------------------------------------*/
void 	SwGlobalTree::EditContent(const SwGlblDocContent* pCont )
{
    USHORT nSlot = 0;
    switch( pCont->GetType() )
    {
        case GLBLDOC_UNKNOWN:
            pActiveShell->GetView().GetEditWin().GrabFocus();
        break;
        case GLBLDOC_TOXBASE:
        {
            const SwTOXBase* pBase = pCont->GetTOX();
            if(pBase)
                nSlot = FN_INSERT_MULTI_TOX;
        }
        break;
        case GLBLDOC_SECTION:
        {
            OpenDoc(pCont);

            nSlot = 0;
            pCont = 0;
        }
        break;
    }
    if(pCont)
        GotoContent(pCont);
    if(nSlot)
    {
        pActiveShell->GetView().GetViewFrame()->GetDispatcher()->Execute(nSlot);
        if(Update( FALSE ))
            Display();
    }
}

/*-----------------13.06.97 14:22-------------------

--------------------------------------------------*/
IMPL_LINK( SwGlobalTree, PopupHdl, Menu* , pMenu)
{
    ExcecuteContextMenuAction( pMenu->GetCurItemId());
    return TRUE;
}
/* -----------------26.08.2003 11:57-----------------

 --------------------------------------------------*/
void    SwGlobalTree::ExcecuteContextMenuAction( USHORT nSelectedPopupEntry )
//IMPL_LINK( SwGlobalTree, PopupHdl, Menu* , pMenu)
{
//  USHORT nId = pMenu->GetCurItemId();
    SvLBoxEntry* pEntry = FirstSelected();
    SwGlblDocContent* pCont = pEntry ? (SwGlblDocContent*)pEntry->GetUserData() : 0;
    // wird waehrend des Dialogs ein RequestHelp gerufen,
    // dann geht der Content verloren. Deshalb wird hier eine
    // Kopie angelegt, in der nur die DocPos richtig gesetzt ist.
    SwGlblDocContent* pContCopy = 0;
    if(pCont)
        pContCopy = new SwGlblDocContent(pCont->GetDocPos());
    SfxDispatcher& rDispatch = *pActiveShell->GetView().GetViewFrame()->GetDispatcher();
    USHORT nSlot = 0;
    bool bDeleteContentCopy = true;
    switch( nSelectedPopupEntry )
    {
        case CTX_UPDATE_SEL:
        {
            // zwei Durchlaeufe: zuerst die Bereiche, dann die Verzeichnisse
            // aktualisieren
            SvLBoxEntry* pSelEntry = FirstSelected();
            while( pSelEntry )
            {
                SwGlblDocContent* pContent = (SwGlblDocContent*)pSelEntry->GetUserData();
                if(GLBLDOC_SECTION == pContent->GetType() &&
                    pContent->GetSection()->IsConnected())
                {
                    ((SwSection*)pContent->GetSection())->UpdateNow();
                }

                pSelEntry = NextSelected(pSelEntry);
            }
            pSelEntry = FirstSelected();
            while( pSelEntry )
            {
                SwGlblDocContent* pContent = (SwGlblDocContent*)pSelEntry->GetUserData();
                if(GLBLDOC_TOXBASE == pContent->GetType())
                    pActiveShell->UpdateTableOf(*pContent->GetTOX());
                pSelEntry = NextSelected(pSelEntry);
            }

        }
        break;
        case CTX_UPDATE_INDEX:
        {
            nSlot = FN_UPDATE_TOX;
        }
        break;
        case CTX_UPDATE_LINK:
        case CTX_UPDATE_ALL:
        {
            pActiveShell->GetLinkManager().UpdateAllLinks(TRUE);
            if(CTX_UPDATE_ALL == nSelectedPopupEntry)
                nSlot = FN_UPDATE_TOX;
            pCont = 0;
        }
        break;
        case CTX_EDIT:
        {
            DBG_ASSERT(pCont, "Edit ohne Entry ? " );
            EditContent(pCont);
        }
        break;
        case CTX_EDIT_LINK:
        {
            DBG_ASSERT(pCont, "Edit ohne Entry ? " );
            SfxStringItem aName(FN_EDIT_REGION, pCont->GetSection()->GetName());
            rDispatch.Execute(FN_EDIT_REGION, SFX_CALLMODE_ASYNCHRON, &aName, 0L);
        }
        break;
        case CTX_DELETE:
        {
            // sind mehrere Eintraege selektiert, dann muss nach jedem delete
            // das Array neu gefuellt werden. Damit man sich nichts merken muss,
            // beginnt das Loeschen am Ende
            SvLBoxEntry* pSelEntry = LastSelected();
            SwGlblDocContents* pTempContents  = 0;
            pActiveShell->StartAction();
            while(pSelEntry)
            {
                pActiveShell->DeleteGlobalDocContent(
                    pTempContents ? *pTempContents : *pSwGlblDocContents,
                                     (USHORT)GetModel()->GetAbsPos(pSelEntry));
                pSelEntry = PrevSelected(pSelEntry);
                if(pSelEntry)
                {
                    pTempContents = new SwGlblDocContents;
                    pActiveShell->GetGlobalDocContent(*pTempContents);
                }
            }
            delete pTempContents;
            pActiveShell->EndAction();
            pCont = 0;
        }
        break;
        case CTX_INSERT_ANY_INDEX:
//		case CTX_INSERT_CNTIDX:
//		case CTX_INSERT_USRIDX:
        {
            if(pContCopy)
            {
                SfxItemSet aSet(pActiveShell->GetView().GetPool(),
                            RES_COL, RES_COL,
                            RES_BACKGROUND, RES_BACKGROUND,
                            RES_FRM_SIZE, RES_FRM_SIZE,
                            SID_ATTR_PAGE_SIZE, SID_ATTR_PAGE_SIZE,
                            RES_LR_SPACE, RES_LR_SPACE,
                            FN_PARAM_TOX_TYPE, FN_PARAM_TOX_TYPE,
                            0);

                SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "Dialogdiet fail!");
                AbstractMultiTOXTabDialog* pDlg = pFact->CreateMultiTOXTabDialog( DLG_MULTI_TOX,
                                                        this, aSet,
                                                        *pActiveShell,
                                                        0,
                                                        USHRT_MAX,
                                                        TRUE);
                DBG_ASSERT(pDlg, "Dialogdiet fail!");
                if(RET_OK == pDlg->Execute())
                {
                    SwTOXDescription&  rDesc = pDlg->GetTOXDescription(
                                                pDlg->GetCurrentTOXType());
                    SwTOXMgr aMgr(pActiveShell);
                    SwTOXBase* pToInsert = 0;
                    if(aMgr.UpdateOrInsertTOX(rDesc, &pToInsert, pDlg->GetOutputItemSet()))
                        pActiveShell->InsertGlobalDocContent( *pContCopy, *pToInsert );
                }
                pCont = 0;
                delete pDlg;
            }
        }
        break;
        case CTX_INSERT_FILE:
        {
            bDeleteContentCopy = false;
            pDocContent = pContCopy;
            InsertRegion( pContCopy );
            pCont = NULL;
        }
        break;
        case CTX_INSERT_NEW_FILE:
        {
            SfxViewFrame* pGlobFrm = pActiveShell->GetView().GetViewFrame();
            SwGlobalFrameListener_Impl aFrmListener(*pGlobFrm);

            ULONG nEntryPos = pEntry ? GetModel()->GetAbsPos(pEntry) : (ULONG)-1;
            // neues Dok anlegen
            SfxStringItem aFactory(SID_NEWDOCDIRECT,
                            SwDocShell::Factory().GetFilterContainer()->GetName());

             const SfxFrameItem* pItem = (SfxFrameItem*)
                            rDispatch.Execute(SID_NEWDOCDIRECT,
                                SFX_CALLMODE_SYNCHRON, &aFactory, 0L);

            // sichern unter
            SfxFrame* pFrm = pItem ? pItem->GetFrame() : 0;
            SfxViewFrame* pFrame = pFrm ? pFrm->GetCurrentViewFrame() : 0;
            if( pFrame )
            {
                const SfxBoolItem* pBool = (const SfxBoolItem*)
                        pFrame->GetDispatcher()->Execute(
                                SID_SAVEASDOC, SFX_CALLMODE_SYNCHRON );
                SfxObjectShell& rObj = *pFrame->GetObjectShell();
                const SfxMedium* pMedium = rObj.GetMedium();
                String sNewFile(pMedium->GetURLObject().GetMainURL(INetURLObject::DECODE_TO_IURI));
                // Bereich mit dem Dok-Namen einfuegen
                // eigenes Dok in den Vordergrund

                if(aFrmListener.IsValid() && sNewFile.Len())
                {
                    pGlobFrm->ToTop();
                    // durch das Update sind die Eintraege invalid
                    if(nEntryPos != (ULONG)-1)
                    {
                        Update( FALSE );
                        Display();
                        Select(GetModel()->GetEntryAtAbsPos(nEntryPos));
                        pEntry = FirstSelected();
                        pCont = pEntry ? (SwGlblDocContent*)pEntry->GetUserData() : 0;
                    }
                    else
                    {
                        pEntry = 0;
                        pCont = 0;
                    }
                    if(pBool->GetValue())
                    {
                        InsertRegion(pCont, &sNewFile);
                        pFrame->ToTop();
                    }
                    else
                        pFrame->GetDispatcher()->Execute(SID_CLOSEWIN,
                                                SFX_CALLMODE_SYNCHRON);
                }
                else
                {
                    pFrame->ToTop();
                    return;
                }
            }
        }
        break;
        case CTX_INSERT_TEXT:
        {
            if(pCont)
                pActiveShell->InsertGlobalDocContent(*pCont);
            else
            {
                pActiveShell->SplitNode(); // leeres Dokument
                pActiveShell->Up( FALSE, 1 );
            }
            pActiveShell->GetView().GetEditWin().GrabFocus();
        }
        break;
        case CTX_UPDATE:
            pCont = 0;
        break;
        default:;
        // hier passiert nichts
    }
    if(pCont)
        GotoContent(pCont);
    if(nSlot)
        rDispatch.Execute(nSlot);
    if(Update( FALSE ))
        Display();
    if ( bDeleteContentCopy )
        delete pContCopy;
    else
        bDeleteContentCopy = true;
//  return TRUE;
}

/*-----------------16.06.97 07:57-------------------

--------------------------------------------------*/
IMPL_LINK( SwGlobalTree, Timeout, Timer*, EMPTYARG )
{
    if(!HasFocus() && Update( FALSE ))
        Display();
    return 0;
}

/*-----------------13.06.97 16:56-------------------

--------------------------------------------------*/
void SwGlobalTree::GotoContent(const SwGlblDocContent* pCont)
{
    pActiveShell->EnterStdMode();

    switch( pCont->GetType()  )
    {
        case GLBLDOC_UNKNOWN:
            pActiveShell->GotoGlobalDocContent(*pCont);
        break;
        case GLBLDOC_TOXBASE:
        {
            String sName = pCont->GetTOX()->GetTOXName();
            if (!pActiveShell->GotoNextTOXBase(&sName))
                pActiveShell->GotoPrevTOXBase(&sName);
        }
        break;
        case GLBLDOC_SECTION:
        break;
    }

}
/*-----------------16.06.97 07:42-------------------

--------------------------------------------------*/
void	SwGlobalTree::ShowTree()
{
    aUpdateTimer.Start();
    SvTreeListBox::Show();
}
/*-----------------16.06.97 07:42-------------------

--------------------------------------------------*/
void	SwGlobalTree::HideTree()
{
    aUpdateTimer.Stop();
    SvTreeListBox::Hide();
}
/*-----------------18.06.97 10:02-------------------

--------------------------------------------------*/
void 	SwGlobalTree::ExecCommand(USHORT nCmd)
{
    SvLBoxEntry* pEntry = FirstSelected();
    DBG_ASSERT(pEntry, "gleich knallt's");
    if(FN_GLOBAL_EDIT == nCmd)
    {
        const SwGlblDocContent* pCont = (const SwGlblDocContent*)
                                                pEntry->GetUserData();
        EditContent(pCont);
    }
    else
    {
        if(GetSelectionCount() == 1)
        {
            BOOL bMove = FALSE;
            USHORT nSource = (USHORT)GetModel()->GetAbsPos(pEntry);
            USHORT nDest = nSource;
            switch(nCmd)
            {
                case FN_ITEM_DOWN:
                {
                    USHORT nEntryCount = (USHORT)GetEntryCount();
                    bMove = nEntryCount > nSource + 1;
                    nDest+= 2;
                }
                break;
                case FN_ITEM_UP:
                {
                    if(nSource)
                        bMove = 0 != nSource;
                    nDest--;
                }
                break;
            }
            if(	bMove && pActiveShell->MoveGlobalDocContent(
                *pSwGlblDocContents, nSource, nSource + 1, nDest ) &&
                    Update( FALSE ))
                Display();
        }
    }
}

/*-----------------16.06.97 07:43-------------------

--------------------------------------------------*/
BOOL  	SwGlobalTree::Update(BOOL bHard)
{
    SwView* pActView = GetParentWindow()->GetCreateView();
    BOOL bRet = FALSE;
    if(pActView && pActView->GetWrtShellPtr())
    {
        const SwWrtShell* pOldShell = pActiveShell;
        pActiveShell = pActView->GetWrtShellPtr();
        if(pActiveShell != pOldShell)
        {
            delete pSwGlblDocContents;
            pSwGlblDocContents = 0;
        }
        if(!pSwGlblDocContents)
        {
            pSwGlblDocContents = new SwGlblDocContents;
            bRet = TRUE;
            pActiveShell->GetGlobalDocContent(*pSwGlblDocContents);
        }
        else
        {
            BOOL bCopy = FALSE;
            SwGlblDocContents* pTempContents  = new SwGlblDocContents;
            pActiveShell->GetGlobalDocContent(*pTempContents);
            if(pTempContents->Count() != pSwGlblDocContents->Count() ||
                    pTempContents->Count() != GetEntryCount())
            {
                bRet = TRUE;
                bCopy = TRUE;
            }
            else
            {
                for(USHORT i = 0; i < pTempContents->Count() && !bCopy; i++)
                {
                    SwGlblDocContent* pLeft = pTempContents->GetObject(i);
                    SwGlblDocContent* pRight = pSwGlblDocContents->GetObject(i);
                    GlobalDocContentType eType = pLeft->GetType();
                    SvLBoxEntry* pEntry = GetEntry(i);
                    String sTemp = GetEntryText(pEntry);
                    if(eType != pRight->GetType() ||
                        eType == GLBLDOC_SECTION &&
                            pLeft->GetSection()->GetName() != sTemp ||
                        eType == GLBLDOC_TOXBASE && pLeft->GetTOX()->GetTitle() != sTemp)
                            bCopy = bRet = TRUE;
                }
            }
            if(bCopy || bHard)
            {
                USHORT i;

                pSwGlblDocContents->DeleteAndDestroy(0, pSwGlblDocContents->Count());
                for( i = 0; i < pTempContents->Count(); i++)
                {
                    pSwGlblDocContents->Insert(pTempContents->GetObject(i));
                }
                for( i = pTempContents->Count(); i; i--)
                    pTempContents->Remove(i - 1);

            }
            delete pTempContents;
        }

    }
    else
    {
        Clear();
        if(pSwGlblDocContents)
            pSwGlblDocContents->DeleteAndDestroy(0, pSwGlblDocContents->Count());
    }
    // hier muss noch eine Veraenderungspruefung rein!
    return bRet;
}

/*-----------------25.06.97 16:20-------------------

--------------------------------------------------*/
void SwGlobalTree::OpenDoc(const SwGlblDocContent* pCont)
{
    String sFileName(pCont->GetSection()->GetLinkFileName().GetToken(0,
            sfx2::cTokenSeperator));
    BOOL bFound = FALSE;
    const SfxObjectShell* pCurr = SfxObjectShell::GetFirst();
    while( !bFound && pCurr )
    {
        if(pCurr->GetMedium() &&
            String(pCurr->GetMedium()->GetURLObject().GetMainURL(INetURLObject::DECODE_TO_IURI)) == sFileName)
        {
            bFound = TRUE;
            SwGlobalTree::SetShowShell(pCurr);
            Application::PostUserEvent( STATIC_LINK(
                        this, SwGlobalTree, ShowFrameHdl ) );
            pCurr = 0;
        }
        else
            pCurr = SfxObjectShell::GetNext(*pCurr);
    }
    if(!bFound)
    {
        SfxStringItem aURL(SID_FILE_NAME,
            sFileName);
        SfxBoolItem aReadOnly(SID_DOC_READONLY, FALSE);
        SfxStringItem aTargetFrameName( SID_TARGETNAME, String::CreateFromAscii("_blank") );
        SfxStringItem aReferer(SID_REFERER, pActiveShell->GetView().GetDocShell()->GetTitle());
        pActiveShell->GetView().GetViewFrame()->GetDispatcher()->
                Execute(SID_OPENDOC, SFX_CALLMODE_ASYNCHRON,
                            &aURL, &aReadOnly, &aReferer, &aTargetFrameName, 0L);
    }
}

/*-----------------25.06.97 16:08-------------------

--------------------------------------------------*/
IMPL_LINK( 	SwGlobalTree, DoubleClickHdl, SwGlobalTree *, EMPTYARG )
{
    SvLBoxEntry* pEntry = GetCurEntry();
    SwGlblDocContent* pCont = (SwGlblDocContent*)pEntry->GetUserData();
    if(pCont->GetType() == GLBLDOC_SECTION)
        OpenDoc(pCont);
    else
    {
        GotoContent(pCont);
        pActiveShell->GetView().GetEditWin().GrabFocus();
    }
    return 0;
}

/*-----------------07.10.97 08:09-------------------

--------------------------------------------------*/
IMPL_STATIC_LINK_NOINSTANCE(SwGlobalTree, ShowFrameHdl, SwGlobalTree*, EMPTYARG)
{
    if(SwGlobalTree::GetShowShell())
        SfxViewFrame::GetFirst(SwGlobalTree::GetShowShell())->ToTop();
    SwGlobalTree::SetShowShell(0);
    return 0;
}
/* -----------------04.11.98 10:43-------------------
 *
 * --------------------------------------------------*/
void SwGlobalTree::InitEntry(SvLBoxEntry* pEntry,
        const XubString& rStr ,const Image& rImg1,const Image& rImg2,
        SvLBoxButtonKind eButtonKind)
{
    USHORT nColToHilite = 1; //0==Bitmap;1=="Spalte1";2=="Spalte2"
    SvTreeListBox::InitEntry( pEntry, rStr, rImg1, rImg2, eButtonKind );
    SvLBoxString* pCol = (SvLBoxString*)pEntry->GetItem( nColToHilite );
    SwLBoxString* pStr = new SwLBoxString( pEntry, 0, pCol->GetText() );
    pEntry->ReplaceItem( pStr, nColToHilite );
}
/* -----------------04.11.98 10:39-------------------
 *
 * --------------------------------------------------*/

void SwLBoxString::Paint( const Point& rPos, SvLBox& rDev, USHORT nFlags,
    SvLBoxEntry* pEntry )
{
    SwGlblDocContent* pCont = (SwGlblDocContent*)pEntry->GetUserData();
    const SwSection* pSect;
    if(pCont->GetType() == GLBLDOC_SECTION &&
        !(pSect = pCont->GetSection())->IsConnectFlag()	)
    {
        Font aOldFont( rDev.GetFont());
        Font aFont(aOldFont);
        Color aCol( COL_LIGHTRED );
        aFont.SetColor( aCol );
        rDev.SetFont( aFont );
        rDev.DrawText( rPos, GetText() );
        rDev.SetFont( aOldFont );
    }
    else
        SvLBoxString::Paint( rPos, rDev, nFlags, pEntry);
}
/* -----------------------------06.05.2002 10:20------------------------------

 ---------------------------------------------------------------------------*/
void    SwGlobalTree::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        USHORT nResId = GetSettings().GetStyleSettings().GetHighContrastMode() ? IMG_NAVI_ENTRYBMPH : IMG_NAVI_ENTRYBMP;
        aEntryImages = ImageList(SW_RES(nResId));
        Update(sal_True);
    }
    SvTreeListBox::DataChanged( rDCEvt );
}

void SwGlobalTree::InsertRegion( const SwGlblDocContent* _pContent, const Sequence< OUString >& _rFiles )
{
    sal_Int32 nFiles = _rFiles.getLength();
    if ( nFiles )
    {
        BOOL bMove = FALSE;
        if ( !_pContent )
        {
            SvLBoxEntry* pLast = (SvLBoxEntry*)LastVisible();
            _pContent = (SwGlblDocContent*)pLast->GetUserData();
            bMove = TRUE;
        }
        String sFilePassword;
        USHORT nEntryCount = (USHORT)GetEntryCount();
        const OUString* pFileNames = _rFiles.getConstArray();
        SwWrtShell& rSh = GetParentWindow()->GetCreateView()->GetWrtShell();
        rSh.StartAction();
        // after insertion of the first new content the 'pCont' parameter becomes invalid
        // find the index of the 'anchor' content to always use a current anchor content
        USHORT nAnchorContent = pSwGlblDocContents->Count() - 1;
        if ( !bMove )
        {
            for( USHORT nContent = 0; nContent < pSwGlblDocContents->Count(); ++nContent )
            {
                if( *_pContent == *pSwGlblDocContents->GetObject( nContent ) )
                {
                    nAnchorContent = nContent;
                    break;
                }
            }
        }
        SwGlblDocContents aTempContents;
        for ( sal_Int32 nFile = 0; nFile < nFiles; ++nFile )
        {
            //update the global document content after each inserted document
            rSh.GetGlobalDocContent(aTempContents);
            SwGlblDocContent* pAnchorContent = 0;
            DBG_ASSERT(aTempContents.Count() > (nAnchorContent + nFile), "invalid anchor content -> last insertion failed");
            if ( aTempContents.Count() > (nAnchorContent + nFile) )
                pAnchorContent = aTempContents.GetObject(nAnchorContent + (USHORT)nFile);
            else
                pAnchorContent = aTempContents.GetObject(aTempContents.Count() - 1);
            String sFileName(pFileNames[nFile]);
            INetURLObject aFileUrl;
            aFileUrl.SetSmartURL( sFileName );
            String sSectionName(String(aFileUrl.GetLastName(
                INetURLObject::DECODE_UNAMBIGUOUS)).GetToken(0,
                sfx2::cTokenSeperator));
            USHORT nSectCount = rSh.GetSectionFmtCount();
            String sTempSectionName(sSectionName);
            USHORT nAddNumber = 0;
            USHORT nCount = 0;
            // evtl : und Index anhaengen, wenn der Bereichsname schon vergeben ist
            while ( nCount < nSectCount )
            {
                const SwSectionFmt& rFmt = rSh.GetSectionFmt(nCount);
                if ( rFmt.GetSection()->GetName() == sTempSectionName && rFmt.IsInNodesArr() )
                {
                    nCount = 0;
                    nAddNumber++;
                    sTempSectionName = sSectionName;
                    sTempSectionName += ':';
                    sTempSectionName += String::CreateFromInt32( nAddNumber );
                }
                else
                    nCount++;
            }

            if ( nAddNumber )
                sSectionName = sTempSectionName;

            SwSectionData aSectionData(CONTENT_SECTION, sSectionName);
            aSectionData.SetProtectFlag(true);
            aSectionData.SetHidden(false);

            aSectionData.SetLinkFileName(sFileName);
            aSectionData.SetType(FILE_LINK_SECTION);
            aSectionData.SetLinkFilePassword( sFilePassword );

            rSh.InsertGlobalDocContent( *pAnchorContent, aSectionData );
        }
        if ( bMove )
        {
            Update( FALSE );
            rSh.MoveGlobalDocContent(
                *pSwGlblDocContents, nEntryCount, nEntryCount + (USHORT)nFiles, nEntryCount - (USHORT)nFiles );
        }
        rSh.EndAction();
        Update( FALSE );
        Display();
    }
}

IMPL_LINK( SwGlobalTree, DialogClosedHdl, sfx2::FileDialogHelper*, _pFileDlg )
{
    Application::SetDefDialogParent( pDefParentWin );
    if ( ERRCODE_NONE == _pFileDlg->GetError() )
    {
        SfxMediumList* pMedList = pDocInserter->CreateMediumList();
        if ( pMedList )
        {
            Sequence< OUString >aFileNames( pMedList->Count() );
            OUString* pFileNames = aFileNames.getArray();
            SfxMedium* pMed = pMedList->First();
            sal_Int32 nPos = 0;
            while ( pMed )
            {
                String sFileName = pMed->GetURLObject().GetMainURL( INetURLObject::NO_DECODE );
                sFileName += sfx2::cTokenSeperator;
                sFileName += pMed->GetFilter()->GetFilterName();
                sFileName += sfx2::cTokenSeperator;
                pFileNames[nPos++] = sFileName;
                pMed = pMedList->Next();
            }
            delete pMedList;
            InsertRegion( pDocContent, aFileNames );
            DELETEZ( pDocContent );
        }
    }
    return 0;
}

