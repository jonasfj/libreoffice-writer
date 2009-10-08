/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swdlgfact.cxx,v $
 * $Revision: 1.17 $
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

#include "swdlgfact.hxx"
#include <svtools/style.hxx>
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#include "dialog.hrc"
#include "misc.hrc"
#include "chrdlg.hrc"
#include "fldui.hrc"
#include "table.hrc"
#include "frmui.hrc"
#include "dbui.hrc"
#include "globals.hrc"
#include "fldui.hrc"
#include "envelp.hrc"
#include "dochdl.hrc"
#include <index.hrc>
#include <regionsw.hrc>
#include <fmtui.hrc>

#include <wordcountdialog.hxx>
#include "abstract.hxx" // add for SwInsertAbstractDlg
#include "addrdlg.hxx" // add for SwAddrDlg
#include "ascfldlg.hxx" // add for SwAsciiFilterDlg
#include "bookmark.hxx" //add for SwInsertBookmarkDlg
#include "break.hxx" //add for SwBreakDlg
#include "changedb.hxx" //add for SwChangeDBDlg
#include "chrdlg.hxx" // add for SwCharDlg
#include "convert.hxx" //add for SwConvertTableDlg
#include "cption.hxx" //add for SwCaptionDialog
#include "dbinsdlg.hxx" //add for  SwInsertDBColAutoPilot
#include "docfnote.hxx" //add for SwFootNoteOptionDlg
#include "docstdlg.hxx" //add for SwDocStatPage
#include "DropDownFieldDialog.hxx" //add for DropDownFieldDialog
#include "envlop.hxx" //add for SwEnvDlg
#include "label.hxx" //add for SwLabDlg
#include "drpcps.hxx" //add for SwDropCapsDlg
#include "swuipardlg.hxx" //add for SwParaDlg
#include "pattern.hxx" //add for SwBackgroundDlg
#include "rowht.hxx" //add for SwTableHeightDlg
#include "selglos.hxx" //add for SwSelGlossaryDlg
#include "splittbl.hxx" //add for SwSplitTblDlg 
#include "srtdlg.hxx" //add for SwSortDlg
#include "tautofmt.hxx" //add for SwAutoFormatDlg
#include "tblnumfm.hxx" //add for SwNumFmtDlg
#include "uiborder.hxx" //add for SwBorderDlg
#include "wrap.hxx" //add for SwWrapDlg
#include "colwd.hxx" //add for SwTableWidthDlg
#include "tabledlg.hxx" //add for SwTableTabDlg
#include "fldtdlg.hxx" //add for SwFldDlg
#include "fldedt.hxx" //add for SwFldEditDlg
#include "swrenamexnameddlg.hxx" //add for SwRenameXNamedDlg
#include "swmodalredlineacceptdlg.hxx" //add for SwModalRedlineAcceptDlg
#include <frmdlg.hxx> //add for SwFrmDlg
#include <tmpdlg.hxx> //add for SwTemplateDlg
#include <glossary.hxx> //add for SwGlossaryDlg
#include <inpdlg.hxx> //add for SwFldInputDlg
#include <insfnote.hxx> //add for SwInsFootNoteDlg
#include <insrc.hxx> //add for SwInsRowColDlg
#include <insrule.hxx> //add for SwInsertGrfRulerDlg
#include <instable.hxx> //add for SwInsTableDlg
#include <javaedit.hxx> //add for SwJavaEditDialog
#include <linenum.hxx> //add for SwLineNumberingDlg
#include <mailmrge.hxx> //add for SwMailMergeDlg, SwMailMergeCreateFromDlg, SwMailMergeFieldConnectionsDlg
#include <mergetbl.hxx> //add for SwMergeTblDlg
#include <multmrk.hxx> //add for SwMultiTOXMarkDlg
#include <num.hxx> //add for SwSvxNumBulletTabDialog
#include <outline.hxx> //add for SwOutlineTabDialog
#include <column.hxx> //add for SwColumnDlg
#include <cnttab.hxx> //add for SwMultiTOXTabDialog
#include <swuicnttab.hxx> //add for SwMultiTOXTabDialog
#include <regionsw.hxx> //add for SwEditRegionDlg, SwInsertSectionTabDialog
#include <optcomp.hxx> //add for SwCompatibilityOptPage
#include <optload.hxx> //add for SwLoadOptPage
#include <optpage.hxx> //add for OptPage
#include <swuiidxmrk.hxx> //add for SwIndexMarkDlg, SwAuthMarkDlg, SwIndexMarkModalDlg, SwAuthMarkModalDlg
#include <svx/dialogs.hrc>
#include <mailmergewizard.hxx>
#include <mailconfigpage.hxx>

using namespace ::com::sun::star;

IMPL_ABSTDLG_BASE(AbstractSwWordCountDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSwInsertAbstractDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractSfxSingleTabDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSwAsciiFilterDlg_Impl);
IMPL_ABSTDLG_BASE(VclAbstractDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSplitTableDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractSwBreakDlg_Impl); //add for SwBreakDlg
IMPL_ABSTDLG_BASE(AbstractTabDialog_Impl); //add for SwCharDlg, SwFootNoteOptionDlg, SwEnvDlg  SwParaDlg SwTableTabDlg
IMPL_ABSTDLG_BASE(AbstractSwConvertTableDlg_Impl); //add for SwConvertTableDlg
IMPL_ABSTDLG_BASE(AbstractSwInsertDBColAutoPilot_Impl); //add for SwInsertDBColAutoPilot
IMPL_ABSTDLG_BASE(AbstractDropDownFieldDialog_Impl); //add for DropDownFieldDialog
IMPL_ABSTDLG_BASE(AbstarctSwLabDlg_Impl);//add for SwLabDlg
IMPL_ABSTDLG_BASE(AbstarctSwSelGlossaryDlg_Impl);//add for SwSelGlossaryDlg
IMPL_ABSTDLG_BASE(AbstractSwAutoFormatDlg_Impl); //add for SwAutoFormatDlg
IMPL_ABSTDLG_BASE(AbstractSwFldDlg_Impl); //add for SwFldDlg
IMPL_ABSTDLG_BASE(AbstractSwRenameXNamedDlg_Impl); //add for SwRenameXNamedDlg
IMPL_ABSTDLG_BASE(AbstractSwModalRedlineAcceptDlg_Impl); //add for SwModalRedlineAcceptDlg
IMPL_ABSTDLG_BASE(AbstractGlossaryDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractFldInputDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractInsFootNoteDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractInsertGrfRulerDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractInsTableDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractJavaEditDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractMailMergeDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractMailMergeCreateFromDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractMailMergeFieldConnectionsDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractMultiTOXTabDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractEditRegionDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractInsertSectionTabDialog_Impl);
IMPL_ABSTDLG_BASE(AbstractIndexMarkFloatDlg_Impl);
IMPL_ABSTDLG_BASE(AbstractAuthMarkFloatDlg_Impl);

// AbstractTabDialog_Impl begin
void AbstractTabDialog_Impl::SetCurPageId( USHORT nId )
{
    pDlg->SetCurPageId( nId );
}

const SfxItemSet* AbstractTabDialog_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

const USHORT* AbstractTabDialog_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return pDlg->GetInputRanges( pItem );
}

void AbstractTabDialog_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     pDlg->SetInputSet( pInSet );
}
//From class Window.
void AbstractTabDialog_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}
String AbstractTabDialog_Impl::GetText() const
{
    return pDlg->GetText();
}

//add for AbstractTabDialog_Impl end

void    AbstractSwWordCountDialog_Impl::SetValues(const SwDocStat& rCurrent, const SwDocStat& rDoc)
{
    pDlg->SetValues(rCurrent, rDoc);
}

//add for SwInsertAbstractDlg begin
BYTE AbstractSwInsertAbstractDlg_Impl::GetLevel() const
{
    return pDlg->GetLevel();
}
BYTE AbstractSwInsertAbstractDlg_Impl::GetPara() const
{
    return pDlg->GetPara();
}
//add for SwInsertAbstractDlg end

//add for SwAddrDlg, SwDropCapsDlg ,SwBackgroundDlg, SwNumFmtDlg SwBorderDlg SwWrapDlg  SwFldEditDlg begin
const SfxItemSet* AbstractSfxSingleTabDialog_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}
//add for SwAddrDlg, SwDropCapsDlg ,SwBackgroundDlg, SwNumFmtDlg SwBorderDlg SwWrapDlg SwFldEditDlg  end

//add for SwAsciiFilterDlg begin
void AbstractSwAsciiFilterDlg_Impl::FillOptions( SwAsciiOptions& rOptions )
{
    pDlg->FillOptions(rOptions);
}
//add for SwAsciiFilterDlg end

//add for SwSplitTblDlg
sal_uInt16 AbstractSplitTableDialog_Impl::GetSplitMode()
{
    return pDlg->GetSplitMode();
}
//add for SwBreakDlg begin
String AbstractSwBreakDlg_Impl::GetTemplateName()
{
    return pDlg->GetTemplateName();
}

USHORT AbstractSwBreakDlg_Impl:: GetKind()
{
    return pDlg->GetKind();
}

USHORT AbstractSwBreakDlg_Impl:: GetPageNumber()
{
    return pDlg->GetPageNumber();
}
//add for SwBreakDlg end

//add for SwConvertTableDlg begin
void AbstractSwConvertTableDlg_Impl::GetValues( sal_Unicode& rDelim,SwInsertTableOptions& rInsTblFlags,
                                                SwTableAutoFmt *& prTAFmt )
{
    pDlg->GetValues(rDelim,rInsTblFlags, prTAFmt);
}
//add for SwConvertTableDlg end

//add for SwInsertDBColAutoPilot begin
void AbstractSwInsertDBColAutoPilot_Impl::DataToDoc( const uno::Sequence< uno::Any >& rSelection,
        uno::Reference< sdbc::XDataSource> rxSource,
        uno::Reference< sdbc::XConnection> xConnection,
        uno::Reference< sdbc::XResultSet > xResultSet)
{
    pDlg->DataToDoc(rSelection, rxSource, xConnection, xResultSet);
}
//add for SwInsertDBColAutoPilot end

// add for DropDownFieldDialog begin
ByteString      AbstractDropDownFieldDialog_Impl::GetWindowState( ULONG nMask  ) const
{
    return pDlg->GetWindowState(nMask);
}
void       AbstractDropDownFieldDialog_Impl::SetWindowState( const ByteString& rStr )
{
    pDlg->SetWindowState(rStr);
}
//add for DropDownFieldDialog end

//add for SwLabDlg begin

void AbstarctSwLabDlg_Impl::SetCurPageId( USHORT nId )
{
    pDlg->SetCurPageId( nId );
}

const SfxItemSet* AbstarctSwLabDlg_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

const USHORT* AbstarctSwLabDlg_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return pDlg->GetInputRanges( pItem );
}

void AbstarctSwLabDlg_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     pDlg->SetInputSet( pInSet );
}

void AbstarctSwLabDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}
String AbstarctSwLabDlg_Impl::GetText() const
{
    return pDlg->GetText();
}
const String& AbstarctSwLabDlg_Impl::GetBusinessCardStr() const
{
    return pDlg->GetBusinessCardStr();
}
Printer * AbstarctSwLabDlg_Impl::GetPrt()
{
    return pDlg->GetPrt();
}
//add for SwLabDlg end


//add for SwSelGlossaryDlg begin

void AbstarctSwSelGlossaryDlg_Impl::InsertGlos(const String &rRegion, const String &rGlosName)
{
    pDlg->InsertGlos( rRegion, rGlosName );
}
USHORT AbstarctSwSelGlossaryDlg_Impl::GetSelectedIdx() const
{
    return pDlg->GetSelectedIdx();
}
void AbstarctSwSelGlossaryDlg_Impl::SelectEntryPos(USHORT nIdx)
{
    pDlg->SelectEntryPos( nIdx );
}

//add for SwSelGlossaryDlg end

//add for SwAutoFormatDlg begin

void AbstractSwAutoFormatDlg_Impl::FillAutoFmtOfIndex( SwTableAutoFmt*& rToFill ) const
{
    pDlg->FillAutoFmtOfIndex(rToFill);
}
//add for SwAutoFormatDlg end

//add for SwFldDlg begin
void AbstractSwFldDlg_Impl::SetCurPageId( USHORT nId )
{
    pDlg->SetCurPageId( nId );
}

const SfxItemSet* AbstractSwFldDlg_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}

const USHORT* AbstractSwFldDlg_Impl::GetInputRanges(const SfxItemPool& pItem )
{
    return pDlg->GetInputRanges( pItem );
}

void AbstractSwFldDlg_Impl::SetInputSet( const SfxItemSet* pInSet )
{
     pDlg->SetInputSet( pInSet );
}

void AbstractSwFldDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}
String AbstractSwFldDlg_Impl::GetText() const
{
    return pDlg->GetText();
}
void AbstractSwFldDlg_Impl::Start( BOOL bShowl )
{
    pDlg->Start( bShowl );
}

void AbstractSwFldDlg_Impl::Initialize(SfxChildWinInfo *pInfo)
{
    pDlg->Initialize( pInfo );
}

void AbstractSwFldDlg_Impl::ReInitDlg()
{
    pDlg->ReInitDlg();
}
void AbstractSwFldDlg_Impl::ActivateDatabasePage()
{
    pDlg->ActivateDatabasePage();
}
Window* AbstractSwFldDlg_Impl::GetWindow()
{
    return (Window*)pDlg;
}
void	AbstractSwFldDlg_Impl::ShowPage( USHORT nId )
{
    pDlg->ShowPage(nId);
}
//add for SwFldD end

//add for SwRenameXNamedDlg begin

void AbstractSwRenameXNamedDlg_Impl::SetForbiddenChars( const String& rSet )
{
    pDlg->SetForbiddenChars( rSet );
}

void	AbstractSwRenameXNamedDlg_Impl::SetAlternativeAccess(
             STAR_REFERENCE( container::XNameAccess ) & xSecond,
             STAR_REFERENCE( container::XNameAccess ) & xThird )
{
    pDlg->SetAlternativeAccess( xSecond, xThird);
}
//add for SwRenameXNamedDlg end

//add for SwModalRedlineAcceptDlg begin

void	AbstractSwModalRedlineAcceptDlg_Impl::AcceptAll( BOOL bAccept )
{
    pDlg->AcceptAll( bAccept);
}
//add for SwModalRedlineAcceptDlg end

// AbstractGlossaryDlg_Impl begin
String AbstractGlossaryDlg_Impl::GetCurrGrpName() const
{
    return pDlg->GetCurrGrpName();
}

String AbstractGlossaryDlg_Impl::GetCurrShortName() const
{
    return pDlg->GetCurrShortName();
}
// AbstractGlossaryDlg_Impl end

// AbstractFldInputDlg_Impl begin
void AbstractFldInputDlg_Impl::SetWindowState( const ByteString& rStr )
{
    pDlg->SetWindowState( rStr );
}

ByteString AbstractFldInputDlg_Impl::GetWindowState( ULONG nMask ) const
{
    return pDlg->GetWindowState( nMask );
}
// AbstractFldInputDlg_Impl end

// AbstractInsFootNoteDlg_Impl begin
String AbstractInsFootNoteDlg_Impl::GetFontName()
{
    return pDlg->GetFontName();
}
BOOL AbstractInsFootNoteDlg_Impl::IsEndNote()
{
    return pDlg->IsEndNote();
}
String AbstractInsFootNoteDlg_Impl::GetStr()
{
    return pDlg->GetStr();
}
void AbstractInsFootNoteDlg_Impl::SetHelpId( ULONG nHelpId )
{
    pDlg->SetHelpId( nHelpId );
}
void AbstractInsFootNoteDlg_Impl::SetText( const XubString& rStr )
{
    pDlg->SetText( rStr );
}
// AbstractInsFootNoteDlg_Impl end

// AbstractInsertGrfRulerDlg_Impl begin
String AbstractInsertGrfRulerDlg_Impl::GetGraphicName()
{
    return pDlg->GetGraphicName();
}
BOOL AbstractInsertGrfRulerDlg_Impl::IsSimpleLine()
{
    return pDlg->IsSimpleLine();
}
BOOL AbstractInsertGrfRulerDlg_Impl::HasImages() const
{
    return pDlg->HasImages();
}
// AbstractInsertGrfRulerDlg_Impl end

// AbstractInsTableDlg_Impl begin
void AbstractInsTableDlg_Impl::GetValues( String& rName, USHORT& rRow, USHORT& rCol,
                                SwInsertTableOptions& rInsTblFlags, String& rTableAutoFmtName,
                                SwTableAutoFmt *& prTAFmt )
{
    pDlg->GetValues( rName, rRow, rCol,	rInsTblFlags, rTableAutoFmtName, prTAFmt);
}
// AbstractInsTableDlg_Impl end

// AbstractJavaEditDialog_Impl begin
String AbstractJavaEditDialog_Impl::GetText()
{
    return pDlg->GetText();
}
String AbstractJavaEditDialog_Impl::GetType()
{
    return pDlg->GetType();
}
BOOL AbstractJavaEditDialog_Impl::IsUrl()
{
    return pDlg->IsUrl();
}
BOOL AbstractJavaEditDialog_Impl::IsNew()
{
    return pDlg->IsNew();
}
BOOL AbstractJavaEditDialog_Impl::IsUpdate()
{
    return pDlg->IsUpdate();
}
// AbstractJavaEditDialog_Impl end

// AbstractMailMergeDlg_Impl begin
USHORT AbstractMailMergeDlg_Impl::GetMergeType()
{
    return pDlg->GetMergeType();
}
const ::rtl::OUString& AbstractMailMergeDlg_Impl::GetSaveFilter() const
{
    return pDlg->GetSaveFilter();
}

const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > AbstractMailMergeDlg_Impl::GetSelection() const
{
    return pDlg->GetSelection();
}
uno::Reference< sdbc::XResultSet> AbstractMailMergeDlg_Impl::GetResultSet() const
{
    return pDlg->GetResultSet();
}
// AbstractMailMergeDlg_Impl end
// AbstractMailMergeCreateFromDlg_Impl begin
BOOL AbstractMailMergeCreateFromDlg_Impl::IsThisDocument() const
{
    return pDlg->IsThisDocument();
}
// AbstractMailMergeCreateFromDlg_Impl end
//AbstractMailMergeFieldConnectionsDlg_Impl begin
BOOL AbstractMailMergeFieldConnectionsDlg_Impl::IsUseExistingConnections() const
{
    return pDlg->IsUseExistingConnections();
}
// AbstractMailMergeFieldConnectionsDlg_Impl end

// AbstractMultiTOXTabDialog_Impl begin
SwForm* AbstractMultiTOXTabDialog_Impl::GetForm(CurTOXType eType)
{
    return pDlg->GetForm(eType);
}
CurTOXType AbstractMultiTOXTabDialog_Impl::GetCurrentTOXType() const
{
    return pDlg->GetCurrentTOXType();
}
SwTOXDescription& AbstractMultiTOXTabDialog_Impl::GetTOXDescription(CurTOXType eTOXTypes)
{
    return pDlg->GetTOXDescription(eTOXTypes);
}
const SfxItemSet* AbstractMultiTOXTabDialog_Impl::GetOutputItemSet() const
{
    return pDlg->GetOutputItemSet();
}
// AbstractMultiTOXTabDialog_Impl end

// AbstractEditRegionDlg_Impl begin
void AbstractEditRegionDlg_Impl::SelectSection(const String& rSectionName)
{
    pDlg->SelectSection(rSectionName);
}
// AbstractEditRegionDlg_Impl end
//AbstractInsertSectionTabDialog_Impl begin
void AbstractInsertSectionTabDialog_Impl::SetSection(const SwSection& rSect)
{
    pDlg->SetSection( rSect);
}
// AbstractInsertSectionTabDialog_Impl end

//AbstractIndexMarkFloatDlg_Impl begin
void AbstractIndexMarkFloatDlg_Impl::ReInitDlg(SwWrtShell& rWrtShell)
{
    pDlg->ReInitDlg( rWrtShell);
}
Window* AbstractIndexMarkFloatDlg_Impl::GetWindow()
{
    return (Window*)pDlg;
}
// AbstractIndexMarkFloatDlg_Impl end

//AbstractAuthMarkFloatDlg_Impl begin
void AbstractAuthMarkFloatDlg_Impl::ReInitDlg(SwWrtShell& rWrtShell)
{
    pDlg->ReInitDlg( rWrtShell);
}
Window* AbstractAuthMarkFloatDlg_Impl::GetWindow()
{
    return (Window*)pDlg;
}
// AbstractAuthMarkFloatDlg_Impl end

AbstractMailMergeWizard_Impl::~AbstractMailMergeWizard_Impl()
{
    delete pDlg;
}

void AbstractMailMergeWizard_Impl::StartExecuteModal( const Link& rEndDialogHdl )
{
    aEndDlgHdl = rEndDialogHdl;
    pDlg->StartExecuteModal(
        LINK( this, AbstractMailMergeWizard_Impl, EndDialogHdl ) );
}

long AbstractMailMergeWizard_Impl::GetResult()
{
    return pDlg->GetResult();
}

#ifdef DBG_UTIL
IMPL_LINK( AbstractMailMergeWizard_Impl, EndDialogHdl, SwMailMergeWizard*, pDialog )
#else
IMPL_LINK( AbstractMailMergeWizard_Impl, EndDialogHdl, SwMailMergeWizard*, EMPTYARG )
#endif
{
    DBG_ASSERT( pDialog == pDlg, "wrong dialog passed to EndDialogHdl!" );

    aEndDlgHdl.Call( this );
    aEndDlgHdl = Link();

    return 0L;
}

void AbstractMailMergeWizard_Impl::SetReloadDocument(const String& rURL)
{
    pDlg->SetReloadDocument(rURL);
}
const String&       AbstractMailMergeWizard_Impl::GetReloadDocument() const
{
    return pDlg->GetReloadDocument();
}
BOOL AbstractMailMergeWizard_Impl::ShowPage( USHORT nLevel )
{
    return pDlg->skipUntil(nLevel);
}
sal_uInt16 AbstractMailMergeWizard_Impl::GetRestartPage() const
{
    return pDlg->GetRestartPage();
}

//-------------- SwAbstractDialogFactory implementation--------------

AbstractSwWordCountDialog* SwAbstractDialogFactory_Impl::CreateSwWordCountDialog(Window* pParent)
{
    SwWordCountDialog* pDlg = new SwWordCountDialog( pParent );
    return new AbstractSwWordCountDialog_Impl( pDlg );
}

//add for SwInsertAbstractDlg begin
AbstractSwInsertAbstractDlg * SwAbstractDialogFactory_Impl::CreateSwInsertAbstractDlg( Window* pParent,
                                                                                      int nResId )
{
    SwInsertAbstractDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSERT_ABSTRACT :
            pDlg = new SwInsertAbstractDlg( pParent);
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwInsertAbstractDlg_Impl( pDlg );
    return 0;
}
//add for SwInsertAbstractDlg end

AbstractSfxSingleTabDialog* SwAbstractDialogFactory_Impl::CreateSfxSingleTabDialog( Window* pParent,
                                                                                 SfxItemSet& rSet,
                                                                                int nResId
                                                                                )
{
    SfxSingleTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_ADDR :
            pDlg = new SwAddrDlg( pParent, rSet );
            break;
        case DLG_SWDROPCAPS :
            pDlg = new SwDropCapsDlg( pParent, rSet );
            break;
        case RC_SWDLG_BACKGROUND :
            pDlg = new SwBackgroundDlg( pParent, rSet );
            break;
        case RC_DLG_SWNUMFMTDLG :
            pDlg = new SwNumFmtDlg( pParent, rSet );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSfxSingleTabDialog_Impl( pDlg );
    return 0;
}

// add for SwAsciiFilterDlg begin
AbstractSwAsciiFilterDlg* SwAbstractDialogFactory_Impl::CreateSwAsciiFilterDlg( Window* pParent,
                                                                               SwDocShell& rDocSh,
                                                                                SvStream* pStream,
                                                                                int nResId )
{
    SwAsciiFilterDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_ASCII_FILTER :
            pDlg = new SwAsciiFilterDlg( pParent, rDocSh, pStream );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwAsciiFilterDlg_Impl( pDlg );
    return 0;
}

//add for SwAsciiFilterDlg end

// add for SwInsertBookmarkDlg begin
VclAbstractDialog* SwAbstractDialogFactory_Impl::CreateSwInsertBookmarkDlg( Window *pParent,
                                                                           SwWrtShell &rSh, 
                                                                           SfxRequest& rReq, 
                                                                           int nResId )
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSERT_BOOKMARK :
            pDlg = new SwInsertBookmarkDlg( pParent, rSh, rReq );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

//add for SwInsertBookmarkDlg end

//add for SwBreakDlg begin

AbstractSwBreakDlg * SwAbstractDialogFactory_Impl::CreateSwBreakDlg ( Window *pParent,
                                                                     SwWrtShell &rSh,
                                                                     int nResId )
{
    SwBreakDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_BREAK :
            pDlg = new SwBreakDlg( pParent, rSh );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwBreakDlg_Impl( pDlg );
    return 0;
}
//add for SwBreakDlg end

// add for SwChangeDBDlg begin
VclAbstractDialog	* SwAbstractDialogFactory_Impl::CreateSwChangeDBDlg( SwView& rVw, int nResId  )
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_CHANGE_DB :
            pDlg = new SwChangeDBDlg( rVw );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

// add for SwChangeDBDlg end

// add for SwCharDlg begin
SfxAbstractTabDialog *  SwAbstractDialogFactory_Impl::CreateSwCharDlg(Window* pParent, SwView& pVw, const SfxItemSet& rCoreSet, int nResId, // add for SwCharDlg
                                                const String* pFmtStr , BOOL bIsDrwTxtDlg )
{

    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_CHAR :
            pDlg = new SwCharDlg( pParent, pVw, rCoreSet, pFmtStr, bIsDrwTxtDlg );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;

}
// add for SwCharDlg end

//add for SwConvertTableDlg begin
AbstractSwConvertTableDlg* SwAbstractDialogFactory_Impl::CreateSwConvertTableDlg ( 
                                    SwView& rView,int nResId, bool bToTable )
{
    SwConvertTableDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_CONV_TEXT_TABLE :
            pDlg = new SwConvertTableDlg( rView, bToTable );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwConvertTableDlg_Impl( pDlg );
    return 0;
}

//add for  SwConvertTableDlg end

//add for SwCaptionDialog begin
VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateSwCaptionDialog ( Window *pParent, SwView &rV,int nResId)
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_CAPTION :
            pDlg = new SwCaptionDialog( pParent, rV );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}
//add for SwCaptionDialog end

//add for  SwInsertDBColAutoPilot begin

AbstractSwInsertDBColAutoPilot* SwAbstractDialogFactory_Impl::CreateSwInsertDBColAutoPilot( SwView& rView, // add for SwInsertDBColAutoPilot
        uno::Reference< sdbc::XDataSource> rxSource,
        uno::Reference<sdbcx::XColumnsSupplier> xColSupp,
        const SwDBData& rData,  int nResId)
{
    SwInsertDBColAutoPilot* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_AP_INSERT_DB_SEL :
            pDlg = new SwInsertDBColAutoPilot( rView, rxSource, xColSupp, rData );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwInsertDBColAutoPilot_Impl( pDlg );
    return 0;
}
//add for  SwInsertDBColAutoPilot end
//add for  SwFootNoteOptionDlg begin
SfxAbstractTabDialog *  SwAbstractDialogFactory_Impl::CreateSwFootNoteOptionDlg( Window *pParent, SwWrtShell &rSh,int nResId)
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_DOC_FOOTNOTE :
            pDlg = new SwFootNoteOptionDlg( pParent, rSh );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}
//add for  SwFootNoteOptionDlg begin


//add for DropDownFieldDialog begin

AbstractDropDownFieldDialog *  SwAbstractDialogFactory_Impl::CreateDropDownFieldDialog ( Window *pParent, SwWrtShell &rSh, //add for DropDownFieldDialog
                                SwField* pField,int nResId, BOOL bNextButton )
{
    sw::DropDownFieldDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FLD_DROPDOWN :
            pDlg = new sw::DropDownFieldDialog( pParent, rSh, pField, bNextButton );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractDropDownFieldDialog_Impl( pDlg );
    return 0;
}
//add for DropDownFieldDialog end

//add for SwEnvDlg begin
SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwEnvDlg ( Window* pParent, const SfxItemSet& rSet,
                                                                 SwWrtShell* pWrtSh, Printer* pPrt, 
                                                                 BOOL bInsert,int nResId ) //add for SwEnvDlg
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_ENV :
            pDlg = new SwEnvDlg( pParent, rSet, pWrtSh,pPrt, bInsert  );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}
//add for SwEnvDlg end

//add for SwLabDlg begin

AbstarctSwLabDlg* SwAbstractDialogFactory_Impl::CreateSwLabDlg ( Window* pParent, const SfxItemSet& rSet, //add for SwLabDlg
                                                     SwNewDBMgr* pNewDBMgr, BOOL bLabel,int nResId  )
{
    SwLabDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_LAB :
            pDlg = new SwLabDlg( pParent, rSet, pNewDBMgr,bLabel  );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstarctSwLabDlg_Impl( pDlg );
    return 0;
}

SwLabDlgMethod SwAbstractDialogFactory_Impl::GetSwLabDlgStaticMethod ()
{
    return SwLabDlg::UpdateFieldInformation;
}

//add for SwLabDlg end

//add for SwParaDlg begin
SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwParaDlg ( Window *pParent, SwView& rVw,
                                                    const SfxItemSet& rCoreSet	,
                                                    BYTE nDialogMode,
                                                    int nResId,
                                                    const String *pCollName,
                                                    BOOL bDraw , UINT16 nDefPage)
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_DRAWPARA :
        case DLG_PARA :
            pDlg = new SwParaDlg( pParent, rVw, rCoreSet,nDialogMode, pCollName, bDraw, nDefPage );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}
//add for SwParaDlg end

//add for SwTableHeightDlg SwSortDlg   begin
VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateVclAbstractDialog ( Window *pParent, SwWrtShell &rSh, int nResId )
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_ROW_HEIGHT :
            pDlg = new SwTableHeightDlg( pParent, rSh);
            break;

        case DLG_SORTING :
            pDlg = new SwSortDlg( pParent, rSh);
            break;
        case DLG_COLUMN :
            pDlg = new SwColumnDlg( pParent, rSh );
            break;
        case DLG_EDIT_AUTHMARK :
            pDlg = new SwAuthMarkModalDlg( pParent, rSh );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}
//add for SwTableHeightDlg SwSortDlg ,end
//add for SwSplitTblDlg  begin
AbstractSplitTableDialog * SwAbstractDialogFactory_Impl::CreateSplitTblDialog ( Window *pParent, SwWrtShell &rSh )
{
    return new AbstractSplitTableDialog_Impl( new SwSplitTblDlg( pParent, rSh) );
}
//add SwSplitTblDlg end

//add for SwSelGlossaryDlg begin
AbstarctSwSelGlossaryDlg * SwAbstractDialogFactory_Impl::CreateSwSelGlossaryDlg ( Window * pParent, const String &rShortName, int nResId )
{
    SwSelGlossaryDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_SEL_GLOS :
            pDlg = new SwSelGlossaryDlg( pParent, rShortName);
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstarctSwSelGlossaryDlg_Impl( pDlg );
    return 0;
}

//add for SwSelGlossaryDlg end

//add for SwAutoFormatDlg begin
AbstractSwAutoFormatDlg * SwAbstractDialogFactory_Impl::CreateSwAutoFormatDlg( Window* pParent, SwWrtShell* pShell, 
                                                                                    int nResId,
                                                                                    BOOL bSetAutoFmt,
                                                                                    const SwTableAutoFmt* pSelFmt )
{
    SwAutoFormatDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_AUTOFMT_TABLE :
            pDlg = new SwAutoFormatDlg( pParent, pShell,bSetAutoFmt,pSelFmt);
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwAutoFormatDlg_Impl( pDlg );
    return 0;
}
//add for SwAutoFormatDlg end

//add for SwBorderDlg begin
AbstractSfxSingleTabDialog * SwAbstractDialogFactory_Impl::CreateSwBorderDlg (Window* pParent, SfxItemSet& rSet, USHORT nType,int nResId )
{
    SfxSingleTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_SWBORDERDLG :
            pDlg = new SwBorderDlg( pParent, rSet, nType );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSfxSingleTabDialog_Impl( pDlg );
    return 0;
}
//add for SwBorderDlg end

//add for SwWrapDlg begin
AbstractSfxSingleTabDialog * SwAbstractDialogFactory_Impl::CreateSwWrapDlg ( Window* pParent, SfxItemSet& rSet, SwWrtShell* pSh, BOOL bDrawMode, int nResId ) 
{
    SfxSingleTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_SWWRAPDLG :
            pDlg = new SwWrapDlg( pParent, rSet, pSh, bDrawMode );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSfxSingleTabDialog_Impl( pDlg );
    return 0;
}
//add for SwWrapDlg end

//add for SwTableWidthDlg begin
VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateSwTableWidthDlg ( Window *pParent, SwTableFUNC &rFnc , int nResId )
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_COL_WIDTH :
            pDlg = new SwTableWidthDlg( pParent, rFnc);
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}
//add for SwTableWidthDlg end

 //add for SwTableTabDlg begin
SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwTableTabDlg( Window* pParent, SfxItemPool& Pool,
                                                        const SfxItemSet* pItemSet, SwWrtShell* pSh,int nResId ) 
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FORMAT_TABLE :
            pDlg = new SwTableTabDlg( pParent, Pool, pItemSet,pSh );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}
 //add for SwTableTabDlg end

//add for SwFldDlg begin
AbstractSwFldDlg * SwAbstractDialogFactory_Impl::CreateSwFldDlg ( SfxBindings* pB, SwChildWinWrapper* pCW, Window *pParent, int nResId )
{
    SwFldDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FLD_INSERT :
            pDlg = new SwFldDlg( pB, pCW,pParent);
            break;

        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwFldDlg_Impl( pDlg );
    return 0;
}
//add for SwFldDlg end

//add for SwFldEditDlg begin
AbstractSfxSingleTabDialog*	 SwAbstractDialogFactory_Impl::CreateSwFldEditDlg ( SwView& rVw, int nResId )
{
    SfxSingleTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case RC_DLG_SWFLDEDITDLG :
            pDlg = new SwFldEditDlg( rVw );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSfxSingleTabDialog_Impl( pDlg );
    return 0;
}
//add for SwFldEditDlg

//add for SwRenameXNamedDlg begin
AbstractSwRenameXNamedDlg * SwAbstractDialogFactory_Impl::CreateSwRenameXNamedDlg( Window* pParent,
                                                                STAR_REFERENCE( container::XNamed ) & xNamed,
                                                                STAR_REFERENCE( container::XNameAccess ) & xNameAccess,int nResId )
{
    SwRenameXNamedDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_RENAME_XNAMED :
            pDlg = new SwRenameXNamedDlg( pParent,xNamed, xNameAccess);
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwRenameXNamedDlg_Impl( pDlg );
    return 0;
}

//add for SwRenameXNamedDlg end

//add for SwModalRedlineAcceptDlg begin
AbstractSwModalRedlineAcceptDlg * SwAbstractDialogFactory_Impl::CreateSwModalRedlineAcceptDlg ( Window *pParent, int nResId )
{
    SwModalRedlineAcceptDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MOD_REDLINE_ACCEPT :
            pDlg = new SwModalRedlineAcceptDlg( pParent );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractSwModalRedlineAcceptDlg_Impl( pDlg );
    return 0;
}
//add for SwModalRedlineAcceptDlg end

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateSwVclDialog( int nResId, 
                                                Window* pParent, BOOL& rWithPrev ) //add for SwMergeTblDlg
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MERGE_TABLE :
            pDlg = new SwMergeTblDlg( pParent, rWithPrev );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}
SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateFrmTabDialog( int nResId,
                                                SfxViewFrame *pFrame, Window *pParent,
                                                const SfxItemSet& rCoreSet,
                                                BOOL			bNewFrm,
                                                USHORT			nResType,
                                                BOOL			bFmt,
                                                UINT16			nDefPage,
                                                const String* 	pFmtStr ) //add for SwFrmDlg
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FRM_GRF :
        case DLG_FRM_OLE :
        case DLG_FRM_STD :
            pDlg = new SwFrmDlg( pFrame, pParent, rCoreSet, bNewFrm, nResType, bFmt, nDefPage, pFmtStr );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateTemplateDialog( int nResId,
                                                Window*             pParent,
                                                SfxStyleSheetBase&  rBase,
                                                USHORT              nRegion,
                                                BOOL 				bColumn,
                                                SwWrtShell* 		pActShell,
                                                BOOL 				bNew ) //add for SwTemplateDlg
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_TEMPLATE_BASE :
            pDlg = new SwTemplateDlg( pParent, rBase, nRegion, bColumn, pActShell, bNew );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}

AbstractGlossaryDlg* SwAbstractDialogFactory_Impl::CreateGlossaryDlg( int nResId,
                                                SfxViewFrame* pViewFrame, 
                                                SwGlossaryHdl* pGlosHdl, 
                                                SwWrtShell *pWrtShell) //add for SwGlossaryDlg
{
    SwGlossaryDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_RENAME_GLOS :
            pDlg = new SwGlossaryDlg( pViewFrame, pGlosHdl, pWrtShell );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractGlossaryDlg_Impl( pDlg );
    return 0;
}

AbstractFldInputDlg* SwAbstractDialogFactory_Impl::CreateFldInputDlg( int nResId,
                                                Window *pParent, SwWrtShell &rSh,
                                                SwField* pField, BOOL bNextButton ) //add for SwFldInputDlg
{
    SwFldInputDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_FLD_INPUT :
            pDlg = new SwFldInputDlg( pParent, rSh, pField, bNextButton );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractFldInputDlg_Impl( pDlg );
    return 0;
}

AbstractInsFootNoteDlg* SwAbstractDialogFactory_Impl::CreateInsFootNoteDlg( int nResId,
                                                Window * pParent, SwWrtShell &rSh, BOOL bEd ) //add for SwInsFootNoteDlg
{
    SwInsFootNoteDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INS_FOOTNOTE :
            pDlg = new SwInsFootNoteDlg( pParent, rSh, bEd );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractInsFootNoteDlg_Impl( pDlg );
    return 0;
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateVclSwViewDialog( int nResId, 
                                            SwView& rView, BOOL bCol ) //add for SwInsRowColDlg, SwLineNumberingDlg
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INS_ROW_COL :
            pDlg = new SwInsRowColDlg( rView, bCol );
            break;
        case DLG_LINE_NUMBERING :
            pDlg = new SwLineNumberingDlg( &rView );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}

AbstractInsertGrfRulerDlg * SwAbstractDialogFactory_Impl::CreateInsertGrfRulerDlg( int nResId, 
                                            Window * pParent ) //add for SwInsertGrfRulerDlg
{
    SwInsertGrfRulerDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSERT_RULER :
            pDlg = new SwInsertGrfRulerDlg( pParent );
            break;
        default:
            break;
    }

    if ( pDlg )
        return new AbstractInsertGrfRulerDlg_Impl( pDlg );
    return 0;
}

AbstractInsTableDlg * SwAbstractDialogFactory_Impl::CreateInsTableDlg( int nResId,
                                            SwView& rView ) //add for SwInsTableDlg
{
    SwInsTableDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSERT_TABLE :
            pDlg = new SwInsTableDlg( rView );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractInsTableDlg_Impl( pDlg );
    return 0;
}

AbstractJavaEditDialog * SwAbstractDialogFactory_Impl::CreateJavaEditDialog( int nResId,
                                                Window* pParent, SwWrtShell* pWrtSh ) //add for SwJavaEditDialog
{
    SwJavaEditDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_JAVAEDIT :
            pDlg = new SwJavaEditDialog( pParent, pWrtSh );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractJavaEditDialog_Impl( pDlg );
    return 0;
}

AbstractMailMergeDlg * SwAbstractDialogFactory_Impl::CreateMailMergeDlg( int nResId,
                                                Window* pParent, SwWrtShell& rSh,
                                                 const String& rSourceName,
                                                const String& rTblName,
                                                sal_Int32 nCommandType,
                                                const uno::Reference< sdbc::XConnection>& xConnection,
                                                uno::Sequence< uno::Any >* pSelection ) //add for SwMailMergeDlg
{
    SwMailMergeDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MAILMERGE :
            pDlg = new SwMailMergeDlg( pParent, rSh, rSourceName, rTblName, nCommandType, xConnection, pSelection );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMailMergeDlg_Impl( pDlg );
    return 0;
}
AbstractMailMergeCreateFromDlg * SwAbstractDialogFactory_Impl::CreateMailMergeCreateFromDlg( int nResId,
                                                                    Window* pParent ) //add for SwMailMergeCreateFromDlg
{
    SwMailMergeCreateFromDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MERGE_CREATE :
            pDlg = new SwMailMergeCreateFromDlg( pParent );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMailMergeCreateFromDlg_Impl( pDlg );
    return 0;
}
AbstractMailMergeFieldConnectionsDlg * SwAbstractDialogFactory_Impl::CreateMailMergeFieldConnectionsDlg( int nResId,
                                                            Window* pParent ) //add for SwMailMergeFieldConnectionsDlg
{
    SwMailMergeFieldConnectionsDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MERGE_FIELD_CONNECTIONS :
            pDlg = new SwMailMergeFieldConnectionsDlg( pParent );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMailMergeFieldConnectionsDlg_Impl( pDlg );
    return 0;
}

VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateMultiTOXMarkDlg( int nResId, 
                                                Window* pParent, SwTOXMgr &rTOXMgr ) //add for SwMultiTOXMarkDlg
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MULTMRK :
            pDlg = new SwMultiTOXMarkDlg( pParent, rTOXMgr );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}
SfxAbstractTabDialog* SwAbstractDialogFactory_Impl::CreateSwTabDialog( int nResId,
                                                Window* pParent,
                                                const SfxItemSet* pSwItemSet,
                                                SwWrtShell & rWrtSh ) //add for SwSvxNumBulletTabDialog, SwOutlineTabDialog
{
    SfxTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_SVXTEST_NUM_BULLET :
            pDlg = new SwSvxNumBulletTabDialog( pParent, pSwItemSet, rWrtSh );
            break;
        case DLG_TAB_OUTLINE :
            pDlg = new SwOutlineTabDialog( pParent, pSwItemSet, rWrtSh );
            break;

        default:
            break;
    }
    if ( pDlg )
        return new AbstractTabDialog_Impl( pDlg );
    return 0;
}
AbstractMultiTOXTabDialog * SwAbstractDialogFactory_Impl::CreateMultiTOXTabDialog( int nResId, 
                                                Window* pParent, const SfxItemSet& rSet,
                                                SwWrtShell &rShell,
                                                SwTOXBase* pCurTOX, USHORT nToxType,
                                                BOOL bGlobal ) //add for SwMultiTOXTabDialog
{
    SwMultiTOXTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_MULTI_TOX :
            pDlg = new SwMultiTOXTabDialog( pParent, rSet, rShell, pCurTOX, nToxType, bGlobal );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractMultiTOXTabDialog_Impl( pDlg );
    return 0;
}
AbstractEditRegionDlg * SwAbstractDialogFactory_Impl::CreateEditRegionDlg( int nResId, 
                                                Window* pParent, SwWrtShell& rWrtSh ) //add for SwEditRegionDlg
{
    SwEditRegionDlg* pDlg=NULL;
    switch ( nResId )
    {
        case MD_EDIT_REGION :
            pDlg = new SwEditRegionDlg( pParent, rWrtSh );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractEditRegionDlg_Impl( pDlg );
    return 0;
}
AbstractInsertSectionTabDialog * SwAbstractDialogFactory_Impl::CreateInsertSectionTabDialog( int nResId, 
                                                    Window* pParent, const SfxItemSet& rSet, SwWrtShell& rSh) //add for SwInsertSectionTabDialog
{
    SwInsertSectionTabDialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSERT_SECTION :
            pDlg = new SwInsertSectionTabDialog( pParent, rSet, rSh );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractInsertSectionTabDialog_Impl( pDlg );
    return 0;
}

AbstractMarkFloatDlg * SwAbstractDialogFactory_Impl::CreateIndexMarkFloatDlg( int nResId, 
                                                    SfxBindings* pBindings,
                                                       SfxChildWindow* pChild,
                                                       Window *pParent,
                                                    SfxChildWinInfo* pInfo,
                                                       sal_Bool bNew ) //add for SwIndexMarkFloatDlg
{
    SwIndexMarkFloatDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSIDXMARK_CJK :
        case DLG_INSIDXMARK :
            pDlg = new SwIndexMarkFloatDlg( pBindings, pChild, pParent, pInfo, bNew );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractIndexMarkFloatDlg_Impl( pDlg );
    return 0;
}

AbstractMarkFloatDlg * SwAbstractDialogFactory_Impl::CreateAuthMarkFloatDlg( int nResId, 
                                                    SfxBindings* pBindings,
                                                       SfxChildWindow* pChild,
                                                       Window *pParent,
                                                    SfxChildWinInfo* pInfo,
                                                       sal_Bool bNew ) //add for SwAuthMarkFloatDlg
{
    SwAuthMarkFloatDlg* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_INSAUTHMARK :
            pDlg = new SwAuthMarkFloatDlg( pBindings, pChild, pParent, pInfo, bNew );
            break;
        default:
            break;
    }
    if ( pDlg )
        return new AbstractAuthMarkFloatDlg_Impl( pDlg );
    return 0;
}

//add for SwIndexMarkModalDlg begin
VclAbstractDialog * SwAbstractDialogFactory_Impl::CreateIndexMarkModalDlg( int nResId, 
                                                Window *pParent, SwWrtShell& rSh, SwTOXMark* pCurTOXMark ) //add for SwIndexMarkModalDlg
{
    Dialog* pDlg=NULL;
    switch ( nResId )
    {
        case DLG_EDIT_IDXMARK_CJK :
        case DLG_EDIT_IDXMARK :
            pDlg = new SwIndexMarkModalDlg( pParent, rSh, pCurTOXMark );
            break;

        default:
            break;
    }

    if ( pDlg )
        return new VclAbstractDialog_Impl( pDlg );
    return 0;
}
//add for SwIndexMarkModalDlg end
AbstractMailMergeWizard*    SwAbstractDialogFactory_Impl::CreateMailMergeWizard(
                                    SwView& rView, SwMailMergeConfigItem& rConfigItem)
{
    return new AbstractMailMergeWizard_Impl( new SwMailMergeWizard(rView, rConfigItem));
}

//add for static func in SwGlossaryDlg
GlossaryGetCurrGroup	SwAbstractDialogFactory_Impl::GetGlossaryCurrGroupFunc( USHORT nId )
{
    switch ( nId )
    {
        case DLG_RENAME_GLOS :
            return SwGlossaryDlg::GetCurrGroup;
        default:
            break;
    }
    return 0;
}
GlossarySetActGroup	SwAbstractDialogFactory_Impl::SetGlossaryActGroupFunc( USHORT nId )
{
    switch ( nId )
    {
        case DLG_RENAME_GLOS :
            return SwGlossaryDlg::SetActGroup;
        default:
            break;
    }
    return 0;
}

//------------------ Factories for TabPages
CreateTabPage SwAbstractDialogFactory_Impl::GetTabPageCreatorFunc( USHORT nId )
{
    CreateTabPage pRet = 0;
    switch ( nId )
    {
        case TP_OPTCOMPATIBILITY_PAGE :
        case RID_SW_TP_OPTCOMPATIBILITY_PAGE :
            pRet = SwCompatibilityOptPage::Create;
            break;
        case TP_OPTLOAD_PAGE :
        case RID_SW_TP_OPTLOAD_PAGE :
            pRet = SwLoadOptPage::Create;
            break;
        case TP_OPTCAPTION_PAGE:
        case RID_SW_TP_OPTCAPTION_PAGE:
            return SwCaptionOptPage::Create;
        case TP_CONTENT_OPT :
        case RID_SW_TP_CONTENT_OPT:
        case RID_SW_TP_HTML_CONTENT_OPT:
            pRet = SwContentOptPage::Create;
            break;
        case TP_OPTSHDWCRSR :
        case RID_SW_TP_OPTSHDWCRSR:
        case RID_SW_TP_HTML_OPTSHDWCRSR:
            pRet = SwShdwCrsrOptionsTabPage::Create;
            break;
        case RID_SW_TP_REDLINE_OPT :
        case TP_REDLINE_OPT :
            pRet = SwRedlineOptionsTabPage::Create;
            break;
        case RID_SW_TP_OPTTEST_PAGE :
        case TP_OPTTEST_PAGE :
#ifndef  PRODUCT
            pRet = SwTestTabPage::Create;
#endif
            break;
        case TP_OPTPRINT_PAGE :
        case RID_SW_TP_HTML_OPTPRINT_PAGE:
        case RID_SW_TP_OPTPRINT_PAGE:
            pRet = SwAddPrinterTabPage::Create;
            break;
        case TP_STD_FONT :
        case RID_SW_TP_STD_FONT:
        case RID_SW_TP_STD_FONT_CJK:
        case RID_SW_TP_STD_FONT_CTL:
            pRet = SwStdFontTabPage::Create;
            break;
        case TP_OPTTABLE_PAGE :
        case RID_SW_TP_HTML_OPTTABLE_PAGE:
        case RID_SW_TP_OPTTABLE_PAGE:
            pRet = SwTableOptionsTabPage::Create;
            break;
        case TP_DOC_STAT :
            pRet = SwDocStatPage::Create;
            break;
        case RID_SW_TP_MAILCONFIG:
            pRet = SwMailConfigPage::Create;
        break;
    }

    return pRet;
}

GetTabPageRanges SwAbstractDialogFactory_Impl::GetTabPageRangesFunc( USHORT nId )
{
    switch ( nId )
    {
    case 1 : //RID_SVXPAGE_TEXTANIMATION :
            //return SvxTextAnimationPage::GetRanges;
            break;
        default:
            break;
    }

    return 0;
}
