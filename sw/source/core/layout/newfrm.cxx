/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: newfrm.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-05 17:03:13 $
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

#ifndef _SVDMODEL_HXX //autogen
#include <svx/svdmodel.hxx>
#endif
#ifndef _SVDPAGE_HXX //autogen
#include <svx/svdpage.hxx>
#endif

#ifndef _FMTFORDR_HXX //autogen
#include <fmtfordr.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif
#ifndef _DFLYOBJ_HXX
#include <dflyobj.hxx>
#endif
#ifndef _FRMTOOL_HXX
#include <frmtool.hxx>
#endif
#ifndef _VIRTOUTP_HXX
#include <virtoutp.hxx>
#endif
#ifndef _BLINK_HXX
#include <blink.hxx>
#endif
#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif
#ifndef _SECTFRM_HXX
#include <sectfrm.hxx>
#endif
#ifndef _NOTXTFRM_HXX
#include <notxtfrm.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#include "viewimp.hxx"
#include "IDocumentTimerAccess.hxx"
#include "IDocumentLayoutAccess.hxx"
#include "IDocumentFieldsAccess.hxx"
#include "IDocumentSettingAccess.hxx"
#include "IDocumentDrawModelAccess.hxx"
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif

SwLayVout     *SwRootFrm::pVout = 0;
BOOL 		   SwRootFrm::bInPaint = FALSE;
BOOL 		   SwRootFrm::bNoVirDev = FALSE;

SwCache *SwFrm::pCache = 0;

long FirstMinusSecond( long nFirst, long nSecond )
    { return nFirst - nSecond; }
long SecondMinusFirst( long nFirst, long nSecond )
    { return nSecond - nFirst; }
long SwIncrement( long nA, long nAdd )
    { return nA + nAdd; }
long SwDecrement( long nA, long nSub )
    { return nA - nSub; }

static SwRectFnCollection aHorizontal = {
    /* fnRectGet      */
    &SwRect::_Top,
    &SwRect::_Bottom,
    &SwRect::_Left,
    &SwRect::_Right,
    &SwRect::_Width,
    &SwRect::_Height,
    &SwRect::TopLeft,
    &SwRect::_Size,
    /* fnRectSet      */
    &SwRect::_Top,
    &SwRect::_Bottom,
    &SwRect::_Left,
    &SwRect::_Right,
    &SwRect::_Width,
    &SwRect::_Height,

    &SwRect::SubTop,
    &SwRect::AddBottom,
    &SwRect::SubLeft,
    &SwRect::AddRight,
    &SwRect::AddWidth,
    &SwRect::AddHeight,

    &SwRect::SetPosX,
    &SwRect::SetPosY,

    &SwFrm::GetTopMargin,
    &SwFrm::GetBottomMargin,
    &SwFrm::GetLeftMargin,
    &SwFrm::GetRightMargin,
    &SwFrm::SetLeftRightMargins,
    &SwFrm::SetTopBottomMargins,
    &SwFrm::GetPrtTop,
    &SwFrm::GetPrtBottom,
    &SwFrm::GetPrtLeft,
    &SwFrm::GetPrtRight,
    &SwRect::GetTopDistance,
    &SwRect::GetBottomDistance,
    &SwRect::GetLeftDistance,
    &SwRect::GetRightDistance,
    &SwFrm::SetMaxBottom,
    &SwRect::OverStepBottom,

    &SwRect::SetUpperLeftCorner,
    &SwFrm::MakeBelowPos,
    &FirstMinusSecond,
    &FirstMinusSecond,
    &SwIncrement,
    &SwIncrement,
    &SwRect::SetLeftAndWidth,
    &SwRect::SetTopAndHeight
};

static SwRectFnCollection aVertical = {
    /* fnRectGet      */
    &SwRect::_Right,
    &SwRect::_Left,
    &SwRect::_Top,
    &SwRect::_Bottom,
    &SwRect::_Height,
    &SwRect::_Width,
    &SwRect::TopRight,
    &SwRect::SwappedSize,
    /* fnRectSet      */
    &SwRect::_Right,
    &SwRect::_Left,
    &SwRect::_Top,
    &SwRect::_Bottom,
    &SwRect::_Height,
    &SwRect::_Width,

    &SwRect::AddRight,
    &SwRect::SubLeft,
    &SwRect::SubTop,
    &SwRect::AddBottom,
    &SwRect::AddHeight,
    &SwRect::AddWidth,

    &SwRect::SetPosY,
    &SwRect::SetPosX,

    &SwFrm::GetRightMargin,
    &SwFrm::GetLeftMargin,
    &SwFrm::GetTopMargin,
    &SwFrm::GetBottomMargin,
    &SwFrm::SetTopBottomMargins,
    &SwFrm::SetRightLeftMargins,
    &SwFrm::GetPrtRight,
    &SwFrm::GetPrtLeft,
    &SwFrm::GetPrtTop,
    &SwFrm::GetPrtBottom,
    &SwRect::GetRightDistance,
    &SwRect::GetLeftDistance,
    &SwRect::GetTopDistance,
    &SwRect::GetBottomDistance,
    &SwFrm::SetMinLeft,
    &SwRect::OverStepLeft,

    &SwRect::SetUpperRightCorner,
    &SwFrm::MakeLeftPos,
    &FirstMinusSecond,
    &SecondMinusFirst,
    &SwIncrement,
    &SwDecrement,
    &SwRect::SetTopAndHeight,
    &SwRect::SetRightAndWidth
};

static SwRectFnCollection aBottomToTop = {
    /* fnRectGet      */
    &SwRect::_Bottom,
    &SwRect::_Top,
    &SwRect::_Left,
    &SwRect::_Right,
    &SwRect::_Width,
    &SwRect::_Height,
    &SwRect::BottomLeft,
    &SwRect::_Size,
    /* fnRectSet      */
    &SwRect::_Bottom,
    &SwRect::_Top,
    &SwRect::_Left,
    &SwRect::_Right,
    &SwRect::_Width,
    &SwRect::_Height,

    &SwRect::AddBottom,
    &SwRect::SubTop,
    &SwRect::SubLeft,
    &SwRect::AddRight,
    &SwRect::AddWidth,
    &SwRect::AddHeight,

    &SwRect::SetPosX,
    &SwRect::SetPosY,

    &SwFrm::GetBottomMargin,
    &SwFrm::GetTopMargin,
    &SwFrm::GetLeftMargin,
    &SwFrm::GetRightMargin,
    &SwFrm::SetLeftRightMargins,
    &SwFrm::SetBottomTopMargins,
    &SwFrm::GetPrtBottom,
    &SwFrm::GetPrtTop,
    &SwFrm::GetPrtLeft,
    &SwFrm::GetPrtRight,
    &SwRect::GetBottomDistance,
    &SwRect::GetTopDistance,
    &SwRect::GetLeftDistance,
    &SwRect::GetRightDistance,
    &SwFrm::SetMinTop,
    &SwRect::OverStepTop,

    &SwRect::SetLowerLeftCorner,
    &SwFrm::MakeUpperPos,
    &FirstMinusSecond,
    &SecondMinusFirst,
    &SwIncrement,
    &SwDecrement,
    &SwRect::SetLeftAndWidth,
    &SwRect::SetBottomAndHeight
};

static SwRectFnCollection aVerticalRightToLeft = {
    /* fnRectGet      */
    &SwRect::_Left,
    &SwRect::_Right,
    &SwRect::_Top,
    &SwRect::_Bottom,
    &SwRect::_Height,
    &SwRect::_Width,
    &SwRect::BottomRight,
    &SwRect::SwappedSize,
    /* fnRectSet      */
    &SwRect::_Left,
    &SwRect::_Right,
    &SwRect::_Top,
    &SwRect::_Bottom,
    &SwRect::_Height,
    &SwRect::_Width,

    &SwRect::SubLeft,
    &SwRect::AddRight,
    &SwRect::SubTop,
    &SwRect::AddBottom,
    &SwRect::AddHeight,
    &SwRect::AddWidth,

    &SwRect::SetPosY,
    &SwRect::SetPosX,

    &SwFrm::GetLeftMargin,
    &SwFrm::GetRightMargin,
    &SwFrm::GetTopMargin,
    &SwFrm::GetBottomMargin,
    &SwFrm::SetTopBottomMargins,
    &SwFrm::SetLeftRightMargins,
    &SwFrm::GetPrtLeft,
    &SwFrm::GetPrtRight,
    &SwFrm::GetPrtBottom,
    &SwFrm::GetPrtTop,
    &SwRect::GetLeftDistance,
    &SwRect::GetRightDistance,
    &SwRect::GetBottomDistance,
    &SwRect::GetTopDistance,
    &SwFrm::SetMaxRight,
    &SwRect::OverStepRight,

    &SwRect::SetLowerLeftCorner,
    &SwFrm::MakeRightPos,
    &FirstMinusSecond,
    &FirstMinusSecond,
    &SwDecrement,
    &SwIncrement,
    &SwRect::SetBottomAndHeight,
    &SwRect::SetLeftAndWidth
};

SwRectFn fnRectHori = &aHorizontal;
SwRectFn fnRectVert = &aVertical;
SwRectFn fnRectB2T = &aBottomToTop;
SwRectFn fnRectVL2R = &aVerticalRightToLeft;

// --> OD 2006-05-10 #i65250#
sal_uInt32 SwFrm::mnLastFrmId=0;
// <--

TYPEINIT1(SwFrm,SwClient);		//rtti fuer SwFrm
TYPEINIT1(SwCntntFrm,SwFrm);	//rtti fuer SwCntntFrm


void _FrmInit()
{
    SwRootFrm::pVout = new SwLayVout();
    SwCache *pNew = new SwCache( 100, 100
#ifndef PRODUCT
    , "static SwBorderAttrs::pCache"
#endif
    );
    SwFrm::SetCache( pNew );
}



void _FrmFinit()
{
#ifndef PRODUCT
    // im Chache duerfen nur noch 0-Pointer stehen
    for( USHORT n = SwFrm::GetCachePtr()->Count(); n; )
        if( (*SwFrm::GetCachePtr())[ --n ] )
        {
            SwCacheObj* pObj = (*SwFrm::GetCachePtr())[ n ];
            ASSERT( !pObj, "Wer hat sich nicht ausgetragen?")
        }
#endif
    delete SwRootFrm::pVout;
    delete SwFrm::GetCachePtr();
}

/*************************************************************************
|*
|*	RootFrm::Alles was so zur CurrShell gehoert
|*
|*	Ersterstellung		MA 09. Sep. 98
|*	Letzte Aenderung	MA 18. Feb. 99
|*
|*************************************************************************/

typedef CurrShell* CurrShellPtr;
SV_DECL_PTRARR_SORT(SwCurrShells,CurrShellPtr,4,4)
SV_IMPL_PTRARR_SORT(SwCurrShells,CurrShellPtr)

CurrShell::CurrShell( ViewShell *pNew )
{
    ASSERT( pNew, "0-Shell einsetzen?" );
    pRoot = pNew->GetLayout();
    if ( pRoot )
    {
        pPrev = pRoot->pCurrShell;
        pRoot->pCurrShell = pNew;
        pRoot->pCurrShells->Insert( this );
    }
    else
        pPrev = 0;
}

CurrShell::~CurrShell()
{
    if ( pRoot )
    {
        pRoot->pCurrShells->Remove( this );
        if ( pPrev )
            pRoot->pCurrShell = pPrev;
        if ( !pRoot->pCurrShells->Count() && pRoot->pWaitingCurrShell )
        {
            pRoot->pCurrShell = pRoot->pWaitingCurrShell;
            pRoot->pWaitingCurrShell = 0;
        }
    }
}

void SetShell( ViewShell *pSh )
{
    SwRootFrm *pRoot = pSh->GetLayout();
    if ( !pRoot->pCurrShells->Count() )
        pRoot->pCurrShell = pSh;
    else
        pRoot->pWaitingCurrShell = pSh;
}

void SwRootFrm::DeRegisterShell( ViewShell *pSh )
{
    //Wenn moeglich irgendeine Shell aktivieren
    if ( pCurrShell == pSh )
        pCurrShell = pSh->GetNext() != pSh ? (ViewShell*)pSh->GetNext() : 0;

    //Das hat sich eruebrigt
    if ( pWaitingCurrShell == pSh )
        pWaitingCurrShell = 0;

    //Referenzen entfernen.
    for ( USHORT i = 0; i < pCurrShells->Count(); ++i )
    {
        CurrShell *pC = (*pCurrShells)[i];
        if (pC->pPrev == pSh)
            pC->pPrev = 0;
    }
}

void InitCurrShells( SwRootFrm *pRoot )
{
    pRoot->pCurrShells = new SwCurrShells;
}


/*************************************************************************
|*
|*	SwRootFrm::SwRootFrm()
|*
|*	Beschreibung:
|* 		Der RootFrm laesst sich grundsaetzlich vom Dokument ein eigenes
|* 		FrmFmt geben. Dieses loescht er dann selbst im DTor.
|* 		Das eigene FrmFmt wird vom uebergebenen Format abgeleitet.
|*	Ersterstellung		SS 05-Apr-1991
|*	Letzte Aenderung	MA 12. Dec. 94
|*
|*************************************************************************/


SwRootFrm::SwRootFrm( SwFrmFmt *pFmt, ViewShell * pSh ) :
    SwLayoutFrm( pFmt->GetDoc()->MakeFrmFmt(
        XubString( "Root", RTL_TEXTENCODING_MS_1252 ), pFmt ) ),
    nBrowseWidth( MM50*4 ), //2cm Minimum
    pTurbo( 0 ),
    pLastPage( 0 ),
    pCurrShell( pSh ),
    pWaitingCurrShell( 0 ),
    pDrawPage( 0 ),
    pDestroy( 0 ),
    nPhyPageNums( 0 ),
    nAccessibleShells( 0 )
{
    nType = FRMC_ROOT;
    bIdleFormat = bTurboAllowed = bAssertFlyPages = bIsNewLayout = TRUE;
    bCheckSuperfluous = bBrowseWidthValid = FALSE;

    InitCurrShells( this );

    IDocumentTimerAccess *pTimerAccess = pFmt->getIDocumentTimerAccess();
    IDocumentLayoutAccess *pLayoutAccess = pFmt->getIDocumentLayoutAccess();
    IDocumentFieldsAccess *pFieldsAccess = pFmt->getIDocumentFieldsAccess();
    const IDocumentSettingAccess *pSettingAccess = pFmt->getIDocumentSettingAccess();
    pTimerAccess->StopIdling();
    pLayoutAccess->SetRootFrm( this );		//Fuer das Erzeugen der Flys durch MakeFrms()
    bCallbackActionEnabled = FALSE;	//vor Verlassen auf TRUE setzen!

    SdrModel *pMd = pFmt->getIDocumentDrawModelAccess()->GetDrawModel();

    if ( pMd )
    {
        pDrawPage = pMd->GetPage( 0 );
        pDrawPage->SetSize( Frm().SSize() );
    }

    //Initialisierung des Layouts: Seiten erzeugen. Inhalt mit cntnt verbinden
    //usw.
    //Zuerst einiges initialiseren und den ersten Node besorgen (der wird
    //fuer den PageDesc benoetigt).

    SwDoc* pDoc = pFmt->GetDoc();
    SwNodeIndex aIndex( *pDoc->GetNodes().GetEndOfContent().StartOfSectionNode() );
    SwCntntNode *pNode = pDoc->GetNodes().GoNextSection( &aIndex, TRUE, FALSE );
    // --> FME 2005-05-25 #123067# pNode = 0 can really happen:
    SwTableNode *pTblNd= pNode ? pNode->FindTableNode() : 0;
    // <--

    //PageDesc besorgen (entweder vom FrmFmt des ersten Node oder den
    //initialen.)
    SwPageDesc *pDesc = 0;
    USHORT nPgNum = 1;

    if ( pTblNd )
    {
        const SwFmtPageDesc &rDesc = pTblNd->GetTable().GetFrmFmt()->GetPageDesc();
        pDesc = (SwPageDesc*)rDesc.GetPageDesc();
        //#19104# Seitennummeroffset beruecksictigen!!
        bIsVirtPageNum = 0 != ( nPgNum = rDesc.GetNumOffset() );
    }
    else if ( pNode )
    {
        const SwFmtPageDesc &rDesc = pNode->GetSwAttrSet().GetPageDesc();
        pDesc = (SwPageDesc*)rDesc.GetPageDesc();
        //#19104# Seitennummeroffset beruecksictigen!!
        bIsVirtPageNum = 0 != ( nPgNum = rDesc.GetNumOffset() );
    }
    else
        bIsVirtPageNum = FALSE;
    if ( !pDesc )
        pDesc = (SwPageDesc*)
            &const_cast<const SwDoc *>(pDoc)->GetPageDesc( 0 );
    const BOOL bOdd = !nPgNum || 0 != ( nPgNum % 2 );

    //Eine Seite erzeugen und in das Layout stellen
    SwPageFrm *pPage = ::InsertNewPage( *pDesc, this, bOdd, FALSE, FALSE, 0 );

    //Erstes Blatt im Bodytext-Bereich suchen.
    SwLayoutFrm *pLay = pPage->FindBodyCont();
    while( pLay->Lower() )
        pLay = (SwLayoutFrm*)pLay->Lower();

    SwNodeIndex aTmp( *pDoc->GetNodes().GetEndOfContent().StartOfSectionNode(), 1 );
    ::_InsertCnt( pLay, pDoc, aTmp.GetIndex(), TRUE );
    //Noch nicht ersetzte Master aus der Liste entfernen.
    RemoveMasterObjs( pDrawPage );
    if( pSettingAccess->get(IDocumentSettingAccess::GLOBAL_DOCUMENT) )
        pFieldsAccess->UpdateRefFlds( NULL );
    //b6433357: Update page fields after loading
    // --->
    if ( !pCurrShell || !pCurrShell->Imp()->IsUpdateExpFlds() )
    {
        SwDocPosUpdate aMsgHnt( pPage->Frm().Top() );
        pFieldsAccess->UpdatePageFlds( &aMsgHnt );
    }
    // <---

    pTimerAccess->StartIdling();
    bCallbackActionEnabled = TRUE;
}

/*************************************************************************
|*
|*	SwRootFrm::~SwRootFrm()
|*
|*	Ersterstellung		SS 05-Apr-1991
|*	Letzte Aenderung	MA 12. Dec. 94
|*
|*************************************************************************/



SwRootFrm::~SwRootFrm()
{
    bTurboAllowed = FALSE;
    pTurbo = 0;
    if(pBlink)
        pBlink->FrmDelete( this );
    ((SwFrmFmt*)pRegisteredIn)->GetDoc()->DelFrmFmt( (SwFrmFmt*)pRegisteredIn );
    delete pDestroy;

    //Referenzen entfernen.
    for ( USHORT i = 0; i < pCurrShells->Count(); ++i )
        (*pCurrShells)[i]->pRoot = 0;

    delete pCurrShells;

    ASSERT( 0==nAccessibleShells, "Some accessible shells are left" );
}

/*************************************************************************
|*
|*	SwRootFrm::RemoveMasterObjs()
|*
|*	Ersterstellung		MA 19.10.95
|*	Letzte Aenderung	MA 19.10.95
|*
|*************************************************************************/


void SwRootFrm::RemoveMasterObjs( SdrPage *pPg )
{
    //Alle Masterobjekte aus der Page entfernen. Nicht loeschen!!
    for( ULONG i = pPg ? pPg->GetObjCount() : 0; i; )
    {
        SdrObject* pObj = pPg->GetObj( --i );
        if( pObj->ISA(SwFlyDrawObj ) )
            pPg->RemoveObject( i );
    }
}




