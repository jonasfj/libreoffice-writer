/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlftn.cxx,v $
 * $Revision: 1.12 $
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




#include <svtools/htmlout.hxx>
#include <svtools/htmlkywd.hxx>
#include <errhdl.hxx>
#include <ndindex.hxx>
#include <fmtftn.hxx>
#include <txtftn.hxx>
#include <ftninfo.hxx>
#include <doc.hxx>
#include <ndtxt.hxx>
#include <charfmt.hxx>


#include "swhtml.hxx"
#include "wrthtml.hxx"

typedef SwTxtFtn *SwTxtFtnPtr;
SV_DECL_PTRARR( SwHTMLTxtFtns, SwTxtFtnPtr, 1, 1 )

struct SwHTMLFootEndNote_Impl
{
    SwHTMLTxtFtns aTxtFtns;
    SvStringsDtor aNames;

    String sName;
    String sContent;			// Infos fuer die letzte Fussnote
    BOOL bEndNote;
    BOOL bFixed;
};


xub_StrLen lcl_html_getNextPart( String& rPart, const String& rContent,
                             xub_StrLen nPos )
{
    rPart = aEmptyStr;
    xub_StrLen nLen = rContent.Len();
    if( nPos >= nLen )
    {
        nPos = STRING_MAXLEN;
    }
    else
    {
        BOOL bQuoted = FALSE, bDone = FALSE;
        for( ; nPos < nLen && !bDone; nPos++ )
        {
            sal_Unicode c = rContent.GetChar( nPos );
            switch( c )
            {
            case '\\':
                if( bQuoted )
                    rPart += c;
                bQuoted = !bQuoted;
                break;

            case ';':
                if( bQuoted )
                    rPart += c;
                else
                    bDone = TRUE;
                bQuoted = FALSE;
                break;

            default:
                rPart += c;
                bQuoted = FALSE;
                break;
            }
        }
    }

    return nPos;
}

xub_StrLen lcl_html_getEndNoteInfo( SwEndNoteInfo& rInfo,
                                    const String& rContent,
                                    BOOL bEndNote )
{
    xub_StrLen nStrPos = 0;
    for( USHORT nPart = 0; nPart < 4; nPart++ )
    {
        String aPart;
        if( STRING_MAXLEN != nStrPos )
            nStrPos = lcl_html_getNextPart( aPart, rContent, nStrPos );

        switch( nPart )
        {
        case 0:
            rInfo.aFmt.SetNumberingType( static_cast< sal_Int16 >(bEndNote ? SVX_NUM_ROMAN_LOWER : SVX_NUM_ARABIC));
            if( aPart.Len() )
                rInfo.aFmt.SetNumberingType(SwHTMLParser::GetNumType( aPart,
                                                             rInfo.aFmt.GetNumberingType() ));
            break;

        case 1:
            rInfo.nFtnOffset = aPart.Len() == 0 ? 0 : (USHORT)aPart.ToInt32();
            break;

        case 2:
            rInfo.SetPrefix( aPart );
            break;

        case 3:
            rInfo.SetSuffix( aPart );
            break;
        }
    }

    return nStrPos;
}

void SwHTMLParser::FillEndNoteInfo( const String& rContent )
{
    SwEndNoteInfo aInfo( pDoc->GetEndNoteInfo() );
    lcl_html_getEndNoteInfo( aInfo, rContent, TRUE );
    pDoc->SetEndNoteInfo( aInfo );
}

void SwHTMLParser::FillFootNoteInfo( const String& rContent )
{
    SwFtnInfo aInfo( pDoc->GetFtnInfo() );

    xub_StrLen nStrPos = lcl_html_getEndNoteInfo( aInfo, rContent, FALSE );

    for( USHORT nPart = 4; nPart < 8; nPart++ )
    {
        String aPart;
        if( STRING_MAXLEN != nStrPos )
            nStrPos = lcl_html_getNextPart( aPart, rContent, nStrPos );

        switch( nPart )
        {
        case 4:
            aInfo.eNum = FTNNUM_DOC;
            if( aPart.Len() )
            {
                switch( aPart.GetChar(0) )
                {
                case 'D': aInfo.eNum = FTNNUM_DOC; break;
                case 'C': aInfo.eNum = FTNNUM_CHAPTER; break;
                case 'P': aInfo.eNum = FTNNUM_PAGE; break;
                }
            }
            break;

        case 5:
            aInfo.ePos = FTNPOS_PAGE;
            if( aPart.Len() )
            {
                switch( aPart.GetChar(0) )
                {
                case 'C': aInfo.ePos = FTNPOS_CHAPTER; break;
                case 'P': aInfo.ePos = FTNPOS_PAGE; break;
                }
            }
            break;

        case 6:
            aInfo.aQuoVadis = aPart;
            break;

        case 7:
            aInfo.aErgoSum = aPart;
            break;
        }
    }

    pDoc->SetFtnInfo( aInfo );
}

void SwHTMLParser::InsertFootEndNote( const String& rName, BOOL bEndNote,
                                      BOOL bFixed )
{
    if( !pFootEndNoteImpl )
        pFootEndNoteImpl = new SwHTMLFootEndNote_Impl;

    pFootEndNoteImpl->sName = rName;
    if( pFootEndNoteImpl->sName.Len() > 3 )
        pFootEndNoteImpl->sName.Erase( pFootEndNoteImpl->sName.Len() - 3 );
    // TODO: ToUpperAscii???
    pFootEndNoteImpl->sName.ToUpperAscii();

    pFootEndNoteImpl->bEndNote = bEndNote;
    pFootEndNoteImpl->bFixed = bFixed;
    pFootEndNoteImpl->sContent = aEmptyStr;
}

void SwHTMLParser::FinishFootEndNote()
{
    if( !pFootEndNoteImpl )
        return;

    SwFmtFtn aFtn( pFootEndNoteImpl->bEndNote );
    if( pFootEndNoteImpl->bFixed )
        aFtn.SetNumStr( pFootEndNoteImpl->sContent );

    pDoc->InsertPoolItem( *pPam, aFtn, 0 );
    SwTxtFtn * const pTxtFtn = static_cast<SwTxtFtn *>(
        pPam->GetNode()->GetTxtNode()->GetTxtAttrForCharAt(
            pPam->GetPoint()->nContent.GetIndex() - 1, RES_TXTATR_FTN ) );
    // In Kopf- und Fusszeilen duerfen keine Fussnoten eingefuegt werden.
    if( pTxtFtn )
    {
        pFootEndNoteImpl->aTxtFtns.Insert( pTxtFtn,
                                           pFootEndNoteImpl->aTxtFtns.Count() );

        pFootEndNoteImpl->aNames.Insert( new String(pFootEndNoteImpl->sName),
                                         pFootEndNoteImpl->aNames.Count() );
    }
    pFootEndNoteImpl->sName = aEmptyStr;
    pFootEndNoteImpl->sContent = aEmptyStr;
    pFootEndNoteImpl->bFixed = FALSE;
}

void SwHTMLParser::InsertFootEndNoteText()
{
    if( pFootEndNoteImpl && pFootEndNoteImpl->bFixed )
        pFootEndNoteImpl->sContent += aToken;
}

void SwHTMLParser::DeleteFootEndNoteImpl()
{
    delete pFootEndNoteImpl;
    pFootEndNoteImpl = 0;
}

SwNodeIndex *SwHTMLParser::GetFootEndNoteSection( const String& rName )
{
    SwNodeIndex *pStartNodeIdx = 0;

    if( pFootEndNoteImpl )
    {
        String aName( rName );
        // TODO: ToUpperAscii
        aName.ToUpperAscii();

        USHORT nCount = pFootEndNoteImpl->aNames.Count();
        for( USHORT i=0; i<nCount; i++ )
        {
            if( *pFootEndNoteImpl->aNames[i] == aName )
            {
                pStartNodeIdx = pFootEndNoteImpl->aTxtFtns[i]->GetStartNode();
                pFootEndNoteImpl->aNames.DeleteAndDestroy( i, 1 );
                pFootEndNoteImpl->aTxtFtns.Remove( i, 1 );
                if( !pFootEndNoteImpl->aNames.Count() )
                {
                    delete pFootEndNoteImpl;
                    pFootEndNoteImpl = 0;
                }

                break;
            }
        }
    }

    return pStartNodeIdx;
}

Writer& OutHTML_SwFmtFtn( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;

    SwFmtFtn& rFmtFtn = (SwFmtFtn&)rHt;
    SwTxtFtn *pTxtFtn = rFmtFtn.GetTxtFtn();
    if( !pTxtFtn )
        return rWrt;

    String sFtnName, sClass;
    USHORT nPos;
    if( rFmtFtn.IsEndNote() )
    {
        nPos = rHTMLWrt.pFootEndNotes ? rHTMLWrt.pFootEndNotes->Count() : 0;
        ASSERT( nPos == rHTMLWrt.nFootNote + rHTMLWrt.nEndNote,
                "OutHTML_SwFmtFtn: Position falsch" );
        sClass.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdendnote_anc );
        sFtnName.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdendnote );
        sFtnName += String::CreateFromInt32( (sal_Int32)(++rHTMLWrt.nEndNote) );
    }
    else
    {
        nPos = rHTMLWrt.nFootNote;
        sClass.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote_anc );
        sFtnName.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote);
        sFtnName += String::CreateFromInt32( (sal_Int32)(++rHTMLWrt.nFootNote));
    }

    if( !rHTMLWrt.pFootEndNotes )
        rHTMLWrt.pFootEndNotes = new SwHTMLTxtFtns;
    rHTMLWrt.pFootEndNotes->Insert( pTxtFtn, nPos );

    ByteString sOut( '<' );
    (((sOut += OOO_STRING_SVTOOLS_HTML_anchor) += ' ') += OOO_STRING_SVTOOLS_HTML_O_class) += "=\"";
    rWrt.Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( rWrt.Strm(), sClass, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
    ((sOut = "\" ") += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"";
    rWrt.Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( rWrt.Strm(), sFtnName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
    (((sOut = OOO_STRING_SVTOOLS_HTML_FTN_anchor) += "\" ") += OOO_STRING_SVTOOLS_HTML_O_href) += "=\"#";
    rWrt.Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( rWrt.Strm(), sFtnName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
    (sOut = OOO_STRING_SVTOOLS_HTML_FTN_symbol)+= '\"';
    if( rFmtFtn.GetNumStr().Len() )
        (sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_sdfixed;
    sOut += '>';
    rWrt.Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_superscript, TRUE );

    HTMLOutFuncs::Out_String( rWrt.Strm(), rFmtFtn.GetViewNumStr(*rWrt.pDoc),
                                 rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
    HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_superscript, FALSE );
    HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_anchor, FALSE );

    return rWrt;
}

void SwHTMLWriter::OutFootEndNotes()
{
    ASSERT( pFootEndNotes,
            "SwHTMLWriter::OutFootEndNotes(): unnoetiger Aufruf" );
    if( !pFootEndNotes )
        return;

#ifndef PRODUCT
    USHORT nFtn = nFootNote, nEn = nEndNote;
#endif
    nFootNote = 0, nEndNote = 0;

    for( USHORT i=0; i<pFootEndNotes->Count(); i++ )
    {
        SwTxtFtn *pTxtFtn = (*pFootEndNotes)[i];
        pFmtFtn = &pTxtFtn->GetFtn();

        String sFtnName, sClass;
        if( pFmtFtn->IsEndNote() )
        {
            sClass.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdendnote );
            sFtnName.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdendnote );
            sFtnName.Append( String::CreateFromInt32((sal_Int32)(++nEndNote)) );
        }
        else
        {
            sClass.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote );
            sFtnName.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote );
            sFtnName.Append( String::CreateFromInt32((sal_Int32)(++nFootNote)));
        }

        if( bLFPossible )
            OutNewLine();
        ByteString sOut( '<' );
        (((sOut += OOO_STRING_SVTOOLS_HTML_division) += ' ') += OOO_STRING_SVTOOLS_HTML_O_id) += "=\"";
        Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( Strm(), sFtnName, eDestEnc, &aNonConvertableCharacters );
        Strm() << "\">";

        bLFPossible = TRUE;
        IncIndentLevel();	// Inhalt von <DIV> einruecken

        ASSERT( pTxtFtn, "SwHTMLWriter::OutFootEndNotes: SwTxtFtn fehlt" );
        SwNodeIndex *pSttNdIdx = pTxtFtn->GetStartNode();
        ASSERT( pSttNdIdx,
                "SwHTMLWriter::OutFootEndNotes: StartNode-Index fehlt" );
        if( pSttNdIdx )
        {
            HTMLSaveData aSaveData( *this, pSttNdIdx->GetIndex()+1,
                pSttNdIdx->GetNode().EndOfSectionIndex(), FALSE );
            Out_SwDoc( pCurPam );
        }

        DecIndentLevel();	// Inhalt von <DIV> einruecken
        if( bLFPossible )
            OutNewLine();
        HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_division, FALSE );
        bLFPossible = TRUE;

        ASSERT( !pFmtFtn,
                "SwHTMLWriter::OutFootEndNotes: Ftn wurde nicht ausgegeben" );
        if( pFmtFtn )
        {
            if( pFmtFtn->IsEndNote() )
                nEndNote++;
            else
                nFootNote++;

            pFmtFtn = 0;
        }
    }

#ifndef PRODUCT
    ASSERT( nFtn == nFootNote,
            "SwHTMLWriter::OutFootEndNotes: Anzahl Fussnoten stimmt nicht" );
    ASSERT( nEn == nEndNote,
            "SwHTMLWriter::OutFootEndNotes: Anzahl Endnoten stimmt nicht" );
#endif

    delete pFootEndNotes;
    pFootEndNotes = 0;
    nFootNote = nEndNote = 0;
}

String SwHTMLWriter::GetFootEndNoteSym( const SwFmtFtn& rFmtFtn )
{
    const SwEndNoteInfo * pInfo = 0;
    if( rFmtFtn.GetNumStr().Len() == 0 )
        pInfo = rFmtFtn.IsEndNote() ? &pDoc->GetEndNoteInfo()
                                    : &pDoc->GetFtnInfo();

    String sRet;
    if( pInfo )
        sRet = pInfo->GetPrefix();
    sRet += rFmtFtn.GetViewNumStr( *pDoc );
    if( pInfo )
        sRet += pInfo->GetSuffix();

    return sRet;
}

void SwHTMLWriter::OutFootEndNoteSym( const SwFmtFtn& rFmtFtn,
                                         const String& rNum,
                                         sal_uInt16 nScript )
{
    const SwEndNoteInfo *pInfo;

    String sFtnName, sClass, sPrefix, sSuffix;
    if( rFmtFtn.IsEndNote() )
    {
        sClass.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdendnote_sym );
        sFtnName.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdendnote );
        sFtnName.Append( String::CreateFromInt32((sal_Int32)nEndNote) );
        pInfo = &pDoc->GetEndNoteInfo();
    }
    else
    {
        sClass.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote_sym );
        sFtnName.AssignAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote );
        sFtnName.Append( String::CreateFromInt32((sal_Int32)nFootNote));
        pInfo = &pDoc->GetFtnInfo();
    }

    const SwCharFmt *pSymCharFmt = pInfo->GetCharFmt( *pDoc );
    if( pSymCharFmt && aScriptTextStyles.Seek_Entry( (String *)&pSymCharFmt->GetName() ) )
    {
        switch( nScript )
        {
        case CSS1_OUTMODE_WESTERN:
            sClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("-western") );
            break;
        case CSS1_OUTMODE_CJK:
            sClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("-cjk") );
            break;
        case CSS1_OUTMODE_CTL:
            sClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("-ctl") );
            break;
        }
    }

    ByteString sOut( '<' );
    (((sOut += OOO_STRING_SVTOOLS_HTML_anchor) +=  ' ') += OOO_STRING_SVTOOLS_HTML_O_class) += "=\"";
    Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( Strm(), sClass, eDestEnc, &aNonConvertableCharacters );
    ((sOut = "\" ") += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"";
    Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( Strm(), sFtnName, eDestEnc, &aNonConvertableCharacters );
    (((sOut = OOO_STRING_SVTOOLS_HTML_FTN_symbol) +="\" ") += OOO_STRING_SVTOOLS_HTML_O_href) += "=\"#";
    Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( Strm(), sFtnName, eDestEnc, &aNonConvertableCharacters );
    (sOut = OOO_STRING_SVTOOLS_HTML_FTN_anchor) += "\">";
    Strm() << sOut.GetBuffer();

    HTMLOutFuncs::Out_String( Strm(), rNum, eDestEnc, &aNonConvertableCharacters );
    HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_anchor, FALSE );
}

USHORT lcl_html_fillEndNoteInfo( const SwEndNoteInfo& rInfo,
                                 String *pParts,
                                 BOOL bEndNote	)
{
    USHORT nParts = 0;
    sal_Int16 eFmt = rInfo.aFmt.GetNumberingType();
    if( (bEndNote ? SVX_NUM_ROMAN_LOWER : SVX_NUM_ARABIC) != eFmt )
    {
        const sal_Char *pStr = SwHTMLWriter::GetNumFormat( eFmt );
        if( pStr )
        {
            pParts[0] = String::CreateFromAscii( pStr );
            nParts = 1;
        }
    }
    if( rInfo.nFtnOffset > 0 )
    {
        pParts[1] =	String::CreateFromInt32( (sal_Int32)rInfo.nFtnOffset );
        nParts = 2;
    }
    if( rInfo.GetPrefix().Len() > 0 )
    {
        pParts[2] =	rInfo.GetPrefix();
        nParts = 3;
    }
    if( rInfo.GetSuffix().Len() > 0 )
    {
        pParts[3] =	rInfo.GetSuffix();
        nParts = 4;
    }

    return nParts;
}

void lcl_html_outFootEndNoteInfo( Writer& rWrt, String *pParts,
                                  USHORT nParts, const sal_Char *pName )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;

    String aContent;
    for( USHORT i=0; i<nParts; i++ )
    {
        xub_StrLen nPos = 0;
        String aTmp( pParts[i] );
        String aRep( String::CreateFromAscii("\\\\") );
        while( STRING_NOTFOUND != (nPos = aTmp.SearchAndReplaceAscii( "\\",
                                                     aRep, nPos ) ) )
            nPos += 2;
        nPos = 0;
        aRep.AssignAscii( "\\;" );
        while( STRING_NOTFOUND != (nPos = aTmp.SearchAndReplaceAscii( ";",
                                                     aRep, nPos ) ) )
            nPos += 2;
        if( i > 0 )
            aContent += ';';
        aContent += aTmp;
    }

    rHTMLWrt.OutNewLine();
    ByteString sOut( '<' );
    (((((((sOut += OOO_STRING_SVTOOLS_HTML_meta) +=  ' ')
        += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"") += pName) += "\" ")
        += OOO_STRING_SVTOOLS_HTML_O_content) += "=\"";
    rWrt.Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( rWrt.Strm(), aContent, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
    rWrt.Strm() << "\">";
}

void SwHTMLWriter::OutFootEndNoteInfo()
{
    // Nummerntyp (1 bzw. i)
    // Offset (0)
    // Davor
    // Dahinter
    // Dok/Seite/Kap (D)
    // Position (S)
    // Folgeseite
    // Beginn

    {
        const SwFtnInfo& rInfo = pDoc->GetFtnInfo();
        String aParts[8];
        USHORT nParts = lcl_html_fillEndNoteInfo( rInfo, aParts, FALSE );
        if( rInfo.eNum != FTNNUM_DOC )
        {
            aParts[4] = rInfo.eNum == FTNNUM_CHAPTER ? 'C' : 'P';
            nParts = 5;
        }
        if( rInfo.ePos != FTNPOS_PAGE)
        {
            aParts[5] = 'C';
            nParts = 6;
        }
        if( rInfo.aQuoVadis.Len() > 0 )
        {
            aParts[6] =	rInfo.aQuoVadis;
            nParts = 7;
        }
        if( rInfo.aErgoSum.Len() > 0 )
        {
            aParts[7] =	rInfo.aErgoSum;
            nParts = 8;
        }
        if( nParts > 0 )
            lcl_html_outFootEndNoteInfo( *this, aParts, nParts,
                                         OOO_STRING_SVTOOLS_HTML_META_sdfootnote );
    }

    {
        const SwEndNoteInfo& rInfo = pDoc->GetEndNoteInfo();
        String aParts[4];
        USHORT nParts = lcl_html_fillEndNoteInfo( rInfo, aParts, TRUE );
        if( nParts > 0 )
            lcl_html_outFootEndNoteInfo( *this, aParts, nParts,
                                         OOO_STRING_SVTOOLS_HTML_META_sdendnote );
    }
}

