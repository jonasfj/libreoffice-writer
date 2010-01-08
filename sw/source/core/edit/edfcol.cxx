/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: edfcol.cxx,v $
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


#include <hintids.hxx>
#include <editeng/brkitem.hxx>
#include <editsh.hxx>
#include <doc.hxx>		// fuer SwTxtFmtColls
#include <edimp.hxx>	// fuer MACROS
#include <ndtxt.hxx>
#include <paratr.hxx>
#include <fmtpdsc.hxx>
// --> FME 2004-06-29 #114856# Formular view
#include <viewopt.hxx>
// <--
#include <SwRewriter.hxx>
#include <undobj.hxx>
#include <swundo.hxx>

/*************************************
 * FormatColl
 *************************************/
// TXT


SwTxtFmtColl& SwEditShell::GetDfltTxtFmtColl() const
{
    return *((SwTxtFmtColl*) (GetDoc()->GetDfltTxtFmtColl()));
}


USHORT SwEditShell::GetTxtFmtCollCount() const
{
    return GetDoc()->GetTxtFmtColls()->Count();
}


SwTxtFmtColl& SwEditShell::GetTxtFmtColl( USHORT nFmtColl) const
{
    return *((*(GetDoc()->GetTxtFmtColls()))[nFmtColl]);
}

// --> OD 2007-11-06 #i62675#
void SwEditShell::SetTxtFmtColl( SwTxtFmtColl *pFmt,
                                 bool bResetListAttrs )
{
    SwTxtFmtColl *pLocal = pFmt? pFmt: (*GetDoc()->GetTxtFmtColls())[0];
    StartAllAction();

    SwRewriter aRewriter;
    aRewriter.AddRule(UNDO_ARG1, pLocal->GetName());

    GetDoc()->StartUndo(UNDO_SETFMTCOLL, &aRewriter);
    FOREACHPAM_START(this)

        if( !PCURCRSR->HasReadonlySel(
                    // --> FME 2004-06-29 #114856# Formular view
                    GetViewOptions()->IsFormView() ) )
                    // <--
            GetDoc()->SetTxtFmtColl( *PCURCRSR, pLocal, true, bResetListAttrs );

    FOREACHPAM_END()
    GetDoc()->EndUndo(UNDO_SETFMTCOLL, NULL);
    EndAllAction();
}
// <--


SwTxtFmtColl* SwEditShell::MakeTxtFmtColl(const String& rFmtCollName,
        SwTxtFmtColl* pParent)
{
    SwTxtFmtColl *pColl;
    if ( pParent == 0 )
        pParent = &GetTxtFmtColl(0);
    if (  (pColl=GetDoc()->MakeTxtFmtColl(rFmtCollName, pParent)) == 0 )
    {
        ASSERT( FALSE, "MakeTxtFmtColl failed" )
    }
    return pColl;

}


void SwEditShell::FillByEx(SwTxtFmtColl* pColl, BOOL bReset)
{
    if( bReset )
    {
        // --> OD 2007-01-25 #i73790# - method renamed
        pColl->ResetAllFmtAttr();
        // <--
    }

    SwPaM * pCrsr = GetCrsr();
    SwCntntNode * pCnt = pCrsr->GetCntntNode();
    const SfxItemSet* pSet = pCnt->GetpSwAttrSet();
    if( pSet )
    {
        // JP 05.10.98: falls eines der Attribute Break/PageDesc/NumRule(auto)
        //		im ItemSet steht, so entferne die VORM setzen an der Collection.
        //		Ansonst wird zu viel gemacht oder falsche gemacht (NumRules!)
        //		Bug 57568

        // AutoNumRules NICHT in die Vorlagen uebernehmen
        const SfxPoolItem* pItem;
        const SwNumRule* pRule = 0;
        if( SFX_ITEM_SET == pSet->GetItemState( RES_BREAK, FALSE ) ||
            SFX_ITEM_SET == pSet->GetItemState( RES_PAGEDESC,FALSE ) ||
            ( SFX_ITEM_SET == pSet->GetItemState( RES_PARATR_NUMRULE,
                FALSE, &pItem ) && 0 != (pRule = GetDoc()->FindNumRulePtr(
                ((SwNumRuleItem*)pItem)->GetValue() )) &&
                pRule && pRule->IsAutoRule() )
            )
        {
            SfxItemSet aSet( *pSet );
            aSet.ClearItem( RES_BREAK );
            aSet.ClearItem( RES_PAGEDESC );

            if( pRule || (SFX_ITEM_SET == pSet->GetItemState( RES_PARATR_NUMRULE,
                FALSE, &pItem ) && 0 != (pRule = GetDoc()->FindNumRulePtr(
                ((SwNumRuleItem*)pItem)->GetValue() )) &&
                pRule && pRule->IsAutoRule() ))
                aSet.ClearItem( RES_PARATR_NUMRULE );

            if( aSet.Count() )
                GetDoc()->ChgFmt(*pColl, aSet );
        }
        else
            GetDoc()->ChgFmt(*pColl, *pSet );
    }
}




