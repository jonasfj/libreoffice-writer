/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: shellio.cxx,v $
 *
 *  $Revision: 1.40 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:35:40 $
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
#define ITEMID_BOXINFO      SID_ATTR_BORDER_INNER
#include <hintids.hxx>

#ifndef _DATE_HXX
#include <tools/date.hxx>
#endif
#ifndef _TIME_HXX
#include <tools/time.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef SVTOOLS_FSTATHELPER_HXX
#include <svtools/fstathelper.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_BOXITEM_HXX //autogen
#include <svx/boxitem.hxx>
#endif
#ifndef _SVX_PAPERINF_HXX //autogen
#include <svx/paperinf.hxx>
#endif

#ifndef _NODE_HXX //autogen
#include <node.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>			// fuer Undo Insert-Dokument
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>			// fuer Undo Insert-Dokument
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _TBLSEL_HXX
#include <tblsel.hxx>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _FLTINI_HXX
#include <fltini.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _SWSWERROR_H
#include <swerror.h>
#endif

#include <paratr.hxx>

using namespace ::com::sun::star;

/*
 * The writer can currently not handle multiple paragraph styles with outline numbering. Thus this method
 * tries to find "the" paragraph style used for outline numbering. In order to find "the" we try to eliminate
 * all paragraph styles with outline numbers which are not used. If one survives we found "the" paragraph.
 * If not - you guess - we have a problem and must wait for the solution of #i31372#!
 */
void adjustOutlineLevel(SwDoc &rDoc)
{
    const SwTxtFmtColls& rTbl = *rDoc.GetTxtFmtColls();
    int fmtsPerOutlineLevel[MAXLEVEL] = {};
    for( sal_uInt16 i = 0, nCnt = rTbl.Count(); i < nCnt; ++i )
    {
        SwTxtFmtColl *pTxtFmtColl = rTbl[i];
        if (pTxtFmtColl->GetOutlineLevel()!=NO_NUMBERING)
        {
            if (rDoc.IsUsed(*pTxtFmtColl))
            {
                ASSERT(pTxtFmtColl->GetOutlineLevel()<MAXLEVEL, "outline level too high!!!");
                fmtsPerOutlineLevel[pTxtFmtColl->GetOutlineLevel()]++;
                const SwNumRuleItem* pRule;
                if (SFX_ITEM_SET == pTxtFmtColl->GetAttrSet().GetItemState(RES_PARATR_NUMRULE, TRUE, reinterpret_cast<const SfxPoolItem **>(&pRule)))
                {
                    if (SwNumRule* pNumRule = rDoc.FindNumRulePtr(pRule->GetValue()))
                    {
                        rDoc.SetOutlineNumRule(*pNumRule);
                    }
                }
            }
            else
            {
                pTxtFmtColl->SetOutlineLevel(NO_NUMBERING);
            }
        }
    }
    for(int i=0;i<MAXLEVEL;i++) {
        if (fmtsPerOutlineLevel[i]>0) {
            SwTxtFmtColl* heading=rDoc.GetTxtCollFromPool(RES_POOLCOLL_HEADLINE1+i);
            if (!rDoc.IsUsed(*heading)) {
                fmtsPerOutlineLevel[i]++; // just for the statistics
            }
            else
            {
                heading->SetOutlineLevel(NO_NUMBERING);
            }
        }
        if (fmtsPerOutlineLevel[i]>1) {
            ASSERT(TRUE, "more than one paragraph style uses outline numbering.");
        }
    };
}

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

    pDoc->bInReading = TRUE;

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
        if( !pDoc->IsHTMLMode() || ReadHTML != po || !po->pTemplate  )
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
            pDoc->StartUndo( UNDO_INSDOKUMENT );
        }
    }
    pDoc->DoUndo( FALSE );

    SwNodeIndex aSplitIdx( pDoc->GetNodes() );

    SwRedlineMode eOld = pDoc->GetRedlineMode();
    pDoc->SetRedlineMode_intern( REDLINE_IGNORE );

    // Array von FlyFormaten
    SwSpzFrmFmts aFlyFrmArr;
    // only read templates? then ignore multi selection!
    BOOL bFmtsOnly = po->aOpt.IsFmtsOnly();

    while( TRUE )
    {
        if( bSaveUndo )
            pUndo = new SwUndoInsDoc( *pPam );

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

        nError = po->Read( *pDoc, GetBaseURL(), *pPam, aFileName );

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
                    if( FLY_PAGE == rAnchor.GetAnchorId() ||
                        ( FLY_AT_CNTNT == rAnchor.GetAnchorId() &&
                            rAnchor.GetCntntAnchor() &&
                            ( pUndoPam->GetPoint()->nNode ==
                            rAnchor.GetCntntAnchor()->nNode ||
                            pUndoPam->GetMark()->nNode ==
                            rAnchor.GetCntntAnchor()->nNode ) ) )
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
                                pDoc->AppendUndo( new SwUndoInsLayFmt( pFrmFmt ) );
                                pDoc->SetRedlineMode_intern( REDLINE_IGNORE );
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
                pDoc->AppendRedline( new SwRedline( REDLINE_INSERT, *pUndoPam ));
            else
                pDoc->SplitRedline( *pUndoPam );
            pDoc->SetRedlineMode_intern( REDLINE_IGNORE );
        }
        if( bSaveUndo )
        {
            pDoc->SetRedlineMode_intern( eOld );
            pUndo->SetInsertRange( *pUndoPam, FALSE );
            pDoc->AppendUndo( pUndo );
            pDoc->SetRedlineMode_intern( REDLINE_IGNORE );
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

    pDoc->bInReading = FALSE;

    pDoc->InvalidateNumRules();
    pDoc->UpdateNumRule();
    pDoc->SetAllUniqueFlyNames();

    if( bReadPageDescs )
        pDoc->DoUndo( TRUE );
    else
    {
        pDoc->DoUndo( bDocUndo );
        if( bSaveUndo )
        {
            pDoc->SetRedlineMode_intern( eOld );
            pDoc->EndUndo( UNDO_INSDOKUMENT );
            pDoc->SetRedlineMode_intern( REDLINE_IGNORE );
        }
    }

    // Wenn der Pam nur fuers Lesen konstruiert wurde, jetzt zerstoeren.
    if( !pCrsr )
    {
        delete pPam;		  // ein neues aufgemacht.

        // --> FME 2005-02-25 #i42634# Moved common code of SwReader::Read() and
        // SwDocShell::UpdateLinks() to new SwDoc::UpdateLinks():
        pDoc->UpdateLinks();
        // <--

        eOld = (SwRedlineMode)(pDoc->GetRedlineMode() & ~REDLINE_IGNORE);

        pDoc->SetFieldsDirty( FALSE );
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

    if( po == ReadSw3 ) 		// am Sw3-Reader noch den pIo-Pointer "loeschen"
        ((Sw3Reader*)po)->SetSw3Io( 0 );

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
SwReader::SwReader(SfxMedium& rMedium, const String& rFileName, SwDoc *pDoc)
    : SwDocFac(pDoc), pStrm(0), pMedium(&rMedium), pCrsr(0),
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

SwReader::SwReader( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage > &rStg, const String& rFilename, SwPaM &rPam )
    : SwDocFac(rPam.GetDoc()), pStrm(0), pMedium(0), pCrsr(&rPam), xStg( rStg ), aFileName(rFilename)
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
                        pTemplate->SetBrowseMode( bTmplBrowseMode );
                        pTemplate->RemoveAllFmtLanguageDependencies();

                        ReadXML->SetOrganizerMode( TRUE );
                        SfxMedium aMedium( aFileName, FALSE );
                        SwReader aRdr( aMedium, aEmptyStr, pTemplate );
                        aRdr.Read( *ReadXML );
                        ReadXML->SetOrganizerMode( FALSE );

                        pTemplate->AddLink();
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
        rDoc.SetFixFields();
        bRet = TRUE;
    }

    return bRet;
}

void Reader::ClearTemplate()
{
    if( pTemplate )
    {
        if( 0 == pTemplate->RemoveLink() )
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
    pTemplate->AddLink();
    pTemplate->SetBrowseMode( bTmplBrowseMode );
    pTemplate->GetPrt( TRUE );
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


void Reader::SetNoOutlineNum( SwDoc& rDoc )
{
    // JP 10.03.96: jetzt wieder keine Nummerierung in den Vorlagen
}


void Reader::ResetFrmFmtAttrs( SfxItemSet &rFrmSet )
{
    rFrmSet.Put( SvxLRSpaceItem() );
    rFrmSet.Put( SvxULSpaceItem() );
    rFrmSet.Put( SvxBoxItem() );
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

        pFrmFmt->ResetAttr( RES_LR_SPACE );
        pFrmFmt->ResetAttr( RES_UL_SPACE );
        pFrmFmt->ResetAttr( RES_BOX );
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

SwWriter::SwWriter(SvStream& rStrm,SwDoc &rDoc)
    : pStrm(&rStrm), pMedium(0), pOutPam(0), pShell(0), rDoc(rDoc),
    bWriteAll(true)
{
}

SwWriter::SwWriter(SvStream& rStrm, SwPaM& rPam, BOOL bInWriteAll)
    : pStrm(&rStrm), pMedium(0), pOutPam(&rPam), pShell(0),
    rDoc(*rPam.GetDoc()), bWriteAll(bInWriteAll)
{
}

SwWriter::SwWriter(SvStorage& rStg,SwDoc &rDoc)
    : pStrm(0), pStg(&rStg), pMedium(0), pOutPam(0), pShell(0), rDoc(rDoc),
    bWriteAll(true)
{
}

SwWriter::SwWriter( const uno::Reference < embed::XStorage >& rStg,SwDoc &rDoc)
    : pStrm(0), pMedium(0), pOutPam(0), pShell(0), rDoc(rDoc), xStg( rStg ), bWriteAll(true)
{
}

SwWriter::SwWriter(SfxMedium& rMedium, SwCrsrShell &rShell, BOOL bInWriteAll)
    : pStrm(0), pMedium(&rMedium), pOutPam(0), pShell(&rShell),
    rDoc(*rShell.GetDoc()), bWriteAll(bInWriteAll)
{
}

SwWriter::SwWriter(SfxMedium& rMedium, SwDoc &rDoc)
    : pStrm(0), pMedium(&rMedium), pOutPam(0), pShell(0), rDoc(rDoc),
    bWriteAll(true)
{
}

ULONG SwWriter::Write( WriterRef& rxWriter, const String* pRealFileName )
{
    BOOL bHasMark = FALSE;
    SwPaM * pPam;

    SwDoc *pDoc = 0;
    SfxObjectShellRef* pRefForDocSh = 0;

    if ( pShell && !bWriteAll && pShell->IsTableMode() )
    {
        bWriteAll = TRUE;
        pDoc = new SwDoc;
        pDoc->AddLink();
        pRefForDocSh = new SfxObjectShellRef();
        pDoc->SetRefForDocShell( pRefForDocSh );

        // kopiere Teile aus einer Tabelle: lege eine Tabelle mit der Breite
        // von der Originalen an und kopiere die selectierten Boxen.
        // Die Groessen werden prozentual korrigiert.

        // lasse ueber das Layout die Boxen suchen
        SwSelBoxes aBoxes;
        GetTblSel( *pShell, aBoxes );
        SwTableNode* pTblNd = (SwTableNode*)aBoxes[0]->GetSttNd()->FindStartNode();
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
                pShell->SttDoc();
                pShell->SetMark();
                pShell->EndDoc();
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
        pPam->Move( fnMoveBackward, fnGoDoc );
        pPam->SetMark();
        pPam->Move( fnMoveForward, fnGoDoc );
    }

    rxWriter->bWriteAll = bWriteAll;
    SwDoc* pOutDoc = pDoc ? pDoc : &rDoc;

    // falls der Standart PageDesc. immer noch auf initalen Werten steht
    // (wenn z.B. kein Drucker gesetzt wurde) dann setze jetzt auf DIN A4
    // --> OD 2004-11-17 #i37248# - Modifications are only allowed at a new document.
    // <pOutDoc> contains a new document, if <pDoc> is set - see above.
    if ( pDoc && !pOutDoc->GetPrt() )
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
            aNewSz.SetHeight( lA4Height );
            aNewSz.SetWidth( lA4Width );
            aNew.GetMaster().SetAttr( aNewSz );
            pOutDoc->ChgPageDesc( 0, aNew );
        }
    }

    SwEditShell* pESh = pOutDoc->GetEditShell();
    if( pESh )
        pESh->StartAllAction();

    BOOL bWasPurgeOle = pOutDoc->IsPurgeOLE();
    pOutDoc->SetPurgeOLE( FALSE );

    ULONG nError = 0;
    if( pMedium )
        nError = rxWriter->Write( *pPam, *pMedium, pRealFileName );
    else if( pStg )
        nError = rxWriter->Write( *pPam, *pStg, pRealFileName );
    else if( pStrm )
        nError = rxWriter->Write( *pPam, *pStrm, pRealFileName );
    else if( xStg.is() )
        nError = rxWriter->Write( *pPam, xStg, pRealFileName );

    pOutDoc->SetPurgeOLE( bWasPurgeOle );
    if( pESh )
        pESh->EndAllAction();

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
        if ( !pDoc->RemoveLink() )
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
            ( SwFmtPageDesc(rDoc.GetPageDescFromPoolSimple(RES_POOLPAGE_HTML,
                                                           FALSE) ) );
        pCNd->ChgFmtColl( rDoc.GetTxtCollFromPoolSimple( RES_POOLCOLL_TEXT,
                                                         FALSE ));
    }

    return bRet;
}


