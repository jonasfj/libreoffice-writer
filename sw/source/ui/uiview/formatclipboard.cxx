/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: formatclipboard.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:35:00 $
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

#include "formatclipboard.hxx"


#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _FORMAT_HXX
#include <format.hxx>
#endif
#ifndef _CHARFMT_HXX
#include <charfmt.hxx>
#endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _DOCSTYLE_HXX
#include <docstyle.hxx>
#endif
#ifndef _FCHRFMT_HXX
#include <fchrfmt.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
// header for class SdrView
#ifndef _SVDVIEW_HXX
#include <svx/svdview.hxx>
#endif
//SvxBrushItem
#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX
#include <svx/shaditem.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
// header for class SvxBoxInfoItem
#ifndef _SVX_BOXITEM_HXX
#include <svx/boxitem.hxx>
#endif
// header for class SvxFmtBreakItem
#ifndef _SVX_BRKITEM_HXX
#include <svx/brkitem.hxx>
#endif
// header for class SwFmtLayoutSplit
#ifndef _FMTTSPLT_HXX
#include <fmtlsplt.hxx>
#endif
// header for class SvxFmtKeepItem
#ifndef _SVX_KEEPITEM_HXX
#include <svx/keepitem.hxx>
#endif
// header for class SvxFrameDirectionItem
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif
#ifndef _PARATR_HXX
#include <paratr.hxx>
#endif
#ifndef _FMTPDSC_HXX
#include <fmtpdsc.hxx>
#endif
#ifndef _FMTROWSPLT_HXX
#include <fmtrowsplt.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>			// fuer die UndoIds
#endif
#include <boost/shared_ptr.hpp>

//#define FORMAT_PAINTBRUSH_ALSO_COPY_NUMBERFORMAT_FOR_TABLES 1

#ifdef FORMAT_PAINTBRUSH_ALSO_COPY_NUMBERFORMAT_FOR_TABLES
#ifndef _CELLATR_HXX
#include <cellatr.hxx>
#endif
#endif

/*--------------------------------------------------------------------
 --------------------------------------------------------------------*/

namespace
{
#define FORMAT_PAINTBRUSH_FRAME_IDS \
RES_FRMATR_BEGIN, RES_FILL_ORDER, \
/* no RES_FRM_SIZE */ \
RES_PAPER_BIN, RES_SURROUND, \
/* no RES_VERT_ORIENT */ \
/* no RES_HORI_ORIENT */ \
/* no RES_ANCHOR */ \
RES_BACKGROUND, RES_SHADOW, \
/* no RES_FRMMACRO */ \
RES_COL, RES_KEEP, \
/* no RES_URL */ \
RES_EDIT_IN_READONLY, RES_LAYOUT_SPLIT, \
/* no RES_CHAIN */ \
RES_TEXTGRID, RES_FRMATR_END-1,

#define FORMAT_PAINTBRUSH_PARAGRAPH_IDS \
RES_PARATR_BEGIN, RES_PARATR_END -1, \
FORMAT_PAINTBRUSH_FRAME_IDS \
FN_NUMBER_NEWSTART, FN_NUMBER_NEWSTART, \
FN_NUMBER_NEWSTART_AT, FN_NUMBER_NEWSTART_AT,

SfxItemSet* lcl_CreateEmptyItemSet( int nSelectionType, SfxItemPool& rPool
            , bool bNoCharacterFormats = false, bool bNoParagraphFormats = false )
{
    SfxItemSet* pItemSet = 0;
    if( nSelectionType & (nsSelectionType::SEL_FRM | nsSelectionType::SEL_OLE | nsSelectionType::SEL_GRF) )
    {
        pItemSet = new SfxItemSet(rPool,
                        FORMAT_PAINTBRUSH_FRAME_IDS
                        0);
    }
    else if( nSelectionType & nsSelectionType::SEL_DRW )
    {
        //is handled different
    }
    else if( nSelectionType == nsSelectionType::SEL_TBL )
    {
        pItemSet = new SfxItemSet(rPool,
                        SID_ATTR_BORDER_INNER,	SID_ATTR_BORDER_SHADOW, //SID_ATTR_BORDER_OUTER is inbetween
                        RES_BACKGROUND,         RES_SHADOW, //RES_BOX is inbetween
                        SID_ATTR_BRUSH_ROW, 	SID_ATTR_BRUSH_TABLE,
                        RES_BREAK,              RES_BREAK,
                        RES_PAGEDESC,           RES_PAGEDESC,
                        RES_LAYOUT_SPLIT,       RES_LAYOUT_SPLIT,
                        RES_ROW_SPLIT,          RES_ROW_SPLIT,
                        RES_KEEP,               RES_KEEP,
                        RES_FRAMEDIR,           RES_FRAMEDIR,
                        FN_PARAM_TABLE_HEADLINE, FN_PARAM_TABLE_HEADLINE,
                        FN_TABLE_BOX_TEXTDIRECTION, FN_TABLE_BOX_TEXTDIRECTION,
                        FN_TABLE_SET_VERT_ALIGN, FN_TABLE_SET_VERT_ALIGN,
#ifdef FORMAT_PAINTBRUSH_ALSO_COPY_NUMBERFORMAT_FOR_TABLES
                        RES_BOXATR_FORMAT,      RES_BOXATR_FORMAT,
#endif
                        0);
    }
    else if( nSelectionType & nsSelectionType::SEL_TXT )
    {
        if( bNoCharacterFormats )
            pItemSet = new SfxItemSet(rPool,
                    FORMAT_PAINTBRUSH_PARAGRAPH_IDS
                    0);
        else if( bNoParagraphFormats )
            pItemSet = new SfxItemSet(rPool,
                    RES_CHRATR_BEGIN, RES_CHRATR_END - 1,
                    0);
        else
            pItemSet = new SfxItemSet(rPool,
                    RES_CHRATR_BEGIN, RES_CHRATR_END - 1,
                    FORMAT_PAINTBRUSH_PARAGRAPH_IDS
                    0);
    }
    return pItemSet;
}

void lcl_getTableAttributes( SfxItemSet& rSet, SwWrtShell &rSh )
{
    SvxBrushItem aBrush( RES_BACKGROUND );
     rSh.GetBoxBackground(aBrush);
    rSet.Put( aBrush );
    if(rSh.GetRowBackground(aBrush))
        rSet.Put( aBrush, SID_ATTR_BRUSH_ROW );
    else
        rSet.InvalidateItem(SID_ATTR_BRUSH_ROW);
    rSh.GetTabBackground(aBrush);
    rSet.Put( aBrush, SID_ATTR_BRUSH_TABLE );

    SvxBoxInfoItem aBoxInfo( SID_ATTR_BORDER_INNER );
    rSet.Put(aBoxInfo);
    rSh.GetTabBorders( rSet );

    SvxFrameDirectionItem aBoxDirection( FRMDIR_ENVIRONMENT, RES_FRAMEDIR );
    if(rSh.GetBoxDirection( aBoxDirection ))
        rSet.Put(aBoxDirection, FN_TABLE_BOX_TEXTDIRECTION);

    rSet.Put(SfxUInt16Item(FN_TABLE_SET_VERT_ALIGN, rSh.GetBoxAlign()));

    rSet.Put( SfxUInt16Item( FN_PARAM_TABLE_HEADLINE, rSh.GetRowsToRepeat() ) );

    SwFrmFmt *pFrmFmt = rSh.GetTableFmt();
    if(pFrmFmt)
    {
        rSet.Put( pFrmFmt->GetShadow() );
        rSet.Put( pFrmFmt->GetBreak() );
        rSet.Put( pFrmFmt->GetPageDesc() );
        rSet.Put( pFrmFmt->GetLayoutSplit() );
        rSet.Put( pFrmFmt->GetKeep() );
        rSet.Put( pFrmFmt->GetFrmDir() );
    }

    SwFmtRowSplit* pSplit = 0;
    rSh.GetRowSplit(pSplit);
    if(pSplit)
        rSet.Put(*pSplit);

    //-- numberformat in cells
#ifdef FORMAT_PAINTBRUSH_ALSO_COPY_NUMBERFORMAT_FOR_TABLES
    rSh.GetTblBoxFormulaAttrs( rSet ); //RES_BOXATR_FORMAT
#endif
}

void lcl_setTableAttributes( const SfxItemSet& rSet, SwWrtShell &rSh )
{
    const SfxPoolItem* pItem = 0;
    BOOL bBorder = ( SFX_ITEM_SET == rSet.GetItemState( RES_BOX ) ||
            SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_BORDER_INNER ) );
    pItem = 0;
    BOOL bBackground = SFX_ITEM_SET == rSet.GetItemState( RES_BACKGROUND, FALSE, &pItem );
    const SfxPoolItem* pRowItem = 0, *pTableItem = 0;
    bBackground |= SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_BRUSH_ROW, FALSE, &pRowItem );
    bBackground |= SFX_ITEM_SET == rSet.GetItemState( SID_ATTR_BRUSH_TABLE, FALSE, &pTableItem );

    if(bBackground)
    {
        if(pItem)
            rSh.SetBoxBackground( *(const SvxBrushItem*)pItem );
        if(pRowItem)
        {
            SvxBrushItem aBrush(*(const SvxBrushItem*)pRowItem);
            aBrush.SetWhich(RES_BACKGROUND);
            rSh.SetRowBackground(aBrush);
        }
        if(pTableItem)
        {
            SvxBrushItem aBrush(*(const SvxBrushItem*)pTableItem);
            aBrush.SetWhich(RES_BACKGROUND);
            rSh.SetTabBackground( aBrush );
        }
    }
    if(bBorder)
        rSh.SetTabBorders( rSet );

    if( SFX_ITEM_SET == rSet.GetItemState( FN_PARAM_TABLE_HEADLINE, FALSE, &pItem) )
        rSh.SetRowsToRepeat( ((SfxUInt16Item*)pItem)->GetValue() );

    SwFrmFmt* pFrmFmt = rSh.GetTableFmt();
    if(pFrmFmt)
    {
        //RES_SHADOW
        pItem=0;
        rSet.GetItemState(rSet.GetPool()->GetWhich(RES_SHADOW), FALSE, &pItem);
        if(pItem)
            pFrmFmt->SetAttr( *pItem );

        //RES_BREAK
        pItem=0;
        rSet.GetItemState(rSet.GetPool()->GetWhich(RES_BREAK), FALSE, &pItem);
        if(pItem)
            pFrmFmt->SetAttr( *pItem );

        //RES_PAGEDESC
        pItem=0;
        rSet.GetItemState(rSet.GetPool()->GetWhich(RES_PAGEDESC), FALSE, &pItem);
        if(pItem)
            pFrmFmt->SetAttr( *pItem );

        //RES_LAYOUT_SPLIT
        pItem=0;
        rSet.GetItemState(rSet.GetPool()->GetWhich(RES_LAYOUT_SPLIT), FALSE, &pItem);
        if(pItem)
            pFrmFmt->SetAttr( *pItem );

        //RES_KEEP
        pItem=0;
        rSet.GetItemState(rSet.GetPool()->GetWhich(RES_KEEP), FALSE, &pItem);
        if(pItem)
            pFrmFmt->SetAttr( *pItem );

        //RES_FRAMEDIR
        pItem=0;
        rSet.GetItemState(rSet.GetPool()->GetWhich(RES_FRAMEDIR), FALSE, &pItem);
        if(pItem)
            pFrmFmt->SetAttr( *pItem );
    }

    if( SFX_ITEM_SET == rSet.GetItemState( FN_TABLE_BOX_TEXTDIRECTION, FALSE, &pItem) )
    {
        SvxFrameDirectionItem aDirection( FRMDIR_ENVIRONMENT, RES_FRAMEDIR );
        aDirection.SetValue(static_cast< const SvxFrameDirectionItem* >(pItem)->GetValue());
        rSh.SetBoxDirection(aDirection);
    }

    if( SFX_ITEM_SET == rSet.GetItemState( FN_TABLE_SET_VERT_ALIGN, FALSE, &pItem))
        rSh.SetBoxAlign(((SfxUInt16Item*)(pItem))->GetValue());

    if( SFX_ITEM_SET == rSet.GetItemState( RES_ROW_SPLIT, FALSE, &pItem) )
        rSh.SetRowSplit(*static_cast<const SwFmtRowSplit*>(pItem));

    //-- numberformat in cells
#ifdef FORMAT_PAINTBRUSH_ALSO_COPY_NUMBERFORMAT_FOR_TABLES
    if( SFX_ITEM_SET == rSet.GetItemState( RES_BOXATR_FORMAT, FALSE, &pItem ))
    {
        SfxItemSet aBoxSet( *rSet.GetPool(), RES_BOXATR_FORMAT, RES_BOXATR_FORMAT );
        aBoxSet.Put( SwTblBoxNumFormat( ((SfxUInt32Item*)pItem)->GetValue() ));
        rSh.SetTblBoxFormulaAttrs( aBoxSet );

    }
#endif
}
}//end anonymous namespace

SwFormatClipboard::SwFormatClipboard()
        : m_nSelectionType(0)
        , m_pItemSet(0)
        , m_pTableItemSet(0)
        , m_bPersistentCopy(false)
{
}
SwFormatClipboard::~SwFormatClipboard()
{
    if(m_pItemSet)
        delete m_pItemSet;
    if(m_pTableItemSet)
        delete m_pTableItemSet;
}

bool SwFormatClipboard::HasContent() const
{
    return m_pItemSet!=0
        || m_pTableItemSet != 0
        || m_aCharStyle.Len()
        || m_aParaStyle.Len()
        ;
}
bool SwFormatClipboard::HasContentForThisType( int nSelectionType ) const
{
    if( !HasContent() )
        return false;

    if( m_nSelectionType == nSelectionType )
        return true;

    if(   ( nSelectionType & (nsSelectionType::SEL_FRM | nsSelectionType::SEL_OLE | nsSelectionType::SEL_GRF) )
        &&
        ( m_nSelectionType & (nsSelectionType::SEL_FRM | nsSelectionType::SEL_OLE | nsSelectionType::SEL_GRF) )
        )
        return true;

    if( nSelectionType & nsSelectionType::SEL_TXT && m_nSelectionType & nsSelectionType::SEL_TXT )
        return true;

    return false;
}

bool SwFormatClipboard::CanCopyThisType( int nSelectionType ) const
{
    if( nSelectionType & (nsSelectionType::SEL_FRM | nsSelectionType::SEL_OLE | nsSelectionType::SEL_GRF
         | nsSelectionType::SEL_TXT | nsSelectionType::SEL_DRW | nsSelectionType::SEL_TBL | nsSelectionType::SEL_TBL_CELLS ) )
         return true;
    return false;
}

void SwFormatClipboard::Copy( SwWrtShell& rWrtShell, SfxItemPool& rPool, bool bPersistentCopy )
{
    this->Erase();
    m_bPersistentCopy = bPersistentCopy;

    int nSelectionType = rWrtShell.GetSelectionType();
    SfxItemSet* pItemSet = lcl_CreateEmptyItemSet( nSelectionType, rPool );

    rWrtShell.StartAction();
    rWrtShell.Push();
    if( nSelectionType == nsSelectionType::SEL_TXT )
    {
        SwPaM* pCrsr = rWrtShell.GetCrsr();
        //select one character only to get the attributes of this single character only
        BOOL bHasSelection = pCrsr->HasMark();
        BOOL bForwardSelection = FALSE;

        if(!bHasSelection) //check for and handle multiselections
        {
            if( pCrsr->GetPrev() != pCrsr && pCrsr->GetPrev() != 0)
            {
                pCrsr = (SwPaM*)pCrsr->GetPrev();
                bForwardSelection = (*pCrsr->GetPoint()) > (*pCrsr->GetMark());
                bHasSelection = true;
                pCrsr->DeleteMark();
                pCrsr->SetMark();
                rWrtShell.KillPams();
                pCrsr = rWrtShell.GetCrsr();
            }
        }
        else
            bForwardSelection = (*pCrsr->GetPoint()) > (*pCrsr->GetMark());
        pCrsr->DeleteMark();
        pCrsr->SetMark();

        if( !bHasSelection && rWrtShell.IsInRightToLeftText() )
            bForwardSelection = !bForwardSelection;

        if( !( !bHasSelection && rWrtShell.IsEndPara() ) )
            pCrsr->Move( bForwardSelection ? fnMoveBackward : fnMoveForward );
    }

    if(pItemSet)
    {
        if( nSelectionType & (nsSelectionType::SEL_FRM | nsSelectionType::SEL_OLE | nsSelectionType::SEL_GRF) )
            rWrtShell.GetFlyFrmAttr(*pItemSet);
        else
        {
            rWrtShell.GetAttr(*pItemSet);

            // additional numbering properties for paragraph styles
            if( nSelectionType & nsSelectionType::SEL_TXT && rWrtShell.GetCurNumRule() )
            {
                SfxBoolItem aStart(FN_NUMBER_NEWSTART, rWrtShell.IsNumRuleStart());
                pItemSet->Put(aStart);
                SfxUInt16Item aStartAt(FN_NUMBER_NEWSTART_AT, rWrtShell.IsNodeNumStart());
                pItemSet->Put(aStartAt);
            }
        }
    }
    else if ( nSelectionType & nsSelectionType::SEL_DRW )
    {
        SdrView* pDrawView = rWrtShell.GetDrawView();
        if(pDrawView)
        {
            BOOL bOnlyHardAttr = TRUE;
            if( pDrawView->AreObjectsMarked() )
            {
                pItemSet = new SfxItemSet( pDrawView->GetAttrFromMarked(bOnlyHardAttr) );
                //remove attributes defining the type/data of custom shapes
                pItemSet->ClearItem(SDRATTR_CUSTOMSHAPE_ENGINE);
                pItemSet->ClearItem(SDRATTR_CUSTOMSHAPE_DATA);
                pItemSet->ClearItem(SDRATTR_CUSTOMSHAPE_GEOMETRY);
                pItemSet->ClearItem(SDRATTR_CUSTOMSHAPE_REPLACEMENT_URL);
            }
        }
    }
    if( nSelectionType & nsSelectionType::SEL_TBL_CELLS )//only copy table attributes if really cells are selected (not only text in tables)
    {
        m_pTableItemSet = lcl_CreateEmptyItemSet( nsSelectionType::SEL_TBL, rPool );
        lcl_getTableAttributes( *m_pTableItemSet, rWrtShell );
    }

    m_nSelectionType = nSelectionType;
    m_pItemSet = pItemSet;

    if( nSelectionType & nsSelectionType::SEL_TXT )
    {
        SwFmt* pFmt = rWrtShell.GetCurCharFmt();
        if( pFmt )
            m_aCharStyle = pFmt->GetName();

        pFmt = rWrtShell.GetCurTxtFmtColl();
        if( pFmt )
            m_aParaStyle = pFmt->GetName();
    }
    rWrtShell.Pop(FALSE);
    rWrtShell.EndAction();
}
typedef boost::shared_ptr< SfxPoolItem > SfxPoolItemSharedPtr;
typedef std::vector< SfxPoolItemSharedPtr > ItemVector;
// #144857# collect all PoolItems from the applied styles
void lcl_AppendSetItems( ItemVector& rItemVector, const SfxItemSet& rStyleAttrSet )
{
    const USHORT*  pRanges = rStyleAttrSet.GetRanges();
    while( *pRanges )
    {
        for ( USHORT nWhich = *pRanges; nWhich <= *(pRanges+1); ++nWhich )
        {
            const SfxPoolItem* pItem;
            if( SFX_ITEM_SET == rStyleAttrSet.GetItemState( nWhich, sal_False, &pItem ) )
            {
                rItemVector.push_back( SfxPoolItemSharedPtr( pItem->Clone() ) );
            }
        }
        pRanges += 2;
    }
}
// #144857# remove all items that are inherited from the styles
void lcl_RemoveEqualItems( SfxItemSet& rTemplateItemSet, ItemVector& rItemVector )
{
    ItemVector::iterator aEnd = rItemVector.end();
    ItemVector::iterator aIter = rItemVector.begin();
    while( aIter != aEnd )
    {
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == rTemplateItemSet.GetItemState( (*aIter)->Which(), sal_True, &pItem ) &&
            *pItem == *(*aIter) )
        {
            rTemplateItemSet.ClearItem( (*aIter)->Which() );
        }
        ++aIter;
    }
}

void SwFormatClipboard::Paste( SwWrtShell& rWrtShell, SfxStyleSheetBasePool* pPool
                              , bool bNoCharacterFormats, bool bNoParagraphFormats )
{
    int nSelectionType = rWrtShell.GetSelectionType();
    if( !this->HasContentForThisType(nSelectionType) )
    {
        if(!m_bPersistentCopy)
            this->Erase();
        return;
    }

    rWrtShell.StartAction();
    rWrtShell.StartUndo(UNDO_INSATTR);

    ItemVector aItemVector;
    if(pPool) //to find the styles we need the pool
    {
        if( nSelectionType & nsSelectionType::SEL_TXT )
        {
            if(m_aCharStyle.Len() && !bNoCharacterFormats )
            {
                SwDocStyleSheet* pStyle = (SwDocStyleSheet*)pPool->Find(m_aCharStyle, SFX_STYLE_FAMILY_CHAR);
                if( pStyle )
                {
                    SwFmtCharFmt aFmt(pStyle->GetCharFmt());
                    // #144857# collect items from character style
                    lcl_AppendSetItems( aItemVector, aFmt.GetCharFmt()->GetAttrSet());
                    USHORT nFlags=0; //(nMode & KEY_SHIFT) ? SETATTR_DONTREPLACE : SETATTR_DEFAULT;
                    rWrtShell.SetAttr( aFmt, nFlags );
                }
            }
            if(m_aParaStyle.Len() && !bNoParagraphFormats )
            {
                SwDocStyleSheet* pStyle = (SwDocStyleSheet*)pPool->Find(m_aParaStyle, SFX_STYLE_FAMILY_PARA);
                if( pStyle )
                {
                    // #144857# collect items from paragraph style
                    lcl_AppendSetItems( aItemVector, pStyle->GetCollection()->GetAttrSet());
                    rWrtShell.SetTxtFmtColl( pStyle->GetCollection() );
                }
            }
        }
    }
    if(m_pItemSet)
    {
        if( nSelectionType & nsSelectionType::SEL_DRW )
        {
            SdrView* pDrawView = rWrtShell.GetDrawView();
            if(pDrawView)
            {
                BOOL bReplaceAll = TRUE;
                pDrawView->SetAttrToMarked(*m_pItemSet, bReplaceAll);
            }
        }
        else
        {
            SfxItemSet* pTemplateItemSet = lcl_CreateEmptyItemSet(
                              nSelectionType, *m_pItemSet->GetPool()
                              , bNoCharacterFormats, bNoParagraphFormats );
            if(pTemplateItemSet)
            {
                pTemplateItemSet->Put( *m_pItemSet );
                // #144857# only _set_ attributes that differ from style attributes should be applied - the style is applied anyway
                lcl_RemoveEqualItems( *pTemplateItemSet, aItemVector );

                if( nSelectionType & (nsSelectionType::SEL_FRM | nsSelectionType::SEL_OLE | nsSelectionType::SEL_GRF) )
                    rWrtShell.SetFlyFrmAttr(*pTemplateItemSet);
                else
                {
                    rWrtShell.SetAttr(*pTemplateItemSet);

                    // additional numbering properties for paragraph styles
                    if( nSelectionType & nsSelectionType::SEL_TXT && rWrtShell.GetCurNumRule() )
                    {
                        if( SFX_ITEM_SET == pTemplateItemSet->GetItemState(FN_NUMBER_NEWSTART) )
                        {
                            BOOL bStart = ((SfxBoolItem&)pTemplateItemSet->Get(FN_NUMBER_NEWSTART)).GetValue();
                            USHORT nNumStart = USHRT_MAX;
                            if( SFX_ITEM_SET == pTemplateItemSet->GetItemState(FN_NUMBER_NEWSTART_AT) )
                            {
                                nNumStart = ((SfxUInt16Item&)pTemplateItemSet->Get(FN_NUMBER_NEWSTART_AT)).GetValue();
                                if(USHRT_MAX != nNumStart)
                                    bStart = FALSE;
                            }
                            rWrtShell.SetNumRuleStart(bStart);
                            rWrtShell.SetNodeNumStart(nNumStart);
                        }
                        else if( SFX_ITEM_SET == pTemplateItemSet->GetItemState(FN_NUMBER_NEWSTART_AT) )
                        {
                            USHORT nNumStart = ((SfxUInt16Item&)pTemplateItemSet->Get(FN_NUMBER_NEWSTART_AT)).GetValue();
                            rWrtShell.SetNodeNumStart(nNumStart);
                            rWrtShell.SetNumRuleStart(FALSE);
                        }
                    }
                }
                delete pTemplateItemSet;
            }
        }
    }

    if( m_pTableItemSet && nSelectionType & (nsSelectionType::SEL_TBL | nsSelectionType::SEL_TBL_CELLS) )
        lcl_setTableAttributes( *m_pTableItemSet, rWrtShell );

    rWrtShell.EndUndo(UNDO_INSATTR);
    rWrtShell.EndAction();

    if(!m_bPersistentCopy)
        this->Erase();
}

void SwFormatClipboard::Erase()
{
    m_nSelectionType = 0;
    if(m_pItemSet)
    {
        delete m_pItemSet;
        m_pItemSet = 0;
    }
    if(m_pTableItemSet)
    {
        delete m_pTableItemSet;
        m_pTableItemSet = 0;
    }
    if( m_aCharStyle.Len() )
        m_aCharStyle.Erase();
    if( m_aParaStyle.Len() )
        m_aParaStyle.Erase();

    m_bPersistentCopy = false;
}
