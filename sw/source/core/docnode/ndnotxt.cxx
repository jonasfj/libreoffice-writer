/*************************************************************************
 *
 *  $RCSfile: ndnotxt.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-17 13:56:27 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/


#pragma hdrstop

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _IPOBJ_HXX
#include <so3/ipobj.hxx>
#endif
#ifndef _SV_POLY_HXX
#include <vcl/poly.hxx>
#endif
#ifndef _CONTDLG_HXX_
#include <svx/contdlg.hxx>
#endif
#ifndef _SV_SVAPP_HXX 
#include <vcl/svapp.hxx>
#endif

#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _FMTCOL_HXX
#include <fmtcol.hxx>
#endif
#ifndef _NDNOTXT_HXX
#include <ndnotxt.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>			// fuer SwFmtChg
#endif


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
    ASSERT( !pAttrSet, "AttrSet ist doch gesetzt" );
    pAttrSet = new SwAttrSet( rPool, aNoTxtNodeSetRange );
    pAttrSet->SetParent( &GetFmtColl()->GetAttrSet() );
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
        SvInPlaceObjectRef xObj( ((SwOLENode*)this)->GetOLEObj().GetOleRef() );
        GDIMetaFile aMtf;
        aRet = xObj->GetGDIMetaFile( aMtf );
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

