/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fmtcol.cxx,v $
 *
 *  $Revision: 1.23 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 15:56:28 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <svx/fhgtitem.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>			// fuer GetAttrPool
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
// --> OD 2006-11-22 #i71574#
#ifndef _FMTCOLFUNC_HXX
#include <fmtcolfunc.hxx>
#endif
// <--
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#ifndef _CALC_HXX
#include <calc.hxx>
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif
#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
#ifndef _PARATR_HXX
#include <paratr.hxx>
#endif

TYPEINIT1( SwTxtFmtColl, SwFmtColl );
TYPEINIT1( SwGrfFmtColl, SwFmtColl );
TYPEINIT1( SwConditionTxtFmtColl, SwTxtFmtColl );
TYPEINIT1( SwCollCondition, SwClient );

SV_IMPL_PTRARR( SwFmtCollConditions, SwCollConditionPtr );


// --> OD 2006-11-22 #i71574#
void TxtFmtCollFunc::CheckTxtFmtCollForDeletionOfAssignmentToOutlineStyle(
                                        SwFmt* pFmt,
                                        const SwNumRuleItem* pNewNumRuleItem )
{
    SwTxtFmtColl* pTxtFmtColl = dynamic_cast<SwTxtFmtColl*>(pFmt);
    if ( !pTxtFmtColl )
    {
#if OSL_DEBUG_LEVEL > 1
        ASSERT( false,
                "<TxtFmtCollFunc::CheckTxtFmtCollFuncForDeletionOfAssignmentToOutlineStyle> - misuse of method - it's only for instances of <SwTxtFmtColl>" );
#endif
        return;
    }

    // --> OD 2007-01-24 #i73790#
//    if ( pTxtFmtColl->AssignedToListLevelOfOutlineStyle() )
    if ( !pTxtFmtColl->StayAssignedToListLevelOfOutlineStyle() &&
         pTxtFmtColl->AssignedToListLevelOfOutlineStyle() )
    // <--
    {
        if ( !pNewNumRuleItem )
        {
            pTxtFmtColl->GetItemState( RES_PARATR_NUMRULE, FALSE, (const SfxPoolItem**)&pNewNumRuleItem );
        }
        if ( pNewNumRuleItem )
        {
            String sNumRuleName = pNewNumRuleItem->GetValue();
            if ( sNumRuleName.Len() == 0 ||
                 sNumRuleName != pTxtFmtColl->GetDoc()->GetOutlineNumRule()->GetName() )
            {
                // delete assignment of paragraph style to list level of outline style.
                pTxtFmtColl->DeleteAssignmentToListLevelOfOutlineStyle();
            }
        }
    }
}
// <--

/*
 * SwTxtFmtColl  TXT
 */

void SwTxtFmtColl::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    if( GetDoc()->IsInDtor() )
    {
        SwFmtColl::Modify( pOld, pNew );
        return;
    }

    // --> OD 2006-06-16 #i66431# - adjust type of <bNewParent>
    bool bNewParent( false );
    // <--
    SvxULSpaceItem *pNewULSpace = 0, *pOldULSpace = 0;
    SvxLRSpaceItem *pNewLRSpace = 0, *pOldLRSpace = 0;
    SvxFontHeightItem* aFontSizeArr[3] = {0,0,0};
    // --> OD 2006-10-17 #i70223#
    const bool bAssignedToListLevelOfOutlineStyle( AssignedToListLevelOfOutlineStyle() );
    const SwNumRuleItem* pNewNumRuleItem( 0L );
    // <--

    SwAttrSetChg *pNewChgSet = 0,  *pOldChgSet = 0;

    switch( pOld ? pOld->Which() : pNew ? pNew->Which() : 0 )
    {
    case RES_ATTRSET_CHG:
        // nur neu berechnen, wenn nicht wir der "Versender" sind !!!
        pNewChgSet = (SwAttrSetChg*)pNew;
        pOldChgSet = (SwAttrSetChg*)pOld;
        pNewChgSet->GetChgSet()->GetItemState(
            RES_LR_SPACE, FALSE, (const SfxPoolItem**)&pNewLRSpace );
        pNewChgSet->GetChgSet()->GetItemState(
            RES_UL_SPACE, FALSE, (const SfxPoolItem**)&pNewULSpace );
        pNewChgSet->GetChgSet()->GetItemState( RES_CHRATR_FONTSIZE,
                        FALSE, (const SfxPoolItem**)&(aFontSizeArr[0]) );
        pNewChgSet->GetChgSet()->GetItemState( RES_CHRATR_CJK_FONTSIZE,
                        FALSE, (const SfxPoolItem**)&(aFontSizeArr[1]) );
        pNewChgSet->GetChgSet()->GetItemState( RES_CHRATR_CTL_FONTSIZE,
                        FALSE, (const SfxPoolItem**)&(aFontSizeArr[2]) );
        // --> OD 2006-10-17 #i70223#
        if ( bAssignedToListLevelOfOutlineStyle )
        {
            pNewChgSet->GetChgSet()->GetItemState( RES_PARATR_NUMRULE, FALSE,
                                                   (const SfxPoolItem**)&pNewNumRuleItem );
        }
        // <--

        break;

    case RES_FMT_CHG:
        if( GetAttrSet().GetParent() )
        {
            const SfxItemSet* pParent = GetAttrSet().GetParent();
            pNewLRSpace = (SvxLRSpaceItem*)&pParent->Get( RES_LR_SPACE );
            pNewULSpace = (SvxULSpaceItem*)&pParent->Get( RES_UL_SPACE );
            aFontSizeArr[0] = (SvxFontHeightItem*)&pParent->Get( RES_CHRATR_FONTSIZE );
            aFontSizeArr[1] = (SvxFontHeightItem*)&pParent->Get( RES_CHRATR_CJK_FONTSIZE );
            aFontSizeArr[2] = (SvxFontHeightItem*)&pParent->Get( RES_CHRATR_CTL_FONTSIZE );
            // --> OD 2006-06-16 #i66431#
            // modify has to be propagated, because of new parent format.
            bNewParent = true;
            // <--
        }
        break;

    case RES_LR_SPACE:
        pNewLRSpace = (SvxLRSpaceItem*)pNew;
        break;
    case RES_UL_SPACE:
        pNewULSpace = (SvxULSpaceItem*)pNew;
        break;
    case RES_CHRATR_FONTSIZE:
        aFontSizeArr[0] = (SvxFontHeightItem*)pNew;
        break;
    case RES_CHRATR_CJK_FONTSIZE:
        aFontSizeArr[1] = (SvxFontHeightItem*)pNew;
        break;
    case RES_CHRATR_CTL_FONTSIZE:
        aFontSizeArr[2] = (SvxFontHeightItem*)pNew;
        break;
    // --> OD 2006-10-17 #i70223#
    case RES_PARATR_NUMRULE:
    {
        if ( bAssignedToListLevelOfOutlineStyle )
        {
            pNewNumRuleItem = (SwNumRuleItem*)pNew;
        }
    }
    default:
        break;
    }

    // --> OD 2006-10-17 #i70223#
    if ( bAssignedToListLevelOfOutlineStyle && pNewNumRuleItem )
    {
        TxtFmtCollFunc::CheckTxtFmtCollForDeletionOfAssignmentToOutlineStyle(
                                                        this, pNewNumRuleItem );
    }
    // <--

    int bWeiter = TRUE;

    // dann pruefe doch mal gegen die eigenen Attribute
    if( pNewLRSpace && SFX_ITEM_SET == GetItemState( RES_LR_SPACE, FALSE,
                                        (const SfxPoolItem**)&pOldLRSpace ))
    {
        int bChg = FALSE;
        if( pOldLRSpace != pNewLRSpace )	// verhinder Rekursion (SetAttr!!)
        {
            SvxLRSpaceItem aNew( *pOldLRSpace );
            // wir hatten eine relative Angabe -> neu berechnen
            if( 100 != aNew.GetPropLeft() )
            {
                long nTmp = aNew.GetLeft();		// alten zum Vergleichen
                aNew.SetLeft( pNewLRSpace->GetLeft(), aNew.GetPropLeft() );
                bChg |= nTmp != aNew.GetLeft();
            }
            // wir hatten eine relative Angabe -> neu berechnen
            if( 100 != aNew.GetPropRight() )
            {
                long nTmp = aNew.GetRight();		// alten zum Vergleichen
                aNew.SetRight( pNewLRSpace->GetRight(), aNew.GetPropRight() );
                bChg |= nTmp != aNew.GetRight();
            }
            // wir hatten eine relative Angabe -> neu berechnen
            if( 100 != aNew.GetPropTxtFirstLineOfst() )
            {
                short nTmp = aNew.GetTxtFirstLineOfst();		// alten zum Vergleichen
                aNew.SetTxtFirstLineOfst( pNewLRSpace->GetTxtFirstLineOfst(),
                                            aNew.GetPropTxtFirstLineOfst() );
                bChg |= nTmp != aNew.GetTxtFirstLineOfst();
            }
            if( bChg )
            {
                SetAttr( aNew );
                bWeiter = 0 != pOldChgSet || bNewParent;
            }
            // bei uns absolut gesetzt -> nicht weiter propagieren, es sei
            // denn es wird bei uns gesetzt!
            else if( pNewChgSet )
                bWeiter = pNewChgSet->GetTheChgdSet() == &GetAttrSet();
        }
    }

    if( pNewULSpace && SFX_ITEM_SET == GetItemState(
            RES_UL_SPACE, FALSE, (const SfxPoolItem**)&pOldULSpace ) &&
        pOldULSpace != pNewULSpace )	// verhinder Rekursion (SetAttr!!)
    {
        SvxULSpaceItem aNew( *pOldULSpace );
        int bChg = FALSE;
        // wir hatten eine relative Angabe -> neu berechnen
        if( 100 != aNew.GetPropUpper() )
        {
            USHORT nTmp = aNew.GetUpper();		// alten zum Vergleichen
            aNew.SetUpper( pNewULSpace->GetUpper(), aNew.GetPropUpper() );
            bChg |= nTmp != aNew.GetUpper();
        }
        // wir hatten eine relative Angabe -> neu berechnen
        if( 100 != aNew.GetPropLower() )
        {
            USHORT nTmp = aNew.GetLower();		// alten zum Vergleichen
            aNew.SetLower( pNewULSpace->GetLower(), aNew.GetPropLower() );
            bChg |= nTmp != aNew.GetLower();
        }
        if( bChg )
        {
            SetAttr( aNew );
            bWeiter = 0 != pOldChgSet || bNewParent;
        }
        // bei uns absolut gesetzt -> nicht weiter propagieren, es sei
        // denn es wird bei uns gesetzt!
        else if( pNewChgSet )
            bWeiter = pNewChgSet->GetTheChgdSet() == &GetAttrSet();
    }


    for( int nC = 0, nArrLen = sizeof(aFontSizeArr) / sizeof( aFontSizeArr[0]);
            nC < nArrLen; ++nC )
    {
        SvxFontHeightItem *pFSize = aFontSizeArr[ nC ], *pOldFSize;
        if( pFSize && SFX_ITEM_SET == GetItemState(
            pFSize->Which(), FALSE, (const SfxPoolItem**)&pOldFSize ) &&
            // verhinder Rekursion (SetAttr!!)
            pFSize != pOldFSize )
        {
            if( 100 == pOldFSize->GetProp() &&
                SFX_MAPUNIT_RELATIVE == pOldFSize->GetPropUnit() )
            {
                // bei uns absolut gesetzt -> nicht weiter propagieren, es sei
                // denn es wird bei uns gesetzt!
                if( pNewChgSet )
                    bWeiter = pNewChgSet->GetTheChgdSet() == &GetAttrSet();
            }
            else
            {
                // wir hatten eine relative Angabe -> neu berechnen
                UINT32 nTmp = pOldFSize->GetHeight();		// alten zum Vergleichen
                SvxFontHeightItem aNew(240 , 100, pFSize->Which());
                aNew.SetHeight( pFSize->GetHeight(), pOldFSize->GetProp(),
                                pOldFSize->GetPropUnit() );
                if( nTmp != aNew.GetHeight() )
                {
                    SetAttr( aNew );
                    bWeiter = 0 != pOldChgSet || bNewParent;
                }
                // bei uns absolut gesetzt -> nicht weiter propagieren, es sei
                // denn es wird bei uns gesetzt!
                else if( pNewChgSet )
                    bWeiter = pNewChgSet->GetTheChgdSet() == &GetAttrSet();
            }
        }
    }

    if( bWeiter )
        SwFmtColl::Modify( pOld, pNew );
}

BOOL SwTxtFmtColl::IsAtDocNodeSet() const
{
    SwClientIter aIter( *(SwModify*)this );
    const SwNodes& rNds = GetDoc()->GetNodes();
    for( SwClient* pC = aIter.First(TYPE(SwCntntNode)); pC; pC = aIter.Next() )
        if( &((SwCntntNode*)pC)->GetNodes() == &rNds )
            return TRUE;

    return FALSE;
}

// --> OD 2007-01-24 #i73790#
USHORT SwTxtFmtColl::ResetAllFmtAttr()
{
    const bool bOldState( mbStayAssignedToListLevelOfOutlineStyle );
    mbStayAssignedToListLevelOfOutlineStyle = true;

    USHORT nRet = SwFmtColl::ResetAllFmtAttr();

    mbStayAssignedToListLevelOfOutlineStyle = bOldState;

    return nRet;
}
// <--

//FEATURE::CONDCOLL

SwCollCondition::SwCollCondition( SwTxtFmtColl* pColl, ULONG nMasterCond,
                                ULONG nSubCond )
    : SwClient( pColl ), nCondition( nMasterCond )
{
    aSubCondition.nSubCondition = nSubCond;
}


SwCollCondition::SwCollCondition( SwTxtFmtColl* pColl, ULONG nMasterCond,
                                    const String& rSubExp )
    : SwClient( pColl ), nCondition( nMasterCond )
{
    if( USRFLD_EXPRESSION & nCondition )
        aSubCondition.pFldExpression = new String( rSubExp );
    else
        aSubCondition.nSubCondition = 0;
}


SwCollCondition::SwCollCondition( const SwCollCondition& rCopy )
    : SwClient( (SwModify*)rCopy.GetRegisteredIn() ), nCondition( rCopy.nCondition )
{
    if( USRFLD_EXPRESSION & rCopy.nCondition )
        aSubCondition.pFldExpression = new String( *rCopy.GetFldExpression() );
    else
        aSubCondition.nSubCondition = rCopy.aSubCondition.nSubCondition;
}


SwCollCondition::~SwCollCondition()
{
    if( USRFLD_EXPRESSION & nCondition )
        delete aSubCondition.pFldExpression;
}


int SwCollCondition::operator==( const SwCollCondition& rCmp ) const
{
    int nRet = 0;
    if( nCondition == rCmp.nCondition )
    {
        if( USRFLD_EXPRESSION & nCondition )
        {
            // in der SubCondition steht die Expression fuer das UserFeld
            const String* pTmp = aSubCondition.pFldExpression;
            if( !pTmp )
                pTmp = rCmp.aSubCondition.pFldExpression;
            if( pTmp )
            {
                SwTxtFmtColl* pColl = GetTxtFmtColl();
                if( !pColl )
                    pColl = rCmp.GetTxtFmtColl();

                if( pColl )
                {
                    SwCalc aCalc( *pColl->GetDoc() );
                    nRet = 0 != aCalc.Calculate( *pTmp ).GetBool();
                }
            }
        }
        else if( aSubCondition.nSubCondition ==
                    rCmp.aSubCondition.nSubCondition )
            nRet = 1;
    }
    return nRet;
}


void SwCollCondition::SetCondition( ULONG nCond, ULONG nSubCond )
{
    if( USRFLD_EXPRESSION & nCondition )
        delete aSubCondition.pFldExpression;
    nCondition = nCond;
    aSubCondition.nSubCondition = nSubCond;
}


SwConditionTxtFmtColl::~SwConditionTxtFmtColl()
{
}

const SwCollCondition* SwConditionTxtFmtColl::HasCondition(
                        const SwCollCondition& rCond ) const
{
    const SwCollCondition* pFnd = 0;
    USHORT n;

    for( n = 0; n < aCondColls.Count(); ++n )
        if( *( pFnd = aCondColls[ n ]) == rCond )
            break;

    return n < aCondColls.Count() ? pFnd : 0;
}


void SwConditionTxtFmtColl::InsertCondition( const SwCollCondition& rCond )
{
    for( USHORT n = 0; n < aCondColls.Count(); ++n )
        if( *aCondColls[ n ] == rCond )
        {
            aCondColls.DeleteAndDestroy( n );
            break;
        }

    // nicht gefunden -> als einfuegen
    SwCollCondition* pNew = new SwCollCondition( rCond );
    aCondColls.Insert( pNew, aCondColls.Count() );
}


BOOL SwConditionTxtFmtColl::RemoveCondition( const SwCollCondition& rCond )
{
    BOOL bRet = FALSE;
    for( USHORT n = 0; n < aCondColls.Count(); ++n )
        if( *aCondColls[ n ] == rCond )
        {
            aCondColls.DeleteAndDestroy( n );
            bRet = TRUE;
        }

    return bRet;
}

void SwConditionTxtFmtColl::SetConditions( const SwFmtCollConditions& rCndClls )
{
    // Kopiere noch die Bedingungen
    // aber erst die alten loeschen!
    if( aCondColls.Count() )
        aCondColls.DeleteAndDestroy( 0, aCondColls.Count() );
    SwDoc& rDoc = *GetDoc();
    for( USHORT n = 0; n < rCndClls.Count(); ++n )
    {
        SwCollCondition* pFnd = rCndClls[ n ];
        SwTxtFmtColl* pTmpColl = pFnd->GetTxtFmtColl()
                                    ? rDoc.CopyTxtColl( *pFnd->GetTxtFmtColl() )
                                    : 0;
        SwCollCondition* pNew;
        if( USRFLD_EXPRESSION & pFnd->GetCondition() )
            pNew = new SwCollCondition( pTmpColl, pFnd->GetCondition(),
                                        *pFnd->GetFldExpression() );
        else
            pNew = new SwCollCondition( pTmpColl, pFnd->GetCondition(),
                                        pFnd->GetSubCondition() );
        aCondColls.Insert( pNew, n );
    }
}

void SwTxtFmtColl::SetOutlineLevel( BYTE nLevel )
{
    ASSERT( nLevel < MAXLEVEL || nLevel == NO_NUMBERING ,
                            "SwTxtFmtColl: Level too low" );
    if (!(nLevel < MAXLEVEL || nLevel == NO_NUMBERING))
        // take care of out-of-bounds values that may occasionally crash the office
        // such values may for example occur directly from some bad used API calls
        nOutlineLevel = NO_NUMBERING;
    else
        nOutlineLevel = nLevel;
}


//FEATURE::CONDCOLL
