/*************************************************************************
 *
 *  $RCSfile: EnhancedPDFExportHelper.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 10:40:16 $
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

#ifndef _COM_SUN_STAR_EMBED_XEMBEDDEDOBJECT_HPP_
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#endif

#ifndef _ENHANCEDPDFEXPORTHELPER_HXX
#include <EnhancedPDFExportHelper.hxx>
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif
#ifndef _SVX_ADJITEM_HXX
#include <svx/adjitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif

#ifndef _SWATRSET_HXX
#include <swatrset.hxx>
#endif
#ifndef _FRMATR_HXX
#include <frmatr.hxx>
#endif
#ifndef _PARATR_HXX
#include <paratr.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _NDOLE_HXX
#include <ndole.hxx>
#endif
#ifndef _SECTION_HXX
#include <section.hxx>
#endif
#ifndef _TOX_HXX
#include <tox.hxx>
#endif
#ifndef _FMTFLD_HXX
#include <fmtfld.hxx>
#endif
#ifndef _FORMAT_HXX
#include <format.hxx>
#endif
#ifndef _TXTINET_HXX
#include <txtinet.hxx>
#endif
#ifndef _FMTINFMT_HXX
#include <fmtinfmt.hxx>
#endif
#ifndef _FMTANCHR_HXX
#include <fmtanchr.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _VISCRS_HXX
#include <viscrs.hxx>
#endif
#ifndef _TXTFLD_HXX
#include <txtfld.hxx>
#endif
#ifndef _REFFLD_HXX
#include <reffld.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _CRSSKIP_HXX
#include <crsskip.hxx>
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>
#endif
#ifndef _DOCUFLD_HXX
#include <docufld.hxx>
#endif
#ifndef _SWREGION_HXX
#include <swregion.hxx>
#endif
#ifndef _FTNIDX_HXX
#include <ftnidx.hxx>
#endif
#ifndef _TXTFTN_HXX
#include <txtftn.hxx>
#endif
#ifndef _FMTFTN_HXX
#include <fmtftn.hxx>
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>
#endif
#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _ROWFRM_HXX
#include <rowfrm.hxx>
#endif
#ifndef _CELLFRM_HXX
#include <cellfrm.hxx>
#endif
#ifndef _FLOWFRM_HXX
#include <flowfrm.hxx>
#endif
#ifndef _SECTFRM_HXX
#include <sectfrm.hxx>
#endif
#ifndef _FLYFRM_HXX
#include <flyfrm.hxx>
#endif
#ifndef _NOTXTFRM_HXX
#include <notxtfrm.hxx>
#endif

#ifndef _PORFLD_HXX
#include <porfld.hxx>
#endif
#include <itrpaint.hxx>
#include <stack>

#include <tools/globname.hxx>

//
// Some global data structures
//
LinkIdMap SwEnhancedPDFExportHelper::aLinkIdMap;
FrmTagIdMap SwEnhancedPDFExportHelper::aFrmTagIdMap;


#ifndef PRODUCT

static std::vector< USHORT > aStructStack;

void lcl_DBGCheckStack()
{
    /* NonStructElement = 0     Document = 1        Part = 2
     * Article = 3              Section = 4         Division = 5
     * BlockQuote = 6           Caption = 7         TOC = 8
     * TOCI = 9                 Index = 10          Paragraph = 11
     * Heading = 12             H1-6 = 13 - 18      List = 19
     * ListItem = 20            LILabel = 21        LIBody = 22
     * Table = 23               TableRow = 24       TableHeader = 25
     * TableData = 26           Span = 27           Quote = 28
     * Note = 29                Reference = 30      BibEntry = 31
     * Code = 32                Link = 33           Figure = 34
     * Formula = 35             Form = 36           Continued frame = 99
     */

    USHORT nElement;
    std::vector< USHORT >::iterator aIter;
    for ( aIter = aStructStack.begin(); aIter != aStructStack.end(); ++aIter )
    {
        nElement = *aIter;
    }
}

#endif


// List all frames for which the NonStructElement tag is set:
bool lcl_IsInNonStructEnv( const SwFrm& rFrm )
{
    bool bRet = false;

    if ( 0 != rFrm.FindFooterOrHeader() &&
           !rFrm.IsHeaderFrm() && !rFrm.IsFooterFrm() )
    {
        bRet = true;
    }
    else if ( rFrm.IsInTab() && !rFrm.IsTabFrm() )
    {
        const SwTabFrm* pTabFrm = rFrm.FindTabFrm();
        if ( rFrm.GetUpper() != pTabFrm &&
             pTabFrm->IsFollow() && pTabFrm->IsInHeadline( rFrm ) )
             bRet = true;
    }

    return bRet;
}

// Generate key from frame for reopening tags:
void* lcl_GetKey( const SwFrm& rFrm )
{
    void* pKey = 0;

    if ( rFrm.IsPageFrm() )
        pKey = (void*)static_cast<const SwPageFrm&>(rFrm).GetFmt()->GetDoc();
    else if ( rFrm.IsTxtFrm() )
        pKey = (void*)static_cast<const SwTxtFrm&>(rFrm).GetTxtNode();
    else if ( rFrm.IsSctFrm() )
        pKey = (void*)static_cast<const SwSectionFrm&>(rFrm).GetSection();
    else if ( rFrm.IsTabFrm() )
        pKey = (void*)static_cast<const SwTabFrm&>(rFrm).GetTable();
    else if ( rFrm.IsRowFrm() )
        pKey = (void*)static_cast<const SwRowFrm&>(rFrm).GetTabLine();
    else if ( rFrm.IsCellFrm() )
        pKey = (void*)static_cast<const SwCellFrm&>(rFrm).GetTabBox();

    return pKey;
}


/*
 * SwTaggedPDFHelper::SwTaggedPDFHelper()
 */
SwTaggedPDFHelper::SwTaggedPDFHelper( const Frm_Info* pFrmInfo,
                                      const Por_Info* pPorInfo,
                                      OutputDevice& rOut )
  : nEndStructureElement( 0 ),
    nRestoreCurrentTag( -1 ),
    mpFrmInfo( pFrmInfo ),
    mpPorInfo( pPorInfo )
{
    mpPDFExtOutDevData =
        PTR_CAST( vcl::PDFExtOutDevData, rOut.GetExtOutDevData() );

    if ( mpPDFExtOutDevData && mpPDFExtOutDevData->GetIsExportTaggedPDF() )
    {
#ifndef PRODUCT
        sal_Int32 nCurrentStruct = mpPDFExtOutDevData->GetCurrentStructureElement();
        lcl_DBGCheckStack();
#endif
        if ( mpFrmInfo )
            BeginBlockStructureElements();
        else if ( mpPorInfo )
            BeginInlineStructureElements();
        else
            BeginTag( vcl::PDFWriter::NonStructElement );

#ifndef PRODUCT
        nCurrentStruct = mpPDFExtOutDevData->GetCurrentStructureElement();
        lcl_DBGCheckStack();
#endif
    }
}


/*
 * SwTaggedPDFHelper::~SwTaggedPDFHelper()
 */
SwTaggedPDFHelper::~SwTaggedPDFHelper()
{
    if ( mpPDFExtOutDevData && mpPDFExtOutDevData->GetIsExportTaggedPDF() )
    {
#ifndef PRODUCT
        sal_Int32 nCurrentStruct = mpPDFExtOutDevData->GetCurrentStructureElement();
        lcl_DBGCheckStack();
#endif
        EndStructureElements();

#ifndef PRODUCT
        nCurrentStruct = mpPDFExtOutDevData->GetCurrentStructureElement();
        lcl_DBGCheckStack();
#endif
    }
}


/*
 * SwTaggedPDFHelper::CheckReopenTag()
 */
bool SwTaggedPDFHelper::CheckReopenTag()
{
    bool bRet = false;
    if ( mpFrmInfo )
    {
        FrmTagIdMap& rFrmTagIdMap = SwEnhancedPDFExportHelper::GetFrmTagIdMap();
        const SwFrm& rFrm = mpFrmInfo->mrFrm;
        const SwFrm* pKeyFrm = 0;

        // Reopen an existing structure element if
        // - rFrm is not the first page frame (reopen Document tag)
        // - rFrm is a follow frame (reopen Master tag)
        // - rFrm is a fly frame anchored at content (reopen Anchor paragraph tag)
        // - rFrm is a fly frame anchord at page (reopen Document tag)
        // - rFrm is a follow flow row (reopen TableRow tag)
        // - rFrm is a cell frame in a follow flow row (reopen TableData tag)
        if ( ( rFrm.IsPageFrm() && static_cast<const SwPageFrm&>(rFrm).GetPrev() ) ||
             ( rFrm.IsFlowFrm() &&
              (SwFlowFrm::CastFlowFrm( &rFrm )->IsFollow() ) ) ||
             ( ( rFrm.IsRowFrm() || rFrm.IsCellFrm() ) &&
               rFrm.IsInFollowFlowRow() ) )
        {
            pKeyFrm = &rFrm;
        }
        else if ( rFrm.IsFlyFrm() )
        {
            const SwFmtAnchor& rAnchor =
                static_cast<const SwFlyFrm*>(&rFrm)->GetFmt()->GetAnchor();
            if ( FLY_AT_CNTNT == rAnchor.GetAnchorId() ||
                 FLY_AUTO_CNTNT == rAnchor.GetAnchorId() ||
                 FLY_PAGE == rAnchor.GetAnchorId() )
            {
                pKeyFrm = static_cast<const SwFlyFrm&>(rFrm).GetAnchorFrm();
            }
        }

        void* pKey = 0;
        if ( pKeyFrm && 0 != ( pKey = lcl_GetKey( *pKeyFrm ) ) )
        {
            FrmTagIdMap::iterator aIter =  rFrmTagIdMap.find( pKey );

            ASSERT( aIter != rFrmTagIdMap.end(), "Could not find information to reopen paragraph" )

            sal_Int32 nId = (*aIter).second;
            nRestoreCurrentTag = mpPDFExtOutDevData->GetCurrentStructureElement();
            bool bSuccess = mpPDFExtOutDevData->SetCurrentStructureElement( nId );
            ASSERT( bSuccess, "Failed to reopen flow frame tag" )

#ifndef PRODUCT
            aStructStack.push_back( 99 );
#endif
            bRet = rFrm.IsPageFrm() || rFrm.IsFlowFrm() ||
                   rFrm.IsRowFrm()  || rFrm.IsCellFrm();
        }
    }
    return bRet;
}


/*
 * SwTaggedPDFHelper::CheckRestoreTag()
 */
bool SwTaggedPDFHelper::CheckRestoreTag() const
{
    bool bRet = false;
    if ( nRestoreCurrentTag != -1 )
    {
        bool bSuccess = mpPDFExtOutDevData->SetCurrentStructureElement( nRestoreCurrentTag );
        ASSERT( bSuccess, "Failed to restore reopened tag" )

#ifndef PRODUCT
        aStructStack.pop_back();
#endif

        bRet = true;
    }

    return bRet;
}


/*
 * SwTaggedPDFHelper::BeginTag()
 */
void SwTaggedPDFHelper::BeginTag( vcl::PDFWriter::StructElement eType )
{
    // The paragraph id has to be stored if
    // Store the id of the current structure element if
    // - rFrm is the first page frame
    // - rFrm is a master frame
    // - rFrm has objects anchored to it
    // - rFrm is a row frame or cell frame in a split table row
    void* pKey = 0;
    if ( mpFrmInfo )
    {
        const SwFrm& rFrm = mpFrmInfo->mrFrm;
        if ( ( rFrm.IsPageFrm() &&
              !static_cast<const SwPageFrm&>(rFrm).GetPrev() ) ||
             ( rFrm.IsFlowFrm() &&
              !(SwFlowFrm::CastFlowFrm( &rFrm )->IsFollow() ) &&
               (SwFlowFrm::CastFlowFrm( &rFrm )->HasFollow() ) ) ||
             ( rFrm.IsTxtFrm() && rFrm.GetDrawObjs() ) ||
             ( ( rFrm.IsRowFrm() || rFrm.IsCellFrm() ) &&
               rFrm.IsInSplitTableRow() ) )
        {
            pKey = lcl_GetKey( rFrm );
        }
    }

    const sal_Int32 nId = mpPDFExtOutDevData->BeginStructureElement( eType );
    ++nEndStructureElement;

#ifndef PRODUCT
    aStructStack.push_back( eType );
#endif

    if ( pKey )
    {
        FrmTagIdMap& rFrmTagIdMap = SwEnhancedPDFExportHelper::GetFrmTagIdMap();
        rFrmTagIdMap[ pKey ] = nId;
    }

    SetAttributes( eType );
}


/*
 * SwTaggedPDFHelper::EndTag()
 */
void SwTaggedPDFHelper::EndTag()
{
    mpPDFExtOutDevData->EndStructureElement();

#ifndef PRODUCT
    aStructStack.pop_back();
#endif
}


/*
 * SwTaggedPDFHelper::SetAttributes()
 *
 * Sets the attributes according to the structure type.
 */
void SwTaggedPDFHelper::SetAttributes( vcl::PDFWriter::StructElement eType )
{
    vcl::PDFWriter::StructAttributeValue eVal;
    sal_Int32 nVal;

    /*
     * ATTRIBUTES FOR BLSE
     */
    if ( mpFrmInfo )
    {
        const SwFrm* pFrm = &mpFrmInfo->mrFrm;
        SWRECTFN( pFrm )

        bool bPlacement = false;
        bool bWritingMode = false;
        bool bSpaceBefore = false;
        bool bSpaceAfter = false;
        bool bStartIndent = false;
        bool bEndIndent = false;
        bool bTextIndent = false;
        bool bTextAlign = false;
        bool bAlternateText = false;
        bool bWidth = false;
        bool bHeight = false;
        bool bBox = false;

        //
        // Check which attributes to set:
        //
        switch ( eType )
        {
            case vcl::PDFWriter::Document :
                bWritingMode = true;
                break;

            case vcl::PDFWriter::Table :
                bPlacement =
                bWritingMode =
                bSpaceBefore =
                bSpaceAfter =
                bStartIndent =
                bEndIndent =
                bWidth =
                bHeight =
                bBox = true;
                break;

            case vcl::PDFWriter::TableRow :
                bPlacement =
                bWritingMode = true;
                break;

            case vcl::PDFWriter::TableHeader :
            case vcl::PDFWriter::TableData :
                bPlacement =
                bWritingMode =
                bWidth =
                bHeight = true;
                break;

            case vcl::PDFWriter::H1 :
            case vcl::PDFWriter::H2 :
            case vcl::PDFWriter::H3 :
            case vcl::PDFWriter::H4 :
            case vcl::PDFWriter::H5 :
            case vcl::PDFWriter::H6 :
            case vcl::PDFWriter::Paragraph :
                bPlacement =
                bWritingMode =
                bSpaceBefore =
                bSpaceAfter =
                bStartIndent =
                bEndIndent =
                bTextIndent =
                bTextAlign = true;
                break;

            case vcl::PDFWriter::Formula :
            case vcl::PDFWriter::Figure :
                bPlacement =
                bAlternateText =
                bWidth =
                bHeight =
                bBox = true;
                break;
        }

        //
        // Set the attributes:
        //
        if ( bPlacement )
        {
            eVal = vcl::PDFWriter::TableHeader == eType ||
                   vcl::PDFWriter::TableData   == eType ?
                   vcl::PDFWriter::Inline :
                   vcl::PDFWriter::Block;

            mpPDFExtOutDevData->SetStructureAttribute( vcl::PDFWriter::Placement, eVal );
        }

        if ( bWritingMode )
        {
            eVal =  pFrm->IsVertical() ?
                    vcl::PDFWriter::TbRl :
                    pFrm->IsRightToLeft() ?
                    vcl::PDFWriter::RlTb :
                    vcl::PDFWriter::LrTb;

            if ( vcl::PDFWriter::LrTb != eVal )
                mpPDFExtOutDevData->SetStructureAttribute( vcl::PDFWriter::WritingMode, eVal );
        }

        if ( bSpaceBefore )
        {
            nVal = (pFrm->*fnRect->fnGetTopMargin)();
            if ( 0 != nVal )
                mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::SpaceBefore, nVal );
        }

        if ( bSpaceAfter )
        {
            nVal = (pFrm->*fnRect->fnGetBottomMargin)();
            if ( 0 != nVal )
                mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::SpaceAfter, nVal );
        }

        if ( bStartIndent )
        {
            nVal = (pFrm->*fnRect->fnGetLeftMargin)();
            if ( 0 != nVal )
                mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::StartIndent, nVal );
        }

        if ( bEndIndent )
        {
            nVal = (pFrm->*fnRect->fnGetRightMargin)();
            if ( 0 != nVal )
                mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::EndIndent, nVal );
        }

        if ( bTextIndent )
        {
            ASSERT( pFrm->IsTxtFrm(), "Frame type <-> tag attribute mismatch" )
            const SvxLRSpaceItem &rSpace =
                static_cast<const SwTxtFrm*>(pFrm)->GetTxtNode()->GetSwAttrSet().GetLRSpace();
            nVal =  rSpace.GetTxtFirstLineOfst();
            if ( 0 != nVal )
                mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::TextIndent, nVal );
        }

        if ( bTextAlign )
        {
            ASSERT( pFrm->IsTxtFrm(), "Frame type <-> tag attribute mismatch" )
            const SwAttrSet& aSet = static_cast<const SwTxtFrm*>(pFrm)->GetTxtNode()->GetSwAttrSet();
            const USHORT nAdjust = aSet.GetAdjust().GetAdjust();
            if ( SVX_ADJUST_BLOCK == nAdjust || SVX_ADJUST_CENTER == nAdjust ||
                 (  pFrm->IsRightToLeft() && SVX_ADJUST_LEFT == nAdjust ||
                   !pFrm->IsRightToLeft() && SVX_ADJUST_RIGHT == nAdjust ) )
            {
                eVal = SVX_ADJUST_BLOCK == nAdjust ?
                       vcl::PDFWriter::Justify :
                       SVX_ADJUST_CENTER == nAdjust ?
                       vcl::PDFWriter::Center :
                       vcl::PDFWriter::End;

                mpPDFExtOutDevData->SetStructureAttribute( vcl::PDFWriter::TextAlign, eVal );
            }
        }

        if ( bAlternateText )
        {
            ASSERT( pFrm->IsFlyFrm(), "Frame type <-> tag attribute mismatch" )
            const SwFlyFrm* pFly = static_cast<const SwFlyFrm*>(pFrm);
            if ( pFly->Lower() && pFly->Lower()->IsNoTxtFrm() )
            {
                const SwNoTxtFrm* pNoTxtFrm   = static_cast<const SwNoTxtFrm*>(pFly->Lower());
                const SwNoTxtNode* pNoTxtNode = static_cast<const SwNoTxtNode*>(pNoTxtFrm->GetNode());

                const String aAlternateTxt( pNoTxtNode->GetAlternateText() );
                mpPDFExtOutDevData->SetAlternateText( aAlternateTxt );
            }
        }

        if ( bWidth )
        {
            nVal = (pFrm->Frm().*fnRect->fnGetWidth)();
            mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::Width, nVal );
        }

        if ( bHeight )
        {
            nVal = (pFrm->Frm().*fnRect->fnGetHeight)();
            mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::Height, nVal );
        }

        if ( bBox )
        {
            // BBox only for non-split tables:
            if ( vcl::PDFWriter::Table != eType ||
                 ( pFrm->IsTabFrm() &&
                   !static_cast<const SwTabFrm*>(pFrm)->IsFollow() &&
                   !static_cast<const SwTabFrm*>(pFrm)->HasFollow() ) )
                mpPDFExtOutDevData->SetStructureBoundingBox( pFrm->Frm().SVRect() );
        }
    }

    /*
     * ATTRIBUTES FOR ILSE
     */
    else if ( mpPorInfo )
    {
        const SwLinePortion* pPor = &mpPorInfo->mrPor;
        const SwTxtPaintInfo& rInf = mpPorInfo->mrTxtPainter.GetInfo();

        bool bActualText = false;
        bool bBaselineShift = false;
        bool bTextDecorationType = false;
        bool bLinkAttribute = false;

        //
        // Check which attributes to set:
        //
        switch ( eType )
        {
            case vcl::PDFWriter::Span :
                bActualText =
                bBaselineShift =
                bTextDecorationType = true;
                break;

            case vcl::PDFWriter::Link :
                bTextDecorationType =
                bBaselineShift =
                bLinkAttribute = true;
                break;
        }

        if ( bActualText )
        {
            const String aActualTxt( rInf.GetTxt(), rInf.GetIdx(), pPor->GetLen() );
            mpPDFExtOutDevData->SetActualText( aActualTxt );
        }

        if ( bBaselineShift )
        {
            // TODO: Calculate correct values!
            nVal = rInf.GetFont()->GetEscapement();
            if ( nVal > 0 ) nVal = 33;
            else if ( nVal < 0 ) nVal = -33;

            if ( 0 != nVal )
            {
                nVal = nVal * pPor->Height() / 100;
                mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::BaselineShift, nVal );
            }
        }

        if ( bTextDecorationType )
        {
            if ( UNDERLINE_NONE    != rInf.GetFont()->GetUnderline() )
                mpPDFExtOutDevData->SetStructureAttribute( vcl::PDFWriter::TextDecorationType, vcl::PDFWriter::Underline );
            if ( STRIKEOUT_NONE    != rInf.GetFont()->GetStrikeout() )
                mpPDFExtOutDevData->SetStructureAttribute( vcl::PDFWriter::TextDecorationType, vcl::PDFWriter::LineThrough );
            if ( EMPHASISMARK_NONE != rInf.GetFont()->GetEmphasisMark() )
                mpPDFExtOutDevData->SetStructureAttribute( vcl::PDFWriter::TextDecorationType, vcl::PDFWriter::Overline );
        }

        if ( bLinkAttribute )
        {
            const LinkIdMap& rLinkIdMap = SwEnhancedPDFExportHelper::GetLinkIdMap();
            SwRect aPorRect;
            rInf.CalcRect( *pPor, &aPorRect );
            const Point aPorCenter = aPorRect.Center();
            LinkIdMap::const_iterator aIter;
            for ( aIter = rLinkIdMap.begin(); aIter != rLinkIdMap.end(); ++aIter )
            {
                const SwRect& rLinkRect = (*aIter).first;
                if ( rLinkRect.IsInside( aPorCenter ) )
                {
                    sal_Int32 nLinkId = (*aIter).second;
                    mpPDFExtOutDevData->SetStructureAttributeNumerical( vcl::PDFWriter::LinkAnnotation, nLinkId );
                    break;
                }
            }
        }
    }
}


/*
 * SwTaggedPDFHelper::BeginBlockStructureElements()
 */
void SwTaggedPDFHelper::BeginBlockStructureElements()
{
    const SwFrm* pFrm = &mpFrmInfo->mrFrm;

    //
    // Lowers of NonStructureElements should not be considered:
    //
    if ( lcl_IsInNonStructEnv( *pFrm ) )
        return;

    // Check if we have to reopen an existing structure element.
    // This has to be done e.g., if pFrm is a follow frame.
    if ( CheckReopenTag() )
        return;

    USHORT nPDFType = USHRT_MAX;

    switch ( pFrm->GetType() )
    {
        /*
         * GROUPING ELEMENTS
         */

        case FRM_PAGE :
            //
            // Document: Document
            //
            nPDFType = vcl::PDFWriter::Document;
            break;

        case FRM_HEADER :
        case FRM_FOOTER :
            //
            // Header, Footer: NonStructElement
            //
            nPDFType = vcl::PDFWriter::NonStructElement;
            break;

        case FRM_FTNCONT :
            //
            // Footnote container: Division
            //
            nPDFType = vcl::PDFWriter::Division;
            break;

        case FRM_FTN :
            //
            // Footnote frame: Note
            //
            // Note: vcl::PDFWriter::Note is actually a ILSE. Nevertheless
            // we treat it like a grouping element!
            nPDFType = vcl::PDFWriter::Note;
            break;

        case FRM_SECTION :
            //
            // Section: TOX, Index, or Sect
            //
            {
                const SwSection* pSection =
                        static_cast<const SwSectionFrm*>(pFrm)->GetSection();
                if ( TOX_CONTENT_SECTION == pSection->GetType() )
                {
                    const SwTOXBase* pTOXBase = pSection->GetTOXBase();
                    if ( pTOXBase )
                    {
                        if ( TOX_INDEX == pTOXBase->GetType() )
                            nPDFType = vcl::PDFWriter::Index;
                        else
                            nPDFType = vcl::PDFWriter::TOC;
                    }
                }
                else if ( CONTENT_SECTION == pSection->GetType() )
                {
                    nPDFType = vcl::PDFWriter::Section;
                }
            }
            break;

        /*
         * BLOCK-LEVEL STRUCTURE ELEMENTS
         */

        case FRM_TXT :
            {
                bool bPara = true;
                const SwTxtNode* pTxtNd =
                    static_cast<const SwTxtFrm*>(pFrm)->GetTxtNode();

                const SwNumRule* pNumRule = pTxtNd->GetNumRule();
                const SwNodeNum* pNodeNum = pTxtNd->GetNum();

                //
                // Heading: H1 - H6
                //
                if ( pNumRule && pNodeNum && pNumRule->IsOutlineRule() )
                {
                    BYTE nRealLevel = pNodeNum->GetRealLevel();
                    nRealLevel = nRealLevel > 5 ? 5 : nRealLevel;

                    nPDFType =  vcl::PDFWriter::H1 + nRealLevel;
                    bPara = false;
                }

                //
                // Numbering: L, LI, LBody
                //
                else if ( pNumRule && pNodeNum )
                {
                    // Check numbering of previous text node:
                    SwNodeIndex aIdx( *pTxtNd );
                    SwCntntNode* pPrevTxtNd = pTxtNd->GetNodes().GoPrevious( &aIdx );
                    const SwNodeNum* pPrevNodeNum = pPrevTxtNd &&
                                                    pPrevTxtNd->IsTxtNode() &&
                                                    (static_cast<SwTxtNode*>(pPrevTxtNd))->GetNumRule() == pNumRule ?
                                                    (static_cast<SwTxtNode*>(pPrevTxtNd))->GetNum() :
                                                    0;

                    // If pPrevNodeNum is set, we know that pTxtNd and pPrevTxtNd
                    // belong to the same NumRule:
                    const USHORT nMyLevel = pNodeNum->GetRealLevel() + 1;
                    USHORT nPreviousLevel = pPrevNodeNum ?
                                            pPrevNodeNum->GetRealLevel() + 1 :
                                            0;

                    if ( nMyLevel <= nPreviousLevel )
                    {
                        BeginTag( vcl::PDFWriter::ListItem );
                        BeginTag( vcl::PDFWriter::LIBody );
                    }
                    else
                    {
                        while ( nMyLevel > nPreviousLevel )
                        {
                            BeginTag( vcl::PDFWriter::List );
                            BeginTag( vcl::PDFWriter::ListItem );
                            BeginTag( vcl::PDFWriter::LIBody );
                            ++nPreviousLevel;
                        }
                    }
                }

                //
                // Section: TOCI
                //
                else if ( pFrm->IsInSct() )
                {
                    const SwSectionFrm* pSctFrm = pFrm->FindSctFrm();
                    const SwSection* pSection =
                            static_cast<const SwSectionFrm*>(pSctFrm)->GetSection();
                    if ( TOX_CONTENT_SECTION == pSection->GetType() )
                    {
                        const SwTOXBase* pTOXBase = pSection->GetTOXBase();
                        if ( pTOXBase && TOX_INDEX != pTOXBase->GetType() )
                        {
                            BeginTag( vcl::PDFWriter::TOCI );
                        }
                    }
                }

                //
                // Paragraph: P
                //
                if ( bPara )
                {
                    nPDFType = vcl::PDFWriter::Paragraph;
                }
            }
            break;

        case FRM_TAB :
            //
            // TabFrm: Table
            //
            nPDFType = vcl::PDFWriter::Table;
            break;

        /*
         * TABLE ELEMENTS
         */

        case FRM_ROW :
            //
            // RowFrm: TR
            //
            if ( !static_cast<const SwRowFrm*>(pFrm)->IsRepeatedHeadline() )
                nPDFType = vcl::PDFWriter::TableRow;
            else
                nPDFType = vcl::PDFWriter::NonStructElement;
            break;

        case FRM_CELL :
            //
            // CellFrm: TH, TD
            //
            {
                const SwTabFrm* pTable = static_cast<const SwCellFrm*>(pFrm)->FindTabFrm();
                if ( pTable->IsInHeadline( *pFrm ) )
                    nPDFType = vcl::PDFWriter::TableHeader;
                else
                    nPDFType = vcl::PDFWriter::TableData;
            }
            break;

        /*
         * ILLUSTRATION
         */

        case FRM_FLY :
            //
            // FlyFrm: Figure, Formula, Control
            // fly in content or fly at page
            {
                bool bFormula = false;
                const SwFlyFrm* pFly = static_cast<const SwFlyFrm*>(pFrm);
                if ( pFly->Lower() && pFly->Lower()->IsNoTxtFrm() )
                {
                    const SwNoTxtFrm* pNoTxtFrm = static_cast<const SwNoTxtFrm*>(pFly->Lower());
                    SwOLENode* pOLENd = const_cast<SwOLENode*>(pNoTxtFrm->GetNode()->GetOLENode());
                    if ( pOLENd )
                    {
                        SwOLEObj& aOLEObj = pOLENd->GetOLEObj();
                        ::com::sun::star::uno::Reference< ::com::sun::star::embed::XEmbeddedObject > aRef = aOLEObj.GetOleRef();
                        if ( aRef.is() )
                        {
                            bFormula = 0 != SotExchange::IsMath( SvGlobalName( aRef->getClassID() ) );
                        }
                    }
                    if ( bFormula )
                        nPDFType = vcl::PDFWriter::Formula;
                    else
                        nPDFType = vcl::PDFWriter::Figure;
                }
                else
                    nPDFType = vcl::PDFWriter::Division;
            }
            break;
    }

    if ( USHRT_MAX != nPDFType )
    {
        BeginTag( static_cast<vcl::PDFWriter::StructElement>(nPDFType) );
    }
}


/*
 * SwTaggedPDFHelper::EndStructureElements()
 */
void SwTaggedPDFHelper::EndStructureElements()
{
    const SwFrm* pFrm = mpFrmInfo ? &mpFrmInfo->mrFrm :
                        mpPorInfo ? mpPorInfo->mrTxtPainter.GetInfo().GetTxtFrm() : 0;

    while ( nEndStructureElement > 0 )
    {
        EndTag();
        --nEndStructureElement;
    }

    CheckRestoreTag();
}


/*
 * SwTaggedPDFHelper::BeginInlineStructureElements()
 */
void SwTaggedPDFHelper::BeginInlineStructureElements()
{
    const SwLinePortion* pPor = &mpPorInfo->mrPor;
    const SwTxtPaintInfo& rInf = mpPorInfo->mrTxtPainter.GetInfo();
    const SwTxtFrm* pFrm = rInf.GetTxtFrm();

    //
    // Lowers of NonStructureElements should not be considered:
    //
    if ( lcl_IsInNonStructEnv( *pFrm ) )
        return;

    USHORT nPDFType = USHRT_MAX;

    switch ( pPor->GetWhichPor() )
    {
        // Check for alternative spelling:
        case POR_HYPHSTR :
        case POR_SOFTHYPHSTR :
            nPDFType = vcl::PDFWriter::Span;
            break;

        case POR_LAY :
        case POR_TXT :
        case POR_PARA :
            {
                // Check for Link:
                SwTxtNode* pNd = (SwTxtNode*)pFrm->GetTxtNode();
                SwIndex aIndex( pNd, rInf.GetIdx() );
                const SwTxtAttr* pAttr = pNd->GetTxtAttr( aIndex, RES_TXTATR_INETFMT );
                if( pAttr )
                {
                    const SwFmtINetFmt& rFmt = pAttr->GetINetFmt();
                    nPDFType = vcl::PDFWriter::Link;
                }
                else
                {
                    const SwTxtAttr* pHint = mpPorInfo->mrTxtPainter.GetAttr( rInf.GetIdx() );
                    if ( UNDERLINE_NONE    != rInf.GetFont()->GetUnderline() ||
                         STRIKEOUT_NONE    != rInf.GetFont()->GetStrikeout() ||
                         EMPHASISMARK_NONE != rInf.GetFont()->GetEmphasisMark() ||
                         0 != rInf.GetFont()->GetEscapement() ||
                         SW_LATIN != rInf.GetFont()->GetActual() )
                        nPDFType = vcl::PDFWriter::Span;
                }
            }
            break;

        case POR_FTN :
            nPDFType = vcl::PDFWriter::Link;
            break;

        case POR_FLD :
            {
                // check field type:
                const xub_StrLen nIdx = static_cast<const SwFldPortion*>(pPor)->IsFollow() ?
                                        rInf.GetIdx() - 1 :
                                        rInf.GetIdx();
                const SwTxtAttr* pHint = mpPorInfo->mrTxtPainter.GetAttr( nIdx );
                const SwField* pFld = 0;
                if ( pHint && RES_TXTATR_FIELD == pHint->Which() )
                {
                    pFld = (SwField*)pHint->GetFld().GetFld();
                    if ( RES_GETREFFLD == pFld->Which() )
                        nPDFType = vcl::PDFWriter::Link;
                    else if ( RES_AUTHORITY == pFld->Which() )
                        nPDFType = vcl::PDFWriter::BibEntry;
                }
            }
            break;

        case POR_TAB :
        case POR_TABRIGHT :
        case POR_TABCENTER :
        case POR_TABDECIMAL :
            nPDFType = vcl::PDFWriter::NonStructElement;
            break;
    }

    if ( USHRT_MAX != nPDFType )
    {
        BeginTag( static_cast<vcl::PDFWriter::StructElement>(nPDFType) );
    }
}


/*
 * SwEnhancedPDFExportHelper::SwEnhancedPDFExportHelper()
 */
SwEnhancedPDFExportHelper::SwEnhancedPDFExportHelper( SwEditShell& rSh,
                                                      OutputDevice& rOut,
                                                      bool bEditEngineOnly )
    : mrSh( rSh ), mrOut( rOut )
{
    aLinkIdMap.clear();
    aFrmTagIdMap.clear();

#ifndef PRODUCT
    aStructStack.clear();
#endif

    EnhancedPDFExport( bEditEngineOnly );
}


/*
 * SwEnhancedPDFExportHelper::EnhancedPDFExport()
 */
void SwEnhancedPDFExportHelper::EnhancedPDFExport( bool bEditEngineOnly )
{
    vcl::PDFExtOutDevData* pPDFExtOutDevData =
        PTR_CAST( vcl::PDFExtOutDevData, mrOut.GetExtOutDevData() );

    if ( !pPDFExtOutDevData )
        return;

    //
    // Prepare the output device:
    //
    mrOut.Push( PUSH_MAPMODE );
    MapMode aMapMode( mrOut.GetMapMode() );
    aMapMode.SetMapUnit( MAP_TWIP );
    mrOut.SetMapMode( aMapMode );

    //
    // Create new cursor and lock the view:
    //
    SwDoc* pDoc = mrSh.GetDoc();
    mrSh.SwCrsrShell::Push();
    mrSh.SwCrsrShell::ClearMark();
    const BOOL bOldLockView = mrSh.IsViewLocked();
    mrSh.LockView( TRUE );

    if ( !bEditEngineOnly )
    {
        //
        // POSTITS
        //
        if ( pPDFExtOutDevData->GetIsExportNotes() )
        {
            SwFieldType* pType = mrSh.GetFldType( RES_POSTITFLD, aEmptyStr );
            SwClientIter aIter( *pType );
            const SwClient * pFirst = aIter.GoStart();
            while( pFirst )
            {
                if( ((SwFmtFld*)pFirst)->GetTxtFld() &&
                    ((SwFmtFld*)pFirst)->IsFldInDoc())
                {
                    const SwTxtNode* pTNd =
                        (SwTxtNode*)((SwFmtFld*)pFirst)->GetTxtFld()->GetpTxtNode();
                    ASSERT( 0 != pTNd, "Enhanced pdf export - text node is missing" )

                    // 1. Check if the whole paragraph is hidden
                    // 2. Move to the field
                    // 3. Check for hidden text attribute
                    if ( !pTNd->IsHidden() &&
                          mrSh.GotoFld( *(SwFmtFld*)pFirst ) &&
                         !mrSh.SelectHiddenRange() )
                    {

                        // Link Note
                        vcl::PDFNote aNote;

                        // Use the NumberFormatter to get the date string:
                        const SwPostItField* pField = (SwPostItField*)((SwFmtFld*)pFirst)->GetFld();
                        SvNumberFormatter* pNumFormatter = pDoc->GetNumberFormatter();
                        const Date aDateDiff( pField->GetDate() -
                                             *pNumFormatter->GetNullDate() );
                        const ULONG nFormat =
                            pNumFormatter->GetStandardFormat( NUMBERFORMAT_DATE, pField->GetLanguage() );
                        String sDate;
                        Color* pColor;
                        pNumFormatter->GetOutputString( aDateDiff.GetDate(), nFormat, sDate, &pColor );

                        // The title should consist of the author and the date:
                        String sTitle( pField->GetPar1() );
                        sTitle.AppendAscii( RTL_CONSTASCII_STRINGPARAM( ", " ) );
                        sTitle += sDate;
                        aNote.Title = sTitle;
                        // Guess what the contents contains...
                        aNote.Contents = pField->GetTxt();

                        // Link Rectangle
                        const SwRect& rNoteRect = mrSh.GetCharRect();

                        // Link PageNum
                        const sal_Int32 nPageNum =
                            mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rNoteRect );

                        // Link Export
                        pPDFExtOutDevData->CreateNote( rNoteRect.SVRect(), aNote, nPageNum );
                    }
                }
                pFirst = aIter++;
                mrSh.SwCrsrShell::ClearMark();
            }
        }

        //
        // HYPERLINKS
        //
        SwGetINetAttrs aArr;
        const sal_uInt16 nHyperLinkCount = mrSh.GetINetAttrs( aArr );
        for( sal_uInt16 n = 0; n < nHyperLinkCount; ++n )
        {
            SwGetINetAttr* p = aArr[ n ];
            ASSERT( 0 != p, "Enhanced pdf export - SwGetINetAttr is missing" )

            const SwTxtNode* pTNd = p->rINetAttr.GetpTxtNode();
            ASSERT( 0 != pTNd, "Enhanced pdf export - text node is missing" )

            // 1. Check if the whole paragraph is hidden
            // 2. Move to the hyperlink
            // 3. Check for hidden text attribute
            if ( !pTNd->IsHidden() &&
                  mrSh.GotoINetAttr( p->rINetAttr ) &&
                 !mrSh.SelectHiddenRange() )
            {
                // Select the hyperlink:
                mrSh.SwCrsrShell::Right( 1, CRSR_SKIP_CHARS );
                if ( mrSh.SwCrsrShell::SelectTxtAttr( RES_TXTATR_INETFMT, sal_True ) )
                {
                    // First, we create the destination, because there may be more
                    // than one link to this destination:
                    String aURL( INetURLObject::decode(
                        p->rINetAttr.GetINetFmt().GetValue(),
                        INET_HEX_ESCAPE,
                        INetURLObject::DECODE_UNAMBIGUOUS,
                        RTL_TEXTENCODING_UTF8 ) );

                    // We have to distinguish between intern and real URLs
                    const bool bIntern = '#' == aURL.GetChar( 0 );

                    // _GetCrsr() is a SwShellCrsr, which is derived from
                    // SwSelPaintRects, therefore the rectangles of the current
                    // selection can be easily obtained:
                    // Note: We make a copy of the rectangles, because they may
                    // be deleted again in JumpToSwMark.
                    SwRects aTmp;
                    aTmp.Insert( mrSh.SwCrsrShell::_GetCrsr(), 0 );
                    ASSERT( aTmp.Count() > 0, "Enhanced pdf export - rectangles are missing" )

                    // Create the destination for internal links:
                    sal_Int32 nDestId = 0;
                    if ( bIntern )
                    {
                        aURL.Erase( 0, 1 );
                        mrSh.SwCrsrShell::ClearMark();
                        JumpToSwMark( &mrSh, aURL );

                        // Destination Rectangle
                        const SwRect& rDestRect = mrSh.GetCharRect();

                        // Destination PageNum
                        const sal_Int32 nDestPageNum =
                            mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rDestRect );

                        // Destination Export
                        nDestId = pPDFExtOutDevData->CreateDest( rDestRect.SVRect(), nDestPageNum );
                    }

                    // Create links for all selected rectangles:
                    const USHORT nNumOfRects = aTmp.Count();
                    for ( int i = 0; i < nNumOfRects; ++i )
                    {
                        // Link Rectangle
                        const SwRect& rLinkRect( aTmp[ i ] );

                        // Link PageNum
                        const sal_Int32 nLinkPageNum =
                            mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rLinkRect );

                        // Link Export
                        const sal_Int32 nLinkId =
                            pPDFExtOutDevData->CreateLink( rLinkRect.SVRect(), nLinkPageNum );

                        // Store link info for tagged pdf output:
                        const IdMapEntry aLinkEntry( rLinkRect, nLinkId );
                        aLinkIdMap.push_back( aLinkEntry );

                        // Connect Link and Destination:
                        if ( bIntern )
                            pPDFExtOutDevData->SetLinkDest( nLinkId, nDestId );
                        else
                            pPDFExtOutDevData->SetLinkURL( nLinkId, aURL );
                    }
                }
            }
            mrSh.SwCrsrShell::ClearMark();
        }

        //
        // REFERENCES
        //
        SwFieldType* pType = mrSh.GetFldType( RES_GETREFFLD, aEmptyStr );
        SwClientIter aIter( *pType );
        const SwClient * pFirst = aIter.GoStart();
        while( pFirst )
        {
            if( ((SwFmtFld*)pFirst)->GetTxtFld() &&
                ((SwFmtFld*)pFirst)->IsFldInDoc())
            {
                const SwTxtNode* pTNd =
                    (SwTxtNode*)((SwFmtFld*)pFirst)->GetTxtFld()->GetpTxtNode();
               ASSERT( 0 != pTNd, "Enhanced pdf export - text node is missing" )

                // 1. Check if the whole paragraph is hidden
                // 2. Move to the field
                // 3. Check for hidden text attribute
                if ( !pTNd->IsHidden() &&
                      mrSh.GotoFld( *(SwFmtFld*)pFirst ) &&
                     !mrSh.SelectHiddenRange() )
                {
                    // Select the field:
                    mrSh.SwCrsrShell::SetMark();
                    mrSh.SwCrsrShell::Right( 1, CRSR_SKIP_CHARS );

                    // Link Rectangles
                    SwRects aTmp;
                    aTmp.Insert( mrSh.SwCrsrShell::_GetCrsr(), 0 );
                    ASSERT( aTmp.Count() > 0, "Enhanced pdf export - rectangles are missing" )

                    mrSh.SwCrsrShell::ClearMark();

                    // Destination Rectangle
                    const SwGetRefField* pField =
                        (SwGetRefField*)((SwFmtFld*)pFirst)->GetFld();
                    const String& rRefName = pField->GetSetRefName();
                    mrSh.GotoRefMark( rRefName, pField->GetSubType(), pField->GetSeqNo() );
                    const SwRect& rDestRect = mrSh.GetCharRect();

                    // Destination PageNum
                    const sal_Int32 nDestPageNum =
                        mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rDestRect );

                    // Destination Export
                    const sal_Int32 nDestId = pPDFExtOutDevData->CreateDest( rDestRect.SVRect(), nDestPageNum );

                    // Create links for all selected rectangles:
                    const USHORT nNumOfRects = aTmp.Count();
                    for ( int i = 0; i < nNumOfRects; ++i )
                    {
                        // Link rectangle
                        const SwRect& rLinkRect( aTmp[ i ] );

                        // Link PageNum
                        const sal_Int32 nLinkPageNum =
                            mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rLinkRect );

                        // Link Export
                        const sal_Int32 nLinkId =
                            pPDFExtOutDevData->CreateLink( rLinkRect.SVRect(), nLinkPageNum );

                        // Store link info for tagged pdf output:
                        const IdMapEntry aLinkEntry( rLinkRect, nLinkId );
                        aLinkIdMap.push_back( aLinkEntry );

                        // Connect Link and Destination:
                        pPDFExtOutDevData->SetLinkDest( nLinkId, nDestId );
                    }
                }
            }
            pFirst = aIter++;
            mrSh.SwCrsrShell::ClearMark();
        }

        //
        // FOOTNOTES
        //
        const USHORT nFtnCount = pDoc->GetFtnIdxs().Count();
        for ( USHORT nIdx = 0; nIdx < nFtnCount; ++nIdx )
        {
            // Set cursor to text node that contains the footnote:
            const SwTxtFtn* pTxtFtn = pDoc->GetFtnIdxs()[ nIdx ];
            SwTxtNode& rTNd = const_cast<SwTxtNode&>(pTxtFtn->GetTxtNode());

            mrSh._GetCrsr()->GetPoint()->nNode = rTNd;
            mrSh._GetCrsr()->GetPoint()->nContent.Assign( &rTNd, *pTxtFtn->GetStart() );

            // 1. Check if the whole paragraph is hidden
            // 2. Check for hidden text attribute
            if ( static_cast<const SwTxtNode&>(rTNd).IsHidden() ||
                 mrSh.SelectHiddenRange() )
                continue;

            SwCrsrSaveState aSaveState( *mrSh._GetCrsr() );

            // Select the footnote:
            mrSh.SwCrsrShell::SetMark();
            mrSh.SwCrsrShell::Right( 1, CRSR_SKIP_CHARS );

            // Link Rectangle
            SwRects aTmp;
            aTmp.Insert( mrSh.SwCrsrShell::_GetCrsr(), 0 );
            ASSERT( aTmp.Count() > 0, "Enhanced pdf export - rectangles are missing" )
            const SwRect aLinkRect( aTmp[ 0 ] );

            mrSh._GetCrsr()->RestoreSavePos();
            mrSh.SwCrsrShell::ClearMark();

            // Goto footnote text:
            if ( mrSh.GotoFtnTxt() )
            {
                // Link PageNum
                const sal_Int32 nLinkPageNum =
                    mrSh.GetPageNumAndSetOffsetForPDF( mrOut, aLinkRect );

                // Link Export
                const sal_Int32 nLinkId =
                    pPDFExtOutDevData->CreateLink( aLinkRect.SVRect(), nLinkPageNum );

                // Store link info for tagged pdf output:
                const IdMapEntry aLinkEntry( aLinkRect, nLinkId );
                aLinkIdMap.push_back( aLinkEntry );

                // Destination Rectangle
                const SwRect& rDestRect = mrSh.GetCharRect();

                // Destination PageNum
                const sal_Int32 nDestPageNum =
                     mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rDestRect );

                // Destination Export
                const sal_Int32 nDestId = pPDFExtOutDevData->CreateDest( rDestRect.SVRect(), nDestPageNum );

                // Connect Link and Destination:
                pPDFExtOutDevData->SetLinkDest( nLinkId, nDestId );
            }
        }

        //
        // OUTLINE
        //
        typedef std::pair< sal_Int8, sal_Int32 > StackEntry;
        std::stack< StackEntry > aOutlineStack;
        aOutlineStack.push( StackEntry( -1, -1 ) ); // push default value

        const sal_uInt16 nOutlineCount = mrSh.GetOutlineCnt();
        for ( sal_uInt16 i = 0; i < nOutlineCount; ++i )
        {
            // Check if outline is hidden
            const SwTxtNode* pTNd = mrSh.GetNodes().GetOutLineNds()[ i ]->GetTxtNode();
            ASSERT( 0 != pTNd, "Enhanced pdf export - text node is missing" )

            if ( pTNd->IsHidden() ||
                 // --> FME 2005-01-10 #i40292# Skip empty outlines:
                 0 == pTNd->GetTxt().Len() )
                 // <--
                continue;

            // Get parent id from stack:
            const sal_Int8 nLevel = (sal_Int8)mrSh.GetOutlineLevel( i );
            sal_Int8 nLevelOnTopOfStack = aOutlineStack.top().first;
            while ( nLevelOnTopOfStack >= nLevel &&
                    nLevelOnTopOfStack != -1 )
            {
                aOutlineStack.pop();
                nLevelOnTopOfStack = aOutlineStack.top().first;
            }
            const sal_Int32 nParent = aOutlineStack.top().second;

            // Destination rectangle
            mrSh.GotoOutline(i);
            const SwRect& rDestRect = mrSh.GetCharRect();

            // Destination PageNum
            const sal_Int32 nDestPageNum =
                mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rDestRect );

            // Destination Export
            const sal_Int32 nDestId =
                pPDFExtOutDevData->CreateDest( rDestRect.SVRect(), nDestPageNum );

            // Outline entry text
            const String& rEntry = mrSh.GetOutlineText( i );

            // Create a new outline item:
            const sal_Int32 nOutlineId =
                pPDFExtOutDevData->CreateOutlineItem( nParent, rEntry, nDestId );

            // Push current level and nOutlineId on stack:
            aOutlineStack.push( StackEntry( nLevel, nOutlineId ) );
        }
    }
    else
    {
        //
        // LINKS FROM EDITENGINE
        //
        std::vector< vcl::PDFExtOutDevBookmarkEntry >& rBookmarks = pPDFExtOutDevData->GetBookmarks();
        std::vector< vcl::PDFExtOutDevBookmarkEntry >::iterator aIBeg = rBookmarks.begin();
        std::vector< vcl::PDFExtOutDevBookmarkEntry >::iterator aIEnd = rBookmarks.end();
        while ( aIBeg != aIEnd )
        {
            String aBookmarkName( aIBeg->aBookmark );
            const bool bIntern = '#' == aBookmarkName.GetChar( 0 );
            if ( bIntern )
            {
                aBookmarkName.Erase( 0, 1 );
                JumpToSwMark( &mrSh, aBookmarkName );

                // Destination Rectangle
                const SwRect& rDestRect = mrSh.GetCharRect();

                // Destination PageNum
                const sal_Int32 nDestPageNum =
                    mrSh.GetPageNumAndSetOffsetForPDF( mrOut, rDestRect );

                // Destination Export
                const sal_Int32 nDestId =
                    pPDFExtOutDevData->CreateDest( rDestRect.SVRect(), nDestPageNum );

                // Connect Link and Destination:
                pPDFExtOutDevData->SetLinkDest( aIBeg->nLinkId, nDestId );
            }
            else
                pPDFExtOutDevData->SetLinkURL( aIBeg->nLinkId, aBookmarkName );

            aIBeg++;
        }
        rBookmarks.clear();
    }

    // Restore view, cursor, and outdev:
    mrSh.LockView( bOldLockView );
    mrSh.SwCrsrShell::Pop( FALSE );
    mrOut.Pop();
}
