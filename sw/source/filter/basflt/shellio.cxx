/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: shellio.cxx,v $
 * $Revision: 1.58 $
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
#include <hintids.hxx>
#include <tools/date.hxx>
#include <tools/time.hxx>
#include <svtools/urihelper.hxx>
#ifndef SVTOOLS_FSTATHELPER_HXX
#include <svtools/fstathelper.hxx>
#endif
#include <svtools/moduleoptions.hxx>
#include <sfx2/docfile.hxx>
#include <svx/lrspitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/paperinf.hxx>
#include <node.hxx>
#include <docary.hxx>
#include <fmtanchr.hxx>
#include <fmtfsize.hxx>
#include <fmtpdsc.hxx>
#include <swtypes.hxx>
#include <shellio.hxx>
#include <doc.hxx>
#include <pam.hxx>
#include <editsh.hxx>
#include <undobj.hxx>			// fuer Undo Insert-Dokument
#include <swundo.hxx>			// fuer Undo Insert-Dokument
#include <swtable.hxx>
#include <tblsel.hxx>
#include <pagedesc.hxx>
#include <poolfmt.hxx>
#include <fltini.hxx>
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#include <redline.hxx>
#include <swerror.h>

#include <paratr.hxx>

// --> OD 2007-03-30 #i73788#
#include <pausethreadstarting.hxx>
// <--

using namespace ::com::sun::star;

//////////////////////////////////////////////////////////////////////////

ULONG SwReader::Read( const Reader& rOptions )
{
    // Variable uebertragen
    Reader* po = (Reader*) &rOptions;
    po->pStrm = pStrm;
    po->pStg  = pStg;
    po->xStg  = xStg;
    po->bInsertMode = 0 != pCrsr;

    // ist ein Medium angegeben, dann aus diesem die Streams besorgen
    if( 0 != (po->pMedium = pMedium ) &&
        !po->SetStrmStgPtr() )
    {
        po->SetReadUTF8( FALSE );
        po->SetBlockMode( FALSE );
        po->SetOrganizerMode( FALSE );
        po->SetIgnoreHTMLComments( FALSE );
        return ERR_SWG_FILE_FORMAT_ERROR;
    }

    ULONG nError = 0L;

    GetDoc();

    // am Sw3-Reader noch den pIo-Pointer "loeschen"
    /*
    if( po == ReadSw3 && pDoc->GetDocShell() &&
        ((Sw3Reader*)po)->GetSw3Io() != pDoc->GetDocShell()->GetIoSystem() )
            ((Sw3Reader*)po)->SetSw3Io( pDoc->GetDocShell()->GetIoSystem() );*/

    // waehrend des einlesens kein OLE-Modified rufen
    Link aOLELink( pDoc->GetOle2Link() );
    pDoc->SetOle2Link( Link() );

    pDoc->SetInReading( true );
    pDoc->SetInXMLImport( 0 != dynamic_cast< XMLReader* >(po) );

    SwPaM *pPam;
    if( pCrsr )
        pPam = pCrsr;
    else
    {
        // Wenn der Reader nicht mit einem Shell konstruiert wurde,
        // selber einen Pam machen.
        SwNodeIndex nNode( pDoc->GetNodes().GetEndOfContent(), -1 );
        pPam = new SwPaM( nNode );
        // Bei Web-Dokumenten wird die Default-Vorlage schon im InitNew
        // gesetzt und braucht deshalb nicht nochmal gesetzt zu werden.
        // Das gilt natuerlich nicht, wenn der Filter nicht der HTML-Filter
        // ist oder im ConvertFrom zuvor ein SetTemplateName gerufen
        // wurde.
        if( !pDoc->get(IDocumentSettingAccess::HTML_MODE) || ReadHTML != po || !po->pTemplate  )
            po->SetTemplate( *pDoc );
    }

    // Pams sind ringfoermig verkettet. Aufhoeren, wenn man wieder beim
    // ersten ist.
    SwPaM *pEnd = pPam;
    SwUndoInsDoc* pUndo = 0;

    BOOL bReadPageDescs = FALSE;
    BOOL bDocUndo = pDoc->DoesUndo();
    BOOL bSaveUndo = bDocUndo && pCrsr;
    if( bSaveUndo )
    {
        // das Einlesen von Seitenvorlagen ist nicht Undofaehig!
        if( 0 != ( bReadPageDescs = po->aOpt.IsPageDescs() ) )
        {
            bSaveUndo = FALSE;
            pDoc->DelAllUndoObj();
        }
        else
        {
            pDoc->ClearRedo();
            pDoc->StartUndo( UNDO_INSDOKUMENT, NULL );
        }
    }
    pDoc->DoUndo( FALSE );

    SwNodeIndex aSplitIdx( pDoc->GetNodes() );

    RedlineMode_t eOld = pDoc->GetRedlineMode();

    // Array von FlyFormaten
    SwSpzFrmFmts aFlyFrmArr;
    // only read templates? then ignore multi selection!
    BOOL bFmtsOnly = po->aOpt.IsFmtsOnly();

    while( TRUE )
    {
        if( bSaveUndo )
            pUndo = new SwUndoInsDoc( *pPam );

        pDoc->SetRedlineMode_intern( nsRedlineMode_t::REDLINE_IGNORE );

        SwPaM* pUndoPam = 0;
        if( bDocUndo || pCrsr )
        {
            // Pam auf den Node davor setzen damit er nicht mit verschoben wird
            const SwNodeIndex& rTmp = pPam->GetPoint()->nNode;
            pUndoPam = new SwPaM( rTmp, rTmp, 0, -1 );
        }

        // Speicher mal alle Fly's
        if( pCrsr )
            aFlyFrmArr.Insert( pDoc->GetSpzFrmFmts(), 0L );

        xub_StrLen nSttCntnt = pPam->GetPoint()->nContent.GetIndex();

        // damit fuer alle Reader die Ende-Position immer stimmt, hier
        // pflegen.
        SwCntntNode* pCNd = pPam->GetCntntNode();
        xub_StrLen nEndCntnt = pCNd ? pCNd->Len() - nSttCntnt : 0;
        SwNodeIndex aEndPos( pPam->GetPoint()->nNode, 1 );

        pDoc->SetRedlineMode_intern( eOld );

        nError = po->Read( *pDoc, GetBaseURL(), *pPam, aFileName );

        pDoc->SetRedlineMode_intern( nsRedlineMode_t::REDLINE_IGNORE );

        if( !IsError( nError )) 	// dann setzen wir das Ende mal richtig
        {
            aEndPos--;
            pCNd = aEndPos.GetNode().GetCntntNode();
            if( !pCNd && 0 == ( pCNd = pDoc->GetNodes().GoPrevious( &aEndPos ) ))
                pCNd = pDoc->GetNodes().GoNext( &aEndPos );

            pPam->GetPoint()->nNode = aEndPos;
            xub_StrLen nLen = pCNd->Len();
            if( nLen < nEndCntnt )
                nEndCntnt = 0;
            else
                nEndCntnt = nLen - nEndCntnt;
            pPam->GetPoint()->nContent.Assign( pCNd, nEndCntnt );
        }

        if( pCrsr )
        {
            *pUndoPam->GetMark() = *pPam->GetPoint();
            pUndoPam->GetPoint()->nNode++;
            SwNode* pNd = pUndoPam->GetNode();
            if( pNd->IsCntntNode() )
                pUndoPam->GetPoint()->nContent.Assign(
                                    (SwCntntNode*)pNd, nSttCntnt );
            else
                pUndoPam->GetPoint()->nContent.Assign( 0, 0 );

            int bChkHeaderFooter = pNd->FindHeaderStartNode() ||
                                   pNd->FindFooterStartNode();

            // Suche alle neuen Fly's und speicher sie als einzelne Undo
            // Objecte
            for( USHORT n = 0; n < pDoc->GetSpzFrmFmts()->Count(); ++n )
            {
                SwFrmFmt* pFrmFmt = (*pDoc->GetSpzFrmFmts())[ n ];
                const SwFmtAnchor& rAnchor = pFrmFmt->GetAnchor();
                if( USHRT_MAX == aFlyFrmArr.GetPos( pFrmFmt) )
                {
                    SwPosition const*const pFrameAnchor(
                            rAnchor.GetCntntAnchor());
                    if  (   (FLY_PAGE == rAnchor.GetAnchorId())
                        ||  (   pFrameAnchor
                            &&  (   (   (FLY_AT_CNTNT == rAnchor.GetAnchorId())
                                    &&  (   (pUndoPam->GetPoint()->nNode ==
                                             pFrameAnchor->nNode)
                                        ||  (pUndoPam->GetMark()->nNode ==
                                             pFrameAnchor->nNode)
                                        )
                                    )
                                // #i97570# also check frames anchored AT char
                                ||  (   (FLY_AUTO_CNTNT == rAnchor.GetAnchorId())
                                    &&  !IsDestroyFrameAnchoredAtChar(
                                              *pFrameAnchor,
                                              *pUndoPam->GetPoint(),
                                              *pUndoPam->GetMark())
                                    )
                                )
                            )
                        )
                    {
                        if( bChkHeaderFooter &&
                            FLY_AT_CNTNT == rAnchor.GetAnchorId() &&
                            RES_DRAWFRMFMT == pFrmFmt->Which() )
                        {
                            // DrawObjecte in Kopf-/Fusszeilen ist nicht
                            // erlaubt!
                            pFrmFmt->DelFrms();
                            pDoc->DelFrmFmt( pFrmFmt );
                            --n;
                        }
                        else
                        {
                            if( bSaveUndo )
                            {
                                pDoc->SetRedlineMode_intern( eOld );
                                pDoc->AppendUndo( new SwUndoInsLayFmt( pFrmFmt,0,0 ) );
                                pDoc->SetRedlineMode_intern( nsRedlineMode_t::REDLINE_IGNORE );
                            }
                            if( pFrmFmt->GetDepends() )
                            {
                                // beim Insert legen Draw-Objecte einen Frame an
                                // also weg damit.
                                pFrmFmt->DelFrms();
                            }

                            if( FLY_PAGE == rAnchor.GetAnchorId() )
                            {
                                if( !rAnchor.GetCntntAnchor() )
                                    pFrmFmt->MakeFrms();
                                else if( pCrsr )
                                    // seitengebundene Flys eingefuegt, dann schalte
                                    // die Optimierungs-Flags vom SwDoc ab. Sonst
                                    // werden die Flys nicht an der Position erzeugt.
                                    pDoc->SetLoaded( FALSE );
                            }
                            else
                                pFrmFmt->MakeFrms();
                        }
                    }
                }
            }
            if( aFlyFrmArr.Count() )
                aFlyFrmArr.Remove( 0, aFlyFrmArr.Count() );

            pDoc->SetRedlineMode_intern( eOld );
            if( pDoc->IsRedlineOn() )
                pDoc->AppendRedline( new SwRedline( nsRedlineType_t::REDLINE_INSERT, *pUndoPam ), true);
            else
                pDoc->SplitRedline( *pUndoPam );
            pDoc->SetRedlineMode_intern( nsRedlineMode_t::REDLINE_IGNORE );
        }
        if( bSaveUndo )
        {
            pDoc->SetRedlineMode_intern( eOld );
            pUndo->SetInsertRange( *pUndoPam, FALSE );
            pDoc->AppendUndo( pUndo );
            pDoc->SetRedlineMode_intern( nsRedlineMode_t::REDLINE_IGNORE );
        }

        delete pUndoPam;

        pPam = (SwPaM *) pPam->GetNext();
        if( pPam == pEnd )
            break;

        // only read templates? then ignore multi selection! Bug 68593
        if( bFmtsOnly )
            break;

        /*
         * !!! man muss selbst den Status vom Stream zuruecksetzen. !!!
         *	   Beim seekg wird der akt. Status, eof- und bad-Bit
         *	   gesetzt, warum weiss keiner
         */
        if( pStrm )
        {
            pStrm->Seek(0);
            pStrm->ResetError();
        }
    }

    pDoc->SetInReading( false );
    pDoc->SetInXMLImport( false );

    pDoc->InvalidateNumRules();
    pDoc->UpdateNumRule();
    pDoc->ChkCondColls();
    pDoc->SetAllUniqueFlyNames();

    if( bReadPageDescs )
        pDoc->DoUndo( TRUE );
    else
    {
        pDoc->DoUndo( bDocUndo );
        if( bSaveUndo )
        {
            pDoc->SetRedlineMode_intern( eOld );
            pDoc->EndUndo( UNDO_INSDOKUMENT, NULL );
            pDoc->SetRedlineMode_intern( nsRedlineMode_t::REDLINE_IGNORE );
        }
    }

    // Wenn der Pam nur fuers Lesen konstruiert wurde, jetzt zerstoeren.
    if( !pCrsr )
    {
        delete pPam;		  // ein neues aufgemacht.

        // --> FME 2005-02-25 #i42634# Moved common code of SwReader::Read() and
        // SwDocShell::UpdateLinks() to new SwDoc::UpdateLinks():
    // ATM still with Update
        pDoc->UpdateLinks( TRUE );
        // <--

    eOld = (RedlineMode_t)(pDoc->GetRedlineMode() & ~nsRedlineMode_t::REDLINE_IGNORE);

        pDoc->SetFieldsDirty(false, NULL, 0);
    }

    pDoc->SetRedlineMode_intern( eOld );
    pDoc->SetOle2Link( aOLELink );

    if( pCrsr )					// das Doc ist jetzt modifiziert
        pDoc->SetModified();
    // --> OD 2005-02-11 #i38810# - If links have been updated, the document
    // have to be modified. During update of links the OLE link at the document
    // isn't set. Thus, the document's modified state has to be set again after
    // the OLE link is restored - see above <pDoc->SetOle2Link( aOLELink )>.
    if ( pDoc->LinksUpdated() )
    {
        pDoc->SetModified();
    }
    // <--

//  if( po == ReadSw3 )         // am Sw3-Reader noch den pIo-Pointer "loeschen"
//      ((Sw3Reader*)po)->SetSw3Io( 0 );

    po->SetReadUTF8( FALSE );
    po->SetBlockMode( FALSE );
    po->SetOrganizerMode( FALSE );
    po->SetIgnoreHTMLComments( FALSE );

    return nError;
}


/*
 * Konstruktoren, Destruktor
 */

// Initiales Einlesben

                                       /*
SwReader::SwReader(SvStorage& rStg, const String& rFileName, SwDoc *pDoc)
    : SwDocFac(pDoc), pStrm(0), pStg(&rStg), pMedium(0), pCrsr(0),
    aFileName(rFileName)
{
}

SwReader::SwReader(const uno::Reference < embed::XStorage >& rStg, const String& rFileName, SwDoc *pDoc)
    : SwDocFac(pDoc), pStrm(0), pMedium(0), pCrsr(0), xStg( rStg ), aFileName(rFileName)
{
}
                                         */
SwReader::SwReader(SfxMedium& rMedium, const String& rFileName, SwDoc *pDocument)
    : SwDocFac(pDocument), pStrm(0), pMedium(&rMedium), pCrsr(0),
    aFileName(rFileName)
{
    SetBaseURL( rMedium.GetBaseURL() );
}

// In ein existierendes Dokument einlesen

SwReader::SwReader(SvStream& rStrm, const String& rFileName, const String& rBaseURL, SwPaM& rPam)
    : SwDocFac(rPam.GetDoc()), pStrm(&rStrm), pMedium(0), pCrsr(&rPam),
    aFileName(rFileName)
{
    SetBaseURL( rBaseURL );
}
/*
SwReader::SwReader(SvStorage& rStg, const String& rFileName, SwPaM& rPam)
    : SwDocFac(rPam.GetDoc()), pStrm(0), pStg(&rStg), pMedium(0), pCrsr(&rPam),
    aFileName(rFileName)
{
}
*/
SwReader::SwReader(SfxMedium& rMedium, const String& rFileName, SwPaM& rPam)
    : SwDocFac(rPam.GetDoc()), pStrm(0), pMedium(&rMedium),
    pCrsr(&rPam), aFileName(rFileName)
{
    SetBaseURL( rMedium.GetBaseURL() );
}

SwReader::SwReader( const uno::Reference < embed::XStorage > &rStg, const String& rFilename, SwPaM &rPam )
    : SwDocFac(rPam.GetDoc()), pStrm(0), xStg( rStg ), pMedium(0), pCrsr(&rPam), aFileName(rFilename)
{
}

Reader::Reader()
    : pTemplate(0), pStrm(0), pMedium(0), bInsertMode(0),
    bTmplBrowseMode(0), bReadUTF8(0), bBlockMode(0), bOrganizerMode(0),
    bHasAskTemplateName(0), bIgnoreHTMLComments(0)
{
}

Reader::~Reader()
{
    delete pTemplate;
}

String Reader::GetTemplateName() const
{
    return aEmptyStr;
}

// Die Filter-Vorlage laden, setzen und wieder freigeben
SwDoc* Reader::GetTemplateDoc()
{
    if( !bHasAskTemplateName )
    {
        SetTemplateName( GetTemplateName() );
        bHasAskTemplateName = TRUE;
    }

    if( !aTemplateNm.Len() )
        ClearTemplate();
    else
    {
        INetURLObject aTDir( aTemplateNm );
        String aFileName = aTDir.GetMainURL( INetURLObject::NO_DECODE );
        DBG_ASSERT( !aTDir.HasError(), "No absolute path for template name!" );
        DateTime aCurrDateTime;
        BOOL bLoad = FALSE;

        // Wenn das Template schon mal geladen wurde, nur einmal pro
        // Minute nachschauen, ob es geaendert wurde.
        if( !pTemplate || aCurrDateTime >= aChkDateTime )
        {
            Date aTstDate;
            Time aTstTime;
            if( FStatHelper::GetModifiedDateTimeOfFile(
                            aTDir.GetMainURL( INetURLObject::NO_DECODE ),
                            &aTstDate, &aTstTime ) &&
                ( !pTemplate || aDStamp != aTstDate || aTStamp != aTstTime ))
            {
                bLoad = TRUE;
                aDStamp = aTstDate;
                aTStamp = aTstTime;
            }

            // Erst in einer Minute wieder mal nachschauen, ob sich die
            // Vorlage geaendert hat.
            aChkDateTime = aCurrDateTime;
            aChkDateTime += Time( 0L, 1L );
        }

        if( bLoad )
        {
            ClearTemplate();
            ASSERT( !pTemplate, "Who holds the template doc?" );

                // #95605#: If the writer module is not installed,
                // we cannot create a SwDocShell. We could create a
                // SwWebDocShell however, because this exists always
                // for the help.
                SvtModuleOptions aModuleOptions;
                if( aModuleOptions.IsWriter() )
                {
                    SwDocShell *pDocSh =
                        new SwDocShell ( SFX_CREATE_MODE_INTERNAL );
                    SfxObjectShellRef xDocSh = pDocSh;
                    if( pDocSh->DoInitNew( 0 ) )
                    {
                        pTemplate = pDocSh->GetDoc();
                        pTemplate->SetOle2Link( Link() );
                        pTemplate->DoUndo( FALSE );		// always FALSE
                        pTemplate->set(IDocumentSettingAccess::BROWSE_MODE, bTmplBrowseMode );
                        pTemplate->RemoveAllFmtLanguageDependencies();

                        ReadXML->SetOrganizerMode( TRUE );
                        SfxMedium aMedium( aFileName, FALSE );
                        SwReader aRdr( aMedium, aEmptyStr, pTemplate );
                        aRdr.Read( *ReadXML );
                        ReadXML->SetOrganizerMode( FALSE );

                        pTemplate->acquire();
                    }
                }
        }

        ASSERT( !pTemplate || FStatHelper::IsDocument( aFileName ) ||
                aTemplateNm.EqualsAscii( "$$Dummy$$" ),
                "TemplatePtr but no template exist!" );
    }

    return pTemplate;
}

BOOL Reader::SetTemplate( SwDoc& rDoc )
{
    BOOL bRet = FALSE;

    GetTemplateDoc();
    if( pTemplate )
    {
        rDoc.RemoveAllFmtLanguageDependencies();
        rDoc.ReplaceStyles( *pTemplate );
        rDoc.SetFixFields(false, NULL);
        bRet = TRUE;
    }

    return bRet;
}

void Reader::ClearTemplate()
{
    if( pTemplate )
    {
        if( 0 == pTemplate->release() )
            delete pTemplate,
        pTemplate = 0;
    }
}

void Reader::SetTemplateName( const String& rDir )
{
    if( rDir.Len() && aTemplateNm != rDir )
    {
        ClearTemplate();
        aTemplateNm = rDir;
    }
}

void Reader::MakeHTMLDummyTemplateDoc()
{
    ClearTemplate();
    pTemplate = new SwDoc;
    pTemplate->acquire();
    pTemplate->set(IDocumentSettingAccess::BROWSE_MODE, bTmplBrowseMode );
    pTemplate->getPrinter( true );
    pTemplate->RemoveAllFmtLanguageDependencies();
    aChkDateTime = Date( 1, 1, 2300 );	// 2300. Jahrtausend sollte reichen
    aTemplateNm.AssignAscii( "$$Dummy$$" );
}

// alle die die Streams / Storages nicht geoeffnet brauchen,
// muessen die Methode ueberladen
int Reader::SetStrmStgPtr()
{
    ASSERT( pMedium, "Wo ist das Medium??" );

    if( pMedium->IsStorage() )
    {
        if( SW_STORAGE_READER & GetReaderType() )
        {
            xStg = pMedium->GetStorage();
            return TRUE;
        }
    }
    else
    {
        pStrm = pMedium->GetInStream();
        if ( pStrm && SotStorage::IsStorageFile(pStrm) && (SW_STORAGE_READER & GetReaderType()) )
        {
            pStg = new SotStorage( *pStrm );
            pStrm = NULL;
        }
        else if ( !(SW_STREAM_READER & GetReaderType()) )
        {
            pStrm = NULL;
            return FALSE;
        }

        return TRUE;
    }
    return FALSE;
}


int Reader::GetReaderType()
{
    return SW_STREAM_READER;
}


void Reader::SetFltName( const String& )
{
}


void Reader::SetNoOutlineNum( SwDoc& /*rDoc*/ )
{
    // JP 10.03.96: jetzt wieder keine Nummerierung in den Vorlagen
}


void Reader::ResetFrmFmtAttrs( SfxItemSet &rFrmSet )
{
    rFrmSet.Put( SvxLRSpaceItem(RES_LR_SPACE) );
    rFrmSet.Put( SvxULSpaceItem(RES_UL_SPACE) );
    rFrmSet.Put( SvxBoxItem(RES_BOX) );
}


void Reader::ResetFrmFmts( SwDoc& rDoc )
{
    for (USHORT i=0; i<3; ++i)
    {
        USHORT nPoolId;
        switch (i)
        {
            default:
                ASSERT(i == 0, "Impossible");
                //fallthrough
            case 0:
                nPoolId = RES_POOLFRM_FRAME;
                break;
            case 1:
                nPoolId = RES_POOLFRM_GRAPHIC;
                break;
            case 2:
                nPoolId = RES_POOLFRM_OLE;
                break;
        }

        SwFrmFmt *pFrmFmt = rDoc.GetFrmFmtFromPool( nPoolId );

        pFrmFmt->ResetFmtAttr( RES_LR_SPACE );
        pFrmFmt->ResetFmtAttr( RES_UL_SPACE );
        pFrmFmt->ResetFmtAttr( RES_BOX );
    }
}

    // read the sections of the document, which is equal to the medium.
    // returns the count of it
USHORT Reader::GetSectionList( SfxMedium&, SvStrings& ) const
{
    return 0;
}

// ------------------------------------------------
BOOL SwReader::HasGlossaries( const Reader& rOptions )
{
    // Variable uebertragen
    Reader* po = (Reader*) &rOptions;
    po->pStrm = pStrm;
    po->pStg  = pStg;
    po->bInsertMode = FALSE;

    // ist ein Medium angegeben, dann aus diesem die Streams besorgen
    BOOL bRet = FALSE;
    if( !( 0 != (po->pMedium = pMedium ) && !po->SetStrmStgPtr() ))
        bRet = po->HasGlossaries();
    return bRet;
}

BOOL SwReader::ReadGlossaries( const Reader& rOptions,
                                SwTextBlocks& rBlocks, BOOL bSaveRelFiles )
{
    // Variable uebertragen
    Reader* po = (Reader*) &rOptions;
    po->pStrm = pStrm;
    po->pStg  = pStg;
    po->bInsertMode = FALSE;

    // ist ein Medium angegeben, dann aus diesem die Streams besorgen
    BOOL bRet = FALSE;
    if( !( 0 != (po->pMedium = pMedium ) && !po->SetStrmStgPtr() ))
        bRet = po->ReadGlossaries( rBlocks, bSaveRelFiles );
    return bRet;
}

BOOL Reader::HasGlossaries() const
{
    return FALSE;
}

BOOL Reader::ReadGlossaries( SwTextBlocks&, BOOL ) const
{
    return FALSE;
}

// ------------------------------------------------

int StgReader::GetReaderType()
{
    return SW_STORAGE_READER;
}




/*
 * Writer
 */

/*
 * Konstruktoren, Destruktoren sind inline (inc/shellio.hxx).
 */

SwWriter::SwWriter(SvStream& rStrm, SwCrsrShell &rShell, BOOL bInWriteAll)
    : pStrm(&rStrm), pMedium(0), pOutPam(0), pShell(&rShell),
    rDoc(*rShell.GetDoc()), bWriteAll(bInWriteAll)
{
}

SwWriter::SwWriter(SvStream& rStrm,SwDoc &rDocument)
    : pStrm(&rStrm), pMedium(0), pOutPam(0), pShell(0), rDoc(rDocument),
    bWriteAll(true)
{
}

SwWriter::SwWriter(SvStream& rStrm, SwPaM& rPam, BOOL bInWriteAll)
    : pStrm(&rStrm), pMedium(0), pOutPam(&rPam), pShell(0),
    rDoc(*rPam.GetDoc()), bWriteAll(bInWriteAll)
{
}

SwWriter::SwWriter( const uno::Reference < embed::XStorage >& rStg, SwDoc &rDocument)
    : pStrm(0), xStg( rStg ), pMedium(0), pOutPam(0), pShell(0), rDoc(rDocument), bWriteAll(true)
{
}

SwWriter::SwWriter(SfxMedium& rMedium, SwCrsrShell &rShell, BOOL bInWriteAll)
    : pStrm(0), pMedium(&rMedium), pOutPam(0), pShell(&rShell),
    rDoc(*rShell.GetDoc()), bWriteAll(bInWriteAll)
{
}

SwWriter::SwWriter(SfxMedium& rMedium, SwDoc &rDocument)
    : pStrm(0), pMedium(&rMedium), pOutPam(0), pShell(0), rDoc(rDocument),
    bWriteAll(true)
{
}

ULONG SwWriter::Write( WriterRef& rxWriter, const String* pRealFileName )
{
    // --> OD 2007-03-30 #i73788#
    SwPauseThreadStarting aPauseThreadStarting;
    // <--

    BOOL bHasMark = FALSE;
    SwPaM * pPam;

    SwDoc *pDoc = 0;
    SfxObjectShellRef* pRefForDocSh = 0;

    if ( pShell && !bWriteAll && pShell->IsTableMode() )
    {
        bWriteAll = TRUE;
        pDoc = new SwDoc;
        pDoc->acquire();
        pRefForDocSh = new SfxObjectShellRef();
        pDoc->SetRefForDocShell( pRefForDocSh );

        // kopiere Teile aus einer Tabelle: lege eine Tabelle mit der Breite
        // von der Originalen an und kopiere die selectierten Boxen.
        // Die Groessen werden prozentual korrigiert.

        // lasse ueber das Layout die Boxen suchen
        SwSelBoxes aBoxes;
        GetTblSel( *pShell, aBoxes );
        SwTableNode* pTblNd = (SwTableNode*)aBoxes[0]->GetSttNd()->StartOfSectionNode();
        SwNodeIndex aIdx( pDoc->GetNodes().GetEndOfExtras(), 2 );
        SwCntntNode *pNd = aIdx.GetNode().GetCntntNode();
        ASSERT( pNd, "Node not found" );
        SwPosition aPos( aIdx, SwIndex( pNd ) );
        pTblNd->GetTable().MakeCopy( pDoc, aPos, aBoxes );
    }

    if( !bWriteAll && ( pShell || pOutPam ))
    {
        if( pShell )
            pPam = pShell->GetCrsr();
        else
            pPam = pOutPam;

        SwPaM *pEnd = pPam;

        // Erste Runde: Nachsehen, ob eine Selektion besteht.
        while(TRUE)
        {
            bHasMark = bHasMark || pPam->HasMark();
            pPam = (SwPaM *) pPam->GetNext();
            if(bHasMark || pPam == pEnd)
                break;
        }

        // Wenn keine Selektion besteht, eine ueber das ganze Dokument aufspannen.
        if(!bHasMark)
        {
            if( pShell )
            {
                pShell->Push();
                pShell->SttEndDoc(TRUE);
                pShell->SetMark();
                pShell->SttEndDoc(FALSE);
            }
            else
            {
                pPam = new SwPaM( *pPam );
                pPam->Move( fnMoveBackward, fnGoDoc );
                pPam->SetMark();
                pPam->Move( fnMoveForward, fnGoDoc );
            }
        }
        // pPam ist immer noch der akt. Cursor !!
    }
    else
    {
        // keine Shell oder alles schreiben -> eigenen Pam erzeugen
        SwDoc* pOutDoc = pDoc ? pDoc : &rDoc;
        pPam = new SwPaM( pOutDoc->GetNodes().GetEndOfContent() );
        if( pOutDoc->IsClipBoard() )
        {
            pPam->Move( fnMoveBackward, fnGoDoc );
            pPam->SetMark();
            pPam->Move( fnMoveForward, fnGoDoc );
        }
        else
        {
            pPam->SetMark();
            pPam->Move( fnMoveBackward, fnGoDoc );
        }
    }

    rxWriter->bWriteAll = bWriteAll;
    SwDoc* pOutDoc = pDoc ? pDoc : &rDoc;

    // falls der Standart PageDesc. immer noch auf initalen Werten steht
    // (wenn z.B. kein Drucker gesetzt wurde) dann setze jetzt auf DIN A4
    // --> OD 2004-11-17 #i37248# - Modifications are only allowed at a new document.
    // <pOutDoc> contains a new document, if <pDoc> is set - see above.
    if ( pDoc && !pOutDoc->getPrinter( false ) )
    // <--
    {
        const SwPageDesc& rPgDsc = const_cast<const SwDoc *>(pOutDoc)->GetPageDesc( 0 );
        //const SwPageDesc& rPgDsc = *pOutDoc->GetPageDescFromPool( RES_POOLPAGE_STANDARD );;
        const SwFmtFrmSize& rSz = rPgDsc.GetMaster().GetFrmSize();
        // Clipboard-Dokument wird immer ohne Drucker angelegt, so ist
        // der Std.PageDesc immer aug LONG_MAX !! Mappe dann auf DIN A4
        if( LONG_MAX == rSz.GetHeight() || LONG_MAX == rSz.GetWidth() )
        {
            SwPageDesc aNew( rPgDsc );
            SwFmtFrmSize aNewSz( rSz );
            Size a4(SvxPaperInfo::GetPaperSize( PAPER_A4 ));
            aNewSz.SetHeight( a4.Width() );
            aNewSz.SetWidth( a4.Height() );
            aNew.GetMaster().SetFmtAttr( aNewSz );
            pOutDoc->ChgPageDesc( 0, aNew );
        }
    }

    BOOL bLockedView(FALSE);
    SwEditShell* pESh = pOutDoc->GetEditShell();
    if( pESh )
    {
        bLockedView = pESh->IsViewLocked();
        pESh->LockView( TRUE );    //lock visible section
        pESh->StartAllAction();
    }

    BOOL bWasPurgeOle = pOutDoc->get(IDocumentSettingAccess::PURGE_OLE);
    pOutDoc->set(IDocumentSettingAccess::PURGE_OLE, false);

    ULONG nError = 0;
    if( pMedium )
        nError = rxWriter->Write( *pPam, *pMedium, pRealFileName );
    else if( pStg )
        nError = rxWriter->Write( *pPam, *pStg, pRealFileName );
    else if( pStrm )
        nError = rxWriter->Write( *pPam, *pStrm, pRealFileName );
    else if( xStg.is() )
        nError = rxWriter->Write( *pPam, xStg, pRealFileName );

    pOutDoc->set(IDocumentSettingAccess::PURGE_OLE, bWasPurgeOle );

    if( pESh )
    {
        pESh->EndAllAction();
        pESh->LockView( bLockedView );
    }

    // Falls nur zum Schreiben eine Selektion aufgespannt wurde, vor der
    // Rueckkehr den alten Crsr wieder herstellen.
    if( !bWriteAll && ( pShell || pOutPam ))
    {
        if(!bHasMark)
        {
            if( pShell )
                pShell->Pop( FALSE );
            else
                delete pPam;
        }
    }
    else
    {
        delete pPam;			// loesche den hier erzeugten Pam
        // Alles erfolgreich geschrieben? Sag' das dem Dokument!
        if ( !IsError( nError ) && !pDoc )
        {
            rDoc.ResetModified();
            // --> OD 2005-02-11 #i38810# - reset also flag, that indicates
            // updated links
            rDoc.SetLinksUpdated( sal_False );
            // <-
        }
    }

    if ( pDoc )
    {
        delete pRefForDocSh;
        if ( !pDoc->release() )
            delete pDoc;
        bWriteAll = FALSE;
    }

    return nError;
}


/*  */

// ----------------------------------------------------------------------


BOOL SetHTMLTemplate( SwDoc & rDoc )
{
    // Vorlagennamen von den Sfx-HTML-Filter besorgen!!!
    if( !ReadHTML->GetTemplateDoc() )
        ReadHTML->MakeHTMLDummyTemplateDoc();

    BOOL bRet = ReadHTML->SetTemplate( rDoc );

    SwNodes& rNds = rDoc.GetNodes();
    SwNodeIndex aIdx( rNds.GetEndOfExtras(), 1 );
    SwCntntNode* pCNd = rNds.GoNext( &aIdx );
    if( pCNd )
    {
        pCNd->SetAttr
            ( SwFmtPageDesc(rDoc.GetPageDescFromPool(RES_POOLPAGE_HTML, false) ) );
        pCNd->ChgFmtColl( rDoc.GetTxtCollFromPool( RES_POOLCOLL_TEXT, false ));
    }

    return bRet;
}


