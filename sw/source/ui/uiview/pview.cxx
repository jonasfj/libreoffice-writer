/*************************************************************************
 *
 *  $RCSfile: pview.cxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: os $ $Date: 2002-12-02 08:36:05 $
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


#ifndef _SV_TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_HELP_HXX //autogen
#include <vcl/help.hxx>
#endif
#ifndef _VCL_CMDEVT_HXX //autogen
#include <vcl/cmdevt.hxx>
#endif
#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _SV_PRINTDLG_HXX_ //autogen
#include <svtools/printdlg.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SFX_TOPFRM_HXX
#include <sfx2/topfrm.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVX_ZOOM_HXX
#include <svx/zoom.hxx>
#endif
#ifndef _SVX_STDDLG_HXX //autogen
#include <svx/stddlg.hxx>
#endif
#ifndef _SVX_PAPERINF_HXX //autogen
#include <svx/paperinf.hxx>
#endif
#ifndef _SVX_SRCHITEM_HXX //autogen
#include <svx/srchitem.hxx>
#endif
#ifndef _SVDVIEW_HXX //autogen
#include <svx/svdview.hxx>
#endif
#ifndef _SVX_DLGUTIL_HXX //autogen
#include <svx/dlgutil.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC //autogen
#include <svx/svxids.hrc>
#endif


#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif
#ifndef _SWGLOBDOCSH_HXX //autogen
#include <globdoc.hxx>
#endif
#ifndef _SWWDOCSH_HXX //autogen
#include <wdocsh.hxx>
#endif
#ifndef _PVPRTDAT_HXX
#include <pvprtdat.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _MODOPT_HXX //autogen
#include <modcfg.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PVIEW_HXX
#include <pview.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _TEXTSH_HXX
#include <textsh.hxx>
#endif
#ifndef _SCROLL_HXX
#include <scroll.hxx>
#endif
#ifndef _SWPRTOPT_HXX
#include <swprtopt.hxx>
#endif
#ifndef _DOCSTAT_HXX
#include <docstat.hxx>
#endif
#ifndef _USRPREF_HXX
#include <usrpref.hxx>
#endif
#ifndef _VIEWFUNC_HXX
#include <viewfunc.hxx>
#endif


#ifndef _HELPID_H
#include <helpid.h>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _POPUP_HRC
#include <popup.hrc>
#endif
#ifndef _PVIEW_HRC
#include <pview.hrc>
#endif

#define SwPagePreView
#ifndef _ITEMDEF_HXX
#include <itemdef.hxx>
#endif
#ifndef _SWSLOTS_HXX
#include <swslots.hxx>
#endif

// OD 02.12.2002 #103492#
#define NEW_PREVIEW

SFX_IMPL_VIEWFACTORY(SwPagePreView, SW_RES(STR_NONAME))
{
    SFX_VIEW_REGISTRATION(SwDocShell);
    SFX_VIEW_REGISTRATION(SwWebDocShell);
    SFX_VIEW_REGISTRATION(SwGlobalDocShell);
}

SFX_IMPL_INTERFACE(SwPagePreView, SfxViewShell, SW_RES(RID_PVIEW_TOOLBOX))
{
    SFX_POPUPMENU_REGISTRATION(SW_RES(MN_PPREVIEW_POPUPMENU));
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT|SFX_VISIBILITY_STANDARD|
                                SFX_VISIBILITY_CLIENT|SFX_VISIBILITY_FULLSCREEN|
                                SFX_VISIBILITY_READONLYDOC,
                                SW_RES(RID_PVIEW_TOOLBOX));
}


TYPEINIT1(SwPagePreView,SfxViewShell)

#define SWVIEWFLAGS ( SFX_VIEW_MAXIMIZE_FIRST|SFX_VIEW_OPTIMIZE_EACH|  \
                      SFX_VIEW_CAN_PRINT|SFX_VIEW_HAS_PRINTOPTIONS )

#define MIN_PREVIEW_ZOOM 25
#define MAX_PREVIEW_ZOOM 600
/*  */
/* -----------------26.11.2002 10:41-----------------
 *
 * --------------------------------------------------*/
USHORT lcl_GetNextZoomStep(USHORT nCurrentZoom, BOOL bZoomIn)
{
    static USHORT aZoomArr[] =
    {
        25, 50, 75, 100, 150, 200, 400, 600
    };
    if(bZoomIn)
        for(USHORT i = sizeof(aZoomArr) / sizeof(USHORT); i >= 0 ; i--)
        {
            if(nCurrentZoom > aZoomArr[i] || !i)
                return aZoomArr[i];
        }
    else
        for(USHORT i = 0; i < sizeof(aZoomArr) / sizeof(USHORT); i++)
        {
            if(nCurrentZoom < aZoomArr[i])
                return aZoomArr[i];
        }
    return bZoomIn ? MAX_PREVIEW_ZOOM : MIN_PREVIEW_ZOOM;
};
/* -----------------02.12.2002 09:11-----------------
 * 
 * --------------------------------------------------*/
void lcl_InvalidateZoomSlots(SfxBindings& rBindings)
{        
    static USHORT __READONLY_DATA aInval[] =
    {
        SID_ATTR_ZOOM, SID_ZOOM_OUT, SID_ZOOM_IN, FN_PREVIEW_ZOOM, FN_STAT_ZOOM, 
        0
    };
    rBindings.Invalidate( aInval );
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

// erstmal der Zoom-Dialog

class SwPreViewZoomDlg : public SvxStandardDialog
{
    FixedText		aRowLbl;
    NumericField 	aRowEdit;
    FixedText		aColLbl;
    NumericField 	aColEdit;

    OKButton	 	aOkBtn;
    CancelButton 	aCancelBtn;
    HelpButton 		aHelpBtn;

    virtual void 	Apply();

public:
    SwPreViewZoomDlg( SwPagePreViewWin& rParent );
    ~SwPreViewZoomDlg();
};

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwPreViewZoomDlg::SwPreViewZoomDlg( SwPagePreViewWin& rParent ) :
    SvxStandardDialog( &rParent, SW_RES(DLG_PAGEPREVIEW_ZOOM) ),
    aColLbl(this,SW_RES(FT_COL)),
    aColEdit(this,SW_RES(ED_COL)),
    aRowLbl(this,SW_RES(FT_ROW)),
    aRowEdit(this,SW_RES(ED_ROW)),
    aOkBtn(this,SW_RES(BT_OK)),
    aCancelBtn(this,SW_RES(BT_CANCEL)),
    aHelpBtn(this,SW_RES(BT_HELP))
{
    FreeResource();

    aRowEdit.SetValue( rParent.GetRow() );
    aColEdit.SetValue( rParent.GetCol() );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


 SwPreViewZoomDlg::~SwPreViewZoomDlg() {}


void  SwPreViewZoomDlg::Apply()
{
    ((SwPagePreViewWin*)GetParent())->CalcWish(
                BYTE(aRowEdit.GetValue()),
                BYTE(aColEdit.GetValue()) );
}

/*  */
/* -----------------20.08.98 15:25-------------------
 * Vorschau fuer den Seitendruck
 * --------------------------------------------------*/
struct PrintSettingsStruct;
class PrtPrvWindow : public Window
{
        const PrintSettingsStruct& 	rSettings;

        virtual void 	Paint(const Rectangle&);
public :
    PrtPrvWindow(Window* pParent, const ResId& rResId, const PrintSettingsStruct& 	rSett) :
        Window(pParent, rResId), rSettings(rSett){}
};
/*--------------------------------------------------------------------
    Beschreibung: Optionen fuer das Drucken der Preview
 --------------------------------------------------------------------*/
struct PrintSettingsStruct
{
    Size				aPageMaxSize;	// groesste Seite
    Size				aPrtSize;		// Papiergroesse
    Size				aPrvPrtSize;	// Groesse innerhalb der LRTB-Raender
    Size 				aGridSize;		// Groesse fuer jede Seite, enthaelt je
                                        // die Haelfte von H- und V-Distance

    long 				nLeft;
    long 				nRight;
    long				nTop;
    long 				nBottom;
    long 				nHori;
    long 				nVert;

    USHORT 				nRows;
    USHORT				nCols;

    BOOL 				bPrinterLandscape;		// Ausrichtung
};

class SwPreviewPrintOptionsDialog : public SvxStandardDialog
{
    FixedLine            aRowColFL;
    FixedText   		aRowsFT;
    NumericField 		aRowsNF;
    FixedText   		aColsFT;
    NumericField 		aColsNF;

    FixedLine            aMarginFL;
    FixedText			aLSpaceFT;
    MetricField 		aLSpaceMF;
    FixedText   		aRSpaceFT;
    MetricField 		aRSpaceMF;
    FixedText   		aTSpaceFT;
    MetricField 		aTSpaceMF;
    FixedText   		aBSpaceFT;
    MetricField 		aBSpaceMF;
    FixedLine            aDistanceFL;
    FixedText   		aHSpaceFT;
    MetricField 		aHSpaceMF;
    FixedText   		aVSpaceFT;
    MetricField 		aVSpaceMF;

    RadioButton			aLandscapeRB;
    RadioButton			aPortraitRB;
    FixedLine            aOrientationFL;

    PrtPrvWindow 		aPreviewWin;

    OKButton	 		aOkBtn;
    CancelButton 		aCancelBtn;
    HelpButton 			aHelpBtn;

    PushButton 			aStandardPB;

    SwPagePreView&		rPreView;
    PrintSettingsStruct aSettings;
/*	Size				aPageMaxSize;
    Size				aPrtSize;

    BOOL 				bOrientation;
*/
    BOOL 				bStandard;

    virtual void 		Apply();
    void				FillControls(SwPagePreViewPrtData& rData);

            DECL_LINK( ModifyHdl, Edit* );
            DECL_LINK( StandardHdl, PushButton* );

public:
    SwPreviewPrintOptionsDialog(SwPagePreViewWin& rParent, SwPagePreView&	rView);
    ~SwPreviewPrintOptionsDialog();
};

/* -----------------19.08.98 13:35-------------------
 *
 * --------------------------------------------------*/
SwPreviewPrintOptionsDialog::SwPreviewPrintOptionsDialog( SwPagePreViewWin& rParent, SwPagePreView&	rView ) :
    SvxStandardDialog( &rParent, SW_RES(DLG_PAGEPREVIEW_PRINTOPTIONS) ),
    aRowColFL(this,SW_RES(  FL_ROWCOL)),
    aRowsFT(this,SW_RES(	FT_ROWS)),
    aRowsNF(this,SW_RES(	NF_ROWS)),
    aColsFT(this,SW_RES(	FT_COLS)),
    aColsNF(this,SW_RES(	NF_COLS)),
    aMarginFL(this,SW_RES(  FL_MARGINS)),
    aLSpaceFT(this,SW_RES(	FT_LMARGIN)),
    aLSpaceMF(this,SW_RES(	MF_LMARGIN)),
    aRSpaceFT(this,SW_RES(	FT_RMARGIN)),
    aRSpaceMF(this,SW_RES(	MF_RMARGIN)),
    aTSpaceFT(this,SW_RES(	FT_TMARGIN)),
    aTSpaceMF(this,SW_RES(	MF_TMARGIN)),
    aBSpaceFT(this,SW_RES(	FT_BMARGIN)),
    aBSpaceMF(this,SW_RES(	MF_BMARGIN)),
    aDistanceFL(this,SW_RES(FL_DISTANCE)),
    aHSpaceFT(this,SW_RES(	FT_HMARGIN)),
    aHSpaceMF(this,SW_RES(	MF_HMARGIN)),
    aVSpaceFT(this,SW_RES(	FT_VMARGIN)),
    aVSpaceMF(this,SW_RES(	MF_VMARGIN)),
    aOrientationFL(this,SW_RES( FL_ORIENTATION)),
    aLandscapeRB(this,SW_RES(	RB_LANDSCAPE)),
    aPortraitRB(this,SW_RES(	RB_PORTRAIT)),
    aPreviewWin(this,SW_RES(	WIN_PREVIEW), aSettings),
    aOkBtn(this,SW_RES(BT_OK)),
    aCancelBtn(this,SW_RES(BT_CANCEL)),
    aHelpBtn(this,SW_RES(BT_HELP)),
    aStandardPB(this,SW_RES(PB_STANDARD)),
    rPreView(rView),
    bStandard(TRUE)
{
    FreeResource();
    ViewShell& rViewSh = rPreView.GetViewShell();
    aSettings.aPageMaxSize = rViewSh.GetPagePreViewPrtMaxSize();
    SfxPrinter*  pPrinter = rViewSh.GetPrt( TRUE );
    aSettings.aPrtSize = pPrinter->GetPaperSize();
    //#97682# make sure that no division by zero occurs
    if(!aSettings.aPrtSize.Width() || !aSettings.aPrtSize.Height())
        aSettings.aPrtSize = Size(lA4Width, lA4Height);
    aSettings.bPrinterLandscape = pPrinter->GetOrientation() == ORIENTATION_LANDSCAPE;


    SwDocShell* pDocShell = rPreView.GetDocShell();
    const SwMasterUsrPref *pUsrPref = SW_MOD()->GetUsrPref(0 != PTR_CAST(SwWebDocShell, pDocShell));
    FieldUnit eFieldUnit = pUsrPref->GetMetric();
    ::SetFieldUnit( aLSpaceMF, eFieldUnit );
    ::SetFieldUnit( aRSpaceMF, eFieldUnit );
    ::SetFieldUnit( aTSpaceMF, eFieldUnit );
    ::SetFieldUnit( aBSpaceMF, eFieldUnit );
    ::SetFieldUnit( aHSpaceMF, eFieldUnit );
    ::SetFieldUnit( aVSpaceMF, eFieldUnit );

    SwDoc* pDoc = pDocShell->GetDoc();
    SwPagePreViewPrtData aData;
    if(pDoc->GetPreViewPrtData())
    {
        aData = *pDoc->GetPreViewPrtData();
        bStandard = FALSE;
    }
    else
    {
        // Orientation der PreviewData an den Drucker anpassen
        aData.SetLandscape(aSettings.bPrinterLandscape);
    }
    FillControls(aData);

    aLSpaceMF.SaveValue();
    aRSpaceMF.SaveValue();
    aTSpaceMF.SaveValue();
    aBSpaceMF.SaveValue();
    aHSpaceMF.SaveValue();
    aVSpaceMF.SaveValue();
    aRowsNF.SaveValue();
    aColsNF.SaveValue();
    aLandscapeRB.SaveValue();
    aPortraitRB.SaveValue();

    aStandardPB.SetClickHdl(LINK(this, SwPreviewPrintOptionsDialog, StandardHdl));
    Link aLk = LINK(this, SwPreviewPrintOptionsDialog, ModifyHdl);
    aLSpaceMF.SetUpHdl(aLk);
    aRSpaceMF.SetUpHdl(aLk);
    aTSpaceMF.SetUpHdl(aLk);
    aBSpaceMF.SetUpHdl(aLk);
    aHSpaceMF.SetUpHdl(aLk);
    aVSpaceMF.SetUpHdl(aLk);
    aRowsNF.SetUpHdl(aLk);
    aColsNF.SetUpHdl(aLk);
    aLSpaceMF.SetDownHdl(aLk);
    aRSpaceMF.SetDownHdl(aLk);
    aTSpaceMF.SetDownHdl(aLk);
    aBSpaceMF.SetDownHdl(aLk);
    aHSpaceMF.SetDownHdl(aLk);
    aVSpaceMF.SetDownHdl(aLk);
    aRowsNF.SetDownHdl(aLk);
    aColsNF.SetDownHdl(aLk);
    aLSpaceMF.SetLoseFocusHdl(aLk);
    aRSpaceMF.SetLoseFocusHdl(aLk);
    aTSpaceMF.SetLoseFocusHdl(aLk);
    aBSpaceMF.SetLoseFocusHdl(aLk);
    aHSpaceMF.SetLoseFocusHdl(aLk);
    aVSpaceMF.SetLoseFocusHdl(aLk);
    aRowsNF.SetLoseFocusHdl(aLk);
    aColsNF.SetLoseFocusHdl(aLk);
    aLandscapeRB.SetClickHdl(aLk);
    aPortraitRB.SetClickHdl(aLk);

    ModifyHdl(0);
    if(bStandard)
        StandardHdl(&aStandardPB);
}


/* -----------------19.08.98 13:36-------------------
 *
 * --------------------------------------------------*/
SwPreviewPrintOptionsDialog::~SwPreviewPrintOptionsDialog()
{
}
/* -----------------12.11.98 11:32-------------------
 *
 * --------------------------------------------------*/
void	SwPreviewPrintOptionsDialog::FillControls(SwPagePreViewPrtData& rData)
{
    aLSpaceMF.SetValue(aLSpaceMF.Normalize(rData.GetLeftSpace()  ), FUNIT_TWIP);
    aRSpaceMF.SetValue(aRSpaceMF.Normalize(rData.GetRightSpace() ), FUNIT_TWIP);
    aTSpaceMF.SetValue(aTSpaceMF.Normalize(rData.GetTopSpace()   ), FUNIT_TWIP);
    aBSpaceMF.SetValue(aBSpaceMF.Normalize(rData.GetBottomSpace()), FUNIT_TWIP);
    aHSpaceMF.SetValue(aHSpaceMF.Normalize(rData.GetHorzSpace()  ), FUNIT_TWIP);
    aVSpaceMF.SetValue(aVSpaceMF.Normalize(rData.GetVertSpace()  ), FUNIT_TWIP);
    aRowsNF.SetValue(rData.GetRow());
    aColsNF.SetValue(rData.GetCol());
    aSettings.bPrinterLandscape ? aLandscapeRB.Check() : aPortraitRB.Check();
    // wenn Drucker und Einstellungen nicht uebereinstimmen, dann Seiten tauschen
    if( rData.GetLandscape() != aSettings.bPrinterLandscape )
    {
        Size aTmp(aSettings.aPrtSize.Height(), aSettings.aPrtSize.Width());
        aSettings.aPrtSize = aTmp;
        aSettings.bPrinterLandscape = !aSettings.bPrinterLandscape;
        // nochmal setzen, denn auch wenn nur die Default-Orientierung dem Drucker
        // angepasst wurde, sollen die Einstellungen gespeichert werden
        aSettings.bPrinterLandscape ? aLandscapeRB.Check() : aPortraitRB.Check();
    }
    aLandscapeRB.SaveValue();
    aPortraitRB.SaveValue();

    aSettings.nLeft		= rData.GetLeftSpace()  ;
    aSettings.nRight    = rData.GetRightSpace() ;
    aSettings.nTop      = rData.GetTopSpace()   ;
    aSettings.nBottom   = rData.GetBottomSpace();
    aSettings.nHori     = rData.GetHorzSpace()  ;
    aSettings.nVert     = rData.GetVertSpace()  ;
    aSettings.nRows	    = rData.GetRow()        ;
    aSettings.nCols     = rData.GetCol()        ;
    aSettings.aPrvPrtSize = aSettings.aPrtSize	;
}

/* -----------------19.08.98 14:31-------------------
 *
 * --------------------------------------------------*/
void SwPreviewPrintOptionsDialog::Apply()
{

    SwDoc* pDoc = rPreView.GetDocShell()->GetDoc();
    if(bStandard)
        pDoc->SetPreViewPrtData(0);
    else if(	aLSpaceMF.GetSavedValue() != aLSpaceMF.GetText() ||
        aRSpaceMF.GetSavedValue() != aRSpaceMF.GetText() ||
        aTSpaceMF.GetSavedValue() != aTSpaceMF.GetText() ||
        aBSpaceMF.GetSavedValue() != aBSpaceMF.GetText() ||
        aHSpaceMF.GetSavedValue() != aHSpaceMF.GetText() ||
        aVSpaceMF.GetSavedValue() != aVSpaceMF.GetText() ||
        aRowsNF.GetSavedValue() != aRowsNF.GetText() ||
        aColsNF.GetSavedValue() != aColsNF.GetText() ||
        aLandscapeRB.GetSavedValue() != aLandscapeRB.IsChecked() ||
        aPortraitRB.GetSavedValue() != aPortraitRB.IsChecked() )
    {
        SwPagePreViewPrtData aData;
        if(pDoc->GetPreViewPrtData())
            aData = *pDoc->GetPreViewPrtData();
        aData.SetLeftSpace(aLSpaceMF.Denormalize(aLSpaceMF.GetValue(FUNIT_TWIP)));
        aData.SetRightSpace(aRSpaceMF.Denormalize(aRSpaceMF.GetValue(FUNIT_TWIP)));
        aData.SetTopSpace(aTSpaceMF.Denormalize(aTSpaceMF.GetValue(FUNIT_TWIP)));
        aData.SetBottomSpace(aBSpaceMF.Denormalize(aBSpaceMF.GetValue(FUNIT_TWIP)));

        aData.SetHorzSpace(aHSpaceMF.Denormalize(aHSpaceMF.GetValue(FUNIT_TWIP)));
        aData.SetVertSpace(aVSpaceMF.Denormalize(aVSpaceMF.GetValue(FUNIT_TWIP)));
        aData.SetRow((BYTE)aRowsNF.GetValue());
        aData.SetCol((BYTE)aColsNF.GetValue());
        aData.SetLandscape(aLandscapeRB.IsChecked());

        ViewShell& rViewSh = rPreView.GetViewShell();
        SfxPrinter*  pPrinter = rViewSh.GetPrt( TRUE );
        if((pPrinter->GetOrientation() == ORIENTATION_LANDSCAPE)
                        != aData.GetLandscape())
                    pPrinter->SetOrientation(aData.GetLandscape() ? ORIENTATION_LANDSCAPE : ORIENTATION_PORTRAIT);


        pDoc->SetPreViewPrtData(&aData);

    }
}
/* -----------------20.08.98 08:48-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwPreviewPrintOptionsDialog, ModifyHdl, Edit*, pEdit )
{
    if(bStandard && pEdit)
    {
        aLSpaceMF.SetUserValue(aLSpaceMF.GetValue());
        aRSpaceMF.SetUserValue(aRSpaceMF.GetValue());
        aTSpaceMF.SetUserValue(aTSpaceMF.GetValue());
        aBSpaceMF.SetUserValue(aBSpaceMF.GetValue());
        aHSpaceMF.SetUserValue(aHSpaceMF.GetValue());
        aVSpaceMF.SetUserValue(aVSpaceMF.GetValue());
        aRowsNF.  SetUserValue(aRowsNF  .GetValue());
        aColsNF.  SetUserValue(aColsNF  .GetValue());

        bStandard = FALSE;
    }
    BOOL bOrientChanged = aSettings.bPrinterLandscape != aLandscapeRB.IsChecked();;
    if(pEdit == &aLSpaceMF)
        aSettings.nLeft		= aLSpaceMF.Denormalize(aLSpaceMF.GetValue(FUNIT_TWIP));
    else if(pEdit == &aRSpaceMF)
        aSettings.nRight    = aRSpaceMF.Denormalize(aRSpaceMF.GetValue(FUNIT_TWIP));
    else if(pEdit == &aTSpaceMF)
        aSettings.nTop      = aTSpaceMF.Denormalize(aTSpaceMF.GetValue(FUNIT_TWIP));
    else if(pEdit == &aBSpaceMF)
        aSettings.nBottom   = aBSpaceMF.Denormalize(aBSpaceMF.GetValue(FUNIT_TWIP));
    else if(pEdit == &aHSpaceMF)
        aSettings.nHori     = aHSpaceMF.Denormalize(aHSpaceMF.GetValue(FUNIT_TWIP));
    else if(pEdit == &aVSpaceMF)
        aSettings.nVert     = aVSpaceMF.Denormalize(aVSpaceMF.GetValue(FUNIT_TWIP));
    else if(pEdit == &aRowsNF)
        aSettings.nRows	= (USHORT)aRowsNF.GetValue();
    else if(pEdit == &aColsNF)
        aSettings.nCols = (USHORT)aColsNF.GetValue();
    else if(pEdit == (Edit*)&aLandscapeRB)
        aSettings.bPrinterLandscape = aLandscapeRB.IsChecked();
    else if(pEdit == (Edit*)&aPortraitRB)
        aSettings.bPrinterLandscape = aLandscapeRB.IsChecked();

    if(bOrientChanged)
    {
        Size aTmp(aSettings.aPrtSize.Height(), aSettings.aPrtSize.Width());
        aSettings.aPrtSize = aTmp;
    }


    aSettings.aPrvPrtSize = Size(aSettings.aPrtSize.Width()  - aSettings.nRight - aSettings.nLeft,
                            aSettings.aPrtSize.Height() - aSettings.nTop - aSettings.nBottom);
    aSettings.aGridSize = Size(aSettings.aPrvPrtSize.Width() / aSettings.nCols,
                                aSettings.aPrvPrtSize.Height() / aSettings.nRows	);
    // was fehlt noch: Orientation auswerten, minimalrand ausrechnen, Beispiel fuettern


    //am Ende Maximalwerte setzen
    long n20Percent = aSettings.aPrtSize.Width() / 5;
    aLSpaceMF.SetMax(aLSpaceMF.Normalize(aSettings.aPrtSize.Width() - aSettings.nRight - n20Percent), FUNIT_TWIP);
    aRSpaceMF.SetMax(aRSpaceMF.Normalize(aSettings.aPrtSize.Width() - aSettings.nLeft - n20Percent), FUNIT_TWIP);
    n20Percent = aSettings.aPrtSize.Height() / 5;
    aTSpaceMF.SetMax(aTSpaceMF.Normalize(aSettings.aPrtSize.Height() - aSettings.nBottom - n20Percent), FUNIT_TWIP);
    aBSpaceMF.SetMax(aBSpaceMF.Normalize(aSettings.aPrtSize.Height() - aSettings.nTop - n20Percent), FUNIT_TWIP);

    long n80Percent = aSettings.aPrvPrtSize.Width() * 4 / 5;
    aHSpaceMF.SetMax(aHSpaceMF.Normalize(n80Percent / aSettings.nRows), FUNIT_TWIP);
    n80Percent = aSettings.aPrvPrtSize.Height()* 4 / 5;
    aVSpaceMF.SetMax(aVSpaceMF.Normalize(n80Percent / aSettings.nCols), FUNIT_TWIP);
    aHSpaceMF.Enable(aSettings.nCols > 1);
    aVSpaceMF.Enable(aSettings.nRows > 1);
    aRowsNF.SetMin(1);// nur damit auch nach Standard wieder der Inhalt angezeigt wird
    aColsNF.SetMin(1);


    aPreviewWin.Invalidate();
    return 0;
}
/* -----------------28.08.98 14:59-------------------
 *
 * --------------------------------------------------*/
IMPL_LINK( SwPreviewPrintOptionsDialog, StandardHdl, PushButton*, EMPTYARG )
{
    SetUpdateMode(TRUE);
    SwPagePreViewPrtData aData;
    FillControls(aData);
    bStandard = TRUE;
    aLSpaceMF.SetText(aEmptyStr);
    aRSpaceMF.SetText(aEmptyStr);
    aTSpaceMF.SetText(aEmptyStr);
    aBSpaceMF.SetText(aEmptyStr);
    aHSpaceMF.SetText(aEmptyStr);
    aVSpaceMF.SetText(aEmptyStr);
    aRowsNF.SetText(aEmptyStr);
    aColsNF.SetText(aEmptyStr);
    aPreviewWin.Invalidate();
    SetUpdateMode(FALSE);
    return 0;
}
/* -----------------20.08.98 15:28-------------------
 * Preview anzeigen
 * --------------------------------------------------*/
void 	PrtPrvWindow::Paint(const Rectangle&)
{
        Size aWinSize(GetOutputSizePixel());
        long nWidth = rSettings.aPrtSize.Width();
        long nHeight = rSettings.aPrtSize.Height();
        BOOL bHoriValid = (aWinSize.Width() * 100 / aWinSize.Height()) <
                    (rSettings.aPrtSize.Width() * 100/ rSettings.aPrtSize.Height());
        Fraction aXScale( aWinSize.Width(), Max( nWidth , 1L ) );
        Fraction aYScale( aWinSize.Height(), Max( nHeight, 1L ) );
        MapMode aMapMode( GetMapMode() );
        aMapMode.SetScaleX( bHoriValid ? aXScale : aYScale);
        aMapMode.SetScaleY( bHoriValid ? aXScale : aYScale);
        SetMapMode( aMapMode );

        aWinSize = GetOutputSize();

        Point aOffset(0,0);
        if(bHoriValid)
            aOffset.Y() = (aWinSize.Height() - rSettings.aPrtSize.Height()) / 2;
        else
            aOffset.X() = (aWinSize.Width() - rSettings.aPrtSize.Width()) / 2;;


        BOOL bAccessibleColors = SvtAccessibilityOptions().GetIsForPagePreviews();

        //der weisse Seitenhintergrund
        Rectangle aRect(aOffset, rSettings.aPrtSize);
        if(bAccessibleColors)
        {
            SetFillColor( GetSettings().GetStyleSettings().GetWindowColor() );
            SetLineColor( GetSettings().GetStyleSettings().GetWindowTextColor() );
        }
        else
        {
            SetFillColor( Color( COL_WHITE ) );
            SetLineColor(Color( COL_BLACK ) );
        }

        DrawRect(aRect);

        Point aTL(aOffset);
        aTL.X() += rSettings.nLeft;
        aTL.Y() += rSettings.nTop;

        Size aPrvPageSize((rSettings.aPrvPrtSize.Width() - (rSettings.nCols - 1) * rSettings.nHori) / rSettings.nCols,
            (rSettings.aPrvPrtSize.Height() - (rSettings.nRows - 1) * rSettings.nVert) / rSettings.nRows);
        // jetzt muss noch das unterschiedliche Groessenverhaeltnis berechnet werden, um es an
        // der richtigen Seite abzuziehen.
        //...
        long nSourceScale = rSettings.aPageMaxSize.Width() * 100 / rSettings.aPageMaxSize.Height();
        long nDestScale = aPrvPageSize.Width() * 100  / aPrvPageSize.Height() ;
        if(nSourceScale > nDestScale) // die Seite ist relativ breiter als das vorhandene Rechteck
        {
            aPrvPageSize.Height() = aPrvPageSize.Width() * 100 / nSourceScale;
        }
        else
        {
            aPrvPageSize.Width() = aPrvPageSize.Height() * nSourceScale / 100;
        }

        if(bAccessibleColors)
            SetFillColor( GetSettings().GetStyleSettings().GetShadowColor() );
        else
            SetFillColor( Color( COL_GRAY ) );


        aRect = Rectangle(aTL, aPrvPageSize);
        for(USHORT i = 0; i < rSettings.nRows; i++)
        {
            for(USHORT j = 0; j < rSettings.nCols; j++)
            {
                DrawRect(aRect);
                aRect.Move(aPrvPageSize.Width() + rSettings.nHori, 0);
            }

            aRect.Move( 0, aPrvPageSize.Height() + rSettings.nVert);
            aRect.SetPos(Point(aTL.X(), aRect.TopLeft().Y()));
        }
    //rSettings;
}

/*  */
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

// alles fuers SwPagePreViewWin


SwPagePreViewWin::SwPagePreViewWin( Window *pParent, SwPagePreView& rPView )
    : Window( pParent, WinBits( WB_CLIPCHILDREN) ),
    pViewShell( 0 ),
    rView( rPView )
{
    SetHelpId(HID_PAGEPREVIEW);
    SetFillColor( GetBackground().GetColor() );
    SetLineColor( GetBackground().GetColor());
    SetMapMode( MapMode(MAP_TWIP) );

    const SwMasterUsrPref *pUsrPref = SW_MOD()->GetUsrPref(FALSE);
    nRow = pUsrPref->GetPagePrevRow();		// 1 Zeile
    nCol = pUsrPref->GetPagePrevCol();		// 1 Spalte
    nVirtPage = nSttPage = nSelectedPage = USHRT_MAX;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwPagePreViewWin::~SwPagePreViewWin()
{
    if( pViewShell )
        delete pViewShell;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreViewWin::Paint( const Rectangle& rRect )
{
    if( !pViewShell || !pViewShell->GetLayout() )
        return;

    USHORT nRowCol = ( nRow << 8 ) + nCol;
    if( USHRT_MAX == nSttPage )		// wurde noch nie berechnet ? (Init-Phase!)
    {
        // das ist die Size, auf die ich mich immer beziehe
        if( !aWinSize.Height() || !aWinSize.Width() )
            aWinSize = GetOutputSizePixel();

        Rectangle aRect( LogicToPixel( rRect ));
#ifdef NEW_PREVIEW // OD 29.11.2002 #103492#
        {
            nRow = BYTE( nRowCol >> 8 );
            nCol = BYTE( nRowCol & 0xff );
            Size aPreviewDocSize;
            pViewShell->InitPreviewLayout( nCol, nRow, aPreviewDocSize,
                                           true, &aWinSize );
            aPgSize = aPreviewDocSize; // for testing
            Point aAbsStartPt;
            pViewShell->PreparePreviewPaint( 1, Point(0,0),
                                             nSttPage, nVirtPage, aAbsStartPt );
            nSelectedPage = nSttPage;
            pViewShell->PaintPreview( nSelectedPage, PixelToLogic( aRect ) );
        }
#else
        nSelectedPage = nSttPage = pViewShell->CalcPreViewPage( aWinSize, nRowCol,
                                                0, aPgSize, nVirtPage,
                                                nSelectedPage );
        pViewShell->PreViewPage( PixelToLogic( aRect ), nRowCol, nSttPage,
                                    aPgSize, nSelectedPage );
        nRow = BYTE( nRowCol >> 8 );
        nCol = BYTE( nRowCol & 0xff );
#endif
        SetPagePreview(nRow, nCol);
        aScale = GetMapMode().GetScaleX();
    }
    else
    {
        MapMode aMM( GetMapMode() );
        aMM.SetScaleX( aScale );
        aMM.SetScaleY( aScale );
        SetMapMode( aMM );
#ifdef NEW_PREVIEW // OD 29.11.2002 #103492#
        pViewShell->PaintPreview( nSelectedPage, rRect );
#else
        pViewShell->PreViewPage( rRect, nRowCol, nSttPage, aPgSize, nSelectedPage );
#endif
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void SwPagePreViewWin::CalcWish( BYTE nNewRow, BYTE nNewCol )
{
    if( !pViewShell || !pViewShell->GetLayout() )
        return;

    USHORT nOldCol = nCol;
    USHORT nRowCol = ( nNewRow << 8 ) + nNewCol;	// Zeilen / Spalten
    USHORT nPages = nNewRow * nNewCol,
           nLastSttPg = rView.GetPageCount()+1 > nPages
                            ? rView.GetPageCount()+1 - nPages : 0;
    if( nSttPage > nLastSttPg )
        nSttPage = nLastSttPg;

#ifdef NEW_PREVIEW // OD 29.11.2002 #103492#
    {
        nRow = BYTE( nRowCol >> 8 );
        nCol = BYTE( nRowCol & 0xff );
        Size aPreviewDocSize;
        pViewShell->InitPreviewLayout( nCol, nRow, aPreviewDocSize,
                                       true, &aWinSize );
        aPgSize = aPreviewDocSize; // for testing
        Point aAbsStartPt;
        pViewShell->PreparePreviewPaint( 1, Point(0,0),
                                         nSttPage, nVirtPage, aAbsStartPt );
        nSelectedPage = nSttPage;
    }
#else
    nSelectedPage = nSttPage = pViewShell->CalcPreViewPage( aWinSize, nRowCol, nSttPage,
                                            aPgSize, nVirtPage, USHRT_MAX );
    nRow = BYTE( nRowCol >> 8 );
    nCol = BYTE( nRowCol & 0xff );
#endif
    SetPagePreview(nRow, nCol);
    aScale = GetMapMode().GetScaleX();

    // falls an der Spaltigkeit gedreht wurde, so muss der Sonderfall
    // Einspaltig beachtet und ggfs. der Scrollbar korrigiert werden
    if( (1 == nOldCol) ^ (1 == nCol) )
        rView.VScrollDocSzChg();

    // Sortierung muss eingehalten werden!!
    static USHORT __READONLY_DATA aInval[] =
    {
        SID_ATTR_ZOOM, SID_ZOOM_OUT, SID_ZOOM_IN, 
        FN_PREVIEW_ZOOM, 
        FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN, 
        FN_STAT_ZOOM, 
        FN_SHOW_TWO_PAGES, FN_SHOW_FOUR_PAGES, 
        0
    };
    SfxBindings& rBindings = rView.GetViewFrame()->GetBindings();
    rBindings.Invalidate( aInval );
    rBindings.Update( FN_SHOW_TWO_PAGES );
    rBindings.Update( FN_SHOW_FOUR_PAGES );
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


int SwPagePreViewWin::MovePage( int eMoveMode )
{
    // soviele Seiten hoch
    USHORT nPages = nRow * nCol;
    USHORT nNewSttPage = nSttPage;
    USHORT nPageCount = rView.GetPageCount() + 1;
    USHORT nDefSttPg = GetDefSttPage();
    USHORT nLastSttPg = nPageCount > nPages ? nPageCount - nPages : nDefSttPg;

    switch( eMoveMode )
    {
    case MV_PAGE_UP: 	nNewSttPage = nSttPage > nPages
                                            ? nSttPage - nPages : nDefSttPg;
                        nSelectedPage -= nPages;
                        break;
    case MV_PAGE_DOWN:	nNewSttPage = nSttPage + nPages;
                        if( nNewSttPage > nLastSttPg )
                            nNewSttPage = nLastSttPg;
                        nSelectedPage += nPages;
                        break;
    case MV_DOC_STT:    nNewSttPage = nDefSttPg;
                        nSelectedPage = nNewSttPage ? nNewSttPage : 1;
                        break;
    case MV_DOC_END:	nNewSttPage = nLastSttPg;
                        nSelectedPage = nPageCount - 1;
                        break;
    default:

        if( nNewSttPage > nLastSttPg )
            nNewSttPage = nLastSttPg;

        //put the the selected page into the current range
        if(nSelectedPage < nNewSttPage || nSelectedPage >= (nNewSttPage + nPages) )
            nSelectedPage = nNewSttPage ? nNewSttPage : 1;
    }

    Size aSave( aPgSize );
    USHORT nRowCol = ( nRow << 8 ) + nCol;	// Zeilen / DoppelSeiten
#ifdef NEW_PREVIEW // OD 29.11.2002 #103492#
    {
        nRow = BYTE( nRowCol >> 8 );
        nCol = BYTE( nRowCol & 0xff );
        Size aPreviewDocSize;
        pViewShell->InitPreviewLayout( nCol, nRow, aPreviewDocSize,
                                       true, &aWinSize );
        aPgSize = aPreviewDocSize; // for testing
        Point aAbsStartPt;
        pViewShell->PreparePreviewPaint( nNewSttPage, Point(0,0),
                                         nNewSttPage, nVirtPage, aAbsStartPt );
        nSelectedPage = nNewSttPage;
    }
#else
    nNewSttPage = pViewShell->CalcPreViewPage( aWinSize, nRowCol,
                                            nNewSttPage, aPgSize, nVirtPage,
                                            nSelectedPage );
#endif
    if( nNewSttPage == nSttPage && aPgSize == aSave )
        return FALSE;

    nRow = BYTE( nRowCol >> 8 );
    nCol = BYTE( nRowCol & 0xff );
    SetPagePreview(nRow, nCol);
    nSttPage = nNewSttPage;
    aScale = GetMapMode().GetScaleX();

    static USHORT __READONLY_DATA aInval[] =
    {
        FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN, 0
    };

    SfxBindings& rBindings = rView.GetViewFrame()->GetBindings();
    rBindings.Invalidate( aInval );

    return TRUE;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreViewWin::SetWinSize( const Size& rNewSize )
{
    // die Size wollen wir aber immer in Pixel-Einheiten haben
    aWinSize = LogicToPixel( rNewSize );
    USHORT nRowCol = ( nRow << 8 ) + nCol;	// Zeilen / DoppelSeiten

    if( USHRT_MAX == nSttPage )
        nSelectedPage = nSttPage = GetDefSttPage();

#ifdef NEW_PREVIEW // OD 29.11.2002 #103492#
    {
        nRow = BYTE( nRowCol >> 8 );
        nCol = BYTE( nRowCol & 0xff );
        Size aPreviewDocSize;
        pViewShell->InitPreviewLayout( nCol, nRow, aPreviewDocSize,
                                       true, &aWinSize );
        aPgSize = aPreviewDocSize; // for testing
        Point aAbsStartPt;
        pViewShell->PreparePreviewPaint( nSttPage, Point(0,0),
                                         nSttPage, nVirtPage, aAbsStartPt );
        nSelectedPage = nSttPage;
    }
#else
    nSttPage = pViewShell->CalcPreViewPage( aWinSize, nRowCol,
                                            nSttPage, aPgSize,
                                            nVirtPage, nSelectedPage );
#endif
    if(nSelectedPage < nSttPage || nSelectedPage > nSttPage + (nRow * nCol) )
    {
        nSelectedPage = nSttPage ? nSttPage : 1;
    }
    nRow = BYTE( nRowCol >> 8 );
    nCol = BYTE( nRowCol & 0xff );
    SetPagePreview(nRow, nCol);
    aScale = GetMapMode().GetScaleX();
}


/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreViewWin::GetStatusStr( String& rStr, USHORT nPageCnt ) const
{
    // Logische Seite gegebenenfalls davor haengen
    USHORT nStt = nSttPage > 1 ? nSttPage : 1;
    if( nVirtPage && nVirtPage != nStt )
    {
        rStr += String::CreateFromInt32( nVirtPage );
        rStr += ' ';
    }
    rStr += String::CreateFromInt32( nStt );
    rStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM(" / "));
    rStr += String::CreateFromInt32( nPageCnt );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreViewWin::KeyInput( const KeyEvent &rKEvt )
{
    //Defaultbehandlung Keyboard
    if( !rView.KeyInput( rKEvt ) )
        Window::KeyInput( rKEvt );
}

/******************************************************************************
 *	Beschreibung:
 ******************************************************************************/

void SwPagePreViewWin::Command( const CommandEvent& rCEvt )
{
    BOOL bCallBase = TRUE;
    switch( rCEvt.GetCommand() )
    {
    case COMMAND_CONTEXTMENU:
        rView.GetViewFrame()->GetDispatcher()->ExecutePopup();
        bCallBase = FALSE;
        break;

    case COMMAND_WHEEL:
    case COMMAND_STARTAUTOSCROLL:
    case COMMAND_AUTOSCROLL:
        bCallBase = !rView.HandleWheelCommands( rCEvt );
        break;

    default:	ASSERT( !this, "unknown command." );
    }

    if( bCallBase )
        Window::Command( rCEvt );
}

void SwPagePreViewWin::MouseButtonDown( const MouseEvent& rMEvt )
{
    if( MOUSE_LEFT == ( rMEvt.GetModifier() + rMEvt.GetButtons() ) &&
        2 == rMEvt.GetClicks() )
    {
        Point aDocPos( PixelToLogic( rMEvt.GetPosPixel() ) );

        // was habe wir denn hier getroffen??
        USHORT nRowCol = ( nRow << 8 ) + nCol;	// Zeilen / DoppelSeiten
        BOOL bDocPos = pViewShell->IsPreViewDocPos( aDocPos, nRowCol,
                                                    nSttPage, aPgSize );

        if( bDocPos )			// dann kann man ja umschalten
        {
            String sNewCrsrPos( String::CreateFromInt32( aDocPos.X() ));
            ((( sNewCrsrPos += ';' )
                            += String::CreateFromInt32( aDocPos.Y() )) )
                            += ';';
            rView.SetNewCrsrPos( sNewCrsrPos );

            SfxViewFrame *pTmpFrm = rView.GetViewFrame();
            pTmpFrm->GetBindings().Execute( SID_VIEWSHELL0, NULL, 0,
                                                    SFX_CALLMODE_ASYNCHRON );
        }
    }
}

/******************************************************************************
 *	Beschreibung: Userprefs bzw Viewoptions setzen
 ******************************************************************************/


void SwPagePreViewWin::SetPagePreview( BYTE nRow, BYTE nCol )
{
    SwMasterUsrPref *pOpt = (SwMasterUsrPref *)SW_MOD()->GetUsrPref(FALSE);

    if (nRow != pOpt->GetPagePrevRow() || nCol != pOpt->GetPagePrevCol())
    {
        pOpt->SetPagePrevRow( nRow );
        pOpt->SetPagePrevCol( nCol );
        pOpt->SetModified();

        //VScrollbar updaten!
        if( rView.IsVScrollbarVisible() )
            rView.VScrollViewSzChg();
    }
}

void SwPagePreViewWin::DataChanged( const DataChangedEvent& rDCEvt )
{
    Window::DataChanged( rDCEvt );

    switch( rDCEvt.GetType() )
    {
    case DATACHANGED_SETTINGS:
        // ScrollBars neu anordnen bzw. Resize ausloesen, da sich
        // ScrollBar-Groesse geaendert haben kann. Dazu muss dann im
        // Resize-Handler aber auch die Groesse der ScrollBars aus
        // den Settings abgefragt werden.
        if( rDCEvt.GetFlags() & SETTINGS_STYLE )
            rView.InvalidateBorder();				//Scrollbarbreiten
        break;

    case DATACHANGED_PRINTER:
    case DATACHANGED_DISPLAY:
    case DATACHANGED_FONTS:
    case DATACHANGED_FONTSUBSTITUTION:
        rView.GetDocShell()->UpdateFontList();	//Fontwechsel
        if ( pViewShell->GetWin() )
            pViewShell->GetWin()->Invalidate();
        break;
    }
}


/*  */
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
// dann mal alles fuer die SwPagePreView
void  SwPagePreView::Execute( SfxRequest &rReq )
{
    int eMvMode;
    BYTE nRow = 1;
    BOOL bRetVal = FALSE, bRefresh = TRUE;

    USHORT nSttPage = aViewWin.GetSttPage(),
            nPages = aViewWin.GetRow() * aViewWin.GetCol(),
            nLineSz = aViewWin.GetCol(),
            nLastSttPg = nPageCount+1 > nPages ? nPageCount+1 - nPages : 0,
            nDefSttPg = aViewWin.GetDefSttPage();

    switch(rReq.GetSlot())
    {
        case FN_REFRESH_VIEW:
        case FN_STAT_PAGE:
        case FN_STAT_ZOOM:
            break;

        case FN_SHOW_FOUR_PAGES:
        {
            const SfxItemSet *pArgs = rReq.GetArgs();
            if( pArgs && pArgs->Count() >= 2 )
            {
                BYTE nCols = (BYTE)((SfxUInt16Item &)pArgs->Get(
                                        SID_ATTR_TABLE_COLUMN)).GetValue();
                BYTE nRows = (BYTE)((SfxUInt16Item &)pArgs->Get(
                                        SID_ATTR_TABLE_ROW)).GetValue();
                aViewWin.CalcWish( nRows, nCols );

            }
            else
                SwPreViewZoomDlg( aViewWin ).Execute();

        }
        break;
        case FN_SHOW_TWO_PAGES:
            aViewWin.CalcWish( nRow, 2 );
            break;

        case FN_PREVIEW_ZOOM:
        case SID_ATTR_ZOOM:
        {
            const SfxItemSet *pArgs = rReq.GetArgs();
            const SfxPoolItem* pItem;
            SvxZoomDialog* pDlg = 0;
            if(!pArgs)
            {
                SfxItemSet aCoreSet(GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
                const SwViewOption* pVOpt = GetViewShell().GetViewOptions();
                SvxZoomItem aZoom( (SvxZoomType)pVOpt->GetZoomType(),
                                            pVOpt->GetZoom() );
                aZoom.SetValueSet(
                        SVX_ZOOM_ENABLE_50|
                        SVX_ZOOM_ENABLE_75|
                        SVX_ZOOM_ENABLE_100|
                        SVX_ZOOM_ENABLE_150|
                        SVX_ZOOM_ENABLE_200|
                        SVX_ZOOM_ENABLE_WHOLEPAGE);
                aCoreSet.Put( aZoom );

                pDlg = new SvxZoomDialog( &GetViewFrame()->GetWindow(), aCoreSet );
                pDlg->SetLimits( MINZOOM, MAXZOOM );

                if( pDlg->Execute() != RET_CANCEL )
                    pArgs = pDlg->GetOutputItemSet();
            }
            if( pArgs )
            {
                enum SvxZoomType eType = SVX_ZOOM_PERCENT;
                USHORT nZoomFactor = USHRT_MAX;
                if(SFX_ITEM_SET == pArgs->GetItemState(SID_ATTR_ZOOM, TRUE, &pItem))
                {
                    eType = ((const SvxZoomItem *)pItem)->GetType();
                    nZoomFactor = ((const SvxZoomItem *)pItem)->GetValue();
                }
                else if(SFX_ITEM_SET == pArgs->GetItemState(FN_PREVIEW_ZOOM, TRUE, &pItem))
                    nZoomFactor = ((const SfxUInt16Item *)pItem)->GetValue();
                if(USHRT_MAX != nZoomFactor)
                    SetZoom(eType, nZoomFactor);
            }
            delete pDlg;
        }
        break;
        case SID_ZOOM_IN:
        case SID_ZOOM_OUT:
        {
            enum SvxZoomType eType = SVX_ZOOM_PERCENT;
            const SwViewOption* pVOpt = GetViewShell().GetViewOptions();
            SetZoom(eType,
                    lcl_GetNextZoomStep(pVOpt->GetZoom(), SID_ZOOM_IN == rReq.GetSlot()));
        }
        break;
        case FN_CHAR_LEFT:
            //change the display only when the selection is already at the first position
            if( aViewWin.GetSelectedPage() < 2 )
                break;
            if( aViewWin.GetSelectedPage()-- > nSttPage )
            {
#ifdef ACCESSIBLE_LAYOUT
                GetViewShell().ShowPreViewSelection( aViewWin.GetSelectedPage() );
#endif
                break;
            }

            if( nDefSttPg == nSttPage-- )
            {
                break;
            }
            aViewWin.SetSttPage( nSttPage );
            eMvMode = SwPagePreViewWin::MV_CALC;		goto MOVEPAGE;

        case FN_CHAR_RIGHT:
            //change the display only when the selection is already at the last position
            if(aViewWin.GetSelectedPage() >= nPageCount)
                break;
            if((aViewWin.GetSelectedPage()++ < nSttPage + nPages - 1) ||
                nLastSttPg == nSttPage++ )
            {
#ifdef ACCESSIBLE_LAYOUT
                GetViewShell().ShowPreViewSelection( aViewWin.GetSelectedPage() );
#endif
                break;
            }
            aViewWin.SetSttPage( nSttPage );
            eMvMode = SwPagePreViewWin::MV_CALC;		goto MOVEPAGE;

        case FN_LINE_UP:
        case FN_LINE_DOWN:
        {
//???		if( !nSttPage &&  1 < nLineSz )
//				--nLineSz;
            USHORT nOldSttPage = nSttPage;
            if( FN_LINE_UP == rReq.GetSlot() )
            {
                if( nSttPage > nLineSz )

                    nSttPage -= nLineSz;
                else
                    nSttPage = nDefSttPg;
            }
            else
            {
                if( nSttPage + nLineSz <= nLastSttPg )
                    nSttPage += nLineSz;
                else
                    nSttPage = nLastSttPg;
            }

            if( nSttPage == aViewWin.GetSttPage() )
            {
                // keine Aenderung
                bRefresh = FALSE;
                break;
            }

            aViewWin.SetSttPage( nSttPage );
            aViewWin.GetSelectedPage() -= (nOldSttPage - nSttPage);
            if(!aViewWin.GetSelectedPage())
                aViewWin.GetSelectedPage() = 1;
            eMvMode = SwPagePreViewWin::MV_CALC;
        }
        goto MOVEPAGE;

        case FN_PAGEUP:
            eMvMode = SwPagePreViewWin::MV_PAGE_UP;		goto MOVEPAGE;
        case FN_PAGEDOWN:
            eMvMode = SwPagePreViewWin::MV_PAGE_DOWN;	goto MOVEPAGE;

        case FN_START_OF_LINE:
        case FN_START_OF_DOCUMENT:
            aViewWin.GetSelectedPage() = 1;
            eMvMode = SwPagePreViewWin::MV_DOC_STT;	bRetVal = TRUE;	goto MOVEPAGE;
        case FN_END_OF_LINE:
        case FN_END_OF_DOCUMENT:
            aViewWin.GetSelectedPage() = nPageCount;
            eMvMode = SwPagePreViewWin::MV_DOC_END; bRetVal = TRUE; goto MOVEPAGE;
MOVEPAGE:
            {
                int nRet = ChgPage( eMvMode, TRUE );
                // return value fuer Basic
                if(bRetVal)
                    rReq.SetReturnValue(SfxBoolItem(rReq.GetSlot(), nRet == 0));

                bRefresh = 0 != nRet;
                rReq.Done();
            }
            break;

        case FN_PRINT_PAGEPREVIEW:
        {
            const SwPagePreViewPrtData* pPPVPD = aViewWin.GetViewShell()->GetDoc()->GetPreViewPrtData();
            // die Sache mit der Orientation
            if(pPPVPD)
            {
                SfxPrinter* pPrinter = GetPrinter();
                if((pPrinter->GetOrientation() == ORIENTATION_LANDSCAPE)
                        != pPPVPD->GetLandscape())
                    pPrinter->SetOrientation(pPPVPD->GetLandscape() ? ORIENTATION_LANDSCAPE : ORIENTATION_PORTRAIT);
            }
            ::SetAppPrintOptions( aViewWin.GetViewShell(), FALSE );
            bNormalPrint = FALSE;
            USHORT nPrtSlot = SID_PRINTDOC;
            rReq.SetSlot( nPrtSlot );
            SfxViewShell::ExecuteSlot( rReq, SfxViewShell::GetInterface() );
            rReq.SetSlot( FN_PRINT_PAGEPREVIEW );
            return;
        }
        case FN_PREVIEW_PRINT_OPTIONS :
        {
            SwPreviewPrintOptionsDialog aDlg(aViewWin, *this);
            aDlg.Execute();
        }
        break;
        case SID_PRINTDOCDIRECT:
        case SID_PRINTDOC:
            ::SetAppPrintOptions( aViewWin.GetViewShell(), FALSE );
            bNormalPrint = TRUE;
            SfxViewShell::ExecuteSlot( rReq, SfxViewShell::GetInterface() );
            return;
        case FN_CLOSE_PAGEPREVIEW:
        case SID_PRINTPREVIEW:
            //	print preview is now always in the same frame as the tab view
            //	-> always switch this frame back to normal view
            //	(ScTabViewShell ctor reads stored view data)
            GetViewFrame()->GetDispatcher()->Execute( SID_VIEWSHELL0, NULL, 0, SFX_CALLMODE_ASYNCHRON );
            break;
        case FN_INSERT_BREAK:
        {
            USHORT nSelPage = aViewWin.GetSelectedPage();
            //if a dummy page is selected (e.g. a non-existing right/left page)
            //the direct neighbor is used
            if(GetViewShell().IsDummyPage( nSelPage ) && GetViewShell().IsDummyPage( --nSelPage ))
                nSelPage +=2;
            SetNewPage( nSelPage );
            SfxViewFrame *pTmpFrm = GetViewFrame();
            pTmpFrm->GetBindings().Execute( SID_VIEWSHELL0, NULL, 0,
                                                    SFX_CALLMODE_ASYNCHRON );
        }
        break;
        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }

    if( bRefresh )
        aViewWin.Invalidate();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::GetState( SfxItemSet& rSet )
{
    SfxWhichIter aIter(rSet);
    BYTE nRow = 1;
    USHORT nWhich = aIter.FirstWhich();
    ASSERT(nWhich, leeres Set);

    while(nWhich)
    {
        switch(nWhich)
        {
        case FN_START_OF_DOCUMENT:
        case FN_PAGEUP:
            if( aViewWin.GetSttPage() == aViewWin.GetDefSttPage() )
                rSet.DisableItem(nWhich);
            break;

        case FN_PAGEDOWN:
        case FN_END_OF_DOCUMENT:
            {
                USHORT nNextPg = ( aViewWin.GetRow() * aViewWin.GetCol() ) +
                                aViewWin.GetSttPage() - 1;

                if( nNextPg >= nPageCount )
                    rSet.DisableItem( nWhich );
            }
            break;

        case FN_STAT_PAGE:
            {
                String aStr( sPageStr );
                aViewWin.GetStatusStr( aStr, nPageCount );
                rSet.Put( SfxStringItem( nWhich, aStr) );
            }
            break;

        case SID_ATTR_ZOOM:
        case FN_STAT_ZOOM:
            {
                const SwViewOption* pVOpt = GetViewShell().GetViewOptions();
                SvxZoomItem aZoom((SvxZoomType)pVOpt->GetZoomType(),
                                    pVOpt->GetZoom());
                aZoom.SetValueSet(
                        SVX_ZOOM_ENABLE_50|
                        SVX_ZOOM_ENABLE_75|
                        SVX_ZOOM_ENABLE_100|
                        SVX_ZOOM_ENABLE_150|
                        SVX_ZOOM_ENABLE_200);
                rSet.Put( aZoom );
            }
        break;
        case FN_PREVIEW_ZOOM:
        {
            const SwViewOption* pVOpt = GetViewShell().GetViewOptions();
            rSet.Put(SfxUInt16Item(nWhich, pVOpt->GetZoom()));
        }
        break;
        case SID_ZOOM_IN:
        case SID_ZOOM_OUT:
        {
            const SwViewOption* pVOpt = GetViewShell().GetViewOptions();
            if((SID_ZOOM_OUT == nWhich && pVOpt->GetZoom() >= MAX_PREVIEW_ZOOM)||
              (SID_ZOOM_IN == nWhich && pVOpt->GetZoom() <= MIN_PREVIEW_ZOOM))
            {
                rSet.DisableItem(nWhich);
            }
        }
        break;
        case FN_SHOW_FOUR_PAGES:
        //should never be disabled
        break;

        case FN_SHOW_TWO_PAGES:
            if( 2 == aViewWin.GetCol() && nRow == aViewWin.GetRow() )
                rSet.DisableItem( nWhich );
            break;

        case FN_PRINT_PAGEPREVIEW:
            // hat den gleichen Status wie das normale Drucken
            {
                const SfxPoolItem* pItem;
                SfxItemSet aSet( *rSet.GetPool(), SID_PRINTDOC, SID_PRINTDOC );
                GetSlotState( SID_PRINTDOC, SfxViewShell::GetInterface(), &aSet );
                if( SFX_ITEM_SET == aSet.GetItemState( SID_PRINTDOC,
                        FALSE, &pItem ))
                {
                    ((SfxPoolItem*)pItem)->SetWhich( FN_PRINT_PAGEPREVIEW );
                    rSet.Put( *pItem );
                }
            }
            break;

        case SID_PRINTPREVIEW:
            rSet.Put( SfxBoolItem( nWhich, TRUE ) );
            break;

        case SID_PRINTDOC:
        case SID_PRINTDOCDIRECT:
            GetSlotState( nWhich, SfxViewShell::GetInterface(),	&rSet );
            break;
        }
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::StateUndo(SfxItemSet& rSet)
{
    SfxWhichIter aIter(rSet);
    USHORT nWhich = aIter.FirstWhich();

    while (nWhich)
    {
        rSet.DisableItem(nWhich);
        nWhich = aIter.NextWhich();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::Init(const SwViewOption * pPrefs)
{
    if ( GetViewShell().HasDrawView() )
        GetViewShell().GetDrawView()->SetAnimationEnabled( FALSE );

    bNormalPrint = TRUE;

    // Die DocSize erfragen und verarbeiten. Ueber die Handler konnte
    // die Shell nicht gefunden werden, weil die Shell innerhalb CTOR-Phase
    // nicht in der SFX-Verwaltung bekannt ist.

    if( !pPrefs )
        pPrefs = SW_MOD()->GetUsrPref(FALSE);

    // die Felder aktualisieren
    // ACHTUNG: hochcasten auf die EditShell, um die SS zu nutzen.
    //			In den Methoden wird auf die akt. Shell abgefragt!
    SwEditShell* pESh = (SwEditShell*)&GetViewShell();
    BOOL bIsModified = pESh->IsModified();


    SwViewOption aOpt( *pPrefs );
    aOpt.SetPagePreview(TRUE);
    aOpt.SetTab( FALSE );
    aOpt.SetBlank( FALSE );
    aOpt.SetHardBlank( FALSE );
    aOpt.SetParagraph( FALSE );
    aOpt.SetLineBreak( FALSE );
    aOpt.SetPageBreak( FALSE );
    aOpt.SetColumnBreak( FALSE );
    aOpt.SetSoftHyph( FALSE );
    aOpt.SetFldName( FALSE );
    aOpt.SetPostIts( FALSE );
    aOpt.SetHidden( FALSE );
    aOpt.SetViewHRuler( FALSE );
    aOpt.SetViewVRuler( FALSE );
    aOpt.SetGraphic( TRUE );
    aOpt.SetTable( TRUE );
    aOpt.SetSnap( FALSE );
    aOpt.SetGridVisible( FALSE );
    aOpt.SetHideSpell( TRUE );
    GetViewShell().ApplyViewOptions( aOpt );
    GetViewShell().ApplyAccessiblityOptions(SW_MOD()->GetAccessibilityOptions());

    if(	pESh->GetDoc()->IsBrowseMode() )
    {
        pESh->GetDoc()->SetBrowseMode( FALSE );
        pESh->CheckBrowseView( TRUE );
    }

    pESh->GetPrt( TRUE );

    GetViewShell().CalcLayout();
    DocSzChgd( GetViewShell().GetDocSize() );

    if( !bIsModified )
        pESh->ResetModified();

    pVScrollbar->Show(pPrefs->IsViewVScrollBar());
    pHScrollbar->Show(pPrefs->IsViewHScrollBar());
    pScrollFill->Show(pPrefs->IsViewVScrollBar() && pPrefs->IsViewHScrollBar());
}


/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwPagePreView::SwPagePreView(SfxViewFrame *pFrame, SfxViewShell* pOldSh):
    SfxViewShell( pFrame, SWVIEWFLAGS ),
    aViewWin( &pFrame->GetWindow(), *this ),
    pHScrollbar(0),
    pVScrollbar(0),
    pPageUpBtn(0),
    pPageDownBtn(0),
    pScrollFill(new ScrollBarBox( &pFrame->GetWindow(),
        GetDocShell()->IsInFrame()? 0 : WB_SIZEABLE )),
    sPageStr( SW_RES(STR_PAGE) ),
    nPageCount( 0 ),
    nNewPage(USHRT_MAX)
{
    SetName(String::CreateFromAscii("PageView" ));
    SetWindow( &aViewWin );
    SetHelpId(SW_PAGEPREVIEW);
    _CreateScrollbar( TRUE );
    _CreateScrollbar( FALSE );

    SfxObjectShell* pObjShell = pFrame->GetObjectShell();
    if ( !pOldSh )
    {
        //Gibt es schon eine Sicht auf das Dokument?
        SfxViewFrame *pF = SfxViewFrame::GetFirst( pObjShell );
        if ( pF == pFrame )
            pF = SfxViewFrame::GetNext( *pF, pObjShell );
        if ( pF )
            pOldSh = pF->GetViewShell();
    }

    ViewShell *pVS, *pNew;

    if( pOldSh && pOldSh->IsA( TYPE( SwPagePreView ) ) )
        pVS = &((SwPagePreView*)pOldSh)->GetViewShell();
    else
    {
        if( pOldSh && pOldSh->IsA( TYPE( SwView ) ) )
        {
            pVS = ((SwView*)pOldSh)->GetWrtShellPtr();
            // save the current ViewData of the previous SwView
            pOldSh->WriteUserData( sSwViewData, FALSE );
        }
        else
            pVS = GetDocShell()->GetWrtShell();
        if( pVS )
        {
            // setze die akt. Seite als die erste
            USHORT nPhysPg, nVirtPg;
            ((SwCrsrShell*)pVS)->GetPageNum( nPhysPg, nVirtPg, /*FALSE*/TRUE, FALSE );
            if( 1 != aViewWin.GetCol() && 1 == nPhysPg )
                --nPhysPg;
            aViewWin.SetSttPage( nPhysPg );
        }
    }

    if( pVS )
        pNew = new ViewShell( *pVS, &aViewWin, 0, VSHELLFLAG_ISPREVIEW );
    else
        pNew = new ViewShell(
                *((SwDocShell*)pFrame->GetObjectShell())->GetDoc(),
                &aViewWin, 0, 0, VSHELLFLAG_ISPREVIEW );

    aViewWin.SetViewShell( pNew );
    pNew->SetSfxViewShell( this );
    Init();
}


/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


 SwPagePreView::~SwPagePreView()
{
    SetWindow( 0 );

    delete pScrollFill;
    delete pHScrollbar;
    delete pVScrollbar;
    delete pPageUpBtn;
    delete pPageDownBtn;

/*    SfxObjectShell* pDocSh = GetDocShell();
    TypeId aType = TYPE( SfxTopViewFrame );

    for( SfxViewFrame *pFrame = SfxViewFrame::GetFirst( pDocSh, aType );
        pFrame; pFrame = SfxViewFrame::GetNext( *pFrame, pDocSh, aType ) )
        if( pFrame != GetViewFrame() )
        {
            // es gibt noch eine weitere Sicht auf unser Dokument, also
            // aktiviere dieses
            pFrame->GetFrame()->Appear();
            break;
        }
*/}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwDocShell* SwPagePreView::GetDocShell()
{
    return PTR_CAST(SwDocShell, GetViewFrame()->GetObjectShell());
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


int	SwPagePreView::_CreateScrollbar( int bHori )
{
    Window *pMDI = &GetViewFrame()->GetWindow();
    SwScrollbar** ppScrollbar = bHori ? &pHScrollbar : &pVScrollbar;

    ASSERT( !*ppScrollbar, "vorher abpruefen!" )

    if( !bHori )
    {

        pPageUpBtn		= new ImageButton(pMDI, SW_RES( BTN_PAGEUP ) );
        pPageUpBtn->SetHelpId(FN_PAGEUP);
        pPageDownBtn	= new ImageButton(pMDI, SW_RES( BTN_PAGEDOWN ) );
        pPageDownBtn->SetHelpId(FN_PAGEDOWN);
        Link aLk( LINK( this, SwPagePreView, BtnPage ) );
        pPageUpBtn->SetClickHdl( aLk );
        pPageDownBtn->SetClickHdl( aLk );
        pPageUpBtn->Show();
        pPageDownBtn->Show();
    }

    *ppScrollbar = new SwScrollbar( pMDI, bHori );

    if( !bHori )
    {
        VScrollDocSzChg();
        (*ppScrollbar)->EnableDrag( TRUE );
        (*ppScrollbar)->SetEndScrollHdl( LINK( this, SwPagePreView, EndScrollHdl ));
    }
    else
    {
        (*ppScrollbar)->SetRangeMax( 0 );
        (*ppScrollbar)->SetThumbPos( 0 );
        (*ppScrollbar)->SetLineSize( 0 );
        (*ppScrollbar)->SetPageSize( 0 );
    }


    (*ppScrollbar)->SetScrollHdl( LINK( this, SwPagePreView, ScrollHdl ));

    InvalidateBorder();
    (*ppScrollbar)->Show();
    return 1;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

/*
 * Button-Handler
 */
IMPL_LINK_INLINE_START( SwPagePreView, BtnPage, Button *, pButton )
{
    int eMvMode = pButton == pPageUpBtn
                        ? SwPagePreViewWin::MV_PAGE_UP
                        : SwPagePreViewWin::MV_PAGE_DOWN;
    if( ChgPage( eMvMode ) )
        aViewWin.Invalidate();
    return 0;
}
IMPL_LINK_INLINE_END( SwPagePreView, BtnPage, Button *, pButton )

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


int SwPagePreView::ChgPage( int eMvMode, int bUpdateScrollbar )
{
    Rectangle aPixVisArea( aViewWin.LogicToPixel( aVisArea ) );
    int bChg = aViewWin.MovePage( eMvMode ) ||
                    eMvMode == SwPagePreViewWin::MV_CALC;
    aVisArea = aViewWin.PixelToLogic( aPixVisArea );

    if( bChg )
    {
        // Statusleiste updaten
        String aStr( sPageStr );
        aViewWin.GetStatusStr( aStr, nPageCount );
        SfxBindings& rBindings = GetViewFrame()->GetBindings();

        if( bUpdateScrollbar )
        {
            if( pVScrollbar )
                VScrollViewSzChg();

            static USHORT __READONLY_DATA aInval[] =
            {
                FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT,
                FN_PAGEUP, FN_PAGEDOWN, 0
            };
            rBindings.Invalidate( aInval );
        }
        rBindings.SetState( SfxStringItem( FN_STAT_PAGE, aStr ) );
    }
    return bChg;
}


/*  */
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


// ab hier alles aus der SwView uebernommen


void SwPagePreView::CalcAndSetBorderPixel( SvBorder &rToFill, FASTBOOL bInner )
{
//	const long nAdd = bInner ? 0 : ScrollBar::GetWindowOverlapPixel();
    const StyleSettings &rSet = aViewWin.GetSettings().GetStyleSettings();
    const long nTmp = rSet.GetScrollBarSize();// - nAdd;
    if ( pVScrollbar->IsVisible())
        rToFill.Right()  = nTmp;
    if ( pHScrollbar->IsVisible() )
        rToFill.Bottom() = nTmp;
    SetBorderPixel( rToFill );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::InnerResizePixel( const Point &rOfst, const Size &rSize )
{
    SvBorder aBorder;
    CalcAndSetBorderPixel( aBorder, TRUE );
    Rectangle aRect( rOfst, rSize );
    aRect += aBorder;
    ViewResizePixel( aViewWin, aRect.TopLeft(), aRect.GetSize(),
                    aViewWin.GetOutputSizePixel(),
                    TRUE,
                    *pVScrollbar, *pHScrollbar, pPageUpBtn, pPageDownBtn, 0,
                    *pScrollFill );

    //EditWin niemals einstellen!
    //VisArea niemals einstellen!
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::OuterResizePixel( const Point &rOfst, const Size &rSize )
{
    SvBorder aBorder;
    CalcAndSetBorderPixel( aBorder, FALSE );
    ViewResizePixel( aViewWin, rOfst, rSize, aViewWin.GetOutputSizePixel(),
                        FALSE, *pVScrollbar,
                        *pHScrollbar, pPageUpBtn, pPageDownBtn, 0, *pScrollFill );

    //EditWin niemals einstellen!

    Size aTmpSize( aViewWin.GetOutputSizePixel() );
    Point aBottomRight( aViewWin.PixelToLogic( Point( aTmpSize.Width(), aTmpSize.Height() ) ) );
    SetVisArea( Rectangle( Point(), aBottomRight ) );

    //Aufruf der DocSzChgd-Methode der Scrollbars ist noetig, da vom maximalen
    //Scrollrange immer die halbe Hoehe der VisArea abgezogen wird.
    if ( pVScrollbar )
        VScrollDocSzChg();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::SetVisArea( const Rectangle &rRect, BOOL bUpdateScrollbar )
{
    const Point aTopLeft(AlignToPixel(rRect.TopLeft()));
    const Point aBottomRight(AlignToPixel(rRect.BottomRight()));
    Rectangle aLR(aTopLeft,aBottomRight);

    if(aLR == aVisArea)
        return;
        // keine negative Position, keine neg. Groesse

    if(aLR.Top() < 0)
    {
        aLR.Bottom() += Abs(aLR.Top());
        aLR.Top() = 0;
    }

    if(aLR.Left() < 0)
    {
        aLR.Right() += Abs(aLR.Left());
        aLR.Left() = 0;
    }
    if(aLR.Right() < 0) aLR.Right() = 0;
    if(aLR.Bottom() < 0) aLR.Bottom() = 0;
    if(aLR == aVisArea ||
        // JP 29.10.97: Bug 45173 - Leeres Rechteck nicht beachten
        ( 0 == aLR.Bottom() - aLR.Top() && 0 == aLR.Right() - aLR.Left() ) )
        return;

    if( aLR.Left() > aLR.Right() || aLR.Top() > aLR.Bottom() )
        return;

    //Bevor die Daten veraendert werden ggf. ein Update rufen. Dadurch wird
    //sichergestellt, da� anliegende Paints korrekt in Dokumentkoordinaten
    //umgerechnet werden.
    //Vorsichtshalber tun wir das nur wenn an der Shell eine Action laeuft,
    //denn dann wir nicht wirklich gepaintet sondern die Rechtecke werden
    //lediglich (in Dokumentkoordinaten) vorgemerkt.
    if( GetViewShell().ActionPend() )
        aViewWin.Update();

    // setze am View-Win die aktuelle Size
    aVisArea = aLR;
    aViewWin.SetWinSize( aLR.GetSize() );
    ChgPage( SwPagePreViewWin::MV_CALC, bUpdateScrollbar );

    aViewWin.Invalidate();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


IMPL_LINK( SwPagePreView, ScrollHdl, SwScrollbar *, pScrollbar )
{
    if( !pScrollbar->IsHoriScroll() &&
        pScrollbar->GetType() == SCROLL_DRAG && Help::IsQuickHelpEnabled())
    {
        // wieviele Seiten scrollen ??
        String sStateStr(sPageStr);
        USHORT nThmbPos = (USHORT)pScrollbar->GetThumbPos();
        if( 1 == aViewWin.GetCol() || !nThmbPos )
            ++nThmbPos;
        sStateStr += String::CreateFromInt32( nThmbPos );
        Point aPos = pScrollbar->GetParent()->OutputToScreenPixel(
                                        pScrollbar->GetPosPixel());
        aPos.Y() = pScrollbar->OutputToScreenPixel(pScrollbar->GetPointerPosPixel()).Y();
        Size aSize = pScrollbar->GetSizePixel();
        Rectangle aRect;
        aRect.Left() 	= aPos.X() -8;
        aRect.Right() 	= aRect.Left();
        aRect.Top()		= aPos.Y();
        aRect.Bottom()	= aRect.Top();

        Help::ShowQuickHelp(pScrollbar, aRect, sStateStr,
                QUICKHELP_RIGHT|QUICKHELP_VCENTER);

    }
    else
        EndScrollHdl( pScrollbar );
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


IMPL_LINK( SwPagePreView, EndScrollHdl, SwScrollbar *, pScrollbar )
{
    if( !pScrollbar->IsHoriScroll() )		// nur Vertikal auswerten
    {
        if(Help::IsQuickHelpEnabled())
            Help::ShowQuickHelp(pScrollbar, Rectangle(), aEmptyStr, 0);
        // wieviele Seiten scrollen ??
        USHORT nThmbPos = (USHORT)pScrollbar->GetThumbPos();
        if( 1 == aViewWin.GetCol() )
            ++nThmbPos;
        if( nThmbPos != aViewWin.GetSttPage() )
        {
            aViewWin.SetSttPage( nThmbPos );
            ChgPage( SwPagePreViewWin::MV_CALC, FALSE );

            static USHORT __READONLY_DATA aInval[] =
            {
                FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN, 0
            };
#ifndef PRODUCT
            {
                const USHORT* pPtr = aInval + 1;
                do {
                    ASSERT( *(pPtr - 1) < *pPtr, "falsche Sortierung!" );
                } while( *++pPtr );
            }
#endif

            SfxBindings& rBindings = GetViewFrame()->GetBindings();
            rBindings.Invalidate( aInval );
            aViewWin.Invalidate();
        }
    }
    return 0;
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


Point SwPagePreView::AlignToPixel(const Point &rPt) const
{
    return aViewWin.PixelToLogic( aViewWin.LogicToPixel( rPt ) );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::DocSzChgd( const Size &rSz )
{
    if( aDocSz == rSz )
        return;

    aDocSz = rSz;

    // die neue Anzahl von Seiten bestimmen
    USHORT nNewCnt = GetViewShell().GetNumPages();
    if( nNewCnt == nPageCount )
        return;

    // dann eine neue Startseite berechnen
    nPageCount = nNewCnt;
    if( aVisArea.GetWidth() )
    {
        ChgPage( SwPagePreViewWin::MV_CALC, TRUE );

        if( pVScrollbar )
            VScrollDocSzChg();

        aViewWin.Invalidate();
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::VScrollViewSzChg()
{
    USHORT nVisPage = aViewWin.GetRow() * aViewWin.GetCol(),
           nLineSz = 1 < nVisPage ? nVisPage / 2 : 1,
           nSttPg = aViewWin.GetSttPage();

    if( nSttPg && 1 == aViewWin.GetCol() )
        --nSttPg;

    pVScrollbar->SetVisibleSize( nVisPage );
    pVScrollbar->SetThumbPos( nSttPg );

    pVScrollbar->SetLineSize( nLineSz );
    pVScrollbar->SetPageSize( nVisPage );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::VScrollDocSzChg()
{
    if( pVScrollbar )
    {
        USHORT nPgCnt = nPageCount;
        if( 1 < aViewWin.GetCol() )
            ++nPgCnt;	/* die 0. Seite! */
        pVScrollbar->SetRangeMax( nPgCnt );
        VScrollViewSzChg();
    }
}


/*  */
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


// alles zum Thema Drucken

USHORT  SwPagePreView::Print( SfxProgress &rProgress, PrintDialog *pDlg )
{
    ViewShell* pSh = aViewWin.GetViewShell();
    SfxPrinter* pPrinter = GetPrinter();
    if( !pPrinter || !pPrinter->InitJob( &aViewWin, pSh->HasDrawView() &&
                    pSh->GetDrawView()->GetModel()->HasTransparentObjects() ))
        return ERRCODE_IO_ABORT;


    SwWait aWait( *GetDocShell(), TRUE );

    USHORT nRowCol = ( aViewWin.GetRow() << 8 ) +
                        aViewWin.GetCol();	// Zeilen / DoppelSeiten

    {
        // die Felder aktualisieren
        // ACHTUNG: hochcasten auf die EditShell, um die SS zu nutzen.
        //			In den Methoden wird auf die akt. Shell abgefragt!
        SwEditShell* pESh = (SwEditShell*)pSh;
        SwDocStat aDocStat;
        BOOL bIsModified = pESh->IsModified();

        pESh->StartAllAction();
        pESh->UpdateDocStat( aDocStat );
        pSh->UpdateFlds();
        pESh->EndAllAction();

        if( !bIsModified )
            pESh->ResetModified();
    }

    // Druckauftrag starten
    SfxObjectShell *pObjShell = GetViewFrame()->GetObjectShell();
    SwPrtOptions aOpts( pObjShell->GetTitle(0) );

    BOOL bPrtPros;
    SwView::MakeOptions( pDlg, aOpts, &bPrtPros, FALSE, GetPrinter(), GetDocShell()->GetDoc()->GetPrintData() );

    if( bNormalPrint )
    {
        if( bPrtPros )
            pSh->PrintProspect( aOpts, rProgress );
        else
            pSh->Prt( aOpts, rProgress );
    }
    else
    {
        const SwPagePreViewPrtData* pPPVPD = pSh->GetDoc()->GetPreViewPrtData();
        if( pPPVPD && pPPVPD->GetCol() && pPPVPD->GetRow() )
        {
            // Zeilen / Seiten
            nRowCol = ( pPPVPD->GetRow() << 8 ) + pPPVPD->GetCol();
        }
        else
            pPPVPD = 0;
        pSh->PrintPreViewPage( aOpts, nRowCol, rProgress, pPPVPD );
    }

    return 0; // OK
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SfxPrinter*  SwPagePreView::GetPrinter( BOOL bCreate )
{
    return aViewWin.GetViewShell()->GetPrt( bCreate );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


USHORT  SwPagePreView::SetPrinter( SfxPrinter *pNew, USHORT nDiffFlags )
{
    ViewShell &rSh = GetViewShell();
    SwEditShell &rESh = (SwEditShell&)rSh;	//Buh...
    if( ( SFX_PRINTER_PRINTER | SFX_PRINTER_JOBSETUP ) & nDiffFlags )
    {
        rSh.SetPrt( pNew );
        if( nDiffFlags & SFX_PRINTER_PRINTER )
            rESh.SetModified();
    }
    if ( ( nDiffFlags & SFX_PRINTER_OPTIONS ) == SFX_PRINTER_OPTIONS )
        ::SetPrinter( pNew, FALSE );

    const BOOL bChgOri = nDiffFlags & SFX_PRINTER_CHG_ORIENTATION ? TRUE : FALSE;
    const BOOL bChgSize= nDiffFlags & SFX_PRINTER_CHG_SIZE ? TRUE : FALSE;
    if ( bChgOri || bChgSize )
    {
        rESh.StartAllAction();
        if ( bChgOri )
            rSh.ChgAllPageOrientation( USHORT(pNew->GetOrientation()) );
        if ( bChgSize )
        {
            Size aSz( SvxPaperInfo::GetPaperSize( pNew ) );
            rSh.ChgAllPageSize( aSz );
        }
        if( !bNormalPrint )
            aViewWin.CalcWish( aViewWin.GetRow(), aViewWin.GetCol() );
        rESh.SetModified();
        rESh.EndAllAction();

        static USHORT __READONLY_DATA aInval[] =
        {
            SID_ATTR_LONG_ULSPACE, SID_ATTR_LONG_LRSPACE,
            SID_RULER_BORDERS, SID_RULER_PAGE_POS, 0
        };
#ifndef PRODUCT
    {
        const USHORT* pPtr = aInval + 1;
        do {
            ASSERT( *(pPtr - 1) < *pPtr, "falsche Sortierung!" );
        } while( *++pPtr );
    }
#endif

        GetViewFrame()->GetBindings().Invalidate(aInval);
    }

    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SfxTabPage*  SwPagePreView::CreatePrintOptionsPage( Window *pParent,
                                                const SfxItemSet &rOptions )
{
    return ::CreatePrintOptionsPage( pParent, rOptions, !bNormalPrint );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


PrintDialog*  SwPagePreView::CreatePrintDialog( Window *pParent )
{
    PrintDialog *pDlg = ::CreatePrintDialog( pParent, 1, 0 );
    pDlg->DisableRange( PRINTDIALOG_SELECTION );
    return pDlg;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


Size  SwPagePreView::GetOptimalSizePixel() const
{
    //JP 09.06.99: was wird hier errechnet ?????
/* ALT:
    SfxApplicationWindow* pWin = SFX_APPWINDOW ;
    Rectangle aRect = pWin->GetClientAreaPixel();
*/
    Window& rWin = GetViewFrame()->GetWindow();
    Rectangle aRect( Point(0, 0), rWin.GetOutputSizePixel() );
    Size aMaxSize( aRect.GetWidth(), aRect.GetHeight() );
    Size aInSize = rWin.GetOutputSizePixel();
    Size aOutSize = rWin.GetSizePixel();
    USHORT nXBorder = USHORT(aOutSize.Width() - aInSize.Width());
    USHORT nYBorder = USHORT(aOutSize.Height() - aInSize.Height());
    aMaxSize.Width() -= nXBorder;
    //'auf Verdacht' etwas vom Border abziehen (Menue)
    nYBorder -= (nYBorder - nXBorder) / 2;
    aMaxSize.Height() -= nYBorder;
    //mit der max. moeglichen Outputsize guenstigstes Verhaeltnis ausrechnen
    aViewWin.GetOptimalSize(aMaxSize);
    // Border wieder dazuzaehlen
    aMaxSize.Height() += nYBorder;
    aMaxSize.Width() += nXBorder;
    return aMaxSize;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreViewWin::GetOptimalSize( Size& rSize ) const
{
    USHORT nRowCol = ( nRow << 8 ) + nCol;

    Size aPageSize;
    USHORT nVirtPage;
#ifdef NEW_PREVIEW // OD 29.11.2002 #103492#
    {
        BYTE nRow = BYTE( nRowCol >> 8 );
        BYTE nCol = BYTE( nRowCol & 0xff );
        Size aPreviewDocSize;
        pViewShell->InitPreviewLayout( nCol, nRow, aPreviewDocSize,
                                       true, &rSize );
        aPageSize = aPreviewDocSize; // for testing
        Point aAbsStartPt;
        sal_uInt16 nDummy;
        pViewShell->PreparePreviewPaint( nSttPage, Point(0,0),
                                         nDummy, nVirtPage, aAbsStartPt );
    }
#else
    pViewShell->CalcPreViewPage( rSize, nRowCol,
                                    nSttPage, aPageSize,
                                    nVirtPage, USHRT_MAX );
#endif

    if(aPageSize.Width() && aPageSize.Height())
    {
        long nXRel = (long )aPageSize.Width() * nCol  + ((  nCol + 1) * 142);
        nXRel /= rSize.Width();
        long nYRel = aPageSize.Height() * nRow  + (( nRow + 1) * 142);
        nYRel /= rSize.Height();
        //n?Rel geben das Verhaeltnis von benoetigten Pixeln zu
        //vorhandenen Pixeln wieder
        //Danach wird in der Richtung abgeschnitten, in der mehr Platz ist
        if(nXRel < nYRel )
        {
            rSize.Width() *= nXRel;
            rSize.Width() /= nYRel;
        }
        else
        {
            rSize.Height() *= nYRel;
            rSize.Height() /= nXRel;
        }
    }
}


void SwPagePreViewWin::RepaintCoreRect( const SwRect& rRect )
{
    USHORT nRowCol = ( nRow << 8 ) + nCol;
    pViewShell->RepaintCoreRect( rRect, nRowCol, nSttPage, aPgSize );
}

BOOL SwPagePreView::HandleWheelCommands( const CommandEvent& rCEvt )
{
    BOOL bOk = FALSE;
    const CommandWheelData* pWData = rCEvt.GetWheelData();
    if( pWData && COMMAND_WHEEL_ZOOM == pWData->GetMode() )
    {
        BYTE nRow = aViewWin.GetRow(), nCol = aViewWin.GetCol();
        if( 0L > pWData->GetDelta() )
        {
            if( PVIEW_MIN_ROW < nRow )	--nRow;
            if( PVIEW_MIN_COL < nCol )	--nCol;
        }
        else
        {
            if( PVIEW_MAX_ROW > nRow )	++nRow;
            if( PVIEW_MAX_COL > nCol )	++nCol;
        }
        aViewWin.CalcWish( nRow, nCol );
        aViewWin.Invalidate();
        bOk = TRUE;
    }
    else
        bOk = aViewWin.HandleScrollCommand( rCEvt, 0,
                        pVScrollbar && pVScrollbar->IsVisible(TRUE) ? pVScrollbar : 0 );
    return bOk;
}


#ifdef ACCESSIBLE_LAYOUT
::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible>
    SwPagePreViewWin::CreateAccessible()
{
    vos::OGuard aGuard(Application::GetSolarMutex());	// this should have
                                                        // happend already!!!

    DBG_ASSERT( GetViewShell() != NULL, "We need a view shell" );
    return GetViewShell()->CreateAccessiblePreview(
        nRow, nCol, nSttPage, aPgSize,
        GetViewShell()->GetPreviewFreePix(), aScale, nSelectedPage );
}
#endif

/* -----------------------------06.05.2002 13:18------------------------------

 ---------------------------------------------------------------------------*/
void SwPagePreView::ApplyAccessiblityOptions(SvtAccessibilityOptions& rAccessibilityOptions)
{
    GetViewShell().ApplyAccessiblityOptions(rAccessibilityOptions);
}
/* -----------------------------2002/06/26 14:30------------------------------

 ---------------------------------------------------------------------------*/
void SwPagePreView::ShowHScrollbar(sal_Bool bShow)
{
    pHScrollbar->Show(bShow);
    InvalidateBorder();
}
/* -----------------------------2002/06/26 14:30------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwPagePreView::IsHScrollbarVisible()const
{
    return     pVScrollbar->IsVisible();
}
/* -----------------------------2002/06/26 14:30------------------------------

 ---------------------------------------------------------------------------*/
void SwPagePreView::ShowVScrollbar(sal_Bool bShow)
{
    pVScrollbar->Show(bShow);
    InvalidateBorder();
}
/* -----------------------------2002/06/26 14:30------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwPagePreView::IsVScrollbarVisible()const
{
    return pVScrollbar->IsVisible();
}
/* -----------------25.11.2002 16:36-----------------
 *
 * --------------------------------------------------*/
void SwPagePreView::SetZoom(SvxZoomType eType, USHORT nFactor)
{
    ViewShell& rSh = GetViewShell();
    SwViewOption aOpt(*rSh.GetViewOptions());
    aOpt.SetZoom(nFactor);
    aOpt.SetZoomType(eType);
    rSh.ApplyViewOptions( aOpt );
    lcl_InvalidateZoomSlots(GetViewFrame()->GetBindings());
}

