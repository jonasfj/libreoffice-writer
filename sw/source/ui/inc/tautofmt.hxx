/*************************************************************************
 *
 *  $RCSfile: tautofmt.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: fme $ $Date: 2001-06-01 10:20:44 $
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
#ifndef SW_TAUTOFMT_HXX
#define SW_TAUTOFMT_HXX

#ifndef _BASEDLGS_HXX //autogen
#include <sfx2/basedlgs.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _MOREBTN_HXX //autogen
#include <vcl/morebtn.hxx>
#endif

#ifndef _VIRDEV_HXX //autogen
#include <vcl/virdev.hxx>
#endif

class SwView;
class SwTableAutoFmt;
class SvxBoxItem;
class SvxBorderLine;
class AutoFmtPreview;
class SwTableAutoFmtTbl;

//------------------------------------------------------------------------

enum AutoFmtLine { TOP_LINE, BOTTOM_LINE, LEFT_LINE, RIGHT_LINE };

//========================================================================

class SwAutoFormatDlg : public SfxModalDialog
{
    FixedText		aFtFormat;
    ListBox			aLbFormat;
    CheckBox		aBtnNumFormat;
    CheckBox		aBtnBorder;
    CheckBox		aBtnFont;
    CheckBox		aBtnPattern;
    CheckBox		aBtnAlignment;
    FixedLine        aFLFormat;
    OKButton		aBtnOk;
    CancelButton	aBtnCancel;
    HelpButton		aBtnHelp;
    PushButton		aBtnAdd;
    PushButton		aBtnRemove;
    PushButton		aBtnRename;
    MoreButton		aBtnMore;
    String			aStrTitle;
    String			aStrLabel;
    String			aStrClose;
    String			aStrDelTitle;
    String			aStrDelMsg;
    String			aStrRenameTitle;
    String			aStrInvalidFmt;
    AutoFmtPreview*	pWndPreview;

    //------------------------
    SwWrtShell* 			pShell;
    SwTableAutoFmtTbl*		pTableTbl;
    BYTE					nIndex;
    BYTE					nDfltStylePos;
    BOOL					bCoreDataChanged : 1;
    BOOL					bSetAutoFmt : 1;


    void Init( const SwTableAutoFmt* pSelFmt );
    void UpdateChecks( const SwTableAutoFmt&, BOOL bEnableBtn );
    //------------------------
    DECL_LINK( CheckHdl, Button * );
    DECL_LINK( OkHdl, Button * );
    DECL_LINK( AddHdl, void * );
    DECL_LINK( RemoveHdl, void * );
    DECL_LINK( RenameHdl, void * );
    DECL_LINK( SelFmtHdl, void * );

public:
    SwAutoFormatDlg( Window* pParent, SwWrtShell* pShell,
                        BOOL bSetAutoFmt = TRUE,
                        const SwTableAutoFmt* pSelFmt = 0 );
    virtual ~SwAutoFormatDlg();

    void FillAutoFmtOfIndex( SwTableAutoFmt*& rToFill ) const;
};


#endif // SW_AUTOFMT_HXX

