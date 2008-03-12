/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmltble.cxx,v $
 *
 *  $Revision: 1.46 $
 *
 *  last change: $Author: rt $ $Date: 2008-03-12 12:42:34 $
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


#include <hintids.hxx>

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_TEXT_XTEXTTABLE_HPP_
#include <com/sun/star/text/XTextTable.hpp>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif

#ifndef XMLOFF_NUMEHELP_HXX
#include <xmloff/numehelp.hxx>
#endif

#ifndef _CNTRSRT_HXX
#include <svtools/cntnrsrt.hxx>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif

#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX
#include <svx/boxitem.hxx>
#endif
#ifndef _FMTROWSPLT_HXX //autogen
#include <fmtrowsplt.hxx>
#endif
#ifndef _SVX_FRAMEDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif

#include <list>

#ifndef _SWTABLE_HXX
#include "swtable.hxx"
#endif
#ifndef _DOC_HXX
#include "doc.hxx"
#endif
#ifndef _PAM_HXX
#include "pam.hxx"
#endif
#ifndef _FRMFMT_HXX
#include "frmfmt.hxx"
#endif
#ifndef _WRTSWTBL_HXX
#include "wrtswtbl.hxx"
#endif
#ifndef _FMTFSIZE_HXX
#include "fmtfsize.hxx"
#endif
#ifndef _FMTORNT_HXX
#include "fmtornt.hxx"
#endif
#ifndef _CELLATR_HXX
#include "cellatr.hxx"
#endif
#ifndef _DDEFLD_HXX
#include "ddefld.hxx"
#endif
#ifndef _SWDDETBL_HXX
#include "swddetbl.hxx"
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif

#ifndef _LINKMGR_HXX
#include <sfx2/linkmgr.hxx>  // for cTokenSeperator
#endif

#ifndef _UNOOBJ_HXX
#include "unoobj.hxx"
#endif
#ifndef _UNOTBL_HXX
#include "unotbl.hxx"
#endif

#ifndef _XMLTEXTE_HXX
#include "xmltexte.hxx"
#endif
#ifndef _XMLEXP_HXX
#include "xmlexp.hxx"
#endif


using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using namespace ::xmloff::token;
using table::XCell;


class SwXMLTableColumn_Impl : public SwWriteTableCol
{
    OUString	sStyleName;
    sal_uInt32	nRelWidth;

public:


    SwXMLTableColumn_Impl( sal_uInt16 nPosition ) :
        SwWriteTableCol( nPosition ),
        nRelWidth( 0UL )
    {};

    void SetStyleName( const OUString& rName ) { sStyleName = rName; }
    const OUString& GetStyleName() const { return sStyleName; }

    void SetRelWidth( sal_uInt32 nSet ) { nRelWidth = nSet; }
    sal_uInt32 GetRelWidth() const { return nRelWidth; }
};

sal_Int32 SwXMLTableColumnCmpWidth_Impl( const SwXMLTableColumn_Impl& r1,
                                           const SwXMLTableColumn_Impl& r2 )
{
    sal_Int32 n = (sal_Int32)r1.GetWidthOpt() - (sal_Int32)r2.GetWidthOpt();
    if( !n )
        n = (sal_Int32)r1.GetRelWidth() - (sal_Int32)r2.GetRelWidth();
    return n;
}

// ---------------------------------------------------------------------

typedef SwXMLTableColumn_Impl *SwXMLTableColumnPtr;
SV_DECL_PTRARR_SORT_DEL( SwXMLTableColumns_Impl, SwXMLTableColumnPtr, 5, 5 )
SV_IMPL_OP_PTRARR_SORT( SwXMLTableColumns_Impl, SwXMLTableColumnPtr )

DECLARE_CONTAINER_SORT( SwXMLTableColumnsSortByWidth_Impl,
                        SwXMLTableColumn_Impl )
IMPL_CONTAINER_SORT( SwXMLTableColumnsSortByWidth_Impl, SwXMLTableColumn_Impl,
                     SwXMLTableColumnCmpWidth_Impl )

class SwXMLTableLines_Impl
{
    SwXMLTableColumns_Impl	aCols;
    const SwTableLines  	*pLines;
    sal_uInt32				nWidth;

public:

    SwXMLTableLines_Impl( const SwTableLines& rLines );

    ~SwXMLTableLines_Impl() {}

    sal_uInt32 GetWidth() const { return nWidth; }
    const SwTableLines *GetLines() const { return pLines; }

    const SwXMLTableColumns_Impl& GetColumns() const { return aCols; }
};

SwXMLTableLines_Impl::SwXMLTableLines_Impl( const SwTableLines& rLines ) :
    pLines( &rLines ),
    nWidth( 0UL )
{
#ifndef PRODUCT
    sal_uInt16 nEndCPos = 0U;
#endif
    sal_uInt16 nLines = rLines.Count();
    sal_uInt16 nLine;
    for( nLine=0U; nLine<nLines; nLine++ )
    {
        const SwTableLine *pLine = rLines[nLine];
        const SwTableBoxes& rBoxes = pLine->GetTabBoxes();
        sal_uInt16 nBoxes = rBoxes.Count();

        sal_uInt16 nCPos = 0U;
        for( sal_uInt16 nBox=0U; nBox<nBoxes; nBox++ )
        {
            const SwTableBox *pBox = rBoxes[nBox];

            if( nBox < nBoxes-1U || nWidth==0UL )
            {
                nCPos = nCPos + (sal_uInt16)SwWriteTable::GetBoxWidth( pBox );
                SwXMLTableColumn_Impl *pCol =
                    new SwXMLTableColumn_Impl( nCPos );

                if( !aCols.Insert( pCol ) )
                    delete pCol;

                if( nBox==nBoxes-1U )
                {
                    ASSERT( nLine==0U && nWidth==0UL,
                            "parent width will be lost" );
                    nWidth = nCPos;
                }
            }
            else
            {
#ifndef PRODUCT
                sal_uInt16 nCheckPos =
                    nCPos + (sal_uInt16)SwWriteTable::GetBoxWidth( pBox );
                if( !nEndCPos )
                {
                    nEndCPos = nCheckPos;
                }
                else
                {
                    /*
                    ASSERT( SwXMLTableColumn_impl(nCheckPos) ==
                                        SwXMLTableColumn_Impl(nEndCPos),
                    "rows have different total widths" );
                    */
                }
#endif
                nCPos = (sal_uInt16)nWidth;
#ifndef PRODUCT
                SwXMLTableColumn_Impl aCol( (sal_uInt16)nWidth );
                ASSERT( aCols.Seek_Entry(&aCol), "couldn't find last column" );
                ASSERT( SwXMLTableColumn_Impl(nCheckPos) ==
                                            SwXMLTableColumn_Impl(nCPos),
                        "rows have different total widths" );
#endif
            }
        }
    }
}

typedef SwXMLTableLines_Impl *SwXMLTableLinesPtr;
DECLARE_LIST( SwXMLTableLinesCache_Impl, SwXMLTableLinesPtr )

// ---------------------------------------------------------------------

typedef SwFrmFmt *SwFrmFmtPtr;
DECLARE_LIST( SwXMLFrmFmts_Impl, SwFrmFmtPtr )

class SwXMLTableFrmFmtsSort_Impl : public SwXMLFrmFmts_Impl
{
public:
    SwXMLTableFrmFmtsSort_Impl ( sal_uInt16 nInit, sal_uInt16 nGrow ) :
        SwXMLFrmFmts_Impl( nInit, nGrow )
    {}

    sal_Bool AddRow( SwFrmFmt& rFrmFmt, const OUString& rNamePrefix, sal_uInt32 nLine );
    sal_Bool AddCell( SwFrmFmt& rFrmFmt, const OUString& rNamePrefix,
                  sal_uInt32 nCol, sal_uInt32 nRow, sal_Bool bTop );
};

sal_Bool SwXMLTableFrmFmtsSort_Impl::AddRow( SwFrmFmt& rFrmFmt,
                                         const OUString& rNamePrefix,
                                            sal_uInt32 nLine )
{
    const SwFmtFrmSize *pFrmSize = 0;
    const SwFmtRowSplit* pRowSplit = 0;
    const SvxBrushItem *pBrush = 0;

    const SfxItemSet& rItemSet = rFrmFmt.GetAttrSet();
    const SfxPoolItem *pItem;
    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_FRM_SIZE, sal_False, &pItem ) )
        pFrmSize = (const SwFmtFrmSize *)pItem;

    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_ROW_SPLIT, sal_False, &pItem ) )
        pRowSplit = (const SwFmtRowSplit *)pItem;

    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_BACKGROUND, sal_False, &pItem ) )
        pBrush = (const SvxBrushItem *)pItem;

    // empty styles have not to be exported
    if( !pFrmSize && !pBrush && !pRowSplit )
        return sal_False;

    // order is: -/brush, size/-, size/brush
    sal_uInt32 nCount2 = Count();
    sal_Bool bInsert = sal_True;
    sal_uInt32 i;
    for( i = 0; i < nCount2; ++i )
    {
        const SwFmtFrmSize *pTestFrmSize = 0;
        const SwFmtRowSplit* pTestRowSplit = 0;
        const SvxBrushItem *pTestBrush = 0;
        const SwFrmFmt *pTestFmt = GetObject(i);
        const SfxItemSet& rTestSet = pTestFmt->GetAttrSet();
        if( SFX_ITEM_SET == rTestSet.GetItemState( RES_FRM_SIZE, sal_False,
                                                  &pItem ) )
        {
            if( !pFrmSize )
                break;

            pTestFrmSize = (const SwFmtFrmSize *)pItem;
        }
        else
        {
            if( pFrmSize )
                continue;
        }

        if( SFX_ITEM_SET == rTestSet.GetItemState( RES_BACKGROUND, sal_False,
                                                  &pItem ) )
        {
            if( !pBrush )
                break;

            pTestBrush = (const SvxBrushItem *)pItem;
        }
        else
        {
            if( pBrush )
                continue;
        }

        if( SFX_ITEM_SET == rTestSet.GetItemState( RES_ROW_SPLIT, sal_False,
                                                  &pItem ) )
        {
            if( !pRowSplit )
                break;

            pTestRowSplit = (const SwFmtRowSplit *)pItem;
        }
        else
        {
            if( pRowSplit )
                continue;
        }


        if( pFrmSize &&
            ( pFrmSize->GetHeightSizeType() != pTestFrmSize->GetHeightSizeType() ||
              pFrmSize->GetHeight() != pTestFrmSize->GetHeight() ) )
            continue;

        if( pBrush && (*pBrush != *pTestBrush) )
            continue;

        if( pRowSplit && (!pRowSplit->GetValue() != !pTestRowSplit->GetValue()) )
            continue;

        // found!
        const String& rFmtName = pTestFmt->GetName();
        rFrmFmt.SetName( rFmtName );
        bInsert = sal_False;
        break;
    }

    if( bInsert )
    {
        OUStringBuffer sBuffer( rNamePrefix.getLength() + 4UL );
        sBuffer.append( rNamePrefix );
        sBuffer.append( (sal_Unicode)'.' );
        sBuffer.append( (sal_Int32)(nLine+1UL) );

        rFrmFmt.SetName( sBuffer.makeStringAndClear() );
        Insert( &rFrmFmt, i );
    }

    return bInsert;
}

void lcl_GetTblBoxColStr( sal_uInt16 nCol, String& rNm );
void lcl_xmltble_appendBoxPrefix( OUStringBuffer& rBuffer,
                                  const OUString& rNamePrefix,
                                  sal_uInt32 nCol, sal_uInt32 nRow, sal_Bool bTop )
{
    rBuffer.append( rNamePrefix );
    rBuffer.append( (sal_Unicode)'.' );
    if( bTop )
    {
        String sTmp;
        lcl_GetTblBoxColStr( (sal_uInt16)nCol, sTmp );
        rBuffer.append( sTmp );
    }
    else
    {
        rBuffer.append( (sal_Int32)(nCol + 1));
        rBuffer.append( (sal_Unicode)'.' );
    }
    rBuffer.append( (sal_Int32)(nRow + 1));
}

sal_Bool SwXMLTableFrmFmtsSort_Impl::AddCell( SwFrmFmt& rFrmFmt,
                                         const OUString& rNamePrefix,
                                            sal_uInt32 nCol, sal_uInt32 nRow, sal_Bool bTop )
{
    const SwFmtVertOrient *pVertOrient = 0;
    const SvxBrushItem *pBrush = 0;
    const SvxBoxItem *pBox = 0;
    const SwTblBoxNumFormat *pNumFmt = 0;
    const SvxFrameDirectionItem *pFrameDir = 0;

    const SfxItemSet& rItemSet = rFrmFmt.GetAttrSet();
    const SfxPoolItem *pItem;
    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_VERT_ORIENT, sal_False,
                                               &pItem ) )
        pVertOrient = (const SwFmtVertOrient *)pItem;

    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_BACKGROUND, sal_False, &pItem ) )
        pBrush = (const SvxBrushItem *)pItem;

    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_BOX, sal_False, &pItem ) )
        pBox = (const SvxBoxItem *)pItem;

    if ( SFX_ITEM_SET == rItemSet.GetItemState( RES_BOXATR_FORMAT,
                                                sal_False, &pItem ) )
        pNumFmt = (const SwTblBoxNumFormat *)pItem;
    if ( SFX_ITEM_SET == rItemSet.GetItemState( RES_FRAMEDIR,
                                                sal_False, &pItem ) )
        pFrameDir = (const SvxFrameDirectionItem *)pItem;

    // empty styles have not to be exported
    if( !pVertOrient && !pBrush && !pBox && !pNumFmt && !pFrameDir )
        return sal_False;

    // order is: -/-/-/num,
    //           -/-/box/-, --/-/box/num,
    //			 -/brush/-/-, -/brush/-/num, -/brush/box/-, -/brush/box/num,
    // 			 vert/-/-/-, vert/-/-/num, vert/-/box/-, ver/-/box/num,
    //			 vert/brush/-/-, vert/brush/-/num, vert/brush/box/-,
    //			 vert/brush/box/num
    sal_uInt32 nCount2 = Count();
    sal_Bool bInsert = sal_True;
    sal_uInt32 i;
    for( i = 0; i < nCount2; ++i )
    {
        const SwFmtVertOrient *pTestVertOrient = 0;
        const SvxBrushItem *pTestBrush = 0;
        const SvxBoxItem *pTestBox = 0;
        const SwTblBoxNumFormat *pTestNumFmt = 0;
        const SvxFrameDirectionItem *pTestFrameDir = 0;
        const SwFrmFmt *pTestFmt = GetObject(i);
        const SfxItemSet& rTestSet = pTestFmt->GetAttrSet();
        if( SFX_ITEM_SET == rTestSet.GetItemState( RES_VERT_ORIENT, sal_False,
                                                  &pItem ) )
        {
            if( !pVertOrient )
                break;

            pTestVertOrient = (const SwFmtVertOrient *)pItem;
        }
        else
        {
            if( pVertOrient )
                continue;
        }

        if( SFX_ITEM_SET == rTestSet.GetItemState( RES_BACKGROUND, sal_False,
                                                  &pItem ) )
        {
            if( !pBrush )
                break;

            pTestBrush = (const SvxBrushItem *)pItem;
        }
        else
        {
            if( pBrush )
                continue;
        }

        if( SFX_ITEM_SET == rTestSet.GetItemState( RES_BOX, sal_False, &pItem ) )
        {
            if( !pBox )
                break;

            pTestBox = (const SvxBoxItem *)pItem;
        }
        else
        {
            if( pBox )
                continue;
        }

        if ( SFX_ITEM_SET == rTestSet.GetItemState( RES_BOXATR_FORMAT,
                                                sal_False, &pItem ) )
        {
            if( !pNumFmt )
                break;

            pTestNumFmt = (const SwTblBoxNumFormat *)pItem;
        }
        else
        {
            if( pNumFmt )
                continue;

        }

        if ( SFX_ITEM_SET == rTestSet.GetItemState( RES_FRAMEDIR,
                                                sal_False, &pItem ) )
        {
            if( !pFrameDir )
                break;

            pTestFrameDir = (const SvxFrameDirectionItem *)pItem;
        }
        else
        {
            if( pFrameDir )
                continue;

        }

        if( pVertOrient &&
            pVertOrient->GetVertOrient() != pTestVertOrient->GetVertOrient() )
            continue;

        if( pBrush && ( *pBrush != *pTestBrush ) )
            continue;

        if( pBox && ( *pBox != *pTestBox ) )
            continue;

        if( pNumFmt && pNumFmt->GetValue() != pTestNumFmt->GetValue() )
            continue;

        if( pFrameDir && pFrameDir->GetValue() != pTestFrameDir->GetValue() )
            continue;

        // found!
        const String& rFmtName = pTestFmt->GetName();
        rFrmFmt.SetName( rFmtName );
        bInsert = sal_False;
        break;
    }

    if( bInsert )
    {
        OUStringBuffer sBuffer( rNamePrefix.getLength() + 8UL );
        lcl_xmltble_appendBoxPrefix( sBuffer, rNamePrefix, nCol, nRow, bTop );
        rFrmFmt.SetName( sBuffer.makeStringAndClear() );
        Insert( &rFrmFmt, i );
    }

    return bInsert;
}
// ---------------------------------------------------------------------

class SwXMLTableInfo_Impl
{
    const SwTable *pTable;
    Reference < XTextSection > xBaseSection;
    sal_Bool bBaseSectionValid;

public:

    inline SwXMLTableInfo_Impl( const SwTable *pTbl );

    const SwTable *GetTable() const { return pTable; }
    const SwFrmFmt *GetTblFmt() const { return pTable->GetFrmFmt(); }

    sal_Bool IsBaseSectionValid() const { return bBaseSectionValid; }
    const Reference < XTextSection >& GetBaseSection() const { return xBaseSection; }
    inline void SetBaseSection( const Reference < XTextSection >& rBase );
};

inline SwXMLTableInfo_Impl::SwXMLTableInfo_Impl( const SwTable *pTbl ) :
    pTable( pTbl ),
    bBaseSectionValid( sal_False )
{
}

inline void SwXMLTableInfo_Impl::SetBaseSection(
        const Reference < XTextSection >& rBaseSection )
{
    xBaseSection = rBaseSection;
    bBaseSectionValid = sal_True;
}

// ---------------------------------------------------------------------


void SwXMLExport::ExportTableColumnStyle( const SwXMLTableColumn_Impl& rCol )
{
    // <style:style ...>
    CheckAttrList();

    // style:name="..."
    sal_Bool bEncoded = sal_False;
    AddAttribute( XML_NAMESPACE_STYLE, XML_NAME,
                    EncodeStyleName( rCol.GetStyleName(), &bEncoded ) );
    if( bEncoded )
        AddAttribute( XML_NAMESPACE_STYLE, XML_DISPLAY_NAME, rCol.GetStyleName() );

    // style:family="table-column"
    AddAttribute( XML_NAMESPACE_STYLE, XML_FAMILY, XML_TABLE_COLUMN );

    {
        SvXMLElementExport aElem( *this, XML_NAMESPACE_STYLE, XML_STYLE, sal_True,
                                  sal_True );
        OUStringBuffer sValue;
        if( rCol.GetWidthOpt() )
        {
            GetTwipUnitConverter().convertMeasure( sValue, rCol.GetWidthOpt() );
            AddAttribute( XML_NAMESPACE_STYLE, XML_COLUMN_WIDTH,
                          sValue.makeStringAndClear() );
        }
        if( rCol.GetRelWidth() )
        {
            sValue.append( (sal_Int32)rCol.GetRelWidth() );
            sValue.append( (sal_Unicode)'*' );
            AddAttribute( XML_NAMESPACE_STYLE, XML_REL_COLUMN_WIDTH,
                          sValue.makeStringAndClear() );
        }

        {
            SvXMLElementExport aElemExport( *this, XML_NAMESPACE_STYLE,
                                      XML_TABLE_COLUMN_PROPERTIES,
                                      sal_True, sal_True );
        }
    }
}

void SwXMLExport::ExportTableLinesAutoStyles( const SwTableLines& rLines,
                                    sal_uInt32 nAbsWidth, sal_uInt32 nBaseWidth,
                                    const OUString& rNamePrefix,
                                    SwXMLTableColumnsSortByWidth_Impl& rExpCols,
                                    SwXMLTableFrmFmtsSort_Impl& rExpRows,
                                    SwXMLTableFrmFmtsSort_Impl& rExpCells,
                                    SwXMLTableInfo_Impl& rTblInfo,
                                    sal_Bool bTop )
{
    // pass 1: calculate columns
    SwXMLTableLines_Impl *pLines =
        new SwXMLTableLines_Impl( rLines );
    if( !pTableLines )
        pTableLines = new SwXMLTableLinesCache_Impl( 5, 5 );
    pTableLines->Insert( pLines, pTableLines->Count() );

    OUStringBuffer sBuffer( rNamePrefix.getLength() + 8L );

    // pass 2: export column styles
    {
        const SwXMLTableColumns_Impl& rCols = pLines->GetColumns();
        sal_uInt16 nCPos = 0U;
        sal_uInt16 nColumns = rCols.Count();
        for( sal_uInt16	nColumn=0U; nColumn<nColumns; nColumn++ )
        {
            SwXMLTableColumn_Impl *pColumn = rCols[nColumn];

            sal_uInt16 nOldCPos = nCPos;
            nCPos = pColumn->GetPos();

            sal_uInt32 nWidth = nCPos - nOldCPos;

            // If a base width is given, the table has either an automatic
            // or margin alignment, or an percentage width. In either case,
            // relative widths should be exported.
            if( nBaseWidth )
            {
                pColumn->SetRelWidth( nWidth );
            }

            // If an absolute width is given, the table either has a fixed
            // width, or the current width is known from the layout. In the
            // later case, a base width is set in addition and must be used
            // to "absoultize" the relative column width.
            if( nAbsWidth )
            {
                sal_uInt32 nColAbsWidth = nWidth;
                if( nBaseWidth )
                {
                    nColAbsWidth *= nAbsWidth;
                    nColAbsWidth += (nBaseWidth/2UL);
                    nColAbsWidth /= nBaseWidth;
                }
                pColumn->SetWidthOpt( (sal_uInt16)nColAbsWidth, sal_False );
            }

            ULONG nExpPos = 0;
            if( rExpCols.Seek_Entry( pColumn, &nExpPos ) )
            {
                pColumn->SetStyleName(
                        rExpCols.GetObject(nExpPos)->GetStyleName() );
            }
            else
            {
                sBuffer.append( rNamePrefix );
                sBuffer.append( (sal_Unicode)'.' );
                if( bTop )
                {
                    String sTmp;
                    lcl_GetTblBoxColStr( nColumn, sTmp );
                    sBuffer.append( sTmp );
                }
                else
                {
                    sBuffer.append( (sal_Int32)(nColumn + 1U) );
                }

                pColumn->SetStyleName( sBuffer.makeStringAndClear() );
                ExportTableColumnStyle( *pColumn );
                rExpCols.Insert( pColumn );
            }
        }
    }

    // pass 3: export line/rows
    sal_uInt16 nLines = rLines.Count();
    for( sal_uInt16 nLine=0U; nLine<nLines; nLine++ )
    {
        SwTableLine *pLine = rLines[nLine];

        SwFrmFmt *pFrmFmt = pLine->GetFrmFmt();
        if( rExpRows.AddRow( *pFrmFmt, rNamePrefix, nLine ) )
            ExportFmt( *pFrmFmt, XML_TABLE_ROW );

        const SwTableBoxes& rBoxes = pLine->GetTabBoxes();
        sal_uInt16 nBoxes = rBoxes.Count();

        sal_uInt16 nCPos = 0U;
        sal_uInt16 nCol = 0U;
        for( sal_uInt16 nBox=0U; nBox<nBoxes; nBox++ )
        {
            SwTableBox *pBox = rBoxes[nBox];

            if( nBox < nBoxes-1U )
                nCPos = nCPos + (sal_uInt16)SwWriteTable::GetBoxWidth( pBox );
            else
                nCPos = (sal_uInt16)pLines->GetWidth();


            // Und ihren Index
            sal_uInt16 nOldCol = nCol;
            SwXMLTableColumn_Impl aCol( nCPos );
#ifndef PRODUCT
            sal_Bool bFound =
#endif
                pLines->GetColumns().Seek_Entry( &aCol, &nCol );
            ASSERT( bFound, "couldn't find column" );

            const SwStartNode *pBoxSttNd = pBox->GetSttNd();
            if( pBoxSttNd )
            {
                SwFrmFmt *pFrmFmt2 = pBox->GetFrmFmt();
                if( rExpCells.AddCell( *pFrmFmt2, rNamePrefix, nOldCol, nLine,
                                       bTop) )
                    ExportFmt( *pFrmFmt2, XML_TABLE_CELL );

                Reference < XCell > xCell = SwXCell::CreateXCell(
                                                (SwFrmFmt *)rTblInfo.GetTblFmt(),
                                                  pBox,
                                                 (SwTable *)rTblInfo.GetTable() );
                if (xCell.is())
                {
                    Reference < XText > xText( xCell, UNO_QUERY );
                    if( !rTblInfo.IsBaseSectionValid() )
                    {
                        Reference<XPropertySet> xCellPropertySet( xCell,
                                                                 UNO_QUERY );
                        OUString sTextSection( RTL_CONSTASCII_USTRINGPARAM("TextSection") );
                        Any aAny = xCellPropertySet->getPropertyValue(sTextSection);
                        Reference < XTextSection > xTextSection;
                        aAny >>= xTextSection;
                        rTblInfo.SetBaseSection( xTextSection );
                    }

                    const bool bExportContent = (getExportFlags() & EXPORT_CONTENT ) != 0;
                    if ( !bExportContent )
                    {
                        // AUTOSTYLES - not needed anymore if we are currently exporting content.xml
                        GetTextParagraphExport()->collectTextAutoStyles(
                            xText, rTblInfo.GetBaseSection(), IsShowProgress() );
                    }
                }
                else
                    DBG_ERROR("here should be a XCell");
            }
            else
            {
                lcl_xmltble_appendBoxPrefix( sBuffer, rNamePrefix, nOldCol,
                                             nLine, bTop );

                ExportTableLinesAutoStyles( pBox->GetTabLines(),
                                            nAbsWidth, nBaseWidth,
                                            sBuffer.makeStringAndClear(),
                                            rExpCols, rExpRows, rExpCells,
                                             rTblInfo );
            }

            nCol++;
        }
    }
}

void SwXMLExport::ExportTableAutoStyles( const SwTableNode& rTblNd )
{
    const SwTable& rTbl = rTblNd.GetTable();
    const SwFrmFmt *pTblFmt = rTbl.GetFrmFmt();

    if( pTblFmt )
    {
        sal_Int16 eTabHoriOri = pTblFmt->GetHoriOrient().GetHoriOrient();
        const SwFmtFrmSize& rFrmSize = pTblFmt->GetFrmSize();

        sal_uInt32 nAbsWidth = rFrmSize.GetSize().Width();
        sal_uInt32 nBaseWidth = 0UL;
        sal_Int8 nPrcWidth = rFrmSize.GetWidthPercent();

        sal_Bool bFixAbsWidth = nPrcWidth != 0 || /*text::*/HoriOrientation::NONE == eTabHoriOri
                                           || /*text::*/HoriOrientation::FULL == eTabHoriOri;
        if( bFixAbsWidth )
        {
            nBaseWidth = nAbsWidth;
            nAbsWidth = pTblFmt->FindLayoutRect(sal_True).Width();
            if( !nAbsWidth )
            {
                // TODO???
            }
        }
        ExportTableFmt( *pTblFmt, nAbsWidth );

        OUString sName( pTblFmt->GetName() );
        SwXMLTableColumnsSortByWidth_Impl aExpCols( 10, 10 );
        SwXMLTableFrmFmtsSort_Impl aExpRows( 10, 10 );
        SwXMLTableFrmFmtsSort_Impl aExpCells( 10, 10 );
        SwXMLTableInfo_Impl aTblInfo( &rTbl );
        ExportTableLinesAutoStyles( rTbl.GetTabLines(), nAbsWidth, nBaseWidth,
                                    sName, aExpCols, aExpRows, aExpCells,
                                    aTblInfo, sal_True);
    }
}

// ---------------------------------------------------------------------

void SwXMLExport::ExportTableBox( const SwTableBox& rBox,
                                  sal_uInt16 nColSpan,
                                  sal_uInt16 nRowSpan,
                                  SwXMLTableInfo_Impl& rTblInfo )
{
    const SwStartNode *pBoxSttNd = rBox.GetSttNd();
    if( pBoxSttNd )
    {
        const SwFrmFmt *pFrmFmt = rBox.GetFrmFmt();
        if( pFrmFmt )
        {
            const String& rName = pFrmFmt->GetName();
            if( rName.Len() )
            {
                AddAttribute( XML_NAMESPACE_TABLE, XML_STYLE_NAME, EncodeStyleName(rName) );
            }
        }
    }

    if( nRowSpan != 1 )
    {
        OUStringBuffer sTmp;
        sTmp.append( (sal_Int32)nRowSpan );
        AddAttribute( XML_NAMESPACE_TABLE, XML_NUMBER_ROWS_SPANNED,
                      sTmp.makeStringAndClear() );
    }

    if( nColSpan != 1 )
    {
        OUStringBuffer sTmp;
        sTmp.append( (sal_Int32)nColSpan );
        AddAttribute( XML_NAMESPACE_TABLE, XML_NUMBER_COLUMNS_SPANNED,
                      sTmp.makeStringAndClear() );
    }

    {
        if( pBoxSttNd )
        {
            // start node -> normal cell

            // get cell range for table
            Reference<XCell> xCell = SwXCell::CreateXCell( (SwFrmFmt *)rTblInfo.GetTblFmt(),
                                                            (SwTableBox *)&rBox,
                                                            (SwTable *)rTblInfo.GetTable() );

            if (xCell.is())
            {
                Reference<XText> xText( xCell, UNO_QUERY );

                // get formula (and protection)
                OUString sCellFormula = xCell->getFormula();

                // if this cell has a formula, export it
                //     (with value and number format)
                if (sCellFormula.getLength()>0)
                {
                    OUString sQValue =
                        GetNamespaceMap().GetQNameByKey(
                                XML_NAMESPACE_OOOW, sCellFormula, sal_False );
                    // formula
                    AddAttribute(XML_NAMESPACE_TABLE, XML_FORMULA, sQValue );
                }

                // value and format (if NumberFormat != -1)
                Reference<XPropertySet> xCellPropertySet(xCell,
                                                        UNO_QUERY);
                if (xCellPropertySet.is())
                {
                    sal_Int32 nNumberFormat = 0;
                    Any aAny = xCellPropertySet->getPropertyValue(sNumberFormat);
                    aAny >>= nNumberFormat;

                    if (NUMBERFORMAT_TEXT == nNumberFormat)
                    {
                        // text format
                        AddAttribute( XML_NAMESPACE_OFFICE,
                                    XML_VALUE_TYPE, XML_STRING );
                    }
                    else if ( (-1 != nNumberFormat) && (xText->getString().getLength() > 0) )
                    {
                        // number format key:
                        // (export values only if cell contains text;
                        //  cf. #83755#)
                        XMLNumberFormatAttributesExportHelper::
                            SetNumberFormatAttributes(
                                *this, nNumberFormat, xCell->getValue(),
                                 sal_True );
                    }
                    // else: invalid key; ignore

                    // cell protection
                    aAny = xCellPropertySet->getPropertyValue(sIsProtected);
                    if (*(sal_Bool*)aAny.getValue())
                    {
                        AddAttribute( XML_NAMESPACE_TABLE, XML_PROTECTED,
                                        XML_TRUE );
                    }

                    if( !rTblInfo.IsBaseSectionValid() )
                    {
                        OUString sTextSection( RTL_CONSTASCII_USTRINGPARAM("TextSection") );
                        aAny = xCellPropertySet->getPropertyValue(sTextSection);
                        Reference < XTextSection > xTextSection;
                        aAny >>= xTextSection;
                        rTblInfo.SetBaseSection( xTextSection );
                    }
                }

                // export cell element
                SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE,
                                        XML_TABLE_CELL, sal_True, sal_True );

                // export cell content
                GetTextParagraphExport()->exportText( xText,
                                                    rTblInfo.GetBaseSection(),
                                                    IsShowProgress() );
            }
            else
            {
                DBG_ERROR("here should be a XCell");
                ClearAttrList();
            }
        }
        else
        {
            // no start node -> merged cells: export subtable in cell
            SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE,
                                      XML_TABLE_CELL, sal_True, sal_True );
            {
                AddAttribute( XML_NAMESPACE_TABLE, XML_IS_SUB_TABLE,
                                GetXMLToken( XML_TRUE ) );

                SvXMLElementExport aElemExport( *this, XML_NAMESPACE_TABLE,
                                          XML_TABLE, sal_True, sal_True );
                ExportTableLines( rBox.GetTabLines(), rTblInfo );
            }
        }
    }
}

void SwXMLExport::ExportTableLine( const SwTableLine& rLine,
                                   const SwXMLTableLines_Impl& rLines,
                                   SwXMLTableInfo_Impl& rTblInfo )
{
    if( rLine.hasSoftPageBreak() )
    {
        SvXMLElementExport aElem( *this, XML_NAMESPACE_TEXT,
                                  XML_SOFT_PAGE_BREAK, sal_True, sal_True );
    }
    const SwFrmFmt *pFrmFmt = rLine.GetFrmFmt();
    if( pFrmFmt )
    {
        const String& rName = pFrmFmt->GetName();
        if( rName.Len() )
        {
            AddAttribute( XML_NAMESPACE_TABLE, XML_STYLE_NAME, EncodeStyleName(rName) );
        }
    }

    {
        SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE,
                                  XML_TABLE_ROW, sal_True, sal_True );
        const SwTableBoxes& rBoxes = rLine.GetTabBoxes();
        sal_uInt16 nBoxes = rBoxes.Count();

        sal_uInt16 nCPos = 0U;
        sal_uInt16 nCol = 0U;
        for( sal_uInt16 nBox=0U; nBox<nBoxes; nBox++ )
        {
            const SwTableBox *pBox = rBoxes[nBox];

            // NEW TABLES
            const long nRowSpan = pBox->getRowSpan();
            if( nRowSpan < 1 )
            {
                SvXMLElementExport aElem2( *this, XML_NAMESPACE_TABLE,
                                          XML_COVERED_TABLE_CELL, sal_True,
                                          sal_False );
            }

            if( nBox < nBoxes-1U )
                nCPos = nCPos + (sal_uInt16)SwWriteTable::GetBoxWidth( pBox );
            else
                nCPos = (sal_uInt16)rLines.GetWidth();

            // Und ihren Index
            sal_uInt16 nOldCol = nCol;
            SwXMLTableColumn_Impl aCol( nCPos );
#ifndef PRODUCT
            sal_Bool bFound =
#endif
                rLines.GetColumns().Seek_Entry( &aCol, &nCol );
            ASSERT( bFound, "couldn't find column" );

            sal_uInt16 nColSpan = nCol - nOldCol + 1U;

            if ( nRowSpan >= 1 )
                ExportTableBox( *pBox, nColSpan, static_cast< sal_uInt16 >(nRowSpan), rTblInfo );

            for( sal_uInt16 i=nOldCol; i<nCol; i++ )
            {
                SvXMLElementExport aElemExport( *this, XML_NAMESPACE_TABLE,
                                          XML_COVERED_TABLE_CELL, sal_True,
                                          sal_False );
            }

            nCol++;
        }
    }
}

void SwXMLExport::ExportTableLines( const SwTableLines& rLines,
                                    SwXMLTableInfo_Impl& rTblInfo,
                                    USHORT nHeaderRows )
{
    ASSERT( pTableLines && pTableLines->Count(),
            "SwXMLExport::ExportTableLines: table columns infos missing" );
    if( !pTableLines || 0 == pTableLines->Count() )
        return;

    SwXMLTableLines_Impl *pLines = 0;
    sal_uInt16 nInfoPos;
    for( nInfoPos=0; nInfoPos < pTableLines->Count(); nInfoPos++ )
    {
        if( pTableLines->GetObject( nInfoPos )->GetLines() == &rLines )
        {
            pLines = pTableLines->GetObject( nInfoPos );
            break;
        }
    }
    ASSERT( pLines,
            "SwXMLExport::ExportTableLines: table columns info missing" );
    ASSERT( 0==nInfoPos,
            "SwXMLExport::ExportTableLines: table columns infos are unsorted" );
    if( !pLines )
        return;

    pTableLines->Remove( nInfoPos );
    if( 0 == pTableLines->Count() )
    {
        delete pTableLines ;
        pTableLines = 0;
    }

    // pass 2: export columns
    const SwXMLTableColumns_Impl& rCols = pLines->GetColumns();
    sal_uInt16 nColumn = 0U;
    sal_uInt16 nColumns = rCols.Count();
    sal_uInt16 nColRep = 1U;
    SwXMLTableColumn_Impl *pColumn = (nColumns > 0) ? rCols[0U] : 0;
    while( pColumn )
    {
        nColumn++;
        SwXMLTableColumn_Impl *pNextColumn =
            (nColumn < nColumns) ? rCols[nColumn] : 0;
        if( pNextColumn &&
            pNextColumn->GetStyleName() == pColumn->GetStyleName() )
        {
            nColRep++;
        }
        else
        {
            AddAttribute( XML_NAMESPACE_TABLE, XML_STYLE_NAME,
                          EncodeStyleName(pColumn->GetStyleName()) );

            if( nColRep > 1U )
            {
                OUStringBuffer sTmp(4);
                sTmp.append( (sal_Int32)nColRep );
                AddAttribute( XML_NAMESPACE_TABLE, XML_NUMBER_COLUMNS_REPEATED,
                              sTmp.makeStringAndClear() );
            }

            {
                SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE,
                                          XML_TABLE_COLUMN, sal_True, sal_True );
            }

            nColRep = 1U;
        }
        pColumn = pNextColumn;
    }

    // pass 3: export line/rows
    sal_uInt16 nLines = rLines.Count();
    // export header rows, if present
    if( nHeaderRows > 0 )
    {
        SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE,
                                  XML_TABLE_HEADER_ROWS, sal_True, sal_True );

        DBG_ASSERT( nHeaderRows <= nLines, "more headers then lines?" );
        for( sal_uInt16 nLine = 0U; nLine < nHeaderRows; nLine++ )
            ExportTableLine( *(rLines[nLine]), *pLines, rTblInfo );
    }
    // export remaining rows
    for( sal_uInt16 nLine = nHeaderRows; nLine < nLines; nLine++ )
    {
        ExportTableLine( *(rLines[nLine]), *pLines, rTblInfo );
    }

    delete pLines;
}

sal_Bool lcl_xmltble_ClearName_Line( const SwTableLine*& rpLine, void* );

sal_Bool lcl_xmltble_ClearName_Box( const SwTableBox*& rpBox, void* )
{
    if( !rpBox->GetSttNd() )
    {
        ((SwTableBox *)rpBox)->GetTabLines().ForEach(
                                            &lcl_xmltble_ClearName_Line, 0 );
    }
    else
    {
        SwFrmFmt *pFrmFmt = ((SwTableBox *)rpBox)->GetFrmFmt();
        if( pFrmFmt && pFrmFmt->GetName().Len() )
            pFrmFmt->SetName( aEmptyStr );
    }

    return sal_True;
}

sal_Bool lcl_xmltble_ClearName_Line( const SwTableLine*& rpLine, void* )
{
    ((SwTableLine *)rpLine)->GetTabBoxes().ForEach(
                                            &lcl_xmltble_ClearName_Box, 0 );

    return sal_True;
}

void SwXMLExport::ExportTable( const SwTableNode& rTblNd )
{
    const SwTable& rTbl = rTblNd.GetTable();
    const SwFrmFmt *pTblFmt = rTbl.GetFrmFmt();
    if( pTblFmt && pTblFmt->GetName().Len() )
    {
        AddAttribute( XML_NAMESPACE_TABLE, XML_NAME, pTblFmt->GetName() );
        AddAttribute( XML_NAMESPACE_TABLE, XML_STYLE_NAME,
                      EncodeStyleName( pTblFmt->GetName() ) );
    }

    {
        SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE, XML_TABLE,
                                  sal_True, sal_True );

        // export DDE source (if this is a DDE table)
        if ( rTbl.ISA(SwDDETable) )
        {
            // get DDE Field Type (contains the DDE connection)
            const SwDDEFieldType* pDDEFldType =
                ((SwDDETable&)rTbl).GetDDEFldType();

            // connection name
            AddAttribute( XML_NAMESPACE_OFFICE, XML_NAME,
                          pDDEFldType->GetName() );

            // DDE command
            const String sCmd = pDDEFldType->GetCmd();
            AddAttribute( XML_NAMESPACE_OFFICE, XML_DDE_APPLICATION,
                          sCmd.GetToken(0, sfx2::cTokenSeperator) );
            AddAttribute( XML_NAMESPACE_OFFICE, XML_DDE_ITEM,
                          sCmd.GetToken(1, sfx2::cTokenSeperator) );
            AddAttribute( XML_NAMESPACE_OFFICE, XML_DDE_TOPIC,
                          sCmd.GetToken(2, sfx2::cTokenSeperator) );

            // auto update
            if (pDDEFldType->GetType() == sfx2::LINKUPDATE_ALWAYS)
            {
                AddAttribute( XML_NAMESPACE_OFFICE,
                              XML_AUTOMATIC_UPDATE, XML_TRUE );
            }

            // DDE source element (always empty)
            SvXMLElementExport aSource(*this, XML_NAMESPACE_OFFICE,
                                       XML_DDE_SOURCE, sal_True, sal_False);
        }

        SwXMLTableInfo_Impl aTblInfo( &rTbl );
        ExportTableLines( rTbl.GetTabLines(), aTblInfo, rTbl.GetRowsToRepeat() );

        ((SwTable &)rTbl).GetTabLines().ForEach( &lcl_xmltble_ClearName_Line,
                                                 0 );
    }
}

void SwXMLTextParagraphExport::exportTable(
        const Reference < XTextContent > & rTextContent,
        sal_Bool bAutoStyles, sal_Bool _bProgress )
{
    sal_Bool bOldShowProgress = ((SwXMLExport&)GetExport()).IsShowProgress();
    ((SwXMLExport&)GetExport()).SetShowProgress( _bProgress );

    Reference < XTextTable > xTxtTbl( rTextContent, UNO_QUERY );
    DBG_ASSERT( xTxtTbl.is(), "text table missing" );
    if( xTxtTbl.is() )
    {
        const SwXTextTable *pXTable = 0;
        Reference<XUnoTunnel> xTableTunnel( rTextContent, UNO_QUERY);
        if( xTableTunnel.is() )
        {
            pXTable = reinterpret_cast< SwXTextTable * >(
                    sal::static_int_cast< sal_IntPtr >( xTableTunnel->getSomething( SwXTextTable::getUnoTunnelId() )));
            ASSERT( pXTable, "SwXTextTable missing" );
        }
        if( pXTable )
        {
            SwFrmFmt *pFmt = pXTable->GetFrmFmt();
            ASSERT( pFmt, "table format missing" );
            const SwTable *pTbl = SwTable::FindTable( pFmt );
            ASSERT( pTbl, "table missing" );
            const SwTableNode *pTblNd = pTbl->GetTableNode();
            ASSERT( pTblNd, "table node missing" );
            if( bAutoStyles )
            {
                SwNodeIndex aIdx( *pTblNd );
                // AUTOSTYLES: Optimization: Do not export table autostyle if
                // we are currently exporting the content.xml stuff and
                // the table is located in header/footer:
                // #144704: During the flat XML export (used e.g. by .sdw-export)
                // ALL flags are set at the same time.
                const bool bExportStyles = ( GetExport().getExportFlags() & EXPORT_STYLES ) != 0;
                if ( bExportStyles || !pFmt->GetDoc()->IsInHeaderFooter( aIdx ) )
                    ((SwXMLExport&)GetExport()).ExportTableAutoStyles( *pTblNd );
            }
            else
            {
                ((SwXMLExport&)GetExport()).ExportTable( *pTblNd );
            }
        }
    }

    ((SwXMLExport&)GetExport()).SetShowProgress( bOldShowProgress );
}


