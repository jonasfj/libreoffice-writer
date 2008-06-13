/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: listsh.cxx,v $
 * $Revision: 1.18 $
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


#include "cmdid.h"
#include "hintids.hxx"
#include <svx/sizeitem.hxx>
#include <svx/brshitem.hxx>
#include <sfx2/app.hxx>
#include <sfx2/request.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/bindings.hxx>
#include <svtools/stritem.hxx>
#include <svtools/eitem.hxx>
#include <tools/urlobj.hxx>
#include <svtools/whiter.hxx>
#include <svtools/intitem.hxx>
#include <tools/shl.hxx>
#include <svx/srchitem.hxx>

// --> FME 2005-01-04 #i35572#
#include <numrule.hxx>
// <--
#include <fmtornt.hxx>
#include "wrtsh.hxx"
#include "swmodule.hxx"
#include "frmatr.hxx"
#include "helpid.h"
#include "globals.hrc"
#include "shells.hrc"
#include "uinums.hxx"
#include "listsh.hxx"
#include "poolfmt.hxx"
#include "view.hxx"
#include "edtwin.hxx"

#define SwListShell
#include "itemdef.hxx"
#include "swslots.hxx"

#include <IDocumentOutlineNodes.hxx>

SFX_IMPL_INTERFACE(SwListShell, SwBaseShell, SW_RES(STR_SHELLNAME_LIST))
{
    SFX_OBJECTBAR_REGISTRATION(SFX_OBJECTBAR_OBJECT, SW_RES(RID_NUM_TOOLBOX));
}


TYPEINIT1(SwListShell,SwBaseShell)

// --> FME 2005-01-04 #i35572# Functionality of Numbering/Bullet toolbar
// for outline numbered paragraphs should match the functions for outlines
// available in the navigator. Therefore the code in the following
// function is quite similar the the code in SwContentTree::ExecCommand.
void lcl_OutlineUpDownWithSubPoints( SwWrtShell& rSh, bool bMove, bool bUp )
{
    const sal_uInt16 nActPos = rSh.GetOutlinePos();
    if ( nActPos < USHRT_MAX && rSh.IsOutlineMovable( nActPos ) )
    {
        rSh.Push();
        rSh.MakeOutlineSel( nActPos, nActPos, TRUE );

        if ( bMove )
        {
            const IDocumentOutlineNodes* pIDoc( rSh.getIDocumentOutlineNodesAccess() );
            const sal_uInt16 nActLevel = static_cast<sal_uInt16>(pIDoc->getOutlineLevel( nActPos ));
            sal_uInt16 nActEndPos = nActPos + 1;
            sal_Int16 nDir = 0;

            if ( !bUp )
            {
                // Move down with subpoints:
                while ( nActEndPos < pIDoc->getOutlineNodesCount() &&
                        pIDoc->getOutlineLevel( nActEndPos ) > nActLevel )
                    ++nActEndPos;

                if ( nActEndPos < pIDoc->getOutlineNodesCount() )
                {
                    // The current subpoint which should be moved
                    // starts at nActPos and ends at nActEndPos - 1
                    --nActEndPos;
                    sal_uInt16 nDest = nActEndPos + 2;
                    while ( nDest < pIDoc->getOutlineNodesCount() &&
                            pIDoc->getOutlineLevel( nDest ) > nActLevel )
                        ++nDest;

                    nDir = nDest - 1 - nActEndPos;
                }
            }
            else
            {
                // Move up with subpoints:
                if ( nActPos > 0 )
                {
                    --nActEndPos;
                    sal_uInt16 nDest = nActPos - 1;
                    while ( nDest > 0 && pIDoc->getOutlineLevel( nDest ) > nActLevel )
                        --nDest;

                    nDir = nDest - nActPos;
                }
            }

            if ( nDir )
            {
                rSh.MoveOutlinePara( nDir );
                rSh.GotoOutline( nActPos + nDir );
            }
        }
        else
        {
            // Up/down with subpoints:
            rSh.OutlineUpDown( bUp ? -1 : 1 );
        }

        rSh.ClearMark();
        rSh.Pop( sal_False );
    }
}
// <--

void SwListShell::Execute(SfxRequest &rReq)
{
    const SfxItemSet* pArgs = rReq.GetArgs();
    USHORT nSlot = rReq.GetSlot();
    SwWrtShell& rSh = GetShell();

    // --> FME 2005-01-04 #i35572#
    const SwNumRule* pCurRule = rSh.GetCurNumRule();
    ASSERT( pCurRule, "SwListShell::Execute without NumRule" )
    bool bOutline = pCurRule && pCurRule->IsOutlineRule();
    // <--

    switch (nSlot)
    {
        case FN_NUM_BULLET_DOWN:
        case FN_NUM_BULLET_UP:
            {
                SfxViewFrame * pFrame = GetView().GetViewFrame();

                rReq.Done();
                rSh.NumUpDown( ( nSlot == FN_NUM_BULLET_DOWN )
                               ? TRUE
                               : FALSE );
                pFrame->GetBindings().Invalidate( SID_TABLE_CELL );	// StatusZeile updaten!
            }
            break;

        case FN_NUM_BULLET_NEXT:
            rSh.GotoNextNum();
            rReq.Done();
            break;

        case FN_NUM_BULLET_NONUM:
            rSh.NoNum();
            rReq.Done();
            break;

        case FN_NUM_BULLET_OFF:
        {
            rReq.Ignore();
            SfxRequest aReq( GetView().GetViewFrame(), FN_NUM_BULLET_ON );
            aReq.AppendItem( SfxBoolItem( FN_PARAM_1, FALSE ) );
            aReq.Done();
            rSh.DelNumRules();
            break;
        }

        case FN_NUM_BULLET_OUTLINE_DOWN:
            if ( bOutline )
                lcl_OutlineUpDownWithSubPoints( rSh, false, false );
            else
                rSh.MoveNumParas(FALSE, FALSE);
            rReq.Done();
            break;

        case FN_NUM_BULLET_OUTLINE_MOVEDOWN:
            if ( bOutline )
                lcl_OutlineUpDownWithSubPoints( rSh, true, false );
            else
                rSh.MoveNumParas(TRUE, FALSE);
            rReq.Done();
            break;

        case FN_NUM_BULLET_OUTLINE_MOVEUP:
            if ( bOutline )
                lcl_OutlineUpDownWithSubPoints( rSh, true, true );
            else
                rSh.MoveNumParas(TRUE, TRUE);
            rReq.Done();
            break;

        case FN_NUM_BULLET_OUTLINE_UP:
            if ( bOutline )
                lcl_OutlineUpDownWithSubPoints( rSh, false, true );
            else
                rSh.MoveNumParas(FALSE, TRUE);
            rReq.Done();
            break;

        case FN_NUM_BULLET_PREV:
            rSh.GotoPrevNum();
            rReq.Done();
            break;

        case FN_NUM_OR_NONUM:
        {
            BOOL bApi = rReq.IsAPI();
            BOOL bDelete = !rSh.IsNoNum(!bApi);
            if(pArgs )
                bDelete = ((SfxBoolItem &)pArgs->Get(rReq.GetSlot())).GetValue();
            rSh.NumOrNoNum( bDelete, !bApi );
            rReq.AppendItem( SfxBoolItem( nSlot, bDelete ) );
            rReq.Done();
        }
        break;
        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }
}


void SwListShell::GetState(SfxItemSet &rSet)
{
    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    SwWrtShell& rSh = GetShell();
    BYTE nCurrentNumLevel = rSh.GetNumLevel();
    while ( nWhich )
    {
        switch( nWhich )
        {
            case FN_NUM_OR_NONUM:
                rSet.Put(SfxBoolItem(nWhich, GetShell().IsNoNum(FALSE)));
            break;
            case FN_NUM_BULLET_OUTLINE_UP:
            case FN_NUM_BULLET_UP:
                if(!nCurrentNumLevel)
                    rSet.DisableItem(nWhich);
            break;
            case FN_NUM_BULLET_OUTLINE_DOWN :
            {
                sal_uInt8 nUpper = 0;
                sal_uInt8 nLower = 0;
                rSh.GetCurrentOutlineLevels( nUpper, nLower );
                if(nLower == (MAXLEVEL - 1))
                    rSet.DisableItem(nWhich);
            }
            break;
            case FN_NUM_BULLET_DOWN:
                if(nCurrentNumLevel == (MAXLEVEL - 1))
                    rSet.DisableItem(nWhich);
            break;
        }
        nWhich = aIter.NextWhich();
    }
}


SwListShell::SwListShell(SwView &_rView) :
    SwBaseShell(_rView)
{
    SetName(String::CreateFromAscii("List"));
    SetHelpId(SW_LISTSHELL);
}


