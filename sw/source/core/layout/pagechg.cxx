/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pagechg.cxx,v $
 *
 *  $Revision: 1.46 $
 *
 *  last change: $Author: vg $ $Date: 2008-02-12 16:10:20 $
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


#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _SFXITEMITER_HXX //autogen
#include <svtools/itemiter.hxx>
#endif

#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _FMTHDFT_HXX //autogen
#include <fmthdft.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _FMTFORDR_HXX //autogen
#include <fmtfordr.hxx>
#endif
#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _FTNINFO_HXX //autogen
#include <ftninfo.hxx>
#endif
#include <tgrditem.hxx>

#include "viewimp.hxx"
#include "pagefrm.hxx"
#include "rootfrm.hxx"
#include "cntfrm.hxx"
#include "flyfrm.hxx"
#include "doc.hxx"
#include "fesh.hxx"
#include "dview.hxx"
#include "dflyobj.hxx"
#include "dcontact.hxx"
#include "frmtool.hxx"
#include "fldbas.hxx"
#include "hints.hxx"
#include "errhdl.hxx"
#include "swtable.hxx"

#include "ftnidx.hxx"
#include "bodyfrm.hxx"
#include "ftnfrm.hxx"
#include "tabfrm.hxx"
#include "txtfrm.hxx"
#include "layact.hxx"
#include "flyfrms.hxx"
#include "htmltbl.hxx"
#include "pagedesc.hxx"
#include "poolfmt.hxx"
#ifndef _SVX_FRMDIRITEM_HXX
#include <svx/frmdiritem.hxx>
#endif
#ifndef _SWFNTCCH_HXX
#include <swfntcch.hxx>	// SwFontAccess
#endif
// OD 2004-05-24 #i28701#
#ifndef _SORTEDOBJS_HXX
#include <sortedobjs.hxx>
#endif

using namespace ::com::sun::star;


/*************************************************************************
|*
|*	SwBodyFrm::SwBodyFrm()
|*
|*	Ersterstellung		MA ??
|*	Letzte Aenderung	MA 01. Aug. 93
|*
|*************************************************************************/
SwBodyFrm::SwBodyFrm( SwFrmFmt *pFmt ):
    SwLayoutFrm( pFmt )
{
    nType = FRMC_BODY;
}

/*************************************************************************
|*
|*	SwBodyFrm::Format()
|*
|*	Ersterstellung		MA 30. May. 94
|*	Letzte Aenderung	MA 20. Jan. 99
|*
|*************************************************************************/
void SwBodyFrm::Format( const SwBorderAttrs * )
{
    //Formatieren des Body ist zu einfach, deshalb bekommt er ein eigenes
    //Format; Umrandungen und dergl. sind hier nicht zu beruecksichtigen.
    //Breite ist die der PrtArea des Uppers, Hoehe ist die Hoehe der PrtArea
    //des Uppers abzueglich der Nachbarn (Wird eigentlich eingestellt aber
    //Vorsicht ist die Mutter der Robustheit).
    //Die PrtArea ist stets so gross wie der Frm itself.

    if ( !bValidSize )
    {
        SwTwips nHeight = GetUpper()->Prt().Height();
        SwTwips nWidth = GetUpper()->Prt().Width();
        const SwFrm *pFrm = GetUpper()->Lower();
        do
        {
            if ( pFrm != this )
            {
                if( pFrm->IsVertical() )
                    nWidth -= pFrm->Frm().Width();
                else
                    nHeight -= pFrm->Frm().Height();
            }
            pFrm = pFrm->GetNext();
        } while ( pFrm );
        if ( nHeight < 0 )
            nHeight = 0;
        Frm().Height( nHeight );
        if( IsVertical() && !IsReverse() && nWidth != Frm().Width() )
            Frm().Pos().X() += Frm().Width() - nWidth;
        Frm().Width( nWidth );
    }

    BOOL bNoGrid = TRUE;
    if( GetUpper()->IsPageFrm() && ((SwPageFrm*)GetUpper())->HasGrid() )
    {
        GETGRID( ((SwPageFrm*)GetUpper()) )
        if( pGrid )
        {
            bNoGrid = FALSE;
            long nSum = pGrid->GetBaseHeight() + pGrid->GetRubyHeight();
            SWRECTFN( this )
            long nSize = (Frm().*fnRect->fnGetWidth)();
            long nBorder = 0;
            if( GRID_LINES_CHARS == pGrid->GetGridType() )
            {
                nBorder = nSize % pGrid->GetBaseHeight();
                nSize -= nBorder;
                nBorder /= 2;
            }
            (Prt().*fnRect->fnSetPosX)( nBorder );
            (Prt().*fnRect->fnSetWidth)( nSize );

            // Height of body frame:
            nBorder = (Frm().*fnRect->fnGetHeight)();

            // Number of possible lines in area of body frame:
            long nNumberOfLines = nBorder / nSum;
            if( nNumberOfLines > pGrid->GetLines() )
                nNumberOfLines = pGrid->GetLines();

            // Space required for nNumberOfLines lines:
            nSize = nNumberOfLines * nSum;
            nBorder -= nSize;
            nBorder /= 2;

            // #i21774# Footnotes and centering the grid does not work together:
            const bool bAdjust = 0 == ((SwPageFrm*)GetUpper())->GetFmt()->GetDoc()->
                                        GetFtnIdxs().Count();

            (Prt().*fnRect->fnSetPosY)( bAdjust ? nBorder : 0 );
            (Prt().*fnRect->fnSetHeight)( nSize );
        }
    }
    if( bNoGrid )
    {
        Prt().Pos().X() = Prt().Pos().Y() = 0;
        Prt().Height( Frm().Height() );
        Prt().Width( Frm().Width() );
    }
    bValidSize = bValidPrtArea = TRUE;
}

/*************************************************************************
|*
|*	SwPageFrm::SwPageFrm(), ~SwPageFrm()
|*
|*	Ersterstellung		MA 20. Oct. 92
|*	Letzte Aenderung	MA 08. Dec. 97
|*
|*************************************************************************/
SwPageFrm::SwPageFrm( SwFrmFmt *pFmt, SwPageDesc *pPgDsc ) :
    SwFtnBossFrm( pFmt ),
    pSortedObjs( 0 ),
    pDesc( pPgDsc ),
    nPhyPageNum( 0 ),
    // OD 2004-05-17 #i28701#
    mbLayoutInProgress( false )
{
    SetDerivedVert( FALSE );
    SetDerivedR2L( FALSE );
    if( pDesc )
    {
        bHasGrid = TRUE;
        GETGRID( this )
        if( !pGrid )
            bHasGrid = FALSE;
    }
    else
        bHasGrid = FALSE;
    SetMaxFtnHeight( pPgDsc->GetFtnInfo().GetHeight() ?
                     pPgDsc->GetFtnInfo().GetHeight() : LONG_MAX ),
    nType = FRMC_PAGE;
    bInvalidLayout = bInvalidCntnt = bInvalidSpelling = bInvalidSmartTags = bInvalidAutoCmplWrds = bInvalidWordCount = TRUE;
    bInvalidFlyLayout = bInvalidFlyCntnt = bInvalidFlyInCnt = bFtnPage = bEndNotePage = FALSE;

    const bool bBrowseMode = pFmt->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE);
    if ( bBrowseMode )
    {
        Frm().Height( 0 );
        ViewShell *pSh = GetShell();
        long nWidth = pSh ? pSh->VisArea().Width():0;
        if ( !nWidth )
            nWidth = 5000L;		//aendert sich sowieso
        Frm().Width ( nWidth );
    }
    else
        Frm().SSize( pFmt->GetFrmSize().GetSize() );

    //Body-Bereich erzeugen und einsetzen, aber nur wenn ich nicht gerade
    //eine Leerseite bin.
    SwDoc *pDoc = pFmt->GetDoc();
    if ( FALSE == (bEmptyPage = pFmt == pDoc->GetEmptyPageFmt()) )
    {
        bEmptyPage = FALSE;
        Calc();								//Damit die PrtArea stimmt.
        SwBodyFrm *pBodyFrm = new SwBodyFrm( pDoc->GetDfltFrmFmt() );
        pBodyFrm->ChgSize( Prt().SSize() );
        pBodyFrm->Paste( this );
        pBodyFrm->Calc();					//Damit die Spalten korrekt
                                            //eingesetzt werden koennen.
        pBodyFrm->InvalidatePos();

        if ( bBrowseMode )
            _InvalidateSize();		//Alles nur gelogen

        //Header/Footer einsetzen, nur rufen wenn aktiv.
        if ( pFmt->GetHeader().IsActive() )
            PrepareHeader();
        if ( pFmt->GetFooter().IsActive() )
            PrepareFooter();

        const SwFmtCol &rCol = pFmt->GetCol();
        if ( rCol.GetNumCols() > 1 )
        {
            const SwFmtCol aOld; //ChgColumns() verlaesst sich darauf, dass ein
                                 //Old-Wert hereingereicht wird.
            pBodyFrm->ChgColumns( aOld, rCol );
        }
    }
}

SwPageFrm::~SwPageFrm()
{
    //FlyContainer entleeren, delete der Flys uebernimmt der Anchor
    //(Basisklasse SwFrm)
    if ( pSortedObjs )
    {
        //Objekte koennen (warum auch immer) auch an Seiten verankert sein,
        //die vor Ihren Ankern stehen. Dann wuerde auf bereits freigegebenen
        //Speicher zugegriffen.
        for ( USHORT i = 0; i < pSortedObjs->Count(); ++i )
        {
            SwAnchoredObject* pAnchoredObj = (*pSortedObjs)[i];
            pAnchoredObj->SetPageFrm( 0L );
        }
        delete pSortedObjs;
        pSortedObjs = 0;		//Auf 0 setzen, sonst rauchts beim Abmdelden von Flys!
    }

    //Damit der Zugriff auf zerstoerte Seiten verhindert werden kann.
    if ( !IsEmptyPage() ) //#59184# sollte fuer Leerseiten unnoetig sein.
    {
        SwDoc *pDoc = GetFmt()->GetDoc();
        if( pDoc && !pDoc->IsInDtor() )
        {
            ViewShell *pSh = GetShell();
            if ( pSh )
            {
                SwViewImp *pImp = pSh->Imp();
                pImp->SetFirstVisPageInvalid();
                if ( pImp->IsAction() )
                    pImp->GetLayAction().SetAgain();
                // OD 12.02.2003 #i9719#, #105645# - retouche area of page
                // including border and shadow area.
                SwRect aRetoucheRect;
                GetBorderAndShadowBoundRect( Frm(), pSh, aRetoucheRect );
                pSh->AddPaintRect( aRetoucheRect );
            }
        }
    }
}


void SwPageFrm::CheckGrid( BOOL bInvalidate )
{
    BOOL bOld = bHasGrid;
    bHasGrid = TRUE;
    GETGRID( this )
    bHasGrid = 0 != pGrid;
    if( bInvalidate || bOld != bHasGrid )
    {
        SwLayoutFrm* pBody = FindBodyCont();
        if( pBody )
        {
            pBody->InvalidatePrt();
            SwCntntFrm* pFrm = pBody->ContainsCntnt();
            while( pBody->IsAnLower( pFrm ) )
            {
                ((SwTxtFrm*)pFrm)->Prepare( PREP_CLEAR );
                pFrm = pFrm->GetNextCntntFrm();
            }
        }
        SetCompletePaint();
    }
}


void SwPageFrm::CheckDirection( BOOL bVert )
{
    UINT16 nDir =
            ((SvxFrameDirectionItem&)GetFmt()->GetAttr( RES_FRAMEDIR )).GetValue();
    if( bVert )
    {
        if( FRMDIR_HORI_LEFT_TOP == nDir || FRMDIR_HORI_RIGHT_TOP == nDir ||
            GetFmt()->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) )
            bVertical = 0;
        else
            bVertical = 1;
/*
        if( pDesc && pDesc->GetName().GetChar(0)=='x')
            bReverse = 1;
        else
 */
            bReverse = 0;
        bInvalidVert = 0;
    }
    else
    {
        if( FRMDIR_HORI_RIGHT_TOP == nDir )
            bRightToLeft = 1;
        else
            bRightToLeft = 0;
        bInvalidR2L = 0;
    }
}

/*************************************************************************
|*
|*	SwPageFrm::PreparePage()
|*
|*	Beschreibung		Erzeugt die Spezifischen Flys zur Seite und formatiert
|* 		generischen Cntnt
|*	Ersterstellung		MA 20. Oct. 92
|*	Letzte Aenderung	MA 09. Nov. 95
|*
|*************************************************************************/
void MA_FASTCALL lcl_FormatLay( SwLayoutFrm *pLay )
{
    //Alle LayoutFrms - nicht aber Tables, Flys o.ae. - formatieren.

    SwFrm *pTmp = pLay->Lower();
    //Erst die untergeordneten
    while ( pTmp )
    {
        if ( pTmp->GetType() & 0x00FF )
            ::lcl_FormatLay( (SwLayoutFrm*)pTmp );
        pTmp = pTmp->GetNext();
    }
    pLay->Calc();
}

void MA_FASTCALL lcl_MakeObjs( const SwSpzFrmFmts &rTbl, SwPageFrm *pPage )
{
    //Anlegen bzw. registrieren von Flys und Drawobjekten.
    //Die Formate stehen in der SpzTbl (vom Dokument).
    //Flys werden angelegt, DrawObjekte werden bei der Seite angemeldet.

    for ( USHORT i = 0; i < rTbl.Count(); ++i )
    {
        SdrObject *pSdrObj;
        SwFrmFmt *pFmt = rTbl[i];
        const SwFmtAnchor &rAnch = pFmt->GetAnchor();
        if ( rAnch.GetPageNum() == pPage->GetPhyPageNum() )
        {
            if( rAnch.GetCntntAnchor() )
            {
                if( FLY_PAGE == rAnch.GetAnchorId() )
                {
                    SwFmtAnchor aAnch( rAnch );
                    aAnch.SetAnchor( 0 );
                    pFmt->SetAttr( aAnch );
                }
                else
                    continue;
            }

            //Wird ein Rahmen oder ein SdrObject beschrieben?
            BOOL bSdrObj = RES_DRAWFRMFMT == pFmt->Which();
            pSdrObj = 0;
            if ( bSdrObj  && 0 == (pSdrObj = pFmt->FindSdrObject()) )
            {
                ASSERT( FALSE, "DrawObject not found." );
                pFmt->GetDoc()->DelFrmFmt( pFmt );
                --i;
                continue;
            }
            //Das Objekt kann noch an einer anderen Seite verankert sein.
            //Z.B. beim Einfuegen einer neuen Seite aufgrund eines
            //Pagedescriptor-Wechsels. Das Objekt muss dann umgehaengt
            //werden.
            //Fuer bestimmte Faelle ist das Objekt bereits an der richtigen
            //Seite verankert. Das wird hier automatisch erledigt und braucht
            //- wenngleich performater machbar - nicht extra codiert werden.
            SwPageFrm *pPg = pPage->IsEmptyPage() ? (SwPageFrm*)pPage->GetNext() : pPage;
            if ( bSdrObj )
            {
                // OD 23.06.2003 #108784# - consider 'virtual' drawing objects
                SwDrawContact *pContact =
                            static_cast<SwDrawContact*>(::GetUserCall(pSdrObj));
                if ( pSdrObj->ISA(SwDrawVirtObj) )
                {
                    SwDrawVirtObj* pDrawVirtObj = static_cast<SwDrawVirtObj*>(pSdrObj);
                    if ( pContact )
                    {
                        pDrawVirtObj->RemoveFromWriterLayout();
                        pDrawVirtObj->RemoveFromDrawingPage();
                        pPg->AppendDrawObj( *(pContact->GetAnchoredObj( pDrawVirtObj )) );
                    }
                }
                else
                {
                    if ( pContact->GetAnchorFrm() )
                        pContact->DisconnectFromLayout( false );
                    pPg->AppendDrawObj( *(pContact->GetAnchoredObj( pSdrObj )) );
                }
            }
            else
            {
                SwClientIter aIter( *pFmt );
                SwClient *pTmp = aIter.First( TYPE(SwFrm) );
                SwFlyFrm *pFly;
                if ( pTmp )
                {
                    pFly = (SwFlyFrm*)pTmp;
                    if( pFly->GetAnchorFrm() )
                        pFly->AnchorFrm()->RemoveFly( pFly );
                }
                else
                    pFly = new SwFlyLayFrm( (SwFlyFrmFmt*)pFmt, pPg );
                pPg->AppendFly( pFly );
                ::RegistFlys( pPg, pFly );
            }
        }
    }
}

void SwPageFrm::PreparePage( BOOL bFtn )
{
    SetFtnPage( bFtn );

    // --> OD 2008-01-30 #i82258#
    // Due to made change on OOo 2.0 code line, method <::lcl_FormatLay(..)> has
    // the side effect, that the content of page header and footer are formatted.
    // For this formatting it is needed that the anchored objects are registered
    // at the <SwPageFrm> instance.
    // Thus, first calling <::RegistFlys(..)>, then call <::lcl_FormatLay(..)>
    ::RegistFlys( this, this );

        if ( Lower() )
    {
                ::lcl_FormatLay( this );
    }
    // <--

    //Flys und DrawObjekte die noch am Dokument bereitstehen.
    //Fussnotenseiten tragen keine Seitengebundenen Flys!
    //Es kann Flys und Objekte geben, die auf Leerseiten (Seitennummernmaessig)
    //stehen wollen, diese werden jedoch von den Leerseiten ignoriert;
    //sie werden von den Folgeseiten aufgenommen.
    if ( !bFtn && !IsEmptyPage() )
    {
        SwDoc *pDoc = GetFmt()->GetDoc();

        if ( GetPrev() && ((SwPageFrm*)GetPrev())->IsEmptyPage() )
            lcl_MakeObjs( *pDoc->GetSpzFrmFmts(), (SwPageFrm*)GetPrev() );
        lcl_MakeObjs( *pDoc->GetSpzFrmFmts(), this );

        //Kopf-/Fusszeilen) formatieren.
        SwLayoutFrm *pLow = (SwLayoutFrm*)Lower();
        while ( pLow )
        {
            if ( pLow->GetType() & (FRMTYPE_HEADER|FRMTYPE_FOOTER) )
            {
                SwCntntFrm *pCntnt = pLow->ContainsCntnt();
                while ( pCntnt && pLow->IsAnLower( pCntnt ) )
                {
                    pCntnt->OptCalc();	//Nicht die Vorgaenger
                    pCntnt = pCntnt->GetNextCntntFrm();
                }
            }
            pLow = (SwLayoutFrm*)pLow->GetNext();
        }
    }
}

/*************************************************************************
|*
|*	SwPageFrm::Modify()
|*
|*	Ersterstellung		MA 20. Oct. 92
|*	Letzte Aenderung	MA 03. Mar. 96
|*
|*************************************************************************/
void SwPageFrm::Modify( SfxPoolItem * pOld, SfxPoolItem * pNew )
{
    ViewShell *pSh = GetShell();
    if ( pSh )
        pSh->SetFirstVisPageInvalid();
    BYTE nInvFlags = 0;

    if( pNew && RES_ATTRSET_CHG == pNew->Which() )
    {
        SfxItemIter aNIter( *((SwAttrSetChg*)pNew)->GetChgSet() );
        SfxItemIter aOIter( *((SwAttrSetChg*)pOld)->GetChgSet() );
        SwAttrSetChg aOldSet( *(SwAttrSetChg*)pOld );
        SwAttrSetChg aNewSet( *(SwAttrSetChg*)pNew );
        while( TRUE )
        {
            _UpdateAttr( (SfxPoolItem*)aOIter.GetCurItem(),
                         (SfxPoolItem*)aNIter.GetCurItem(), nInvFlags,
                         &aOldSet, &aNewSet );
            if( aNIter.IsAtEnd() )
                break;
            aNIter.NextItem();
            aOIter.NextItem();
        }
        if ( aOldSet.Count() || aNewSet.Count() )
            SwLayoutFrm::Modify( &aOldSet, &aNewSet );
    }
    else
        _UpdateAttr( pOld, pNew, nInvFlags );

    if ( nInvFlags != 0 )
    {
        InvalidatePage( this );
        if ( nInvFlags & 0x01 )
            _InvalidatePrt();
        if ( nInvFlags & 0x02 )
            SetCompletePaint();
        if ( nInvFlags & 0x04 && GetNext() )
            GetNext()->InvalidatePos();
        if ( nInvFlags & 0x08 )
            PrepareHeader();
        if ( nInvFlags & 0x10 )
            PrepareFooter();
        if ( nInvFlags & 0x20 )
            CheckGrid( nInvFlags & 0x40 );
    }
}

void SwPageFrm::_UpdateAttr( SfxPoolItem *pOld, SfxPoolItem *pNew,
                             BYTE &rInvFlags,
                             SwAttrSetChg *pOldSet, SwAttrSetChg *pNewSet )
{
    BOOL bClear = TRUE;
    const USHORT nWhich = pOld ? pOld->Which() : pNew ? pNew->Which() : 0;
    switch( nWhich )
    {
        case RES_FMT_CHG:
        {
            //Wenn sich das FrmFmt aendert kann hier einiges passieren.
            //Abgesehen von den Grossenverhaeltnissen sind noch andere
            //Dinge betroffen.
            //1. Spaltigkeit.
            ASSERT( pOld && pNew, "FMT_CHG Missing Format." );
            const SwFmt* pOldFmt = ((SwFmtChg*)pOld)->pChangedFmt;
            const SwFmt* pNewFmt = ((SwFmtChg*)pNew)->pChangedFmt;
            ASSERT( pOldFmt && pNewFmt, "FMT_CHG Missing Format." );

            const SwFmtCol &rOldCol = pOldFmt->GetCol();
            const SwFmtCol &rNewCol = pNewFmt->GetCol();
            if( rOldCol != rNewCol )
            {
                SwLayoutFrm *pB = FindBodyCont();
                ASSERT( pB, "Seite ohne Body." );
                pB->ChgColumns( rOldCol, rNewCol );
                rInvFlags |= 0x20;
            }

            //2. Kopf- und Fusszeilen.
            const SwFmtHeader &rOldH = pOldFmt->GetHeader();
            const SwFmtHeader &rNewH = pNewFmt->GetHeader();
            if( rOldH != rNewH )
                rInvFlags |= 0x08;

            const SwFmtFooter &rOldF = pOldFmt->GetFooter();
            const SwFmtFooter &rNewF = pNewFmt->GetFooter();
            if( rOldF != rNewF )
                rInvFlags |= 0x10;
            CheckDirChange();
        }
            /* kein break hier */
        case RES_FRM_SIZE:
        {
            const SwRect aOldPageFrmRect( Frm() );
            if ( GetFmt()->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) )
            {
                bValidSize = FALSE;
                // OD 28.10.2002 #97265# - Don't call <SwPageFrm::MakeAll()>
                // Calculation of the page is not necessary, because its size is
                // is invalidated here and further invalidation is done in the
                // calling method <SwPageFrm::Modify(..)> and probably by calling
                // <SwLayoutFrm::Modify(..)> at the end.
                // It can also causes inconsistences, because the lowers are
                // adjusted, but not calculated, and a <SwPageFrm::MakeAll()> of
                // a next page is called. This is performed on the switch to the
                // online layout.
                //MakeAll();
            }
            else
            {
                const SwFmtFrmSize &rSz = nWhich == RES_FMT_CHG ?
                        ((SwFmtChg*)pNew)->pChangedFmt->GetFrmSize() :
                        (const SwFmtFrmSize&)*pNew;

                Frm().Height( Max( rSz.GetHeight(), long(MINLAY) ) );
                Frm().Width ( Max( rSz.GetWidth(),	long(MINLAY) ) );
                AdjustRootSize( CHG_CHGPAGE, &aOldPageFrmRect );
            }
            //Window aufraeumen.
            ViewShell *pSh;
            if ( 0 != (pSh = GetShell()) && pSh->GetWin() && aOldPageFrmRect.HasArea() )
            {
                // OD 12.02.2003 #i9719#, #105645# - consider border and shadow of
                // page frame for determine 'old' rectangle - it's used for invalidating.
                SwRect aOldRectWithBorderAndShadow;
                GetBorderAndShadowBoundRect( aOldPageFrmRect, pSh, aOldRectWithBorderAndShadow );
                pSh->InvalidateWindows( aOldRectWithBorderAndShadow );
            }
            rInvFlags |= 0x03;
            if ( aOldPageFrmRect.Height() != Frm().Height() )
                rInvFlags |= 0x04;
        }
        break;

        case RES_COL:
        {
            SwLayoutFrm *pB = FindBodyCont();
            ASSERT( pB, "Seite ohne Body." );
            pB->ChgColumns( *(const SwFmtCol*)pOld, *(const SwFmtCol*)pNew );
            rInvFlags |= 0x22;
        }
        break;

        case RES_HEADER:
            rInvFlags |= 0x08;
            break;

        case RES_FOOTER:
            rInvFlags |= 0x10;
            break;
        case RES_TEXTGRID:
            rInvFlags |= 0x60;
            break;

        case RES_PAGEDESC_FTNINFO:
            //Die derzeit einzig sichere Methode:
            ((SwRootFrm*)GetUpper())->SetSuperfluous();
            SetMaxFtnHeight( pDesc->GetFtnInfo().GetHeight() );
            if ( !GetMaxFtnHeight() )
                SetMaxFtnHeight( LONG_MAX );
            SetColMaxFtnHeight();
            //Hier wird die Seite ggf. zerstoert!
            ((SwRootFrm*)GetUpper())->RemoveFtns( 0, FALSE, TRUE );
            break;
        case RES_FRAMEDIR :
            CheckDirChange();
            break;

        default:
            bClear = FALSE;
    }
    if ( bClear )
    {
        if ( pOldSet || pNewSet )
        {
            if ( pOldSet )
                pOldSet->ClearItem( nWhich );
            if ( pNewSet )
                pNewSet->ClearItem( nWhich );
        }
        else
            SwLayoutFrm::Modify( pOld, pNew );
    }
}

/*************************************************************************
|*
|*				  SwPageFrm::GetInfo()
|*
|*	  Beschreibung		erfragt Informationen
|*	  Ersterstellung	JP 31.03.94
|*	  Letzte Aenderung	JP 31.03.94
|*
*************************************************************************/
    // erfrage vom Modify Informationen
BOOL SwPageFrm::GetInfo( SfxPoolItem & rInfo ) const
{
    if( RES_AUTOFMT_DOCNODE == rInfo.Which() )
    {
        // es gibt einen PageFrm also wird er benutzt
        return FALSE;
    }
    return TRUE;		// weiter suchen
}

/*************************************************************************
|*
|*	SwPageFrm::SetPageDesc()
|*
|*	Ersterstellung		MA 02. Nov. 94
|*	Letzte Aenderung	MA 02. Nov. 94
|*
|*************************************************************************/
void  SwPageFrm::SetPageDesc( SwPageDesc *pNew, SwFrmFmt *pFmt )
{
    pDesc = pNew;
    if ( pFmt )
        SetFrmFmt( pFmt );
}

/*************************************************************************
|*
|*	SwPageFrm::FindPageDesc()
|*
|*	Beschreibung		Der richtige PageDesc wird bestimmt:
|*		0.	Vom Dokument bei Fussnotenseiten und Endnotenseiten
|* 		1.	vom ersten BodyCntnt unterhalb der Seite.
|* 		2.	vom PageDesc der vorstehenden Seite.
|* 		3.	bei Leerseiten vom PageDesc der vorigen Seite.
|* 		3.1 vom PageDesc der folgenden Seite wenn es keinen Vorgaenger gibt.
|* 		4.	es ist der Default-PageDesc sonst.
|*		5.  Im BrowseMode ist der Pagedesc immer der vom ersten Absatz im
|* 			Dokument oder Standard (der 0-te) wenn der erste Absatz keinen
|* 			wuenscht.
|*	   (6.  Im HTML-Mode ist der Pagedesc immer die HTML-Seitenvorlage.)
|*	Ersterstellung		MA 15. Feb. 93
|*	Letzte Aenderung	MA 17. Jun. 99
|*
|*************************************************************************/
SwPageDesc *SwPageFrm::FindPageDesc()
{
    //0.
    if ( IsFtnPage() )
    {
        SwDoc *pDoc = GetFmt()->GetDoc();
        if ( IsEndNotePage() )
            return pDoc->GetEndNoteInfo().GetPageDesc( *pDoc );
        else
            return pDoc->GetFtnInfo().GetPageDesc( *pDoc );
    }

    //6.
    //if ( GetFmt()->GetDoc()->IsHTMLMode() )
    //	return GetFmt()->GetDoc()->GetPageDescFromPool( RES_POOLPAGE_HTML );

    SwPageDesc *pRet = 0;

    //5.
    if ( GetFmt()->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE) )
    {
        SwCntntFrm *pFrm = GetUpper()->ContainsCntnt();
        while ( !pFrm->IsInDocBody() )
            pFrm = pFrm->GetNextCntntFrm();
        SwFrm *pFlow = pFrm;
        if ( pFlow->IsInTab() )
            pFlow = pFlow->FindTabFrm();
        pRet = (SwPageDesc*)pFlow->GetAttrSet()->GetPageDesc().GetPageDesc();
        if ( !pRet )
            pRet = &GetFmt()->GetDoc()->_GetPageDesc( 0 );
        return pRet;
    }

    SwFrm *pFlow = FindFirstBodyCntnt();
    if ( pFlow && pFlow->IsInTab() )
        pFlow = pFlow->FindTabFrm();

    //1.
    if ( pFlow )
    {
        SwFlowFrm *pTmp = SwFlowFrm::CastFlowFrm( pFlow );
        if ( !pTmp->IsFollow() )
            pRet = (SwPageDesc*)pFlow->GetAttrSet()->GetPageDesc().GetPageDesc();
    }

    //3. und 3.1
    if ( !pRet && IsEmptyPage() )
        pRet = GetPrev() ? ((SwPageFrm*)GetPrev())->GetPageDesc()->GetFollow() :
               GetNext() ? ((SwPageFrm*)GetNext())->GetPageDesc() : 0;

    //2.
    if ( !pRet )
        pRet = GetPrev() ?
                    ((SwPageFrm*)GetPrev())->GetPageDesc()->GetFollow() : 0;

    //4.
    if ( !pRet )
        pRet = (SwPageDesc*)&(const_cast<const SwDoc *>(GetFmt()->GetDoc())
                              ->GetPageDesc( 0 ));


    ASSERT( pRet, "Kein Descriptor gefunden." );
    return pRet;
}
/*************************************************************************
|*
|*	SwPageFrm::AdjustRootSize()
|*
|*	Ersterstellung		MA 13. Aug. 93
|*	Letzte Aenderung	MA 25. Jun. 95
|*
|*************************************************************************/
//Wenn der RootFrm seine Groesse aendert muss benachrichtigt werden.
void AdjustSizeChgNotify( SwRootFrm *pRoot )
{
    const BOOL bOld = pRoot->IsSuperfluous();
    pRoot->bCheckSuperfluous = FALSE;
    ViewShell *pSh = pRoot->GetCurrShell();
    if ( pSh )
    {
        pSh->Imp()->NotifySizeChg( pRoot->Frm().SSize() );//Einmal fuer das Drawing.
        do
        {   pSh->SizeChgNotify();     //Einmal fuer jede Sicht.
            pSh = (ViewShell*)pSh->GetNext();
        } while ( pSh != pRoot->GetCurrShell() );
    }
    pRoot->bCheckSuperfluous = bOld;
}

void MA_FASTCALL lcl_AdjustRoot( SwFrm *pPage, long nOld )
{
    //Groesse der groessten Seite ermitteln.
    //nOld enthaelt den alten Wert wenn die Seite geschrumpft ist und
    //den aktuellen Wert wenn sie etwa ausgeschnitten wurde. Dadurch
    //kann abgebrochen werden, wenn eine Seite gefunden wird, deren Wert
    //dem alten entspricht.
    long nMax = pPage->Frm().Width();
    if ( nMax == nOld )
        nMax = 0;
    const SwFrm *pFrm = pPage->GetUpper()->Lower();
    while ( pFrm )
    {
        if ( pFrm != pPage )
        {
            const SwTwips nTmp = pFrm->Frm().Width();
            if ( nTmp == nOld )
            {
                nMax = 0;
                break;
            }
            else if ( nTmp > nMax )
                nMax = nTmp;
        }
        pFrm = pFrm->GetNext();
    }
    if ( nMax )
        pPage->GetUpper()->ChgSize( Size( nMax,
                                          pPage->GetUpper()->Frm().Height() ) );
}

inline void SetLastPage( SwPageFrm *pPage )
{
    ((SwRootFrm*)pPage->GetUpper())->pLastPage = pPage;
}

void SwPageFrm::AdjustRootSize( const SwPageChg eChgType, const SwRect *pOld )
{
    if ( !GetUpper() )
        return;

    const SwRect aOld( GetUpper()->Frm() );

    const SwTwips nVar = Frm().Height();
    SwTwips nFix = Frm().Width();
    SwTwips nDiff = 0;

    switch ( eChgType )
    {
        case CHG_NEWPAGE:
            {
                if( nFix > GetUpper()->Prt().Width() )
                    GetUpper()->ChgSize( Size(nFix,GetUpper()->Frm().Height()));
                nDiff = nVar;
                if ( GetPrev() && !((SwPageFrm*)GetPrev())->IsEmptyPage() )
                    nDiff += DOCUMENTBORDER/2;
                else if ( !IsEmptyPage() && GetNext() )
                    nDiff += DOCUMENTBORDER/2;
            }
            break;
        case CHG_CUTPAGE:
            {
                if ( nFix == GetUpper()->Prt().Width() )
                    ::lcl_AdjustRoot( this, nFix );
                nDiff = -nVar;
                if ( GetPrev() && !((SwPageFrm*)GetPrev())->IsEmptyPage() )
                    nDiff -= DOCUMENTBORDER/2;
                else if ( !IsEmptyPage() && GetNext() )
                    nDiff -= DOCUMENTBORDER/2;
                if ( IsEmptyPage() && GetNext() && GetPrev() )
                    nDiff = -nVar;
            }
            break;
        case CHG_CHGPAGE:
            {
                ASSERT( pOld, "ChgPage ohne OldValue nicht moeglich." );
                if ( pOld->Width() < nFix )
                {
                    if ( nFix > GetUpper()->Prt().Width() )
                        GetUpper()->ChgSize( Size( nFix,
                                                GetUpper()->Frm().Height() ) );
                }
                else if ( pOld->Width() > nFix )
                    ::lcl_AdjustRoot( this, pOld->Width() );
                nDiff = nVar - pOld->Height();
            }
            break;

        default:
            ASSERT( FALSE, "Neuer Typ fuer PageChg." );
    }

    if ( nDiff > 0 )
        GetUpper()->Grow( nDiff );
    else if ( nDiff < 0 )
        GetUpper()->Shrink( -nDiff );

    //Fix(8522): Calc auf die Root damit sich dir PrtArea sofort einstellt.
    //Anderfalls gibt es Probleme wenn mehrere Aenderungen innerhalb einer
    //Action laufen.
    GetUpper()->Calc();

    if ( aOld != GetUpper()->Frm() )
    {
        SwLayoutFrm *pUp = GetUpper();
        if ( eChgType == CHG_CUTPAGE )
        {
            //Seiten vorher kurz aushaengen, weil sonst falsch formatiert wuerde.
            SwFrm *pSibling = GetNext();
            if ( ((SwRootFrm*)pUp)->GetLastPage() == this )
                ::SetLastPage( (SwPageFrm*)GetPrev() );
            Remove();
            ::AdjustSizeChgNotify( (SwRootFrm*)pUp );
            InsertBefore( pUp, pSibling );
        }
        else
            ::AdjustSizeChgNotify( (SwRootFrm*)pUp );
    }
}

/*************************************************************************
|*
|*	SwPageFrm::Cut()
|*
|*	Ersterstellung		MA 23. Feb. 94
|*	Letzte Aenderung	MA 22. Jun. 95
|*
|*************************************************************************/
void SwPageFrm::Cut()
{
    AdjustRootSize( CHG_CUTPAGE, 0 );

    ViewShell *pSh = GetShell();
    if ( !IsEmptyPage() )
    {
        if ( GetNext() )
            GetNext()->InvalidatePos();

        //Flys deren Anker auf anderen Seiten stehen umhaengen.
        //DrawObjecte spielen hier keine Rolle.
        if ( GetSortedObjs() )
        {
            for ( int i = 0; GetSortedObjs() &&
                             (USHORT)i < GetSortedObjs()->Count(); ++i )
            {
                // --> OD 2004-06-29 #i28701#
                SwAnchoredObject* pAnchoredObj = (*GetSortedObjs())[i];

                if ( pAnchoredObj->ISA(SwFlyAtCntFrm) )
                {
                    SwFlyFrm* pFly = static_cast<SwFlyAtCntFrm*>(pAnchoredObj);
                    SwPageFrm *pAnchPage = pFly->GetAnchorFrm() ?
                                pFly->AnchorFrm()->FindPageFrm() : 0;
                    if ( pAnchPage && (pAnchPage != this) )
                    {
                        MoveFly( pFly, pAnchPage );
                        --i;
                        pFly->InvalidateSize();
                        pFly->_InvalidatePos();
                    }
                }
                // <--
            }
        }
        //Window aufraeumen
        if ( pSh && pSh->GetWin() )
            pSh->InvalidateWindows( Frm() );
    }

    // die Seitennummer der Root runterzaehlen.
    ((SwRootFrm*)GetUpper())->DecrPhyPageNums();
    SwPageFrm *pPg = (SwPageFrm*)GetNext();
    if ( pPg )
    {
        while ( pPg )
        {
            pPg->DecrPhyPageNum();	//inline --nPhyPageNum
            pPg = (SwPageFrm*)pPg->GetNext();
        }
    }
    else
        ::SetLastPage( (SwPageFrm*)GetPrev() );

    // Alle Verbindungen kappen.
    Remove();
    if ( pSh )
        pSh->SetFirstVisPageInvalid();
}

/*************************************************************************
|*
|*	SwPageFrm::Paste()
|*
|*	Ersterstellung		MA 23. Feb. 94
|*	Letzte Aenderung	MA 07. Dec. 94
|*
|*************************************************************************/
void SwPageFrm::Paste( SwFrm* pParent, SwFrm* pSibling )
{
    ASSERT( pParent->IsRootFrm(), "Parent ist keine Root." );
    ASSERT( pParent, "Kein Parent fuer Paste." );
    ASSERT( pParent != this, "Bin selbst der Parent." );
    ASSERT( pSibling != this, "Bin mein eigener Nachbar." );
    ASSERT( !GetPrev() && !GetNext() && !GetUpper(),
            "Bin noch irgendwo angemeldet." );

    //In den Baum einhaengen.
    InsertBefore( (SwLayoutFrm*)pParent, pSibling );

    // die Seitennummer am Root hochzaehlen.
    ((SwRootFrm*)GetUpper())->IncrPhyPageNums();
    if( GetPrev() )
        SetPhyPageNum( ((SwPageFrm*)GetPrev())->GetPhyPageNum() + 1 );
    else
        SetPhyPageNum( 1 );
    SwPageFrm *pPg = (SwPageFrm*)GetNext();
    if ( pPg )
    {
        while ( pPg )
        {
            pPg->IncrPhyPageNum();	//inline ++nPhyPageNum
            pPg->_InvalidatePos();
            pPg->InvalidateLayout();
            pPg = (SwPageFrm*)pPg->GetNext();
        }
    }
    else
        ::SetLastPage( this );

    if( Frm().Width() != pParent->Prt().Width() )
        _InvalidateSize();
    InvalidatePos();

    AdjustRootSize( CHG_NEWPAGE, 0 );

    ViewShell *pSh = GetShell();
    if ( pSh )
        pSh->SetFirstVisPageInvalid();
}

/*************************************************************************
|*
|*	SwPageFrm::PrepareRegisterChg()
|*
|*	Ersterstellung		AMA 22. Jul. 96
|*	Letzte Aenderung	AMA 22. Jul. 96
|*
|*************************************************************************/
void lcl_PrepFlyInCntRegister( SwCntntFrm *pFrm )
{
    pFrm->Prepare( PREP_REGISTER );
    if( pFrm->GetDrawObjs() )
    {
        for( USHORT i = 0; i < pFrm->GetDrawObjs()->Count(); ++i )
        {
            // --> OD 2004-06-29 #i28701#
            SwAnchoredObject* pAnchoredObj = (*pFrm->GetDrawObjs())[i];
            if ( pAnchoredObj->ISA(SwFlyInCntFrm) )
            {
                SwFlyFrm* pFly = static_cast<SwFlyInCntFrm*>(pAnchoredObj);
                SwCntntFrm *pCnt = pFly->ContainsCntnt();
                while ( pCnt )
                {
                    lcl_PrepFlyInCntRegister( pCnt );
                    pCnt = pCnt->GetNextCntntFrm();
                }
            }
            // <--
        }
    }
}

void SwPageFrm::PrepareRegisterChg()
{
    SwCntntFrm *pFrm = FindFirstBodyCntnt();
    while( pFrm )
    {
        lcl_PrepFlyInCntRegister( pFrm );
        pFrm = pFrm->GetNextCntntFrm();
        if( !IsAnLower( pFrm ) )
            break;
    }
    if( GetSortedObjs() )
    {
        for( USHORT i = 0; i < GetSortedObjs()->Count(); ++i )
        {
            // --> OD 2004-06-29 #i28701#
            SwAnchoredObject* pAnchoredObj = (*GetSortedObjs())[i];
            if ( pAnchoredObj->ISA(SwFlyFrm) )
            {
                SwFlyFrm *pFly = static_cast<SwFlyFrm*>(pAnchoredObj);
                pFrm = pFly->ContainsCntnt();
                while ( pFrm )
                {
                    ::lcl_PrepFlyInCntRegister( pFrm );
                    pFrm = pFrm->GetNextCntntFrm();
                }
            }
        }
    }
}

/*************************************************************************
|*
|*	SwFrm::CheckPageDescs()
|*
|*	Beschreibung		Prueft alle Seiten ab der uebergebenen, daraufhin,
|* 		ob sie das richtige FrmFmt verwenden. Wenn 'falsche' Seiten
|*		aufgespuehrt werden, so wird versucht die Situation moeglichst
|* 		einfache zu bereinigen.
|*
|*	Ersterstellung		MA 10. Feb. 93
|*	Letzte Aenderung	MA 18. Apr. 96
|*
|*************************************************************************/
void SwFrm::CheckPageDescs( SwPageFrm *pStart, BOOL bNotifyFields )
{
    ASSERT( pStart, "Keine Startpage." );

    ViewShell *pSh	 = pStart->GetShell();
    SwViewImp *pImp  = pSh ? pSh->Imp() : 0;

    if ( pImp && pImp->IsAction() && !pImp->GetLayAction().IsCheckPages() )
    {
        pImp->GetLayAction().SetCheckPageNum( pStart->GetPhyPageNum() );
        return;
    }

    //Fuer das Aktualisieren der Seitennummern-Felder gibt nDocPos
    //die Seitenposition an, _ab_ der invalidiert werden soll.
    SwTwips nDocPos  = LONG_MAX;

    SwRootFrm *pRoot = (SwRootFrm*)pStart->GetUpper();
    SwDoc* pDoc		 = pStart->GetFmt()->GetDoc();
    const BOOL bFtns = 0 != pDoc->GetFtnIdxs().Count();

    SwPageFrm *pPage = pStart;
    if( pPage->GetPrev() && ((SwPageFrm*)pPage->GetPrev())->IsEmptyPage() )
        pPage = (SwPageFrm*)pPage->GetPrev();
    while ( pPage )
    {
        //gewuenschten PageDesc und FrmFmt festellen.
        SwPageDesc *pDesc = pPage->FindPageDesc();
        BOOL bCheckEmpty = pPage->IsEmptyPage();
        BOOL bActOdd = pPage->OnRightPage();
        BOOL bOdd = pPage->WannaRightPage();
        SwFrmFmt *pFmtWish = bOdd ? pDesc->GetRightFmt()
                                  : pDesc->GetLeftFmt();

        if ( bActOdd != bOdd ||
             pDesc != pPage->GetPageDesc() ||		//falscher Desc
             ( pFmtWish != pPage->GetFmt()  &&      //falsches Format und
               ( !pPage->IsEmptyPage() || pFmtWish ) //nicht Leerseite
             )
           )
        {
            //Wenn wir schon ein Seite veraendern muessen kann das eine
            //Weile dauern, deshalb hier den WaitCrsr pruefen.
            if( pImp )
                pImp->CheckWaitCrsr();

            //Ab hier muessen die Felder invalidiert werden!
            if ( nDocPos == LONG_MAX )
                nDocPos = pPage->GetPrev() ?
                            pPage->GetPrev()->Frm().Top() :	pPage->Frm().Top();

            //Faelle:
            //1. Wir haben eine EmptyPage und wollen eine "Normalseite".
            //		->EmptyPage wegwerfen und weiter mit der naechsten.
            //2. Wir haben eine EmptyPage und wollen eine EmptyPage mit
            //	 anderem Descriptor.
            //		->Descriptor austauschen.
            //3. Wir haben eine "Normalseite" und wollen eine EmptyPage.
            //		->Emptypage einfuegen, nicht aber wenn die Vorseite
            //							   bereits eine EmptyPage ist -> 6.
            //4. Wir haben eine "Normalseite" und wollen eine "Normalseite"
            //	 mit anderem Descriptor
            //		->Descriptor und Format austauschen
            //5. Wir haben eine "Normalseite" und wollen eine "Normalseite"
            //	 mit anderem Format
            //		->Format austauschen.
            //6. Wir haben kein Wunschformat erhalten, also nehmen wir das
            //	 'andere' Format (rechts/links) des PageDesc.

            if ( pPage->IsEmptyPage() && ( pFmtWish ||			//1.
                 ( !bOdd && !pPage->GetPrev() ) ) )
            {
                SwPageFrm *pTmp = (SwPageFrm*)pPage->GetNext();
                pPage->Cut();
                delete pPage;
                if ( pStart == pPage )
                    pStart = pTmp;
                pPage = pTmp;
                continue;
            }
            else if ( pPage->IsEmptyPage() && !pFmtWish &&	//2.
                      pDesc != pPage->GetPageDesc() )
            {
                pPage->SetPageDesc( pDesc, 0 );
            }
            else if ( !pPage->IsEmptyPage() &&      //3.
                      bActOdd != bOdd &&
                      ( ( !pPage->GetPrev() && !bOdd ) ||
                        ( pPage->GetPrev() &&
                          !((SwPageFrm*)pPage->GetPrev())->IsEmptyPage() )
                      )
                    )
            {
                if ( pPage->GetPrev() )
                    pDesc = ((SwPageFrm*)pPage->GetPrev())->GetPageDesc();
                SwPageFrm *pTmp = new SwPageFrm( pDoc->GetEmptyPageFmt(),pDesc);
                pTmp->Paste( pRoot, pPage );
                pTmp->PreparePage( FALSE );
                pPage = pTmp;
            }
            else if ( pPage->GetPageDesc() != pDesc )			//4.
            {
                SwPageDesc *pOld = pPage->GetPageDesc();
                pPage->SetPageDesc( pDesc, pFmtWish );
                if ( bFtns )
                {
                    //Wenn sich bestimmte Werte der FtnInfo veraendert haben
                    //muss etwas passieren. Wir versuchen den Schaden zu
                    //begrenzen.
                    //Wenn die Seiten keinen FtnCont hat, ist zwar theoretisches
                    //ein Problem denkbar, aber das ignorieren wir mit aller Kraft.
                    //Bei Aenderungen hoffen wir mal, dass eine Invalidierung
                    //ausreicht, denn alles andere wuerde viel Kraft kosten.
                    SwFtnContFrm *pCont = pPage->FindFtnCont();
                    if ( pCont && !(pOld->GetFtnInfo() == pDesc->GetFtnInfo()) )
                        pCont->_InvalidateAll();
                }
            }
            else if ( pFmtWish && pPage->GetFmt() != pFmtWish )			//5.
            {
                pPage->SetFrmFmt( pFmtWish );
            }
            else if ( !pFmtWish )										//6.
            {
                //Format mit verdrehter Logic besorgen.
                pFmtWish = bOdd ? pDesc->GetLeftFmt() : pDesc->GetRightFmt();
                if ( pPage->GetFmt() != pFmtWish )
                    pPage->SetFrmFmt( pFmtWish );
            }
#ifndef PRODUCT
            else
            {
                ASSERT( FALSE, "CheckPageDescs, missing solution" );
            }
#endif
        }
        if ( bCheckEmpty )
        {
            //Es kann noch sein, dass die Leerseite schlicht  ueberflussig ist.
            //Obiger Algorithmus kann dies leider nicht feststellen.
            //Eigentlich muesste die Leerseite einfach praeventiv entfernt
            //werden; sie wuerde ja ggf. wieder eingefuegt.
            //Die EmptyPage ist genau dann ueberfluessig, wenn die Folgeseite
            //auch ohne sie auskommt. Dazu muessen wir uns die Verhaeltnisse
            //genauer ansehen. Wir bestimmen den PageDesc und die virtuelle
            //Seitennummer manuell.
            SwPageFrm *pPg = (SwPageFrm*)pPage->GetNext();
            if( !pPg || pPage->OnRightPage() == pPg->WannaRightPage() )
            {
                //Die Folgeseite hat kein Problem ein FrmFmt zu finden oder keinen
                //Nachfolger, also ist die Leerseite ueberfluessig.
                SwPageFrm *pTmp = (SwPageFrm*)pPage->GetNext();
                pPage->Cut();
                delete pPage;
                if ( pStart == pPage )
                    pStart = pTmp;
                pPage = pTmp;
                continue;
            }
        }
        pPage = (SwPageFrm*)pPage->GetNext();
    }

    pRoot->SetAssertFlyPages();
    pRoot->AssertPageFlys( pStart );

    if ( bNotifyFields && (!pImp || !pImp->IsUpdateExpFlds()) )
    {
        SwDocPosUpdate aMsgHnt( nDocPos );
        pDoc->UpdatePageFlds( &aMsgHnt );
    }

#ifndef PRODUCT
    //Ein paar Pruefungen muessen schon erlaubt sein.

    //1. Keine zwei EmptyPages hintereinander.
    //2. Alle PageDescs richtig?
    BOOL bEmpty = FALSE;
    SwPageFrm *pPg = pStart;
    while ( pPg )
    {
        if ( pPg->IsEmptyPage() )
        {
            if ( bEmpty )
            {
                ASSERT( FALSE, "Doppelte Leerseiten." );
                break;	//Einmal reicht.
            }
            bEmpty = TRUE;
        }
        else
            bEmpty = FALSE;

//MA 21. Jun. 95: Kann zu testzwecken 'rein, ist aber bei zyklen durchaus
//moeglich: Ein paar Seiten, auf der ersten 'erste Seite' anwenden,
//rechte als folge der ersten, linke als folge der rechten, rechte als
//folge der linken.
//		ASSERT( pPg->GetPageDesc() == pPg->FindPageDesc(),
//				"Seite mit falschem Descriptor." );

        pPg = (SwPageFrm*)pPg->GetNext();
    }
#endif
}

/*************************************************************************
|*
|*	SwFrm::InsertPage()
|*
|*	Beschreibung
|*	Ersterstellung		MA 10. Feb. 93
|*	Letzte Aenderung	MA 27. Jul. 93
|*
|*************************************************************************/
SwPageFrm *SwFrm::InsertPage( SwPageFrm *pPrevPage, BOOL bFtn )
{
    SwRootFrm *pRoot = (SwRootFrm*)pPrevPage->GetUpper();
    SwPageFrm *pSibling = (SwPageFrm*)pRoot->GetLower();
    SwPageDesc *pDesc = pSibling->GetPageDesc();

    pSibling = (SwPageFrm*)pPrevPage->GetNext();
        //Rechte (ungerade) oder linke (gerade) Seite einfuegen?
    BOOL bNextOdd = !pPrevPage->OnRightPage();
    BOOL bWishedOdd = bNextOdd;

    //Welcher PageDesc gilt?
    //Bei CntntFrm der aus dem Format wenn einer angegeben ist,
    //der Follow vom bereits in der PrevPage gueltigen sonst.
    pDesc = 0;
    if ( IsFlowFrm() && !SwFlowFrm::CastFlowFrm( this )->IsFollow() )
    {	SwFmtPageDesc &rDesc = (SwFmtPageDesc&)GetAttrSet()->GetPageDesc();
        pDesc = rDesc.GetPageDesc();
        if ( rDesc.GetNumOffset() )
        {
            bWishedOdd = rDesc.GetNumOffset() % 2 ? TRUE : FALSE;
            //Die Gelegenheit nutzen wir um das Flag an der Root zu pflegen.
            pRoot->SetVirtPageNum( TRUE );
        }
    }
    if ( !pDesc )
        pDesc = pPrevPage->GetPageDesc()->GetFollow();

    ASSERT( pDesc, "Missing PageDesc" );
    if( !(bWishedOdd ? pDesc->GetRightFmt() : pDesc->GetLeftFmt()) )
        bWishedOdd = !bWishedOdd;

    SwDoc *pDoc = pPrevPage->GetFmt()->GetDoc();
    SwFrmFmt *pFmt;
    BOOL bCheckPages = FALSE;
    //Wenn ich kein FrmFmt fuer die Seite gefunden habe, muss ich eben eine
    //Leerseite einfuegen.
    if( bWishedOdd != bNextOdd )
    {	pFmt = pDoc->GetEmptyPageFmt();
        SwPageDesc *pTmpDesc = pPrevPage->GetPageDesc();
        SwPageFrm *pPage = new SwPageFrm( pFmt, pTmpDesc );
        pPage->Paste( pRoot, pSibling );
        pPage->PreparePage( bFtn );
        //Wenn der Sibling keinen Bodytext enthaelt kann ich ihn vernichten
        //Es sei denn, es ist eine Fussnotenseite
        if ( pSibling && !pSibling->IsFtnPage() &&
             !pSibling->FindFirstBodyCntnt() )
        {
            SwPageFrm *pDel = pSibling;
            pSibling = (SwPageFrm*)pSibling->GetNext();
            if ( pDoc->GetFtnIdxs().Count() )
                pRoot->RemoveFtns( pDel, TRUE );
            pDel->Cut();
            delete pDel;
        }
        else
            bCheckPages = TRUE;
    }
    pFmt = bWishedOdd ? pDesc->GetRightFmt() : pDesc->GetLeftFmt();
    ASSERT( pFmt, "Descriptor without format." );
    SwPageFrm *pPage = new SwPageFrm( pFmt, pDesc );
    pPage->Paste( pRoot, pSibling );
    pPage->PreparePage( bFtn );
    //Wenn der Sibling keinen Bodytext enthaelt kann ich ihn vernichten
    //Es sei denn es ist eine Fussnotenseite.
    if ( pSibling && !pSibling->IsFtnPage() &&
         !pSibling->FindFirstBodyCntnt() )
    {
        SwPageFrm *pDel = pSibling;
        pSibling = (SwPageFrm*)pSibling->GetNext();
        if ( pDoc->GetFtnIdxs().Count() )
            pRoot->RemoveFtns( pDel, TRUE );
        pDel->Cut();
        delete pDel;
    }
    else
        bCheckPages = TRUE;

    if ( pSibling )
    {
        if ( bCheckPages )
        {
            CheckPageDescs( pSibling, FALSE );
            ViewShell *pSh = GetShell();
            SwViewImp *pImp = pSh ? pSh->Imp() : 0;
            if ( pImp && pImp->IsAction() && !pImp->GetLayAction().IsCheckPages() )
            {
                const USHORT nNum = pImp->GetLayAction().GetCheckPageNum();
                if ( nNum == pPrevPage->GetPhyPageNum() + 1 )
                    pImp->GetLayAction().SetCheckPageNumDirect(
                                                    pSibling->GetPhyPageNum() );
                return pPage;
            }
        }
        else
            pRoot->AssertPageFlys( pSibling );
    }

    //Fuer das Aktualisieren der Seitennummern-Felder gibt nDocPos
    //die Seitenposition an, _ab_ der invalidiert werden soll.
    ViewShell *pSh = GetShell();
    if ( !pSh || !pSh->Imp()->IsUpdateExpFlds() )
    {
        SwDocPosUpdate aMsgHnt( pPrevPage->Frm().Top() );
        pDoc->UpdatePageFlds( &aMsgHnt );
    }
    return pPage;
}

/*************************************************************************
|*
|*	SwRootFrm::GrowFrm()
|*
|*	Ersterstellung		MA 30. Jul. 92
|*	Letzte Aenderung	MA 05. May. 94
|*
|*************************************************************************/

SwTwips SwRootFrm::GrowFrm( SwTwips nDist, BOOL bTst, BOOL )
{
    if ( !bTst )
        Frm().SSize().Height() += nDist;
    return nDist;
}
/*************************************************************************
|*
|*	SwRootFrm::ShrinkFrm()
|*
|*	Ersterstellung		MA 30. Jul. 92
|*	Letzte Aenderung	MA 05. May. 94
|*
|*************************************************************************/
SwTwips SwRootFrm::ShrinkFrm( SwTwips nDist, BOOL bTst, BOOL )
{
    ASSERT( nDist >= 0, "nDist < 0." );
    ASSERT( nDist <= Frm().Height(), "nDist > als aktuelle Groesse." );

    if ( !bTst )
        Frm().SSize().Height() -= nDist;
    return nDist;
}

/*************************************************************************
|*
|*	SwRootFrm::RemoveSuperfluous()
|*
|*	Beschreibung:		Entfernung von ueberfluessigen Seiten.
|*			Arbeitet nur wenn das Flag bCheckSuperfluous gesetzt ist.
|*			Definition: Eine Seite ist genau dann leer, wenn der
|*			Body-Textbereich keinen CntntFrm enthaelt, aber nicht, wenn noch
|* 			mindestens ein Fly an der Seite klebt.
|* 			Die Seite ist auch dann nicht leer, wenn sie noch eine
|* 			Fussnote enthaelt.
|*			Es muss zweimal angesetzt werden um leeren Seiten aufzuspueren:
|* 				- einmal fuer die Endnotenseiten.
|* 				- und einmal fuer die Seiten des Bodytextes.
|*
|*	Ersterstellung		MA 20. May. 92
|*	Letzte Aenderung	MA 10. Jan. 95
|*
|*************************************************************************/
void SwRootFrm::RemoveSuperfluous()
{
    if ( !IsSuperfluous() )
        return;
    bCheckSuperfluous = FALSE;

    SwPageFrm *pPage = GetLastPage();
    long nDocPos = LONG_MAX;

    //Jetzt wird fuer die jeweils letzte Seite geprueft ob sie leer ist
    //bei der ersten nicht leeren Seite wird die Schleife beendet.
    do
    {
        bool bExistEssentialObjs = ( 0 != pPage->GetSortedObjs() );
        if ( bExistEssentialObjs )
        {
            //Nur weil die Seite Flys hat sind wir noch lange nicht fertig,
            //denn wenn alle Flys an generischem Inhalt haengen, so ist sie
            //trotzdem ueberfluessig (Ueberpruefung auf DocBody sollte reichen).
            // OD 19.06.2003 #108784# - consider that drawing objects in
            // header/footer are supported now.
            bool bOnlySuperfluosObjs = true;
            SwSortedObjs &rObjs = *pPage->GetSortedObjs();
            for ( USHORT i = 0; bOnlySuperfluosObjs && i < rObjs.Count(); ++i )
            {
                // --> OD 2004-06-29 #i28701#
                SwAnchoredObject* pAnchoredObj = rObjs[i];
                // OD 2004-01-19 #110582# - do not consider hidden objects
                if ( pPage->GetFmt()->GetDoc()->IsVisibleLayerId(
                                    pAnchoredObj->GetDrawObj()->GetLayer() ) &&
                     !pAnchoredObj->GetAnchorFrm()->FindFooterOrHeader() )
                {
                    bOnlySuperfluosObjs = false;
                }
                // <--
            }
            bExistEssentialObjs = !bOnlySuperfluosObjs;
        }

        // OD 19.06.2003 #108784# - optimization: check first, if essential objects
        // exists.
        const SwLayoutFrm* pBody = 0;
        if ( bExistEssentialObjs ||
             pPage->FindFtnCont() ||
             ( 0 != ( pBody = pPage->FindBodyCont() ) &&
                ( pBody->ContainsCntnt() ||
                    // --> FME 2005-05-18 #i47580#
                    // Do not delete page if there's an empty tabframe
                    // left. I think it might be correct to use ContainsAny()
                    // instead of ContainsCntnt() to cover the empty-table-case,
                    // but I'm not fully sure, since ContainsAny() also returns
                    // SectionFrames. Therefore I prefer to do it the safe way:
                  ( pBody->Lower() && pBody->Lower()->IsTabFrm() ) ) ) )
                    // <--
        {
            if ( pPage->IsFtnPage() )
            {
                while ( pPage->IsFtnPage() )
                {
                    pPage = (SwPageFrm*)pPage->GetPrev();
                    ASSERT( pPage, "Nur noch Endnotenseiten uebrig." );
                }
                continue;
            }
            else
                pPage = 0;
        }

        if ( pPage )
        {
            SwPageFrm *pEmpty = pPage;
            pPage = (SwPageFrm*)pPage->GetPrev();
            if ( GetFmt()->GetDoc()->GetFtnIdxs().Count() )
                RemoveFtns( pEmpty, TRUE );
            pEmpty->Cut();
            delete pEmpty;
            nDocPos = pPage ? pPage->Frm().Top() : 0;
        }
    } while ( pPage );

    ViewShell *pSh = GetShell();
    if ( nDocPos != LONG_MAX &&
         (!pSh || !pSh->Imp()->IsUpdateExpFlds()) )
    {
        SwDocPosUpdate aMsgHnt( nDocPos );
        GetFmt()->GetDoc()->UpdatePageFlds( &aMsgHnt );
    }
}

/*************************************************************************
|*
|*	SwRootFrm::AssertFlyPages()
|*
|*	Beschreibung		Stellt sicher, dass genuegend Seiten vorhanden
|* 		sind, damit alle Seitengebundenen Rahmen und DrawObject
|*		untergebracht sind.
|*
|*	Ersterstellung		MA 27. Jul. 93
|*	Letzte Aenderung	MA 24. Apr. 97
|*
|*************************************************************************/
void SwRootFrm::AssertFlyPages()
{
    if ( !IsAssertFlyPages() )
        return;
    bAssertFlyPages = FALSE;

    SwDoc *pDoc = GetFmt()->GetDoc();
    const SwSpzFrmFmts *pTbl = pDoc->GetSpzFrmFmts();

    //Auf welche Seite will der 'letzte' Fly?
    USHORT nMaxPg = 0;
    USHORT i;

    for ( i = 0; i < pTbl->Count(); ++i )
    {
        const SwFmtAnchor &rAnch = (*pTbl)[i]->GetAnchor();
        if ( !rAnch.GetCntntAnchor() && nMaxPg < rAnch.GetPageNum() )
            nMaxPg = rAnch.GetPageNum();
    }
    //Wieviele Seiten haben wir derzeit?
    SwPageFrm *pPage = (SwPageFrm*)Lower();
    while ( pPage && pPage->GetNext() &&
            !((SwPageFrm*)pPage->GetNext())->IsFtnPage() )
    {
        pPage = (SwPageFrm*)pPage->GetNext();
    }

    if ( nMaxPg > pPage->GetPhyPageNum() )
    {
        //Die Seiten werden ausgehend von der letzten Seite konsequent
        //nach den Regeln der PageDescs weitergefuehrt.
        BOOL bOdd = pPage->GetPhyPageNum() % 2 ? TRUE : FALSE;
        SwPageDesc *pDesc = pPage->GetPageDesc();
        SwFrm *pSibling = pPage->GetNext();
        for ( i = pPage->GetPhyPageNum(); i < nMaxPg; ++i  )
        {
            if ( !(bOdd ? pDesc->GetRightFmt() : pDesc->GetLeftFmt()) )
            {
                //Leerseite einfuegen, die Flys werden aber erst von
                //der naechsten Seite aufgenommen!
                pPage = new SwPageFrm( pDoc->GetEmptyPageFmt(), pDesc );
                pPage->Paste( this, pSibling );
                pPage->PreparePage( FALSE );
                bOdd = bOdd ? FALSE : TRUE;
                ++i;
            }
            pPage = new
                    SwPageFrm( (bOdd ? pDesc->GetRightFmt() :
                                       pDesc->GetLeftFmt()), pDesc );
            pPage->Paste( this, pSibling );
            pPage->PreparePage( FALSE );
            bOdd = bOdd ? FALSE : TRUE;
            pDesc = pDesc->GetFollow();
        }
        //Jetzt koennen die Endnotenseiten natuerlich wieder krumm sein;
        //in diesem Fall werden sie vernichtet.
        if ( pDoc->GetFtnIdxs().Count() )
        {
            pPage = (SwPageFrm*)Lower();
            while ( pPage && !pPage->IsFtnPage() )
                pPage = (SwPageFrm*)pPage->GetNext();

            if ( pPage )
            {
                SwPageDesc *pTmpDesc = pPage->FindPageDesc();
                bOdd = pPage->OnRightPage();
                if ( pPage->GetFmt() !=
                     (bOdd ? pTmpDesc->GetRightFmt() : pTmpDesc->GetLeftFmt()) )
                    RemoveFtns( pPage, FALSE, TRUE );
            }
        }
    }
}

/*************************************************************************
|*
|*	SwRootFrm::AssertPageFlys()
|*
|*	Beschreibung		Stellt sicher, dass ab der uebergebenen Seite
|* 		auf allen Seiten die Seitengebunden Objecte auf der richtigen
|* 		Seite (Seitennummer stehen).
|*
|*	Ersterstellung		MA 02. Nov. 94
|*	Letzte Aenderung	MA 10. Aug. 95
|*
|*************************************************************************/
void SwRootFrm::AssertPageFlys( SwPageFrm *pPage )
{
    while ( pPage )
    {
        if ( pPage->GetSortedObjs() )
        {
            pPage->GetSortedObjs();
            for ( int i = 0;
                  pPage->GetSortedObjs() && USHORT(i) < pPage->GetSortedObjs()->Count();
                  ++i)
            {
                // --> OD 2004-06-29 #i28701#
                SwFrmFmt& rFmt = (*pPage->GetSortedObjs())[i]->GetFrmFmt();
                const SwFmtAnchor &rAnch = rFmt.GetAnchor();
                const USHORT nPg = rAnch.GetPageNum();
                if ( rAnch.GetAnchorId() == FLY_PAGE &&
                     nPg != pPage->GetPhyPageNum() )
                {
                    //Das er auf der falschen Seite steht muss noch nichts
                    //heissen, wenn er eigentlich auf der Vorseite
                    //stehen will und diese eine EmptyPage ist.
                    if( nPg && !(pPage->GetPhyPageNum()-1 == nPg &&
                        ((SwPageFrm*)pPage->GetPrev())->IsEmptyPage()) )
                    {
                        //Umhaengen kann er sich selbst, indem wir ihm
                        //einfach ein Modify mit seinem AnkerAttr schicken.
#ifdef PRODUCT
                        rFmt.SwModify::Modify( 0, (SwFmtAnchor*)&rAnch );
#else
                        const sal_uInt32 nCnt = pPage->GetSortedObjs()->Count();
                        rFmt.SwModify::Modify( 0, (SwFmtAnchor*)&rAnch );
                        ASSERT( !pPage->GetSortedObjs() ||
                                nCnt != pPage->GetSortedObjs()->Count(),
                                "Kann das Obj nicht umhaengen." );
#endif
                        --i;
                    }
                }
            }
        }
        pPage = (SwPageFrm*)pPage->GetNext();
    }
}

/*************************************************************************
|*
|*	SwRootFrm::ChgSize()
|*
|*	Ersterstellung		MA 24. Jul. 92
|*	Letzte Aenderung	MA 13. Aug. 93
|*
|*************************************************************************/
Size SwRootFrm::ChgSize( const Size& aNewSize )
{
    Frm().SSize() = aNewSize;
    _InvalidatePrt();
    bFixSize = FALSE;
    return Frm().SSize();
}

/*************************************************************************
|*
|*	SwRootFrm::MakeAll()
|*
|*	Ersterstellung		MA 17. Nov. 92
|*	Letzte Aenderung	MA 19. Apr. 93
|*
|*************************************************************************/
void SwRootFrm::MakeAll()
{
    if ( !bValidPos )
    {	bValidPos = TRUE;
        aFrm.Pos().X() = aFrm.Pos().Y() = DOCUMENTBORDER;
    }
    if ( !bValidPrtArea )
    {	bValidPrtArea = TRUE;
        aPrt.Pos().X() = aPrt.Pos().Y() = 0;
        aPrt.SSize( aFrm.SSize() );
    }
    if ( !bValidSize )
        //SSize wird von den Seiten (Cut/Paste) eingestellt.
        bValidSize = TRUE;
}

/*************************************************************************
|*
|*	SwRootFrm::ImplInvalidateBrowseWidth()
|*
|*	Ersterstellung		MA 08. Jun. 96
|*	Letzte Aenderung	MA 08. Jun. 96
|*
|*************************************************************************/
void SwRootFrm::ImplInvalidateBrowseWidth()
{
    bBrowseWidthValid = FALSE;
    SwFrm *pPg = Lower();
    while ( pPg )
    {
        pPg->InvalidateSize();
        pPg = pPg->GetNext();
    }
}

/*************************************************************************
|*
|*	SwRootFrm::ImplCalcBrowseWidth()
|*
|*	Ersterstellung		MA 07. Jun. 96
|*	Letzte Aenderung	MA 13. Jun. 96
|*
|*************************************************************************/
void SwRootFrm::ImplCalcBrowseWidth()
{
    ASSERT( GetFmt()->getIDocumentSettingAccess()->get(IDocumentSettingAccess::BROWSE_MODE),
            "CalcBrowseWidth and not in BrowseView" );

    //Die (minimale) Breite wird von Rahmen, Tabellen und Zeichenobjekten
    //bestimmt. Die Breite wird nicht anhand ihrer aktuellen Groessen bestimmt,
    //sondern anhand der Attribute. Es interessiert also nicht wie breit sie
    //sind, sondern wie breit sie sein wollen.
    //Rahmen und Zeichenobjekte innerhalb ander Objekte (Rahmen, Tabellen)
    //Zaehlen nicht.
    //Seitenraender und Spalten werden hier nicht beruecksichtigt.

    SwFrm *pFrm = ContainsCntnt();
    while ( pFrm && !pFrm->IsInDocBody() )
        pFrm = ((SwCntntFrm*)pFrm)->GetNextCntntFrm();
    if ( !pFrm )
        return;

    bBrowseWidthValid = TRUE;
    ViewShell *pSh = GetShell();
    nBrowseWidth = pSh
                    ? MINLAY + 2 * pSh->GetOut()->
                                PixelToLogic( pSh->GetBrowseBorder() ).Width()
                    : 5000;
    do
    {
        if ( pFrm->IsInTab() )
            pFrm = pFrm->FindTabFrm();

        if ( pFrm->IsTabFrm() &&
             !((SwLayoutFrm*)pFrm)->GetFmt()->GetFrmSize().GetWidthPercent() )
        {
            SwBorderAttrAccess aAccess( SwFrm::GetCache(), pFrm );
            const SwBorderAttrs &rAttrs = *aAccess.Get();
            const SwFmtHoriOrient &rHori = rAttrs.GetAttrSet().GetHoriOrient();
            long nWidth = rAttrs.GetSize().Width();
            if ( nWidth < USHRT_MAX-2000 && //-2000, weil bei Randeinstellung per
                                            //Zuppeln das USHRT_MAX verlorengeht!
                 text::HoriOrientation::FULL != rHori.GetHoriOrient() )
            {
                const SwHTMLTableLayout *pLayoutInfo =
                    ((const SwTabFrm *)pFrm)->GetTable()
                                            ->GetHTMLTableLayout();
                if ( pLayoutInfo )
                    nWidth = Min( nWidth, pLayoutInfo->GetBrowseWidthMin() );

                switch ( rHori.GetHoriOrient() )
                {
                    case text::HoriOrientation::NONE:
                        // OD 23.01.2003 #106895# - add 1st param to <SwBorderAttrs::CalcRight(..)>
                        nWidth += rAttrs.CalcLeft( pFrm ) + rAttrs.CalcRight( pFrm );
                        break;
                    case text::HoriOrientation::LEFT_AND_WIDTH:
                        nWidth += rAttrs.CalcLeft( pFrm );
                        break;
                    default:
                        break;

                }
                nBrowseWidth = Max( nBrowseWidth, nWidth );
            }
        }
        else if ( pFrm->GetDrawObjs() )
        {
            for ( USHORT i = 0; i < pFrm->GetDrawObjs()->Count(); ++i )
            {
                // --> OD 2004-06-29 #i28701#
                SwAnchoredObject* pAnchoredObj = (*pFrm->GetDrawObjs())[i];
                const SwFrmFmt& rFmt = pAnchoredObj->GetFrmFmt();
                const BOOL bFly = pAnchoredObj->ISA(SwFlyFrm);
                if ( bFly &&
                     WEIT_WECH == pAnchoredObj->GetObjRect().Width()||
                     rFmt.GetFrmSize().GetWidthPercent() )
                    continue;

                long nWidth = 0;
                switch ( rFmt.GetAnchor().GetAnchorId() )
                {
                    case FLY_IN_CNTNT:
                        nWidth = bFly ? rFmt.GetFrmSize().GetWidth() :
                                        pAnchoredObj->GetObjRect().Width();
                        break;
                    case FLY_AT_CNTNT:
                        {
                            // --> FME 2004-09-13 #i33170#
                            // Reactivated old code because
                            // nWidth = pAnchoredObj->GetObjRect().Right()
                            // gives wrong results for objects that are still
                            // at position WEIT_WECH.
                            if ( bFly )
                            {
                                nWidth = rFmt.GetFrmSize().GetWidth();
                                const SwFmtHoriOrient &rHori = rFmt.GetHoriOrient();
                                switch ( rHori.GetHoriOrient() )
                                {
                                    case text::HoriOrientation::NONE:
                                        nWidth += rHori.GetPos();
                                        break;
                                    case text::HoriOrientation::INSIDE:
                                    case text::HoriOrientation::LEFT:
                                        if ( text::RelOrientation::PRINT_AREA == rHori.GetRelationOrient() )
                                            nWidth += pFrm->Prt().Left();
                                        break;
                                    default:
                                        break;
                                }
                            }
                            else
                                //Fuer Zeichenobjekte ist die Auswahl sehr klein,
                                //weil sie keine Attribute haben, also durch ihre
                                //aktuelle Groesse bestimmt werden.
                                nWidth = pAnchoredObj->GetObjRect().Right() -
                                         pAnchoredObj->GetDrawObj()->GetAnchorPos().X();
                            // <--
                        }
                        break;
                    default:	/* do nothing */;
                }
                nBrowseWidth = Max( nBrowseWidth, nWidth );
            }
        }
        pFrm = pFrm->FindNextCnt();
    } while ( pFrm );
}

/*************************************************************************
|*
|*	SwRootFrm::StartAllAction()
|*
|*	Ersterstellung		MA 08. Mar. 98
|*	Letzte Aenderung	MA 08. Mar. 98
|*
|*************************************************************************/

void SwRootFrm::StartAllAction()
{
    ViewShell *pSh = GetCurrShell();
    if ( pSh )
        do
        {	if ( pSh->ISA( SwCrsrShell ) )
                ((SwCrsrShell*)pSh)->StartAction();
            else
                pSh->StartAction();
            pSh = (ViewShell*)pSh->GetNext();

        } while ( pSh != GetCurrShell() );
}

void SwRootFrm::EndAllAction( BOOL bVirDev )
{
    ViewShell *pSh = GetCurrShell();
    if ( pSh )
        do
        {
            const BOOL bOldEndActionByVirDev = pSh->IsEndActionByVirDev();
            pSh->SetEndActionByVirDev( bVirDev );
            if ( pSh->ISA( SwCrsrShell ) )
            {
                ((SwCrsrShell*)pSh)->EndAction();
                ((SwCrsrShell*)pSh)->CallChgLnk();
                if ( pSh->ISA( SwFEShell ) )
                    ((SwFEShell*)pSh)->SetChainMarker();
            }
            else
                pSh->EndAction();
            pSh->SetEndActionByVirDev( bOldEndActionByVirDev );
            pSh = (ViewShell*)pSh->GetNext();

        } while ( pSh != GetCurrShell() );
}

void SwRootFrm::UnoRemoveAllActions()
{
    ViewShell *pSh = GetCurrShell();
    if ( pSh )
        do
        {
            DBG_ASSERT(!pSh->GetRestoreActions(), "Restore action count is already set!")
            BOOL bCrsr = pSh->ISA( SwCrsrShell );
            BOOL bFE = pSh->ISA( SwFEShell );
            USHORT nRestore = 0;
            while( pSh->ActionCount() )
            {
                if( bCrsr )
                {
                    ((SwCrsrShell*)pSh)->EndAction();
                    ((SwCrsrShell*)pSh)->CallChgLnk();
                    if ( bFE )
                        ((SwFEShell*)pSh)->SetChainMarker();
                }
                else
                    pSh->EndAction();
                nRestore++;
            }
            pSh->SetRestoreActions(nRestore);
            pSh->LockView(TRUE);
            pSh = (ViewShell*)pSh->GetNext();

        } while ( pSh != GetCurrShell() );
}

void SwRootFrm::UnoRestoreAllActions()
{
    ViewShell *pSh = GetCurrShell();
    if ( pSh )
        do
        {
            USHORT nActions = pSh->GetRestoreActions();
            while( nActions-- )
            {
                if ( pSh->ISA( SwCrsrShell ) )
                    ((SwCrsrShell*)pSh)->StartAction();
                else
                    pSh->StartAction();
            }
            pSh->SetRestoreActions(0);
            pSh->LockView(FALSE);
            pSh = (ViewShell*)pSh->GetNext();

        } while ( pSh != GetCurrShell() );
}

