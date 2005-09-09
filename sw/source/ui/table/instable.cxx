/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: instable.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 10:59:33 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


#pragma hdrstop

#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif

#include "wrtsh.hxx"
#include "view.hxx"
#include "itabenum.hxx"
#include "instable.hxx"
//CHINA001 #include "tautofmt.hxx"
#include "tblafmt.hxx"
#include "modcfg.hxx"
#include "swmodule.hxx"
#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif

#include "table.hrc"
#include "instable.hrc"

#include "swabstdlg.hxx" //CHINA001

namespace swui
{
    SwAbstractDialogFactory * GetFactory();
}

#define ROW_COL_PROD 16384

void SwInsTableDlg::GetValues( String& rName, USHORT& rRow, USHORT& rCol,
                                SwInsertTableOptions& rInsTblOpts, String& rAutoName,
                                SwTableAutoFmt *& prTAFmt )
{
    USHORT nInsMode = 0;
    rName = aNameEdit.GetText();
    rRow = (USHORT)aRowEdit.GetValue();
    rCol = (USHORT)aColEdit.GetValue();

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
    {
        prTAFmt = new SwTableAutoFmt( *pTAutoFmt );
        rAutoName = prTAFmt->GetName();
    }

    rInsTblOpts.mnInsMode = nInsMode;
}

// CTOR / DTOR -----------------------------------------------------------


SwInsTableDlg::SwInsTableDlg( SwView& rView )
    : SfxModalDialog( rView.GetWindow(), SW_RES(DLG_INSERT_TABLE) ),
    aNameEdit				(this, SW_RES(ED_NAME)),
    aNameFT         		(this, SW_RES(FT_NAME)),
    aColLbl					(this, SW_RES(FT_COL)),
    aColEdit				(this, SW_RES(ED_COL)),
    aRowLbl					(this, SW_RES(FT_ROW)),
    aRowEdit				(this, SW_RES(ED_ROW)),
    aFL             		(this, SW_RES(FL_TABLE)),
    aHeaderCB				(this, SW_RES(CB_HEADER)),
    aRepeatHeaderCB			(this, SW_RES(CB_REPEAT_HEADER)),
    aRepeatHeaderFT			(this, SW_RES(FT_REPEAT_HEADER)),
    aRepeatHeaderBeforeFT	(this),
    aRepeatHeaderNF			(this, SW_RES(NF_REPEAT_HEADER)),
    aRepeatHeaderAfterFT	(this),
    aRepeatHeaderCombo		(this, SW_RES(WIN_REPEAT_HEADER), aRepeatHeaderNF, aRepeatHeaderBeforeFT, aRepeatHeaderAfterFT),
    aDontSplitCB			(this, SW_RES(CB_DONT_SPLIT)),
    aBorderCB				(this, SW_RES(CB_BORDER)),
    aOptionsFL      		(this, SW_RES(FL_OPTIONS)),
    aOkBtn					(this, SW_RES(BT_OK)),
    aCancelBtn				(this, SW_RES(BT_CANCEL)),
    aHelpBtn				(this, SW_RES(BT_HELP)),
    aAutoFmtBtn				(this, SW_RES(BT_AUTOFORMAT)),
    pTAutoFmt( 0 ),
    pShell(&rView.GetWrtShell()),
    nEnteredValRepeatHeaderNF( -1 )
{
    FreeResource();
    aNameEdit.SetText(pShell->GetUniqueTblName());
    aNameEdit.SetModifyHdl(LINK(this, SwInsTableDlg, ModifyName));
    aColEdit.SetModifyHdl(LINK(this, SwInsTableDlg, ModifyRowCol));
    aRowEdit.SetModifyHdl(LINK(this, SwInsTableDlg, ModifyRowCol));

    aRowEdit.SetMax(ROW_COL_PROD/aColEdit.GetValue());
    aColEdit.SetMax(ROW_COL_PROD/aRowEdit.GetValue());
    aAutoFmtBtn.SetClickHdl(LINK(this, SwInsTableDlg, AutoFmtHdl));

    BOOL bHTMLMode = 0 != (::GetHtmlMode(rView.GetDocShell())&HTMLMODE_ON);
    const SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();

    SwInsertTableOptions aInsOpts = pModOpt->GetInsTblFlags(bHTMLMode);
    USHORT nInsTblFlags = aInsOpts.mnInsMode;

    aHeaderCB.Check(nInsTblFlags & tabopts::HEADLINE);
    aRepeatHeaderCB.Check(aInsOpts.mnRowsToRepeat > 0);
    if(bHTMLMode)
    {
        aDontSplitCB.Hide();
        aBorderCB.SetPosPixel(aDontSplitCB.GetPosPixel());
    }
    else
    {
        aDontSplitCB.Check(!(nInsTblFlags & tabopts::SPLIT_LAYOUT));
    }
    aBorderCB.Check(nInsTblFlags & tabopts::DEFAULT_BORDER);

    aRepeatHeaderNF.SetModifyHdl( LINK( this, SwInsTableDlg, ModifyRepeatHeaderNF_Hdl ) );
    aHeaderCB.SetClickHdl(LINK(this, SwInsTableDlg, CheckBoxHdl));
    aRepeatHeaderCB.SetClickHdl(LINK(this, SwInsTableDlg, ReapeatHeaderCheckBoxHdl));
    ReapeatHeaderCheckBoxHdl();
    CheckBoxHdl();

    long nMax = aRowEdit.GetValue();
    if( nMax <= 1 )
        nMax = 1;
    else
        --nMax;
    aRepeatHeaderNF.SetMax( nMax );

    aRepeatHeaderCombo.Arrange( aRepeatHeaderFT );
}

SwInsTableDlg::~SwInsTableDlg()
{
    delete pTAutoFmt;
}

IMPL_LINK_INLINE_START( SwInsTableDlg, ModifyName, Edit *, pEdit )
{
    String sTblName = pEdit->GetText();
    if(sTblName.Search(' ') != STRING_NOTFOUND)
    {
        sTblName.EraseAllChars( );
        pEdit->SetText(sTblName);
    }

    aOkBtn.Enable(pShell->GetTblStyle( sTblName ) == 0);
    return 0;
}
IMPL_LINK_INLINE_END( SwInsTableDlg, ModifyName, Edit *, EMPTYARG )

/*-----------------15.04.98 11:36-------------------

--------------------------------------------------*/
IMPL_LINK( SwInsTableDlg, ModifyRowCol, NumericField *, pField )
{
    if(pField == &aColEdit)
    {
        long nCol = aColEdit.GetValue();
        if(!nCol)
            nCol = 1;
        aRowEdit.SetMax(ROW_COL_PROD/nCol);
    }
    else
    {
        long nRow = aRowEdit.GetValue();
        if(!nRow)
            nRow = 1;
        aColEdit.SetMax(ROW_COL_PROD/nRow);

        // adjust depending NF for repeated rows
        long nMax = ( nRow == 1 )? 1 : nRow - 1 ;
        long nActVal = aRepeatHeaderNF.GetValue();

        aRepeatHeaderNF.SetMax( nMax );

        if( nActVal > nMax )
            aRepeatHeaderNF.SetValue( nMax );
        else if( nActVal < nEnteredValRepeatHeaderNF )
            aRepeatHeaderNF.SetValue( ( nEnteredValRepeatHeaderNF < nMax )? nEnteredValRepeatHeaderNF : nMax );
    }
    return 0;
}

IMPL_LINK( SwInsTableDlg, AutoFmtHdl, PushButton*, pButton )
{
    //CHINA001 SwAutoFormatDlg aDlg( pButton, pShell, FALSE, pTAutoFmt );
    SwAbstractDialogFactory* pFact = swui::GetFactory();//CHINA001
    DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001
            
    AbstractSwAutoFormatDlg* pDlg = pFact->CreateSwAutoFormatDlg(pButton,pShell,ResId( DLG_AUTOFMT_TABLE ), FALSE, pTAutoFmt );
    DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001			
    if( RET_OK == pDlg->Execute()) //CHINA001  if( RET_OK == aDlg.Execute())
        pDlg->FillAutoFmtOfIndex( pTAutoFmt ); //CHINA001 aDlg.FillAutoFmtOfIndex( pTAutoFmt );
    delete pDlg; //CHINA001 
    return 0;
}

IMPL_LINK(SwInsTableDlg, CheckBoxHdl, CheckBox*, EMPTYARG)
{
    aRepeatHeaderCB.Enable(aHeaderCB.IsChecked());
    ReapeatHeaderCheckBoxHdl();

    return 0;
}

IMPL_LINK(SwInsTableDlg, ReapeatHeaderCheckBoxHdl, void*, EMPTYARG)
{
    aRepeatHeaderCombo.Enable(aHeaderCB.IsChecked() && aRepeatHeaderCB.IsChecked());

    return 0;
}

IMPL_LINK(SwInsTableDlg, ModifyRepeatHeaderNF_Hdl, void*, EMPTYARG)
{
    nEnteredValRepeatHeaderNF = aRepeatHeaderNF.GetValue();
    return 0;
}

