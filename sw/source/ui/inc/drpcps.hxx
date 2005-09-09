/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drpcps.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:11:45 $
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
#ifndef _DRPCPS_HXX
#define _DRPCPS_HXX

#ifndef _BASEDLGS_HXX //autogen
#include <sfx2/basedlgs.hxx>
#endif

#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

class SwWrtShell;

// class SwDropCapsDlg ******************************************************

class SwDropCapsDlg : public SfxSingleTabDialog
{

public:

     SwDropCapsDlg(Window *pParent, const SfxItemSet &rSet );
    ~SwDropCapsDlg();

};

class SwDropCapsPict;

// class SwDropCapsPage *****************************************************

class SwDropCapsPage : public SfxTabPage
{
friend class SwDropCapsPict;

    CheckBox  		aDropCapsBox;
    CheckBox  		aWholeWordCB;
    FixedText		aSwitchText;
    NumericField  	aDropCapsField;
    FixedText 		aLinesText;
    NumericField 	aLinesField;
    FixedText 		aDistanceText;
    MetricField 	aDistanceField;
    FixedLine       aSettingsFL;

    FixedText      	aTextText;
    Edit           	aTextEdit;
    FixedText      	aTemplateText;
    ListBox        	aTemplateBox;
    FixedLine       aContentFL;

    SwDropCapsPict 	*pPict;

    BOOL 			bModified;
    BOOL 			bFormat;
    BOOL			bHtmlMode;

    SwWrtShell &rSh;

     SwDropCapsPage(Window *pParent, const SfxItemSet &rSet);
    ~SwDropCapsPage();

    virtual int     DeactivatePage(SfxItemSet *pSet);
    void 	FillSet( SfxItemSet &rSet );

    DECL_LINK( ClickHdl, Button * );
    DECL_LINK( ModifyHdl, Edit * );
    DECL_LINK( SelectHdl, ListBox * );
    DECL_LINK( WholeWordHdl, CheckBox * );

public:

    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);
    static USHORT* GetRanges();

    virtual BOOL FillItemSet(      SfxItemSet &rSet);
    virtual void Reset      (const SfxItemSet &rSet);

    void 	SetFormat(BOOL bSet){bFormat = bSet;}
};

#endif

