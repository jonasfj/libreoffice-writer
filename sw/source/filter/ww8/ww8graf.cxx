/*************************************************************************
 *
 *  $RCSfile: ww8graf.cxx,v $
 *
 *  $Revision: 1.56 $
 *
 *  last change: $Author: cmc $ $Date: 2002-05-09 12:32:00 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
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

#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef SVTOOLS_FSTATHELPER_HXX
#include <svtools/fstathelper.hxx>
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVX_FMGLOB_HXX
#include <svx/fmglob.hxx>
#endif
#ifndef _SDTAITM_HXX
#include <svx/sdtaitm.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX //autogen
#include <svx/udlnitem.hxx>
#endif
#ifndef _SVX_XLINIIT_HXX //autogen
#include <svx/xlineit.hxx>
#endif
#ifndef _SVX_FILLITEM_HXX //autogen
#include <svx/xfillit.hxx>
#endif
#ifndef _SVDMODEL_HXX //autogen
#include <svx/svdmodel.hxx>
#endif
#ifndef _SDTAITM_HXX //autogen
#include <svx/sdtaitm.hxx>
#endif
#ifndef _SVDCAPT_HXX //autogen
#include <svx/svdocapt.hxx>
#endif
#ifndef _SXCTITM_HXX //autogen
#include <svx/sxctitm.hxx>
#endif
#ifndef _MyEDITENG_HXX
#include <svx/editeng.hxx>
#endif
#ifndef _SVDPAGE_HXX //autogen
#include <svx/svdpage.hxx>
#endif
#ifndef _SVDOPATH_HXX //autogen
#include <svx/svdopath.hxx>
#endif
#ifndef _SVDOCIRC_HXX //autogen
#include <svx/svdocirc.hxx>
#endif
#ifndef _OUTLOBJ_HXX //autogen
#include <svx/outlobj.hxx>
#endif
#ifndef _SVDOGRP_HXX //autogen
#include <svx/svdogrp.hxx>
#endif
#ifndef _SVDOGRAF_HXX
#include <svx/svdograf.hxx>
#endif
#ifndef _SVDOOLE2_HXX
#include <svx/svdoole2.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX
#include <svx/colritem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX
#include <svx/postitem.hxx>
#endif
#ifndef _SVX_ADJITEM_HXX
#include <svx/adjitem.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX
#include <svx/wghtitem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX
#include <svx/crsditem.hxx>
#endif
#ifndef _SVX_CNTRITEM_HXX
#include <svx/cntritem.hxx>
#endif
#ifndef _SVX_SHDDITEM_HXX
#include <svx/shdditem.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_IMPGRF_HXX
#include <svx/impgrf.hxx>
#endif
#ifndef _SVDOATTR_HXX
#include <svx/svdoattr.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_RECTENUM_HXX //autogen
#include <svx/rectenum.hxx>
#endif
#ifndef _SVX_OPAQITEM_HXX //autogen
#include <svx/opaqitem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _OUTLINER_HXX
#include <svx/outliner.hxx>			// #79453#
#endif
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif
#ifndef _MSDFFIMP_HXX
#include <svx/msdffimp.hxx>
#endif

#ifndef _GRFATR_HXX
#include <grfatr.hxx>			// class SwCropGrf
#endif
#ifndef _FMTORNT_HXX
#include <fmtornt.hxx>
#endif
#ifndef _FMTCNTNT_HXX //autogen
#include <fmtcntnt.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _NDGRF_HXX
#include <ndgrf.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _DCONTACT_HXX
#include <dcontact.hxx>
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>			// Progress
#endif
#ifndef _FMTCNCT_HXX
#include <fmtcnct.hxx>
#endif
#ifndef _SWUNODEF_HXX
#include <swunodef.hxx>
#endif

#ifndef _WW8STRUC_HXX
#include "ww8struc.hxx"
#endif
#ifndef _WW8SCAN_HXX
#include "ww8scan.hxx"
#endif
#ifndef _WW8PAR_HXX
#include "ww8par.hxx"			// class SwWWImplReader
#endif
#ifndef _WW8PAR2_HXX
#include "ww8par2.hxx"			// SwWW8StyInf
#endif
#ifndef _WW8GRAF_HXX
#include "ww8graf.hxx"
#endif

// Hilfsroutinen

// inline geht bei BLC leider nicht mit "for"
/*inline*/ Color WW8TransCol( SVBT32 nWC )
{
#if 1				// 1 = Vordefinierte Farben benutzen, 0 = ignorieren

    // Farbtabelle zum Umrechnen RGB-Werte in vordefinierte Farben
    // ( Damit bei der Writer-UI die Farbnamen stimmen )
    // Die Tabelle int im *3er-System* aufgeteilt. Die Grauwerte fehlen,
    // da sie nicht ins 3er-System passen ( 4 Werte: sw, ws, 2 * grau )
    static ColorData eColA[] = { 					//  B G R  B G R  B G R
        COL_BLACK, COL_RED, COL_LIGHTRED,			//  0 0 0, 0 0 1, 0 0 2
        COL_GREEN, COL_BROWN, COL_BLACK,			//  0 1 0, 0 1 1, 0 1 2
        COL_LIGHTGREEN, COL_BLACK, COL_YELLOW,		//  0 2 0, 0 2 1, 0 2 2
        COL_BLUE, COL_MAGENTA, COL_BLACK,			//  1 0 0, 1 0 1, 1 0 2
        COL_CYAN, COL_LIGHTGRAY, COL_BLACK,			//  1 1 0, 1 1 1, 1 1 2
        COL_BLACK, COL_BLACK, COL_BLACK,			//  1 2 0, 1 2 1, 1 2 2
        COL_LIGHTBLUE, COL_BLACK, COL_LIGHTMAGENTA,	//  2 0 0, 2 0 1, 2 0 2
        COL_BLACK, COL_BLACK, COL_BLACK,			//  2 1 0, 2 1 1, 2 1 2
        COL_LIGHTCYAN, COL_BLACK, COL_WHITE };		//  2 2 0, 2 2 1, 2 2 2

    // In nWC[3] steht ein Byte, dass in der WW-Doku nicht beschrieben ist.
    // Die Bedeutung ist anscheinend folgende: Bei 0 ist es eine normale
    // Farbe, dessen RGB-Wert in nWC[0..2] steht. stehen in nWC[3] die
    // Werte 0x1, 0x7d oder 0x83, dann ist es ein Grauwert, dessen
    // Schwarzanteil in 1/2 % in nWC[0] steht.
    // Ich vermute, dass es auf Bit0 in nWV[3] ankommt, ob es RGB oder Grau ist.

    if( !( nWC[3] & 0x1 ) &&             			// keine Spezial-Farbe (grau)
        (    ( nWC[0] == 0 ||  nWC[0]== 0x80 || nWC[0] == 0xff )	// R-Anteil
          && ( nWC[1] == 0 ||  nWC[1]== 0x80 || nWC[1] == 0xff )	// G-Anteil
          && ( nWC[2] == 0 ||  nWC[2]== 0x80 || nWC[2] == 0xff ) ) ){// B-Anteil
        int nIdx = 0;		// und nun: Idx-Berechnung im 3er-System
        for( int i = 2; i >= 0; i-- ){
            nIdx *= 3;
            if( nWC[i] )
                nIdx += ( ( nWC[i] == 0xff ) ? 2 : 1 );
        }
        if( eColA[ nIdx ] != COL_BLACK )
            return Color( eColA[ nIdx ] );	// Standard-Color
    }
#endif
    if( nWC[3] & 0x1 ){								// Spezialfarbe: Grau
        register BYTE u = (BYTE)( (ULONG)( 200 - nWC[0] ) * 256 / 200 );
        return Color( u, u, u );
    }
                    // User-Color
    return Color( (USHORT)nWC[0] << 8, (USHORT)nWC[1] << 8,
                  (USHORT)nWC[2] << 8 );
}

void wwFrameNamer::SetUniqueGraphName(SwFrmFmt *pFrmFmt, const String &rFixed)
{
    if (mbIsDisabled || !rFixed.Len())
        return;
    mnImportedGraphicsCount++;
    String aName(msSeed);
    aName += String::CreateFromInt32(mnImportedGraphicsCount);
    aName.APPEND_CONST_ASC( ": " );
    aName += rFixed;
    pFrmFmt->SetName( aName );
}

// ReadGrafStart liest die ObjektDaten ein und erzeugt falls noetig einen Anker
BOOL SwWW8ImplReader::ReadGrafStart( void* pData, short nDataSiz, 
    WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    if( (INT16)SVBT16ToShort( pHd->cb )
        < (short)sizeof( WW8_DPHEAD ) + nDataSiz ){
        ASSERT( !this, "+Grafik-Element: Size ?" );
        pStrm->SeekRel( (INT16)SVBT16ToShort( pHd->cb ) - sizeof( WW8_DPHEAD ) );
        return FALSE;
    }
    pStrm->Read( pData, nDataSiz );

    RndStdIds eAnchor = (SVBT8ToByte( pDo->by ) < 2) ? FLY_PAGE : FLY_AT_CNTNT;
    if( (bIsHeader || bIsFooter) && (FLY_AT_CNTNT != eAnchor) )
    {
        eAnchor = FLY_AT_CNTNT;
        pNode_FLY_AT_CNTNT = &pPaM->GetPoint()->nNode.GetNode();
    }

    pDrawFmt->SetAttr( SwFmtAnchor(eAnchor) );
    pAnchorStck->AddAnchor(*pPaM->GetPoint(), pDrawFmt);

    nDrawXOfs2 = nDrawXOfs;
    nDrawYOfs2 = nDrawYOfs;

    if( eAnchor == FLY_AT_CNTNT )
    {
        if( SVBT8ToByte( pDo->bx ) == 1 )		// Pos: echt links
            nDrawXOfs2 -= nPgLeft;
        if( nTable )							// Obj in Table
            nDrawXOfs2 -= GetTableLeft();		// -> siehe Kommentar
                                                // bei GetTableLeft()
    }
    else
    {
        if( SVBT8ToByte( pDo->bx ) != 1 )
            nDrawXOfs2 += nPgLeft;
        if( SVBT8ToByte( pDo->by ) == 0 )
            nDrawYOfs2 += nPgTop;
    }

    return TRUE;
}

// SetStdAttr() setzt die Attribute, die jedes Objekt hat

static void SetStdAttr( SfxItemSet& rSet, WW8_DP_LINETYPE& rL,
                        WW8_DP_SHADOW& rSh )
{
    if( SVBT16ToShort( rL.lnps ) == 5 ){			// unsichtbar
        rSet.Put( XLineStyleItem( XLINE_NONE ) );
    }else{											// sichtbar
        Color aCol( WW8TransCol( rL.lnpc ) );			// LinienFarbe
        rSet.Put( XLineColorItem( aEmptyStr, aCol ) );
        rSet.Put( XLineWidthItem( SVBT16ToShort( rL.lnpw ) ) );
                                                    // LinienDicke
        if( SVBT16ToShort( rL.lnps ) >= 1
            && SVBT16ToShort(rL.lnps ) <= 4 ){		// LinienStil
            rSet.Put( XLineStyleItem( XLINE_DASH ) );
            INT16 nLen = SVBT16ToShort( rL.lnpw );
            XDash aD( XDASH_RECT, 1, 2 * nLen, 1, 5 * nLen, 5 * nLen );
            switch( SVBT16ToShort( rL.lnps ) ){
            case 1: aD.SetDots( 0 );			// Dash
                    aD.SetDashLen( 6 * nLen );
                    aD.SetDistance( 4 * nLen );
                    break;
            case 2: aD.SetDashes( 0 ); break;	// Dot
            case 3: break;						// Dash Dot
            case 4:	aD.SetDots( 2 ); break;		// Dash Dot Dot
            }
            rSet.Put( XLineDashItem( aEmptyStr, aD ) );
        }else{
            rSet.Put( XLineStyleItem( XLINE_SOLID ) );  // noetig fuer TextBox
        }
    }
    if( SVBT16ToShort( rSh.shdwpi ) ){					// Schatten
        rSet.Put( SdrShadowItem( TRUE ) );
        rSet.Put( SdrShadowXDistItem( SVBT16ToShort( rSh.xaOffset ) ) );
        rSet.Put( SdrShadowYDistItem( SVBT16ToShort( rSh.yaOffset ) ) );
    }
}

// SetFill setzt Fuellattribute wie Vordergrund- und Hintergrund-Farbe
// und Muster durch Reduktion auf eine Farbe.
// SetFill() setzt z.Zt kein Muster, da Sdr das nur sehr umstaendlich kann
// und die Sdr-Schraffur ( XDash ) noch nicht fertig ist.
// Statt dessen wird eine Mischfarbe gewaehlt, die auf den entsprechenden
// Farbton zwischen den Farben liegt.

static void SetFill( SfxItemSet& rSet, WW8_DP_FILL& rFill )
{
    static BYTE nPatA[] = { 0, 0, 5, 10, 20, 25, 30, 40, 50, 60, 70, 75, 80,
                        90, 50, 50, 50, 50,50, 50, 33, 33, 33, 33, 33, 33 };
    register short nPat = SVBT16ToShort( rFill.flpp );

    if( nPat == 0 ){							// durchsichtig
        rSet.Put( XFillStyleItem( XFILL_NONE ) );
    }else{
        rSet.Put( XFillStyleItem( XFILL_SOLID ) );  // noetig fuer TextBox
        if( nPat <= 1 || nPat > sizeof( nPatA ) ){	// solid Bg oder unbekannt
            rSet.Put( XFillColorItem( aEmptyStr, WW8TransCol( rFill.dlpcBg ) ) );
        }else{										// Brush -> Farbmischung
            Color aB( WW8TransCol( rFill.dlpcBg ) );
            Color aF( WW8TransCol( rFill.dlpcFg ) );
            aB.SetRed( (BYTE)( ( (ULONG)aF.GetRed() * nPatA[nPat]
                        + (ULONG)aB.GetRed() * ( 100 - nPatA[nPat] ) ) / 100 ) );
            aB.SetGreen( (BYTE)( ( (ULONG)aF.GetGreen() * nPatA[nPat]
                        + (ULONG)aB.GetGreen() * ( 100 - nPatA[nPat] ) ) / 100 ) );
            aB.SetBlue( (BYTE)( ( (ULONG)aF.GetBlue() * nPatA[nPat]
                        + (ULONG)aB.GetBlue() * ( 100 - nPatA[nPat] ) ) / 100 ) );
            rSet.Put( XFillColorItem( aEmptyStr, aB ) );
        }
    }
}

static void SetLineEndAttr( SfxItemSet& rSet, WW8_DP_LINEEND& rLe,
                            WW8_DP_LINETYPE& rLt )
{
    UINT16 aSB = SVBT16ToShort( rLe.aStartBits );
    if( aSB & 0x3 ){
        XPolygon aXP(3);
        aXP[0] = Point( 0, 330 );
        aXP[1] = Point( 100, 0 );
        aXP[2] = Point( 200, 330 );
        rSet.Put( XLineEndItem( aEmptyStr, aXP ) );
        USHORT nSiz = SVBT16ToShort( rLt.lnpw )
                        * ( ( aSB >> 2 & 0x3 ) + ( aSB >> 4 & 0x3 ) );
        if( nSiz < 220 ) nSiz = 220;
        rSet.Put( XLineEndWidthItem( nSiz ) );
        rSet.Put( XLineEndCenterItem( FALSE ) );
    }

    UINT16 aEB = SVBT16ToShort( rLe.aEndBits );
    if( aEB & 0x3 ){
        XPolygon aXP(3);
        aXP[0] = Point( 0, 330 );
        aXP[1] = Point( 100, 0 );
        aXP[2] = Point( 200, 330 );
        rSet.Put( XLineStartItem( aEmptyStr, aXP ) );
        USHORT nSiz = SVBT16ToShort( rLt.lnpw )
                        * ( ( aEB >> 2 & 0x3 ) + ( aEB >> 4 & 0x3 ) );
        if( nSiz < 220 ) nSiz = 220;
        rSet.Put( XLineStartWidthItem( nSiz ) );
        rSet.Put( XLineStartCenterItem( FALSE ) );
    }
}

// Ab hier folgen die Routinen fuer die einzelnen Objekte
void SwWW8ImplReader::ReadLine( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    WW8_DP_LINE aLine;

    if( !ReadGrafStart( (void*)&aLine, sizeof( aLine ), pHd, pDo ) )
        return;

    Point aP[2];
    {
        Point& rP0 = aP[0];
        Point& rP1 = aP[1];

        rP0.X() = (INT16)SVBT16ToShort( pHd->xa ) + nDrawXOfs2;
        rP0.Y() = (INT16)SVBT16ToShort( pHd->ya ) + nDrawYOfs2;
        rP1 = rP0;
        rP0.X() += (INT16)SVBT16ToShort( aLine.xaStart );
        rP0.Y() += (INT16)SVBT16ToShort( aLine.yaStart );
        rP1.X() += (INT16)SVBT16ToShort( aLine.xaEnd );
        rP1.Y() += (INT16)SVBT16ToShort( aLine.yaEnd );
    }
    SdrObject* pObj = new SdrPathObj( OBJ_LINE, XPolygon( Polygon( 2, aP ) ) );
    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    SfxAllItemSet aSet( pDrawModel->GetItemPool() );

    SetStdAttr( aSet, aLine.aLnt, aLine.aShd );
    SetLineEndAttr( aSet, aLine.aEpp, aLine.aLnt );

//-/	pObj->SetAttributes( aSet, FALSE );
    pObj->SetItemSetAndBroadcast(aSet);
}

void SwWW8ImplReader::ReadRect( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    WW8_DP_RECT aRect;

    if( !ReadGrafStart( (void*)&aRect, sizeof( aRect ), pHd, pDo ) )
        return;

    Point aP0( (INT16)SVBT16ToShort( pHd->xa ) + nDrawXOfs2,
               (INT16)SVBT16ToShort( pHd->ya ) + nDrawYOfs2 );
    Point aP1( aP0 );
    aP1.X() += (INT16)SVBT16ToShort( pHd->dxa );
    aP1.Y() += (INT16)SVBT16ToShort( pHd->dya );

    SdrObject* pObj = new SdrRectObj( Rectangle( aP0, aP1 ) );
    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    SfxAllItemSet aSet( pDrawModel->GetItemPool() );

    SetStdAttr( aSet, aRect.aLnt, aRect.aShd );
    SetFill( aSet, aRect.aFill );

//-/	pObj->SetAttributes( aSet, FALSE );
    pObj->SetItemSetAndBroadcast(aSet);
}

void SwWW8ImplReader::ReadElipse( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    WW8_DP_ELIPSE aElipse;

    if( !ReadGrafStart( (void*)&aElipse, sizeof( aElipse ), pHd, pDo ) )
        return;

    Point aP0( (INT16)SVBT16ToShort( pHd->xa ) + nDrawXOfs2,
               (INT16)SVBT16ToShort( pHd->ya ) + nDrawYOfs2 );
    Point aP1( aP0 );
    aP1.X() += (INT16)SVBT16ToShort( pHd->dxa );
    aP1.Y() += (INT16)SVBT16ToShort( pHd->dya );

    SdrObject* pObj = new SdrCircObj( OBJ_CIRC, Rectangle( aP0, aP1 ) );
    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    SfxAllItemSet aSet( pDrawModel->GetItemPool() );

    SetStdAttr( aSet, aElipse.aLnt, aElipse.aShd );
    SetFill( aSet, aElipse.aFill );

//-/	pObj->SetAttributes( aSet, FALSE );
    pObj->SetItemSetAndBroadcast(aSet);
}

void SwWW8ImplReader::ReadArc( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    WW8_DP_ARC aArc;

    if( !ReadGrafStart( (void*)&aArc, sizeof( aArc ), pHd, pDo ) )
        return;

    Point aP0( (INT16)SVBT16ToShort( pHd->xa ) + nDrawXOfs2,
               (INT16)SVBT16ToShort( pHd->ya ) + nDrawYOfs2 );
    Point aP1( aP0 );
    aP1.X() += (INT16)SVBT16ToShort( pHd->dxa ) * 2;
    aP1.Y() += (INT16)SVBT16ToShort( pHd->dya ) * 2;

    short nA[] = { 2, 3, 1, 0 };
    short nW = nA[ ( ( SVBT8ToByte( aArc.fLeft ) & 1 ) << 1 )
                    + ( SVBT8ToByte( aArc.fUp ) & 1 ) ];
    if( !SVBT8ToByte( aArc.fLeft ) ){
        aP0.Y() -= (INT16)SVBT16ToShort( pHd->dya );
        aP1.Y() -= (INT16)SVBT16ToShort( pHd->dya );
    }
    if( SVBT8ToByte( aArc.fUp ) ){
        aP0.X() -= (INT16)SVBT16ToShort( pHd->dxa );
        aP1.X() -= (INT16)SVBT16ToShort( pHd->dxa );
    }

    SdrObject* pObj = new SdrCircObj( OBJ_SECT, Rectangle( aP0, aP1 ),
                               nW * 9000, ( ( nW + 1 ) & 3 ) * 9000 );
    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    SfxAllItemSet aSet( pDrawModel->GetItemPool() );

    SetStdAttr( aSet, aArc.aLnt, aArc.aShd );
    SetFill( aSet, aArc.aFill );

//-/	pObj->SetAttributes( aSet, FALSE );
    pObj->SetItemSetAndBroadcast(aSet);
}

void SwWW8ImplReader::ReadPolyLine( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    WW8_DP_POLYLINE aPoly;

    if( !ReadGrafStart( (void*)&aPoly, sizeof( aPoly ), pHd, pDo ) )
        return;

    UINT16 nCount = SVBT16ToShort( aPoly.aBits1 ) >> 1 & 0x7fff;
    SVBT16 *pP = new SVBT16[nCount * 2];
    pStrm->Read( pP, nCount * 4 );		// Punkte einlesen
    Polygon aP( nCount );
    Point aPt;
    USHORT i;

    for( i=0; i<nCount; i++ ){
        aPt.X() = SVBT16ToShort( pP[i << 1] ) + nDrawXOfs2
                  + (INT16)SVBT16ToShort( pHd->xa );
        aPt.Y() = SVBT16ToShort( pP[( i << 1 ) + 1] ) + nDrawYOfs2
                  + (INT16)SVBT16ToShort( pHd->ya );
        aP[i] = aPt;
    }
    delete[] pP;

    SdrObject* pObj = new SdrPathObj(
                ( SVBT16ToShort( aPoly.aBits1 ) & 0x1 ) ? OBJ_POLY : OBJ_PLIN,
                                XPolygon( aP ) );

    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    SfxAllItemSet aSet( pDrawModel->GetItemPool() );

    SetStdAttr( aSet, aPoly.aLnt, aPoly.aShd );
    SetFill( aSet, aPoly.aFill );

//-/	pObj->SetAttributes( aSet, FALSE );
    pObj->SetItemSetAndBroadcast(aSet);
}

ESelection SwWW8ImplReader::GetESelection( long nCpStart, long nCpEnd )
{
    USHORT nPCnt = pDrawEditEngine->GetParagraphCount();
    USHORT nSP = 0;
    USHORT nEP = 0;
    while(		(nSP < nPCnt)
            &&  (nCpStart >= pDrawEditEngine->GetTextLen( nSP ) + 1) )
    {
        nCpStart -= pDrawEditEngine->GetTextLen( nSP ) + 1;
        nSP++;
    }
        // Beim Ende erst 1 Zeichen spaeter auf naechste Zeile umschalten,
        // da sonst Zeilenattribute immer eine Zeile zu weit reichen.
    while(		(nEP < nPCnt)
            &&	(nCpEnd > pDrawEditEngine->GetTextLen( nEP ) + 1) )
    {
        nCpEnd -= pDrawEditEngine->GetTextLen( nEP ) + 1;
        nEP++;
    }
    return ESelection( nSP, (USHORT)nCpStart, nEP, (USHORT)nCpEnd );
}

// InsertTxbxStyAttrs() setzt die Style-Attribute in den uebergebenen ItemSet.
// Es werden die SW-Styles genommen, die Import-WW-Styles sind zu diesem
// Zeitpunkt schon destruiert.
// Die SW-Styles werden per Tiefensuche, d.h. mit Parent-Styles nach den
// in aSrcTab angegebenen Attributen untersucht. Diese werden per Clone
// dupliziert, bei den Duplikaten werden die Which-IDs
// gemaess der Tabelle aDstTab umgesetzt, damit die EditEngine sie nicht
// ignoriert.
// Es werden hierbei sowohl Para- wie auch Zeichen-Attribute in den
// ItemSet gestopft.
void SwWW8ImplReader::InsertTxbxStyAttrs( SfxItemSet& rS, USHORT nColl )
{
    if( nColl < nColls && pCollA[nColl].pFmt && pCollA[nColl].bColl )
    {
        const SfxPoolItem* pItem;
        for( USHORT i = POOLATTR_BEGIN; i < POOLATTR_END; i++ )
        {
            //If we are set in the source and not set in the destination
            //then add it in.
            if ( SFX_ITEM_SET == pCollA[nColl].pFmt->GetItemState(
                i, TRUE, &pItem ) )
            {
                SfxItemPool *pEditPool = rS.GetPool();
                USHORT nWhich = i;
                USHORT nSlotId = rDoc.GetAttrPool().GetSlotId(nWhich);
                if (
                    nSlotId && nWhich != nSlotId &&
                    0 != (nWhich = pEditPool->GetWhich(nSlotId)) &&
                    nWhich != nSlotId &&
                    ( SFX_ITEM_SET != rS.GetItemState(nWhich, FALSE ) )
                   )
                {
                    SfxPoolItem* pCopy = pItem->Clone();
                    pCopy->SetWhich( nWhich );
                    rS.Put( *pCopy );
                    delete pCopy;
                }
            }
        }
    }

}

// InsertTxbxAttrs() setzt zwischen StartCp und EndCp die Attribute.
// Dabei werden Style-Attribute als harte Attribute, Absatz- und Zeichen-
// attribute gesetzt.
void SwWW8ImplReader::InsertTxbxAttrs( long nStartCp, long nEndCp,
    BOOL bONLYnPicLocFc )
{
    nStartCp += nDrawCpO;
    nEndCp   += nDrawCpO;
    WW8ReaderSave aSave(this,nStartCp);

    BOOL bOldAdjust = pPlcxMan->GetDoingDrawTextBox();
    pPlcxMan->SetDoingDrawTextBox(TRUE);
    WW8_CP nStart = pPlcxMan->Where();
    WW8_CP nNext,nEnd,nStartReplace;

    BOOL bDoingSymbol = FALSE;
    sal_Unicode cReplaceSymbol = cSymbol;

    nAktColl = pPlcxMan->GetColl();

    SfxItemSet *pS = new SfxItemSet(pDrawEditEngine->GetEmptyItemSet());
    WW8PLCFManResult aRes;

    USHORT nIstd;
    //Here store stack location
    USHORT nCurrentCount = pCtrlStck->Count();
    while( nStart <= nEndCp )
    {
        // get position of next SPRM
        BOOL bStartAttr = pPlcxMan->Get( &aRes );
        nAktColl = pPlcxMan->GetColl();
        if (aRes.nSprmId)
        {
            if( bONLYnPicLocFc )
            {
                if ( (68 == aRes.nSprmId) || (0x6A03 == aRes.nSprmId) )
                {
                    Read_PicLoc( aRes.nSprmId,
                        aRes.pMemPos + 1 + (8 > pWwFib->nVersion ? 0 : 1)
                        + WW8SprmDataOfs( aRes.nSprmId ), 4 );
                    // Ok, that's it.  Now let's get out of here!
                    break;
                }
            }
            else if ( aRes.nSprmId && (
                (256 >  aRes.nSprmId) || (0x0800 <= aRes.nSprmId) ) )
            {
                //Here place them onto our usual stack and we will pop them
                //off and convert them later
                if (bStartAttr)
                {
                    ImportSprm(aRes.pMemPos, aRes.nSprmId);
                    if (!bDoingSymbol && bSymbol == TRUE)
                    {
                        bDoingSymbol = TRUE;
                        nStartReplace = nStart;
                        cReplaceSymbol = cSymbol;
                    }
                }
                else
                {
                    EndSprm( aRes.nSprmId );
                    if (bSymbol == FALSE && bDoingSymbol)
                    {
                        bDoingSymbol = FALSE;
                        String sTemp;
                        sTemp.Fill(nStart-nStartReplace,cReplaceSymbol);
                        pDrawEditEngine->QuickInsertText(sTemp,
                            GetESelection(nStartReplace - nStartCp, 
                            nStart - nStartCp ) );
                    }
                }
            }
        }

        nIstd = pPlcxMan->GetPapPLCF()->GetIstd();
        (*pPlcxMan)++;
        nNext = pPlcxMan->Where();

        if( (nNext != nStart) && !bONLYnPicLocFc )
        {
            nEnd = ( nNext < nEndCp ) ? nNext : nEndCp;
            SfxItemPool *pEditPool = pS->GetPool();

            //Here read current properties and convert them into pS
            //and put those attrs into the draw box if they can be converted
            //to draw attributes
            if (pCtrlStck->Count() - nCurrentCount)
            {
                for (USHORT i = nCurrentCount; i < pCtrlStck->Count(); i++)
                {
                    const SfxPoolItem *pItem = ((*pCtrlStck)[i])->pAttr;
                    USHORT nWhich = pItem->Which();
                    if( nWhich < RES_FLTRATTR_BEGIN ||
                        nWhich >= RES_FLTRATTR_END )
                    {
                        USHORT nSlotId = rDoc.GetAttrPool().GetSlotId(nWhich);
                        if (
                            nSlotId && nWhich != nSlotId &&
                            0 != (nWhich = pEditPool->GetWhich(nSlotId)) &&
                            nWhich != nSlotId
                           )
                        {
                            SfxPoolItem* pCopy = pItem->Clone();
                            pCopy->SetWhich( nWhich );
                            pS->Put( *pCopy );
                            delete pCopy;
                        }
                    }
                }
            }
            //Fill in the remainder from the style
            InsertTxbxStyAttrs( *pS, nIstd );

            if( pS->Count() )
            {
                pDrawEditEngine->QuickSetAttribs( *pS,
                    GetESelection( nStart - nStartCp, nEnd - nStartCp ) );
                delete pS;
                pS = new SfxItemSet(pDrawEditEngine->GetEmptyItemSet());
            }
        }
        nStart = nNext;
    }
    delete pS;

    //pop off as far as recorded location just in case there were some left
    //unclosed
    for (USHORT nI = pCtrlStck->Count(); nI > nCurrentCount; nI--)
        pCtrlStck->DeleteAndDestroy(nI-1);
    pPlcxMan->SetDoingDrawTextBox(bOldAdjust);
    aSave.Restore(this);
}

BOOL SwWW8ImplReader::GetTxbxTextSttEndCp( long& rStartCp, long& rEndCp,
                                            USHORT nTxBxS, USHORT nSequence )
{
    // rasch den TextBox-PLCF greifen
    WW8PLCFspecial* pT = pPlcxMan->GetTxbx();
    if( !pT )
    {
        ASSERT( !this, "+Wo ist der Grafik-Text (1) ?" );
        return FALSE;
    }

    // ggfs. zuerst die richtige TextBox-Story finden
    BOOL bCheckTextBoxStory = ( nTxBxS && pT->GetIMax() >= nTxBxS );
    if(  bCheckTextBoxStory )
        pT->SetIdx( nTxBxS-1 );

    // dann Start und Ende ermitteln
    void* pT0;
    if( !pT->Get( rStartCp, pT0 ) )
    {
        ASSERT( !this, "+Wo ist der Grafik-Text (2) ?" );
        return FALSE;
    }

    if( bCheckTextBoxStory )
    {
        BOOL bReusable = (0 != SVBT16ToShort( ((WW8_TXBXS*)pT0)->fReusable ));
        while( bReusable )
        {
            (*pT)++;
            if( !pT->Get( rStartCp, pT0 ) )
            {
                ASSERT( !this, "+Wo ist der Grafik-Text (2-a) ?" );
                return FALSE;
            }
            bReusable = (0 != SVBT16ToShort( ((WW8_TXBXS*)pT0)->fReusable ));
        }
    }
    (*pT)++;
    if( !pT->Get( rEndCp, pT0 ) )
    {
        ASSERT( !this, "+Wo ist der Grafik-Text (3) ?" );
        return FALSE;
    }

    // jetzt ggfs. die passende Page in der Break-Table finden
    if( bCheckTextBoxStory )
    {
        // Sonderfall: gesamte(!) Kette soll ermittelt werden,
        //             dann sind wir hier schon fertig!
        if( USHRT_MAX > nSequence )
        {
            long nMinStartCp = rStartCp;
            long nMaxEndCp   = rEndCp;
            // rasch den TextBox-Break-Deskriptor-PLCF greifen
            pT = pPlcxMan->GetTxbxBkd();
            if (!pT) //It can occur on occasion, Caolan
                return FALSE;

            // den ersten Eintrag fuer diese TextBox-Story finden
            if( !pT->SeekPos( rStartCp ) )
            {
                ASSERT( !this, "+Wo ist der Grafik-Text (4) ?" );
                return FALSE;
            }
            // ggfs. entsprechende Anzahl Eintraege weitergehen
            for(USHORT iSequence = 0; iSequence < nSequence; iSequence++) (*pT)++;
            // dann die tatsaechlichen Start und Ende ermitteln
            if(    (!pT->Get( rStartCp, pT0 ))
                || ( nMinStartCp > rStartCp  ) )
            {
                ASSERT( !this, "+Wo ist der Grafik-Text (5) ?" );
                return FALSE;
            }
            if( rStartCp >= nMaxEndCp )
                rEndCp = rStartCp;  // kein Error: leerer String!
            else
            {
                (*pT)++;
                if(    (!pT->Get( rEndCp, pT0 ))
                    || ( nMaxEndCp < rEndCp-1  ) )
                {
                    ASSERT( !this, "+Wo ist der Grafik-Text (6) ?" );
                    return FALSE;
                }
                rEndCp -= 1;
            }
        }
        else
            rEndCp -= 1;
    }
    else
        rEndCp -= 1;
    return TRUE;
}

// TxbxText() holt aus WW-File den Text und gibt diesen und den Anfangs- und
// den um -2 (bzw. -1 bei Ver8) korrigierten End-Cp zurueck


BOOL SwWW8ImplReader::GetTxbxText( String& rString,
                                   long nStartCp, long nEndCp )
{
    nDrawTxbx++;
    BOOL bOk = FALSE;
    if( nStartCp > nEndCp )
    {
        ASSERT( !this, "+Wo ist der Grafik-Text (7) ?" );
    }
    else
    if( nStartCp == nEndCp )
        rString.Erase();	  // leerer String: durchaus denkbar!
    else
    {
        // den Text einlesen: kann sich ueber mehrere Pieces erstrecken!!!
        USHORT nLen = pSBase->WW8ReadString( *pStrm, rString,
                                            nStartCp + nDrawCpO,
                                            nEndCp   - nStartCp,
                                            eTextCharSet );
        if( !nLen )
        {
            ASSERT( !this, "+Wo ist der Grafik-Text (8) ?" );
        }
        else
        {
            bOk = TRUE;
            if( 0x0d == rString.GetChar(nLen - 1) )
                rString.Erase(nLen - 1);
        }
    }
    return bOk;
}


// InsertTxbxText() fuegt fuer TextBoxen und CaptionBoxen den Text
// und die Attribute ein
SwFrmFmt* SwWW8ImplReader::InsertTxbxText(SdrTextObj* pTextObj,
                                     Size*		 pObjSiz,
                                     USHORT		 nTxBxS,
                                     USHORT		 nSequence,
                                     long		 nPosCp,
                                     SwFrmFmt*	 pOldFlyFmt,
                                     BOOL		 bMakeSdrGrafObj,
                                     BOOL&		 rbEraseTextObj,
                                     BOOL*		 pbTestTxbxContainsText,
                                     long*		 pnStartCp,
                                     long*		 pnEndCp,
                                     BOOL*       pbContainsGraphics,
                                     SvxMSDffImportRec* pRecord)
{
    SwFrmFmt* pFlyFmt = 0;
    ULONG nOld = pStrm->Tell();

    rbEraseTextObj = FALSE;

    String aString;
    long nStartCp, nEndCp, nNewStartCp;
    BOOL bContainsGraphics=FALSE;
    BOOL bTextWasRead = GetTxbxTextSttEndCp( nStartCp, nEndCp, nTxBxS,
        nSequence ) && GetTxbxText( aString, nStartCp, nEndCp );

    nNewStartCp = nStartCp;

    if( !pbTestTxbxContainsText )
    {
        if( bTextWasRead )
            while( STRING_NOTFOUND != aString.SearchAndReplace( 0xb, ' ' ))
                ;	// HardNewline kann EE noch nicht in der EE-Core

        if( !pDrawEditEngine )
#if SUPD>601
            pDrawEditEngine = new EditEngine(0);
#else
            pDrawEditEngine = new EditEngine;
#endif
        if( pObjSiz )
            pDrawEditEngine->SetPaperSize( *pObjSiz );
    }

    if( bTextWasRead )
    {
        for(USHORT i=0; i < aString.Len(); i++)
        {
            if( 0x13 == aString.GetChar( i ) )
            {
                do
                {
                    aString.Erase( i, 1 );
                    nNewStartCp++;
                }
                while(              aString.Len()
                        && (   i  < aString.Len())
                        && (0x14 != aString.GetChar( i ) )
                        && (0x15 != aString.GetChar( i ) ) );
                if( aString.Len() )
                {
                    if( 0x14 == aString.GetChar( i ) )
                    {
                        aString.Erase( i, 1 );
                        nNewStartCp++;
                        do
                        {
                            i++;
                        }
                        while(              aString.Len()
                                && (   i  < aString.Len())
                                && (0x15 != aString.GetChar( i ) ) );
                        if( i < aString.Len() )
                            aString.Erase( i, 1 );
                    }
                    else if( 0x15 == aString.GetChar( i ) )
                        aString.Erase( i, 1 );
                }
            }
        }

        if (1 != aString.Len())
        {
            if ( (STRING_NOTFOUND != aString.Search(0x1)) ||
                (STRING_NOTFOUND != aString.Search(0x8)) )
                bContainsGraphics=TRUE;
        }
        else        // May be a single graphic or object
        {
            BOOL bDone = TRUE;
            switch( aString.GetChar(0) )
            {
                case 0x1:
                    if( !pbTestTxbxContainsText )
                    {
                        WW8ReaderSave aSave(this,nNewStartCp + nDrawCpO -1);
                        BOOL bOldEmbeddObj = bEmbeddObj;
                        //bEmbedd Ordinarily would have been set by field
                        //parse, but this is impossible here so...
                        bEmbeddObj = TRUE;

                        // 1st look for OLE- or Graph-Indicator Sprms
                        WW8PLCFx_Cp_FKP* pChp = pPlcxMan->GetChpPLCF();
                        WW8PLCFxDesc aDesc;
                        pChp->GetSprms( &aDesc );
                        WW8SprmIter aSprmIter( aDesc.pMemPos, aDesc.nSprmsLen,
                            GetFib().nVersion );

                        const BYTE* pParams = aSprmIter.GetAktParams();
                        for( int nLoop = 0; nLoop < 2; ++nLoop )
                        {
                            while( aSprmIter.GetSprms()
                                && (0 != (pParams = aSprmIter.GetAktParams())) )
                            {
                                USHORT nAktId = aSprmIter.GetAktId();
                                switch( nAktId )
                                {
                                    case     75:
                                    case    118:
                                    case 0x080A:
                                    case 0x0856:
                                            Read_Obj(nAktId, pParams, 1);
                                        break;
                                    case     68:  // Read_Pic()
                                    case 0x6A03:
                                    case 0x680E:
                                            Read_PicLoc(nAktId, pParams, 1);
                                        break;
                                }
                                aSprmIter++;
                            }

                            //if( bRead_Obj || bRead_PicLoc ) break;
                            if( !nLoop )
                            {
                                pChp->GetPCDSprms(  aDesc );
                                aSprmIter.SetSprms( aDesc.pMemPos,
                                    aDesc.nSprmsLen );
                            }
                            //if( bRead_Obj || bRead_PicLoc ) break;
                        }
                        aSave.Restore(this);
                        bEmbeddObj=bOldEmbeddObj;

                        // then import either an OLE of a Graphic
                        if( bObj )
                        {
                            if( bMakeSdrGrafObj && pTextObj &&
                                pTextObj->GetUpGroup() )
                            {
                                // SdrOleObj/SdrGrafObj anstatt des
                                // SdrTextObj in dessen Gruppe einsetzen

                                Graphic aGraph;
                                SdrObject* pNew = ImportOleBase( aGraph );

                                if( !pNew )
                                {
                                    pNew = new SdrGrafObj;
                                    ((SdrGrafObj*)pNew)->SetGraphic(aGraph);
                                }

                                if( !pDrawModel )
                                    GrafikCtor();

                                pNew->SetModel( pDrawModel );
                                pNew->SetLogicRect( pTextObj->GetBoundRect() );
                                pNew->SetLayer( pTextObj->GetLayer() );

                                pTextObj->GetUpGroup()->GetSubList()->
                                    ReplaceObject(pNew, pTextObj->GetOrdNum());
                            }
                            else
                                pFlyFmt = ImportOle();
                            bObj = FALSE;
                        }
                        else
                        {
                            InsertTxbxAttrs(nNewStartCp,nNewStartCp+1,TRUE);
                            pFlyFmt = ImportGraf(bMakeSdrGrafObj ? pTextObj : 0,
                                pOldFlyFmt,
                                pTextObj ?  (nDrawHell == pTextObj->GetLayer())
                                : FALSE );
                        }
                    }
                    break;
                case 0x8:
                    if ( (!pbTestTxbxContainsText) && (!bObj) )
                        pFlyFmt = Read_GrafLayer( nPosCp );
                    break;
                default:
                    bDone = FALSE;
                    break;
            }

            if( bDone )
            {
                if( pFlyFmt )
                {
                    if( pRecord )
                    {
                        SfxItemSet aFlySet( rDoc.GetAttrPool(),
                            RES_FRMATR_BEGIN, RES_FRMATR_END-1 );

                        Rectangle aInnerDist(	pRecord->nDxTextLeft,
                                                pRecord->nDyTextTop,
                                                pRecord->nDxTextRight,
                                                pRecord->nDyTextBottom  );
                        MatchSdrItemsIntoFlySet( pTextObj,
                                                 aFlySet,
                                                 pRecord->eLineStyle,
                                                 pRecord->eShapeType,
                                                 aInnerDist );

                        pFlyFmt->SetAttr( aFlySet );

                        MatchWrapDistancesIntoFlyFmt( pRecord, pFlyFmt );
                    }
                }
                aString.Erase();
                rbEraseTextObj = (0 != pFlyFmt);
            }
        }
    }

    if( pnStartCp )
        *pnStartCp = nStartCp;
    if( pnEndCp )
        *pnEndCp = nEndCp;

    if( pbTestTxbxContainsText )
        *pbTestTxbxContainsText = bTextWasRead && ! rbEraseTextObj;
    else if( !rbEraseTextObj )
    {
        if( bTextWasRead )
        {
            pDrawEditEngine->SetText( aString );
            InsertTxbxAttrs( nStartCp, nEndCp, FALSE );
        }

        BOOL bVertical = pTextObj->IsVerticalWriting();
        OutlinerParaObject* pOp = new OutlinerParaObject(
            *pDrawEditEngine->CreateTextObject());
        pOp->SetOutlinerMode( OUTLINERMODE_TEXTOBJECT );
        pOp->SetVertical( bVertical );
        pTextObj->NbcSetOutlinerParaObject( pOp );
        pTextObj->SetVerticalWriting(bVertical);

        // Fuer die naechste Textbox noch die alten Absatz-Attribute
        // und Styles entfernen, sonst startet die naechste Box
        // mit falschen Attributen.
        // Vorgehen: Text loeschen = auf 1 Absatz reduzieren
        // und an diesem Absatz die Absatzattribute und Styles loeschen
        // (Empfehlung JOE)
        pDrawEditEngine->SetText( aEmptyStr );
        pDrawEditEngine->SetParaAttribs(0, pDrawEditEngine->GetEmptyItemSet());
    }

    pStrm->Seek( nOld );
    if (pbContainsGraphics)
        *pbContainsGraphics = bContainsGraphics;
    return pFlyFmt;
}

BOOL SwWW8ImplReader::TxbxChainContainsRealText( USHORT nTxBxS,
                                                 long&  rStartCp,
                                                 long&  rEndCp )
{
    BOOL bErase, bContainsText;
    InsertTxbxText( 0,0,nTxBxS,USHRT_MAX,0,0,0, bErase, &bContainsText,
        &rStartCp, &rEndCp );
    return bContainsText;
}


// TextBoxes only for Ver67 !!
void SwWW8ImplReader::ReadTxtBox( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    BOOL bDummy;
    WW8_DP_TXTBOX aTxtB;

    if( !ReadGrafStart( (void*)&aTxtB, sizeof( aTxtB ), pHd, pDo ) )
        return;

    Point aP0( (INT16)SVBT16ToShort( pHd->xa ) + nDrawXOfs2,
               (INT16)SVBT16ToShort( pHd->ya ) + nDrawYOfs2 );
    Point aP1( aP0 );
    aP1.X() += (INT16)SVBT16ToShort( pHd->dxa );
    aP1.Y() += (INT16)SVBT16ToShort( pHd->dya );

    SdrObject* pObj = new SdrRectObj( OBJ_TEXT, Rectangle( aP0, aP1 ) );
    pObj->SetModel( pDrawModel );
    Size aSize( (INT16)SVBT16ToShort( pHd->dxa ) ,
        (INT16)SVBT16ToShort( pHd->dya ) );

    long nStartCpFly,nEndCpFly;
    BOOL bContainsGraphics;
    InsertTxbxText(PTR_CAST(SdrTextObj,pObj), &aSize, 0, 0, 0, 0, FALSE,
        bDummy,0,&nStartCpFly,&nEndCpFly,&bContainsGraphics);

    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    SfxAllItemSet aSet( pDrawModel->GetItemPool() );

    SetStdAttr( aSet, aTxtB.aLnt, aTxtB.aShd );
    SetFill( aSet, aTxtB.aFill );

    aSet.Put( SdrTextFitToSizeTypeItem( SDRTEXTFIT_NONE ) );
    aSet.Put( SdrTextAutoGrowWidthItem(  FALSE ) );
    aSet.Put( SdrTextAutoGrowHeightItem( FALSE ) );
    aSet.Put( SdrTextLeftDistItem(  MIN_BORDER_DIST*2 ) );
    aSet.Put( SdrTextRightDistItem( MIN_BORDER_DIST*2 ) );
    aSet.Put( SdrTextUpperDistItem( MIN_BORDER_DIST ) );
    aSet.Put( SdrTextLowerDistItem( MIN_BORDER_DIST ) );

    pObj->SetItemSetAndBroadcast(aSet);

    //Cannot properly have draw objects in header, here with txtbox we can
    //convert it successfully to a flyframe, nor can we support graphics
    //in text drawboxes
    if (bHdFtFtnEdn || bContainsGraphics)
    {
        SfxItemSet aFlySet(rDoc.GetAttrPool(), RES_FRMATR_BEGIN,
            RES_FRMATR_END-1);

        aFlySet.Put(SwFmtFrmSize(ATT_MIN_SIZE, aSize.Width(), aSize.Height()));
        aFlySet.Put(SwFmtSurround(SURROUND_THROUGHT));
        if (SVBT16ToShort( pDo->dhgt ) & 0x2000)
            aFlySet.Put(SvxOpaqueItem(RES_OPAQUE,FALSE));

        //InnerDist is all 0 as word 6 doesn't store distance from borders and
        //neither does it store the border style so its always simple
        Rectangle aInnerDist(Point(0,0),Point(0,0));
        MatchSdrItemsIntoFlySet( pObj, aFlySet, mso_lineSimple, mso_sptMin,
            aInnerDist );

        //undo the anchor setting for draw graphics, remove the setting in the
        //control stack, get the id from the drawpg and reuse it in the new
        //flyframe and then remove it as well
        pCtrlStck->DeleteAndDestroy(pCtrlStck->Count()-1);
        const SwFmtAnchor &rAnchor = (const SwFmtAnchor&)pDrawFmt->GetAttr(
            RES_ANCHOR);
        SwFlyFrmFmt *pRetFrmFmt = rDoc.MakeFlySection( rAnchor.GetAnchorId(),
            pPaM->GetPoint(), &aFlySet );
        pDrawFmt->ResetAttr( RES_ANCHOR );

        if (nEndCpFly-nStartCpFly)
        {
            WW8ReaderSave aSave( this );

            // set Pam into the FlyFrame
            const SwFmtCntnt& rCntnt = pRetFrmFmt->GetCntnt();
            ASSERT( rCntnt.GetCntntIdx(), "Oops" );
            pPaM->GetPoint()->nNode = rCntnt.GetCntntIdx()->GetIndex() + 1;
            pPaM->GetPoint()->nContent.Assign( pPaM->GetCntntNode(), 0 );

            ReadText( nStartCpFly, (nEndCpFly-nStartCpFly),
                  MAN_MAINTEXT == pPlcxMan->GetManType()
                ? MAN_TXBX
                : MAN_TXBX_HDFT );

            aSave.Restore( this );
        }

        if( pObj->GetPage() )
            pDrawPg->RemoveObject( pObj->GetOrdNum() );
        delete pObj;

        SdrObject* pOurNewObject = CreateContactObject( pRetFrmFmt );
        if( pOurNewObject && !pOurNewObject->IsInserted() )
            pDrawPg->InsertObject( pOurNewObject );
    }
}

void SwWW8ImplReader::ReadCaptionBox( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    static SdrCaptionType aCaptA[] = { SDRCAPT_TYPE1, SDRCAPT_TYPE2,
                                       SDRCAPT_TYPE3, SDRCAPT_TYPE4 };

    WW8_DP_CALLOUT_TXTBOX aCallB;

    if( !ReadGrafStart( (void*)&aCallB, sizeof( aCallB ), pHd, pDo ) )
        return;

    UINT16 nCount = SVBT16ToShort( aCallB.dpPolyLine.aBits1 ) >> 1 & 0x7fff;
    SVBT16 *pP = new SVBT16[nCount * 2];
    pStrm->Read( pP, nCount * 4 );		// Punkte einlesen
    BYTE nTyp = (BYTE)nCount - 1;
    if( nTyp == 1 && SVBT16ToShort( pP[0] ) == SVBT16ToShort( pP[2] ) )
        nTyp = 0;

    Point aP0( (INT16)SVBT16ToShort( pHd->xa ) +
               (INT16)SVBT16ToShort( aCallB.dpheadTxbx.xa )	+ nDrawXOfs2,
               (INT16)SVBT16ToShort( pHd->ya )
               + (INT16)SVBT16ToShort( aCallB.dpheadTxbx.ya ) + nDrawYOfs2 );
    Point aP1( aP0 );
    aP1.X() += (INT16)SVBT16ToShort( aCallB.dpheadTxbx.dxa );
    aP1.Y() += (INT16)SVBT16ToShort( aCallB.dpheadTxbx.dya );
    Point aP2( (INT16)SVBT16ToShort( pHd->xa )
                + (INT16)SVBT16ToShort( aCallB.dpheadPolyLine.xa )
                + nDrawXOfs2 + (INT16)SVBT16ToShort( pP[0] ),
               (INT16)SVBT16ToShort( pHd->ya )
               + (INT16)SVBT16ToShort( aCallB.dpheadPolyLine.ya )
               + nDrawYOfs2	+ (INT16)SVBT16ToShort( pP[1] ) );
    delete[] pP;

    SdrCaptionObj* pObj = new SdrCaptionObj( Rectangle( aP0, aP1 ), aP2 );
    Size aSize( (INT16)SVBT16ToShort( aCallB.dpheadTxbx.dxa ),
                           (INT16)SVBT16ToShort(  aCallB.dpheadTxbx.dya ) );
    BOOL bEraseThisObject;

    InsertTxbxText(pObj, &aSize, 0, 0, 0, 0, FALSE, bEraseThisObject );

    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    SfxAllItemSet aSet( pDrawModel->GetItemPool() );

    if( SVBT16ToShort( aCallB.dptxbx.aLnt.lnps ) != 5 )	// Umrandung sichtbar ?
        SetStdAttr( aSet, aCallB.dptxbx.aLnt, aCallB.dptxbx.aShd );
    else												// nein -> Nimm Linie
        SetStdAttr( aSet, aCallB.dpPolyLine.aLnt, aCallB.dptxbx.aShd );
    SetFill( aSet, aCallB.dptxbx.aFill );
    aSet.Put( SdrCaptionTypeItem( aCaptA[nTyp] ) );

    pObj->SetItemSetAndBroadcast(aSet);
}


void SwWW8ImplReader::ReadGroup( WW8_DPHEAD* pHd, WW8_DO* pDo )
{
    INT16 nGrouped;

    if( !ReadGrafStart( (void*)&nGrouped, sizeof( nGrouped ), pHd, pDo ) )
        return;

#ifdef __BIGENDIAN
    nGrouped = (INT16)SWAPSHORT( nGrouped );
#endif

    nDrawXOfs += (INT16)SVBT16ToShort( pHd->xa );
    nDrawYOfs += (INT16)SVBT16ToShort( pHd->ya );

    SdrObject* pObj = new SdrObjGroup;

    pWWZOrder->InsertDrawingObject(pObj, pDrawFmt, SVBT16ToShort(pDo->dhgt));

    pWWZOrder->EnterDrawingGroup(pObj->GetSubList());

    short nLeft = (INT16)SVBT16ToShort( pHd->cb ) - sizeof( WW8_DPHEAD );
    for (int i = 0; i < nGrouped; i++)
        ReadGrafPrimitive( nLeft, pDo );

    pWWZOrder->LeaveDrawingGroup();

    nDrawXOfs -= (INT16)SVBT16ToShort( pHd->xa );
    nDrawYOfs -= (INT16)SVBT16ToShort( pHd->ya );
}

void SwWW8ImplReader::ReadGrafPrimitive( short& rLeft, WW8_DO* pDo )
{
    WW8_DPHEAD aHd;							// Lese Draw-Primitive-Header
    pStrm->Read( &aHd, sizeof( WW8_DPHEAD ) );

    if( rLeft >= (INT16)SVBT16ToShort( aHd.cb ) )	// Vorsichtsmassmahme
    {
        switch( (INT16)SVBT16ToShort( aHd.dpk ) & 0xff )
        {
        case 0:	 ReadGroup( &aHd, pDo );
                 break;
        case 1:  ReadLine( &aHd, pDo );
                 break;
        case 2:  ReadTxtBox( &aHd, pDo );
                 break;
        case 3:  ReadRect( &aHd, pDo );
                 break;
        case 4:  ReadElipse( &aHd, pDo );
                 break;
        case 5:  ReadArc( &aHd, pDo );
                 break;
        case 6:  ReadPolyLine( &aHd, pDo );
                 break;
        case 7:  ReadCaptionBox( &aHd, pDo );
                 break;
        default: 							// unbekannt
                 pStrm->SeekRel( (INT16)SVBT16ToShort( aHd.cb )
                                 - sizeof( WW8_DPHEAD )  );
                 break;
        }
    }
    else
    {
        ASSERT( !this, "+Grafik-Overlap" );
    }
    rLeft -= (INT16)SVBT16ToShort( aHd.cb );
}

void SwWW8ImplReader::ReadGrafLayer1( WW8PLCFspecial* pPF, long nGrafAnchorCp )
{
    pPF->SeekPos( nGrafAnchorCp );
    long nStartFc;
    void* pF0;
    if( !pPF->Get( nStartFc, pF0 ) )
    {
        ASSERT( !this, "+Wo ist die Grafik (2) ?" );
        return;
    }
    WW8_FDOA* pF = (WW8_FDOA*)pF0;
    if( !SVBT32ToLong( pF->fc ) )
    {
        ASSERT( !this, "+Wo ist die Grafik (3) ?" );
        return;
    }
    WW8_DO aDo;
    pStrm->Seek( SVBT32ToLong( pF->fc ) );           		// Lese Draw-Header
    pStrm->Read( &aDo, sizeof( WW8_DO ) );

    short nLeft = SVBT16ToShort( aDo.cb ) - sizeof( WW8_DO );
    while( nLeft > sizeof( WW8_DPHEAD ) )
    {
        ReadGrafPrimitive( nLeft, &aDo );
    }
}

INT32 SwMSDffManager::GetEscherLineMatch(MSO_LineStyle eStyle,
    MSO_SPT eShapeType, INT32 &rThick)
{
    INT32 nOutsideThick = 0;
    /*
    Beachte: im Gegensatz zu den Winword-ueblichen Tabellen- und
    Rahmen-Randbreiten-Angaben, bei denen jeweils aus der Staerke *einer*
    Linie die Gesamt-Randbreite zu errechnen ist, liegen die aus dem ESCHER
    stammenden Daten bereits als Gesamt-Breite [twips] vor!

    Der Winword default ist 15 tw. Wir nehmen hierfuer unsere 20 tw Linie.  (
    0.75 pt uns 1.0 pt sehen sich auf dem Ausdruck naemlich aehnlicher als
    etwas 0.75 pt und unsere 0.05 pt Haarlinie. ) Die Haarlinie setzen wir nur
    bei Winword-Staerken bis zu maximal 0.5 pt ein.
    */
    switch( eStyle )
    {
    case mso_lineTriple:
    case mso_lineSimple:
        nOutsideThick = eShapeType != mso_sptTextBox ? rThick : rThick/2;
        break;
    case mso_lineDouble:
        if (eShapeType == mso_sptTextBox)
        {
            nOutsideThick = rThick/6;
            rThick = rThick*2/3;
        }
        else
            nOutsideThick = rThick*2/3;
        break;
    case mso_lineThickThin:
        if (eShapeType == mso_sptTextBox)
        {
            nOutsideThick = rThick*3/10;
            rThick = rThick*4/5;
        }
        else
            nOutsideThick = rThick*4/5;
        break;
    case mso_lineThinThick:
        {
        if (eShapeType == mso_sptTextBox)
        {
            nOutsideThick = rThick/10;
            rThick = rThick*3/5;
        }
        else
            nOutsideThick = rThick*3/5;
        }
        break;
    default:
        break;
    }
    return nOutsideThick;
}

//Returns the thickness of the line outside the frame, the logic of
//words positioning of borders around floating objects is that of a
//disturbed mind.
INT32 SwWW8ImplReader::MatchSdrBoxIntoFlyBoxItem(const Color& rLineColor,
    MSO_LineStyle eLineStyle, MSO_SPT eShapeType, INT32 &rLineThick,
    SvxBoxItem& rBox )
{
    INT32 nOutsideThick = 0;
    if( !rLineThick )
        return nOutsideThick;

    // Deklarationen gemaess BOXITEM.HXX
    WW8_DECL_LINETAB_ARRAY

    USHORT nIdx = USHRT_MAX;

    INT32 nLineThick=rLineThick;
    nOutsideThick = SwMSDffManager::GetEscherLineMatch(eLineStyle,
        eShapeType, rLineThick);

    /*
    Beachte: im Gegensatz zu den Winword-ueblichen Tabellen- und
    Rahmen-Randbreiten-Angaben, bei denen jeweils aus der Staerke *einer*
    Linie die Gesamt-Randbreite zu errechnen ist, liegen die aus dem ESCHER
    stammenden Daten bereits als Gesamt-Breite [twips] vor!

    Der Winword default ist 15 tw. Wir nehmen hierfuer unsere 20 tw Linie.  (
    0.75 pt uns 1.0 pt sehen sich auf dem Ausdruck naemlich aehnlicher als
    etwas 0.75 pt und unsere 0.05 pt Haarlinie. ) Die Haarlinie setzen wir nur
    bei Winword-Staerken bis zu maximal 0.5 pt ein.
    */
    switch( eLineStyle )
    {
    // zuerst die Einzel-Linien
    case mso_lineSimple:
        if( nLineThick < 20)
            nIdx =            0;//   1 Twip bei uns
        else if( nLineThick < 50)
            nIdx =            1;//  20 Twips
        else if( nLineThick < 80)
            nIdx =            2;//  50
        else if( nLineThick < 100)
            nIdx =            3;//  80
        else if( nLineThick <150)
            nIdx =            4;// 100
        // Pfusch: fuer die ganz dicken Linien muessen wir doppelte Linien
        // malen, weil unsere Einfach-Linie nicht dicker als 5 Punkt wird
        else if( nLineThick <180)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+2;// 150
        else
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+5;// 180
    break;
    // dann die Doppel-Linien, fuer die wir feine Entsprechungen haben :-)))
    case mso_lineDouble:
        if( nLineThick <  60)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 0;//  22 Twips bei uns
        else if( nLineThick < 135)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 7;// some more space
        else if( nLineThick < 180)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 1;//  60
        else
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 2;// 150
    break;
    case mso_lineThickThin:
        if( nLineThick <  87)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 8;//  71 Twips bei uns
        else if( nLineThick < 117)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 9;// 101
        else if( nLineThick < 166)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+10;// 131
        else
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 5;// 180
    break;
    case mso_lineThinThick:
        if( nLineThick < 137)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 4;//  90 Twips bei uns
        else
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 6;// 180
    break;
    // zu guter Letzt die Dreifach-Linien, an deren Stelle wir eine
    // Doppel-Linie setzen
    case mso_lineTriple:
        if( nLineThick <  46)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 0;//  22 Twips bei uns
        else if( nLineThick < 106)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 1;//  60
        else if( nLineThick < 166)
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 2;// 150
        else
            nIdx = WW8_DECL_LINETAB_OFS_DOUBLE+ 5;// 180
    break;
    // no line style is set
    case (MSO_LineStyle)USHRT_MAX:
        break;
    // erroneously not implemented line style is set
    default:
        ASSERT( !this, "eLineStyle is not (yet) implemented!" );
        break;
    }

    if( USHRT_MAX != nIdx )
    {
        SvxBorderLine aLine;
        aLine.SetColor( rLineColor );

        const WW8_BordersSO& rBorders = nLineTabVer8[ nIdx ];
        aLine.SetOutWidth( rBorders.Out  );
        aLine.SetInWidth ( rBorders.In   );
        aLine.SetDistance( rBorders.Dist );

        for(USHORT nLine = 0; nLine < 4; ++nLine)
            rBox.SetLine(new SvxBorderLine( aLine ), nLine);
    }

    return nOutsideThick;
}


#define WW8ITEMVALUE(ItemSet,Id,Cast)  ((const Cast&)(ItemSet).Get(Id)).GetValue()

void SwWW8ImplReader::MatchSdrItemsIntoFlySet( SdrObject* pSdrObj,
    SfxItemSet& rFlySet, MSO_LineStyle eLineStyle, MSO_SPT eShapeType,
    Rectangle& rInnerDist )
{
/*
    am Rahmen zu setzende Frame-Attribute
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SwFmtFrmSize 			falls noch nicht gesetzt, hier setzen
    SvxLRSpaceItem			hier setzen
    SvxULSpaceItem			hier setzen
    SvxOpaqueItem 			(Derzeit bei Rahmen nicht moeglich! khz 10.2.1999)
    SwFmtSurround 			bereits gesetzt
    SwFmtVertOrient 		bereits gesetzt
    SwFmtHoriOrient 		bereits gesetzt
    SwFmtAnchor 			bereits gesetzt
    SvxBoxItem				hier setzen
    SvxBrushItem			hier setzen
    SvxShadowItem 			hier setzen
*/

    // 1. GrafikObjekt des Docs?
    if( !pDrawModel )
        GrafikCtor();

    const SfxItemSet& rOldSet = pSdrObj->GetItemSet();

    // einige Items koennen direkt so uebernommen werden
    const USHORT nDirectMatch = 2;
    static RES_FRMATR __READONLY_DATA aDirectMatch[ nDirectMatch ] =
    {
        RES_LR_SPACE,	// Aussenabstand links/rechts: SvxLRSpaceItem
        RES_UL_SPACE	// Aussenabstand Oben/unten:   SvxULSpaceItem
    };
    const SfxPoolItem* pPoolItem;
    for(USHORT nItem = 0; nItem < nDirectMatch; ++nItem)
        if( SFX_ITEM_SET == rOldSet.GetItemState( aDirectMatch[ nItem ], FALSE,
            &pPoolItem) )
        {
            rFlySet.Put( *pPoolItem );
        }


    // jetzt die Umrandung berechnen und die Box bauen: Das Mass wird fuer die
    // Rahmen-GROESSE benoetigt!
    SvxBoxItem aBox;
    // dashed oder solid wird zu solid
    INT32 nLineThick = 0, nOutside=0;

    // check if LineStyle is *really* set!
    const SfxPoolItem* pItem;

    SfxItemState eState = rOldSet.GetItemState(XATTR_LINESTYLE,TRUE,&pItem);
    if( eState == SFX_ITEM_SET )
    {
        // Now, that we know there is a line style we will make use the
        // parameter given to us when calling the method...  :-)
        const Color aLineColor = WW8ITEMVALUE(rOldSet, XATTR_LINECOLOR,
            XLineColorItem);
        nLineThick = WW8ITEMVALUE(rOldSet, XATTR_LINEWIDTH, XLineWidthItem);

        if( !nLineThick )
            nLineThick = 15; // WW-default: 0.75 pt

        nOutside = MatchSdrBoxIntoFlyBoxItem(aLineColor, eLineStyle,
            eShapeType, nLineThick, aBox);
    }

    rInnerDist.Left()+=nLineThick;
    rInnerDist.Top()+=nLineThick;
    rInnerDist.Right()+=nLineThick;
    rInnerDist.Bottom()+=nLineThick;

    const SvxBorderLine *pLine;
    if (pLine = aBox.GetLine(BOX_LINE_LEFT))
    {
        rInnerDist.Left() -= (pLine->GetOutWidth() + pLine->GetInWidth() +
            pLine->GetDistance());
    }

    if (pLine = aBox.GetLine(BOX_LINE_TOP))
    {
        rInnerDist.Top() -= (pLine->GetOutWidth() + pLine->GetInWidth() +
            pLine->GetDistance());
    }

    if (pLine = aBox.GetLine(BOX_LINE_RIGHT))
    {
        rInnerDist.Right() -= (pLine->GetOutWidth() + pLine->GetInWidth() +
            pLine->GetDistance());
    }

    if (pLine = aBox.GetLine(BOX_LINE_BOTTOM))
    {
        rInnerDist.Bottom() -= (pLine->GetOutWidth() + pLine->GetInWidth() +
            pLine->GetDistance());
    }

    // set distances from box's border to text contained within the box
    if( 0 < rInnerDist.Left() )
        aBox.SetDistance( (USHORT)rInnerDist.Left(), BOX_LINE_LEFT );
    if( 0 < rInnerDist.Top() )
        aBox.SetDistance( (USHORT)rInnerDist.Top(), BOX_LINE_TOP );
    if( 0 < rInnerDist.Right() )
        aBox.SetDistance( (USHORT)rInnerDist.Right(), BOX_LINE_RIGHT );
    if( 0 < rInnerDist.Bottom() )
        aBox.SetDistance( (USHORT)rInnerDist.Bottom(), BOX_LINE_BOTTOM );

    BOOL bFixSize = !(WW8ITEMVALUE(rOldSet, SDRATTR_TEXT_AUTOGROWHEIGHT, 
        SdrTextAutoGrowHeightItem));

    // Size: SwFmtFrmSize
    if( SFX_ITEM_SET != rFlySet.GetItemState(RES_FRM_SIZE, FALSE) )
    {
        const Rectangle& rSnapRect = pSdrObj->GetSnapRect();
        // if necessary adapt width and position of the framework: The
        // recorded interior is to remain equally large despite thick edges.
        rFlySet.Put( SwFmtFrmSize(bFixSize ? ATT_FIX_SIZE : ATT_VAR_SIZE,
            rSnapRect.GetWidth()  + 2*nOutside,
            rSnapRect.GetHeight() + 2*nOutside) );
    }
    else //If a size is set, adjust it to consider border thickness
    {
        SwFmtFrmSize aSize = (const SwFmtFrmSize &)(rFlySet.Get(RES_FRM_SIZE));
        rFlySet.Put( SwFmtFrmSize(bFixSize ? ATT_FIX_SIZE : ATT_VAR_SIZE,
            aSize.GetWidth()  + 2*nOutside,
            aSize.GetHeight() + 2*nOutside) );
    }

    //Sadly word puts escher borders outside the graphic, but orients the
    //graphic in relation to the top left inside the border. We don't
    if (nOutside)
    {
        SwFmtHoriOrient aHori = (const SwFmtHoriOrient &)(rFlySet.Get(
            RES_HORI_ORIENT));
        aHori.SetPos(aHori.GetPos()-nOutside);
        rFlySet.Put(aHori);

        SwFmtVertOrient aVert = (const SwFmtVertOrient &)(rFlySet.Get(
            RES_VERT_ORIENT));
        aVert.SetPos(aVert.GetPos()-nOutside);
        rFlySet.Put(aVert);
    }

    // jetzt die Umrandung setzen
    rFlySet.Put( aBox );

    // Schattenwurf der Box: SvxShadowItem
    if( WW8ITEMVALUE(rOldSet, SDRATTR_SHADOW, SdrShadowItem) )
    {
        SvxShadowItem aShadow;

        const Color aShdColor = WW8ITEMVALUE(rOldSet, SDRATTR_SHADOWCOLOR,
            SdrShadowColorItem);
        const INT32 nShdDistX = WW8ITEMVALUE(rOldSet, SDRATTR_SHADOWXDIST,
            SdrShadowXDistItem);
        const INT32 nShdDistY = WW8ITEMVALUE(rOldSet, SDRATTR_SHADOWYDIST,
            SdrShadowYDistItem);
        //const USHORT nShdTrans= WW8ITEMVALUE(rOldSet,
        //   SDRATTR_SHADOWTRANSPARENCE, SdrShadowTransparenceItem);

        // diese gibt es im Writer nicht  :-(
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //
        // SfxVoidItem( SDRATTR_SHADOW3D    )
        // SfxVoidItem( SDRATTR_SHADOWPERSP )

        aShadow.SetColor( Color( aShdColor ) );

        aShadow.SetWidth( (Abs( nShdDistX ) + Abs( nShdDistY )) / 2 );

        SvxShadowLocation eShdPosi;
        if( 0 <= nShdDistX )
        {
            if( 0 <= nShdDistY )
                eShdPosi = SVX_SHADOW_BOTTOMRIGHT;
            else
                eShdPosi = SVX_SHADOW_TOPRIGHT;
        }
        else
        {
            if( 0 <= nShdDistY )
                eShdPosi = SVX_SHADOW_BOTTOMLEFT;
            else
                eShdPosi = SVX_SHADOW_TOPLEFT;
        }
        aShadow.SetLocation( eShdPosi );

        rFlySet.Put( aShadow );
    }

    // Hintergrund: SvxBrushItem
    eState = rOldSet.GetItemState(XATTR_FILLSTYLE,TRUE,&pItem);
    if( eState == SFX_ITEM_SET )
    {
        const XFillStyle eFill = ((const XFillStyleItem*)pItem)->GetValue();

        if(eFill != XFILL_NONE)
        {
            SvxBrushItem aBrushItem;
            BOOL bBrushItemOk = FALSE;

            switch( eFill )
            {
                case XFILL_NONE:
                    {
                        aBrushItem.SetColor( Color( COL_TRANSPARENT ) );
                        bBrushItemOk = TRUE;
                    }
                break;
                case XFILL_SOLID:
                    {
                        const Color aColor =
                            WW8ITEMVALUE(rOldSet, XATTR_FILLCOLOR,
                            XFillColorItem);
                        aBrushItem.SetColor( aColor );
                        bBrushItemOk = TRUE;
                    }
                break;
                case XFILL_GRADIENT:
                break;
                case XFILL_HATCH:
                break;
                case XFILL_BITMAP:
                    {
                        const Graphic aGraphic(WW8ITEMVALUE(rOldSet,
                            XATTR_FILLBITMAP, XFillBitmapItem).GetBitmap());
                        BOOL bTile = WW8ITEMVALUE(rOldSet, XATTR_FILLBMP_TILE,
                            SfxBoolItem);
                        aBrushItem.SetGraphic( aGraphic );
                        aBrushItem.SetGraphicPos( bTile ? GPOS_TILED
                            : GPOS_AREA );
                        bBrushItemOk = TRUE;
                    }
                break;
            }
            if( bBrushItemOk )
                rFlySet.Put( aBrushItem, RES_BACKGROUND );
        }
    }
}


void SwWW8ImplReader::MatchWrapDistancesIntoFlyFmt( SvxMSDffImportRec* pRecord,
                                                    SwFrmFmt*          pFlyFmt )
{
    if( pRecord->nDxWrapDistLeft || pRecord->nDxWrapDistRight )
    {
        SvxLRSpaceItem aLR;
        aLR.SetLeft(pRecord->nDxWrapDistLeft);
        aLR.SetRight(pRecord->nDxWrapDistRight);
        pFlyFmt->SetAttr( aLR );
    }
    if( pRecord->nDyWrapDistTop || pRecord->nDyWrapDistBottom )
    {
        SvxULSpaceItem aUL;
        aUL.SetUpper(	(USHORT)pRecord->nDyWrapDistTop );
        aUL.SetLower(	(USHORT)pRecord->nDyWrapDistBottom );
        pFlyFmt->SetAttr( aUL );
    }
}


void SwWW8ImplReader::SetAttributesAtGrfNode( SvxMSDffImportRec* pRecord,
    SwFrmFmt *pFlyFmt, WW8_FSPA *pF )
{
    const SwNodeIndex* pIdx = pFlyFmt->GetCntnt( FALSE ).GetCntntIdx();
    SwGrfNode* pGrfNd;
    if( pIdx && (pGrfNd = rDoc.GetNodes()[pIdx->GetIndex() + 1]->GetGrfNode() ))
    {
        Size aSz(pGrfNd->GetTwipSize());
        ULONG rHeight = aSz.Height();
        ULONG rWidth  = aSz.Width();
        if( !rWidth && pF)
            rWidth  = pF->nXaRight  - pF->nXaLeft;
        else if( !rHeight && pF)
            rHeight = pF->nYaBottom - pF->nYaTop;

        if( pRecord->nCropFromTop || pRecord->nCropFromBottom ||
            pRecord->nCropFromLeft || pRecord->nCropFromRight )
        {
            SwCropGrf aCrop;			// Cropping is stored in 'fixed floats'
                                        // 16.16 (it est fraction times total
            if( pRecord->nCropFromTop )	//        image width or height resp.)
                aCrop.SetTop(
                (   ( (pRecord->nCropFromTop    >> 16   ) * rHeight )
                  + (((pRecord->nCropFromTop    & 0xffff) * rHeight ) >> 16) ));
            if( pRecord->nCropFromBottom )
                aCrop.SetBottom(
                (   ( (pRecord->nCropFromBottom >> 16   ) * rHeight )
                  + (((pRecord->nCropFromBottom & 0xffff) * rHeight ) >> 16) ));
            if( pRecord->nCropFromLeft )
                aCrop.SetLeft(
                (   ( (pRecord->nCropFromLeft   >> 16   ) * rWidth  )
                  + (((pRecord->nCropFromLeft   & 0xffff) * rWidth  ) >> 16) ));
            if( pRecord->nCropFromRight )
                aCrop.SetRight(
                (   ( (pRecord->nCropFromRight  >> 16   ) * rWidth  )
                  + (((pRecord->nCropFromRight  & 0xffff) * rWidth  ) >> 16) ));

            pGrfNd->SetAttr( aCrop );
        }

        if (pRecord && pRecord->pObj)
        {
            const SfxItemSet& rOldSet = pRecord->pObj->GetItemSet();
            //contrast
            if (WW8ITEMVALUE(rOldSet, SDRATTR_GRAFCONTRAST,
                SdrGrafContrastItem))
            {
                SwContrastGrf aContrast(
                    WW8ITEMVALUE(rOldSet,
                    SDRATTR_GRAFCONTRAST, SdrGrafContrastItem));
                pGrfNd->SetAttr( aContrast );
            }

            //luminance
            if (WW8ITEMVALUE(rOldSet, SDRATTR_GRAFLUMINANCE,
                SdrGrafLuminanceItem))
            {
                SwLuminanceGrf aLuminance(WW8ITEMVALUE(rOldSet,
                    SDRATTR_GRAFLUMINANCE, SdrGrafLuminanceItem));
                pGrfNd->SetAttr( aLuminance );
            }
            //gamma
            if (WW8ITEMVALUE(rOldSet, SDRATTR_GRAFGAMMA, SdrGrafGamma100Item))
            {
                SwGammaGrf aGamma(WW8ITEMVALUE(rOldSet, SDRATTR_GRAFGAMMA,
                    SdrGrafGamma100Item));
                pGrfNd->SetAttr( aGamma );
            }

            //drawmode
            if (WW8ITEMVALUE(rOldSet, SDRATTR_GRAFMODE, SdrGrafModeItem))
            {
                SwDrawModeGrf aDrawMode(WW8ITEMVALUE(rOldSet, SDRATTR_GRAFMODE,
                    SdrGrafModeItem));
                pGrfNd->SetAttr( aDrawMode );
            }
        }
    }
}

SdrObject* SwWW8ImplReader::CreateContactObject(SwFlyFrmFmt* pFlyFmt)
{
    if (pFlyFmt)
    {
        //JP 11.1.2002: task 96329
        SdrObject* pNewObject = bNew ? 0 : pFlyFmt->FindRealSdrObject();
        if (!pNewObject)
            pNewObject = pFlyFmt->FindSdrObject();
        if (!pNewObject)
        {
            SwFlyDrawContact* pContactObject
                = new SwFlyDrawContact(pFlyFmt, pDrawModel);
            pNewObject = pContactObject->GetMaster();
        }
        return pNewObject;
    }
    return 0;
}

void SwWW8ImplReader::ProcessEscherAlign( SvxMSDffImportRec* pRecord,
    WW8_FSPA *pFSPA, SfxItemSet &rFlySet, BOOL bOrgObjectWasReplace )
{
    if( pRecord )
    {
        // nXAlign - abs. Position, Left,  Centered,  Right,  Inside, Outside
        // nYAlign - abs. Position, Top,   Centered,  Bottom, Inside, Outside

        // nXRelTo - Page printable area, Page,  Column,    Character
        // nYRelTo - Page printable area, Page,  Paragraph, Line

        const int nCntXAlign = 6;
        const int nCntYAlign = 6;

        const int nCntRelTo  = 4;

/*
        // our anchor settings
        static const RndStdIds aAnchorTab[] = {
            FLY_AT_CNTNT,	// Frame bound to paragraph
            FLY_IN_CNTNT,	//             to character
            FLY_PAGE,		//             to page
            FLY_AT_FLY,		//             to another fly ( LAYER_IMPL )
            FLY_AUTO_CNTNT,	// automat. positioned frame bound to paragraph
        };
*/

        // horizontal Adjustment
        static const SwHoriOrient aHoriOriTab[ nCntXAlign ] = {
            HORI_NONE,		// Value of nXPos defined RelPos directly.

            HORI_LEFT,		// automatical adjustment
            HORI_CENTER,	// automatical adjustment
            HORI_RIGHT,		// automatical adjustment

            HORI_LEFT,		// will be converted to HORI_INSIDE when SetPosToggle() called
            HORI_RIGHT		// will be converted to HORI_OUTSIDE...
        };


        // vertical Adjustment
        static const SwVertOrient aVertOriTab[ nCntYAlign ] = {
            VERT_NONE,			// Value of nXPos defined RelPos directly.
            VERT_TOP,			// automatical adjustment
            VERT_CENTER,		// automatical adjustment
            VERT_BOTTOM,		// automatical adjustment
            VERT_LINE_TOP,		// automatical adjustment
            VERT_LINE_BOTTOM	// automatical adjustment
        };

        // Adjustment is relative to...
        static const SwRelationOrient aRelOriTab[ nCntRelTo ] = {
            REL_PG_PRTAREA,	// Page printable area, when bound to page. identical with PRTAREA
            REL_PG_FRAME,	// Page,                when bound to page. identical with FRAME
            FRAME,			// Paragraph Text area
//			PRTAREA,		// Paragraph area
            REL_CHAR 		// to a Character

//			REL_PG_LEFT,	// in left page-border
//			REL_PG_RIGHT,   // in right page-border
//			REL_FRM_LEFT,   // in left paragraph-border
//			REL_FRM_RIGHT,  // in right paragraph-border
        };


        UINT32 nXAlign = nCntXAlign > pRecord->nXAlign ? pRecord->nXAlign : 1;
        UINT32 nYAlign = nCntYAlign > pRecord->nYAlign ? pRecord->nYAlign : 1;

        if (pFSPA)
        {
            /*
            #74188#
            Strangely in this case the FSPA value seems to be considered before
            the newer escher nXRelTo record.
            */
            if ((pRecord->nXRelTo == 2) && (pFSPA->nbx != pRecord->nXRelTo))
                pRecord->nXRelTo = pFSPA->nbx;
            if ((pRecord->nYRelTo == 2) && (pFSPA->nby != pRecord->nYRelTo))
                pRecord->nYRelTo = pFSPA->nby;
        }

        UINT32 nXRelTo = nCntRelTo > pRecord->nXRelTo ? pRecord->nXRelTo : 1;
        UINT32 nYRelTo = nCntRelTo > pRecord->nYRelTo ? pRecord->nYRelTo : 1;

        RndStdIds eAnchor;

        eAnchor = 3 == nXRelTo  ?  FLY_AUTO_CNTNT
            :  2 <= nYRelTo  ?  FLY_AT_CNTNT :  FLY_PAGE;


        //No drawing layer stuff in the header, so if
        //this is going to be in the headerfooter and
        //cannot be replaced by a fly then anchor it
        //to the current page.
        if ((bIsHeader || bIsFooter) && !bOrgObjectWasReplace &&
            !pRecord->bReplaceByFly)
        {
            eAnchor=FLY_PAGE;
        }

        SwFmtAnchor aAnchor( eAnchor );
        aAnchor.SetAnchor( pPaM->GetPoint() );
        rFlySet.Put( aAnchor );

        if (pFSPA)
        {
            SwHoriOrient eHoriOri;
            eHoriOri = aHoriOriTab[ nXAlign ];
            SwRelationOrient eHoriRel;
            eHoriRel = aRelOriTab[  nXRelTo ];

            /*
            ##640##
            If we are inside another frame we have to adjust our x and y
            offsets correspondingly by the offsets of the parent
            */
            const SwFrmFmt *pFmt = pPaM->GetCntntNode()->GetFrmFmt();
            if (pFmt && !pRecord->bReplaceByFly)
            {
                const SvxBoxItem &rParentBox = pFmt->GetBox();
                pFSPA->nYaTop -= rParentBox.GetDistance();
                pFSPA->nYaBottom -= rParentBox.GetDistance();

                if (eHoriRel == FRAME)
                {
                    const SwFmtHoriOrient &rParentHori = pFmt->GetHoriOrient();
                    pFSPA->nXaLeft += rParentHori.GetPos();
                    pFSPA->nXaLeft += rParentBox.GetDistance();

                    pFSPA->nXaRight += rParentHori.GetPos();
                    pFSPA->nXaRight += rParentBox.GetDistance();

                    if (rParentHori.GetRelationOrient() == REL_PG_FRAME)
                    {
                        pFSPA->nXaLeft -= nPgLeft;
                        pFSPA->nXaRight -= nPgLeft;
                    }
                }
            }

            SwFmtHoriOrient aHoriOri( pFSPA->nXaLeft, eHoriOri, eHoriRel );
            if( 4 <= nXAlign )
                aHoriOri.SetPosToggle( TRUE );
            rFlySet.Put( aHoriOri );

            //Writer honours this wrap distance when aligned as "left" or "right",
            //Word doesn't. Writer doesn't honour it when its "from left".
            if (eHoriOri == HORI_LEFT)
                pRecord->nDxWrapDistLeft=0;
            else if (eHoriOri == HORI_RIGHT)
                pRecord->nDxWrapDistRight=0;

            SwVertOrient eVertOri;
            eVertOri = aVertOriTab[ nYAlign ];
            SwRelationOrient eVertRel;
            eVertRel = FLY_AUTO_CNTNT == eAnchor ? REL_CHAR :
                aRelOriTab[  nYRelTo ];

            rFlySet.Put(SwFmtVertOrient( pFSPA->nYaTop,  eVertOri, eVertRel ));
        }
    }
}

#pragma optimize("",off)
SwFrmFmt* SwWW8ImplReader::Read_GrafLayer( long nGrafAnchorCp )
{
    if( nIniFlags & WW8FL_NO_GRAFLAYER )
        return 0;

    ::SetProgressState( nProgress, rDoc.GetDocShell() );	 // Update

    nDrawCpO =    pWwFib->ccpText + pWwFib->ccpFtn
                + pWwFib->ccpHdr  + pWwFib->ccpMcr
                + pWwFib->ccpAtn  + pWwFib->ccpEdn;

    if( pPlcxMan->GetManType() == MAN_HDFT )
        nDrawCpO += pWwFib->ccpTxbx;

    if( !pDrawModel )// 1. GrafikObjekt des Docs
        GrafikCtor();

    WW8PLCFspecial* pPF = pPlcxMan->GetFdoa();
    if( !pPF )
    {
        ASSERT( !this, "Where is the grapic (1) ?" );
        return 0;
    }

    if( bVer67 )
    {
        long nOldPos = pStrm->Tell();

        nDrawXOfs = nDrawYOfs = 0;
        pDrawFmt = rDoc.MakeDrawFrmFmt(CREATE_CONST_ASC("DrawObject"),
            rDoc.GetDfltFrmFmt() );
        ReadGrafLayer1( pPF, nGrafAnchorCp );

        pStrm->Seek( nOldPos );
        return 0;
    }

    //Normal case of Word 8+ version stuff
    pPF->SeekPos( nGrafAnchorCp );

    long nStartFc;
    void* pF0;
    if( !pPF->Get( nStartFc, pF0 ) ){
        ASSERT( !this, "+Wo ist die Grafik (2) ?" );
        return 0;
    }

    WW8_FSPA_SHADOW* pFS = (WW8_FSPA_SHADOW*)pF0;
    WW8_FSPA*        pF;
#ifdef __WW8_NEEDS_COPY
    WW8_FSPA aFSFA;
    pF = &aFSFA;
    WW8FSPAShadowToReal( pFS, pF );
#else
    pF = (WW8_FSPA*)pFS;
#endif // defined __WW8_NEEDS_COPY
    if( !pF->nSpId )
    {
        ASSERT( !this, "+Wo ist die Grafik (3) ?" );
        return 0;
    }

    if( !pMSDffManager->GetModel() )
         pMSDffManager->SetModel(pDrawModel, 1440);

    SdrObject* pObject = 0;

    Rectangle aRect(pF->nXaLeft,  pF->nYaTop, pF->nXaRight, pF->nYaBottom);
    SvxMSDffImportData aData( aRect );

    if (!(pMSDffManager->GetShape( pF->nSpId, pObject, aData ) && pObject))
    {
        ASSERT( !this, "Where is the Shape ?" );
        return 0;
    }

    const SdrObject* pOrgShapeObject = pObject;
    SvxMSDffImportRec* pRecord;
    BOOL bDone = FALSE;
    SdrObject* pOurNewObject = 0;
    BOOL bReplaceable = FALSE;

    switch (SdrObjKind(pObject->GetObjIdentifier()))
    {
        case OBJ_GRAF:
            bReplaceable = TRUE;
            bDone = TRUE;
            break;
        case OBJ_OLE2:
            bReplaceable = TRUE;
            break;
        default:
            break;

    }

    // Umfluss-Modus ermitteln
    SfxItemSet aFlySet(rDoc.GetAttrPool(), RES_FRMATR_BEGIN, RES_FRMATR_END-1);
    SwSurround eSurround = SURROUND_PARALLEL;
    BOOL bContour = FALSE;
    switch( pF->nwr )
    {
        case 0: //0 like 2, but doesn't require absolute object
        case 2: //2 wrap around absolute object
            eSurround = SURROUND_PARALLEL;
            break;
        case 1: //1 no text next to shape
            eSurround = SURROUND_NONE;
            break;
        case 3: //3 wrap as if no object present
            eSurround = SURROUND_THROUGHT;
            break;
        case 4: //4 wrap tightly around object
        case 5: //5 wrap tightly, but allow holes
            eSurround = SURROUND_PARALLEL;
            // ensure the object will be conterted into a Writer structure!
            // the Writer doesn't support Contour on Sdr objects
            if( bReplaceable )
                bContour = TRUE;
            break;
    }

    // bei Modus 2 oder 4 auch den Zusatzparameter beruecksichtigen
    if( ( 2 == pF->nwr ) || ( 4 == pF->nwr ) )
    {
        switch( pF->nwrk )
        {
        //0 wrap both sides
        case 0: eSurround = SURROUND_PARALLEL;	break;
        //1 wrap only on left
        case 1: eSurround = SURROUND_LEFT;		break;
        //2 wrap only on right
        case 2: eSurround = SURROUND_RIGHT;		break;
        //3 wrap only on largest side
        case 3: eSurround = SURROUND_IDEAL;		break;
        }
    }

    SwFmtSurround aSur( eSurround );
    aSur.SetContour( bContour );
    aSur.SetOutside( TRUE ); // Winword kann nur Aussen-Konturen
    aFlySet.Put( aSur );

    // eingelesenes Objekt (kann eine ganze Gruppe sein) jetzt korrekt
    // positionieren usw.

    SwFrmFmt* pRetFrmFmt = 0;
    pRecord = ( aData.HasRecords() && (1 == aData.GetRecCount() ) )
        ? aData.GetRecord( 0 ) : 0;

    //If we are to be "below text" then we are not to be opaque
    if( pF->bBelowText || ( pRecord ? pRecord->bDrawHell : 0 ) )
        aFlySet.Put(SvxOpaqueItem(RES_OPAQUE,FALSE));


    if( bReplaceable )
    {
        pRetFrmFmt = ImportReplaceableDrawables(pObject, pOurNewObject, pRecord,
            pF, aFlySet);
    }
    else
    {
        // eingelesenes Objekt (kann eine ganze Gruppe sein) jetzt korrekt
        // positionieren usw.
        if( pF->bRcaSimple )
        {
            pF->nbx = nbxRelPageBorder;
            pF->nby = nbyRelPageBorder;
        }

        RndStdIds eAnchor = FLY_AT_CNTNT;  //FLY_PAGE
        if( (nbyRelText != pF->nby) )
        {
            if( bIsHeader || bIsFooter)
                pNode_FLY_AT_CNTNT = &pPaM->GetPoint()->nNode.GetNode();
            else
                eAnchor = FLY_PAGE;
        }

        ProcessEscherAlign( pRecord, pF, aFlySet, bReplaceable );

        // Should we, and is it possible to make this into a writer textbox
        if( (!(nIniFlags1 & WW8FL_NO_FLY_FOR_TXBX)) &&
            (pRecord && pRecord->bReplaceByFly) )
        {
            pRetFrmFmt = ConvertDrawTextToFly(pObject, pOurNewObject, pRecord,
                eAnchor, pF, aFlySet);
            if (pRetFrmFmt)
                bDone = TRUE;
        }
        else if ((pObject) && FmFormInventor == pObject->GetObjInventor())
        {
            /*
            #79055#
            This was a FormControl. This means that the msdffimp ole import
            has already converted it to the SdrObject that we have in pObject
            here, *and* has already inserted it as a StarWriter FlyFrm,
            tragically though it does not have its position set correctly, and
            the OCXConverter returns a uno XShape so we turn this back into a
            FlyFrm through the uno api and then set its correct position
            information.
            */
            if (pRetFrmFmt = pMSDffManager->GetLastOCXShapeFrm())
            {
                pRetFrmFmt->SetAttr(aFlySet.Get(RES_VERT_ORIENT));
                pRetFrmFmt->SetAttr(aFlySet.Get(RES_HORI_ORIENT));
            }
            bDone=TRUE;
        }

        if( !bDone )
        {
            if( pF->bBelowText || (pRecord ? pRecord->bDrawHell : 0) )
                pObject->SetLayer( nDrawHell );
            else
                pObject->SetLayer( nDrawHeaven );

            /*
            #97824#  Need to make sure that the correct layer ordering is
            applied.
            */
            pWWZOrder->InsertEscherObject(pObject,pF->nSpId);
            pRetFrmFmt = rDoc.Insert( *pPaM, *pObject, &aFlySet );
        }
    }

    /*
        Adjust inner and outer edge distances, and insert text if necessary
        into textboxes contained in groups that were not converted to
        textframes.
    */
    if( !bDone && aData.HasRecords() )
    {
        USHORT nRecCount = aData.GetRecCount();
        for(USHORT nTxbx=0; nTxbx < nRecCount; nTxbx++ )
        {
            pRecord = aData.GetRecord( nTxbx );
            if( pRecord && pRecord->pObj )
            {
                SdrObject* pTrueObject =
                    (bReplaceable && (pOrgShapeObject == pRecord->pObj))
                    ? pOurNewObject : pRecord->pObj;

                if( pTrueObject )
                {
                    if( pF->bBelowText || pRecord->bDrawHell)
                        pTrueObject->SetLayer( nDrawHell );
                    else
                        pTrueObject->SetLayer( nDrawHeaven );
                }

                if( pTrueObject == pRecord->pObj )
                    MatchWrapDistancesIntoFlyFmt(pRecord, pRetFrmFmt);

                if( pRecord->aTextId.nTxBxS && !bReplaceable)
                {
                    MungeTextIntoDrawBox(pTrueObject,pRecord,nGrafAnchorCp,
                        pRetFrmFmt);
                }
            }
        }
    }

    return AddAutoAnchor(pRetFrmFmt);
}

SwFrmFmt *SwWW8ImplReader::AddAutoAnchor(SwFrmFmt *pFmt)
{
    /*
     * anchored to character at the current position will move along the
     * aragraph as text is added because we are at the insertion point.
     *
     * Leave to later and set the correct location then.
     */
    if ((pFmt) && (pFmt->GetAnchor().GetAnchorId() != FLY_IN_CNTNT))
        pAnchorStck->AddAnchor(*pPaM->GetPoint(), pFmt);
    return pFmt;
}

void SwWW8ImplReader::RemoveAutoAnchor(const SwFrmFmt *pFmt)
{
    ASSERT(pFmt,"remove anchor, no fmt");
    if ((pFmt) && (pFmt->GetAnchor().GetAnchorId() != FLY_IN_CNTNT))
        pAnchorStck->RemoveAnchor(pFmt);
}

void SwWW8ImplReader::MungeTextIntoDrawBox(SdrObject* pTrueObject,
    SvxMSDffImportRec *pRecord, long nGrafAnchorCp, SwFrmFmt* pRetFrmFmt)
{
    SdrTextObj* pSdrTextObj;

    // Pruefen, ob Gruppenobjekt (z.B. zwei Klammern) vorliegt
    SdrObjGroup* pThisGroup = PTR_CAST(SdrObjGroup, pRecord->pObj);
    if( pThisGroup )
    {
        // Gruppenobjekte haben keinen Text. Fuege ein Textobjekt in die
        // Gruppe ein, um den Text zu halten.
        pSdrTextObj = new SdrRectObj( OBJ_TEXT, pThisGroup->GetBoundRect());

        SfxItemSet aSet(pDrawModel->GetItemPool());
        aSet.Put(XFillStyleItem(XFILL_NONE));
        aSet.Put(XLineStyleItem(XLINE_NONE));
        aSet.Put(SdrTextFitToSizeTypeItem( SDRTEXTFIT_NONE ));
        aSet.Put(SdrTextAutoGrowHeightItem( FALSE ));
        aSet.Put(SdrTextAutoGrowWidthItem( FALSE ));
        pSdrTextObj->SetItemSet(aSet);

        long nAngle = pRecord->nTextRotationAngle;
        if ( nAngle )
        {
            double a = nAngle*nPi180;
            pSdrTextObj->NbcRotate(pSdrTextObj->GetBoundRect().Center(), nAngle,
                sin(a), cos(a) );
        }

        pSdrTextObj->NbcSetLayer( pThisGroup->GetLayer() );
        pThisGroup->GetSubList()->NbcInsertObject(pSdrTextObj);
    }
    else
    {
        pSdrTextObj = PTR_CAST(SdrTextObj, pRecord->pObj);
        /*
            Die Frage: was tun, wenn hier FALSE hereuskommt, z.B. bei
            3D-Objekten (nicht von SdrTextObj abgeleitet)

            Wunsch: neues SdrTextObj hinzufuegen, das mit dem alten in einer
            neu zu schaffenden Gruppe geklammert wird.

            Implementierung: nicht zur 5.1 (jp und khz am 11.02.1999)

            if( !pSdrTextObj )
            {
                ...
            }
        */
    }

    if( pSdrTextObj )
    {
        Size aObjSize(pSdrTextObj->GetSnapRect().GetWidth(),
            pSdrTextObj->GetSnapRect().GetHeight());

        // Objekt ist Bestandteil einer Gruppe?
        SdrObject* pGroupObject = pSdrTextObj->GetUpGroup();

        UINT32 nOrdNum = pSdrTextObj->GetOrdNum();
        BOOL bEraseThisObject;
        InsertTxbxText( pSdrTextObj, &aObjSize, pRecord->aTextId.nTxBxS,
            pRecord->aTextId.nSequence, nGrafAnchorCp, pRetFrmFmt,
            (pSdrTextObj != pTrueObject) || (0 != pGroupObject),
            bEraseThisObject, FALSE, 0, 0, 0, pRecord);

        // wurde dieses Objekt ersetzt ??
        if( bEraseThisObject )
        {
            if( pGroupObject || (pSdrTextObj != pTrueObject) )
            {
                // Objekt wurde bereits (in der Gruppe und) der Drawing-Page
                // durch ein neues SdrGrafObj ersetzt.

                SdrObject* pNewObj = pGroupObject ?
                    pGroupObject->GetSubList()->GetObj(nOrdNum) : pTrueObject;
                if (pSdrTextObj != pNewObj)
                {
                    // Objekt in der Z-Order-Liste ersetzen
                    pMSDffManager->ExchangeInShapeOrder(pSdrTextObj, 0,0, pNewObj);
                    // Objekt jetzt noch loeschen
                    delete pRecord->pObj;
                    // und das neue Objekt merken.
                    pRecord->pObj = pNewObj;
                }
            }
            else
            {
                // Objekt aus der Z-Order-Liste loeschen
                pMSDffManager->RemoveFromShapeOrder( pSdrTextObj );
                // Objekt aus der Drawing-Page rausnehmen
                if( pSdrTextObj->GetPage() )
                    pDrawPg->RemoveObject( pSdrTextObj->GetOrdNum() );
                // und FrameFormat entfernen, da durch Grafik ersetzt (dies
                // loescht auch das Objekt)
                rDoc.DelFrmFmt( pRetFrmFmt );
                // auch den Objektmerker loeschen
                pRecord->pObj = 0;
            }
        }
        else
        {
            // ww8-default Randabstand einsetzen
            SfxItemSet aItemSet(pDrawModel->GetItemPool(),
                SDRATTR_TEXT_LEFTDIST, SDRATTR_TEXT_LOWERDIST);
            aItemSet.Put( SdrTextLeftDistItem( pRecord->nDxTextLeft ) );
            aItemSet.Put( SdrTextRightDistItem( pRecord->nDxTextRight  ) );
            aItemSet.Put( SdrTextUpperDistItem( pRecord->nDyTextTop    ) );
            aItemSet.Put( SdrTextLowerDistItem( pRecord->nDyTextBottom ) );
            pSdrTextObj->SetItemSetAndBroadcast(aItemSet);
        }
    }
}

SwFlyFrmFmt* SwWW8ImplReader::ConvertDrawTextToFly(SdrObject* &rpObject,
    SdrObject* &rpOurNewObject, SvxMSDffImportRec* pRecord, RndStdIds eAnchor,
    WW8_FSPA *pF, SfxItemSet &rFlySet)
{
    SwFlyFrmFmt* pRetFrmFmt = 0;
    long nStartCp;
    long nEndCp;

    // Pruefen, ob in dieser Textbox-Kette denn Text enthalten ist.  (
    // Umwandeln einer leeren Kette in Rahmen waere Unsinn. )
    if ( TxbxChainContainsRealText(pRecord->aTextId.nTxBxS,nStartCp,nEndCp) )
    {
        // Der Text wird nicht in das SdrTextObj eingelesen!  Stattdessen wird
        // ein Rahmen eingefuegt und der Text von nStartCp bis nEndCp dort
        // hinein gelesen.
        //
        // Vorteil: im Rahmen sind viel mehr Attribute moeglich als in der
        // Edit-Enging, und es koennen auch Felder, OLEs oder Grafiken darin
        // sein...

        Rectangle aInnerDist(pRecord->nDxTextLeft, pRecord->nDyTextTop,
            pRecord->nDxTextRight, pRecord->nDyTextBottom);

        SdrTextObj *pSdrTextObj = PTR_CAST(SdrTextObj, rpObject);
        if (pSdrTextObj && pSdrTextObj->IsVerticalWriting())
        {
            rFlySet.Put(SvxFrameDirectionItem(FRMDIR_VERT_TOP_RIGHT));
            rFlySet.Put(SwFmtFrmSize(ATT_FIX_SIZE, pF->nYaBottom - pF->nYaTop,
                pF->nXaRight - pF->nXaLeft));
        }
        else
        {
            rFlySet.Put(SwFmtFrmSize(ATT_FIX_SIZE, pF->nXaRight - pF->nXaLeft, 
                pF->nYaBottom - pF->nYaTop));
        }

        MatchSdrItemsIntoFlySet( rpObject, rFlySet, pRecord->eLineStyle,
            pRecord->eShapeType, aInnerDist );

        pRetFrmFmt = rDoc.MakeFlySection(eAnchor, pPaM->GetPoint(), &rFlySet );

        MatchWrapDistancesIntoFlyFmt( pRecord, pRetFrmFmt );

        // falls alles Ok, Zeiger auf neues Objekt ermitteln und Z-Order-Liste
        // entsprechend korrigieren (oder Eintrag loeschen)
        rpOurNewObject = CreateContactObject(pRetFrmFmt);

        // altes Objekt aus der Z-Order-Liste entfernen
        pMSDffManager->RemoveFromShapeOrder( rpObject );

        // und das Objekt loeschen
        DELETEZ( rpObject );
        /*
            Achtung: ab jetzt nur noch pOrgShapeObject
            abfragen!
        */

        if (rpOurNewObject)
        {
            /*
            #96375#
            We do not store our rpOutNewObject in the ShapeOrder because we
            have a FrmFmt from which we can regenerate the contact object when
            we need it. Because, we can have frames anchored to paragraphs in
            header/footers and we can copy header/footers, if we do copy a
            header/footer with a nonpage anchored frame in it then the contact
            objects are invalidated. Under this condition the FrmFmt will be
            updated to reflect this change and can be used to get a new
            contact object, while a raw rpOutNewObject stored here becomes
            deleted and useless.
            */
            pMSDffManager->StoreShapeOrder( pF->nSpId,
                (((ULONG)pRecord->aTextId.nTxBxS) << 16) +
                pRecord->aTextId.nSequence, 0, pRetFrmFmt, 
                nActSectionNo + bIsHeader ? 1 : 0 + bIsFooter ? 2 : 0 );

            // Das Kontakt-Objekt MUSS in die Draw-Page gesetzt werden, damit
            // in SwWW8ImplReader::LoadDoc1() die Z-Order festgelegt werden
            // kann !!!
            if (!rpOurNewObject->IsInserted())
                pWWZOrder->InsertEscherObject(rpOurNewObject,pF->nSpId);
        }

        // Box-0 erhaelt den Text fuer die ganze Kette!
        if( !pRecord->aTextId.nSequence )
        {
            // rette Flags u.ae. und setze sie zurueck
            WW8ReaderSave aSave( this );

            MoveInsideFly(pRetFrmFmt);

            SwNodeIndex aStart(pPaM->GetPoint()->nNode);

            // lies den Text ein
            bTxbxFlySection = TRUE;
            BOOL bJoined = ReadText( nStartCp, (nEndCp-nStartCp),
                MAN_MAINTEXT == pPlcxMan->GetManType() ?
                MAN_TXBX : MAN_TXBX_HDFT );

            MoveOutsideFly(pRetFrmFmt, aSave.GetStartPos(),!bJoined);
#if 0
            /*
            ##505##
            Special test to see if we will be forced to disable allowing this
            box to grow. Only if contains only one flyframe which is bigger
            than the nominal size of this frame
            */
            if (pRecord->bLastBoxInChain)
                EmbeddedFlyFrameSizeLock(aStart,pRetFrmFmt);
#endif
            aSave.Restore( this );
        }
    }
    return pRetFrmFmt;
}

SwFlyFrmFmt* SwWW8ImplReader::ImportReplaceableDrawables( SdrObject* &rpObject,
    SdrObject* &rpOurNewObject, SvxMSDffImportRec* pRecord, WW8_FSPA *pF,
    SfxItemSet &rFlySet )
{
    SwFlyFrmFmt* pRetFrmFmt = 0;
    long nWidthTw  = pF->nXaRight - pF->nXaLeft;
    if (0 > nWidthTw)
        nWidthTw = 0;
    long nHeightTw = pF->nYaBottom - pF->nYaTop;
    if (0 > nHeightTw)
        nHeightTw = 0;

    /*
     Absolute positions in winword for graphics are broken when the graphic is
     in a table, all absolute positions now become relative to the top left
     corner of that cell.  We really cannot import that feature correctly as
     we have not the same functionality (yet). This normalizes the absolute
     position by the left of the table, which at least puts it close, theres
     nothing we can do about the vertical either.
     */
    if (nTable)
        pF->nXaLeft -= GetTableLeft();

    ProcessEscherAlign( pRecord, pF, rFlySet, TRUE );

    rFlySet.Put(SwFmtFrmSize( ATT_FIX_SIZE, nWidthTw, nHeightTw));

    if( pRecord )
    {
        //Note that the escher inner distance only seems to be honoured in
        //word for textboxes, not for graphics and ole objects.
        Rectangle aInnerDist(0,0,0,0);

        MatchSdrItemsIntoFlySet( rpObject, rFlySet, pRecord->eLineStyle,
            pRecord->eShapeType, aInnerDist );
    }

    String aObjectName( rpObject->GetName() );
    if( OBJ_OLE2 == SdrObjKind(rpObject->GetObjIdentifier()) )
    {
        SvInPlaceObjectRef xIPRef(((SdrOle2Obj*)rpObject)->GetObjRef());

        // kein GrafSet uebergeben, da nur fuer Cropping sinnvoll, was die UI
        // derzeit (27.1.99) noch nicht kann khz.
        pRetFrmFmt = rDoc.Insert( *pPaM, &xIPRef, &rFlySet );
        //JP 10.4.2001: Bug 85614 - don't remove in DTOR the object from
        //our persist
        SvInPlaceObjectRef xEmpty;
        ((SdrOle2Obj*)rpObject)->SetObjRef( xEmpty );
    }
    else
    {
        const Graphic& rGraph = ((SdrGrafObj*)rpObject)->GetGraphic();
        BOOL bDone = FALSE;

        if( ((SdrGrafObj*)rpObject)->IsLinkedGraphic() )
        {
            String aGrfName( URIHelper::SmartRelToAbs(
                    ((SdrGrafObj*)rpObject)->GetFileName()) );

            if ( GRAPHIC_NONE == rGraph.GetType() ||
                FStatHelper::IsDocument( aGrfName ) )
            {
                pRetFrmFmt = rDoc.Insert( *pPaM, aGrfName, aEmptyStr,
                    0 /*Graphic*/, &rFlySet, 0 /*SwFrmFmt*/);
                bDone = TRUE;
            }
        }
        if( !bDone )
        {
            pRetFrmFmt = rDoc.Insert( *pPaM, aEmptyStr, aEmptyStr, &rGraph,
                &rFlySet, 0 /* SwFrmFmt*/ );
        }
    }

    if (pRetFrmFmt)
    {
        if( pRecord )
        {
            MatchWrapDistancesIntoFlyFmt(pRecord, pRetFrmFmt);
            if( OBJ_OLE2 != SdrObjKind(rpObject->GetObjIdentifier()) )
                SetAttributesAtGrfNode( pRecord, pRetFrmFmt, pF );
        }
        // mehrfaches Auftreten gleicher Grafik-Namen vermeiden
        aGrfNameGenerator.SetUniqueGraphName(pRetFrmFmt, aObjectName);
    }
    //falls alles Ok, Zeiger auf neues Objekt ermitteln und Z-Order-Liste
    //entsprechend korrigieren (oder Eintrag loeschen)
    rpOurNewObject = CreateContactObject(pRetFrmFmt);

    // altes Objekt aus der Z-Order-Liste entfernen
    pMSDffManager->RemoveFromShapeOrder( rpObject );
    // aus der Drawing-Page rausnehmen
    if( rpObject->GetPage() )
        pDrawPg->RemoveObject( rpObject->GetOrdNum() );

    // und das Objekt loeschen
    DELETEZ( rpObject );
    /*
        Achtung: ab jetzt nur noch pOrgShapeObject abfragen!
    */

    // Kontakt-Objekt in die Z-Order-Liste und die Page aufnehmen
    if (rpOurNewObject)
    {
        if (!bHdFtFtnEdn)
            pMSDffManager->StoreShapeOrder(pF->nSpId, 0, rpOurNewObject, 0 );

        // Das Kontakt-Objekt MUSS in die Draw-Page gesetzt werden, damit in
        // SwWW8ImplReader::LoadDoc1() die Z-Order festgelegt werden kann !!!
        if (!rpOurNewObject->IsInserted())
            pWWZOrder->InsertEscherObject(rpOurNewObject,pF->nSpId);
    }
    return pRetFrmFmt;
}
#pragma optimize("",on)

void SwWW8ImplReader::GrafikCtor()	// Fuer SVDraw und VCControls und Escher
{
    rDoc.MakeDrawModel( );
    pDrawModel	= rDoc.GetDrawModel();
    ASSERT(pDrawModel, "Kann DrawModel nicht anlegen");
    pDrawPg		= pDrawModel->GetPage(0);
    nDrawHeaven	= rDoc.GetHeavenId();
    nDrawHell	= rDoc.GetHellId();

    pWWZOrder = new wwZOrderer(pDrawPg,
        pMSDffManager ? pMSDffManager->GetShapeOrders() : 0, nDrawHeaven, 
        nDrawHell);
}

void SwWW8ImplReader::GrafikDtor()
{
    DELETEZ(pDrawEditEngine); // evtl. von Grafik angelegt
    DELETEZ(pWWZOrder);	      // dito
}
#if 0
void SwWW8ImplReader::EmbeddedFlyFrameSizeLock(SwNodeIndex &rStart,
    SwFrmFmt *pFrmFmt)
{
    /*
    ##505###
    If we would ordinarily insert this textbox with a flexible size to expand
    to fit its contents, but it consists solely of a textbox inside a textbox
    where the internal textbox will force the size larger than itself then we
    disable the grow to fit feature. We usually want the box to grow to fit if
    it is the last in a chain so as to display all text, but some strange ww6
    -> ww97 textbox conversions in word have placed textboxes without a size
    setting inside textboxes that have them, and in this case the outer size
    setting is the one that dominates
    */
    if ( (rStart == pPaM->GetPoint()->nNode) && rStart.GetNode().IsCntntNode() )
    {
        /*
        Loop through the flyframes and see if any are anchored inside this
        otherwise empty flyframe. If so and its size is bigger than the
        current one then disable the current ones grow to fit feature
        */
        const SwSpzFrmFmts& rFmts = *rDoc.GetSpzFrmFmts();
        for( USHORT iN = 0, nN = rFmts.Count(); iN < nN; ++iN )
        {
            const SwPosition *pAnchor = rFmts[iN]->GetAnchor().GetCntntAnchor();
            if ( pAnchor && (pAnchor->nNode == rStart.GetNode()) )
            {
                const SwFmtFrmSize& rSNew = rFmts[iN]->GetFrmSize();
                SwFmtFrmSize aSOld = pFrmFmt->GetFrmSize();
                if (
                    (rSNew.GetWidth() > aSOld.GetWidth()) ||
                    (rSNew.GetHeight() > aSOld.GetHeight())
                   )
                {
                    aSOld.SetSizeType(ATT_FIX_SIZE);
                    pFrmFmt->SetAttr(aSOld);
                }
            break;
            }
        }
    }
}
#endif
void SwWW8ImplReader::GetNoninlineNodeAttribs(const SwTxtNode *pNode,
    std::vector<const xub_StrLen*> &rPositions)
{
    USHORT nSize = pAnchorStck->Count();
    while (nSize)
    {
        const SwFltStackEntry* pEntry = (*pAnchorStck)[--nSize];
        if (pEntry->nMkNode == pNode->GetIndex()-1)
        {
            if (pEntry->pAttr->Which() == RES_FLTR_ANCHOR)
            {
                const SwFrmFmt* pFmt=((SwFltAnchor*)pEntry->pAttr)->GetFrmFmt();
                RndStdIds eAnchor = pFmt->GetAnchor().GetAnchorId();
                if (eAnchor == FLY_AT_CNTNT || eAnchor == FLY_AUTO_CNTNT) 
                    rPositions.push_back(&pEntry->nMkCntnt);
            }
            else
                rPositions.push_back(&pEntry->nMkCntnt);
        }
    }

    nSize = pRefStck->Count();
    while (nSize)
    {
        const SwFltStackEntry* pEntry = (*pRefStck)[--nSize];
        if (pEntry->nMkNode == pNode->GetIndex()-1)
            rPositions.push_back(&pEntry->nMkCntnt);
    }

}

void SwWW8FltAnchorStack::AddAnchor(const SwPosition& rPos, SwFrmFmt *pFmt)
{
    ASSERT(pFmt->GetAnchor().GetAnchorId() != FLY_IN_CNTNT,
        "Don't use fltanchors with inline frames, slap!");
    NewAttr(rPos, SwFltAnchor(pFmt));
}

void SwWW8FltAnchorStack::RemoveAnchor(const SwFrmFmt *pFmt)
{
    ASSERT(pFmt->GetAnchor().GetAnchorId() != FLY_IN_CNTNT,
        "Don't use fltanchors with inline frames, slap!");
    USHORT nCnt = Count();
    for (USHORT i=0; i < nCnt; ++i)
    {
        SwFltStackEntry* pEntry = (*this)[i];
        ASSERT(pEntry->pAttr->Which() == RES_FLTR_ANCHOR,"Impossible!");
        const SwFltAnchor *pAnchor = (const SwFltAnchor *)(pEntry->pAttr);
        if (pAnchor->GetFrmFmt() == pFmt)
        {
            DeleteAndDestroy(i--);
            --nCnt;
        }
    }
}

void SwWW8FltAnchorStack::Flush()
{
    USHORT nCnt = Count();
    for (USHORT i=0; i < nCnt; ++i)
    {
        SwFltStackEntry *pEntry = (*this)[i];
        SwPosition aDummy(pEntry->nMkNode);
        SetAttrInDoc(aDummy,pEntry);
        DeleteAndDestroy(i--);
        --nCnt;
    }
}
