/*************************************************************************
 *
 *  $RCSfile: mailmrge.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: os $ $Date: 2001-06-06 06:20:21 $
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
#ifndef _MAILMRGE_HXX
#define _MAILMRGE_HXX

#ifndef _SVX_STDDLG_HXX //autogen
#include <svx/stddlg.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

class SwWrtShell;
class SwModuleOptions;
class SwXSelChgLstnr_Impl;
struct SwMailMergeDlg_Impl;
namespace com{namespace sun{namespace star{
    namespace frame{
        class XFrame;
    }
    namespace sdbc{
        class XResultSet;
    }
}}}
class SwMailMergeDlg : public SvxStandardDialog
{
    friend class SwXSelChgLstnr_Impl;

    Window*          pBeamerWin;

    RadioButton     aAllRB;
    RadioButton		aMarkedRB;
    RadioButton		aFromRB;
    NumericField	aFromNF;
    FixedText		aBisFT;
    NumericField	aToNF;
    FixedLine       aRecordFL;

    FixedLine       aSeparatorFL;

    RadioButton     aPrinterRB;
    RadioButton		aMailingRB;
    RadioButton		aFileRB;

    CheckBox		aSingleJobsCB;

    FixedText		aPathFT;
    Edit			aPathED;
    PushButton		aPathPB;
    FixedText 		aFilenameFT;
    RadioButton		aColumnRB;
    RadioButton		aFilenameRB;
    ListBox			aColumnLB;
    Edit			aFilenameED;

    FixedText		aAddressFT;
    ListBox			aAddressFldLB;
    FixedText		aSubjectFT;
    Edit			aSubjectED;
    FixedText		aFormatFT;
    FixedText		aAttachFT;
    Edit			aAttachED;
    PushButton		aAttachPB;
    CheckBox		aFormatHtmlCB;
    CheckBox		aFormatRtfCB;
    CheckBox		aFormatSwCB;
    FixedLine       aDestFL;

    OKButton		aOkBTN;
    CancelButton	aCancelBTN;
    HelpButton		aHelpBTN;

    SwMailMergeDlg_Impl* pImpl;

    SwWrtShell&     rSh;
    SwModuleOptions* pModOpt;
    const String&	rDBName;
    const String&	rTableName;

    USHORT			nMergeType;
    ::com::sun::star::uno::Sequence< sal_Int32 >  aSelection;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > xFrame;


    DECL_LINK( ButtonHdl, Button* pBtn );
    DECL_LINK( InsertPathHdl, PushButton * );
    DECL_LINK( AttachFileHdl, PushButton * );
    DECL_LINK( RadioButtonHdl, RadioButton* pBtn );
    DECL_LINK( FilenameHdl, RadioButton* pBtn );
    DECL_LINK( ModifyHdl, NumericField* pLB );

    virtual void 	Apply();
    void 			ExecQryShell(BOOL bVisible);

public:

     SwMailMergeDlg(Window* pParent, SwWrtShell& rSh,
         const String& rSourceName,
        const String& rTblName,
        sal_Int32 nCommandType,
        ::com::sun::star::uno::Sequence< sal_Int32 >* pSelection = 0);
    ~SwMailMergeDlg();

    inline USHORT	GetMergeType() { return nMergeType; }
    const ::com::sun::star::uno::Sequence< sal_Int32 > GetSelection() const{return aSelection;}
    ::com::sun::star::uno::Reference<::com::sun::star::sdbc::XResultSet> GetResultSet();

};

#endif

