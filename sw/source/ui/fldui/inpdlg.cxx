/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: inpdlg.cxx,v $
 * $Revision: 1.11 $
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



#define _INPDLG_CXX

#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#include <unotools/charclass.hxx>
#include <editeng/unolingu.hxx>
#include <wrtsh.hxx>
#include <fldbas.hxx>
#include <expfld.hxx>
#include <usrfld.hxx>
#include <inpdlg.hxx>
#include <fldmgr.hxx>

#ifndef _FLDUI_HRC
#include <fldui.hrc>
#endif
#ifndef _INPDLG_HRC
#include <inpdlg.hrc>
#endif


/*--------------------------------------------------------------------
    Beschreibung: Feldeinfuegen bearbeiten
 --------------------------------------------------------------------*/

SwFldInputDlg::SwFldInputDlg( Window *pParent, SwWrtShell &rS,
                              SwField* pField, BOOL bNextButton ) :

    SvxStandardDialog(pParent,	SW_RES(DLG_FLD_INPUT)),

    rSh( rS ),
    pInpFld(0),
    pSetFld(0),
    pUsrType(0),

    aLabelED    (this, SW_RES(ED_LABEL  )),
    aEditED		(this, SW_RES(ED_EDIT	)),
    aEditFL     (this, SW_RES(FL_EDIT       )),

    aOKBT       (this, SW_RES(BT_OK     )),
    aCancelBT	(this, SW_RES(BT_CANCEL	)),
    aNextBT		(this, SW_RES(PB_NEXT	)),
    aHelpBT     (this, SW_RES(PB_HELP    ))
{
    // Font fuers Edit umschalten
    Font aFont(aEditED.GetFont());
    aFont.SetWeight(WEIGHT_LIGHT);
    aEditED.SetFont(aFont);

    if( bNextButton )
    {
        aNextBT.Show();
        aNextBT.SetClickHdl(LINK(this, SwFldInputDlg, NextHdl));
    }
    else
    {
        long nDiff = aCancelBT.GetPosPixel().Y() - aOKBT.GetPosPixel().Y();
        Point aPos = aHelpBT.GetPosPixel();
        aPos.Y() -= nDiff;
        aHelpBT.SetPosPixel(aPos);
    }

    // Auswertung hier
    String aStr;
    if( RES_INPUTFLD == pField->GetTyp()->Which() )
    {	// Es ist eine Eingabefeld
        //
        pInpFld = (SwInputField*)pField;
        aLabelED.SetText( pInpFld->GetPar2() );
        USHORT nSubType = pInpFld->GetSubType();

        switch(nSubType & 0xff)
        {
            case INP_TXT:
                aStr = pInpFld->GetPar1();
                break;

            case INP_USR:
                // Benutzerfeld
                if( 0 != ( pUsrType = (SwUserFieldType*)rSh.GetFldType(
                            RES_USERFLD, pInpFld->GetPar1() ) ) )
                    aStr = pUsrType->GetContent();
                break;
        }
    }
    else
    {
        // es ist eine SetExpression
        pSetFld = (SwSetExpField*)pField;
        String sFormula(pSetFld->GetFormula());
        //values are formatted - formulas are not
        CharClass aCC( SvxCreateLocale( pSetFld->GetLanguage() ));
        if( aCC.isNumeric( sFormula ))
            aStr = pSetFld->Expand();
        else
            aStr = sFormula;
        aLabelED.SetText( pSetFld->GetPromptText() );
    }

    // JP 31.3.00: Inputfields in readonly regions must be allowed to
    //				input any content. - 74639
    BOOL bEnable = !rSh.IsCrsrReadonly();
                    /*!rSh.IsReadOnlyAvailable() || !rSh.HasReadonlySel()*/;
    aOKBT.Enable( bEnable );
    aEditED.SetReadOnly( !bEnable );

    if( aStr.Len() )
        aEditED.SetText( aStr.ConvertLineEnd() );
    FreeResource();
}

SwFldInputDlg::~SwFldInputDlg()
{
}

void SwFldInputDlg::StateChanged( StateChangedType nType )
{
    if ( nType == STATE_CHANGE_INITSHOW )
        aEditED.GrabFocus();
    SvxStandardDialog::StateChanged( nType );
}

/*--------------------------------------------------------------------
     Beschreibung:	Schliessen
 --------------------------------------------------------------------*/

void SwFldInputDlg::Apply()
{
    String aTmp( aEditED.GetText() );
    aTmp.EraseAllChars( '\r' );

    rSh.StartAllAction();
    BOOL bModified = FALSE;
    if(pInpFld)
    {
        if(pUsrType)
        {
            if( aTmp != pUsrType->GetContent() )
            {
                pUsrType->SetContent(aTmp);
                pUsrType->UpdateFlds();
                bModified = TRUE;
            }
        }
        else if( aTmp != pInpFld->GetPar1() )
        {
            pInpFld->SetPar1(aTmp);
            rSh.SwEditShell::UpdateFlds(*pInpFld);
            bModified = TRUE;
        }
    }
    else if( aTmp != pSetFld->GetPar2() )
    {
        pSetFld->SetPar2(aTmp);
        rSh.SwEditShell::UpdateFlds(*pSetFld);
        bModified = TRUE;
    }

    if( bModified )
        rSh.SetUndoNoResetModified();

    rSh.EndAllAction();
}


IMPL_LINK(SwFldInputDlg, NextHdl, PushButton*, EMPTYARG)
{
    EndDialog(RET_OK);
    return 0;
}


