/*************************************************************************
 *
 *  $RCSfile: viewsrch.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: kz $ $Date: 2004-05-18 14:13:00 $
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

#include <string> // HACK: prevent conflict between STLPORT and Workshop headers

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif

#ifndef _COM_SUN_STAR_UTIL_SEARCHOPTIONS_HPP_
#include <com/sun/star/util/SearchOptions.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif

#ifndef _SVTOOLS_CJKOPTIONS_HXX
#include <svtools/cjkoptions.hxx>
#endif
#ifndef _SVTOOLS_CTLOPTIONS_HXX
#include <svtools/ctloptions.hxx>
#endif
#ifndef _SVX_PAGEITEM_HXX //autogen
#include <svx/pageitem.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _T2XTCMP_HXX //autogen
#include <svtools/txtcmp.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX //autogen
#include <svtools/itempool.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SVX_SRCHITEM_HXX //autogen
#include <svx/srchitem.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVX_SRCHDLG_HXX //autogen
#include <svx/srchdlg.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif

#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif
#ifndef _WORKCTRL_HXX
#include <workctrl.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>               	// fuer Undo-Ids
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif

#ifndef _VIEW_HRC
#include <view.hrc>
#endif

using namespace com::sun::star;
using namespace com::sun::star::i18n;
using namespace com::sun::star::lang;
using namespace com::sun::star::util;
using namespace com::sun::star::i18n;

#define SRCH_ATTR_OFF	0
#define SRCH_ATTR_ON    1
#define SRCH_ATTR_SET   2

/*--------------------------------------------------------------------
    Beschreibung:	Search Parameter
 --------------------------------------------------------------------*/

struct SwSearchOptions
{
    SwDocPositions eStart, eEnd;
    BOOL bDontWrap;

    SwSearchOptions( SwWrtShell* pSh, BOOL bBackward );
};


inline Window* GetParentWindow( SvxSearchDialog* pSrchDlg )
{
    Window* pWin;
    if( pSrchDlg && pSrchDlg->IsVisible() )
        pWin = pSrchDlg;
    else
        pWin = 0;
    return pWin;
}


/*-----------------12.04.97 13:04-------------------

--------------------------------------------------*/


void SwView::ExecSearch(SfxRequest& rReq, BOOL bNoMessage)
{
    const SfxItemSet* pArgs = rReq.GetArgs();
    const SfxPoolItem* pItem = 0;
    const USHORT nId = SvxSearchDialogWrapper::GetChildWindowId();
    SvxSearchDialogWrapper *pWrp = (SvxSearchDialogWrapper*)GetViewFrame()->GetChildWindow(nId);
    BOOL bQuiet = FALSE;
    if(pArgs && SFX_ITEM_SET == pArgs->GetItemState(SID_SEARCH_QUIET, FALSE, &pItem))
        bQuiet = ((const SfxBoolItem*) pItem)->GetValue();

    BOOL bApi = bQuiet | bNoMessage;
    BOOL bSrchList = TRUE;

    USHORT nSlot = rReq.GetSlot();
    if (nSlot == FN_REPEAT_SEARCH && !pSrchItem)
    {
        if(bApi)
        {
            rReq.SetReturnValue(SfxBoolItem(nSlot, FALSE));
            nSlot = 0;
        }
    }
    switch (nSlot)
    {
    // erstmal Nichts tun
    case SID_SEARCH_ITEM:
    {
        delete pSrchItem;
        pSrchItem = (SvxSearchItem*) pArgs->Get(SID_SEARCH_ITEM).Clone();
    }
    break;

    case FID_SEARCH_ON:
        bJustOpened = TRUE;
        GetViewFrame()->GetBindings().Invalidate(SID_SEARCH_ITEM);
        break;

    case FID_SEARCH_OFF:
        if(pArgs)
        {
            // Dialog abmelden
            delete pSrchItem;
            pSrchItem = (SvxSearchItem*) pArgs->Get(SID_SEARCH_ITEM).Clone();

            const USHORT nId = SvxSearchDialogWrapper::GetChildWindowId();
            SvxSearchDialogWrapper *pWrp = (SvxSearchDialogWrapper*)GetViewFrame()->GetChildWindow(nId);

            DELETEZ( pSrchList );
            DELETEZ( pReplList );

            if ( pWrp )
            {
                pSrchDlg = (SvxSearchDialog*)(pWrp->GetWindow());
                // die Search / Replace -Items merken wir uns
                const SearchAttrItemList* pList = pSrchDlg->GetSearchItemList();
                if( pList && pList->Count() )
                    pSrchList = new SearchAttrItemList( *pList );

                if( 0 != (pList = pSrchDlg->GetReplaceItemList() ) &&
                    pList->Count() )
                    pReplList = new SearchAttrItemList( *pList );
            }
        }
        break;

        case FN_REPEAT_SEARCH:
        case FID_SEARCH_NOW:
        {
            {
                if(FID_SEARCH_NOW == nSlot && !rReq.IsAPI())
                    SwView::SetMoveType(NID_SRCH_REP);
                if ( pWrp )
                {
                    pSrchDlg = (SvxSearchDialog*)(pWrp->GetWindow());
                }
                else
                    pSrchDlg = 0;
            }

            if (pSrchDlg)
            {
                DELETEZ( pSrchList );
                DELETEZ( pReplList );

                const SearchAttrItemList* pList = pSrchDlg->GetSearchItemList();
                if( pList && pList->Count() )
                    pSrchList = new SearchAttrItemList( *pList );

                if( 0 != (pList = pSrchDlg->GetReplaceItemList() ) &&
                    pList->Count() )
                    pReplList = new SearchAttrItemList( *pList );
            }

            if (nSlot == FN_REPEAT_SEARCH)
            {
                ASSERT(pSrchItem, "Search-Item fehlt");
                if( !pSrchItem )
                    pSrchItem = new SvxSearchItem;
            }
            else
            {
                // SearchItem aus Request besorgen
                ASSERT(pArgs, "Args fehlen");
                delete pSrchItem;
                pSrchItem = (SvxSearchItem*) pArgs->Get(SID_SEARCH_ITEM).Clone();
            }

            switch (pSrchItem->GetCommand())
            {
            case SVX_SEARCHCMD_FIND:
            {
                BOOL bRet = SearchAndWrap(bApi);
                if( bRet )
                    Scroll(pWrtShell->GetCharRect().SVRect());
                rReq.SetReturnValue(SfxBoolItem(nSlot, bRet));
            }
            break;
            case SVX_SEARCHCMD_FIND_ALL:
            {
                BOOL bRet = SearchAll();
                if( !bRet )
                {
                    if( !bApi )
                    {
                        Window* pParentWindow = GetParentWindow( pSrchDlg );
                        InfoBox( pParentWindow, SW_RES(MSG_NOT_FOUND)).Execute();
                    }
                    bFound = FALSE;
                }
                rReq.SetReturnValue(SfxBoolItem(nSlot, bRet));
            }
            break;
            case SVX_SEARCHCMD_REPLACE:
                pWrtShell->StartAllAction();
                {

                    // 1) Selektion ersetzen (nicht. wenn nur Attribute ersetzt
                    //    werden sollen)
//JP 27.04.95: warum ?
// 		was ist, wenn man das gefundene nur attributieren will??

                    USHORT nCmd = SVX_SEARCHCMD_FIND;
                    if( pSrchItem->GetReplaceString().Len() ||
                        !pReplList )
                    {
                        // Verhindern, dass - falls der Suchstring im
                        // Ersetzungsstring enthalten ist - der ersetzte String
                        // noch einmal gefunden wird.

                        BOOL bBack = pSrchItem->GetBackward();
                        if (bBack)
                            pWrtShell->Push();
                        Replace();
                        if (bBack)
                        {
                            pWrtShell->Pop();
                            pWrtShell->SwapPam();
                        }
                    }
                    else if( pReplList )
                        nCmd = SVX_SEARCHCMD_REPLACE;

                    // 2) Weiter suchen (ohne zu ersetzen!)

                    USHORT nOldCmd = pSrchItem->GetCommand();
                    pSrchItem->SetCommand( nCmd );
                    BOOL bRet = SearchAndWrap(bApi);
                    if( bRet )
                        Scroll( pWrtShell->GetCharRect().SVRect());
                    pSrchItem->SetCommand( nOldCmd );
                    rReq.SetReturnValue(SfxBoolItem(nSlot, bRet));
                }
                pWrtShell->EndAllAction();
                break;

            case SVX_SEARCHCMD_REPLACE_ALL:
                {
                    SwSearchOptions aOpts( pWrtShell, pSrchItem->GetBackward() );


                    if( !pSrchItem->GetSelection() )
                    {
                        // bestehende Selektionen aufheben,
                        // wenn nicht in selektierten Bereichen gesucht werden soll
                        (pWrtShell->*pWrtShell->fnKillSel)(0, FALSE);
                        if( DOCPOS_START == aOpts.eEnd )
                            pWrtShell->EndDoc();
                        else
                            pWrtShell->SttDoc();
                    }

                    bExtra = FALSE;
                    ULONG nFound;

                    {	//Scope for SwWait-Object
                        SwWait aWait( *GetDocShell(), TRUE );
                        pWrtShell->StartAllAction();
                        nFound = FUNC_Search( aOpts );
                        pWrtShell->EndAllAction();
                    }
                    rReq.SetReturnValue(SfxBoolItem(nSlot, nFound != 0 && ULONG_MAX != nFound));
                    if( !nFound )
                    {
                        if( !bApi )
                        {
                            Window* pParentWindow = GetParentWindow( pSrchDlg );
                            InfoBox( pParentWindow, SW_RES(MSG_NOT_FOUND)).Execute();
                        }
                        bFound = FALSE;
                        return;
                    }

                    if( !bApi && ULONG_MAX != nFound)
                    {
                        String aText( SW_RES( STR_NB_REPLACED ) );
                        const xub_StrLen nPos = aText.Search( String::CreateFromAscii("XX") );
                        aText.Erase( nPos, 2 );
                        aText.Insert( String::CreateFromInt32( nFound ), nPos );
                        Window* pParentWindow = GetParentWindow( pSrchDlg );
                        InfoBox( pParentWindow, aText ).Execute();
                    }
                }
                break;
            }
            com::sun::star::uno::Reference< com::sun::star::frame::XDispatchRecorder > xRecorder =
                    GetViewFrame()->GetBindings().GetRecorder();
            //prevent additional dialogs in recorded macros
            if ( xRecorder.is() )
                rReq.AppendItem(SfxBoolItem(SID_SEARCH_QUIET, sal_True));

            rReq.Done();
        }
        break;
        case FID_SEARCH_SEARCHSET:
        case FID_SEARCH_REPLACESET:
        {
            static const USHORT aNormalAttr[] =
            {
/* 0 */			RES_CHRATR_CASEMAP,		RES_CHRATR_CASEMAP,
/* 2 */			RES_CHRATR_COLOR, 		RES_CHRATR_POSTURE,
/* 4 */			RES_CHRATR_SHADOWED, 	RES_CHRATR_WORDLINEMODE,
/* 6 */			RES_CHRATR_BLINK,		RES_CHRATR_BLINK,
/* 8 */			RES_CHRATR_BACKGROUND,	RES_CHRATR_BACKGROUND,
/*10 */			RES_CHRATR_ROTATE,		RES_CHRATR_ROTATE,
/*12 */			RES_CHRATR_SCALEW,		RES_CHRATR_RELIEF,
// insert position for CJK/CTL attributes!
/*14 */			RES_PARATR_LINESPACING, RES_PARATR_HYPHENZONE,
/*16 */			RES_PARATR_REGISTER, 	RES_PARATR_REGISTER,
/*18 */			RES_PARATR_VERTALIGN, 	RES_PARATR_VERTALIGN,
/*20 */			RES_LR_SPACE, 			RES_UL_SPACE,
/*22 */			SID_ATTR_PARA_MODEL, 	SID_ATTR_PARA_KEEP,
/*24 */ 		0
            };

            static const USHORT aCJKAttr[] =
            {
                RES_CHRATR_CJK_FONT,	RES_CHRATR_CJK_WEIGHT,
                RES_CHRATR_EMPHASIS_MARK, RES_CHRATR_TWO_LINES,
                RES_PARATR_SCRIPTSPACE, RES_PARATR_FORBIDDEN_RULES
            };
            static const USHORT aCTLAttr[] =
            {
                RES_CHRATR_CTL_FONT,	RES_CHRATR_CTL_WEIGHT
            };

            SvUShorts aArr( 0, 16 );
            aArr.Insert(	aNormalAttr,
                            sizeof( aNormalAttr ) / sizeof( aNormalAttr[0] ),
                            0 );
            if( SW_MOD()->GetCTLOptions().IsCTLFontEnabled() )
                aArr.Insert(	aCTLAttr,
                                sizeof( aCTLAttr ) / sizeof( aCTLAttr[0] ),
                                14 );
            SvtCJKOptions aCJKOpt;
            if( aCJKOpt.IsAnyEnabled() )
                aArr.Insert( 	aCJKAttr,
                                sizeof( aCJKAttr ) / sizeof( aCJKAttr[0] ),
                                14 );

            SfxItemSet aSet( pWrtShell->GetAttrPool(), aArr.GetData() );
            USHORT nWhich = SID_SEARCH_SEARCHSET;

            if ( FID_SEARCH_REPLACESET == nSlot )
            {
                nWhich = SID_SEARCH_REPLACESET;

                if ( pReplList )
                {
                    pReplList->Get( aSet );
                    DELETEZ( pReplList );
                }
            }
            else if ( pSrchList )
            {
                pSrchList->Get( aSet );
                DELETEZ( pSrchList );
            }
            rReq.SetReturnValue( SvxSetItem( nWhich, aSet ) );
        }
        break;
        default:
#ifndef PRODUCT
            if(nSlot)
            {
                ByteString sStr( "nSlot: " );
                sStr += ByteString::CreateFromInt32( nSlot );
                sStr += " falscher Dispatcher (viewsrch.cxx)";
                DBG_ERROR( sStr.GetBuffer() );
            }
#endif
            return;
    }
}


BOOL SwView::SearchAndWrap(BOOL bApi)
{
    SwSearchOptions aOpts( pWrtShell, pSrchItem->GetBackward() );

        // Startposition der Suche fuer WrapAround merken
        // Start- / EndAction wegen vielleicht bestehender Selektionen
        // aus 'Suche alle'
    pWrtShell->StartAllAction();
    pWrtShell->Push();
        // falls in selektierten Bereichen gesucht werden soll, duerfen sie
        // nicht aufgehoben werden
    if (!pSrchItem->GetSelection())
        (pWrtShell->*pWrtShell->fnKillSel)(0, FALSE);

    SwWait *pWait = new SwWait( *GetDocShell(), TRUE );
    if( FUNC_Search( aOpts ) )
    {
        bFound = TRUE;
        if(pWrtShell->IsSelFrmMode())
        {
            pWrtShell->UnSelectFrm();
            pWrtShell->LeaveSelFrmMode();
        }
        pWrtShell->Pop();
        pWrtShell->EndAllAction();
        delete pWait;
        return TRUE;
    }
    delete pWait, pWait = 0;

        // Suchen in den Sonderbereichen, wenn keine
        // Suche in Selektionen vorliegt. Bei Suche in Selektionen
        // wird ohnehin in diesen Sonderbereichen gesucht
    BOOL bHasSrchInOther = bExtra;
    if (!pSrchItem->GetSelection() && !bExtra )
    {
        bExtra = TRUE;
        if( FUNC_Search( aOpts ) )
        {
            bFound = TRUE;
            pWrtShell->Pop();
            pWrtShell->EndAllAction();
            return TRUE;
        }
        bExtra = FALSE;
    }
    else
        bExtra = !bExtra;

    const USHORT nId = SvxSearchDialogWrapper::GetChildWindowId();
    SvxSearchDialogWrapper *pWrp = (SvxSearchDialogWrapper*)GetViewFrame()->GetChildWindow(nId);
    pSrchDlg = pWrp ? (SvxSearchDialog*)(pWrp->GetWindow()) : 0;

        // falls Startposition am Dokumentende / -anfang
    if (aOpts.bDontWrap)
    {
        pWrtShell->EndAllAction();
        if( !bApi )
        {
            Window* pParentWindow = GetParentWindow( pSrchDlg );
            InfoBox( pParentWindow, SW_RES(MSG_NOT_FOUND)).Execute();
        }
        bFound = FALSE;
        pWrtShell->Pop();
        return FALSE;
    }
    pWrtShell->EndAllAction();
        // noch mal mit WrapAround versuchen?

    if( bApi || RET_NO == QueryBox( GetParentWindow( pSrchDlg ),
                                        SW_RES( DOCPOS_START == aOpts.eEnd
                                            ? MSG_SEARCH_START
                                            : MSG_SEARCH_END )
                                    ).Execute() )
    {
        bFound = FALSE;
        pWrtShell->Pop();
        return FALSE;
    }
    pWrtShell->StartAllAction();
    pWrtShell->Pop(FALSE);
    pWait = new SwWait( *GetDocShell(), TRUE );

    BOOL bSrchBkwrd = DOCPOS_START == aOpts.eEnd;

    aOpts.eEnd =  bSrchBkwrd ? DOCPOS_START : DOCPOS_END;
    aOpts.eStart = bSrchBkwrd ? DOCPOS_END : DOCPOS_START;

    if (bHasSrchInOther)
    {
        pWrtShell->ClearMark();
        if (bSrchBkwrd)
            pWrtShell->EndDoc();
        else
            pWrtShell->SttDoc();
    }

    bFound = 0 != FUNC_Search( aOpts );
    pWrtShell->EndAllAction();
    delete pWait;
    if ( bFound )
        return bFound;
    if(!bApi)
    {
        Window* pParentWindow = GetParentWindow( pSrchDlg );
        InfoBox( pParentWindow, SW_RES(MSG_NOT_FOUND)).Execute();
    }
    return bFound = FALSE;
}


BOOL SwView::SearchAll(USHORT* pFound)
{
    SwWait aWait( *GetDocShell(), TRUE );
    pWrtShell->StartAllAction();

    SwSearchOptions aOpts( pWrtShell, pSrchItem->GetBackward() );

    if (!pSrchItem->GetSelection())
    {
        // bestehende Selektionen aufheben,
        // wenn nicht in selektierten Bereichen gesucht werden soll
        (pWrtShell->*pWrtShell->fnKillSel)(0, FALSE);

        if( DOCPOS_START == aOpts.eEnd )
            pWrtShell->EndDoc();
        else
            pWrtShell->SttDoc();
    }
    bExtra = FALSE;
    USHORT nFound = (USHORT)FUNC_Search( aOpts );
    if(pFound)
        *pFound = nFound;
    bFound = 0 != nFound;

    pWrtShell->EndAllAction();
    return bFound;
}


void SwView::Replace()
{
    SwWait aWait( *GetDocShell(), TRUE );

    pWrtShell->StartAllAction();

    if( pSrchItem->GetPattern() ) // Vorlagen?
    {
        pWrtShell->StartUndo(UIUNDO_REPLACE); // #111827#

        pWrtShell->SetTxtFmtColl( pWrtShell->GetParaStyle(
                            pSrchItem->GetReplaceString(),
                            SwWrtShell::GETSTYLE_CREATESOME ));

        pWrtShell->EndUndo(UIUNDO_REPLACE); // #111827#
    }
    else
    {
        pWrtShell->SwEditShell::Replace( pSrchItem->GetReplaceString(),
                                            pSrchItem->GetRegExp());

        if( pReplList && pReplList->Count() && pWrtShell->HasSelection() )
        {
            SfxItemSet aReplSet( pWrtShell->GetAttrPool(),
                                    aTxtFmtCollSetRange );
            if( pReplList->Get( aReplSet ).Count() )
            {
                ::SfxToSwPageDescAttr( *pWrtShell, aReplSet );
                pWrtShell->SwEditShell::SetAttr( aReplSet );
            }
        }
    }

    pWrtShell->EndAllAction();
}



SwSearchOptions::SwSearchOptions( SwWrtShell* pSh, BOOL bBackward )
{
    eStart = DOCPOS_CURR;
    if( bBackward )
    {
        eEnd = DOCPOS_START;
        bDontWrap = pSh->IsEndOfDoc();
    }
    else
    {
        eEnd = DOCPOS_END;
        bDontWrap = pSh->IsStartOfDoc();
    }
}


ULONG SwView::FUNC_Search( const SwSearchOptions& rOptions )
{
    BOOL bDoReplace = pSrchItem->GetCommand() == SVX_SEARCHCMD_REPLACE ||
                      pSrchItem->GetCommand() == SVX_SEARCHCMD_REPLACE_ALL;

    int eRanges = pSrchItem->GetSelection() ?
        FND_IN_SEL : bExtra ? FND_IN_OTHER : FND_IN_BODY;
    if (pSrchItem->GetCommand() == SVX_SEARCHCMD_FIND_ALL    ||
        pSrchItem->GetCommand() == SVX_SEARCHCMD_REPLACE_ALL)
        eRanges |= FND_IN_SELALL;

    pWrtShell->SttSelect();

    static USHORT __READONLY_DATA aSearchAttrRange[] = {
        RES_FRMATR_BEGIN, RES_FRMATR_END-1,
        RES_CHRATR_BEGIN, RES_CHRATR_END-1,
        RES_PARATR_BEGIN, RES_PARATR_END-1,
        SID_ATTR_PARA_MODEL, SID_ATTR_PARA_KEEP,
        0 };

    SfxItemSet aSrchSet( pWrtShell->GetAttrPool(), aSearchAttrRange);
    if( pSrchList && pSrchList->Count() )
    {
        pSrchList->Get( aSrchSet );

        /*  -- Seitenumbruch mit Seitenvorlage */
        ::SfxToSwPageDescAttr( *pWrtShell, aSrchSet );
    }

    SfxItemSet* pReplSet = 0;
    if( bDoReplace && pReplList && pReplList->Count() )
    {
        pReplSet = new SfxItemSet( pWrtShell->GetAttrPool(),
                                        aSearchAttrRange );
        pReplList->Get( *pReplSet );

        /*  -- Seitenumbruch mit Seitenvorlage */
        ::SfxToSwPageDescAttr( *pWrtShell, *pReplSet );

        if( !pReplSet->Count() )		// schade, die Attribute
            DELETEZ( pReplSet );		// kennen wir nicht
    }

    //
    // build SearchOptions to be used
    //
    SearchOptions aSearchOpt( pSrchItem->GetSearchOptions() );
    aSearchOpt.Locale = CreateLocale( (USHORT)GetAppLanguage() );
    if( !bDoReplace )
        aSearchOpt.replaceString = aEmptyStr;

    ULONG nFound;
    if( aSrchSet.Count() || ( pReplSet && pReplSet->Count() ))
    {
        nFound = pWrtShell->SearchAttr(
            aSrchSet,
            !pSrchItem->GetPattern(),
            rOptions.eStart,
            rOptions.eEnd,
            FindRanges(eRanges),
            pSrchItem->GetSearchString().Len() ? &aSearchOpt : 0,
            pReplSet );
    }
    else if( pSrchItem->GetPattern() )
    {
        // Suchen (und ersetzen) von Vorlagen
        const String sRplStr( pSrchItem->GetReplaceString() );
        nFound = pWrtShell->SearchTempl( pSrchItem->GetSearchString(),
            rOptions.eStart,
            rOptions.eEnd,
            FindRanges(eRanges),
            bDoReplace ? &sRplStr : 0 );
    }
    else
    {
        // Normale Suche
        nFound = pWrtShell->SearchPattern(aSearchOpt,
                                          rOptions.eStart,
                                          rOptions.eEnd,
                                          FindRanges(eRanges),
                                          bDoReplace );
    }
    pWrtShell->EndSelect();
    return nFound;
}


Dialog* SwView::GetSearchDialog()
{
    const USHORT nId = SvxSearchDialogWrapper::GetChildWindowId();
    SvxSearchDialogWrapper *pWrp = (SvxSearchDialogWrapper*)SfxViewFrame::Current()->GetChildWindow(nId);
    if ( pWrp )
        pSrchDlg = (SvxSearchDialog*)(pWrp->GetWindow());
    else
        pSrchDlg = 0;
    return pSrchDlg;
}


void SwView::StateSearch(SfxItemSet &rSet)
{
    SfxWhichIter aIter(rSet);
    USHORT nWhich = aIter.FirstWhich();
    SearchAttrItemList* pSrchRepList = pSrchList;

    while(nWhich)
    {
        switch(nWhich)
        {
            case SID_SEARCH_OPTIONS:
            {
                UINT16 nOpt = 0xFFFF;
                if( GetDocShell()->IsReadOnly() )
                    nOpt &= ~( SEARCH_OPTIONS_REPLACE |
                               SEARCH_OPTIONS_REPLACE_ALL );
                rSet.Put( SfxUInt16Item( SID_SEARCH_OPTIONS, nOpt));
            }
            break;
            case SID_SEARCH_ITEM:
            {
                if ( !pSrchItem )
                {
                    pSrchItem = new SvxSearchItem( SID_SEARCH_ITEM );
                    pSrchItem->SetFamily(SFX_STYLE_FAMILY_PARA);
                    pSrchItem->SetSearchString( pWrtShell->GetSelTxt() );
                }

                if( bJustOpened && pWrtShell->IsSelection() )
                {
                    String aTxt;
                    if( 1 == pWrtShell->GetCrsrCnt() &&
                        ( aTxt = pWrtShell->SwCrsrShell::GetSelTxt() ).Len() )
                    {
                        pSrchItem->SetSearchString( aTxt );
                        pSrchItem->SetSelection( FALSE );
                    }
                    else
                        pSrchItem->SetSelection( TRUE );
                }

                bJustOpened = FALSE;
                rSet.Put( *pSrchItem );
            }
            break;

/*			case SID_SEARCH_REPLACESET:
            case SID_SEARCH_SEARCHSET:
            {
                static USHORT __READONLY_DATA aSearchAttrRange[] =
                {
                        RES_CHRATR_CASEMAP,		RES_CHRATR_POSTURE,
                        RES_CHRATR_SHADOWED, 	RES_CHRATR_WORDLINEMODE,
                        RES_PARATR_LINESPACING, RES_PARATR_HYPHENZONE,
                        RES_LR_SPACE, 			RES_UL_SPACE,
                        SID_ATTR_PARA_MODEL, 	SID_ATTR_PARA_KEEP,
                        0
                };

                SfxItemSet aSet(pWrtShell->GetAttrPool(), aSearchAttrRange );
                if( SID_SEARCH_REPLACESET==nWhich )
                {
                    if( pReplList )
                    {
                        pReplList->Get( aSet );
                        DELETEZ( pReplList );
                    }
                }
                else if( pSrchList )
                {
                    pSrchList->Get( aSet );
                    DELETEZ( pSrchList );
                }
                rSet.Put( SvxSetItem( nWhich, aSet ));
            }
            break;
*/
        }
        nWhich = aIter.NextWhich();
    }
}



