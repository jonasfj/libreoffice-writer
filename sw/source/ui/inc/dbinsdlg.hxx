/*************************************************************************
 *
 *  $RCSfile: dbinsdlg.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: os $ $Date: 2001-06-06 12:01:40 $
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

#ifndef _DBINSDLG_HXX
#define _DBINSDLG_HXX


#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SVEDIT_HXX //autogen
#include <svtools/svmedit.hxx>
#endif
#ifndef _BASEDLGS_HXX //autogen
#include <sfx2/basedlgs.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _SWNUMFMTLB_HXX //autogen
#include <numfmtlb.hxx>
#endif
#ifndef _SWDBDATA_HXX
#include <swdbdata.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
namespace com{namespace sun{namespace star{
    namespace sdbcx{
    class XColumnsSupplier;
    }
    namespace sdbc{
    class XDataSource;
    class XConnection;
    class XResultSet;
    }
}}}

class SwTableAutoFmt;
class SwView;
class SfxItemSet;
class SwTableRep;
class _DB_Columns;

struct SwInsDBColumn
{
    rtl::OUString sColumn, sUsrNumFmt;
    sal_Int32 nDBNumFmt;
    ULONG nUsrNumFmt;
    LanguageType eUsrNumFmtLng;
    USHORT nCol;
    BOOL bHasFmt : 1;
    BOOL bIsDBFmt : 1;

    SwInsDBColumn( const String& rStr, USHORT nColumn )
        : sColumn( rStr ), nCol( nColumn ), nDBNumFmt( 0 ), nUsrNumFmt( 0 ),
        bHasFmt(FALSE), bIsDBFmt(TRUE), eUsrNumFmtLng( LANGUAGE_SYSTEM )
    {}

    int operator==( const SwInsDBColumn& rCmp ) const
        { return sColumn == rCmp.sColumn; }
    int operator<( const SwInsDBColumn& rCmp ) const;
};

typedef SwInsDBColumn* SwInsDBColumnPtr;
SV_DECL_PTRARR_SORT_DEL( SwInsDBColumns, SwInsDBColumnPtr, 32, 32 )


class SwInsertDBColAutoPilot : public SfxModalDialog, public utl::ConfigItem
{
    FixedText 		aFtInsertData;
    RadioButton 	aRbAsTable;
    RadioButton 	aRbAsField;
    RadioButton 	aRbAsText;

    FixedLine       aFlHead;
    FixedText 		aFtDbColumn;

    ListBox 		aLbTblDbColumn;
    ListBox 		aLbTxtDbColumn;

    FixedLine       aFlFormat;
    RadioButton 	aRbDbFmtFromDb;
    RadioButton 	aRbDbFmtFromUsr;
    NumFormatListBox aLbDbFmtFromUsr;

    /* ----- Page Text/Field ------- */
    ImageButton     aIbDbcolToEdit;
    MultiLineEdit 	aEdDbText;
    FixedText 		aFtDbParaColl;
    ListBox 		aLbDbParaColl;

    /* ----- Page Table ------------ */
    ImageButton     aIbDbcolAllTo;
    ImageButton     aIbDbcolOneTo;
    ImageButton     aIbDbcolOneFrom;
    ImageButton     aIbDbcolAllFrom;
    FixedText 		aFtTableCol;
    ListBox 		aLbTableCol;
    CheckBox 		aCbTableHeadon;
    RadioButton 	aRbHeadlColnms;
    RadioButton 	aRbHeadlEmpty;
    PushButton 		aPbTblFormat;
    PushButton 		aPbTblAutofmt;

    OKButton 		aBtOk;
    CancelButton 	aBtCancel;
    HelpButton 		aBtHelp;

    FixedLine       aFlBottom;

    SwInsDBColumns  aDBColumns;
    const SwDBData	aDBData;

    Link			aOldNumFmtLnk;
    String			sNoTmpl;

    SwView* 		pView;
    SwTableAutoFmt*	pTAutoFmt;

    SfxItemSet*		pTblSet;
    SwTableRep* 	pRep;
    USHORT			nGBFmtLen;

    DECL_LINK( PageHdl, Button* );
    DECL_LINK( AutoFmtHdl, PushButton* );
    DECL_LINK( TblFmtHdl, PushButton* );
    DECL_LINK( DBFormatHdl, Button* );
    DECL_LINK( TblToFromHdl, Button* );
    DECL_LINK( SelectHdl, ListBox* );
    DECL_LINK( DblClickHdl, ListBox* );
    DECL_LINK( HeaderHdl, Button* );

    FASTBOOL SplitTextToColArr( const String& rTxt, _DB_Columns& rColArr, BOOL bInsField );


    virtual void			Commit();
    void 					Load();

    // setze die Tabellen - Eigenschaften
    void SetTabSet();

public:
    SwInsertDBColAutoPilot( SwView& rView,
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource> rxSource,
        com::sun::star::uno::Reference<com::sun::star::sdbcx::XColumnsSupplier>,
        const SwDBData& rData  );

    virtual ~SwInsertDBColAutoPilot();

    void DataToDoc( const ::com::sun::star::uno::Sequence<::com::sun::star::uno::Any>& rSelection,
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource> rxSource,
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection> xConnection,
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > xResultSet);

};

#endif
