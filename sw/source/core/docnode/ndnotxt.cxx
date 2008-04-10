/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ndnotxt.cxx,v $
 * $Revision: 1.13 $
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
#include <tools/poly.hxx>
#include <svtools/stritem.hxx>
#include <svx/contdlg.hxx>
#include <vcl/svapp.hxx>
#include <docary.hxx>
#include <doc.hxx>
#include <fmtcol.hxx>
#include <ndnotxt.hxx>
#include <ndgrf.hxx>
#include <ndole.hxx>
#include <ndindex.hxx>
#include <hints.hxx>			// fuer SwFmtChg
#include <istyleaccess.hxx>
#include <SwStyleNameMapper.hxx>

SwNoTxtNode::SwNoTxtNode( const SwNodeIndex & rWhere,
                  const BYTE nNdType,
                  SwGrfFmtColl *pGrfColl,
                  SwAttrSet* pAutoAttr ) :
    SwCntntNode( rWhere, nNdType, pGrfColl ),
    pContour( 0 ),
    bAutomaticContour( FALSE ),
    bContourMapModeValid( TRUE ),
    bPixelContour( FALSE )
{
    // soll eine Harte-Attributierung gesetzt werden?
    if( pAutoAttr )
        SetAttr( *pAutoAttr );
}


SwNoTxtNode::~SwNoTxtNode()
{
    delete pContour;
}


// erzeugt fuer alle Ableitungen einen AttrSet mit Bereichen
// fuer Frame- und Grafik-Attributen
void SwNoTxtNode::NewAttrSet( SwAttrPool& rPool )
{
    ASSERT( !mpAttrSet.get(), "AttrSet ist doch gesetzt" );
    SwAttrSet aNewAttrSet( rPool, aNoTxtNodeSetRange );

    // put names of parent style and conditional style:
    const SwFmtColl* pFmtColl = GetFmtColl();
    String sVal;
    SwStyleNameMapper::FillProgName( pFmtColl->GetName(), sVal, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL, sal_True );
    SfxStringItem aFmtColl( RES_FRMATR_STYLE_NAME, sVal );
    aNewAttrSet.Put( aFmtColl );

    aNewAttrSet.SetParent( &GetFmtColl()->GetAttrSet() );
    mpAttrSet = GetDoc()->GetIStyleAccess().getAutomaticStyle( aNewAttrSet, IStyleAccess::AUTO_STYLE_NOTXT );
}

// Dummies fuer das Laden/Speichern von persistenten Daten
// bei Grafiken und OLE-Objekten


BOOL SwNoTxtNode::RestorePersistentData()
{
    return TRUE;
}


BOOL SwNoTxtNode::SavePersistentData()
{
    return TRUE;
}


void SwNoTxtNode::SetContour( const PolyPolygon *pPoly, BOOL bAutomatic )
{
    delete pContour;
    if ( pPoly )
        pContour = new PolyPolygon( *pPoly );
    else
        pContour = 0;
    bAutomaticContour = bAutomatic;
    bContourMapModeValid = TRUE;
    bPixelContour = FALSE;
}


void SwNoTxtNode::CreateContour()
{
    ASSERT( !pContour, "Contour available." );
    pContour = new PolyPolygon(SvxContourDlg::CreateAutoContour(GetGraphic()));
    bAutomaticContour = TRUE;
    bContourMapModeValid = TRUE;
    bPixelContour = FALSE;
}

const PolyPolygon *SwNoTxtNode::HasContour() const
{
    if( !bContourMapModeValid )
    {
        const MapMode aGrfMap( GetGraphic().GetPrefMapMode() );
        BOOL bPixelGrf = aGrfMap.GetMapUnit() == MAP_PIXEL;
        const MapMode aContourMap( bPixelGrf ? MAP_PIXEL : MAP_100TH_MM );
        if( bPixelGrf ? !bPixelContour : aGrfMap != aContourMap )
        {
            ASSERT( !bPixelGrf || aGrfMap == aContourMap,
                    "scale factor for pixel unsupported" );
            OutputDevice* pOutDev =
                (bPixelGrf || bPixelContour) ? Application::GetDefaultDevice()
                                             : 0;
            USHORT nPolyCount = pContour->Count();
            for( USHORT j=0; j<nPolyCount; j++ )
            {
                Polygon& rPoly = (*pContour)[j];

                USHORT nCount = rPoly.GetSize();
                for( USHORT i=0 ; i<nCount; i++ )
                {
                    if( bPixelGrf )
                        rPoly[i] = pOutDev->LogicToPixel( rPoly[i],
                                                          aContourMap );
                    else if( bPixelContour )
                        rPoly[i] = pOutDev->PixelToLogic( rPoly[i], aGrfMap );
                    else
                        rPoly[i] = OutputDevice::LogicToLogic( rPoly[i],
                                                                 aContourMap,
                                                                 aGrfMap );
                }
            }
        }
        ((SwNoTxtNode *)this)->bContourMapModeValid = TRUE;
        ((SwNoTxtNode *)this)->bPixelContour = FALSE;
    }

    return pContour;
}

void SwNoTxtNode::GetContour( PolyPolygon &rPoly ) const
{
    ASSERT( pContour, "Contour not available." );
    rPoly = *HasContour();
}

void SwNoTxtNode::SetContourAPI( const PolyPolygon *pPoly )
{
    delete pContour;
    if ( pPoly )
        pContour = new PolyPolygon( *pPoly );
    else
        pContour = 0;
    bContourMapModeValid = FALSE;
}

BOOL SwNoTxtNode::GetContourAPI( PolyPolygon &rContour ) const
{
    if( !pContour )
        return FALSE;

    rContour = *pContour;
    if( bContourMapModeValid )
    {
        const MapMode aGrfMap( GetGraphic().GetPrefMapMode() );
        const MapMode aContourMap( MAP_100TH_MM );
        ASSERT( aGrfMap.GetMapUnit() != MAP_PIXEL ||
                aGrfMap == MapMode( MAP_PIXEL ),
                    "scale factor for pixel unsupported" );
        if( aGrfMap.GetMapUnit() != MAP_PIXEL &&
            aGrfMap != aContourMap )
        {
            USHORT nPolyCount = rContour.Count();
            for( USHORT j=0; j<nPolyCount; j++ )
            {
                Polygon& rPoly = (*pContour)[j];

                USHORT nCount = rPoly.GetSize();
                for( USHORT i=0 ; i<nCount; i++ )
                {
                    rPoly[i] = OutputDevice::LogicToLogic( rPoly[i], aGrfMap,
                                                           aContourMap );
                }
            }
        }
    }

    return TRUE;
}

const BOOL SwNoTxtNode::IsPixelContour() const
{
    BOOL bRet;
    if( bContourMapModeValid )
    {
        const MapMode aGrfMap( GetGraphic().GetPrefMapMode() );
        bRet = aGrfMap.GetMapUnit() == MAP_PIXEL;
    }
    else
    {
        bRet = bPixelContour;
    }

    return bRet;
}


Graphic SwNoTxtNode::GetGraphic() const
{
    Graphic aRet;
    if ( GetGrfNode() )
    {
        ((SwGrfNode*)this)->SwapIn( TRUE );
        aRet = ((SwGrfNode*)this)->GetGrf();
    }
    else
    {
        ASSERT( GetOLENode(), "new type of Node?" );
        aRet = *((SwOLENode*)this)->SwOLENode::GetGraphic();
    }
    return aRet;
}


void SwNoTxtNode::SetAlternateText( const String& rTxt, sal_Bool bBroadcast )
{
    if( bBroadcast )
    {
        SwStringMsgPoolItem aOld( RES_ALT_TEXT_CHANGED, aAlternateText );
        SwStringMsgPoolItem aNew( RES_ALT_TEXT_CHANGED, rTxt );
        aAlternateText = rTxt;
        Modify( &aOld, &aNew );
    }
    else
    {
        aAlternateText = rTxt;
    }
}

