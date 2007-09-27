/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SwUndoPageDesc.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 09:28:51 $
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

#include <tools/resid.hxx>
#include <doc.hxx>
#include <swundo.hxx>
#include <pagedesc.hxx>
#include <SwUndoPageDesc.hxx>
#include <SwRewriter.hxx>
#include <undobj.hxx>
#include <comcore.hrc>
#include <fmtcntnt.hxx>
#include <fmthdft.hxx>

#ifdef DEBUG
#include <ndindex.hxx>
#endif


#ifdef DEBUG
// Pure debug help function to have a quick look at the header/footer attributes.
void DebugHeaderFooterContent( const SwPageDesc& rPageDesc )
{
    ULONG nHeaderMaster = ULONG_MAX;
    ULONG nHeaderLeft = ULONG_MAX;
    ULONG nFooterMaster = ULONG_MAX;
    ULONG nFooterLeft = ULONG_MAX;
    int nHeaderCount = 0;
    int nLeftHeaderCount = 0;
    int nFooterCount = 0;
    int nLeftFooterCount = 0;
    bool bSharedHeader = false;
    bool bSharedFooter = false;

    SwFmtHeader& rHead = (SwFmtHeader&)rPageDesc.GetMaster().GetHeader();
    SwFmtFooter& rFoot = (SwFmtFooter&)rPageDesc.GetMaster().GetFooter();
    SwFmtHeader& rLeftHead = (SwFmtHeader&)rPageDesc.GetLeft().GetHeader();
    SwFmtFooter& rLeftFoot = (SwFmtFooter&)rPageDesc.GetLeft().GetFooter();
    if( rHead.IsActive() )
    {
        SwFrmFmt* pHeaderFmt = rHead.GetHeaderFmt();
        if( pHeaderFmt )
        {
            SwClientIter aIter( *pHeaderFmt );
            SwClient *pLast = aIter.GoStart();
            if( pLast )
                do
                {
                    ++nHeaderCount;
                } while( 0 != ( pLast = aIter++ ));
            const SwFmtCntnt* pCntnt = &pHeaderFmt->GetCntnt();
            if( pCntnt->GetCntntIdx() )
                nHeaderMaster = pCntnt->GetCntntIdx()->GetIndex();
            else
                nHeaderMaster = 0;
        }
        bSharedHeader = rPageDesc.IsHeaderShared();
        SwFrmFmt* pLeftHeaderFmt = rLeftHead.GetHeaderFmt();
        if( pLeftHeaderFmt )
        {
            SwClientIter aIter( *pLeftHeaderFmt );
            SwClient *pLast = aIter.GoStart();
            if( pLast )
                do
                {
                    ++nLeftHeaderCount;
                } while( 0 != ( pLast = aIter++ ));
            const SwFmtCntnt* pLeftCntnt = &pLeftHeaderFmt->GetCntnt();
            if( pLeftCntnt->GetCntntIdx() )
                nHeaderLeft = pLeftCntnt->GetCntntIdx()->GetIndex();
            else
                nHeaderLeft = 0;
        }
    }
    if( rFoot.IsActive() )
    {
        SwFrmFmt* pFooterFmt = rFoot.GetFooterFmt();
        if( pFooterFmt )
        {
            SwClientIter aIter( *pFooterFmt );
            SwClient *pLast = aIter.GoStart();
            if( pLast )
                do
                {
                    ++nFooterCount;
                } while( 0 != ( pLast = aIter++ ));
            const SwFmtCntnt* pCntnt = &pFooterFmt->GetCntnt();
            if( pCntnt->GetCntntIdx() )
                nFooterMaster = pCntnt->GetCntntIdx()->GetIndex();
            else
                nFooterMaster = 0;
        }
        bSharedFooter = rPageDesc.IsFooterShared();
        SwFrmFmt* pLeftFooterFmt = rLeftFoot.GetFooterFmt();
        if( pLeftFooterFmt )
        {
            SwClientIter aIter( *pLeftFooterFmt );
            SwClient *pLast = aIter.GoStart();
            if( pLast )
                do
                {
                    ++nLeftFooterCount;
                } while( 0 != ( pLast = aIter++ ));
            const SwFmtCntnt* pLeftCntnt = &pLeftFooterFmt->GetCntnt();
            if( pLeftCntnt->GetCntntIdx() )
                nFooterLeft = pLeftCntnt->GetCntntIdx()->GetIndex();
            else
                nFooterLeft = 0;
        }
    }
    int i = 0;
    ++i; // To set a breakpoint
}
#endif

SwUndoPageDesc::SwUndoPageDesc(const SwPageDesc & _aOld,
                               const SwPageDesc & _aNew,
                               SwDoc * _pDoc)
    : SwUndo( _aOld.GetName() != _aNew.GetName() ?
              UNDO_RENAME_PAGEDESC :
              UNDO_CHANGE_PAGEDESC ),
      aOld(_aOld, _pDoc), aNew(_aNew, _pDoc), pDoc(_pDoc), bExchange( false )
{
    ASSERT(0 != pDoc, "no document?");

#ifdef DEBUG
    DebugHeaderFooterContent( (SwPageDesc&)aOld );
    DebugHeaderFooterContent( (SwPageDesc&)aNew );
#endif

    /*
    The page description changes.
    If there are no header/footer content changes like header on/off or change from shared content
    to unshared etc., there is no reason to duplicate the content nodes (Crash i55547)
    But this happens, this Undo Ctor will destroy the unnecessary duplicate and manipulate the
    content pointer of the both page descriptions.
    */
    SwPageDesc &rOldDesc = (SwPageDesc&)aOld;
    SwPageDesc &rNewDesc = (SwPageDesc&)aNew;
    const SwFmtHeader& rOldHead = rOldDesc.GetMaster().GetHeader();
    const SwFmtHeader& rNewHead = rNewDesc.GetMaster().GetHeader();
    const SwFmtFooter& rOldFoot = rOldDesc.GetMaster().GetFooter();
    const SwFmtFooter& rNewFoot = rNewDesc.GetMaster().GetFooter();
    /* bExchange must not be set, if the old page descriptor will stay active.
    Two known situations:
    #i67735#: renaming a page descriptor
    #i67334#: changing the follow style
    If header/footer will be activated or deactivated, this undo will not work.
    */
    bExchange = ( aOld.GetName() == aNew.GetName() ) &&
        ( _aOld.GetFollow() == _aNew.GetFollow() ) &&
        ( rOldHead.IsActive() == rNewHead.IsActive() ) &&
        ( rOldFoot.IsActive() == rNewFoot.IsActive() );
    if( rOldHead.IsActive() && ( rOldDesc.IsHeaderShared() != rNewDesc.IsHeaderShared() ) )
        bExchange = false;
    if( rOldFoot.IsActive() && ( rOldDesc.IsFooterShared() != rNewDesc.IsFooterShared() ) )
        bExchange = false;
    if( bExchange )
    {
        if( rNewHead.IsActive() )
        {
            SwFrmFmt* pFormat = new SwFrmFmt( *rNewHead.GetHeaderFmt() );
            // The Ctor of this object will remove the duplicate!
            SwFmtHeader aFmtHeader( pFormat );
            if( !rNewDesc.IsHeaderShared() )
            {
                pFormat = new SwFrmFmt( *rNewDesc.GetLeft().GetHeader().GetHeaderFmt() );
                // The Ctor of this object will remove the duplicate!
                SwFmtHeader aFormatHeader( pFormat );
            }
        }
        // Same procedure for footers...
        if( rNewFoot.IsActive() )
        {
            SwFrmFmt* pFormat = new SwFrmFmt( *rNewFoot.GetFooterFmt() );
            // The Ctor of this object will remove the duplicate!
            SwFmtFooter aFmtFooter( pFormat );
            if( !rNewDesc.IsFooterShared() )
            {
                pFormat = new SwFrmFmt( *rNewDesc.GetLeft().GetFooter().GetFooterFmt() );
                // The Ctor of this object will remove the duplicate!
                SwFmtFooter aFormatFooter( pFormat );
            }
        }
    
        // After this exchange method the old page description will point to zero,
        // the new one will point to the node position of the original content nodes.
        ExchangeContentNodes( (SwPageDesc&)aOld, (SwPageDesc&)aNew );
#ifdef DEBUG
        DebugHeaderFooterContent( (SwPageDesc&)aOld );
        DebugHeaderFooterContent( (SwPageDesc&)aNew );
#endif
    }
}

SwUndoPageDesc::~SwUndoPageDesc()
{
}


void SwUndoPageDesc::ExchangeContentNodes( SwPageDesc& rSource, SwPageDesc &rDest )
{
    ASSERT( bExchange, "You shouldn't do that." );
    const SwFmtHeader& rDestHead = rDest.GetMaster().GetHeader();
    const SwFmtHeader& rSourceHead = rSource.GetMaster().GetHeader();
    if( rDestHead.IsActive() )
    {
        // Let the destination page descrition point to the source node position,
        // from now on this descriptor is responsible for the content nodes!
        const SfxPoolItem* pItem;
        rDest.GetMaster().GetAttrSet().GetItemState( RES_HEADER, FALSE, &pItem );
        SfxPoolItem *pNewItem = pItem->Clone();
        SwFrmFmt* pNewFmt = ((SwFmtHeader*)pNewItem)->GetHeaderFmt();
#ifdef DEBUG
        const SwFmtCntnt& rSourceCntnt = rSourceHead.GetHeaderFmt()->GetCntnt();
        (void)rSourceCntnt;
        const SwFmtCntnt& rDestCntnt = rDestHead.GetHeaderFmt()->GetCntnt();
        (void)rDestCntnt;
#endif
        pNewFmt->SetAttr( rSourceHead.GetHeaderFmt()->GetCntnt() );
        delete pNewItem;

        // Let the source page description point to zero node position,
        // it loses the responsible and can be destroyed without removing the content nodes.
        rSource.GetMaster().GetAttrSet().GetItemState( RES_HEADER, FALSE, &pItem );
        pNewItem = pItem->Clone();
        pNewFmt = ((SwFmtHeader*)pNewItem)->GetHeaderFmt();
        pNewFmt->SetAttr( SwFmtCntnt() );
        delete pNewItem;
        
        if( !rDest.IsHeaderShared() )
        {
            // Same procedure for unshared header..
            const SwFmtHeader& rSourceLeftHead = rSource.GetLeft().GetHeader();
            rDest.GetLeft().GetAttrSet().GetItemState( RES_HEADER, FALSE, &pItem );
            pNewItem = pItem->Clone();
            pNewFmt = ((SwFmtHeader*)pNewItem)->GetHeaderFmt();
#ifdef DEBUG
            const SwFmtCntnt& rSourceCntnt1 = rSourceLeftHead.GetHeaderFmt()->GetCntnt();
            (void)rSourceCntnt1;
            const SwFmtCntnt& rDestCntnt1 = rDest.GetLeft().GetHeader().GetHeaderFmt()->GetCntnt();
            (void)rDestCntnt1;
#endif
            pNewFmt->SetAttr( rSourceLeftHead.GetHeaderFmt()->GetCntnt() );
            delete pNewItem;
            rSource.GetLeft().GetAttrSet().GetItemState( RES_HEADER, FALSE, &pItem );
            pNewItem = pItem->Clone();
            pNewFmt = ((SwFmtHeader*)pNewItem)->GetHeaderFmt();
            pNewFmt->SetAttr( SwFmtCntnt() );
            delete pNewItem;
        }
    }
    // Same procedure for footers...
    const SwFmtFooter& rDestFoot = rDest.GetMaster().GetFooter();
    const SwFmtFooter& rSourceFoot = rSource.GetMaster().GetFooter();
    if( rDestFoot.IsActive() )
    {
        const SfxPoolItem* pItem;
        rDest.GetMaster().GetAttrSet().GetItemState( RES_FOOTER, FALSE, &pItem );
        SfxPoolItem *pNewItem = pItem->Clone();
        SwFrmFmt *pNewFmt = ((SwFmtFooter*)pNewItem)->GetFooterFmt();
        pNewFmt->SetAttr( rSourceFoot.GetFooterFmt()->GetCntnt() );
        delete pNewItem;

#ifdef DEBUG
        const SwFmtCntnt& rFooterSourceCntnt = rSourceFoot.GetFooterFmt()->GetCntnt();
        (void)rFooterSourceCntnt;
        const SwFmtCntnt& rFooterDestCntnt = rDestFoot.GetFooterFmt()->GetCntnt();
        (void)rFooterDestCntnt;
#endif
        rSource.GetMaster().GetAttrSet().GetItemState( RES_FOOTER, FALSE, &pItem );
        pNewItem = pItem->Clone();
        pNewFmt = ((SwFmtFooter*)pNewItem)->GetFooterFmt();
        pNewFmt->SetAttr( SwFmtCntnt() );
        delete pNewItem;
        
        if( !rDest.IsFooterShared() )
        {
            const SwFmtFooter& rSourceLeftFoot = rSource.GetLeft().GetFooter();
#ifdef DEBUG
            const SwFmtCntnt& rFooterSourceCntnt2 = rSourceLeftFoot.GetFooterFmt()->GetCntnt();
            const SwFmtCntnt& rFooterDestCntnt2 =
                rDest.GetLeft().GetFooter().GetFooterFmt()->GetCntnt();
            (void)rFooterSourceCntnt2;
            (void)rFooterDestCntnt2;
#endif
            rDest.GetLeft().GetAttrSet().GetItemState( RES_FOOTER, FALSE, &pItem );
            pNewItem = pItem->Clone();
            pNewFmt = ((SwFmtFooter*)pNewItem)->GetFooterFmt();
            pNewFmt->SetAttr( rSourceLeftFoot.GetFooterFmt()->GetCntnt() );
            delete pNewItem;
            rSource.GetLeft().GetAttrSet().GetItemState( RES_FOOTER, FALSE, &pItem );
            pNewItem = pItem->Clone();
            pNewFmt = ((SwFmtFooter*)pNewItem)->GetFooterFmt();
            pNewFmt->SetAttr( SwFmtCntnt() );
            delete pNewItem;
        }
    }
}

void SwUndoPageDesc::Undo(SwUndoIter &)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);
    
    // Move (header/footer)content node responsibility from new page descriptor to old one again.
    if( bExchange )
        ExchangeContentNodes( (SwPageDesc&)aNew, (SwPageDesc&)aOld ); 
    pDoc->ChgPageDesc(aOld.GetName(), aOld);
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDesc::Redo(SwUndoIter &)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);

    // Move (header/footer)content node responsibility from old page descriptor to new one again.
    if( bExchange )
        ExchangeContentNodes( (SwPageDesc&)aOld, (SwPageDesc&)aNew ); 
    pDoc->ChgPageDesc(aNew.GetName(), aNew);
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDesc::Repeat(SwUndoIter &)
{
}

SwRewriter SwUndoPageDesc::GetRewriter() const
{
    SwRewriter aResult;

    aResult.AddRule(UNDO_ARG1, aOld.GetName());
    aResult.AddRule(UNDO_ARG2, SW_RES(STR_YIELDS));
    aResult.AddRule(UNDO_ARG3, aNew.GetName());

    return aResult;
}

// #116530#
SwUndoPageDescCreate::SwUndoPageDescCreate(const SwPageDesc * pNew, 
                                           SwDoc * _pDoc)
    : SwUndo(UNDO_CREATE_PAGEDESC), pDesc(pNew), aNew(*pNew, _pDoc), 
      pDoc(_pDoc)
{
    ASSERT(0 != pDoc, "no document?");
}

SwUndoPageDescCreate::~SwUndoPageDescCreate()
{
}

void SwUndoPageDescCreate::Undo(SwUndoIter &)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);

    // -> #116530#
    if (pDesc)
    {
        aNew = *pDesc;
        pDesc = NULL;
    }
    // <- #116530#

    pDoc->DelPageDesc(aNew.GetName(), TRUE);
    pDoc->DoUndo(bUndo);
}


void SwUndoPageDescCreate::Redo(SwUndoIter &)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);
    
    SwPageDesc aPageDesc = aNew;
    pDoc->MakePageDesc(aNew.GetName(), &aPageDesc, FALSE, TRUE); // #116530#

    pDoc->DoUndo(bUndo);
}

void SwUndoPageDescCreate::Repeat(SwUndoIter & rIt)
{
    Redo(rIt);
}

SwRewriter SwUndoPageDescCreate::GetRewriter() const
{
    SwRewriter aResult;

    if (pDesc)
        aResult.AddRule(UNDO_ARG1, pDesc->GetName());
    else
        aResult.AddRule(UNDO_ARG1, aNew.GetName());
        

    return aResult;
}

SwUndoPageDescDelete::SwUndoPageDescDelete(const SwPageDesc & _aOld,
                                           SwDoc * _pDoc)
    : SwUndo(UNDO_DELETE_PAGEDESC), aOld(_aOld, _pDoc), pDoc(_pDoc)
{
    ASSERT(0 != pDoc, "no document?");
}
                                           
SwUndoPageDescDelete::~SwUndoPageDescDelete()
{
}

void SwUndoPageDescDelete::Undo(SwUndoIter &)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);

    SwPageDesc aPageDesc = aOld;
    pDoc->MakePageDesc(aOld.GetName(), &aPageDesc, FALSE, TRUE); // #116530#
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDescDelete::Redo(SwUndoIter &)
{
    BOOL bUndo = pDoc->DoesUndo();

    pDoc->DoUndo(FALSE);
    pDoc->DelPageDesc(aOld.GetName(), TRUE); // #116530#
    pDoc->DoUndo(bUndo);
}

void SwUndoPageDescDelete::Repeat(SwUndoIter & rIt)
{
    Redo(rIt);
}

SwRewriter SwUndoPageDescDelete::GetRewriter() const
{
    SwRewriter aResult;

    aResult.AddRule(UNDO_ARG1, aOld.GetName());

    return aResult;
}
