/*************************************************************************
 *
 *  $RCSfile: textsh2.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: os $ $Date: 2001-06-20 14:40:59 $
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

#if STLPORT_VERSION>=321
#include <cstdarg>
#endif

#include <svtools/svmedit.hxx>
#ifndef _SBASLTID_HRC //autogen
#include <offmgr/sbasltid.hrc>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SFXEVENT_HXX //autogen
#include <sfx2/event.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _OFF_APP_HXX //autogen
#include <offmgr/app.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SFXREQUEST_HXX
#include <sfx2/request.hxx>
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATASOURCE_HPP_
#include <com/sun/star/sdbc/XDataSource.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XQUERIESSUPPLIER_HPP_
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XDATABASEACCESS_HPP_
#include <com/sun/star/sdb/XDatabaseAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _SFXFRAME_HXX
#include <sfx2/frame.hxx>
#endif
#ifndef _FLDMGR_HXX
#include <fldmgr.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#include "dbmgr.hxx"


#include "view.hxx"
#include "wrtsh.hxx"
#include "swtypes.hxx"
#include "cmdid.h"
#include "swevent.hxx"
#include "shells.hrc"
#include "textsh.hxx"
#include "dbinsdlg.hxx"

using namespace rtl;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::container;
using namespace com::sun::star::lang;
using namespace com::sun::star::sdb;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;
using namespace com::sun::star::beans;

#define C2U(cChar) rtl::OUString::createFromAscii(cChar)
#define C2S(cChar) UniString::CreateFromAscii(cChar)
#define DB_DD_DELIM 0x0b


inline void AddSelList( List& rLst, long nRow )
{
    rLst.Insert( (void*)nRow , LIST_APPEND );
}
void SwTextShell::ExecDB(SfxRequest &rReq)
{
    const SfxItemSet *pArgs = rReq.GetArgs();
    SwNewDBMgr* pNewDBMgr = GetShell().GetNewDBMgr();
    USHORT nSlot = rReq.GetSlot();

    switch (nSlot)
    {
        case FN_QRY_INSERT:
            {
                String* pNew = new String( ((const SfxStringItem&)pArgs->
                                            Get(nSlot)).GetValue() );
                Application::PostUserEvent( STATIC_LINK( this, SwBaseShell,
                                            InsertDBTextHdl ), pNew );
                // der String wird im InsertDBTextHdl geloescht !!
            }
            break;

        case FN_QRY_MERGE_FIELD:
            {
                String sSbaData = ((const SfxStringItem&)pArgs->Get(nSlot)).GetValue();
                String sDBName = sSbaData.GetToken(0, DB_DD_DELIM);
                String sTableName(sSbaData.GetToken(1, DB_DD_DELIM));
                BOOL bTable = sSbaData.GetToken(2, DB_DD_DELIM) == C2S("1");
                String sStatement = sSbaData.GetToken(3, DB_DD_DELIM);

                USHORT nCount = sSbaData.GetTokenCount(DB_DD_DELIM);
                Sequence<sal_Int32> aSelection(nCount - 4);
                sal_Int32 * pSelection = aSelection.getArray();
                sal_Int32 nIdx = 0;
                for( USHORT i = 4; i < nCount; i++ , nIdx++)
                    pSelection[nIdx] = sSbaData.GetToken( i, DB_DD_DELIM).ToInt32();

                Reference<XResultSet>  xResultSet;
                Reference<XDataSource> xSource;
                SwDBData aData;
                aData.sDataSource = sDBName;
                aData.sCommand = sTableName;
                aData.nCommandType = 0;
                Reference< XConnection> xConnection = pNewDBMgr->GetConnection(sDBName, xSource);
                if(!xConnection.is())
                    return ;
                Reference<XStatement> xStatement = xConnection->createStatement();
                if(!sStatement.Len())
                {
                    Reference< sdbc::XDatabaseMetaData >  xMetaData = xConnection->getMetaData();
                    OUString aQuoteChar = xMetaData->getIdentifierQuoteString();
                    OUString sStatement(C2U("SELECT * FROM "));
                    sStatement = C2U("SELECT * FROM ");
                    sStatement += aQuoteChar;
                    sStatement += aData.sCommand;
                    sStatement += aQuoteChar;
                }
                xResultSet = xStatement->executeQuery( sStatement );
                Sequence<PropertyValue> aProperties(5);
                PropertyValue* pProperties = aProperties.getArray();
                pProperties[0].Name = C2U("DataSourceName");
                pProperties[0].Value <<= (OUString)sDBName;
                pProperties[1].Name = C2U("Command");
                pProperties[1].Value <<= (OUString)sTableName;
                pProperties[2].Name = C2U("Cursor");
                pProperties[2].Value <<= xResultSet;
                pProperties[3].Name = C2U("Selection");
                pProperties[3].Value <<= aSelection;
                pProperties[4].Name = C2U("CommandType");
                pProperties[4].Value <<= (sal_Int32)0;

                pNewDBMgr->MergeNew(DBMGR_MERGE, *GetShellPtr(), aProperties);

            }
            break;

        case FN_QRY_INSERT_FIELD:
            {
                String sSbaData = ((const SfxStringItem&)pArgs->Get(FN_QRY_INSERT_FIELD)).GetValue();
                const SfxPoolItem* pConnectionItem = 0;
                const SfxPoolItem* pColumnItem = 0;
                pArgs->GetItemState(FN_DB_CONNECTION_ANY, FALSE, &pConnectionItem);
                pArgs->GetItemState(FN_DB_COLUMN_ANY, FALSE, &pColumnItem);

                String sDBName = sSbaData.GetToken(0, DB_DD_DELIM);
                sDBName += DB_DELIM;
                sDBName += sSbaData.GetToken(1, DB_DD_DELIM);
                sDBName += DB_DELIM;
                BOOL bTable = sSbaData.GetToken(2, DB_DD_DELIM) == C2S("1");
                sDBName += sSbaData.GetToken(3, DB_DD_DELIM);	// Column name

                SwFldMgr aFldMgr(GetShellPtr());
                SwInsertFld_Data aData(TYP_DBFLD, 0, sDBName, aEmptyStr, 0, FALSE, TRUE);
                if(pConnectionItem)
                    aData.aDBConnection = ((SfxUsrAnyItem*)pConnectionItem)->GetValue();
                if(pColumnItem)
                    aData.aDBColumn = ((SfxUsrAnyItem*)pColumnItem)->GetValue();
                aFldMgr.InsertFld(aData);
            }
            break;

        default:
            ASSERT(!this, falscher Dispatcher);
            return;
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

IMPL_STATIC_LINK( SwBaseShell, InsertDBTextHdl, String*, pString )
{
    if( pString )
    {

        USHORT nTokenPos = 0;
        String sSourceName( pString->GetToken( 0, DB_DD_DELIM, nTokenPos ));
        String sTblQryName( pString->GetToken( 0, DB_DD_DELIM, nTokenPos ));
        String sStatmnt( pString->GetToken( 1, DB_DD_DELIM, nTokenPos ));

        Reference<XDataSource> xSource;
        Reference< sdbc::XConnection> xConnection = SwNewDBMgr::GetConnection(sSourceName, xSource);
        Reference< XColumnsSupplier> xColSupp;
        if(xConnection.is())
            xColSupp = SwNewDBMgr::GetColumnSupplier(xConnection,
                                    sTblQryName,
                                    SW_DB_SELECT_UNKNOWN);

        if( xColSupp.is() )
        {
            SwDBData aDBData;
            aDBData.sDataSource = sSourceName;
            aDBData.sCommand = sStatmnt.Len() ? sStatmnt  : sTblQryName;
            aDBData.nCommandType = sStatmnt.Len() ? sdb::CommandType::COMMAND : sdb::CommandType::TABLE;
            SwInsertDBColAutoPilot *pDlg = new SwInsertDBColAutoPilot(
                    pThis->GetView(),
                    xSource,
                    xColSupp,
                    aDBData );
            if( RET_OK == pDlg->Execute() )
            {
                sal_Int32 nTokenCount = pString->GetTokenCount(DB_DD_DELIM);
                if(!pString->GetToken(nTokenCount -1 , DB_DD_DELIM).Len())
                    --nTokenCount;
                sal_Int32 nSelectionCount = nTokenCount - 4;
                Sequence<Any> aSelection(nSelectionCount);
                Any* pSelection = aSelection.getArray();
                for(sal_Int32 nPos = 0; nPos < nSelectionCount; nPos++)
                {
                    pSelection[nPos] <<= pString->GetToken( 0, DB_DD_DELIM, nTokenPos ).ToInt32();
                }
                Reference <XResultSet> xResSet;
                pDlg->DataToDoc( aSelection, xSource, xConnection, xResSet);
            }
            delete pDlg;
        }
        Reference <XComponent> xComp(xConnection, UNO_QUERY);
        if(xComp.is())
            xComp->dispose();
    }

    delete pString;
    return 0;
}




