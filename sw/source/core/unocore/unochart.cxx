/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unochart.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: ihi $ $Date: 2007-08-17 13:59:52 $
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

#include <memory>
#include <algorithm>


#ifndef _COM_SUN_STAR_CHART_DATAROWSOURCE_HPP_
#include <com/sun/star/chart/ChartDataRowSource.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_DATAROWSOURCE_HPP_
#include <com/sun/star/chart2/data/LabelOrigin.hpp>
#endif

#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif

#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>     // SvNumberFormatter
#endif

#include <tools/link.hxx>

#include <XMLRangeHelper.hxx>

#ifndef _UNOCHART_HXX
#include <unochart.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _UNOPRNMS_HXX
#include <unoprnms.hxx>
#endif
#ifndef _UNOMAP_HXX
#include <unomap.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _UNOCRSR_HXX
#include <unocrsr.hxx>
#endif
#ifndef _UNOTBL_HXX
#include <unotbl.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _SWDOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _UNOCORE_HRC
#include <unocore.hrc>
#endif

#include <docary.hxx>

#define SN_DATA_PROVIDER            "com.sun.star.chart2.data.DataProvider"
#define SN_DATA_SOURCE              "com.sun.star.chart2.data.DataSource"
#define SN_DATA_SEQUENCE            "com.sun.star.chart2.data.DataSequence"
#define SN_LABELED_DATA_SEQUENCE    "com.sun.star.chart2.data.LabeledDataSequence"

#define DIRECTION_DONT_KNOW     -1
#define DIRECTION_HAS_ERROR     -2
#define DIRECTION_COLS           0
#define DIRECTION_ROWS           1

using namespace ::com::sun::star;
using namespace ::rtl;

// from unotbl.cxx
extern void lcl_GetCellPosition( const String &rCellName, sal_Int32 &rColumn, sal_Int32 &rRow);
extern String lcl_GetCellName( sal_Int32 nColumn, sal_Int32 nRow );
extern int lcl_CompareCellsByColFirst( const String &rCellName1, const String &rCellName2 );
extern int lcl_CompareCellsByRowFirst( const String &rCellName1, const String &rCellName2 );
extern int lcl_CompareCellRanges(
        const String &rRange1StartCell, const String &rRange1EndCell,
        const String &rRange2StartCell, const String &rRange2EndCell,
        sal_Bool bCmpColsFirst );
extern void lcl_NormalizeRange( String &rCell1, String &rCell2 );

//////////////////////////////////////////////////////////////////////

SwChartLockController_Helper::SwChartLockController_Helper( SwDoc *pDocument ) :
    pDoc( pDocument )
{
    aUnlockTimer.SetTimeout( 1500 );
    aUnlockTimer.SetTimeoutHdl( LINK( this, SwChartLockController_Helper, DoUnlockAllCharts ));
}


SwChartLockController_Helper::~SwChartLockController_Helper()
{
    if (pDoc)   // still connected?
        Disconnect();
}


void SwChartLockController_Helper::StartOrContinueLocking()
{
    if (!bIsLocked)
        LockAllCharts();
    aUnlockTimer.Start();   // start or continue time of locking
}


void SwChartLockController_Helper::Disconnect()
{
    aUnlockTimer.Stop();
    UnlockAllCharts();
    pDoc = 0;
}


void SwChartLockController_Helper::LockUnlockAllCharts( sal_Bool bLock )
{
    if (!pDoc)
        return;

    const SwFrmFmts& rTblFmts = *pDoc->GetTblFrmFmts();
    for( USHORT n = 0; n < rTblFmts.Count(); ++n )
    {
        SwTable* pTmpTbl;
        const SwTableNode* pTblNd;
        SwFrmFmt* pFmt = rTblFmts[ n ];

        if( 0 != ( pTmpTbl = SwTable::FindTable( pFmt ) ) &&
            0 != ( pTblNd = pTmpTbl->GetTableNode() ) &&
            pTblNd->GetNodes().IsDocNodes() )
        {
            uno::Reference< frame::XModel > xRes;

            String aName( pTmpTbl->GetFrmFmt()->GetName() );
            SwOLENode *pONd;
            SwStartNode *pStNd;
            SwNodeIndex aIdx( *pDoc->GetNodes().GetEndOfAutotext().StartOfSectionNode(), 1 );
            while( 0 != (pStNd = aIdx.GetNode().GetStartNode()) )
            {
                aIdx++;
                if (0 != ( pONd = aIdx.GetNode().GetOLENode() ) &&
                    pONd->GetChartTblName().Len() > 0 /* is chart object? */)
                {
                    uno::Reference < embed::XEmbeddedObject > xIP = pONd->GetOLEObj().GetOleRef();
                    if ( svt::EmbeddedObjectRef::TryRunningState( xIP ) )
                    {
                        xRes = uno::Reference < frame::XModel >( xIP->getComponent(), uno::UNO_QUERY );
                        if (xRes.is())
                        {
                            if (bLock)
                                xRes->lockControllers();
                            else
                                xRes->unlockControllers();
                        }
                    }
                }
                aIdx.Assign( *pStNd->EndOfSectionNode(), + 1 );
            }
        }
    }

    bIsLocked = bLock;
}


IMPL_LINK( SwChartLockController_Helper, DoUnlockAllCharts, Timer *, /*pTimer*/ )
{
    UnlockAllCharts();
    return 0;
}


//////////////////////////////////////////////////////////////////////

static osl::Mutex &    GetChartMutex()
{
    static osl::Mutex   aMutex;
    return aMutex;
}


static void LaunchModifiedEvent(
        ::cppu::OInterfaceContainerHelper &rICH,
        const uno::Reference< uno::XInterface > &rxI )
{
    lang::EventObject aEvtObj( rxI );
    cppu::OInterfaceIteratorHelper aIt( rICH );
    while (aIt.hasMoreElements())
    {
        uno::Reference< util::XModifyListener > xRef( aIt.next(), uno::UNO_QUERY );
        if (xRef.is())
            xRef->modified( aEvtObj );
    }
}

//////////////////////////////////////////////////////////////////////

// rCellRangeName needs to be of one of the following formats:
// - e.g. "A2:E5" or
// - e.g. "Table1.A2:E5"
sal_Bool FillRangeDescriptor(
        SwRangeDescriptor &rDesc,
        const String &rCellRangeName )
{
    xub_StrLen nToken = STRING_NOTFOUND == rCellRangeName.Search('.') ? 0 : 1;
    String aCellRangeNoTableName( rCellRangeName.GetToken( nToken, '.' ) );
    String aTLName( aCellRangeNoTableName.GetToken(0, ':') );  // name of top left cell
    String aBRName( aCellRangeNoTableName.GetToken(1, ':') );  // name of bottom right cell
    if(!aTLName.Len() || !aBRName.Len())
        return sal_False;

    rDesc.nTop = rDesc.nLeft = rDesc.nBottom = rDesc.nRight = -1;
    lcl_GetCellPosition( aTLName, rDesc.nLeft,  rDesc.nTop );
    lcl_GetCellPosition( aBRName, rDesc.nRight, rDesc.nBottom );
    rDesc.Normalize();
    DBG_ASSERT( rDesc.nTop    != -1 &&
                rDesc.nLeft   != -1 &&
                rDesc.nBottom != -1 &&
                rDesc.nRight  != -1,
            "failed to get range descriptor" );
    DBG_ASSERT( rDesc.nTop <= rDesc.nBottom  &&  rDesc.nLeft <= rDesc.nRight,
            "invalid range descriptor");
    return sal_True;
}


static String GetCellRangeName( SwFrmFmt &rTblFmt, SwUnoCrsr &rTblCrsr )
{
    String aRes;

    //!! see also SwXTextTableCursor::getRangeName

    SwUnoTableCrsr* pUnoTblCrsr = rTblCrsr;
#if OSL_DEBUG_LEVEL > 1
    if (!pUnoTblCrsr)
        return String();
#endif
    pUnoTblCrsr->MakeBoxSels();

    const SwStartNode*  pStart;
    const SwTableBox*   pStartBox   = 0;
    const SwTableBox*   pEndBox     = 0;

    pStart = pUnoTblCrsr->GetPoint()->nNode.GetNode().FindTableBoxStartNode();
    const SwTable* pTable = SwTable::FindTable( &rTblFmt );
    pEndBox = pTable->GetTblBox( pStart->GetIndex());
    aRes = pEndBox->GetName();

    if(pUnoTblCrsr->HasMark())
    {
        pStart = pUnoTblCrsr->GetMark()->nNode.GetNode().FindTableBoxStartNode();
        pStartBox = pTable->GetTblBox( pStart->GetIndex());
    }
    DBG_ASSERT( pStartBox, "start box not found" );
    DBG_ASSERT( pEndBox, "end box not found" );
    if (pStart)
    {
        // need to switch start and end?
        if (*pUnoTblCrsr->GetPoint() < *pUnoTblCrsr->GetMark())
        {
            const SwTableBox* pTmpBox = pStartBox;
            pStartBox = pEndBox;
            pEndBox = pTmpBox;
        }

        aRes = pStartBox->GetName();
        aRes += (sal_Unicode)':';
        if (pEndBox)
            aRes += pEndBox->GetName();
        else
            aRes += pStartBox->GetName();
    }

    return aRes;
}


static String GetRangeRepFromTableAndCells( const String &rTableName,
        const String &rStartCell, const String &rEndCell,
        sal_Bool bForceEndCellName )
{
    DBG_ASSERT( rTableName.Len(), "table name missing" );
    DBG_ASSERT( rStartCell.Len(), "cell name missing" );
    String aRes( rTableName );
    aRes += (sal_Unicode) '.';
    aRes += rStartCell;

    if (rEndCell.Len())
    {
        aRes += (sal_Unicode) ':';
        aRes += rEndCell;
    }
    else if (bForceEndCellName)
    {
        aRes += (sal_Unicode) ':';
        aRes += rStartCell;
    }

    return aRes;
}


static sal_Bool GetTableAndCellsFromRangeRep(
        const OUString &rRangeRepresentation,
        String &rTblName,
        String &rStartCell,
        String &rEndCell,
        sal_Bool bSortStartEndCells = sal_True )
{
    // parse range representation for table name and cell/range names
    // accepted format sth like: "Table1.A2:C5" , "Table2.A2.1:B3.2"
    String aTblName;    // table name
    OUString aRange;    // cell range
    String aStartCell;  // name of top left cell
    String aEndCell;    // name of bottom right cell
    sal_Int32 nIdx = rRangeRepresentation.indexOf( '.' );
    if (nIdx >= 0)
    {
        aTblName = rRangeRepresentation.copy( 0, nIdx );
        aRange = rRangeRepresentation.copy( nIdx + 1 );
        sal_Int32 nPos = aRange.indexOf( ':' );
        if (nPos >= 0) // a cell-range like "Table1.A2:D4"
        {
            aStartCell = aRange.copy( 0, nPos );
            aEndCell   = aRange.copy( nPos + 1 );

            // need to switch start and end cell ?
            // (does not check for normalization here)
            if (bSortStartEndCells && 1 == lcl_CompareCellsByColFirst( aStartCell, aEndCell ))
            {
                String aTmp( aStartCell );
                aStartCell  = aEndCell;
                aEndCell    = aTmp;
            }
        }
        else	// a single cell like in "Table1.B3"
        {
            aStartCell = aEndCell = aRange;
        }
    }

    sal_Bool bSuccess = aTblName.Len() != 0 &&
                        aStartCell.Len() != 0 && aEndCell.Len() != 0;
    if (bSuccess)
    {
        rTblName    = aTblName;
        rStartCell  = aStartCell;
        rEndCell    = aEndCell;
    }
    return bSuccess;
}


static void GetTableByName( const SwDoc &rDoc, const String &rTableName,
        SwFrmFmt **ppTblFmt, SwTable **ppTable)
{
    SwFrmFmt *pTblFmt = NULL;

    // find frame format of table
    //! see SwXTextTables::getByName
    sal_uInt16 nCount = rDoc.GetTblFrmFmtCount(sal_True);
    for (sal_uInt16 i = 0; i < nCount && !pTblFmt; ++i)
    {
        SwFrmFmt& rTblFmt = rDoc.GetTblFrmFmt(i, sal_True);
        if(rTableName == rTblFmt.GetName())
            pTblFmt = &rTblFmt;
    }

    if (ppTblFmt)
        *ppTblFmt = pTblFmt;

    if (ppTable)
        *ppTable = pTblFmt ? SwTable::FindTable( pTblFmt ) : 0;
}


static void GetFormatAndCreateCursorFromRangeRep(
        const SwDoc    *pDoc,
        const OUString &rRangeRepresentation,   // must be a single range (i.e. so called sub-range)
        SwFrmFmt    **ppTblFmt,     // will be set to the table format of the table used in the range representation
        SwUnoCrsr   **ppUnoCrsr )   // will be set to cursor spanning the cell range
                                    // (cursor will be created!)
{
    String aTblName;    // table name
    String aStartCell;  // name of top left cell
    String aEndCell;    // name of bottom right cell
    sal_Bool bNamesFound = GetTableAndCellsFromRangeRep( rRangeRepresentation,
                                  aTblName, aStartCell, aEndCell );

    if (!bNamesFound)
    {
        if (ppTblFmt)
            *ppTblFmt   = NULL;
        if (ppUnoCrsr)
            *ppUnoCrsr  = NULL;
    }
    else
    {
        SwFrmFmt *pTblFmt = NULL;

        // is the correct table format already provided?
        if (*ppTblFmt != NULL  &&  (*ppTblFmt)->GetName() == aTblName)
            pTblFmt = *ppTblFmt;
        else if (ppTblFmt)
            GetTableByName( *pDoc, aTblName, &pTblFmt, NULL );

        if (ppTblFmt)
            *ppTblFmt = pTblFmt;

        if (ppUnoCrsr != NULL)
        {
            *ppUnoCrsr = NULL;  // default result in case of failure

            SwTable *pTable = pTblFmt ? SwTable::FindTable( pTblFmt ) : 0;
            // create new SwUnoCrsr spanning the specified range
            //! see also SwXTextTable::GetRangeByName
            // --> OD 2007-08-03 #i80314#
            // perform validation check. Thus, pass <true> as 2nd parameter to <SwTable::GetTblBox(..)>
            const SwTableBox* pTLBox =
                            pTable ? pTable->GetTblBox( aStartCell, true ) : 0;
            // <--
            if(pTLBox)
            {
                // hier muessen die Actions aufgehoben werden
                UnoActionRemoveContext aRemoveContext(pTblFmt->GetDoc());
                const SwStartNode* pSttNd = pTLBox->GetSttNd();
                SwPosition aPos(*pSttNd);
                // set cursor to top left box of range
                SwUnoCrsr* pUnoCrsr = pTblFmt->GetDoc()->CreateUnoCrsr(aPos, sal_True);
                pUnoCrsr->Move( fnMoveForward, fnGoNode );
                pUnoCrsr->SetRemainInSection( sal_False );
                // --> OD 2007-08-03 #i80314#
                // perform validation check. Thus, pass <true> as 2nd parameter to <SwTable::GetTblBox(..)>
                const SwTableBox* pBRBox = pTable->GetTblBox( aEndCell, true );
                // <--
                if(pBRBox)
                {
                    pUnoCrsr->SetMark();
                    pUnoCrsr->GetPoint()->nNode = *pBRBox->GetSttNd();
                    pUnoCrsr->Move( fnMoveForward, fnGoNode );
                    SwUnoTableCrsr* pCrsr = *pUnoCrsr;
                    pCrsr->MakeBoxSels();

                    if (ppUnoCrsr)
                        *ppUnoCrsr = pCrsr;
                }
                else
                {
                    delete pUnoCrsr;
                }
            }
        }
    }
}


static sal_Bool GetSubranges( const OUString &rRangeRepresentation,
        uno::Sequence< OUString > &rSubRanges, sal_Bool bNormalize )
{
    sal_Bool bRes = sal_True;
    String aRangesStr( rRangeRepresentation );
    xub_StrLen nLen = aRangesStr.GetTokenCount( ';' );
    uno::Sequence< OUString > aRanges( nLen );

    sal_Int32 nCnt = 0;
    if (nLen != 0)
    {
        OUString *pRanges = aRanges.getArray();
        String aFirstTable;
        for ( xub_StrLen i = 0;  i < nLen && bRes;  ++i)
        {
            String aRange( aRangesStr.GetToken( i, ';' ) );
            if (aRange.Len())
            {
                pRanges[nCnt] = aRange;

                String aTableName, aStartCell, aEndCell;
                bRes &= GetTableAndCellsFromRangeRep( aRange,
                                aTableName, aStartCell, aEndCell );

                if (bNormalize)
                {
                    lcl_NormalizeRange( aStartCell, aEndCell );
                    pRanges[nCnt] = GetRangeRepFromTableAndCells( aTableName,
                                    aStartCell, aEndCell, sal_True );
                }

                // make sure to use only a single table
                if (nCnt == 0)
                    aFirstTable = aTableName;
                else
                    bRes &= aFirstTable == aTableName;

                ++nCnt;
            }
        }
    }
    aRanges.realloc( nCnt );

    rSubRanges = aRanges;
    return bRes;
}


static void SortSubranges( uno::Sequence< OUString > &rSubRanges, sal_Bool bCmpByColumn )
{
    sal_Int32 nLen = rSubRanges.getLength();
    OUString *pSubRanges = rSubRanges.getArray();

    String aSmallestTblName;
    String aSmallestStartCell;
    String aSmallestEndCell;

    for (sal_Int32 i = 0;  i < nLen;  ++i)
    {
        sal_Int32 nIdxOfSmallest = i;
        GetTableAndCellsFromRangeRep( pSubRanges[nIdxOfSmallest],
                aSmallestTblName, aSmallestStartCell, aSmallestEndCell );
        if (aSmallestEndCell.Len() == 0)
            aSmallestEndCell = aSmallestStartCell;

        for (sal_Int32 k = i+1;  k < nLen;  ++k)
        {
            // get cell names for sub range
            String aTblName;
            String aStartCell;
            String aEndCell;
            GetTableAndCellsFromRangeRep( pSubRanges[k],
                    aTblName, aStartCell, aEndCell );
            if (aEndCell.Len() == 0)
                aEndCell = aStartCell;

            // compare cell ranges ( is the new one smaller? )
            if (-1 == lcl_CompareCellRanges( aStartCell, aEndCell,
                                aSmallestStartCell, aSmallestEndCell, bCmpByColumn ))
            {
                nIdxOfSmallest = k;
                aSmallestTblName    = aTblName;
                aSmallestStartCell  = aStartCell;
                aSmallestEndCell    = aEndCell;
            }
        }

        // move smallest element to the start of the not sorted area
        OUString aTmp( pSubRanges[ nIdxOfSmallest ] );
        pSubRanges[ nIdxOfSmallest ] = pSubRanges[ i ];
        pSubRanges[ i ] = aTmp;
    }
}

//////////////////////////////////////////////////////////////////////

SwChartDataProvider::SwChartDataProvider( const SwDoc* pSwDoc ) :
    aEvtListeners( GetChartMutex() ),
    pDoc( pSwDoc )
{
    bDisposed = sal_False;
}


SwChartDataProvider::~SwChartDataProvider()
{
}

uno::Reference< chart2::data::XDataSource > SwChartDataProvider::Impl_createDataSource(
        const uno::Sequence< beans::PropertyValue >& rArguments, sal_Bool bTestOnly )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Reference< chart2::data::XDataSource > xRes;

    if (!pDoc)
        throw uno::RuntimeException();

    // get arguments
    OUString aRangeRepresentation;
    uno::Sequence< sal_Int32 > aSequenceMapping;
    sal_Bool bFirstIsLabel      = sal_False;
    sal_Bool bDtaSrcIsColumns   = sal_True; // true : DataSource will be sequence of columns
                                            // false: DataSource will be sequence of rows
    OUString aChartOleObjectName;//work around wrong writer ranges ( see Issue 58464 )
    sal_Int32 nArgs = rArguments.getLength();
    DBG_ASSERT( nArgs != 0, "no properties provided" );
    if (nArgs == 0)
        return xRes;
    const beans::PropertyValue *pArg = rArguments.getConstArray();
    for (sal_Int32 i = 0;  i < nArgs;  ++i)
    {
        if (pArg[i].Name.equalsAscii( "DataRowSource" ))
        {
            chart::ChartDataRowSource eSource;
            if (!(pArg[i].Value >>= eSource))
            {
                sal_Int32 nTmp;
                if (!(pArg[i].Value >>= nTmp))
                    throw lang::IllegalArgumentException();
                eSource = static_cast< chart::ChartDataRowSource >( nTmp );
            }
            bDtaSrcIsColumns = eSource == chart::ChartDataRowSource_COLUMNS;
        }
        else if (pArg[i].Name.equalsAscii( "FirstCellAsLabel" ))
        {
            if (!(pArg[i].Value >>= bFirstIsLabel))
                throw lang::IllegalArgumentException();
        }
        else if (pArg[i].Name.equalsAscii( "CellRangeRepresentation" ))
        {
            if (!(pArg[i].Value >>= aRangeRepresentation))
                throw lang::IllegalArgumentException();
        }
        else if (pArg[i].Name.equalsAscii( "SequenceMapping" ))
        {
            if (!(pArg[i].Value >>= aSequenceMapping))
                throw lang::IllegalArgumentException();
        }
        else if (pArg[i].Name.equalsAscii( "ChartOleObjectName" ))
        {
            if (!(pArg[i].Value >>= aChartOleObjectName))
                throw lang::IllegalArgumentException();
        }
    }

    uno::Sequence< OUString > aSubRanges;
    // get sub-ranges and check that they all are from the very same table
    sal_Bool bOk = GetSubranges( aRangeRepresentation, aSubRanges, sal_True );

    if (!bOk && pDoc && aChartOleObjectName.getLength() )
    {
        //try to correct the range here
        //work around wrong writer ranges ( see Issue 58464 )
        String aChartTableName;

        const SwNodes& rNodes = pDoc->GetNodes();
        for( ULONG nN = rNodes.Count(); nN--; )
        {
            SwNodePtr pNode = rNodes[nN];
            if( !pNode )
                continue;
            const SwOLENode* pOleNode = pNode->GetOLENode();
            if( !pOleNode )
                continue;
            const SwOLEObj& rOObj = pOleNode->GetOLEObj();
            if( aChartOleObjectName.equals( rOObj.GetCurrentPersistName() ) )
            {
                aChartTableName = pOleNode->GetChartTblName();
                break;
            }
        }

        if( aChartTableName.Len() )
        {
            //the wrong range is still shifted one row down
            //thus the first row is missing and an invalid row at the end is added.
            //Therefore we need to shift the range one row up
            SwRangeDescriptor aDesc;
            if (aRangeRepresentation.getLength() == 0)
                return xRes;        // we cant handle this thus returning an empty references
            aRangeRepresentation = aRangeRepresentation.copy( 1 );    // get rid of '.' to have only the cell range left
            FillRangeDescriptor( aDesc, aRangeRepresentation );
            aDesc.Normalize();
            if (aDesc.nTop <= 0)    // no chance to shift the range one row up?
                return xRes;        // we cant handle this thus returning an empty references
            aDesc.nTop      -= 1;
            aDesc.nBottom   -= 1;

            String aNewStartCell( lcl_GetCellName( aDesc.nLeft, aDesc.nTop ) );
            String aNewEndCell( lcl_GetCellName( aDesc.nRight, aDesc.nBottom ) );
            aRangeRepresentation = GetRangeRepFromTableAndCells(
                        aChartTableName, aNewStartCell, aNewEndCell, sal_True );
            bOk = GetSubranges( aRangeRepresentation, aSubRanges, sal_True );
        }
    }
    if (!bOk)    // different tables used, or incorrect range specifiers
        throw lang::IllegalArgumentException();

    SortSubranges( aSubRanges, bDtaSrcIsColumns );
    const OUString *pSubRanges = aSubRanges.getConstArray();
#if OSL_DEBUG_LEVEL > 1
    {
        sal_Int32 nSR = aSubRanges.getLength();
        OUString *pSR = aSubRanges.getArray();
        OUString aRg;
        for (sal_Int32 i = 0;  i < nSR;  ++i)
        {
            aRg = pSR[i];
        }
    }
#endif

    // get table format for that single table from above
    SwFrmFmt    *pTblFmt  = 0;      // pointer to table format
    SwUnoCrsr   *pUnoCrsr = 0;      // here required to check if the cells in the range do actually exist
    std::auto_ptr< SwUnoCrsr > pAuto( pUnoCrsr );  // to end lifetime of object pointed to by pUnoCrsr
    if (aSubRanges.getLength() > 0)
        GetFormatAndCreateCursorFromRangeRep( pDoc, pSubRanges[0], &pTblFmt, &pUnoCrsr );
    if (!pTblFmt || !pUnoCrsr)
        throw lang::IllegalArgumentException();

    if(pTblFmt)
    {
        SwTable* pTable = SwTable::FindTable( pTblFmt );
        if(pTable->IsTblComplex())
            return xRes;    // we cant handle this thus returning an empty references
        else
        {
            // get a character map in the size of the table to mark
            // all the ranges to use in
            sal_Int32 nRows = pTable->GetTabLines().Count();
            sal_Int32 nCols = pTable->GetTabLines().GetObject(0)->GetTabBoxes().Count();
            std::vector< std::vector< sal_Char > > aMap( nRows );
            for (sal_Int32 i = 0;  i < nRows;  ++i)
                aMap[i].resize( nCols );

            // iterate over subranges and mark used cells in above map
            //!! by proceeding this way we automatically get rid of
            //!! multiple listed or overlapping cell ranges which should
            //!! just be ignored silently
            sal_Int32 nSubRanges = aSubRanges.getLength();
            for (sal_Int32 i = 0;  i < nSubRanges;  ++i)
            {
                String aTblName, aStartCell, aEndCell;
                sal_Bool bOk = GetTableAndCellsFromRangeRep(
                                    pSubRanges[i], aTblName, aStartCell, aEndCell );
                DBG_ASSERT( bOk, "failed to get table and start/end cells" );

                sal_Int32 nStartRow, nStartCol, nEndRow, nEndCol;
                lcl_GetCellPosition( aStartCell, nStartCol, nStartRow );
                lcl_GetCellPosition( aEndCell,   nEndCol,   nEndRow );
                DBG_ASSERT( nStartRow <= nEndRow && nStartCol <= nEndCol,
                        "cell range not normalized");

                // test if the ranges span more than the available cells
                if( nStartRow < 0 || nEndRow >= nRows ||
                    nStartCol < 0 || nEndCol >= nCols )
                {
                    throw lang::IllegalArgumentException();
                }
                for (sal_Int32 k1 = nStartRow;  k1 <= nEndRow;  ++k1)
                {
                    for (sal_Int32 k2 = nStartCol;  k2 <= nEndCol;  ++k2)
                        aMap[k1][k2] = 'x';
                }
            }

            //
            // find label and data sequences to use
            //
            sal_Int32 oi;  // outer index (slower changing index)
            sal_Int32 ii;  // inner index (faster changing index)
            sal_Int32 oiEnd = bDtaSrcIsColumns ? nCols : nRows;
            sal_Int32 iiEnd = bDtaSrcIsColumns ? nRows : nCols;
            std::vector< sal_Int32 > aLabelIdx( oiEnd );
            std::vector< sal_Int32 > aDataStartIdx( oiEnd );
            std::vector< sal_Int32 > aDataLen( oiEnd );
            for (oi = 0;  oi < oiEnd;  ++oi)
            {
                aLabelIdx[oi]       = -1;
                aDataStartIdx[oi]   = -1;
                aDataLen[oi]        = 0;
            }
            //
            for (oi = 0;  oi < oiEnd;  ++oi)
            {
                ii = 0;
                while (ii < iiEnd)
                {
                    sal_Char &rChar = bDtaSrcIsColumns ? aMap[ii][oi] : aMap[oi][ii];

                    // label should be used but is not yet found?
                    if (rChar == 'x' && bFirstIsLabel && aLabelIdx[oi] == -1)
                    {
                        aLabelIdx[oi] = ii;
                        rChar = 'L';    // setting a different char for labels here
                                        // makes the test for the data sequence below
                                        // easier
                    }

                    // find data sequence
                    if (rChar == 'x' && aDataStartIdx[oi] == -1)
                    {
                        aDataStartIdx[oi] = ii;

                        // get length of data sequence
                        sal_Int32 nL = 0;
                        sal_Char c;
                        while (ii< iiEnd && 'x' == (c = bDtaSrcIsColumns ? aMap[ii][oi] : aMap[oi][ii]))
                        {
                            ++nL;   ++ii;
                        }
                        aDataLen[oi] = nL;

                        // check that there is no other seperate sequence of data
                        // to be found because that is not supported
                        while (ii < iiEnd)
                        {
                            if ('x' == (c = bDtaSrcIsColumns ? aMap[ii][oi] : aMap[oi][ii]))
                                throw lang::IllegalArgumentException();
                            ++ii;
                        }
                    }
                    else
                        ++ii;
                }
            }

            // make some other consistency checks while calculating
            // the number of XLabeledDataSequence to build:
            // - labels should always be used or not at all
            // - the data sequences should have equal non-zero length
            sal_Int32 nNumLDS = 0;
            if (oiEnd > 0)
            {
                sal_Int32 nFirstSeqLen = 0;
                sal_Int32 nFirstSeqLabelIdx = -1;
                for (oi = 0;  oi < oiEnd;  ++oi)
                {
                    sal_Bool bFirstFound = sal_False;
                    // row/col used at all?
                    if (aDataStartIdx[oi] != -1 &&
                        (!bFirstIsLabel || aLabelIdx[oi] != -1))
                    {
                        ++nNumLDS;
                        if (!bFirstFound)
                        {
                            nFirstSeqLen        = aDataLen[oi];
                            nFirstSeqLabelIdx   = aLabelIdx[oi];
                            bFirstFound = sal_True;
                        }
                        else
                        {
                            if (nFirstSeqLen != aDataLen[oi] ||
                                nFirstSeqLabelIdx != aLabelIdx[oi])
                                throw lang::IllegalArgumentException();
                        }
                    }
                }
            }
            if (nNumLDS == 0)
                throw lang::IllegalArgumentException();

            // now we should have all necessary data to build a proper DataSource
            // thus if we came this far there should be no further problem
            if (bTestOnly)
                return xRes;    // have createDataSourcePossible return true

            // create data source from found label and data sequences
            uno::Sequence< uno::Reference< chart2::data::XDataSequence > > aLabelSeqs( nNumLDS );
            uno::Reference< chart2::data::XDataSequence > *pLabelSeqs = aLabelSeqs.getArray();
            uno::Sequence< uno::Reference< chart2::data::XDataSequence > > aDataSeqs( nNumLDS );
            uno::Reference< chart2::data::XDataSequence > *pDataSeqs = aDataSeqs.getArray();
            sal_Int32 nSeqsIdx = 0;
            for (oi = 0;  oi < oiEnd;  ++oi)
            {
                // row/col not used? (see if-statement above where nNumLDS was counted)
                if (!(aDataStartIdx[oi] != -1 &&
                        (!bFirstIsLabel || aLabelIdx[oi] != -1)))
                    continue;

                // get cell ranges for label and data
                //
                SwRangeDescriptor aLabelDesc;
                SwRangeDescriptor aDataDesc;
                if (bDtaSrcIsColumns)   // use columns
                {
                    aLabelDesc.nTop     = aLabelIdx[oi];
                    aLabelDesc.nLeft    = oi;
                    aLabelDesc.nBottom  = aLabelDesc.nTop;
                    aLabelDesc.nRight   = oi;

                    aDataDesc.nTop      = aDataStartIdx[oi];
                    aDataDesc.nLeft     = oi;
                    aDataDesc.nBottom   = aDataDesc.nTop + aDataLen[oi] - 1;
                    aDataDesc.nRight    = oi;
                }
                else    // use rows
                {
                    aLabelDesc.nTop     = oi;
                    aLabelDesc.nLeft    = aLabelIdx[oi];
                    aLabelDesc.nBottom  = oi;
                    aLabelDesc.nRight   = aLabelDesc.nLeft;

                    aDataDesc.nTop      = oi;
                    aDataDesc.nLeft     = aDataStartIdx[oi];
                    aDataDesc.nBottom   = oi;
                    aDataDesc.nRight    = aDataDesc.nLeft + aDataLen[oi] - 1;
                }
                String aBaseName( pTblFmt->GetName() );
                aBaseName += '.';
                //
                String aLabelRange;
                if (aLabelIdx[oi] != -1)
                {
                    aLabelRange += aBaseName;
                    aLabelRange += lcl_GetCellName( aLabelDesc.nLeft, aLabelDesc.nTop );
                    aLabelRange += ':';
                    aLabelRange += lcl_GetCellName( aLabelDesc.nRight, aLabelDesc.nBottom );
                }
                //
                String aDataRange;
                if (aDataStartIdx[oi] != -1)
                {
                    aDataRange += aBaseName;
                    aDataRange += lcl_GetCellName( aDataDesc.nLeft, aDataDesc.nTop );
                    aDataRange += ':';
                    aDataRange += lcl_GetCellName( aDataDesc.nRight, aDataDesc.nBottom );
                }

                // get cursors spanning the cell ranges for label and data
                SwUnoCrsr   *pLabelUnoCrsr  = 0;
                SwUnoCrsr   *pDataUnoCrsr   = 0;
                GetFormatAndCreateCursorFromRangeRep( pDoc, aLabelRange, &pTblFmt, &pLabelUnoCrsr);
                GetFormatAndCreateCursorFromRangeRep( pDoc, aDataRange,  &pTblFmt, &pDataUnoCrsr);

                // create XDataSequence's from cursors
                if (pLabelUnoCrsr)
                    pLabelSeqs[ nSeqsIdx ] = new SwChartDataSequence( *this, *pTblFmt, pLabelUnoCrsr );
                DBG_ASSERT( pDataUnoCrsr, "pointer to data sequence missing" );
                if (pDataUnoCrsr)
                    pDataSeqs [ nSeqsIdx ] = new SwChartDataSequence( *this, *pTblFmt, pDataUnoCrsr );
                if (pLabelUnoCrsr || pDataUnoCrsr)
                    ++nSeqsIdx;
            }
            DBG_ASSERT( nSeqsIdx == nNumLDS,
                    "mismatch between sequence size and num,ber of entries" );

            // build data source from data and label sequences
            uno::Sequence< uno::Reference< chart2::data::XLabeledDataSequence > > aLDS( nNumLDS );
            uno::Reference< chart2::data::XLabeledDataSequence > *pLDS = aLDS.getArray();
            for (sal_Int32 i = 0;  i < nNumLDS;  ++i)
            {
                SwChartLabeledDataSequence *pLabeledDtaSeq = new SwChartLabeledDataSequence;
                pLabeledDtaSeq->setLabel( pLabelSeqs[i] );
                pLabeledDtaSeq->setValues( pDataSeqs[i] );
                pLDS[i] = pLabeledDtaSeq;
            }

            // apply 'SequenceMapping' if it was provided
            sal_Int32 nSequenceMappingLen = aSequenceMapping.getLength();
            if (nSequenceMappingLen)
            {
                sal_Int32 *pSequenceMapping = aSequenceMapping.getArray();
                uno::Sequence< uno::Reference< chart2::data::XLabeledDataSequence > > aOld_LDS( aLDS );
                uno::Reference< chart2::data::XLabeledDataSequence > *pOld_LDS = aOld_LDS.getArray();

                sal_Int32 nNewCnt = 0;
                for (sal_Int32 i = 0;  i < nSequenceMappingLen;  ++i)
                {
                    // check that index to be used is valid
                    // and has not yet been used
                    sal_Int32 nIdx = pSequenceMapping[i];
                    if (0 <= nIdx && nIdx < nNumLDS && pOld_LDS[nIdx].is())
                    {
                        pLDS[nNewCnt++] = pOld_LDS[nIdx];

                        // mark index as being used already (avoids duplicate entries)
                        pOld_LDS[nIdx].clear();
                    }
                }
                // add not yet used 'old' sequences to new one
                for (sal_Int32 i = 0;  i < nNumLDS;  ++i)
                {
#if OSL_DEBUG_LEVEL > 1
                        if (!pOld_LDS[i].is())
                            i = i;
#endif
                    if (pOld_LDS[i].is())
                        pLDS[nNewCnt++] = pOld_LDS[i];
                }
                DBG_ASSERT( nNewCnt == nNumLDS, "unexpected size of resulting sequence" );
            }

            xRes = new SwChartDataSource( aLDS );
        }
    }

    return xRes;
}

sal_Bool SAL_CALL SwChartDataProvider::createDataSourcePossible(
        const uno::Sequence< beans::PropertyValue >& rArguments )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    sal_Bool bPossible = sal_True;
    try
    {
        Impl_createDataSource( rArguments, sal_True );
    }
    catch (lang::IllegalArgumentException &)
    {
        bPossible = sal_False;
    }

    return bPossible;
}

uno::Reference< chart2::data::XDataSource > SAL_CALL SwChartDataProvider::createDataSource(
        const uno::Sequence< beans::PropertyValue >& rArguments )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    return Impl_createDataSource( rArguments );
}

////////////////////////////////////////////////////////////
// SwChartDataProvider::GetBrokenCellRangeForExport
//
// fix for #i79009
// we need to return a property that has the same value as the property
// 'CellRangeRepresentation' but for all rows which are increased by one.
// E.g. Table1:A1:D5 -> Table1:A2:D6
// Since the problem is only for old charts which did not support multiple
// we do not need to provide that property/string if the 'CellRangeRepresentation'
// contains multiple ranges.
OUString SwChartDataProvider::GetBrokenCellRangeForExport(
    const OUString &rCellRangeRepresentation )
{
    OUString aRes;

    // check that we do not have multiple ranges
    if (-1 == rCellRangeRepresentation.indexOf( ';' ))
    {
        // get current cell and table names
        String aTblName, aStartCell, aEndCell;
        GetTableAndCellsFromRangeRep( rCellRangeRepresentation,
            aTblName, aStartCell, aEndCell, sal_False );
        sal_Int32 nStartCol = -1, nStartRow = -1, nEndCol = -1, nEndRow = -1;
        lcl_GetCellPosition( aStartCell, nStartCol, nStartRow );
        lcl_GetCellPosition( aEndCell, nEndCol, nEndRow );

        // get new cell names
        ++nStartRow;
        ++nEndRow;
        aStartCell = lcl_GetCellName( nStartCol, nStartRow );
        aEndCell   = lcl_GetCellName( nEndCol, nEndRow );

        aRes = GetRangeRepFromTableAndCells( aTblName,
                aStartCell, aEndCell, sal_False );
    }

    return aRes;
}

uno::Sequence< beans::PropertyValue > SAL_CALL SwChartDataProvider::detectArguments(
        const uno::Reference< chart2::data::XDataSource >& xDataSource )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Sequence< beans::PropertyValue > aResult;
    if (!xDataSource.is())
        return aResult;

    const uno::Sequence< uno::Reference< chart2::data::XLabeledDataSequence > > aDS_LDS( xDataSource->getDataSequences() );
    const uno::Reference< chart2::data::XLabeledDataSequence > *pDS_LDS = aDS_LDS.getConstArray();
    sal_Int32 nNumDS_LDS = aDS_LDS.getLength();

    DBG_ASSERT( nNumDS_LDS != 0, "data source contains no XLabeledDataSequence" );
    if (nNumDS_LDS == 0)
        return aResult;

    SwFrmFmt *pTableFmt = 0;
    SwTable  *pTable    = 0;
    String    aTableName;
    sal_Int32 nTableRows = 0;
    sal_Int32 nTableCols = 0;

    // data used to build 'CellRangeRepresentation' from later on
    std::vector< std::vector< sal_Char > > aMap;

    uno::Sequence< sal_Int32 > aSequenceMapping( nNumDS_LDS );
    sal_Int32 *pSequenceMapping = aSequenceMapping.getArray();

    String aCellRanges;
    sal_Int16 nDtaSrcIsColumns = -1;// -1: don't know yet, 0: false, 1: true  -2: neither
    sal_Int32 nLabelSeqLen  = -1;   // used to see if labels are always used or not and have
                                    // the expected size of 1 (i.e. if FirstCellAsLabel can
                                    // be determined)
                                    // -1: don't know yet, 0: not used, 1: always a single labe cell, ...
                                    // -2: neither/failed
//     sal_Int32 nValuesSeqLen = -1;   // used to see if all value sequences have the same size
    for (sal_Int32 i = 0;  i < nNumDS_LDS;  ++i)
    {
        uno::Reference< chart2::data::XLabeledDataSequence > xLabeledDataSequence( pDS_LDS[i] );
        if( !xLabeledDataSequence.is() )
        {
            DBG_ERROR("got NULL for XLabeledDataSequence from Data source");
            continue;
        }
        const uno::Reference< chart2::data::XDataSequence > xCurLabel( xLabeledDataSequence->getLabel(), uno::UNO_QUERY );
        const uno::Reference< chart2::data::XDataSequence > xCurValues( xLabeledDataSequence->getValues(), uno::UNO_QUERY );

        // get sequence lengths for label and values.
        // (0 length is Ok)
        sal_Int32 nCurLabelSeqLen   = -1;
        sal_Int32 nCurValuesSeqLen  = -1;
        if (xCurLabel.is())
            nCurLabelSeqLen = xCurLabel->getData().getLength();
        if (xCurValues.is())
            nCurValuesSeqLen = xCurValues->getData().getLength();

        // check for consistent use of 'first cell as label'
        if (nLabelSeqLen == -1)		// set initial value to compare with below further on
            nLabelSeqLen = nCurLabelSeqLen;
        if (nLabelSeqLen != nCurLabelSeqLen)
            nLabelSeqLen = -2;	// failed / no consistent use of label cells

        // get table and cell names for label and values data sequences
        // (start and end cell will be sorted, i.e. start cell <= end cell)
        String aLabelTblName, aLabelStartCell, aLabelEndCell;
        String aValuesTblName, aValuesStartCell, aValuesEndCell;
        String aLabelRange, aValuesRange;
        if (xCurLabel.is())
            aLabelRange = xCurLabel->getSourceRangeRepresentation();
        if (xCurValues.is())
            aValuesRange = xCurValues->getSourceRangeRepresentation();
        if ((aLabelRange.Len() && !GetTableAndCellsFromRangeRep( aLabelRange,
                aLabelTblName, aLabelStartCell, aLabelEndCell ))  ||
            !GetTableAndCellsFromRangeRep( aValuesRange,
                aValuesTblName, aValuesStartCell, aValuesEndCell ))
        {
            return aResult; // failed -> return empty property sequence
        }

        // make sure all sequences use the same table
        if (!aTableName.Len())
            aTableName = aValuesTblName;  // get initial value to compare with
        if (!aTableName.Len() ||
             aTableName != aValuesTblName ||
            (aLabelTblName.Len() && aTableName != aLabelTblName))
        {
            return aResult; // failed -> return empty property sequence
        }


        // try to get 'DataRowSource' value (ROWS or COLUMNS) from inspecting
        // first and last cell used in both sequences
        //
        sal_Int32 nFirstCol, nFirstRow, nLastCol, nLastRow;
        String aCell( aLabelStartCell.Len() ? aLabelStartCell : aValuesStartCell );
        DBG_ASSERT( aCell.Len() , "start cell missing?" );
        lcl_GetCellPosition( aCell, nFirstCol, nFirstRow);
        lcl_GetCellPosition( aValuesEndCell, nLastCol, nLastRow);
        //
        sal_Int16 nDirection = -1;  // -1: not yet set,  0: columns,  1: rows, -2: failed
        if (nFirstCol == nLastCol && nFirstRow == nLastRow) // a single cell...
        {
            DBG_ASSERT( nCurLabelSeqLen == 0 && nCurValuesSeqLen == 1,
                    "trying to determine 'DataRowSource': something's fishy... should have been a single cell");
            nDirection = 0;     // default direction for a single cell should be 'columns'
        }
        else    // more than one cell is availabale (in values and label together!)
        {
            if (nFirstCol == nLastCol && nFirstRow != nLastRow)
                nDirection = 1;
            else if (nFirstCol != nLastCol && nFirstRow == nLastRow)
                nDirection = 0;
            else
            {
                DBG_ERROR( "trying to determine 'DataRowSource': unexpected case found" );
                nDirection = -2;
            }
        }
        // check for consistent direction of data source
        if (nDtaSrcIsColumns == -1)     // set initial value to compare with below
            nDtaSrcIsColumns = nDirection;
        if (nDtaSrcIsColumns != nDirection)
        {
            nDtaSrcIsColumns = -2;	// failed
        }


        if (nDtaSrcIsColumns == 0 || nDtaSrcIsColumns == 1)
        {
            // build data to obtain 'SequenceMapping' later on
            //
            DBG_ASSERT( nDtaSrcIsColumns == 0  ||   /* rows */
                        nDtaSrcIsColumns == 1,      /* columns */
                    "unexpected value for 'nDtaSrcIsColumns'" );
            pSequenceMapping[i] = nDtaSrcIsColumns ? nFirstCol : nFirstRow;


            // build data used to determine 'CellRangeRepresentation' later on
            //
            GetTableByName( *pDoc, aTableName, &pTableFmt, &pTable );
            if (!pTable || pTable->IsTblComplex())
                return aResult; // failed -> return empty property sequence
            nTableRows = pTable->GetTabLines().Count();
            nTableCols = pTable->GetTabLines().GetObject(0)->GetTabBoxes().Count();
            aMap.resize( nTableRows );
            for (sal_Int32 i = 0;  i < nTableRows;  ++i)
                aMap[i].resize( nTableCols );
            //
            if (aLabelStartCell.Len() && aLabelEndCell.Len())
            {
                sal_Int32 nStartCol, nStartRow, nEndCol, nEndRow;
                lcl_GetCellPosition( aLabelStartCell, nStartCol, nStartRow );
                lcl_GetCellPosition( aLabelEndCell,   nEndCol,   nEndRow );
                if (nStartRow < 0 || nEndRow >= nTableRows ||
                    nStartCol < 0 || nEndCol >= nTableCols)
                {
                    return aResult; // failed -> return empty property sequence
                }
                for (sal_Int32 i = nStartRow;  i <= nEndRow;  ++i)
                {
                    for (sal_Int32 k = nStartCol;  k <= nEndCol;  ++k)
                    {
                        sal_Char &rChar = aMap[i][k];
                        if (rChar == '\0')   // check for overlapping values and/or labels
                            rChar = 'L';
                        else
                            return aResult; // failed -> return empty property sequence
                    }
                }
            }
            if (aValuesStartCell.Len() && aValuesEndCell.Len())
            {
                sal_Int32 nStartCol, nStartRow, nEndCol, nEndRow;
                lcl_GetCellPosition( aValuesStartCell, nStartCol, nStartRow );
                lcl_GetCellPosition( aValuesEndCell,   nEndCol,   nEndRow );
                if (nStartRow < 0 || nEndRow >= nTableRows ||
                    nStartCol < 0 || nEndCol >= nTableCols)
                {
                    return aResult; // failed -> return empty property sequence
                }
                for (sal_Int32 i = nStartRow;  i <= nEndRow;  ++i)
                {
                    for (sal_Int32 k = nStartCol;  k <= nEndCol;  ++k)
                    {
                        sal_Char &rChar = aMap[i][k];
                        if (rChar == '\0')   // check for overlapping values and/or labels
                            rChar = 'x';
                        else
                            return aResult; // failed -> return empty property sequence
                    }
                }
            }
        }

#if OSL_DEBUG_LEVEL > 1
        // do some extra sanity checking that the length of the sequences
        // matches their range representation
        {
            sal_Int32 nStartRow, nStartCol, nEndRow, nEndCol;
            if (xCurLabel.is())
            {
                lcl_GetCellPosition( aLabelStartCell, nStartCol, nStartRow);
                lcl_GetCellPosition( aLabelEndCell,   nEndCol,   nEndRow);
                DBG_ASSERT( (nStartCol == nEndCol && (nEndRow - nStartRow + 1) == xCurLabel->getData().getLength()) ||
                            (nStartRow == nEndRow && (nEndCol - nStartCol + 1) == xCurLabel->getData().getLength()),
                        "label sequence length does not match range representation!" );
            }
            if (xCurValues.is())
            {
                lcl_GetCellPosition( aValuesStartCell, nStartCol, nStartRow);
                lcl_GetCellPosition( aValuesEndCell,   nEndCol,   nEndRow);
                DBG_ASSERT( (nStartCol == nEndCol && (nEndRow - nStartRow + 1) == xCurValues->getData().getLength()) ||
                            (nStartRow == nEndRow && (nEndCol - nStartCol + 1) == xCurValues->getData().getLength()),
                        "value sequence length does not match range representation!" );
            }
        }
#endif
    } // for


    // build value for 'CellRangeRepresentation'
    //
    String aCellRangeBase( aTableName );
    aCellRangeBase += '.';
    String aCurRange;
    for (sal_Int32 i = 0;  i < nTableRows;  ++i)
    {
        for (sal_Int32 k = 0;  k < nTableCols;  ++k)
        {
            if (aMap[i][k] != '\0')  // top-left cell of a sub-range found
            {
                // find rectangular sub-range to use
                sal_Int32 ri = i;   // row index
                sal_Int32 ci = k;   // column index
                sal_Int32 nRowSubLen = 0;
                sal_Int32 nColSubLen = 0;
                while (ri < nTableRows && aMap[ri++][k] != '\0')
                    ++nRowSubLen;
                // be aware of shifted sequences!
                // (according to the checks done prior the length should be ok)
                while (ci < nTableCols && aMap[i][ci] != '\0'
                                       && aMap[i + nRowSubLen-1][ci] != '\0')
                {
                    ++ci;
                    ++nColSubLen;
                }
                String aStartCell( lcl_GetCellName( k, i ) );
                String aEndCell( lcl_GetCellName( k + nColSubLen - 1, i + nRowSubLen - 1) );
                aCurRange = aCellRangeBase;
                aCurRange += aStartCell;
                aCurRange += ':';
                aCurRange += aEndCell;
                if (aCellRanges.Len())
                    aCellRanges += ';';
                aCellRanges += aCurRange;

                // clear already found sub-range from map
                for (sal_Int32 ri = 0;  ri < nRowSubLen;  ++ri)
                    for (sal_Int32 ci = 0;  ci < nColSubLen;  ++ci)
                        aMap[i + ri][k + ci] = '\0';
            }
        }
    }
    // to be nice to the user we now sort the cell ranges according to
    // rows or columns depending on the direction used in the data source
    uno::Sequence< OUString > aSortedRanges;
    GetSubranges( aCellRanges, aSortedRanges, sal_False /*sub ranges should already be normalized*/ );
    SortSubranges( aSortedRanges, (nDtaSrcIsColumns == 1) );
    sal_Int32 nSortedRanges = aSortedRanges.getLength();
    const OUString *pSortedRanges = aSortedRanges.getConstArray();
    OUString aSortedCellRanges;
    for (sal_Int32 i = 0;  i < nSortedRanges;  ++i)
    {
        if (aSortedCellRanges.getLength())
            aSortedCellRanges += OUString::valueOf( (sal_Unicode) ';');
        aSortedCellRanges += pSortedRanges[i];
    }


    // build value for 'SequenceMapping'
    //
    uno::Sequence< sal_Int32 > aSortedMapping( aSequenceMapping );
    sal_Int32 *pSortedMapping = aSortedMapping.getArray();
    std::sort( pSortedMapping, pSortedMapping + aSortedMapping.getLength() );
    DBG_ASSERT( aSortedMapping.getLength() == nNumDS_LDS, "unexpected size of sequence" );
    sal_Bool bNeedSequenceMapping = sal_False;
    for (sal_Int32 i = 0;  i < nNumDS_LDS;  ++i)
    {
        sal_Int32 *pIt = std::find( pSortedMapping, pSortedMapping + nNumDS_LDS,
                                    pSequenceMapping[i] );
        DBG_ASSERT( pIt, "index not found" );
        if (!pIt)
            return aResult; // failed -> return empty property sequence
        pSequenceMapping[i] = pIt - pSortedMapping;
        
        if (i != pSequenceMapping[i])
            bNeedSequenceMapping = sal_True;
    }

    // check if 'SequenceMapping' is actually not required...
    // (don't write unnecessary properties to the XML file)
    if (!bNeedSequenceMapping)
        aSequenceMapping.realloc(0);


#ifdef TL_NOT_USED  // in the end chart2 did not want to have the sequence minimized
    // try to shorten the 'SequenceMapping' as much as possible
    sal_Int32 k;
    for (k = nNumDS_LDS - 1;  k >= 0;  --k)
    {
        if (pSequenceMapping[k] != k)
            break;
    }
    aSequenceMapping.realloc( k + 1 );
#endif


    //
    // build resulting properties
    //
    DBG_ASSERT(nLabelSeqLen >= 0 || nLabelSeqLen == -2 /*not used*/,
            "unexpected value for 'nLabelSeqLen'" );
    sal_Bool bFirstCellIsLabel = sal_False;     // default value if 'nLabelSeqLen' could not properly determined
    if (nLabelSeqLen > 0) // == 0 means no label sequence in use
        bFirstCellIsLabel = sal_True;
    //
    DBG_ASSERT( aSortedCellRanges.getLength(), "CellRangeRepresentation missing" );
    OUString aBrokenCellRangeForExport( GetBrokenCellRangeForExport( aSortedCellRanges ) );
    //
    aResult.realloc(5);
    sal_Int32 nProps = 0;
    aResult[nProps  ].Name = C2U("FirstCellAsLabel");
    aResult[nProps++].Value <<= bFirstCellIsLabel;
    aResult[nProps  ].Name = C2U("CellRangeRepresentation");
    aResult[nProps++].Value <<= aSortedCellRanges;
    if (0 != aBrokenCellRangeForExport.getLength())
    {
        aResult[nProps  ].Name = C2U("BrokenCellRangeForExport");
        aResult[nProps++].Value <<= aBrokenCellRangeForExport;
    }
    if (nDtaSrcIsColumns == 0 || nDtaSrcIsColumns == 1)
    {
        chart::ChartDataRowSource eDataRowSource = (nDtaSrcIsColumns == 1) ?
                    chart::ChartDataRowSource_COLUMNS : chart::ChartDataRowSource_ROWS;
        aResult[nProps  ].Name = C2U("DataRowSource");
        aResult[nProps++].Value <<= eDataRowSource;

        if (aSequenceMapping.getLength() != 0)
        {
            aResult[nProps  ].Name = C2U("SequenceMapping");
            aResult[nProps++].Value <<= aSequenceMapping;
        }
    }
    aResult.realloc( nProps );

    return aResult;
}

uno::Reference< chart2::data::XDataSequence > SwChartDataProvider::Impl_createDataSequenceByRangeRepresentation(
        const OUString& rRangeRepresentation, sal_Bool bTestOnly )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    if (bDisposed)
        throw lang::DisposedException();

    SwFrmFmt    *pTblFmt    = 0;    // pointer to table format
    SwUnoCrsr   *pUnoCrsr   = 0;    // pointer to new created cursor spanning the cell range
    GetFormatAndCreateCursorFromRangeRep( pDoc, rRangeRepresentation,
                                          &pTblFmt, &pUnoCrsr );
    if (!pTblFmt || !pUnoCrsr)
        throw lang::IllegalArgumentException();

    // check that cursors point and mark are in a single row or column.
    String aCellRange( GetCellRangeName( *pTblFmt, *pUnoCrsr ) );
    SwRangeDescriptor aDesc;
    FillRangeDescriptor( aDesc, aCellRange );
    if (aDesc.nTop != aDesc.nBottom  &&  aDesc.nLeft != aDesc.nRight)
        throw lang::IllegalArgumentException();

    DBG_ASSERT( pTblFmt && pUnoCrsr, "table format or cursor missing" );
    uno::Reference< chart2::data::XDataSequence > xDataSeq;
    if (!bTestOnly)
        xDataSeq = new SwChartDataSequence( *this, *pTblFmt, pUnoCrsr );

    return xDataSeq;
}

sal_Bool SAL_CALL SwChartDataProvider::createDataSequenceByRangeRepresentationPossible(
        const OUString& rRangeRepresentation )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );

    sal_Bool bPossible = sal_True;
    try
    {
        Impl_createDataSequenceByRangeRepresentation( rRangeRepresentation, sal_True );
    }
    catch (lang::IllegalArgumentException &)
    {
        bPossible = sal_False;
    }

    return bPossible;
}

uno::Reference< chart2::data::XDataSequence > SAL_CALL SwChartDataProvider::createDataSequenceByRangeRepresentation(
        const OUString& rRangeRepresentation )
    throw (lang::IllegalArgumentException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    return Impl_createDataSequenceByRangeRepresentation( rRangeRepresentation );
}


uno::Reference< sheet::XRangeSelection > SAL_CALL SwChartDataProvider::getRangeSelection(  )
    throw (uno::RuntimeException)
{
    // note: it is no error to return nothing here
    return uno::Reference< sheet::XRangeSelection >();
}


void SAL_CALL SwChartDataProvider::dispose(  )
    throw (uno::RuntimeException)
{
    sal_Bool bMustDispose( sal_False );
    {
        osl::MutexGuard  aGuard( GetChartMutex() );
        bMustDispose = !bDisposed;
        if (!bDisposed)
            bDisposed = sal_True;
    }
    if (bMustDispose)
    {
        // dispose all data-sequences
        Map_Set_DataSequenceRef_t::iterator aIt( aDataSequences.begin() );
        while (aIt != aDataSequences.end())
        {
            DisposeAllDataSequences( (*aIt).first );
            ++aIt;
        }
        // release all references to data-sequences
        aDataSequences.clear();

        // require listeners to release references to this object
        lang::EventObject aEvtObj( dynamic_cast< chart2::data::XDataSequence * >(this) );
        aEvtListeners.disposeAndClear( aEvtObj );
    }
}


void SAL_CALL SwChartDataProvider::addEventListener(
        const uno::Reference< lang::XEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aEvtListeners.addInterface( rxListener );
}


void SAL_CALL SwChartDataProvider::removeEventListener(
        const uno::Reference< lang::XEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aEvtListeners.removeInterface( rxListener );
}



OUString SAL_CALL SwChartDataProvider::getImplementationName(  )
    throw (uno::RuntimeException)
{
    return C2U("SwChartDataProvider");
}


sal_Bool SAL_CALL SwChartDataProvider::supportsService(
        const OUString& rServiceName )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    return rServiceName.equalsAscii( SN_DATA_PROVIDER );
}


uno::Sequence< OUString > SAL_CALL SwChartDataProvider::getSupportedServiceNames(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    uno::Sequence< OUString > aRes(1);
    aRes.getArray()[0] = C2U( SN_DATA_PROVIDER );
    return aRes;
}


void SwChartDataProvider::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    // actually this function should be superfluous (need to check later)
    ClientModify(this, pOld, pNew );
}


void SwChartDataProvider::AddDataSequence( const SwTable &rTable, uno::Reference< chart2::data::XDataSequence > &rxDataSequence )
{
    aDataSequences[ &rTable ].insert( rxDataSequence );
}


void SwChartDataProvider::RemoveDataSequence( const SwTable &rTable, uno::Reference< chart2::data::XDataSequence > &rxDataSequence )
{
    aDataSequences[ &rTable ].erase( rxDataSequence );
}


void SwChartDataProvider::InvalidateTable( const SwTable *pTable )
{
    DBG_ASSERT( pTable, "table pointer is NULL" );
    if (pTable)
    {
        if (!bDisposed)
           pTable->GetFrmFmt()->GetDoc()->GetChartControllerHelper().StartOrContinueLocking();

        const Set_DataSequenceRef_t &rSet = aDataSequences[ pTable ];
        Set_DataSequenceRef_t::iterator aIt( rSet.begin() );
        while (aIt != rSet.end())
        {
            uno::Reference< util::XModifiable > xRef( uno::Reference< chart2::data::XDataSequence >(*aIt), uno::UNO_QUERY );
            if (xRef.is())
            {
                // mark the sequence as 'dirty' and notify listeners
                xRef->setModified( sal_True );
            }
            ++aIt;
        }
    }
}


sal_Bool SwChartDataProvider::DeleteBox( const SwTable *pTable, const SwTableBox &rBox )
{
    sal_Bool bRes = sal_False;
    DBG_ASSERT( pTable, "table pointer is NULL" );
    if (pTable)
    {
        if (!bDisposed)
            pTable->GetFrmFmt()->GetDoc()->GetChartControllerHelper().StartOrContinueLocking();

        Set_DataSequenceRef_t &rSet = aDataSequences[ pTable ];

        // iterate over all data-sequences for that table...
        Set_DataSequenceRef_t::iterator aIt( rSet.begin() );
        Set_DataSequenceRef_t::iterator aEndIt( rSet.end() );
        Set_DataSequenceRef_t::iterator aDelIt;     // iterator used for deletion when appropriate
        while (aIt != aEndIt)
        {
            SwChartDataSequence *pDataSeq = 0;
            sal_Bool bNowEmpty = sal_False;

            // check if weak reference is still valid...
            uno::Reference< chart2::data::XDataSequence > xRef( uno::Reference< chart2::data::XDataSequence>(*aIt), uno::UNO_QUERY );
            if (xRef.is())
            {
                // then delete that table box (check if implementation cursor needs to be adjusted)
                pDataSeq = static_cast< SwChartDataSequence * >( xRef.get() );
                if (pDataSeq)
                {
#if OSL_DEBUG_LEVEL > 1
                    OUString aRangeStr( pDataSeq->getSourceRangeRepresentation() );
#endif
                    bNowEmpty = pDataSeq->DeleteBox( rBox );
                    if (bNowEmpty)
                        aDelIt = aIt;
                }
            }
            ++aIt;

            if (bNowEmpty)
            {
                rSet.erase( aDelIt );
                if (pDataSeq)
                    pDataSeq->dispose();    // the current way to tell chart that sth. got removed
            }
        }
    }
    return bRes;
}


void SwChartDataProvider::DisposeAllDataSequences( const SwTable *pTable )
{
    DBG_ASSERT( pTable, "table pointer is NULL" );
    if (pTable)
    {
        if (!bDisposed)
            pTable->GetFrmFmt()->GetDoc()->GetChartControllerHelper().StartOrContinueLocking();

        const Set_DataSequenceRef_t &rSet = aDataSequences[ pTable ];
        Set_DataSequenceRef_t::iterator aIt( rSet.begin() );
        Set_DataSequenceRef_t::iterator aEndIt( rSet.end() );
        while (aIt != aEndIt)
        {
            uno::Reference< lang::XComponent > xRef( uno::Reference< chart2::data::XDataSequence >(*aIt), uno::UNO_QUERY );
            if (xRef.is())
            {
                xRef->dispose();
            }
            ++aIt;
        }
    }
}


////////////////////////////////////////
// SwChartDataProvider::AddRowCols tries to notify charts of added columns
// or rows and extends the value sequence respectively (if possible).
// If those can be added to the end of existing value data-sequences those
// sequences get mofdified accordingly and will send a modification
// notification (calling 'setModified').
//
// Since this function is a work-around for non existent Writer core functionality
// (no arbitrary multi-selection in tables that can be used to define a
// data-sequence) this function will be somewhat unreliable.
// For example we will only try to adapt value sequences. For this we assume
// that a sequence of length 1 is a label sequence and those with length >= 2
// we presume to be value sequences. Also new cells can only be added in the
// direction the value sequence is already pointing (rows / cols) and at the
// start or end of the values data-sequence.
// Nothing needs to be done if the new cells are in between the table cursors
// point and mark since data-sequence are considered to consist of all cells
// between those.
// New rows/cols need to be added already to the table before calling
// this function.
//
void SwChartDataProvider::AddRowCols(
        const SwTable &rTable,
        const SwSelBoxes& rBoxes,
        USHORT nLines, BOOL bBehind )
{
    if (rTable.IsTblComplex())
        return;

    const USHORT nBoxes		= rBoxes.Count();
    if (nBoxes < 1 || nLines < 1)
        return;

    SwTableBox* pFirstBox	= *( rBoxes.GetData() + 0 );
    SwTableBox* pLastBox	= *( rBoxes.GetData() + nBoxes - 1 );

    bool bChanged = false; // no data-sequence changed yet...

    sal_Int32 nFirstCol = -1, nFirstRow = -1, nLastCol = -1, nLastRow = -1;
    if (pFirstBox && pLastBox)
    {
        lcl_GetCellPosition( pFirstBox->GetName(), nFirstCol, nFirstRow  );
        lcl_GetCellPosition( pLastBox->GetName(),  nLastCol,  nLastRow );

        bool bAddCols = false;  // default; also to be used if nBoxes == 1 :-/
        if (nFirstCol == nLastCol && nFirstRow != nLastRow)
            bAddCols = true;
        if (nFirstCol == nLastCol || nFirstRow == nLastRow)
        {
            //get range of indices in col/rows for new cells
            sal_Int32 nFirstNewCol = nFirstCol;
            sal_Int32 nLastNewCol  = nLastCol;
            sal_Int32 nFirstNewRow = bBehind ?  nFirstRow + 1 : nFirstRow - nLines;
            sal_Int32 nLastNewRow  = nFirstNewRow - 1 + nLines;
            if (bAddCols)
            {
                DBG_ASSERT( nFirstCol == nLastCol, "column indices seem broken" );
                nFirstNewCol = bBehind ?  nFirstCol + 1 : nFirstCol - nLines;
                nLastNewCol  = nFirstNewCol - 1 + nLines;
                nFirstNewRow = nFirstRow;
                nLastNewRow  = nLastRow;
            }

            // iterate over all data-sequences for the table
            const Set_DataSequenceRef_t &rSet = aDataSequences[ &rTable ];
            Set_DataSequenceRef_t::iterator aIt( rSet.begin() );
            while (aIt != rSet.end())
            {
                uno::Reference< chart2::data::XTextualDataSequence > xRef( uno::Reference< chart2::data::XDataSequence >(*aIt), uno::UNO_QUERY );
                if (xRef.is())
                {
                    const sal_Int32 nLen = xRef->getTextualData().getLength();
                    if (nLen > 1) // value data-sequence ?
                    {
                        SwChartDataSequence *pDataSeq = 0;
                        uno::Reference< lang::XUnoTunnel > xTunnel( xRef, uno::UNO_QUERY );
                        if(xTunnel.is())
                        {
                            pDataSeq = reinterpret_cast< SwChartDataSequence * >(
                                    sal::static_int_cast< sal_IntPtr >( xTunnel->getSomething( SwChartDataSequence::getUnoTunnelId() )));

                            if (pDataSeq)
                            {
                                SwRangeDescriptor aDesc;
                                pDataSeq->FillRangeDesc( aDesc );

                                chart::ChartDataRowSource eDRSource = chart::ChartDataRowSource_COLUMNS;
                                if (aDesc.nTop == aDesc.nBottom && aDesc.nLeft != aDesc.nRight)
                                    eDRSource = chart::ChartDataRowSource_ROWS;

                                if (!bAddCols && eDRSource == chart::ChartDataRowSource_COLUMNS)
                                {
                                    // add rows: extend affected columns by newly added row cells
                                    pDataSeq->ExtendTo( true, nFirstNewRow, nLines );
                                }
                                else if (bAddCols && eDRSource == chart::ChartDataRowSource_ROWS)
                                {
                                    // add cols: extend affected rows by newly added column cells
                                    pDataSeq->ExtendTo( false, nFirstNewCol, nLines );
                                }
                            }
                        }
                    }
                }
                ++aIt;
            }

        }
    }
}


// XRangeXMLConversion ---------------------------------------------------

rtl::OUString SAL_CALL SwChartDataProvider::convertRangeToXML( const rtl::OUString& rRangeRepresentation )
    throw ( uno::RuntimeException, lang::IllegalArgumentException )
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    String aRes;
    String aRangeRepresentation( rRangeRepresentation );

    // multiple ranges are delimeted by a ';' like in
    // "Table1.A1:A4;Table1.C2:C5" the same table must be used in all ranges!
    xub_StrLen nNumRanges = aRangeRepresentation.GetTokenCount( ';' );
    SwTable* pFirstFoundTable = 0;	// to check that only one table will be used
    for (USHORT i = 0;  i < nNumRanges;  ++i)
    {
        String aRange( aRangeRepresentation.GetToken(i, ';') );
        SwFrmFmt    *pTblFmt  = 0;      // pointer to table format
        // BM: For what should the check be necessary? for #i79009# it is required that NO check is done
//         SwUnoCrsr   *pUnoCrsr = 0;      // here required to check if the cells in the range do actually exist
//         std::auto_ptr< SwUnoCrsr > pAuto( pUnoCrsr );  // to end lifetime of object pointed to by pUnoCrsr
        GetFormatAndCreateCursorFromRangeRep( pDoc, aRange, &pTblFmt, NULL );
        if (!pTblFmt)
            throw lang::IllegalArgumentException();
//    if (!pUnoCrsr)
//        throw uno::RuntimeException();
        SwTable* pTable = SwTable::FindTable( pTblFmt );
        if  (pTable->IsTblComplex())
            throw uno::RuntimeException();

        // check that there is only one table used in all ranges
        if (!pFirstFoundTable)
            pFirstFoundTable = pTable;
        if (pTable != pFirstFoundTable)
            throw lang::IllegalArgumentException();

        String aTblName;
        String aStartCell;
        String aEndCell;
        if (!GetTableAndCellsFromRangeRep( aRange, aTblName, aStartCell, aEndCell ))
            throw lang::IllegalArgumentException();

        sal_Int32 nCol, nRow;
        lcl_GetCellPosition( aStartCell, nCol, nRow );
        if (nCol < 0 || nRow < 0)
            throw uno::RuntimeException();

        //!! following objects/functions are implemented in XMLRangeHelper.?xx
        //!! which is a copy of the respective file from chart2 !!
        XMLRangeHelper::CellRange aCellRange;
        aCellRange.aTableName = aTblName;
        aCellRange.aUpperLeft.nColumn   = nCol;
        aCellRange.aUpperLeft.nRow      = nRow;
        aCellRange.aUpperLeft.bIsEmpty  = false;
        if (aStartCell != aEndCell && aEndCell.Len() != 0)
        {
            lcl_GetCellPosition( aEndCell, nCol, nRow );
            if (nCol < 0 || nRow < 0)
                throw uno::RuntimeException();

            aCellRange.aLowerRight.nColumn   = nCol;
            aCellRange.aLowerRight.nRow      = nRow;
            aCellRange.aLowerRight.bIsEmpty  = false;
        }
        String aTmp( XMLRangeHelper::getXMLStringFromCellRange( aCellRange ) );
        if (aRes.Len())	// in case of multiple ranges add delimeter
            aRes.AppendAscii( " " );
        aRes += aTmp;
    }

    return aRes;
}

rtl::OUString SAL_CALL SwChartDataProvider::convertRangeFromXML( const rtl::OUString& rXMLRange )
    throw ( uno::RuntimeException, lang::IllegalArgumentException )
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    String aRes;
    String aXMLRange( rXMLRange );

    // multiple ranges are delimeted by a ' ' like in
    // "Table1.$A$1:.$A$4 Table1.$C$2:.$C$5" the same table must be used in all ranges!
    xub_StrLen nNumRanges = aXMLRange.GetTokenCount( ' ' );
    rtl::OUString aFirstFoundTable;	// to check that only one table will be used
    for (USHORT i = 0;  i < nNumRanges;  ++i)
    {
        String aRange( aXMLRange.GetToken(i, ' ') );

        //!! following objects and function are implemented in XMLRangeHelper.?xx
        //!! which is a copy of the respective file from chart2 !!
        XMLRangeHelper::CellRange aCellRange( XMLRangeHelper::getCellRangeFromXMLString( aRange ));

        // check that there is only one table used in all ranges
        if (aFirstFoundTable.getLength() == 0)
            aFirstFoundTable = aCellRange.aTableName;
        if (aCellRange.aTableName != aFirstFoundTable)
            throw lang::IllegalArgumentException();

        OUString aTmp( aCellRange.aTableName );
        aTmp += OUString::valueOf((sal_Unicode) '.');
        aTmp += lcl_GetCellName( aCellRange.aUpperLeft.nColumn,
                                 aCellRange.aUpperLeft.nRow );
        // does cell range consist of more than a single cell?
        if (!aCellRange.aLowerRight.bIsEmpty)
        {
            aTmp += OUString::valueOf((sal_Unicode) ':');
            aTmp += lcl_GetCellName( aCellRange.aLowerRight.nColumn,
                                     aCellRange.aLowerRight.nRow );
        }

        if (aRes.Len())	// in case of multiple ranges add delimeter
            aRes.AppendAscii( ";" );
        aRes += String(aTmp);
    }

    return aRes;
}


//////////////////////////////////////////////////////////////////////

SwChartDataSource::SwChartDataSource(
        const uno::Sequence< uno::Reference< chart2::data::XLabeledDataSequence > > &rLDS ) :
    aLDS( rLDS )
{
}


SwChartDataSource::~SwChartDataSource()
{
//    delete pTblCrsr;
}


uno::Sequence< uno::Reference< chart2::data::XLabeledDataSequence > > SAL_CALL SwChartDataSource::getDataSequences(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    return aLDS;
}


OUString SAL_CALL SwChartDataSource::getImplementationName(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    return C2U("SwChartDataSource");
}


sal_Bool SAL_CALL SwChartDataSource::supportsService(
        const OUString& rServiceName )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    return rServiceName.equalsAscii( SN_DATA_SOURCE );
}


uno::Sequence< OUString > SAL_CALL SwChartDataSource::getSupportedServiceNames(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    uno::Sequence< OUString > aRes(1);
    aRes.getArray()[0] = C2U( SN_DATA_SOURCE );
    return aRes;
}

//////////////////////////////////////////////////////////////////////

SwChartDataSequence::SwChartDataSequence(
        SwChartDataProvider &rProvider,
        SwFrmFmt   &rTblFmt,
        SwUnoCrsr  *pTableCursor ) :
    SwClient( &rTblFmt ),
    aEvtListeners( GetChartMutex() ),
    aModifyListeners( GetChartMutex() ),
    xDataProvider( &rProvider ),
    pDataProvider( &rProvider ),
    pTblCrsr( pTableCursor ),
    aCursorDepend( this, pTableCursor ),
    pMap( aSwMapProvider.GetPropertyMap( PROPERTY_MAP_CHART2_DATA_SEQUENCE ) ),
    aRowLabelText( SW_RES( STR_CHART2_ROW_LABEL_TEXT ) ),
    aColLabelText( SW_RES( STR_CHART2_COL_LABEL_TEXT ) )
{
    bDisposed = sal_False;

    acquire();
    try
    {
        const SwTable* pTable = SwTable::FindTable( &rTblFmt );
        if (pTable)
        {
            uno::Reference< chart2::data::XDataSequence > xRef( dynamic_cast< chart2::data::XDataSequence * >(this), uno::UNO_QUERY );
            pDataProvider->AddDataSequence( *pTable, xRef );
            pDataProvider->addEventListener( dynamic_cast< lang::XEventListener * >(this) );
        }
        else
            DBG_ERROR( "table missing" );
    }
    catch (uno::RuntimeException &)
    {
        throw;
    }
    catch (uno::Exception &)
    {
    }
    release();

#if OSL_DEBUG_LEVEL > 1
    OUString aRangeStr( getSourceRangeRepresentation() );

    // check if it can properly convert into a SwUnoTableCrsr
    // which is required for some functions
    SwUnoTableCrsr* pUnoTblCrsr = *pTblCrsr;
    if (!pUnoTblCrsr)
        pUnoTblCrsr = *pTblCrsr;
#endif
}


SwChartDataSequence::SwChartDataSequence( const SwChartDataSequence &rObj ) :
    SwClient( rObj.GetFrmFmt() ),
    aEvtListeners( GetChartMutex() ),
    aModifyListeners( GetChartMutex() ),
    aRole( rObj.aRole ),
    xDataProvider( rObj.pDataProvider ),
    pDataProvider( rObj.pDataProvider ),
    pTblCrsr( rObj.pTblCrsr->Clone() ),
    aCursorDepend( this, pTblCrsr ),
    pMap( rObj.pMap ),
    aRowLabelText( SW_RES(STR_CHART2_ROW_LABEL_TEXT) ),
    aColLabelText( SW_RES(STR_CHART2_COL_LABEL_TEXT) )
{
    bDisposed = sal_False;

    acquire();
    try
    {
        const SwTable* pTable = SwTable::FindTable( GetFrmFmt() );
        if (pTable)
        {
            uno::Reference< chart2::data::XDataSequence > xRef( dynamic_cast< chart2::data::XDataSequence * >(this), uno::UNO_QUERY );
            pDataProvider->AddDataSequence( *pTable, xRef );
            pDataProvider->addEventListener( dynamic_cast< lang::XEventListener * >(this) );
        }
        else
            DBG_ERROR( "table missing" );
    }
    catch (uno::RuntimeException &)
    {
        throw;
    }
    catch (uno::Exception &)
    {
    }
    release();

#if OSL_DEBUG_LEVEL > 1
    OUString aRangeStr( getSourceRangeRepresentation() );

    // check if it can properly convert into a SwUnoTableCrsr
    // which is required for some functions
    SwUnoTableCrsr* pUnoTblCrsr = *pTblCrsr;
    if (!pUnoTblCrsr)
        pUnoTblCrsr = *pTblCrsr;
#endif
}


SwChartDataSequence::~SwChartDataSequence()
{
    // since the data-provider holds only weak references to the data-sequence
    // there should be no need here to release them explicitly...

    delete pTblCrsr;
}


const uno::Sequence< sal_Int8 > & SwChartDataSequence::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}


sal_Int64 SAL_CALL SwChartDataSequence::getSomething( const uno::Sequence< sal_Int8 > &rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
        return (sal_Int64)this;
    }
    return 0;
}


uno::Sequence< uno::Any > SAL_CALL SwChartDataSequence::getData(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Sequence< uno::Any > aRes;
    SwFrmFmt* pTblFmt = GetFrmFmt();
    if(pTblFmt)
    {
        SwTable* pTable = SwTable::FindTable( pTblFmt );
        if(!pTable->IsTblComplex())
        {
            SwRangeDescriptor aDesc;
            if (FillRangeDescriptor( aDesc, GetCellRangeName( *pTblFmt, *pTblCrsr ) ))
            {
                //!! make copy of pTblCrsr (SwUnoCrsr )
                // keep original cursor and make copy of it that gets handed
                // over to the SwXCellRange object which takes ownership and
                // thus will destroy the copy later.
                SwXCellRange aRange( pTblCrsr->Clone(), *pTblFmt, aDesc );
                aRange.GetDataSequence( &aRes, 0, 0 );
            }
        }
    }
    return aRes;
}


OUString SAL_CALL SwChartDataSequence::getSourceRangeRepresentation(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    String aRes;
    SwFrmFmt* pTblFmt = GetFrmFmt();
    if (pTblFmt)
    {
        aRes = pTblFmt->GetName();
        String aCellRange( GetCellRangeName( *pTblFmt, *pTblCrsr ) );
        DBG_ASSERT( aCellRange.Len() != 0, "failed to get cell range" );
        aRes += (sal_Unicode) '.';
        aRes += aCellRange;
    }
    return aRes;
}

uno::Sequence< OUString > SAL_CALL SwChartDataSequence::generateLabel(
        chart2::data::LabelOrigin eLabelOrigin )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Sequence< OUString > aLabels;

    {
        SwRangeDescriptor aDesc;
        sal_Bool bOk sal_False;
        SwFrmFmt* pTblFmt = GetFrmFmt();
        SwTable* pTable = pTblFmt ? SwTable::FindTable( pTblFmt ) : 0;
        if (!pTblFmt || !pTable || pTable->IsTblComplex())
            throw uno::RuntimeException();
        else
        {
            String aCellRange( GetCellRangeName( *pTblFmt, *pTblCrsr ) );
            DBG_ASSERT( aCellRange.Len() != 0, "failed to get cell range" );
            bOk = FillRangeDescriptor( aDesc, aCellRange );
            DBG_ASSERT( bOk, "falied to get SwRangeDescriptor" );
        }
        if (bOk)
        {
            aDesc.Normalize();
            sal_Int32 nColSpan = aDesc.nRight - aDesc.nLeft + 1;
            sal_Int32 nRowSpan = aDesc.nBottom - aDesc.nTop + 1;
            DBG_ASSERT( nColSpan == 1 || nRowSpan == 1,
                    "unexpected range of selected cells" );

            String aTxt;    // label text to be returned
            sal_Bool bReturnEmptyTxt = sal_False;
            sal_Bool bUseCol = sal_True;
            if (eLabelOrigin == chart2::data::LabelOrigin_COLUMN)
                bUseCol = sal_True;
            else if (eLabelOrigin == chart2::data::LabelOrigin_ROW)
                bUseCol = sal_False;
            else if (eLabelOrigin == chart2::data::LabelOrigin_SHORT_SIDE)
            {
                bUseCol = nColSpan < nRowSpan;
                bReturnEmptyTxt = nColSpan == nRowSpan;
            }
            else if (eLabelOrigin == chart2::data::LabelOrigin_LONG_SIDE)
            {
                bUseCol = nColSpan > nRowSpan;
                bReturnEmptyTxt = nColSpan == nRowSpan;
            }
            else
                DBG_ERROR( "unexpected case" );

            // build label sequence
            //
            sal_Int32 nSeqLen = bUseCol ? nColSpan : nRowSpan;
            aLabels.realloc( nSeqLen );
            OUString *pLabels = aLabels.getArray();
            for (sal_Int32 i = 0;  i < nSeqLen;  ++i)
            {
                if (!bReturnEmptyTxt)
                {
                    aTxt = bUseCol ? aColLabelText : aRowLabelText;
                    sal_Int32 nCol = aDesc.nLeft;
                    sal_Int32 nRow = aDesc.nTop;
                    if (bUseCol)
                        nCol = nCol + i;
                    else
                        nRow = nRow + i;
                    String aCellName( lcl_GetCellName( nCol, nRow ) );

                    xub_StrLen nLen = aCellName.Len();
                    if (nLen)
                    {
                        const sal_Unicode *pBuf = aCellName.GetBuffer();
                        const sal_Unicode *pEnd = pBuf + nLen;
                        while (pBuf < pEnd && !('0' <= *pBuf && *pBuf <= '9'))
                            ++pBuf;
                        // start of number found?
                        if (pBuf < pEnd && ('0' <= *pBuf && *pBuf <= '9'))
                        {
                            String aRplc;
                            String aNew;
                            if (bUseCol)
                            {
                                aRplc = String::CreateFromAscii( "%COLUMNLETTER" );
                                aNew = String( aCellName.GetBuffer(), static_cast<xub_StrLen>(pBuf - aCellName.GetBuffer()) );
                            }
                            else
                            {
                                aRplc = String::CreateFromAscii( "%ROWNUMBER" );
                                aNew = String( pBuf, static_cast<xub_StrLen>((aCellName.GetBuffer() + nLen) - pBuf) );
                            }
                            xub_StrLen nPos = aTxt.Search( aRplc );
                            if (nPos != STRING_NOTFOUND)
                                aTxt = aTxt.Replace( nPos, aRplc.Len(), aNew );
                        }
                    }
                }
                pLabels[i] = aTxt;
            }
        }
    }

    return aLabels;
}

::sal_Int32 SAL_CALL SwChartDataSequence::getNumberFormatKeyByIndex(
    ::sal_Int32 /*nIndex*/ )
    throw (lang::IndexOutOfBoundsException,
           uno::RuntimeException)
{
    return 0;
}



uno::Sequence< OUString > SAL_CALL SwChartDataSequence::getTextualData(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Sequence< OUString > aRes;
    SwFrmFmt* pTblFmt = GetFrmFmt();
    if(pTblFmt)
    {
        SwTable* pTable = SwTable::FindTable( pTblFmt );
        if(!pTable->IsTblComplex())
        {
            SwRangeDescriptor aDesc;
            if (FillRangeDescriptor( aDesc, GetCellRangeName( *pTblFmt, *pTblCrsr ) ))
            {
                //!! make copy of pTblCrsr (SwUnoCrsr )
                // keep original cursor and make copy of it that gets handed
                // over to the SwXCellRange object which takes ownership and
                // thus will destroy the copy later.
                SwXCellRange aRange( pTblCrsr->Clone(), *pTblFmt, aDesc );
                aRange.GetDataSequence( 0, &aRes, 0 );
            }
        }
    }
    return aRes;
}


uno::Sequence< double > SAL_CALL SwChartDataSequence::getNumericalData(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Sequence< double > aRes;
    SwFrmFmt* pTblFmt = GetFrmFmt();
    if(pTblFmt)
    {
        SwTable* pTable = SwTable::FindTable( pTblFmt );
        if(!pTable->IsTblComplex())
        {
            SwRangeDescriptor aDesc;
            if (FillRangeDescriptor( aDesc, GetCellRangeName( *pTblFmt, *pTblCrsr ) ))
            {
                //!! make copy of pTblCrsr (SwUnoCrsr )
                // keep original cursor and make copy of it that gets handed
                // over to the SwXCellRange object which takes ownership and
                // thus will destroy the copy later.
                SwXCellRange aRange( pTblCrsr->Clone(), *pTblFmt, aDesc );

                // get numerical values and make an effort to return the
                // numerical value for text formatted cells
                aRange.GetDataSequence( 0, 0, &aRes, sal_True );
            }
        }
    }
    return aRes;
}


uno::Reference< util::XCloneable > SAL_CALL SwChartDataSequence::createClone(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();
    return new SwChartDataSequence( *this );
}


uno::Reference< beans::XPropertySetInfo > SAL_CALL SwChartDataSequence::getPropertySetInfo(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    static uno::Reference< beans::XPropertySetInfo > xRes = new SfxItemPropertySetInfo( pMap );
    return xRes;
}


void SAL_CALL SwChartDataSequence::setPropertyValue(
        const OUString& rPropertyName,
        const uno::Any& rValue )
    throw (beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    if (rPropertyName.equalsAscii( SW_PROP_NAME_STR( UNO_NAME_ROLE )))
    {
        if ( !(rValue >>= aRole) )
            throw lang::IllegalArgumentException();
    }
    else
        throw beans::UnknownPropertyException();
}


uno::Any SAL_CALL SwChartDataSequence::getPropertyValue(
        const OUString& rPropertyName )
    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Any aRes;
    if (rPropertyName.equalsAscii( SW_PROP_NAME_STR( UNO_NAME_ROLE )))
        aRes <<= aRole;
    else
        throw beans::UnknownPropertyException();

    return aRes;
}


void SAL_CALL SwChartDataSequence::addPropertyChangeListener(
        const OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    //vos::OGuard aGuard( Application::GetSolarMutex() );
    DBG_ERROR( "not implemented" );
}


void SAL_CALL SwChartDataSequence::removePropertyChangeListener(
        const OUString& /*rPropertyName*/,
        const uno::Reference< beans::XPropertyChangeListener >& /*xListener*/ )
    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    //vos::OGuard aGuard( Application::GetSolarMutex() );
    DBG_ERROR( "not implemented" );
}


void SAL_CALL SwChartDataSequence::addVetoableChangeListener(
        const OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/ )
    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    //vos::OGuard aGuard( Application::GetSolarMutex() );
    DBG_ERROR( "not implemented" );
}


void SAL_CALL SwChartDataSequence::removeVetoableChangeListener(
        const OUString& /*rPropertyName*/,
        const uno::Reference< beans::XVetoableChangeListener >& /*xListener*/ )
    throw (beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    //vos::OGuard aGuard( Application::GetSolarMutex() );
    DBG_ERROR( "not implemented" );
}


OUString SAL_CALL SwChartDataSequence::getImplementationName(  )
    throw (uno::RuntimeException)
{
    return C2U("SwChartDataSequence");
}


sal_Bool SAL_CALL SwChartDataSequence::supportsService(
        const OUString& rServiceName )
    throw (uno::RuntimeException)
{
    return rServiceName.equalsAscii( SN_DATA_SEQUENCE );
}


uno::Sequence< OUString > SAL_CALL SwChartDataSequence::getSupportedServiceNames(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    uno::Sequence< OUString > aRes(1);
    aRes.getArray()[0] = C2U( SN_DATA_SEQUENCE );
    return aRes;
}


void SwChartDataSequence::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew );

    // table was deleted or cursor was deleted
    if(!GetRegisteredIn() || !aCursorDepend.GetRegisteredIn())
    {
        pTblCrsr = 0;
        dispose();
    }
    else
    {
        setModified( sal_True );
    }
}


sal_Bool SAL_CALL SwChartDataSequence::isModified(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    return sal_True;
}


void SAL_CALL SwChartDataSequence::setModified(
        ::sal_Bool bModified )
    throw (beans::PropertyVetoException, uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    if (bModified)
        LaunchModifiedEvent( aModifyListeners, dynamic_cast< XModifyBroadcaster * >(this) );
}


void SAL_CALL SwChartDataSequence::addModifyListener(
        const uno::Reference< util::XModifyListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aModifyListeners.addInterface( rxListener );
}


void SAL_CALL SwChartDataSequence::removeModifyListener(
        const uno::Reference< util::XModifyListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aModifyListeners.removeInterface( rxListener );
}


void SAL_CALL SwChartDataSequence::disposing( const lang::EventObject& rSource )
    throw (uno::RuntimeException)
{
    if (bDisposed)
        throw lang::DisposedException();
    if (rSource.Source == xDataProvider)
    {
        pDataProvider = 0;
        xDataProvider.clear();
    }
}


void SAL_CALL SwChartDataSequence::dispose(  )
    throw (uno::RuntimeException)
{
    sal_Bool bMustDispose( sal_False );
    {
        osl::MutexGuard  aGuard( GetChartMutex() );
        bMustDispose = !bDisposed;
        if (!bDisposed)
            bDisposed = sal_True;
    }
    if (bMustDispose)
    {
        bDisposed = sal_True;
        if (pDataProvider)
        {
            const SwTable* pTable = SwTable::FindTable( GetFrmFmt() );
            if (pTable)
            {
                uno::Reference< chart2::data::XDataSequence > xRef( dynamic_cast< chart2::data::XDataSequence * >(this), uno::UNO_QUERY );
                pDataProvider->RemoveDataSequence( *pTable, xRef );
            }
            else
                DBG_ERROR( "table missing" );
        }

        // require listeners to release references to this object
        lang::EventObject aEvtObj( dynamic_cast< chart2::data::XDataSequence * >(this) );
        aModifyListeners.disposeAndClear( aEvtObj );
        aEvtListeners.disposeAndClear( aEvtObj );
    }
}


void SAL_CALL SwChartDataSequence::addEventListener(
        const uno::Reference< lang::XEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aEvtListeners.addInterface( rxListener );
}


void SAL_CALL SwChartDataSequence::removeEventListener(
        const uno::Reference< lang::XEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aEvtListeners.removeInterface( rxListener );
}


sal_Bool SwChartDataSequence::DeleteBox( const SwTableBox &rBox )
{
#if OSL_DEBUG_LEVEL > 1
    String aBoxName( rBox.GetName() );
#endif

    // to be set if the last box of the data-sequence was removed here
    sal_Bool bNowEmpty = sal_False;

    // if the implementation cursor gets affected (i.e. thew box where it is located
    // in gets removed) we need to move it before that... (otherwise it does not need to change)
    //
    const SwStartNode* pPointStartNode = pTblCrsr->GetPoint()->nNode.GetNode().FindTableBoxStartNode();
    const SwStartNode* pMarkStartNode  = pTblCrsr->GetMark()->nNode.GetNode().FindTableBoxStartNode();
    //
    if (!pTblCrsr->HasMark() || (pPointStartNode == rBox.GetSttNd()  &&  pMarkStartNode == rBox.GetSttNd()))
    {
        bNowEmpty = sal_True;
    }
    else if (pPointStartNode == rBox.GetSttNd()  ||  pMarkStartNode == rBox.GetSttNd())
    {
        sal_Int32 nPointRow, nPointCol;
        sal_Int32 nMarkRow, nMarkCol;
        const SwTable* pTable = SwTable::FindTable( GetFrmFmt() );
        String aPointCellName( pTable->GetTblBox( pPointStartNode->GetIndex() )->GetName() );
        String aMarkCellName( pTable->GetTblBox( pMarkStartNode->GetIndex() )->GetName() );

        lcl_GetCellPosition( aPointCellName, nPointCol, nPointRow );
        lcl_GetCellPosition( aMarkCellName,  nMarkCol,  nMarkRow );
        DBG_ASSERT( nPointRow >= 0 && nPointCol >= 0, "invalid row and col" );
        DBG_ASSERT( nMarkRow >= 0 && nMarkCol >= 0, "invalid row and col" );

        // move vertical or horizontal?
        DBG_ASSERT( nPointRow == nMarkRow || nPointCol == nMarkCol,
                "row/col indices not matching" );
        DBG_ASSERT( nPointRow != nMarkRow || nPointCol != nMarkCol,
                "point and mark are identical" );
        sal_Bool bMoveVertical      = (nPointCol == nMarkCol);
        sal_Bool bMoveHorizontal    = (nPointRow == nMarkRow);

        // get movement direction
        sal_Bool bMoveLeft  = sal_False;    // move left or right?
        sal_Bool bMoveUp    = sal_False;    // move up or down?
        if (bMoveVertical)
        {
            if (pPointStartNode == rBox.GetSttNd()) // move point?
                bMoveUp = nPointRow > nMarkRow;
            else    // move mark
                bMoveUp = nMarkRow > nPointRow;
        }
        else if (bMoveHorizontal)
        {
            if (pPointStartNode == rBox.GetSttNd()) // move point?
                bMoveLeft = nPointCol > nMarkCol;
            else    // move mark
                bMoveLeft = nMarkCol > nPointCol;
        }
        else
            DBG_ERROR( "neither vertical nor horizontal movement" );

        // get new box (position) to use...
        sal_Int32 nRow = (pPointStartNode == rBox.GetSttNd()) ? nPointRow : nMarkRow;
        sal_Int32 nCol = (pPointStartNode == rBox.GetSttNd()) ? nPointCol : nMarkCol;
        if (bMoveVertical)
            nRow += bMoveUp ? -1 : +1;
        if (bMoveHorizontal)
            nCol += bMoveLeft ? -1 : +1;
        String aNewCellName = lcl_GetCellName( nCol, nRow );
        SwTableBox* pNewBox = (SwTableBox*) pTable->GetTblBox( aNewCellName );

        if (pNewBox)    // set new position (cell range) to use
        {
            // So erh�lt man den ersten Inhaltsnode in einer gegebenen Zelle:
            // Zun�chst einen SwNodeIndex auf den Node hinter dem SwStartNode der Box...
            SwNodeIndex aIdx( *pNewBox->GetSttNd(), +1 );
            // Dies kann ein SwCntntNode sein, kann aber auch ein Tabellen oder Sectionnode sein,
            // deshalb das GoNext;
            SwCntntNode *pCNd = aIdx.GetNode().GetCntntNode();
            if (!pCNd)
                pCNd = GetFrmFmt()->GetDoc()->GetNodes().GoNext( &aIdx );
            //und damit kann man z.B. eine SwPosition erzeugen:
            SwPosition aNewPos( *pCNd );   // new position to beused with cursor

            // if the mark is to be changed make sure there is one...
            if (pMarkStartNode == rBox.GetSttNd() && !pTblCrsr->HasMark())
                pTblCrsr->SetMark();

            // set cursor to new position...
            SwPosition *pPos = (pPointStartNode == rBox.GetSttNd()) ?
                        pTblCrsr->GetPoint() : pTblCrsr->GetMark();
            if (pPos)
            {
                pPos->nNode     = aNewPos.nNode;
                pPos->nContent  = aNewPos.nContent;
            }
            else
                DBG_ERROR( "neither point nor mark available for change" );
        }
        else
            DBG_ERROR( "failed to get position" );
    }

    return bNowEmpty;
}


chart::ChartDataRowSource SwChartDataSequence::GetDataRowSource() const
{
    // default value if sequence is not long enough to properly get this value
    chart::ChartDataRowSource eDataRowSource = chart::ChartDataRowSource_COLUMNS;

    SwFrmFmt* pTblFmt = GetFrmFmt();
    if(pTblFmt)
    {
        SwTable* pTable = SwTable::FindTable( pTblFmt );
        if(!pTable->IsTblComplex())
        {
            SwRangeDescriptor aDesc;
            if (FillRangeDescriptor( aDesc, GetCellRangeName( *pTblFmt, *pTblCrsr ) ))
            {
                if (aDesc.nTop == aDesc.nBottom && aDesc.nLeft != aDesc.nRight)
                    eDataRowSource = chart::ChartDataRowSource_ROWS;
            }
        }
    }
    return eDataRowSource;
}


void SwChartDataSequence::FillRangeDesc( SwRangeDescriptor &rRangeDesc ) const
{
    SwFrmFmt* pTblFmt = GetFrmFmt();
    if(pTblFmt)
    {
        SwTable* pTable = SwTable::FindTable( pTblFmt );
        if(!pTable->IsTblComplex())
        {
            FillRangeDescriptor( rRangeDesc, GetCellRangeName( *pTblFmt, *pTblCrsr ) );
        }
    }
}

/**
SwChartDataSequence::ExtendTo

extends the data-sequence by new cells added at the end of the direction
the data-sequence points to.
If the cells are already within the range of the sequence nothing needs
to be done.
If the cells are beyond the end of the sequence (are not adjacent to the
current last cell) nothing can be done. Only if the cells are adjacent to
the last cell they can be added.

@returns     true if the data-sequence was changed.
@param       bExtendCols
             specifies if columns or rows are to be extended
@param       nFirstNew
             index of first new row/col to be included in data-sequence
@param       nLastNew
             index of last new row/col to be included in data-sequence
*/
bool SwChartDataSequence::ExtendTo( bool bExtendCol,
        sal_Int32 nFirstNew, sal_Int32 nCount )
{
    bool bChanged = false;

    SwUnoTableCrsr* pUnoTblCrsr = *pTblCrsr;
    //pUnoTblCrsr->MakeBoxSels();

    const SwStartNode *pStartNd  = 0;
    const SwTableBox  *pStartBox = 0;
    const SwTableBox  *pEndBox   = 0;

    const SwTable* pTable = SwTable::FindTable( GetFrmFmt() );
    DBG_ASSERT( !pTable->IsTblComplex(), "table too complex" );
    if (nCount < 1 || nFirstNew < 0 || pTable->IsTblComplex())
        return false;

    //
    // get range descriptor (cell range) for current data-sequence
    //
    pStartNd = pUnoTblCrsr->GetPoint()->nNode.GetNode().FindTableBoxStartNode();
    pEndBox = pTable->GetTblBox( pStartNd->GetIndex() );
    const String aEndBox( pEndBox->GetName() );
    //
    pStartNd = pUnoTblCrsr->GetMark()->nNode.GetNode().FindTableBoxStartNode();
    pStartBox = pTable->GetTblBox( pStartNd->GetIndex() );
    const String aStartBox( pStartBox->GetName() );
    //
    String aCellRange( aStartBox );     // note that cell range here takes the newly added rows/cols already into account
    aCellRange.AppendAscii( ":" );
    aCellRange += aEndBox;
    SwRangeDescriptor aDesc;
    FillRangeDescriptor( aDesc, aCellRange );

    String aNewStartCell;
    String aNewEndCell;
    if (bExtendCol && aDesc.nBottom + 1 == nFirstNew)
    {
        // new column cells adjacent to the bottom of the
        // current data-sequence to be added...
        DBG_ASSERT( aDesc.nLeft == aDesc.nRight, "data-sequence is not a column" );
        aNewStartCell = lcl_GetCellName(aDesc.nLeft,  aDesc.nTop);
        aNewEndCell   = lcl_GetCellName(aDesc.nRight, aDesc.nBottom + nCount);
        bChanged = true;
    }
    else if (bExtendCol && aDesc.nTop - nCount == nFirstNew)
    {
        // new column cells adjacent to the top of the
        // current data-sequence to be added...
        DBG_ASSERT( aDesc.nLeft == aDesc.nRight, "data-sequence is not a column" );
        aNewStartCell = lcl_GetCellName(aDesc.nLeft,  aDesc.nTop - nCount);
        aNewEndCell   = lcl_GetCellName(aDesc.nRight, aDesc.nBottom);
        bChanged = true;
    }
    else if (!bExtendCol && aDesc.nRight + 1 == nFirstNew)
    {
        // new row cells adjacent to the right of the
        // current data-sequence to be added...
        DBG_ASSERT( aDesc.nTop == aDesc.nBottom, "data-sequence is not a row" );
        aNewStartCell = lcl_GetCellName(aDesc.nLeft, aDesc.nTop);
        aNewEndCell   = lcl_GetCellName(aDesc.nRight + nCount, aDesc.nBottom);
        bChanged = true;
    }
    else if (!bExtendCol && aDesc.nLeft - nCount == nFirstNew)
    {
        // new row cells adjacent to the left of the
        // current data-sequence to be added...
        DBG_ASSERT( aDesc.nTop == aDesc.nBottom, "data-sequence is not a row" );
        aNewStartCell = lcl_GetCellName(aDesc.nLeft - nCount, aDesc.nTop);
        aNewEndCell   = lcl_GetCellName(aDesc.nRight, aDesc.nBottom);
        bChanged = true;
    }

    if (bChanged)
    {
        // move table cursor to new start and end of data-sequence
        const SwTableBox *pNewStartBox = pTable->GetTblBox( aNewStartCell );
        const SwTableBox *pNewEndBox   = pTable->GetTblBox( aNewEndCell );
        pUnoTblCrsr->SetMark();
        pUnoTblCrsr->GetPoint()->nNode = *pNewEndBox->GetSttNd();
        pUnoTblCrsr->GetMark()->nNode  = *pNewStartBox->GetSttNd();
        pUnoTblCrsr->Move( fnMoveForward, fnGoNode );
        pUnoTblCrsr->MakeBoxSels();
    }

    return bChanged;
}

//////////////////////////////////////////////////////////////////////

SwChartLabeledDataSequence::SwChartLabeledDataSequence() :
    aEvtListeners( GetChartMutex() ),
    aModifyListeners( GetChartMutex() )
{
    bDisposed = sal_False;
}


SwChartLabeledDataSequence::~SwChartLabeledDataSequence()
{
}


uno::Reference< chart2::data::XDataSequence > SAL_CALL SwChartLabeledDataSequence::getValues(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();
    return xData;
}


void SwChartLabeledDataSequence::SetDataSequence(
        uno::Reference< chart2::data::XDataSequence >& rxDest,
        const uno::Reference< chart2::data::XDataSequence >& rxSource)
{
    uno::Reference< util::XModifyListener >  xML( dynamic_cast< util::XModifyListener* >(this), uno::UNO_QUERY );
    uno::Reference< lang::XEventListener >   xEL( dynamic_cast< lang::XEventListener* >(this), uno::UNO_QUERY );

    // stop listening to old data-sequence
    uno::Reference< util::XModifyBroadcaster > xMB( rxDest, uno::UNO_QUERY );
    if (xMB.is())
        xMB->removeModifyListener( xML );
    uno::Reference< lang::XComponent > xC( rxDest, uno::UNO_QUERY );
    if (xC.is())
        xC->removeEventListener( xEL );

    rxDest = rxSource;

    // start listening to new data-sequence
    xC = uno::Reference< lang::XComponent >( rxDest, uno::UNO_QUERY );
    if (xC.is())
        xC->addEventListener( xEL );
    xMB = uno::Reference< util::XModifyBroadcaster >( rxDest, uno::UNO_QUERY );
    if (xMB.is())
        xMB->addModifyListener( xML );
}


void SAL_CALL SwChartLabeledDataSequence::setValues(
        const uno::Reference< chart2::data::XDataSequence >& rxSequence )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    if (xData != rxSequence)
    {
        SetDataSequence( xData, rxSequence );
        // inform listeners of changes
        LaunchModifiedEvent( aModifyListeners, dynamic_cast< XModifyBroadcaster * >(this) );
    }
}


uno::Reference< chart2::data::XDataSequence > SAL_CALL SwChartLabeledDataSequence::getLabel(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();
    return xLabels;
}


void SAL_CALL SwChartLabeledDataSequence::setLabel(
        const uno::Reference< chart2::data::XDataSequence >& rxSequence )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    if (xLabels != rxSequence)
    {
        SetDataSequence( xLabels, rxSequence );
        // inform listeners of changes
        LaunchModifiedEvent( aModifyListeners, dynamic_cast< XModifyBroadcaster * >(this) );
    }
}


uno::Reference< util::XCloneable > SAL_CALL SwChartLabeledDataSequence::createClone(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    if (bDisposed)
        throw lang::DisposedException();

    uno::Reference< util::XCloneable > xRes;

    uno::Reference< util::XCloneable > xDataCloneable( xData, uno::UNO_QUERY );
    uno::Reference< util::XCloneable > xLabelsCloneable( xLabels, uno::UNO_QUERY );
    SwChartLabeledDataSequence *pRes = new SwChartLabeledDataSequence();
    if (xDataCloneable.is())
    {
        uno::Reference< chart2::data::XDataSequence > xDataClone( xDataCloneable->createClone(), uno::UNO_QUERY );
        pRes->setValues( xDataClone );
    }

    if (xLabelsCloneable.is())
    {
        uno::Reference< chart2::data::XDataSequence > xLabelsClone( xLabelsCloneable->createClone(), uno::UNO_QUERY );
        pRes->setLabel( xLabelsClone );
    }
    xRes = pRes;
    return xRes;
}


OUString SAL_CALL SwChartLabeledDataSequence::getImplementationName(  )
    throw (uno::RuntimeException)
{
    return C2U("SwChartLabeledDataSequence");
}


sal_Bool SAL_CALL SwChartLabeledDataSequence::supportsService(
        const OUString& rServiceName )
    throw (uno::RuntimeException)
{
    return rServiceName.equalsAscii( SN_LABELED_DATA_SEQUENCE );
}


uno::Sequence< OUString > SAL_CALL SwChartLabeledDataSequence::getSupportedServiceNames(  )
    throw (uno::RuntimeException)
{
    vos::OGuard aGuard( Application::GetSolarMutex() );
    uno::Sequence< OUString > aRes(1);
    aRes.getArray()[0] = C2U( SN_LABELED_DATA_SEQUENCE );
    return aRes;
}


void SAL_CALL SwChartLabeledDataSequence::disposing(
        const lang::EventObject& rSource )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    uno::Reference< uno::XInterface > xRef( rSource.Source );
    if (xRef == xData)
        xData.clear();
    if (xRef == xLabels)
        xLabels.clear();
    if (!xData.is() && !xLabels.is())
        dispose();
}


void SAL_CALL SwChartLabeledDataSequence::modified(
        const lang::EventObject& rEvent )
    throw (uno::RuntimeException)
{
    if (rEvent.Source == xData || rEvent.Source == xLabels)
    {
        LaunchModifiedEvent( aModifyListeners, dynamic_cast< XModifyBroadcaster * >(this) );
    }
}


void SAL_CALL SwChartLabeledDataSequence::addModifyListener(
        const uno::Reference< util::XModifyListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aModifyListeners.addInterface( rxListener );
}


void SAL_CALL SwChartLabeledDataSequence::removeModifyListener(
        const uno::Reference< util::XModifyListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aModifyListeners.removeInterface( rxListener );
}


void SAL_CALL SwChartLabeledDataSequence::dispose(  )
    throw (uno::RuntimeException)
{
    sal_Bool bMustDispose( sal_False );
    {
        osl::MutexGuard  aGuard( GetChartMutex() );
        bMustDispose = !bDisposed;
        if (!bDisposed)
            bDisposed = sal_True;
    }
    if (bMustDispose)
    {
        bDisposed = sal_True;

        // require listeners to release references to this object
        lang::EventObject aEvtObj( dynamic_cast< chart2::data::XLabeledDataSequence * >(this) );
        aModifyListeners.disposeAndClear( aEvtObj );
        aEvtListeners.disposeAndClear( aEvtObj );
    }
}


void SAL_CALL SwChartLabeledDataSequence::addEventListener(
        const uno::Reference< lang::XEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aEvtListeners.addInterface( rxListener );
}


void SAL_CALL SwChartLabeledDataSequence::removeEventListener(
        const uno::Reference< lang::XEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    osl::MutexGuard  aGuard( GetChartMutex() );
    if (!bDisposed && rxListener.is())
        aEvtListeners.removeInterface( rxListener );
}

//////////////////////////////////////////////////////////////////////
