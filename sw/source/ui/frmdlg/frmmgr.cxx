/*************************************************************************
 *
 *  $RCSfile: frmmgr.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: os $ $Date: 2002-09-13 13:51:23 $
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

#ifdef PRECOMPILED
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#include "cmdid.h"
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif
#include "hintids.hxx"


#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SVX_PROTITEM_HXX //autogen
#include <svx/protitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_OPAQITEM_HXX //autogen
#include <svx/opaqitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_SHADITEM_HXX //autogen
#include <svx/shaditem.hxx>
#endif


#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#include "wrtsh.hxx"
#include "view.hxx"
#include "viewopt.hxx"
#include "uitool.hxx"
#include "frmmgr.hxx"
#include "format.hxx"
#include "mdiexp.hxx"
#include "poolfmt.hxx"

static USHORT __FAR_DATA aFrmMgrRange[] = {
                            RES_FRMATR_BEGIN, RES_FRMATR_END-1,
                            SID_ATTR_BORDER_INNER, SID_ATTR_BORDER_INNER,
                            FN_SET_FRM_NAME, FN_SET_FRM_NAME,
                            0};

/*--------------------------------------------------------------------
     Beschreibung: Rahmen-Attribute ueber Shell ermitteln
 --------------------------------------------------------------------*/

SwFlyFrmAttrMgr::SwFlyFrmAttrMgr( BOOL bNew, SwWrtShell* pSh, BYTE nType ) :
    bAbsPos( FALSE ),
    bNewFrm( bNew ),
    bIsInVertical( FALSE ),
    aSet( (SwAttrPool&)pSh->GetAttrPool(), aFrmMgrRange ),
    pOwnSh( pSh )

{
    if ( bNewFrm )
    {
        // Defaults einstellen:
        USHORT nId;
        switch ( nType )
        {
            case FRMMGR_TYPE_TEXT:	nId = RES_POOLFRM_FRAME;	break;
            case FRMMGR_TYPE_OLE:	nId = RES_POOLFRM_OLE;		break;
            case FRMMGR_TYPE_GRF:	nId = RES_POOLFRM_GRAPHIC;	break;
        }
        aSet.SetParent( &pOwnSh->GetFmtFromPool( nId )->GetAttrSet());
        aSet.Put( SwFmtFrmSize( ATT_MIN_SIZE, DFLT_WIDTH, DFLT_HEIGHT ));
        if ( 0 != ::GetHtmlMode(pSh->GetView().GetDocShell()) )
            aSet.Put( SwFmtHoriOrient( 0, HORI_LEFT, PRTAREA ) );
    }
    else if ( nType == FRMMGR_TYPE_NONE )
    {
        pOwnSh->GetFlyFrmAttr( aSet );
        BOOL bRightToLeft;
        bIsInVertical = pOwnSh->IsFrmVertical(TRUE, bRightToLeft);
    }
    ::PrepareBoxInfo( aSet, *pOwnSh );
}

SwFlyFrmAttrMgr::SwFlyFrmAttrMgr( BOOL bNew, SwWrtShell* pSh, const SfxItemSet &rSet ) :
    bAbsPos( FALSE ),
    bNewFrm( bNew ),
    aSet( rSet ),
    pOwnSh( pSh )

{
}


/*--------------------------------------------------------------------
     Beschreibung:	Initialisieren
 --------------------------------------------------------------------*/

void SwFlyFrmAttrMgr::UpdateAttrMgr()
{
    if ( !bNewFrm && pOwnSh->IsFrmSelected() )
        pOwnSh->GetFlyFrmAttr( aSet );
    ::PrepareBoxInfo( aSet, *pOwnSh );
}

void SwFlyFrmAttrMgr::_UpdateFlyFrm()
{
    const SfxPoolItem* pItem = 0;

    if (aSet.GetItemState(FN_SET_FRM_NAME, FALSE, &pItem) == SFX_ITEM_SET)
        pOwnSh->SetFlyName(((SfxStringItem *)pItem)->GetValue());

    pOwnSh->SetModified();

    if ( bAbsPos )
    {
        pOwnSh->SetFlyPos( aAbsPos );
        bAbsPos = FALSE;
    }
}

/*--------------------------------------------------------------------
    Beschreibung: Bestehenden Fly-Frame aendern
 --------------------------------------------------------------------*/

void SwFlyFrmAttrMgr::UpdateFlyFrm()
{
    ASSERT( pOwnSh->IsFrmSelected(),
        "Kein Rahmen selektiert oder keine Shell, Update nicht moeglich");

    if( pOwnSh->IsFrmSelected() )
    {
        //JP 6.8.2001: set never an invalid anchor into the core.
        const SfxPoolItem *pGItem, *pItem;
        if( SFX_ITEM_SET == aSet.GetItemState( RES_ANCHOR, FALSE, &pItem ))
        {
            SfxItemSet aGetSet( *aSet.GetPool(), RES_ANCHOR, RES_ANCHOR );
            if( pOwnSh->GetFlyFrmAttr( aGetSet ) && 1 == aGetSet.Count() &&
                SFX_ITEM_SET == aGetSet.GetItemState( RES_ANCHOR, FALSE, &pGItem )
                && ((SwFmtAnchor*)pGItem)->GetAnchorId() ==
                   ((SwFmtAnchor*)pItem)->GetAnchorId() )
                aSet.ClearItem( RES_ANCHOR );
        }

        // return wg. BASIC
        if( aSet.Count() )
        {
            pOwnSh->StartAllAction();
            pOwnSh->SetFlyFrmAttr( aSet );
            _UpdateFlyFrm();
            pOwnSh->EndAllAction();
        }
    }
}

/*--------------------------------------------------------------------
     Beschreibung:	Rahmen einfuegen
 --------------------------------------------------------------------*/

BOOL SwFlyFrmAttrMgr::InsertFlyFrm()
{
    pOwnSh->StartAllAction();

    BOOL bRet = 0 != pOwnSh->NewFlyFrm( aSet );

    // richtigen Mode an der Shell einschalten, Rahmen wurde aut. selektiert.
    if ( bRet )
    {
        _UpdateFlyFrm();
        pOwnSh->EnterSelFrmMode();
        FrameNotify(pOwnSh, FLY_DRAG_START);
    }
    pOwnSh->EndAllAction();
    return bRet;
}

/*------------------------------------------------------------------------
 Beschreibung:	Rahmen des Typs eAnchorType einfuegen. Position und
                Groesse werden explizit angegeben.
                Nicht erlaubte Werte des Aufzaehlungstypes werden
                korrigiert.
------------------------------------------------------------------------*/

void SwFlyFrmAttrMgr::InsertFlyFrm(RndStdIds	eAnchorType,
                                   const Point	&rPos,
                                   const Size	&rSize,
                                   BOOL bAbs )
{
    ASSERT( eAnchorType == FLY_PAGE		||
            eAnchorType == FLY_AT_CNTNT ||
            eAnchorType == FLY_AUTO_CNTNT ||
            eAnchorType == FLY_AT_FLY ||
            eAnchorType == FLY_IN_CNTNT, "Rahmentyp nicht erlaubt" );

    if ( bAbs )
        SetAbsPos( rPos );
    else
        SetPos( rPos );

    SetSize( rSize );
    SetAnchor( eAnchorType );
    InsertFlyFrm();
}

/*--------------------------------------------------------------------
     Beschreibung:	Anker setzen
 --------------------------------------------------------------------*/

void SwFlyFrmAttrMgr::SetAnchor( RndStdIds eId )
{
    USHORT nPhyPageNum, nVirtPageNum;
    pOwnSh->GetPageNum( nPhyPageNum, nVirtPageNum );

    aSet.Put( SwFmtAnchor( eId, nPhyPageNum ) );
    if( FLY_PAGE == eId || FLY_AT_CNTNT == eId || FLY_AUTO_CNTNT == eId
        || FLY_AT_FLY == eId )
    {
        SwFmtVertOrient aVertOrient( GetVertOrient() );
        SwFmtHoriOrient aHoriOrient( GetHoriOrient() );
        aHoriOrient.SetRelationOrient( FRAME );
        aVertOrient.SetRelationOrient( FRAME );
        aSet.Put( aVertOrient );
        aSet.Put( aHoriOrient );
    }
}

/*------------------------------------------------------------------------
 Beschreibung:	Setzen des Attributs fuer Spalten
------------------------------------------------------------------------*/

void SwFlyFrmAttrMgr::SetCol( const SwFmtCol &rCol )
{
    aSet.Put( rCol );
}
/*--------------------------------------------------------------------
     Beschreibung:	Absolute Position setzen
 --------------------------------------------------------------------*/

void SwFlyFrmAttrMgr::SetAbsPos( const Point& rPoint )
{
    bAbsPos = TRUE;
    aAbsPos = rPoint;

    SwFmtVertOrient aVertOrient( GetVertOrient() );
    SwFmtHoriOrient aHoriOrient( GetHoriOrient() );
    aHoriOrient.SetHoriOrient( HORI_NONE );
    aVertOrient.SetVertOrient( VERT_NONE );
    aSet.Put( aVertOrient );
    aSet.Put( aHoriOrient );
}

/*--------------------------------------------------------------------
    Beschreibung: Metriken auf Korrektheit pruefen
 --------------------------------------------------------------------*/

void SwFlyFrmAttrMgr::ValidateMetrics( SwFrmValid& rVal, BOOL bOnlyPercentRefValue )
{
    if (!bOnlyPercentRefValue)
    {
        rVal.nMinHeight = MINFLY + CalcTopSpace() + CalcBottomSpace();
        rVal.nMinWidth =  MINFLY + CalcLeftSpace()+ CalcRightSpace();
    }

    SwRect aBoundRect;

    pOwnSh->CalcBoundRect(aBoundRect, rVal.eArea, rVal.eHRel, rVal.bMirror, NULL, &rVal.aPercentSize);

    if (bOnlyPercentRefValue)
        return;

    if(bIsInVertical)
    {
        Point aPos(aBoundRect.Pos());
        long nTmp = aPos.X();
        aPos.X() = aPos.Y();
        aPos.Y() = nTmp;
        Size aSize(aBoundRect.SSize());
        nTmp = aSize.Width();
        aSize.Width() = aSize.Height();
        aSize.Height() = nTmp;
        aBoundRect.Chg( aPos, aSize );
        //exchange width/height to enable correct values
        nTmp = rVal.nWidth;
        rVal.nWidth = rVal.nHeight;
        rVal.nHeight = nTmp;
    }
    if ( rVal.eArea == FLY_PAGE || rVal.eArea == FLY_AT_FLY )
    {
        // MinimalPosition
        rVal.nMinHPos = aBoundRect.Left();
        rVal.nMinVPos = aBoundRect.Top();
        SwTwips nH = rVal.nHPos;
        SwTwips nV = rVal.nVPos;

        if (rVal.nHPos + rVal.nWidth > aBoundRect.Right())
        {
            if (rVal.eHori == HORI_NONE)
            {
                rVal.nHPos -= ((rVal.nHPos + rVal.nWidth) - aBoundRect.Right());
                nH = rVal.nHPos;
            }
            else
                rVal.nWidth = aBoundRect.Right() - rVal.nHPos;
        }

        if (rVal.nHPos + rVal.nWidth > aBoundRect.Right())
            rVal.nWidth = aBoundRect.Right() - rVal.nHPos;

        if (rVal.nVPos + rVal.nHeight > aBoundRect.Bottom())
        {
            if (rVal.eVert == SVX_VERT_NONE)
            {
                rVal.nVPos -= ((rVal.nVPos + rVal.nHeight) - aBoundRect.Bottom());
                nV = rVal.nVPos;
            }
            else
                rVal.nHeight = aBoundRect.Bottom() - rVal.nVPos;
        }

        if (rVal.nVPos + rVal.nHeight > aBoundRect.Bottom())
            rVal.nHeight = aBoundRect.Bottom() - rVal.nVPos;

        if ( rVal.eVert != SVX_VERT_NONE )
            nV = aBoundRect.Top();

        if ( rVal.eHori != HORI_NONE )
            nH = aBoundRect.Left();

        rVal.nMaxHPos	= aBoundRect.Right()  - rVal.nWidth;
        rVal.nMaxHeight = aBoundRect.Bottom() - nV;

        rVal.nMaxVPos	= aBoundRect.Bottom() - rVal.nHeight;
        rVal.nMaxWidth	= aBoundRect.Right()  - nH;
    }
    else if ( rVal.eArea == FLY_AT_CNTNT || rVal.eArea == FLY_AUTO_CNTNT )
    {
        SwTwips nH = rVal.nHPos;
        SwTwips nV = rVal.nVPos;

        if (rVal.nHPos + rVal.nWidth > aBoundRect.Right())
        {
            if (rVal.eHori == HORI_NONE)
            {
                rVal.nHPos -= ((rVal.nHPos + rVal.nWidth) - aBoundRect.Right());
                nH = rVal.nHPos;
            }
            else
                rVal.nWidth = aBoundRect.Right() - rVal.nHPos;
        }

        if (rVal.nVPos + rVal.nHeight > aBoundRect.Bottom())
        {
            if (rVal.eVert == SVX_VERT_NONE)
            {
                rVal.nVPos -= ((rVal.nVPos + rVal.nHeight) - aBoundRect.Bottom());
                nV = rVal.nVPos;
            }
            else
                rVal.nHeight = aBoundRect.Bottom() - rVal.nVPos;
        }

        if ( rVal.eVert != SVX_VERT_NONE )
            nV = aBoundRect.Top();

        if ( rVal.eHori != HORI_NONE )
            nH = aBoundRect.Left();

        rVal.nMinVPos  = aBoundRect.Top();
        rVal.nMaxVPos  = aBoundRect.Height() - rVal.nHeight;

        rVal.nMinHPos  = aBoundRect.Left();
        rVal.nMaxHPos  = aBoundRect.Right() - rVal.nWidth;

        // Maximale Breite Hoehe
        rVal.nMaxHeight  = rVal.nMaxVPos + rVal.nHeight - nV;
        rVal.nMaxWidth	 = rVal.nMaxHPos + rVal.nWidth - nH;
    }
    else if ( rVal.eArea == FLY_IN_CNTNT )
    {
        rVal.nMinHPos = 0;
        rVal.nMaxHPos = 0;

        rVal.nMaxHeight = aBoundRect.Height();
        rVal.nMaxWidth	= aBoundRect.Width();

        rVal.nMaxVPos	= aBoundRect.Height();
        rVal.nMinVPos	= -aBoundRect.Height() + rVal.nHeight;
        if (rVal.nMaxVPos < rVal.nMinVPos)
        {
            rVal.nMinVPos = rVal.nMaxVPos;
            rVal.nMaxVPos = -aBoundRect.Height();
        }
    }
    if(bIsInVertical)
    {
        //restore width/height exchange
        long nTmp = rVal.nWidth;
        rVal.nWidth = rVal.nHeight;
        rVal.nHeight = nTmp;
    }

    if (rVal.nMaxWidth < rVal.nWidth)
        rVal.nWidth = rVal.nMaxWidth;
    if (rVal.nMaxHeight < rVal.nHeight)
        rVal.nHeight = rVal.nMaxHeight;
}

/*--------------------------------------------------------------------
    Beschreibung: Korrektur fuer Umrandung
 --------------------------------------------------------------------*/

long SwFlyFrmAttrMgr::CalcWidthSpace()
{
    SvxLRSpaceItem &rLR = (SvxLRSpaceItem&)aSet.Get(RES_LR_SPACE);
    return rLR.GetLeft() + rLR.GetRight() + CalcLeftSpace() + CalcRightSpace();
}

long SwFlyFrmAttrMgr::CalcHeightSpace()
{
    SvxULSpaceItem &rUL = (SvxULSpaceItem&)aSet.Get(RES_UL_SPACE);
    return rUL.GetUpper() + rUL.GetLower() + CalcTopSpace() + CalcBottomSpace();
}

SwTwips SwFlyFrmAttrMgr::CalcTopSpace()
{
    const SvxShadowItem& rShadow = GetShadow();
    const SvxBoxItem& 	 rBox 	 = GetBox();
    return rShadow.CalcShadowSpace(SHADOW_TOP ) + rBox.CalcLineSpace(BOX_LINE_TOP);
}

SwTwips SwFlyFrmAttrMgr::CalcBottomSpace()
{
    const SvxShadowItem& rShadow = GetShadow();
    const SvxBoxItem& rBox 		 = GetBox();
    return rShadow.CalcShadowSpace(SHADOW_BOTTOM) + rBox.CalcLineSpace(BOX_LINE_BOTTOM);
}

SwTwips SwFlyFrmAttrMgr::CalcLeftSpace()
{
    const SvxShadowItem& rShadow = GetShadow();
    const SvxBoxItem&	 rBox 	 = GetBox();
    return rShadow.CalcShadowSpace(SHADOW_LEFT) + rBox.CalcLineSpace(BOX_LINE_LEFT);
}

SwTwips SwFlyFrmAttrMgr::CalcRightSpace()
{
    const SvxShadowItem& rShadow = GetShadow();
    const SvxBoxItem&	 rBox 	 = GetBox();
    return rShadow.CalcShadowSpace(SHADOW_RIGHT) + rBox.CalcLineSpace(BOX_LINE_RIGHT);
}


/*--------------------------------------------------------------------
    Beschreibung: Attribut aus dem Set loeschen
 --------------------------------------------------------------------*/
void SwFlyFrmAttrMgr::DelAttr( USHORT nId )
{
    aSet.ClearItem( nId );
}

void SwFlyFrmAttrMgr::SetLRSpace( long nLeft, long nRight )
{
    ASSERT( LONG_MAX != nLeft && LONG_MAX != nRight, "Welchen Raend setzen?" );

    SvxLRSpaceItem aTmp( (SvxLRSpaceItem&)aSet.Get( RES_LR_SPACE ) );
    if( LONG_MAX != nLeft )
        aTmp.SetLeft( USHORT(nLeft) );
    if( LONG_MAX != nRight )
        aTmp.SetRight( USHORT(nRight) );
    aSet.Put( aTmp );
}

void SwFlyFrmAttrMgr::SetULSpace( long nTop, long nBottom )
{
    ASSERT( LONG_MAX != nTop && LONG_MAX != nBottom, Welchen Raend setzen? );

    SvxULSpaceItem aTmp( (SvxULSpaceItem&)aSet.Get( RES_UL_SPACE ) );
    if( LONG_MAX != nTop )
        aTmp.SetUpper( USHORT(nTop) );
    if( LONG_MAX != nBottom )
        aTmp.SetLower( USHORT(nBottom) );
    aSet.Put( aTmp );
}

void SwFlyFrmAttrMgr::SetWrapType( SwSurround eFly )
{
    SwFmtSurround aWrap( GetSurround() );
    aWrap.SetSurround( eFly );
    aSet.Put( aWrap );
}

void SwFlyFrmAttrMgr::SetAnchorOnly(BOOL bSet)
{
    SwFmtSurround aWrap( GetSurround() );
    aWrap.SetAnchorOnly(bSet);
    aSet.Put( aWrap );
}

void SwFlyFrmAttrMgr::SetContour(	BOOL bSet)
{
    SwFmtSurround aWrap( GetSurround() );
    aWrap.SetContour(bSet);
    aSet.Put( aWrap );
}

void SwFlyFrmAttrMgr::SetPos( const Point& rPoint )
{
    SwFmtVertOrient aVertOrient( GetVertOrient() );
    SwFmtHoriOrient aHoriOrient( GetHoriOrient() );

    aHoriOrient.SetPos		 ( rPoint.X() );
    aHoriOrient.SetHoriOrient( HORI_NONE  );

    aVertOrient.SetPos		 ( rPoint.Y() );
    aVertOrient.SetVertOrient( VERT_NONE  );

    aSet.Put( aVertOrient );
    aSet.Put( aHoriOrient );
}

void SwFlyFrmAttrMgr::SetHorzOrientation(SwHoriOrient eOrient)
{
    SwFmtHoriOrient aHoriOrient( GetHoriOrient() );
    aHoriOrient.SetHoriOrient( eOrient );
    aSet.Put( aHoriOrient );
}

void SwFlyFrmAttrMgr::SetVertOrientation(SvxFrameVertOrient eOrient)
{
    SwFmtVertOrient aVertOrient( GetVertOrient() );
    aVertOrient.SetVertOrient( (SwVertOrient)eOrient );
    aSet.Put( aVertOrient );
}

void SwFlyFrmAttrMgr::SetSizeType( SwFrmSize eType )
{
    SwFmtFrmSize aSize( GetFrmSize() );
    aSize.SetSizeType( eType );
    aSet.Put( aSize );
}

void SwFlyFrmAttrMgr::SetSize( const Size& rSize )
{
    SwFmtFrmSize aSize( GetFrmSize() );
    aSize.SetSize(Size(Max(rSize.Width(), long(MINFLY)), Max(rSize.Height(), long(MINFLY))));
    aSet.Put( aSize );
}

void SwFlyFrmAttrMgr::SetFrmSize(const SwFmtFrmSize& rFrmSize)
{
    aSet.Put(rFrmSize);
}

void SwFlyFrmAttrMgr::ProtectPosSize( BOOL bProt )
{
    SvxProtectItem aProtection( ((SvxProtectItem&)aSet.Get(RES_PROTECT)));
    aProtection.SetSizeProtect( bProt );
    aProtection.SetPosProtect ( bProt );
    aSet.Put( aProtection );
}

void SwFlyFrmAttrMgr::SetTransparent(BOOL bTrans)
{
    aSet.Put( SvxOpaqueItem( RES_OPAQUE, !bTrans ));
}


void SwFlyFrmAttrMgr::SetAttrSet(const SfxItemSet& rSet)
{
    aSet.ClearItem();
    aSet.Put( rSet );
}

/*--------------------------------------------------------------------
    Beschreibung: Validierung der Inputs
 --------------------------------------------------------------------*/

SwFrmValid::SwFrmValid() :
    bAuto(0),
    bMirror(0),
    nHPos(0),
    nMaxHPos(LONG_MAX),
    nMinHPos(0),

    nVPos(0),
    nMaxVPos(LONG_MAX),
    nMinVPos(0),

    nWidth( DFLT_WIDTH ),
    nMinWidth(0),
    nMaxWidth(LONG_MAX),

    nHeight( DFLT_HEIGHT ),
    nMaxHeight(LONG_MAX)
{
}


