/*************************************************************************
 *
 *  $RCSfile: regionsw.cxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: jp $ $Date: 2001-11-14 16:30:21 $
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

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _UITOOL_HXX
#include <uitool.hxx>
#endif

#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _SVTOOLS_PASSWORDHELPER_HXX
#include <svtools/PasswordHelper.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SOT_FORMATS_HXX //autogen
#include <sot/formats.hxx>
#endif
#ifndef _PASSWD_HXX //autogen
#include <sfx2/passwd.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX //autogen
#include <sfx2/docfilt.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _OFF_APP_HXX //autogen
#include <offmgr/app.hxx>
#endif
#ifndef _LINKMGR_HXX
#include <so3/linkmgr.hxx>
#endif
#ifndef _SVX_SIZEITEM_HXX //autogen
#define ITEMID_SIZE 0
#include <svx/sizeitem.hxx>
#endif
#ifndef _SVX_BACKGRND_HXX //autogen
#include <svx/backgrnd.hxx>
#endif
#ifndef _OFA_HTMLCFG_HXX
#include <offmgr/htmlcfg.hxx>
#endif

#ifndef _BOOKMRK_HXX //autogen
#include <bookmrk.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>						// fuers SwSectionFmt-Array
#endif
#ifndef _REGIONSW_HXX
#include <regionsw.hxx>
#endif
#ifndef _BASESH_HXX
#include <basesh.hxx>
#endif
#ifndef _WDOCSH_HXX
#include <wdocsh.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>               	// fuer Undo-Ids
#endif
#ifndef _COLUMN_HXX
#include <column.hxx>
#endif
#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _SWUNODEF_HXX
#include <swunodef.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif

#ifndef _HELPID_H
#include <helpid.h>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _REGIONSW_HRC
#include <regionsw.hrc>
#endif
#ifndef _COMCORE_HRC
#include <comcore.hrc>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif

#define FILE_NAME_LENGTH 17

SV_IMPL_OP_PTRARR_SORT( SectReprArr, SectReprPtr )


USHORT lcl_GetRegion( const String& rRegionName, SwWrtShell& rWrtShell );
void lcl_ReadSections( SwWrtShell& rSh, SfxMedium& rMedium, ComboBox& rBox );

/* -----------------25.06.99 15:38-------------------

 --------------------------------------------------*/
class SwTestPasswdDlg : public SfxPasswordDialog
{
public:
        SwTestPasswdDlg(Window* pParent) :
        SfxPasswordDialog(pParent)
        {
            SetHelpId(HID_DLG_PASSWD_SECTION);
        }
};

/*----------------------------------------------------------------------------
 Beschreibung: User Data Klasse fuer Bereichsinformationen
----------------------------------------------------------------------------*/

SectRepr::SectRepr( USHORT nPos, SwSection& rSect ) :
    aSection( CONTENT_SECTION, aEmptyStr ),
    bSelected(FALSE)
{
    aSection = rSect;
    bContent = aSection.GetLinkFileName().Len() == 0;
    nArrPos=nPos;
    SwSectionFmt *pFmt = rSect.GetFmt();
    if( pFmt )
    {
        aCol = pFmt->GetCol();
        aBrush = pFmt->GetBackground();
        aFtnNtAtEnd = pFmt->GetFtnAtTxtEnd();
        aEndNtAtEnd = pFmt->GetEndAtTxtEnd();
        aBalance.SetValue(pFmt->GetBalancedColumns().GetValue());
    }
}

void SectRepr::SetFile( const String& rFile )
{
    String sNewFile( INetURLObject::decode( rFile, INET_HEX_ESCAPE,
                                           INetURLObject::DECODE_UNAMBIGUOUS,
                                        RTL_TEXTENCODING_UTF8 ));
    String sOldFileName( aSection.GetLinkFileName() );
    String sSub( sOldFileName.GetToken( 2, so3::cTokenSeperator ) );

    if( rFile.Len() || sSub.Len() )
    {
        sNewFile += so3::cTokenSeperator;
        if( rFile.Len() ) // Filter nur mit FileName
            sNewFile += sOldFileName.GetToken( 1, so3::cTokenSeperator );

        sNewFile += so3::cTokenSeperator;
        sNewFile +=	sSub;
    }

    aSection.SetLinkFileName( sNewFile );

    if( rFile.Len() || sSub.Len() )
        aSection.SetType( FILE_LINK_SECTION );
    else
        aSection.SetType( CONTENT_SECTION );
}


void SectRepr::SetFilter( const String& rFilter )
{
    String sNewFile;
    String sOldFileName( aSection.GetLinkFileName() );
    String sFile( sOldFileName.GetToken( 0, so3::cTokenSeperator ) );
    String sSub( sOldFileName.GetToken( 2, so3::cTokenSeperator ) );

    if( sFile.Len() )
        (((( sNewFile = sFile ) += so3::cTokenSeperator ) += rFilter )
                                += so3::cTokenSeperator ) += sSub;
    else if( sSub.Len() )
        (( sNewFile = so3::cTokenSeperator ) += so3::cTokenSeperator ) += sSub;

    aSection.SetLinkFileName( sNewFile );

    if( sNewFile.Len() )
        aSection.SetType( FILE_LINK_SECTION );
}

void SectRepr::SetSubRegion(const String& rSubRegion)
{
    String sNewFile;
    String sOldFileName( aSection.GetLinkFileName() );
    String sFilter( sOldFileName.GetToken( 1, so3::cTokenSeperator ) );
    sOldFileName = sOldFileName.GetToken( 0, so3::cTokenSeperator );

    if( rSubRegion.Len() || sOldFileName.Len() )
        (((( sNewFile = sOldFileName ) += so3::cTokenSeperator ) += sFilter )
                                       += so3::cTokenSeperator ) += rSubRegion;

    aSection.SetLinkFileName( sNewFile );

    if( rSubRegion.Len() || sOldFileName.Len() )
        aSection.SetType( FILE_LINK_SECTION );
    else
        aSection.SetType( CONTENT_SECTION );
}


String SectRepr::GetFile() const
{
    String sLinkFile( aSection.GetLinkFileName() );
    if( sLinkFile.Len() )
    {
#ifdef DDE_AVAILABLE
        if( DDE_LINK_SECTION == aSection.GetType() )
        {
            USHORT n = sLinkFile.SearchAndReplace( so3::cTokenSeperator, ' ' );
            sLinkFile.SearchAndReplace( so3::cTokenSeperator, ' ',  n );
        }
        else
#endif
            sLinkFile = INetURLObject::decode( sLinkFile.GetToken( 0,
                                               so3::cTokenSeperator ),
                                        INET_HEX_ESCAPE,
                                           INetURLObject::DECODE_UNAMBIGUOUS,
                                        RTL_TEXTENCODING_UTF8 );
    }
    return sLinkFile;
}


String SectRepr::GetSubRegion() const
{
    String sLinkFile( aSection.GetLinkFileName() );
    if( sLinkFile.Len() )
        sLinkFile = sLinkFile.GetToken( 2, so3::cTokenSeperator );
    return sLinkFile;
}




/*----------------------------------------------------------------------------
 Beschreibung: Dialog Bearbeiten Bereiche
----------------------------------------------------------------------------*/

//---------------------------------------------------------------------

SwEditRegionDlg::SwEditRegionDlg( Window* pParent, SwWrtShell& rWrtSh )
    : SfxModalDialog( pParent, SW_RES(MD_EDIT_REGION) ),
    pAktEntry( 0 ),
    rSh( rWrtSh ),
    aNameFL             ( this, SW_RES( FL_NAME ) ),
    aCurName            ( this, SW_RES( ED_RANAME ) ),
    aTree               ( this, SW_RES( TLB_SECTION )),
    aLinkFL             ( this, SW_RES( FL_LINK ) ),
    aFileCB             ( this, SW_RES( CB_FILE ) ),
#ifdef DDE_AVAILABLE
    aDDECB              ( this, SW_RES( CB_DDE ) ) ,
    aDDECommandFT       ( this, SW_RES( FT_DDE ) ) ,
#endif
    aFileNameFT         ( this, SW_RES( FT_FILE ) ) ,
    aFileNameED         ( this, SW_RES( ED_FILE ) ),
    aFilePB             ( this, SW_RES( PB_FILE ) ),
    aSubRegionFT        ( this, SW_RES( FT_SUBREG ) ) ,
    aSubRegionED        ( this, SW_RES( LB_SUBREG ) ) ,

    aProtectFL          ( this, SW_RES( FL_PROTECT ) ),
    aProtectCB          ( this, SW_RES( CB_PROTECT ) ),
    aPasswdCB			( this, SW_RES( CB_PASSWD ) ),
    aPasswdPB           ( this, SW_RES( PB_PASSWD ) ),

    aHideFL             ( this, SW_RES( FL_HIDE ) ),
    aHideCB             ( this, SW_RES( CB_HIDE ) ),
    aConditionFT             ( this, SW_RES( FT_CONDITION ) ),
    aConditionED        ( this, SW_RES( ED_CONDITION ) ),

    aOK                 ( this, SW_RES( PB_OK ) ),
    aOptionsPB  		( this, SW_RES( PB_OPTIONS ) ),
    aDismiss			( this, SW_RES( CB_DISMISS ) ),
    aHelp               ( this, SW_RES( PB_HELP ) ),
    aCancel             ( this, SW_RES( PB_CANCEL ) ),
    aProtHideBM       	( SW_RES( BMP_REG_PROT_HIDE ) ),
    aProtNoHideBM    	( SW_RES( BMP_REG_PROT_NOHIDE ) ),
    aNoProtHideBM    	( SW_RES( BMP_REG_NOPROT_HIDE ) ),
    aNoProtNoHideBM  	( SW_RES( BMP_REG_NOPROT_NOHIDE ) ),
    aExpNode			( SW_RES( BMP_REG_EXPNODE ) ),
    aCollNode           ( SW_RES( BMP_REG_COLLNODE ) ),
    bDontCheckPasswd(sal_True)
{
    FreeResource();

    bWeb = 0 != PTR_CAST( SwWebDocShell, rSh.GetView().GetDocShell() );

    aTree.SetSelectHdl		( LINK( this, SwEditRegionDlg, GetFirstEntryHdl));
    aTree.SetDeselectHdl	( LINK( this, SwEditRegionDlg, DeselectHdl));
    aCurName.SetModifyHdl	( LINK( this, SwEditRegionDlg, NameEditHdl));
    aConditionED.SetModifyHdl( LINK( this, SwEditRegionDlg, ConditionEditHdl));
    aOK.SetClickHdl			( LINK( this, SwEditRegionDlg, OkHdl));
    aPasswdCB.SetClickHdl	( LINK( this, SwEditRegionDlg, ChangePasswdHdl));
    aPasswdPB.SetClickHdl   ( LINK( this, SwEditRegionDlg, ChangePasswdHdl));
    aHideCB.SetClickHdl		( LINK( this, SwEditRegionDlg, ChangeHideHdl));
    aOptionsPB.Show();
    aOptionsPB.SetClickHdl	( LINK( this, SwEditRegionDlg, OptionsHdl));
    aProtectCB.SetClickHdl	( LINK( this, SwEditRegionDlg, ChangeProtectHdl));
    aDismiss.SetClickHdl	( LINK( this, SwEditRegionDlg, ChangeDismissHdl));
    aFileCB.SetClickHdl     ( LINK( this, SwEditRegionDlg, UseFileHdl ));
    aFilePB.SetClickHdl     ( LINK( this, SwEditRegionDlg, FileSearchHdl ));
    aFileNameED.SetModifyHdl( LINK( this, SwEditRegionDlg, FileNameHdl ));
    aSubRegionED.SetModifyHdl( LINK( this, SwEditRegionDlg, FileNameHdl ));

    aTree.SetHelpId(HID_REGION_TREE);
    aTree.SetSelectionMode( MULTIPLE_SELECTION );
    aTree.SetWindowBits(WB_HASBUTTONSATROOT|WB_CLIPCHILDREN|WB_HSCROLL);
    aTree.SetNodeBitmaps( aExpNode,aCollNode );
    aTree.SetSpaceBetweenEntries(0);

    if(bWeb)
    {
        aConditionFT         .Hide();
        aConditionED    .Hide();
        aPasswdCB		.Hide();
        aHideCB			.Hide();
#ifdef DDE_AVAILABLE
    aDDECB              .Hide();
    aDDECommandFT       .Hide();
#endif
    }

#ifdef DDE_AVAILABLE
    aDDECB.SetClickHdl		( LINK( this, SwEditRegionDlg, DDEHdl ));
#endif

    //Array enthaelt Bitmaps nach den Eigenschaften Hide und Protect
    aBmpArr[0]=	aNoProtNoHideBM;
    aBmpArr[1]=	aProtNoHideBM;
    aBmpArr[2]=	aNoProtHideBM;
    aBmpArr[3]=	aProtHideBM;

    //Ermitteln der vorhandenen Bereiche
    pCurrSect = rSh.GetCurrSection();
    RecurseList( 0, 0 );
    //falls der Cursor nicht in einem Bereich steht,
    //wird immer der erste selektiert
    if( !aTree.FirstSelected() && aTree.First() )
        aTree.Select( aTree.First() );
    aTree.Show();
    bDontCheckPasswd = sal_False;
}
/* -----------------------------26.04.01 14:56--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwEditRegionDlg::CheckPasswd(CheckBox* pBox)
{
    if(bDontCheckPasswd)
        return TRUE;
    sal_Bool bRet = TRUE;
    SvLBoxEntry* pEntry = aTree.FirstSelected();
    while( pEntry )
    {
        SectReprPtr pRepr = (SectReprPtr)pEntry->GetUserData();
        if(!pRepr->GetTempPasswd().getLength() && pRepr->GetPasswd().getLength())
        {
            SwTestPasswdDlg aPasswdDlg(this);
            bRet = FALSE;
            if (aPasswdDlg.Execute())
            {
                String sNewPasswd( aPasswdDlg.GetPassword() );
                UNO_NMSPC::Sequence <sal_Int8 > aNewPasswd;
                SvPasswordHelper::GetHashPassword( aNewPasswd, sNewPasswd );
                if(aNewPasswd == pRepr->GetPasswd())
                {
                    pRepr->SetTempPasswd(aNewPasswd);
                    bRet = TRUE;
                }
                else
                {
                    InfoBox(this, SW_RES(REG_WRONG_PASSWORD)).Execute();
                }
            }
        }
        pEntry = aTree.NextSelected(pEntry);
    }
    if(!bRet && pBox)
    {
        //reset old button state
        if(pBox->IsTriStateEnabled())
            pBox->SetState(pBox->IsChecked() ? STATE_NOCHECK : STATE_DONTKNOW);
        else
            pBox->Check(!pBox->IsChecked());
    }

    return bRet;
}
/*---------------------------------------------------------------------
    Beschreibung: Durchsuchen nach Child-Sections, rekursiv
---------------------------------------------------------------------*/

void SwEditRegionDlg::RecurseList( const SwSectionFmt* pFmt, SvLBoxEntry* pEntry )
{
    SwSection* pSect;
    SvLBoxEntry* pSelEntry = 0;

    if (!pFmt)
    {
        SvLBoxEntry* pEntry;
        const SwSectionFmt* pFmt;
        USHORT nCount=rSh.GetSectionFmtCount();
        for ( USHORT n=0; n < nCount; n++ )
        {
            SectionType eTmpType;
            if( !( pFmt = &rSh.GetSectionFmt(n))->GetParent() &&
                pFmt->IsInNodesArr() &&
                (eTmpType = pFmt->GetSection()->GetType()) != TOX_CONTENT_SECTION
                && TOX_HEADER_SECTION != eTmpType )
            {
                SectRepr* pSectRepr = new SectRepr( n,
                                            *(pSect=pFmt->GetSection()) );
                Bitmap aBM=BuildBitmap( pSect->IsProtect(),pSect->IsHidden());
                pEntry = aTree.InsertEntry( pSect->GetName(), aBM, aBM );
                pEntry->SetUserData(pSectRepr);
                RecurseList( pFmt, pEntry );
                if (pEntry->HasChilds())
                    aTree.Expand(pEntry);
                if (pCurrSect==pSect)
                    aTree.Select(pEntry);
            }
        }
    }
    else
    {
        SwSections aTmpArr;
        SvLBoxEntry* pNEntry;
        USHORT nCnt = pFmt->GetChildSections(aTmpArr,SORTSECT_POS);
        if( nCnt )
        {
            for( USHORT n = 0; n < nCnt; ++n )
            {
                SectionType eTmpType;
                const SwSectionFmt* pFmt = aTmpArr[n]->GetFmt();
                if( pFmt->IsInNodesArr() &&
                    (eTmpType = pFmt->GetSection()->GetType()) != TOX_CONTENT_SECTION
                    && TOX_HEADER_SECTION != eTmpType )
                {
                    pSect=aTmpArr[n];
                    SectRepr* pSectRepr=new SectRepr(
                                    FindArrPos( pSect->GetFmt() ), *pSect );
                    Bitmap aBM=BuildBitmap( pSect->IsProtect(),
                                            pSect->IsHidden());
                    pNEntry=aTree.InsertEntry( pSect->GetName(),aBM,aBM,pEntry);
                    pNEntry->SetUserData(pSectRepr);
                    RecurseList( aTmpArr[n]->GetFmt(), pNEntry );
                    if( pNEntry->HasChilds())
                        aTree.Expand(pNEntry);
                    if (pCurrSect==pSect)
                        pSelEntry = pNEntry;
                }
            }
        }
    }
    if(0 != pSelEntry)
    {
        aTree.MakeVisible(pSelEntry);
        aTree.Select(pSelEntry);
    }
}
/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

USHORT SwEditRegionDlg::FindArrPos(const SwSectionFmt* pFmt )
{
    USHORT nCount=rSh.GetSectionFmtCount();
    for (USHORT i=0;i<nCount;i++)
        if (pFmt==&rSh.GetSectionFmt(i))
            return i;

    DBG_ERROR(  "SectionFormat nicht in der Liste" );
    return USHRT_MAX;
}
/*---------------------------------------------------------------------
 Beschreibung:
---------------------------------------------------------------------*/

SwEditRegionDlg::~SwEditRegionDlg( )
{
    SvLBoxEntry* pEntry = aTree.First();
    while( pEntry )
    {
        delete (SectRepr*)pEntry->GetUserData();
        pEntry = aTree.Next( pEntry );
    }

    aSectReprArr.DeleteAndDestroy( 0, aSectReprArr.Count() );
}
/* -----------------------------09.10.2001 15:41------------------------------

 ---------------------------------------------------------------------------*/
void    SwEditRegionDlg::SelectSection(const String& rSectionName)
{
    SvLBoxEntry* pEntry = aTree.First();
    while(pEntry)
    {
        SectReprPtr pRepr = (SectReprPtr)pEntry->GetUserData();
        if(pRepr->GetSection().GetName() == rSectionName)
            break;
        pEntry = aTree.Next(pEntry);
    }
    if(pEntry)
    {
        aTree.SelectAll( FALSE);
        aTree.Select(pEntry);
        aTree.MakeVisible(pEntry);
    }
}
/*---------------------------------------------------------------------
    Beschreibung: 	Selektierte Eintrag in der TreeListBox wird im
                    Edit-Fenster angezeigt
                    Bei Multiselektion werden einige Controls disabled
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, GetFirstEntryHdl, SvTreeListBox *, pBox )
{
    bDontCheckPasswd = sal_True;
    SvLBoxEntry* pEntry=pBox->FirstSelected();
    aHideCB		.Enable(TRUE);
    aProtectCB	.Enable(TRUE);
    aFileCB		.Enable(TRUE);
    UNO_NMSPC::Sequence <sal_Int8> aCurPasswd;
    if( 1 < pBox->GetSelectionCount() )
    {
        aHideCB.EnableTriState( TRUE );
        aProtectCB.EnableTriState( TRUE );
        aFileCB.EnableTriState( TRUE );

        BOOL bHiddenValid 		= TRUE;
        BOOL bProtectValid 		= TRUE;
        BOOL bConditionValid 	= TRUE;
        BOOL bHidden 			= TRUE;
        BOOL bProtect 			= TRUE;
        String sCondition;
        BOOL bFirst 			= TRUE;
        BOOL bFileValid 		= TRUE;
        BOOL bFile 				= TRUE;
        BOOL bPasswdValid       = TRUE;

        SvLBoxEntry* pEntry = pBox->FirstSelected();
        while( pEntry )
        {
            SectRepr* pRepr=(SectRepr*) pEntry->GetUserData();
            if(bFirst)
            {
                sCondition = pRepr->GetCondition();
                bHidden 		= pRepr->IsHidden();
                bProtect 		= pRepr->IsProtect();
                bFile 			= pRepr->GetSectionType() != CONTENT_SECTION;
                aCurPasswd      = pRepr->GetPasswd();
            }
            else
            {
                String sTemp(pRepr->GetCondition());
                if(sCondition != sTemp)
                    bConditionValid = FALSE;
                bHiddenValid 	  = bHidden == pRepr->IsHidden();
                bProtectValid 	  = bProtect == pRepr->IsProtect();
                bFileValid  	  = (pRepr->GetSectionType() != CONTENT_SECTION) == bFile;
                bPasswdValid      =  aCurPasswd == pRepr->GetPasswd();
            }
            pEntry = pBox->NextSelected(pEntry);
            bFirst = FALSE;
        }

        aHideCB.SetState( !bHiddenValid ? STATE_DONTKNOW :
                    bHidden ? STATE_CHECK : STATE_NOCHECK);
        aProtectCB.SetState( !bProtectValid ? STATE_DONTKNOW :
                    bProtect ? STATE_CHECK : STATE_NOCHECK);
        aFileCB.SetState(!bFileValid ? STATE_DONTKNOW :
                    bFile ? STATE_CHECK : STATE_NOCHECK);

        if(bConditionValid)
            aConditionED.SetText(sCondition);
        else
        {
//			aConditionED.SetText(aEmptyStr);
            aConditionFT.Enable(FALSE);
            aConditionED.Enable(FALSE);
        }

        aFilePB.Enable(FALSE);
        aFileNameFT	.Enable(FALSE);
        aFileNameED	.Enable(FALSE);
        aSubRegionFT.Enable(FALSE);
        aSubRegionED.Enable(FALSE);
//        aNameFT     .Enable(FALSE);
        aCurName	.Enable(FALSE);
        aOptionsPB	.Enable(FALSE);
#ifdef DDE_AVAILABLE
    aDDECB   			.Enable(FALSE);
    aDDECommandFT       .Enable(FALSE);
#endif
        BOOL bPasswdEnabled = aProtectCB.GetState() == STATE_CHECK;
        aPasswdCB.Enable(bPasswdEnabled);
        aPasswdPB.Enable(bPasswdEnabled);
        if(!bPasswdValid)
        {
            pEntry = pBox->FirstSelected();
            pBox->SelectAll( FALSE );
            pBox->Select( pEntry );
            GetFirstEntryHdl(pBox);
            return 0;
        }
        else
            aPasswdCB.Check(aCurPasswd.getLength() > 0);
    }
    else if (pEntry )
    {
//        aNameFT     .Enable(TRUE);
        aCurName	.Enable(TRUE);
        aOptionsPB	.Enable(TRUE);
        SectRepr* pRepr=(SectRepr*) pEntry->GetUserData();
        aConditionED.SetText(pRepr->GetCondition());
        aHideCB.Enable();
        aHideCB.SetState(pRepr->IsHidden() ? STATE_CHECK : STATE_NOCHECK);
        BOOL bHide = STATE_CHECK == aHideCB.GetState();
        aConditionED.Enable(bHide);
        aConditionFT.Enable(bHide);
         aPasswdCB.Check(pRepr->GetPasswd().getLength() > 0);

        aOK.Enable();
        aPasswdCB.Enable();
        aCurName.SetText(pBox->GetEntryText(pEntry));
        aCurName.Enable();
        aDismiss.Enable();
        String aFile = pRepr->GetFile();
        String sSub = pRepr->GetSubRegion();
        if(aFile.Len()||sSub.Len())
        {
            aFileCB.Check(TRUE);
            aFileNameED.SetText(aFile);
            aSubRegionED.SetText(sSub);
#ifdef DDE_AVAILABLE
            aDDECB.Check(pRepr->GetSectionType() == DDE_LINK_SECTION );
#endif
        }
        else
        {
            aFileCB.Check(FALSE);
            aFileNameED.SetText(aFile);
#ifdef DDE_AVAILABLE
            aDDECB.Enable(FALSE);
            aDDECB.Check(FALSE);
#endif
        }
        UseFileHdl(&aFileCB);
#ifdef DDE_AVAILABLE
        DDEHdl( &aDDECB );
#endif
        aProtectCB.SetState(pRepr->IsProtect() ? STATE_CHECK : STATE_NOCHECK);
        aProtectCB.Enable();
        BOOL bPasswdEnabled = aProtectCB.IsChecked();
        aPasswdCB.Enable(bPasswdEnabled);
        aPasswdPB.Enable(bPasswdEnabled);
    }
    bDontCheckPasswd = sal_False;
    return 0;
}
/*-----------------28.06.97 09:19-------------------

--------------------------------------------------*/
IMPL_LINK( SwEditRegionDlg, DeselectHdl, SvTreeListBox *, pBox )
{
    if( !pBox->GetSelectionCount() )
    {
        aHideCB		.Enable(FALSE);
        aProtectCB	.Enable(FALSE);
        aPasswdCB   .Enable(FALSE);
        aPasswdCB   .Enable(FALSE);
        aConditionFT     .Enable(FALSE);
        aConditionED.Enable(FALSE);
        aFileCB		.Enable(FALSE);
        aFilePB		.Enable(FALSE);
        aFileNameFT  .Enable(FALSE);
        aFileNameED  .Enable(FALSE);
        aSubRegionFT .Enable(FALSE);
        aSubRegionED .Enable(FALSE);
//        aNameFT      .Enable(FALSE);
        aCurName	 .Enable(FALSE);
#ifdef DDE_AVAILABLE
    aDDECB   			.Enable(FALSE);
    aDDECommandFT       .Enable(FALSE);
#endif

        UseFileHdl(&aFileCB);
#ifdef DDE_AVAILABLE
        DDEHdl( &aDDECB );
#endif
    }
    return 0;
}

/*---------------------------------------------------------------------
    Beschreibung:	Im OkHdl werden die veraenderten Einstellungen
                    uebernommen und aufgehobene Bereiche geloescht
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, OkHdl, CheckBox *, EMPTYARG )
{
    // JP 13.03.96:
    // temp. Array weil sich waehrend des aendern eines Bereiches die
    // Position innerhalb des "Core-Arrays" verschieben kann:
    //	- bei gelinkten Bereichen, wenn sie weitere SubBereiche haben oder
    //	  neu erhalten.
    // JP 30.05.97: StartUndo darf natuerlich auch erst nach dem Kopieren
    //				der Formate erfolgen (ClearRedo!)

    const SwSectionFmts& rDocFmts = rSh.GetDoc()->GetSections();
    SwSectionFmts aOrigArray( 0, 5 );
    aOrigArray.Insert( &rDocFmts, 0 );

    rSh.StartAllAction();
    rSh.StartUndo( UNDO_CHGSECTION );
    rSh.ResetSelect( 0,FALSE );
    SvLBoxEntry* pEntry = aTree.First();

    while( pEntry )
    {
        SectReprPtr pRepr = (SectReprPtr) pEntry->GetUserData();
        SwSectionFmt* pFmt = aOrigArray[ pRepr->GetArrPos() ];
        if( !pRepr->GetSection().IsProtectFlag())
            pRepr->GetSection().SetPasswd(UNO_NMSPC::Sequence <sal_Int8 >());
        USHORT nNewPos = rDocFmts.GetPos( pFmt );
        if( USHRT_MAX != nNewPos )
        {
            SfxItemSet* pSet = pFmt->GetAttrSet().Clone( FALSE );
            if( pFmt->GetCol() != pRepr->GetCol() )
                pSet->Put( pRepr->GetCol() );

            if( pFmt->GetBackground(FALSE) != pRepr->GetBackground() )
                pSet->Put( pRepr->GetBackground() );

            if( pFmt->GetFtnAtTxtEnd(FALSE) != pRepr->GetFtnNtAtEnd() )
                pSet->Put( pRepr->GetFtnNtAtEnd() );

            if( pFmt->GetEndAtTxtEnd(FALSE) != pRepr->GetEndNtAtEnd() )
                pSet->Put( pRepr->GetEndNtAtEnd() );

            if( pFmt->GetBalancedColumns() != pRepr->GetBalance() )
                pSet->Put( pRepr->GetBalance() );

            rSh.ChgSection( nNewPos, pRepr->GetSection(),
                            pSet->Count() ? pSet : 0 );
            delete pSet;
        }
        pEntry = aTree.Next( pEntry );
    }

    for(USHORT i = aSectReprArr.Count(); i; )
    {
        SwSectionFmt* pFmt = aOrigArray[ aSectReprArr[ --i ]->GetArrPos() ];
        USHORT nNewPos = rDocFmts.GetPos( pFmt );
        if( USHRT_MAX != nNewPos )
            rSh.DelSectionFmt( nNewPos );
    }
//    rSh.ChgSectionPasswd(aNewPasswd);

    aOrigArray.Remove( 0, aOrigArray.Count() );

    //JP 21.05.97: EndDialog muss vor Ende der EndAction gerufen werden,
    //				sonst kann es ScrollFehler geben.
    EndDialog(RET_OK);

    rSh.EndUndo( UNDO_CHGSECTION );
    rSh.EndAllAction();

    return 0;
}
/*---------------------------------------------------------------------
 Beschreibung: Toggle protect
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, ChangeProtectHdl, TriStateBox *, pBox )
{
    if(!CheckPasswd(pBox))
        return 0;
    pBox->EnableTriState( FALSE );
    SvLBoxEntry* pEntry=aTree.FirstSelected();
    DBG_ASSERT(pEntry,"kein Entry gefunden");
    BOOL bCheck = STATE_CHECK == pBox->GetState();
    while( pEntry )
    {
        SectReprPtr pRepr = (SectReprPtr) pEntry->GetUserData();
        pRepr->SetProtect(bCheck);
        Bitmap& aBmp=BuildBitmap( bCheck,
                                    STATE_CHECK == aHideCB.GetState());
        aTree.SetExpandedEntryBmp(pEntry,aBmp);
        aTree.SetCollapsedEntryBmp(pEntry,aBmp);
        pEntry = aTree.NextSelected(pEntry);
    }
    aPasswdCB.Enable(bCheck);
    aPasswdPB.Enable(bCheck);
    return 0;
}
/*---------------------------------------------------------------------
 Beschreibung: Toggle hide
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, ChangeHideHdl, TriStateBox *, pBox )
{
    if(!CheckPasswd(pBox))
        return 0;
    pBox->EnableTriState( FALSE );
    SvLBoxEntry* pEntry=aTree.FirstSelected();
    DBG_ASSERT(pEntry,"kein Entry gefunden");
    while( pEntry )
    {
        SectReprPtr pRepr = (SectReprPtr) pEntry->GetUserData();
        pRepr->SetHidden(STATE_CHECK == pBox->GetState());
        Bitmap& aBmp=BuildBitmap(STATE_CHECK == aProtectCB.GetState(),
                                    STATE_CHECK == pBox->GetState());
        aTree.SetExpandedEntryBmp(pEntry,aBmp);
        aTree.SetCollapsedEntryBmp(pEntry,aBmp);

        pEntry = aTree.NextSelected(pEntry);
    }

    BOOL bHide = STATE_CHECK == pBox->GetState();
    aConditionED.Enable(bHide);
    aConditionFT.Enable(bHide);
    return 0;
}
/*---------------------------------------------------------------------
 Beschreibung: selektierten Bereich aufheben
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, ChangeDismissHdl, CheckBox *, EMPTYARG )
{
    if(!CheckPasswd())
        return 0;
    SvLBoxEntry* pEntry = aTree.FirstSelected();
    SvLBoxEntry* pChild;
    SvLBoxEntry* pParent;
    //zuerst alle selektierten markieren
    while(pEntry)
    {
        const SectReprPtr pSectRepr = (SectRepr*)pEntry->GetUserData();
        pSectRepr->SetSelected();
        pEntry = aTree.NextSelected(pEntry);
    }
    pEntry = aTree.FirstSelected();
    // dann loeschen
    while(pEntry)
    {
        const SectReprPtr pSectRepr = (SectRepr*)pEntry->GetUserData();
        SvLBoxEntry* pRemove = 0;
        BOOL bRestart = FALSE;
        if(pSectRepr->IsSelected())
        {
            aSectReprArr.Insert( pSectRepr );
            while( (pChild = aTree.FirstChild(pEntry) )!= 0 )
            {
                //durch das Umhaengen muss wieder am Anfang aufgesetzt werden
                bRestart = TRUE;
                pParent=aTree.GetParent(pEntry);
                aTree.GetModel()->Move(pChild, pParent, aTree.GetModel()->GetRelPos(pEntry));
            }
            pRemove = pEntry;
        }
        if(bRestart)
            pEntry = aTree.First();
        else
            pEntry = aTree.Next(pEntry);
        if(pRemove)
            aTree.GetModel()->Remove( pRemove );
    }

    if ( (pEntry=aTree.FirstSelected()) == 0 )
    {
        aConditionFT.        Enable(FALSE);
        aConditionED.   Enable(FALSE);
        aDismiss.		Enable(FALSE);
        aCurName.		Enable(FALSE);
        aProtectCB.		Enable(FALSE);
        aPasswdCB.		Enable(FALSE);
        aHideCB.		Enable(FALSE);
        aProtectCB.		SetState(STATE_NOCHECK);
        aPasswdCB.		Check(FALSE);
        aHideCB.		SetState(STATE_NOCHECK);
        aFileCB.		Check(FALSE);
        //sonst liegt der Focus auf dem HelpButton
        aOK.GrabFocus();
        UseFileHdl(&aFileCB);
    }
    return 0;
}
/*---------------------------------------------------------------------
 Beschreibung: CheckBox mit Datei verknuepfen�
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, UseFileHdl, CheckBox *, pBox )
{
    if(!CheckPasswd(pBox))
        return 0;
    SvLBoxEntry* pEntry = aTree.FirstSelected();
    pBox->EnableTriState(FALSE);
    BOOL bMulti = 1 < aTree.GetSelectionCount();
    BOOL bFile = pBox->IsChecked();
    if(pEntry)
    {
        while(pEntry)
        {
            const SectReprPtr pSectRepr = (SectRepr*)pEntry->GetUserData();
            BOOL bContent = pSectRepr->IsContent();
            if( pBox->IsChecked() && bContent && rSh.HasSelection() )
            {
                if( RET_NO == QueryBox( this, SW_RES(QB_CONNECT) ).Execute() )
                    pBox->Check( FALSE );
            }
            if( bFile )
                pSectRepr->SetContent(FALSE);
            else
            {
                pSectRepr->SetFile(aEmptyStr);
                pSectRepr->SetSubRegion(aEmptyStr);
                pSectRepr->SetFilePasswd(aEmptyStr);
            }

            pEntry = aTree.NextSelected(pEntry);
        }
        aFileNameFT.Enable(bFile && ! bMulti);
        aFileNameED.Enable(bFile && ! bMulti);
        aFilePB.Enable(bFile && ! bMulti);
        aSubRegionED.Enable(bFile && ! bMulti);
        aSubRegionFT.Enable(bFile && ! bMulti);
#ifdef DDE_AVAILABLE
        aDDECommandFT.Enable(bFile && ! bMulti);
        aDDECB.Enable(bFile && ! bMulti);
#endif
        if( bFile )
        {
            aProtectCB.SetState(STATE_CHECK);
            aFileNameED.GrabFocus();

        }
        else
        {
#ifdef DDE_AVAILABLE
            aDDECB.Check(FALSE);
            DDEHdl(&aDDECB);
#endif
//			aFileNameED.SetText(aEmptyStr);
            aSubRegionED.SetText(aEmptyStr);
        }
    }
    else
    {
        pBox->Check(FALSE);
        pBox->Enable(FALSE);
        aFilePB.Enable(FALSE);
        aFileNameED.Enable(FALSE);
        aFileNameFT.Enable(FALSE);
        aSubRegionED.Enable(FALSE);
        aSubRegionFT.Enable(FALSE);
#ifdef DDE_AVAILABLE
        aDDECB.Check(FALSE);
        aDDECB.Enable(FALSE);
        aDDECommandFT.Enable(FALSE);
#endif
    }
    return 0;
}

/*---------------------------------------------------------------------
    Beschreibung: Dialog Datei einfuegen rufen
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, FileSearchHdl, PushButton *, EMPTYARG )
{
    if(!CheckPasswd(0))
        return 0;
    SfxMedium* pMed;
    String sFileName, sFilterName, sFilePasswd;
    if( GetFileFilterNameDlg( *this, sFileName, &sFilePasswd,
                                &sFilterName, &pMed ))
    {
        ::lcl_ReadSections( rSh, *pMed, aSubRegionED );
        delete pMed;
    }

    SvLBoxEntry* pEntry = aTree.FirstSelected();
    DBG_ASSERT(pEntry,"kein Entry gefunden");
    if(pEntry)
    {
        SectReprPtr pSectRepr = (SectRepr*)pEntry->GetUserData();
        pSectRepr->SetFile( sFileName );
        pSectRepr->SetFilter( sFilterName );
        pSectRepr->SetFilePasswd( sFilePasswd );
        aFileNameED.SetText( pSectRepr->GetFile());
    }
    return 0;
}

/*---------------------------------------------------------------------
    Beschreibung:
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, OptionsHdl, PushButton *, EMPTYARG )
{
    if(!CheckPasswd())
        return 0;
    SvLBoxEntry* pEntry = aTree.FirstSelected();

    if(pEntry)
    {
        SectReprPtr pSectRepr = (SectRepr*)pEntry->GetUserData();
        SfxItemSet aSet(rSh.GetView().GetPool(),
                            RES_COL, RES_COL,
                            RES_COLUMNBALANCE, RES_COLUMNBALANCE,
                            RES_BACKGROUND, RES_BACKGROUND,
                            RES_FRM_SIZE, RES_FRM_SIZE,
                            SID_ATTR_PAGE_SIZE, SID_ATTR_PAGE_SIZE,
                            RES_LR_SPACE, RES_LR_SPACE,
                            RES_FTN_AT_TXTEND, RES_END_AT_TXTEND,
                            0);

        aSet.Put( pSectRepr->GetCol() );
        aSet.Put( pSectRepr->GetBackground() );
        aSet.Put( pSectRepr->GetFtnNtAtEnd() );
        aSet.Put( pSectRepr->GetEndNtAtEnd() );
        aSet.Put( pSectRepr->GetBalance() );

        const SwSectionFmts& rDocFmts = rSh.GetDoc()->GetSections();
        SwSectionFmts aOrigArray( 0, 5 );
        aOrigArray.Insert( &rDocFmts, 0 );

        SwSectionFmt* pFmt = aOrigArray[pSectRepr->GetArrPos()];
        long nWidth = rSh.GetSectionWidth(*pFmt);
        aOrigArray.Remove( 0, aOrigArray.Count() );
        if (!nWidth)
            nWidth = USHRT_MAX;

        aSet.Put(SwFmtFrmSize(ATT_VAR_SIZE, nWidth));
        aSet.Put(SvxSizeItem(SID_ATTR_PAGE_SIZE, Size(nWidth, nWidth)));

        SwSectionPropertyTabDialog aTabDlg(this, aSet, rSh);
        if(RET_OK == aTabDlg.Execute())
        {
            const SfxItemSet* pOutSet = aTabDlg.GetOutputItemSet();
            if( pOutSet && pOutSet->Count() )
            {
                const SfxPoolItem *pColItem, *pBrushItem,
                                  *pFtnItem, *pEndItem, *pBalanceItem;
                SfxItemState eColState = pOutSet->GetItemState(
                                        RES_COL, FALSE, &pColItem );
                SfxItemState eBrushState = pOutSet->GetItemState(
                                        RES_BACKGROUND, FALSE, &pBrushItem );
                SfxItemState eFtnState = pOutSet->GetItemState(
                                        RES_FTN_AT_TXTEND, FALSE, &pFtnItem );
                SfxItemState eEndState = pOutSet->GetItemState(
                                        RES_END_AT_TXTEND, FALSE, &pEndItem );
                SfxItemState eBalanceState = pOutSet->GetItemState(
                                        RES_COLUMNBALANCE, FALSE, &pBalanceItem );
                if( SFX_ITEM_SET == eColState ||
                    SFX_ITEM_SET == eBrushState ||
                    SFX_ITEM_SET == eFtnState ||
                    SFX_ITEM_SET == eEndState ||
                    SFX_ITEM_SET == eBalanceState)
                {
                    SvLBoxEntry* pEntry = aTree.FirstSelected();
                    while( pEntry )
                    {
                        SectReprPtr pRepr = (SectReprPtr)pEntry->GetUserData();
                        if( SFX_ITEM_SET == eColState )
                            pRepr->GetCol() = *(SwFmtCol*)pColItem;
                        if( SFX_ITEM_SET == eBrushState )
                            pRepr->GetBackground() = *(SvxBrushItem*)pBrushItem;
                        if( SFX_ITEM_SET == eFtnState )
                            pRepr->GetFtnNtAtEnd() = *(SwFmtFtnAtTxtEnd*)pFtnItem;
                        if( SFX_ITEM_SET == eEndState )
                            pRepr->GetEndNtAtEnd() = *(SwFmtEndAtTxtEnd*)pEndItem;
                        if( SFX_ITEM_SET == eBalanceState )
                            pRepr->GetBalance().SetValue(((SwFmtNoBalancedColumns*)pBalanceItem)->GetValue());

                        pEntry = aTree.NextSelected(pEntry);
                    }
                }
            }
        }
    }

    return 0;
}

/*---------------------------------------------------------------------
    Beschreibung:  	Uebernahme des Dateinamen oder
                    des verknuepften Bereichs
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, FileNameHdl, Edit *, pEdit )
{
    Selection aSelect = pEdit->GetSelection();
    if(!CheckPasswd())
        return 0;
    pEdit->SetSelection(aSelect);
    SvLBoxEntry* pEntry=aTree.FirstSelected();
    DBG_ASSERT(pEntry,"kein Entry gefunden");
    SectReprPtr pSectRepr = (SectRepr*)pEntry->GetUserData();
    if(pEdit == &aFileNameED)
    {
        BOOL bDDe = FALSE;
#ifdef DDE_AVAILABLE
        bDDe = aDDECB.IsChecked();
#endif
        if( bDDe )
        {
            String sLink( pEdit->GetText() );
            USHORT nPos = 0;
            while( STRING_NOTFOUND != (nPos = sLink.SearchAscii( "  ", nPos )) )
                sLink.Erase( nPos--, 1 );

            nPos = sLink.SearchAndReplace( ' ', so3::cTokenSeperator );
            sLink.SearchAndReplace( ' ', so3::cTokenSeperator, nPos );

            pSectRepr->GetSection().SetLinkFileName( sLink );
            pSectRepr->GetSection().SetType( DDE_LINK_SECTION );
        }
        else
        {
            String sTmp(pEdit->GetText());
            if(sTmp.Len())
                sTmp = URIHelper::SmartRelToAbs( sTmp );
            pSectRepr->SetFile( sTmp );
            pSectRepr->SetFilePasswd( aEmptyStr );
        }
    }
    else
    {
        pSectRepr->SetSubRegion( pEdit->GetText() );
    }
    return 0;
}
/*---------------------------------------------------------------------
    Beschreibung:
---------------------------------------------------------------------*/
#ifdef DDE_AVAILABLE

IMPL_LINK( SwEditRegionDlg, DDEHdl, CheckBox*, pBox )
{
    if(!CheckPasswd(pBox))
        return 0;
    SvLBoxEntry* pEntry=aTree.FirstSelected();
    if(pEntry)
    {
        BOOL bFile = aFileCB.IsChecked();
        SectReprPtr pSectRepr = (SectRepr*)pEntry->GetUserData();
        BOOL bDDE = pBox->IsChecked();
        if(bDDE)
        {
            aFileNameFT.Hide();
            aDDECommandFT.Enable();
            aDDECommandFT.Show();
            aSubRegionFT.Hide();
            aSubRegionED.Hide();
            if(FILE_LINK_SECTION == pSectRepr->GetSectionType() )
            {
                pSectRepr->SetFile(aEmptyStr);
                aFileNameED.SetText(aEmptyStr);
                pSectRepr->SetFilePasswd( aEmptyStr );
            }
            pSectRepr->SetSectionType( DDE_LINK_SECTION );
        }
        else
        {
            aDDECommandFT.Hide();
            aFileNameFT.Enable(bFile);
            aFileNameFT.Show();
            aSubRegionED.Show();
            aSubRegionFT.Show();
            aSubRegionED.Enable(bFile);
            aSubRegionFT.Enable(bFile);
            aSubRegionED.Enable(bFile);
            if(DDE_LINK_SECTION == pSectRepr->GetSectionType() )
            {
                pSectRepr->SetSectionType( FILE_LINK_SECTION );
                pSectRepr->SetFile(aEmptyStr);
                pSectRepr->SetFilePasswd( aEmptyStr );
                aFileNameED.SetText(aEmptyStr);
            }
        }
        aFilePB.Enable(bFile && !bDDE);
    }
    return 0;
}
#endif
/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, ChangePasswdHdl, Button *, pBox )
{
    sal_Bool bChange = pBox == &aPasswdPB;
    if(!CheckPasswd(0))
    {
        if(!bChange)
            aPasswdCB.Check(!aPasswdCB.IsChecked());
        return 0;
    }
    SvLBoxEntry* pEntry=aTree.FirstSelected();
    sal_Bool bSet = bChange ? bChange : aPasswdCB.IsChecked();
    DBG_ASSERT(pEntry,"kein Entry gefunden");
    while( pEntry )
    {
        SectReprPtr pRepr = (SectReprPtr)pEntry->GetUserData();
        if(bSet)
        {
            if(!pRepr->GetTempPasswd().getLength() || bChange)
            {
                SwTestPasswdDlg aPasswdDlg(this);
                aPasswdDlg.ShowExtras(SHOWEXTRAS_CONFIRM);
                if(RET_OK == aPasswdDlg.Execute())
                {
                    String sNewPasswd( aPasswdDlg.GetPassword() );
                    if( aPasswdDlg.GetConfirm() == sNewPasswd )
                    {
                        SvPasswordHelper::GetHashPassword( pRepr->GetTempPasswd(), sNewPasswd );
                    }
                    else
                    {
                        InfoBox(pBox, SW_RES(REG_WRONG_PASSWD_REPEAT)).Execute();
                        ChangePasswdHdl(pBox);
                        break;
                    }
                }
                else
                {
                    if(!bChange)
                        aPasswdCB.Check(FALSE);
                    break;
                }
            }
            pRepr->GetSection().SetPasswd(pRepr->GetTempPasswd());
        }
        else
            pRepr->GetSection().SetPasswd(UNO_NMSPC::Sequence <sal_Int8 >());
        pEntry = aTree.NextSelected(pEntry);
    }
    return 0;
}
/*---------------------------------------------------------------------
    Beschreibung:	Aktueller Bereichsname wird sofort beim editieren
                    in die TreeListBox eingetragen, mit leerem String
                    kein Ok()
---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, NameEditHdl, Edit *, EMPTYARG )
{
    if(!CheckPasswd(0))
        return 0;
    SvLBoxEntry* pEntry=aTree.FirstSelected();
    DBG_ASSERT(pEntry,"kein Entry gefunden");
    if (pEntry)
    {
        String	aName = aCurName.GetText();
        Bitmap& aBmp=BuildBitmap(STATE_CHECK == aProtectCB.GetState(),
                                    STATE_CHECK == aHideCB.GetState());
        aTree.SetEntryText(pEntry,aName);
        aTree.SetExpandedEntryBmp(pEntry,aBmp);
        aTree.SetCollapsedEntryBmp(pEntry,aBmp);
        SectReprPtr pRepr = (SectReprPtr) pEntry->GetUserData();
        pRepr->GetSection().SetName(aName);

        aOK.Enable(aName.Len() != 0);
    }
    return 0;
}
/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

IMPL_LINK( SwEditRegionDlg, ConditionEditHdl, Edit *, pEdit )
{
    Selection aSelect = pEdit->GetSelection();
    if(!CheckPasswd(0))
        return 0;
    pEdit->SetSelection(aSelect);
    SvLBoxEntry* pEntry = aTree.FirstSelected();
    DBG_ASSERT(pEntry,"kein Entry gefunden");
    while( pEntry )
    {
        SectReprPtr pRepr = (SectReprPtr)pEntry->GetUserData();
        pRepr->SetCondition (pEdit->GetText());
        pEntry = aTree.NextSelected(pEntry);
    }
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:	Bereiche einfuegen
 --------------------------------------------------------------------*/


void SwBaseShell::InsertRegionDialog(SfxRequest& rReq)
{
    SwWrtShell& rSh = GetShell();
    const SfxItemSet *pSet = rReq.GetArgs();

    SfxItemSet aSet(GetPool(),
            RES_COL, RES_COL,
            RES_COLUMNBALANCE, RES_COLUMNBALANCE,
            RES_BACKGROUND, RES_BACKGROUND,
            RES_FRM_SIZE, RES_FRM_SIZE,
            RES_FTN_AT_TXTEND, RES_END_AT_TXTEND,
            SID_ATTR_PAGE_SIZE, SID_ATTR_PAGE_SIZE,
            0);

    if (!pSet || pSet->Count()==0)
    {
        SwRect aRect;
        rSh.CalcBoundRect(aRect, FLY_IN_CNTNT);

        long nWidth = aRect.Width();
        aSet.Put(SwFmtFrmSize(ATT_VAR_SIZE, nWidth));

        // Hoehe=Breite fuer konsistentere Vorschau (analog zu Bereich bearbeiten)
        aSet.Put(SvxSizeItem(SID_ATTR_PAGE_SIZE, Size(nWidth, nWidth)));
        SwInsertSectionTabDialog aTabDlg(&GetView().GetViewFrame()->GetWindow(),aSet , rSh);
        aTabDlg.Execute();
    }
    else
    {
        const SfxPoolItem *pItem = 0;
        String aTmpStr = rSh.GetUniqueSectionName();

        SwSection	aSection(CONTENT_SECTION,aTmpStr);
        rReq.SetReturnValue(SfxStringItem(FN_INSERT_REGION, aTmpStr));

        aSet.Put( *pSet );
        if(SFX_ITEM_SET == pSet->GetItemState(SID_ATTR_COLUMNS, FALSE, &pItem))
        {
            SwFmtCol aCol;
            SwRect aRect;
            rSh.CalcBoundRect(aRect, FLY_IN_CNTNT);
            long nWidth = aRect.Width();

            USHORT nCol = ((SfxUInt16Item *)pItem)->GetValue();
            if(nCol)
            {
                aCol.Init( nCol, 0, nWidth );
                aSet.Put(aCol);
            }
        }
        else if(SFX_ITEM_SET == pSet->GetItemState(RES_COL, FALSE, &pItem))
        {
            aSet.Put(*pItem);
        }
        rSh.InsertSection(aSection, aSet.Count() ? &aSet : 0);
    }
}

IMPL_STATIC_LINK( SwWrtShell, InsertRegionDialog, SwSection*, pSect )
{
    if( pSect )
    {
        SfxItemSet aSet(pThis->GetView().GetPool(),
                RES_COL, RES_COL,
                RES_BACKGROUND, RES_BACKGROUND,
                RES_FRM_SIZE, RES_FRM_SIZE,
                SID_ATTR_PAGE_SIZE, SID_ATTR_PAGE_SIZE,
                0);
        SwRect aRect;
        pThis->CalcBoundRect(aRect, FLY_IN_CNTNT);
        long nWidth = aRect.Width();
        aSet.Put(SwFmtFrmSize(ATT_VAR_SIZE, nWidth));
        // Hoehe=Breite fuer konsistentere Vorschau (analog zu Bereich bearbeiten)
        aSet.Put(SvxSizeItem(SID_ATTR_PAGE_SIZE, Size(nWidth, nWidth)));
        SwInsertSectionTabDialog aTabDlg(&pThis->GetView().GetViewFrame()->GetWindow(),aSet , *pThis);
        aTabDlg.SetSection(*pSect);
        aTabDlg.Execute();

        delete pSect;
    }
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung:	Bereich bearbeiten
 --------------------------------------------------------------------*/

void SwBaseShell::EditRegionDialog(SfxRequest& rReq)
{
    const SfxItemSet* pArgs = rReq.GetArgs();
    int nSlot = rReq.GetSlot();
    const SfxPoolItem* pItem = 0;
    if(pArgs)
        pArgs->GetItemState(nSlot, FALSE, &pItem);
    SwWrtShell& rWrtShell = GetShell();

    switch ( nSlot )
    {
        case FN_EDIT_REGION:
        {
            Window* pParentWin = &GetView().GetViewFrame()->GetWindow();
            BOOL bStart = TRUE;
            if(bStart)
            {
                SwEditRegionDlg* pEditRegionDlg = new SwEditRegionDlg(
                                        pParentWin, rWrtShell );
                if(pItem && pItem->ISA(SfxStringItem))
                {
                    pEditRegionDlg->SelectSection(((const SfxStringItem*)pItem)->GetValue());
                }
                pEditRegionDlg->Execute();
                delete pEditRegionDlg;
            }
            else
                InfoBox(pParentWin, SW_RES(REG_WRONG_PASSWORD)).Execute();
        }
        break;
    }
}

/*--------------------------------------------------------------------
    Beschreibung:	Hilfsfunktion - Bereichsindex ermitteln
 --------------------------------------------------------------------*/

USHORT lcl_GetRegion( const String& rRegionName, SwWrtShell& rWrtShell )
{
    USHORT nCount = rWrtShell.GetSectionFmtCount();
    for(USHORT i=0; i< nCount; i++)
    {
        const SwSectionFmt& rFmt = rWrtShell.GetSectionFmt(i);
        if(rFmt.IsInNodesArr() && rFmt.GetSection()->GetName()
                == rRegionName)
            return i;
    }
    return USHRT_MAX;
}

/*--------------------------------------------------------------------
    Beschreibung:	Hilfsfunktion - Bereichsnamen aus dem Medium lesen
 --------------------------------------------------------------------*/

void lcl_ReadSections( SwWrtShell& rSh, SfxMedium& rMedium, ComboBox& rBox )
{
    rBox.Clear();
    SvStorage* pStg;
    if( rMedium.IsStorage() && 0 != (pStg = rMedium.GetStorage() ) )
    {
        SvStringsDtor aArr( 10, 10 );
        switch( pStg->GetFormat() )
        {
        case SOT_FORMATSTR_ID_STARWRITER_50:
        case SOT_FORMATSTR_ID_STARWRITER_40:
        case SOT_FORMATSTR_ID_STARWRITER_30:
        case SOT_FORMATSTR_ID_STARWRITERGLOB_50:
        case SOT_FORMATSTR_ID_STARWRITERGLOB_40:
            {
                Sw3Reader* pRdr = (Sw3Reader*)ReadSw3;
                Sw3Io* pOldIo = pRdr->GetSw3Io();
                  pRdr->SetSw3Io( rSh.GetView().GetDocShell()->GetIoSystem() );
                pRdr->GetSectionList( rMedium, (SvStrings&) aArr );
                  pRdr->SetSw3Io( pOldIo );
            }
            break;
        case SOT_FORMATSTR_ID_STARWRITER_60:
        case SOT_FORMATSTR_ID_STARWRITERGLOB_60:
            ReadXML->GetSectionList( rMedium, (SvStrings&) aArr );
            break;
        }
        for( USHORT n = 0; n < aArr.Count(); ++n )
            rBox.InsertEntry( *aArr[ n ] );
    }
}
/* -----------------21.05.99 10:16-------------------
 *
 * --------------------------------------------------*/
SwInsertSectionTabDialog::SwInsertSectionTabDialog(
            Window* pParent, const SfxItemSet& rSet, SwWrtShell& rSh) :
    SfxTabDialog( pParent, SW_RES(DLG_INSERT_SECTION), &rSet ),
    rWrtSh(rSh),
    pToInsertSection(0)
{
    String sInsert(ResId(ST_INSERT));
    GetOKButton().SetText(sInsert);
    FreeResource();
    AddTabPage(TP_INSERT_SECTION, SwInsertSectionTabPage::Create, 0);
    AddTabPage(TP_COLUMN,	SwColumnPage::Create,  	 0);
    AddTabPage(TP_BACKGROUND,SvxBackgroundTabPage::Create, 	0);
    AddTabPage(TP_SECTION_FTNENDNOTES, SwSectionFtnEndTabPage::Create, 0);

    OfaHtmlOptions* pHtmlOpt = OFF_APP()->GetHtmlOptions();
    long nHtmlMode = pHtmlOpt->GetExportMode();

    BOOL bWeb = 0 != PTR_CAST( SwWebDocShell, rSh.GetView().GetDocShell() );
    if(bWeb)
    {
        RemoveTabPage(TP_SECTION_FTNENDNOTES);
        if( HTML_CFG_NS30 != nHtmlMode && HTML_CFG_NS40 != nHtmlMode && HTML_CFG_WRITER != nHtmlMode)
            RemoveTabPage(TP_COLUMN);
    }
    SetCurPageId(TP_INSERT_SECTION);
}
/* -----------------21.05.99 10:17-------------------
 *
 * --------------------------------------------------*/
SwInsertSectionTabDialog::~SwInsertSectionTabDialog()
{
    delete pToInsertSection;
}
/* -----------------21.05.99 10:23-------------------
 *
 * --------------------------------------------------*/
void SwInsertSectionTabDialog::PageCreated( USHORT nId, SfxTabPage &rPage )
{
    if(TP_INSERT_SECTION == nId)
        ((SwInsertSectionTabPage&)rPage).SetWrtShell(rWrtSh);
    else if( TP_BACKGROUND == nId  )
        ((SvxBackgroundTabPage&)rPage).ShowSelector();
    else if( TP_COLUMN == nId )
    {
        const SwFmtFrmSize& rSize = (const SwFmtFrmSize&)GetInputSetImpl()->Get(RES_FRM_SIZE);
        ((SwColumnPage&)rPage).SetPageWidth(rSize.GetWidth());
        ((SwColumnPage&)rPage).ShowBalance(TRUE);
    }
}
/* -----------------21.05.99 13:08-------------------
 *
 * --------------------------------------------------*/

void SwInsertSectionTabDialog::SetSection(const SwSection& rSect)
{
    pToInsertSection = new SwSection(CONTENT_SECTION, aEmptyStr);
    *pToInsertSection = rSect;
}
/* -----------------21.05.99 13:10-------------------
 *
 * --------------------------------------------------*/
short	SwInsertSectionTabDialog::Ok()
{
    short nRet = SfxTabDialog::Ok();
    DBG_ASSERT(pToInsertSection, "keiner Section?")
    rWrtSh.InsertSection(*pToInsertSection, GetOutputItemSet());
    return nRet;
}

/* -----------------21.05.99 10:31-------------------
 *
 * --------------------------------------------------*/
SwInsertSectionTabPage::SwInsertSectionTabPage(
                            Window *pParent, const SfxItemSet &rAttrSet) :
    SfxTabPage( pParent, SW_RES(TP_INSERT_SECTION), rAttrSet ),
    aNameFL       ( this, SW_RES( FL_NAME ) ),
    aCurName            ( this, SW_RES( ED_RNAME ) ),
    aLinkFL             ( this, SW_RES( FL_LINK ) ),
    aFileCB             ( this, SW_RES( CB_FILE ) ),
#ifdef DDE_AVAILABLE
    aDDECB              ( this, SW_RES( CB_DDE ) ) ,
    aDDECommandFT       ( this, SW_RES( FT_DDE ) ) ,
#endif
    aFileNameFT         ( this, SW_RES( FT_FILE ) ) ,
    aFileNameED         ( this, SW_RES( ED_FILE ) ),
    aFilePB             ( this, SW_RES( PB_FILE ) ),
    aSubRegionFT        ( this, SW_RES( FT_SUBREG ) ) ,
    aSubRegionED        ( this, SW_RES( LB_SUBREG ) ) ,

    aProtectFL          ( this, SW_RES( FL_PROTECT ) ),
    aProtectCB          ( this, SW_RES( CB_PROTECT ) ),
    aPasswdCB           ( this, SW_RES( CB_PASSWD ) ),
    aPasswdPB           ( this, SW_RES( PB_PASSWD ) ),

    aHideFL             ( this, SW_RES( FL_HIDE ) ),
    aHideCB             ( this, SW_RES( CB_HIDE ) ),
    aConditionFT             ( this, SW_RES( FT_CONDITION ) ),
    aConditionED        ( this, SW_RES( ED_CONDITION ) ),

    sSection            (SW_RES( STR_REGION_DEFNAME )),
    pWrtSh(0)
{
    FreeResource();

    aProtectCB.SetClickHdl	( LINK( this, SwInsertSectionTabPage, ChangeProtectHdl));
    aPasswdCB.SetClickHdl   ( LINK( this, SwInsertSectionTabPage, ChangePasswdHdl));
    aPasswdPB.SetClickHdl   ( LINK( this, SwInsertSectionTabPage, ChangePasswdHdl));
    aHideCB.SetClickHdl     ( LINK( this, SwInsertSectionTabPage, ChangeHideHdl));
    aFileCB.SetClickHdl     ( LINK( this, SwInsertSectionTabPage, UseFileHdl ));
    aFilePB.SetClickHdl     ( LINK( this, SwInsertSectionTabPage, FileSearchHdl ));
    aCurName.SetModifyHdl	( LINK( this, SwInsertSectionTabPage, NameEditHdl));
#ifdef DDE_AVAILABLE
    aDDECB.SetClickHdl		( LINK( this, SwInsertSectionTabPage, DDEHdl ));
#endif
    ChangeProtectHdl(&aProtectCB);
}
/* -----------------21.05.99 10:31-------------------
 *
 * --------------------------------------------------*/
SwInsertSectionTabPage::~SwInsertSectionTabPage()
{
}
/* -----------------21.05.99 12:58-------------------
 *
 * --------------------------------------------------*/
void	SwInsertSectionTabPage::SetWrtShell(SwWrtShell& rSh)
{
    pWrtSh = &rSh;

    BOOL bWeb = 0 != PTR_CAST(SwWebDocShell, pWrtSh->GetView().GetDocShell());
    if(bWeb)
    {
        aHideCB    		.Hide();
        aConditionED    .Hide();
        aConditionFT    .Hide();
#ifdef DDE_AVAILABLE
        aDDECB           .Hide();
        aDDECommandFT    .Hide();
#endif
    }

    USHORT nCount = pWrtSh->GetSectionFmtCount();
    FillList();
    USHORT nCnt = pWrtSh->GetBookmarkCnt();
    for( USHORT i = 0; i < nCnt; ++i )
    {
        SwBookmark& rBm = pWrtSh->GetBookmark( i );
        if( rBm.GetOtherPos() )
            aSubRegionED.InsertEntry( rBm.GetName() );
    }

    SwSection* pSect = ((SwInsertSectionTabDialog*)GetTabDialog())->GetSection();
    if( pSect )		// etwas vorgegeben ?
    {
        aCurName.SetText( rSh.GetUniqueSectionName( &pSect->GetName() ));
        aProtectCB.Check( 0 != pSect->IsProtect() );
        sFileName = pSect->GetLinkFileName();
        sFilePasswd = pSect->GetLinkFilePassWd();
        aFileCB.Check( 0 != sFileName.Len() );
        aFileNameED.SetText( sFileName );
        UseFileHdl( &aFileCB );
    }
    else
    {
        aCurName.SetText( rSh.GetUniqueSectionName() );
    }
}
/* -----------------21.05.99 10:32-------------------
 *
 * --------------------------------------------------*/
BOOL SwInsertSectionTabPage::FillItemSet( SfxItemSet& rSet)
{
    SwSection aSection(CONTENT_SECTION, aCurName.GetText());
    aSection.SetCondition(aConditionED.GetText());
    BOOL bProtected = aProtectCB.IsChecked();
    aSection.SetProtect(bProtected);
    aSection.SetHidden(aHideCB.IsChecked());
    if(bProtected)
        aSection.SetPasswd(aNewPasswd);
    String sFileName = aFileNameED.GetText();
    String sSubRegion = aSubRegionED.GetText();
    BOOL bDDe = FALSE;
#ifdef DDE_AVAILABLE
    bDDe = aDDECB.IsChecked();
#endif
    if(aFileCB.IsChecked() && (sFileName.Len() || sSubRegion.Len() || bDDe))
    {
        String aLinkFile;
        if( bDDe )
        {
            aLinkFile = sFileName;

            USHORT nPos = 0;
            while( STRING_NOTFOUND != (nPos = aLinkFile.SearchAscii( "  ", nPos )) )
                aLinkFile.Erase( nPos--, 1 );

            nPos = aLinkFile.SearchAndReplace( ' ', so3::cTokenSeperator );
            aLinkFile.SearchAndReplace( ' ', so3::cTokenSeperator, nPos );
        }
        else
        {
            if(sFileName.Len())
            {
                aLinkFile = URIHelper::SmartRelToAbs( sFileName );
                aSection.SetLinkFilePassWd( sFilePasswd );
            }

            aLinkFile += so3::cTokenSeperator;
            aLinkFile += sFilterName;
            aLinkFile += so3::cTokenSeperator;
            aLinkFile += sSubRegion;
        }

        aSection.SetLinkFileName(aLinkFile);
        if(aLinkFile.Len())
        {
#ifdef DDE_AVAILABLE
            aSection.SetType( aDDECB.IsChecked() ?
                                    DDE_LINK_SECTION :
                                        FILE_LINK_SECTION);
#else
            aSection.SetType( FILE_LINK_SECTION);
#endif
        }
    }
    ((SwInsertSectionTabDialog*)GetTabDialog())->SetSection(aSection);
    return TRUE;
}
/* -----------------21.05.99 10:32-------------------
 *
 * --------------------------------------------------*/
void SwInsertSectionTabPage::Reset( const SfxItemSet& )
{
}
/* -----------------21.05.99 11:22-------------------
 *
 * --------------------------------------------------*/
SfxTabPage*	SwInsertSectionTabPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet)
{
    return new SwInsertSectionTabPage(pParent, rAttrSet);
}
/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

IMPL_LINK( SwInsertSectionTabPage, ChangeHideHdl, CheckBox *, pBox )
{
    BOOL bHide = pBox->IsChecked();
    aConditionED.Enable(bHide);
    aConditionFT.Enable(bHide);
    return 0;
}
/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

IMPL_LINK( SwInsertSectionTabPage, ChangeProtectHdl, CheckBox *, pBox )
{
    sal_Bool bCheck = pBox->IsChecked();
    aPasswdCB.Enable(bCheck);
    aPasswdPB.Enable(bCheck);
    return 0;
}
/* -----------------------------26.04.01 14:50--------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertSectionTabPage, ChangePasswdHdl, Button *, pButton )
{
    sal_Bool bChange = pButton == &aPasswdPB;
    sal_Bool bSet = bChange ? bChange : aPasswdCB.IsChecked();
    if(bSet)
    {
        if(!aNewPasswd.getLength() || bChange)
        {
            SwTestPasswdDlg aPasswdDlg(this);
            aPasswdDlg.ShowExtras(SHOWEXTRAS_CONFIRM);
            if(RET_OK == aPasswdDlg.Execute())
            {
                String sNewPasswd( aPasswdDlg.GetPassword() );
                if( aPasswdDlg.GetConfirm() == sNewPasswd )
                {
                    SvPasswordHelper::GetHashPassword( aNewPasswd, sNewPasswd );
                }
                else
                {
                    InfoBox(pButton, SW_RES(REG_WRONG_PASSWD_REPEAT)).Execute();
                }
            }
            else if(!bChange)
                aPasswdCB.Check(FALSE);
        }
    }
    else
        aNewPasswd.realloc(0);
    return 0;
}
/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

IMPL_LINK_INLINE_START( SwInsertSectionTabPage, NameEditHdl, Edit *, EMPTYARG )
{
    String	aName=aCurName.GetText();
    GetTabDialog()->GetOKButton().Enable(aName.Len() && aCurName.GetEntryPos( aName ) == USHRT_MAX);
    return 0;
}
IMPL_LINK_INLINE_END( SwInsertSectionTabPage, NameEditHdl, Edit *, EMPTYARG )

/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

IMPL_LINK( SwInsertSectionTabPage, UseFileHdl, CheckBox *, pBox )
{
    if( pBox->IsChecked() )
    {
        if( pWrtSh->HasSelection() &&
            RET_NO == QueryBox( this, SW_RES(QB_CONNECT) ).Execute() )
            pBox->Check( FALSE );
    }

    BOOL bFile = pBox->IsChecked();
    aFileNameFT.Enable(bFile);
    aFileNameED.Enable(bFile);
    aFilePB.Enable(bFile);
    aSubRegionFT.Enable(bFile);
    aSubRegionED.Enable(bFile);
#ifdef DDE_AVAILABLE
    aDDECommandFT.Enable(bFile);
    aDDECB.Enable(bFile);
#endif
    if( bFile )
    {
//		aFileNameED.SetText( aFileName );
        aFileNameED.GrabFocus();
        aProtectCB.Check( TRUE );
    }
    else
    {
#ifdef DDE_AVAILABLE
        aDDECB.Check(FALSE);
        DDEHdl(&aDDECB);
#endif
//		aFileNameED.SetText(aEmptyStr);
    }
    return 0;
}

/*---------------------------------------------------------------------

---------------------------------------------------------------------*/

IMPL_LINK( SwInsertSectionTabPage, FileSearchHdl, PushButton *, EMPTYARG )
{
    SfxMedium* pMed;
    if( GetFileFilterNameDlg( *this, sFileName, &sFilePasswd,
                                &sFilterName, &pMed ))
    {
        aFileNameED.SetText( INetURLObject::decode( sFileName, INET_HEX_ESCAPE,
                                           INetURLObject::DECODE_UNAMBIGUOUS,
                                        RTL_TEXTENCODING_UTF8 ));

        ::lcl_ReadSections( *pWrtSh, *pMed, aSubRegionED );
        delete pMed;		// das brauchen wir nicht mehr !
    }
    else
        sFilterName = sFilePasswd = aEmptyStr;
    return 0;
}

/*---------------------------------------------------------------------

---------------------------------------------------------------------*/
#ifdef DDE_AVAILABLE

IMPL_LINK( SwInsertSectionTabPage, DDEHdl, CheckBox*, pBox )
{
    BOOL bDDE = pBox->IsChecked();
    BOOL bFile = aFileCB.IsChecked();
    aFilePB.Enable(!bDDE && bFile);
    if(bDDE)
    {
        aFileNameFT.Hide();
        aDDECommandFT.Enable(bDDE);
        aDDECommandFT.Show();
        aSubRegionFT.Hide();
        aSubRegionED.Hide();
    }
    else
    {
        aDDECommandFT.Hide();
        aFileNameFT.Enable(bFile);
        aFileNameFT.Show();
        aSubRegionFT.Show();
        aSubRegionED.Show();
        aSubRegionED.Enable(bFile);
    }
    return 0;
}
#endif
/*--------------------------------------------------------------------
    Beschreibung:	Liste der verwendeten Namen fuellen
 --------------------------------------------------------------------*/

void SwInsertSectionTabPage::FillList(  const SwSectionFmt* pNewFmt )
{
    const SwSectionFmt* pFmt;
    if( !pNewFmt )
    {
        USHORT nCount = pWrtSh->GetSectionFmtCount();
        for(USHORT i=0;i<nCount;i++)
        {
            SectionType eTmpType;
            if( !(pFmt = &pWrtSh->GetSectionFmt(i))->GetParent() &&
                    pFmt->IsInNodesArr() &&
                    (eTmpType = pFmt->GetSection()->GetType()) != TOX_CONTENT_SECTION
                    && TOX_HEADER_SECTION != eTmpType )
            {
                    String* pString = new String(pFmt->GetSection()->GetName());
                    aCurName.InsertEntry(*pString);
                    aSubRegionED.InsertEntry(*pString);
                    FillList( pFmt );
            }
        }
    }
    else
    {
        SwSections aTmpArr;
        USHORT nCnt = pNewFmt->GetChildSections(aTmpArr,SORTSECT_POS);
        if( nCnt )
        {
            SectionType eTmpType;
            for( USHORT n = 0; n < nCnt; ++n )
                if( (pFmt = aTmpArr[n]->GetFmt())->IsInNodesArr()&&
                    (eTmpType = pFmt->GetSection()->GetType()) != TOX_CONTENT_SECTION
                    && TOX_HEADER_SECTION != eTmpType )
                {
                    String* pString = new String(pFmt->GetSection()->GetName());
                    aCurName.InsertEntry(*pString);
                    aSubRegionED.InsertEntry(*pString);
                    FillList( pFmt );
                }
        }
    }
}

// --------------------------------------------------------------

// Numerierungsformat Umsetzung:
// ListBox	- Format			- Enum-Wert
// 0 		- A, B, C, ...		- 0
// 1		- a, b, c, ...		- 1
// 2		- I, II, III, ... 	- 2
// 3        - i, ii, iii, ...	- 3
// 4        - 1, 2, 3, ...		- 4
// 5        - A, .., AA, .., 	- 9
// 6        - a, .., aa, .., 	- 10

inline USHORT GetNumPos( USHORT n )
{
    return SVX_NUM_ARABIC < n ? n - 4 : n;
}

inline SvxExtNumType GetNumType( USHORT n )
{
    return (SvxExtNumType)(4 < n ? n + 4 : n );
}

SwSectionFtnEndTabPage::SwSectionFtnEndTabPage( Window *pParent,
                                                const SfxItemSet &rAttrSet)
    : SfxTabPage( pParent, SW_RES( TP_SECTION_FTNENDNOTES ), rAttrSet ),
    aFtnFL              ( this, SW_RES( FL_FTN ) ),
    aFtnNtAtTextEndCB   ( this, SW_RES( CB_FTN_AT_TXTEND ) ),
    aFtnNtNumCB   		( this, SW_RES( CB_FTN_NUM ) ),
    aFtnNtNumFmtCB		( this, SW_RES( CB_FTN_NUM_FMT ) ),
    aFtnNumViewBox		( this, SW_RES( LB_FTN_NUMVIEW	), INSERT_NUM_EXTENDED_TYPES),
    aFtnOffsetLbl		( this, SW_RES( FT_FTN_OFFSET	)),
    aFtnOffsetFld		( this, SW_RES( FLD_FTN_OFFSET   )),
    aFtnPrefixFT		( this, SW_RES( FT_FTN_PREFIX  	)),
    aFtnPrefixED		( this, SW_RES( ED_FTN_PREFIX    )),
    aFtnSuffixFT		( this, SW_RES( FT_FTN_SUFFIX    )),
    aFtnSuffixED		( this, SW_RES( ED_FTN_SUFFIX    )),
    aEndFL              ( this, SW_RES( FL_END ) ),
    aEndNtAtTextEndCB   ( this, SW_RES( CB_END_AT_TXTEND )),
    aEndNtNumCB   		( this, SW_RES( CB_END_NUM )),
    aEndNtNumFmtCB		( this, SW_RES( CB_END_NUM_FMT ) ),
    aEndNumViewBox		( this, SW_RES( LB_END_NUMVIEW	)),
    aEndOffsetLbl		( this, SW_RES( FT_END_OFFSET	)),
    aEndOffsetFld		( this, SW_RES( FLD_END_OFFSET   )),
    aEndPrefixFT		( this, SW_RES( FT_END_PREFIX  	)),
    aEndPrefixED		( this, SW_RES( ED_END_PREFIX    )),
    aEndSuffixFT		( this, SW_RES( FT_END_SUFFIX    )),
    aEndSuffixED		( this, SW_RES( ED_END_SUFFIX    ))
{
    FreeResource();

    Link aLk( LINK( this, SwSectionFtnEndTabPage, FootEndHdl));
    aFtnNtAtTextEndCB.SetClickHdl( aLk );
    aFtnNtNumCB.SetClickHdl( aLk );
    aEndNtAtTextEndCB.SetClickHdl( aLk );
    aEndNtNumCB.SetClickHdl( aLk );
    aFtnNtNumFmtCB.SetClickHdl( aLk );
    aEndNtNumFmtCB.SetClickHdl( aLk );
}

SwSectionFtnEndTabPage::~SwSectionFtnEndTabPage()
{
}

BOOL SwSectionFtnEndTabPage::FillItemSet( SfxItemSet& rSet )
{
    BOOL bRecording = FALSE;//pRequest && 0 != SfxRequest::GetRecordingMacro();

    SwFmtFtnAtTxtEnd aFtn( aFtnNtAtTextEndCB.IsChecked()
                            ? ( aFtnNtNumCB.IsChecked()
                                ? ( aFtnNtNumFmtCB.IsChecked()
                                    ? FTNEND_ATTXTEND_OWNNUMANDFMT
                                    : FTNEND_ATTXTEND_OWNNUMSEQ )
                                : FTNEND_ATTXTEND )
                            : FTNEND_ATPGORDOCEND );

    switch( aFtn.GetValue() )
    {
    case FTNEND_ATTXTEND_OWNNUMANDFMT:
        aFtn.SetNumType( aFtnNumViewBox.GetSelectedNumberingType() );
        aFtn.SetPrefix( aFtnPrefixED.GetText() );
        aFtn.SetSuffix( aFtnSuffixED.GetText() );
        // no break;

    case FTNEND_ATTXTEND_OWNNUMSEQ:
        aFtn.SetOffset( aFtnOffsetFld.GetValue()-1 );
        // no break;
    }

    SwFmtEndAtTxtEnd aEnd( aEndNtAtTextEndCB.IsChecked()
                            ? ( aEndNtNumCB.IsChecked()
                                ? ( aEndNtNumFmtCB.IsChecked()
                                    ? FTNEND_ATTXTEND_OWNNUMANDFMT
                                    : FTNEND_ATTXTEND_OWNNUMSEQ )
                                : FTNEND_ATTXTEND )
                            : FTNEND_ATPGORDOCEND );

    switch( aEnd.GetValue() )
    {
    case FTNEND_ATTXTEND_OWNNUMANDFMT:
        aEnd.SetNumType( GetNumType( aEndNumViewBox.GetSelectEntryPos() ));
        aEnd.SetPrefix( aEndPrefixED.GetText() );
        aEnd.SetSuffix( aEndSuffixED.GetText() );
        // no break;

    case FTNEND_ATTXTEND_OWNNUMSEQ:
        aEnd.SetOffset( aEndOffsetFld.GetValue()-1 );
        // no break;
    }

    rSet.Put( aFtn );
    rSet.Put( aEnd );

    return TRUE;
}

void SwSectionFtnEndTabPage::ResetState( BOOL bFtn,
                                    const SwFmtFtnEndAtTxtEnd& rAttr )
{
    CheckBox *pNtAtTextEndCB, *pNtNumCB, *pNtNumFmtCB;
    FixedText*pPrefixFT, *pSuffixFT;
    Edit *pPrefixED, *pSuffixED;
    SwNumberingTypeListBox *pNumViewBox;
    FixedText* pOffsetTxt;
    NumericField *pOffsetFld;

    if( bFtn )
    {
        pNtAtTextEndCB = &aFtnNtAtTextEndCB;
        pNtNumCB = &aFtnNtNumCB;
        pNtNumFmtCB = &aFtnNtNumFmtCB;
        pPrefixFT = &aFtnPrefixFT;
        pPrefixED = &aFtnPrefixED;
        pSuffixFT = &aFtnSuffixFT;
        pSuffixED = &aFtnSuffixED;
        pNumViewBox = &aFtnNumViewBox;
        pOffsetTxt = &aFtnOffsetLbl;
        pOffsetFld = &aFtnOffsetFld;
    }
    else
    {
        pNtAtTextEndCB = &aEndNtAtTextEndCB;
        pNtNumCB = &aEndNtNumCB;
        pNtNumFmtCB = &aEndNtNumFmtCB;
        pPrefixFT = &aEndPrefixFT;
        pPrefixED = &aEndPrefixED;
        pSuffixFT = &aEndSuffixFT;
        pSuffixED = &aEndSuffixED;
        pNumViewBox = &aEndNumViewBox;
        pOffsetTxt = &aEndOffsetLbl;
        pOffsetFld = &aEndOffsetFld;
    }

    USHORT eState = rAttr.GetValue();
    switch( eState )
    {
    // case FTNEND_ATPGORDOCEND:
    case FTNEND_ATTXTEND_OWNNUMANDFMT:
        pNtNumFmtCB->SetState( STATE_CHECK );
        // no break;

    case FTNEND_ATTXTEND_OWNNUMSEQ:
        pNtNumCB->SetState( STATE_CHECK );
        // no break;

    case FTNEND_ATTXTEND:
        pNtAtTextEndCB->SetState( STATE_CHECK );
        // no break;
    }

    pNumViewBox->SelectNumberingType( rAttr.GetNumType() );
    pOffsetFld->SetValue( rAttr.GetOffset() + 1 );
    pPrefixED->SetText( rAttr.GetPrefix() );
    pSuffixED->SetText( rAttr.GetSuffix() );

    switch( eState )
    {
    case FTNEND_ATPGORDOCEND:
        pNtNumCB->Enable( FALSE );
        // no break;

    case FTNEND_ATTXTEND:
        pNtNumFmtCB->Enable( FALSE );
        pOffsetFld->Enable( FALSE );
        pOffsetTxt->Enable( FALSE );
        // no break;

    case FTNEND_ATTXTEND_OWNNUMSEQ:
        pNumViewBox->Enable( FALSE );
        pPrefixFT->Enable( FALSE );
        pPrefixED->Enable( FALSE );
        pSuffixFT->Enable( FALSE );
        pSuffixED->Enable( FALSE );
        // no break;
    }
}

void SwSectionFtnEndTabPage::Reset( const SfxItemSet& rSet )
{
    ResetState( TRUE, (const SwFmtFtnAtTxtEnd&)rSet.Get(
                                    RES_FTN_AT_TXTEND, FALSE ));
    ResetState( FALSE, (const SwFmtEndAtTxtEnd&)rSet.Get(
                                    RES_END_AT_TXTEND, FALSE ));
}

SfxTabPage*	SwSectionFtnEndTabPage::Create( Window* pParent,
                                const SfxItemSet& rAttrSet)
{
    return new SwSectionFtnEndTabPage(pParent, rAttrSet);
}

IMPL_LINK( SwSectionFtnEndTabPage, FootEndHdl, CheckBox *, pBox )
{
//	pBox->EnableTriState( FALSE );
    BOOL bFoot = &aFtnNtAtTextEndCB == pBox || &aFtnNtNumCB == pBox ||
                    &aFtnNtNumFmtCB == pBox ;

    CheckBox *pNumBox, *pNumFmtBox, *pEndBox;
    SwNumberingTypeListBox* pNumViewBox;
    FixedText* pOffsetTxt;
    NumericField *pOffsetFld;
    FixedText*pPrefixFT, *pSuffixFT;
    Edit *pPrefixED, *pSuffixED;

    if( bFoot )
    {
        pEndBox = &aFtnNtAtTextEndCB;
        pNumBox = &aFtnNtNumCB;
        pNumFmtBox = &aFtnNtNumFmtCB;
        pNumViewBox = &aFtnNumViewBox;
        pOffsetTxt = &aFtnOffsetLbl;
        pOffsetFld = &aFtnOffsetFld;
        pPrefixFT = &aFtnPrefixFT;
        pSuffixFT = &aFtnSuffixFT;
        pPrefixED = &aFtnPrefixED;
        pSuffixED = &aFtnSuffixED;
    }
    else
    {
        pEndBox = &aEndNtAtTextEndCB;
        pNumBox = &aEndNtNumCB;
        pNumFmtBox = &aEndNtNumFmtCB;
        pNumViewBox = &aEndNumViewBox;
        pOffsetTxt = &aEndOffsetLbl;
        pOffsetFld = &aEndOffsetFld;
        pPrefixFT = &aEndPrefixFT;
        pSuffixFT = &aEndSuffixFT;
        pPrefixED = &aEndPrefixED;
        pSuffixED = &aEndSuffixED;
    }

    BOOL bEnableAtEnd = STATE_CHECK == pEndBox->GetState();
    BOOL bEnableNum = bEnableAtEnd && STATE_CHECK == pNumBox->GetState();
    BOOL bEnableNumFmt = bEnableNum && STATE_CHECK == pNumFmtBox->GetState();

    pNumBox->Enable( bEnableAtEnd );
    pOffsetTxt->Enable( bEnableNum );
    pOffsetFld->Enable( bEnableNum );
    pNumFmtBox->Enable( bEnableNum );
    pNumViewBox->Enable( bEnableNumFmt );
    pPrefixED->Enable( bEnableNumFmt );
    pSuffixED->Enable( bEnableNumFmt );
    pPrefixFT->Enable( bEnableNumFmt );
    pSuffixFT->Enable( bEnableNumFmt );

    return 0;
}

/* -----------------21.05.99 13:59-------------------
 *
 * --------------------------------------------------*/
SwSectionPropertyTabDialog::SwSectionPropertyTabDialog(
    Window* pParent, const SfxItemSet& rSet, SwWrtShell& rSh) :
    SfxTabDialog(pParent, SW_RES(DLG_SECTION_PROPERTIES), &rSet)
{
    FreeResource();

    AddTabPage(TP_COLUMN,	SwColumnPage::Create,  	 0);
    AddTabPage(TP_BACKGROUND,SvxBackgroundTabPage::Create, 	0);
    AddTabPage(TP_SECTION_FTNENDNOTES, SwSectionFtnEndTabPage::Create, 0);

    OfaHtmlOptions* pHtmlOpt = OFF_APP()->GetHtmlOptions();
    long nHtmlMode = pHtmlOpt->GetExportMode();
    BOOL bWeb = 0 != PTR_CAST( SwWebDocShell, rSh.GetView().GetDocShell() );
    if(bWeb)
    {
        RemoveTabPage(TP_SECTION_FTNENDNOTES);
        if( HTML_CFG_NS30 != nHtmlMode && HTML_CFG_NS40 != nHtmlMode && HTML_CFG_WRITER != nHtmlMode)
            RemoveTabPage(TP_COLUMN);
    }
}
/* -----------------21.05.99 13:59-------------------
 *
 * --------------------------------------------------*/
SwSectionPropertyTabDialog::~SwSectionPropertyTabDialog()
{
}
/* -----------------21.05.99 13:59-------------------
 *
 * --------------------------------------------------*/
void SwSectionPropertyTabDialog::PageCreated( USHORT nId, SfxTabPage &rPage )
{
    if( TP_BACKGROUND == nId  )
        ((SvxBackgroundTabPage&)rPage).ShowSelector();
    else if( TP_COLUMN == nId )
        ((SwColumnPage&)rPage).ShowBalance(TRUE);
}


