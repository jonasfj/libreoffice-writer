/*************************************************************************
 *
 *  $RCSfile: edtdd.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: os $ $Date: 2002-11-01 10:19:56 $
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

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif


#ifndef _SVDVIEW_HXX //autogen
#include <svx/svdview.hxx>
#endif
#ifndef _OUTLINER_HXX //autogen
#include <svx/outliner.hxx>
#endif
#ifndef _SVDVMARK_HXX //autogen
#include <svx/svdvmark.hxx>
#endif
#ifndef _SVDOBJ_HXX //autogen
#include <svx/svdobj.hxx>
#endif
#ifndef _SOT_EXCHANGE_HXX //autogen
#include <sot/exchange.hxx>
#endif
#ifndef _OFF_APP_HXX //autogen
#include <offmgr/app.hxx>
#endif
#ifndef _SOT_FORMATS_HXX //autogen
#include <sot/formats.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX
#include <sfx2/bindings.hxx>
#endif


#ifndef _FMTURL_HXX //autogen
#include <fmturl.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _SWDTFLVR_HXX
#include <swdtflvr.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _SWWDOCSH_HXX //autogen
#include <wdocsh.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif


extern BOOL bNoInterrupt;
extern BOOL bFrmDrag;
extern BOOL bDDTimerStarted;

BOOL bExecuteDrag = FALSE;

void SwEditWin::StartDDTimer()
{
    aTimer.SetTimeoutHdl(LINK(this, SwEditWin, DDHandler));
    aTimer.SetTimeout(480);
    aTimer.Start();
    bDDTimerStarted = TRUE;
}


void SwEditWin::StopDDTimer(SwWrtShell *pSh, const Point &rPt)
{
    aTimer.Stop();
    bDDTimerStarted = FALSE;
    if(!pSh->IsSelFrmMode())
        (pSh->*pSh->fnSetCrsr)(&rPt,FALSE);
    aTimer.SetTimeoutHdl(LINK(this,SwEditWin, TimerHandler));
}

void SwEditWin::StartDrag( sal_Int8 nAction, const Point& rPosPixel )
{
    SwWrtShell &rSh = rView.GetWrtShell();
    if( rSh.GetDrawView() )
    {
        CommandEvent aDragEvent( rPosPixel, COMMAND_STARTDRAG, TRUE );
        if( rSh.GetDrawView()->Command( aDragEvent, this ) )
        {
            rView.GetViewFrame()->GetBindings().InvalidateAll(FALSE);
            return; // Event von der SdrView ausgewertet
        }
    }

    if ( !pApplyTempl && !rSh.IsDrawCreate() && !IsDrawAction())
    {
        BOOL bStart = FALSE, bDelSelect = FALSE;
        SdrObject *pObj = NULL;
        Point aDocPos( PixelToLogic( rPosPixel ) );
        if ( !rSh.IsInSelect() && rSh.ChgCurrPam( aDocPos, TRUE, TRUE))
            //Wir sind nicht beim Selektieren und stehen auf einer
            //Selektion
            bStart = TRUE;
        else if ( !bFrmDrag && rSh.IsSelFrmMode() &&
                    rSh.IsInsideSelectedObj( aDocPos ) )
        {
            //Wir sind nicht am internen Draggen und stehen auf
            //einem Objekt (Rahmen, Zeichenobjekt)

            bStart = TRUE;
        }
        else if( !bFrmDrag && rView.GetDocShell()->IsReadOnly() &&
                OBJCNT_NONE != rSh.GetObjCntType( aDocPos, pObj ))
        {
            rSh.LockPaint();
            if( rSh.SelectObj( aDocPos, 0, pObj ))
                bStart = bDelSelect = TRUE;
            else
                rSh.UnlockPaint();
        }
        else
        {
            SwContentAtPos aSwContentAtPos( SwContentAtPos::SW_INETATTR );
            bStart = rSh.GetContentAtPos( aDocPos,
                        aSwContentAtPos,
                        FALSE );
        }

        if ( bStart && !bIsInDrag )
        {
            bMBPressed = FALSE;
            ReleaseMouse();
            bFrmDrag = FALSE;
            bExecuteDrag = TRUE;
            SwEditWin::nDDStartPosY = aDocPos.Y();
            SwEditWin::nDDStartPosX = aDocPos.X();
            aMovePos = aDocPos;
            StartExecuteDrag();
            if( bDelSelect )
            {
                rSh.UnSelectFrm();
                rSh.UnlockPaint();
            }
        }
    }
}

void SwEditWin::StartExecuteDrag()
{
    if( !bExecuteDrag || bIsInDrag )
        return;

    bIsInDrag = TRUE;

    SwTransferable* pTransfer = new SwTransferable( rView.GetWrtShell() );
    ::com::sun::star::uno::Reference<
        ::com::sun::star::datatransfer::XTransferable > xRef( pTransfer );

    pTransfer->StartDrag( this, aMovePos );
}

void SwEditWin::DragFinished()
{
    DropCleanup();
    aTimer.SetTimeoutHdl( LINK(this,SwEditWin, TimerHandler) );
    bIsInDrag = FALSE;
}


void SwEditWin::DropCleanup()
{
    SwWrtShell &rSh =  rView.GetWrtShell();

    // Stati zuruecksetzen
    bNoInterrupt = FALSE;
    if ( bOldIdleSet )
    {
        ((SwViewOption*)rSh.GetViewOptions())->SetIdle( bOldIdle );
        bOldIdleSet = FALSE;
    }
    if ( pUserMarker )
        CleanupDropUserMarker();
    else
        rSh.UnSetVisCrsr();

}

void SwEditWin::CleanupDropUserMarker()
{
    if ( pUserMarker )
    {
        delete pUserMarker, pUserMarker = 0;
        pUserMarkerObj = 0;
    }
}


//Messehack (MA,MBA)
void lcl_SelectShellForDrop( SwView &rView )
{
    if ( !rView.GetCurShell() )
        rView.SelectShell();
}

sal_Int8 SwEditWin::ExecuteDrop( const ExecuteDropEvent& rEvt )
{
    ::lcl_SelectShellForDrop( GetView() );
    DropCleanup();
    sal_Int8 nRet = DND_ACTION_NONE;

    //Ein Drop auf eine offene OutlinerView geht uns nichts an (siehe auch QueryDrop)
    SwWrtShell &rSh = rView.GetWrtShell();
    const Point aDocPt( PixelToLogic( rEvt.maPosPixel ));
    SdrObject *pObj = 0;
    OutlinerView* pOLV;
    rSh.GetObjCntType( aDocPt, pObj );

    if( pObj && 0 != ( pOLV = rSh.GetDrawView()->GetTextEditOutlinerView() ))
    {
        Rectangle aRect( pOLV->GetOutputArea() );
        aRect.Union( pObj->GetLogicRect() );
        const Point aPos = pOLV->GetWindow()->PixelToLogic(rEvt.maPosPixel);
        if ( aRect.IsInside(aPos) )
        {
            rSh.StartAllAction();
//!!			sal_Int8 nRet = DND_ACTION_NONE/*pOLV->ExecuteDrop( rEvt )*/;
            rSh.EndAllAction();
            return nRet;
        }
    }


    // dvo 2002-05-27, #99027#: There's a special treatment for file lists with a single
    //                          element, that depends on the actual content of the 
    //                          Transferable to be accessible. Since the transferable
    //                          may only be accessed after the drop has been accepted
    //                          (according to KA due to Java D&D), we'll have to 
    //                          reevaluate the drop action once more _with_ the
    //                          Transferable.
    USHORT nEventAction;
    sal_Int8 nUserOpt = rEvt.mbDefault ? EXCHG_IN_ACTION_DEFAULT
                                       : rEvt.mnAction;
    nDropAction = SotExchange::GetExchangeAction(
                                GetDataFlavorExVector(),
                                nDropDestination,
                                rEvt.mnAction,
//!!								rEvt.GetSourceOptions(),
                                nUserOpt, nDropFormat, nEventAction, 0,
                                &rEvt.maDropEvent.Transferable );


    TransferableDataHelper aData( rEvt.maDropEvent.Transferable );
    nRet = rEvt.mnAction;
    if( !SwTransferable::PasteData( aData, rSh, nDropAction, nDropFormat,
                                nDropDestination, FALSE, rEvt.mbDefault, &aDocPt, nRet))
//!!	nRet = OFF_APP()->ExecuteDrop( rEvt );
        nRet = DND_ACTION_NONE;
    else if ( SW_MOD()->pDragDrop )
        //Bei internem D&D nicht mehr aufraeumen!
        SW_MOD()->pDragDrop->SetCleanUp( FALSE );

    return nRet;
}


USHORT SwEditWin::GetDropDestination( const Point& rPixPnt, SdrObject ** ppObj )
{
    SwWrtShell &rSh = rView.GetWrtShell();
    const Point aDocPt( PixelToLogic( rPixPnt ) );
    if( rSh.ChgCurrPam( aDocPt ) || rSh.IsOverReadOnlyPos( aDocPt ) )
        return 0;

    SdrObject *pObj = NULL;
    const ObjCntType eType = rSh.GetObjCntType( aDocPt, pObj );

    //Drop auf OutlinerView (TextEdit im Drawing) soll diese selbst entscheiden!
    if( pObj )
    {
        OutlinerView* pOLV = rSh.GetDrawView()->GetTextEditOutlinerView();
        if ( pOLV )
        {
            Rectangle aRect( pOLV->GetOutputArea() );
            aRect.Union( pObj->GetLogicRect() );
            const Point aPos = pOLV->GetWindow()->PixelToLogic( rPixPnt );
            if( aRect.IsInside( aPos ) )
                return 0;
        }
    }

    //Auf was wollen wir denn gerade droppen?
    USHORT nDropDestination = 0;

    //Sonst etwas aus der DrawingEngine getroffen?
    if( OBJCNT_NONE != eType )
    {
        switch ( eType )
        {
        case OBJCNT_GRF:
            {
                BOOL bLink,
                    bIMap = 0 != rSh.GetFmtFromObj( aDocPt )->GetURL().GetMap();
                String aDummy;
                rSh.GetGrfAtPos( aDocPt, aDummy, bLink );
                if ( bLink && bIMap )
                    nDropDestination = EXCHG_DEST_DOC_LNKD_GRAPH_W_IMAP;
                else if ( bLink )
                    nDropDestination = EXCHG_DEST_DOC_LNKD_GRAPHOBJ;
                else if ( bIMap )
                    nDropDestination = EXCHG_DEST_DOC_GRAPH_W_IMAP;
                else
                    nDropDestination = EXCHG_DEST_DOC_GRAPHOBJ;
            }
            break;
        case OBJCNT_FLY:
            if( rSh.GetView().GetDocShell()->ISA(SwWebDocShell) )
                nDropDestination = EXCHG_DEST_DOC_TEXTFRAME_WEB;
            else
                nDropDestination = EXCHG_DEST_DOC_TEXTFRAME;
            break;
        case OBJCNT_OLE:		nDropDestination = EXCHG_DEST_DOC_OLEOBJ; break;
        case OBJCNT_CONTROL:	/* no Action avail */
        case OBJCNT_SIMPLE:		nDropDestination = EXCHG_DEST_DOC_DRAWOBJ; break;
        case OBJCNT_URLBUTTON:	nDropDestination = EXCHG_DEST_DOC_URLBUTTON; break;
        case OBJCNT_GROUPOBJ:	nDropDestination = EXCHG_DEST_DOC_GROUPOBJ;		break;

        default: ASSERT( !this, "new ObjectType?" );
        }
    }
    if ( !nDropDestination )
    {
        if( rSh.GetView().GetDocShell()->ISA(SwWebDocShell) )
            nDropDestination = EXCHG_DEST_SWDOC_FREE_AREA_WEB;
        else
            nDropDestination = EXCHG_DEST_SWDOC_FREE_AREA;
    }
    if( ppObj )
        *ppObj = pObj;
    return nDropDestination;
}

sal_Int8 SwEditWin::AcceptDrop( const AcceptDropEvent& rEvt )
{
    if( rEvt.mbLeaving )
    {
        DropCleanup();
        return rEvt.mnAction;
    }

    if( rView.GetDocShell()->IsReadOnly() )
        return DND_ACTION_NONE;

    SwWrtShell &rSh = rView.GetWrtShell();

    //Ein bischen scrollen?
    Point aPixPt( rEvt.maPosPixel );
    Point aPoint;
    Rectangle aWin( aPoint, GetOutputSizePixel() );
    Rectangle aWin2( aWin );
    aWin.Left()	 += 10;
    aWin.Top()	 += 10;
    aWin.Right() -= 10;
    aWin.Bottom()-= 10;

    if ( !aWin.IsInside( aPixPt ) )
    {
        if ( !bOldIdleSet )
        {
            bOldIdle = rSh.GetViewOptions()->IsIdle();
            ((SwViewOption *)rSh.GetViewOptions())->SetIdle(FALSE);
            bOldIdleSet = TRUE;
        }
        CleanupDropUserMarker();

        aWin2.Left()  += 5;
        aWin2.Top()   += 5;
        aWin2.Right() -= 5;
        aWin2.Bottom()-= 5;

        long x = 0;
        if ( aPixPt.Y() < aWin2.Top() )
            x = aPixPt.Y() = -5;
        else if ( aPixPt.Y() > aWin2.Bottom() )
            x = aPixPt.Y() += 10;
        if ( aPixPt.X() < aWin2.Left() )
            x = aPixPt.X() = -5;
        else if ( aPixPt.X() > aWin2.Right() )
            x = aPixPt.X() += 10;
        Point aDocPt( PixelToLogic( aPixPt ) );
        aDocPt = rSh.GetCntntPos( aDocPt, x > 0 );
        rSh.SwCrsrShell::SetVisCrsr( aDocPt );
        return TRUE;
    }

    if ( bOldIdleSet )
    {
        ((SwViewOption *)rSh.GetViewOptions())->SetIdle( bOldIdle );
        bOldIdleSet = FALSE;
    }

    SdrObject *pObj = NULL;
    nDropDestination = GetDropDestination( aPixPt, &pObj );
    if( !nDropDestination )
        return DND_ACTION_NONE;

    USHORT nEventAction;
    sal_Int8 nUserOpt = rEvt.mbDefault ? EXCHG_IN_ACTION_DEFAULT
                                       : rEvt.mnAction;

    nDropAction = SotExchange::GetExchangeAction(
                                GetDataFlavorExVector(),
                                nDropDestination,
                                rEvt.mnAction,
//!!								rEvt.GetSourceOptions(),
                                nUserOpt, nDropFormat, nEventAction );

    if( EXCHG_INOUT_ACTION_NONE != nDropAction )
    {
        const Point aDocPt( PixelToLogic( aPixPt ) );

        //Bei den default Aktionen wollen wir noch ein bischen mitreden.
        SwModule *pMod = SW_MOD();
        if( pMod->pDragDrop )
        {
            BOOL bCleanup = FALSE;
            //Zeichenobjekte in Kopf-/Fusszeilen sind nicht erlaubt

            SwWrtShell *pSrcSh = pMod->pDragDrop->GetShell();
            if( (pSrcSh->GetSelFrmType() == FRMTYPE_DRAWOBJ) &&
                 (rSh.GetFrmType( &aDocPt, FALSE ) & (FRMTYPE_HEADER|FRMTYPE_FOOTER)) )
            {
                bCleanup = TRUE;
            }
            // keine positionsgeschuetzten Objecte verschieben!
            else if( DND_ACTION_MOVE == rEvt.mnAction &&
                     pSrcSh->IsSelObjProtected( FLYPROTECT_POS ) )
            {
                bCleanup = TRUE;
            }
            else if( rEvt.mbDefault )
            {
                // JP 13.08.98: internes Drag&Drop: bei gleichem Doc ein Move
                //				ansonten ein Copy - Task 54974
                nEventAction = pSrcSh->GetDoc() == rSh.GetDoc()
                                    ? DND_ACTION_MOVE
                                    : DND_ACTION_COPY;
            }
            if ( bCleanup )
            {
                CleanupDropUserMarker();
                rSh.UnSetVisCrsr();
                return DND_ACTION_NONE;
            }
        }
        else
        {
            //D&D von ausserhalb des SW soll per default ein Copy sein.
            if( EXCHG_IN_ACTION_DEFAULT == nEventAction &&
                DND_ACTION_MOVE == rEvt.mnAction )
                nEventAction = DND_ACTION_COPY;

            if( (SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE == nDropFormat &&
                 EXCHG_IN_ACTION_LINK == nDropAction) ||
                 SOT_FORMATSTR_ID_SBA_CTRLDATAEXCHANGE == nDropFormat  )
            {
                SdrMarkView* pMView = PTR_CAST( SdrMarkView, rSh.GetDrawView() );
                if( pMView && !pMView->IsDesignMode() )
                    return DND_ACTION_NONE;
            }
            //controls cannot be created for the complete table
            if(EXCHG_IN_ACTION_LINK == nDropAction && 
                SOT_FORMATSTR_ID_SBA_DATAEXCHANGE == nDropFormat)
                return DND_ACTION_NONE;
        }

        if ( EXCHG_IN_ACTION_DEFAULT != nEventAction )
            nUserOpt = nEventAction;

        // show DropCursor or UserMarker ?
        if( EXCHG_DEST_SWDOC_FREE_AREA_WEB == nDropDestination ||
            EXCHG_DEST_SWDOC_FREE_AREA == nDropDestination )
        {
            CleanupDropUserMarker();
            rSh.SwCrsrShell::SetVisCrsr( aDocPt );
        }
        else
        {
            rSh.UnSetVisCrsr();
            if ( !pUserMarker )
                pUserMarker = new SdrViewUserMarker( rSh.GetDrawView() );
            if ( pUserMarkerObj != pObj )
            {
                pUserMarkerObj = pObj;
                pUserMarker->SetXPolyPolygon( pUserMarkerObj,
                    rSh.GetDrawView()->GetPageView(
                            rSh.GetDrawView()->GetModel()->GetPage(0)));
                pUserMarker->Show();
            }
        }
        return nUserOpt;
    }

    CleanupDropUserMarker();
    rSh.UnSetVisCrsr();
//!!	return OFF_APP()->AcceptDrop( rEvt );
    return DND_ACTION_NONE;
}


IMPL_LINK( SwEditWin, DDHandler, Timer *, EMPTYARG )
{
    bDDTimerStarted = FALSE;
    aTimer.Stop();
    aTimer.SetTimeout(240);
    bMBPressed = FALSE;
    ReleaseMouse();
    bFrmDrag = FALSE;

    if ( rView.GetViewFrame() && rView.GetViewFrame()->GetFrame()  &&
         !rView.GetViewFrame()->GetFrame()->TransferForReplaceInProgress() )
    {
        bExecuteDrag = TRUE;
        StartExecuteDrag();
    }
    return 0;
}


/*------------------------------------------------------------------------

    $Log: not supported by cvs2svn $
    Revision 1.12  2002/06/20 11:50:17  os
    #100050# enable inserting of database fields if design mode is switched on
    
    Revision 1.11  2002/05/27 16:12:24  dvo
    #99027# re-evaluate drop action after the drop has been accepted
            this should enable the special treatment of single-element file
            lists as files
    
    Revision 1.10  2002/04/09 14:20:51  ama
    Fix #98156#: Leave the selected frame
    
    Revision 1.9  2001/10/11 17:20:34  jp
    Bug #93007#: StartDrag - ask DrawView if they will handle the DragEvent

    Revision 1.8  2001/09/11 15:10:28  jp
    Task #91678#: 'selection clipbord' implemented

    Revision 1.7  2001/08/13 22:02:14  jp
    Bug #86173#: ask for the default flag on the event

    Revision 1.6  2001/08/09 05:36:54  os
    #89714# default drag and drop of the navigator works again

    Revision 1.5  2001/08/01 10:12:08  jp
    Bug #90411#: DragFinished calls also the DropCleanUp for showing the correct TextCursor

    Revision 1.4  2001/05/07 14:49:00  jp
    use the correct D&D constants

    Revision 1.3  2001/03/23 15:55:45  jp
    use new Drag&Drop / Clipboard API

    Revision 1.2  2001/02/02 17:45:49  jp
    SwDataExchange interfaces renamed

    Revision 1.1.1.1  2000/09/18 17:14:35  hr
    initial import

    Revision 1.174  2000/09/18 16:05:23  willem.vandorp
    OpenOffice header added.

    Revision 1.173  1999/05/11 19:50:40  JP
    Task #66127#: Methoden rund ums Lineal verbessert und Schnittstellen veraendert/erweitert


      Rev 1.172   11 May 1999 21:50:40   JP
   Task #66127#: Methoden rund ums Lineal verbessert und Schnittstellen veraendert/erweitert

      Rev 1.171   25 Jan 1999 16:11:42   JP
   Bug #61109#: QueryDrop - DropModifier nicht ueberbuegeln

      Rev 1.170   19 Jan 1999 22:57:28   JP
   Task #58677#: Crsr in Readonly Bereichen zulassen

      Rev 1.169   11 Dec 1998 11:46:02   JP
   Task #58217#: WebDocShell als D&D Ziel erkennen

      Rev 1.168   27 Nov 1998 14:50:42   AMA
   Fix #59951#59825#: Unterscheiden zwischen Rahmen-,Seiten- und Bereichsspalten

      Rev 1.167   17 Nov 1998 22:17:06   JP
   Task #59398#: ClipboardId Umstellungen

      Rev 1.166   12 Nov 1998 13:31:08   JP
   Task #59398# Umstellung von SotFormatStringId

      Rev 1.165   02 Sep 1998 12:10:46   MA
   #55895# ctrldataexchange

      Rev 1.164   13 Aug 1998 14:20:14   JP
   Bug #54974# QueryDrop - bei DefaultAction per internem Format im eigenen Doc immer move, sonst copy

      Rev 1.163   16 Jul 1998 19:21:22   JP
   Bug #53113# DatenbankControls nur im DesignModus einfuegen

      Rev 1.162   08 Jul 1998 14:17:32   JP
   das richtige define fuer supd benutzen

      Rev 1.161   08 Jul 1998 12:24:36   JP
   neues Drag&Drop ab Version 5.0

      Rev 1.160   04 Jul 1998 16:06:24   JP
   Umstellung auf Tabelle weiergemacht

      Rev 1.159   27 Jun 1998 18:44:00   JP
   Umstellung auf Tabelle weitergemacht

      Rev 1.158   25 Jun 1998 14:12:50   JP
   SvDataObject -> SotObject

      Rev 1.157   16 Jun 1998 17:14:42   OM
   #51085# Draw-Objekte nicht auf die Wiese droppen

      Rev 1.156   16 Jun 1998 16:35:44   OM
   #51085# Draw-Objekte nicht auf die Wiese droppen

      Rev 1.155   09 Jun 1998 15:31:32   OM
   VC-Controls entfernt

      Rev 1.154   27 Apr 1998 21:15:26   JP
   Bug #49842#: Drop - bei Grafiken den FileNamen in eine URL wandeln

      Rev 1.153   31 Mar 1998 17:30:22   MA
   erstmal abgebrochen, neues DD

      Rev 1.152   26 Mar 1998 18:02:54   MA
   ein paar vorb. D&D

      Rev 1.151   20 Feb 1998 16:18:20   MA
   headerfiles gewandert

      Rev 1.150   19 Feb 1998 08:53:20   OK
   NEW: include mieclip.hxx

      Rev 1.149   07 Jan 1998 18:46:26   MIB
   5.0 Fileformat

      Rev 1.148   28 Nov 1997 18:02:26   MA
   includes

      Rev 1.147   25 Nov 1997 10:33:00   MA
   includes

      Rev 1.146   22 Oct 1997 16:04:52   OM
   Farbendrop

      Rev 1.145   17 Oct 1997 13:24:02   JP
   neu: MSE40-HTML-Format erkennen/lesen

      Rev 1.144   15 Oct 1997 14:03:44   OS
   lcl_SelectShellForDrop - SelectShell() nur bei Bedarf rufen #44690#

      Rev 1.143   17 Sep 1997 11:26:04   MA
   #43801# precedence nicht beachtet

      Rev 1.142   17 Sep 1997 09:57:22   OS
   Nummer 3: pSdrView pruefen!

      Rev 1.141   10 Sep 1997 10:50:08   JP
   neu: Undo fuers kopieren von Tabellen & in Tabellen

      Rev 1.140   08 Sep 1997 11:02:36   MA
   includes

      Rev 1.139   05 Sep 1997 13:10:48   OM
   #42027# D&D auf DrawText-Objekte

      Rev 1.138   02 Sep 1997 13:20:48   OS
   includes

      Rev 1.137   01 Sep 1997 13:15:38   OS
   DLL-Umstellung

      Rev 1.136   28 Aug 1997 10:29:56   JP
   Bug #42430#: Drop - bei INetAttr. den SourceCursor auf das Attribut setzen

      Rev 1.135   15 Aug 1997 12:09:54   OS
   chartar/frmatr/txtatr aufgeteilt

      Rev 1.134   11 Aug 1997 20:06:16   HJS
   includes

      Rev 1.133   11 Aug 1997 10:08:40   MH
   chg: header

      Rev 1.132   17 Jul 1997 19:06:32   MA
   #41801# File zur absoluten URL verarbeiten

      Rev 1.131   19 Jun 1997 11:38:52   MA
   #40833# Drop_Link, sdw nicht einfuegen bei sw/web

      Rev 1.130   09 Jun 1997 20:08:48   JP
   SwFilterNms/SwFilterNm entfernt

      Rev 1.129   29 May 1997 14:31:14   MA
   fix: Grafik nur verknuepft fuer WebDocShell

      Rev 1.128   16 May 1997 15:07:42   MH
   add: defines wg. CLOOKS

      Rev 1.127   25 Apr 1997 13:00:46   MA
   #39306# ggf. den UserMarker loeschen

      Rev 1.126   14 Apr 1997 17:25:00   MH
   add: header

      Rev 1.125   10 Apr 1997 16:05:18   TRI
   includes

      Rev 1.124   08 Apr 1997 10:31:46   OM
   Fehlende Includes

      Rev 1.123   08 Apr 1997 09:55:34   NF
   includes...

      Rev 1.122   21 Mar 1997 13:53:18   MA
   #36900# D&D offset beseitigt

      Rev 1.121   16 Mar 1997 13:44:16   MA
   fix: Kein D&D wenn der Frame schon klinisch tot ist

      Rev 1.120   07 Mar 1997 09:47:24   MA
   Neue D&D Moeglichkeiten, Handling verbessert

      Rev 1.119   23 Feb 1997 22:03:10   MA
   fix: Drop von Grafik auf Grafik reanimiert

      Rev 1.118   07 Feb 1997 12:05:44   OS
   Navigator benutzt eigenes Drag-Format

      Rev 1.117   05 Feb 1997 19:13:18   JP
   ExcuteDrag: Modus der WrtShell setzen, wenn Flys selektiert sind

      Rev 1.116   19 Dec 1996 16:12:18   MA
   fix: Crsr wieder umschalten

      Rev 1.115   28 Nov 1996 14:20:06   MA
   fix: Drop per Link auch bei einem File

      Rev 1.114   20 Nov 1996 16:58:42   MA
   #33444# QueryDrop, Pos fuer Objekte anpassen

      Rev 1.113   18 Oct 1996 16:57:56   MA
   Messehack: eine hoffentlich kurzlebige Freundschaft

      Rev 1.112   15 Oct 1996 12:14:20   MA
   new: Drawing per Clipboard

      Rev 1.111   02 Oct 1996 13:50:52   JP
   StartExecuteDrag: aus ReadOnly-Docs darf nur kopiert werden!

      Rev 1.110   16 Sep 1996 14:00:58   JP
   Drop: INetBookmark am URL-Button setzen

-------------------------------------------------------------------------*/

