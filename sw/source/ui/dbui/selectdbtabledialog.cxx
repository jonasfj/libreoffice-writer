/*************************************************************************
 *
 *  $RCSfile: selectdbtabledialog.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2005-03-01 15:27:27 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif

#pragma hdrstop

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif

#ifndef _SELECTDBTABLEDIALOG_HXX
#include <selectdbtabledialog.hxx>
#endif
#ifndef _DBTABLEPREVIEWDIALOG_HXX
#include <dbtablepreviewdialog.hxx>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XQUERIESSUPPLIER_HPP_
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATASOURCE_HPP_
#include <com/sun/star/sdbc/XDataSource.hpp>
#endif


#include <selectdbtabledialog.hrc>
#include <dbui.hrc>
#include <helpid.h>

using namespace com::sun::star::sdbcx;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdb;
using namespace com::sun::star::uno;
using namespace com::sun::star::container;
using namespace com::sun::star::beans;

#define C2U(cChar) ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(cChar))
/*-- 08.04.2004 14:33:56---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSelectDBTableDialog::SwSelectDBTableDialog(Window* pParent, 
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection>& rConnection) :
    SfxModalDialog(pParent, SW_RES(DLG_MM_SELECTDBTABLEDDIALOG)),
#pragma warning (disable : 4355)
    m_aSelectFI( this, ResId(       FI_SELECT     )),
    m_aTableHB( this, WB_BUTTONSTYLE | WB_BOTTOMBORDER),
    m_aTableLB( this, ResId(        LB_TABLE      )),
    m_aPreviewPB( this, ResId(      PB_PREVIEW    )),
    m_aSeparatorFL(this, ResId(    FL_SEPARATOR      )),
    m_aOK( this, ResId(             PB_OK         )),
    m_aCancel( this, ResId(         PB_CANCEL     )),
    m_aHelp( this, ResId(           PB_HELP       )),
#pragma warning (default : 4355)
    m_sName( ResId( ST_NAME )),
    m_sType( ResId( ST_TYPE )),
    m_sTable( ResId( ST_TABLE )),
    m_sQuery( ResId( ST_QUERY )),
    rxConnection(rConnection)
{
    FreeResource();
    
    Size aLBSize(m_aTableLB.GetSizePixel());
    m_aTableHB.SetSizePixel(aLBSize);
    Size aHeadSize(m_aTableHB.CalcWindowSizePixel());
    aHeadSize.Width() = aLBSize.Width();
    m_aTableHB.SetSizePixel(aHeadSize);
    Point aLBPos(m_aTableLB.GetPosPixel());
    m_aTableHB.SetPosPixel(aLBPos);
    aLBPos.Y() += aHeadSize.Height();
    aLBSize.Height() -= aHeadSize.Height();
    m_aTableLB.SetPosSizePixel(aLBPos, aLBSize);
    
    Size aSz(m_aTableHB.GetOutputSizePixel());
    m_aTableHB.InsertItem( 1, m_sName,
                            aSz.Width()/2,
                            HIB_LEFT | HIB_VCENTER /*| HIB_CLICKABLE | HIB_UPARROW */);
    m_aTableHB.InsertItem( 2, m_sType,
                            aSz.Width()/2,
                            HIB_LEFT | HIB_VCENTER /*| HIB_CLICKABLE | HIB_UPARROW */);
    m_aTableHB.SetHelpId(HID_MM_ADDRESSLIST_HB );
    m_aTableHB.Show();
    
    static long nTabs[] = {3, 0, aSz.Width()/2, aSz.Width() };
    m_aTableLB.SetTabs(&nTabs[0], MAP_PIXEL);
    m_aTableLB.SetHelpId(HID_MM_SELECTDBTABLEDDIALOG_LISTBOX);
    m_aTableLB.SetWindowBits( WB_CLIPCHILDREN );
    m_aTableLB.SetSpaceBetweenEntries(3);
    m_aTableLB.SetSelectionMode( SINGLE_SELECTION );
    m_aTableLB.SetDragDropMode(   0 );
    m_aTableLB.EnableAsyncDrag(FALSE);

    m_aPreviewPB.SetClickHdl(LINK(this, SwSelectDBTableDialog, PreviewHdl));

    Reference<XTablesSupplier> xTSupplier(rxConnection, UNO_QUERY);
    if(xTSupplier.is())
    {
        Reference<XNameAccess> xTbls = xTSupplier->getTables();
        Sequence<rtl::OUString> aTbls = xTbls->getElementNames();
        const rtl::OUString* pTbls = aTbls.getConstArray();
        for(long i = 0; i < aTbls.getLength(); i++)
        {
            String sEntry = pTbls[i];
            sEntry += '\t';
            sEntry += m_sTable;
            SvLBoxEntry* pEntry = m_aTableLB.InsertEntry(sEntry);
            pEntry->SetUserData((void*)0);
        }
    }
    Reference<XQueriesSupplier> xQSupplier(rxConnection, UNO_QUERY);
    if(xQSupplier.is())
    {
        Reference<XNameAccess> xQueries = xQSupplier->getQueries();
        Sequence<rtl::OUString> aQueries = xQueries->getElementNames();
        const rtl::OUString* pQueries = aQueries.getConstArray();
        for(long i = 0; i < aQueries.getLength(); i++)
        {
            String sEntry = pQueries[i];
            sEntry += '\t';
            sEntry += m_sQuery;
            SvLBoxEntry* pEntry = m_aTableLB.InsertEntry(sEntry);
            pEntry->SetUserData((void*)1);
        }
    }
}
/*-- 08.04.2004 14:33:57---------------------------------------------------

  -----------------------------------------------------------------------*/
SwSelectDBTableDialog::~SwSelectDBTableDialog()
{
}                                              
/*-- 08.04.2004 14:33:57---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK(SwSelectDBTableDialog, PreviewHdl, PushButton*, pButton)
{
    SvLBoxEntry* pEntry = m_aTableLB.FirstSelected();
    if(pEntry)
    {
        ::rtl::OUString sTableOrQuery = m_aTableLB.GetEntryText(pEntry, 0);
        sal_Int32 nCommandType = 0 == pEntry->GetUserData() ? 0 : 1;
        
        ::rtl::OUString sDataSourceName;
        Reference<XChild> xChild(rxConnection, UNO_QUERY);
        if(xChild.is())
        {
            Reference<XDataSource> xSource(xChild->getParent(), UNO_QUERY);
            Reference<XPropertySet> xPrSet(xSource, UNO_QUERY);
            xPrSet->getPropertyValue(C2U("Name")) >>= sDataSourceName;
        }            
        DBG_ASSERT(sDataSourceName.getLength(), "no data source found")
        Sequence<PropertyValue> aProperties(5);
        PropertyValue* pProperties = aProperties.getArray();
        pProperties[0].Name = C2U("DataSourceName");
        pProperties[0].Value <<= sDataSourceName;
        pProperties[1].Name = C2U("Command");
        pProperties[1].Value <<= sTableOrQuery;
        pProperties[2].Name = C2U("CommandType");
        pProperties[2].Value <<= nCommandType;
        pProperties[3].Name = C2U("ShowTreeView");
        sal_Bool bFalse = sal_False;
        pProperties[3].Value <<= bFalse;
        pProperties[4].Name = C2U("ShowTreeViewButton");
        pProperties[4].Value <<= bFalse;
                
        SwDBTablePreviewDialog* pDlg = new SwDBTablePreviewDialog(pButton, aProperties);
        pDlg->Execute();
        delete pDlg;
    }

    return 0;
}
/*-- 19.04.2004 10:03:26---------------------------------------------------

  -----------------------------------------------------------------------*/
String      SwSelectDBTableDialog::GetSelectedTable(bool& bIsTable)
{
    SvLBoxEntry* pEntry = m_aTableLB.FirstSelected();
    bIsTable = pEntry->GetUserData() ? false : true;
    return pEntry ? m_aTableLB.GetEntryText(pEntry, 0) : String();
}
/*-- 13.05.2004 12:58:26---------------------------------------------------

  -----------------------------------------------------------------------*/
void   SwSelectDBTableDialog::SetSelectedTable(const String& rTable, bool bIsTable)
{
    SvLBoxEntry*    pEntry = m_aTableLB.First();
    while(pEntry)
    {
        if((m_aTableLB.GetEntryText(pEntry, 0) == rTable) && 
                 ((pEntry->GetUserData() == 0 ) == bIsTable))
        {
            m_aTableLB.Select(pEntry);
            break;
        }
        pEntry = m_aTableLB.Next( pEntry );
    }
}
