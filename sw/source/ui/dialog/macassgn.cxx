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


#include "hintids.hxx"

#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#include <svx/htmlmode.hxx>
#ifndef _SVX_SVXIDS_HRC //autogen
#include <svx/svxids.hrc>
#endif
#include <sfx2/app.hxx>
#include <vcl/msgbox.hxx>

#include "swtypes.hxx"
#include "wrtsh.hxx"
#include "viewopt.hxx"
#include "macassgn.hxx"
#include "swevent.hxx"
#include "docsh.hxx"
#include "globals.hrc"
#include "view.hxx"
#include <sfx2/viewfrm.hxx>

#include <svx/svxdlg.hxx>

#include <doc.hxx>

using ::com::sun::star::uno::Reference;
using ::com::sun::star::frame::XFrame;

// SvStringsDtor* __EXPORT _GetRangeHdl( _SfxMacroTabPage*, const String& );

SfxEventNamesItem SwMacroAssignDlg::AddEvents( DlgEventType eType )
{
    // const SfxItemSet& rSet = rPg.GetItemSet();
    SfxEventNamesItem aItem(SID_EVENTCONFIG);

    BOOL bHtmlMode = FALSE;
    USHORT nHtmlMode = ::GetHtmlMode((const SwDocShell*)SfxObjectShell::Current());
    bHtmlMode = nHtmlMode & HTMLMODE_ON ? TRUE : FALSE;

    switch( eType )
    {
    case MACASSGN_TEXTBAUST:			// Textbausteine
        // rPg.SetGetRangeLink( &_GetRangeHdl );
        aItem.AddEvent( String( SW_RES(STR_EVENT_START_INS_GLOSSARY) ), String(),
                            SW_EVENT_START_INS_GLOSSARY );
        aItem.AddEvent( String( SW_RES(STR_EVENT_END_INS_GLOSSARY) ), String(), 
                            SW_EVENT_END_INS_GLOSSARY);
        // damit der neue Handler aktiv wird!
        // rPg.Reset( rSet );
        break;
    case MACASSGN_ALLFRM:
    case MACASSGN_GRAPHIC:			// Grafiken
        {
            aItem.AddEvent( String( SW_RES(STR_EVENT_IMAGE_ERROR) ), String(),
                                SVX_EVENT_IMAGE_ERROR);
            aItem.AddEvent( String( SW_RES(STR_EVENT_IMAGE_ABORT) ), String(),
                                SVX_EVENT_IMAGE_ABORT);
            aItem.AddEvent( String( SW_RES(STR_EVENT_IMAGE_LOAD) ), String(),
                                SVX_EVENT_IMAGE_LOAD);
        }
        // kein break;
    case MACASSGN_FRMURL:			// Frm - URL-Attribute
        {
            if( !bHtmlMode &&
                (MACASSGN_FRMURL == eType || MACASSGN_ALLFRM == eType))
            {
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_KEYINPUT_A ) ), String(),
                                SW_EVENT_FRM_KEYINPUT_ALPHA );
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_KEYINPUT_NOA ) ), String(),
                                SW_EVENT_FRM_KEYINPUT_NOALPHA );
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_RESIZE ) ), String(),
                                SW_EVENT_FRM_RESIZE );
                aItem.AddEvent( String( SW_RES( STR_EVENT_FRM_MOVE ) ), String(),
                                SW_EVENT_FRM_MOVE );
            }
        }
        // kein break;
    case MACASSGN_OLE:				// OLE
        {
            if( !bHtmlMode )
                aItem.AddEvent( String( SW_RES(STR_EVENT_OBJECT_SELECT) ), String(),
                                SW_EVENT_OBJECT_SELECT );
        }
        // kein break;
    case MACASSGN_INETFMT:			// INetFmt-Attribute
        {
            aItem.AddEvent( String( SW_RES(STR_EVENT_MOUSEOVER_OBJECT) ), String(),
                                SFX_EVENT_MOUSEOVER_OBJECT );
            aItem.AddEvent( String( SW_RES(STR_EVENT_MOUSECLICK_OBJECT) ), String(),
                                SFX_EVENT_MOUSECLICK_OBJECT);
            aItem.AddEvent( String( SW_RES(STR_EVENT_MOUSEOUT_OBJECT) ), String(),
                                SFX_EVENT_MOUSEOUT_OBJECT);
        }
        break;
    }

    return aItem;
}


BOOL SwMacroAssignDlg::INetFmtDlg( Window* pParent, SwWrtShell& rSh,
                                    SvxMacroItem*& rpINetItem )
{
    BOOL bRet = FALSE;
    SfxItemSet aSet( rSh.GetAttrPool(), RES_FRMMACRO, RES_FRMMACRO, SID_EVENTCONFIG, SID_EVENTCONFIG, 0 );
    SvxMacroItem aItem( RES_FRMMACRO );
    if( !rpINetItem )
        rpINetItem = new SvxMacroItem( RES_FRMMACRO );
    else
        aItem.SetMacroTable( rpINetItem->GetMacroTable() );

    aSet.Put( aItem );
    aSet.Put( AddEvents( MACASSGN_INETFMT ) );

    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
    SfxAbstractDialog* pMacroDlg = pFact->CreateSfxDialog( pParent, aSet, 
        rSh.GetView().GetViewFrame()->GetFrame()->GetFrameInterface(),
        SID_EVENTCONFIG );
    if ( pMacroDlg && pMacroDlg->Execute() == RET_OK )
    {
        const SfxItemSet* pOutSet = pMacroDlg->GetOutputItemSet();
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pOutSet->GetItemState( RES_FRMMACRO, FALSE, &pItem ))
        {
            rpINetItem->SetMacroTable( ((SvxMacroItem*)pItem)->GetMacroTable() );
            bRet = TRUE;
        }
    }
    return bRet;
}

/*
SvStringsDtor* __EXPORT _GetRangeHdl( _SfxMacroTabPage* , const String& rLanguage )
{
    SvStringsDtor* pNew = new SvStringsDtor;

    SfxApplication* pSfxApp = SFX_APP();
    if ( !rLanguage.EqualsAscii(SVX_MACRO_LANGUAGE_JAVASCRIPT) )
    {
        pSfxApp->EnterBasicCall();

        String* pNewEntry = new String( pSfxApp->GetName() );
        pNew->Insert( pNewEntry, pNew->Count() );

        TypeId aType( TYPE( SwDocShell ));
        SfxObjectShell* pDoc = SfxObjectShell::GetFirst( &aType );
        while( pDoc )
        {
            pNewEntry = new String( pDoc->GetTitle() );
            pNew->Insert( pNewEntry, pNew->Count() );
            pDoc = SfxObjectShell::GetNext( *pDoc, &aType );
        }
        pSfxApp->LeaveBasicCall();
    }

    return pNew;
}
*/



