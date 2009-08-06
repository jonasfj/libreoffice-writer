/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: docglos.cxx,v $
 * $Revision: 1.11 $
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
#include <shellio.hxx>
#include <pam.hxx>
#include <swundo.hxx>
#include <ndtxt.hxx>
#include <acorrect.hxx>
#include <crsrsh.hxx>
#include <docsh.hxx>

#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/document/XDocumentProperties.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>


using namespace ::com::sun::star;


/// copy document properties via public interface
static void lcl_copyDocumentProperties(
        uno::Reference<document::XDocumentProperties> i_xSource,
        uno::Reference<document::XDocumentProperties> i_xTarget) {
    DBG_ASSERT(i_xSource.is(), "null reference");
    DBG_ASSERT(i_xTarget.is(), "null reference");

    i_xTarget->setAuthor(i_xSource->getAuthor());
    i_xTarget->setGenerator(i_xSource->getGenerator());
    i_xTarget->setCreationDate(i_xSource->getCreationDate());
    i_xTarget->setTitle(i_xSource->getTitle());
    i_xTarget->setSubject(i_xSource->getSubject());
    i_xTarget->setDescription(i_xSource->getDescription());
    i_xTarget->setKeywords(i_xSource->getKeywords());
    i_xTarget->setLanguage(i_xSource->getLanguage());
    i_xTarget->setModifiedBy(i_xSource->getModifiedBy());
    i_xTarget->setModificationDate(i_xSource->getModificationDate());
    i_xTarget->setPrintedBy(i_xSource->getPrintedBy());
    i_xTarget->setPrintDate(i_xSource->getPrintDate());
    i_xTarget->setTemplateName(i_xSource->getTemplateName());
    i_xTarget->setTemplateURL(i_xSource->getTemplateURL());
    i_xTarget->setTemplateDate(i_xSource->getTemplateDate());
    i_xTarget->setAutoloadURL(i_xSource->getAutoloadURL());
    i_xTarget->setAutoloadSecs(i_xSource->getAutoloadSecs());
    i_xTarget->setDefaultTarget(i_xSource->getDefaultTarget());
    i_xTarget->setDocumentStatistics(i_xSource->getDocumentStatistics());
    i_xTarget->setEditingCycles(i_xSource->getEditingCycles());
    i_xTarget->setEditingDuration(i_xSource->getEditingDuration());

    uno::Reference<beans::XPropertySet> xSourceUDSet(
        i_xSource->getUserDefinedProperties(), uno::UNO_QUERY_THROW);
    uno::Reference<beans::XPropertyContainer> xTargetUD(
        i_xTarget->getUserDefinedProperties());
    uno::Reference<beans::XPropertySet> xTargetUDSet(xTargetUD,
        uno::UNO_QUERY_THROW);
    uno::Sequence<beans::Property> tgtprops
        = xTargetUDSet->getPropertySetInfo()->getProperties();
    for (sal_Int32 i = 0; i < tgtprops.getLength(); ++i) {
        try {
            xTargetUD->removeProperty(tgtprops [i].Name);
        } catch (uno::Exception &) {
            // ignore
        }
    }
    try {
        uno::Reference<beans::XPropertySetInfo> xSetInfo
            = xSourceUDSet->getPropertySetInfo();
        uno::Sequence<beans::Property> srcprops = xSetInfo->getProperties();
        for (sal_Int32 i = 0; i < srcprops.getLength(); ++i) {
            ::rtl::OUString name = srcprops[i].Name;
            xTargetUD->addProperty(name, srcprops[i].Attributes,
                xSourceUDSet->getPropertyValue(name));
        }
    } catch (uno::Exception &) {
        // ignore
    }
}

/* -----------------22.07.99 11:47-------------------
    Description: inserts an AutoText block
 --------------------------------------------------*/
BOOL SwDoc::InsertGlossary( SwTextBlocks& rBlock, const String& rEntry,
                            SwPaM& rPaM, SwCrsrShell* pShell )
{
    BOOL bRet = FALSE;
    USHORT nIdx = rBlock.GetIndex( rEntry );
    if( (USHORT) -1 != nIdx )
    {
        // Bug #70238# ask the TextOnly-Flag before BeginGetDoc, because
        //				the method closed the Storage!
        BOOL bSav_IsInsGlossary = mbInsOnlyTxtGlssry;
        mbInsOnlyTxtGlssry = rBlock.IsOnlyTextBlock( nIdx );

        if( rBlock.BeginGetDoc( nIdx ) )
        {
            SwDoc* pGDoc = rBlock.GetDoc();

            // alle FixFelder aktualisieren. Dann aber auch mit der
            // richtigen DocInfo!
            // FIXME: UGLY: Because we cannot limit the range in which to do
            // field updates, we must update the fixed fields at the glossary
            // entry document.
            // To be able to do this, we copy the document properties of the
            // target document to the glossary document
//            DBG_ASSERT(GetDocShell(), "no SwDocShell"); // may be clipboard!
            DBG_ASSERT(pGDoc->GetDocShell(), "no SwDocShell at glossary");
            if (GetDocShell() && pGDoc->GetDocShell()) {
                uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
                    GetDocShell()->GetModel(), uno::UNO_QUERY_THROW);
                uno::Reference<document::XDocumentProperties> xDocProps(
                    xDPS->getDocumentProperties() );
                uno::Reference<document::XDocumentPropertiesSupplier> xGlosDPS(
                    pGDoc->GetDocShell()->GetModel(), uno::UNO_QUERY_THROW);
                uno::Reference<document::XDocumentProperties> xGlosDocProps(
                    xGlosDPS->getDocumentProperties() );
                lcl_copyDocumentProperties(xDocProps, xGlosDocProps);
        }
            pGDoc->SetFixFields(false, NULL);

            //StartAllAction();
            LockExpFlds();

            SwNodeIndex aStt( pGDoc->GetNodes().GetEndOfExtras(), 1 );
            SwCntntNode* pCntntNd = pGDoc->GetNodes().GoNext( &aStt );
            const SwTableNode* pTblNd = pCntntNd->FindTableNode();
            SwPaM aCpyPam( pTblNd ? *(SwNode*)pTblNd : *(SwNode*)pCntntNd );
            aCpyPam.SetMark();

            // dann bis zum Ende vom Nodes Array
            aCpyPam.GetPoint()->nNode = pGDoc->GetNodes().GetEndOfContent().GetIndex()-1;
            pCntntNd = aCpyPam.GetCntntNode();
            aCpyPam.GetPoint()->nContent.Assign( pCntntNd, pCntntNd->Len() );

            StartUndo( UNDO_INSGLOSSARY, NULL );
            SwPaM *_pStartCrsr = &rPaM, *__pStartCrsr = _pStartCrsr;
            do {

                SwPosition& rInsPos = *_pStartCrsr->GetPoint();
                SwStartNode* pBoxSttNd = (SwStartNode*)rInsPos.nNode.GetNode().
                                            FindTableBoxStartNode();

                if( pBoxSttNd && 2 == pBoxSttNd->EndOfSectionIndex() -
                                      pBoxSttNd->GetIndex() &&
                    aCpyPam.GetPoint()->nNode != aCpyPam.GetMark()->nNode )
                {
                    // es wird mehr als 1 Node in die akt. Box kopiert.
                    // Dann muessen die BoxAttribute aber entfernt werden.
                    ClearBoxNumAttrs( rInsPos.nNode );
                }

                SwDontExpandItem aACD;
                aACD.SaveDontExpandItems( rInsPos );

                pGDoc->Copy( aCpyPam, rInsPos, false );

                aACD.RestoreDontExpandItems( rInsPos );
                if( pShell )
                    pShell->SaveTblBoxCntnt( &rInsPos );
            } while( (_pStartCrsr=(SwPaM *)_pStartCrsr->GetNext()) !=
                        __pStartCrsr );
            EndUndo( UNDO_INSGLOSSARY, NULL );

            UnlockExpFlds();
            if( !IsExpFldsLocked() )
                UpdateExpFlds(NULL, true);
            bRet = TRUE;
        }
        mbInsOnlyTxtGlssry = bSav_IsInsGlossary;
    }
    rBlock.EndGetDoc();
    return bRet;
}


