/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: convert.cxx,v $
 * $Revision: 1.16 $
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


#include <vcl/msgbox.hxx>
#include <svtools/stritem.hxx>
#include <sfx2/viewfrm.hxx>
#include <modcfg.hxx>
#include <svx/htmlmode.hxx>
#include <viewopt.hxx>
#include "swmodule.hxx"
#include "cmdid.h"
#include "convert.hxx"
#include "tablemgr.hxx"
#include "wrtsh.hxx"
#include "view.hxx"
#include "tblafmt.hxx"

#include "table.hrc"
#include "convert.hrc"
#include "swabstdlg.hxx"

namespace swui
{
    SwAbstractDialogFactory * GetFactory();
}

//keep the state of the buttons on runtime
static int nSaveButtonState = -1; // 0: tab, 1: semicolon, 2: paragraph, 3: other, -1: not yet used
static sal_Bool bIsKeepColumn = sal_True;
static sal_Unicode uOther = ',';

void SwConvertTableDlg::GetValues(  sal_Unicode& rDelim,
                                    SwInsertTableOptions& rInsTblOpts,
                                    SwTableAutoFmt *& prTAFmt )
{
    if( aTabBtn.IsChecked() )
    {
        //0x0b mustn't be set when re-converting table into text
        bIsKeepColumn = !aKeepColumn.IsVisible() || aKeepColumn.IsChecked();
        rDelim = bIsKeepColumn ? 0x09 : 0x0b;
        nSaveButtonState = 0;
    }
    else if( aSemiBtn.IsChecked() )
    {
        rDelim = ';';
        nSaveButtonState = 1;
    }
    else if( aOtherBtn.IsChecked() && aOtherEd.GetText().Len() )
    {
        uOther = aOtherEd.GetText().GetChar( 0 );
        rDelim = uOther;
        nSaveButtonState = 3;
    }
    else
    {
        nSaveButtonState = 2;
        rDelim = cParaDelim;
        if(aOtherBtn.IsChecked())
        {
            nSaveButtonState = 3;
            uOther = 0;
        }
    }


    USHORT nInsMode = 0;
    if (aBorderCB.IsChecked())
        nInsMode |= tabopts::DEFAULT_BORDER;
    if (aHeaderCB.IsChecked())
        nInsMode |= tabopts::HEADLINE;
    if (aRepeatHeaderCB.IsEnabled() && aRepeatHeaderCB.IsChecked())
        rInsTblOpts.mnRowsToRepeat = USHORT( aRepeatHeaderNF.GetValue() );
    else
        rInsTblOpts.mnRowsToRepeat = 0;
    if (!aDontSplitCB.IsChecked())
        nInsMode |= tabopts::SPLIT_LAYOUT;

    if( pTAutoFmt )
        prTAFmt = new SwTableAutoFmt( *pTAutoFmt );

    rInsTblOpts.mnInsMode = nInsMode;
}


SwConvertTableDlg::SwConvertTableDlg( SwView& rView, bool bToTable )

    : SfxModalDialog( &rView.GetViewFrame()->GetWindow(), SW_RES(DLG_CONV_TEXT_TABLE)),
#ifdef MSC
#pragma warning (disable : 4355)
#endif
    aTabBtn         (this, SW_RES(CB_TAB)),
    aSemiBtn		(this, SW_RES(CB_SEMI)),
    aParaBtn		(this, SW_RES(CB_PARA)),
    aOtherBtn       (this, SW_RES(RB_OTHER)),
    aOtherEd        (this, SW_RES(ED_OTHER)),
    aKeepColumn     (this, SW_RES(CB_KEEPCOLUMN)),
    aDelimFL       (this, SW_RES(FL_DELIM)),

    aHeaderCB       (this, SW_RES(CB_HEADER)),
    aRepeatHeaderCB	(this, SW_RES(CB_REPEAT_HEADER)),

    aRepeatHeaderFT         (this, SW_RES(FT_REPEAT_HEADER)),
    aRepeatHeaderBeforeFT   (this),
    aRepeatHeaderNF         (this, SW_RES(NF_REPEAT_HEADER)),
    aRepeatHeaderAfterFT    (this),
    aRepeatHeaderCombo      (this, SW_RES(WIN_REPEAT_HEADER), aRepeatHeaderNF, aRepeatHeaderBeforeFT, aRepeatHeaderAfterFT),

    aDontSplitCB	(this, SW_RES(CB_DONT_SPLIT)),
    aBorderCB		(this, SW_RES(CB_BORDER)),
    aOptionsFL      (this, SW_RES(FL_OPTIONS)),

    aOkBtn(this,SW_RES(BT_OK)),
    aCancelBtn(this,SW_RES(BT_CANCEL)),
    aHelpBtn(this, SW_RES(BT_HELP)),
    aAutoFmtBtn(this,SW_RES(BT_AUTOFORMAT)),
#ifdef MSC
#pragma warning (default : 4355)
#endif
    sConvertTextTable(SW_RES(STR_CONVERT_TEXT_TABLE)),
    pTAutoFmt( 0 ),
    pShell( &rView.GetWrtShell() )
{
    FreeResource();
    if(nSaveButtonState > -1)
    {
        switch (nSaveButtonState)
        {
            case 0:
                aTabBtn.Check();
                aKeepColumn.Check(bIsKeepColumn);
            break;
            case 1: aSemiBtn.Check();break;
            case 2: aParaBtn.Check();break;
            case 3:
                aOtherBtn.Check();
                if(uOther)
                    aOtherEd.SetText(uOther);
            break;
        }

    }
    if( bToTable )
    {
        SetText( sConvertTextTable );
        aAutoFmtBtn.SetClickHdl(LINK(this, SwConvertTableDlg, AutoFmtHdl));
        aAutoFmtBtn.Show();
        aKeepColumn.Show();
        aKeepColumn.Enable( aTabBtn.IsChecked() );
        aRepeatHeaderCombo.Arrange( aRepeatHeaderFT );
    }
    else
    {
        //Einfuege-Optionen verstecken
        aHeaderCB		   .Show(FALSE);
        aRepeatHeaderCB	   .Show(FALSE);
        aDontSplitCB	   .Show(FALSE);
        aBorderCB		   .Show(FALSE);
        aOptionsFL         .Show(FALSE);
        aRepeatHeaderCombo.Show(FALSE);

        //Groesse anpassen
        Size aSize(GetSizePixel());
        aSize.Height() = 8 + aHelpBtn.GetSizePixel().Height() + aHelpBtn.GetPosPixel().Y();
        SetOutputSizePixel(aSize);
    }
    aKeepColumn.SaveValue();

    Link aLk( LINK(this, SwConvertTableDlg, BtnHdl) );
    aTabBtn.SetClickHdl( aLk );
    aSemiBtn.SetClickHdl( aLk );
    aParaBtn.SetClickHdl( aLk );
    aOtherBtn.SetClickHdl(aLk );
    aOtherEd.Enable( aOtherBtn.IsChecked() );

    const SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();

    BOOL bHTMLMode = 0 != (::GetHtmlMode(rView.GetDocShell())&HTMLMODE_ON);

    SwInsertTableOptions aInsOpts = pModOpt->GetInsTblFlags(bHTMLMode);
    USHORT nInsTblFlags = aInsOpts.mnInsMode;

    aHeaderCB.Check( 0 != (nInsTblFlags & tabopts::HEADLINE) );
    aRepeatHeaderCB.Check(aInsOpts.mnRowsToRepeat > 0);
    aDontSplitCB.Check( 0 == (nInsTblFlags & tabopts::SPLIT_LAYOUT));
    aBorderCB.Check( 0!= (nInsTblFlags & tabopts::DEFAULT_BORDER) );

    aHeaderCB.SetClickHdl(LINK(this, SwConvertTableDlg, CheckBoxHdl));
    aRepeatHeaderCB.SetClickHdl(LINK(this, SwConvertTableDlg, ReapeatHeaderCheckBoxHdl));
    ReapeatHeaderCheckBoxHdl();
    CheckBoxHdl();
}

SwConvertTableDlg::	~SwConvertTableDlg()
{
    delete pTAutoFmt;
}

IMPL_LINK( SwConvertTableDlg, AutoFmtHdl, PushButton*, pButton )
{
    SwAbstractDialogFactory* pFact = swui::GetFactory();
    DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

    AbstractSwAutoFormatDlg* pDlg = pFact->CreateSwAutoFormatDlg(pButton, pShell, DLG_AUTOFMT_TABLE, FALSE, pTAutoFmt);
    DBG_ASSERT(pDlg, "Dialogdiet fail!");
    if( RET_OK == pDlg->Execute())
        pDlg->FillAutoFmtOfIndex( pTAutoFmt );
    delete pDlg;
    return 0;
}

IMPL_LINK( SwConvertTableDlg, BtnHdl, Button*, pButton )
{
    if( pButton == &aTabBtn )
        aKeepColumn.SetState( aKeepColumn.GetSavedValue() );
    else
    {
        if( aKeepColumn.IsEnabled() )
            aKeepColumn.SaveValue();
        aKeepColumn.Check( TRUE );
    }
    aKeepColumn.Enable( aTabBtn.IsChecked() );
    aOtherEd.Enable( aOtherBtn.IsChecked() );
    return 0;
}

/*********************************************************************/
/*                                                                   */
/*********************************************************************/

IMPL_LINK(SwConvertTableDlg, CheckBoxHdl, CheckBox*, EMPTYARG)
{
    aRepeatHeaderCB.Enable(aHeaderCB.IsChecked());
    ReapeatHeaderCheckBoxHdl();

    return 0;
}

IMPL_LINK(SwConvertTableDlg, ReapeatHeaderCheckBoxHdl, void*, EMPTYARG)
{
    sal_Bool bEnable = aHeaderCB.IsChecked() && aRepeatHeaderCB.IsChecked();
    aRepeatHeaderBeforeFT.Enable(bEnable);
    aRepeatHeaderAfterFT.Enable(bEnable);
    aRepeatHeaderNF.Enable(bEnable);

    return 0;
}
