/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: edglss.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:27:58 $
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


#pragma hdrstop

#ifndef _OSL_ENDIAN_H_
#include <osl/endian.h>
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _CACHESTR_HXX //autogen
#include <tools/cachestr.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _EDIMP_HXX
#include <edimp.hxx>
#endif
#ifndef _FRMFMT_HXX //autogen
#include <frmfmt.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>		// fuer die UndoIds
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _TBLSEL_HXX
#include <tblsel.hxx> 		// fuers kopieren von Tabellen
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx> 		// fuers kopieren von Tabellen
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx> 		// SwTextBlocks
#endif
#ifndef _ACORRECT_HXX
#include <acorrect.hxx>
#endif
#ifndef _SWSWERROR_H
#include <swerror.h> 		// SwTextBlocks
#endif


/******************************************************************************
 *				jetzt mit einem verkappten Reader/Writer/Dokument
 ******************************************************************************/



void SwEditShell::InsertGlossary( SwTextBlocks& rGlossary, const String& rStr )
{
    StartAllAction();
    GetDoc()->InsertGlossary( rGlossary, rStr, *GetCrsr(), this );
    EndAllAction();
}


/******************************************************************************
 *				aktuelle Selektion zum Textbaustein machen und ins
 *			Textbausteindokument einfuegen, einschliesslich Vorlagen
 ******************************************************************************/


USHORT SwEditShell::MakeGlossary( SwTextBlocks& rBlks, const String& rName, const String& rShortName,
                                    BOOL bSaveRelFile, BOOL bSaveRelNet,
                                    const String* pOnlyTxt )
{
    SwDoc* pGDoc = rBlks.GetDoc();

    String sBase;
    if(bSaveRelFile)
    {
        INetURLObject aURL( rBlks.GetFileName() );
        sBase = aURL.GetMainURL( INetURLObject::NO_DECODE );
    }
    rBlks.SetBaseURL( sBase );

    USHORT nRet;

    if( pOnlyTxt )
        nRet = rBlks.PutText( rShortName, rName, *pOnlyTxt );
    else
    {
        rBlks.ClearDoc();
        if( rBlks.BeginPutDoc( rShortName, rName ) )
        {
            rBlks.GetDoc()->SetRedlineMode_intern( REDLINE_DELETE_REDLINES );
            _CopySelToDoc( pGDoc );
            rBlks.GetDoc()->SetRedlineMode_intern( 0 );
            nRet = rBlks.PutDoc();
        }
        else
            nRet = (USHORT) -1;
    }

    return nRet;
}

USHORT SwEditShell::SaveGlossaryDoc( SwTextBlocks& rBlock,
                                    const String& rName,
                                    const String& rShortName,
                                    BOOL bSaveRelFile, BOOL bSaveRelNet,
                                    BOOL bOnlyTxt )
{
    StartAllAction();

    SwDoc* pGDoc = rBlock.GetDoc();
    SwDoc* pDoc = GetDoc();

    String sBase;
    if(bSaveRelFile)
    {
        INetURLObject aURL( rBlock.GetFileName() );
        sBase = aURL.GetMainURL( INetURLObject::NO_DECODE );
    }
    rBlock.SetBaseURL( sBase );
    USHORT nRet = USHRT_MAX;

    if( bOnlyTxt )
    {
        KillPams();

        SwPaM* pCrsr = GetCrsr();

        SwNodeIndex aStt( pDoc->GetNodes().GetEndOfExtras(), 1 );
        SwCntntNode* pCntntNd = pDoc->GetNodes().GoNext( &aStt );
        const SwNode* pNd = pCntntNd->FindTableNode();
        if( !pNd )
            pNd = pCntntNd;

        pCrsr->GetPoint()->nNode = *pNd;
        if( pNd == pCntntNd )
            pCrsr->GetPoint()->nContent.Assign( pCntntNd, 0 );
        pCrsr->SetMark();

        // dann bis zum Ende vom Nodes Array
        pCrsr->GetPoint()->nNode = pDoc->GetNodes().GetEndOfContent().GetIndex()-1;
        pCntntNd = pCrsr->GetCntntNode();
        if( pCntntNd )
            pCrsr->GetPoint()->nContent.Assign( pCntntNd, pCntntNd->Len() );

        String sBuf;
        if( GetSelectedText( sBuf, GETSELTXT_PARABRK_TO_ONLYCR ) && sBuf.Len() )
            nRet = rBlock.PutText( rShortName, rName, sBuf );
    }
    else
    {
        rBlock.ClearDoc();
        if( rBlock.BeginPutDoc( rShortName, rName ) )
        {
            SwNodeIndex aStt( pDoc->GetNodes().GetEndOfExtras(), 1 );
            SwCntntNode* pCntntNd = pDoc->GetNodes().GoNext( &aStt );
            const SwNode* pNd = pCntntNd->FindTableNode();
            if( !pNd ) pNd = pCntntNd;
            SwPaM aCpyPam( *pNd );
            aCpyPam.SetMark();

            // dann bis zum Ende vom Nodes Array
            aCpyPam.GetPoint()->nNode = pDoc->GetNodes().GetEndOfContent().GetIndex()-1;
            pCntntNd = aCpyPam.GetCntntNode();
            aCpyPam.GetPoint()->nContent.Assign( pCntntNd, pCntntNd->Len() );

            aStt = pGDoc->GetNodes().GetEndOfExtras();
            pCntntNd = pGDoc->GetNodes().GoNext( &aStt );
            SwPosition aInsPos( aStt, SwIndex( pCntntNd ));
            pDoc->Copy( aCpyPam, aInsPos );

            nRet = rBlock.PutDoc();
        }
    }
    EndAllAction();
    return nRet;
}

/******************************************************************************
 *					kopiere alle Selectionen und das Doc
 ******************************************************************************/


BOOL SwEditShell::_CopySelToDoc( SwDoc* pInsDoc, SwNodeIndex* pSttNd )
{
    ASSERT( pInsDoc, "kein Ins.Dokument"  );

    SwNodes& rNds = pInsDoc->GetNodes();

    SwNodeIndex aIdx( rNds.GetEndOfContent(), -1 );
    SwCntntNode * pNd = aIdx.GetNode().GetCntntNode();
    SwPosition aPos( aIdx, SwIndex( pNd, pNd->Len() ));

    // soll der Index auf Anfang returnt werden ?
    if( pSttNd )
    {
        *pSttNd = aPos.nNode;
        (*pSttNd)--;
    }

    BOOL bRet = FALSE;
    SET_CURR_SHELL( this );

    pInsDoc->LockExpFlds();

    if( IsTableMode() )
    {
        // kopiere Teile aus einer Tabelle: lege eine Tabelle mit der Breite
        // von der Originalen an und kopiere die selectierten Boxen.
        // Die Groessen werden prozentual korrigiert.

        // lasse ueber das Layout die Boxen suchen
        SwTableNode* pTblNd;
        SwSelBoxes aBoxes;
        GetTblSel( *this, aBoxes );
        if( aBoxes.Count() && 0 != (pTblNd = (SwTableNode*)aBoxes[0]
            ->GetSttNd()->FindTableNode() ))
        {
            // teste ob der TabellenName kopiert werden kann
            BOOL bCpyTblNm = aBoxes.Count() == pTblNd->GetTable().GetTabSortBoxes().Count();
            if( bCpyTblNm )
            {
                const String& rTblName = pTblNd->GetTable().GetFrmFmt()->GetName();
                const SwFrmFmts& rTblFmts = *pInsDoc->GetTblFrmFmts();
                for( USHORT n = rTblFmts.Count(); n; )
                    if( rTblFmts[ --n ]->GetName() == rTblName )
                    {
                        bCpyTblNm = FALSE;
                        break;
                    }
            }
            bRet = pInsDoc->InsCopyOfTbl( aPos, aBoxes, 0, bCpyTblNm, FALSE );
        }
        else
            bRet = FALSE;
    }
    else
    {
        FOREACHPAM_START(this)

            if( !PCURCRSR->HasMark() )
            {
                if( 0 != (pNd = PCURCRSR->GetCntntNode()) && !pNd->GetTxtNode() )
                {
                    PCURCRSR->SetMark();
                    PCURCRSR->Move( fnMoveForward, fnGoCntnt );
                    bRet |= GetDoc()->Copy( *PCURCRSR, aPos );
                    PCURCRSR->Exchange();
                    PCURCRSR->DeleteMark();
                }
            }
            else
                bRet |= GetDoc()->Copy( *PCURCRSR, aPos );

        FOREACHPAM_END()
    }

    pInsDoc->UnlockExpFlds();
    if( !pInsDoc->IsExpFldsLocked() )
        pInsDoc->UpdateExpFlds();

    // die gemerkte Node-Position wieder auf den richtigen Node
    if( bRet && pSttNd )
        (*pSttNd)++;


    return bRet;
}

/*------------------------------------------------------------------------
 Beschreibung:	Text innerhalb der Selektion erfragen
 Returnwert:	liefert FALSE, wenn der selektierte Bereich
                zu gross ist, um in den Stringpuffer kopiert zu werden.
------------------------------------------------------------------------*/

BOOL SwEditShell::GetSelectedText( String &rBuf, int nHndlParaBrk )
{
    BOOL bRet = FALSE;
    GetCrsr();	// ggfs. alle Cursor erzeugen lassen
    if( IsSelOnePara() )
    {
        rBuf = GetSelTxt();
        if( GETSELTXT_PARABRK_TO_BLANK == nHndlParaBrk )
        {
            xub_StrLen nPos = 0;
            while( STRING_NOTFOUND !=
                ( nPos = rBuf.SearchAndReplace( 0x0a, ' ', nPos )) )
                ;
        }
        else if( IsSelFullPara() &&
            GETSELTXT_PARABRK_TO_ONLYCR != nHndlParaBrk )
        {
#if defined(MAC)
                rBuf += '\015';
#elif defined(UNX)
                rBuf += '\012';
#else
                rBuf += String::CreateFromAscii(
                            RTL_CONSTASCII_STRINGPARAM( "\015\012" ));
#endif
        }
        bRet = TRUE;
    }
    else if( IsSelection() )
    {
        SvCacheStream aStream(20480);
#ifdef OSL_BIGENDIAN
        aStream.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
#else
        aStream.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
#endif
        WriterRef xWrt;
        SwIoSystem::GetWriter( String::CreateFromAscii( FILTER_TEXT ), String(), xWrt );
        if( xWrt.Is() )
        {
                // Selektierte Bereiche in ein ASCII Dokument schreiben
            SwWriter aWriter( aStream, *this);
            xWrt->SetShowProgress( FALSE );

            switch( nHndlParaBrk )
            {
            case GETSELTXT_PARABRK_TO_BLANK:
                xWrt->bASCII_ParaAsBlanc = TRUE;
                xWrt->bASCII_NoLastLineEnd = TRUE;
                break;

            case GETSELTXT_PARABRK_TO_ONLYCR:
                xWrt->bASCII_ParaAsCR = TRUE;
                xWrt->bASCII_NoLastLineEnd = TRUE;
                break;
            }

            //JP 09.05.00: write as UNICODE ! (and not as ANSI)
            SwAsciiOptions aAsciiOpt( xWrt->GetAsciiOptions() );
            aAsciiOpt.SetCharSet( RTL_TEXTENCODING_UCS2 );
            xWrt->SetAsciiOptions( aAsciiOpt );
            xWrt->bUCS2_WithStartChar = FALSE;

            long lLen;
            if( !IsError( aWriter.Write( xWrt ) ) &&
                STRING_MAXLEN > (( lLen  = aStream.GetSize() )
                                        / sizeof( sal_Unicode )) + 1 )
            {
                aStream << (sal_Unicode)'\0';

                const sal_Unicode *p = (sal_Unicode*)aStream.GetBuffer();
                if( p )
                    rBuf = p;
                else
                {
                    sal_Unicode* pStrBuf = rBuf.AllocBuffer( xub_StrLen(
                                    ( lLen / sizeof( sal_Unicode )) + 1 ) );
                    aStream.Seek( 0 );
                    aStream.ResetError();
                    aStream.Read( pStrBuf, lLen );
                    pStrBuf[ lLen / sizeof( sal_Unicode ) ] = '\0';
                }
            }
        }
    }

    return TRUE;
}





