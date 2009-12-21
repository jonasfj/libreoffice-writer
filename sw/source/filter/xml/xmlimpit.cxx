/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xmlimpit.cxx,v $
 * $Revision: 1.24 $
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
#include "xmlimpit.hxx"
#include <xmloff/xmluconv.hxx>
#include <svl/itempool.hxx>
#include <svl/poolitem.hxx>
#include <svl/itemset.hxx>
#include <xmloff/attrlist.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <svx/xmlcnitm.hxx>


#include "hintids.hxx"
#include "unomid.h"
#include <svx/unomid.hxx>
#include <svx/lrspitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/shaditem.hxx>
#include <svx/boxitem.hxx>
#include <svx/brkitem.hxx>
#include <svx/keepitem.hxx>
#include <svx/brshitem.hxx>
#include "fmtpdsc.hxx"
#include "fmtornt.hxx"
#include "fmtfsize.hxx"

#ifndef _FMTLSPLT_HXX
#include "fmtlsplt.hxx"
#endif
#include <xmloff/prhdlfac.hxx>
#include <xmloff/xmltypes.hxx>
#include "xmlithlp.hxx"
#include <com/sun/star/uno/Any.hxx>

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::xmloff::token;
using uno::Any;

SvXMLImportItemMapper::SvXMLImportItemMapper(
                                SvXMLItemMapEntriesRef rMapEntries,
                                USHORT nUnknWhich ) :
    mrMapEntries( rMapEntries ),
    nUnknownWhich( nUnknWhich )
{
}

SvXMLImportItemMapper::~SvXMLImportItemMapper()
{
}

/** fills the given itemset with the attributes in the given list */
void SvXMLImportItemMapper::importXML( SfxItemSet& rSet,
                                      uno::Reference< xml::sax::XAttributeList > xAttrList,
                                      const SvXMLUnitConverter& rUnitConverter,
                                       const SvXMLNamespaceMap& rNamespaceMap ) const
{
    INT16 nAttr = xAttrList->getLength();

    SvXMLAttrContainerItem *pUnknownItem = 0;
    for( INT16 i=0; i < nAttr; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );
        OUString aLocalName, aPrefix, aNamespace;
        USHORT nPrefix =
            rNamespaceMap.GetKeyByAttrName( rAttrName, &aPrefix, &aLocalName,
                                            &aNamespace );
        if( XML_NAMESPACE_XMLNS == nPrefix )
            continue;

        const OUString& rValue = xAttrList->getValueByIndex( i );

        // find a map entry for this attribute
        SvXMLItemMapEntry* pEntry = mrMapEntries->getByName( nPrefix, aLocalName );

        if( pEntry )
        {
            // we have a valid map entry here, so lets use it...
            if( 0 == (pEntry->nMemberId & (MID_SW_FLAG_NO_ITEM_IMPORT|
                                       MID_SW_FLAG_ELEMENT_ITEM_IMPORT)) )
            {
                // first get item from itemset
                const SfxPoolItem* pItem = 0;
                SfxItemState eState = rSet.GetItemState( pEntry->nWhichId, TRUE,
                                                         &pItem );

                // if its not set, try the pool
                if(SFX_ITEM_SET != eState && SFX_WHICH_MAX > pEntry->nWhichId )
                    pItem = &rSet.GetPool()->GetDefaultItem(pEntry->nWhichId);

                // do we have an item?
                if(eState >= SFX_ITEM_DEFAULT && pItem)
                {
                    SfxPoolItem *pNewItem = pItem->Clone();
                    BOOL bPut = FALSE;

                    if( 0 == (pEntry->nMemberId&MID_SW_FLAG_SPECIAL_ITEM_IMPORT) )
                    {
// 						bPut = pNewItem->importXML( rValue,
// 											pEntry->nMemberId & MID_SW_FLAG_MASK,
// 											rUnitConverter );
                        bPut = PutXMLValue( *pNewItem, rValue,
                                            static_cast<sal_uInt16>( pEntry->nMemberId & MID_SW_FLAG_MASK ),
                                            rUnitConverter );

                    }
                    else
                    {
                        bPut = handleSpecialItem( *pEntry, *pNewItem, rSet,
                                                  rValue, rUnitConverter,
                                                  rNamespaceMap );
                    }

                    if( bPut )
                        rSet.Put( *pNewItem );

                    delete pNewItem;
                }
                else
                {
                    DBG_ERROR( "Could not get a needed item for xml import!" );
                }
            }
            else if( 0 != (pEntry->nMemberId & MID_SW_FLAG_NO_ITEM_IMPORT) )
            {
                handleNoItem( *pEntry, rSet, rValue, rUnitConverter,
                              rNamespaceMap );
            }
        }
        else if( USHRT_MAX != nUnknownWhich )
        {
            if( !pUnknownItem )
            {
                const SfxPoolItem* pItem = 0;
                if( SFX_ITEM_SET == rSet.GetItemState( nUnknownWhich, TRUE,
                                                       &pItem ) )
                {
                    SfxPoolItem *pNew = pItem->Clone();
                    pUnknownItem = PTR_CAST( SvXMLAttrContainerItem, pNew );
                    DBG_ASSERT( pUnknownItem,
                                "SvXMLAttrContainerItem expected" );
                    if( !pUnknownItem )
                        delete pNew;
                }
                else
                {
                    pUnknownItem = new SvXMLAttrContainerItem( nUnknownWhich );
                }
            }
            if( pUnknownItem )
            {
                if( XML_NAMESPACE_NONE == nPrefix )
                    pUnknownItem->AddAttr( aLocalName, rValue );
                else
                    pUnknownItem->AddAttr( aPrefix, aNamespace, aLocalName,
                                           rValue );
            }
        }
    }

    if( pUnknownItem )
    {
        rSet.Put( *pUnknownItem );
        delete pUnknownItem;
    }

    finished( rSet );
}

/** this method is called for every item that has the
    MID_SW_FLAG_SPECIAL_ITEM_IMPORT flag set */
BOOL
SvXMLImportItemMapper::handleSpecialItem(  const SvXMLItemMapEntry& /*rEntry*/,
                                            SfxPoolItem& /*rItem*/,
                                            SfxItemSet& /*rSet*/,
                                            const OUString& /*rValue*/,
                                            const SvXMLUnitConverter& /*rUnitConverter*/,
                                            const SvXMLNamespaceMap& /*rNamespaceMap*/ ) const
{
    DBG_ERROR( "unsuported special item in xml import" );
    return FALSE;
}

/** this method is called for every item that has the
    MID_SW_FLAG_NO_ITEM_IMPORT flag set */
BOOL SvXMLImportItemMapper::handleNoItem( const SvXMLItemMapEntry& /*rEntry*/,
                                           SfxItemSet& /*rSet*/,
                                           const OUString& /*rValue*/,
                                           const SvXMLUnitConverter& /*rUnitConverter*/,
                                           const SvXMLNamespaceMap& /*rNamespaceMap*/ ) const
{
    DBG_ERROR( "unsuported no item in xml import" );
    return FALSE;
}

void SvXMLImportItemMapper::finished( SfxItemSet& ) const
{
    // nothing to do here
}



// put an XML-string value into an item
sal_Bool SvXMLImportItemMapper::PutXMLValue(
    SfxPoolItem& rItem,
    const ::rtl::OUString& rValue,
    sal_uInt16 nMemberId,
    const SvXMLUnitConverter& rUnitConverter )
{
    sal_Bool bOk = sal_False;

    switch (rItem.Which())
        {
        case RES_LR_SPACE:
        {
            SvxLRSpaceItem* pLRSpace = PTR_CAST(SvxLRSpaceItem, &rItem);
            DBG_ASSERT( pLRSpace != NULL, "Wrong Which-ID!" );

            switch( nMemberId )
            {
                case MID_L_MARGIN:
                case MID_R_MARGIN:
                {
                    sal_Int32 nProp = 100;
                    sal_Int32 nAbs = 0;

                    if( rValue.indexOf( sal_Unicode('%') ) != -1 )
                        bOk = rUnitConverter.convertPercent( nProp, rValue );
                    else
                        bOk = rUnitConverter.convertMeasure( nAbs, rValue );

                    if( bOk )
                    {
                        switch( nMemberId )
                        {
                            case MID_L_MARGIN:
                                pLRSpace->SetTxtLeft( (sal_Int32)nAbs, (sal_uInt16)nProp );
                                break;
                            case MID_R_MARGIN:
                                pLRSpace->SetRight( (sal_Int32)nAbs, (sal_uInt16)nProp );
                                break;
                        }
                    }
                }
                break;
                case MID_FIRST_LINE_INDENT:
                {
                    sal_Int32 nProp = 100;
                    sal_Int32 nAbs = 0;

                    if( rValue.indexOf( sal_Unicode('%') ) != -1 )
                        bOk = rUnitConverter.convertPercent( nProp, rValue );
                    else
                        bOk = rUnitConverter.convertMeasure( nAbs, rValue,
                                                             -0x7fff, 0x7fff );

                    pLRSpace->SetTxtFirstLineOfst( (short)nAbs, (sal_uInt16)nProp );
                }

                case MID_FIRST_AUTO:
                {
                    sal_Bool bAutoFirst;
                    bOk = rUnitConverter.convertBool( bAutoFirst, rValue );
                    if( bOk )
                        pLRSpace->SetAutoFirst( bAutoFirst );
                }
                break;

                default:
                    DBG_ERROR( "unknown member id!");
            }
        }
        break;

        case RES_UL_SPACE:
        {
            SvxULSpaceItem* pULSpace = PTR_CAST(SvxULSpaceItem, &rItem);
            DBG_ASSERT( pULSpace != NULL, "Wrong Which-ID!" );

            sal_Int32 nProp = 100;
            sal_Int32 nAbs = 0;

            if( rValue.indexOf( sal_Unicode('%') ) != -1 )
                bOk = rUnitConverter.convertPercent( nProp, rValue );
            else
                bOk = rUnitConverter.convertMeasure( nAbs, rValue );

            switch( nMemberId )
            {
                case MID_UP_MARGIN:
                    pULSpace->SetUpper( (sal_uInt16)nAbs, (sal_uInt16)nProp );
                    break;
                case MID_LO_MARGIN:
                    pULSpace->SetLower( (sal_uInt16)nAbs, (sal_uInt16)nProp );
                    break;
                default:
                    DBG_ERROR("unknown MemberId");
            }
        }
        break;

        case RES_SHADOW:
        {
            SvxShadowItem* pShadow = PTR_CAST(SvxShadowItem, &rItem);
            DBG_ASSERT( pShadow != NULL, "Wrong Which-ID" );

            sal_Bool bColorFound = sal_False;
            sal_Bool bOffsetFound = sal_False;

            SvXMLTokenEnumerator aTokenEnum( rValue );

            Color aColor( 128,128, 128 );
            pShadow->SetLocation( SVX_SHADOW_BOTTOMRIGHT );

            OUString aToken;
            while( aTokenEnum.getNextToken( aToken ) )
            {
                if( IsXMLToken( aToken, XML_NONE ) )
                {
                    pShadow->SetLocation( SVX_SHADOW_NONE );
                    bOk = sal_True;
                }
                else if( !bColorFound && aToken.compareToAscii( "#", 1 ) == 0 )
                {
                    bOk = rUnitConverter.convertColor( aColor, aToken );
                    if( !bOk )
                        return sal_False;

                    bColorFound = sal_True;
                }
                else if( !bOffsetFound )
                {
                    sal_Int32 nX = 0, nY = 0;

                    bOk = rUnitConverter.convertMeasure( nX, aToken );
                    if( bOk && aTokenEnum.getNextToken( aToken ) )
                        bOk = rUnitConverter.convertMeasure( nY, aToken );

                    if( bOk )
                    {
                        if( nX < 0 )
                        {
                            if( nY < 0 )
                            {
                                pShadow->SetLocation( SVX_SHADOW_TOPLEFT );
                            }
                            else
                            {
                                pShadow->SetLocation( SVX_SHADOW_BOTTOMLEFT );
                            }
                        }
                        else
                        {
                            if( nY < 0 )
                            {
                                pShadow->SetLocation( SVX_SHADOW_TOPRIGHT );
                            }
                            else
                            {
                                pShadow->SetLocation( SVX_SHADOW_BOTTOMRIGHT );
                            }
                        }

                        if( nX < 0 ) nX *= -1;
                        if( nY < 0 ) nY *= -1;

                        pShadow->SetWidth( static_cast< USHORT >( (nX + nY) >> 1 ) );
                    }
                }
            }

            if( bOk && ( bColorFound || bOffsetFound ) )
            {
                pShadow->SetColor( aColor );
            }
            else
                bOk = sal_False;
        }
        break;

        case RES_BOX:
        {
            SvxBoxItem* pBox = PTR_CAST(SvxBoxItem, &rItem);
            DBG_ASSERT( pBox != NULL, "Wrong WHich-ID" );

            /** copy SvxBorderLines */
            SvxBorderLine* pTop    = pBox->GetTop() == NULL ?
                                NULL : new SvxBorderLine( *pBox->GetTop() );
            SvxBorderLine* pBottom = pBox->GetBottom() == NULL ?
                                NULL : new SvxBorderLine( *pBox->GetBottom() );
            SvxBorderLine* pLeft   = pBox->GetLeft() == NULL ?
                                NULL : new SvxBorderLine( *pBox->GetLeft() );
            SvxBorderLine* pRight  = pBox->GetRight() == NULL ?
                                NULL : new SvxBorderLine( *pBox->GetRight() );

            sal_Int32 nTemp;

            switch( nMemberId )
            {
                case ALL_BORDER_PADDING:
                case LEFT_BORDER_PADDING:
                case RIGHT_BORDER_PADDING:
                case TOP_BORDER_PADDING:
                case BOTTOM_BORDER_PADDING:
                    if(!rUnitConverter.convertMeasure( nTemp, rValue, 0, 0xffff ))
                        return sal_False;

                    if( nMemberId == LEFT_BORDER_PADDING ||
                        nMemberId == ALL_BORDER_PADDING )
                        pBox->SetDistance( (sal_uInt16)nTemp, BOX_LINE_LEFT );
                    if( nMemberId == RIGHT_BORDER_PADDING ||
                        nMemberId == ALL_BORDER_PADDING )
                        pBox->SetDistance( (sal_uInt16)nTemp, BOX_LINE_RIGHT );
                    if( nMemberId == TOP_BORDER_PADDING ||
                        nMemberId == ALL_BORDER_PADDING )
                        pBox->SetDistance( (sal_uInt16)nTemp, BOX_LINE_TOP );
                    if( nMemberId == BOTTOM_BORDER_PADDING ||
                        nMemberId == ALL_BORDER_PADDING )
                        pBox->SetDistance( (sal_uInt16)nTemp, BOX_LINE_BOTTOM);
                    break;

                case ALL_BORDER:
                case LEFT_BORDER:
                case RIGHT_BORDER:
                case TOP_BORDER:
                case BOTTOM_BORDER:
                {
                    sal_Bool bHasStyle = sal_False;
                    sal_Bool bHasWidth = sal_False;
                    sal_Bool bHasColor = sal_False;

                    sal_uInt16 nStyle = USHRT_MAX;
                    sal_uInt16 nWidth = 0;
                    sal_uInt16 nNamedWidth = USHRT_MAX;

                    Color aColor( COL_BLACK );

                    if( !lcl_frmitems_parseXMLBorder( rValue, rUnitConverter,
                                                      bHasStyle, nStyle,
                                                      bHasWidth, nWidth, nNamedWidth,
                                                      bHasColor, aColor ) )
                        return sal_False;

                    if( TOP_BORDER == nMemberId || ALL_BORDER == nMemberId )
                        lcl_frmitems_setXMLBorder( pTop,
                                                   bHasStyle, nStyle,
                                                   bHasWidth, nWidth, nNamedWidth,
                                                   bHasColor, aColor );

                    if( BOTTOM_BORDER == nMemberId || ALL_BORDER == nMemberId )
                        lcl_frmitems_setXMLBorder( pBottom,
                                                   bHasStyle, nStyle,
                                                   bHasWidth, nWidth, nNamedWidth,
                                                   bHasColor, aColor );

                    if( LEFT_BORDER == nMemberId || ALL_BORDER == nMemberId )
                        lcl_frmitems_setXMLBorder( pLeft,
                                                   bHasStyle, nStyle,
                                                   bHasWidth, nWidth, nNamedWidth,
                                                   bHasColor, aColor );

                    if( RIGHT_BORDER == nMemberId || ALL_BORDER == nMemberId )
                        lcl_frmitems_setXMLBorder( pRight,
                                                   bHasStyle, nStyle,
                                                   bHasWidth, nWidth, nNamedWidth,
                                                   bHasColor, aColor );
                }
                break;
                case ALL_BORDER_LINE_WIDTH:
                case LEFT_BORDER_LINE_WIDTH:
                case RIGHT_BORDER_LINE_WIDTH:
                case TOP_BORDER_LINE_WIDTH:
                case BOTTOM_BORDER_LINE_WIDTH:
                {
                    SvXMLTokenEnumerator aTokenEnum( rValue );

                    sal_Int32 nInWidth, nDistance, nOutWidth;

                    OUString aToken;
                    if( !aTokenEnum.getNextToken( aToken ) )
                        return sal_False;

                    if( !rUnitConverter.convertMeasure( nInWidth, aToken,
                                                        DEF_LINE_WIDTH_0,
                                                        DEF_LINE_WIDTH_4 ) )
                        return sal_False;

                    if( !aTokenEnum.getNextToken( aToken ) )
                        return sal_False;

                    if( !rUnitConverter.convertMeasure( nDistance, aToken,
                                                        DEF_LINE_WIDTH_0,
                                                        DEF_LINE_WIDTH_4 ) )
                        return sal_False;

                    if( !aTokenEnum.getNextToken( aToken ) )
                        return sal_False;

                    if( !rUnitConverter.convertMeasure( nOutWidth, aToken,
                                                        DEF_LINE_WIDTH_0,
                                                        DEF_LINE_WIDTH_4 ) )
                        return sal_False;
                        
                    // #i61946: accept line style even it's not part of our "normal" set of line styles
                    sal_uInt16 nWidth = 0;

                    if( TOP_BORDER_LINE_WIDTH == nMemberId ||
                        ALL_BORDER_LINE_WIDTH == nMemberId )
                        lcl_frmitems_setXMLBorder( pTop, nWidth,
                                static_cast< sal_uInt16 >( nOutWidth ),
                                static_cast< sal_uInt16 >( nInWidth ),
                                static_cast< sal_uInt16 >( nDistance ) );

                    if( BOTTOM_BORDER_LINE_WIDTH == nMemberId ||
                        ALL_BORDER_LINE_WIDTH == nMemberId )
                        lcl_frmitems_setXMLBorder( pBottom, nWidth,
                                static_cast< sal_uInt16 >( nOutWidth ),
                                static_cast< sal_uInt16 >( nInWidth ),
                                static_cast< sal_uInt16 >( nDistance ) );

                    if( LEFT_BORDER_LINE_WIDTH == nMemberId ||
                        ALL_BORDER_LINE_WIDTH == nMemberId )
                        lcl_frmitems_setXMLBorder( pLeft, nWidth,
                                static_cast< sal_uInt16 >( nOutWidth ),
                                static_cast< sal_uInt16 >( nInWidth ),
                                static_cast< sal_uInt16 >( nDistance ) );

                    if( RIGHT_BORDER_LINE_WIDTH == nMemberId ||
                        ALL_BORDER_LINE_WIDTH == nMemberId )
                        lcl_frmitems_setXMLBorder( pRight, nWidth,
                                static_cast< sal_uInt16 >( nOutWidth ),
                                static_cast< sal_uInt16 >( nInWidth ),
                                static_cast< sal_uInt16 >( nDistance ) );
                }
                break;
            }

            pBox->SetLine( pTop,    BOX_LINE_TOP    );
            pBox->SetLine( pBottom, BOX_LINE_BOTTOM );
            pBox->SetLine( pLeft,   BOX_LINE_LEFT   );
            pBox->SetLine( pRight,  BOX_LINE_RIGHT  );

            delete pTop;
            delete pBottom;
            delete pLeft;
            delete pRight;

            bOk = sal_True;
        }
        break;

        case RES_BREAK:
        {
            SvxFmtBreakItem* pFmtBreak = PTR_CAST(SvxFmtBreakItem, &rItem);
            DBG_ASSERT( pFmtBreak != NULL, "Wrong Which-ID" );

            sal_uInt16 eEnum;

            if( !rUnitConverter.convertEnum( eEnum, rValue, psXML_BreakType ) )
                return sal_False;

            if( eEnum == 0 )
            {
                pFmtBreak->SetValue( SVX_BREAK_NONE );
                bOk = sal_True;
            }
            else
            {
                switch( nMemberId )
                {
                    case MID_BREAK_BEFORE:
                        pFmtBreak->SetValue( static_cast< USHORT >((eEnum == 1) ?
                                             SVX_BREAK_COLUMN_BEFORE :
                                             SVX_BREAK_PAGE_BEFORE) );
                        break;
                    case MID_BREAK_AFTER:
                        pFmtBreak->SetValue( static_cast< USHORT >((eEnum == 1) ?
                                             SVX_BREAK_COLUMN_AFTER :
                                             SVX_BREAK_PAGE_AFTER) );
                        break;
                }
                bOk = sal_True;
            }
        }
        break;

        case RES_KEEP:
        {
            SvxFmtKeepItem* pFmtKeep = PTR_CAST(SvxFmtKeepItem, &rItem);
            DBG_ASSERT( pFmtKeep != NULL, "Wrong Which-ID" );

            if( IsXMLToken( rValue, XML_ALWAYS ) ||
                 IsXMLToken( rValue, XML_TRUE ) )
            {
                pFmtKeep->SetValue( sal_True );
                bOk = sal_True;
            }
            else if( IsXMLToken( rValue, XML_AUTO ) ||
                     IsXMLToken( rValue, XML_FALSE ) )
            {
                pFmtKeep->SetValue( sal_False );
                bOk = sal_True;
            }
        }
        break;

        case RES_BACKGROUND:
        {
            SvxBrushItem* pBrush = PTR_CAST(SvxBrushItem, &rItem);
            DBG_ASSERT( pBrush != NULL, "Wrong Which-ID" );

            Color aTempColor;
            switch( nMemberId )
                {
                case MID_BACK_COLOR:
                    if(	IsXMLToken( rValue, XML_TRANSPARENT ) )
                    {
                        pBrush->GetColor().SetTransparency(0xff);
                        bOk = sal_True;
                    }
                    else if( rUnitConverter.convertColor( aTempColor, rValue ) )
                    {
                        aTempColor.SetTransparency(0);
                        pBrush->SetColor( aTempColor );
                        bOk = sal_True;
                    }
                    break;

                case MID_GRAPHIC_LINK:
                {
                    SvxGraphicPosition eOldGraphicPos = pBrush->GetGraphicPos();
                    uno::Any aAny;
                    aAny <<= rValue;
                    pBrush->PutValue( aAny, MID_GRAPHIC_URL );
                    if( GPOS_NONE == eOldGraphicPos &&
                        GPOS_NONE != pBrush->GetGraphicPos() )
                        pBrush->SetGraphicPos( GPOS_TILED );
                    bOk = sal_True	;
                }
                break;

                case MID_GRAPHIC_REPEAT:
                {
                    SvxGraphicPosition eGraphicPos = pBrush->GetGraphicPos();
                    sal_uInt16 nPos = GPOS_NONE;
                    if( rUnitConverter.convertEnum( nPos, rValue,
                                                    psXML_BrushRepeat ) )
                    {
                        if( GPOS_MM != nPos || GPOS_NONE == eGraphicPos ||
                            GPOS_AREA == eGraphicPos || GPOS_TILED == eGraphicPos )
                            pBrush->SetGraphicPos( (SvxGraphicPosition)nPos );
                        bOk = sal_True;
                    }
                }
                break;

                case MID_GRAPHIC_POSITION:
                {
                    SvxGraphicPosition ePos = GPOS_NONE, eTmp;
                    sal_uInt16 nTmp;
                    SvXMLTokenEnumerator aTokenEnum( rValue );
                    OUString aToken;
                    sal_Bool bHori = sal_False, bVert = sal_False;
                    bOk = sal_True;
                    while( bOk && aTokenEnum.getNextToken( aToken ) )
                    {
                        if( bHori && bVert )
                        {
                            bOk = sal_False;
                        }
                        else if( -1 != aToken.indexOf( sal_Unicode('%') ) )
                        {
                            sal_Int32 nPrc = 50;
                            if( rUnitConverter.convertPercent( nPrc, aToken ) )
                            {
                                if( !bHori )
                                {
                                    ePos = nPrc < 25 ? GPOS_LT :
                                               (nPrc < 75 ? GPOS_MM : GPOS_RB);
                                    bHori = sal_True;
                                }
                                else
                                {
                                    eTmp = nPrc < 25 ? GPOS_LT:
                                               (nPrc < 75 ? GPOS_LM : GPOS_LB);
                                    lcl_frmitems_MergeXMLVertPos( ePos, eTmp );
                                    bVert = sal_True;
                                }
                            }
                            else
                            {
                                // wrong percentage
                                bOk = sal_False;
                            }
                        }
                        else if( IsXMLToken( aToken, XML_CENTER ) )
                        {
                            if( bHori )
                                lcl_frmitems_MergeXMLVertPos( ePos, GPOS_MM );
                            else if ( bVert )
                                lcl_frmitems_MergeXMLHoriPos( ePos, GPOS_MM );
                            else
                                ePos = GPOS_MM;
                        }
                        else if( rUnitConverter.convertEnum( nTmp, aToken,
                                                         psXML_BrushHoriPos ) )
                        {
                            if( bVert )
                                lcl_frmitems_MergeXMLHoriPos(
                                    ePos, (SvxGraphicPosition)nTmp );
                            else if( !bHori )
                                ePos = (SvxGraphicPosition)nTmp;
                            else
                                bOk = sal_False;
                            bHori = sal_True;
                        }
                        else if( rUnitConverter.convertEnum( nTmp, aToken,
                                                         psXML_BrushVertPos ) )
                        {
                            if( bHori )
                                lcl_frmitems_MergeXMLVertPos(
                                    ePos, (SvxGraphicPosition)nTmp );
                            else if( !bVert )
                                ePos = (SvxGraphicPosition)nTmp;
                            else
                                bOk = sal_False;
                            bVert = sal_True;
                        }
                        else
                        {
                            bOk = sal_False;
                        }
                    }

                    bOk &= GPOS_NONE != ePos;
                    if( bOk )
                        pBrush->SetGraphicPos( ePos );
                }
                break;

                case MID_GRAPHIC_FILTER:
                    pBrush->SetGraphicFilter( rValue.getStr() );
                    bOk = sal_True;
                    break;
                }
        }
        break;

        case RES_PAGEDESC:
        {
            SwFmtPageDesc* pPageDesc = PTR_CAST(SwFmtPageDesc, &rItem);
            DBG_ASSERT( pPageDesc != NULL, "Wrong Which-ID" );

            if( MID_PAGEDESC_PAGENUMOFFSET==nMemberId )
            {
                sal_Int32 nVal;
                bOk = rUnitConverter.convertNumber( nVal, rValue, 0, USHRT_MAX );
                if( bOk )
                    pPageDesc->SetNumOffset( (USHORT)nVal );
            }
        }
        break;

        case RES_LAYOUT_SPLIT:
        case RES_ROW_SPLIT:
        {
            SfxBoolItem* pSplit = PTR_CAST(SfxBoolItem, &rItem);
            DBG_ASSERT( pSplit != NULL, "Wrong Which-ID" );
            
            if( IsXMLToken( rValue, XML_AUTO ) ||
                 IsXMLToken( rValue, XML_TRUE ) )
            {
                pSplit->SetValue( sal_True );
                bOk = sal_True;
            }
            else if( IsXMLToken( rValue, XML_ALWAYS ) ||
                     IsXMLToken( rValue, XML_FALSE ) )
            {
                pSplit->SetValue( sal_False );
                bOk = sal_True;
            }
        }
        break;

        case RES_HORI_ORIENT:
        {
            SwFmtHoriOrient* pHoriOrient = PTR_CAST(SwFmtHoriOrient, &rItem);
            DBG_ASSERT( pHoriOrient != NULL, "Wrong Which-ID" );

            sal_uInt16 nValue;
            bOk = rUnitConverter.convertEnum( nValue, rValue,
                                              aXMLTableAlignMap );
            if( bOk )
                pHoriOrient->SetHoriOrient( nValue );
        }
        break;

        case RES_VERT_ORIENT:
        {
            SwFmtVertOrient* pVertOrient = PTR_CAST(SwFmtVertOrient, &rItem);
            DBG_ASSERT( pVertOrient != NULL, "Wrong Which-ID" );

            sal_uInt16 nValue;
            bOk = rUnitConverter.convertEnum( nValue, rValue,
                                              aXMLTableVAlignMap );
            if( bOk )
                pVertOrient->SetVertOrient( nValue );
            //#i8855# text::VertOrientation::NONE is stored as empty string and should be applied here
            else if(!rValue.getLength())
            {
                pVertOrient->SetVertOrient( text::VertOrientation::NONE );
                bOk = sal_True;
            }
        }
        break;

        case RES_FRM_SIZE:
        {
            SwFmtFrmSize* pFrmSize = PTR_CAST(SwFmtFrmSize, &rItem);
            DBG_ASSERT( pFrmSize != NULL, "Wrong Which-ID" );

            sal_Bool bSetHeight = sal_False;
            sal_Bool bSetWidth = sal_False;
            sal_Bool bSetSizeType = sal_False;
            SwFrmSize eSizeType = ATT_VAR_SIZE;
            sal_Int32 nMin = MINLAY;

            switch( nMemberId )
                {
                case MID_FRMSIZE_REL_WIDTH:
                {
                    sal_Int32 nValue;
                    bOk = rUnitConverter.convertPercent( nValue, rValue );
                    if( bOk )
                    {
                        if( nValue < 1 )
                            nValue = 1;
                        else if( nValue > 100 )
                            nValue = 100;

                        pFrmSize->SetWidthPercent( (sal_Int8)nValue );
                    }
                }
                break;
                case MID_FRMSIZE_WIDTH:
                    bSetWidth = sal_True;
                    break;
                case MID_FRMSIZE_MIN_HEIGHT:
                    eSizeType = ATT_MIN_SIZE;
                    bSetHeight = sal_True;
                    nMin = 1;
                    bSetSizeType = sal_True;
                    break;
                case MID_FRMSIZE_FIX_HEIGHT:
                    eSizeType = ATT_FIX_SIZE;
                    bSetHeight = sal_True;
                    nMin = 1;
                    bSetSizeType = sal_True;
                    break;
                case MID_FRMSIZE_COL_WIDTH:
                    eSizeType = ATT_FIX_SIZE;
                    bSetWidth = sal_True;
                    bSetSizeType = sal_True;
                    break;
                case MID_FRMSIZE_REL_COL_WIDTH:
                {
                    sal_Int32 nPos = rValue.indexOf( (sal_Unicode)'*' );
                    if( -1L != nPos )
                    {
                        OUString sNum( rValue.copy( 0L, nPos ) );
                        sal_Int32 nValue = rValue.toInt32();
                        if( nValue < MINLAY )
                            nValue = MINLAY;
                        else if( nValue > USHRT_MAX )
                            nValue = USHRT_MAX;

                        pFrmSize->SetWidth( (sal_uInt16)nValue );
                        pFrmSize->SetHeightSizeType( ATT_VAR_SIZE );
                        bOk = sal_True;
                    }
                }
                break;
                }

            sal_Int32 nValue;
            if( bSetHeight || bSetWidth )
            {
                bOk = rUnitConverter.convertMeasure( nValue, rValue, nMin,
                                                     USHRT_MAX );
                if( bOk )
                {
                    if( bSetWidth )
                        pFrmSize->SetWidth( (sal_uInt16)nValue );
                    if( bSetHeight )
                        pFrmSize->SetHeight( (sal_uInt16)nValue );
                    if( bSetSizeType )
                        pFrmSize->SetHeightSizeType( eSizeType );
                }
            }
        }
        break;

        case RES_FRAMEDIR:
        {
            const XMLPropertyHandler* pWritingModeHandler =
                XMLPropertyHandlerFactory::CreatePropertyHandler(
                    XML_TYPE_TEXT_WRITING_MODE_WITH_DEFAULT );
            if( pWritingModeHandler != NULL )
            {
                Any aAny;
                bOk = pWritingModeHandler->importXML( rValue, aAny,
                                                      rUnitConverter );
                if( bOk )
                    bOk = rItem.PutValue( aAny );
            }
        }
        break;

        case RES_COLLAPSING_BORDERS:
        {
            SfxBoolItem* pBorders = PTR_CAST(SfxBoolItem, &rItem);
            DBG_ASSERT( pBorders != NULL, "Wrong Which-ID" );

            if( IsXMLToken( rValue, XML_COLLAPSING ) )
            {
                pBorders->SetValue( sal_True );
                bOk = sal_True;
            }
            else if( IsXMLToken( rValue, XML_SEPARATING ) )
            {
                pBorders->SetValue( sal_False );
                bOk = sal_True;
            }
            else
                bOk = sal_False;
        }
        break;

        default:
            DBG_ERROR("Item not implemented!");
        break;
   }

    return bOk;
}


