/*************************************************************************
 *
 *  $RCSfile: grfsh.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: os $ $Date: 2001-07-17 08:43:35 $
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


#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _UIPARAM_HXX
#include <uiparam.hxx>
#endif

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVX_SIZEITEM_HXX //autogen
#include <svx/sizeitem.hxx>
#endif
#ifndef _SVX_PROTITEM_HXX //autogen
#include <svx/protitem.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVX_SRCHITEM_HXX
#include <svx/srchitem.hxx>
#endif
#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif
#ifndef _SDGLUITM_HXX
#include <svx/sdgluitm.hxx>
#endif
#ifndef _SDGCOITM_HXX
#include <svx/sdgcoitm.hxx>
#endif
#ifndef _SDGGAITM_HXX
#include <svx/sdggaitm.hxx>
#endif
#ifndef _SDGTRITM_HXX
#include <svx/sdgtritm.hxx>
#endif
#ifndef _SDGINITM_HXX
#include <svx/sdginitm.hxx>
#endif
#ifndef _SDGMOITM_HXX
#include <svx/sdgmoitm.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_GRFFLT_HXX //autogen
#include <svx/grfflt.hxx>
#endif

#ifndef _FMTURL_HXX //autogen
#include <fmturl.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _GRFSH_HXX
#include <grfsh.hxx>
#endif
#ifndef _FRMMGR_HXX
#include <frmmgr.hxx>
#endif
#ifndef _FRMDLG_HXX
#include <frmdlg.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _GRFATR_HXX
#include <grfatr.hxx>
#endif
#ifndef _USRPREF_HXX
#include <usrpref.hxx>
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif
#ifndef _SHELLS_HRC
#include <shells.hrc>
#endif
#ifndef _POPUP_HRC
#include <popup.hrc>
#endif

#define SwGrfShell
#include "itemdef.hxx"
#include "swslots.hxx"



SFX_IMPL_INTERFACE(SwGrfShell, SwBaseShell, SW_RES(STR_SHELLNAME_GRAPHIC))
{
    SFX_POPUPMENU_REGISTRATION(SW_RES(MN_GRF_POPUPMENU));
    SFX_OBJECTBAR_REGISTRATION(SFX_OBJECTBAR_OBJECT, SW_RES(RID_GRAFIK_TOOLBOX));
    SFX_OBJECTMENU_REGISTRATION(SID_OBJECTMENU0, SW_RES(MN_OBJECTMENU_GRAFIK));
}


void SwGrfShell::Execute(SfxRequest &rReq)
{
    const SfxItemSet* pArgs = rReq.GetArgs();
    SwWrtShell &rSh = GetShell();

    USHORT nSlot = rReq.GetSlot();
    switch(nSlot)
    {
        case SID_INSERT_GRAPHIC:
        case FN_FORMAT_GRAFIC_DLG:
        {
            SwFlyFrmAttrMgr aMgr( FALSE, &rSh, rSh.IsFrmSelected() ?
                                               FRMMGR_TYPE_NONE : FRMMGR_TYPE_GRF);
            const SwViewOption* pVOpt = rSh.GetViewOptions();
            SwViewOption aUsrPref( *pVOpt );

            SfxItemSet aSet(GetPool(), RES_FRMATR_BEGIN, RES_FRMATR_END-1,
                            RES_GRFATR_MIRRORGRF, 	RES_GRFATR_CROPGRF,
                            SID_ATTR_BORDER_INNER, 	SID_ATTR_BORDER_INNER,
                            SID_ATTR_GRAF_KEEP_ZOOM, SID_ATTR_GRAF_KEEP_ZOOM,
                            SID_ATTR_GRAF_FRMSIZE, SID_ATTR_GRAF_FRMSIZE,
                            SID_ATTR_GRAF_FRMSIZE_PERCENT, SID_ATTR_GRAF_FRMSIZE_PERCENT,
                            SID_ATTR_GRAF_GRAPHIC, SID_ATTR_GRAF_GRAPHIC,
                            FN_PARAM_GRF_CONNECT, 	FN_PARAM_GRF_CONNECT,
                            SID_ATTR_PAGE_SIZE, 	SID_ATTR_PAGE_SIZE,
                            FN_GET_PRINT_AREA, 		FN_GET_PRINT_AREA,
                            FN_SET_FRM_NAME, 		FN_KEEP_ASPECT_RATIO,
                            FN_PARAM_GRF_DIALOG, 	FN_PARAM_GRF_DIALOG,
                            SID_DOCFRAME, 			SID_DOCFRAME,
                            SID_HTML_MODE, 			SID_HTML_MODE,
                            FN_SET_FRM_ALT_NAME,	FN_SET_FRM_ALT_NAME,
                            0);

            USHORT nHtmlMode = ::GetHtmlMode(GetView().GetDocShell());
            aSet.Put(SfxUInt16Item(SID_HTML_MODE, nHtmlMode));
            FieldUnit eMetric = ::GetDfltMetric((0 != (nHtmlMode&HTMLMODE_ON)));
            SW_MOD()->PutItem(SfxUInt16Item(SID_ATTR_METRIC, eMetric));

            const SwRect* pRect = &rSh.GetAnyCurRect(RECT_PAGE);
            SwFmtFrmSize aFrmSize( ATT_VAR_SIZE, pRect->Width(), pRect->Height());
            aFrmSize.SetWhich( GetPool().GetWhich( SID_ATTR_PAGE_SIZE ) );
            aSet.Put( aFrmSize );

            aSet.Put(SfxStringItem(FN_SET_FRM_NAME, rSh.GetFlyName()));
            if(nSlot == FN_FORMAT_GRAFIC_DLG)
                aSet.Put(SfxStringItem(FN_SET_FRM_ALT_NAME, rSh.GetAlternateText()));

            pRect = &rSh.GetAnyCurRect(RECT_PAGE_PRT);
            aFrmSize.SetWidth( pRect->Width() );
            aFrmSize.SetHeight( pRect->Height() );
            aFrmSize.SetWhich( GetPool().GetWhich(FN_GET_PRINT_AREA) );
            aSet.Put( aFrmSize );

            aSet.Put( aMgr.GetAttrSet() );
            aSet.SetParent( aMgr.GetAttrSet().GetParent() );

            // Bei %-Werten Groesse initialisieren
            SwFmtFrmSize& rSize = (SwFmtFrmSize&)aSet.Get(RES_FRM_SIZE);
            if (rSize.GetWidthPercent() && rSize.GetWidthPercent() != 0xff)
                rSize.SetWidth(rSh.GetAnyCurRect(RECT_FLY_EMBEDDED).Width());
            if (rSize.GetHeightPercent() && rSize.GetHeightPercent() != 0xff)
                rSize.SetHeight(rSh.GetAnyCurRect(RECT_FLY_EMBEDDED).Height());

            // and now set the size for "external" tabpages
            {
                SvxSizeItem aSzItm( SID_ATTR_GRAF_FRMSIZE, rSize.GetSize() );
                aSet.Put( aSzItm );

                Size aSz( rSize.GetWidthPercent(), rSize.GetHeightPercent() );
                if( 0xff == aSz.Width() ) 	aSz.Width() = 0;
                if( 0xff == aSz.Height() )  aSz.Height() = 0;

                aSzItm.SetSize( aSz );
                aSzItm.SetWhich( SID_ATTR_GRAF_FRMSIZE_PERCENT );
                aSet.Put( aSzItm );
            }

            String sGrfNm, sFilterNm;
            rSh.GetGrfNms( &sGrfNm, &sFilterNm );
            if( sGrfNm.Len() )
            {
                aSet.Put( SvxBrushItem( INetURLObject::decode( sGrfNm,
                                        INET_HEX_ESCAPE,
                                           INetURLObject::DECODE_UNAMBIGUOUS,
                                        RTL_TEXTENCODING_UTF8 ),
                                        sFilterNm, GPOS_LT,
                                        SID_ATTR_GRAF_GRAPHIC ));
            }
            else
            {
                aSet.Put( SvxBrushItem( rSh.GetGraphicObj(), GPOS_LT,
                                        SID_ATTR_GRAF_GRAPHIC ));
            }
            aSet.Put(SfxBoolItem( FN_PARAM_GRF_CONNECT, sGrfNm.Len() ));

            // get Mirror and Crop
            {
                SfxItemSet aTmpSet( rSh.GetAttrPool(),
                                RES_GRFATR_MIRRORGRF, RES_GRFATR_CROPGRF );

                rSh.GetAttr( aTmpSet );
                aSet.Put( aTmpSet );
            }

            aSet.Put(SfxBoolItem(FN_KEEP_ASPECT_RATIO, aUsrPref.IsKeepRatio()));
            aSet.Put(SfxBoolItem( SID_ATTR_GRAF_KEEP_ZOOM, aUsrPref.IsGrfKeepZoom()));

            aSet.Put(SfxFrameItem( SID_DOCFRAME, GetView().GetViewFrame()->GetTopFrame()));

            SwFrmDlg *pDlg = new SwFrmDlg( GetView().GetViewFrame(),
                                           GetView().GetWindow(),
                                           aSet, FALSE, DLG_FRM_GRF );
            if( pDlg->Execute() )
            {
                rSh.StartAllAction();
                rSh.StartUndo(UNDO_START);
                const SfxPoolItem* pItem;
                SfxItemSet* pSet = (SfxItemSet*)pDlg->GetOutputItemSet();

                // change the 2 frmsize SizeItems to the correct SwFrmSizeItem
                if( SFX_ITEM_SET == pSet->GetItemState(
                                SID_ATTR_GRAF_FRMSIZE, FALSE, &pItem ))
                {
                    SwFmtFrmSize aSize;
                    const Size& rSz = ((SvxSizeItem*)pItem)->GetSize();
                    aSize.SetWidth( rSz.Width() );
                    aSize.SetHeight( rSz.Height() );

                    if( SFX_ITEM_SET == pSet->GetItemState(
                            SID_ATTR_GRAF_FRMSIZE_PERCENT, FALSE, &pItem ))
                    {
                        const Size& rSz = ((SvxSizeItem*)pItem)->GetSize();
                        aSize.SetWidthPercent( rSz.Width() );
                        aSize.SetHeightPercent( rSz.Height() );
                    }
                    pSet->Put( aSize );
                }

                // Vorlagen-AutoUpdate
                SwFrmFmt* pFmt = rSh.GetCurFrmFmt();
                if(pFmt && pFmt->IsAutoUpdateFmt())
                {
                    pFmt->SetAttr(*pSet);
                    SfxItemSet aShellSet(GetPool(), RES_FRM_SIZE,	RES_FRM_SIZE,
                                                    RES_SURROUND, 	RES_SURROUND,
                                                    RES_ANCHOR,		RES_ANCHOR,
                                                    RES_VERT_ORIENT,RES_HORI_ORIENT,
                                                    0);
                    aShellSet.Put(*pSet);
                    aMgr.SetAttrSet(aShellSet);
                }
                else
                {
                    aMgr.SetAttrSet(*pSet);
                }
                aMgr.UpdateFlyFrm();

                BOOL bApplyUsrPref = FALSE;
                if (SFX_ITEM_SET == pSet->GetItemState(
                    FN_KEEP_ASPECT_RATIO, TRUE, &pItem ))
                {
                    aUsrPref.SetKeepRatio(
                                    ((const SfxBoolItem*)pItem)->GetValue() );
                    bApplyUsrPref = TRUE;
                }
                if( SFX_ITEM_SET == pSet->GetItemState(
                    SID_ATTR_GRAF_KEEP_ZOOM, TRUE, &pItem ))
                {
                    aUsrPref.SetGrfKeepZoom(
                                    ((const SfxBoolItem*)pItem)->GetValue() );
                    bApplyUsrPref = TRUE;
                }

                if( bApplyUsrPref )
                    SW_MOD()->ApplyUsrPref(aUsrPref, &GetView());

                // and now set all the graphic attributes and other stuff
                if( SFX_ITEM_SET == pSet->GetItemState(
                                        SID_ATTR_GRAF_GRAPHIC, TRUE, &pItem ))
                {
                    if( ((SvxBrushItem*)pItem)->GetGraphicLink() )
                        sGrfNm = *((SvxBrushItem*)pItem)->GetGraphicLink();
                    else
                        sGrfNm.Erase();

                    if( ((SvxBrushItem*)pItem)->GetGraphicFilter() )
                        sFilterNm = *((SvxBrushItem*)pItem)->GetGraphicFilter();
                    else
                        sFilterNm.Erase();

                    if( sGrfNm.Len() )
                    {
                        SwWait aWait( *GetView().GetDocShell(), TRUE );
                        rSh.ReRead( URIHelper::SmartRelToAbs( sGrfNm ),
                                     sFilterNm, 0 );
                    }
                }
                if( SFX_ITEM_SET == pSet->GetItemState(
                                        FN_SET_FRM_ALT_NAME, TRUE, &pItem ))
                    rSh.SetAlternateText(
                                ((const SfxStringItem*)pItem)->GetValue() );

                SfxItemSet aGrfSet( rSh.GetAttrPool(), RES_GRFATR_BEGIN,
                                                       RES_GRFATR_END-1 );
                aGrfSet.Put( *pSet );
                if( aGrfSet.Count() )
                    rSh.SetAttr( aGrfSet );

                rSh.EndUndo(UNDO_END);
                rSh.EndAllAction();
            }
            delete pDlg;
        }
        break;

        case FN_GRAPHIC_MIRROR_ON_EVEN_PAGES:
        {
            SfxItemSet aSet(rSh.GetAttrPool(), RES_GRFATR_MIRRORGRF, RES_GRFATR_MIRRORGRF);
            rSh.GetAttr( aSet );
            SwMirrorGrf aGrf((const SwMirrorGrf &)aSet.Get(RES_GRFATR_MIRRORGRF));
            aGrf.SetGrfToggle(!aGrf.IsGrfToggle());
            rSh.SetAttr(aGrf);
        }
        break;

        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }
}


void SwGrfShell::ExecAttr( SfxRequest &rReq )
{
    USHORT nGrfType;
    if( CNT_GRF == GetShell().GetCntType() &&
        ( GRAPHIC_BITMAP == ( nGrfType = GetShell().GetGraphicType()) ||
          GRAPHIC_GDIMETAFILE == nGrfType ))
    {
        SfxItemSet aGrfSet( GetShell().GetAttrPool(), RES_GRFATR_BEGIN,
                                                      RES_GRFATR_END -1 );
        const SfxItemSet *pArgs = rReq.GetArgs();
        const SfxPoolItem* pItem;
        USHORT nSlot = rReq.GetSlot();
        if( !pArgs || SFX_ITEM_SET != pArgs->GetItemState( nSlot, FALSE, &pItem ))
            pItem = 0;

        switch( nSlot )
        {
        case FN_FLIP_VERT_GRAFIC:
        case FN_FLIP_HORZ_GRAFIC:
            {
                GetShell().GetAttr( aGrfSet );
                SwMirrorGrf aMirror( (SwMirrorGrf&)aGrfSet.Get(
                                                    RES_GRFATR_MIRRORGRF ) );
                USHORT nMirror = aMirror.GetValue();
                if( FN_FLIP_VERT_GRAFIC == nSlot )
                    switch( nMirror )
                    {
                    case RES_DONT_MIRROR_GRF:	nMirror = RES_MIRROR_GRF_VERT;
                                                break;
                    case RES_MIRROR_GRF_HOR:	nMirror = RES_MIRROR_GRF_BOTH;
                                                break;
                    case RES_MIRROR_GRF_VERT:	nMirror = RES_DONT_MIRROR_GRF;
                                                break;
                    case RES_MIRROR_GRF_BOTH:	nMirror = RES_MIRROR_GRF_HOR;
                                                break;
                    }
                else
                    switch( nMirror )
                    {
                    case RES_DONT_MIRROR_GRF:	nMirror = RES_MIRROR_GRF_HOR;
                                                break;
                    case RES_MIRROR_GRF_VERT:	nMirror = RES_MIRROR_GRF_BOTH;
                                                break;
                    case RES_MIRROR_GRF_HOR:	nMirror = RES_DONT_MIRROR_GRF;
                                                break;
                    case RES_MIRROR_GRF_BOTH:	nMirror = RES_MIRROR_GRF_VERT;
                                                break;
                    }
                aMirror.SetValue( nMirror );
                aGrfSet.ClearItem();
                aGrfSet.Put( aMirror );
            }
            break;

        case SID_ATTR_GRAF_LUMINANCE:
            if( pItem )
                aGrfSet.Put( SwLuminanceGrf(
                            ((SfxInt16Item*)pItem)->GetValue() ));
            break;
        case SID_ATTR_GRAF_CONTRAST:
            if( pItem )
                aGrfSet.Put( SwContrastGrf(
                            ((SfxInt16Item*)pItem)->GetValue() ));
            break;
        case SID_ATTR_GRAF_RED:
            if( pItem )
                aGrfSet.Put( SwChannelRGrf(
                            ((SfxInt16Item*)pItem)->GetValue() ));
            break;
        case SID_ATTR_GRAF_GREEN:
            if( pItem )
                aGrfSet.Put( SwChannelGGrf(
                            ((SfxInt16Item*)pItem)->GetValue() ));
            break;
        case SID_ATTR_GRAF_BLUE:
            if( pItem )
                aGrfSet.Put( SwChannelBGrf(
                            ((SfxInt16Item*)pItem)->GetValue() ));
            break;
        case SID_ATTR_GRAF_GAMMA:
            if( pItem )
                aGrfSet.Put( SwGammaGrf(
                            ((SfxUInt32Item*)pItem)->GetValue() / 100 ));
            break;
        case SID_ATTR_GRAF_TRANSPARENCE:
            if( pItem )
                aGrfSet.Put( SwTransparencyGrf(
                            ((SfxUInt16Item*)pItem)->GetValue() ));
            break;
        case SID_ATTR_GRAF_INVERT:
            if( pItem )
                aGrfSet.Put( SwInvertGrf(
                            ((SfxBoolItem*)pItem)->GetValue() ));
            break;

        case SID_ATTR_GRAF_MODE:
            if( pItem )
                aGrfSet.Put( SwDrawModeGrf(
                            ((SfxUInt16Item*)pItem)->GetValue() ));
            break;

        case SID_GRFFILTER:
        case SID_GRFFILTER_INVERT:
        case SID_GRFFILTER_SMOOTH:
        case SID_GRFFILTER_SHARPEN:
        case SID_GRFFILTER_REMOVENOISE:
        case SID_GRFFILTER_SOBEL:
        case SID_GRFFILTER_MOSAIC:
        case SID_GRFFILTER_EMBOSS:
        case SID_GRFFILTER_POSTER:
        case SID_GRFFILTER_POPART:
        case SID_GRFFILTER_SEPIA:
        case SID_GRFFILTER_SOLARIZE:
            if( GRAPHIC_BITMAP == nGrfType )
            {
                GraphicObject aFilterObj( GetShell().GetGraphicObj() );

                if( SVX_GRAPHICFILTER_ERRCODE_NONE ==
                    SvxGraphicFilter::ExecuteGrfFilterSlot( rReq, aFilterObj ))
                    GetShell().ReRead( aEmptyStr, aEmptyStr,
                                        &aFilterObj.GetGraphic() );
            }
            break;

        default:
            ASSERT(!this, falscher Dispatcher);
        }
        if( aGrfSet.Count() )
            GetShell().SetAttr( aGrfSet );
    }
    GetView().GetViewFrame()->GetBindings().Invalidate(rReq.GetSlot());
}

void SwGrfShell::GetAttrState(SfxItemSet &rSet)
{
    SwWrtShell &rSh = GetShell();
    SfxItemSet aCoreSet( GetPool(), aNoTxtNodeSetRange );
    rSh.GetAttr( aCoreSet );
    BOOL bParentCntProt = 0 != rSh.IsSelObjProtected(
                    (FlyProtectType)(FLYPROTECT_CONTENT|FLYPROTECT_PARENT) );
    BOOL bIsGrfCntnt = CNT_GRF == GetShell().GetCntType();
    BOOL bSwappedOut = rSh.IsGrfSwapOut( TRUE );
    BOOL bBitmapType = !bSwappedOut && GRAPHIC_BITMAP == rSh.GetGraphicType();

    SetGetStateSet( &rSet );

    SfxWhichIter aIter( rSet );
    USHORT nWhich = aIter.FirstWhich();
    while( nWhich )
    {
        BOOL bDisable = bParentCntProt;
        switch( nWhich )
        {
        case FN_FORMAT_GRAFIC_DLG:
            break;

        case FN_FLIP_VERT_GRAFIC:
            if( !bParentCntProt )
            {
                UINT16 nState = ((const SwMirrorGrf &) aCoreSet.Get(
                                        RES_GRFATR_MIRRORGRF )).GetValue();

                rSet.Put(SfxBoolItem( nWhich, nState == RES_MIRROR_GRF_VERT ||
                                              nState == RES_MIRROR_GRF_BOTH));
            }
            break;

        case FN_FLIP_HORZ_GRAFIC:
            if( !bParentCntProt )
            {
                UINT16 nState = ((const SwMirrorGrf &) aCoreSet.Get(
                                        RES_GRFATR_MIRRORGRF )).GetValue();

                rSet.Put(SfxBoolItem( nWhich, nState == RES_MIRROR_GRF_HOR ||
                                              nState == RES_MIRROR_GRF_BOTH));
            }
            break;


        case SID_ATTR_GRAF_LUMINANCE:
            if( !bParentCntProt )
                rSet.Put( SfxInt16Item( nWhich, ((SwLuminanceGrf&)
                        aCoreSet.Get(RES_GRFATR_LUMINANCE)).GetValue() ));
            break;
        case SID_ATTR_GRAF_CONTRAST:
            if( !bParentCntProt )
                rSet.Put( SfxInt16Item( nWhich, ((SwContrastGrf&)
                        aCoreSet.Get(RES_GRFATR_CONTRAST)).GetValue() ));
            break;
        case SID_ATTR_GRAF_RED:
            if( !bParentCntProt )
                rSet.Put( SfxInt16Item( nWhich, ((SwChannelRGrf&)
                        aCoreSet.Get(RES_GRFATR_CHANNELR)).GetValue() ));
            break;
        case SID_ATTR_GRAF_GREEN:
            if( !bParentCntProt )
                rSet.Put( SfxInt16Item( nWhich, ((SwChannelGGrf&)
                        aCoreSet.Get(RES_GRFATR_CHANNELG)).GetValue() ));
            break;
        case SID_ATTR_GRAF_BLUE:
            if( !bParentCntProt )
                rSet.Put( SfxInt16Item( nWhich, ((SwChannelBGrf&)
                        aCoreSet.Get(RES_GRFATR_CHANNELB)).GetValue() ));
            break;

        case SID_ATTR_GRAF_GAMMA:
            if( !bParentCntProt )
                rSet.Put( SfxUInt32Item( nWhich, ((SwGammaGrf&)
                        aCoreSet.Get(RES_GRFATR_GAMMA)).GetValue() * 100 ));
            break;
        case SID_ATTR_GRAF_TRANSPARENCE:
            if( !bParentCntProt )
            {
                const GraphicObject& rGrfObj = rSh.GetGraphicObj();
                if( rGrfObj.IsAnimated() ||
                    GRAPHIC_GDIMETAFILE == rGrfObj.GetType() )
                    bDisable = TRUE;
                else
                    rSet.Put( SfxUInt16Item( nWhich, ((SwTransparencyGrf&)
                        aCoreSet.Get(RES_GRFATR_TRANSPARENCY)).GetValue() ));
            }
            break;
        case SID_ATTR_GRAF_INVERT:
            if( !bParentCntProt )
                rSet.Put( SfxBoolItem( nWhich, ((SwInvertGrf&)
                        aCoreSet.Get(RES_GRFATR_INVERT)).GetValue() ));
            break;

        case SID_ATTR_GRAF_MODE:
            if( !bParentCntProt )
                rSet.Put( SfxUInt16Item( nWhich, (GraphicDrawMode)((SwDrawModeGrf&)
                        aCoreSet.Get(RES_GRFATR_DRAWMODE)).GetValue() ));
            break;

        case SID_GRFFILTER:
        case SID_GRFFILTER_INVERT:
        case SID_GRFFILTER_SMOOTH:
        case SID_GRFFILTER_SHARPEN:
        case SID_GRFFILTER_REMOVENOISE:
        case SID_GRFFILTER_SOBEL:
        case SID_GRFFILTER_MOSAIC:
        case SID_GRFFILTER_EMBOSS:
        case SID_GRFFILTER_POSTER:
        case SID_GRFFILTER_POPART:
        case SID_GRFFILTER_SEPIA:
        case SID_GRFFILTER_SOLARIZE:
            if( bParentCntProt || !bIsGrfCntnt )
                bDisable = TRUE;
            else if( bSwappedOut )
            {
                rSet.DisableItem( nWhich );
                if( AddGrfUpdateSlot( nWhich ))
                    rSh.GetGraphic(FALSE);  // start the loading
            }
            else
                bDisable = !bBitmapType;
            break;

        default:
            bDisable = FALSE;
        }

        if( bDisable )
            rSet.DisableItem( nWhich );
        nWhich = aIter.NextWhich();
    }
    SetGetStateSet( 0 );
}


SwGrfShell::SwGrfShell(SwView &rView) :
    SwBaseShell(rView)

{
    SetName(String::CreateFromAscii("Graphic"));
    SetHelpId(SW_GRFSHELL);
}




