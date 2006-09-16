/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docchart.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 20:50:22 $
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



#include <float.h>

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SCH_DLL_HXX
#include <sch/schdll.hxx>
#endif
#ifndef _SCH_MEMCHRT_HXX
#include <sch/memchrt.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _CALC_HXX
#include <calc.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _CELLFML_HXX
#include <cellfml.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _SWTBLFMT_HXX
#include <swtblfmt.hxx>
#endif
#ifndef _TBLSEL_HXX
#include <tblsel.hxx>
#endif
#ifndef _CELLATR_HXX
#include <cellatr.hxx>
#endif


SchMemChart *SwTable::UpdateData( SchMemChart* pData,
                                const String* pSelection ) const
{
    SwCalc aCalc( *GetFrmFmt()->GetDoc() );
    SwTblCalcPara aCalcPara( aCalc, *this );
    String sSelection, sRowColInfo;
    BOOL bSetChartRange = TRUE;

    // worauf bezieht sich das Chart?
    if( pData && pData->SomeData1().Len() )
    {
        sSelection = pData->SomeData1();
        sRowColInfo = pData->SomeData2();
    }
    else if( pData && pData->GetChartRange().maRanges.size() )
    {
        SchDLL::ConvertChartRangeForWriter( *pData, FALSE );
        sSelection = pData->SomeData1();
        sRowColInfo = pData->SomeData2();
        bSetChartRange = FALSE;
    }
    else if( pSelection )
    {
        sSelection = *pSelection;
        sRowColInfo.AssignAscii( RTL_CONSTASCII_STRINGPARAM("11") );
    }

    SwChartLines aLines;
    if( !IsTblComplexForChart( sSelection, &aLines ))
    {
        USHORT nLines = aLines.Count(), nBoxes = aLines[0]->Count();

        if( !pData )
        {
            //JP 08.02.99: als default wird mit Spalten/Zeilenueberschrift
            //				eingefuegt, deshalb das -1
            pData = SchDLL::NewMemChart( nBoxes-1, nLines-1 );
            pData->SetSubTitle( aEmptyStr );
            pData->SetXAxisTitle( aEmptyStr );
            pData->SetYAxisTitle( aEmptyStr );
            pData->SetZAxisTitle( aEmptyStr );
        }

        USHORT nRowStt = 0, nColStt = 0;
        if( sRowColInfo.Len() )
        {
            if( '1' == sRowColInfo.GetChar( 0 ))
                ++nRowStt;
            if( '1' == sRowColInfo.GetChar( 1 ))
                ++nColStt;
        }

        if( (nBoxes - nColStt) > pData->GetColCount() )
            SchDLL::MemChartInsertCols( *pData, 0, (nBoxes - nColStt) - pData->GetColCount() );
        else if( (nBoxes - nColStt) < pData->GetColCount() )
            SchDLL::MemChartRemoveCols( *pData, 0, pData->GetColCount() - (nBoxes - nColStt) );

        if( (nLines - nRowStt) > pData->GetRowCount() )
            SchDLL::MemChartInsertRows( *pData, 0, (nLines - nRowStt) - pData->GetRowCount() );
        else if( (nLines - nRowStt) < pData->GetRowCount() )
            SchDLL::MemChartRemoveRows( *pData, 0, pData->GetRowCount() - (nLines - nRowStt) );


        ASSERT( pData->GetRowCount() >= (nLines - nRowStt ) &&
                pData->GetColCount() >= (nBoxes - nColStt ),
                    "Die Struktur fuers Chart ist zu klein,\n"
                    "es wird irgendwo in den Speicher geschrieben!" );

        // Row-Texte setzen
        USHORT n;
        if( nRowStt )
            for( n = nColStt; n < nBoxes; ++n )
            {
                const SwTableBox *pBox = (*aLines[ 0 ])[ n ];
                ASSERT( pBox->GetSttNd(), "Box without SttIdx" );
                SwNodeIndex aIdx( *pBox->GetSttNd(), 1 );
                const SwTxtNode* pTNd = aIdx.GetNode().GetTxtNode();
                if( !pTNd )
                    pTNd = aIdx.GetNodes().GoNextSection( &aIdx, TRUE, FALSE )
                                ->GetTxtNode();

                pData->SetColText( n - nColStt, pTNd->GetExpandTxt() );
            }
        else
        {
            String aText;
            for( n = nColStt; n < nBoxes; ++n )
            {
                SchDLL::GetDefaultForColumnText( *pData, n - nColStt, aText );
                pData->SetColText( n - nColStt, aText );
            }
        }

        // Col-Texte setzen
        if( nColStt )
            for( n = nRowStt; n < nLines; ++n )
            {
                const SwTableBox *pBox = (*aLines[ n ])[ 0 ];
                ASSERT( pBox->GetSttNd(), "Box without SttIdx" );
                SwNodeIndex aIdx( *pBox->GetSttNd(), 1 );
                const SwTxtNode* pTNd = aIdx.GetNode().GetTxtNode();
                if( !pTNd )
                    pTNd = aIdx.GetNodes().GoNextSection( &aIdx, TRUE, FALSE )
                                ->GetTxtNode();

                pData->SetRowText( n - nRowStt, pTNd->GetExpandTxt() );
            }
        else
        {
            String aText;
            for( n = nRowStt; n < nLines; ++n )
            {
                SchDLL::GetDefaultForRowText( *pData, n - nRowStt, aText );
                pData->SetRowText( n - nRowStt, aText );
            }
        }

        // und dann fehlen nur noch die Daten
        const SwTblBoxNumFormat& rDfltNumFmt = *(SwTblBoxNumFormat*)
                                        GetDfltAttr( RES_BOXATR_FORMAT );
        pData->SetNumberFormatter( GetFrmFmt()->GetDoc()->GetNumberFormatter());

        int bFirstRow = TRUE;
        for( n = nRowStt; n < nLines; ++n )
        {
            for( USHORT i = nColStt; i < nBoxes; ++i )
            {
                const SwTableBox* pBox = (*aLines[ n ])[ i ];
                ASSERT( pBox->GetSttNd(), "Box without SttIdx" );
                SwNodeIndex aIdx( *pBox->GetSttNd(), 1 );
                const SwTxtNode* pTNd = aIdx.GetNode().GetTxtNode();
                if( !pTNd )
                    pTNd = aIdx.GetNodes().GoNextSection( &aIdx, TRUE, FALSE )
                                ->GetTxtNode();

                pData->SetData( short( i - nColStt ),
                                short( n - nRowStt ),
                                pTNd->GetTxt().Len()
                                        ? pBox->GetValue( aCalcPara )
                                        : DBL_MIN );

                if( i == nColStt || bFirstRow )
                {
                    // first box of row set the numberformat
                    const SwTblBoxNumFormat& rNumFmt = pBox->GetFrmFmt()->
                                                        GetTblBoxNumFmt();
                    if( rNumFmt != rDfltNumFmt )
                    {
                        pData->SetNumFormatIdCol( i, rNumFmt.GetValue() );
                        if( bFirstRow )
                            pData->SetNumFormatIdRow( n, rNumFmt.GetValue() );
                    }
                }
            }
            bFirstRow = FALSE;
        }
    }
    else if( pData )
    {
        if( pData->GetColCount() )
            SchDLL::MemChartRemoveCols( *pData, 0, pData->GetColCount() );
        if( pData->GetRowCount() )
            SchDLL::MemChartRemoveRows( *pData, 0, pData->GetRowCount() );
    }
    else
        bSetChartRange = FALSE;

    if( bSetChartRange )
    {
        // convert the selection string to the SchartRanges
        pData->SomeData1() = sSelection;
        pData->SomeData2() = sRowColInfo;
        SchDLL::ConvertChartRangeForWriter( *pData, TRUE );
    }

    return pData;
}

BOOL SwTable::IsTblComplexForChart( const String& rSelection,
                                    SwChartLines* pGetCLines ) const
{
    const SwTableBox* pSttBox, *pEndBox;
    if( 2 < rSelection.Len() )
    {
        // spitze Klammern am Anfang & Ende enfernen
        String sBox( rSelection );
        if( '<' == sBox.GetChar( 0  ) ) sBox.Erase( 0, 1 );
        if( '>' == sBox.GetChar( sBox.Len()-1  ) ) sBox.Erase( sBox.Len()-1 );

        xub_StrLen nTrenner = sBox.Search( ':' );
        ASSERT( STRING_NOTFOUND != nTrenner, "keine gueltige Selektion" );

        pSttBox = GetTblBox( sBox.Copy( 0, nTrenner ));
        pEndBox = GetTblBox( sBox.Copy( nTrenner+1 ));
    }
    else
    {
        const SwTableLines* pLns = &GetTabLines();
        pSttBox = (*pLns)[ 0 ]->GetTabBoxes()[ 0 ];
        while( !pSttBox->GetSttNd() )
            // bis zur Content Box!
            pSttBox = pSttBox->GetTabLines()[ 0 ]->GetTabBoxes()[ 0 ];

        const SwTableBoxes* pBoxes = &(*pLns)[ pLns->Count()-1 ]->GetTabBoxes();
        pEndBox = (*pBoxes)[ pBoxes->Count()-1 ];
        while( !pEndBox->GetSttNd() )
        {
            // bis zur Content Box!
            pLns = &pEndBox->GetTabLines();
            pBoxes = &(*pLns)[ pLns->Count()-1 ]->GetTabBoxes();
            pEndBox = (*pBoxes)[ pBoxes->Count()-1 ];
        }
    }

    return !pSttBox || !pEndBox || !::ChkChartSel( *pSttBox->GetSttNd(),
                                        *pEndBox->GetSttNd(), pGetCLines );
}



IMPL_LINK( SwDoc, DoUpdateAllCharts, Timer *, pTimer )
{
    ViewShell* pVSh;
    GetEditShell( &pVSh );
    if( pVSh )
    {
        const SwFrmFmts& rTblFmts = *GetTblFrmFmts();
        for( USHORT n = 0; n < rTblFmts.Count(); ++n )
        {
            SwTable* pTmpTbl;
            const SwTableNode* pTblNd;
            SwFrmFmt* pFmt = rTblFmts[ n ];

            if( 0 != ( pTmpTbl = SwTable::FindTable( pFmt ) ) &&
                0 != ( pTblNd = pTmpTbl->GetTableNode() ) &&
                pTblNd->GetNodes().IsDocNodes() )
            {
                _UpdateCharts( *pTmpTbl, *pVSh );
            }
        }
    }
    return 0;
}

void SwDoc::_UpdateCharts( const SwTable& rTbl, ViewShell& rVSh ) const
{
    String aName( rTbl.GetFrmFmt()->GetName() );
    SwOLENode *pONd;
    SwStartNode *pStNd;
    SwNodeIndex aIdx( *GetNodes().GetEndOfAutotext().StartOfSectionNode(), 1 );
    while( 0 != (pStNd = aIdx.GetNode().GetStartNode()) )
    {
        aIdx++;
        SwFrm* pFrm;
        if( 0 != ( pONd = aIdx.GetNode().GetOLENode() ) &&
            aName.Equals( pONd->GetChartTblName() ) &&
            0 != ( pFrm = pONd->GetFrm() ) )
        {
            SwOLEObj& rOObj = pONd->GetOLEObj();

            SchMemChart *pData = SchDLL::GetChartData( rOObj.GetOleRef() );
            FASTBOOL bDelData = 0 == pData;

            ASSERT( pData, "UpdateChart ohne irgendwelche Daten?" );
            pData = rTbl.UpdateData( pData );

            if( pData->GetColCount() && pData->GetRowCount() )
            {
                SchDLL::Update( rOObj.GetOleRef(), pData, rVSh.GetWin() );
                rOObj.GetObject().UpdateReplacement();

                SwClientIter aIter( *pONd );
                for( pFrm = (SwFrm*)aIter.First( TYPE(SwFrm) ); pFrm;
                        pFrm = (SwFrm*)aIter.Next() )
                {
                    if( pFrm->Frm().HasArea() )
                        rVSh.InvalidateWindows( pFrm->Frm() );
                }
            }

            if ( bDelData )
                delete pData;
        }
        aIdx.Assign( *pStNd->EndOfSectionNode(), + 1 );
    }
}

void SwDoc::UpdateCharts( const String &rName ) const
{
    SwTable* pTmpTbl = SwTable::FindTable( FindTblFmtByName( rName ) );
    if( pTmpTbl )
    {
        ViewShell* pVSh;
        GetEditShell( &pVSh );

        if( pVSh )
            _UpdateCharts( *pTmpTbl, *pVSh );
    }
}

void SwDoc::SetTableName( SwFrmFmt& rTblFmt, const String &rNewName )
{
    const String aOldName( rTblFmt.GetName() );

    BOOL bNameFound = 0 == rNewName.Len();
    if( !bNameFound )
    {
        SwFrmFmt* pFmt;
        const SwFrmFmts& rTbl = *GetTblFrmFmts();
        for( USHORT i = rTbl.Count(); i; )
            if( !( pFmt = rTbl[ --i ] )->IsDefault() &&
                pFmt->GetName() == rNewName && IsUsed( *pFmt ) )
            {
                bNameFound = TRUE;
                break;
            }
    }

    if( !bNameFound )
        rTblFmt.SetName( rNewName, sal_True );
    else
        rTblFmt.SetName( GetUniqueTblName(), sal_True );

    SwStartNode *pStNd;
    SwNodeIndex aIdx( *GetNodes().GetEndOfAutotext().StartOfSectionNode(), 1 );
    while ( 0 != (pStNd = aIdx.GetNode().GetStartNode()) )
    {
        aIdx++;
        SwOLENode *pNd = aIdx.GetNode().GetOLENode();
        if( pNd && aOldName == pNd->GetChartTblName() )
        {
            pNd->SetChartTblName( rNewName );

            SwOLEObj& rOObj = pNd->GetOLEObj();
            SchMemChart *pData = SchDLL::GetChartData( rOObj.GetOleRef() );
            if( pData )
            {
                ViewShell* pVSh;
                GetEditShell( &pVSh );

                if( aOldName == pData->GetMainTitle() )
                {
                    pData->SetMainTitle( rNewName );
                    if( pVSh )
                    {
                        SchDLL::Update( rOObj.GetOleRef(), pData, pVSh->GetWin() );
                        rOObj.GetObject().UpdateReplacement();
                    }
                }

                if( pVSh )
                {
                    SwFrm *pFrm;
                    SwClientIter aIter( *pNd );
                    for( pFrm = (SwFrm*)aIter.First( TYPE(SwFrm) ); pFrm;
                            pFrm = (SwFrm*)aIter.Next() )
                    {
                        if( pFrm->Frm().HasArea() )
                            pVSh->InvalidateWindows( pFrm->Frm() );
                    }
                }
            }
        }
        aIdx.Assign( *pStNd->EndOfSectionNode(), + 1 );
    }
    SetModified();
}


