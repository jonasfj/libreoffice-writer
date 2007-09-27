/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: textfld.cxx,v $
 *
 *  $Revision: 1.34 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:30:01 $
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


#ifndef _HINTIDS_HXX
#include <hintids.hxx>	//_immer_ vor den solar-Items
#endif

#include <sfx2/lnkbase.hxx>

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX //autogen
#include <svtools/itempool.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXMACITEM_HXX //autogen
#include <svtools/macitem.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVX_POSTATTR_HXX //autogen
#include <svx/postattr.hxx>
#endif
#ifndef _SVX_HLNKITEM_HXX //autogen
#include <svx/hlnkitem.hxx>
#endif

#include <svx/svxdlg.hxx>

#ifndef _SVXLINKMGR_HXX
#include <svx/linkmgr.hxx>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif

#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif
#ifndef _FLDWRAP_HXX //autogen
#include <fldwrap.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _BASESH_HXX
#include <basesh.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _FLDDAT_HXX
#include <flddat.hxx>
#endif
#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
#ifndef _TEXTSH_HXX
#include <textsh.hxx>
#endif
#ifndef _DOCUFLD_HXX
#include <docufld.hxx>
#endif
#ifndef _USRFLD_HXX
#include <usrfld.hxx>
#endif
#ifndef _DDEFLD_HXX
#include <ddefld.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>
#endif
#ifndef _FLDMGR_HXX
#include <fldmgr.hxx>
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _SHELLS_HRC
#include <shells.hrc>
#endif

#include <sfx2/app.hxx>
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
#include "swabstdlg.hxx"
#include "dialog.hrc"
#include <fldui.hrc>

using namespace nsSwDocInfoSubType;


extern BOOL bNoInterrupt;		// in mainwn.cxx

String& lcl_AppendRedlineStr( String& rStr, USHORT nRedlId )
{
    USHORT nResId = 0;
    switch( nRedlId )
    {
    case nsRedlineType_t::REDLINE_INSERT:	nResId = STR_REDLINE_INSERTED; 		break;
    case nsRedlineType_t::REDLINE_DELETE:	nResId = STR_REDLINE_DELETED;		break;
    case nsRedlineType_t::REDLINE_FORMAT:	nResId = STR_REDLINE_FORMATED;		break;
    case nsRedlineType_t::REDLINE_TABLE:		nResId = STR_REDLINE_TABLECHG;		break;
    case nsRedlineType_t::REDLINE_FMTCOLL:	nResId = STR_REDLINE_FMTCOLLSET;	break;
    }
    if( nResId )
        rStr += SW_RESSTR( nResId );
    return rStr;
}

// STATIC DATA -----------------------------------------------------------

void SwTextShell::ExecField(SfxRequest &rReq)
{
    SwWrtShell& rSh = GetShell();
    const SfxPoolItem* pItem = 0;

    USHORT nSlot = rReq.GetSlot();
    const SfxItemSet* pArgs = rReq.GetArgs();
    if(pArgs)
        pArgs->GetItemState(GetPool().GetWhich(nSlot), FALSE, &pItem);

    Window *pMDI = &GetView().GetViewFrame()->GetWindow();
    BOOL bMore = FALSE;
    BOOL bIsText = TRUE;
    USHORT nInsertType = 0;
    USHORT nInsertSubType = 0;
    ULONG nInsertFormat = 0;

    switch(nSlot)
    {
        case FN_EDIT_FIELD:
        {
            SwField* pFld = rSh.GetCurFld();
            if( pFld )
            {
                switch ( pFld->GetTypeId() )
                {
                    case TYP_DDEFLD:
                    {
#if !defined(DDE_AVAILABLE)
                        return;
#endif

                        ::sfx2::SvBaseLink& rLink = ((SwDDEFieldType*)pFld->GetTyp())->
                                                GetBaseLink();
                        if(rLink.IsVisible())
                        {
                            SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                            SfxAbstractLinksDialog* pDlg = pFact->CreateLinksDialog( pMDI, &rSh.GetLinkManager(), FALSE, &rLink );
                            if ( pDlg )
                            {
                                pDlg->Execute();
                                delete pDlg;
                            }
                        }
                        break;
                    }
                    default:
                    {
                        SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                        DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

                        AbstractSfxSingleTabDialog* pDlg = pFact->CreateSwFldEditDlg( GetView(),RC_DLG_SWFLDEDITDLG );
                        DBG_ASSERT(pDlg, "Dialogdiet fail!");
                        pDlg->Execute();
                        delete pDlg;
                    }
                }
            }
            break;
        }
        case FN_EXECUTE_MACROFIELD:
        {
            SwField* pFld = rSh.GetCurFld();
            if(pFld && pFld->GetTyp()->Which() == RES_MACROFLD)
            {

                const String& rMacro = ((SwMacroField*)pFld)->GetMacro();
                USHORT nPos = rMacro.Search('.', 0);
                if(nPos != STRING_NOTFOUND)
                {
                    SvxMacro aMacro( rMacro.Copy(nPos + 1), rMacro.Copy(0,nPos), STARBASIC );
                    rSh.ExecMacro(aMacro);
                }
            }
        }
        break;

        case FN_GOTO_NEXT_INPUTFLD:
        case FN_GOTO_PREV_INPUTFLD:
            {
                BOOL bRet = FALSE;
                SwFieldType* pFld = rSh.GetFldType( 0, RES_INPUTFLD );
                if( pFld && rSh.MoveFldType( pFld,
                            FN_GOTO_NEXT_INPUTFLD == nSlot ))
                {
                    rSh.ClearMark();
                    rSh.StartInputFldDlg( rSh.GetCurFld(), FALSE );
                    bRet = TRUE;
                }

                rReq.SetReturnValue( SfxBoolItem( nSlot, bRet ));
            }
            break;

        default:
            bMore = TRUE;
    }
    if(bMore)
    {
        // hier kommen die Slots mit FldMgr
        SwFldMgr aFldMgr(GetShellPtr());
        switch(nSlot)
        {
            case FN_INSERT_DBFIELD:
            {
                BOOL bRes = FALSE;
                if( pItem )
                {
                    ULONG  nFormat = 0;
                    USHORT nType = 0;
                    String aPar1 = ((SfxStringItem *)pItem)->GetValue();
                    String aPar2;
                    sal_Int32 nCommand = 0;

                    if( SFX_ITEM_SET == pArgs->GetItemState( FN_PARAM_FIELD_TYPE,
                                                                FALSE, &pItem ))
                        nType = ((SfxUInt16Item *)pItem)->GetValue();
                    aPar1 += DB_DELIM;
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_1, FALSE, &pItem ))
                    {
                        aPar1 += ((SfxStringItem *)pItem)->GetValue();
                    }
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_3, FALSE, &pItem ))
                        nCommand = ((SfxInt32Item*)pItem)->GetValue();
                    aPar1 += DB_DELIM;
                    aPar1 += String::CreateFromInt32(nCommand);
                    aPar1 += DB_DELIM;
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_2, FALSE, &pItem ))
                    {
                        aPar1 += ((SfxStringItem *)pItem)->GetValue();
                    }
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_FIELD_CONTENT, FALSE, &pItem ))
                        aPar2 = ((SfxStringItem *)pItem)->GetValue();
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_FIELD_FORMAT, FALSE, &pItem ))
                        nFormat = ((SfxUInt32Item *)pItem)->GetValue();
                    DBG_WARNING("Command is not yet used")
                    sal_Unicode cSeparator = ' ';
                    SwInsertFld_Data aData(nType, 0, aPar1, aPar2, nFormat, GetShellPtr(), cSeparator );
                    bRes = aFldMgr.InsertFld(aData);
                }
                rReq.SetReturnValue(SfxBoolItem( nSlot, bRes ));
            }
            break;
            case FN_INSERT_FIELD_CTRL:
            case FN_INSERT_FIELD:
            {
                BOOL bRes = FALSE;
                if( pItem && nSlot != FN_INSERT_FIELD_CTRL)
                {
                    ULONG  nFormat = 0;
                    USHORT nType = 0;
                    USHORT nSubType = 0;
                    String aPar1 = ((SfxStringItem *)pItem)->GetValue();
                    String aPar2;
                    sal_Unicode cSeparator = ' ';

                    if( SFX_ITEM_SET == pArgs->GetItemState( FN_PARAM_FIELD_TYPE,
                                                                FALSE, &pItem ))
                        nType = ((SfxUInt16Item *)pItem)->GetValue();
                    if( SFX_ITEM_SET == pArgs->GetItemState( FN_PARAM_FIELD_SUBTYPE,
                                                                FALSE, &pItem ))
                        nSubType = ((SfxUInt16Item *)pItem)->GetValue();
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_FIELD_CONTENT, FALSE, &pItem ))
                        aPar2 = ((SfxStringItem *)pItem)->GetValue();
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_FIELD_FORMAT, FALSE, &pItem ))
                        nFormat = ((SfxUInt32Item *)pItem)->GetValue();
                    if( SFX_ITEM_SET == pArgs->GetItemState(
                                        FN_PARAM_3, FALSE, &pItem ))
                    {
                        String sTmp = ((SfxStringItem *)pItem)->GetValue();
                        if(sTmp.Len())
                            cSeparator = sTmp.GetChar(0);
                    }
                    SwInsertFld_Data aData(nType, nSubType, aPar1, aPar2, nFormat, GetShellPtr(), cSeparator );
                    bRes = aFldMgr.InsertFld( aData );
                }
                else
                        //#i5788# prevent closing of the field dialog while a modal dialog ( Input field dialog ) is active
                        if(!GetView().GetViewFrame()->IsInModalMode())
                {
                    SfxViewFrame* pVFrame = GetView().GetViewFrame();
                    pVFrame->ToggleChildWindow(FN_INSERT_FIELD);
                    bRes = pVFrame->GetChildWindow( nSlot ) != 0;
                    Invalidate(rReq.GetSlot());
                    Invalidate(FN_INSERT_FIELD_CTRL);
                    rReq.Ignore();
                }
                rReq.SetReturnValue(SfxBoolItem( nSlot, bRes ));
            }
            break;

            case FN_INSERT_REF_FIELD:
            {
                SfxViewFrame* pVFrame = GetView().GetViewFrame();
                if (!pVFrame->HasChildWindow(FN_INSERT_FIELD))
                    pVFrame->ToggleChildWindow(FN_INSERT_FIELD);	// Dialog anzeigen

                // Flddlg auf neue TabPage umschalten
                USHORT nId = SwFldDlgWrapper::GetChildWindowId();
                SwFldDlgWrapper *pWrp = (SwFldDlgWrapper*)pVFrame->GetChildWindow(nId);
                if (pWrp)
                    pWrp->ShowPage();
                rReq.Ignore();
            }
            break;


            case FN_POSTIT:
            {
                pPostItFldMgr = new SwFldMgr;
                SwPostItField* pPostIt = (SwPostItField*)pPostItFldMgr->GetCurFld();
                BOOL bNew = !(pPostIt && pPostIt->GetTyp()->Which() == RES_POSTITFLD);
                BOOL bTravel = FALSE;
                BOOL bNext = FALSE, bPrev = FALSE;

                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                DBG_ASSERT(pFact, "Dialogdiet fail!");
                ::DialogGetRanges fnGetRange = pFact->GetDialogGetRangesFunc( RID_SVXDLG_POSTIT );
                DBG_ASSERT(fnGetRange, "Dialogdiet fail! GetRanges()");
                SfxItemSet aSet(GetPool(), fnGetRange());

                if(!bNew)
                {
                    aSet.Put(SvxPostItTextItem(pPostIt->GetPar2().ConvertLineEnd(), SID_ATTR_POSTIT_TEXT));
                    aSet.Put(SvxPostItAuthorItem(pPostIt->GetPar1(), SID_ATTR_POSTIT_AUTHOR));

                    aSet.Put( SvxPostItDateItem(
                            GetAppLocaleData().getDate( pPostIt->GetDate() ),
                            SID_ATTR_POSTIT_DATE ));

                    // Traveling nur bei mehr als einem Feld
                    rSh.StartAction();

                    bNext = pPostItFldMgr->GoNext();
                    if( bNext )
                        pPostItFldMgr->GoPrev();

                    if( 0 != ( bPrev = pPostItFldMgr->GoPrev() ) )
                        pPostItFldMgr->GoNext();
                    bTravel |= bNext|bPrev;

                    rSh.EndAction();
                }
                else
                {
                    SvtUserOptions aUserOpt;
                    aSet.Put(SvxPostItTextItem( aEmptyStr,
                                                    SID_ATTR_POSTIT_TEXT));
                    aSet.Put(SvxPostItAuthorItem( aUserOpt.GetID(),
                                                    SID_ATTR_POSTIT_AUTHOR));
                    aSet.Put(SvxPostItDateItem(
                            GetAppLocaleData().getDate( Date() ),
                            SID_ATTR_POSTIT_DATE));
                }

                const SfxItemSet* pSet = pArgs;
                AbstractSvxPostItDialog *pDlg = NULL;
                if ( !pArgs )
                {
                    SvxAbstractDialogFactory* pFact2 = SvxAbstractDialogFactory::Create();
                    DBG_ASSERT(pFact2, "Dialogdiet fail!");
                    pDlg = pFact2->CreateSvxPostItDialog( pMDI, aSet, RID_SVXDLG_POSTIT, bTravel );
                    DBG_ASSERT(pDlg, "Dialogdiet fail!");
                    pDlg->SetReadonlyPostIt(rSh.IsReadOnlyAvailable() && rSh.HasReadonlySel());

                    if (bTravel)
                    {
                        pDlg->EnableTravel(bNext, bPrev);
                        pDlg->SetPrevHdl(LINK(this, SwTextShell, PostItPrevHdl));
                        pDlg->SetNextHdl(LINK(this, SwTextShell, PostItNextHdl));
                    }

                    if (bNew)
                        pDlg->SetText(SW_RESSTR(STR_NOTIZ_INSERT));

                    bNoInterrupt = TRUE;
                    if ( pDlg->Execute() == RET_OK )
                    {
                        pSet = pDlg->GetOutputItemSet();
                        rReq.Done( *pSet );
                    }
                    else
                        rReq.Ignore();
                }

                if ( pSet )
                {
                    String sMsg(((const SvxPostItTextItem&)pSet->Get(SID_ATTR_POSTIT_TEXT)).GetValue());
                    String sAuthor(((const SvxPostItAuthorItem&)pSet->Get(SID_ATTR_POSTIT_AUTHOR)).GetValue());

                    if(bNew)
                    {
                        // neues PostIt anlegen
                        SwInsertFld_Data aData(TYP_POSTITFLD, 0, sAuthor, sMsg, 0);
                        pPostItFldMgr->InsertFld(aData);
                    }
                    else
                        // altes PostIt updaten
                        pPostItFldMgr->UpdateCurFld(0, sAuthor, sMsg);
                }

                delete pDlg;
                delete pPostItFldMgr;
                bNoInterrupt = FALSE;
                GetView().AttrChangedNotify(GetShellPtr());
            }
            break;

            case FN_REDLINE_COMMENT:
            {
                String sComment;
                const SwRedline *pRedline = rSh.GetCurrRedline();

                if (pRedline)
                {
                    sComment = pRedline->GetComment();


                    BOOL bTravel = FALSE;

                    SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                    DBG_ASSERT(pFact, "Dialogdiet fail!");
                    ::DialogGetRanges fnGetRange = pFact->GetDialogGetRangesFunc( RID_SVXDLG_POSTIT );
                    DBG_ASSERT(fnGetRange, "Dialogdiet fail! GetRanges()");
                    SfxItemSet aSet(GetPool(), fnGetRange());
                    aSet.Put(SvxPostItTextItem(sComment.ConvertLineEnd(), SID_ATTR_POSTIT_TEXT));
                    aSet.Put(SvxPostItAuthorItem(pRedline->GetAuthorString(), SID_ATTR_POSTIT_AUTHOR));

                    aSet.Put( SvxPostItDateItem( GetAppLangDateTimeString(
                                pRedline->GetRedlineData().GetTimeStamp() ),
                                SID_ATTR_POSTIT_DATE ));

                    // Traveling nur bei mehr als einem Feld
                    rSh.StartAction();

                    rSh.Push();
                    const SwRedline *pActRed = rSh.SelPrevRedline();

                    if (pActRed == pRedline)
                    {	// Neuer Cursor steht am Anfang des Current Redlines
                        rSh.Pop();	// Alten Cursor wegwerfen
                        rSh.Push();
                        pActRed = rSh.SelPrevRedline();
                    }

                    BOOL bPrev = pActRed != 0;
                    rSh.Pop(FALSE);
                    rSh.EndAction();

                    rSh.ClearMark();
                    rSh.SelNextRedline();	// Aktueller Redline wird selektiert

                    rSh.StartAction();
                    rSh.Push();
                    pActRed = rSh.SelNextRedline();
                    BOOL bNext = pActRed != 0;
                    rSh.Pop(FALSE);	// Cursorpos restaurieren

                    if( rSh.IsCrsrPtAtEnd() )
                        rSh.SwapPam();

                    rSh.EndAction();

                    bTravel |= bNext|bPrev;

                    SvxAbstractDialogFactory* pFact2 = SvxAbstractDialogFactory::Create();
                    DBG_ASSERT(pFact2, "Dialogdiet fail!");
                    AbstractSvxPostItDialog* pDlg = pFact2->CreateSvxPostItDialog( pMDI, aSet, RID_SVXDLG_POSTIT, bTravel, TRUE );
                    DBG_ASSERT(pDlg, "Dialogdiet fail!");
                    pDlg->HideAuthor();

                    String sTitle(SW_RES(STR_REDLINE_COMMENT));
                    ::lcl_AppendRedlineStr( sTitle, pRedline->GetType() );

                    pDlg->SetText(sTitle);

                    if (bTravel)
                    {
                        pDlg->EnableTravel(bNext, bPrev);
                        pDlg->SetPrevHdl(LINK(this, SwTextShell, RedlinePrevHdl));
                        pDlg->SetNextHdl(LINK(this, SwTextShell, RedlineNextHdl));
                    }

                    rSh.SetCareWin(pDlg->GetWindow());
                    bNoInterrupt = TRUE;

                    if ( pDlg->Execute() == RET_OK )
                    {
                        const SfxItemSet* pOutSet = pDlg->GetOutputItemSet();
                        String sMsg(((const SvxPostItTextItem&)pOutSet->Get(SID_ATTR_POSTIT_TEXT)).GetValue());

                        // Kommentar einfuegen bzw aendern
                        rSh.SetRedlineComment(sMsg);
                    }

                    delete pDlg;
                    rSh.SetCareWin(NULL);
                    bNoInterrupt = FALSE;
                    rSh.ClearMark();
                    GetView().AttrChangedNotify(GetShellPtr());
                }
            }
            break;

            case FN_JAVAEDIT:
            {
                String aType, aText;
                BOOL bIsUrl=FALSE;
                BOOL bNew=FALSE, bUpdate=FALSE;
                SwFldMgr* pMgr = new SwFldMgr;
                if ( pItem )
                {
                    aText = ((SfxStringItem*)pItem)->GetValue();
                    SFX_REQUEST_ARG( rReq, pType, SfxStringItem, FN_PARAM_2 , sal_False );
                    SFX_REQUEST_ARG( rReq, pIsUrl, SfxBoolItem, FN_PARAM_1 , sal_False );
                    if ( pType )
                        aType = pType->GetValue();
                    if ( pIsUrl )
                        bIsUrl = pIsUrl->GetValue();

                    SwScriptField* pFld = (SwScriptField*)pMgr->GetCurFld();
                    bNew = !pFld || !(pFld->GetTyp()->Which() == RES_SCRIPTFLD);
                    bUpdate = pFld && ( bIsUrl != pFld->GetFormat() || pFld->GetPar2() != aType || pFld->GetPar1() != aText );
                }
                else
                {
                    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
                    DBG_ASSERT(pFact, "Dialogdiet fail!");
                    AbstractJavaEditDialog* pDlg = pFact->CreateJavaEditDialog( DLG_JAVAEDIT,
                                                            pMDI, &rSh);
                    DBG_ASSERT(pDlg, "Dialogdiet fail!");
                    if ( pDlg->Execute() )
                    {
                        aType = pDlg->GetType();
                        aText = pDlg->GetText();
                        bIsUrl = pDlg->IsUrl();
                        bNew = pDlg->IsNew();
                        bUpdate = pDlg->IsUpdate();
                        rReq.AppendItem( SfxStringItem( FN_JAVAEDIT, aText ) );
                        rReq.AppendItem( SfxStringItem( FN_PARAM_2, aType ) );
                        rReq.AppendItem( SfxBoolItem( FN_PARAM_1, bIsUrl ) );
                    }

                    delete pDlg;
                }

                if( bNew )
                {
                    SwInsertFld_Data aData(TYP_SCRIPTFLD, 0, aType, aText, bIsUrl);
                    pMgr->InsertFld(aData);
                    rReq.Done();
                }
                else if( bUpdate )
                {
                    pMgr->UpdateCurFld( bIsUrl, aType, aText );
                    rSh.SetUndoNoResetModified();
                    rReq.Done();
                }
                else
                    rReq.Ignore();
            }
            break;

            case FN_INSERT_FLD_DATE    :
                nInsertType = TYP_DATEFLD;
                bIsText = FALSE;
                goto FIELD_INSERT;
            case FN_INSERT_FLD_TIME    :
                nInsertType = TYP_TIMEFLD;
                bIsText = FALSE;
                goto FIELD_INSERT;
            case FN_INSERT_FLD_PGNUMBER:
                nInsertType = TYP_PAGENUMBERFLD;
                nInsertFormat = SVX_NUM_PAGEDESC; // wie Seitenvorlage
                bIsText = FALSE;
                goto FIELD_INSERT;
            case FN_INSERT_FLD_PGCOUNT :
                nInsertType = TYP_DOCSTATFLD;
                nInsertSubType = 0;
                bIsText = FALSE;
                nInsertFormat = SVX_NUM_PAGEDESC;
                goto FIELD_INSERT;
            case FN_INSERT_FLD_TOPIC   :
                nInsertType = TYP_DOCINFOFLD;
                nInsertSubType = DI_THEMA;
                goto FIELD_INSERT;
            case FN_INSERT_FLD_TITLE   :
                nInsertType = TYP_DOCINFOFLD;
                nInsertSubType = DI_TITEL;
                goto FIELD_INSERT;
            case FN_INSERT_FLD_AUTHOR  :
                nInsertType = TYP_DOCINFOFLD;
                nInsertSubType = DI_CREATE|DI_SUB_AUTHOR;

FIELD_INSERT:
            {
                //format conversion should only be done for number formatter formats
                if(!nInsertFormat)
                    nInsertFormat = aFldMgr.GetDefaultFormat(nInsertType, bIsText, rSh.GetNumberFormatter());
                SwInsertFld_Data aData(nInsertType, nInsertSubType,
                                    aEmptyStr, aEmptyStr, nInsertFormat);
                aFldMgr.InsertFld(aData);
                rReq.Done();
            }
            break;
            default:
                ASSERT(FALSE, falscher Dispatcher);
                return;
        }
    }
}


void SwTextShell::StateField( SfxItemSet &rSet )
{
    SwWrtShell& rSh = GetShell();
    SfxWhichIter aIter( rSet );
    const SwField* pField = 0;
    int bGetField = FALSE;
    USHORT nWhich = aIter.FirstWhich();

    while (nWhich)
    {
        switch (nWhich)
        {
            case FN_EDIT_FIELD:
            {
                /* #108536# Fields can be selected, too now. Removed

                if( rSh.HasSelection() )
                     rSet.DisableItem(nWhich);
                else ...
                */

                if( !bGetField )
                {
                    pField = rSh.GetCurFld();
                    bGetField = TRUE;
                }

                USHORT nTempWhich = pField ? pField->GetTyp()->Which() : USHRT_MAX;
                if(	USHRT_MAX == nTempWhich ||
                    RES_POSTITFLD == nTempWhich ||
                    RES_SCRIPTFLD == nTempWhich ||
                    RES_AUTHORITY == nTempWhich )
                    rSet.DisableItem( nWhich );
                else if( RES_DDEFLD == nTempWhich &&
                        !((SwDDEFieldType*)pField->GetTyp())->GetBaseLink().IsVisible())
                {
                    // nested links cannot be edited
                    rSet.DisableItem( nWhich );
                }
            }
            break;
            case FN_EXECUTE_MACROFIELD:
            {
                if(!bGetField)
                {
                    pField = rSh.GetCurFld();
                    bGetField = TRUE;
                }
                if(!pField || pField->GetTyp()->Which() != RES_MACROFLD)
                    rSet.DisableItem(nWhich);
            }
            break;

            case FN_INSERT_FIELD:
            {
                SfxViewFrame* pVFrame = GetView().GetViewFrame();
                //#i5788# prevent closing of the field dialog while a modal dialog ( Input field dialog ) is active
                if(!pVFrame->IsInModalMode() &&
                        pVFrame->KnowsChildWindow(FN_INSERT_FIELD) && !pVFrame->HasChildWindow(FN_INSERT_FIELD_DATA_ONLY) )
                    rSet.Put(SfxBoolItem( FN_INSERT_FIELD, pVFrame->HasChildWindow(nWhich)));
                else
                    rSet.DisableItem(FN_INSERT_FIELD);
            }
            break;
            case FN_INSERT_REF_FIELD:
            {
                SfxViewFrame* pVFrame = GetView().GetViewFrame();
                if (!pVFrame->KnowsChildWindow(FN_INSERT_FIELD))
                    rSet.DisableItem(FN_INSERT_REF_FIELD);
            }
            break;
            case FN_INSERT_FIELD_CTRL:
                rSet.Put(SfxBoolItem( nWhich, GetView().GetViewFrame()->HasChildWindow(FN_INSERT_FIELD)));
            break;
            case FN_REDLINE_COMMENT:
                if (!rSh.GetCurrRedline())
                    rSet.DisableItem(nWhich);
                break;
            case FN_POSTIT :
            case FN_JAVAEDIT :
                BOOL bCurField = FALSE;
                pField = rSh.GetCurFld();
                if(nWhich == FN_POSTIT)
                    bCurField = pField && pField->GetTyp()->Which() == RES_POSTITFLD;
                else
                    bCurField = pField && pField->GetTyp()->Which() == RES_SCRIPTFLD;

                if(!bCurField && rSh.IsReadOnlyAvailable() && rSh.HasReadonlySel() )
                    rSet.DisableItem(nWhich);
            break;
        }
        nWhich = aIter.NextWhich();
    }
}

/*---------------------------------------------------------------------------
    Beschreibung:
 ----------------------------------------------------------------------------*/


void SwTextShell::InsertHyperlink(const SvxHyperlinkItem& rHlnkItem)
{
    const String& rName   = rHlnkItem.GetName();
    const String& rURL    = rHlnkItem.GetURL();
    const String& rTarget = rHlnkItem.GetTargetFrame();
    USHORT nType =  (USHORT)rHlnkItem.GetInsertMode();
    nType &= ~HLINK_HTMLMODE;
    const SvxMacroTableDtor* pMacroTbl = rHlnkItem.GetMacroTbl();

    SwWrtShell& rSh = GetShell();

    if( rSh.GetSelectionType() & nsSelectionType::SEL_TXT )
    {
        rSh.StartAction();
        SfxItemSet aSet(GetPool(), RES_TXTATR_INETFMT, RES_TXTATR_INETFMT);
        rSh.GetAttr( aSet );

        const SfxPoolItem* pItem;
        if(SFX_ITEM_SET == aSet.GetItemState(RES_TXTATR_INETFMT, FALSE, &pItem))
        {
            // Links selektieren
            rSh.SwCrsrShell::SelectTxtAttr(RES_TXTATR_INETFMT, FALSE);
        }
        switch (nType)
        {
        case HLINK_DEFAULT:
        case HLINK_FIELD:
            {
                SwFmtINetFmt aINetFmt( rURL, rTarget );
                aINetFmt.SetName(rHlnkItem.GetIntName());
                if(pMacroTbl)
                {
                    SvxMacro *pMacro = pMacroTbl->Get( SFX_EVENT_MOUSEOVER_OBJECT );
                    if( pMacro )
                        aINetFmt.SetMacro(SFX_EVENT_MOUSEOVER_OBJECT, *pMacro);
                    pMacro = pMacroTbl->Get( SFX_EVENT_MOUSECLICK_OBJECT );
                    if( pMacro )
                        aINetFmt.SetMacro(SFX_EVENT_MOUSECLICK_OBJECT, *pMacro);
                    pMacro = pMacroTbl->Get( SFX_EVENT_MOUSEOUT_OBJECT );
                    if( pMacro )
                        aINetFmt.SetMacro(SFX_EVENT_MOUSEOUT_OBJECT, *pMacro);
                }
                rSh.SttSelect();
                rSh.InsertURL( aINetFmt, rName, TRUE );
                rSh.EndSelect();
            }
            break;

        case HLINK_BUTTON:
            BOOL bSel = rSh.HasSelection();
            if(bSel)
                rSh.DelRight();
            InsertURLButton( rURL, rTarget, rName );
            rSh.EnterStdMode();
            break;
        }
        rSh.EndAction();
    }
}

/*--------------------------------------------------------------------
    Beschreibung: Traveling zwishen PostIts
 --------------------------------------------------------------------*/


IMPL_LINK( SwTextShell, PostItNextHdl, AbstractSvxPostItDialog *, pBtn )
{
    AbstractSvxPostItDialog *pDlg = (AbstractSvxPostItDialog*)pBtn;
    if( pDlg->IsOkEnabled() )
    {
        //SvtUserOptions aUserOpt;
        //pPostItFldMgr->UpdateCurFld( 0, aUserOpt.GetID(), pDlg->GetNote() );
        SwPostItField* pPostIt = (SwPostItField*)pPostItFldMgr->GetCurFld();
        pPostItFldMgr->UpdateCurFld( 0, pPostIt->GetPar1(), pDlg->GetNote() );
    }
    pPostItFldMgr->GoNext();
    SwPostItField* pPostIt = (SwPostItField*)pPostItFldMgr->GetCurFld();
    pDlg->SetNote(pPostIt->GetPar2().ConvertLineEnd());
    pDlg->ShowLastAuthor( pPostIt->GetPar1(),
                          GetAppLocaleData().getDate(pPostIt->GetDate()));

    // Traveling nur bei mehr als einem Feld
    SwWrtShell* pSh = GetShellPtr();
    pDlg->SetReadonlyPostIt(pSh->IsReadOnlyAvailable() && pSh->HasReadonlySel());
    pSh->StartAction();

    BOOL bEnable = FALSE;
    if( pPostItFldMgr->GoNext() )
    {
        bEnable = TRUE;
        pPostItFldMgr->GoPrev();
    }
    pDlg->EnableTravel(bEnable, TRUE);

    pSh->EndAction();

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwTextShell, PostItPrevHdl, AbstractSvxPostItDialog *, pBtn )
{
    AbstractSvxPostItDialog *pDlg = (AbstractSvxPostItDialog*)pBtn;

    if( pDlg->IsOkEnabled() )
    {
        //SvtUserOptions aUserOpt;
        //pPostItFldMgr->UpdateCurFld( 0, aUserOpt.GetID(), pDlg->GetNote() );
        SwPostItField* pPostIt = (SwPostItField*)pPostItFldMgr->GetCurFld();
        pPostItFldMgr->UpdateCurFld( 0, pPostIt->GetPar1(), pDlg->GetNote() );
    }
    pPostItFldMgr->GoPrev();
    SwPostItField* pPostIt = (SwPostItField*)pPostItFldMgr->GetCurFld();
    pDlg->SetNote(pPostIt->GetPar2().ConvertLineEnd());
    pDlg->ShowLastAuthor( pPostIt->GetPar1(),
                          GetAppLocaleData().getDate(pPostIt->GetDate()));

    // Traveling nur bei mehr als einem Feld
    SwWrtShell* pSh = GetShellPtr();
    pDlg->SetReadonlyPostIt(pSh->IsReadOnlyAvailable() && pSh->HasReadonlySel());
    pSh->StartAction();

    BOOL bEnable = FALSE;
    if( pPostItFldMgr->GoPrev() )
    {
        bEnable = TRUE;
        pPostItFldMgr->GoNext();
    }
    pDlg->EnableTravel(TRUE, bEnable);

    pSh->EndAction();

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung: Traveling zwischen Redlines
 --------------------------------------------------------------------*/

IMPL_LINK( SwTextShell, RedlineNextHdl, AbstractSvxPostItDialog *, pBtn )
{
    SwWrtShell* pSh = GetShellPtr();
    AbstractSvxPostItDialog *pDlg = (AbstractSvxPostItDialog*)pBtn;

    // Kommentar einfuegen bzw aendern
    pSh->SetRedlineComment(pDlg->GetNote());

    const SwRedline *pRedline = pSh->GetCurrRedline();

    String sComment;

    if (pRedline)
    {
        // Traveling nur bei mehr als einem Feld
        if( !pSh->IsCrsrPtAtEnd() )
            pSh->SwapPam();	// Cursor hinter den Redline stellen

        pSh->Push();
        const SwRedline *pActRed = pSh->SelNextRedline();
        pSh->Pop(pActRed != 0);

        BOOL bEnable = FALSE;

        if (pActRed)
        {
            pSh->StartAction();
            pSh->Push();
            bEnable = pSh->SelNextRedline() != 0;
            pSh->Pop(FALSE);
            pSh->EndAction();
        }

        pDlg->EnableTravel(bEnable, TRUE);

        if( pSh->IsCrsrPtAtEnd() )
            pSh->SwapPam();

        pRedline = pSh->GetCurrRedline();
        sComment = pRedline->GetComment();

        pDlg->SetNote( sComment.ConvertLineEnd() );
        pDlg->ShowLastAuthor( pRedline->GetAuthorString(),
                    GetAppLangDateTimeString(
                                pRedline->GetRedlineData().GetTimeStamp() ));

        String sTitle(SW_RES(STR_REDLINE_COMMENT));
        ::lcl_AppendRedlineStr( sTitle, pRedline->GetType() );

        pDlg->SetText(sTitle);
    }

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwTextShell, RedlinePrevHdl, AbstractSvxPostItDialog *, pBtn )
{
    SwWrtShell* pSh = GetShellPtr();
    AbstractSvxPostItDialog *pDlg = (AbstractSvxPostItDialog*)pBtn;

    // Kommentar einfuegen bzw aendern
    pSh->SetRedlineComment(pDlg->GetNote());

    const SwRedline *pRedline = pSh->GetCurrRedline();

    String sComment;

    if (pRedline)
    {
        // Traveling nur bei mehr als einem Feld
        pSh->Push();
        const SwRedline *pActRed = pSh->SelPrevRedline();
        pSh->Pop(pActRed != 0);

        BOOL bEnable = FALSE;

        if (pActRed)
        {
            pSh->StartAction();
            pSh->Push();
            bEnable = pSh->SelPrevRedline() != 0;
            pSh->Pop(FALSE);
            pSh->EndAction();
        }

        pDlg->EnableTravel(TRUE, bEnable);

        pRedline = pSh->GetCurrRedline();
        sComment = pRedline->GetComment();

        pDlg->SetNote(sComment.ConvertLineEnd());
        pDlg->ShowLastAuthor(pRedline->GetAuthorString(),
                GetAppLangDateTimeString(
                                pRedline->GetRedlineData().GetTimeStamp() ));

        String sTitle(SW_RES(STR_REDLINE_COMMENT));
        ::lcl_AppendRedlineStr( sTitle, pRedline->GetType() );

        pDlg->SetText(sTitle);
    }

    return 0;
}



