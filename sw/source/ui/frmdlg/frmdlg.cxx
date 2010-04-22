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

#include <svx/dialogs.hrc>
#include <hintids.hxx>
#include <tools/list.hxx>
#include <sfx2/viewfrm.hxx>
#include <svx/htmlmode.hxx>
#include <fmtfsize.hxx>
#include <wrtsh.hxx>
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <docsh.hxx>
#include <viewopt.hxx>
#include <frmdlg.hxx>
#include <frmpage.hxx>
#include <wrap.hxx>
#include <column.hxx>
#include <macassgn.hxx>

#ifndef _FRMUI_HRC
#include <frmui.hrc>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#include <svx/svxids.hrc>
#include <svx/flagsdef.hxx>
#include <svx/svxdlg.hxx>

/*--------------------------------------------------------------------
    Beschreibung:	Der Traeger des Dialoges
 --------------------------------------------------------------------*/

SwFrmDlg::SwFrmDlg( SfxViewFrame*       pViewFrame,
                    Window*				pParent,
                    const SfxItemSet& 	rCoreSet,
                    BOOL 				bNewFrm,
                    USHORT				nResType,
                    BOOL                bFormat,
                    UINT16				nDefPage,
                    const String* 		pStr) :

    SfxTabDialog(pViewFrame, pParent, SW_RES(nResType), &rCoreSet, pStr != 0),
    m_bFormat(bFormat),
    m_bNew(bNewFrm),
    m_rSet(rCoreSet),
    m_nDlgType(nResType),
    m_pWrtShell(((SwView*)pViewFrame->GetViewShell())->GetWrtShellPtr())
{
    FreeResource();
    USHORT nHtmlMode = ::GetHtmlMode(m_pWrtShell->GetView().GetDocShell());
    m_bHTMLMode = static_cast< BOOL >(nHtmlMode & HTMLMODE_ON);

    // BspFont fuer beide Bsp-TabPages
    //
    if(pStr)
    {
        String aTmp( GetText() );
        aTmp += SW_RESSTR(STR_COLL_HEADER);
        aTmp += *pStr;
        aTmp += ')';
    }
    AddTabPage(TP_FRM_STD,	SwFrmPage::Create, 0);
    AddTabPage(TP_FRM_ADD,	SwFrmAddPage::Create, 0);
    AddTabPage(TP_FRM_WRAP,	SwWrapTabPage::Create, 0);
    AddTabPage(TP_FRM_URL,	SwFrmURLPage::Create, 0);
    if(m_nDlgType == DLG_FRM_GRF)
    {
        AddTabPage( TP_GRF_EXT,	SwGrfExtPage::Create, 0 );
        AddTabPage( RID_SVXPAGE_GRFCROP );
    }
    if (m_nDlgType == DLG_FRM_STD)
    {
        AddTabPage(TP_COLUMN,	SwColumnPage::Create,  	 0);
    }
    SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();
    DBG_ASSERT(pFact, "Dialogdiet fail!");
    AddTabPage(TP_BACKGROUND, pFact->GetTabPageCreatorFunc( RID_SVXPAGE_BACKGROUND ), 0 );
    AddTabPage( TP_MACRO_ASSIGN, pFact->GetTabPageCreatorFunc(RID_SVXPAGE_MACROASSIGN), 0);
    AddTabPage( TP_BORDER, pFact->GetTabPageCreatorFunc( RID_SVXPAGE_BORDER ), 0 );

    if(m_bHTMLMode)
    {
        switch( m_nDlgType )
        {
        case DLG_FRM_STD:
                if(0  == (nHtmlMode & HTMLMODE_SOME_ABS_POS))
                    RemoveTabPage(TP_BORDER);
                RemoveTabPage(TP_COLUMN);
            // kein break
        case DLG_FRM_OLE:
                RemoveTabPage(TP_FRM_URL);
                RemoveTabPage(TP_MACRO_ASSIGN);
            break;
        case DLG_FRM_GRF:
                RemoveTabPage(RID_SVXPAGE_GRFCROP);
            break;
        }
        if( 0  == (nHtmlMode & HTMLMODE_SOME_ABS_POS) ||
            m_nDlgType != DLG_FRM_STD )
            RemoveTabPage(TP_BACKGROUND);
    }

    if (m_bNew)
        SetCurPageId(TP_FRM_STD);

    if (nDefPage)
        SetCurPageId(nDefPage);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwFrmDlg::~SwFrmDlg()
{
}




void SwFrmDlg::PageCreated( USHORT nId, SfxTabPage &rPage )
{
    SfxAllItemSet aSet(*(GetInputSetImpl()->GetPool()));
    switch ( nId )
    {
    case TP_FRM_STD:
        ((SwFrmPage&)rPage).SetNewFrame(m_bNew);
        ((SwFrmPage&)rPage).SetFormatUsed(m_bFormat);
        ((SwFrmPage&)rPage).SetFrmType(m_nDlgType);
        break;

    case TP_FRM_ADD:
        ((SwFrmAddPage&)rPage).SetFormatUsed(m_bFormat);
        ((SwFrmAddPage&)rPage).SetFrmType(m_nDlgType);
        ((SwFrmAddPage&)rPage).SetNewFrame(m_bNew);
        ((SwFrmAddPage&)rPage).SetShell(m_pWrtShell);
        break;

    case TP_FRM_WRAP:
        ((SwWrapTabPage&)rPage).SetNewFrame(m_bNew);
        ((SwWrapTabPage&)rPage).SetFormatUsed(m_bFormat, FALSE);
        ((SwWrapTabPage&)rPage).SetShell(m_pWrtShell);
        break;

    case TP_COLUMN:
        {
            ((SwColumnPage&)rPage).SetFrmMode(TRUE);
            ((SwColumnPage&)rPage).SetFormatUsed(m_bFormat);

            const SwFmtFrmSize& rSize = (const SwFmtFrmSize&)
                                                m_rSet.Get( RES_FRM_SIZE );
            ((SwColumnPage&)rPage).SetPageWidth( rSize.GetWidth() );
        }
        break;

    case TP_MACRO_ASSIGN:
        {
        SfxAllItemSet aNewSet(*aSet.GetPool());
        aNewSet.Put( SwMacroAssignDlg::AddEvents( 
            DLG_FRM_GRF == m_nDlgType ? MACASSGN_GRAPHIC : DLG_FRM_OLE == m_nDlgType ? MACASSGN_OLE : MACASSGN_FRMURL ) );
        if ( m_pWrtShell )
            rPage.SetFrame( m_pWrtShell->GetView().GetViewFrame()->GetFrame().GetFrameInterface() );
        rPage.PageCreated(aNewSet);
        break;
        }

    case TP_BACKGROUND:
        if( DLG_FRM_STD == m_nDlgType )
        {
            sal_Int32 nFlagType = SVX_SHOW_SELECTOR;
            if(!m_bHTMLMode)
                nFlagType |= SVX_ENABLE_TRANSPARENCY;
            aSet.Put (SfxUInt32Item(SID_FLAG_TYPE, nFlagType));
            rPage.PageCreated(aSet);
        }
        break;

    case TP_BORDER:
        {
            aSet.Put (SfxUInt16Item(SID_SWMODE_TYPE,SW_BORDER_MODE_FRAME));
            rPage.PageCreated(aSet);
        }
        break;
    }
}
