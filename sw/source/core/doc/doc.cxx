/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include <doc.hxx>
#include <hintids.hxx>

#include <tools/shl.hxx>
#include <tools/globname.hxx>
#include <svx/svxids.hrc>
#include <com/sun/star/i18n/WordType.hdl>
#include <com/sun/star/i18n/ForbiddenCharacters.hdl>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/document/XDocumentProperties.hpp>
#include <comphelper/processfactory.hxx>
#include <tools/urlobj.hxx>
#include <tools/poly.hxx>
#include <tools/multisel.hxx>
#include <rtl/ustring.hxx>
#include <vcl/virdev.hxx>
#include <svl/itemiter.hxx>
#include <unotools/syslocale.hxx>
#include <sfx2/printer.hxx>
#include <editeng/keepitem.hxx>
#include <editeng/cscoitem.hxx>
#include <editeng/brkitem.hxx>
#include <sfx2/linkmgr.hxx>
#include <editeng/forbiddencharacterstable.hxx>
#include <svx/svdmodel.hxx>
#include <unotools/charclass.hxx>
#include <unotools/localedatawrapper.hxx>

#include <swmodule.hxx>
#include <fmtpdsc.hxx>
#include <fmtanchr.hxx>
#include <fmtrfmrk.hxx>
#include <fmtinfmt.hxx>
#include <fmtfld.hxx>
#include <txtfld.hxx>
#include <dbfld.hxx>
#include <txtinet.hxx>
#include <txtrfmrk.hxx>
#include <frmatr.hxx>
#include <linkenum.hxx>
#include <errhdl.hxx>
#include <pagefrm.hxx>
#include <rootfrm.hxx>
#include <swtable.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <swundo.hxx>			// fuer die UndoIds
#include <undobj.hxx>
#include <pagedesc.hxx> //DTor
#include <breakit.hxx>
#include <ndole.hxx>
#include <ndgrf.hxx>
#include <rolbck.hxx>			// Undo-Attr
#include <doctxm.hxx>			// fuer die Verzeichnisse
#include <grfatr.hxx>
#include <poolfmt.hxx>			// PoolVorlagen-Id's
#include <mvsave.hxx>			// fuer Server-Funktionalitaet
#include <SwGrammarMarkUp.hxx>
#include <scriptinfo.hxx>
#include <acorrect.hxx>			// Autokorrektur
#include <mdiexp.hxx>	   		// Statusanzeige
#include <docstat.hxx>
#include <docary.hxx>
#include <redline.hxx>
#include <fldupde.hxx>
#include <swbaslnk.hxx>
#include <printdata.hxx>
#include <swprtopt.hxx>
#include <cmdid.h>              // fuer den dflt - Printer in SetJob
#include <statstr.hrc>      	// StatLine-String
#include <comcore.hrc>
#include <SwUndoTOXChange.hxx>
#include <SwUndoFmt.hxx>
#include <unocrsr.hxx>
#include <docsh.hxx>
#include <docfld.hxx>           // _SetGetExpFld
#include <docufld.hxx>          // SwPostItField
#include <viewsh.hxx>
#include <shellres.hxx>
#include <txtfrm.hxx>

#include <vector>
#include <map>

#include <osl/diagnose.h>
#include <osl/interlck.h>

/* @@@MAINTAINABILITY-HORROR@@@
   Probably unwanted dependency on SwDocShell
*/
// --> OD 2005-08-29 #125370#
#include <layouter.hxx>
// <--

using namespace ::com::sun::star;
using ::rtl::OUString;


// Seiten-Deskriptoren
SV_IMPL_PTRARR(SwPageDescs,SwPageDescPtr);
// Verzeichnisse
SV_IMPL_PTRARR( SwTOXTypes, SwTOXTypePtr )
// FeldTypen
SV_IMPL_PTRARR( SwFldTypes, SwFldTypePtr)

/** IInterface
*/
sal_Int32 SwDoc::acquire()
{
    OSL_ASSERT(mReferenceCount >= 0 && "Negative reference count detected! This is a sign for unbalanced acquire/release calls.");
    return osl_incrementInterlockedCount(&mReferenceCount);
}

sal_Int32 SwDoc::release()
{
    OSL_PRECOND(mReferenceCount >= 1, "Object is already released! Releasing it again leads to a negative reference count.");
    return osl_decrementInterlockedCount(&mReferenceCount);
}

sal_Int32 SwDoc::getReferenceCount() const
{
    OSL_ASSERT(mReferenceCount >= 0 && "Negative reference count detected! This is a sign for unbalanced acquire/release calls.");
    return mReferenceCount;
}

/** IDocumentSettingAccess
*/
bool SwDoc::get(/*[in]*/ DocumentSettingId id) const
{
    switch (id)
    {
        // COMPATIBILITY FLAGS START
        case PARA_SPACE_MAX: return mbParaSpaceMax; //(n8Dummy1 & DUMMY_PARASPACEMAX);
        case PARA_SPACE_MAX_AT_PAGES: return mbParaSpaceMaxAtPages; //(n8Dummy1 & DUMMY_PARASPACEMAX_AT_PAGES);
        case TAB_COMPAT: return mbTabCompat; //(n8Dummy1 & DUMMY_TAB_COMPAT);
        case ADD_FLY_OFFSETS: return mbAddFlyOffsets; //(n8Dummy2 & DUMMY_ADD_FLY_OFFSETS);
        case ADD_EXT_LEADING: return mbAddExternalLeading; //(n8Dummy2 & DUMMY_ADD_EXTERNAL_LEADING);
        case USE_VIRTUAL_DEVICE: return mbUseVirtualDevice; //(n8Dummy1 & DUMMY_USE_VIRTUAL_DEVICE);
        case USE_HIRES_VIRTUAL_DEVICE: return mbUseHiResolutionVirtualDevice; //(n8Dummy2 & DUMMY_USE_HIRES_VIR_DEV);
        case OLD_NUMBERING: return mbOldNumbering;
        case OLD_LINE_SPACING: return mbOldLineSpacing;
        case ADD_PARA_SPACING_TO_TABLE_CELLS: return mbAddParaSpacingToTableCells;
        case USE_FORMER_OBJECT_POS: return mbUseFormerObjectPos;
        case USE_FORMER_TEXT_WRAPPING: return mbUseFormerTextWrapping;
        case CONSIDER_WRAP_ON_OBJECT_POSITION: return mbConsiderWrapOnObjPos;
        case DO_NOT_JUSTIFY_LINES_WITH_MANUAL_BREAK: return mbDoNotJustifyLinesWithManualBreak;
        case IGNORE_FIRST_LINE_INDENT_IN_NUMBERING: return mbIgnoreFirstLineIndentInNumbering;
        case OUTLINE_LEVEL_YIELDS_OUTLINE_RULE: return mbOutlineLevelYieldsOutlineRule;
        case TABLE_ROW_KEEP: return mbTableRowKeep;
        case IGNORE_TABS_AND_BLANKS_FOR_LINE_CALCULATION: return mbIgnoreTabsAndBlanksForLineCalculation;
        case DO_NOT_CAPTURE_DRAW_OBJS_ON_PAGE: return mbDoNotCaptureDrawObjsOnPage;
        // --> OD 2006-08-25 #i68949#
        case CLIP_AS_CHARACTER_ANCHORED_WRITER_FLY_FRAME: return mbClipAsCharacterAnchoredWriterFlyFrames;
        // <--
        case UNIX_FORCE_ZERO_EXT_LEADING: return mbUnixForceZeroExtLeading;
        case USE_OLD_PRINTER_METRICS: return mbOldPrinterMetrics;
        case TABS_RELATIVE_TO_INDENT : return mbTabRelativeToIndent;
        case PROTECT_FORM: return mbProtectForm;
        // --> OD 2008-06-05 #i89181#
        case TAB_AT_LEFT_INDENT_FOR_PARA_IN_LIST: return mbTabAtLeftIndentForParagraphsInList;
        // <--
         // COMPATIBILITY FLAGS END

        case BROWSE_MODE: return mbBrowseMode;
        case HTML_MODE: return mbHTMLMode;
        case GLOBAL_DOCUMENT: return mbIsGlobalDoc;
        case GLOBAL_DOCUMENT_SAVE_LINKS: return mbGlblDocSaveLinks;
        case LABEL_DOCUMENT: return mbIsLabelDoc;
        case PURGE_OLE: return mbPurgeOLE;
        case KERN_ASIAN_PUNCTUATION: return mbKernAsianPunctuation;
        case DO_NOT_RESET_PARA_ATTRS_FOR_NUM_FONT: return mbDoNotResetParaAttrsForNumFont;
        default:
            ASSERT(false, "Invalid setting id");
    }
    return false;
}

void SwDoc::set(/*[in]*/ DocumentSettingId id, /*[in]*/ bool value)
{
    switch (id)
    {
        // COMPATIBILITY FLAGS START
        case PARA_SPACE_MAX:
            mbParaSpaceMax = value;
            break;
        case PARA_SPACE_MAX_AT_PAGES:
            mbParaSpaceMaxAtPages = value;
            break;
        case TAB_COMPAT:
            mbTabCompat = value;
            break;
        case ADD_FLY_OFFSETS:
            mbAddFlyOffsets = value;
            break;
        case ADD_EXT_LEADING:
            mbAddExternalLeading = value;
            break;
        case USE_VIRTUAL_DEVICE:
            mbUseVirtualDevice = value;
            break;
        case USE_HIRES_VIRTUAL_DEVICE:
            mbUseHiResolutionVirtualDevice = value;
            break;
        case OLD_NUMBERING:
            if (mbOldNumbering != value)
            {
                mbOldNumbering = value;

                const SwNumRuleTbl& rNmTbl = GetNumRuleTbl();
                for( USHORT n = 0; n < rNmTbl.Count(); ++n )
                    rNmTbl[n]->SetInvalidRule(TRUE);

                UpdateNumRule();

                if (pOutlineRule)
                {
                    pOutlineRule->Validate();
                    // --> OD 2005-10-21 - counting of phantoms depends on <IsOldNumbering()>
                    pOutlineRule->SetCountPhantoms( !mbOldNumbering );
                    // <--
                }
            }
            break;
        case OLD_LINE_SPACING:
            mbOldLineSpacing = value;
            break;
        case ADD_PARA_SPACING_TO_TABLE_CELLS:
            mbAddParaSpacingToTableCells = value;
            break;
        case USE_FORMER_OBJECT_POS:
            mbUseFormerObjectPos = value;
            break;
        case USE_FORMER_TEXT_WRAPPING:
            mbUseFormerTextWrapping = value;
            break;
        case CONSIDER_WRAP_ON_OBJECT_POSITION:
            mbConsiderWrapOnObjPos = value;
            break;
        case DO_NOT_JUSTIFY_LINES_WITH_MANUAL_BREAK:
            mbDoNotJustifyLinesWithManualBreak = value;
            break;
        case IGNORE_FIRST_LINE_INDENT_IN_NUMBERING:
            mbIgnoreFirstLineIndentInNumbering = value;
            break;

        case OUTLINE_LEVEL_YIELDS_OUTLINE_RULE:
            mbOutlineLevelYieldsOutlineRule = value;
            break;

        case TABLE_ROW_KEEP:
            mbTableRowKeep = value;
            break;

        case IGNORE_TABS_AND_BLANKS_FOR_LINE_CALCULATION:
            mbIgnoreTabsAndBlanksForLineCalculation = value;
            break;

        case DO_NOT_CAPTURE_DRAW_OBJS_ON_PAGE:
            mbDoNotCaptureDrawObjsOnPage = value;
            break;

        // --> OD 2006-08-25 #i68949#
        case CLIP_AS_CHARACTER_ANCHORED_WRITER_FLY_FRAME:
            mbClipAsCharacterAnchoredWriterFlyFrames = value;
            break;
        // <--
        case UNIX_FORCE_ZERO_EXT_LEADING:
            mbUnixForceZeroExtLeading = value;
            break;
    case PROTECT_FORM:
        mbProtectForm = value;
        break;

        case USE_OLD_PRINTER_METRICS:
            mbOldPrinterMetrics = value;
            break;
        case TABS_RELATIVE_TO_INDENT:
            mbTabRelativeToIndent = value;
        break;
        // --> OD 2008-06-05 #i89181#
        case TAB_AT_LEFT_INDENT_FOR_PARA_IN_LIST:
            mbTabAtLeftIndentForParagraphsInList = value;
        break;
        // <--
         // COMPATIBILITY FLAGS END

        case BROWSE_MODE:
            mbBrowseMode = value;
            break;
        case HTML_MODE:
            mbHTMLMode = value;
            break;
        case GLOBAL_DOCUMENT:
            mbIsGlobalDoc = value;
            break;
        case GLOBAL_DOCUMENT_SAVE_LINKS:
            mbGlblDocSaveLinks = value;
            break;
        case LABEL_DOCUMENT:
            mbIsLabelDoc = value;
            break;
        case PURGE_OLE:
            mbPurgeOLE = value;
            break;
        case KERN_ASIAN_PUNCTUATION:
            mbKernAsianPunctuation = value;
            break;
        case DO_NOT_RESET_PARA_ATTRS_FOR_NUM_FONT:
            mbDoNotResetParaAttrsForNumFont = value;
            break;
        default:
            ASSERT(false, "Invalid setting id");
    }
}

const i18n::ForbiddenCharacters*
    SwDoc::getForbiddenCharacters(/*[in]*/ USHORT nLang, /*[in]*/ bool bLocaleData ) const
{
    const i18n::ForbiddenCharacters* pRet = 0;
    if( xForbiddenCharsTable.isValid() )
        pRet = xForbiddenCharsTable->GetForbiddenCharacters( nLang, FALSE );
    if( bLocaleData && !pRet && pBreakIt )
        pRet = &pBreakIt->GetForbidden( (LanguageType)nLang );
    return pRet;
}

void SwDoc::setForbiddenCharacters(/*[in]*/ USHORT nLang,
                                   /*[in]*/ const i18n::ForbiddenCharacters& rFChars )
{
    if( !xForbiddenCharsTable.isValid() )
    {
        uno::Reference<
            lang::XMultiServiceFactory > xMSF =
                                    ::comphelper::getProcessServiceFactory();
        xForbiddenCharsTable = new SvxForbiddenCharactersTable( xMSF );
    }
    xForbiddenCharsTable->SetForbiddenCharacters( nLang, rFChars );
    if( pDrawModel )
    {
        pDrawModel->SetForbiddenCharsTable( xForbiddenCharsTable );
        if( !mbInReading )
            pDrawModel->ReformatAllTextObjects();
    }

    if( pLayout && !mbInReading )
    {
        pLayout->StartAllAction();
        pLayout->InvalidateAllCntnt();
        pLayout->EndAllAction();
    }
    SetModified();
}

vos::ORef<SvxForbiddenCharactersTable>& SwDoc::getForbiddenCharacterTable()
{
    if( !xForbiddenCharsTable.isValid() )
    {
        uno::Reference<
            lang::XMultiServiceFactory > xMSF =
                                    ::comphelper::getProcessServiceFactory();
        xForbiddenCharsTable = new SvxForbiddenCharactersTable( xMSF );
    }
    return xForbiddenCharsTable;
}

const vos::ORef<SvxForbiddenCharactersTable>& SwDoc::getForbiddenCharacterTable() const
{
    return xForbiddenCharsTable;
}

sal_uInt16 SwDoc::getLinkUpdateMode( /*[in]*/bool bGlobalSettings ) const
{
    sal_uInt16 nRet = nLinkUpdMode;
    if( bGlobalSettings && GLOBALSETTING == nRet )
        nRet = SW_MOD()->GetLinkUpdMode(get(IDocumentSettingAccess::HTML_MODE));
    return nRet;
}

void SwDoc::setLinkUpdateMode( /*[in]*/sal_uInt16 eMode )
{
    nLinkUpdMode = eMode;
}

SwFldUpdateFlags SwDoc::getFieldUpdateFlags( /*[in]*/bool bGlobalSettings ) const
{
    SwFldUpdateFlags eRet = eFldUpdMode;
    if( bGlobalSettings && AUTOUPD_GLOBALSETTING == eRet )
        eRet = SW_MOD()->GetFldUpdateFlags(get(IDocumentSettingAccess::HTML_MODE));
    return eRet;
}

void SwDoc::setFieldUpdateFlags(/*[in]*/SwFldUpdateFlags eMode )
{
    eFldUpdMode = eMode;
}

SwCharCompressType SwDoc::getCharacterCompressionType() const
{
    return eChrCmprType;
}

void SwDoc::setCharacterCompressionType( /*[in]*/SwCharCompressType n )
{
    if( eChrCmprType != n )
    {
        eChrCmprType = n;
        if( pDrawModel )
        {
            pDrawModel->SetCharCompressType( static_cast<UINT16>(n) );
            if( !mbInReading )
                pDrawModel->ReformatAllTextObjects();
        }

        if( pLayout && !mbInReading )
        {
            pLayout->StartAllAction();
            pLayout->InvalidateAllCntnt();
            pLayout->EndAllAction();
        }
        SetModified();
    }
}

/** IDocumentDeviceAccess
*/
SfxPrinter* SwDoc::getPrinter(/*[in]*/ bool bCreate ) const
{
    SfxPrinter* pRet = 0;
    if ( !bCreate || pPrt )
        pRet = pPrt;
    else
        pRet = &CreatePrinter_();

    return pRet;
}

void SwDoc::setPrinter(/*[in]*/ SfxPrinter *pP,/*[in]*/ bool bDeleteOld,/*[in]*/ bool bCallPrtDataChanged )
{
    if ( pP != pPrt )
    {
        if ( bDeleteOld )
            delete pPrt;
        pPrt = pP;

        if ( pDrawModel && !get( IDocumentSettingAccess::USE_VIRTUAL_DEVICE ) )
            pDrawModel->SetRefDevice( pPrt );
    }

    if ( bCallPrtDataChanged &&
         // --> FME 2005-01-21 #i41075# Do not call PrtDataChanged() if we do not
         // use the printer for formatting:
         !get(IDocumentSettingAccess::USE_VIRTUAL_DEVICE) )
        // <--
        PrtDataChanged();
}

VirtualDevice* SwDoc::getVirtualDevice(/*[in]*/ bool bCreate ) const
{
    VirtualDevice* pRet = 0;
    if ( !bCreate || pVirDev )
        pRet = pVirDev;
    else
        pRet = &CreateVirtualDevice_();

    return pRet;
}

void SwDoc::setVirtualDevice(/*[in]*/ VirtualDevice* pVd,/*[in]*/ bool bDeleteOld, /*[in]*/ bool )
{
    if ( pVirDev != pVd )
    {
        if ( bDeleteOld )
            delete pVirDev;
        pVirDev = pVd;

        if ( pDrawModel && get( IDocumentSettingAccess::USE_VIRTUAL_DEVICE ) )
            pDrawModel->SetRefDevice( pVirDev );
    }
}

OutputDevice* SwDoc::getReferenceDevice(/*[in]*/ bool bCreate ) const
{
    OutputDevice* pRet = 0;
    if ( !get(IDocumentSettingAccess::USE_VIRTUAL_DEVICE) )
    {
        pRet = getPrinter( bCreate );

        if ( bCreate && !pPrt->IsValid() )
        {
            pRet = getVirtualDevice( sal_True );
        }
    }
    else
    {
        pRet = getVirtualDevice( bCreate );
    }

    return pRet;
}

void SwDoc::setReferenceDeviceType(/*[in]*/ bool bNewVirtual,/*[in]*/ bool bNewHiRes )
{
    if ( get(IDocumentSettingAccess::USE_VIRTUAL_DEVICE) != bNewVirtual ||
         get(IDocumentSettingAccess::USE_HIRES_VIRTUAL_DEVICE) != bNewHiRes )
    {
        if ( bNewVirtual )
        {
            VirtualDevice* pMyVirDev = getVirtualDevice( true );
            if ( !bNewHiRes )
                pMyVirDev->SetReferenceDevice( VirtualDevice::REFDEV_MODE06 );
            else
                pMyVirDev->SetReferenceDevice( VirtualDevice::REFDEV_MODE_MSO1 );

            if( pDrawModel )
                pDrawModel->SetRefDevice( pMyVirDev );
        }
        else
        {
            // --> FME 2005-01-21 #i41075#
            // We have to take care that a printer exists before calling
            // PrtDataChanged() in order to prevent that PrtDataChanged()
            // triggers this funny situation:
            // getReferenceDevice()->getPrinter()->CreatePrinter_()
            // ->setPrinter()-> PrtDataChanged()
            SfxPrinter* pPrinter = getPrinter( true );
            // <--
            if( pDrawModel )
                pDrawModel->SetRefDevice( pPrinter );
        }

        set(IDocumentSettingAccess::USE_VIRTUAL_DEVICE, bNewVirtual );
        set(IDocumentSettingAccess::USE_HIRES_VIRTUAL_DEVICE, bNewHiRes );
        PrtDataChanged();
        SetModified();
    }
}

const JobSetup* SwDoc::getJobsetup() const
{
    return pPrt ? &pPrt->GetJobSetup() : 0;
}

void SwDoc::setJobsetup(/*[in]*/ const JobSetup &rJobSetup )
{
    BOOL bCheckPageDescs = 0 == pPrt;
    BOOL bDataChanged = FALSE;

    if ( pPrt )
    {
        if ( pPrt->GetName() == rJobSetup.GetPrinterName() )
        {
            if ( pPrt->GetJobSetup() != rJobSetup )
            {
                pPrt->SetJobSetup( rJobSetup );
                bDataChanged = TRUE;
            }
        }
        else
            delete pPrt, pPrt = 0;
    }

    if( !pPrt )
    {
        //Das ItemSet wird vom Sfx geloescht!
        SfxItemSet *pSet = new SfxItemSet( GetAttrPool(),
                        FN_PARAM_ADDPRINTER, FN_PARAM_ADDPRINTER,
                        SID_HTML_MODE,  SID_HTML_MODE,
                        SID_PRINTER_NOTFOUND_WARN, SID_PRINTER_NOTFOUND_WARN,
                        SID_PRINTER_CHANGESTODOC, SID_PRINTER_CHANGESTODOC,
                        0 );
        SfxPrinter *p = new SfxPrinter( pSet, rJobSetup );
        if ( bCheckPageDescs )
            setPrinter( p, true, true );
        else
        {
            pPrt = p;
            bDataChanged = TRUE;
        }
    }
    if ( bDataChanged && !get(IDocumentSettingAccess::USE_VIRTUAL_DEVICE) )
        PrtDataChanged();
}

SwPrintData* SwDoc::getPrintData() const
{
    return pPrtData;
}

void SwDoc::setPrintData(/*[in]*/ const SwPrintData& rPrtData )
{
    if(!pPrtData)
        pPrtData = new SwPrintData;
    *pPrtData = rPrtData;
}

/** Implementations the next Interface here
*/

/*
 * Dokumenteditieren (Doc-SS) zum Fuellen des Dokuments
 * durch den RTF Parser und fuer die EditShell.
 */
void SwDoc::ChgDBData(const SwDBData& rNewData)
{
    if( rNewData != aDBData )
    {
        aDBData = rNewData;
        SetModified();
    }
    GetSysFldType(RES_DBNAMEFLD)->UpdateFlds();
}

bool SwDoc::SplitNode( const SwPosition &rPos, bool bChkTableStart )
{
    SwCntntNode *pNode = rPos.nNode.GetNode().GetCntntNode();
    if(0 == pNode)
        return false;

    {
        // Bug 26675:	DataChanged vorm loeschen verschicken, dann bekommt
        //			man noch mit, welche Objecte sich im Bereich befinden.
        //			Danach koennen sie vor/hinter der Position befinden.
        SwDataChanged aTmp( this, rPos, 0 );
    }

    SwUndoSplitNode* pUndo = 0;
    if ( DoesUndo() )
    {
        ClearRedo();
        // einfuegen vom Undo-Object, z.Z. nur beim TextNode
        if( pNode->IsTxtNode() )
        {
            pUndo = new SwUndoSplitNode( this, rPos, bChkTableStart );
            AppendUndo(pUndo);
        }
    }

    //JP 28.01.97: Sonderfall fuer SplitNode am Tabellenanfang:
    //				steht die am Doc/Fly/Footer/..-Anfang oder direkt
    //				hinter einer Tabelle, dann fuege davor
    //				einen Absatz ein
    if( bChkTableStart && !rPos.nContent.GetIndex() && pNode->IsTxtNode() )
    {
        ULONG nPrevPos = rPos.nNode.GetIndex() - 1;
        const SwTableNode* pTblNd;
        const SwNode* pNd = GetNodes()[ nPrevPos ];
        if( pNd->IsStartNode() &&
            SwTableBoxStartNode == ((SwStartNode*)pNd)->GetStartNodeType() &&
            0 != ( pTblNd = GetNodes()[ --nPrevPos ]->GetTableNode() ) &&
            ((( pNd = GetNodes()[ --nPrevPos ])->IsStartNode() &&
               SwTableBoxStartNode != ((SwStartNode*)pNd)->GetStartNodeType() )
               || ( pNd->IsEndNode() && pNd->StartOfSectionNode()->IsTableNode() )
               || pNd->IsCntntNode() ))
        {
            if( pNd->IsCntntNode() )
            {
                //JP 30.04.99 Bug 65660:
                // ausserhalb des normalen BodyBereiches gibt es keine
                // Seitenumbrueche, also ist das hier kein gueltige
                // Bedingung fuers einfuegen eines Absatzes
                if( nPrevPos < GetNodes().GetEndOfExtras().GetIndex() )
                    pNd = 0;
                else
                {
                    // Dann nur, wenn die Tabelle Umbrueche traegt!
                    const SwFrmFmt* pFrmFmt = pTblNd->GetTable().GetFrmFmt();
                    if( SFX_ITEM_SET != pFrmFmt->GetItemState(RES_PAGEDESC, FALSE) &&
                        SFX_ITEM_SET != pFrmFmt->GetItemState( RES_BREAK, FALSE ) )
                        pNd = 0;
                }
            }

            if( pNd )
            {
                SwTxtNode* pTxtNd = GetNodes().MakeTxtNode(
                                        SwNodeIndex( *pTblNd ),
                                        GetTxtCollFromPool( RES_POOLCOLL_TEXT ));
                if( pTxtNd )
                {
                    ((SwPosition&)rPos).nNode = pTblNd->GetIndex()-1;
                    ((SwPosition&)rPos).nContent.Assign( pTxtNd, 0 );

                    // nur im BodyBereich den SeitenUmbruch/-Vorlage umhaengem
                    if( nPrevPos > GetNodes().GetEndOfExtras().GetIndex() )
                    {
                        SwFrmFmt* pFrmFmt = pTblNd->GetTable().GetFrmFmt();
                        const SfxPoolItem *pItem;
                        if( SFX_ITEM_SET == pFrmFmt->GetItemState( RES_PAGEDESC,
                            FALSE, &pItem ) )
                        {
                            pTxtNd->SetAttr( *pItem );
                            pFrmFmt->ResetFmtAttr( RES_PAGEDESC );
                        }
                        if( SFX_ITEM_SET == pFrmFmt->GetItemState( RES_BREAK,
                            FALSE, &pItem ) )
                        {
                            pTxtNd->SetAttr( *pItem );
                            pFrmFmt->ResetFmtAttr( RES_BREAK );
                        }
                    }

                    if( pUndo )
                        pUndo->SetTblFlag();
                    SetModified();
                    return true;
                }
            }
        }
    }

    SvULongs aBkmkArr( 15, 15 );
    _SaveCntntIdx( this, rPos.nNode.GetIndex(), rPos.nContent.GetIndex(),
                    aBkmkArr, SAVEFLY_SPLIT );
    // FIXME: only SwTxtNode has a valid implementation of SplitCntntNode!
    ASSERT(pNode->IsTxtNode(), "splitting non-text node?");
    pNode = pNode->SplitCntntNode( rPos );
    if (pNode)
    {
        // verschiebe noch alle Bookmarks/TOXMarks/FlyAtCnt
        if( aBkmkArr.Count() )
            _RestoreCntntIdx( this, aBkmkArr, rPos.nNode.GetIndex()-1, 0, TRUE );

        if( IsRedlineOn() || (!IsIgnoreRedline() && pRedlineTbl->Count() ))
        {
            SwPaM aPam( rPos );
            aPam.SetMark();
            aPam.Move( fnMoveBackward );
            if( IsRedlineOn() )
                AppendRedline( new SwRedline( nsRedlineType_t::REDLINE_INSERT, aPam ), true);
            else
                SplitRedline( aPam );
        }
    }

    SetModified();
    return true;
}

bool SwDoc::AppendTxtNode( SwPosition& rPos )
{
    /*
     * Neuen Node vor EndOfContent erzeugen.
     */
    SwTxtNode *pCurNode = GetNodes()[ rPos.nNode ]->GetTxtNode();
    if( !pCurNode )
    {
        // dann kann ja einer angelegt werden!
        SwNodeIndex aIdx( rPos.nNode, 1 );
        pCurNode = GetNodes().MakeTxtNode( aIdx,
                        GetTxtCollFromPool( RES_POOLCOLL_STANDARD ));
    }
    else
        pCurNode = (SwTxtNode*)pCurNode->AppendNode( rPos );

    rPos.nNode++;
    rPos.nContent.Assign( pCurNode, 0 );

    if( DoesUndo() )
    {
        ClearRedo();
        AppendUndo( new SwUndoInsert( rPos.nNode ));
    }

    if( IsRedlineOn() || (!IsIgnoreRedline() && pRedlineTbl->Count() ))
    {
        SwPaM aPam( rPos );
        aPam.SetMark();
        aPam.Move( fnMoveBackward );
        if( IsRedlineOn() )
            AppendRedline( new SwRedline( nsRedlineType_t::REDLINE_INSERT, aPam ), true);
        else
            SplitRedline( aPam );
    }

    SetModified();
    return TRUE;
}

bool SwDoc::InsertString( const SwPaM &rRg, const String &rStr,
        const enum InsertFlags nInsertMode )
{
    if( DoesUndo() )
    {
        ClearRedo();
    }

    const SwPosition& rPos = *rRg.GetPoint();

    if( pACEWord )					// Aufnahme in die Autokorrektur
    {
        if( 1 == rStr.Len() && pACEWord->IsDeleted() )
        {
            pACEWord->CheckChar( rPos, rStr.GetChar( 0 ) );
        }
        delete pACEWord, pACEWord = 0;
    }

    SwTxtNode *const pNode = rPos.nNode.GetNode().GetTxtNode();
    if(!pNode)
    {
        return false;
    }

    SwDataChanged aTmp( rRg, 0 );

    if( !DoesUndo() || !DoesGroupUndo() )
    {
        pNode->InsertText( rStr, rPos.nContent, nInsertMode );

        if( DoesUndo() )
        {
            SwUndoInsert * const pUndo( new SwUndoInsert(
                rPos.nNode, rPos.nContent.GetIndex(), rStr.Len(), nInsertMode));
            AppendUndo(pUndo);
        }
    }
    else
    {			// ist Undo und Gruppierung eingeschaltet, ist alles anders !
        SwUndoInsert * pUndo = NULL; // #111827#

        // don't group the start if hints at the start should be expanded
        if (!(nInsertMode & IDocumentContentOperations::INS_FORCEHINTEXPAND))
        // -> #111827#
        {
            USHORT const nUndoSize = pUndos->Count();
            if (0 != nUndoSize)
            {
                SwUndo * const pLastUndo = (*pUndos)[ nUndoSize - 1 ];

                switch (pLastUndo->GetId())
                {
                    case UNDO_INSERT:
                    case UNDO_TYPING:
                        if (static_cast<SwUndoInsert*>(pLastUndo)
                                ->CanGrouping( rPos ))
                        {
                            pUndo = static_cast<SwUndoInsert*>(pLastUndo);
                        }
                        break;

                    default:
                        break;
                }
            }
        }
        // <- #111827#

        CharClass const& rCC = GetAppCharClass();
        xub_StrLen nInsPos = rPos.nContent.GetIndex();

        if (!pUndo)
        {
            pUndo = new SwUndoInsert( rPos.nNode, nInsPos, 0, nInsertMode,
                            !rCC.isLetterNumeric( rStr, 0 ) );
            AppendUndo( pUndo );
        }

        pNode->InsertText( rStr, rPos.nContent, nInsertMode );

        for( xub_StrLen i = 0; i < rStr.Len(); ++i )
        {
            nInsPos++;
            // wenn CanGrouping() TRUE returnt, ist schon alles erledigt
            if( !pUndo->CanGrouping( rStr.GetChar( i ) ))
            {
                pUndo = new SwUndoInsert( rPos.nNode, nInsPos, 1, nInsertMode,
                            !rCC.isLetterNumeric( rStr, i ) );
                AppendUndo( pUndo );
            }
        }
    }

    if( IsRedlineOn() || (!IsIgnoreRedline() && pRedlineTbl->Count() ))
    {
        SwPaM aPam( rPos.nNode, aTmp.GetCntnt(),
                    rPos.nNode, rPos.nContent.GetIndex());
        if( IsRedlineOn() )
        {
            AppendRedline(
                new SwRedline( nsRedlineType_t::REDLINE_INSERT, aPam ), true);
        }
        else
        {
            SplitRedline( aPam );
        }
    }

    SetModified();
    return true;
}

SwFlyFrmFmt* SwDoc::_InsNoTxtNode( const SwPosition& rPos, SwNoTxtNode* pNode,
                                    const SfxItemSet* pFlyAttrSet,
                                    const SfxItemSet* pGrfAttrSet,
                                    SwFrmFmt* pFrmFmt)
{
    SwFlyFrmFmt *pFmt = 0;
    if( pNode )
    {
        pFmt = _MakeFlySection( rPos, *pNode, FLY_AT_PARA,
                                pFlyAttrSet, pFrmFmt );
        if( pGrfAttrSet )
            pNode->SetAttr( *pGrfAttrSet );
    }
    return pFmt;
}

SwFlyFrmFmt* SwDoc::Insert( const SwPaM &rRg,
                            const String& rGrfName,
                            const String& rFltName,
                            const Graphic* pGraphic,
                            const SfxItemSet* pFlyAttrSet,
                            const SfxItemSet* pGrfAttrSet,
                            SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
        pFrmFmt = GetFrmFmtFromPool( RES_POOLFRM_GRAPHIC );
    return _InsNoTxtNode( *rRg.GetPoint(), GetNodes().MakeGrfNode(
                            SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                            rGrfName, rFltName, pGraphic,
                            pDfltGrfFmtColl ),
                            pFlyAttrSet, pGrfAttrSet, pFrmFmt );
}
SwFlyFrmFmt* SwDoc::Insert( const SwPaM &rRg, const GraphicObject& rGrfObj,
                            const SfxItemSet* pFlyAttrSet,
                            const SfxItemSet* pGrfAttrSet,
                            SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
        pFrmFmt = GetFrmFmtFromPool( RES_POOLFRM_GRAPHIC );
    return _InsNoTxtNode( *rRg.GetPoint(), GetNodes().MakeGrfNode(
                            SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                            rGrfObj, pDfltGrfFmtColl ),
                            pFlyAttrSet, pGrfAttrSet, pFrmFmt );
}

SwFlyFrmFmt* SwDoc::Insert(const SwPaM &rRg, const svt::EmbeddedObjectRef& xObj,
                        const SfxItemSet* pFlyAttrSet,
                        const SfxItemSet* pGrfAttrSet,
                        SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
    {
        USHORT nId = RES_POOLFRM_OLE;
        SvGlobalName aClassName( xObj->getClassID() );
        if (SotExchange::IsMath(aClassName))
            nId = RES_POOLFRM_FORMEL;

        pFrmFmt = GetFrmFmtFromPool( nId );
    }
    return _InsNoTxtNode( *rRg.GetPoint(), GetNodes().MakeOLENode(
                            SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                            xObj,
                            pDfltGrfFmtColl ),
                            pFlyAttrSet, pGrfAttrSet,
                            pFrmFmt );
}

SwFlyFrmFmt* SwDoc::InsertOLE(const SwPaM &rRg, const String& rObjName,
                        sal_Int64 nAspect,
                        const SfxItemSet* pFlyAttrSet,
                        const SfxItemSet* pGrfAttrSet,
                        SwFrmFmt* pFrmFmt )
{
    if( !pFrmFmt )
        pFrmFmt = GetFrmFmtFromPool( RES_POOLFRM_OLE );

    return _InsNoTxtNode( *rRg.GetPoint(),
                            GetNodes().MakeOLENode(
                                SwNodeIndex( GetNodes().GetEndOfAutotext() ),
                                rObjName,
                                nAspect,
                                pDfltGrfFmtColl,
                                0 ),
                            pFlyAttrSet, pGrfAttrSet,
                            pFrmFmt );
}

/*************************************************************************
|*				  SwDoc::GetFldType()
|*	  Beschreibung: liefert den am Doc eingerichteten Feldtypen zurueck
*************************************************************************/

SwFieldType *SwDoc::GetSysFldType( const USHORT eWhich ) const
{
    for( USHORT i = 0; i < INIT_FLDTYPES; ++i )
        if( eWhich == (*pFldTypes)[i]->Which() )
            return (*pFldTypes)[i];
    return 0;
}
/*************************************************************************
 *			   void SetDocStat( const SwDocStat& rStat );
 *************************************************************************/

void SwDoc::SetDocStat( const SwDocStat& rStat )
{
    *pDocStat = rStat;
}

const SwDocStat& SwDoc::GetDocStat() const
{
    return *pDocStat;
}

/*************************************************************************/


struct _PostItFld : public _SetGetExpFld
{
    _PostItFld( const SwNodeIndex& rNdIdx, const SwTxtFld* pFld,  const SwIndex* pIdx = 0 )
        : _SetGetExpFld( rNdIdx, pFld, pIdx ) {}

    USHORT GetPageNo( const StringRangeEnumerator &rRangeEnum,
            const std::set< sal_Int32 > &rPossiblePages, 
            USHORT& rVirtPgNo, USHORT& rLineNo );
    
    SwPostItField* GetPostIt() const
    { 
        return (SwPostItField*) GetFld()->GetFld().GetFld();
    }
};


USHORT _PostItFld::GetPageNo( 
    const StringRangeEnumerator &rRangeEnum,
    const std::set< sal_Int32 > &rPossiblePages,
    /* out */ USHORT& rVirtPgNo, /* out */ USHORT& rLineNo )
{
    //Problem: Wenn ein PostItFld in einem Node steht, der von mehr als
    //einer Layout-Instanz repraesentiert wird, steht die Frage im Raum,
    //ob das PostIt nur ein- oder n-mal gedruck werden soll.
    //Wahrscheinlich nur einmal, als Seitennummer soll hier keine Zufaellige
    //sondern die des ersten Auftretens des PostIts innerhalb des selektierten
    //Bereichs ermittelt werden.
    rVirtPgNo = 0;
    USHORT nPos = GetCntnt();
    SwClientIter aIter( (SwModify &)GetFld()->GetTxtNode() );
    for( SwTxtFrm* pFrm = (SwTxtFrm*)aIter.First( TYPE( SwFrm ));
            pFrm;  pFrm = (SwTxtFrm*)aIter.Next() )
    {
        if( pFrm->GetOfst() > nPos ||
            (pFrm->HasFollow() && pFrm->GetFollow()->GetOfst() <= nPos) )
            continue;
        USHORT nPgNo = pFrm->GetPhyPageNum();
        if( rRangeEnum.hasValue( nPgNo, &rPossiblePages ))
        {
            rLineNo = (USHORT)(pFrm->GetLineCount( nPos ) +
                      pFrm->GetAllLines() - pFrm->GetThisLines());
            rVirtPgNo = pFrm->GetVirtPageNum();
            return nPgNo;
        }
    }
    return 0;
}


bool lcl_GetPostIts( 
    IDocumentFieldsAccess* pIDFA, 
    _SetGetExpFlds * pSrtLst )
{
    bool bHasPostIts = false;

    SwFieldType* pFldType = pIDFA->GetSysFldType( RES_POSTITFLD );
    DBG_ASSERT( pFldType, "kein PostItType ? ");

    if( pFldType->GetDepends() )
    {
        // Modify-Object gefunden, trage alle Felder ins Array ein
        SwClientIter aIter( *pFldType );
        SwClient* pLast;
        const SwTxtFld* pTxtFld;

        for( pLast = aIter.First( TYPE(SwFmtFld)); pLast; pLast = aIter.Next() )
        {
            if( 0 != ( pTxtFld = ((SwFmtFld*)pLast)->GetTxtFld() ) &&
                pTxtFld->GetTxtNode().GetNodes().IsDocNodes() )
            {
                bHasPostIts = true;
                if (pSrtLst)
                {
                    SwNodeIndex aIdx( pTxtFld->GetTxtNode() );
                    _PostItFld* pNew = new _PostItFld( aIdx, pTxtFld );
                    pSrtLst->Insert( pNew );
                }
                else
                    break;  // we just wanted to check for the existence of postits ...
            }
        }
    }

    return bHasPostIts;
}


static void lcl_FormatPostIt( 
    IDocumentContentOperations* pIDCO, 
    SwPaM& aPam, 
    SwPostItField* pField,
    bool bNewPage, bool bIsFirstPostIt,
    USHORT nPageNo, USHORT nLineNo )
{
    static char __READONLY_DATA sTmp[] = " : ";

    DBG_ASSERT( ViewShell::GetShellRes(), "missing ShellRes" );

    if (bNewPage)
    {
        pIDCO->InsertPoolItem( aPam, SvxFmtBreakItem( SVX_BREAK_PAGE_AFTER, RES_BREAK ), 0 );
        pIDCO->SplitNode( *aPam.GetPoint(), false );
    }
    else if (!bIsFirstPostIt)
    {
        // add an empty line between different notes
        pIDCO->SplitNode( *aPam.GetPoint(), false );
        pIDCO->SplitNode( *aPam.GetPoint(), false );
    }
    
    String aStr( ViewShell::GetShellRes()->aPostItPage );
    aStr.AppendAscii(sTmp);

    aStr += XubString::CreateFromInt32( nPageNo );
    aStr += ' ';
    if( nLineNo )
    {
        aStr += ViewShell::GetShellRes()->aPostItLine;
        aStr.AppendAscii(sTmp);
        aStr += XubString::CreateFromInt32( nLineNo );
        aStr += ' ';
    }
    aStr += ViewShell::GetShellRes()->aPostItAuthor;
    aStr.AppendAscii(sTmp);
    aStr += pField->GetPar1();
    aStr += ' ';
    SvtSysLocale aSysLocale;    
    aStr += /*(LocaleDataWrapper&)*/aSysLocale.GetLocaleData().getDate( pField->GetDate() );
    pIDCO->InsertString( aPam, aStr );

    pIDCO->SplitNode( *aPam.GetPoint(), false );
    aStr = pField->GetPar2();
#if defined( WIN ) || defined( WNT ) || defined( PM2 )
    // Bei Windows und Co alle CR rausschmeissen
    aStr.EraseAllChars( '\r' );
#endif
    pIDCO->InsertString( aPam, aStr );
}


void SwDoc::CalculatePagesForPrinting(
    /* out */ SwRenderData &rData, 
    const SwPrintUIOptions &rOptions,
    bool bIsPDFExport,
    sal_Int32 nDocPageCount )
{
    DBG_ASSERT( pLayout, "no layout present" );
    if (!pLayout)
        return;

    // properties to take into account when calcualting the set of pages
    // (PDF export UI does not allow for selecting left or right pages only)
    bool bPrintLeftPages    = bIsPDFExport ? true : rOptions.IsPrintLeftPages();
    bool bPrintRightPages   = bIsPDFExport ? true : rOptions.IsPrintRightPages();
    bool bPrintEmptyPages   = rOptions.IsPrintEmptyPages( bIsPDFExport );
        
    Range aPages( 1, nDocPageCount );
        
    MultiSelection aMulti( aPages );
    aMulti.SetTotalRange( Range( 0, RANGE_MAX ) );
    aMulti.Select( aPages );

    const SwPageFrm *pStPage  = (SwPageFrm*)pLayout->Lower();
    const SwFrm     *pEndPage = pStPage;

    USHORT nFirstPageNo = 0;
    USHORT nLastPageNo  = 0;
    USHORT nPageNo      = 1;

    for( USHORT i = 1; i <= (USHORT)aPages.Max(); ++i )
    {
        if( i < (USHORT)aPages.Min() )
        {
            if( !pStPage->GetNext() )
                break;
            pStPage = (SwPageFrm*)pStPage->GetNext();
            pEndPage= pStPage;
        }
        else if( i == (USHORT)aPages.Min() )
        {
            nFirstPageNo = i;
            nLastPageNo = nFirstPageNo;
            if( !pStPage->GetNext() || (i == (USHORT)aPages.Max()) )
                break;
            pEndPage = pStPage->GetNext();
        }
        else if( i > (USHORT)aPages.Min() )
        {
            nLastPageNo = i;
            if( !pEndPage->GetNext() || (i == (USHORT)aPages.Max()) )
                break;
            pEndPage = pEndPage->GetNext();
        }
    }

    DBG_ASSERT( nFirstPageNo, "first page not found!  Should not happen!" );
    if (nFirstPageNo)
    {
// HACK: Hier muss von der MultiSelection noch eine akzeptable Moeglichkeit
// geschaffen werden, alle Seiten von Seite x an zu deselektieren.
// Z.B. durch SetTotalRange ....

//              aMulti.Select( Range( nLastPageNo+1, SELECTION_MAX ), FALSE );
        MultiSelection aTmpMulti( Range( 1, nLastPageNo ) );
        long nTmpIdx = aMulti.FirstSelected();
        static long nEndOfSelection = SFX_ENDOFSELECTION;
        while ( nEndOfSelection != nTmpIdx && nTmpIdx <= long(nLastPageNo) )
        {
            aTmpMulti.Select( nTmpIdx );
            nTmpIdx = aMulti.NextSelected();
        }
        aMulti = aTmpMulti;
// Ende des HACKs

        nPageNo = nFirstPageNo;

        std::set< sal_Int32 > &rValidPages = rData.GetValidPagesSet();
        std::map< sal_Int32, const SwPageFrm * > &rValidStartFrms = rData.GetValidStartFrames();
        rValidPages.clear();
        rValidStartFrms.clear();
        while ( pStPage )
        {
            const BOOL bRightPg = pStPage->OnRightPage();
            if ( aMulti.IsSelected( nPageNo ) &&
                ( (bRightPg && bPrintRightPages) ||
                    (!bRightPg && bPrintLeftPages) ) )
            {
                // --> FME 2005-12-12 #b6354161# Feature - Print empty pages
                if ( bPrintEmptyPages || pStPage->Frm().Height() )
                // <--
                {
                    // pStPage->GetUpper()->Paint( pStPage->Frm() );
                    rValidPages.insert( nPageNo );
                    rValidStartFrms[ nPageNo ] = pStPage;
                }
            }

            if ( pStPage == pEndPage )
            {
                pStPage = 0;
            }
            else
            {   ++nPageNo;
                pStPage = (SwPageFrm*)pStPage->GetNext();
            }
        }
    }

    
    //
    // now that we have identified the valid pages for printing according
    // to the print settings we need to get the PageRange to use and
    // use both results to get the actual pages to be printed
    // (post-it settings need to be taken into account later on!)
    //

    // get PageRange value to use
    OUString aPageRange;
    if (bIsPDFExport)
    {
        // ?? rOptions.getValue( C2U("Selection") );
        aPageRange = rOptions.getStringValue( "PageRange", OUString() );
    }
    else
    {
        // PageContent : 
        // 0 -> print all pages (default if aPageRange is empty)
        // 1 -> print range according to PageRange
        // 2 -> print selection
        const sal_Int32 nContent = rOptions.getIntValue( "PrintContent", 0 );
        if (1 == nContent)
            aPageRange = rOptions.getStringValue( "PageRange", OUString() );
        if (2 == nContent)
        {
            // note that printing selections is actually implemented by copying
            // the selection to a new temporary document and printing all of that one.
            // Thus for Writer "PrintContent" must never be 2.
            // See SwXTextDocument::GetRenderDoc for evaluating if a selection is to be 
            // printed and for creating the temporary document.
        }    

        // please note
    }
    if (aPageRange.getLength() == 0)    // empty string -> print all
    {
        // set page range to print to 'all pages'
        aPageRange = OUString::valueOf( (sal_Int32)1 );
        aPageRange += OUString::valueOf( (sal_Unicode)'-');
        aPageRange += OUString::valueOf( nDocPageCount );
    }
    rData.SetPageRange( aPageRange );

    // get vector of pages to print according to PageRange and valid pages set from above
    // (result may be an empty vector, for example if the range string is not correct)
    StringRangeEnumerator::getRangesFromString( 
            aPageRange, rData.GetPagesToPrint(),
            1, nDocPageCount, 0, &rData.GetValidPagesSet() );
}
    

void SwDoc::UpdatePagesForPrintingWithPostItData( 
    /* out */ SwRenderData &rData, 
    const SwPrintUIOptions &rOptions,
    bool /*bIsPDFExport*/, 
    sal_Int32 nDocPageCount )
{

    sal_Int16 nPostItMode = (sal_Int16) rOptions.getIntValue( "PrintAnnotationMode", 0 );
    DBG_ASSERT(nPostItMode == POSTITS_NONE || rData.HasPostItData(),
            "print post-its without post-it data?" );
    const USHORT nPostItCount = rData.HasPostItData() ? rData.m_pPostItFields->Count() : 0;
    if (nPostItMode != POSTITS_NONE && nPostItCount > 0)
    {
        SET_CURR_SHELL( rData.m_pPostItShell );

        // clear document and move to end of it
        SwPaM aPam( rData.m_pPostItDoc->GetNodes().GetEndOfContent() );
        aPam.Move( fnMoveBackward, fnGoDoc );
        aPam.SetMark();
        aPam.Move( fnMoveForward, fnGoDoc );
        rData.m_pPostItDoc->DeleteRange( aPam );

        const StringRangeEnumerator aRangeEnum( rData.GetPageRange(), 1, nDocPageCount, 0 );

        // For mode POSTITS_ENDPAGE: 
        // maps a physical page number to the page number in post-it document that holds
        // the first post-it for that physical page . Needed to relate the correct start frames
        // from the post-it doc to the physical page of the document
        std::map< sal_Int32, sal_Int32 >  aPostItLastStartPageNum;

        // add all post-its on valid pages within the the page range to the
        // temporary post-it document. 
        // Since the array of post-it fileds is sorted by page and line number we will
        // already get them in the correct order
        USHORT nVirtPg = 0, nLineNo = 0, nLastPageNum = 0, nPhyPageNum = 0;
        bool bIsFirstPostIt = true;
        for (USHORT i = 0; i < nPostItCount; ++i)
        {
            _PostItFld& rPostIt = (_PostItFld&)*(*rData.m_pPostItFields)[ i ];
            nLastPageNum = nPhyPageNum;
            nPhyPageNum = rPostIt.GetPageNo( 
                    aRangeEnum, rData.GetValidPagesSet(), nVirtPg, nLineNo );
            if (nPhyPageNum)
            {
                // need to insert a page break?
                // In POSTITS_ENDPAGE mode for each document page the following
                // post-it page needs to start on a new page
                const bool bNewPage = nPostItMode == POSTITS_ENDPAGE &&
                        !bIsFirstPostIt && nPhyPageNum != nLastPageNum;
                
                lcl_FormatPostIt( rData.m_pPostItShell->GetDoc(), aPam,
                        rPostIt.GetPostIt(), bNewPage, bIsFirstPostIt, nVirtPg, nLineNo );
                bIsFirstPostIt = false;
                
                if (nPostItMode == POSTITS_ENDPAGE)
                {
                    // get the correct number of current pages for the post-it document
                    rData.m_pPostItShell->CalcLayout();
                    const sal_Int32 nPages = rData.m_pPostItDoc->GetPageCount();
                    aPostItLastStartPageNum[ nPhyPageNum ] = nPages;
                }    
            }
        }
        
        // format post-it doc to get correct number of pages
        rData.m_pPostItShell->CalcLayout();
        const sal_Int32 nPostItDocPageCount = rData.m_pPostItDoc->GetPageCount();

        if (nPostItMode == POSTITS_ONLY || nPostItMode == POSTITS_ENDDOC)
        {
            // now add those post-it pages to the vector of pages to print 
            // or replace them if only post-its should be printed
            
            rData.GetPostItStartFrames().clear();
            if (nPostItMode == POSTITS_ENDDOC)
            {
                // set all values up to number of pages to print currently known to NULL,
                // meaning none of the pages currently in the vector is from the 
                // post-it document, they are the documents pages.
                rData.GetPostItStartFrames().resize( rData.GetPagesToPrint().size() );
            }    
            else if (nPostItMode == POSTITS_ONLY)
            {
                // no document page to be printed
                rData.GetPagesToPrint().clear();
            }
            
            // now we just need to add the post-it pages to be printed to the end
            // of the vector of pages to print and keep the GetValidStartFrames 
            // data conform with it
            sal_Int32 nPageNum = 0;
            const SwPageFrm * pPageFrm = (SwPageFrm*)rData.m_pPostItShell->GetLayout()->Lower();
            while( pPageFrm && nPageNum < nPostItDocPageCount )
            {
                DBG_ASSERT( pPageFrm, "Empty page frame. How are we going to print this?" );
                ++nPageNum;
                rData.GetPagesToPrint().push_back( 0 );  // a page number of 0 indicates this page is from the post-it doc
                DBG_ASSERT( pPageFrm, "pPageFrm is NULL!" );
                rData.GetPostItStartFrames().push_back( pPageFrm );
                pPageFrm = (SwPageFrm*)pPageFrm->GetNext();
            }
            DBG_ASSERT( nPageNum == nPostItDocPageCount, "unexpected number of pages" );
        }    
        else if (nPostItMode == POSTITS_ENDPAGE)
        {
            // the next step is to find all the start frames from the post-it
            // document that should be printed for a given physical page of the document
            std::map< sal_Int32, std::vector< const SwPageFrm * > > aPhysPageToPostItFrames;

            // ... thus, first collect all post-it doc start frames in a vector
            sal_Int32 nPostItPageNum = 0;
            std::vector< const SwPageFrm * > aAllPostItStartFrames;
            const SwPageFrm * pPageFrm = (SwPageFrm*)rData.m_pPostItShell->GetLayout()->Lower();
            while( pPageFrm && sal_Int32(aAllPostItStartFrames.size()) < nPostItDocPageCount )
            {
                DBG_ASSERT( pPageFrm, "Empty page frame. How are we going to print this?" );
                ++nPostItPageNum;
                aAllPostItStartFrames.push_back( pPageFrm );
                pPageFrm = (SwPageFrm*)pPageFrm->GetNext();
            }
            DBG_ASSERT( sal_Int32(aAllPostItStartFrames.size()) == nPostItDocPageCount, 
                    "unexpected number of frames; does not match number of pages" );
            
            // get a map that holds all post-it frames to be printed for a 
            // given physical page from the document
            sal_Int32 nLastStartPageNum = 0;
            std::map< sal_Int32, sal_Int32 >::const_iterator  aIt;
            for (aIt = aPostItLastStartPageNum.begin();  aIt != aPostItLastStartPageNum.end();  ++aIt)
            {
                const sal_Int32 nFrames = aIt->second - nLastStartPageNum;
                const sal_Int32 nFirstStartPageNum = aIt == aPostItLastStartPageNum.begin() ?
                        1 : aIt->second - nFrames + 1;
                DBG_ASSERT( 1 <= nFirstStartPageNum && nFirstStartPageNum <= nPostItDocPageCount,
                        "page number for first frame out of range" );
                std::vector<  const SwPageFrm * > aStartFrames;
                for (sal_Int32 i = 0; i < nFrames; ++i)
                {
                    const sal_Int32 nIdx = nFirstStartPageNum - 1 + i;   // -1 because lowest page num is 1
                    DBG_ASSERT( 0 <= nIdx && nIdx < sal_Int32(aAllPostItStartFrames.size()), 
                            "index out of range" );
                    aStartFrames.push_back( aAllPostItStartFrames[ nIdx ] );
                }
                aPhysPageToPostItFrames[ aIt->first /* phys page num */ ] = aStartFrames;
                nLastStartPageNum = aIt->second;
            }

            
            // ok, now that aPhysPageToPostItFrames can give the start frames for all 
            // post-it pages to be printed we need to merge those at the correct
            // position into the GetPagesToPrint vector and build and maintain the
            // GetValidStartFrames vector as well.
            // Since inserting a larger number of entries in the middle of a vector 
            // isn't that efficient we will create new vectors by copying the required data
            std::vector< sal_Int32 >            aTmpPagesToPrint;
            std::vector< const SwPageFrm * >    aTmpPostItStartFrames;
            const size_t nNum = rData.GetPagesToPrint().size();
            for (size_t i = 0 ;  i < nNum;  ++i)
            {
                // add the physical page to print from the document
                const sal_Int32 nPhysPage = rData.GetPagesToPrint()[i];
                aTmpPagesToPrint.push_back( nPhysPage );
                aTmpPostItStartFrames.push_back( NULL );
                
                // add the post-it document pages to print, i.e those
                // post-it pages that have the data for the above physical page
                const std::vector< const SwPageFrm * > &rPostItFrames = aPhysPageToPostItFrames[ nPhysPage ];
                const size_t nPostItFrames = rPostItFrames.size();
                for (size_t k = 0;  k < nPostItFrames;  ++k)
                {
                    aTmpPagesToPrint.push_back( 0 );
                    aTmpPostItStartFrames.push_back( rPostItFrames[k] );
                }    
            }

            // finally we need to assign those vectors to the resulting ones.
            // swapping the data should be more efficient than assigning since
            // we won't need the temporary vectors anymore
            rData.GetPagesToPrint().swap( aTmpPagesToPrint );
            rData.GetPostItStartFrames().swap( aTmpPostItStartFrames );
        }    
    }    
}
    

void SwDoc::CalculatePagePairsForProspectPrinting( 
    /* out */ SwRenderData &rData, 
    const SwPrintUIOptions &rOptions,
    sal_Int32 nDocPageCount )
{
    std::set< sal_Int32 > &rValidPagesSet   = rData.GetValidPagesSet();
    std::map< sal_Int32, const SwPageFrm * > &rValidStartFrms  = rData.GetValidStartFrames();
    std::vector< std::pair< sal_Int32, sal_Int32 > > &rPagePairs = rData.GetPagePairsForProspectPrinting();
    
    rPagePairs.clear();
    rValidPagesSet.clear();
    rValidStartFrms.clear();

    rtl::OUString aPageRange = rOptions.getStringValue( "PageRange", rtl::OUString() );
    StringRangeEnumerator aRange( aPageRange, 1, nDocPageCount, 0 );

    DBG_ASSERT( pLayout, "no layout present" );
    if (!pLayout || aRange.size() <= 0)
        return;

    const SwPageFrm *pStPage  = (SwPageFrm*)pLayout->Lower();
    sal_Int32 i = 0;
    for ( i = 1; pStPage && i < nDocPageCount; ++i )
        pStPage = (SwPageFrm*)pStPage->GetNext();
    if ( !pStPage )          // dann wars das
        return;

    // currently for prospect printing all pages are valid to be printed 
    // thus we add them all to the respective map and set for later use
    sal_Int32 nPageNum = 0;
    const SwPageFrm *pPageFrm = (SwPageFrm*)pLayout->Lower();
    while( pPageFrm && nPageNum < nDocPageCount )
    {
        DBG_ASSERT( pPageFrm, "Empty page frame. How are we going to print this?" );
        ++nPageNum;
        rValidPagesSet.insert( nPageNum );
        rValidStartFrms[ nPageNum ] = pPageFrm;
        pPageFrm = (SwPageFrm*)pPageFrm->GetNext();
    }
    DBG_ASSERT( nPageNum == nDocPageCount, "unexpected number of pages" );

    // properties to take into account when calcualting the set of pages
    // Note: here bPrintLeftPages and bPrintRightPages refer to the (virtual) resulting pages
    //      of the prospect!
    bool bPrintLeftPages     = rOptions.IsPrintLeftPages();
    bool bPrintRightPages    = rOptions.IsPrintRightPages();
    bool bPrintProspectRTL = rOptions.getIntValue( "PrintProspectRTL",  0 ) ? true : false;

    // get pages for prospect printing according to the 'PageRange'
    // (duplicates and any order allowed!) 
    std::vector< sal_Int32 > aPagesToPrint;
    StringRangeEnumerator::getRangesFromString( 
            aPageRange, aPagesToPrint, 1, nDocPageCount, 0 );
    
    // now fill the vector for calculating the page pairs with the start frames
    // from the above obtained vector
    std::vector< const SwPageFrm * > aVec;
    for ( i = 0; i < sal_Int32(aPagesToPrint.size()); ++i)
    {
        const sal_Int32 nPage = aPagesToPrint[i];
        const SwPageFrm *pFrm = rValidStartFrms[ nPage ];
        aVec.push_back( pFrm );
    }

    // just one page is special ...
    if ( 1 == aVec.size() )    
        aVec.insert( aVec.begin() + 1, 0 ); // insert a second empty page
    else
    {
        // now extend the number of pages to fit a multiple of 4
        // (4 'normal' pages are needed for a single prospect paper
        //  with back and front)
        while( aVec.size() & 3 )
            aVec.push_back( 0 );
    }

    // dann sorge mal dafuer, das alle Seiten in der richtigen
    // Reihenfolge stehen:
    USHORT nSPg = 0, nEPg = aVec.size(), nStep = 1;
    if ( 0 == (nEPg & 1 ))      // ungerade gibt es nicht!
        --nEPg;

    if ( !bPrintLeftPages )
        ++nStep;
    else if ( !bPrintRightPages )
    {
        ++nStep;
        ++nSPg, --nEPg;
    }

    // the number of 'virtual' pages to be printed
    sal_Int32 nCntPage = (( nEPg - nSPg ) / ( 2 * nStep )) + 1;

    for ( USHORT nPrintCount = 0; nSPg < nEPg &&
            nPrintCount < nCntPage; ++nPrintCount )
    {
        pStPage = aVec[ nSPg ];
        const SwPageFrm* pNxtPage = nEPg < aVec.size() ? aVec[ nEPg ] : 0;

        short nRtlOfs = bPrintProspectRTL ? 1 : 0;
        if ( 0 == (( nSPg + nRtlOfs) & 1 ) )     // switch for odd number in LTR, even number in RTL
        {
            const SwPageFrm* pTmp = pStPage;
            pStPage = pNxtPage;
            pNxtPage = pTmp;
        }

        sal_Int32 nFirst = -1, nSecond = -1;
        for ( int nC = 0; nC < 2; ++nC )
        {
            sal_Int32 nPage = -1;
            if ( pStPage )
                nPage = pStPage->GetPhyPageNum();
            if (nC == 0)
                nFirst  = nPage;
            else
                nSecond = nPage;

            pStPage = pNxtPage;
        }
        rPagePairs.push_back( std::pair< sal_Int32, sal_Int32 >(nFirst, nSecond) );

        nSPg = nSPg + nStep;
        nEPg = nEPg - nStep;
    }
    DBG_ASSERT( size_t(nCntPage) == rPagePairs.size(), "size mismatch for number of page pairs" );

    // luckily prospect printing does not make use of post-its so far, 
    // thus we are done here.
}


sal_uInt16 SwDoc::GetPageCount() const
{
    return GetRootFrm() ? GetRootFrm()->GetPageNum() : 0;
}

const Size SwDoc::GetPageSize( sal_uInt16 nPageNum, bool bSkipEmptyPages ) const
{
    Size aSize;
    if( GetRootFrm() && nPageNum )
    {
        const SwPageFrm* pPage = static_cast<const SwPageFrm*>
                                 (GetRootFrm()->Lower());

        while( --nPageNum && pPage->GetNext() )
            pPage = static_cast<const SwPageFrm*>( pPage->GetNext() );

        if( !bSkipEmptyPages && pPage->IsEmptyPage() && pPage->GetNext() )
            pPage = static_cast<const SwPageFrm*>( pPage->GetNext() );

        aSize = pPage->Frm().SSize();
    }
    return aSize;
}


/*************************************************************************
 *			  void UpdateDocStat( const SwDocStat& rStat );
 *************************************************************************/

void SwDoc::UpdateDocStat( SwDocStat& rStat )
{
    if( rStat.bModified )
    {
        rStat.Reset();
        rStat.nPara = 0;        // Default ist auf 1 !!
        SwNode* pNd;

        for( ULONG i = GetNodes().Count(); i; )
        {
            switch( ( pNd = GetNodes()[ --i ])->GetNodeType() )
            {
            case ND_TEXTNODE:
                ((SwTxtNode*)pNd)->CountWords( rStat, 0, ((SwTxtNode*)pNd)->GetTxt().Len() );
                break;
            case ND_TABLENODE:		++rStat.nTbl;	break;
            case ND_GRFNODE:		++rStat.nGrf;	break;
            case ND_OLENODE:		++rStat.nOLE;	break;
            case ND_SECTIONNODE:	break;
            }
        }

        // #i93174#: notes contain paragraphs that are not nodes
        {
            SwFieldType * const pPostits( GetSysFldType(RES_POSTITFLD) );
            SwClientIter aIter(*pPostits);
            SwFmtFld const * pFmtFld =
                static_cast<SwFmtFld const*>(aIter.First( TYPE(SwFmtFld) ));
            while (pFmtFld)
            {
                if (pFmtFld->IsFldInDoc())
                {
                    SwPostItField const * const pField(
                        static_cast<SwPostItField const*>(pFmtFld->GetFld()));
                    rStat.nAllPara += pField->GetNumberOfParagraphs();
                }
                pFmtFld = static_cast<SwFmtFld const*>(aIter.Next());
            }
        }

        rStat.nPage 	= GetRootFrm() ? GetRootFrm()->GetPageNum() : 0;
        rStat.bModified = FALSE;
        SetDocStat( rStat );

        com::sun::star::uno::Sequence < com::sun::star::beans::NamedValue > aStat( rStat.nPage ? 7 : 6);
        sal_Int32 n=0;
        aStat[n].Name = ::rtl::OUString::createFromAscii("TableCount");
        aStat[n++].Value <<= (sal_Int32)rStat.nTbl;
        aStat[n].Name = ::rtl::OUString::createFromAscii("ImageCount");
        aStat[n++].Value <<= (sal_Int32)rStat.nGrf;
        aStat[n].Name = ::rtl::OUString::createFromAscii("ObjectCount");
        aStat[n++].Value <<= (sal_Int32)rStat.nOLE;
        if ( rStat.nPage )
        {
            aStat[n].Name = ::rtl::OUString::createFromAscii("PageCount");
            aStat[n++].Value <<= (sal_Int32)rStat.nPage;
        }
        aStat[n].Name = ::rtl::OUString::createFromAscii("ParagraphCount");
        aStat[n++].Value <<= (sal_Int32)rStat.nPara;
        aStat[n].Name = ::rtl::OUString::createFromAscii("WordCount");
        aStat[n++].Value <<= (sal_Int32)rStat.nWord;
        aStat[n].Name = ::rtl::OUString::createFromAscii("CharacterCount");
        aStat[n++].Value <<= (sal_Int32)rStat.nChar;

        // For e.g. autotext documents there is no pSwgInfo (#i79945)
        SfxObjectShell * const pObjShell( GetDocShell() );
        if (pObjShell)
        {
            const uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
                pObjShell->GetModel(), uno::UNO_QUERY_THROW);
            const uno::Reference<document::XDocumentProperties> xDocProps(
                xDPS->getDocumentProperties());
            // #i96786#: do not set modified flag when updating statistics
            const bool bDocWasModified( IsModified() );
            const ModifyBlocker_Impl b(pObjShell);
            xDocProps->setDocumentStatistics(aStat);
            if (!bDocWasModified)
            {
                ResetModified();
            }
        }

        // event. Stat. Felder Updaten
        SwFieldType *pType = GetSysFldType(RES_DOCSTATFLD);
        pType->UpdateFlds();
    }
}


// Dokument - Info

void SwDoc::DocInfoChgd( )
{
    GetSysFldType( RES_DOCINFOFLD )->UpdateFlds();
    GetSysFldType( RES_TEMPLNAMEFLD )->UpdateFlds();
    SetModified();
}

    // returne zum Namen die im Doc gesetzte Referenz
const SwFmtRefMark* SwDoc::GetRefMark( const String& rName ) const
{
    const SfxPoolItem* pItem;
    USHORT nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_REFMARK );
    for( USHORT n = 0; n < nMaxItems; ++n )
    {
        if( 0 == (pItem = GetAttrPool().GetItem( RES_TXTATR_REFMARK, n ) ))
            continue;

        const SwFmtRefMark* pFmtRef = (SwFmtRefMark*)pItem;
        const SwTxtRefMark* pTxtRef = pFmtRef->GetTxtRefMark();
        if( pTxtRef && &pTxtRef->GetTxtNode().GetNodes() == &GetNodes() &&
            rName.Equals( pFmtRef->GetRefName() ) )
            return pFmtRef;
    }
    return 0;
}

    // returne die RefMark per Index - fuer Uno
const SwFmtRefMark* SwDoc::GetRefMark( USHORT nIndex ) const
{
    const SfxPoolItem* pItem;
    const SwTxtRefMark* pTxtRef;
    const SwFmtRefMark* pRet = 0;

    USHORT nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_REFMARK );
    USHORT nCount = 0;
    for( USHORT n = 0; n < nMaxItems; ++n )
        if( 0 != (pItem = GetAttrPool().GetItem( RES_TXTATR_REFMARK, n )) &&
            0 != (pTxtRef = ((SwFmtRefMark*)pItem)->GetTxtRefMark()) &&
            &pTxtRef->GetTxtNode().GetNodes() == &GetNodes() )
        {
            if(nCount == nIndex)
            {
                pRet = (SwFmtRefMark*)pItem;
                break;
            }
            nCount++;
        }
   return pRet;
}

    // returne die Namen aller im Doc gesetzten Referenzen
    //JP 24.06.96: Ist der ArrayPointer 0 dann returne nur, ob im Doc. eine
    //				RefMark gesetzt ist
    // OS 25.06.96: ab jetzt wird immer die Anzahl der Referenzen returnt
USHORT SwDoc::GetRefMarks( SvStringsDtor* pNames ) const
{
    const SfxPoolItem* pItem;
    const SwTxtRefMark* pTxtRef;

    USHORT nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_REFMARK );
    USHORT nCount = 0;
    for( USHORT n = 0; n < nMaxItems; ++n )
        if( 0 != (pItem = GetAttrPool().GetItem( RES_TXTATR_REFMARK, n )) &&
            0 != (pTxtRef = ((SwFmtRefMark*)pItem)->GetTxtRefMark()) &&
            &pTxtRef->GetTxtNode().GetNodes() == &GetNodes() )
        {
            if( pNames )
            {
                String* pTmp = new String( ((SwFmtRefMark*)pItem)->GetRefName() );
                pNames->Insert( pTmp, nCount );
            }
            nCount ++;
        }

    return nCount;
}

bool SwDoc::IsLoaded() const
{
    return mbLoaded;
}

bool SwDoc::IsUpdateExpFld() const
{
    return mbUpdateExpFld;
}

bool SwDoc::IsNewDoc() const
{
    return mbNewDoc;
}

bool SwDoc::IsPageNums() const
{
  return mbPageNums;
}

void SwDoc::SetPageNums(bool b)
{
    mbPageNums = b;
}

void SwDoc::SetNewDoc(bool b)
{
    mbNewDoc = b;
}

void SwDoc::SetUpdateExpFldStat(bool b)
{
    mbUpdateExpFld = b;
}

void SwDoc::SetLoaded(bool b)
{
    mbLoaded = b;
}

bool SwDoc::IsModified() const
{
    return mbModified;
}

void SwDoc::SetModified()
{
    // --> OD 2005-08-29 #125370#
    SwLayouter::ClearMovedFwdFrms( *this );
    SwLayouter::ClearObjsTmpConsiderWrapInfluence( *this );
    SwLayouter::ClearFrmsNotToWrap( *this );
    // <--
    // --> OD 2006-05-10 #i65250#
    SwLayouter::ClearMoveBwdLayoutInfo( *this );
    // <--
    // dem Link wird der Status returnt, wie die Flags waren und werden
    // 	Bit 0:	-> alter Zustand
    //	Bit 1: 	-> neuer Zustand
    long nCall = mbModified ? 3 : 2;
    mbModified = TRUE;
    pDocStat->bModified = TRUE;
    if( aOle2Link.IsSet() )
    {
        mbInCallModified = TRUE;
        aOle2Link.Call( (void*)nCall );
        mbInCallModified = FALSE;
    }

    if( pACEWord && !pACEWord->IsDeleted() )
        delete pACEWord, pACEWord = 0;
}

void SwDoc::ResetModified()
{
    // dem Link wird der Status returnt, wie die Flags waren und werden
    // 	Bit 0:	-> alter Zustand
    //	Bit 1: 	-> neuer Zustand
    long nCall = mbModified ? 1 : 0;
    mbModified = FALSE;
    // If there is already a document statistic, we assume that
    // it is correct. In this case we reset the modified flag.
    if ( 0 != pDocStat->nChar )
        pDocStat->bModified = FALSE;
    nUndoSavePos = nUndoPos;
    if( nCall && aOle2Link.IsSet() )
    {
        mbInCallModified = TRUE;
        aOle2Link.Call( (void*)nCall );
        mbInCallModified = FALSE;
    }
}


void SwDoc::ReRead( SwPaM& rPam, const String& rGrfName,
                    const String& rFltName, const Graphic* pGraphic,
                    const GraphicObject* pGrafObj )
{
    SwGrfNode *pGrfNd;
    if( ( !rPam.HasMark()
         || rPam.GetPoint()->nNode.GetIndex() == rPam.GetMark()->nNode.GetIndex() )
         && 0 != ( pGrfNd = rPam.GetPoint()->nNode.GetNode().GetGrfNode() ) )
    {
        if( DoesUndo() )
        {
            ClearRedo();
            AppendUndo( new SwUndoReRead( rPam, *pGrfNd ) );
        }

        // Weil nicht bekannt ist, ob sich die Grafik spiegeln laesst,
        // immer das SpiegelungsAttribut zuruecksetzen
        if( RES_MIRROR_GRAPH_DONT != pGrfNd->GetSwAttrSet().
                                                GetMirrorGrf().GetValue() )
            pGrfNd->SetAttr( SwMirrorGrf() );

        pGrfNd->ReRead( rGrfName, rFltName, pGraphic, pGrafObj, TRUE );
        SetModified();
    }
}

BOOL lcl_SpellAndGrammarAgain( const SwNodePtr& rpNd, void* pArgs )
{
    SwTxtNode *pTxtNode = (SwTxtNode*)rpNd->GetTxtNode();
    BOOL bOnlyWrong = *(BOOL*)pArgs;
    if( pTxtNode )
    {
        if( bOnlyWrong )
        {
            if( pTxtNode->GetWrong() &&
                pTxtNode->GetWrong()->InvalidateWrong() )
                pTxtNode->SetWrongDirty( true );
            if( pTxtNode->GetGrammarCheck() &&
                pTxtNode->GetGrammarCheck()->InvalidateWrong() )
                pTxtNode->SetGrammarCheckDirty( true );
        }
        else
        {
            pTxtNode->SetWrongDirty( true );
            if( pTxtNode->GetWrong() )
                pTxtNode->GetWrong()->SetInvalid( 0, STRING_LEN );
            pTxtNode->SetGrammarCheckDirty( true );
            if( pTxtNode->GetGrammarCheck() )
                pTxtNode->GetGrammarCheck()->SetInvalid( 0, STRING_LEN );
        }
    }
    return TRUE;
}

BOOL lcl_CheckSmartTagsAgain( const SwNodePtr& rpNd, void*  )
{
    SwTxtNode *pTxtNode = (SwTxtNode*)rpNd->GetTxtNode();
//	BOOL bOnlyWrong = *(BOOL*)pArgs;
    if( pTxtNode )
    {
        pTxtNode->SetSmartTagDirty( true );
        if( pTxtNode->GetSmartTags() )
        {
//            if ( bOnlyWrong ) // only some smart tag types have been enabled or disabled
//				pTxtNode->GetSmartTags()->SetInvalid( 0, STRING_LEN );
//            else // smart tags all have been enabled or disabled
                pTxtNode->SetSmartTags( NULL );
        }
    }
    return TRUE;
}


/*************************************************************************
 * 		SwDoc::SpellItAgainSam( BOOL bInvalid, BOOL bOnlyWrong )
 *
 * stoesst das Spelling im Idle-Handler wieder an.
 * Wird bInvalid als TRUE uebergeben, so werden zusaetzlich die WrongListen
 * an allen Nodes invalidiert und auf allen Seiten das SpellInvalid-Flag
 * gesetzt.
 * Mit bOnlyWrong kann man dann steuern, ob nur die Bereiche mit falschen
 * Woertern oder die kompletten Bereiche neu ueberprueft werden muessen.
 ************************************************************************/

void SwDoc::SpellItAgainSam( BOOL bInvalid, BOOL bOnlyWrong, BOOL bSmartTags )
{
    ASSERT( GetRootFrm(), "SpellAgain: Where's my RootFrm?" );
    if( bInvalid )
    {
        SwPageFrm *pPage = (SwPageFrm*)GetRootFrm()->Lower();
        while ( pPage )
        {
            if ( bSmartTags )
                pPage->InvalidateSmartTags();

            pPage->InvalidateSpelling();
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        GetRootFrm()->SetNeedGrammarCheck( true );

        if ( bSmartTags )
            GetNodes().ForEach( lcl_CheckSmartTagsAgain, &bOnlyWrong );

        GetNodes().ForEach( lcl_SpellAndGrammarAgain, &bOnlyWrong );
    }

    GetRootFrm()->SetIdleFlags();
}

void SwDoc::InvalidateAutoCompleteFlag()
{
    if( GetRootFrm() )
    {
        SwPageFrm *pPage = (SwPageFrm*)GetRootFrm()->Lower();
        while ( pPage )
        {
            pPage->InvalidateAutoCompleteWords();
            pPage = (SwPageFrm*)pPage->GetNext();
        }
        for( ULONG nNd = 1, nCnt = GetNodes().Count(); nNd < nCnt; ++nNd )
        {
            SwTxtNode* pTxtNode = GetNodes()[ nNd ]->GetTxtNode();
            if ( pTxtNode ) pTxtNode->SetAutoCompleteWordDirty( true );
        }
        GetRootFrm()->SetIdleFlags();
    }
}

const SwFmtINetFmt* SwDoc::FindINetAttr( const String& rName ) const
{
    const SwFmtINetFmt* pItem;
    const SwTxtINetFmt* pTxtAttr;
    const SwTxtNode* pTxtNd;
    USHORT n, nMaxItems = GetAttrPool().GetItemCount( RES_TXTATR_INETFMT );
    for( n = 0; n < nMaxItems; ++n )
        if( 0 != (pItem = (SwFmtINetFmt*)GetAttrPool().GetItem(
            RES_TXTATR_INETFMT, n ) ) &&
            pItem->GetName().Equals( rName ) &&
            0 != ( pTxtAttr = pItem->GetTxtINetFmt()) &&
            0 != ( pTxtNd = pTxtAttr->GetpTxtNode() ) &&
            &pTxtNd->GetNodes() == &GetNodes() )
        {
            return pItem;
        }

    return 0;
}

void SwDoc::Summary( SwDoc* pExtDoc, BYTE nLevel, BYTE nPara, BOOL bImpress )
{
    const SwOutlineNodes& rOutNds = GetNodes().GetOutLineNds();
    if( pExtDoc && rOutNds.Count() )
    {
        USHORT i;
        ::StartProgress( STR_STATSTR_SUMMARY, 0, rOutNds.Count(), GetDocShell() );
        SwNodeIndex aEndOfDoc( pExtDoc->GetNodes().GetEndOfContent(), -1 );
        for( i = 0; i < rOutNds.Count(); ++i )
        {
            ::SetProgressState( i, GetDocShell() );
            const ULONG nIndex = rOutNds[ i ]->GetIndex();
            //BYTE nLvl = ((SwTxtNode*)GetNodes()[ nIndex ])->GetTxtColl()//#outline level,zhaojianwei
                        // ->GetOutlineLevel();
            const int nLvl = ((SwTxtNode*)GetNodes()[ nIndex ])->GetAttrOutlineLevel()-1;//<-end,zhaojianwei
            if( nLvl > nLevel )
                continue;
            USHORT nEndOfs = 1;
            BYTE nWish = nPara;
            ULONG nNextOutNd = i + 1 < rOutNds.Count() ?
                rOutNds[ i + 1 ]->GetIndex() : GetNodes().Count();
            BOOL bKeep = FALSE;
            while( ( nWish || bKeep ) && nIndex + nEndOfs < nNextOutNd &&
                   GetNodes()[ nIndex + nEndOfs ]->IsTxtNode() )
            {
                SwTxtNode* pTxtNode = (SwTxtNode*)GetNodes()[ nIndex+nEndOfs ];
                if( pTxtNode->GetTxt().Len() && nWish )
                    --nWish;
                bKeep = pTxtNode->GetSwAttrSet().GetKeep().GetValue();
                ++nEndOfs;
            }

            SwNodeRange aRange( *rOutNds[ i ], 0, *rOutNds[ i ], nEndOfs );
            GetNodes()._Copy( aRange, aEndOfDoc );
        }
        const SwTxtFmtColls *pColl = pExtDoc->GetTxtFmtColls();
        for( i = 0; i < pColl->Count(); ++i )
            (*pColl)[ i ]->ResetFmtAttr( RES_PAGEDESC, RES_BREAK );
        SwNodeIndex aIndx( pExtDoc->GetNodes().GetEndOfExtras() );
        ++aEndOfDoc;
        while( aIndx < aEndOfDoc )
        {
            SwNode *pNode;
            BOOL bDelete = FALSE;
            if(	(pNode = &aIndx.GetNode())->IsTxtNode() )
            {
                SwTxtNode *pNd = (SwTxtNode*)pNode;
                if( pNd->HasSwAttrSet() )
                    pNd->ResetAttr( RES_PAGEDESC, RES_BREAK );
                if( bImpress )
                {
                    SwTxtFmtColl* pMyColl = pNd->GetTxtColl();
                    //USHORT nHeadLine = static_cast<USHORT>(pMyColl->GetOutlineLevel()==NO_NUMBERING ?//#outlinelevel,zhaojianwei
                    const USHORT nHeadLine = static_cast<USHORT>(
                                !pMyColl->IsAssignedToListLevelOfOutlineStyle() //<-end,zhaojianwei
                                ? RES_POOLCOLL_HEADLINE2
                                : RES_POOLCOLL_HEADLINE1 );
                    pMyColl = pExtDoc->GetTxtCollFromPool( nHeadLine );
                    pNd->ChgFmtColl( pMyColl );
                }
                if( !pNd->Len() &&
                    pNd->StartOfSectionIndex()+2 < pNd->EndOfSectionIndex() )
                {
                    bDelete = TRUE;
                    pExtDoc->GetNodes().Delete( aIndx );
                }
            }
            if( !bDelete )
                ++aIndx;
        }
        ::EndProgress( GetDocShell() );
    }
}

    // loesche den nicht sichtbaren Content aus dem Document, wie z.B.:
    // versteckte Bereiche, versteckte Absaetze
BOOL SwDoc::RemoveInvisibleContent()
{
    BOOL bRet = FALSE;
    StartUndo( UNDO_UI_DELETE_INVISIBLECNTNT, NULL );

    {
        SwTxtNode* pTxtNd;
        SwClientIter aIter( *GetSysFldType( RES_HIDDENPARAFLD ) );
        for( SwFmtFld* pFmtFld = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
                pFmtFld; pFmtFld = (SwFmtFld*)aIter.Next() )
        {
            if( pFmtFld->GetTxtFld() &&
                0 != ( pTxtNd = (SwTxtNode*)pFmtFld->GetTxtFld()->GetpTxtNode() ) &&
                pTxtNd->GetpSwpHints() && pTxtNd->HasHiddenParaField() &&
                &pTxtNd->GetNodes() == &GetNodes() )
            {
                bRet = TRUE;
                SwPaM aPam( *pTxtNd, 0, *pTxtNd, pTxtNd->GetTxt().Len() );

                // Remove hidden paragraph or delete contents:
                // Delete contents if
                // 1. removing the paragraph would result in an empty section or
                // 2. if the paragraph is the last paragraph in the section and
                //    there is no paragraph in front of the paragraph:
                if ( ( 2 == pTxtNd->EndOfSectionIndex() - pTxtNd->StartOfSectionIndex() ) ||
                     ( 1 == pTxtNd->EndOfSectionIndex() - pTxtNd->GetIndex() &&
                       !GetNodes()[ pTxtNd->GetIndex() - 1 ]->GetTxtNode() ) )
                {
                    DeleteRange( aPam );
                }
                else
                {
                    aPam.DeleteMark();
                    DelFullPara( aPam );
                }
            }
        }
    }

    //
    // Remove any hidden paragraph (hidden text attribute)
    //
    for( ULONG n = GetNodes().Count(); n; )
    {
        SwTxtNode* pTxtNd = GetNodes()[ --n ]->GetTxtNode();
        if ( pTxtNd )
        {
            bool bRemoved = false;
            SwPaM aPam( *pTxtNd, 0, *pTxtNd, pTxtNd->GetTxt().Len() );
            if ( pTxtNd->HasHiddenCharAttribute( true ) )
            {
                bRemoved = TRUE;
                bRet = TRUE;

                // Remove hidden paragraph or delete contents:
                // Delete contents if
                // 1. removing the paragraph would result in an empty section or
                // 2. if the paragraph is the last paragraph in the section and
                //    there is no paragraph in front of the paragraph:

                if ( ( 2 == pTxtNd->EndOfSectionIndex() - pTxtNd->StartOfSectionIndex() ) ||
                     ( 1 == pTxtNd->EndOfSectionIndex() - pTxtNd->GetIndex() &&
                       !GetNodes()[ pTxtNd->GetIndex() - 1 ]->GetTxtNode() ) )
                {
                    DeleteRange( aPam );
                }
                else
                {
                    aPam.DeleteMark();
                    DelFullPara( aPam );
                }
            }
            else if ( pTxtNd->HasHiddenCharAttribute( false ) )
            {
                bRemoved = TRUE;
                bRet = TRUE;
                SwScriptInfo::DeleteHiddenRanges( *pTxtNd );
            }

            // --> FME 2006-01-11 #120473#
            // Footnotes/Frames may have been removed, therefore we have
            // to reset n:
            if ( bRemoved )
                n = aPam.GetPoint()->nNode.GetIndex();
            // <--
        }
    }

    {
        // dann noch alle versteckten Bereiche loeschen/leeren
        SwSectionFmts aSectFmts;
        SwSectionFmts& rSectFmts = GetSections();
        USHORT n;

        for( n = rSectFmts.Count(); n; )
        {
            SwSectionFmt* pSectFmt = rSectFmts[ --n ];
            // don't add sections in Undo/Redo
            if( !pSectFmt->IsInNodesArr())
                continue;
            SwSection* pSect = pSectFmt->GetSection();
            if( pSect->CalcHiddenFlag() )
            {
                SwSection* pParent = pSect, *pTmp;
                while( 0 != (pTmp = pParent->GetParent() ))
                {
                    if( pTmp->IsHiddenFlag() )
                        pSect = pTmp;
                    pParent = pTmp;
                }

                if( USHRT_MAX == aSectFmts.GetPos( pSect->GetFmt() ) )
                    aSectFmts.Insert( pSect->GetFmt(), 0 );
            }
            if( pSect->GetCondition().Len() )
            {
                SwSectionData aSectionData( *pSect );
                aSectionData.SetCondition( aEmptyStr );
                aSectionData.SetHidden( false );
                UpdateSection( n, aSectionData );
            }
        }

        if( 0 != ( n = aSectFmts.Count() ))
        {
            while( n )
            {
                SwSectionFmt* pSectFmt = aSectFmts[ --n ];
                SwSectionNode* pSectNd = pSectFmt->GetSectionNode();
                if( pSectNd )
                {
                    bRet = TRUE;
                    SwPaM aPam( *pSectNd );

                    if( pSectNd->StartOfSectionNode()->StartOfSectionIndex() ==
                        pSectNd->GetIndex() - 1 &&
                        pSectNd->StartOfSectionNode()->EndOfSectionIndex() ==
                        pSectNd->EndOfSectionIndex() + 1 )
                    {
                        // nur den Inhalt loeschen
                        SwCntntNode* pCNd = GetNodes().GoNext(
                                                &aPam.GetPoint()->nNode );
                        aPam.GetPoint()->nContent.Assign( pCNd, 0 );
                        aPam.SetMark();
                        aPam.GetPoint()->nNode = *pSectNd->EndOfSectionNode();
                        pCNd = GetNodes().GoPrevious(
                                                &aPam.GetPoint()->nNode );
                        aPam.GetPoint()->nContent.Assign( pCNd, pCNd->Len() );

                        DeleteRange( aPam );
                    }
                    else
                    {
                        // die gesamte Section loeschen
                        aPam.SetMark();
                        aPam.GetPoint()->nNode = *pSectNd->EndOfSectionNode();
                        DelFullPara( aPam );
                    }

                }
            }
            aSectFmts.Remove( 0, aSectFmts.Count() );
        }
    }

    if( bRet )
        SetModified();
    EndUndo( UNDO_UI_DELETE_INVISIBLECNTNT, NULL );
    return bRet;
}
/*-- 11.06.2004 08:34:04---------------------------------------------------

  -----------------------------------------------------------------------*/
BOOL SwDoc::ConvertFieldsToText()
{
    BOOL bRet = FALSE;
    LockExpFlds();
    StartUndo( UNDO_UI_REPLACE, NULL );

    const SwFldTypes* pMyFldTypes = GetFldTypes();
    sal_uInt16 nCount = pMyFldTypes->Count();
    //go backward, field types are removed
    for(sal_uInt16 nType = nCount;  nType > 0;  --nType)
    {
        const SwFieldType *pCurType = pMyFldTypes->GetObject(nType - 1);

        if ( RES_POSTITFLD == pCurType->Which() )
            continue;

        SwClientIter aIter( *(SwFieldType*)pCurType );
        const SwFmtFld* pCurFldFmt = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
        ::std::vector<const SwFmtFld*> aFieldFmts;
        while (pCurFldFmt)
        {
            aFieldFmts.push_back(pCurFldFmt);
            pCurFldFmt = (SwFmtFld*)aIter.Next();
        }
        ::std::vector<const SwFmtFld*>::iterator aBegin = aFieldFmts.begin();
        ::std::vector<const SwFmtFld*>::iterator aEnd = aFieldFmts.end();
        while(aBegin != aEnd)
        {
            const SwTxtFld *pTxtFld = (*aBegin)->GetTxtFld();
            // skip fields that are currently not in the document
            // e.g. fields in undo or redo array

            BOOL bSkip = !pTxtFld ||
                         !pTxtFld->GetpTxtNode()->GetNodes().IsDocNodes();

            if (!bSkip)
            {
                BOOL bInHeaderFooter = IsInHeaderFooter(SwNodeIndex(*pTxtFld->GetpTxtNode()));
                const SwFmtFld& rFmtFld = pTxtFld->GetFld();
                const SwField*  pField = rFmtFld.GetFld();

                //#i55595# some fields have to be excluded in headers/footers
                USHORT nWhich = pField->GetTyp()->Which();
                if(!bInHeaderFooter ||
                        (nWhich != RES_PAGENUMBERFLD &&
                        nWhich != RES_CHAPTERFLD &&
                        nWhich != RES_GETEXPFLD&&
                        nWhich != RES_SETEXPFLD&&
                        nWhich != RES_INPUTFLD&&
                        nWhich != RES_REFPAGEGETFLD&&
                        nWhich != RES_REFPAGESETFLD))
                {
                    String sText = pField->GetCntnt();
                    //database fields should not convert their command into text
                    if( RES_DBFLD == pCurType->Which() && !static_cast<const SwDBField*>(pField)->IsInitialized())
                        sText.Erase();

                    //now remove the field and insert the string
                    SwPaM aPam1(*pTxtFld->GetpTxtNode(), *pTxtFld->GetStart());
                    aPam1.Move();
                    //insert first to keep the field's attributes
                    InsertString( aPam1, sText );
                    SwPaM aPam2(*pTxtFld->GetpTxtNode(), *pTxtFld->GetStart());
                    aPam2.SetMark();
                    aPam2.Move();
                    DeleteAndJoin(aPam2);//remove the field 
                }
            }
            ++aBegin;
        }
    }

    if( bRet )
        SetModified();
    EndUndo( UNDO_UI_REPLACE, NULL );
    UnlockExpFlds();
    return bRet;

}

bool SwDoc::IsVisibleLinks() const
{
    return mbVisibleLinks;
}

void SwDoc::SetVisibleLinks(bool bFlag)
{
    mbVisibleLinks = bFlag;
}

sfx2::LinkManager& SwDoc::GetLinkManager()
{
    return *pLinkMgr;
}

const sfx2::LinkManager& SwDoc::GetLinkManager() const
{
    return *pLinkMgr;
}

void SwDoc::SetLinksUpdated(const bool bNewLinksUpdated)
{
    mbLinksUpdated = bNewLinksUpdated;
}

bool SwDoc::LinksUpdated() const
{
    return mbLinksUpdated;
}

    // embedded alle lokalen Links (Bereiche/Grafiken)
bool SwDoc::EmbedAllLinks()
{
    BOOL bRet = FALSE;
    sfx2::LinkManager& rLnkMgr = GetLinkManager();
    const ::sfx2::SvBaseLinks& rLnks = rLnkMgr.GetLinks();
    if( rLnks.Count() )
    {
        BOOL bDoesUndo = DoesUndo();
        DoUndo( FALSE );

        for( USHORT n = 0; n < rLnks.Count(); ++n )
        {
            ::sfx2::SvBaseLink* pLnk = &(*rLnks[ n ]);
            if( pLnk &&
                ( OBJECT_CLIENT_GRF == pLnk->GetObjType() ||
                  OBJECT_CLIENT_FILE == pLnk->GetObjType() ) &&
                pLnk->ISA( SwBaseLink ) )
            {
                ::sfx2::SvBaseLinkRef xLink = pLnk;
                USHORT nCount = rLnks.Count();

                String sFName;
                rLnkMgr.GetDisplayNames( xLink, 0, &sFName, 0, 0 );

                INetURLObject aURL( sFName );
                if( INET_PROT_FILE == aURL.GetProtocol() ||
                    INET_PROT_CID == aURL.GetProtocol() )
                {
                    // dem Link sagen, das er aufgeloest wird!
                    xLink->Closed();

                    // falls einer vergessen hat sich auszutragen
                    if( xLink.Is() )
                        rLnkMgr.Remove( xLink );

                    if( nCount != rLnks.Count() + 1 )
                        n = 0;		// wieder von vorne anfangen, es wurden
                                    // mehrere Links entfernt
                    bRet = TRUE;
                }
            }
        }

        DelAllUndoObj();
        DoUndo( bDoesUndo );
        SetModified();
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL SwDoc::IsInsTblFormatNum() const
{
    return SW_MOD()->IsInsTblFormatNum(get(IDocumentSettingAccess::HTML_MODE));
}

BOOL SwDoc::IsInsTblChangeNumFormat() const
{
    return SW_MOD()->IsInsTblChangeNumFormat(get(IDocumentSettingAccess::HTML_MODE));
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL SwDoc::IsInsTblAlignNum() const
{
    return SW_MOD()->IsInsTblAlignNum(get(IDocumentSettingAccess::HTML_MODE));
}

        // setze das InsertDB als Tabelle Undo auf:
void SwDoc::AppendUndoForInsertFromDB( const SwPaM& rPam, BOOL bIsTable )
{
    if( bIsTable )
    {
        const SwTableNode* pTblNd = rPam.GetPoint()->nNode.GetNode().FindTableNode();
        if( pTblNd )
        {
            SwUndoCpyTbl* pUndo = new SwUndoCpyTbl;
            pUndo->SetTableSttIdx( pTblNd->GetIndex() );
            AppendUndo( pUndo );
        }
    }
    else if( rPam.HasMark() )
    {
        SwUndoCpyDoc* pUndo = new SwUndoCpyDoc( rPam );
        pUndo->SetInsertRange( rPam, FALSE );
        AppendUndo( pUndo );
    }
}

void SwDoc::ChgTOX(SwTOXBase & rTOX, const SwTOXBase & rNew)
{
    if (DoesUndo())
    {
        DelAllUndoObj();

        SwUndo * pUndo = new SwUndoTOXChange(&rTOX, rNew);

        AppendUndo(pUndo);
    }

    rTOX = rNew;

    if (rTOX.ISA(SwTOXBaseSection))
    {
        static_cast<SwTOXBaseSection &>(rTOX).Update();
        static_cast<SwTOXBaseSection &>(rTOX).UpdatePageNum();
    }
}

// #111827#
String SwDoc::GetPaMDescr(const SwPaM & rPam) const
{
    String aResult;
    bool bOK = false;

    if (rPam.GetNode(TRUE) == rPam.GetNode(FALSE))
    {
        SwTxtNode * pTxtNode = rPam.GetNode(TRUE)->GetTxtNode();

        if (0 != pTxtNode)
        {
            xub_StrLen nStart = rPam.Start()->nContent.GetIndex();
            xub_StrLen nEnd = rPam.End()->nContent.GetIndex();

            aResult += String(SW_RES(STR_START_QUOTE));
            aResult += ShortenString(pTxtNode->GetTxt().
                                     Copy(nStart, nEnd - nStart),
                                     nUndoStringLength,
                                     String(SW_RES(STR_LDOTS)));
            aResult += String(SW_RES(STR_END_QUOTE));

            bOK = true;
        }
    }
    else if (0 != rPam.GetNode(TRUE))
    {
        if (0 != rPam.GetNode(FALSE))
            aResult += String(SW_RES(STR_PARAGRAPHS));

        bOK = true;
    }

    if (! bOK)
        aResult += String("??", RTL_TEXTENCODING_ASCII_US);

    return aResult;
}

// -> #111840#
SwField * SwDoc::GetField(const SwPosition & rPos)
{
    SwTxtFld * const pAttr = GetTxtFld(rPos);

    return (pAttr) ? const_cast<SwField *>( pAttr->GetFld().GetFld() ) : 0;
}

SwTxtFld * SwDoc::GetTxtFld(const SwPosition & rPos)
{
    SwTxtNode * const pNode = rPos.nNode.GetNode().GetTxtNode();

    return (pNode)
        ? static_cast<SwTxtFld*>( pNode->GetTxtAttrForCharAt(
                    rPos.nContent.GetIndex(), RES_TXTATR_FIELD) )
        : 0;
}
// <- #111840#

bool SwDoc::ContainsHiddenChars() const
{
    for( ULONG n = GetNodes().Count(); n; )
    {
        SwNode* pNd = GetNodes()[ --n ];
        if ( ND_TEXTNODE == pNd->GetNodeType() &&
             ((SwTxtNode*)pNd)->HasHiddenCharAttribute( false ) )
            return true;
    }

    return false;
}

SwUnoCrsr* SwDoc::CreateUnoCrsr( const SwPosition& rPos, BOOL bTblCrsr )
{
    SwUnoCrsr* pNew;
    if( bTblCrsr )
        pNew = new SwUnoTableCrsr( rPos );
    else
        pNew = new SwUnoCrsr( rPos );

    pUnoCrsrTbl->Insert( pNew, pUnoCrsrTbl->Count() );
    return pNew;
}

void SwDoc::ChkCondColls()
{
     for (USHORT n = 0; n < pTxtFmtCollTbl->Count(); n++)
     {
        SwTxtFmtColl *pColl = (*pTxtFmtCollTbl)[n];

        if (RES_CONDTXTFMTCOLL == pColl->Which())
        {
            SwClientIter aIter(*pColl);

            SwClient * pClient = aIter.First(TYPE(SwTxtNode));
            while (pClient)
            {
                SwTxtNode * pTxtNode = static_cast<SwTxtNode *>(pClient);

                pTxtNode->ChkCondColl();

                pClient = aIter.Next();
            }
        }
     }
}

void SwDoc::setExternalData(::sw::tExternalDataType eType,
                            ::sw::tExternalDataPointer pPayload)
{
    m_externalData[eType] = pPayload;
}

::sw::tExternalDataPointer SwDoc::getExternalData(::sw::tExternalDataType eType)
{
    return m_externalData[eType];
}
