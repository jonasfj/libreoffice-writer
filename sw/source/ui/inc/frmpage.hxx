/*************************************************************************
 *
 *  $RCSfile: frmpage.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: os $ $Date: 2002-09-25 11:36:19 $
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

#ifndef _FRMPAGE_HXX
#define _FRMPAGE_HXX

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _BMPWIN_HXX
#include <bmpwin.hxx>
#endif
#ifndef _FRMEX_HXX
#include <frmex.hxx>
#endif
#ifndef _PRCNTFLD_HXX
#include <prcntfld.hxx>
#endif

namespace sfx2{class FileDialogHelper;}
class SwWrtShell;
struct FrmMap;


/*--------------------------------------------------------------------
     Beschreibung:	Rahmendialog
 --------------------------------------------------------------------*/

class SwFrmPage: public SfxTabPage
{
    // Size
    FixedText		aWidthFT;
    PercentField 	aWidthED;
    CheckBox		aRelWidthCB;
    FixedText		aHeightFT;
    PercentField 	aHeightED;
    CheckBox		aRelHeightCB;
    CheckBox		aFixedRatioCB;
    CheckBox		aAutoHeightCB;
    PushButton		aRealSizeBT;
    FixedLine       aSizeFL;
    BOOL			bWidthLastChanged;

    // Anker
    FixedLine       aTypeFL;
    FixedLine       aTypeSepFL;
    RadioButton		aAnchorAtPageRB;
    RadioButton		aAnchorAtParaRB;
    RadioButton		aAnchorAtCharRB;
    RadioButton		aAnchorAsCharRB;
    RadioButton		aAnchorAtFrameRB;

    // Position
    FixedText		aHorizontalFT;
    ListBox 		aHorizontalDLB;
    FixedText		aAtHorzPosFT;
    MetricField 	aAtHorzPosED;
    FixedText		aHoriRelationFT;
    ListBox			aHoriRelationLB;
    CheckBox		aMirrorPagesCB;
    FixedText		aVerticalFT;
    ListBox 		aVerticalDLB;
    FixedText		aAtVertPosFT;
    MetricField 	aAtVertPosED;
    FixedText		aVertRelationFT;
    ListBox			aVertRelationLB;
    FixedLine       aPositionFL;
    BOOL			bAtHorzPosModified;
    BOOL			bAtVertPosModified;

    // Example
    SwFrmPagePreview	aExampleWN;

    BOOL			bFormat;
    BOOL			bNew;
    BOOL 			bHtmlMode;
    BOOL			bNoModifyHdl;
    BOOL            bVerticalChanged;  //check done whether frame is in vertical environment
    BOOL            bIsVerticalFrame;  //current frame is in vertical environment - strings are exchanged
    BOOL            bIsInRightToLeft; // current frame is in right-to-left environment - strings are exchanged
    USHORT          nHtmlMode;
    USHORT			nDlgType;
    Size			aGrfSize;
    Size			aWrap;
    SwTwips			nUpperBorder;
    SwTwips			nLowerBorder;
    double          fWidthHeightRatio; //width-to-height ratio to support the KeepRatio button

    // Die alten Ausrichtungen
    USHORT 			nOldH;
    USHORT 			nOldHRel;
    USHORT 			nOldV;
    USHORT 			nOldVRel;

    virtual void 	ActivatePage(const SfxItemSet& rSet);
    virtual int 	DeactivatePage(SfxItemSet *pSet);


    DECL_LINK( RangeModifyHdl, Edit * );
    DECL_LINK( AnchorTypeHdl, RadioButton * );
    DECL_LINK( PosHdl, ListBox * );
    DECL_LINK( RelHdl, ListBox * );
    void 			InitPos(USHORT nId, USHORT nH, USHORT nHRel,
                            USHORT nV,	USHORT nVRel,
                            long   nX,	long   nY);

    DECL_LINK( EditModifyHdl, Edit * );
    DECL_LINK( RealSizeHdl, Button * );
    DECL_LINK( RelSizeClickHdl, CheckBox * );
    DECL_LINK( MirrorHdl, CheckBox * );

    DECL_LINK( ManualHdl, Button * );

    // Beispiel aktualisieren
    void			UpdateExample();
    DECL_LINK( ModifyHdl, Edit * );

    void			Init(const SfxItemSet& rSet, BOOL bReset = FALSE);
    USHORT			FillPosLB(FrmMap *pMap, USHORT nAlign, ListBox &rLB);
    ULONG           FillRelLB(FrmMap *pMap, USHORT nLBSelPos, USHORT nAlign, USHORT nRel, ListBox &rLB, FixedText &rFT);
    USHORT			GetMapPos(FrmMap *pMap, ListBox &rAlignLB);
    USHORT			GetAlignment(FrmMap *pMap, USHORT nMapPos, ListBox &rAlignLB, ListBox &rRelationLB);
    USHORT			GetRelation(FrmMap *pMap, ListBox &rRelationLB);
    USHORT			GetAnchor();

    SwFrmPage(Window *pParent, const SfxItemSet &rSet);
    ~SwFrmPage();

public:
    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);
    static USHORT* GetRanges();

    virtual BOOL FillItemSet(SfxItemSet &rSet);
    virtual void Reset(const SfxItemSet &rSet);

    void 			SetNewFrame(BOOL bNewFrame) { bNew		= bNewFrame; }
    void			SetFormatUsed(BOOL bFmt);
    void			SetFrmType(USHORT nType) 	{ nDlgType 	= nType;	 }
};

class SwGrfExtPage: public SfxTabPage
{
    // Spiegeln
    FixedLine       aMirrorFL;
    CheckBox		aMirrorVertBox;
    CheckBox		aMirrorHorzBox;
    RadioButton 	aAllPagesRB;
    RadioButton 	aLeftPagesRB;
    RadioButton 	aRightPagesRB;
    BmpWindow		aBmpWin;

    FixedLine       aConnectFL;
    FixedText		aConnectFT;
    Edit			aConnectED;
    PushButton		aBrowseBT;

    String			aFilterName;
    String			aGrfName, aNewGrfName;

    ::sfx2::FileDialogHelper*     pGrfDlg;

    BOOL 			bHtmlMode;

    // Handler fuer Spiegeln
    DECL_LINK( MirrorHdl, CheckBox * );
    DECL_LINK( BrowseHdl, Button * );

    virtual void 	ActivatePage(const SfxItemSet& rSet);
    SwGrfExtPage(Window *pParent, const SfxItemSet &rSet);
    ~SwGrfExtPage();

public:
    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);

    virtual BOOL FillItemSet(SfxItemSet &rSet);
    virtual void Reset(const SfxItemSet &rSet);
    virtual int  DeactivatePage(SfxItemSet *pSet);
};


class SwFrmURLPage : public SfxTabPage
{
    //Hyperlink
    FixedLine       aHyperLinkFL;
    FixedText		aURLFT;
    Edit			aURLED;
    PushButton		aSearchPB;
    FixedText		aNameFT;
    Edit			aNameED;
    FixedText		aFrameFT;
    ComboBox		aFrameCB;

    //Image map
    FixedLine       aImageFL;
    CheckBox		aServerCB;
    CheckBox		aClientCB;

    DECL_LINK( InsertFileHdl, PushButton * );


    SwFrmURLPage(Window *pParent, const SfxItemSet &rSet);
    ~SwFrmURLPage();
public:
    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);

    virtual BOOL FillItemSet(SfxItemSet &rSet);
    virtual void Reset(const SfxItemSet &rSet);
};

/*-----------------13.11.96 12.59-------------------

--------------------------------------------------*/

class SwFrmAddPage : public SfxTabPage
{
    FixedText 	aNameFT;
    Edit		aNameED;
    FixedText	aAltNameFT;
    Edit		aAltNameED;
    FixedText 	aPrevFT;
    ListBox     aPrevLB;
    FixedText 	aNextFT;
    ListBox     aNextLB;
    FixedLine   aNamesFL;

    CheckBox	aProtectContentCB;
    CheckBox	aProtectFrameCB;
    CheckBox	aProtectSizeCB;
    FixedLine   aProtectFL;

    CheckBox	aEditInReadonlyCB;
    CheckBox	aPrintFrameCB;
    FixedText   aTextFlowFT;
    ListBox     aTextFlowLB;

    FixedLine   aExtFL;

    SwWrtShell* pWrtSh;

    USHORT		nDlgType;
    BOOL 		bHtmlMode;
    BOOL 		bFormat;
    BOOL 		bNew;

    DECL_LINK(EditModifyHdl, Edit*);
    DECL_LINK(ChainModifyHdl, ListBox*);

    SwFrmAddPage(Window *pParent, const SfxItemSet &rSet);
    ~SwFrmAddPage();

public:

    static SfxTabPage*	Create(Window *pParent, const SfxItemSet &rSet);
    static USHORT* 		GetRanges();

    virtual BOOL FillItemSet(SfxItemSet &rSet);
    virtual void Reset(const SfxItemSet &rSet);

    void			SetFormatUsed(BOOL bFmt);
    void			SetFrmType(USHORT nType) { nDlgType = nType; }
    void 			SetNewFrame(BOOL bNewFrame) { bNew  = bNewFrame; }
    void			SetShell(SwWrtShell* pSh) { pWrtSh  = pSh; }

};

#endif // _FRMPAGE_HXX
