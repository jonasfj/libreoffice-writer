/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dbmgr.cxx,v $
 *
 *  $Revision: 1.108 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-13 15:55:52 $
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

#if STLPORT_VERSION>=321
#include <cstdarg>
#endif

#include <stdio.h>

#include <com/sun/star/text/NotePrintMode.hpp>
#include <sfx2/app.hxx>
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XDOCUMENTDATASOURCE_HPP_
#include <com/sun/star/sdb/XDocumentDataSource.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#ifndef _COM_SUN_STAR_UTIL_iXNUMBERFORMATTER_HPP_
#include <com/sun/star/util/XNumberFormatter.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XCOMPLETEDCONNECTION_HPP_
#include <com/sun/star/sdb/XCompletedConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XCOMPLETEDEXECUTION_HPP_
#include <com/sun/star/sdb/XCompletedExecution.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_MAILMERGEEVENT_
#include <com/sun/star/text/MailMergeEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XNAMINGSERVICE_HPP_
#include <com/sun/star/uno/XNamingService.hpp>
#endif
#ifndef _SFX_FCONTNR_HXX
#include <sfx2/fcontnr.hxx>
#endif
#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef _DBCONFIG_HXX
#include <dbconfig.hxx>
#endif
#ifndef _SWDBTOOLSCLIENT_HXX
#include <swdbtoolsclient.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _UNOTOOLS_TEMPFILE_HXX
#include <unotools/tempfile.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif
#ifndef _ZFORLIST_HXX //autogen
#include <svtools/zforlist.hxx>
#endif
#ifndef _ZFORMAT_HXX //autogen
#include <svtools/zformat.hxx>
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
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFX_PROGRESS_HXX //autogen
#include <sfx2/progress.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX //autogen
#include <sfx2/docfilt.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _MAILENUM_HXX //autogen
#include <goodies/mailenum.hxx>
#endif
#include <cmdid.h>
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _INITUI_HXX
#include <initui.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif
#ifndef _FLDDAT_HXX
#include <flddat.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _MODCFG_HXX
#include <modcfg.hxx>
#endif
#ifndef _SWPRTOPT_HXX
#include <swprtopt.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _DBUI_HXX
#include <dbui.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif
#ifndef _SWUNOHELPER_HXX
#include <swunohelper.hxx>
#endif
#ifndef _FMTHDFT_HXX
#include <fmthdft.hxx>
#endif

#ifndef _DBUI_HRC
#include <dbui.hrc>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif
#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif
#ifndef _MMCONFIGITEM_HXX
#include <mmconfigitem.hxx>
#endif

#ifndef _SFXREQUEST_HXX
#include <sfx2/request.hxx>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
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
#ifndef _COM_SUN_STAR_SDBC_XROWSET_HPP_
#include <com/sun/star/sdbc/XRowSet.hpp>
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
#ifndef _COM_SUN_STAR_SDB_XCOLUMN_HPP_
#include <com/sun/star/sdb/XColumn.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XSTATEMENT_HPP_
#include <com/sun/star/sdbc/XStatement.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_RESULTSETTYPE_HPP_
#include <com/sun/star/sdbc/ResultSetType.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_MAIL_MAILATTACHMENT_HPP_
#include <com/sun/star/mail/MailAttachment.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _MAILMERGEHELPER_HXX
#include <mailmergehelper.hxx>
#endif
#ifndef INCLUDED_MAILDISPATCHER_HXX
#include <maildispatcher.hxx>
#endif
#ifndef _SVX_HTMLCFG_HXX
#include <svx/htmlcfg.hxx>
#endif
#ifndef INCLUDED_I18NPOOL_MSLANGID_HXX
#include <i18npool/mslangid.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATTYPES_HPP_
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#endif
#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif
#ifndef _SVX_UNOMID_HXX
#include <svx/unomid.hxx>
#endif
#ifndef _NUMUNO_HXX
#include <svtools/numuno.hxx>
#endif

//CHINA001 #include "mailmrge.hxx"
#include <unomailmerge.hxx>

#ifndef _SFXEVENT_HXX
#include <sfx2/event.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVX_DATACCESSDESCRIPTOR_HXX_
#include <svx/dataaccessdescriptor.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _RTL_TEXTENC_H
#include <rtl/textenc.h>
#endif
#ifndef _NDINDEX_HXX
#include <ndindex.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _SWCRSR_HXX
#include <swcrsr.hxx>
#endif
#ifndef _SWEVENT_HXX
#include <swevent.hxx>
#endif
#include <osl/file.hxx>
#include <swabstdlg.hxx> //CHINA001
#include <dbui.hrc> //CHINA001
#include <envelp.hrc> //CHINA001
#include <memory>
#include <vector>

using namespace ::osl;
using namespace ::svx;
using namespace ::com::sun::star;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::sdb;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;
using namespace com::sun::star::task;
using namespace com::sun::star::ui::dialogs;

#define C2S(cChar) String::CreateFromAscii(cChar)
#define C2U(cChar) ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(cChar))

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
            sal_Bool bFinal;
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
    rTargetShell.SetParaSpaceMax( rSourceShell.IsParaSpaceMax(), rSourceShell.IsParaSpaceMaxAtPages() );
    rTargetShell.SetTabCompat( rSourceShell.IsTabCompat() );
    rTargetShell.SetAddExtLeading( rSourceShell.IsAddExtLeading() );
    rTargetShell.SetUseVirtualDevice( rSourceShell.IsUseVirtualDevice() );
    rTargetShell.SetAddParaSpacingToTableCells( rSourceShell.IsAddParaSpacingToTableCells() );
    rTargetShell.SetUseFormerLineSpacing( rSourceShell.IsFormerLineSpacing() );
    rTargetShell.SetUseFormerObjectPositioning( rSourceShell.IsFormerObjectPositioning() );
    rTargetShell.SetConsiderWrapOnObjPos( rSourceShell.ConsiderWrapOnObjPos() );
    rTargetShell.SetUseFormerTextWrapping( rSourceShell.IsFormerTextWrapping() );
}
}
/* -----------------09.12.2002 12:35-----------------
 *
 * --------------------------------------------------*/

class SwConnectionDisposedListener_Impl : public cppu::WeakImplHelper1
< com::sun::star::lang::XEventListener >
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
            DBG_ERROR("no absolute positioning available")
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
    catch( lang::DisposedException& rEx)
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

    DBG_ASSERT(!bInMerge && !pImpl->pMergeData, "merge already activated!")

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
        DBG_ERROR("exception in MergeNew()")
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
        SwDBData aData = rMergeDesc.rSh.GetDBData();
        String sDBName = aData.sDataSource;
        sDBName += DB_DELIM;
        sDBName += (String)aData.sCommand;
        sDBName += DB_DELIM;
        sDBName += String::CreateFromInt32(aData.nCommandType);
        rMergeDesc.rSh.ChangeDBFields( aDBNames, sDBName);
        SetInitDBFields(FALSE);
    }

    BOOL bRet = TRUE;
    switch(rMergeDesc.nMergeType)
    {
        case DBMGR_MERGE:
            bRet = Merge(&rMergeDesc.rSh);   // Mischen
            break;

        case DBMGR_MERGE_MAILMERGE: // Serienbrief
        {
            SwView& rView = rMergeDesc.rSh.GetView();
            SfxDispatcher *pDis = rView.GetViewFrame()->GetDispatcher();
            SfxItemSet aPrintArgs( rView.GetPool(),
                    SID_SILENT, SID_SILENT, //5528
                    SID_ASYNCHRON, SID_ASYNCHRON, //5811
                    SID_PRINT_FIRST_PAGE, SID_PRINT_FIRST_PAGE,  //   5001
                    SID_PRINT_LAST_PAGE,  SID_PRINT_LAST_PAGE,   //   5002
                    SID_PRINT_COPIES,     SID_PRINT_COPIES,      //   5003
                    SID_PRINTER_NAME,     SID_PRINTER_NAME,     //5322
                    SID_SELECTION,        SID_SELECTION,        //5346
                    SID_FILE_NAME,        SID_FILE_NAME,         // 5507
                    SID_PRINT_PAGES,      SID_PRINT_PAGES,      //6589
                    SID_PRINT_COLLATE,    SID_PRINT_COLLATE,    //6590
                    FN_QRY_MERGE, FN_QRY_MERGE,
                    0 );
            aPrintArgs.Put(SfxBoolItem(FN_QRY_MERGE, TRUE) );

            // !! Currently (Jan-2003) silent is defined by supplying *any*
            // !! item!!  (Thus according to OS it would be silent even when
            // !! other items then SID_SILENT would be supplied!)
            // !! Therefore it has to be the 0 pointer when not silent.
            if(IsMergeSilent())
            {
                aPrintArgs.Put( SfxBoolItem(SID_SILENT, TRUE) );
                // #i25686# printing should be done asynchronously to prevent dangling offices
                // when mail merge is called as command line macro
                // #i52629# aynchronous printing should only be done in silent mode - otherwise 
                // the printer dialog does not come up
                aPrintArgs.Put( SfxBoolItem( SID_ASYNCHRON, rMergeDesc.bPrintAsync ));
            }
            // convert PropertyValues
            const beans::PropertyValue* pPrintOptions = rMergeDesc.aPrintOptions.getConstArray();
            for( sal_Int32 nOption = 0; nOption < rMergeDesc.aPrintOptions.getLength(); ++nOption)
            {
                if( pPrintOptions[nOption].Name.equalsAscii( "CopyCount" ))
                {
                    sal_Int16 nCopies;
                    if((pPrintOptions[nOption].Value >>= nCopies) && nCopies > 0)
                        aPrintArgs.Put( SfxInt16Item( SID_PRINT_COPIES, nCopies ));
                }
                else if( pPrintOptions[nOption].Name.equalsAscii( "FileName" ))
                {
                    ::rtl::OUString sFileName;
                    if( (pPrintOptions[nOption].Value >>= sFileName) && sFileName.getLength() > 0)
                        aPrintArgs.Put( SfxStringItem( SID_FILE_NAME, sFileName ));
                }
                else if( pPrintOptions[nOption].Name.equalsAscii( "Collate" ))
                {
                    sal_Bool bCollate;
                    if( pPrintOptions[nOption].Value >>= bCollate )
                        aPrintArgs.Put( SfxBoolItem( SID_PRINT_COLLATE, bCollate ));
                }
                else if( pPrintOptions[nOption].Name.equalsAscii( "Pages" ))
                {
                    ::rtl::OUString sPages;
                    if( (pPrintOptions[nOption].Value >>= sPages) && sPages.getLength() )
                        aPrintArgs.Put( SfxStringItem( SID_PRINT_PAGES, sPages ));
                }
                else if( pPrintOptions[nOption].Name.equalsAscii( "Wait" ))
                {
                    sal_Bool bWait;
                    if( pPrintOptions[nOption].Value >>= bWait )
                        aPrintArgs.Put( SfxBoolItem( SID_ASYNCHRON, !bWait ));
                }

            }
            pDis->Execute( SID_PRINTDOC,
                                 SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD,
                                 aPrintArgs );
        }
        break;

        case DBMGR_MERGE_MAILING:
        case DBMGR_MERGE_MAILFILES:
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
            pSh->StartUndo(0);
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
            pSh->EndUndo(0);
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

inline String lcl_GetDBInsertMode( const SwDBData& rData )
{
    return aEmptyStr;
#if 0
//JP 13.11.00: must be change to the new configuration
    return  SFX_APP()->GetIniManager()->Get( String::CreateFromAscii(
                RTL_CONSTASCII_STRINGPARAM( "DataBaseFormatInfo" )),
                FALSE, FALSE, sDBName );
#endif
}


void SwNewDBMgr::ImportDBEntry(SwWrtShell* pSh)
{
    if(pImpl->pMergeData && !pImpl->pMergeData->bEndOfDB)
    {
        uno::Reference< XColumnsSupplier > xColsSupp( pImpl->pMergeData->xResultSet, UNO_QUERY );
        uno::Reference<XNameAccess> xCols = xColsSupp->getColumns();
        String sFormatStr( lcl_GetDBInsertMode( *pImpl->pMergeData ));
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
            pSh->SwEditShell::Insert(sStr);
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
    SwDSParam* pParam = FindDSConnection(rDBName, FALSE);
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
            pImpl(new SwNewDBMgr_Impl(*this)),
            pMergeEvtSrc(NULL),
            bInMerge(FALSE),
            bMergeLock(FALSE),
            bMergeSilent(FALSE),
            nMergeType(DBMGR_INSERT),
            bInitDBFields(FALSE)
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
    Beschreibung:	Serienbrief drucken
 --------------------------------------------------------------------*/


BOOL SwNewDBMgr::MergePrint( SwView& rView,
                             SwPrtOptions& rOpt, SfxProgress& rProgress )
{
    SwWrtShell* pSh = &rView.GetWrtShell();
    //check if the doc is synchronized and contains at least one linked section
    BOOL bSynchronizedDoc = pSh->IsLabelDoc() && pSh->GetSectionFmtCount() > 1;
    //merge source is already open
    rOpt.nMergeCnt = 0;
    //#i56195# no field update while printing mail merge documents
    rOpt.bUpdateFieldsInPrinting = sal_False;
    if(pImpl->pMergeData)
    {
        if(pImpl->pMergeData->aSelection.getLength())
            rOpt.nMergeCnt = pImpl->pMergeData->aSelection.getLength();
        else if(pImpl->pMergeData->xResultSet.is())
        {
            sal_Int32 nCount;
            if( lcl_getCountFromResultSet( nCount, pImpl->pMergeData->xResultSet ) )
                rOpt.nMergeCnt = (ULONG)nCount;
        }
    }

    SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();
    pModOpt->SetSinglePrintJob(rOpt.IsPrintSingleJobs());

    SfxPrinter *pPrt = pSh->GetPrt();
    Link aSfxSaveLnk = pPrt->GetEndPrintHdl();
    if( rOpt.IsPrintSingleJobs()  )
        pPrt->SetEndPrintHdl( Link() );

    BOOL bNewJob = FALSE,
         bUserBreak = FALSE,
         bRet = FALSE;
    long nStartRow, nEndRow;
    //calculate number of data sets to be printed

    Sequence<PropertyValue> aViewProperties(16);
    PropertyValue* pViewProperties =  aViewProperties.getArray();
    pViewProperties[0].Name = C2U("MailMergeCount");
    pViewProperties[0].Value <<= (sal_Int32)rOpt.nMergeCnt;
    pViewProperties[1].Name = C2U("PrintGraphics");
    pViewProperties[1].Value <<= (sal_Bool)rOpt.IsPrintGraphic();
    pViewProperties[2].Name = C2U("PrintTables");
    pViewProperties[2].Value <<= (sal_Bool)rOpt.IsPrintTable();
    pViewProperties[3].Name = C2U("PrintDrawings");
    pViewProperties[3].Value <<= (sal_Bool)rOpt.IsPrintDraw();
    pViewProperties[4].Name = C2U("PrintLeftPages");
    pViewProperties[4].Value <<= (sal_Bool)rOpt.IsPrintLeftPage();
    pViewProperties[5].Name = C2U("PrintRightPages");
    pViewProperties[5].Value <<= (sal_Bool)rOpt.IsPrintRightPage();
    pViewProperties[6].Name = C2U("PrintControls");
    pViewProperties[6].Value <<= (sal_Bool)rOpt.IsPrintControl();
    pViewProperties[7].Name = C2U("PrintReversed");
    pViewProperties[7].Value <<= (sal_Bool)rOpt.IsPrintReverse();
    pViewProperties[8].Name = C2U("PrintPaperFromSetup");
    pViewProperties[8].Value <<= (sal_Bool)rOpt.IsPaperFromSetup();
    pViewProperties[9].Name = C2U("PrintFaxName");
    pViewProperties[9].Value <<= rOpt.GetFaxName();
    pViewProperties[10].Name = C2U("PrintAnnotationMode");
    pViewProperties[10].Value <<= (com::sun::star::text::NotePrintMode) rOpt.GetPrintPostIts();
    pViewProperties[11].Name = C2U("PrintProspect");
    pViewProperties[11].Value <<= (sal_Bool)rOpt.IsPrintProspect();
    pViewProperties[12].Name = C2U("PrintPageBackground");
    pViewProperties[12].Value <<= (sal_Bool)rOpt.IsPrintPageBackground();
    pViewProperties[13].Name = C2U("PrintBlackFonts");
    pViewProperties[13].Value <<= (sal_Bool)rOpt.IsPrintBlackFont();
    pViewProperties[14].Name = C2U("IsSinglePrintJob");
    pViewProperties[14].Value <<= (sal_Bool)rOpt.IsPrintSingleJobs();
    pViewProperties[15].Name = C2U("PrintEmptyPages");
    pViewProperties[15].Value <<= (sal_Bool)rOpt.IsPrintEmptyPages();

    rView.SetAdditionalPrintOptions(aViewProperties);
    do {
        nStartRow = pImpl->pMergeData ? pImpl->pMergeData->xResultSet->getRow() : 0;
        {
            SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE, pSh->GetView().GetViewFrame()->GetObjectShell()));
            pSh->ViewShell::UpdateFlds();
            SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE_FINISHED, pSh->GetView().GetViewFrame()->GetObjectShell()));
            ++rOpt.nMergeAct;

            // launch MailMergeEvent if required
            const SwXMailMerge *pEvtSrc = GetMailMergeEvtSrc();
            if (pEvtSrc)
            {
                uno::Reference< XInterface > xRef( (XMailMergeBroadcaster *) pEvtSrc );
                text::MailMergeEvent aEvt( xRef, rView.GetDocShell()->GetModel() );
                pEvtSrc->LaunchMailMergeEvent( aEvt );
            }

            rView.SfxViewShell::Print( rProgress ); // ggf Basic-Macro ausfuehren
            if( rOpt.IsPrintSingleJobs() && bRet )
            {
                //rOpt.bJobStartet = FALSE;
                bRet = FALSE;
            }

            bMergeLock = TRUE;
            if(rOpt.IsPrintProspect())
            {
                if( pPrt->IsJobActive() || pPrt->StartJob( rOpt.GetJobName() ))
                {
                    pSh->PrintProspect( rOpt, rProgress );
                    bRet = TRUE;
                }
            }
            else if( pSh->Prt( rOpt, &rProgress ) )
                bRet = TRUE;
            bMergeLock = FALSE;

            if( !pPrt->IsJobActive() )
            {
                bUserBreak = TRUE;
                bRet = FALSE;
                break;
            }
            if( !rOpt.IsPrintSingleJobs() )
            {
                String& rJNm = (String&)rOpt.GetJobName();
                rJNm.Erase();
            }
        }
        nEndRow = pImpl->pMergeData ? pImpl->pMergeData->xResultSet->getRow() : 0;
    } while( bSynchronizedDoc && (nStartRow != nEndRow)? ExistsNextRecord() : ToNextMergeRecord());

    if( rOpt.IsPrintSingleJobs() )
    {
        pSh->GetPrt()->SetEndPrintHdl( aSfxSaveLnk );
        if ( !bUserBreak && !pSh->GetPrt()->IsJobActive() )		//Schon zu spaet?
            aSfxSaveLnk.Call( pSh->GetPrt() );
    }

    rOpt.nMergeCnt = 0;
    rOpt.nMergeAct = 0;

    nMergeType = DBMGR_INSERT;

    SwDocShell* pDocSh = rView.GetDocShell();
    SfxViewFrame *pTmpFrm = SfxViewFrame::GetFirst(pDocSh);

    while (pTmpFrm)		// Alle Views Invalidieren
    {
        SwView *pVw = PTR_CAST(SwView, pTmpFrm->GetViewShell());
        if (pVw)
            pVw->GetEditWin().Invalidate();
        pTmpFrm = pTmpFrm->GetNext(*pTmpFrm, pDocSh);
    }

    return bRet;
}
/*-- 21.06.2004 09:08:16---------------------------------------------------

  -----------------------------------------------------------------------*/
BOOL SwNewDBMgr::MergePrintDocuments( SwView& rView,
                                SwPrtOptions& rOpt, SfxProgress& rProgress )
{
    SwWrtShell* pSh = &rView.GetWrtShell();
    //check if the doc is synchronized and contains at least one linked section
    //merge source is already open
    rOpt.nMergeCnt = 0;
    rOpt.SetPrintSingleJobs( sal_True );

    SfxPrinter *pPrt = pSh->GetPrt();
    Link aSfxSaveLnk = pPrt->GetEndPrintHdl();
    if( rOpt.IsPrintSingleJobs()  )
        pPrt->SetEndPrintHdl( Link() );

    BOOL bNewJob = FALSE,
         bUserBreak = FALSE,
         bRet = FALSE;
    //calculate number of data sets to be printed

    Sequence<PropertyValue> aViewProperties(16);
    PropertyValue* pViewProperties =  aViewProperties.getArray();
    pViewProperties[0].Name = C2U("MailMergeCount");
    pViewProperties[0].Value <<= (sal_Int32)rOpt.nMergeCnt;
    pViewProperties[1].Name = C2U("PrintGraphics");
    pViewProperties[1].Value <<= (sal_Bool)rOpt.IsPrintGraphic();
    pViewProperties[2].Name = C2U("PrintTables");
    pViewProperties[2].Value <<= (sal_Bool)rOpt.IsPrintTable();
    pViewProperties[3].Name = C2U("PrintDrawings");
    pViewProperties[3].Value <<= (sal_Bool)rOpt.IsPrintDraw();
    pViewProperties[4].Name = C2U("PrintLeftPages");
    pViewProperties[4].Value <<= (sal_Bool)rOpt.IsPrintLeftPage();
    pViewProperties[5].Name = C2U("PrintRightPages");
    pViewProperties[5].Value <<= (sal_Bool)rOpt.IsPrintRightPage();
    pViewProperties[6].Name = C2U("PrintControls");
    pViewProperties[6].Value <<= (sal_Bool)rOpt.IsPrintControl();
    pViewProperties[7].Name = C2U("PrintReversed");
    pViewProperties[7].Value <<= (sal_Bool)rOpt.IsPrintReverse();
    pViewProperties[8].Name = C2U("PrintPaperFromSetup");
    pViewProperties[8].Value <<= (sal_Bool)rOpt.IsPaperFromSetup();
    pViewProperties[9].Name = C2U("PrintFaxName");
    pViewProperties[9].Value <<= rOpt.GetFaxName();
    pViewProperties[10].Name = C2U("PrintAnnotationMode");
    pViewProperties[10].Value <<= (com::sun::star::text::NotePrintMode) rOpt.GetPrintPostIts();
    pViewProperties[11].Name = C2U("PrintProspect");
    pViewProperties[11].Value <<= (sal_Bool)rOpt.IsPrintProspect();
    pViewProperties[12].Name = C2U("PrintPageBackground");
    pViewProperties[12].Value <<= (sal_Bool)rOpt.IsPrintPageBackground();
    pViewProperties[13].Name = C2U("PrintBlackFonts");
    pViewProperties[13].Value <<= (sal_Bool)rOpt.IsPrintBlackFont();
    pViewProperties[14].Name = C2U("IsSinglePrintJob");
    pViewProperties[14].Value <<= (sal_Bool)rOpt.IsPrintSingleJobs();
    pViewProperties[15].Name = C2U("PrintEmptyPages");
    pViewProperties[15].Value <<= (sal_Bool)rOpt.IsPrintEmptyPages();

    rView.SetAdditionalPrintOptions(aViewProperties);

    SwMailMergeConfigItem* pConfigItem = rView.GetMailMergeConfigItem();
    DBG_ASSERT(pConfigItem, "mail merge config item is missing")
    if(!pConfigItem)
        return sal_False;

    USHORT nDocStart = pConfigItem->GetPrintRangeStart();
    USHORT nDocEnd = pConfigItem->GetPrintRangeEnd();
    DBG_ASSERT(nDocStart < nDocEnd && nDocEnd <= pConfigItem->GetMergedDocumentCount(),
            "merge print settings are not correct")
    for( sal_uInt32 nPrintDocument = nDocStart; nPrintDocument < nDocEnd; ++nPrintDocument)
    {
        SwDocMergeInfo& rDocInfo = pConfigItem->GetDocumentMergeInfo(nPrintDocument);
        rOpt.aMulti.SelectAll(FALSE);
        rOpt.aMulti.Select(Range( rDocInfo.nStartPageInTarget, rDocInfo.nEndPageInTarget ), TRUE );

        ++rOpt.nMergeAct;

        // launch MailMergeEvent if required
        const SwXMailMerge *pEvtSrc = GetMailMergeEvtSrc();
        if (pEvtSrc)
        {
            uno::Reference< XInterface > xRef( (XMailMergeBroadcaster *) pEvtSrc );
            text::MailMergeEvent aEvt( xRef, rView.GetDocShell()->GetModel() );
            pEvtSrc->LaunchMailMergeEvent( aEvt );
        }

        String aTmp;
        aTmp += String::CreateFromInt32( rDocInfo.nStartPageInTarget );
        aTmp += '-';
        aTmp += String::CreateFromInt32( rDocInfo.nEndPageInTarget );

        Sequence<PropertyValue> aViewProperties(1);
        PropertyValue* pViewProperties =  aViewProperties.getArray();
        pViewProperties[0].Name = C2U("Pages");
        pViewProperties[0].Value <<= ::rtl::OUString( aTmp );
        rView.SetAdditionalPrintOptions(aViewProperties);

        rView.SfxViewShell::Print( rProgress ); // ggf Basic-Macro ausfuehren
        if( rOpt.IsPrintSingleJobs() && bRet )
        {
            //rOpt.bJobStartet = FALSE;
            bRet = FALSE;
        }

        bMergeLock = TRUE;
        if(rOpt.IsPrintProspect())
        {
            if( pPrt->IsJobActive() || pPrt->StartJob( rOpt.GetJobName() ))
            {
                pSh->PrintProspect( rOpt, rProgress );
                bRet = TRUE;
            }
        }
        else if( pSh->Prt( rOpt, &rProgress ) )
            bRet = TRUE;
        bMergeLock = FALSE;

        if( !pPrt->IsJobActive() )
        {
            bUserBreak = TRUE;
            bRet = FALSE;
            break;
        }
        if( !rOpt.IsPrintSingleJobs() )
        {
            String& rJNm = (String&)rOpt.GetJobName();
            rJNm.Erase();
        }
    }

    if( rOpt.IsPrintSingleJobs() )
    {
        pSh->GetPrt()->SetEndPrintHdl( aSfxSaveLnk );
        if ( !bUserBreak && !pSh->GetPrt()->IsJobActive() )     //Schon zu spaet?
            aSfxSaveLnk.Call( pSh->GetPrt() );
    }

    rOpt.nMergeCnt = 0;
    rOpt.nMergeAct = 0;

    nMergeType = DBMGR_INSERT;

    SwDocShell* pDocSh = rView.GetDocShell();
    SfxViewFrame *pTmpFrm = SfxViewFrame::GetFirst(pDocSh);

    while (pTmpFrm)     // Alle Views Invalidieren
    {
        SwView *pVw = PTR_CAST(SwView, pTmpFrm->GetViewShell());
        if (pVw)
            pVw->GetEditWin().Invalidate();
        pTmpFrm = pTmpFrm->GetNext(*pTmpFrm, pDocSh);
    }

    return bRet;
}



/*--------------------------------------------------------------------
    Beschreibung:	Serienbriefe als einzelne Dokumente speichern
 --------------------------------------------------------------------*/

BOOL SwNewDBMgr::MergeMailFiles(SwWrtShell* pSourceShell,
        const SwMergeDescriptor& rMergeDescriptor)
{
    //check if the doc is synchronized and contains at least one linked section
    BOOL bSynchronizedDoc = pSourceShell->IsLabelDoc() && pSourceShell->GetSectionFmtCount() > 1;
    BOOL bLoop = TRUE;
    BOOL bEMail = rMergeDescriptor.nMergeType == DBMGR_MERGE_MAILING;

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
        USHORT nColPos = 0;
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
        SwDocShell* pDocSh = pSourceShell->GetView().GetDocShell();
        pSfxDispatcher->Execute( pDocSh->HasName() ? SID_SAVEDOC : SID_SAVEASDOC, SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD);
        // has document been saved successfully?
        if( !pDocSh->IsModified() )
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
            String sAddress;
            bCancel = FALSE;

            // in case of creating a single resulting file this has to be created here
            SwWrtShell* pTargetShell = 0;
            SfxObjectShellRef xTargetDocShell;
            std::auto_ptr< utl::TempFile > aTempFile;
            String sStartingPageDesc;
            USHORT nStartingPageNo = 0;
            if(rMergeDescriptor.bCreateSingleFile)
            {
                // create a target docshell to put the merged document into
                xTargetDocShell = new SwDocShell( SFX_CREATE_MODE_STANDARD );
                xTargetDocShell->DoInitNew( 0 );
                SfxViewFrame* pTargetFrame = SfxViewFrame::CreateViewFrame( *xTargetDocShell, 0, TRUE );

                SwView* pTargetView = static_cast<SwView*>( pTargetFrame->GetViewShell() );

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
                pSourceShell->SwCrsrShell::SttDoc();
                nStartingPageNo = pSourceShell->GetVirtPageNum();
                sStartingPageDesc = pSourceShell->GetPageDesc(
                                            pSourceShell->GetCurPageDesc()).GetName();
                // #122799# copy compatibility options
                lcl_CopyCompatibilityOptions( *pSourceShell, *pTargetShell);
            }

            PrintMonitor aPrtMonDlg(&pSourceShell->GetView().GetEditWin());
            aPrtMonDlg.aDocName.SetText(pSourceShell->GetView().GetDocShell()->GetTitle(22));

            aPrtMonDlg.aCancel.SetClickHdl(LINK(this, SwNewDBMgr, PrtCancelHdl));
            if (!IsMergeSilent())
                aPrtMonDlg.Show();

            SwDocShell *pDocSh = pSourceShell->GetView().GetDocShell();
            // Progress, um KeyInputs zu unterbinden
            SfxProgress aProgress(pDocSh, ::aEmptyStr, 1);

            // Alle Dispatcher sperren
            SfxViewFrame* pViewFrm = SfxViewFrame::GetFirst(pDocSh);
            while (pViewFrm)
            {
                pViewFrm->GetDispatcher()->Lock(TRUE);
                pViewFrm = SfxViewFrame::GetNext(*pViewFrm, pDocSh);
            }
            ULONG nDocNo = 1;
            ULONG nCounter = 0;

            long nStartRow, nEndRow;
            // collect temporary files
            ::std::vector< String> aFilesToRemove;
            do
            {
                nStartRow = pImpl->pMergeData ? pImpl->pMergeData->xResultSet->getRow() : 0;
                {
                    String sPath(sSubject);

                    if( !bEMail && bColumnName )
                    {
                        SwDBFormatData aDBFormat;
                        aDBFormat.xFormatter = pImpl->pMergeData->xFormatter;
                        aDBFormat.aNullDate = pImpl->pMergeData->aNullDate;
                        sAddress = GetDBField( xColumnProp,	aDBFormat);
                        if (!sAddress.Len())
                            sAddress = '_';
                        sPath += sAddress;
                        nCounter = 0;
                    }

                    // create a new temporary file name - only done once in case of bCreateSingleFile
                    if( 1 == nDocNo || !rMergeDescriptor.bCreateSingleFile )
                    {
                        INetURLObject aEntry(sPath);
                        String sLeading(aEntry.GetBase());
                        aEntry.removeSegment();
                        sPath = aEntry.GetMainURL( INetURLObject::NO_DECODE );
                        String sExt( pStoreToFilter->GetDefaultExtension() );
                        sExt.EraseLeadingChars('*');
                        aTempFile = std::auto_ptr< utl::TempFile >(
                                new utl::TempFile(sLeading,&sExt,&sPath ));
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
                            SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE, xWorkDocSh));
                            pWorkDoc->UpdateFlds(0);
                            SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE_FINISHED, xWorkDocSh));

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

                            if(rMergeDescriptor.bCreateSingleFile)
                            {
                                DBG_ASSERT( pTargetShell, "no target shell available!" )
                                // copy created file into the target document
                                rWorkShell.ConvertFieldsToText();
                                rWorkShell.SetNumberingRestart();

                                // insert the document into the target document
                                rWorkShell.SwCrsrShell::EndDoc();
                                rWorkShell.SwCrsrShell::SttDoc();
                                rWorkShell.SelAll();
                                pTargetShell->SwCrsrShell::EndDoc();
                                if(nDocNo > 1)
                                    pTargetShell->InsertPageBreak( &sStartingPageDesc, nStartingPageNo );
                                else
                                    pTargetShell->SetPageStyle(sStartingPageDesc);
                                DBG_ASSERT(!pTargetShell->GetTableFmt(),"target document ends with a table - paragraph should be appended")
                                //#i51359# add a second paragraph in case there's only one
                                {
                                    SwNodeIndex aIdx( pWorkDoc->GetNodes().GetEndOfExtras(), 2 );
                  SwPosition aTestPos( aIdx );
                  SwCursor aTestCrsr(aTestPos);
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
                                        DBG_ERROR("invalid e-Mail address in database column")
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
                                                DBG_ASSERT(pInStream, "no output file created?")
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
            if(rMergeDescriptor.bCreateSingleFile)
            {
                DBG_ASSERT( aTempFile.get(), "Temporary file not available" )
                INetURLObject aTempFileURL(aTempFile->GetURL());
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
                xTargetDocShell->DoClose();
            }

            //remove the temporary files
            ::std::vector<String>::iterator aFileIter;
            for(aFileIter = aFilesToRemove.begin();
                        aFileIter != aFilesToRemove.end(); aFileIter++)
                SWUnoHelper::UCB_DeleteFile( *aFileIter );

            // Alle Dispatcher freigeben
            pViewFrm = SfxViewFrame::GetFirst(pDocSh);
            while (pViewFrm)
            {
                pViewFrm->GetDispatcher()->Lock(FALSE);
                pViewFrm = SfxViewFrame::GetNext(*pViewFrm, pDocSh);
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
        if(pImpl->pMergeData &&
            pImpl->pMergeData->sDataSource.equals(rDBName) && pImpl->pMergeData->sCommand.equals(rTableName))
        {
            xConnection = pImpl->pMergeData->xConnection;
            uno::Reference<XDataSource> xSource = SwNewDBMgr::getDataSourceAsParent(xConnection,rDBName);
            bUseMergeData = sal_True;
            xColsSupp = xColsSupp.query( pImpl->pMergeData->xResultSet );
        }
        if(!xConnection.is())
        {
            SwDSParam* pParam = FindDSConnection(rDBName, FALSE);
            if(pParam && pParam->xConnection.is())
            {
                xConnection = pParam->xConnection;
                xColsSupp = xColsSupp.query( pParam->xResultSet );
            }
            else
            {
                rtl::OUString sDBName(rDBName);
                xConnection = RegisterConnection( sDBName );
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
                DBG_ERROR("Exception in getColumns()")
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
        Any aFormat = xColumn->getPropertyValue(C2U("FormatKey"));
        if(aFormat.hasValue())
        {
            sal_Int32 nFmt;
            aFormat >>= nFmt;
            if(xNumberFormats.is())
            {
                try
                {
                    uno::Reference<XPropertySet> xNumProps = xNumberFormats->getByKey( nFmt );
                    Any aFormat = xNumProps->getPropertyValue(C2U("FormatString"));
                    Any aLocale = xNumProps->getPropertyValue(C2U("Locale"));
                    rtl::OUString sFormat;
                    aFormat >>= sFormat;
                    com::sun::star::lang::Locale aLoc;
                    aLocale >>= aLoc;
                    nFmt = xDocNumberFormats->queryKey( sFormat, aLoc, sal_False );
                    if(NUMBERFORMAT_ENTRY_NOT_FOUND == nFmt)
                        nFmt = xDocNumberFormats->addNew( sFormat, aLoc );
                    nRet = nFmt;
                }
                catch(Exception&)
                {
                    DBG_ERROR("illegal number format key")
                }
            }
        }
        else
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
    SwDSParam* pParam = FindDSConnection(rDBName, FALSE);
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
        if(xCols->hasByName(rColNm))
        {
            Any aCol = xCols->getByName(rColNm);
            uno::Reference<XPropertySet> xCol;
            aCol >>= xCol;
            Any aType = xCol->getPropertyValue(C2S("Type"));
            aType >>= nRet;
        }
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
    catch( const uno::Exception& rEx )
    {
        rEx;
        DBG_ERROR("Exception in SwDBMgr::GetColumnSupplier")
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
    DBG_ASSERT(xColumn.is(), "SwNewDBMgr::::ImportDBField: illegal arguments")
    if(!xColumn.is())
        return sRet;

    Any aType = xColumnProps->getPropertyValue(C2U("Type"));
    sal_Int32 eDataType;
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
            catch( SQLException& rEx)
            {
                rEx;
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
                DBG_ERROR("exception caught")
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
    DBG_ASSERT(bInMerge, "merge is not active")
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
                rDataSource == (String)pImpl->pMergeData->sDataSource &&
                    rTableOrQuery == (String)pImpl->pMergeData->sCommand &&
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
            sal_Int32 nSelection;
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
                                String &rResult, double *pNumber, sal_uInt32 *pFormat)
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
    DBG_ASSERT(pImpl->pMergeData && pImpl->pMergeData->xResultSet.is(), "no data source in merge")
    return ToNextRecord(pImpl->pMergeData);
}
/* -----------------------------10.07.01 14:28--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwNewDBMgr::ToNextRecord(
    const String& rDataSource, const String& rCommand, sal_Int32 nCommandType)
{
    SwDSParam* pFound = 0;
    BOOL bRet = TRUE;
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
    DBG_ASSERT(pImpl->pMergeData && pImpl->pMergeData->xResultSet.is(), "no data source in merge")
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
    DBG_ASSERT(pImpl->pMergeData && pImpl->pMergeData->xResultSet.is(), "no data source in merge")
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
    sal_uInt32 nRet = -1;
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

/* -----------------------------17.07.00 14:31--------------------------------
    rDBName: <Source> + DB_DELIM + <Table>; + <Statement>
 ---------------------------------------------------------------------------*/
void    SwNewDBMgr::AddDSData(const SwDBData& rData, long nSelStart, long nSelEnd)
{
    SwDSParam* pFound = FindDSData(rData, TRUE);
    if(nSelStart > 0)
    {
        if(nSelEnd < nSelStart)
        {
            sal_uInt32 nZw = nSelEnd;
            nSelEnd = nSelStart;
            nSelStart = nZw;
        }

        pFound->aSelection.realloc(nSelEnd - nSelStart + 1);
        Any* pSelection = pFound->aSelection.getArray();
        for (long i = nSelStart; i <= nSelEnd; i++, ++pSelection)
            *pSelection <<= i;
    }
    else
        pFound->aSelection.realloc(0);
}
/* -----------------------------17.07.00 14:31--------------------------------

 ---------------------------------------------------------------------------*/
void    SwNewDBMgr::GetDSSelection(const SwDBData& rData, long& rSelStart, long& rSelEnd)
{
    SwDSParam* pFound = FindDSData(rData, FALSE);
    if(!pFound || !pFound->aSelection.getLength())
        rSelStart = -1L;
    else
    {
        pFound->aSelection.getConstArray()[0] >>= rSelStart;
        pFound->aSelection.getConstArray()[pFound->aSelection.getLength() - 1] >>= rSelEnd;
    }
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

    xFltMgr->appendFilter( sFilterAll, C2U("*") );
    xFltMgr->appendFilter( sFilterAllData, C2U("*.ods;*.sxc;*.dbf;*.xls;*.txt;*.csv"));

    xFltMgr->appendFilter( sFilterSXB, C2U("*.odb") );
    xFltMgr->appendFilter( sFilterSXC, C2U("*.ods;*.sxc") );
    xFltMgr->appendFilter( sFilterDBF, C2U("*.dbf") );
    xFltMgr->appendFilter( sFilterXLS, C2U("*.xls") );
    xFltMgr->appendFilter( sFilterTXT, C2U("*.txt") );
    xFltMgr->appendFilter( sFilterCSV, C2U("*.csv") );

    xFltMgr->setCurrentFilter( sFilterAll ) ;
    String sFind;
    if( ERRCODE_NONE == aDlgHelper.Execute() )
    {
        String sURL = xFP->getFiles().getConstArray()[0];
        //data sources have to be registered depending on their extensions
        INetURLObject aURL( sURL );
        String sExt( aURL.GetExtension() );
        Any aURLAny;
        Any aTableFilterAny;
        Any aInfoAny;
        INetURLObject aTempURL(aURL);
        bool bStore = true;
        if(sExt.EqualsAscii("odb"))
        {
            bStore = false;
        }
        else if(sExt.EqualsAscii("sxc")
            || sExt.EqualsAscii("ods")
                || sExt.EqualsAscii("xls"))
        {
            rtl::OUString sDBURL(C2U("sdbc:calc:"));
            sDBURL += aTempURL.GetMainURL(INetURLObject::NO_DECODE);
            aURLAny <<= sDBURL;
        }
        else if(sExt.EqualsAscii("dbf"))
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
        else if(sExt.EqualsAscii("csv") || sExt.EqualsAscii("txt"))
        {
            aTempURL.removeSegment();
            aTempURL.removeFinalSlash();
            rtl::OUString sDBURL(C2U("sdbc:flat:"));
            //only the 'path' has to be added
            sDBURL += aTempURL.GetMainURL(INetURLObject::NO_DECODE);
            aURLAny <<= sDBURL;

            //set the filter to the file name without extension
            Sequence<rtl::OUString> aFilters(1);
            rtl::OUString sTmp(aURL.getBase());
            aFilters[0] = aURL.getBase();
            aTableFilterAny <<= aFilters;

            Sequence<PropertyValue> aInfo(5);
            PropertyValue* pInfo = aInfo.getArray();
            pInfo[0].Name = C2U("FieldDelimiter");
            pInfo[0].Value <<= rtl::OUString(String(','));
            pInfo[1].Name = C2U("ThousandDelimiter");
            pInfo[1].Value <<= rtl::OUString(String('.'));
            pInfo[2].Name = C2U("StringDelimiter");
            pInfo[2].Value <<= rtl::OUString('"');
            pInfo[3].Name = C2U("Extension");
            pInfo[3].Value <<= rtl::OUString(sExt);
            pInfo[4].Name = C2U("CharSet");
            pInfo[4].Value <<= sal_Int32(gsl_getSystemTextEncoding());
            aInfoAny <<= aInfo;
        }
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
            xub_StrLen nExtLen = aURL.GetExtension().getLength();
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
                if(aInfoAny.hasValue())
                    xDataProperties->setPropertyValue(C2U("Info"), aInfoAny);

                Reference<XDocumentDataSource> xDS(xNewInstance, UNO_QUERY_THROW);
                Reference<XStorable> xStore(xDS->getDatabaseDocument(), UNO_QUERY_THROW);
                String sExt = String::CreateFromAscii(".odb");
                String sTmpName;
                {
                    utl::TempFile aTempFile(sNewName , &sExt, &sHomePath);
                    aTempFile.EnableKillingFile(sal_True);
                    sTmpName = aTempFile.GetURL();
                }
                xStore->storeAsURL(sTmpName, Sequence< PropertyValue >());
            }
            Reference<XNamingService> xNaming(xDBContext, UNO_QUERY);
            xNaming->registerObject( sFind, xNewInstance );

        }
        catch(Exception& rEx)
        {
            rEx; // make compiler happy
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

    sal_Int32 nSelectionPos = 0;
    sal_Int32 nResultSetIdx = -1;
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
        DBG_ERROR("PropertyValues missing or unset")
        return;
    }

//CHINA001     pImpl->pMergeDialog = new SwMailMergeDlg(
//CHINA001	&rSh.GetView().GetViewFrame()->GetWindow(), rSh,
//CHINA001	sDataSource,
//CHINA001	sDataTableOrQuery,
//CHINA001	nCmdType,
//CHINA001	xConnection,
//CHINA001	bWithDataSourceBrowser ? 0 : &aSelection );
    //always create a connection for the dialog and dispose it after the dialog has been closed
    SwDSParam* pFound = 0;
    if(!xConnection.is())
    {
        xConnection = SwNewDBMgr::RegisterConnection(sDataSource);
        pFound = FindDSConnection(sDataSource, TRUE);
    }
    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
    DBG_ASSERT(pFact, "Dialogdiet fail!");//CHINA001
    pImpl->pMergeDialog = pFact->CreateMailMergeDlg( ResId(DLG_MAILMERGE),
                                                        &rSh.GetView().GetViewFrame()->GetWindow(), rSh,
                                                        sDataSource,
                                                        sDataTableOrQuery,
                                                        nCmdType,
                                                        xConnection,
                                                        bWithDataSourceBrowser ? 0 : &aSelection);
    DBG_ASSERT(pImpl->pMergeDialog, "Dialogdiet fail!");//CHINA001
    if(pImpl->pMergeDialog->Execute() == RET_OK)
    {
        aDescriptor[daSelection] <<= pImpl->pMergeDialog->GetSelection();

        uno::Reference<XResultSet> xResSet = pImpl->pMergeDialog->GetResultSet();
        if(xResSet.is())
            aDescriptor[daCursor] <<= xResSet;

        SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_MAIL_MERGE, rSh.GetView().GetViewFrame()->GetObjectShell()));
        SwMergeDescriptor aMergeDesc( pImpl->pMergeDialog->GetMergeType(), rSh, aDescriptor );
        MergeNew(aMergeDesc);
        SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_MAIL_MERGE_END, rSh.GetView().GetViewFrame()->GetObjectShell()));

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
        DBG_ERROR("PropertyValues missing or unset")
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

//CHINA001	SwInsertDBColAutoPilot *pDlg = new SwInsertDBColAutoPilot(
//CHINA001	rSh.GetView(),
//CHINA001	xSource,
//CHINA001	xColSupp,
//CHINA001	aDBData );
    SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();//CHINA001
    DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");//CHINA001

    AbstractSwInsertDBColAutoPilot* pDlg = pFact->CreateSwInsertDBColAutoPilot( rSh.GetView(),
                                                                                xSource,
                                                                                xColSupp,
                                                                                aDBData,
                                                                                ResId( DLG_AP_INSERT_DB_SEL ));
    DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
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
            DBG_ERROR("exception caught")
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
        DBG_ERROR("exception in getDataSourceAsParent caught")
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
        DBG_ERROR("exception in MergeNew()")
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
    String sSourceDocURL;
    //save the settings of the first
    rSourceShell.SwCrsrShell::SttDoc();
    USHORT nStartingPageNo = rSourceShell.GetVirtPageNum();
    String sModifiedStartingPageDesc;
    String sStartingPageDesc = sModifiedStartingPageDesc = rSourceShell.GetPageDesc(
                                rSourceShell.GetCurPageDesc()).GetName();

    try
    {
        // save the working document into a temporary location
        sSourceDocURL = URIHelper::SmartRel2Abs(
            INetURLObject(), utl::TempFile::CreateTempName(),
            URIHelper::GetMaybeFileHdl());
        const SfxFilter *pSfxFlt = SwIoSystem::GetFilterOfFormat(
                String::CreateFromAscii( FILTER_XML ),
                SwDocShell::Factory().GetFilterContainer() );

        SfxStringItem aFilterName( SID_FILTER_NAME, pSfxFlt->GetFilterName());
        uno::Sequence< beans::PropertyValue > aValues(1);
        beans::PropertyValue* pValues = aValues.getArray();
        pValues[0].Name = C2U("FilterName");
        pValues[0].Value <<= ::rtl::OUString(pSfxFlt->GetFilterName());
        uno::Reference< frame::XStorable > xStore( rSourceView.GetDocShell()->GetModel(), uno::UNO_QUERY);
        xStore->storeToURL( sSourceDocURL, aValues );

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

            // create a new docshell from the temporary document
            SfxBoolItem aHidden( SID_HIDDEN, TRUE );
            SfxStringItem aReferer( SID_REFERER, String::CreateFromAscii(URL_PREFIX_PRIV_SOFFICE ));
            SfxStringItem aTarget( SID_TARGETNAME, String::CreateFromAscii("_blank") );
            SfxStringItem aURL( SID_FILE_NAME, sSourceDocURL );
            const SfxPoolItem* pReturnValue =
                            rSourceView.GetViewFrame()->GetDispatcher()->Execute( SID_OPENDOC, SFX_CALLMODE_SYNCHRON,
                                    &aURL, &aFilterName, &aHidden, &aReferer, &aTarget, 0);
            if(pReturnValue)
            {
                SfxViewFrameItem* pVItem = (SfxViewFrameItem*)pReturnValue;
                SwView* pWorkView = (SwView*) pVItem->GetFrame()->GetViewShell();
                SwWrtShell& rWorkShell = pWorkView->GetWrtShell();
                pWorkView->AttrChangedNotify( &rWorkShell );//Damit SelectShell gerufen wird.

                // merge the data
                SwDoc* pWorkDoc = rWorkShell.GetDoc();
                SwNewDBMgr* pWorkDBMgr = pWorkDoc->GetNewDBMgr();
                pWorkDoc->SetNewDBMgr( this );
                pWorkDoc->EmbedAllLinks();
                if(UIUNDO_DELETE_INVISIBLECNTNT == rWorkShell.GetUndoIds())
                    rWorkShell.Undo();
                // create a layout
                rWorkShell.CalcLayout();
                SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE, rWorkShell.GetView().GetViewFrame()->GetObjectShell()));
                rWorkShell.ViewShell::UpdateFlds();
                SFX_APP()->NotifyEvent(SfxEventHint(SW_EVENT_FIELD_MERGE_FINISHED, rWorkShell.GetView().GetViewFrame()->GetObjectShell()));

                // strip invisible content and convert fields to text
                rWorkShell.RemoveInvisibleContent();
                rWorkShell.ConvertFieldsToText();
                rWorkShell.SetNumberingRestart();


                // insert the document into the target document
                rWorkShell.SwCrsrShell::EndDoc();
                rWorkShell.SwCrsrShell::SttDoc();
                rWorkShell.SelAll();
                pTargetShell->SwCrsrShell::EndDoc();
                
                //#i63806# put the styles to the target document
                //if the source uses headers or footers each new copy need to copy a new page styles 
                if(bPageStylesWithHeaderFooter)
                {
                    //create a new pagestyle
                    //copy the pagedesc from the current document to the new document and change the name of the to-be-applied style
                                  
                    SwDoc* pTargetDoc = pTargetShell->GetDoc();
                    SwPageDesc* pSourcePageDesc = rWorkShell.FindPageDescByName( sStartingPageDesc );
                    String sNewPageDescName = lcl_FindUniqueName(pTargetShell, sStartingPageDesc, nDocNo );
                    pTargetShell->GetDoc()->MakePageDesc( sNewPageDescName );
                    SwPageDesc* pDestPageDesc = pTargetShell->FindPageDescByName( sNewPageDescName );
                    if(pSourcePageDesc && pDestPageDesc)
                    {
                        pTargetDoc->CopyPageDesc( *pSourcePageDesc, *pDestPageDesc );
                        sModifiedStartingPageDesc = sNewPageDescName;
                    }
                }
                if(nDocNo == 1 || bPageStylesWithHeaderFooter)
                {
                    pTargetView->GetDocShell()->_LoadStyles( *pWorkView->GetDocShell(), sal_True );
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
                DBG_ASSERT(!pTargetShell->GetTableFmt(),"target document ends with a table - paragraph should be appended")
                //#i51359# add a second paragraph in case there's only one
                {
                    SwNodeIndex aIdx( pWorkDoc->GetNodes().GetEndOfExtras(), 2 );
                  SwPosition aTestPos( aIdx );
                  SwCursor aTestCrsr(aTestPos);
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
            }
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
    catch(Exception& rEx)
    {
        rEx; // make compiler happy
        DBG_ERROR("exception caught in SwNewDBMgr::MergeDocuments")
    }
    if(sSourceDocURL.Len())
        File::remove( sSourceDocURL );
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

