/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: addresslistdialog.hxx,v $
 * $Revision: 1.7 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _ADDRESSLISTDIALOG_HXX
#define _ADDRESSLISTDIALOG_HXX

#include <sfx2/basedlgs.hxx>
#include <svtools/stdctrl.hxx>
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#include <svtools/svtabbx.hxx>
#include <svtools/headbar.hxx>
#include <swdbdata.hxx>
#include "sharedconnection.hxx"


namespace com{namespace sun{namespace star{
    namespace container{
        class XNameAccess;
    }
    namespace sdbc{
        class XDataSource;
        class XConnection;
    }
    namespace sdbcx{
        class XColumnsSupplier;
    }
}}}
class SwMailMergeAddressBlockPage;
/*-- 08.04.2004 14:04:29---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwAddressListDialog : public SfxModalDialog
{
    FixedInfo       m_aDescriptionFI;

    FixedInfo       m_aListFT;
    HeaderBar       m_aListHB;
    SvTabListBox    m_aListLB;

    PushButton      m_aLoadListPB;
    PushButton      m_aCreateListPB;
    PushButton      m_aFilterPB;
    PushButton      m_aEditPB;
    PushButton      m_aTablePB;

    FixedLine       m_aSeparatorFL;

    OKButton        m_aOK;
    CancelButton    m_aCancel;
    HelpButton      m_aHelp;

    String          m_sName;
    String          m_sTable;
    String          m_sConnecting;

    String          m_sCreatedURL;
    SvLBoxEntry*    m_pCreatedDataSource;

    bool            m_bInSelectHdl;

    SwMailMergeAddressBlockPage* m_pAddressPage;

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess>     m_xDBContext;
//    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource>          m_xSource;
//    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection>          m_xConnection;
//    ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XColumnsSupplier>    m_xColumnsSupplier;
    SwDBData                                                                        m_aDBData;

    void DetectTablesAndQueries(SvLBoxEntry* pSelect, bool bWidthDialog);

    DECL_LINK(FilterHdl_Impl, PushButton*);
    DECL_LINK(LoadHdl_Impl,   PushButton*);
    DECL_LINK(CreateHdl_Impl, PushButton*);
    DECL_LINK(ListBoxSelectHdl_Impl, SvTabListBox*);
    DECL_LINK(EditHdl_Impl, PushButton*);
    DECL_LINK(TableSelectHdl_Impl, PushButton*);
    DECL_LINK(OKHdl_Impl, PushButton*);

    DECL_STATIC_LINK(SwAddressListDialog, StaticListBoxSelectHdl_Impl, SvLBoxEntry*);

public:
    SwAddressListDialog(SwMailMergeAddressBlockPage* pParent);
    ~SwAddressListDialog();

    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource>
                        GetSource();

    SharedConnection    GetConnection();

    ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XColumnsSupplier>
                        GetColumnsSupplier();

    const SwDBData&     GetDBData() const       {return m_aDBData;}
    ::rtl::OUString     GetFilter();
};
#endif

