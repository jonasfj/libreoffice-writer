/*************************************************************************
 *
 *  $RCSfile: tautofmt.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: vg $ $Date: 2003-04-17 15:47:54 $
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


#pragma hdrstop


#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _ZFORLIST_HXX //autogen
#include <svtools/zforlist.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _SVTOOLS_SCRIPTEDTEXT_HXX
#include <svtools/scriptedtext.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX
#include <svtools/accessibilityoptions.hxx>
#endif

#ifndef _SWMODULE_HXX
#include "swmodule.hxx"
#endif
#ifndef _UIPARAM_HXX
#include "uiparam.hxx"
#endif
#ifndef _SWTYPES_HXX
#include "swtypes.hxx"
#endif
#ifndef _VIEW_HXX
#include "view.hxx"
#endif
#ifndef _WRTSH_HXX
#include "wrtsh.hxx"
#endif
#ifndef _TBLAFMT_HXX
#include "tblafmt.hxx"
#endif
#ifndef _TAUTOFMT_HXX
#include "tautofmt.hxx"
#endif
#ifndef _SHELLRES_HXX
#include "shellres.hxx"
#endif
#ifndef _BREAKIT_HXX
#include "breakit.hxx"
#endif
#ifndef _TAUTOFMT_HRC
#include "tautofmt.hrc"
#endif

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;

#define FRAME_OFFSET 4

//========================================================================

class AutoFmtPreview : public Window
{
public:
            AutoFmtPreview( Window* pParent, const ResId& rRes );
            ~AutoFmtPreview();

    void NotifyChange( const SwTableAutoFmt& rNewData );

protected:
    virtual void Paint( const Rectangle& rRect );

private:
    SwTableAutoFmt          aCurData;
    VirtualDevice           aVD;
    SvtScriptedTextHelper   aScriptedText;
    BOOL                    bFitWidth;
    static BYTE             aFmtMap[25];        // Zuordnung: Zelle->Format
    Rectangle               aCellArray[25];     // Position und Groesse der Zellen
    SvxBoxItem*             aLinePtrArray[49];  // LinienAttribute
    Size                    aPrvSize;
    const USHORT            nLabelColWidth;
    const USHORT            nDataColWidth1;
    const USHORT            nDataColWidth2;
    const USHORT            nRowHeight;
    const String            aStrJan;
    const String            aStrFeb;
    const String            aStrMar;
    const String            aStrNorth;
    const String            aStrMid;
    const String            aStrSouth;
    const String            aStrSum;
    SvNumberFormatter*      pNumFmt;
    //-------------------------------------------
    void	Init			();
    void	DoPaint			( const Rectangle& rRect );
    void	CalcCellArray	( BOOL bFitWidth );
    void	CalcLineMap		();
    void	PaintCells		();
    void	DrawBackground	( BYTE nIndex );
    void	DrawFrame		( BYTE nIndex );
    void	DrawString		( BYTE nIndex );
    void    MakeFonts       ( BYTE nIndex, Font& rFont, Font& rCJKFont, Font& rCTLFont );
    String	MakeNumberString( String cellString, BOOL bAddDec );
    void	DrawFrameLine	( const SvxBorderLine&	rLineD,
                              Point					from,
                              Point					to,
                              BOOL					bHorizontal,
                              const SvxBorderLine&	rLineLT,
                              const SvxBorderLine&	rLineL,
                              const SvxBorderLine&	rLineLB,
                              const SvxBorderLine&	rLineRT,
                              const SvxBorderLine&	rLineR,
                              const SvxBorderLine&	rLineRB );
    void 	CheckPriority	( USHORT			nCurLine,
                              AutoFmtLine		eLine,
                              SvxBorderLine&	rLine );
    void	GetLines		( BYTE nIndex, AutoFmtLine eLine,
                              SvxBorderLine&	rLineD,
                              SvxBorderLine&	rLineLT,
                              SvxBorderLine&	rLineL,
                              SvxBorderLine&	rLineLB,
                              SvxBorderLine&	rLineRT,
                              SvxBorderLine&	rLineR,
                              SvxBorderLine&	rLineRB );
};

//========================================================================

struct SwLineStruct
{
    short nLeft;  //Breite der linken Linie
    short nMiddle;//Breite des Zwischenraums
    short nRight; //Breite der rechten Linie
};

void lcl_SwLinkLine( const SwLineStruct& dLine,
                        const SwLineStruct& ltLine,
                        const SwLineStruct& lLine,
                        const SwLineStruct& lbLine,
                        const SwLineStruct& rtLine,
                        const SwLineStruct& rLine,
                        const SwLineStruct& rbLine,
                        short* dxArr);

//SC-Produkt!
BOOL lcl_HasPriority(	const SvxBorderLine* pThis,
                        const SvxBorderLine* pOther );

//------------------------------------------------------------------------

class SwStringInputDlg : public ModalDialog
{
public:
            SwStringInputDlg( 	  Window* pParent,
                            const String& rTitle,
                            const String& rEditTitle,
                            const String& rDefault );
            ~SwStringInputDlg();

    void GetInputString( String& rString ) const;

private:
    Edit			aEdInput;	// Edit erhaelt so den Focus
    FixedText		aFtEditTitle;
    OKButton		aBtnOk;
    CancelButton	aBtnCancel;
};


BYTE AutoFmtPreview::aFmtMap[] = { 0,  1,  2,  1,  3, // Zuordnung:
                                   4,  5,  6,  5,  7, // Zelle->Format
                                   8,  9, 10,  9, 11,
                                   4,  5,  6,  5,  7,
                                  12, 13, 14, 13, 15  };




SwStringInputDlg::SwStringInputDlg( Window* 		pParent,
                                    const String&	rTitle,
                                    const String&	rEditTitle,
                                    const String&	rDefault	) :
    ModalDialog 	( pParent, SW_RES( DLG_SWDLG_STRINPUT ) ),
    //
    aEdInput		( this, SW_RES( ED_INPUT ) ),
    aBtnOk			( this, SW_RES( BTN_OK ) ),
    aBtnCancel		( this, SW_RES( BTN_CANCEL ) ),
    aFtEditTitle	( this, SW_RES( FT_LABEL ) )
{
    SetText( rTitle );
    aFtEditTitle.SetText( rEditTitle );
    aEdInput.SetText( rDefault );
    //-------------
    FreeResource();
}

//------------------------------------------------------------------------

void SwStringInputDlg::GetInputString( String& rString ) const
{
    rString = aEdInput.GetText();
}


__EXPORT SwStringInputDlg::~SwStringInputDlg()
{
}

//========================================================================
// AutoFormat-Dialog:


SwAutoFormatDlg::SwAutoFormatDlg( Window* pParent, SwWrtShell* pWrtShell,
                    BOOL bSetAutoFormat, const SwTableAutoFmt* pSelFmt )
    : SfxModalDialog( pParent, SW_RES( DLG_AUTOFMT_TABLE ) ),
    //
    aFlFormat       ( this, SW_RES( FL_FORMAT ) ),
    aLbFormat		( this, SW_RES( LB_FORMAT ) ),
    pWndPreview 	( new AutoFmtPreview( this, SW_RES( WND_PREVIEW ) ) ),
    aBtnNumFormat	( this, SW_RES( BTN_NUMFORMAT ) ),
    aBtnBorder		( this, SW_RES( BTN_BORDER ) ),
    aBtnFont		( this, SW_RES( BTN_FONT ) ),
    aBtnPattern 	( this, SW_RES( BTN_PATTERN ) ),
    aBtnAlignment	( this, SW_RES( BTN_ALIGNMENT ) ),
    aFlFormats       ( this, SW_RES( FL_FORMATS ) ),
    aBtnOk			( this, SW_RES( BTN_OK ) ),
    aBtnCancel		( this, SW_RES( BTN_CANCEL ) ),
    aBtnHelp		( this, SW_RES( BTN_HELP ) ),
    aBtnAdd 		( this, SW_RES( BTN_ADD ) ),
    aBtnRemove		( this, SW_RES( BTN_REMOVE ) ),
    aBtnMore		( this, SW_RES( BTN_MORE ) ),
    aBtnRename		( this, SW_RES( BTN_RENAME ) ),
    aStrTitle		( SW_RES( STR_ADD_TITLE ) ),
    aStrLabel		( SW_RES( STR_ADD_LABEL ) ),
    aStrClose		( SW_RES( STR_BTN_CLOSE ) ),
    aStrDelTitle	( SW_RES( STR_DEL_TITLE ) ),
    aStrDelMsg		( SW_RES( STR_DEL_MSG ) ),
    aStrRenameTitle	( SW_RES( STR_RENAME_TITLE ) ),
    aStrInvalidFmt	( SW_RES( STR_INVALID_AFNAME )),
    //
    nIndex			( 0 ),
    nDfltStylePos	( 0 ),
    bCoreDataChanged( FALSE ),
    bSetAutoFmt		( bSetAutoFormat ),
    pShell			( pWrtShell )
{
    pTableTbl = new SwTableAutoFmtTbl;
    pTableTbl->Load();

    Init( pSelFmt );
    //------------- >
    FreeResource();
}

//------------------------------------------------------------------------


__EXPORT SwAutoFormatDlg::~SwAutoFormatDlg()
{
    delete pWndPreview;

    if( bCoreDataChanged )
        pTableTbl->Save();
    delete pTableTbl;
}

//------------------------------------------------------------------------


void SwAutoFormatDlg::Init( const SwTableAutoFmt* pSelFmt )
{
    Link aLk( LINK( this, SwAutoFormatDlg, CheckHdl ) );
    aBtnBorder.SetClickHdl( aLk );
    aBtnFont.SetClickHdl( aLk );
    aBtnPattern.SetClickHdl( aLk );
    aBtnAlignment.SetClickHdl( aLk );
    aBtnNumFormat.SetClickHdl( aLk );

    aBtnAdd.SetClickHdl ( LINK( this, SwAutoFormatDlg, AddHdl ) );
    aBtnRemove.SetClickHdl ( LINK( this, SwAutoFormatDlg, RemoveHdl ) );
    aBtnRename.SetClickHdl ( LINK( this, SwAutoFormatDlg, RenameHdl ) );
    aBtnOk.SetClickHdl ( LINK( this, SwAutoFormatDlg, OkHdl ) );
    aLbFormat.SetSelectHdl( LINK( this, SwAutoFormatDlg, SelFmtHdl ) );

    aBtnMore.AddWindow( &aBtnNumFormat );
    aBtnMore.AddWindow( &aBtnBorder );
    aBtnMore.AddWindow( &aBtnFont );
    aBtnMore.AddWindow( &aBtnPattern );
    aBtnMore.AddWindow( &aBtnAlignment );
    aBtnMore.AddWindow( &aFlFormats );
    aBtnMore.AddWindow( &aBtnRename );

    aBtnAdd.Enable( bSetAutoFmt );

    nIndex = 0;
    if( !bSetAutoFmt )
    {
        // dann muss die Liste um den Eintrag <Keins> erweitert werden.
        aLbFormat.InsertEntry( ViewShell::GetShellRes()->aStrNone );
        nDfltStylePos = 1;
        nIndex = 255;
    }

    for( BYTE i = 0, nCount = (BYTE)pTableTbl->Count(); i < nCount; i++ )
    {
        SwTableAutoFmt* pFmt = (*pTableTbl)[ i ];
        aLbFormat.InsertEntry( pFmt->GetName() );
        if( pSelFmt && pFmt->GetName() == pSelFmt->GetName() )
            nIndex = i;
    }

    aLbFormat.SelectEntryPos( 255 != nIndex ? (nDfltStylePos + nIndex) : 0 );
    SelFmtHdl( 0 );
}

//------------------------------------------------------------------------


void SwAutoFormatDlg::UpdateChecks( const SwTableAutoFmt& rFmt, BOOL bEnable )
{
    aBtnNumFormat.Enable( bEnable );
    aBtnNumFormat.Check( rFmt.IsValueFormat() );

    aBtnBorder.Enable( bEnable );
    aBtnBorder.Check( rFmt.IsFrame() );

    aBtnFont.Enable( bEnable );
    aBtnFont.Check( rFmt.IsFont() );

    aBtnPattern.Enable( bEnable );
    aBtnPattern.Check( rFmt.IsBackground() );

    aBtnAlignment.Enable( bEnable );
    aBtnAlignment.Check( rFmt.IsJustify() );
}

void SwAutoFormatDlg::FillAutoFmtOfIndex( SwTableAutoFmt*& rToFill ) const
{
    if( 255 != nIndex )
    {
        if( rToFill )
            *rToFill = *(*pTableTbl)[ nIndex ];
        else
            rToFill = new SwTableAutoFmt( *(*pTableTbl)[ nIndex ] );
    }
    else if( rToFill )
        delete rToFill, rToFill = 0;
}


/*------------------------------------------------------------------------
  Handler:
  ---------*/


IMPL_LINK( SwAutoFormatDlg, CheckHdl, Button *, pBtn )
{
    SwTableAutoFmtPtr pData  = (*pTableTbl)[nIndex];
    BOOL bCheck = ((CheckBox*)pBtn)->IsChecked(), bDataChgd = TRUE;

    if( pBtn == &aBtnNumFormat )
        pData->SetValueFormat( bCheck );
    else if ( pBtn == &aBtnBorder )
        pData->SetFrame( bCheck );
    else if ( pBtn == &aBtnFont )
        pData->SetFont( bCheck );
    else if ( pBtn == &aBtnPattern )
        pData->SetBackground( bCheck );
    else if ( pBtn == &aBtnAlignment )
        pData->SetJustify( bCheck );
//	  else if ( pBtn == &aBtnAdjust )
//		  pData->SetIncludeWidthHeight( bCheck );
    else
        bDataChgd = FALSE;

    if( bDataChgd )
    {
        if( !bCoreDataChanged )
        {
            aBtnCancel.SetText( aStrClose );
            bCoreDataChanged = TRUE;
        }

        pWndPreview->NotifyChange( *pData );
    }
    return 0;
}

/*------------------------------------------------------------------------*/


IMPL_LINK( SwAutoFormatDlg, AddHdl, void *, EMPTYARG )
{
    BOOL bOk = FALSE, bFmtInserted = FALSE;
    while( !bOk )
    {
        SwStringInputDlg*	pDlg = new SwStringInputDlg( this,
                                                            aStrTitle,
                                                            aStrLabel,
                                                            aEmptyStr );
        if( RET_OK == pDlg->Execute() )
        {
            String aFormatName;
            pDlg->GetInputString( aFormatName );

            if( aFormatName.Len() > 0 )
            {
                for( USHORT n = 0; n < pTableTbl->Count(); ++n )
                    if( (*pTableTbl)[n]->GetName() == aFormatName )
                        break;

                if( n >= pTableTbl->Count() )
                {
                    // Format mit dem Namen noch nicht vorhanden, also
                    // aufnehmen
                    SwTableAutoFmtPtr pNewData = new
                                        SwTableAutoFmt( aFormatName );
                    pShell->GetTableAutoFmt( *pNewData );

                    // Sortiert einfuegen!!
                    for( n = 1; n < pTableTbl->Count(); ++n )
                        if( (*pTableTbl)[ n ]->GetName() > aFormatName )
                            break;

                    pTableTbl->Insert( pNewData, n );
                    aLbFormat.InsertEntry( aFormatName, nDfltStylePos + n );
                    aLbFormat.SelectEntryPos( nDfltStylePos + n );
                    bFmtInserted = TRUE;
                    aBtnAdd.Enable( FALSE );
                    if ( !bCoreDataChanged )
                    {
                        aBtnCancel.SetText( aStrClose );
                        bCoreDataChanged = TRUE;
                    }

                    SelFmtHdl( 0 );
                    bOk = TRUE;
                }
            }

            if( !bFmtInserted )
            {
                bOk = RET_CANCEL == ErrorBox( this,
                                    WinBits( WB_OK_CANCEL | WB_DEF_OK),
                                    aStrInvalidFmt
                                    ).Execute();
            }
        }
        else
            bOk = TRUE;
        delete pDlg;
    }
    return 0;
}

//------------------------------------------------------------------------

IMPL_LINK( SwAutoFormatDlg, RemoveHdl, void *, EMPTYARG )
{
    String aMessage	= aStrDelMsg ;
    aMessage.AppendAscii("\n\n");
    aMessage += aLbFormat.GetSelectEntry() ;
    aMessage += '\n';

    MessBox* pBox = new MessBox( this, WinBits( WB_OK_CANCEL ),
                                    aStrDelTitle, aMessage);

    if ( pBox->Execute() == RET_OK )
    {
        aLbFormat.RemoveEntry( nDfltStylePos + nIndex );
        aLbFormat.SelectEntryPos( nDfltStylePos + nIndex-1 );

        pTableTbl->DeleteAndDestroy( nIndex );
        nIndex--;

        if( !nIndex )
        {
            aBtnRemove.Enable(FALSE);
            aBtnRename.Enable(FALSE);
        }

        if( !bCoreDataChanged )
        {
            aBtnCancel.SetText( aStrClose );
            bCoreDataChanged = TRUE;
        }
    }
    delete pBox;

    SelFmtHdl( 0 );

    return 0;
}

IMPL_LINK( SwAutoFormatDlg, RenameHdl, void *, EMPTYARG )
{
    BOOL bOk = FALSE;
    while( !bOk )
    {
        SwStringInputDlg* pDlg = new SwStringInputDlg( this,
                        aStrRenameTitle, aLbFormat.GetSelectEntry(),
                                                        aEmptyStr );
        if( pDlg->Execute() == RET_OK )
        {
            BOOL bFmtRenamed = FALSE;
            String aFormatName;
            pDlg->GetInputString( aFormatName );

            if ( aFormatName.Len() > 0 )
            {
                for( USHORT n = 0; n < pTableTbl->Count(); ++n )
                    if ((*pTableTbl)[n]->GetName() == aFormatName)
                        break;

                if( n >= pTableTbl->Count() )
                {
                    // Format mit dem Namen noch nicht vorhanden, also
                    // umbenennen

                    aLbFormat.RemoveEntry( nDfltStylePos + nIndex );
                    SwTableAutoFmtPtr p = (*pTableTbl)[ nIndex ];
                    pTableTbl->Remove( nIndex );

                    p->SetName( aFormatName );

                    // Sortiert einfuegen!!
                    for( n = 1; n < pTableTbl->Count(); ++n )
                        if( (*pTableTbl)[ n ]->GetName() > aFormatName )
                            break;

                    pTableTbl->Insert( p, n );
                    aLbFormat.InsertEntry( aFormatName, nDfltStylePos + n );
                    aLbFormat.SelectEntryPos( nDfltStylePos + n );

                    if ( !bCoreDataChanged )
                    {
                        aBtnCancel.SetText( aStrClose );
                        bCoreDataChanged = TRUE;
                    }

                    SelFmtHdl( 0 );
                    bOk = TRUE;
                    bFmtRenamed = TRUE;
                }
            }

            if( !bFmtRenamed )
            {
                bOk = RET_CANCEL == ErrorBox( this,
                                    WinBits( WB_OK_CANCEL | WB_DEF_OK),
                                    aStrInvalidFmt
                                    ).Execute();
            }
        }
        else
            bOk = TRUE;
        delete pDlg;
    }
    return 0;
}

//------------------------------------------------------------------------

IMPL_LINK( SwAutoFormatDlg, SelFmtHdl, void *, EMPTYARG )
{
    BOOL bBtnEnable = FALSE;
    BYTE nSelPos = (BYTE) aLbFormat.GetSelectEntryPos(), nOldIdx = nIndex;
    if( nSelPos >= nDfltStylePos )
    {
        nIndex = nSelPos - nDfltStylePos;
        pWndPreview->NotifyChange( *(*pTableTbl)[nIndex] );
        bBtnEnable = 0 != nIndex;
        UpdateChecks( *(*pTableTbl)[nIndex], TRUE );
    }
    else
    {
        nIndex = 255;

        SwTableAutoFmt aTmp( ViewShell::GetShellRes()->aStrNone );
        aTmp.SetFont( FALSE );
        aTmp.SetJustify( FALSE );
        aTmp.SetFrame( FALSE );
        aTmp.SetBackground( FALSE );
        aTmp.SetValueFormat( FALSE );
        aTmp.SetWidthHeight( FALSE );

        if( nOldIdx != nIndex )
            pWndPreview->NotifyChange( aTmp );
        UpdateChecks( aTmp, FALSE );
    }

    aBtnRemove.Enable( bBtnEnable );
    aBtnRename.Enable( bBtnEnable );

    return 0;
}
//------------------------------------------------------------------------

IMPL_LINK_INLINE_START( SwAutoFormatDlg, OkHdl, Button *, EMPTYARG )
{
    if( bSetAutoFmt )
        pShell->SetTableAutoFmt( *(*pTableTbl)[ nIndex ] );
    EndDialog( RET_OK );
    return TRUE;
}
IMPL_LINK_INLINE_END( SwAutoFormatDlg, OkHdl, Button *, EMPTYARG )

//========================================================================
// AutoFmtPreview

//------------------------------------------------------------------------

AutoFmtPreview::AutoFmtPreview( Window* pParent, const ResId& rRes ) :
        Window			( pParent, rRes ),
        aVD 			( *this ),
        aScriptedText   ( aVD ),
        aCurData		( aEmptyStr ),
        bFitWidth		( FALSE ),
        aStrJan 		( SW_RES( STR_JAN ) ),
        aStrFeb 		( SW_RES( STR_FEB ) ),
        aStrMar 		( SW_RES( STR_MAR ) ),
        aStrNorth		( SW_RES( STR_NORTH ) ),
        aStrMid 		( SW_RES( STR_MID ) ),
        aStrSouth		( SW_RES( STR_SOUTH ) ),
        aStrSum 		( SW_RES( STR_SUM ) ),
        aPrvSize		( GetSizePixel().Width()  - 6,
                          GetSizePixel().Height() - 30 ),
        nLabelColWidth	( (USHORT)(((aPrvSize.Width()-4)/4)-12) ),
        nDataColWidth1	( (USHORT)(((aPrvSize.Width()-4)-(nLabelColWidth*2)) / 3) ),
        nDataColWidth2	( (USHORT)(((aPrvSize.Width()-4)-(nLabelColWidth*2)) / 4) ),
        nRowHeight		( (USHORT)((aPrvSize.Height()-4) / 5) )
{
    Reference< XMultiServiceFactory > xMSF = ::comphelper::getProcessServiceFactory();
    pNumFmt	= new SvNumberFormatter( xMSF, LANGUAGE_SYSTEM );

    Init();
}

//------------------------------------------------------------------------

__EXPORT AutoFmtPreview::~AutoFmtPreview()
{
    for ( USHORT i=0; i<=35; i++ )
    {
        delete aLinePtrArray[i];
    }
    delete pNumFmt;
}

//------------------------------------------------------------------------

void lcl_AssignLine( SvxBorderLine& 	  dest,
                        const SvxBorderLine* src )
{
    if( src )
    {
        dest.SetColor( src->GetColor() );
        dest.SetOutWidth( src->GetOutWidth() );
        dest.SetInWidth( src->GetInWidth() );
        dest.SetDistance( src->GetDistance() );
    }
    else
    {
        dest.SetColor( Color( COL_WHITE ) );
        dest.SetOutWidth ( 0 );
        dest.SetInWidth  ( 0 );
        dest.SetDistance ( 0 );
    }
}

//------------------------------------------------------------------------

void lcl_GetLineStruct(	SwLineStruct&		 rLine,
                        const SvxBorderLine& rBoxLine )
{
    if ( rBoxLine.GetOutWidth() > 0 )
    {
        rLine.nLeft 	= rBoxLine.GetOutWidth();
        rLine.nMiddle	= rBoxLine.GetDistance();
        rLine.nRight	= rBoxLine.GetInWidth();

        /* Linienstaerke auf dick/duenn abbilden:
         *
         * (in TWIPS, 1pt = 20 TWIPS = duenn)
         * alles was <=0  ist -> (0,0,0)
         * alles was <=20 ist -> (1,0,0)/(1,1,1)
         * alles andere 	  -> (3,0,0)/(2,1,2)
         */

        if ( rLine.nMiddle == 0 ) // einfache Linie?
        {
            rLine.nRight = 0; // sicher ist sicher

            if ( rLine.nLeft <= 20 )
            {
                rLine.nLeft = 1;
            }
            else if ( rLine.nLeft > 20 )
            {
                rLine.nLeft = 3;
            }
        }
        else // doppelte Linie
        {
            rLine.nMiddle = 1;
            if ( rLine.nLeft <= 20 )
            {
                rLine.nLeft = rLine.nRight = 1;
            }
            else if ( rLine.nLeft > 20 )
            {
                rLine.nLeft = rLine.nRight = 2;
            }
        }
    }
    else
    {
        rLine.nLeft 	= 0;
        rLine.nMiddle	= 0;
        rLine.nRight	= 0;
    }
}

//------------------------------------------------------------------------

void AutoFmtPreview::CheckPriority( USHORT			nCurLine,
                                    AutoFmtLine 	eLine,
                                    SvxBorderLine&	rLine )
{

     const SvxBorderLine*	 pDrawLine;
    USHORT					nOther = 0;

    switch ( eLine )
    {
        case TOP_LINE:
            {
                lcl_AssignLine( rLine, aLinePtrArray[nCurLine]->GetTop() );
                nOther = nCurLine-7;
                pDrawLine = aLinePtrArray[nOther]->GetBottom();

                if ( lcl_HasPriority( pDrawLine, &rLine ) )
                    lcl_AssignLine( rLine, pDrawLine );
            }
            break;

        case BOTTOM_LINE:
            {
                lcl_AssignLine( rLine, aLinePtrArray[nCurLine]->GetBottom() );
                nOther = nCurLine+7;
                pDrawLine = aLinePtrArray[nOther]->GetTop();

                if ( lcl_HasPriority( pDrawLine, &rLine ) )
                    lcl_AssignLine( rLine, pDrawLine );
            }
            break;

        case LEFT_LINE:
            {
                lcl_AssignLine( rLine, aLinePtrArray[nCurLine]->GetLeft() );
                nOther = nCurLine-1;
                pDrawLine = aLinePtrArray[nOther]->GetRight();

                if ( lcl_HasPriority( pDrawLine, &rLine ) )
                    lcl_AssignLine( rLine, pDrawLine );
            }
            break;

        case RIGHT_LINE:
            {
                lcl_AssignLine( rLine, aLinePtrArray[nCurLine]->GetRight() );
                nOther = nCurLine+1;
                pDrawLine = aLinePtrArray[nOther]->GetLeft();

                if ( lcl_HasPriority( pDrawLine, &rLine ) )
                    lcl_AssignLine( rLine, pDrawLine );
            }
            break;
    }

}

//------------------------------------------------------------------------

void AutoFmtPreview::GetLines( BYTE nIndex, AutoFmtLine eLine,
                               SvxBorderLine&	rLineD,
                               SvxBorderLine&	rLineLT,
                               SvxBorderLine&	rLineL,
                               SvxBorderLine&	rLineLB,
                               SvxBorderLine&	rLineRT,
                               SvxBorderLine&	rLineR,
                               SvxBorderLine&	rLineRB )
{
    {
        SvxBorderLine aNullLine;

        lcl_AssignLine( rLineD,  &aNullLine);
        lcl_AssignLine( rLineLT, &aNullLine);
        lcl_AssignLine( rLineL,  &aNullLine);
        lcl_AssignLine( rLineLB, &aNullLine);
        lcl_AssignLine( rLineRT, &aNullLine);
        lcl_AssignLine( rLineR,  &aNullLine);
        lcl_AssignLine( rLineRB, &aNullLine);
    }

    USHORT	nCurLine   = nIndex + 8 + ((nIndex/5)*2);
    USHORT	nOther	   = 0;

    switch ( eLine )
    {
        case TOP_LINE:
            {
                // obere Linien werden nur in der
                // ersten Zeile gemalt:
                if ( (nIndex >= 0) && (nIndex <= 4) )
                {
                    // links
                    CheckPriority( nCurLine, eLine, rLineD );
                    nOther = nCurLine-7;
                    CheckPriority( nOther, LEFT_LINE, rLineLT );
                    nOther = nCurLine-1;
                    CheckPriority( nOther, TOP_LINE, rLineL );
                    CheckPriority( nCurLine, LEFT_LINE, rLineLB );
                    // rechts
                    nOther = nCurLine-7;
                    CheckPriority( nOther, RIGHT_LINE, rLineRT );
                    nOther = nCurLine+1;
                    CheckPriority( nOther, TOP_LINE, rLineR );
                    CheckPriority( nCurLine, RIGHT_LINE, rLineRB );
                }
            }
            break;

        case BOTTOM_LINE:
            {
                // links
                CheckPriority( nCurLine, eLine, rLineD );
                CheckPriority( nCurLine, LEFT_LINE, rLineLT );
                nOther = nCurLine-1;
                CheckPriority( nOther, BOTTOM_LINE, rLineL );
                nOther = nCurLine+7;
                CheckPriority( nOther, LEFT_LINE, rLineLB );
                // rechts
                CheckPriority( nCurLine, RIGHT_LINE, rLineRT );
                nOther = nCurLine+1;
                CheckPriority( nOther, BOTTOM_LINE, rLineR );
                nOther = nCurLine+7;
                CheckPriority( nOther, RIGHT_LINE, rLineRB );
            }
            break;

        case LEFT_LINE:
            {
                // linke Linien werden nur in der
                // ersten Spalte gemalt:
                if ( (nIndex%5) == 0 )
                {
                    // oben
                    CheckPriority( nCurLine, eLine, rLineD );
                    CheckPriority( nCurLine, TOP_LINE, rLineLT );
                    nOther = nCurLine-7;
                    CheckPriority( nOther, LEFT_LINE, rLineL );
                    nOther = nCurLine-1;
                    CheckPriority( nOther, TOP_LINE, rLineLB );
                    // unten
                    CheckPriority( nCurLine, BOTTOM_LINE, rLineRT );
                    nOther = nCurLine+7;
                    CheckPriority( nOther, LEFT_LINE, rLineR );
                    nOther = nCurLine-1;
                    CheckPriority( nOther, BOTTOM_LINE, rLineRB );
                }
            }
            break;

        case RIGHT_LINE:
            {
                // oben
                CheckPriority( nCurLine, eLine, rLineD );
                nOther = nCurLine+1;
                CheckPriority( nOther, TOP_LINE, rLineLT );
                nOther = nCurLine-7;
                CheckPriority( nOther, RIGHT_LINE, rLineL );
                CheckPriority( nCurLine, TOP_LINE, rLineLB );
                // unten
                nOther = nCurLine+1;
                CheckPriority( nOther, BOTTOM_LINE, rLineRT );
                nOther = nCurLine+7;
                CheckPriority( nOther, RIGHT_LINE, rLineR );
                CheckPriority( nCurLine, BOTTOM_LINE, rLineRB );
            }
            break;
    }
}

//------------------------------------------------------------------------

void lcl_DrawHorizontalLine( OutputDevice& rDev, const Point& rStart, const Point& rEnd )
{
    DBG_ASSERT( rStart.Y() <= rEnd.Y(), "lcl_DrawHorizontalLine - wrong point order" );
    Point aLineStart( rStart );
    Point aLineEnd( rEnd.X(), rStart.Y() );
    while( aLineStart.Y() <= rEnd.Y() )
    {
        rDev.DrawLine( aLineStart, aLineEnd );
        ++aLineStart.Y();
        ++aLineEnd.Y();
    }
}

void lcl_DrawVerticalLine( OutputDevice& rDev, const Point& rStart, const Point& rEnd )
{
    DBG_ASSERT( rStart.X() <= rEnd.X(), "lcl_DrawVerticalLine - wrong point order" );
    Point aLineStart( rStart );
    Point aLineEnd( rStart.X(), rEnd.Y() );
    while( aLineStart.X() <= rEnd.X() )
    {
        rDev.DrawLine( aLineStart, aLineEnd );
        ++aLineStart.X();
        ++aLineEnd.X();
    }
}

void AutoFmtPreview::DrawFrameLine( const SvxBorderLine&	rLineD,
                                    Point					from,
                                    Point					to,
                                    BOOL					bHorizontal,
                                    const SvxBorderLine&	rLineLT,
                                    const SvxBorderLine&	rLineL,
                                    const SvxBorderLine&	rLineLB,
                                    const SvxBorderLine&	rLineRT,
                                    const SvxBorderLine&	rLineR,
                                    const SvxBorderLine&	rLineRB )
{
    SwLineStruct	dLine;
    SwLineStruct	ltLine;
    SwLineStruct	lLine;
    SwLineStruct	lbLine;
    SwLineStruct	rtLine;
    SwLineStruct	rLine;
    SwLineStruct	rbLine;
    short			dxArr[4];

    lcl_GetLineStruct( dLine,	rLineD	);
    lcl_GetLineStruct( ltLine,	rLineLT );
    lcl_GetLineStruct( lLine,	rLineL	);
    lcl_GetLineStruct( lbLine,	rLineLB );
    lcl_GetLineStruct( rtLine,	rLineRT );
    lcl_GetLineStruct( rLine,	rLineR	);
    lcl_GetLineStruct( rbLine,	rLineRB );

    if ( dLine.nLeft > 0 )
    {
        Color oldColor = aVD.GetLineColor();
        aVD.SetLineColor( rLineD.GetColor() );

        USHORT	nHeight  = dLine.nLeft + dLine.nMiddle + dLine.nRight;
        Point	from2	 = from;
        Point	to2 	 = to;

        lcl_SwLinkLine( dLine,
                    ltLine, lLine, lbLine,
                    rtLine, rLine, rbLine,
                    dxArr );

        if ( bHorizontal )
        {
            from.Y() -= nHeight/2;
            to.Y()	 -= nHeight/2;
            to.Y()	 += (dLine.nLeft-1);

            from.X() += dxArr[0];
            to.X()	 += dxArr[2];

            lcl_DrawHorizontalLine( aVD, from, to );

            // noch eine zweite Linie zu malen?
            if ( dLine.nRight != 0 )
            {
                from2.Y() -= nHeight/2;
                from2.Y() += dLine.nLeft+dLine.nMiddle;
                to2.Y()   -= nHeight/2;
                to2.Y()   += dLine.nMiddle+dLine.nLeft;
                to2.Y()   += (dLine.nRight-1);

                from2.X() += dxArr[1];
                to2.X()   += dxArr[3];

                lcl_DrawHorizontalLine( aVD, from2, to2 );
            }
        }
        else
        {
            from.X() += nHeight/2;
            from.X() -= (dLine.nLeft-1);
            to.X()	 += nHeight/2;

            from.Y() += dxArr[0];
            to.Y()	 += dxArr[2];

            lcl_DrawVerticalLine( aVD, from, to );

            // noch eine zweite Linie zu malen?
            if ( dLine.nRight != 0 )
            {
                from2.X() -= nHeight/2;
                to2.X()   -= nHeight/2;
                to2.X()   += (dLine.nRight-1);

                from2.Y() += dxArr[1];
                to2.Y()   += dxArr[3];

                lcl_DrawVerticalLine( aVD, from2, to2 );
            }
        }
        aVD.SetLineColor( oldColor );
    }
}

//------------------------------------------------------------------------

void AutoFmtPreview::DrawFrame( BYTE nIndex )
{
    //----------------------
    // Malen des Zellrahmens
    //----------------------

    SvxBorderLine	aLineD;
    SvxBorderLine	aLineLT;
    SvxBorderLine	aLineL;
    SvxBorderLine	aLineLB;
    SvxBorderLine	aLineRT;
    SvxBorderLine	aLineR;
    SvxBorderLine	aLineRB;
    Rectangle&		cellRect = aCellArray[nIndex];

    //---------
    // TopLine ---------------------------------------------
    //---------
    GetLines( nIndex, TOP_LINE, aLineD,
                aLineLT, aLineL, aLineLB,
                aLineRT, aLineR, aLineRB );

    DrawFrameLine( aLineD,
                    cellRect.TopLeft(), cellRect.TopRight(), TRUE,
                    aLineLT,  aLineL, aLineLB,
                    aLineRT,  aLineR, aLineRB );

    //------------
    // BottomLine ------------------------------------------
    //------------
    GetLines( nIndex, BOTTOM_LINE, aLineD,
                aLineLT, aLineL, aLineLB,
                aLineRT, aLineR, aLineRB );

    DrawFrameLine( aLineD,
                    cellRect.BottomLeft(), cellRect.BottomRight(), TRUE,
                    aLineLT,  aLineL, aLineLB,
                    aLineRT,  aLineR, aLineRB );

    //----------
    // LeftLine --------------------------------------------
    //----------
    GetLines( nIndex, LEFT_LINE, aLineD,
                aLineLT, aLineL, aLineLB,
                aLineRT, aLineR, aLineRB );

    DrawFrameLine( aLineD,
                    cellRect.TopLeft(), cellRect.BottomLeft(), FALSE,
                    aLineLT,  aLineL, aLineLB,
                    aLineRT,  aLineR, aLineRB );

    //-----------
    // RightLine -------------------------------------------
    //-----------
    GetLines( nIndex, RIGHT_LINE, aLineD,
                aLineLT, aLineL, aLineLB,
                aLineRT, aLineR, aLineRB );

    DrawFrameLine( aLineD,
                    cellRect.TopRight(), cellRect.BottomRight(), FALSE,
                    aLineLT,  aLineL, aLineLB,
                    aLineRT,  aLineR, aLineRB );
}

//------------------------------------------------------------------------

void lcl_SetFontProperties(
        Font& rFont,
        const SvxFontItem& rFontItem,
        const SvxWeightItem& rWeightItem,
        const SvxPostureItem& rPostureItem )
{
    rFont.SetFamily     ( rFontItem.GetFamily() );
    rFont.SetName       ( rFontItem.GetFamilyName() );
    rFont.SetStyleName  ( rFontItem.GetStyleName() );
    rFont.SetCharSet    ( rFontItem.GetCharSet() );
    rFont.SetPitch      ( rFontItem.GetPitch() );
    rFont.SetWeight     ( (FontWeight)rWeightItem.GetValue() );
    rFont.SetItalic     ( (FontItalic)rPostureItem.GetValue() );
}

#define SETONALLFONTS( MethodName, Value )                  \
rFont.MethodName( Value );                                  \
rCJKFont.MethodName( Value );                               \
rCTLFont.MethodName( Value );

void AutoFmtPreview::MakeFonts( BYTE nIndex, Font& rFont, Font& rCJKFont, Font& rCTLFont )
{
    const SwBoxAutoFmt& rBoxFmt = aCurData.GetBoxFmt( nIndex );

    rFont = rCJKFont = rCTLFont = GetFont();
    Size aFontSize( rFont.GetSize().Width(), 10 );

    lcl_SetFontProperties( rFont, rBoxFmt.GetFont(), rBoxFmt.GetWeight(), rBoxFmt.GetPosture() );
    lcl_SetFontProperties( rCJKFont, rBoxFmt.GetCJKFont(), rBoxFmt.GetCJKWeight(), rBoxFmt.GetCJKPosture() );
    lcl_SetFontProperties( rCTLFont, rBoxFmt.GetCTLFont(), rBoxFmt.GetCTLWeight(), rBoxFmt.GetCTLPosture() );

    SETONALLFONTS( SetUnderline,    (FontUnderline)rBoxFmt.GetUnderline().GetValue() );
    SETONALLFONTS( SetStrikeout,    (FontStrikeout)rBoxFmt.GetCrossedOut().GetValue() );
    SETONALLFONTS( SetOutline,      rBoxFmt.GetContour().GetValue() );
    SETONALLFONTS( SetShadow,       rBoxFmt.GetShadowed().GetValue() );
    SETONALLFONTS( SetColor,        rBoxFmt.GetColor().GetValue() );
    SETONALLFONTS( SetSize,         aFontSize );
    SETONALLFONTS( SetTransparent,  TRUE );
}

//------------------------------------------------------------------------

void AutoFmtPreview::DrawString( BYTE nIndex )
{
    //------------------------
    // Ausgabe des Zelltextes:
    //------------------------
    ULONG   nNum;
    double  nVal;
    String cellString;
    switch( nIndex )
    {
        case  1: cellString = aStrJan;			break;
        case  2: cellString = aStrFeb;			break;
        case  3: cellString = aStrMar;			break;
        case  5: cellString = aStrNorth;		break;
        case 10: cellString = aStrMid;			break;
        case 15: cellString = aStrSouth;		break;
        case  4:
        case 20: cellString = aStrSum;			break;

        case  6:
        case  8:
        case 16:
        case 18:    nVal = nIndex;
                    nNum = 5;
                    goto MAKENUMSTR;
        case 17:
        case  7: 	nVal = nIndex;
                    nNum = 6;
                    goto MAKENUMSTR;
        case 11:
        case 12:
        case 13: 	nVal = nIndex;
                    nNum = 12 == nIndex ? 10 : 9;
                    goto MAKENUMSTR;

        case  9:	nVal = 21; nNum = 7; 	goto MAKENUMSTR;
        case 14:	nVal = 36; nNum = 11; 	goto MAKENUMSTR;
        case 19:	nVal = 51; nNum = 7;	goto MAKENUMSTR;
        case 21:	nVal = 33; nNum = 13;	goto MAKENUMSTR;
        case 22:	nVal = 36; nNum = 14;	goto MAKENUMSTR;
        case 23:	nVal = 39; nNum = 13;	goto MAKENUMSTR;
        case 24:	nVal = 108; nNum = 15;	goto MAKENUMSTR;
MAKENUMSTR:
            if( aCurData.IsValueFormat() )
            {
                String sFmt; LanguageType eLng, eSys;
                aCurData.GetBoxFmt( (BYTE)nNum ).GetValueFormat( sFmt, eLng, eSys );

                ULONG nKey = 0;
                if ( eLng == LANGUAGE_SYSTEM && eSys != ::GetAppLanguage() )
                {
                    //	#53381# wenn System beim Speichern etwas anderes war,
                    //	muss konvertiert werden (geht nur mit eingebauten Formaten)
                    ULONG nOrig = pNumFmt->GetEntryKey( sFmt, eSys );
                    if ( nOrig != NUMBERFORMAT_ENTRY_NOT_FOUND )
                        nKey = pNumFmt->GetFormatForLanguageIfBuiltIn( nOrig,
                                                        ::GetAppLanguage() );
                }
                else	// sonst einfach suchen oder anlegen
                {
                    nKey = pNumFmt->GetEntryKey( sFmt, eLng );
                    if( NUMBERFORMAT_ENTRY_NOT_FOUND == nKey )
                    {
                        xub_StrLen nCheckPos;
                        short nType;
                        pNumFmt->PutEntry( sFmt, nCheckPos, nType, nKey, eLng );
                    }
                }
                Color* pDummy;
                pNumFmt->GetOutputString( nVal, nKey, cellString, &pDummy );
            }
            else
                cellString = String::CreateFromInt32(nVal);
            break;

    }

    if( cellString.Len() )
    {
        Size				aStrSize;
        BYTE			  	nFmtIndex		= (BYTE) aFmtMap[nIndex];
        Rectangle&			cellRect		= aCellArray[nIndex];
        Point				aPos			= aCellArray[nIndex].TopLeft();
        USHORT				nRightX 		= 0;
//			  BOOL				  bJustify		  = aCurData.IsJustify();
//			  ScHorJustifyAttr	  aHorJustifyItem;
//			CellHorJustify	  eJustification;

        Size theMaxStrSize( cellRect.GetWidth() - FRAME_OFFSET,
                            cellRect.GetHeight() - FRAME_OFFSET );
        if( aCurData.IsFont() )
        {
            Font aFont, aCJKFont, aCTLFont;
            MakeFonts( nFmtIndex, aFont, aCJKFont, aCTLFont );
            aScriptedText.SetFonts( &aFont, &aCJKFont, &aCTLFont );
        }
        else
            aScriptedText.SetDefaultFont();

        aScriptedText.SetText( cellString, pBreakIt->xBreak );
        aStrSize = aScriptedText.GetTextSize();

        if( aCurData.IsFont() &&
            theMaxStrSize.Height() < aStrSize.Height() )
        {
                // wenn der String in diesem Font nicht
                // in die Zelle passt, wird wieder der
                // Standard-Font genommen:
                aScriptedText.SetDefaultFont();
                aStrSize = aScriptedText.GetTextSize();
        }

        while( theMaxStrSize.Width() <= aStrSize.Width() &&
                cellString.Len() > 1 )
        {
//					if( eJustification == SVX_HOR_JUSTIFY_RIGHT )
//  						cellString.Erase( 0, 1 );
//					else
            cellString.Erase( cellString.Len() - 1 );
            aScriptedText.SetText( cellString, pBreakIt->xBreak );
            aStrSize = aScriptedText.GetTextSize();
        }

        nRightX  = (USHORT)(  cellRect.GetWidth()
                                - aStrSize.Width()
                                - FRAME_OFFSET );
        //-------------
        // Ausrichtung:
        //-------------
        /*   if ( bJustify )
        {
            aCurData.GetHorJustify( nFmtIndex, aHorJustifyItem );
            eJustification = (CellHorJustify)aHorJustifyItem.GetValue();
        }
        else
        {
            eJustification = SC_HOR_JUSTIFY_STANDARD;
        }*/

        //-----------------------------
        // vertikal (immer zentrieren):
        //-----------------------------
        aPos.Y() += (nRowHeight - (USHORT)aStrSize.Height()) / 2;

        //-----------
        // horizontal
        //-----------
/* 		  if ( eJustification != SC_HOR_JUSTIFY_STANDARD )*/
        if (aCurData.IsJustify())
        {
            USHORT nHorPos = (USHORT)
                    ((cellRect.GetWidth()-aStrSize.Width())/2);
            const SvxAdjustItem& rAdj = aCurData.GetBoxFmt(nFmtIndex).GetAdjust();
            switch ( rAdj.GetAdjust() )
            {
                case SVX_ADJUST_LEFT:
                    aPos.X() += FRAME_OFFSET;
                    break;
                case SVX_ADJUST_RIGHT:
                    aPos.X() += nRightX;
                    break;
                default:
                    aPos.X() += nHorPos;
                    break;
            }
        }
        else
        {
            //---------------------
            // Standardausrichtung:
            //---------------------
            if ( ((nIndex%5) == 0) || (nIndex == 4) )
            {
                // Text-Label links oder Summe linksbuendig
                aPos.X() += FRAME_OFFSET;
            }
            else
            {
                    // Zahlen/Datum rechtsbuendig
                aPos.X() += nRightX;
            }
        }

        //-------------------------------
        aScriptedText.DrawText( aPos );
        //-------------------------------
    }
}

#undef FRAME_OFFSET

//------------------------------------------------------------------------


void AutoFmtPreview::DrawBackground( BYTE nIndex )
{
    SvxBrushItem aBrushItem( aCurData.GetBoxFmt( aFmtMap[nIndex])
                                .GetBackground() );
    Color 	oldColor	 = aVD.GetLineColor();
    aVD.SetLineColor( Color(COL_TRANSPARENT) );
    Color oldFillColor = aVD.GetFillColor();
    aVD.SetFillColor( aBrushItem.GetColor() );
    //-----------------------
    aVD.DrawRect( aCellArray[nIndex] );
    //-----------------------
    aVD.SetLineColor( oldColor );
    aVD.SetFillColor( oldFillColor );
}

//------------------------------------------------------------------------


void AutoFmtPreview::PaintCells()
{
    BYTE i = 0;

    //---------------
    // 1. Hintergrund
    //---------------
    if ( aCurData.IsBackground() )
    {
        for ( i=0; i<=24; i++ )
        {
            DrawBackground( i );
        }
    }

    //----------
    // 2. Rahmen
    //----------
    if ( aCurData.IsFrame() )
    {
        for ( i=0; i<=24; i++ )
        {
            DrawFrame( i );
        }
    }

    //---------
    // 3. Werte
    //---------
    for ( i = 0; i<=24; i++ )
    {
        DrawString( i );
    }
}

//------------------------------------------------------------------------


void __EXPORT AutoFmtPreview::Init()
{
    SetBorderStyle( GetBorderStyle() | WINDOW_BORDER_MONO );

    SvxBoxItem aEmptyBoxItem;

    aEmptyBoxItem.SetLine	 ( NULL, BOX_LINE_TOP );
    aEmptyBoxItem.SetLine	 ( NULL, BOX_LINE_BOTTOM );
    aEmptyBoxItem.SetLine	 ( NULL, BOX_LINE_LEFT );
    aEmptyBoxItem.SetLine	 ( NULL, BOX_LINE_RIGHT );
    aEmptyBoxItem.SetDistance( 0 );

    //------------------------
    // Linienattribut-Feld mit
    // Null-BoxItems fuellen
    //------------------------
    for ( USHORT i=0; i<=48; i++ )
    {
        aLinePtrArray[i] = new SvxBoxItem( aEmptyBoxItem );
    }

    CalcCellArray( FALSE );
    CalcLineMap();
}

//------------------------------------------------------------------------


void AutoFmtPreview::CalcCellArray( BOOL bFitWidth )
{
    // Initialisieren des Zellfeldes (5x5 Zellen).
    // Rectangles enthalten Position und Groesse einer Zelle.
    // abhaengig, ob Zellbreite an Format angepasst werden soll (bFitWidth)
    USHORT	nRow;
    USHORT	nDataColWidth = (bFitWidth) ? nDataColWidth2 : nDataColWidth1;

    for ( nRow=0; nRow<=4; nRow++ )
    {
        Point	topLeftPos ( 2, (nRowHeight * nRow) + 2 );
        Size	rectSize   ( 0, nRowHeight );
        USHORT	nCell;
        USHORT	nFirst = nRow*5;
        USHORT	nLast  = nFirst+4;

        for ( nCell=nFirst; nCell<=nLast; nCell++ )
        {
            //--------------------
            // Position berechnen:
            //--------------------
            if ( nCell == 0 )
            {
                // erste Zelle benoetigt keine Sonderbehandlung
            }
            else if ( (nCell <= 4) && (nCell != 0) )
            {
                // Zelle aus der ersten Zeile
                topLeftPos = aCellArray[nCell-1].TopRight();
            }
            else
            {
                topLeftPos = aCellArray[nCell-5].BottomLeft();
            }

            //-------------------
            // Groesse berechnen:
            //-------------------
            if ( (nCell == nFirst) || (nCell == nLast) )
                rectSize.Width() = nLabelColWidth;
            else
                rectSize.Width() = nDataColWidth;

            aCellArray[nCell] = Rectangle( topLeftPos, rectSize );
        }
    }
    aPrvSize.Width()  = aCellArray[24].BottomRight().X() + 3;
    aPrvSize.Height() = aCellArray[24].BottomRight().Y() + 3;

}

//------------------------------------------------------------------------


void AutoFmtPreview::CalcLineMap()
{
    //----------------------------------------------------
    // Initialisieren des Linenattributfeldes (7x7 Zellen)
    // Ein Eintrag dieses Feldes enthaelt einen Verweis
    // auf die Linienattribute des zugehoerigen Feldes.
    // Dieses Feld "umschliesst" das Zellfeld mit einer
    // Reihe "leerer" Zellen.
    //----------------------------------------------------

    SvxBoxItem aFrameItem;
    BYTE nRow;

    for ( nRow=0; nRow<=4; nRow++ )
    {
        USHORT	nLine;
        BYTE  nCell;
        BYTE  nFirst = (nRow * 5);
        BYTE  nLast  = nFirst+4;

        for ( nCell=nFirst; nCell<=nLast; nCell++ )
        {
            nLine = nCell + 8 + ((nCell/5)*2);
                aFrameItem = aCurData.GetBoxFmt( aFmtMap[nCell]).GetBox();
            *(aLinePtrArray[nLine]) = aFrameItem;
        }
    }
}

//------------------------------------------------------------------------


void AutoFmtPreview::NotifyChange( const SwTableAutoFmt& rNewData )
{
    aCurData  = rNewData;
    bFitWidth = aCurData.IsJustify();//TRUE;  //???
    CalcCellArray( bFitWidth );
    CalcLineMap();
    DoPaint( Rectangle( Point(0,0), GetSizePixel() ) );
}

//------------------------------------------------------------------------


void AutoFmtPreview::DoPaint( const Rectangle& rRect )
{
    sal_uInt32 nOldDrawMode = aVD.GetDrawMode();
    if( GetSettings().GetStyleSettings().GetHighContrastMode() &&
            SW_MOD()->GetAccessibilityOptions().GetIsForBorders() )
        aVD.SetDrawMode( DRAWMODE_SETTINGSLINE | DRAWMODE_SETTINGSFILL | DRAWMODE_SETTINGSTEXT | DRAWMODE_SETTINGSGRADIENT );

    Bitmap	thePreview;
    Point	aCenterPos;
    Size	theWndSize = GetSizePixel();
    Size	thePrevSize;
    Color 	oldColor;
    Font	aFont;

    aFont = aVD.GetFont();
    aFont.SetTransparent( TRUE );

    aVD.SetFont 		 ( aFont );
    aVD.SetLineColor     ();
    const Color& rWinColor = GetSettings().GetStyleSettings().GetWindowColor();
    aVD.SetBackground    ( Wallpaper(rWinColor) );
    aVD.SetFillColor     ( rWinColor );
    aVD.SetOutputSizePixel	( aPrvSize );

    //--------------------------------
    // Zellen auf virtual Device malen
    // und Ergebnis sichern
    //--------------------------------
    PaintCells();
    thePreview = aVD.GetBitmap( Point(0,0), aPrvSize );

    //--------------------------------------
    // Rahmen malen und Vorschau zentrieren:
    // (virtual Device fuer Fensterausgabe)
    //--------------------------------------
    aVD.SetOutputSizePixel( theWndSize );
    oldColor = aVD.GetLineColor();
    aVD.SetLineColor();
    aVD.DrawRect( Rectangle( Point(0,0), theWndSize ) );
    SetLineColor( oldColor );
    aCenterPos	= Point( (theWndSize.Width()  - aPrvSize.Width() ) / 2,
                         (theWndSize.Height() - aPrvSize.Height()) / 2 );
    aVD.DrawBitmap( aCenterPos, thePreview );

    //----------------------------
    // Ausgabe im Vorschaufenster:
    //----------------------------
    DrawBitmap( Point(0,0), aVD.GetBitmap( Point(0,0), theWndSize ) );

    aVD.SetDrawMode( nOldDrawMode );
}

//------------------------------------------------------------------------

void __EXPORT AutoFmtPreview::Paint( const Rectangle& rRect )
{
    DoPaint( rRect );
}
//------------------------------------------------------------------------


BOOL lcl_HasPriority(	const SvxBorderLine* pThis,
                        const SvxBorderLine* pOther )
{

    if (!pThis)
        return FALSE;
    if (!pOther)
        return TRUE;

    USHORT nThisSize = pThis->GetOutWidth() + pThis->GetDistance() + pThis->GetInWidth();
    USHORT nOtherSize = pOther->GetOutWidth() + pOther->GetDistance() + pOther->GetInWidth();

    if (nThisSize > nOtherSize)
        return TRUE;
    else if (nThisSize < nOtherSize)
        return FALSE;
    else
    {
        if ( pOther->GetInWidth() && !pThis->GetInWidth() )
            return TRUE;
        else if ( pThis->GetInWidth() && !pOther->GetInWidth() )
            return FALSE;
        else
        {
            return TRUE;			//! ???
        }
    }
}

void lcl_SwLinkLine(const SwLineStruct& dLine,
                    const SwLineStruct& ltLine,
                    const SwLineStruct& lLine,
                    const SwLineStruct& lbLine,
                    const SwLineStruct& rtLine,
                    const SwLineStruct& rLine,
                    const SwLineStruct& rbLine,
                    short* dxArr)
{
    short nDx;
    short nDxDx;
    short nltW = ltLine.nLeft + ltLine.nMiddle + ltLine.nRight;
    short nlbW = lbLine.nLeft + lbLine.nMiddle + lbLine.nRight;
    short nrtW = rtLine.nLeft + rtLine.nMiddle + rtLine.nRight;
    short nrbW = rbLine.nLeft + rbLine.nMiddle + rbLine.nRight;
    if (dLine.nRight == 0) // einfache Linie
    {
        dxArr[1] = 0;
        dxArr[3] = 0;

        // Linker Teil
        if ((lLine.nLeft != 0) && (lLine.nRight == 0)) // links einfache Linie
            dxArr[0] = 0;
        else
        {
            nDx = Max(nltW, nlbW) / 2;
            nDxDx = (Max(nltW, nlbW) + 1) % 2;
            if ((nDx == 0) || (ltLine.nRight == 0) || (lbLine.nRight == 0))
            {
                if (lLine.nLeft == 0)
                    dxArr[0] = nDxDx - nDx;
                else
                    dxArr[0] = nDxDx + nDx;
            }
            else
                dxArr[0] = nDxDx + nDx;
        }

        // Rechter Teil
        if ((rLine.nLeft != 0) && (rLine.nRight == 0)) // rechts keine Linie
            dxArr[2] = 0;
        else
        {
            nDx = Max(nrtW, nrbW) / 2;
            if ((nDx == 0) || (rtLine.nRight == 0) || (rbLine.nRight == 0))
            {
                if (rLine.nLeft == 0)
                    dxArr[2] = nDx;
                else
                    dxArr[2] = -nDx;
            }
            else
                dxArr[2] = -nDx;
        }
    }
    else
    {
        // Linker oberer Teil
        if ((nltW == 0) || (ltLine.nRight == 0)) // Links oben keine oder einfache Linie
        {
            nDx = (nlbW + 1) / 2;
            nDxDx = (nlbW + 1) % 2;
            if (nDx == 0)
                dxArr[0] = nDxDx + nDx;
            else
                dxArr[0] = nDxDx - (nlbW / 2) + ltLine.nLeft;
        }
        else
        {
            nDx = (nltW + 1) / 2;
            nDxDx = (nltW + 1) % 2;
            if (nDx == 0)
                dxArr[0] = nDxDx - nDx;
            else
                dxArr[0] = nDxDx + nDx - ltLine.nRight;
        }
        // Linker unterer Teil
        if ((nlbW == 0) || (lbLine.nRight == 0)) // Links unten keine oder einfache Linie
        {
            nDx = (nltW + 1) / 2;
            nDxDx = (nltW + 1) % 2;
            if (nDx == 0)
                dxArr[1] = nDxDx + nDx;
            else
                dxArr[1] = nDxDx - (nltW / 2) + lbLine.nLeft;
        }
        else
        {
            nDx = (nlbW + 1) / 2;
            nDxDx = (nlbW + 1) % 2;
            if (nDx == 0)
                dxArr[1] = nDxDx - nDx;
            else
                dxArr[1] = nDxDx + nDx - lbLine.nRight;
        }

        // Rechter oberer Teil
        if ((nrtW == 0) || (rtLine.nRight == 0)) // Rechts oben keine oder einfache Linie
        {
            nDx = (nrbW + 1) / 2;
            if (nDx == 0)
                dxArr[2] = -nDx;
            else
                dxArr[2] = (nrbW / 2) - rtLine.nLeft;
        }
        else
        {
            nDx = (nrtW + 1) / 2;
            if (nDx == 0)
                dxArr[2] = nDx;
            else
                dxArr[2] = -nDx + rtLine.nLeft;
        }
        // Rechter unterer Teil
        if ((nrbW == 0) || (rbLine.nRight == 0)) // Rechts unten keine oder einfache Linie
        {
            nDx = (nrtW + 1) / 2;
            if (nDx == 0)
                dxArr[3] = -nDx;
            else
                dxArr[3] = (nrtW / 2) - rbLine.nLeft;
        }
        else
        {
            nDx = (nrbW + 1) / 2;
            if (nDx == 0)
                dxArr[3] = nDx;
            else
                dxArr[3] = -nDx + rbLine.nLeft;
        }
    }
}


