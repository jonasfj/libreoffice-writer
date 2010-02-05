/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: gctable.cxx,v $
 * $Revision: 1.9 $
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
#include <editeng/boxitem.hxx>
#include <tblrwcl.hxx>
#include <swtblfmt.hxx>


inline const SvxBorderLine* GetLineTB( const SvxBoxItem* pBox, BOOL bTop )
{
    return bTop ? pBox->GetTop() : pBox->GetBottom();
}


BOOL _SwGCBorder_BoxBrd::CheckLeftBorderOfFormat( const SwFrmFmt& rFmt )
{
    const SvxBorderLine* pBrd;
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rFmt.GetItemState( RES_BOX, TRUE, &pItem ) &&
        0 != ( pBrd = ((SvxBoxItem*)pItem)->GetLeft() ) )
    {
        if( *pBrdLn == *pBrd )
            bAnyBorderFnd = TRUE;
        return TRUE;
    }
    return FALSE;
}



BOOL lcl_GCBorder_ChkBoxBrd_L( const SwTableLine*& rpLine, void* pPara )
{
    const SwTableBox* pBox = rpLine->GetTabBoxes()[ 0 ];
    return lcl_GCBorder_ChkBoxBrd_B( pBox, pPara );
}

BOOL lcl_GCBorder_ChkBoxBrd_B( const SwTableBox*& rpBox, void* pPara )
{
    BOOL bRet = TRUE;
    if( rpBox->GetTabLines().Count() )
    {
        for( USHORT n = 0, nLines = rpBox->GetTabLines().Count();
                n < nLines && bRet; ++n )
        {
            const SwTableLine* pLine = rpBox->GetTabLines()[ n ];
            bRet = lcl_GCBorder_ChkBoxBrd_L( pLine, pPara );
        }
    }
    else
    {
        _SwGCBorder_BoxBrd* pBPara = (_SwGCBorder_BoxBrd*)pPara;
        bRet = pBPara->CheckLeftBorderOfFormat( *rpBox->GetFrmFmt() );
    }
    return bRet;
}

BOOL lcl_GCBorder_GetLastBox_L( const SwTableLine*& rpLine, void* pPara )
{
    const SwTableBoxes& rBoxes = rpLine->GetTabBoxes();
    const SwTableBox* pBox = rBoxes[ rBoxes.Count()-1 ];
    ::lcl_GCBorder_GetLastBox_B( pBox, pPara );
    return TRUE;
}

BOOL lcl_GCBorder_GetLastBox_B( const SwTableBox*& rpBox, void* pPara )
{
    SwTableLines& rLines = (SwTableLines&)rpBox->GetTabLines();
    if( rLines.Count() )
        rLines.ForEach( &lcl_GCBorder_GetLastBox_L, pPara );
    else
        ((SwTableBoxes*)pPara)->Insert( rpBox, ((SwTableBoxes*)pPara)->Count() );
    return TRUE;
}

// suche das "Ende" der vorgegebene BorderLine. Returnt wird die "Layout"Pos!
USHORT lcl_FindEndPosOfBorder( const SwCollectTblLineBoxes& rCollTLB,
                        const SvxBorderLine& rBrdLn, USHORT& rStt, BOOL bTop )
{
    USHORT nPos, nLastPos = 0;
    for( USHORT nEnd = rCollTLB.Count(); rStt < nEnd; ++rStt )
    {
        const SfxPoolItem* pItem;
        const SvxBorderLine* pBrd;
        const SwTableBox& rBox = rCollTLB.GetBox( rStt, &nPos );

        if( SFX_ITEM_SET != rBox.GetFrmFmt()->GetItemState(RES_BOX,TRUE, &pItem )
            || 0 == ( pBrd = GetLineTB( (SvxBoxItem*)pItem, bTop ))
            || !( *pBrd == rBrdLn ))
            break;
        nLastPos = nPos;
    }
    return nLastPos;
}

inline const SvxBorderLine* lcl_GCBorder_GetBorder( const SwTableBox& rBox,
                                                BOOL bTop,
                                                const SfxPoolItem** ppItem )
{
    return SFX_ITEM_SET == rBox.GetFrmFmt()->GetItemState( RES_BOX, TRUE, ppItem )
            ? GetLineTB( (SvxBoxItem*)*ppItem, bTop )
            : 0;
}

void lcl_GCBorder_DelBorder( const SwCollectTblLineBoxes& rCollTLB,
                                USHORT& rStt, BOOL bTop,
                                const SvxBorderLine& rLine,
                                const SfxPoolItem* pItem,
                                USHORT nEndPos,
                                SwShareBoxFmts* pShareFmts )
{
    SwTableBox* pBox = (SwTableBox*)&rCollTLB.GetBox( rStt );
    USHORT nNextPos;
    const SvxBorderLine* pLn = &rLine;

    do {
        if( pLn && *pLn == rLine )
        {
            SvxBoxItem aBox( *(SvxBoxItem*)pItem );
            if( bTop )
                aBox.SetLine( 0, BOX_LINE_TOP );
            else
                aBox.SetLine( 0, BOX_LINE_BOTTOM );

            if( pShareFmts )
                pShareFmts->SetAttr( *pBox, aBox );
            else
                pBox->ClaimFrmFmt()->SetFmtAttr( aBox );
        }

        if( ++rStt >= rCollTLB.Count() )
            break;

        pBox = (SwTableBox*)&rCollTLB.GetBox( rStt, &nNextPos );
        if( nNextPos > nEndPos )
            break;

        pLn = lcl_GCBorder_GetBorder( *pBox, bTop, &pItem );

    } while( TRUE );
}


BOOL lcl_GC_Line_Border( const SwTableLine*& rpLine, void* pPara )
{
    _SwGCLineBorder* pGCPara = (_SwGCLineBorder*)pPara;

    // zuerst die rechte Kante mit der linken Kante der naechsten Box
    // innerhalb dieser Line
    {
        _SwGCBorder_BoxBrd aBPara;
        const SvxBorderLine* pBrd;
        const SfxPoolItem* pItem;
        const SwTableBoxes& rBoxes = rpLine->GetTabBoxes();
        for( USHORT n = 0, nBoxes = rBoxes.Count() - 1; n < nBoxes; ++n )
        {
            SwTableBoxes aBoxes;
            {
                const SwTableBox* pBox = rBoxes[ n ];
                if( pBox->GetSttNd() )
                    aBoxes.Insert( pBox, 0 );
                else
                    lcl_GCBorder_GetLastBox_B( pBox, &aBoxes );
            }

            SwTableBox* pBox;
            for( USHORT i = aBoxes.Count(); i; )
                if( SFX_ITEM_SET == (pBox = aBoxes[ --i ])->GetFrmFmt()->
                    GetItemState( RES_BOX, TRUE, &pItem ) &&
                    0 != ( pBrd = ((SvxBoxItem*)pItem)->GetRight() ) )
                {
                    aBPara.SetBorder( *pBrd );
                    const SwTableBox* pNextBox = rBoxes[n+1];
                    if( lcl_GCBorder_ChkBoxBrd_B( pNextBox, &aBPara ) &&
                        aBPara.IsAnyBorderFound() )
                    {
                        SvxBoxItem aBox( *(SvxBoxItem*)pItem );
                        aBox.SetLine( 0, BOX_LINE_RIGHT );
                        if( pGCPara->pShareFmts )
                            pGCPara->pShareFmts->SetAttr( *pBox, aBox );
                        else
                            pBox->ClaimFrmFmt()->SetFmtAttr( aBox );
                    }
                }

            aBoxes.Remove( 0, aBoxes.Count() );
        }
    }

    // und jetzt die eigene untere Kante mit der nachfolgenden oberen Kante
    if( !pGCPara->IsLastLine() )
    {
        SwCollectTblLineBoxes aBottom( FALSE );
        SwCollectTblLineBoxes aTop( TRUE );

        ::lcl_Line_CollectBox( rpLine, &aBottom );

        const SwTableLine* pNextLine = (*pGCPara->pLines)[ pGCPara->nLinePos+1 ];
        ::lcl_Line_CollectBox( pNextLine, &aTop );

        // dann entferne mal alle "doppelten" gleichen Lines
        USHORT nBtmPos, nTopPos,
                nSttBtm = 0, nSttTop = 0,
                nEndBtm = aBottom.Count(), nEndTop = aTop.Count();

        const SwTableBox *pBtmBox = &aBottom.GetBox( nSttBtm++, &nBtmPos ),
                         *pTopBox = &aTop.GetBox( nSttTop++, &nTopPos );
        const SfxPoolItem *pBtmItem = 0, *pTopItem = 0;
        const SvxBorderLine *pBtmLine(0), *pTopLine(0);
        BOOL bGetTopItem = TRUE, bGetBtmItem = TRUE;

        do {
            if( bGetBtmItem )
                pBtmLine = lcl_GCBorder_GetBorder( *pBtmBox, FALSE, &pBtmItem );
            if( bGetTopItem )
                pTopLine = lcl_GCBorder_GetBorder( *pTopBox, TRUE, &pTopItem );

            if( pTopLine && pBtmLine && *pTopLine == *pBtmLine )
            {
                // dann kann einer entfernt werden, aber welche?
                USHORT nSavSttBtm = nSttBtm, nSavSttTop = nSttTop;
                USHORT nBtmEndPos = ::lcl_FindEndPosOfBorder( aBottom,
                                                *pTopLine, nSttBtm, FALSE );
                if( !nBtmEndPos ) nBtmEndPos = nBtmPos;
                USHORT nTopEndPos = ::lcl_FindEndPosOfBorder( aTop,
                                                *pTopLine, nSttTop, TRUE );
                if( !nTopEndPos ) nTopEndPos = nTopPos;


                if( nTopEndPos <= nBtmEndPos )
                {
                    // dann die TopBorder bis zur BottomEndPos loeschen
                    nSttTop = nSavSttTop;
                    if( nTopPos <= nBtmEndPos )
                        lcl_GCBorder_DelBorder( aTop, --nSttTop, TRUE,
                                            *pBtmLine, pTopItem, nBtmEndPos,
                                            pGCPara->pShareFmts );
                    else
                        nSttBtm = nSavSttBtm;
                }
                else
                {
                    // sonst die BottomBorder bis zur TopEndPos loeschen
                    nSttBtm = nSavSttBtm;
                    if( nBtmPos <= nTopEndPos )
                        lcl_GCBorder_DelBorder( aBottom, --nSttBtm, FALSE,
                                            *pTopLine, pBtmItem, nTopEndPos,
                                            pGCPara->pShareFmts );
                    else
                        nSttTop = nSavSttTop;
                }
                nTopPos = nBtmPos;
            }

            if( nTopPos == nBtmPos )
            {
                if( nSttBtm >= nEndBtm || nSttTop >= nEndTop )
                    break;

                pBtmBox = &aBottom.GetBox( nSttBtm++, &nBtmPos );
                pTopBox = &aTop.GetBox( nSttTop++, &nTopPos );
                bGetTopItem = bGetBtmItem = TRUE;
            }
            else if( nTopPos < nBtmPos )
            {
                if( nSttTop >= nEndTop )
                    break;
                pTopBox = &aTop.GetBox( nSttTop++, &nTopPos );
                bGetTopItem = TRUE;
                bGetBtmItem = FALSE;
            }
            else
            {
                if( nSttBtm >= nEndBtm )
                    break;
                pBtmBox = &aBottom.GetBox( nSttBtm++, &nBtmPos );
                bGetTopItem = FALSE;
                bGetBtmItem = TRUE;
            }

        } while( TRUE );
    }

    ((SwTableLine*)rpLine)->GetTabBoxes().ForEach( &lcl_GC_Box_Border, pPara );

    ++pGCPara->nLinePos;

    return TRUE;
}

BOOL lcl_GC_Box_Border( const SwTableBox*& rpBox, void* pPara )
{
    if( rpBox->GetTabLines().Count() )
    {
        _SwGCLineBorder aPara( *rpBox );
        aPara.pShareFmts = ((_SwGCLineBorder*)pPara)->pShareFmts;
        ((SwTableBox*)rpBox)->GetTabLines().ForEach( &lcl_GC_Line_Border, &aPara );
    }
    return TRUE;
}

struct _GCLinePara
{
    SwTableLines* pLns;
    SwShareBoxFmts* pShareFmts;

    _GCLinePara( SwTableLines& rLns, _GCLinePara* pPara = 0 )
        : pLns( &rLns ), pShareFmts( pPara ? pPara->pShareFmts : 0 )
    {}
};

BOOL lcl_MergeGCBox( const SwTableBox*& rpTblBox, void* pPara )
{
    SwTableBox*& rpBox = (SwTableBox*&)rpTblBox;
    USHORT n, nLen = rpBox->GetTabLines().Count();
    if( nLen )
    {
        // ACHTUNG: die Anzahl der Lines kann sich aendern!
        _GCLinePara aPara( rpBox->GetTabLines(), (_GCLinePara*)pPara );
        for( n = 0; n < rpBox->GetTabLines().Count() &&
            lcl_MergeGCLine( *(rpBox->GetTabLines().GetData() + n), &aPara );
            ++n )
            ;

        if( 1 == rpBox->GetTabLines().Count() )
        {
            // Box mit einer Line, dann verschiebe alle Boxen der Line
            // hinter diese Box in der Parent-Line und loesche diese Box
            SwTableLine* pInsLine = rpBox->GetUpper();
            SwTableLine* pCpyLine = rpBox->GetTabLines()[0];
            USHORT nInsPos = pInsLine->GetTabBoxes().C40_GETPOS( SwTableBox, rpBox );
            for( n = 0; n < pCpyLine->GetTabBoxes().Count(); ++n )
                pCpyLine->GetTabBoxes()[n]->SetUpper( pInsLine );

            pInsLine->GetTabBoxes().Insert( &pCpyLine->GetTabBoxes(), nInsPos+1 );
            pCpyLine->GetTabBoxes().Remove( 0, n );
            // loesche alte die Box mit der Line
            pInsLine->GetTabBoxes().DeleteAndDestroy( nInsPos );

            return FALSE;		// neu aufsetzen
        }
    }
    return TRUE;
}

BOOL lcl_MergeGCLine( const SwTableLine*& rpLine, void* pPara )
{
    SwTableLine* pLn = (SwTableLine*)rpLine;
    USHORT nLen = pLn->GetTabBoxes().Count();
    if( nLen )
    {
        _GCLinePara* pGCPara = (_GCLinePara*)pPara;
        while( 1 == nLen )
        {
            // es gibt eine Box mit Lines
            SwTableBox* pBox = pLn->GetTabBoxes()[0];
            if( !pBox->GetTabLines().Count() )
                break;

            SwTableLine* pLine = pBox->GetTabLines()[0];

            // pLine wird zu der aktuellen, also der rpLine,
            // die restlichen werden ins LinesArray hinter der akt.
            // verschoben.
            // Das LinesArray ist im pPara!
            nLen = pBox->GetTabLines().Count();

            SwTableLines& rLns = *pGCPara->pLns;
            const SwTableLine* pTmp = pLn;
            USHORT nInsPos = rLns.GetPos( pTmp );
            ASSERT( USHRT_MAX != nInsPos, "Line nicht gefunden!" );

            SwTableBox* pUpper = pLn->GetUpper();

            rLns.Remove( nInsPos, 1 );		// die Line dem aus Array loeschen
            rLns.Insert( &pBox->GetTabLines(), nInsPos );

            // JP 31.03.99: Bug 60000 - die Attribute der zu loeschenden
            // Line an die "eingefuegten" uebertragen
            const SfxPoolItem* pItem;
            if( SFX_ITEM_SET == pLn->GetFrmFmt()->GetItemState(
                                    RES_BACKGROUND, TRUE, &pItem ))
            {
                SwTableLines& rBoxLns = pBox->GetTabLines();
                for( USHORT nLns = 0; nLns < nLen; ++nLns )
                    if( SFX_ITEM_SET != rBoxLns[ nLns ]->GetFrmFmt()->
                            GetItemState( RES_BACKGROUND, TRUE ))
                        pGCPara->pShareFmts->SetAttr( *rBoxLns[ nLns ], *pItem );
            }

            pBox->GetTabLines().Remove( 0, nLen );	// Lines aus Array loeschen

            delete pLn;

            // Abhaengigkeit neu setzen
            while( nLen-- )
                rLns[ nInsPos++ ]->SetUpper( pUpper );

            pLn = pLine;						// und neu setzen
            nLen = pLn->GetTabBoxes().Count();
        }

        // ACHTUNG: die Anzahl der Boxen kann sich aendern!
        for( nLen = 0; nLen < pLn->GetTabBoxes().Count(); ++nLen )
            if( !lcl_MergeGCBox( *(pLn->GetTabBoxes().GetData() + nLen ), pPara ))
                --nLen;
    }
    return TRUE;
}

        // Struktur ein wenig aufraeumen
void SwTable::GCLines()
{
    // ACHTUNG: die Anzahl der Lines kann sich aendern!
    _GCLinePara aPara( GetTabLines() );
    SwShareBoxFmts aShareFmts;
    aPara.pShareFmts = &aShareFmts;
    for( USHORT n = 0; n < GetTabLines().Count() &&
            lcl_MergeGCLine( *(GetTabLines().GetData() + n ), &aPara ); ++n )
        ;
}


