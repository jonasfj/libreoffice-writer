/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: w1sprm.cxx,v $
 * $Revision: 1.15 $
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

#include <tools/solar.h>
#include <svx/paperinf.hxx>
#include <svx/lrspitem.hxx>
#include <svx/adjitem.hxx>
#include <svx/spltitem.hxx>
#include <svx/keepitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/shaditem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/lspcitem.hxx>
#include <svx/tstpitem.hxx>

#include <fmtclds.hxx>
#include <fmtfsize.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <charatr.hxx>
#include <frmatr.hxx>
#include <doc.hxx>
#include <errhdl.hxx>
#include <fltini.hxx>
#include <docufld.hxx>
#include <pagedesc.hxx>
#include <flddat.hxx>
#include <reffld.hxx>
#include <expfld.hxx>
#include <w1par.hxx>
#include <mdiexp.hxx>
#include <swerror.h>
#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif

using namespace ::com::sun::star;

/////////////////////////////////////////////////////////////// Ww1Sprm
void Ww1Sprm::Stop( Ww1Shell& rOut, Ww1Manager& rMan)
{
    if(IsUsed())
//		for(USHORT i=0;i<Count();i++)
        for(short i=Count()-1;i>=0;i--){	// rueckwaerts
            BYTE nId;
            USHORT nSize;
            BYTE* pSprm;
            Fill(i, nId, nSize, pSprm);
            GetTab(nId).Stop(rOut, nId, pSprm, nSize, rMan);
        }
}

void Ww1Sprm::Start(
    Ww1Shell& rOut, Ww1Manager& rMan, USHORT i)
{
    BYTE nId;
    USHORT nSize;
    BYTE* pSprm;
    Fill(i, nId, nSize, pSprm);
    GetTab(nId).Start(rOut, nId, pSprm, nSize, rMan);
}

void Ww1Sprm::Start(
    Ww1Shell& rOut, Ww1Manager& rMan)
{
    if(IsUsed())
        for(USHORT i=0;i<Count();i++)
            Start(rOut, rMan, i);
}

//////////////////////////////////////////////////////////// SingleSprm
void Ww1SingleSprm::Start(
    Ww1Shell&, BYTE /*nId*/, BYTE*, USHORT, Ww1Manager&)
{
}

void Ww1SingleSprm::Stop(
    Ww1Shell&, BYTE, BYTE*, USHORT, Ww1Manager&)
{
//	ASSERT(FALSE, "Unknown Sprm");
}

////////////////////////////////////////////////////////////////// STOP
//
// folgende defines werden genutzt zur implementierung der
// Stop()-Member der SingleSprm-klassen, da diese im normalfalle
// lediglich EndItem(s) in die shell stellen.
//
#define STOP1(Class, Code) \
    void Class::Stop( \
     Ww1Shell& rOut, BYTE, BYTE*, USHORT, Ww1Manager&) { \
        rOut.EndItem(Code); }
#define STOP2(Class, Code1, Code2) \
    void Class::Stop( \
     Ww1Shell& rOut, BYTE, BYTE*, USHORT, Ww1Manager&) { \
        rOut.EndItem(Code1).EndItem(Code2); }

/////////////////////////////////////////////////////// SingleSprmXxxxx
//
// hier beginnt die auswertung der eigentlichen sprms. jeder sprmtyp
// hat eine eigene klasse, die die virtualen methoden start und stop
// implementiert. die klassen stehen in der sprm-tab, die statischer
// member von Ww1Sprm ist. taucht ein sprm im dokument auf, werden die
// virtualen methoden bei beginn und ende der formatierung gerufen.
//
void Ww1SingleSprmPDxaLeft::Start(
    Ww1Shell& rOut, BYTE, BYTE* pSprm, USHORT, Ww1Manager&)
{
    SvxLRSpaceItem aLR((SvxLRSpaceItem&)rOut.GetAttr(RES_LR_SPACE));
    short nPara = SVBT16ToShort(pSprm);
    if(nPara < 0)
        nPara = 0;
    if(aLR.GetTxtFirstLineOfst() < -nPara)
        aLR.SetTxtFirstLineOfst(-nPara); // sonst weigert sich SetTxtLeft()
    aLR.SetTxtLeft(nPara);
    rOut << aLR;
}

void Ww1SingleSprmPDxaRight::Start(
    Ww1Shell& rOut, BYTE, BYTE* pSprm, USHORT, Ww1Manager&)
{
    SvxLRSpaceItem aLR((SvxLRSpaceItem&)rOut.GetAttr(RES_LR_SPACE));
    short nPara = SVBT16ToShort(pSprm);
    if(nPara < 0)
        nPara = 0;
    aLR.SetRight(nPara);
    rOut << aLR;
}

void Ww1SingleSprmPDxaLeft1::Start(
    Ww1Shell& rOut, BYTE, BYTE* pSprm, USHORT, Ww1Manager&)
{
    SvxLRSpaceItem aLR((SvxLRSpaceItem&)rOut.GetAttr(RES_LR_SPACE));
    short nPara = SVBT16ToShort(pSprm);
    if(-nPara >(short)aLR.GetTxtLeft())
        nPara = -(short)aLR.GetTxtLeft();
    aLR.SetTxtFirstLineOfst(nPara);
    rOut << aLR;
}

STOP1(Ww1SingleSprmPDxa, RES_LR_SPACE)

void Ww1SingleSprmPJc::Start(
    Ww1Shell& rOut, BYTE, BYTE* pSprm, USHORT, Ww1Manager&)
{
    static SvxAdjust __READONLY_DATA aAdj[] = {
        SVX_ADJUST_LEFT,
        SVX_ADJUST_CENTER,
        SVX_ADJUST_RIGHT,
        SVX_ADJUST_BLOCK };
    BYTE nPara = SVBT8ToByte(pSprm);
    nPara %=(sizeof(aAdj)/sizeof(*aAdj));
    rOut << SvxAdjustItem(aAdj[nPara], RES_PARATR_ADJUST);
}

STOP1(Ww1SingleSprmPJc, RES_PARATR_ADJUST)

void Ww1SingleSprmPFKeep::Start(
    Ww1Shell& rOut, BYTE, BYTE* pSprm, USHORT, Ww1Manager&)
{
    rOut << SvxFmtSplitItem((SVBT8ToByte(pSprm) & 1) == 0, RES_PARATR_SPLIT);
}

STOP1(Ww1SingleSprmPFKeep, RES_PARATR_SPLIT)

void Ww1SingleSprmPFKeepFollow::Start(
    Ww1Shell& rOut, BYTE, BYTE* pSprm, USHORT, Ww1Manager&)
{
    rOut << SvxFmtKeepItem((SVBT8ToByte(pSprm) & 1) != 0, RES_KEEP);
}

STOP1(Ww1SingleSprmPFKeepFollow, RES_KEEP)

void Ww1SingleSprmPPageBreakBefore::Start(
    Ww1Shell& rOut, BYTE, BYTE* pSprm, USHORT, Ww1Manager&)
{
    rOut << SvxFmtBreakItem(SVBT8ToByte(pSprm) & 1?
     SVX_BREAK_PAGE_BEFORE:SVX_BREAK_NONE, RES_BREAK );
}

STOP1(Ww1SingleSprmPPageBreakBefore, RES_BREAK)

SvxBorderLine* Ww1SingleSprmPBrc::SetBorder(SvxBorderLine* pLine, W1_BRC10* pBrc)
{
    USHORT nCode;
    if(pBrc->dxpLine2WidthGet() == 0)
    {
        switch(pBrc->dxpLine1WidthGet())
        {
        default: ASSERT(FALSE, "unknown linewidth");
        case 0: return 0;							// keine Linie
        case 1: nCode = DEF_LINE_WIDTH_0; break;
        case 2: nCode = DEF_LINE_WIDTH_1; break;
        case 3: nCode = DEF_LINE_WIDTH_2; break;
        case 4: nCode = DEF_LINE_WIDTH_3; break;
        case 5: nCode = DEF_LINE_WIDTH_4; break;
        }
        pLine->SetOutWidth(nCode);
        pLine->SetInWidth(0);
    }
    else
    {
        switch(pBrc->dxpLine1WidthGet())
        {
        default: ASSERT(FALSE, "unknown linewidth");
        case 1: nCode = DEF_DOUBLE_LINE0_IN; break;
        }
        pLine->SetOutWidth(nCode);
        switch(pBrc->dxpLine2WidthGet())
        {
        default: ASSERT(FALSE, "unknown linewidth");
        case 1: nCode = DEF_DOUBLE_LINE0_OUT; break;
        }
        pLine->SetInWidth(nCode);
    }
    switch(pBrc->dxpLine1WidthGet())
    {
    default: ASSERT(FALSE, "unknown space");
    case 0: nCode = DEF_DOUBLE_LINE0_DIST; break;
    case 1: nCode = DEF_DOUBLE_LINE1_DIST; break;
    case 2: nCode = DEF_DOUBLE_LINE2_DIST; break;
    case 3: nCode = DEF_DOUBLE_LINE3_DIST; break;
    }
    pLine->SetDistance(nCode);
    return pLine;
}

void Ww1SingleSprmPBrc::Start(
    Ww1Shell& rOut, BYTE,
    W1_BRC10* pBrc,
    USHORT
#ifndef PRODUCT
    nSize
#endif
    ,
    Ww1Manager& /*rMan*/,
    SvxBoxItem& aBox)
{
    ASSERT(sizeof(W1_BRC10) == nSize, "sizemissmatch");
    if(pBrc->dxpSpaceGet())
        aBox.SetDistance(10 + 20 * pBrc->dxpSpaceGet());
            //??? Warum 10+... ????

    if( rOut.IsInFly() )
        rOut.SetFlyFrmAttr( aBox );
    else
        rOut << aBox;

    if(pBrc->fShadowGet())
    {
        Color aBlack(COL_BLACK); // schwarzer...
        SvxShadowItem aS(RES_SHADOW,(const Color*)&aBlack, 32,
                         SVX_SHADOW_BOTTOMRIGHT); // 1.6 tw breit
        if( rOut.IsInFly() )
            rOut.SetFlyFrmAttr( aS );
        else
            rOut << aS;
    }
}

STOP2(Ww1SingleSprmPBrc, RES_BOX, RES_SHADOW)

static USHORT __READONLY_DATA nBrcTrans[BRC_ANZ] =
     { BOX_LINE_TOP, BOX_LINE_LEFT, BOX_LINE_BOTTOM, BOX_LINE_RIGHT };

void Ww1SingleSprmPBrc10::Start(
    Ww1Shell& rOut, BYTE nId, BYTE* pSprm, USHORT nSize, Ww1Manager& rMan)
{

    W1_BRC10* pBrc =(W1_BRC10*)pSprm;
    const SfxPoolItem &rItem = ( ( rOut.IsInFly() ) ?
                     rOut.GetFlyFrmAttr(RES_BOX) :rOut.GetAttr(RES_BOX));
    const SvxBoxItem &rBoxItem = (const SvxBoxItem&)rItem;
    SvxBoxItem aBox( rBoxItem );
//	rOut >> aBox;
    SvxBorderLine aLine;
    aBox.SetLine(SetBorder(&aLine, pBrc), nBrcTrans[nLine] );
    Ww1SingleSprmPBrc::Start(rOut, nId, pBrc, nSize, rMan, aBox);
}

STOP1(Ww1SingleSprmParaSpace, RES_UL_SPACE)

void Ww1SingleSprmPDyaBefore::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nPara = SVBT16ToShort(pSprm);
    if(nPara < 0)
        nPara = -nPara;
    SvxULSpaceItem aUL((SvxULSpaceItem&)rOut.GetAttr(RES_UL_SPACE));
    aUL.SetUpper(nPara);
    rOut << aUL;
}

void Ww1SingleSprmPDyaAfter::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nPara = SVBT16ToShort(pSprm);
    if(nPara < 0)
        nPara = -nPara;
    SvxULSpaceItem aUL((SvxULSpaceItem&)rOut.GetAttr(RES_UL_SPACE));
    aUL.SetLower(nPara);
    rOut << aUL;
}

STOP1(Ww1SingleSprmPDyaLine, RES_PARATR_LINESPACING)

void Ww1SingleSprmPDyaLine::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nSpace = SVBT16ToShort(pSprm);
    if(nSpace < 0)
        nSpace = -nSpace;
    SvxLineSpacingItem aLSpc( LINE_SPACE_DEFAULT_HEIGHT, RES_PARATR_LINESPACING );
    if(TRUE)
    {// MultilineSpace(proportional)
        long n = nSpace * 100 / 240;	// W1: 240 = 100%, SW: 100 = 100%
        if(n>200)
            n = 200; // SW_UI-Maximum
        aLSpc.GetLineSpaceRule() = SVX_LINE_SPACE_AUTO;
        aLSpc.SetPropLineSpace((BYTE)n);
    }
    else
    {// Fixed / Minimum
        aLSpc.SetLineHeight((USHORT)nSpace);
        aLSpc.GetInterLineSpaceRule() = SVX_INTER_LINE_SPACE_OFF;
    }
    rOut << aLSpc;
}

void Ww1SingleSprmPChgTabsPapx::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
#if OSL_DEBUG_LEVEL > 1
//	rOut << 'T';
#endif
    short nLeftPMgn = 0;	// Koordinaten etwa gleich ??
//	( pAktColl ) ? pCollA[nAktColl].nLeftParaMgn
//								   : nLeftParaMgn;		// Absatz L-Space

    short i;
    BYTE nDel = pSprm[1];
    BYTE* pDel = pSprm + 2;						// Del - Array
    BYTE nIns = pSprm[nDel*2+2];
    BYTE* pIns = pSprm + 2*nDel + 3;			// Ins - Array
    W1_TBD* pTyp = (W1_TBD*)(pSprm + 2*nDel + 2*nIns + 3);// Typ - Array

    SvxTabStopItem aAttr( (SvxTabStopItem&)rOut.GetNodeOrStyAttr( RES_PARATR_TABSTOP ));

    SvxTabStop aTabStop;

    for( i=0; i<nDel; i++ ){
        USHORT nPos = aAttr.GetPos( SVBT16ToShort( pDel + i*2 ) - nLeftPMgn );
        if( nPos != SVX_TAB_NOTFOUND )
            aAttr.Remove( nPos, 1 );
    }

    for( i=0; i<nIns; i++ ){
        short nPos = SVBT16ToShort( pIns + i*2 ) - nLeftPMgn;
        if( nPos < 0 )
            continue;
        aTabStop.GetTabPos() = nPos;
        switch( pTyp[i].jcGet() ){
        case 0: aTabStop.GetAdjustment() = SVX_TAB_ADJUST_LEFT; break;
        case 1:	aTabStop.GetAdjustment() = SVX_TAB_ADJUST_CENTER; break;
        case 2: aTabStop.GetAdjustment() = SVX_TAB_ADJUST_RIGHT; break;
        case 3:	aTabStop.GetAdjustment() = SVX_TAB_ADJUST_DECIMAL; break;
        case 4: continue;					// ignoriere Bar
        }

        switch( pTyp[i].tlcGet() ){
        case 0: aTabStop.GetFill() = ' '; break;
        case 1: aTabStop.GetFill() = '.'; break;
        case 2: aTabStop.GetFill() = '-'; break;
        case 3:
        case 4: aTabStop.GetFill() = '_'; break;
        }

        USHORT nPos2 = aAttr.GetPos( nPos );
        if( nPos2 != SVX_TAB_NOTFOUND )
            aAttr.Remove( nPos2, 1 );		// sonst weigert sich das Insert()

        aAttr.Insert( aTabStop );
    }
    rOut << aAttr;
}

STOP1(Ww1SingleSprmPChgTabsPapx, RES_PARATR_TABSTOP)

void Ww1SingleSprmSGprfIhdt::Start(
    Ww1Shell& /*rOut*/, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& rMan)
{
    rMan.GetSep().SetGrpfIhdt(SVBT8ToByte(pSprm));
}

void Ww1SingleSprmSColumns::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nColSpace = 708; // default 1,25 cm
    short nNettoWidth = -1; //~ mdt: dxaLeft/Right missing...
    short nCols = SVBT16ToShort(pSprm);
    nCols++;
    if (nCols < 2)
        return;
    SwFmtCol aCol;
    aCol.Init(nCols, nColSpace, nNettoWidth);
    rOut.GetPageDesc().GetMaster().SetFmtAttr(aCol);
}

void Ww1SingleSprmPTtp::Start(
    Ww1Shell& /*rOut*/, BYTE, BYTE*, USHORT, Ww1Manager& rMan)
{
    rMan.SetInTtp( TRUE );		// Besonderheit: wird bei InTable::Stop und
                                // nicht bei InTtp::Stop zurueckgesetzt,
                                // da Auswertung in InTable
}

void Ww1SingleSprmPTtp::Stop(
    Ww1Shell& rOut, BYTE, BYTE*, USHORT, Ww1Manager& rMan)
{
    if (rOut.IsInTable() && rMan.HasInTable())
        rOut.NextTableRow();
}

void Ww1SingleSprmPFInTable::Start(
    Ww1Shell& /*rOut*/, BYTE, BYTE*, USHORT,
    Ww1Manager& /*rMan*/)
{
}

void Ww1SingleSprmPFInTable::Stop(
    Ww1Shell&
#ifndef PRODUCT
    rOut
#endif
    ,
    BYTE, BYTE*, USHORT,
    Ww1Manager& rMan)
{
    ASSERT(rOut.IsInTable(), "");
    rMan.SetInTtp( FALSE );
}

void Ww1SingleSprmTDxaGapHalf::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nSpace = SVBT16ToShort(pSprm);
    rOut.SetCellSpace(nSpace);
}

void Ww1SingleSprmTDefTable10::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT nSize, Ww1Manager& /*rMan*/)
{
    USHORT i;
    BYTE *p = pSprm + 2;	// LaengenWord ueberlesen
    BYTE nCount = *p;
    p++;
    nSize -= 3;
// Es fehlt noch:
// - GapHalf
// - eventuelle Ausduennung der Zellenumrandungen

    if( nCount < 1 || nCount > 32 || nSize < ( nCount + 1 ) * 2  )
        return;

// Erstmal die Zellenpositionen einlesen
    short nPos = SVBT16ToShort( p );	// signed, kann auch neg. sein !!!

//	if( !rOut.IsTableWidthSet() ){		// Muss Tabellenbreite und -Ausrichtung
                                        // noch gesetzt werden ?
    {
        short nWholeWidth = SVBT16ToShort( p + 2 * nCount ) - nPos;
        rOut.SetTableWidth( (USHORT)nWholeWidth );	// Tabellenbreite setzen

// Pos der Tabelle setzen
        long nMidTab = nPos + nWholeWidth / 2;		// TabellenMitte
        const SwFrmFmt &rFmt = rOut.GetPageDesc().GetMaster();
        const SvxLRSpaceItem& rLR = rFmt.GetLRSpace();
        long nRight = rFmt.GetFrmSize().GetWidth()
                      - rLR.GetLeft() - rLR.GetRight();

        sal_Int16 eOri = text::HoriOrientation::LEFT;
        if( nPos > MINLAY ){						// per Zuppeln positioniert
            if ( nMidTab <= nRight / 3 )			// 1/3 der Seite
                eOri = text::HoriOrientation::LEFT;
            else if ( nMidTab <= 2 * nRight / 3 )	// 2/3 der Seite
                eOri = text::HoriOrientation::CENTER;
            else
                eOri = text::HoriOrientation::RIGHT;
        }
        rOut.SetTableOrient( eOri );
    }

    BYTE* pEndPos = p+2;
    BYTE* pTc0 = ( nSize >= nCount * 10 ) ? pEndPos + 2 * nCount : 0;
    USHORT nCellsDeleted = 0;		// fuer gemergte Zellen

    for( i = 0; i < nCount; i++ ){
// Info sammeln
        W1_TC* pTc = (W1_TC*)pTc0;
        BOOL bMerged = (pTc) ? pTc->fMergedGet() : FALSE;

// Zellenbreiten setzen
        USHORT nPos1 = SVBT16ToShort( pEndPos );
        if( !bMerged )
            rOut.SetCellWidth( nPos1 - nPos, i - nCellsDeleted );
                                        // Zellenbreite setzen
                                        // Wechselwirkung mit GapHalf fehlt noch
                                        // ( GapHalf wird noch ignoriert )
        pEndPos+=2;
        nPos = nPos1;

        if( pTc0 ){						// gibts TCs ueberhaupt ?
            W1_TC* pTc2 = (W1_TC*)pTc0;
            BOOL bMerged2 = pTc2->fMergedGet();
//			ASSERT( !bMerged2, "Gemergte Tabellenzellen noch nicht vollstaendig implementiert" );
            if( !bMerged2 ){
// und nun die Umrandungen
                SvxBoxItem aBox( (SvxBoxItem&)rOut.GetCellAttr( RES_BOX ));
                SvxBorderLine aLine;
                W1_BRC10* pBrc = pTc2->rgbrcGet();
                for( USHORT j=0; j<4; j++ ){
                    aBox.SetLine(Ww1SingleSprmPBrc::SetBorder(&aLine, pBrc),
                                 nBrcTrans[j]);
                    pBrc++;
                }
                rOut.SetCellBorder( aBox, i - nCellsDeleted );
            }else{
// gemergte Zellen entfernen
                rOut.DeleteCell( i - nCellsDeleted );
                nCellsDeleted++;
            }
            pTc0+=sizeof(W1_TC);	// 10
        }
    }
}


void Ww1SingleSprmTDyaRowHeight::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nSpace = SVBT16ToShort(pSprm);
    rOut.SetCellHeight(nSpace);
}

// Fastsave-Attribute brauche ich als Dymmys nicht

void Ww1SingleSprmPpc::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& rMan)
{
    BYTE nPpc = SVBT8ToByte(pSprm);

    if (rOut.IsInTable())						// Flys in Tabellen kann PMW
        return;									// nicht

    RndStdIds eAnchor;			// Bindung
    sal_Int16 eHRel;        // Seite oder Seitenrand
    sal_Int16 eVRel;        // Seite oder Seitenrand

    switch ( ( nPpc & 0x30 ) >> 4 ){		// Y - Bindung bestimmt Sw-Bindung
    case 0: eAnchor = FLY_AT_PARA;          // Vert Margin
            eVRel = text::RelOrientation::PRINT_AREA;
//			if( nYPos < 0 )
//				nYPos = 0;					// koennen wir nicht
            break;
/*	case 1:*/								// Vert. Seite
    default:eAnchor = FLY_AT_PAGE;          // Vert Page oder unknown
            eVRel = text::RelOrientation::FRAME;
            break;							// 2=Vert. Paragraph, 3=Use Default
    }

    switch ( ( nPpc & 0xc0 ) >> 6 ){        // X - Bindung -> Koordinatentransformation
    case 0: 								// Hor. Spalte
    case 1: 								// Hor. Absatz
            eHRel = text::RelOrientation::PRINT_AREA;
//			nXPos += nPgLeft;				// in Seiten-Koordinaten umrechnen
            break;
/*	case 2:*/								// Hor. Seite
    default:
            eHRel = text::RelOrientation::FRAME;
            break;
    }

    if( !rOut.IsInFly() && rMan.IsInStyle() ){
        rOut.BeginFly( eAnchor );			// Starte StyleFly
    }else{
        rOut.SetFlyAnchor( eAnchor );		// Setze Anker
    }
}

void Ww1SingleSprmPDxaAbs::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& rMan)
{
    short nXPos = SVBT16ToShort(pSprm);

    if( rMan.IsInStyle() && !rOut.IsInFly() ){
        rOut.BeginFly();					// Fly ohne PPc-Attribut
    }

    sal_Int16 eHRel = text::RelOrientation::FRAME;
    sal_Int16 eHAlign = text::HoriOrientation::NONE;

    switch( nXPos ){						// besondere X-Positionen ?
    case 0:
    case -12: eHAlign = text::HoriOrientation::NONE; nXPos = 0; break;   // Mogel: innen -> links
                            // eigentich text::HoriOrientation::LEFT, aber dann verrueckt
                            // ein Abstand nach aussen den Fly
    case -4:  eHAlign = text::HoriOrientation::CENTER; nXPos = 0; break; // zentriert
    case -8:                             			   // rechts
    case -16: eHAlign = text::HoriOrientation::RIGHT; nXPos = 0; break;  // Mogel: aussen -> rechts
//	default:  nXPos += (short)nIniFlyDx; break;	// Korrekturen per Ini-Datei
    }
    rOut.SetFlyXPos( nXPos, eHRel, eHAlign );
}

void Ww1SingleSprmPDyaAbs::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nYPos = SVBT16ToShort(pSprm);
    sal_Int16 eVRel = text::RelOrientation::FRAME;
    sal_Int16 eVAlign = text::VertOrientation::NONE;

    switch( nYPos ){							// besondere Y-Positionen ?
    case -4:  eVAlign = text::VertOrientation::TOP; nYPos = 0; break; // oben
    case -8:  eVAlign = text::VertOrientation::CENTER; nYPos = 0; break;  // zentriert
    case -12: eVAlign = text::VertOrientation::BOTTOM; nYPos = 0; break;  // unten
//	default:  nYPos += (short)nIniFlyDy; break;	// Korrekturen per Ini-Datei
    }
    rOut.SetFlyYPos( nYPos, eVRel, eVAlign );
}

void Ww1SingleSprmPDxaWidth::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    short nDxaWidth = SVBT16ToShort(pSprm);
    rOut.SetFlyFrmAttr( SwFmtFrmSize( ATT_VAR_SIZE, nDxaWidth, MINFLY ) );
}

void Ww1SingleSprmPFromText::Start(
    Ww1Shell& rOut, BYTE /*nId*/, BYTE* pSprm, USHORT /*nSize*/, Ww1Manager& /*rMan*/)
{
    if( rOut.IsInFly() ){	// Kommt auch ausserhalb eines Flys vor, hat
                            // dann aber offensichtlich nichts zu bedeuten.
                            // Einen impliziten Fly-Anfang bedeutet es
                            // definitiv nicht
        short nFromText = SVBT16ToShort(pSprm);

        SvxLRSpaceItem aLR( RES_LR_SPACE );
        aLR.SetTxtLeft( nFromText );
        aLR.SetRight( nFromText );
        rOut.SetFlyFrmAttr( aLR );

        rOut.SetFlyFrmAttr( SvxULSpaceItem( nFromText, nFromText, RES_UL_SPACE ) );
    }
}

#undef STOP1
#undef STOP2


