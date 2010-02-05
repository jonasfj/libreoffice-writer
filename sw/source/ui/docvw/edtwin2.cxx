/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: edtwin2.cxx,v $
 * $Revision: 1.31.130.3 $
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
#include <tools/ref.hxx>
#include <hintids.hxx>

#include <doc.hxx>
#ifdef DBG_UTIL
#include <stdio.h>
#endif

#ifndef _HELP_HXX //autogen
#include <vcl/help.hxx>
#endif
#include <svl/stritem.hxx>
#include <unotools/securityoptions.hxx>
#include <tools/urlobj.hxx>
#include <txtrfmrk.hxx>
#include <fmtrfmrk.hxx>
#include <editeng/flditem.hxx>
#include <svl/urihelper.hxx>
#include <svx/svdotext.hxx>
#ifndef _OUTLINER_HXX //autogen
#define _EEITEMID_HXX
#include <editeng/outliner.hxx>
#endif
#include <svl/itemiter.hxx>
#include <svx/svdview.hxx>
#include <svx/svdpagv.hxx>
#include <swmodule.hxx>
#ifndef _MODCFG_HXX
#include <modcfg.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <wrtsh.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <edtwin.hxx>
#include <dpage.hxx>
#include <shellres.hxx>
#include <docufld.hxx>
#include <dbfld.hxx>
#include <reffld.hxx>
#include <cellatr.hxx>
#include <shdwcrsr.hxx>
#include <fmtcol.hxx>
#include <charfmt.hxx>
#include <fmtftn.hxx>
#include <redline.hxx>
#include <tox.hxx>
#include <txttxmrk.hxx>
#include <uitool.hxx>
#include <viewopt.hxx>
#ifndef _DOCVW_HRC
#include <docvw.hrc>
#endif
#ifndef _UTLUI_HRC
#include <utlui.hrc>
#endif

#include <postit.hxx>
#include <PostItMgr.hxx>
#include <fmtfld.hxx>

// --> OD 2009-08-18 #i104300#
#include <IDocumentMarkAccess.hxx>
#include <ndtxt.hxx>
// <--

/*--------------------------------------------------------------------
    Beschreibung:	KeyEvents
 --------------------------------------------------------------------*/
static void lcl_GetRedlineHelp( const SwRedline& rRedl, String& rTxt, BOOL bBalloon )
{
    USHORT nResId = 0;
    switch( rRedl.GetType() )
    {
    case nsRedlineType_t::REDLINE_INSERT:	nResId = STR_REDLINE_INSERT; break;
    case nsRedlineType_t::REDLINE_DELETE:	nResId = STR_REDLINE_DELETE; break;
    case nsRedlineType_t::REDLINE_FORMAT:	nResId = STR_REDLINE_FORMAT; break;
    case nsRedlineType_t::REDLINE_TABLE:		nResId = STR_REDLINE_TABLE; break;
    case nsRedlineType_t::REDLINE_FMTCOLL:	nResId = STR_REDLINE_FMTCOLL; break;
    }

    if( nResId )
    {
        rTxt = SW_RESSTR( nResId );
        rTxt.AppendAscii( RTL_CONSTASCII_STRINGPARAM(": " ));
        rTxt += rRedl.GetAuthorString();
        rTxt.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " - " ));
        rTxt += GetAppLangDateTimeString( rRedl.GetTimeStamp() );
        if( bBalloon && rRedl.GetComment().Len() )
            ( rTxt += '\n' ) += rRedl.GetComment();
    }
}


void SwEditWin::RequestHelp(const HelpEvent &rEvt)
{
    SwWrtShell &rSh = rView.GetWrtShell();
    BOOL bQuickBalloon = 0 != (rEvt.GetMode() & ( HELPMODE_QUICK | HELPMODE_BALLOON ));
    if(bQuickBalloon && rSh.GetViewOptions()->IsPreventTips())
        return;
    BOOL bWeiter = TRUE;
    SET_CURR_SHELL(&rSh);
    String sTxt;
    Point aPos( PixelToLogic( ScreenToOutputPixel( rEvt.GetMousePosPixel() ) ));
    BOOL bBalloon = static_cast< BOOL >(rEvt.GetMode() & HELPMODE_BALLOON);

    SdrView *pSdrView = rSh.GetDrawView();

    if( bQuickBalloon )
    {
        if( pSdrView )
        {
            SdrPageView* pPV = pSdrView->GetSdrPageView();
            SwDPage* pPage = pPV ? ((SwDPage*)pPV->GetPage()) : 0;
            bWeiter = pPage && pPage->RequestHelp(this, pSdrView, rEvt);
        }
    }

    if( bWeiter && bQuickBalloon)
    {
        SwRect aFldRect;
        USHORT nStyle = 0; // style of quick help
        SwContentAtPos aCntntAtPos( SwContentAtPos::SW_FIELD |
                                    SwContentAtPos::SW_INETATTR |
                                    SwContentAtPos::SW_FTN |
                                    SwContentAtPos::SW_REDLINE |
                                    SwContentAtPos::SW_TOXMARK |
                                    SwContentAtPos::SW_REFMARK |
                                    SwContentAtPos::SW_SMARTTAG |
#ifdef DBG_UTIL
                                    SwContentAtPos::SW_TABLEBOXVALUE |
                        ( bBalloon ? SwContentAtPos::SW_CURR_ATTRS : 0) |
#endif
                                    SwContentAtPos::SW_TABLEBOXFML );

        if( rSh.GetContentAtPos( aPos, aCntntAtPos, FALSE, &aFldRect ) )
        {
             switch( aCntntAtPos.eCntntAtPos )
            {
            case SwContentAtPos::SW_TABLEBOXFML:
                sTxt.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "= " ));
                sTxt += ((SwTblBoxFormula*)aCntntAtPos.aFnd.pAttr)->GetFormula();
                break;
#ifdef DBG_UTIL
            case SwContentAtPos::SW_TABLEBOXVALUE:
            {
                sTxt = UniString(
                            ByteString::CreateFromDouble(
                                ((SwTblBoxValue*)aCntntAtPos.aFnd.pAttr)->GetValue()  )
                            , gsl_getSystemTextEncoding());
            }
            break;
            case SwContentAtPos::SW_CURR_ATTRS:
                sTxt = aCntntAtPos.sStr;
                break;
#endif

            case SwContentAtPos::SW_INETATTR:
            {
                sTxt = ((SfxStringItem*)aCntntAtPos.aFnd.pAttr)->GetValue();
                sTxt = URIHelper::removePassword( sTxt,
                                        INetURLObject::WAS_ENCODED,
                                           INetURLObject::DECODE_UNAMBIGUOUS);
                //#i63832# remove the link target type
                xub_StrLen nFound = sTxt.Search(cMarkSeperator);
                if( nFound != STRING_NOTFOUND && (++nFound) < sTxt.Len() )
                {
                    String sSuffix( sTxt.Copy(nFound) );
                    if( sSuffix.EqualsAscii( pMarkToTable ) ||
                        sSuffix.EqualsAscii( pMarkToFrame ) ||
                        sSuffix.EqualsAscii( pMarkToRegion ) ||
                        sSuffix.EqualsAscii( pMarkToOutline ) ||
                        sSuffix.EqualsAscii( pMarkToText ) ||
                        sSuffix.EqualsAscii( pMarkToGraphic ) ||
                        sSuffix.EqualsAscii( pMarkToOLE ))
                    sTxt = sTxt.Copy( 0, nFound - 1);
                }
                // --> OD 2009-08-18 #i104300#
                // special handling if target is a cross-reference bookmark
                {
                    String sTmpSearchStr = sTxt.Copy( 1, sTxt.Len() );
                    IDocumentMarkAccess* const pMarkAccess =
                                                rSh.getIDocumentMarkAccess();
                    IDocumentMarkAccess::const_iterator_t ppBkmk =
                                    pMarkAccess->findBookmark( sTmpSearchStr );
                    if ( ppBkmk != pMarkAccess->getBookmarksEnd() &&
                         IDocumentMarkAccess::GetType( *(ppBkmk->get()) )
                            == IDocumentMarkAccess::CROSSREF_HEADING_BOOKMARK )
                    {
                        SwTxtNode* pTxtNode = ppBkmk->get()->GetMarkStart().nNode.GetNode().GetTxtNode();
                        if ( pTxtNode )
                        {
                            sTxt = pTxtNode->GetExpandTxt( 0, pTxtNode->Len(), true, true );

                            if( sTxt.Len() )
                            {
                                sTxt.EraseAllChars( 0xad );
                                for( sal_Unicode* p = sTxt.GetBufferAccess(); *p; ++p )
                                {
                                    if( *p < 0x20 )
                                        *p = 0x20;
                                    else if(*p == 0x2011)
                                        *p = '-';
                                }
                            }
                        }
                    }
                }
                // <--
                // --> OD 2007-07-26 #i80029#
                BOOL bExecHyperlinks = rView.GetDocShell()->IsReadOnly();
                if ( !bExecHyperlinks )
                {
                    SvtSecurityOptions aSecOpts;
                    bExecHyperlinks = !aSecOpts.IsOptionSet( SvtSecurityOptions::E_CTRLCLICK_HYPERLINK );

                    if ( !bExecHyperlinks )
                    {
                        sTxt.InsertAscii( ": ", 0 );
                        sTxt.Insert( ViewShell::GetShellRes()->aHyperlinkClick, 0 );
                    }
                }
                // <--
                break;
            }
            case SwContentAtPos::SW_SMARTTAG:
            {
                sTxt = SW_RESSTR(STR_SMARTTAG_CLICK);

                KeyCode aCode( KEY_SPACE );
                KeyCode aModifiedCode( KEY_SPACE, KEY_MOD1 );
                String aModStr( aModifiedCode.GetName() );
                aModStr.SearchAndReplace( aCode.GetName(), String() );
                aModStr.SearchAndReplaceAllAscii( "+", String() );
                sTxt.SearchAndReplaceAllAscii( "%s", aModStr );
            }
            break;

            case SwContentAtPos::SW_FTN:
                if( aCntntAtPos.pFndTxtAttr && aCntntAtPos.aFnd.pAttr )
                {
                    const SwFmtFtn* pFtn = (SwFmtFtn*)aCntntAtPos.aFnd.pAttr;
                    pFtn->GetFtnText( sTxt );
                    sTxt.Insert( SW_RESSTR( pFtn->IsEndNote()
                                    ? STR_ENDNOTE : STR_FTNNOTE ), 0 );
                    if( aCntntAtPos.IsInRTLText() )
                        nStyle |= QUICKHELP_BIDI_RTL;
                }
                break;

            case SwContentAtPos::SW_REDLINE:
                lcl_GetRedlineHelp( *aCntntAtPos.aFnd.pRedl, sTxt, bBalloon );
                break;

            case SwContentAtPos::SW_TOXMARK:
                sTxt = aCntntAtPos.sStr;
                if( sTxt.Len() && aCntntAtPos.pFndTxtAttr )
                {
                    const SwTOXType* pTType = aCntntAtPos.pFndTxtAttr->
                                        GetTOXMark().GetTOXType();
                    if( pTType && pTType->GetTypeName().Len() )
                    {
                        sTxt.InsertAscii( ": ", 0 );
                        sTxt.Insert( pTType->GetTypeName(), 0 );
                    }
                }
                break;
            case SwContentAtPos::SW_REFMARK:
                if(aCntntAtPos.aFnd.pAttr)
                {
                    sTxt = SW_RES(STR_CONTENT_TYPE_SINGLE_REFERENCE);
                    sTxt.AppendAscii( RTL_CONSTASCII_STRINGPARAM( ": "));
                    sTxt += ((const SwFmtRefMark*)aCntntAtPos.aFnd.pAttr)->GetRefName();
                }
            break;

            default:
                {
                    SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();
                    if(!pModOpt->IsHideFieldTips())
                    {
                        const SwField* pFld = aCntntAtPos.aFnd.pFld;
                        switch( pFld->Which() )
                        {
                        case RES_SETEXPFLD:
                        case RES_TABLEFLD:
                        case RES_GETEXPFLD:
                        {
                            USHORT nOldSubType = pFld->GetSubType();
                            ((SwField*)pFld)->SetSubType(nsSwExtendedSubType::SUB_CMD);
                            sTxt = pFld->Expand();
                            ((SwField*)pFld)->SetSubType(nOldSubType);
                        }
                        break;

                        case RES_POSTITFLD:
                            {
                                /*
                                SwPostItMgr* pMgr = rView.GetPostItMgr();
                                if (pMgr->ShowNotes())
                                {
                                    SwFmtFld* pSwFmtFld = 0;
                                    if (pMgr->ShowPreview(pFld,pSwFmtFld))
                                    {
                                        SwPostIt* pPostIt = new SwPostIt(static_cast<Window*>(this),0,pSwFmtFld,pMgr,PB_Preview);
                                        pPostIt->InitControls();
                                        pPostIt->SetReadonly(true);
                                        pMgr->SetColors(pPostIt,static_cast<SwPostItField*>(pSwFmtFld->GetFld()));
                                        pPostIt->SetVirtualPosSize(rEvt.GetMousePosPixel(),Size(180,70));
                                        pPostIt->ShowNote();
                                        SetPointerPosPixel(pPostIt->GetPosPixel() + Point(20,20));
                                    }
                                    return;
                                }
                                */
                                break;
                            }
                        case RES_INPUTFLD:	// BubbleHelp, da der Hinweis ggf ziemlich lang sein kann
                            bBalloon = TRUE;
                            /* no break */
                        case RES_JUMPEDITFLD:
                            sTxt = pFld->GetPar2();
                            break;

                        case RES_DBFLD:
                            sTxt = ((SwDBField*)pFld)->GetCntnt(TRUE);
                            break;

                        case RES_USERFLD:
                        case RES_HIDDENTXTFLD:
                            sTxt = pFld->GetPar1();
                            break;

                        case RES_DOCSTATFLD:
                            break;

                        case RES_MACROFLD:
                            sTxt = ((const SwMacroField*)pFld)->GetMacro();
                            break;

                        case RES_GETREFFLD:
                        {
                            // --> OD 2008-01-09 #i85090#
                            const SwGetRefField* pRefFld( dynamic_cast<const SwGetRefField*>(pFld) );
                            ASSERT( pRefFld,
                                    "<SwEditWin::RequestHelp(..)> - unexpected type of <pFld>" );
                            if ( pRefFld )
                            {
                                if ( pRefFld->IsRefToHeadingCrossRefBookmark() ||
                                     pRefFld->IsRefToNumItemCrossRefBookmark() )
                                {
                                    sTxt = pRefFld->GetExpandedTxtOfReferencedTxtNode();
                                    if ( sTxt.Len() > 80  )
                                    {
                                        sTxt.Erase( 80 );
                                        sTxt += '.';
                                        sTxt += '.';
                                        sTxt += '.';
                                    }
                                }
                                else
                                {
                                    sTxt = ((SwGetRefField*)pFld)->GetSetRefName();
                                }
                            }
                            // <--
                        }
                        break;
                        }
                    }

                    if( !sTxt.Len() )
                    {
                        aCntntAtPos.eCntntAtPos = SwContentAtPos::SW_REDLINE;
                        if( rSh.GetContentAtPos( aPos, aCntntAtPos, FALSE, &aFldRect ) )
                            lcl_GetRedlineHelp( *aCntntAtPos.aFnd.pRedl,
                                                    sTxt, bBalloon );
                    }
                }
            }
            if (sTxt.Len() )
            {
                if( bBalloon )
                    Help::ShowBalloon( this, rEvt.GetMousePosPixel(), sTxt );
                else
                {
                    // dann zeige die Hilfe mal an:
                    Rectangle aRect( aFldRect.SVRect() );
                    Point aPt( OutputToScreenPixel( LogicToPixel( aRect.TopLeft() )));
                    aRect.Left()   = aPt.X();
                    aRect.Top()    = aPt.Y();
                    aPt = OutputToScreenPixel( LogicToPixel( aRect.BottomRight() ));
                    aRect.Right()  = aPt.X();
                    aRect.Bottom() = aPt.Y();
                    Help::ShowQuickHelp( this, aRect, sTxt, nStyle );
                }
            }

            bWeiter = FALSE;
        }
        if( bWeiter )
        {
            BYTE nTabCols = rSh.WhichMouseTabCol(aPos);
            USHORT nTabRes = 0;
            switch(nTabCols)
            {
                case SW_TABCOL_HORI:
                case SW_TABCOL_VERT:
                    nTabRes = STR_TABLE_COL_ADJUST;
                    break;
                case SW_TABROW_HORI:
                case SW_TABROW_VERT:
                    nTabRes = STR_TABLE_ROW_ADJUST;
                    break;
                // --> FME 2004-07-30 #i32329# Enhanced table selection
                case SW_TABSEL_HORI:
                case SW_TABSEL_HORI_RTL:
                case SW_TABSEL_VERT:
                    nTabRes = STR_TABLE_SELECT_ALL;
                    break;
                case SW_TABROWSEL_HORI:
                case SW_TABROWSEL_HORI_RTL:
                case SW_TABROWSEL_VERT:
                    nTabRes = STR_TABLE_SELECT_ROW;
                    break;
                case SW_TABCOLSEL_HORI:
                case SW_TABCOLSEL_VERT:
                    nTabRes = STR_TABLE_SELECT_COL;
                    break;
                // <--
            }
            if(nTabRes)
            {
                sTxt = SW_RESSTR(nTabRes);
                Size aTxtSize( GetTextWidth(sTxt), GetTextHeight());
                Rectangle aRect(rEvt.GetMousePosPixel(), aTxtSize);
                Help::ShowQuickHelp(this, aRect, sTxt);
            }
            bWeiter = FALSE;
        }
    }

/*
aktuelle Zeichenvorlage anzeigen?
    if( bWeiter && rEvt.GetMode() & ( HELPMODE_QUICK | HELPMODE_BALLOON ))
    {
        SwCharFmt* pChrFmt = rSh.GetCurCharFmt();

    }
*/
    if( bWeiter && pSdrView && bQuickBalloon)
    {
        SdrViewEvent aVEvt;
        SdrHitKind eHit = pSdrView->PickAnything(aPos, aVEvt);
        const SvxURLField *pField;
        SdrObject* pObj = NULL;

        if ((pField = aVEvt.pURLField) != 0)
        {
            // URL-Feld getroffen
            if (pField)
            {
                pObj = aVEvt.pObj;
                sTxt = pField->GetURL();

                bWeiter = FALSE;
            }
        }
        if (bWeiter && eHit == SDRHIT_TEXTEDIT)
        {
            // URL-Feld in zum Editieren ge?ffneten DrawText-Objekt suchen
            OutlinerView* pOLV = pSdrView->GetTextEditOutlinerView();
            const SvxFieldItem* pFieldItem;

            if (pSdrView->AreObjectsMarked())
            {
                const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();

                if (rMarkList.GetMarkCount() == 1)
                    pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
            }

            if (pObj && pObj->ISA(SdrTextObj) && pOLV &&
                    (pFieldItem = pOLV->GetFieldUnderMousePointer()) != 0)
            {
                pField = dynamic_cast<const SvxURLField*>(pFieldItem->GetField());

                if (pField )
                {
                    sTxt = ((const SvxURLField*) pField)->GetURL();
                    bWeiter = FALSE;
                }
            }
        }
        if (sTxt.Len() && pObj)
        {
            sTxt = URIHelper::removePassword( sTxt, INetURLObject::WAS_ENCODED,
                                           INetURLObject::DECODE_UNAMBIGUOUS);

            Rectangle aLogicPix = LogicToPixel(pObj->GetLogicRect());
            Rectangle aScreenRect(OutputToScreenPixel(aLogicPix.TopLeft()),
                                OutputToScreenPixel(aLogicPix.BottomRight()));

            if (bBalloon)
                Help::ShowBalloon(this, rEvt.GetMousePosPixel(), aScreenRect, sTxt);
            else
                Help::ShowQuickHelp(this, aScreenRect, sTxt);
        }
    }

    if( bWeiter )
        Window::RequestHelp( rEvt );
}

void SwEditWin::PrePaint()
{
    SwWrtShell* pWrtShell = GetView().GetWrtShellPtr();

    if(pWrtShell)
    {
        pWrtShell->PrePaint();
    }
}

void  SwEditWin::Paint(const Rectangle& rRect)
{
#if defined(MYDEBUG)
    // StartUp-Statistik
    if ( pTickList )
    {
        SYSTICK( "Start SwEditWin::Paint" );
        READ_FIRST_TICKS()
        FLUSH_TICKS()
    }
#endif

    SwWrtShell* pWrtShell = GetView().GetWrtShellPtr();
    if(!pWrtShell)
        return;
    BOOL bPaintShadowCrsr = FALSE;
    if( pShadCrsr )
    {
        Rectangle aRect( pShadCrsr->GetRect());
        // liegt vollstaendig drin?
        if( rRect.IsInside( aRect ) )
            // dann aufheben
            delete pShadCrsr, pShadCrsr = 0;
        else if( rRect.IsOver( aRect ))
        {
            // liegt irgendwie drueber, dann ist alles ausserhalb geclippt
            // und wir muessen den "inneren Teil" am Ende vom Paint
            // wieder sichtbar machen. Sonst kommt es zu Paintfehlern!
            bPaintShadowCrsr = TRUE;
        }
    }
/*
    //TODO/LATER: what's the replacement for this? Do we need it?
    SwDocShell* pDocShell = GetView().GetDocShell();

  SvInPlaceEnvironment *pIpEnv =  pDocShell ?
                                  pDocShell->GetIPEnv() : 0;
    if ( pIpEnv && pIpEnv->GetRectsChangedLockCount() )
        //Wir stehen in Groessenverhandlungen (MM), Paint verzoegern
        Invalidate( rRect );
    else */
    if ( GetView().GetVisArea().GetWidth()  <= 0 ||
              GetView().GetVisArea().GetHeight() <= 0 )
        Invalidate( rRect );
    else
        pWrtShell->Paint( rRect );

    if( bPaintShadowCrsr )
        pShadCrsr->Paint();
}


