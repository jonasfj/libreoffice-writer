/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: changedb.cxx,v $
 * $Revision: 1.21 $
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


#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


#define _CHANGEDB_CXX

#include <svtools/stdctrl.hxx>
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/sdb/XDatabaseAccess.hpp>
#include <comphelper/processfactory.hxx>
#include <sfx2/viewfrm.hxx>

#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <wrtsh.hxx>
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#include <fldmgr.hxx>
#include <expfld.hxx>
#include <txtatr.hxx>
#include <ndtxt.hxx>
#include <fldbas.hxx>
#include <dbfld.hxx>
#include <changedb.hxx>

#ifndef _FLDUI_HRC
#include <fldui.hrc>
#endif
#ifndef _UTLUI_HRC
#include <utlui.hrc>
#endif
#ifndef _CHANGEDB_HRC
#include <changedb.hrc>
#endif

#include <unomid.h>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;


/*--------------------------------------------------------------------
    Beschreibung: Feldeinfuegen bearbeiten
 --------------------------------------------------------------------*/


SwChangeDBDlg::SwChangeDBDlg(SwView& rVw) :
    SvxStandardDialog(&rVw.GetViewFrame()->GetWindow(), SW_RES(DLG_CHANGE_DB)),

    aDBListFL   (this, SW_RES(FL_DBLIST     )),
    aUsedDBFT	(this, SW_RES(FT_USEDDB  	)),
    aAvailDBFT	(this, SW_RES(FT_AVAILDB 	)),
    aUsedDBTLB	(this, SW_RES(TLB_USEDDB  	)),
    aAvailDBTLB (this, SW_RES(TLB_AVAILDB   ), 0),
    aAddDBPB    (this, SW_RES(PB_ADDDB)),
    aDescFT     (this, SW_RES(FT_DESC       )),
    aDocDBTextFT(this, SW_RES(FT_DOCDBTEXT	)),
    aDocDBNameFT(this, SW_RES(FT_DOCDBNAME	)),
    aOKBT		(this, SW_RES(BT_OK      	)),
    aCancelBT	(this, SW_RES(BT_CANCEL  	)),
    aHelpBT		(this, SW_RES(BT_HELP    	)),
//	aChangeBT	(this, SW_RES(BT_CHANGEDB    )),
    aImageList      (SW_RES(ILIST_DB_DLG    )),
    aImageListHC    (SW_RES(ILIST_DB_DLG_HC )),

    pSh(rVw.GetWrtShellPtr()),
    pMgr( new SwFldMgr() )
{
    aAvailDBTLB.SetWrtShell(*pSh);
    FillDBPopup();

    FreeResource();

    ShowDBName(pSh->GetDBData());
    aOKBT.SetClickHdl(LINK(this, SwChangeDBDlg, ButtonHdl));
    aAddDBPB.SetClickHdl(LINK(this, SwChangeDBDlg, AddDBHdl));

    aUsedDBTLB.SetSelectionMode(MULTIPLE_SELECTION);
    aUsedDBTLB.SetWindowBits(WB_HASLINES|WB_CLIPCHILDREN|WB_SORT|WB_HASBUTTONS|WB_HASBUTTONSATROOT|WB_HSCROLL);
    aUsedDBTLB.SetSpaceBetweenEntries(0);

    aUsedDBTLB.SetNodeBitmaps( aImageList.GetImage(IMG_COLLAPSE),
                    aImageList.GetImage(IMG_EXPAND  ), BMP_COLOR_NORMAL );
    aUsedDBTLB.SetNodeBitmaps( aImageListHC.GetImage(IMG_COLLAPSE),
                    aImageListHC.GetImage(IMG_EXPAND  ), BMP_COLOR_HIGHCONTRAST );

    Link aLink = LINK(this, SwChangeDBDlg, TreeSelectHdl);

    aUsedDBTLB.SetSelectHdl(aLink);
    aUsedDBTLB.SetDeselectHdl(aLink);
    aAvailDBTLB.SetSelectHdl(aLink);
    aAvailDBTLB.SetDeselectHdl(aLink);
    TreeSelectHdl();
}

/*--------------------------------------------------------------------
    Beschreibung: Datenbank-Listboxen initialisieren
 --------------------------------------------------------------------*/


void SwChangeDBDlg::FillDBPopup()
{
    Reference<XNameAccess> xDBContext;
    Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
    if( xMgr.is() )
    {
        Reference<XInterface> xInstance = xMgr->createInstance( C2U( "com.sun.star.sdb.DatabaseContext" ));
        xDBContext = Reference<XNameAccess>(xInstance, UNO_QUERY) ;
    }
    DBG_ASSERT(xDBContext.is(), "com.sun.star.sdb.DataBaseContext: service not available");

    const SwDBData& rDBData = pSh->GetDBData();
    String sDBName(rDBData.sDataSource);
    String sTableName(rDBData.sCommand);
    aAvailDBTLB.Select(sDBName, sTableName, aEmptyStr);

    SvStringsDtor aAllDBNames(5, 5);

    Sequence< ::rtl::OUString > aDBNames = xDBContext->getElementNames();
    const ::rtl::OUString* pDBNames = aDBNames.getConstArray();
    sal_Int32 nDBCount = aDBNames.getLength();
    for(sal_Int32 i = 0; i < nDBCount; i++)
    {
        aAllDBNames.Insert(new String(pDBNames[i]), aAllDBNames.Count());
    }

    SvStringsDtor aDBNameList(5, 1);
    pSh->GetAllUsedDB( aDBNameList, &aAllDBNames );

    USHORT nCount = aDBNameList.Count();
    aUsedDBTLB.Clear();
    SvLBoxEntry *pFirst = 0;
    SvLBoxEntry *pLast = 0;

    for (USHORT k = 0; k < nCount; k++)
    {
        sDBName = *aDBNameList.GetObject(k);
        sDBName = sDBName.GetToken(0);
        pLast = Insert(sDBName);
        if (!pFirst)
            pFirst = pLast;
    }

    if (pFirst)
    {
        aUsedDBTLB.MakeVisible(pFirst);
        aUsedDBTLB.Select(pFirst);
    }

}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SvLBoxEntry* SwChangeDBDlg::Insert(const String& rDBName)
{
    String sDBName(rDBName.GetToken(0, DB_DELIM));
    String sTableName(rDBName.GetToken(1, DB_DELIM));
    int nCommandType = rDBName.GetToken(2, DB_DELIM).ToInt32();
    SvLBoxEntry* pParent;
    SvLBoxEntry* pChild;

    USHORT nParent = 0;
    USHORT nChild = 0;

    Image aTableImg = aImageList.GetImage(IMG_DBTABLE);
    Image aDBImg = aImageList.GetImage(IMG_DB);
    Image aQueryImg = aImageList.GetImage(IMG_DBQUERY);
    Image aHCTableImg = aImageListHC.GetImage(IMG_DBTABLE);
    Image aHCDBImg = aImageListHC.GetImage(IMG_DB);
    Image aHCQueryImg = aImageListHC.GetImage(IMG_DBQUERY);
    Image& rToInsert = nCommandType ? aQueryImg : aTableImg;
    Image& rHCToInsert = nCommandType ? aHCQueryImg : aHCTableImg;
    while ((pParent = aUsedDBTLB.GetEntry(nParent++)) != NULL)
    {
        if (sDBName == aUsedDBTLB.GetEntryText(pParent))
        {
            while ((pChild = aUsedDBTLB.GetEntry(pParent, nChild++)) != NULL)
            {
                if (sTableName == aUsedDBTLB.GetEntryText(pChild))
                    return pChild;
            }
            SvLBoxEntry* pRet = aUsedDBTLB.InsertEntry(sTableName, rToInsert, rToInsert, pParent);
            aUsedDBTLB.SetExpandedEntryBmp(pRet, rHCToInsert, BMP_COLOR_HIGHCONTRAST);
            aUsedDBTLB.SetCollapsedEntryBmp(pRet, rHCToInsert, BMP_COLOR_HIGHCONTRAST);
            pRet->SetUserData((void*)nCommandType);
            return pRet;
        }
    }
    pParent = aUsedDBTLB.InsertEntry(sDBName, aDBImg, aDBImg);
    aUsedDBTLB.SetExpandedEntryBmp(pParent, aHCDBImg, BMP_COLOR_HIGHCONTRAST);
    aUsedDBTLB.SetCollapsedEntryBmp(pParent, aHCDBImg, BMP_COLOR_HIGHCONTRAST);

    SvLBoxEntry* pRet = aUsedDBTLB.InsertEntry(sTableName, rToInsert, rToInsert, pParent);
    aUsedDBTLB.SetExpandedEntryBmp(pRet, rHCToInsert, BMP_COLOR_HIGHCONTRAST);
    aUsedDBTLB.SetCollapsedEntryBmp(pRet, rHCToInsert, BMP_COLOR_HIGHCONTRAST);
    pRet->SetUserData((void*)nCommandType);
    return pRet;
}

/*--------------------------------------------------------------------
    Beschreibung: Dialog zerstoeren
 --------------------------------------------------------------------*/
__EXPORT SwChangeDBDlg::~SwChangeDBDlg()
{
    delete pMgr;
}

/*--------------------------------------------------------------------
     Beschreibung:	Schliessen
 --------------------------------------------------------------------*/
void __EXPORT SwChangeDBDlg::Apply()
{
    UpdateFlds();
}
/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/
void SwChangeDBDlg::UpdateFlds()
{
    SvStringsDtor aDBNames( (BYTE)aUsedDBTLB.GetSelectionCount(), 1 );
    SvLBoxEntry* pEntry = aUsedDBTLB.FirstSelected();

    while( pEntry )
    {
        if( aUsedDBTLB.GetParent( pEntry ))
        {
            String* pTmp = new String( aUsedDBTLB.GetEntryText(
                                            aUsedDBTLB.GetParent( pEntry )));
            *pTmp += DB_DELIM;
            *pTmp += aUsedDBTLB.GetEntryText( pEntry );
            *pTmp += DB_DELIM;
            int nCommandType = (int)(ULONG)pEntry->GetUserData();
            *pTmp += String::CreateFromInt32(nCommandType);
            aDBNames.Insert(pTmp, aDBNames.Count() );
        }
        pEntry = aUsedDBTLB.NextSelected(pEntry);
    }

    pSh->StartAllAction();
    String sTableName, sColumnName;
    sal_Bool bIsTable = sal_False;
    String sTemp(aAvailDBTLB.GetDBName(sTableName, sColumnName, &bIsTable));
    sTemp += DB_DELIM;
    sTemp += sTableName;
    sTemp += DB_DELIM;
    sTemp += bIsTable ? '0' : '1';
    pSh->ChangeDBFields( aDBNames, sTemp);
    pSh->EndAllAction();
}

/*------------------------------------------------------------------------
 Beschreibung:
------------------------------------------------------------------------*/


IMPL_LINK( SwChangeDBDlg, ButtonHdl, Button *, EMPTYARG )
{
    String sTableName, sColumnName;
    SwDBData aData;
    sal_Bool bIsTable = sal_False;
    aData.sDataSource = aAvailDBTLB.GetDBName(sTableName, sColumnName, &bIsTable);
    aData.sCommand = sTableName;
    aData.nCommandType = bIsTable ? 0 : 1;;
    pSh->ChgDBData(aData);
    ShowDBName(pSh->GetDBData());
    EndDialog(RET_OK);

    return 0;
}

/*------------------------------------------------------------------------
 Beschreibung:
------------------------------------------------------------------------*/


IMPL_LINK( SwChangeDBDlg, TreeSelectHdl, SvTreeListBox *, EMPTYARG )
{
    BOOL bEnable = FALSE;

    SvLBoxEntry* pEntry = aAvailDBTLB.GetCurEntry();

    if (pEntry)
    {
        if (aAvailDBTLB.GetParent(pEntry))
            bEnable = TRUE;
        aOKBT.Enable( bEnable );
    }
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung: Datenbankname fuer Anzeige wandeln
 --------------------------------------------------------------------*/

void SwChangeDBDlg::ShowDBName(const SwDBData& rDBData)
{
    String sTmp(rDBData.sDataSource);
    String sName;
    sTmp += '.';
    sTmp += (String)rDBData.sCommand;

    for (USHORT i = 0; i < sTmp.Len(); i++)
    {
        sName += sTmp.GetChar(i);
        if (sTmp.GetChar(i) == '~')
            sName += '~';
    }

    aDocDBNameFT.SetText(sName);
}
/*-- 27.05.2004 09:14:01---------------------------------------------------

  -----------------------------------------------------------------------*/
IMPL_LINK( SwChangeDBDlg, AddDBHdl, PushButton *, EMPTYARG )
{
    String sNewDB = SwNewDBMgr::LoadAndRegisterDataSource();
    if(sNewDB.Len())
        aAvailDBTLB.AddDataSource(sNewDB);
    return 0;
}

