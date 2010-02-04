/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dbmgr.cxx,v $
 * $Revision: 1.132.44.2 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
#if STLPORT_VERSION>=321
#include <cstdarg>
#endif

#include <stdio.h>
#include <unotxdoc.hxx>
#include <com/sun/star/text/NotePrintMode.hpp>
#include <sfx2/app.hxx>
#include <com/sun/star/sdb/CommandType.hpp>
#include <com/sun/star/sdb/XDocumentDataSource.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/util/XNumberFormatter.hpp>
#include <com/sun/star/sdb/XCompletedConnection.hpp>
#include <com/sun/star/sdb/XCompletedExecution.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/text/MailMergeEvent.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/uno/XNamingService.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <sfx2/fcontnr.hxx>
#include <sfx2/filedlghelper.hxx>
#include <sfx2/viewfrm.hxx>
#include <dbconfig.hxx>
#include <swdbtoolsclient.hxx>
#include <pagedesc.hxx>
#include <vcl/lstbox.hxx>
#include <unotools/tempfile.hxx>
#include <unotools/pathoptions.hxx>
#include <svl/urihelper.hxx>
#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#endif
#include <svl/zforlist.hxx>
#include <svl/zformat.hxx>
#include <svl/stritem.hxx>
#include <svl/eitem.hxx>
#include <vcl/oldprintadaptor.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/progress.hxx>
#include <sfx2/dispatch.hxx>
#include <goodies/mailenum.hxx>
#include <cmdid.h>
#include <swmodule.hxx>
#include <view.hxx>
#include <docsh.hxx>
#include <edtwin.hxx>
#include <wrtsh.hxx>
#include <fldbas.hxx>
#include <initui.hxx>
#include <swundo.hxx>
#include <flddat.hxx>
#include <modcfg.hxx>
#include <swprtopt.hxx>
#include <shellio.hxx>
#include <dbui.hxx>
#include <dbmgr.hxx>
#include <doc.hxx>
#include <swwait.hxx>
#include <swunohelper.hxx>
#include <dbui.hrc>
#include <globals.hrc>
#include <statstr.hrc>
#include <mmconfigitem.hxx>
#include <sfx2/request.hxx>
#include <hintids.hxx>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#include <com/sun/star/sdb/XColumn.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/ResultSetType.hpp>
#include <com/sun/star/mail/MailAttachment.hpp>
#include <comphelper/processfactory.hxx>
#include <comphelper/types.hxx>
#include <comphelper/property.hxx>
#include <mailmergehelper.hxx>
#include <maildispatcher.hxx>
#include <svx/htmlcfg.hxx>
#include <i18npool/mslangid.hxx>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <svx/langitem.hxx>
#include <svl/numuno.hxx>

#include <unomailmerge.hxx>
#include <sfx2/event.hxx>
#include <vcl/msgbox.hxx>
#include <svx/dataaccessdescriptor.hxx>
#include <vos/mutex.hxx>
#include <rtl/textenc.h>
#include <ndindex.hxx>
#include <pam.hxx>
#include <swcrsr.hxx>
#include <swevent.hxx>
#include <osl/file.hxx>
#include <swabstdlg.hxx>
#include <fmthdft.hxx>
#include <dbui.hrc>
#include <envelp.hrc>
#include <memory>
#include <vector>
#include <unomid.h>

using namespace ::osl;
using namespace ::svx;
using namespace ::com::sun::star;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::ui::dialogs;

#define DB_SEP_SPACE  	0
#define DB_SEP_TAB  	1
#define DB_SEP_RETURN   2
#define DB_SEP_NEWLINE  3

SV_IMPL_PTRARR(SwDSParamArr, SwDSParamPtr);
const sal_Char cCursor[] = "Cursor";
const sal_Char cCommand[] = "Command";
const sal_Char cCommandType[] = "CommandType";
const sal_Char cDataSourceName[] = "DataSourceName";
const sal_Char cSelection[] = "Selection";
const sal_Char cActiveConnection[] = "ActiveConnection";

// -----------------------------------------------------------------------------
// Use nameless namespace to avoid to rubbish the global namespace
// -----------------------------------------------------------------------------
namespace
{

bool lcl_getCountFromResultSet( sal_Int32& rCount, const uno::Reference<XResultSet>& xResultSet )
{
    uno::Reference<XPropertySet> xPrSet(xResultSet, UNO_QUERY);
    if(xPrSet.is())
    {
        try
        {
            sal_Bool bFinal = sal_False;
            Any aFinal = xPrSet->getPropertyValue(C2U("IsRowCountFinal"));
            aFinal >>= bFinal;
            if(!bFinal)
            {
                xResultSet->last();
                xResultSet->first();
            }
            Any aCount = xPrSet->getPropertyValue(C2U("RowCount"));
            if( aCount >>= rCount )
                return true;
        }
        catch(Exception&)
        {
        }
    }
    return false;
}
// #122799# copy compatibility options
void lcl_CopyCompatibilityOptions( SwWrtShell& rSourceShell, SwWrtShell& rTargetShell)
{
    IDocumentSettingAccess* pIDsa = rSourceShell.getIDocumentSettingAccess();

    rTargetShell.SetParaSpaceMax( pIDsa->get(IDocumentSettingAccess::PARA_SPACE_MAX));
    rTargetShell.SetParaSpaceMaxAtPages(pIDsa->get(IDocumentSettingAccess::PARA_SPACE_MAX_AT_PAGES));
    rTargetShell.SetTabCompat( pIDsa->get(IDocumentSettingAccess::TAB_COMPAT));
    rTargetShell.SetAddExtLeading( pIDsa->get(IDocumentSettingAccess::ADD_EXT_LEADING));
    rTargetShell.SetUseVirDev( pIDsa->get(IDocumentSettingAccess::USE_VIRTUAL_DEVICE));
    rTargetShell.SetAddParaSpacingToTableCells( pIDsa->get(IDocumentSettingAccess::ADD_PARA_SPACING_TO_TABLE_CELLS));
    rTargetShell.SetUseFormerLineSpacing( pIDsa->get(IDocumentSettingAccess::OLD_LINE_SPACING));
    rTargetShell.SetUseFormerObjectPositioning( pIDsa->get(IDocumentSettingAccess::USE_FORMER_OBJECT_POS));
    rTargetShell.SetConsiderWrapOnObjPos( pIDsa->get(IDocumentSettingAccess::CONSIDER_WRAP_ON_OBJECT_POSITION));
    rTargetShell.SetUseFormerTextWrapping( pIDsa->get(IDocumentSettingAccess::USE_FORMER_TEXT_WRAPPING));
}
}
/* -----------------09.12.2002 12:35-----------------
 *
 * --------------------------------------------------*/

class SwConnectionDisposedListener_Impl : public cppu::WeakImplHelper1
< lang::XEventListener >
{
    SwNewDBMgr&     rDBMgr;

    virtual void SAL_CALL disposing( const EventObject& Source ) throw (RuntimeException);
public:
    SwConnectionDisposedListener_Impl(SwNewDBMgr& rMgr);
    ~SwConnectionDisposedListener_Impl();

};
// -----------------------------------------------------------------------------
struct SwNewDBMgr_Impl
{
    SwDSParam*          pMergeData;
    AbstractMailMergeDlg*     pMergeDialog;
    uno::Reference<lang::XEventListener> xDisposeListener;

    SwNewDBMgr_Impl(SwNewDBMgr& rDBMgr)
       :pMergeData(0)
       ,pMergeDialog(0)
       ,xDisposeListener(new SwConnectionDisposedListener_Impl(rDBMgr))
        {}
};
/*-- 24.10.2003 15:54:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void lcl_InitNumberFormatter(SwDSParam& rParam, uno::Reference<XDataSource> xSource)
{
    uno::Reference<XMultiServiceFactory> xMgr = ::comphelper::getProcessServiceFactory();
    if( xMgr.is() )
    {
        uno::Reference<XInterface> xInstance = xMgr->createInstance( C2U( "com.sun.star.util.NumberFormatter" ));
        rParam.xFormatter = uno::Reference<util::XNumberFormatter>(xInstance, UNO_QUERY) ;
    }
    if(!xSource.is())
        xSource = SwNewDBMgr::getDataSourceAsParent(rParam.xConnection, rParam.sDataSource);

    uno::Reference<XPropertySet> xSourceProps(xSource, UNO_QUERY);
    if(xSourceProps.is())
    {
        Any aFormats = xSourceProps->getPropertyValue(C2U("NumberFormatsSupplier"));
        if(aFormats.hasValue())
        {
            uno::Reference<XNumberFormatsSupplier> xSuppl;
            aFormats >>= xSuppl;
            if(xSuppl.is())
            {
                uno::Reference< XPropertySet > xSettings = xSuppl->getNumberFormatSettings();
                Any aNull = xSettings->getPropertyValue(C2U("NullDate"));
                aNull >>= rParam.aNullDate;
                if(rParam.xFormatter.is())
                    rParam.xFormatter->attachNumberFormatsSupplier(xSuppl);
            }
        }
    }
}
/* -----------------------------17.07.00 17:04--------------------------------

 ---------------------------------------------------------------------------*/
BOOL lcl_MoveAbsolute(SwDSParam* pParam, long nAbsPos)
{
    BOOL bRet = FALSE;
    try
    {
        if(pParam->bScrollable)
        {
            bRet = pParam->xResultSet->absolute( nAbsPos );
        }
        else
        {
            DBG_ERROR("no absolute positioning available");
        }
    }
    catch(Exception aExcept)
    {
    }
    return bRet;
}
/* -----------------------------17.07.00 17:23--------------------------------

 ---------------------------------------------------------------------------*/
BOOL lcl_GetColumnCnt(SwDSParam* pParam,
    const String& rColumnName, long nLanguage, String& rResult, double* pNumber)
{
    uno::Reference< XColumnsSupplier > xColsSupp( pParam->xResultSet, UNO_QUERY );
    uno::Reference<XNameAccess> xCols;
    try
    {
        xCols = xColsSupp->getColumns();
    }
    catch( lang::DisposedException& )
    {
    }
    if(!xCols.is() || !xCols->hasByName(rColumnName))
        return FALSE;
    Any aCol = xCols->getByName(rColumnName);
    uno::Reference< XPropertySet > xColumnProps;
    aCol >>= xColumnProps;

    SwDBFormatData aFormatData;
    if(!pParam->xFormatter.is())
    {
        uno::Reference<XDataSource> xSource = SwNewDBMgr::getDataSourceAsParent(
                                    pParam->xConnection,pParam->sDataSource);
        lcl_InitNumberFormatter(*pParam, xSource );
    }
    aFormatData.aNullDate = pParam->aNullDate;
    aFormatData.xFormatter = pParam->xFormatter;

    MsLangId::convertLanguageToLocale( (LanguageType)nLanguage, aFormatData.aLocale );

    rResult = SwNewDBMgr::GetDBField( xColumnProps, aFormatData, pNumber);
    return TRUE;
};
/*--------------------------------------------------------------------
    Beschreibung: Daten importieren
 --------------------------------------------------------------------*/
BOOL SwNewDBMgr::MergeNew(const SwMergeDescriptor& rMergeDesc )
{
    SetMergeType( rMergeDesc.nMergeType );

    DBG_ASSERT(!bInMerge && !pImpl->pMergeData, "merge already activated!");

    SwDBData aData;
    aData.nCommandType = CommandType::TABLE;
    uno::Reference<XResultSet>  xResSet;
    Sequence<Any> aSelection;
    uno::Reference< XConnection> xConnection;

    aData.sDataSource = rMergeDesc.rDescriptor.getDataSource();
    rMergeDesc.rDescriptor[daCommand]      >>= aData.sCommand;
    rMergeDesc.rDescriptor[daCommandType]  >>= aData.nCommandType;

    if ( rMergeDesc.rDescriptor.has(daCursor) )
        rMergeDesc.rDescriptor[daCursor] >>= xResSet;
    if ( rMergeDesc.rDescriptor.has(daSelection) )
        rMergeDesc.rDescriptor[daSelection] >>= aSelection;
    if ( rMergeDesc.rDescriptor.has(daConnection) )
        rMergeDesc.rDescriptor[daConnection] >>= xConnection;

    if(!aData.sDataSource.getLength() || !aData.sCommand.getLength() || !xResSet.is())
    {
        return FALSE;
    }

    pImpl->pMergeData = new SwDSParam(aData, xResSet, aSelection);
    SwDSParam*  pTemp = FindDSData(aData, FALSE);
    if(pTemp)
        *pTemp = *pImpl->pMergeData;
    else
    {
        //#94779# calls from the calculator may have added a connection with an invalid commandtype
        //"real" data base connections added here have to re-use the already available
        //DSData and set the correct CommandType
        SwDBData aTempData(aData);
        aData.nCommandType = -1;
        pTemp = FindDSData(aData, FALSE);
        if(pTemp)
            *pTemp = *pImpl->pMergeData;
        else
        {
            SwDSParam* pInsert = new SwDSParam(*pImpl->pMergeData);
            aDataSourceParams.Insert(pInsert, aDataSourceParams.Count());
            try
            {
                uno::Reference<XComponent> xComponent(pInsert->xConnection, UNO_QUERY);
                if(xComponent.is())
                    xComponent->addEventListener(pImpl->xDisposeListener);
            }
            catch(Exception&)
            {
            }
        }
    }
    if(!pImpl->pMergeData->xConnection.is())
        pImpl->pMergeData->xConnection = xConnection;
    // add an XEventListener

    try{
        //set to start position
        if(pImpl->pMergeData->aSelection.getLength())
        {
            sal_Int32 nPos = 0;
            pImpl->pMergeData->aSelection.getConstArray()[ pImpl->pMergeData->nSelectionIndex++ ] >>= nPos;
            pImpl->pMergeData->bEndOfDB = !pImpl->pMergeData->xResultSet->absolute( nPos );
            pImpl->pMergeData->CheckEndOfDB();
            if(pImpl->pMergeData->nSelectionIndex >= pImpl->pMergeData->aSelection.getLength())
                pImpl->pMergeData->bEndOfDB = TRUE;
        }
        else
        {
            pImpl->pMergeData->bEndOfDB = !pImpl->pMergeData->xResultSet->first();
            pImpl->pMergeData->CheckEndOfDB();
        }
    }
    catch(Exception&)
    {
        pImpl->pMergeData->bEndOfDB = TRUE;
        pImpl->pMergeData->CheckEndOfDB();
        DBG_ERROR("exception in MergeNew()");
    }

    uno::Reference<XDataSource> xSource = SwNewDBMgr::getDataSourceAsParent(xConnection,aData.sDataSource);

    lcl_InitNumberFormatter(*pImpl->pMergeData, xSource);

    rMergeDesc.rSh.ChgDBData(aData);
    bInMerge = TRUE;

    if (IsInitDBFields())
    {
        // Bei Datenbankfeldern ohne DB-Name DB-Name von Dok einsetzen
        SvStringsDtor aDBNames(1, 1);
        aDBNames.Insert( new String(), 0);
        SwDBData aInsertData = rMergeDesc.rSh.GetDBData();
        String sDBName = aInsertData.sDataSource;
        sDBName += DB_DELIM;
        sDBName += (String)aInsertData.sCommand;
        sDBName += DB_DELIM;
        sDBName += String::CreateFromInt32(aInsertData.nCommandType);
        rMergeDesc.rSh.ChangeDBFields( aDBNames, sDBName);
        SetInitDBFields(FALSE);
    }

    BOOL bRet = TRUE;
    switch(rMergeDesc.nMergeType)
    {
        case DBMGR_MERGE:
            bRet = Merge(&rMergeDesc.rSh);   // Mischen
            break;

        case DBMGR_MERGE_MAILMERGE: // printing merge from 'old' merge dialog or from UNO-component
        case DBMGR_MERGE_MAILING:
        case DBMGR_MERGE_MAILFILES:
        case DBMGR_MERGE_SINGLE_FILE:
            // save files and send them as e-Mail if required
            bRet = MergeMailFiles(&rMergeDesc.rSh,
                    rMergeDesc);
            break;

        default:    	// Einfuegen der selektierten Eintraege
                        // (war: InsertRecord)
            ImportFromConnection(&rMergeDesc.rSh);
            break;
    }

    EndMerge();
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung: Daten importieren
 --------------------------------------------------------------------*/


BOOL SwNewDBMgr::Merge(SwWrtShell* pSh)
{
    pSh->StartAllAction();

    pSh->ViewShell::UpdateFlds(TRUE);
    pSh->SetModified();

    pSh->EndAllAction();

    return TRUE;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void SwNewDBMgr::ImportFromConnection(	SwWrtShell* pSh )
{
    if(pImpl->pMergeData && !pImpl->pMergeData->bEndOfDB)
    {
        {
            pSh->StartAllAction();
            pSh->StartUndo(UNDO_EMPTY);
            BOOL bGroupUndo(pSh->DoesGroupUndo());
            pSh->DoGroupUndo(FALSE);

            if( pSh->HasSelection() )
                pSh->DelRight();

            SwWait *pWait = 0;

            {
                ULONG i = 0;
                do {

                    ImportDBEntry(pSh);
                    if( 10 == ++i )
                        pWait = new SwWait( *pSh->GetView().GetDocShell(), TRUE);

                } while(ToNextMergeRecord());
            }

            pSh->DoGroupUndo(bGroupUndo);
            pSh->EndUndo(UNDO_EMPTY);
            pSh->EndAllAction();
            delete pWait;
        }
    }
}
/*-----------------24.02.97 10.30-------------------

--------------------------------------------------*/

String 	lcl_FindColumn(const String& sFormatStr,USHORT  &nUsedPos, BYTE &nSeparator)
{
    String sReturn;
    USHORT nLen = sFormatStr.Len();
    nSeparator = 0xff;
    while(nUsedPos < nLen && nSeparator == 0xff)
    {
        sal_Unicode cAkt = sFormatStr.GetChar(nUsedPos);
        switch(cAkt)
        {
            case ',':
                nSeparator = DB_SEP_SPACE;
            break;
            case ';':
                nSeparator = DB_SEP_RETURN;
            break;
            case ':':
                nSeparator = DB_SEP_TAB;
            break;
            case '#':
                nSeparator = DB_SEP_NEWLINE;
            break;
            default:
                sReturn += cAkt;
        }
        nUsedPos++;

    }
    return sReturn;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void SwNewDBMgr::ImportDBEntry(SwWrtShell* pSh)
{
    if(pImpl->pMergeData && !pImpl->pMergeData->bEndOfDB)
    {
        uno::Reference< XColumnsSupplier > xColsSupp( pImpl->pMergeData->xResultSet, UNO_QUERY );
        uno::Reference<XNameAccess> xCols = xColsSupp->getColumns();
        String sFormatStr;
        USHORT nFmtLen = sFormatStr.Len();
        if( nFmtLen )
        {
            const char cSpace = ' ';
            const char cTab = '\t';
            USHORT nUsedPos = 0;
            BYTE	nSeparator;
            String sColumn = lcl_FindColumn(sFormatStr, nUsedPos, nSeparator);
            while( sColumn.Len() )
            {
                if(!xCols->hasByName(sColumn))
                    return;
                Any aCol = xCols->getByName(sColumn);
                uno::Reference< XPropertySet > xColumnProp;
                aCol >>= xColumnProp;
                if(xColumnProp.is())
                {
                    SwDBFormatData aDBFormat;
                    String sInsert = GetDBField( xColumnProp,	aDBFormat);
                    if( DB_SEP_SPACE == nSeparator )
                            sInsert += cSpace;
                    else if( DB_SEP_TAB == nSeparator)
                            sInsert += cTab;
                    pSh->Insert(sInsert);
                    if( DB_SEP_RETURN == nSeparator)
                        pSh->SplitNode();
                    else if(DB_SEP_NEWLINE == nSeparator)
                            pSh->InsertLineBreak();
                }
                else
                {
                    // Spalte nicht gefunden -> Fehler anzeigen
                    String sInsert = '?';
                    sInsert += sColumn;
                    sInsert += '?';
                    pSh->Insert(sInsert);
                }
                sColumn = lcl_FindColumn(sFormatStr, nUsedPos, nSeparator);
            }
            pSh->SplitNode();
        }
        else
        {
            String sStr;
            Sequence<rtl::OUString> aColNames = xCols->getElementNames();
            const rtl::OUString* pColNames = aColNames.getConstArray();
            long nLength = aColNames.getLength();
            for(long i = 0; i < nLength; i++)
            {
                Any aCol = xCols->getByName(pColNames[i]);
                uno::Reference< XPropertySet > xColumnProp;
                aCol >>= xColumnProp;
                SwDBFormatData aDBFormat;
                sStr += GetDBField( xColumnProp, aDBFormat);
                if (i < nLength - 1)
                    sStr += '\t';
            }
            pSh->SwEditShell::Insert2(sStr);
            pSh->SwFEShell::SplitNode();	// Zeilenvorschub
        }
    }
}
/*--------------------------------------------------------------------
    Beschreibung: Listbox mit Tabellenliste fuellen
 --------------------------------------------------------------------*/
BOOL SwNewDBMgr::GetTableNames(ListBox* pListBox, const String& rDBName)
{
    BOOL bRet = FALSE;
    String sOldTableName(pListBox->GetSelectEntry());
    pListBox->Clear();
    SwDSParam* pParam = FindDSConnection(rDBName, FALSE);
    uno::Reference< XConnection> xConnection;
    if(pParam && pParam->xConnection.is())
        xConnection = pParam->xConnection;
    else
    {
        rtl::OUString sDBName(rDBName);
        xConnection = RegisterConnection( sDBName );
    }
    if(xConnection.is())
    {
        uno::Reference<XTablesSupplier> xTSupplier = uno::Reference<XTablesSupplier>(xConnection, UNO_QUERY);
        if(xTSupplier.is())
        {
            uno::Reference<XNameAccess> xTbls = xTSupplier->getTables();
            Sequence<rtl::OUString> aTbls = xTbls->getElementNames();
            const rtl::OUString* pTbls = aTbls.getConstArray();
            for(long i = 0; i < aTbls.getLength(); i++)
            {
                USHORT nEntry = pListBox->InsertEntry(pTbls[i]);
                pListBox->SetEntryData(nEntry, (void*)0);
            }
        }
        uno::Reference<XQueriesSupplier> xQSupplier = uno::Reference<XQueriesSupplier>(xConnection, UNO_QUERY);
        if(xQSupplier.is())
        {
            uno::Reference<XNameAccess> xQueries = xQSupplier->getQueries();
            Sequence<rtl::OUString> aQueries = xQueries->getElementNames();
            const rtl::OUString* pQueries = aQueries.getConstArray();
            for(long i = 0; i < aQueries.getLength(); i++)
            {
                USHORT nEntry = pListBox->InsertEntry(pQueries[i]);
                pListBox->SetEntryData(nEntry, (void*)1);
            }
        }
        if (sOldTableName.Len())
            pListBox->SelectEntry(sOldTableName);
        bRet = TRUE;
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung: Listbox mit Spaltennamen einer Datenbank fuellen
 --------------------------------------------------------------------*/
BOOL SwNewDBMgr::GetColumnNames(ListBox* pListBox,
            const String& rDBName, const String& rTableName, BOOL bAppend)
{
    if (!bAppend)
        pListBox->Clear();
    SwDBData aData;
    aData.sDataSource = rDBName;
    aData.sCommand = rTableName;
    aData.nCommandType = -1;
    SwDSParam* pParam = FindDSData(aData, FALSE);
    uno::Reference< XConnection> xConnection;
    if(pParam && pParam->xConnection.is())
        xConnection = pParam->xConnection;
    else
    {
        rtl::OUString sDBName(rDBName);
        xConnection = RegisterConnection( sDBName );
    }
    uno::Reference< XColumnsSupplier> xColsSupp = SwNewDBMgr::GetColumnSupplier(xConnection, rTableName);
    if(xColsSupp.is())
    {
        uno::Reference<XNameAccess> xCols = xColsSupp->getColumns();
        const Sequence<rtl::OUString> aColNames = xCols->getElementNames();
        const rtl::OUString* pColNames = aColNames.getConstArray();
        for(int nCol = 0; nCol < aColNames.getLength(); nCol++)
        {
            pListBox->InsertEntry(pColNames[nCol]);
        }
        ::comphelper::disposeComponent( xColsSupp );
    }
    return(TRUE);
}
/* -----------------------------08.06.01 15:11--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::GetColumnNames(ListBox* pListBox,
        uno::Reference< XConnection> xConnection,
        const String& rTableName, BOOL bAppend)
{
    if (!bAppend)
        pListBox->Clear();
    uno::Reference< XColumnsSupplier> xColsSupp = SwNewDBMgr::GetColumnSupplier(xConnection, rTableName);
    if(xColsSupp.is())
    {
        uno::Reference<XNameAccess> xCols = xColsSupp->getColumns();
        const Sequence<rtl::OUString> aColNames = xCols->getElementNames();
        const rtl::OUString* pColNames = aColNames.getConstArray();
        for(int nCol = 0; nCol < aColNames.getLength(); nCol++)
        {
            pListBox->InsertEntry(pColNames[nCol]);
        }
        ::comphelper::disposeComponent( xColsSupp );
    }
    return(TRUE);
}

/*--------------------------------------------------------------------
    Beschreibung: CTOR
 --------------------------------------------------------------------*/

SwNewDBMgr::SwNewDBMgr() :
            nMergeType(DBMGR_INSERT),
            bInitDBFields(FALSE),
            bInMerge(FALSE),
            bMergeSilent(FALSE),
            bMergeLock(FALSE),
            pImpl(new SwNewDBMgr_Impl(*this)),
            pMergeEvtSrc(NULL)
{
}
/* -----------------------------18.07.00 08:56--------------------------------

 ---------------------------------------------------------------------------*/
SwNewDBMgr::~SwNewDBMgr()
{
    for(USHORT nPos = 0; nPos < aDataSourceParams.Count(); nPos++)
    {
        SwDSParam* pParam = aDataSourceParams[nPos];
        if(pParam->xConnection.is())
        {
            try
            {
                uno::Reference<XComponent> xComp(pParam->xConnection, UNO_QUERY);
                if(xComp.is())
                    xComp->dispose();
            }
            catch(const RuntimeException& )
            {
                //may be disposed already since multiple entries may have used the same connection
            }
        }
    }
    delete pImpl;
}

/*--------------------------------------------------------------------
    Beschreibung:	Serienbriefe als einzelne Dokumente speichern
 --------------------------------------------------------------------*/
String lcl_FindUniqueName(SwWrtShell* pTargetShell, const String& rStartingPageDesc, ULONG nDocNo )
{
    do
    {
        String sTest = rStartingPageDesc;
        sTest += String::CreateFromInt32( nDocNo );
        if( !pTargetShell->FindPageDescByName( sTest ) )
            return sTest;
        ++nDocNo;
    }while(true);
}
void lcl_CopyDynamicDefaults( const SwDoc& rSource, SwDoc& rTarget )
{
    USHORT __FAR_DATA aRangeOfDefaults[] = {
        RES_FRMATR_BEGIN, RES_FRMATR_END-1,
        RES_CHRATR_BEGIN, RES_CHRATR_END-1,
        RES_PARATR_BEGIN, RES_PARATR_END-1,
        // --> OD 2008-02-25 #refactorlists##
        RES_PARATR_LIST_BEGIN, RES_PARATR_LIST_END-1,
        // <--
        RES_UNKNOWNATR_BEGIN, RES_UNKNOWNATR_END-1,
        0
    };

    SfxItemSet aNewDefaults( rTarget.GetAttrPool(), aRangeOfDefaults );

    USHORT nWhich;
    USHORT nRange = 0;
    while( aRangeOfDefaults[nRange] != 0)
    {
        for( nWhich = aRangeOfDefaults[nRange]; nWhich < aRangeOfDefaults[nRange + 1]; ++nWhich )
        {
            const SfxPoolItem& rSourceAttr = rSource.GetDefault( nWhich );
            if( rSourceAttr != rTarget.GetDefault( nWhich ) )
                aNewDefaults.Put( rSourceAttr );
        }
        nRange += 2;
    }
    if( aNewDefaults.Count() )
        rTarget.SetDefault( aNewDefaults );
}
void lcl_CopyFollowPageDesc(
                            SwWrtShell& rTargetShell,
                            const SwPageDesc& rSourcePageDesc,
                            const SwPageDesc& rTargetPageDesc,
                            const ULONG nDocNo )
{
    //now copy the follow page desc, too
    const SwPageDesc* pFollowPageDesc = rSourcePageDesc.GetFollow();
    String sFollowPageDesc = pFollowPageDesc->GetName();
    if( sFollowPageDesc != rSourcePageDesc.GetName() )
    {
        SwDoc* pTargetDoc = rTargetShell.GetDoc();
        String sNewFollowPageDesc = lcl_FindUniqueName(&rTargetShell, sFollowPageDesc, nDocNo );
        sal_uInt16 nNewDesc = pTargetDoc->MakePageDesc( sNewFollowPageDesc );
        SwPageDesc& rTargetFollowPageDesc = pTargetDoc->_GetPageDesc( nNewDesc );

        pTargetDoc->CopyPageDesc( *pFollowPageDesc, rTargetFollowPageDesc, sal_False );
        SwPageDesc aDesc( rTargetPageDesc );
        aDesc.SetFollow( &rTargetFollowPageDesc );
        pTargetDoc->ChgPageDesc( rTargetPageDesc.GetName(), aDesc );
    }
}

BOOL SwNewDBMgr::MergeMailFiles(SwWrtShell* pSourceShell,
        const SwMergeDescriptor& rMergeDescriptor)
{
    //check if the doc is synchronized and contains at least one linked section
    BOOL bSynchronizedDoc = pSourceShell->IsLabelDoc() && pSourceShell->GetSectionFmtCount() > 1;
    BOOL bLoop = TRUE;
    BOOL bEMail = rMergeDescriptor.nMergeType == DBMGR_MERGE_MAILING;
    const bool bAsSingleFile = rMergeDescriptor.nMergeType == DBMGR_MERGE_SINGLE_FILE;

    ::rtl::Reference< MailDispatcher >          xMailDispatcher;
    ::rtl::OUString sBodyMimeType;
    rtl_TextEncoding eEncoding = ::gsl_getSystemTextEncoding();

    if(bEMail)
    {
        xMailDispatcher.set( new MailDispatcher(rMergeDescriptor.xSmtpServer));
        if(!rMergeDescriptor.bSendAsAttachment && rMergeDescriptor.bSendAsHTML)
        {
            sBodyMimeType = ::rtl::OUString::createFromAscii("text/html; charset=");
            sBodyMimeType += ::rtl::OUString::createFromAscii(
                                rtl_getBestMimeCharsetFromTextEncoding( eEncoding ));
            SvxHtmlOptions* pHtmlOptions = SvxHtmlOptions::Get();
            eEncoding = pHtmlOptions->GetTextEncoding();
        }
        else
            sBodyMimeType =
                ::rtl::OUString::createFromAscii("text/plain; charset=UTF-8; format=flowed");
    }

    uno::Reference< XPropertySet > xColumnProp;
    {
        BOOL bColumnName = sEMailAddrFld.Len() > 0;

        if (bColumnName)
        {
            uno::Reference< XColumnsSupplier > xColsSupp( pImpl->pMergeData->xResultSet, UNO_QUERY );
            uno::Reference<XNameAccess> xCols = xColsSupp->getColumns();
            if(!xCols->hasByName(sEMailAddrFld))
                return FALSE;
            Any aCol = xCols->getByName(sEMailAddrFld);
            aCol >>= xColumnProp;
        }

        SfxDispatcher* pSfxDispatcher = pSourceShell->GetView().GetViewFrame()->GetDispatcher();
        SwDocShell* pSourrceDocSh = pSourceShell->GetView().GetDocShell();
        pSfxDispatcher->Execute( pSourrceDocSh->HasName() ? SID_SAVEDOC : SID_SAVEASDOC, SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD);
        // has document been saved successfully?
        if( !pSourrceDocSh->IsModified() )
        {
            SfxMedium* pOrig = pSourceShell->GetView().GetDocShell()->GetMedium();
            String sSourceDocumentURL(pOrig->GetURLObject().GetMainURL( INetURLObject::NO_DECODE ));
            const SfxFilter* pSfxFlt = SwIoSystem::GetFileFilter(
                                                    sSourceDocumentURL, ::aEmptyStr );
            const SfxFilter* pStoreToFilter = pSfxFlt;
            SfxFilterContainer* pFilterContainer = SwDocShell::Factory().GetFilterContainer();
            const String* pStoreToFilterOptions = 0;
            // if a save_to filter is set then use it - otherwise use the default
            if( bEMail && !rMergeDescriptor.bSendAsAttachment )
            {
                String sExtension( String::CreateFromAscii(
                        rMergeDescriptor.bSendAsHTML ? "html" : "txt" ));
                pStoreToFilter = pFilterContainer->GetFilter4Extension(sExtension, SFX_FILTER_EXPORT);
            }
            else if( rMergeDescriptor.sSaveToFilter.Len())
            {
                const SfxFilter* pFilter =
                        pFilterContainer->GetFilter4FilterName( rMergeDescriptor.sSaveToFilter );
                if(pFilter)
                {
                    pStoreToFilter = pFilter;
                    if(rMergeDescriptor.sSaveToFilterOptions.Len())
                        pStoreToFilterOptions = &rMergeDescriptor.sSaveToFilterOptions;
                }
            }
            bCancel = FALSE;

            // in case of creating a single resulting file this has to be created here
            SwWrtShell* pTargetShell = 0;
            SfxObjectShellRef xTargetDocShell;
            SwView* pTargetView = 0;
            std::auto_ptr< utl::TempFile > aTempFile;
            String sModifiedStartingPageDesc;
            String sStartingPageDesc;
            USHORT nStartingPageNo = 0;
            bool bPageStylesWithHeaderFooter = false;
            if(bAsSingleFile || rMergeDescriptor.bCreateSingleFile)
            {
                // create a target docshell to put the merged document into
                xTargetDocShell = new SwDocShell( SFX_CREATE_MODE_STANDARD );
                xTargetDocShell->DoInitNew( 0 );
                SfxViewFrame* pTargetFrame = SfxViewFrame::CreateViewFrame( *xTargetDocShell, 0, TRUE );

                pTargetView = static_cast<SwView*>( pTargetFrame->GetViewShell() );

                //initiate SelectShell() to create sub shells
                pTargetView->AttrChangedNotify( &pTargetView->GetWrtShell() );
                pTargetShell = pTargetView->GetWrtShellPtr();
                //copy the styles from the source to the target document
                SwgReaderOption aOpt;
                aOpt.SetTxtFmts( sal_True );
                aOpt.SetFrmFmts( sal_True );
                aOpt.SetPageDescs( sal_True );
                aOpt.SetNumRules( sal_True );
                aOpt.SetMerge( sal_False );
                pTargetView->GetDocShell()->LoadStylesFromFile(
                        sSourceDocumentURL, aOpt, sal_True );
                //determine the page style and number used at the start of the source document
                pSourceShell->SttEndDoc(TRUE);
                nStartingPageNo = pSourceShell->GetVirtPageNum();
                sStartingPageDesc = sModifiedStartingPageDesc = pSourceShell->GetPageDesc(
                                            pSourceShell->GetCurPageDesc()).GetName();
                // #122799# copy compatibility options
                lcl_CopyCompatibilityOptions( *pSourceShell, *pTargetShell);
                // #72821# copy dynamic defaults
                lcl_CopyDynamicDefaults( *pSourceShell->GetDoc(), *pTargetShell->GetDoc() );
                // #i72517#
                const SwPageDesc* pSourcePageDesc = pSourceShell->FindPageDescByName( sStartingPageDesc );
                const SwFrmFmt& rMaster = pSourcePageDesc->GetMaster();
                bPageStylesWithHeaderFooter = rMaster.GetHeader().IsActive()  ||
                                                rMaster.GetFooter().IsActive();

            }

            PrintMonitor aPrtMonDlg(&pSourceShell->GetView().GetEditWin(), PrintMonitor::MONITOR_TYPE_PRINT);
            aPrtMonDlg.aDocName.SetText(pSourceShell->GetView().GetDocShell()->GetTitle(22));

            aPrtMonDlg.aCancel.SetClickHdl(LINK(this, SwNewDBMgr, PrtCancelHdl));
            if (!IsMergeSilent())
                aPrtMonDlg.Show();

            // Progress, um KeyInputs zu unterbinden
            SfxProgress aProgress(pSourrceDocSh, ::aEmptyStr, 1);

            // Alle Dispatcher sperren
            SfxViewFrame* pViewFrm = SfxViewFrame::GetFirst(pSourrceDocSh);
            while (pViewFrm)
            {
                pViewFrm->GetDispatcher()->Lock(TRUE);
                pViewFrm = SfxViewFrame::GetNext(*pViewFrm, pSourrceDocSh);
            }
            ULONG nDocNo = 1;

            long nStartRow, nEndRow;
            // collect temporary files
            ::std::vector< String> aFilesToRemove;
            do
            {
                nStartRow = pImpl->pMergeData ? pImpl->pMergeData->xResultSet->getRow() : 0;
                {
                    String sPath(sSubject);

                    String sAddress;
                    if( !bEMail && bColumnName )
                    {
                        SwDBFormatData aDBFormat;
                        aDBFormat.xFormatter = pImpl->pMergeData->xFormatter;
                        aDBFormat.aNullDate = pImpl->pMergeData->aNullDate;
                        sAddress = GetDBField( xColumnProp,	aDBFormat);
                        if (!sAddress.Len())
                            sAddress = '_';
                        sPath += sAddress;
                    }

                    // create a new temporary file name - only done once in case of bCreateSingleFile
                    if( 1 == nDocNo || (!rMergeDescriptor.bCreateSingleFile && !bAsSingleFile) )
                    {
                        INetURLObject aEntry(sPath);
                        String sLeading;
                        //#i97667# if the name is from a database field then it will be used _as is_
                        if( sAddress.Len() )
                            sLeading = sAddress;
                        else
                            sLeading = aEntry.GetBase();
                        aEntry.removeSegment();
                        sPath = aEntry.GetMainURL( INetURLObject::NO_DECODE );
                        String sExt( pStoreToFilter->GetDefaultExtension() );
                        sExt.EraseLeadingChars('*');
                        aTempFile = std::auto_ptr< utl::TempFile >(
                                new utl::TempFile(sLeading,&sExt,&sPath ));
                        if( bAsSingleFile )
                            aTempFile->EnableKillingFile();
                    }

                    if( !aTempFile->IsValid() )
                    {
                        ErrorHandler::HandleError( ERRCODE_IO_NOTSUPPORTED );
                        bLoop = FALSE;
                        bCancel = TRUE;
                    }
                    else
                    {
                        INetURLObject aTempFileURL(aTempFile->GetURL());
                        aPrtMonDlg.aPrinter.SetText( aTempFileURL.GetBase() );
                        String sStat(SW_RES(STR_STATSTR_LETTER));	// Brief
                        sStat += ' ';
                        sStat += String::CreateFromInt32( nDocNo );
                        aPrtMonDlg.aPrintInfo.SetText(sStat);

                        // Rechenzeit fuer Save-Monitor:
                        for (USHORT i = 0; i < 10; i++)
                            Application::Reschedule();

                        // Neues Dokument erzeugen und speichern
                        SfxObjectShellRef xWorkDocSh( new SwDocShell( SFX_CREATE_MODE_INTERNAL ));
                        SfxMedium* pWorkMed = new SfxMedium( sSourceDocumentURL, STREAM_STD_READ, TRUE );
                        pWorkMed->SetFilter( pSfxFlt );

                        if (xWorkDocSh->DoLoad(pWorkMed))
                        {
                            //create a view frame for the document
                            SfxViewFrame* pWorkFrame = SfxViewFrame::CreateViewFrame( *xWorkDocSh, 0, TRUE );
                            //request the layout calculation
                            SwWrtShell& rWorkShell =
                                    static_cast< SwView* >(pWorkFrame->GetViewShell())->GetWrtShell();
                            rWorkShell.CalcLayout();
                            SwDoc* pWorkDoc = ((SwDocShell*)(&xWorkDocSh))->GetDoc();
                            SwNewDBMgr* pOldDBMgr = pWorkDoc->GetNewDBMgr();
                            pWorkDoc->SetNewDBMgr( this );
                            SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE, SwDocShell::GetEventName(STR_SW_EVENT_FIELD_MERGE), xWorkDocSh));
                            pWorkDoc->UpdateFlds(NULL, false);
                            SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE_FINISHED, SwDocShell::GetEventName(STR_SW_EVENT_FIELD_MERGE_FINISHED), xWorkDocSh));

                            // alle versteckten Felder/Bereiche entfernen
                            pWorkDoc->RemoveInvisibleContent();

                            // launch MailMergeEvent if required
                            const SwXMailMerge *pEvtSrc = GetMailMergeEvtSrc();
                            if(pEvtSrc)
                            {
                                uno::Reference< XInterface > xRef( (XMailMergeBroadcaster *) pEvtSrc );
                                text::MailMergeEvent aEvt( xRef, xWorkDocSh->GetModel() );
                                pEvtSrc->LaunchMailMergeEvent( aEvt );
                            }

                            if(rMergeDescriptor.bCreateSingleFile || bAsSingleFile )
                            {
                                DBG_ASSERT( pTargetShell, "no target shell available!" );
                                // copy created file into the target document
                                rWorkShell.ConvertFieldsToText();
                                rWorkShell.SetNumberingRestart();

                                // insert the document into the target document
                                rWorkShell.SttEndDoc(FALSE);
                                rWorkShell.SttEndDoc(TRUE);
                                rWorkShell.SelAll();
                                pTargetShell->SwCrsrShell::SttEndDoc( FALSE );
                                //#i72517# the headers and footers are still those from the source - update in case of fields inside header/footer
                                if( !nDocNo && bPageStylesWithHeaderFooter )
                                    pTargetShell->GetView().GetDocShell()->_LoadStyles( *rWorkShell.GetView().GetDocShell(), sal_True );
                                //#i72517# put the styles to the target document
                                //if the source uses headers or footers each new copy need to copy a new page styles
                                if(bPageStylesWithHeaderFooter)
                                {
                                    //create a new pagestyle
                                    //copy the pagedesc from the current document to the new document and change the name of the to-be-applied style

                                    SwDoc* pTargetDoc = pTargetShell->GetDoc();
                                    SwPageDesc* pSourcePageDesc = rWorkShell.FindPageDescByName( sStartingPageDesc );
                                    String sNewPageDescName = lcl_FindUniqueName(pTargetShell, sStartingPageDesc, nDocNo );
                                    pTargetDoc->MakePageDesc( sNewPageDescName );
                                    SwPageDesc* pTargetPageDesc = pTargetShell->FindPageDescByName( sNewPageDescName );
                                    if(pSourcePageDesc && pTargetPageDesc)
                                    {
                                        pTargetDoc->CopyPageDesc( *pSourcePageDesc, *pTargetPageDesc, sal_False );
                                        sModifiedStartingPageDesc = sNewPageDescName;
                                        lcl_CopyFollowPageDesc( *pTargetShell, *pSourcePageDesc, *pTargetPageDesc, nDocNo );
                                    }
                                }

                                if(nDocNo > 1)
                                    pTargetShell->InsertPageBreak( &sModifiedStartingPageDesc, nStartingPageNo );
                                else
                                    pTargetShell->SetPageStyle(sModifiedStartingPageDesc);
                                DBG_ASSERT(!pTargetShell->GetTableFmt(),"target document ends with a table - paragraph should be appended");
                                //#i51359# add a second paragraph in case there's only one
                                {
                                    SwNodeIndex aIdx( pWorkDoc->GetNodes().GetEndOfExtras(), 2 );
                                    SwPosition aTestPos( aIdx );
                                    SwCursor aTestCrsr(aTestPos,0,false);
                                    if(!aTestCrsr.MovePara(fnParaNext, fnParaStart))
                                    {
                                        //append a paragraph
                                        pWorkDoc->AppendTxtNode( aTestPos );
                                    }
                                }
                                pTargetShell->Paste( rWorkShell.GetDoc(), sal_True );

                                //convert fields in page styles (header/footer - has to be done after the first document has been pasted
                                if(1 == nDocNo)
                                {
                                    pTargetShell->CalcLayout();
                                    pTargetShell->ConvertFieldsToText();
                                }
                            }
                            else
                            {
                                String sFileURL =  aTempFileURL.GetMainURL( INetURLObject::NO_DECODE );
                                SfxMedium* pDstMed = new SfxMedium(
                                    sFileURL,
                                    STREAM_STD_READWRITE, TRUE );
                                pDstMed->SetFilter( pStoreToFilter );
                                if(pDstMed->GetItemSet())
                                {
                                    if(pStoreToFilterOptions )
                                        pDstMed->GetItemSet()->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, *pStoreToFilterOptions));
                                    if(rMergeDescriptor.aSaveToFilterData.getLength())
                                        pDstMed->GetItemSet()->Put(SfxUsrAnyItem(SID_FILTER_DATA, makeAny(rMergeDescriptor.aSaveToFilterData)));
                                }

                                //convert fields to text if we are exporting to PDF
                                //this prevents a second merge while updating the fields in SwXTextDocument::getRendererCount()
                                if( pStoreToFilter && pStoreToFilter->GetFilterName().EqualsAscii("writer_pdf_Export"))
                                    rWorkShell.ConvertFieldsToText();
                                xWorkDocSh->DoSaveAs(*pDstMed);
                                xWorkDocSh->DoSaveCompleted(pDstMed);
                                if( xWorkDocSh->GetError() )
                                {
                                    // error message ??
                                    ErrorHandler::HandleError( xWorkDocSh->GetError() );
                                    bCancel = TRUE;
                                    bLoop = FALSE;
                                }
                                if( bEMail )
                                {
                                    SwDBFormatData aDBFormat;
                                    aDBFormat.xFormatter = pImpl->pMergeData->xFormatter;
                                    aDBFormat.aNullDate = pImpl->pMergeData->aNullDate;
                                    String sMailAddress = GetDBField( xColumnProp, aDBFormat);
                                    if(!SwMailMergeHelper::CheckMailAddress( sMailAddress ))
                                    {
                                        DBG_ERROR("invalid e-Mail address in database column");
                                    }
                                    else
                                    {
                                        SwMailMessage* pMessage = 0;
                                        uno::Reference< mail::XMailMessage > xMessage =
                                                                pMessage = new SwMailMessage;
                                        if(rMergeDescriptor.pMailMergeConfigItem->IsMailReplyTo())
                                            pMessage->setReplyToAddress(rMergeDescriptor.pMailMergeConfigItem->GetMailReplyTo());
                                        pMessage->addRecipient( sMailAddress );
                                        pMessage->SetSenderAddress( rMergeDescriptor.pMailMergeConfigItem->GetMailAddress() );
                                        ::rtl::OUString sBody;
                                        if(rMergeDescriptor.bSendAsAttachment)
                                        {
                                            sBody = rMergeDescriptor.sMailBody;
                                            mail::MailAttachment aAttach;
                                            aAttach.Data = new SwMailTransferable(
                                                    sFileURL,
                                                    rMergeDescriptor.sAttachmentName,
                                                    pStoreToFilter->GetMimeType());
                                            aAttach.ReadableName = rMergeDescriptor.sAttachmentName;
                                            pMessage->addAttachment( aAttach );
                                        }
                                        else
                                        {
                                            {
                                                //read in the temporary file and use it as mail body
                                                SfxMedium aMedium( sFileURL, STREAM_READ, TRUE);
                                                SvStream* pInStream = aMedium.GetInStream();
                                                DBG_ASSERT(pInStream, "no output file created?");
                                                if(pInStream)
                                                {
                                                    pInStream->SetStreamCharSet( eEncoding );
                                                    ByteString sLine;
                                                    sal_Bool bDone = pInStream->ReadLine( sLine );
                                                    while ( bDone )
                                                    {
                                                        sBody += String(sLine, eEncoding);
                                                        sBody += ::rtl::OUString('\n');
                                                        bDone = pInStream->ReadLine( sLine );
                                                    }
                                                }
                                            }
                                        }
                                        pMessage->setSubject( rMergeDescriptor.sSubject );
                                        uno::Reference< datatransfer::XTransferable> xBody =
                                                    new SwMailTransferable(
                                                        sBody,
                                                        sBodyMimeType);
                                        pMessage->setBody( xBody );

                                        if(rMergeDescriptor.aCopiesTo.getLength())
                                        {
                                            const ::rtl::OUString* pCopies = rMergeDescriptor.aCopiesTo.getConstArray();
                                            for( sal_Int32 nToken = 0; nToken < rMergeDescriptor.aCopiesTo.getLength(); ++nToken)
                                                pMessage->addCcRecipient( pCopies[nToken] );
                                        }
                                        if(rMergeDescriptor.aBlindCopiesTo.getLength())
                                        {
                                            const ::rtl::OUString* pCopies = rMergeDescriptor.aBlindCopiesTo.getConstArray();
                                            for( sal_Int32 nToken = 0; nToken < rMergeDescriptor.aBlindCopiesTo.getLength(); ++nToken)
                                                pMessage->addBccRecipient( pCopies[nToken] );
                                        }
                                        xMailDispatcher->enqueueMailMessage( xMessage );
                                        if(!xMailDispatcher->isStarted())
                                                xMailDispatcher->start();
                                        //schedule for removal
                                        aFilesToRemove.push_back(sFileURL);
                                    }
                                }
                            }
                            pWorkDoc->SetNewDBMgr( pOldDBMgr );
                        }
                        xWorkDocSh->DoClose();
                    }
                }
                nDocNo++;
                nEndRow = pImpl->pMergeData ? pImpl->pMergeData->xResultSet->getRow() : 0;
            } while( !bCancel &&
                (bSynchronizedDoc && (nStartRow != nEndRow)? ExistsNextRecord() : ToNextMergeRecord()));
            // save the single output document
            if(rMergeDescriptor.bCreateSingleFile || bAsSingleFile)
            {
                if( rMergeDescriptor.nMergeType != DBMGR_MERGE_MAILMERGE )
                {
                    DBG_ASSERT( aTempFile.get(), "Temporary file not available" );
                    INetURLObject aTempFileURL(bAsSingleFile ? sSubject : aTempFile->GetURL());
                    SfxMedium* pDstMed = new SfxMedium(
                        aTempFileURL.GetMainURL( INetURLObject::NO_DECODE ),
                        STREAM_STD_READWRITE, TRUE );
                    pDstMed->SetFilter( pStoreToFilter );
                    if(pDstMed->GetItemSet())
                    {
                        if(pStoreToFilterOptions )
                            pDstMed->GetItemSet()->Put(SfxStringItem(SID_FILE_FILTEROPTIONS, *pStoreToFilterOptions));
                        if(rMergeDescriptor.aSaveToFilterData.getLength())
                            pDstMed->GetItemSet()->Put(SfxUsrAnyItem(SID_FILTER_DATA, makeAny(rMergeDescriptor.aSaveToFilterData)));
                    }

                    xTargetDocShell->DoSaveAs(*pDstMed);
                    xTargetDocShell->DoSaveCompleted(pDstMed);
                    if( xTargetDocShell->GetError() )
                    {
                        // error message ??
                        ErrorHandler::HandleError( xTargetDocShell->GetError() );
                        bLoop = FALSE;
                    }
                }
                else if( pTargetView ) // must be available!
                {
                    //print the target document
        #if OSL_DEBUG_LEVEL > 1
                    sal_Bool  _bVal;
                    sal_Int16 _nVal;
                    rtl::OUString  _sVal;
                    const beans::PropertyValue* pDbgPrintOptions = rMergeDescriptor.aPrintOptions.getConstArray();
                    for( sal_Int32 nOption = 0; nOption < rMergeDescriptor.aPrintOptions.getLength(); ++nOption)
                    {
                        rtl::OUString aName( pDbgPrintOptions[nOption].Name );
                        uno::Any aVal( pDbgPrintOptions[nOption].Value );
                        aVal >>= _bVal;
                        aVal >>= _nVal;
                        aVal >>= _sVal;
                    }
        #endif
                    // printing should be done synchronously otherwise the document 
                    // might already become invalid during the process
                    uno::Sequence< beans::PropertyValue > aOptions( rMergeDescriptor.aPrintOptions );
                    const sal_Int32 nOpts = aOptions.getLength();
                    aOptions.realloc( nOpts + 1 );
                    aOptions[ nOpts ].Name = rtl::OUString::createFromAscii( "Wait" );
                    aOptions[ nOpts ].Value <<= sal_True ;
//                    aPrintArgs.Put(SfxBoolItem(FN_QRY_MERGE, TRUE) ); 
//                    // #i52629# aynchronous printing should only be done in silent mode - otherwise
//                    // the printer dialog does not come up
//                    aPrintArgs.Put( SfxBoolItem( SID_ASYNCHRON, rMergeDescriptor.bPrintAsync ));
                    // move print options 
                    const beans::PropertyValue* pPrintOptions = rMergeDescriptor.aPrintOptions.getConstArray();
                    for( sal_Int32 nOption = 0; nOption < rMergeDescriptor.aPrintOptions.getLength(); ++nOption)
                    {
                        if( pPrintOptions[nOption].Name.equalsAscii( "CopyCount" )
                            ||( pPrintOptions[nOption].Name.equalsAscii( "FileName" ))
                            ||( pPrintOptions[nOption].Name.equalsAscii( "Collate" ))
                            ||( pPrintOptions[nOption].Name.equalsAscii( "Pages" ))
                            ||( pPrintOptions[nOption].Name.equalsAscii( "Wait" )))
                        {
                            aOptions.realloc( nOpts + 1 );
                            aOptions[ nOpts ].Name = pPrintOptions[nOption].Name;
                            aOptions[ nOpts ].Value = pPrintOptions[nOption].Value ;
                        }
                    }

//                    const SwModuleOptions * pModOpt = SW_MOD()->GetModuleConfig();
//                    if (pModOpt->IsSinglePrintJob())
//                    {
//                    }    
//                    else
//                    {
                        pTargetView->ExecPrint( aOptions, IsMergeSilent(), rMergeDescriptor.bPrintAsync );
//                    }    
                }
                xTargetDocShell->DoClose();
            }

            //remove the temporary files
            ::std::vector<String>::iterator aFileIter;
            for(aFileIter = aFilesToRemove.begin();
                        aFileIter != aFilesToRemove.end(); aFileIter++)
                SWUnoHelper::UCB_DeleteFile( *aFileIter );

            // Alle Dispatcher freigeben
            pViewFrm = SfxViewFrame::GetFirst(pSourrceDocSh);
            while (pViewFrm)
            {
                pViewFrm->GetDispatcher()->Lock(FALSE);
                pViewFrm = SfxViewFrame::GetNext(*pViewFrm, pSourrceDocSh);
            }

            SW_MOD()->SetView(&pSourceShell->GetView());
        }

        nMergeType = DBMGR_INSERT;
    }

    if(bEMail)
    {
        xMailDispatcher->stop();
        xMailDispatcher->shutdown();

    }

    return bLoop;
}

/*--------------------------------------------------------------------
    Beschreibung:
  --------------------------------------------------------------------*/

IMPL_LINK_INLINE_START( SwNewDBMgr, PrtCancelHdl, Button *, pButton )
{
    pButton->GetParent()->Hide();
    bCancel = TRUE;
    return 0;
}
IMPL_LINK_INLINE_END( SwNewDBMgr, PrtCancelHdl, Button *, pButton )


/*--------------------------------------------------------------------
    Beschreibung: Numberformat der Spalte ermitteln und ggfs. in
                    den uebergebenen Formatter uebertragen
  --------------------------------------------------------------------*/

ULONG SwNewDBMgr::GetColumnFmt( const String& rDBName,
                                const String& rTableName,
                                const String& rColNm,
                                SvNumberFormatter* pNFmtr,
                                long nLanguage )
{
    ULONG nRet = 0;
    if(pNFmtr)
    {
        uno::Reference< XDataSource> xSource;
        uno::Reference< XConnection> xConnection;
        sal_Bool bUseMergeData = sal_False;
        uno::Reference< XColumnsSupplier> xColsSupp;
        bool bDisposeConnection = false;
        if(pImpl->pMergeData &&
            pImpl->pMergeData->sDataSource.equals(rDBName) && pImpl->pMergeData->sCommand.equals(rTableName))
        {
            xConnection = pImpl->pMergeData->xConnection;
            xSource = SwNewDBMgr::getDataSourceAsParent(xConnection,rDBName);
            bUseMergeData = sal_True;
            xColsSupp = xColsSupp.query( pImpl->pMergeData->xResultSet );
        }
        if(!xConnection.is())
        {
            SwDBData aData;
            aData.sDataSource = rDBName;
            aData.sCommand = rTableName;
            aData.nCommandType = -1;
            SwDSParam* pParam = FindDSData(aData, FALSE);
            if(pParam && pParam->xConnection.is())
            {
                xConnection = pParam->xConnection;
                xColsSupp = xColsSupp.query( pParam->xResultSet );
            }
            else
            {
                rtl::OUString sDBName(rDBName);
                xConnection = RegisterConnection( sDBName );
                bDisposeConnection = true;
            }
            if(bUseMergeData)
                pImpl->pMergeData->xConnection = xConnection;
        }
        bool bDispose = !xColsSupp.is();
        if(bDispose)
        {
            xColsSupp = SwNewDBMgr::GetColumnSupplier(xConnection, rTableName);
        }
        if(xColsSupp.is())
        {
            uno::Reference<XNameAccess> xCols;
            try
            {
                xCols = xColsSupp->getColumns();
            }
            catch(Exception&)
            {
                DBG_ERROR("Exception in getColumns()");
            }
            if(!xCols.is() || !xCols->hasByName(rColNm))
                return nRet;
            Any aCol = xCols->getByName(rColNm);
            uno::Reference< XPropertySet > xColumn;
            aCol >>= xColumn;
            nRet = GetColumnFmt(xSource, xConnection, xColumn, pNFmtr, nLanguage);
            if(bDispose)
            {
                ::comphelper::disposeComponent( xColsSupp );
            }
            if(bDisposeConnection)
            {
                ::comphelper::disposeComponent( xConnection );
            }
        }
        else
            nRet = pNFmtr->GetFormatIndex( NF_NUMBER_STANDARD, LANGUAGE_SYSTEM );
    }
    return nRet;
}
/* -----------------------------07.06.01 15:43--------------------------------

 ---------------------------------------------------------------------------*/
ULONG SwNewDBMgr::GetColumnFmt( uno::Reference< XDataSource> xSource,
                        uno::Reference< XConnection> xConnection,
                        uno::Reference< XPropertySet> xColumn,
                        SvNumberFormatter* pNFmtr,
                        long nLanguage )
{
    //JP 12.01.99: ggfs. das NumberFormat im Doc setzen
    ULONG nRet = 0;

    if(!xSource.is())
    {
        uno::Reference<XChild> xChild(xConnection, UNO_QUERY);
        xSource = uno::Reference<XDataSource>(xChild->getParent(), UNO_QUERY);
    }
    if(xSource.is() && xConnection.is() && xColumn.is() && pNFmtr)
    {
        SvNumberFormatsSupplierObj* pNumFmt = new SvNumberFormatsSupplierObj( pNFmtr );
        uno::Reference< util::XNumberFormatsSupplier >  xDocNumFmtsSupplier = pNumFmt;
        uno::Reference< XNumberFormats > xDocNumberFormats = xDocNumFmtsSupplier->getNumberFormats();
        uno::Reference< XNumberFormatTypes > xDocNumberFormatTypes(xDocNumberFormats, UNO_QUERY);

        Locale aLocale( MsLangId::convertLanguageToLocale( (LanguageType)nLanguage ));

        //get the number formatter of the data source
        uno::Reference<XPropertySet> xSourceProps(xSource, UNO_QUERY);
        uno::Reference< XNumberFormats > xNumberFormats;
        if(xSourceProps.is())
        {
            Any aFormats = xSourceProps->getPropertyValue(C2U("NumberFormatsSupplier"));
            if(aFormats.hasValue())
            {
                uno::Reference<XNumberFormatsSupplier> xSuppl;
                aFormats >>= xSuppl;
                if(xSuppl.is())
                {
                    xNumberFormats = xSuppl->getNumberFormats();
                }
            }
        }
        bool bUseDefault = true;
        try
        {
            Any aFormatKey = xColumn->getPropertyValue(C2U("FormatKey"));
            if(aFormatKey.hasValue())
            {
                sal_Int32 nFmt = 0;
                aFormatKey >>= nFmt;
                if(xNumberFormats.is())
                {
                    try
                    {
                        uno::Reference<XPropertySet> xNumProps = xNumberFormats->getByKey( nFmt );
                        Any aFormatString = xNumProps->getPropertyValue(C2U("FormatString"));
                        Any aLocaleVal = xNumProps->getPropertyValue(C2U("Locale"));
                        rtl::OUString sFormat;
                        aFormatString >>= sFormat;
                        lang::Locale aLoc;
                        aLocaleVal >>= aLoc;
                        nFmt = xDocNumberFormats->queryKey( sFormat, aLoc, sal_False );
                        if(NUMBERFORMAT_ENTRY_NOT_FOUND == sal::static_int_cast< sal_uInt32, sal_Int32>(nFmt))
                            nFmt = xDocNumberFormats->addNew( sFormat, aLoc );
                        nRet = nFmt;
                        bUseDefault = false;
                    }
                    catch(const Exception&)
                    {
                        DBG_ERROR("illegal number format key");
                    }
                }
            }
        }
        catch( const Exception& )
        {
            DBG_ERROR("no FormatKey property found");
        }
        if(bUseDefault)
            nRet = SwNewDBMgr::GetDbtoolsClient().getDefaultNumberFormat(xColumn, xDocNumberFormatTypes,  aLocale);
    }
    return nRet;
}

/* -----------------------------17.07.00 09:47--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int32 SwNewDBMgr::GetColumnType( const String& rDBName,
                          const String& rTableName,
                          const String& rColNm )
{
    sal_Int32 nRet = DataType::SQLNULL;
    SwDBData aData;
    aData.sDataSource = rDBName;
    aData.sCommand = rTableName;
    aData.nCommandType = -1;
    SwDSParam* pParam = FindDSData(aData, FALSE);
    uno::Reference< XConnection> xConnection;
    uno::Reference< XColumnsSupplier > xColsSupp;
    bool bDispose = false;
    if(pParam && pParam->xConnection.is())
    {
        xConnection = pParam->xConnection;
        xColsSupp = uno::Reference< XColumnsSupplier >( pParam->xResultSet, UNO_QUERY );
    }
    else
    {
        rtl::OUString sDBName(rDBName);
        xConnection = RegisterConnection( sDBName );
    }
    if( !xColsSupp.is() )
    {
        xColsSupp = SwNewDBMgr::GetColumnSupplier(xConnection, rTableName);
        bDispose = true;
    }
    if(xColsSupp.is())
    {
        uno::Reference<XNameAccess> xCols = xColsSupp->getColumns();
        if(xCols->hasByName(rColNm))
        {
            Any aCol = xCols->getByName(rColNm);
            uno::Reference<XPropertySet> xCol;
            aCol >>= xCol;
            Any aType = xCol->getPropertyValue(C2S("Type"));
            aType >>= nRet;
        }
        if(bDispose)
            ::comphelper::disposeComponent( xColsSupp );
    }
    return nRet;
}

/* -----------------------------03.07.00 17:12--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< sdbc::XConnection> SwNewDBMgr::GetConnection(const String& rDataSource,
                                                    uno::Reference<XDataSource>& rxSource)
{
    Reference< sdbc::XConnection> xConnection;
    Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
    try
    {
        Reference<XCompletedConnection> xComplConnection(SwNewDBMgr::GetDbtoolsClient().getDataSource(rDataSource, xMgr),UNO_QUERY);
        if ( xComplConnection.is() )
        {
            rxSource.set(xComplConnection,UNO_QUERY);
            Reference< XInteractionHandler > xHandler(
                    xMgr->createInstance( C2U( "com.sun.star.sdb.InteractionHandler" )), UNO_QUERY);
                xConnection = xComplConnection->connectWithCompletion( xHandler );
        }
    }
    catch(Exception&) {}

    return xConnection;
}
/* -----------------------------03.07.00 17:12--------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< sdbcx::XColumnsSupplier> SwNewDBMgr::GetColumnSupplier(uno::Reference<sdbc::XConnection> xConnection,
                                    const String& rTableOrQuery,
                                    BYTE	eTableOrQuery)
{
    Reference< sdbcx::XColumnsSupplier> xRet;
    try
    {
        if(eTableOrQuery == SW_DB_SELECT_UNKNOWN)
        {
            //search for a table with the given command name
            Reference<XTablesSupplier> xTSupplier = Reference<XTablesSupplier>(xConnection, UNO_QUERY);
            if(xTSupplier.is())
            {
                Reference<XNameAccess> xTbls = xTSupplier->getTables();
                eTableOrQuery = xTbls->hasByName(rTableOrQuery) ?
                            SW_DB_SELECT_TABLE : SW_DB_SELECT_QUERY;
            }
        }
        sal_Int32 nCommandType = SW_DB_SELECT_TABLE == eTableOrQuery ?
                CommandType::TABLE : CommandType::QUERY;
        Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
        Reference<XRowSet> xRowSet(
                xMgr->createInstance(C2U("com.sun.star.sdb.RowSet")), UNO_QUERY);

        ::rtl::OUString sDataSource;
        Reference<XDataSource> xSource = SwNewDBMgr::getDataSourceAsParent(xConnection, sDataSource);
        Reference<XPropertySet> xSourceProperties(xSource, UNO_QUERY);
        if(xSourceProperties.is())
        {
            xSourceProperties->getPropertyValue(C2U("Name")) >>= sDataSource;
        }

        Reference<XPropertySet> xRowProperties(xRowSet, UNO_QUERY);
        xRowProperties->setPropertyValue(C2U("DataSourceName"), makeAny(sDataSource));
        xRowProperties->setPropertyValue(C2U("Command"), makeAny(::rtl::OUString(rTableOrQuery)));
        xRowProperties->setPropertyValue(C2U("CommandType"), makeAny(nCommandType));
        xRowProperties->setPropertyValue(C2U("FetchSize"), makeAny((sal_Int32)10));
        xRowProperties->setPropertyValue(C2U("ActiveConnection"), makeAny(xConnection));
        xRowSet->execute();
        xRet = Reference<XColumnsSupplier>( xRowSet, UNO_QUERY );
    }
    catch( const uno::Exception& )
    {
        DBG_ERROR("Exception in SwDBMgr::GetColumnSupplier");
    }

    return xRet;
}
/* -----------------------------05.07.00 13:44--------------------------------

 ---------------------------------------------------------------------------*/
String SwNewDBMgr::GetDBField(uno::Reference<XPropertySet> xColumnProps,
                        const SwDBFormatData& rDBFormatData,
                        double* pNumber)
{
    uno::Reference< XColumn > xColumn(xColumnProps, UNO_QUERY);
    String sRet;
    DBG_ASSERT(xColumn.is(), "SwNewDBMgr::::ImportDBField: illegal arguments");
    if(!xColumn.is())
        return sRet;

    Any aType = xColumnProps->getPropertyValue(C2U("Type"));
    sal_Int32 eDataType = 0;
    aType >>= eDataType;
    switch(eDataType)
    {
        case DataType::CHAR:
        case DataType::VARCHAR:
        case DataType::LONGVARCHAR:
            try
            {
                sRet = xColumn->getString();
            }
            catch( SQLException& )
            {
            }
        break;
        case DataType::BIT:
        case DataType::BOOLEAN:
        case DataType::TINYINT:
        case DataType::SMALLINT:
        case DataType::INTEGER:
        case DataType::BIGINT:
        case DataType::FLOAT:
        case DataType::REAL:
        case DataType::DOUBLE:
        case DataType::NUMERIC:
        case DataType::DECIMAL:
        case DataType::DATE:
        case DataType::TIME:
        case DataType::TIMESTAMP:
        {
//			::Date aTempDate(rDBFormatData.aNullDate.Day,
//				rDBFormatData.aNullDate.Month, rDBFormatData.aNullDate.Year);

            try
            {
                SwDbtoolsClient& aClient = SwNewDBMgr::GetDbtoolsClient();
                sRet = aClient.getValue(
                    xColumnProps,
                    rDBFormatData.xFormatter,
                    rDBFormatData.aLocale,
                    rDBFormatData.aNullDate);
                if (pNumber)
                {
                    double fVal = xColumn->getDouble();
                    if(!xColumn->wasNull())
                    {
                        *pNumber = fVal;
                    }
                }
            }
            catch(Exception& )
            {
                DBG_ERROR("exception caught");
            }

        }
        break;

//		case DataType::BINARY:
//		case DataType::VARBINARY:
//		case DataType::LONGVARBINARY:
//		case DataType::SQLNULL:
//		case DataType::OTHER:
//		case DataType::OBJECT:
//		case DataType::DISTINCT:
//		case DataType::STRUCT:
//		case DataType::ARRAY:
//		case DataType::BLOB:
//		case DataType::CLOB:
//		case DataType::REF:
//		default:
    }
//	if (pFormat)
//	{
//		SFX_ITEMSET_GET(*pCol, pFormatItem, SfxUInt32Item, SBA_DEF_FMTVALUE, sal_True);
//		*pFormat = pFormatItem->GetValue();
//	}

    return sRet;
}
/* -----------------------------06.07.00 14:28--------------------------------
    releases the merge data source table or query after merge is completed
 ---------------------------------------------------------------------------*/
void	SwNewDBMgr::EndMerge()
{
    DBG_ASSERT(bInMerge, "merge is not active");
    bInMerge = FALSE;
    delete pImpl->pMergeData;
    pImpl->pMergeData = 0;
}
/* -----------------------------06.07.00 14:28--------------------------------
    checks if a desired data source table or query is open
 ---------------------------------------------------------------------------*/
BOOL    SwNewDBMgr::IsDataSourceOpen(const String& rDataSource,
            const String& rTableOrQuery, sal_Bool bMergeOnly)
{
    if(pImpl->pMergeData)
    {
        return !bMergeLock &&
                ((rDataSource == (String)pImpl->pMergeData->sDataSource &&
                    rTableOrQuery == (String)pImpl->pMergeData->sCommand)
                    ||(!rDataSource.Len() && !rTableOrQuery.Len()))
                    &&
                    pImpl->pMergeData->xResultSet.is();
    }
    else if(!bMergeOnly)
    {
        SwDBData aData;
        aData.sDataSource = rDataSource;
        aData.sCommand = rTableOrQuery;
        aData.nCommandType = -1;
        SwDSParam* pFound = FindDSData(aData, FALSE);
        return (pFound && pFound->xResultSet.is());
    }
    return sal_False;
}
/* -----------------------------17.07.00 16:44--------------------------------
    read column data a a specified position
 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::GetColumnCnt(const String& rSourceName, const String& rTableName,
                            const String& rColumnName, sal_uInt32 nAbsRecordId,
                            long nLanguage,
                            String& rResult, double* pNumber)
{
    BOOL bRet = FALSE;
    SwDSParam* pFound = 0;
    //check if it's the merge data source
    if(pImpl->pMergeData &&
        rSourceName == (String)pImpl->pMergeData->sDataSource &&
        rTableName == (String)pImpl->pMergeData->sCommand)
    {
        pFound = pImpl->pMergeData;
    }
    else
    {
        SwDBData aData;
        aData.sDataSource = rSourceName;
        aData.sCommand = rTableName;
        aData.nCommandType = -1;
        pFound = FindDSData(aData, FALSE);
    }
    //check validity of supplied record Id
    if(pFound->aSelection.getLength())
    {
        //the destination has to be an element of the selection
        const Any* pSelection = pFound->aSelection.getConstArray();
        sal_Bool bFound = sal_False;
        for(sal_Int32 nPos = 0; !bFound && nPos < pFound->aSelection.getLength(); nPos++)
        {
            sal_Int32 nSelection = 0;
            pSelection[nPos] >>= nSelection;
            if(nSelection == static_cast<sal_Int32>(nAbsRecordId))
                bFound = sal_True;
        }
        if(!bFound)
            return FALSE;
    }
    if(pFound && pFound->xResultSet.is() && !pFound->bAfterSelection)
    {
        sal_Int32 nOldRow = 0;
        try
        {
            nOldRow = pFound->xResultSet->getRow();
        }
        catch(const Exception& )
        {
            return FALSE;
        }
        //position to the desired index
        BOOL bMove = TRUE;
        if ( nOldRow != static_cast<sal_Int32>(nAbsRecordId) )
            bMove = lcl_MoveAbsolute(pFound, nAbsRecordId);
        if(bMove)
        {
            bRet = lcl_GetColumnCnt(pFound, rColumnName, nLanguage, rResult, pNumber);
        }
        if ( nOldRow != static_cast<sal_Int32>(nAbsRecordId) )
            bMove = lcl_MoveAbsolute(pFound, nOldRow);
    }
    return bRet;
}
/* -----------------------------06.07.00 16:47--------------------------------
    reads the column data at the current position
 ---------------------------------------------------------------------------*/
BOOL	SwNewDBMgr::GetMergeColumnCnt(const String& rColumnName, USHORT nLanguage,
                                String &rResult, double *pNumber, sal_uInt32 * /*pFormat*/)
{
    if(!pImpl->pMergeData || !pImpl->pMergeData->xResultSet.is() || pImpl->pMergeData->bAfterSelection )
    {
        rResult.Erase();
        return FALSE;
    }

    BOOL bRet = lcl_GetColumnCnt(pImpl->pMergeData, rColumnName, nLanguage, rResult, pNumber);
    return bRet;
}
/* -----------------------------07.07.00 14:28--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::ToNextMergeRecord()
{
    DBG_ASSERT(pImpl->pMergeData && pImpl->pMergeData->xResultSet.is(), "no data source in merge");
    return ToNextRecord(pImpl->pMergeData);
}
/* -----------------------------10.07.01 14:28--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::ToNextRecord(
    const String& rDataSource, const String& rCommand, sal_Int32 /*nCommandType*/)
{
    SwDSParam* pFound = 0;
    if(pImpl->pMergeData &&
        rDataSource == (String)pImpl->pMergeData->sDataSource &&
        rCommand == (String)pImpl->pMergeData->sCommand)
        pFound = pImpl->pMergeData;
    else
    {
        SwDBData aData;
        aData.sDataSource = rDataSource;
        aData.sCommand = rCommand;
        aData.nCommandType = -1;
        pFound = FindDSData(aData, FALSE);
    }
    return ToNextRecord(pFound);
}
/* -----------------------------10.07.01 14:38--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::ToNextRecord(SwDSParam* pParam)
{
    BOOL bRet = TRUE;
    if(!pParam || !pParam->xResultSet.is() || pParam->bEndOfDB ||
            (pParam->aSelection.getLength() && pParam->aSelection.getLength() <= pParam->nSelectionIndex))
    {
        if(pParam)
            pParam->CheckEndOfDB();
        return FALSE;
    }
    try
    {
        if(pParam->aSelection.getLength())
        {
            sal_Int32 nPos = 0;
            pParam->aSelection.getConstArray()[ pParam->nSelectionIndex++ ] >>= nPos;
            pParam->bEndOfDB = !pParam->xResultSet->absolute( nPos );
            pParam->CheckEndOfDB();
            bRet = !pParam->bEndOfDB;
            if(pParam->nSelectionIndex >= pParam->aSelection.getLength())
                pParam->bEndOfDB = TRUE;
        }
        else
        {
            sal_Int32 nBefore = pParam->xResultSet->getRow();
            pParam->bEndOfDB = !pParam->xResultSet->next();
            if( !pParam->bEndOfDB && nBefore == pParam->xResultSet->getRow())
            {
                //next returned true but it didn't move
                pParam->bEndOfDB = sal_True;
            }

            pParam->CheckEndOfDB();
            bRet = !pParam->bEndOfDB;
            ++pParam->nSelectionIndex;
        }
    }
    catch(Exception&)
    {
    }
    return bRet;
}

/* -----------------------------13.07.00 17:23--------------------------------
    synchronized labels contain a next record field at their end
    to assure that the next page can be created in mail merge
    the cursor position must be validated
 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::ExistsNextRecord() const
{
    return pImpl->pMergeData && !pImpl->pMergeData->bEndOfDB;
}
/* -----------------------------13.07.00 10:41--------------------------------

 ---------------------------------------------------------------------------*/
sal_uInt32 	SwNewDBMgr::GetSelectedRecordId()
{
    sal_uInt32 	nRet = 0;
    DBG_ASSERT(pImpl->pMergeData && pImpl->pMergeData->xResultSet.is(), "no data source in merge");
    if(!pImpl->pMergeData || !pImpl->pMergeData->xResultSet.is())
        return FALSE;
    try
    {
        nRet = pImpl->pMergeData->xResultSet->getRow();
    }
    catch(Exception& )
    {
    }
    return nRet;
}
/* -----------------------------13.07.00 10:58--------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwNewDBMgr::ToRecordId(sal_Int32 nSet)
{
    DBG_ASSERT(pImpl->pMergeData && pImpl->pMergeData->xResultSet.is(), "no data source in merge");
    if(!pImpl->pMergeData || !pImpl->pMergeData->xResultSet.is()|| nSet < 0)
        return FALSE;
    sal_Bool bRet = FALSE;
    sal_Int32 nAbsPos = nSet;

    if(nAbsPos >= 0)
    {
        bRet = lcl_MoveAbsolute(pImpl->pMergeData, nAbsPos);
        pImpl->pMergeData->bEndOfDB = !bRet;
        pImpl->pMergeData->CheckEndOfDB();
    }
    return bRet;
}

/* -----------------------------17.07.00 14:17--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::OpenDataSource(const String& rDataSource, const String& rTableOrQuery,
            sal_Int32 nCommandType, bool bCreate)
{
    SwDBData aData;
    aData.sDataSource = rDataSource;
    aData.sCommand = rTableOrQuery;
    aData.nCommandType = nCommandType;

    SwDSParam* pFound = FindDSData(aData, TRUE);
    uno::Reference< XDataSource> xSource;
    if(pFound->xResultSet.is())
        return TRUE;
    SwDSParam* pParam = FindDSConnection(rDataSource, FALSE);
    uno::Reference< XConnection> xConnection;
    if(pParam && pParam->xConnection.is())
        pFound->xConnection = pParam->xConnection;
    else if(bCreate)
    {
        rtl::OUString sDataSource(rDataSource);
        pFound->xConnection = RegisterConnection( sDataSource );
    }
    if(pFound->xConnection.is())
    {
        try
        {
            uno::Reference< sdbc::XDatabaseMetaData >  xMetaData = pFound->xConnection->getMetaData();
            try
            {
                pFound->bScrollable = xMetaData
                        ->supportsResultSetType((sal_Int32)ResultSetType::SCROLL_INSENSITIVE);
            }
            catch(Exception&)
            {
                //#98373# DB driver may not be ODBC 3.0 compliant
                pFound->bScrollable = TRUE;
            }
            pFound->xStatement = pFound->xConnection->createStatement();
            rtl::OUString aQuoteChar = xMetaData->getIdentifierQuoteString();
            rtl::OUString sStatement(C2U("SELECT * FROM "));
            sStatement = C2U("SELECT * FROM ");
            sStatement += aQuoteChar;
            sStatement += rTableOrQuery;
            sStatement += aQuoteChar;
            pFound->xResultSet = pFound->xStatement->executeQuery( sStatement );

            //after executeQuery the cursor must be positioned
            pFound->bEndOfDB = !pFound->xResultSet->next();
            pFound->bAfterSelection = sal_False;
            pFound->CheckEndOfDB();
            ++pFound->nSelectionIndex;
        }
        catch (Exception&)
        {
            pFound->xResultSet = 0;
            pFound->xStatement = 0;
            pFound->xConnection = 0;
        }
    }
    return pFound->xResultSet.is();
}
/* -----------------------------14.08.2001 10:26------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference< XConnection> SwNewDBMgr::RegisterConnection(rtl::OUString& rDataSource)
{
    SwDSParam* pFound = SwNewDBMgr::FindDSConnection(rDataSource, TRUE);
    uno::Reference< XDataSource> xSource;
    if(!pFound->xConnection.is())
    {
        pFound->xConnection = SwNewDBMgr::GetConnection(rDataSource, xSource );
        try
        {
            uno::Reference<XComponent> xComponent(pFound->xConnection, UNO_QUERY);
            if(xComponent.is())
                xComponent->addEventListener(pImpl->xDisposeListener);
        }
        catch(Exception&)
        {
        }
    }
    return pFound->xConnection;
}
/* -----------------------------17.07.00 15:55--------------------------------

 ---------------------------------------------------------------------------*/
sal_uInt32      SwNewDBMgr::GetSelectedRecordId(
    const String& rDataSource, const String& rTableOrQuery, sal_Int32 nCommandType)
{
    sal_uInt32 nRet = 0xffffffff;
    //check for merge data source first
    if(pImpl->pMergeData && rDataSource == (String)pImpl->pMergeData->sDataSource &&
                    rTableOrQuery == (String)pImpl->pMergeData->sCommand &&
                    (nCommandType == -1 || nCommandType == pImpl->pMergeData->nCommandType) &&
                    pImpl->pMergeData->xResultSet.is())
        nRet = GetSelectedRecordId();
    else
    {
        SwDBData aData;
        aData.sDataSource = rDataSource;
        aData.sCommand = rTableOrQuery;
        aData.nCommandType = nCommandType;
        SwDSParam* pFound = FindDSData(aData, FALSE);
        if(pFound && pFound->xResultSet.is())
        {
            try
            {	//if a selection array is set the current row at the result set may not be set yet
                if(pFound->aSelection.getLength())
                {
                    sal_Int32 nSelIndex = pFound->nSelectionIndex;
                    if(nSelIndex >= pFound->aSelection.getLength())
                        nSelIndex = pFound->aSelection.getLength() -1;
                    pFound->aSelection.getConstArray()[nSelIndex] >>= nRet;

                }
                else
                    nRet = pFound->xResultSet->getRow();
            }
            catch(Exception&){}
        }
    }
    return nRet;
}

/* -----------------------------17.07.00 14:18--------------------------------
    close all data sources - after fields were updated
 ---------------------------------------------------------------------------*/
void	SwNewDBMgr::CloseAll(BOOL bIncludingMerge)
{
    //the only thing done here is to reset the selection index
    //all connections stay open
    for(USHORT nPos = 0; nPos < aDataSourceParams.Count(); nPos++)
    {
        SwDSParam* pParam = aDataSourceParams[nPos];
        if(bIncludingMerge || pParam != pImpl->pMergeData)
        {
            pParam->nSelectionIndex = 0;
            pParam->bAfterSelection = sal_False;
            pParam->bEndOfDB = sal_False;
            try
            {
                if(!bInMerge && pParam->xResultSet.is())
                    pParam->xResultSet->first();
            }
            catch(Exception& )
            {}
        }
    }
}
/* -----------------------------17.07.00 14:54--------------------------------

 ---------------------------------------------------------------------------*/
SwDSParam* SwNewDBMgr::FindDSData(const SwDBData& rData, BOOL bCreate)
{
    //prefer merge data if available
    if(pImpl->pMergeData && rData.sDataSource == pImpl->pMergeData->sDataSource &&
        rData.sCommand == pImpl->pMergeData->sCommand &&
        (rData.nCommandType == -1 || rData.nCommandType == pImpl->pMergeData->nCommandType ||
        (bCreate && pImpl->pMergeData->nCommandType == -1)))
    {
         return pImpl->pMergeData;
    }

    SwDSParam* pFound = 0;
    for(USHORT nPos = aDataSourceParams.Count(); nPos; nPos--)
    {
        SwDSParam* pParam = aDataSourceParams[nPos - 1];
        if(rData.sDataSource == pParam->sDataSource &&
            rData.sCommand == pParam->sCommand &&
            (rData.nCommandType == -1 || rData.nCommandType == pParam->nCommandType ||
            (bCreate && pParam->nCommandType == -1)))
            {
                //#94779# calls from the calculator may add a connection with an invalid commandtype
                //later added "real" data base connections have to re-use the already available
                //DSData and set the correct CommandType
                if(bCreate && pParam->nCommandType == -1)
                    pParam->nCommandType = rData.nCommandType;
                pFound = pParam;
                break;
            }
    }
    if(bCreate)
    {
        if(!pFound)
        {
            pFound = new SwDSParam(rData);
            aDataSourceParams.Insert(pFound, aDataSourceParams.Count());
            try
            {
                uno::Reference<XComponent> xComponent(pFound->xConnection, UNO_QUERY);
                if(xComponent.is())
                    xComponent->addEventListener(pImpl->xDisposeListener);
            }
            catch(Exception&)
            {
            }
        }
    }
    return pFound;
}
/* -----------------------------14.08.2001 10:27------------------------------

 ---------------------------------------------------------------------------*/

SwDSParam*  SwNewDBMgr::FindDSConnection(const rtl::OUString& rDataSource, BOOL bCreate)
{
    //prefer merge data if available
    if(pImpl->pMergeData && rDataSource == pImpl->pMergeData->sDataSource )
    {
         return pImpl->pMergeData;
    }
    SwDSParam* pFound = 0;
    for(USHORT nPos = 0; nPos < aDataSourceParams.Count(); nPos++)
    {
        SwDSParam* pParam = aDataSourceParams[nPos];
        if(rDataSource == pParam->sDataSource)
        {
            pFound = pParam;
            break;
        }
    }
    if(bCreate && !pFound)
    {
        SwDBData aData;
        aData.sDataSource = rDataSource;
        pFound = new SwDSParam(aData);
        aDataSourceParams.Insert(pFound, aDataSourceParams.Count());
        try
        {
            uno::Reference<XComponent> xComponent(pFound->xConnection, UNO_QUERY);
            if(xComponent.is())
                xComponent->addEventListener(pImpl->xDisposeListener);
        }
        catch(Exception&)
        {
        }
    }
    return pFound;
}

/* -----------------------------17.07.00 14:34--------------------------------

 ---------------------------------------------------------------------------*/
const SwDBData&	SwNewDBMgr::GetAddressDBName()
{
    return SW_MOD()->GetDBConfig()->GetAddressSource();
}
/* -----------------------------18.07.00 13:13--------------------------------

 ---------------------------------------------------------------------------*/
Sequence<rtl::OUString> SwNewDBMgr::GetExistingDatabaseNames()
{
    uno::Reference<XNameAccess> xDBContext;
    uno::Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
    if( xMgr.is() )
    {
        uno::Reference<XInterface> xInstance = xMgr->createInstance( C2U( "com.sun.star.sdb.DatabaseContext" ));
        xDBContext = uno::Reference<XNameAccess>(xInstance, UNO_QUERY) ;
    }
    if(xDBContext.is())
    {
        return xDBContext->getElementNames();
    }
    return Sequence<rtl::OUString>();
}
/*-- 26.05.2004 14:33:13---------------------------------------------------

  -----------------------------------------------------------------------*/
String SwNewDBMgr::LoadAndRegisterDataSource()
{
    sfx2::FileDialogHelper aDlgHelper( TemplateDescription::FILEOPEN_SIMPLE, 0 );
    Reference < XFilePicker > xFP = aDlgHelper.GetFilePicker();

    String sHomePath(SvtPathOptions().GetWorkPath());
    aDlgHelper.SetDisplayDirectory( sHomePath );

    Reference<XFilterManager> xFltMgr(xFP, UNO_QUERY);

    String sFilterAll(SW_RES(STR_FILTER_ALL));
    String sFilterAllData(SW_RES(STR_FILTER_ALL_DATA));
    String sFilterSXB(SW_RES(STR_FILTER_SXB));
    String sFilterSXC(SW_RES(STR_FILTER_SXC));
    String sFilterDBF(SW_RES(STR_FILTER_DBF));
    String sFilterXLS(SW_RES(STR_FILTER_XLS));
    String sFilterTXT(SW_RES(STR_FILTER_TXT));
    String sFilterCSV(SW_RES(STR_FILTER_CSV));
#ifdef WNT
    String sFilterMDB(SW_RES(STR_FILTER_MDB));
    String sFilterACCDB(SW_RES(STR_FILTER_ACCDB));
#endif
    xFltMgr->appendFilter( sFilterAll, C2U("*") );
    xFltMgr->appendFilter( sFilterAllData, C2U("*.ods;*.sxc;*.dbf;*.xls;*.txt;*.csv"));

    xFltMgr->appendFilter( sFilterSXB, C2U("*.odb") );
    xFltMgr->appendFilter( sFilterSXC, C2U("*.ods;*.sxc") );
    xFltMgr->appendFilter( sFilterDBF, C2U("*.dbf") );
    xFltMgr->appendFilter( sFilterXLS, C2U("*.xls") );
    xFltMgr->appendFilter( sFilterTXT, C2U("*.txt") );
    xFltMgr->appendFilter( sFilterCSV, C2U("*.csv") );
#ifdef WNT
    xFltMgr->appendFilter( sFilterMDB, C2U("*.mdb") );
    xFltMgr->appendFilter( sFilterACCDB, C2U("*.accdb") );
#endif

    xFltMgr->setCurrentFilter( sFilterAll ) ;
    String sFind;
    bool bTextConnection = false;
    if( ERRCODE_NONE == aDlgHelper.Execute() )
    {
        String sURL = xFP->getFiles().getConstArray()[0];
        //data sources have to be registered depending on their extensions
        INetURLObject aURL( sURL );
        String sExt( aURL.GetExtension() );
        Any aURLAny;
        Any aTableFilterAny;
        Any aSuppressVersionsAny;
        Any aInfoAny;
        INetURLObject aTempURL(aURL);
        bool bStore = true;
        if(sExt.EqualsAscii("odb"))
        {
            bStore = false;
        }
        else if(sExt.EqualsIgnoreCaseAscii("sxc")
            || sExt.EqualsIgnoreCaseAscii("ods")
                || sExt.EqualsIgnoreCaseAscii("xls"))
        {
            rtl::OUString sDBURL(C2U("sdbc:calc:"));
            sDBURL += aTempURL.GetMainURL(INetURLObject::NO_DECODE);
            aURLAny <<= sDBURL;
        }
        else if(sExt.EqualsIgnoreCaseAscii("dbf"))
        {
            aTempURL.removeSegment();
            aTempURL.removeFinalSlash();
            rtl::OUString sDBURL(C2U("sdbc:dbase:"));
            sDBURL += aTempURL.GetMainURL(INetURLObject::NO_DECODE);
            aURLAny <<= sDBURL;
            //set the filter to the file name without extension
            Sequence<rtl::OUString> aFilters(1);
            rtl::OUString sTmp(aURL.getBase());
            aFilters[0] = aURL.getBase();
            aTableFilterAny <<= aFilters;
        }
        else if(sExt.EqualsIgnoreCaseAscii("csv") || sExt.EqualsIgnoreCaseAscii("txt"))
        {
            aTempURL.removeSegment();
            aTempURL.removeFinalSlash();
            rtl::OUString sDBURL(C2U("sdbc:flat:"));
            //only the 'path' has to be added
            sDBURL += aTempURL.GetMainURL(INetURLObject::NO_DECODE);
            aURLAny <<= sDBURL;

            bTextConnection = true;
            //set the filter to the file name without extension
            Sequence<rtl::OUString> aFilters(1);
            rtl::OUString sTmp(aURL.getBase());
            aFilters[0] = aURL.getBase();
            aTableFilterAny <<= aFilters;
        }
#ifdef WNT
        else if(sExt.EqualsIgnoreCaseAscii("mdb"))
        {
            rtl::OUString sDBURL(C2U("sdbc:ado:access:PROVIDER=Microsoft.Jet.OLEDB.4.0;DATA SOURCE="));
            sDBURL += aTempURL.PathToFileName();
            aURLAny <<= sDBURL;
            aSuppressVersionsAny <<= makeAny(true);
        }
        else if(sExt.EqualsIgnoreCaseAscii("accdb"))
        {
            rtl::OUString sDBURL(C2U("sdbc:ado:PROVIDER=Microsoft.ACE.OLEDB.12.0;DATA SOURCE="));
            sDBURL += aTempURL.PathToFileName();
            aURLAny <<= sDBURL;
            aSuppressVersionsAny <<= makeAny(true);
        }
#endif
        try
        {
            Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
            Reference<XInterface> xInstance = xMgr->createInstance( C2U( "com.sun.star.sdb.DatabaseContext" ));
            Reference<XNameAccess> xDBContext(xInstance, UNO_QUERY_THROW);
            Reference<XSingleServiceFactory> xFact( xDBContext, UNO_QUERY);

            String sNewName = INetURLObject::decode( aURL.getName(),
                                                     INET_HEX_ESCAPE,
                                                     INetURLObject::DECODE_UNAMBIGUOUS,
                                                     RTL_TEXTENCODING_UTF8 );
            xub_StrLen nExtLen = static_cast< xub_StrLen >(aURL.GetExtension().getLength());
            sNewName.Erase( sNewName.Len() - nExtLen - 1, nExtLen + 1 );

            //find a unique name if sNewName already exists
            sFind = sNewName;
            sal_Int32 nIndex = 0;
            while(xDBContext->hasByName(sFind))
            {
                sFind = sNewName;
                sFind += String::CreateFromInt32(++nIndex);
            }

            Reference<XInterface> xNewInstance;
            if(!bStore)
            {
                //odb-file
                Any aDataSource = xDBContext->getByName(aTempURL.GetMainURL(INetURLObject::NO_DECODE));
                aDataSource >>= xNewInstance;
            }
            else
            {
                xNewInstance = xFact->createInstance();
                Reference<XPropertySet> xDataProperties(xNewInstance, UNO_QUERY);

                if(aURLAny.hasValue())
                    xDataProperties->setPropertyValue(C2U("URL"), aURLAny);
                if(aTableFilterAny.hasValue())
                    xDataProperties->setPropertyValue(C2U("TableFilter"), aTableFilterAny);
                if(aSuppressVersionsAny.hasValue())
                    xDataProperties->setPropertyValue(C2U("SuppressVersionColumns"), aSuppressVersionsAny);
                if(aInfoAny.hasValue())
                    xDataProperties->setPropertyValue(C2U("Info"), aInfoAny);

                if( bTextConnection )
                {
                    uno::Reference < ui::dialogs::XExecutableDialog > xSettingsDlg(
                                xMgr->createInstance( C2U( "com.sun.star.sdb.TextConnectionSettings" ) ), uno::UNO_QUERY);
                    if( xSettingsDlg->execute() )
                    {
                        uno::Any aSettings = xDataProperties->getPropertyValue( C2U( "Settings" ) );
                        uno::Reference < beans::XPropertySet > xDSSettings;
                        aSettings >>= xDSSettings;
                        ::comphelper::copyProperties(
                            uno::Reference < beans::XPropertySet >( xSettingsDlg, uno::UNO_QUERY ),
                            xDSSettings );
                        xDSSettings->setPropertyValue( C2U("Extension"), uno::makeAny( ::rtl::OUString( sExt )));
                    }
                }

                Reference<XDocumentDataSource> xDS(xNewInstance, UNO_QUERY_THROW);
                Reference<XStorable> xStore(xDS->getDatabaseDocument(), UNO_QUERY_THROW);
                String sOutputExt = String::CreateFromAscii(".odb");
                String sTmpName;
                {
                    utl::TempFile aTempFile(sNewName , &sOutputExt, &sHomePath);
                    aTempFile.EnableKillingFile(sal_True);
                    sTmpName = aTempFile.GetURL();
                }
                xStore->storeAsURL(sTmpName, Sequence< PropertyValue >());
            }
            Reference<XNamingService> xNaming(xDBContext, UNO_QUERY);
            xNaming->registerObject( sFind, xNewInstance );

        }
        catch(Exception&)
        {
        }
    }
    return sFind;

}
/* -----------------------------10.11.00 17:10--------------------------------

 ---------------------------------------------------------------------------*/
void SwNewDBMgr::ExecuteFormLetter(	SwWrtShell& rSh,
                        const Sequence<PropertyValue>& rProperties,
                        BOOL bWithDataSourceBrowser)
{
    //prevent second call
    if(pImpl->pMergeDialog)
        return ;
    rtl::OUString sDataSource, sDataTableOrQuery;
    Sequence<Any> aSelection;

    sal_Int16 nCmdType = CommandType::TABLE;
    uno::Reference< XConnection> xConnection;

    ODataAccessDescriptor aDescriptor(rProperties);
    sDataSource = aDescriptor.getDataSource();
    aDescriptor[daCommand]		>>= sDataTableOrQuery;
    aDescriptor[daCommandType]	>>= nCmdType;

    if ( aDescriptor.has(daSelection) )
        aDescriptor[daSelection] >>= aSelection;
    if ( aDescriptor.has(daConnection) )
        aDescriptor[daConnection] >>= xConnection;

    if(!sDataSource.getLength() || !sDataTableOrQuery.getLength())
    {
        DBG_ERROR("PropertyValues missing or unset");
        return;
    }

    //always create a connection for the dialog and dispose it after the dialog has been closed
    SwDSParam* pFound = 0;
    if(!xConnection.is())
    {
        xConnection = SwNewDBMgr::RegisterConnection(sDataSource);
        pFound = FindDSConnection(sDataSource, TRUE);
    }
    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
    DBG_ASSERT(pFact, "Dialogdiet fail!");
    pImpl->pMergeDialog = pFact->CreateMailMergeDlg( DLG_MAILMERGE,
                                                        &rSh.GetView().GetViewFrame()->GetWindow(), rSh,
                                                        sDataSource,
                                                        sDataTableOrQuery,
                                                        nCmdType,
                                                        xConnection,
                                                        bWithDataSourceBrowser ? 0 : &aSelection);
    DBG_ASSERT(pImpl->pMergeDialog, "Dialogdiet fail!");
    if(pImpl->pMergeDialog->Execute() == RET_OK)
    {
        aDescriptor[daSelection] <<= pImpl->pMergeDialog->GetSelection();

        uno::Reference<XResultSet> xResSet = pImpl->pMergeDialog->GetResultSet();
        if(xResSet.is())
            aDescriptor[daCursor] <<= xResSet;

        SfxObjectShellRef xDocShell = rSh.GetView().GetViewFrame()->GetObjectShell();
        SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_MAIL_MERGE, SwDocShell::GetEventName(STR_SW_EVENT_MAIL_MERGE), xDocShell));
        {
            //copy rSh to aTempFile
            ::rtl::OUString sTempURL;
            const SfxFilter *pSfxFlt = SwIoSystem::GetFilterOfFormat(
                        String::CreateFromAscii( FILTER_XML ),
                        SwDocShell::Factory().GetFilterContainer() );
            try
            {

                uno::Sequence< beans::PropertyValue > aValues(1);
                beans::PropertyValue* pValues = aValues.getArray();
                pValues[0].Name = C2U("FilterName");
                pValues[0].Value <<= ::rtl::OUString(pSfxFlt->GetFilterName());
                uno::Reference< frame::XStorable > xStore( xDocShell->GetModel(), uno::UNO_QUERY);
                sTempURL = URIHelper::SmartRel2Abs( INetURLObject(), utl::TempFile::CreateTempName() );
                xStore->storeToURL( sTempURL, aValues );
            }
            catch( const uno::Exception& rEx )
            {
                (void) rEx;
            }
            if( xDocShell->GetError() )
            {
                // error message ??
                ErrorHandler::HandleError( xDocShell->GetError() );
            }
            else
            {
                SfxObjectShellRef xWorkDocSh( new SwDocShell( SFX_CREATE_MODE_INTERNAL ));
                SfxMedium* pWorkMed = new SfxMedium( sTempURL, STREAM_STD_READ, TRUE );
                pWorkMed->SetFilter( pSfxFlt );
                if( xWorkDocSh->DoLoad(pWorkMed) )
                {
                    SfxViewFrame *pFrame = SfxViewFrame::CreateViewFrame( *xWorkDocSh, 0, TRUE );
                    SwView *pView = (SwView*) pFrame->GetViewShell();
                    pView->AttrChangedNotify( &pView->GetWrtShell() );//Damit SelectShell gerufen wird.
                    //set the current DBMgr
                    SwDoc* pWorkDoc = pView->GetWrtShell().GetDoc();
                    SwNewDBMgr* pWorkDBMgr = pWorkDoc->GetNewDBMgr();
                    pWorkDoc->SetNewDBMgr( this );

                    SwMergeDescriptor aMergeDesc( pImpl->pMergeDialog->GetMergeType(), pView->GetWrtShell(), aDescriptor );
                    aMergeDesc.sSaveToFilter = pImpl->pMergeDialog->GetSaveFilter(); 
                    aMergeDesc.bCreateSingleFile= true;
                    MergeNew(aMergeDesc);

                    pWorkDoc->SetNewDBMgr( pWorkDBMgr );
                    //close the temporary file
                    uno::Reference< util::XCloseable > xClose( xWorkDocSh->GetModel(), uno::UNO_QUERY );
                    if (xClose.is())
                    {
                        try
                        {
                            //! 'sal_True' -> transfer ownership to vetoing object if vetoed!
                            //! I.e. now that object is responsible for closing the model and doc shell.
                            xClose->close( sal_True );
                        }
                        catch ( const uno::Exception& )
                        {
                        }
                    }
                }
            }
            //remove the temporary file
            SWUnoHelper::UCB_DeleteFile( sTempURL );
        }
        SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_MAIL_MERGE_END, SwDocShell::GetEventName(STR_SW_EVENT_MAIL_MERGE_END), rSh.GetView().GetViewFrame()->GetObjectShell()));

        // reset the cursor inside
        xResSet = NULL;
        aDescriptor[daCursor] <<= xResSet;
    }
    if(pFound)
    {
        for(USHORT nPos = 0; nPos < aDataSourceParams.Count(); nPos++)
        {
            SwDSParam* pParam = aDataSourceParams[nPos];
            if(pParam == pFound)
            {
                try
                {
                    uno::Reference<XComponent> xComp(pParam->xConnection, UNO_QUERY);
                    if(xComp.is())
                        xComp->dispose();
                }
                catch(const RuntimeException& )
                {
                    //may be disposed already since multiple entries may have used the same connection
                }
                break;
            }
            //pFound doesn't need to be removed/deleted -
            //this has been done by the SwConnectionDisposedListener_Impl already
        }
    }
    DELETEZ(pImpl->pMergeDialog);
}
/* -----------------------------13.11.00 08:20--------------------------------

 ---------------------------------------------------------------------------*/
void SwNewDBMgr::InsertText(SwWrtShell& rSh,
                        const Sequence< PropertyValue>& rProperties)
{
    rtl::OUString sDataSource, sDataTableOrQuery;
    uno::Reference<XResultSet>  xResSet;
    Sequence<Any> aSelection;
    BOOL bHasSelectionProperty = FALSE;
    sal_Int32 nSelectionPos = 0;
    sal_Int16 nCmdType = CommandType::TABLE;
    const PropertyValue* pValues = rProperties.getConstArray();
    uno::Reference< XConnection> xConnection;
    for(sal_Int32 nPos = 0; nPos < rProperties.getLength(); nPos++)
    {
        if(pValues[nPos].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(cDataSourceName)))
            pValues[nPos].Value >>= sDataSource;
        else if(pValues[nPos].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(cCommand)))
            pValues[nPos].Value >>= sDataTableOrQuery;
        else if(pValues[nPos].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(cCursor)))
            pValues[nPos].Value >>= xResSet;
        else if(pValues[nPos].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(cSelection)))
        {
            bHasSelectionProperty = TRUE;
            nSelectionPos = nPos;
            pValues[nPos].Value >>= aSelection;
        }
        else if(pValues[nPos].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(cCommandType)))
            pValues[nPos].Value >>= nCmdType;
        else if(pValues[nPos].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(cActiveConnection)))
            pValues[nPos].Value >>= xConnection;
    }
    if(!sDataSource.getLength() || !sDataTableOrQuery.getLength() || !xResSet.is())
    {
        DBG_ERROR("PropertyValues missing or unset");
        return;
    }
    uno::Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
    uno::Reference<XDataSource> xSource;
    uno::Reference<XChild> xChild(xConnection, UNO_QUERY);
    if(xChild.is())
        xSource = uno::Reference<XDataSource>(xChild->getParent(), UNO_QUERY);
    if(!xSource.is())
        xSource = SwNewDBMgr::GetDbtoolsClient().getDataSource(sDataSource, xMgr);
    uno::Reference< XColumnsSupplier > xColSupp( xResSet, UNO_QUERY );
    SwDBData aDBData;
    aDBData.sDataSource = sDataSource;
    aDBData.sCommand = sDataTableOrQuery;
    aDBData.nCommandType = nCmdType;

    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
    DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

    AbstractSwInsertDBColAutoPilot* pDlg = pFact->CreateSwInsertDBColAutoPilot( rSh.GetView(),
                                                                                xSource,
                                                                                xColSupp,
                                                                                aDBData,
                                                                                DLG_AP_INSERT_DB_SEL );
    DBG_ASSERT(pDlg, "Dialogdiet fail!");
    if( RET_OK == pDlg->Execute() )
    {
        rtl::OUString sDummy;
        if(!xConnection.is())
            xConnection = xSource->getConnection(sDummy, sDummy);
        try
        {
            pDlg->DataToDoc( aSelection , xSource, xConnection, xResSet);
        }
        catch(Exception& )
        {
            DBG_ERROR("exception caught");
        }
    }
    delete pDlg;

}
/* -----------------------------30.08.2001 12:00------------------------------

 ---------------------------------------------------------------------------*/
SwDbtoolsClient* SwNewDBMgr::pDbtoolsClient = NULL;

SwDbtoolsClient& SwNewDBMgr::GetDbtoolsClient()
{
    if ( !pDbtoolsClient )
        pDbtoolsClient = new SwDbtoolsClient;
    return *pDbtoolsClient;
}
/* -----------------13.05.2003 15:34-----------------

 --------------------------------------------------*/
void SwNewDBMgr::RemoveDbtoolsClient()
{
    delete pDbtoolsClient;
    pDbtoolsClient = 0;
}
/* -----------------------------20.08.2002 12:00------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference<XDataSource> SwNewDBMgr::getDataSourceAsParent(const uno::Reference< XConnection>& _xConnection,const ::rtl::OUString& _sDataSourceName)
{
    uno::Reference<XDataSource> xSource;
    try
    {
        uno::Reference<XChild> xChild(_xConnection, UNO_QUERY);
        if ( xChild.is() )
            xSource = uno::Reference<XDataSource>(xChild->getParent(), UNO_QUERY);
        if ( !xSource.is() )
            xSource = SwNewDBMgr::GetDbtoolsClient().getDataSource(_sDataSourceName, ::comphelper::getProcessServiceFactory());
    }
    catch(const Exception&)
    {
        DBG_ERROR("exception in getDataSourceAsParent caught");
    }
    return xSource;
}
/* -----------------------------20.08.2002 12:00------------------------------

 ---------------------------------------------------------------------------*/
uno::Reference<XResultSet> SwNewDBMgr::createCursor(const ::rtl::OUString& _sDataSourceName,
                                       const ::rtl::OUString& _sCommand,
                                       sal_Int32 _nCommandType,
                                       const uno::Reference<XConnection>& _xConnection
                                      )
{
    uno::Reference<XResultSet> xResultSet;
    try
    {
        uno::Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
        if( xMgr.is() )
        {
            uno::Reference<XInterface> xInstance = xMgr->createInstance(
                C2U( "com.sun.star.sdb.RowSet" ));
            uno::Reference<XPropertySet> xRowSetPropSet(xInstance, UNO_QUERY);
            if(xRowSetPropSet.is())
            {
                xRowSetPropSet->setPropertyValue(C2U("DataSourceName"), makeAny(_sDataSourceName));
                xRowSetPropSet->setPropertyValue(C2U("ActiveConnection"), makeAny(_xConnection));
                xRowSetPropSet->setPropertyValue(C2U("Command"), makeAny(_sCommand));
                xRowSetPropSet->setPropertyValue(C2U("CommandType"), makeAny(_nCommandType));

                uno::Reference< XCompletedExecution > xRowSet(xInstance, UNO_QUERY);

                if ( xRowSet.is() )
                {
                    uno::Reference< XInteractionHandler > xHandler(xMgr->createInstance(C2U("com.sun.star.sdb.InteractionHandler")), UNO_QUERY);
                    xRowSet->executeWithCompletion(xHandler);
                }
                xResultSet = uno::Reference<XResultSet>(xRowSet, UNO_QUERY);
            }
        }
    }
    catch(const Exception&)
    {
        DBG_ASSERT(0,"Caught exception while creating a new RowSet!");
    }
    return xResultSet;
}
/*-- 13.05.2004 16:14:15---------------------------------------------------
    merge all data into one resulting document and return the number of
    merged documents
  -----------------------------------------------------------------------*/

sal_Int32 SwNewDBMgr::MergeDocuments( SwMailMergeConfigItem& rMMConfig,
                            SwView& rSourceView )
{
    // check the availability of all data in the config item
    uno::Reference< XResultSet> xResultSet = rMMConfig.GetResultSet();
    if(!xResultSet.is())
        return false;
    bInMerge = TRUE;
    sal_Int32 nRet  = 0;
    pImpl->pMergeData = new SwDSParam(
                rMMConfig.GetCurrentDBData(), xResultSet, rMMConfig.GetSelection());

    try{
        //set to start position
        if(pImpl->pMergeData->aSelection.getLength())
        {
            sal_Int32 nPos = 0;
            pImpl->pMergeData->aSelection.getConstArray()[ pImpl->pMergeData->nSelectionIndex++ ] >>= nPos;
            pImpl->pMergeData->bEndOfDB = !pImpl->pMergeData->xResultSet->absolute( nPos );
            pImpl->pMergeData->CheckEndOfDB();
            if(pImpl->pMergeData->nSelectionIndex >= pImpl->pMergeData->aSelection.getLength())
                pImpl->pMergeData->bEndOfDB = TRUE;
        }
        else
        {
            pImpl->pMergeData->bEndOfDB = !pImpl->pMergeData->xResultSet->first();
            pImpl->pMergeData->CheckEndOfDB();
        }
    }
    catch(Exception&)
    {
        pImpl->pMergeData->bEndOfDB = TRUE;
        pImpl->pMergeData->CheckEndOfDB();
        DBG_ERROR("exception in MergeNew()");
    }

    //bCancel is set from the PrintMonitor
    bCancel = FALSE;

    CreateMonitor aMonitorDlg(&rSourceView.GetEditWin());
    aMonitorDlg.SetCancelHdl(LINK(this, SwNewDBMgr, PrtCancelHdl));
    if (!IsMergeSilent())
    {
        aMonitorDlg.Show();
        aMonitorDlg.Invalidate();
        aMonitorDlg.Update();
        // the print monitor needs some time to act
        for( USHORT i = 0; i < 25; i++)
            Application::Reschedule();
    }

    SwWrtShell& rSourceShell = rSourceView.GetWrtShell();
    BOOL bSynchronizedDoc = rSourceShell.IsLabelDoc() && rSourceShell.GetSectionFmtCount() > 1;
    //save the settings of the first
    rSourceShell.SttEndDoc(TRUE);
    USHORT nStartingPageNo = rSourceShell.GetVirtPageNum();
    String sModifiedStartingPageDesc;
    String sStartingPageDesc = sModifiedStartingPageDesc = rSourceShell.GetPageDesc(
                                rSourceShell.GetCurPageDesc()).GetName();

    try
    {
        // create a target docshell to put the merged document into
        SfxObjectShellRef xTargetDocShell( new SwDocShell( SFX_CREATE_MODE_STANDARD ) );
        xTargetDocShell->DoInitNew( 0 );
        SfxViewFrame* pTargetFrame = SfxViewFrame::CreateViewFrame( *xTargetDocShell, 0, TRUE );

        //the created window has to be located at the same position as the source window
        Window& rTargetWindow = pTargetFrame->GetFrame()->GetWindow();
        Window& rSourceWindow = rSourceView.GetViewFrame()->GetFrame()->GetWindow();
        rTargetWindow.SetPosPixel(rSourceWindow.GetPosPixel());

//        pTargetFrame->GetFrame()->Appear();
        SwView* pTargetView = static_cast<SwView*>( pTargetFrame->GetViewShell() );
        rMMConfig.SetTargetView(pTargetView);
        //initiate SelectShell() to create sub shells
        pTargetView->AttrChangedNotify( &pTargetView->GetWrtShell() );
        SwWrtShell* pTargetShell = pTargetView->GetWrtShellPtr();
        // #i63806#
        const SwPageDesc* pSourcePageDesc = rSourceShell.FindPageDescByName( sStartingPageDesc );
        const SwFrmFmt& rMaster = pSourcePageDesc->GetMaster();
        bool bPageStylesWithHeaderFooter = rMaster.GetHeader().IsActive()  ||
                                        rMaster.GetFooter().IsActive();


        // #122799# copy compatibility options
        lcl_CopyCompatibilityOptions( rSourceShell, *pTargetShell);
        // #72821# copy dynamic defaults
        lcl_CopyDynamicDefaults( *rSourceShell.GetDoc(), *pTargetShell->GetDoc() );


        long nStartRow, nEndRow;
        ULONG nDocNo = 1;
        sal_Int32 nDocCount = 0;
        if( !IsMergeSilent() && lcl_getCountFromResultSet( nDocCount, pImpl->pMergeData->xResultSet ) )
            aMonitorDlg.SetTotalCount( nDocCount );

        do
        {
            nStartRow = pImpl->pMergeData->xResultSet->getRow();
            if (!IsMergeSilent())
            {
                aMonitorDlg.SetCurrentPosition( nDocNo );
                aMonitorDlg.Invalidate();
                aMonitorDlg.Update();
                // the print monitor needs some time to act
                for( USHORT i = 0; i < 25; i++)
                    Application::Reschedule();
            }

            // copy the source document
            SfxObjectShellRef xWorkDocSh;
            if(nDocNo == 1 )
            {
                uno::Reference< util::XCloneable > xClone( rSourceView.GetDocShell()->GetModel(), uno::UNO_QUERY);
                uno::Reference< lang::XUnoTunnel > xWorkDocShell( xClone->createClone(), uno::UNO_QUERY);
                SwXTextDocument* pWorkModel = reinterpret_cast<SwXTextDocument*>(xWorkDocShell->getSomething(SwXTextDocument::getUnoTunnelId()));
                xWorkDocSh = pWorkModel->GetDocShell();
            }
            else
            {
                SwDoc* pNewDoc = rSourceView.GetDocShell()->GetDoc()->CreateCopy();
                xWorkDocSh = new SwDocShell( pNewDoc, SFX_CREATE_MODE_STANDARD );
                xWorkDocSh->DoInitNew();
            }
            //create a ViewFrame
            SwView* pWorkView = static_cast< SwView* >( SfxViewFrame::CreateViewFrame( *xWorkDocSh, 0, sal_True )->GetViewShell() );
            SwWrtShell& rWorkShell = pWorkView->GetWrtShell();
            pWorkView->AttrChangedNotify( &rWorkShell );//Damit SelectShell gerufen wird.

                // merge the data
                SwDoc* pWorkDoc = rWorkShell.GetDoc();
                SwNewDBMgr* pWorkDBMgr = pWorkDoc->GetNewDBMgr();
                pWorkDoc->SetNewDBMgr( this );
                pWorkDoc->EmbedAllLinks();
                if(UNDO_UI_DELETE_INVISIBLECNTNT == rWorkShell.GetUndoIds())
                    rWorkShell.Undo();
                // #i69485# lock fields to prevent access to the result set while calculating layout
                rWorkShell.LockExpFlds();
                // create a layout
                rWorkShell.CalcLayout();
                rWorkShell.UnlockExpFlds();
                SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE, SwDocShell::GetEventName(STR_SW_EVENT_FIELD_MERGE), rWorkShell.GetView().GetViewFrame()->GetObjectShell()));
            rWorkShell.ViewShell::UpdateFlds();
                SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE_FINISHED, SwDocShell::GetEventName(STR_SW_EVENT_FIELD_MERGE_FINISHED), rWorkShell.GetView().GetViewFrame()->GetObjectShell()));

            // strip invisible content and convert fields to text
            rWorkShell.RemoveInvisibleContent();
            rWorkShell.ConvertFieldsToText();
            rWorkShell.SetNumberingRestart();


            // insert the document into the target document
            rWorkShell.SttEndDoc(FALSE);
            rWorkShell.SttEndDoc(TRUE);
            rWorkShell.SelAll();
            pTargetShell->SttEndDoc(FALSE);

            //#i63806# put the styles to the target document
            //if the source uses headers or footers each new copy need to copy a new page styles
            if(bPageStylesWithHeaderFooter)
            {
                //create a new pagestyle
                //copy the pagedesc from the current document to the new document and change the name of the to-be-applied style

                SwDoc* pTargetDoc = pTargetShell->GetDoc();
                String sNewPageDescName = lcl_FindUniqueName(pTargetShell, sStartingPageDesc, nDocNo );
                pTargetShell->GetDoc()->MakePageDesc( sNewPageDescName );
                SwPageDesc* pTargetPageDesc = pTargetShell->FindPageDescByName( sNewPageDescName );
                const SwPageDesc* pWorkPageDesc = rWorkShell.FindPageDescByName( sStartingPageDesc );

                if(pWorkPageDesc && pTargetPageDesc)
                {
                    pTargetDoc->CopyPageDesc( *pWorkPageDesc, *pTargetPageDesc, sal_False );
                    sModifiedStartingPageDesc = sNewPageDescName;
                    lcl_CopyFollowPageDesc( *pTargetShell, *pWorkPageDesc, *pTargetPageDesc, nDocNo );
                }
            }
            if(nDocNo == 1 || bPageStylesWithHeaderFooter)
            {
                pTargetView->GetDocShell()->_LoadStyles( *rSourceView.GetDocShell(), sal_True );
            }
            if(nDocNo > 1)
            {
                pTargetShell->InsertPageBreak( &sModifiedStartingPageDesc, nStartingPageNo );
            }
            else
            {
                pTargetShell->SetPageStyle(sModifiedStartingPageDesc);
            }
            USHORT nPageCountBefore = pTargetShell->GetPageCnt();
            DBG_ASSERT(!pTargetShell->GetTableFmt(),"target document ends with a table - paragraph should be appended");
            //#i51359# add a second paragraph in case there's only one
            {
                SwNodeIndex aIdx( pWorkDoc->GetNodes().GetEndOfExtras(), 2 );
              SwPosition aTestPos( aIdx );
              SwCursor aTestCrsr(aTestPos,0,false);
                if(!aTestCrsr.MovePara(fnParaNext, fnParaStart))
                {
                    //append a paragraph
                    pWorkDoc->AppendTxtNode( aTestPos );
                }
            }
            pTargetShell->Paste( rWorkShell.GetDoc(), sal_True );
            //convert fields in page styles (header/footer - has to be done after the first document has been pasted
            if(1 == nDocNo)
            {
                pTargetShell->CalcLayout();
                pTargetShell->ConvertFieldsToText();
            }
            //add the document info to the config item
            SwDocMergeInfo aMergeInfo;
            aMergeInfo.nStartPageInTarget = nPageCountBefore;
            //#i72820# calculate layout to be able to find the correct page index
            pTargetShell->CalcLayout();
            aMergeInfo.nEndPageInTarget = pTargetShell->GetPageCnt();
            aMergeInfo.nDBRow = nStartRow;
            rMMConfig.AddMergedDocument( aMergeInfo );
            ++nRet;

            // the print monitor needs some time to act
            for( USHORT i = 0; i < 25; i++)
                Application::Reschedule();

            //restore the ole DBMgr
            pWorkDoc->SetNewDBMgr( pWorkDBMgr );
            //now the temporary document should be closed
            SfxObjectShellRef xDocSh(pWorkView->GetDocShell());
            xDocSh->DoClose();
            nEndRow = pImpl->pMergeData->xResultSet->getRow();
            ++nDocNo;
        } while( !bCancel &&
                (bSynchronizedDoc && (nStartRow != nEndRow)? ExistsNextRecord() : ToNextMergeRecord()));

        //deselect all, go out of the frame and go to the beginning of the document
        Point aPt(LONG_MIN, LONG_MIN);
        pTargetShell->SelectObj(aPt, SW_LEAVE_FRAME);
        if (pTargetShell->IsSelFrmMode())
        {
            pTargetShell->UnSelectFrm();
            pTargetShell->LeaveSelFrmMode();
        }
        pTargetShell->EnterStdMode();
        pTargetShell->SttDoc();
        //
    }
    catch( Exception& )
    {
        DBG_ERROR("exception caught in SwNewDBMgr::MergeDocuments");
    }
    DELETEZ(pImpl->pMergeData);
    bInMerge = FALSE;
    return nRet;
}
/* -----------------09.12.2002 12:38-----------------
 *
 * --------------------------------------------------*/
SwConnectionDisposedListener_Impl::SwConnectionDisposedListener_Impl(SwNewDBMgr& rMgr) :
    rDBMgr(rMgr)
{};
/* -----------------09.12.2002 12:39-----------------
 *
 * --------------------------------------------------*/
SwConnectionDisposedListener_Impl::~SwConnectionDisposedListener_Impl()
{};
/* -----------------09.12.2002 12:39-----------------
 *
 * --------------------------------------------------*/
void SwConnectionDisposedListener_Impl::disposing( const EventObject& rSource )
        throw (RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Reference<XConnection> xSource(rSource.Source, UNO_QUERY);
    for(USHORT nPos = rDBMgr.aDataSourceParams.Count(); nPos; nPos--)
    {
        SwDSParam* pParam = rDBMgr.aDataSourceParams[nPos - 1];
        if(pParam->xConnection.is() &&
                (xSource == pParam->xConnection))
        {
            rDBMgr.aDataSourceParams.DeleteAndDestroy(nPos - 1);
        }
    }
}
