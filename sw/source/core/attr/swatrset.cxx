/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swatrset.cxx,v $
 * $Revision: 1.16.140.2 $
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
#include <svl/whiter.hxx>
#include <svx/colritem.hxx>
#include <svx/brshitem.hxx>
#include <svx/bolnitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/xtable.hxx>
#include <fmtpdsc.hxx>
#include <pagedesc.hxx>
#include <charfmt.hxx>
#include <doc.hxx>
#include <node.hxx>
#include <paratr.hxx>		// fuer SetModifyAtAttr
#include <cellatr.hxx>		// fuer SetModifyAtAttr
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#include <istyleaccess.hxx>
#include <numrule.hxx>
// --> OD 2008-03-19 #refactorlists#
#include <list.hxx>
// <--


SwAttrPool::SwAttrPool( SwDoc* pD )
    : SfxItemPool( String::CreateFromAscii(
                                RTL_CONSTASCII_STRINGPARAM( "SWG" )),
                    POOLATTR_BEGIN, POOLATTR_END-1,
                    aSlotTab, aAttrTab ),
    pDoc( pD )
{
    SetVersionMap( 1, 1, 60, pVersionMap1 );
    SetVersionMap( 2, 1, 75, pVersionMap2 );
    SetVersionMap( 3, 1, 86, pVersionMap3 );
    SetVersionMap( 4, 1,121, pVersionMap4 );
    // OD 2004-01-21 #i18732# - apply new version map
    SetVersionMap( 5, 1,130, pVersionMap5 );
    SetVersionMap( 6, 1,136, pVersionMap6 );
}

SwAttrPool::~SwAttrPool()
{
}

SwAttrSet::SwAttrSet( SwAttrPool& rPool, USHORT nWh1, USHORT nWh2 )
    : SfxItemSet( rPool, nWh1, nWh2 ), pOldSet( 0 ), pNewSet( 0 )
{
}


SwAttrSet::SwAttrSet( SwAttrPool& rPool, const USHORT* nWhichPairTable )
    : SfxItemSet( rPool, nWhichPairTable ), pOldSet( 0 ), pNewSet( 0 )
{
}


SwAttrSet::SwAttrSet( const SwAttrSet& rSet )
    : SfxItemSet( rSet ), pOldSet( 0 ), pNewSet( 0 )
{
}

SfxItemSet* SwAttrSet::Clone( BOOL bItems, SfxItemPool *pToPool ) const
{
    if ( pToPool && pToPool != GetPool() )
    {
        SwAttrPool* pAttrPool = dynamic_cast< SwAttrPool* >(pToPool);
        SfxItemSet* pTmpSet = 0;
        if ( !pAttrPool )
            pTmpSet = SfxItemSet::Clone( bItems, pToPool );
        else
        {
            pTmpSet = new SwAttrSet( *pAttrPool, GetRanges() );
            if ( bItems )
            {
                SfxWhichIter aIter(*pTmpSet);
                USHORT nWhich = aIter.FirstWhich();
                while ( nWhich )
                {
                    const SfxPoolItem* pItem;
                    if ( SFX_ITEM_SET == GetItemState( nWhich, FALSE, &pItem ) )
                        pTmpSet->Put( *pItem, pItem->Which() );
                    nWhich = aIter.NextWhich();
                }
            }
        }
        return pTmpSet;
    }
    else
        return bItems
                ? new SwAttrSet( *this )
                : new SwAttrSet( *GetPool(), GetRanges() );
}

int SwAttrSet::Put_BC( const SfxPoolItem& rAttr,
                    SwAttrSet* pOld, SwAttrSet* pNew )
{
    pNewSet = pNew;
    pOldSet = pOld;
    int nRet = 0 != SfxItemSet::Put( rAttr );
    pOldSet = pNewSet = 0;
    return nRet;
}


int SwAttrSet::Put_BC( const SfxItemSet& rSet,
                    SwAttrSet* pOld, SwAttrSet* pNew )
{
    pNewSet = pNew;
    pOldSet = pOld;
    int nRet = 0 != SfxItemSet::Put( rSet );
    pOldSet = pNewSet = 0;
    return nRet;
}



USHORT SwAttrSet::ClearItem_BC( USHORT nWhich,
                        SwAttrSet* pOld, SwAttrSet* pNew )
{
    pNewSet = pNew;
    pOldSet = pOld;
    USHORT nRet = SfxItemSet::ClearItem( nWhich );
    pOldSet = pNewSet = 0;
    return nRet;
}


USHORT SwAttrSet::ClearItem_BC( USHORT nWhich1, USHORT nWhich2,
                        SwAttrSet* pOld, SwAttrSet* pNew )
{
    ASSERT( nWhich1 <= nWhich2, "kein gueltiger Bereich" );
    pNewSet = pNew;
    pOldSet = pOld;
    USHORT nRet = 0;
    for( ; nWhich1 <= nWhich2; ++nWhich1 )
        nRet = nRet + SfxItemSet::ClearItem( nWhich1 );
    pOldSet = pNewSet = 0;
    return nRet;
}



int SwAttrSet::Intersect_BC( const SfxItemSet& rSet,
                            SwAttrSet* pOld, SwAttrSet* pNew )
{
    pNewSet = pNew;
    pOldSet = pOld;
    SfxItemSet::Intersect( rSet );
    pOldSet = pNewSet = 0;
    return pNew ? pNew->Count() : ( pOld ? pOld->Count() : 0 );
}

// Notification-Callback
void  SwAttrSet::Changed( const SfxPoolItem& rOld,
                                const SfxPoolItem& rNew )
{
    if( pOldSet )
        pOldSet->PutChgd( rOld );

    if( pNewSet )
        pNewSet->PutChgd( rNew );
}


// ----------------------------------------------------------------
// Sonderbehandlung fuer einige Attribute
// Setze den Modify-Pointer (alten pDefinedIn) bei folgenden Attributen:
//	- SwFmtDropCaps
//	- SwFmtPageDesc
// (Wird beim Einfuegen in Formate/Nodes gerufen)
// ----------------------------------------------------------------

bool SwAttrSet::SetModifyAtAttr( const SwModify* pModify )
{
    bool bSet = false;

    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == GetItemState( RES_PAGEDESC, FALSE, &pItem ) &&
        ((SwFmtPageDesc*)pItem)->GetDefinedIn() != pModify  )
    {
        ((SwFmtPageDesc*)pItem)->ChgDefinedIn( pModify );
        bSet = true;
    }

    if( SFX_ITEM_SET == GetItemState( RES_PARATR_DROP, FALSE, &pItem ) &&
        ((SwFmtDrop*)pItem)->GetDefinedIn() != pModify )
    {
        // CharFormat gesetzt und dann noch in unterschiedlichen
        // Attribut Pools, dann muss das CharFormat kopiert werden!
        SwCharFmt* pCharFmt;
        if( 0 != ( pCharFmt = ((SwFmtDrop*)pItem)->GetCharFmt() )
            && GetPool() != pCharFmt->GetAttrSet().GetPool() )
        {
           pCharFmt = GetDoc()->CopyCharFmt( *pCharFmt );
           ((SwFmtDrop*)pItem)->SetCharFmt( pCharFmt );
        }
        ((SwFmtDrop*)pItem)->ChgDefinedIn( pModify );
        bSet = true;
    }

    if( SFX_ITEM_SET == GetItemState( RES_BOXATR_FORMULA, FALSE, &pItem ) &&
        ((SwTblBoxFormula*)pItem)->GetDefinedIn() != pModify )
    {
        ((SwTblBoxFormula*)pItem)->ChgDefinedIn( pModify );
        bSet = true;
    }

    return bSet;
}

void SwAttrSet::CopyToModify( SwModify& rMod ) const
{
    // kopiere die Attribute ggfs. ueber Dokumentgrenzen
    SwCntntNode* pCNd = PTR_CAST( SwCntntNode, &rMod );
    SwFmt* pFmt = PTR_CAST( SwFmt, &rMod );

    if( pCNd || pFmt )
    {
        if( Count() )
        {
            // --> OD 2008-08-15 #i92811#
            SfxStringItem* pNewListIdItem( 0 );
            // <--

            const SfxPoolItem* pItem;
            const SwDoc *pSrcDoc = GetDoc();
            SwDoc *pDstDoc = pCNd ? pCNd->GetDoc() : pFmt->GetDoc();

            // muss die NumRule kopiert werden?
            if( pSrcDoc != pDstDoc && SFX_ITEM_SET == GetItemState(
                                    RES_PARATR_NUMRULE, FALSE, &pItem ) )
            {
                const String& rNm = ((SwNumRuleItem*)pItem)->GetValue();
                if( rNm.Len() )
                {
                    SwNumRule* pDestRule = pDstDoc->FindNumRulePtr( rNm );
                    if( pDestRule )
                        pDestRule->SetInvalidRule( TRUE );
                    else
                        pDstDoc->MakeNumRule( rNm,
                                            pSrcDoc->FindNumRulePtr( rNm ) );
                }
            }

            // --> OD 2008-03-19 #refactorlists#
            // copy list and if needed also the corresponding list style
            // for text nodes
            if ( pSrcDoc != pDstDoc &&
                 pCNd && pCNd->IsTxtNode() &&
                 GetItemState( RES_PARATR_LIST_ID, FALSE, &pItem ) == SFX_ITEM_SET )
            {
                const String& sListId =
                        dynamic_cast<const SfxStringItem*>(pItem)->GetValue();
                if ( sListId.Len() > 0 &&
                     !pDstDoc->getListByName( sListId ) )
                {
                    const SwList* pList = pSrcDoc->getListByName( sListId );
                    // copy list style, if needed
                    const String sDefaultListStyleName =
                                            pList->GetDefaultListStyleName();
                    // --> OD 2008-08-15 #i92811#
                    const SwNumRule* pDstDocNumRule =
                                pDstDoc->FindNumRulePtr( sDefaultListStyleName );
                    if ( !pDstDocNumRule )
                    {
                        pDstDoc->MakeNumRule( sDefaultListStyleName,
                                              pSrcDoc->FindNumRulePtr( sDefaultListStyleName ) );
                    }
                    else
                    {
                        const SwNumRule* pSrcDocNumRule =
                                pSrcDoc->FindNumRulePtr( sDefaultListStyleName );
                        // If list id of text node equals the list style's
                        // default list id in the source document, the same
                        // should be hold in the destination document.
                        // Thus, create new list id item.
                        if ( sListId == pSrcDocNumRule->GetDefaultListId() )
                        {
                            pNewListIdItem = new SfxStringItem (
                                            RES_PARATR_LIST_ID,
                                            pDstDocNumRule->GetDefaultListId() );
                        }
                    }
                    // check again, if list exist, because <SwDoc::MakeNumRule(..)>
                    // could have also created it.
                    if ( pNewListIdItem == 0 &&
                         !pDstDoc->getListByName( sListId ) )
                    {
                        // copy list
                        pDstDoc->createList( sListId, sDefaultListStyleName );
                    }
                    // <--
                }
            }
            // <--

            // JP 04.02.99: Task #61467# Seitenvorlagenwechsel mit kopieren
            //				Gegenueber dem alten Verhalten, sie zu entfernen
            const SwPageDesc* pPgDesc;
            if( pSrcDoc != pDstDoc && SFX_ITEM_SET == GetItemState(
                                            RES_PAGEDESC, FALSE, &pItem ) &&
                0 != ( pPgDesc = ((SwFmtPageDesc*)pItem)->GetPageDesc()) )
            {
                SfxItemSet aTmpSet( *this );

                SwPageDesc* pDstPgDesc = pDstDoc->FindPageDescByName(
                                                    pPgDesc->GetName() );
                if( !pDstPgDesc )
                {
                    // dann kopieren, ansonsten den benutzen
                    pDstPgDesc = &pDstDoc->_GetPageDesc( pDstDoc->MakePageDesc(
                                                    pPgDesc->GetName() ));
                    pDstDoc->CopyPageDesc( *pPgDesc, *pDstPgDesc );
                }
                SwFmtPageDesc aDesc( pDstPgDesc );
                aDesc.SetNumOffset( ((SwFmtPageDesc*)pItem)->GetNumOffset() );
                aTmpSet.Put( aDesc );

                if( pCNd )
                {
                    // --> OD 2008-08-15 #i92811#
                    if ( pNewListIdItem != 0 )
                    {
                        aTmpSet.Put( *pNewListIdItem );
                    }
                    // <--
                    pCNd->SetAttr( aTmpSet );
                }
                else
                    pFmt->SetFmtAttr( aTmpSet );
            }
            else if( pCNd )
            {
                // --> OD 2008-08-15 #i92811#
                if ( pNewListIdItem != 0 )
                {
                    SfxItemSet aTmpSet( *this );
                    aTmpSet.Put( *pNewListIdItem );
                    pCNd->SetAttr( aTmpSet );
                }
                else
                {
                    pCNd->SetAttr( *this );
                }
                // <--
            }
            else
                pFmt->SetFmtAttr( *this );

            // --> OD 2008-08-15 #i92811#
            delete pNewListIdItem;
            pNewListIdItem = 0;
            // <--
        }
    }
#ifdef DBG_UTIL
    else
        ASSERT( !this, "weder Format noch ContentNode - keine Attribute kopiert");
#endif
}

// check if ID is InRange of AttrSet-Ids
BOOL IsInRange( const USHORT* pRange, const USHORT nId )
{
    while( *pRange )
    {
        if( *pRange <= nId && nId <= *(pRange+1) )
            return TRUE;
        pRange += 2;
    }
    return FALSE;
}

