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


#define _SVSTDARR_LONGS
#define _SVSTDARR_USHORTS

#include <ctype.h>
#include <hintids.hxx>

#include <svl/svstdarr.hxx>
#include <unotools/charclass.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/brkitem.hxx>
#include <editeng/adjitem.hxx>
#include <editeng/tstpitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/langitem.hxx>
#include <editeng/cscoitem.hxx>
#include <editeng/unolingu.hxx>

#include <editeng/acorrcfg.hxx>
#include <swwait.hxx>
#include <fmtpdsc.hxx>
#include <fmtanchr.hxx>
#include <doc.hxx>
#include <docary.hxx>
#include <editsh.hxx>
#include <index.hxx>
#include <pam.hxx>
#include <edimp.hxx>
#include <fesh.hxx>
#include <swundo.hxx>		// fuer die UndoIds
#include <poolfmt.hxx>
#include <ndtxt.hxx>
#include <txtfrm.hxx>
#include <frminf.hxx>
#include <pagedesc.hxx>
#include <paratr.hxx>
#include <swtable.hxx>
#include <acorrect.hxx>
#include <shellres.hxx>
#include <section.hxx>
#include <frmatr.hxx>
#include <charatr.hxx>
#include <mdiexp.hxx>
#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif
#ifndef _COMCORE_HRC
#include <comcore.hrc>
#endif
#include <vcl/msgbox.hxx>
#include <numrule.hxx>

using namespace ::com::sun::star;

//-------------------------------------------------------------------

//JP 16.12.99: definition:
//		from pos cPosEnDash to cPosEmDash all chars changed to endashes,
//		from pos cPosEmDash to cPosEnd    all chars changed to emdashes
// 		all other chars are changed to the user configuration

const sal_Unicode pBulletChar[6] = { '+', '*', '-', 0x2013, 0x2014, 0 };
const int cnPosEnDash = 2, cnPosEmDash = 4, cnPosEnd = 5;

const sal_Unicode cStarSymbolEnDash = 0x2013;
const sal_Unicode cStarSymbolEmDash = 0x2014;


SvxSwAutoFmtFlags* SwEditShell::pAutoFmtFlags = 0;

// Anzahl von Num-/Bullet-Absatzvorlagen. MAXLEVEL wird demnaechst auf
// x erhoeht, die Anzahl Vorlagen aber nicht (Ueberbleibsel aus <= 4.0)
const USHORT cnNumBullColls = 4;

class SwAutoFormat
{
    SvxSwAutoFmtFlags aFlags;
    SwPaM aDelPam;				// ein Pam der benutzt werden kann
    SwNodeIndex aNdIdx;			// der Index auf den akt. TextNode
    SwNodeIndex aEndNdIdx;		// Index auf das Ende vom Bereich

    SwEditShell* pEditShell;
    SwDoc* pDoc;
    SwTxtNode* pAktTxtNd;		// der akt. TextNode
    SwTxtFrm* pAktTxtFrm;		// Frame vom akt. TextNode
    CharClass* pCharClass;		// Character classification
    ULONG nEndNdIdx;			// fuer die Prozent-Anzeige
    LanguageType eCharClassLang;

    USHORT nLastHeadLvl, nLastCalcHeadLvl;
    USHORT nLastEnumLvl, nLastCalcEnumLvl;
    USHORT nRedlAutoFmtSeqId;

    enum
    {
        NONE = 0,
        DELIM = 1,
        DIGIT = 2,
        CHG = 4,
        LOWER_ALPHA = 8,
        UPPER_ALPHA = 16,
        LOWER_ROMAN = 32,
        UPPER_ROMAN = 64,
        NO_DELIM = (DIGIT|LOWER_ALPHA|UPPER_ALPHA|LOWER_ROMAN|UPPER_ROMAN)
    };

    enum Format_Status
    {
        READ_NEXT_PARA,
        TST_EMPTY_LINE,
        TST_ALPHA_LINE,
        GET_ALL_INFO,
        IS_ONE_LINE,
        TST_ENUMERIC,
        TST_IDENT,
        TST_NEG_IDENT,
        TST_TXT_BODY,
        HAS_FMTCOLL,
        IS_ENDE
    } eStat;

    BOOL bEnde : 1;
    BOOL bEmptyLine : 1;
    BOOL bMoreLines : 1;

    static BOOL  m_bAskForCancelUndoWhileBufferOverflow;
    static short m_nActionWhileAutoformatUndoBufferOverflow;


    // ------------- private methods -----------------------------
    void _GetCharClass( LanguageType eLang );
    CharClass& GetCharClass( LanguageType eLang ) const
    {
        if( !pCharClass || eLang != eCharClassLang )
        {
            SwAutoFormat* pThis = (SwAutoFormat*)this;
            pThis->_GetCharClass( eLang );
        }
        return *pCharClass;
    }


    BOOL IsSpace( const sal_Unicode c ) const
        { return (' ' == c || '\t' == c || 0x0a == c|| 0x3000 == c /* Jap. space */) ? TRUE : FALSE; }

    void SetColl( USHORT nId, BOOL bHdLineOrText = FALSE );
    String GoNextPara();
    BOOL HasObjects( const SwNode& rNd );

    // TxtNode Methoden
    const SwTxtNode* GetNextNode() const;
    BOOL IsEmptyLine( const SwTxtNode& rNd ) const
        {	return 0 == rNd.GetTxt().Len() ||
                rNd.GetTxt().Len() == GetLeadingBlanks( rNd.GetTxt() ); }

    BOOL IsOneLine( const SwTxtNode& ) const;
    BOOL IsFastFullLine( const SwTxtNode& ) const;
    BOOL IsNoAlphaLine( const SwTxtNode&) const;
    BOOL IsEnumericChar( const SwTxtNode&) const;
    BOOL IsBlanksInString( const SwTxtNode&) const;
    USHORT CalcLevel( const SwTxtNode&, USHORT *pDigitLvl = 0 ) const;
    xub_StrLen GetBigIndent( xub_StrLen& rAktSpacePos ) const;

    String& DelLeadingBlanks( String& rStr ) const;
    String& DelTrailingBlanks( String& rStr ) const;
    xub_StrLen GetLeadingBlanks( const String& rStr ) const;
    xub_StrLen GetTrailingBlanks( const String& rStr ) const;

    BOOL IsFirstCharCapital( const SwTxtNode& rNd ) const;
    USHORT GetDigitLevel( const SwTxtNode& rTxtNd, xub_StrLen& rPos,
                            String* pPreFix = 0, String* pPostFix = 0,
                            String* pNumTypes = 0 ) const;
        // hole den FORMATIERTEN TextFrame
    SwTxtFrm* GetFrm( const SwTxtNode& rTxtNd ) const;

    void BuildIndent();
    void BuildText();
    void BuildTextIndent();
    void BuildEnum( USHORT nLvl, USHORT nDigitLevel );
    void BuildNegIndent( SwTwips nSpaces );
    void BuildHeadLine( USHORT nLvl );

    BOOL HasSelBlanks( SwPaM& rPam ) const;
    BOOL HasBreakAttr( const SwTxtNode& ) const;
    void DeleteSel( SwPaM& rPam );
    BOOL DeleteAktNxtPara( const String& rNxtPara );
    // loesche im Node Anfang oder/und Ende
    void DeleteAktPara( BOOL bStart = TRUE, BOOL nEnd = TRUE );
    void DelEmptyLine( BOOL bTstNextPara = TRUE );
        // loesche bei mehrzeiligen Absaetzen die "linken" und/oder
        // "rechten" Raender
    void DelMoreLinesBlanks( BOOL bWithLineBreaks = FALSE );
        // loesche den vorherigen Absatz
    void DelPrevPara();
        // dann lasse doch mal das AutoCorrect auf den akt. TextNode los
    void AutoCorrect( xub_StrLen nSttPos = 0 );

    BOOL CanJoin( const SwTxtNode* pTxtNd ) const
    {
        return !bEnde && pTxtNd &&
             !IsEmptyLine( *pTxtNd ) &&
             !IsNoAlphaLine( *pTxtNd) &&
             !IsEnumericChar( *pTxtNd ) &&
             ((STRING_MAXLEN - 50 - pTxtNd->GetTxt().Len()) >
                    pAktTxtNd->GetTxt().Len()) &&
             !HasBreakAttr( *pTxtNd );
    }

    // ist ein Punkt am Ende ??
    BOOL IsSentenceAtEnd( const SwTxtNode& rTxtNd ) const;

    BOOL DoUnderline();
    BOOL DoTable();

    void _SetRedlineTxt( USHORT nId );
    BOOL SetRedlineTxt( USHORT nId )
        { if( aFlags.bWithRedlining )	_SetRedlineTxt( nId );  return TRUE; }
    BOOL ClearRedlineTxt()
        { if( aFlags.bWithRedlining )	pDoc->SetAutoFmtRedlineComment(0);  return TRUE; }

public:
    SwAutoFormat( SwEditShell* pEdShell, SvxSwAutoFmtFlags& rFlags,
                SwNodeIndex* pSttNd = 0, SwNodeIndex* pEndNd = 0 );
    ~SwAutoFormat() {
        delete pCharClass;
    }
};

BOOL  SwAutoFormat::m_bAskForCancelUndoWhileBufferOverflow     = TRUE;
short SwAutoFormat::m_nActionWhileAutoformatUndoBufferOverflow = RET_YES;

const sal_Unicode* StrChr( const sal_Unicode* pSrc, sal_Unicode c )
{
    while( *pSrc && *pSrc != c )
        ++pSrc;
    return *pSrc ? pSrc : 0;
}

SwTxtFrm* SwAutoFormat::GetFrm( const SwTxtNode& rTxtNd ) const
{
    // besorge mal den Frame
    const SwCntntFrm *pFrm = rTxtNd.GetFrm();
    ASSERT( pFrm, "zum Autoformat muss das Layout vorhanden sein" );
    if( aFlags.bAFmtByInput && !pFrm->IsValid() )
    {
        SwRect aTmpFrm( pFrm->Frm() );
        SwRect aTmpPrt( pFrm->Prt() );
        pFrm->Calc();
        if( pFrm->Frm() != aTmpFrm || pFrm->Prt() != aTmpPrt ||
            ( pFrm->IsTxtFrm() && !((SwTxtFrm*)pFrm)->Paint().IsEmpty() ) )
            pFrm->SetCompletePaint();
    }
    return ((SwTxtFrm*)pFrm)->GetFormatted();
}

void SwAutoFormat::_GetCharClass( LanguageType eLang )
{
    delete pCharClass;
    pCharClass = new CharClass( SvxCreateLocale( eLang ));
    eCharClassLang = eLang;
}

void SwAutoFormat::_SetRedlineTxt( USHORT nActionId )
{
    String sTxt;
    USHORT nSeqNo = 0;
    if( STR_AUTOFMTREDL_END > nActionId )
    {
        sTxt = *ViewShell::GetShellRes()->GetAutoFmtNameLst()[ nActionId ];
        switch( nActionId )
        {
        case STR_AUTOFMTREDL_SET_NUMBULET:
        case STR_AUTOFMTREDL_DEL_MORELINES:

        // AutoCorrect-Actions
        case STR_AUTOFMTREDL_USE_REPLACE:
        case STR_AUTOFMTREDL_CPTL_STT_WORD:
        case STR_AUTOFMTREDL_CPTL_STT_SENT:
        case STR_AUTOFMTREDL_TYPO:
        case STR_AUTOFMTREDL_UNDER:
        case STR_AUTOFMTREDL_BOLD:
        case STR_AUTOFMTREDL_FRACTION:
        case STR_AUTOFMTREDL_DASH:
        case STR_AUTOFMTREDL_ORDINAL:
        case STR_AUTOFMTREDL_NON_BREAK_SPACE:
            nSeqNo = ++nRedlAutoFmtSeqId;
            break;
        }
    }
#if OSL_DEBUG_LEVEL > 1
    else
        sTxt = String::CreateFromAscii(
                            RTL_CONSTASCII_STRINGPARAM( "Action-Text fehlt" ));
#endif

    pDoc->SetAutoFmtRedlineComment( &sTxt, nSeqNo );
}

String SwAutoFormat::GoNextPara()
{
    SwNode* pNewNd = 0;
    do {
        //has to be checed twice before and after incrementation
        if( aNdIdx.GetIndex() >= aEndNdIdx.GetIndex() )
        {
            bEnde = TRUE;
            return aEmptyStr;
        }

        aNdIdx++;
        if( aNdIdx.GetIndex() >= aEndNdIdx.GetIndex() )
        {
            bEnde = TRUE;
            return aEmptyStr;
        }
        else
            pNewNd = &aNdIdx.GetNode();

        // kein TextNode ->
        // 		TableNode	: Tabelle ueberspringen
        // 		NoTxtNode	: Nodes ueberspringen
        // 		EndNode		: Ende erreicht, beenden
        if( pNewNd->IsEndNode() )
        {
            bEnde = TRUE;
            return aEmptyStr;
        }
        else if( pNewNd->IsTableNode() )
            aNdIdx = *pNewNd->EndOfSectionNode();
        else if( pNewNd->IsSectionNode() )
        {
            const SwSection& rSect = pNewNd->GetSectionNode()->GetSection();
            if( rSect.IsHiddenFlag() || rSect.IsProtectFlag() )
                aNdIdx = *pNewNd->EndOfSectionNode();
        }
    } while( !pNewNd->IsTxtNode() );

    if( !aFlags.bAFmtByInput )
        ::SetProgressState( aNdIdx.GetIndex() + nEndNdIdx - aEndNdIdx.GetIndex(),
                            pDoc->GetDocShell() );

    pAktTxtNd = (SwTxtNode*)pNewNd;
    pAktTxtFrm = GetFrm( *pAktTxtNd );
    return pAktTxtNd->GetTxt();
}

BOOL SwAutoFormat::HasObjects( const SwNode& rNd )
{
    // haengt irgend etwas absatzgebundenes am Absatz?
    // z.B. Rahmen, DrawObjecte, ..
    BOOL bRet = FALSE;
    const SwSpzFrmFmts& rFmts = *pDoc->GetSpzFrmFmts();
    for( USHORT n = 0; n < rFmts.Count(); ++n )
    {
        const SwFmtAnchor& rAnchor = rFmts[ n ]->GetAnchor();
        if ((FLY_AT_PAGE != rAnchor.GetAnchorId()) &&
            rAnchor.GetCntntAnchor() &&
            &rAnchor.GetCntntAnchor()->nNode.GetNode() == &rNd )
        {
            bRet = TRUE;
            break;
        }
    }
    return bRet;
}

const SwTxtNode* SwAutoFormat::GetNextNode() const
{
    if( aNdIdx.GetIndex()+1 >= aEndNdIdx.GetIndex() )
        return 0;
    return pDoc->GetNodes()[ aNdIdx.GetIndex() + 1 ]->GetTxtNode();
}


BOOL SwAutoFormat::IsOneLine( const SwTxtNode& rNd ) const
{
    SwTxtFrmInfo aFInfo( GetFrm( rNd ) );
    return aFInfo.IsOneLine();
}


BOOL SwAutoFormat::IsFastFullLine( const SwTxtNode& rNd ) const
{
    BOOL bRet = aFlags.bRightMargin;
    if( bRet )
    {
        SwTxtFrmInfo aFInfo( GetFrm( rNd ) );
        bRet = aFInfo.IsFilled( aFlags.nRightMargin );
    }
    return bRet;
}


BOOL SwAutoFormat::IsEnumericChar( const SwTxtNode& rNd ) const
{
    const String& rTxt = rNd.GetTxt();
    String sTmp( rTxt );
    xub_StrLen nBlnks = GetLeadingBlanks( sTmp );
    xub_StrLen nLen = rTxt.Len() - nBlnks;
    if( !nLen )
        return FALSE;

    // -, +, * getrennt durch Blank ??
    if( 2 < nLen && IsSpace( rTxt.GetChar( nBlnks + 1 ) ) )
    {
        if( StrChr( pBulletChar, rTxt.GetChar( nBlnks ) ) )
            return TRUE;
        // sollte an der Position ein Symbolfont existieren ?
        SwTxtFrmInfo aFInfo( GetFrm( rNd ) );
        if( aFInfo.IsBullet( nBlnks ))
            return TRUE;
    }

    // 1.) / 1. / 1.1.1 / (1). / (1) / ....
    return USHRT_MAX != GetDigitLevel( rNd, nBlnks );
}


BOOL SwAutoFormat::IsBlanksInString( const SwTxtNode& rNd ) const
{
    // suche im String mehr als 5 Blanks/Tabs
    String sTmp( rNd.GetTxt() );
    DelTrailingBlanks( DelLeadingBlanks( sTmp ));
    const sal_Unicode* pTmp = sTmp.GetBuffer();
    while( *pTmp )
    {
        if( IsSpace( *pTmp ) )
        {
            if( IsSpace( *++pTmp ))		// 2 Space nach einander
            {
                const sal_Unicode* pStt = pTmp;
                while( *pTmp && IsSpace( *++pTmp ))
                    ;
                if( 5 <= pTmp - pStt )
                    return TRUE;
            }
            else
                ++pTmp;
        }
        else
            ++pTmp;
    }
    return FALSE;
}


USHORT SwAutoFormat::CalcLevel( const SwTxtNode& rNd, USHORT *pDigitLvl ) const
{
    USHORT nLvl = 0, nBlnk = 0;
    const String& rTxt = rNd.GetTxt();
    if( pDigitLvl )
        *pDigitLvl = USHRT_MAX;

    if( RES_POOLCOLL_TEXT_MOVE == rNd.GetTxtColl()->GetPoolFmtId() )
    {
        if( aFlags.bAFmtByInput )
        {
            nLvl = rNd.GetAutoFmtLvl();
            ((SwTxtNode&)rNd).SetAutoFmtLvl( 0 );
            if( nLvl )
                return nLvl;
        }
        ++nLvl;
    }


    for( xub_StrLen n = 0, nEnd = rTxt.Len(); n < nEnd; ++n )
    {
        switch( rTxt.GetChar( n ) )
        {
        case ' ': 	if( 3 == ++nBlnk )
                        ++nLvl, nBlnk = 0;
                    break;
        case '\t':	++nLvl, nBlnk = 0;
                    break;
        default:
            if( pDigitLvl )
                // Teste auf 1.) / 1. / 1.1.1 / (1). / (1) / ....
                *pDigitLvl = GetDigitLevel( rNd, n );
            return nLvl;
        }
    }
    return nLvl;
}



xub_StrLen SwAutoFormat::GetBigIndent( xub_StrLen& rAktSpacePos ) const
{
    SwTxtFrmInfo aFInfo( GetFrm( *pAktTxtNd ) );
    const SwTxtFrm* pNxtFrm = 0;

    if( !bMoreLines )
    {
        const SwTxtNode* pNxtNd = GetNextNode();
        if( !CanJoin( pNxtNd ) || !IsOneLine( *pNxtNd ) )
            return 0;

        pNxtFrm = GetFrm( *pNxtNd );
    }

    return aFInfo.GetBigIndent( rAktSpacePos, pNxtFrm );
}


BOOL SwAutoFormat::IsNoAlphaLine( const SwTxtNode& rNd ) const
{
    const String& rStr = rNd.GetTxt();
    if( !rStr.Len() )
        return FALSE;
    // oder besser: ueber die Anzahl von Alpha/Num- und !AN-Zeichen
    //				bestimmen.
    xub_StrLen nANChar = 0, nBlnk = 0;

    CharClass& rCC = GetCharClass( rNd.GetSwAttrSet().GetLanguage().GetLanguage() );
    for( xub_StrLen n = 0, nEnd = rStr.Len(); n < nEnd; ++n )
        if( IsSpace( rStr.GetChar( n ) ) )
            ++nBlnk;
        else if( rCC.isLetterNumeric( rStr, n ))
            ++nANChar;

    // sind zu 75% keine Alpha-Nummerische-Zeichen, dann TRUE
    ULONG nLen = rStr.Len() - nBlnk;
    nLen = ( nLen * 3 ) / 4;		    // long overflow, if the strlen > USHORT
    return xub_StrLen(nLen) < (rStr.Len() - nANChar - nBlnk);
}



BOOL SwAutoFormat::DoUnderline()
{
    if( !aFlags.bSetBorder )
        return FALSE;

    const sal_Unicode* pStr = pAktTxtNd->GetTxt().GetBuffer();
    int eState = 0;
    xub_StrLen nCnt = 0;
    while( *pStr )
    {
//JP 29.03.96: Spaces unterbrechen die Umrandung!
//		if( !IsSpace( *pStr ) )
        {
            int eTmp = 0;
            switch( *pStr )
            {
            case '-': eTmp = 1;	break;
            case '_': eTmp = 2;	break;
            case '=': eTmp = 3;	break;
            case '*': eTmp = 4;	break;
            case '~': eTmp = 5;	break;
            case '#': eTmp = 6;	break;
            default:
                return FALSE;
            }
            if( 0 == eState )
                eState = eTmp;
            else if( eState != eTmp )
                return FALSE;
            ++nCnt;
        }
        ++pStr;
    }

    if( 2 < nCnt )
    {
        // dann unterstreiche mal den vorherigen Absatz, wenn es diesen gibt!
        DelEmptyLine( FALSE );
        aDelPam.SetMark();
        aDelPam.GetMark()->nContent = 0;
//JP 19.03.96: kein Underline sondern eine Umrandung setzen!
//		pDoc->Insert( aDelPam, SvxUnderlineItem( eState ) );

        SvxBorderLine aLine;
        switch( eState )
        {
        case 1:			// einfach, 0,05 pt
            aLine.SetOutWidth( DEF_LINE_WIDTH_0 );
            break;
        case 2:			// einfach, 1,0 pt
            aLine.SetOutWidth( DEF_LINE_WIDTH_1 );
            break;
        case 3:			// doppelt, 1,1 pt
            aLine.SetOutWidth( DEF_DOUBLE_LINE0_OUT );
            aLine.SetInWidth( DEF_DOUBLE_LINE0_IN );
            aLine.SetDistance( DEF_DOUBLE_LINE0_DIST );
            break;
        case 4:			// doppelt, 4,5 pt
            aLine.SetOutWidth( DEF_DOUBLE_LINE4_OUT );
            aLine.SetInWidth( DEF_DOUBLE_LINE4_IN );
            aLine.SetDistance( DEF_DOUBLE_LINE4_DIST );
            break;
        case 5:			// doppelt, 6,0 pt
            aLine.SetOutWidth( DEF_DOUBLE_LINE5_OUT );
            aLine.SetInWidth( DEF_DOUBLE_LINE5_IN );
            aLine.SetDistance( DEF_DOUBLE_LINE5_DIST );
            break;
        case 6:			// doppelt, 9,0 pt
            aLine.SetOutWidth( DEF_DOUBLE_LINE6_OUT );
            aLine.SetInWidth( DEF_DOUBLE_LINE6_IN );
            aLine.SetDistance( DEF_DOUBLE_LINE6_DIST );
            break;
        }
        SfxItemSet aSet(pDoc->GetAttrPool(),
                    RES_PARATR_CONNECT_BORDER, RES_PARATR_CONNECT_BORDER,
                    RES_BOX, RES_BOX,
                    0);
        aSet.Put( SwParaConnectBorderItem( FALSE ) );
        SvxBoxItem aBox( RES_BOX );
        aBox.SetLine( &aLine, BOX_LINE_BOTTOM );
        aBox.SetDistance( 42 );		// ~0,75 mm
        aSet.Put(aBox);
        pDoc->InsertItemSet( aDelPam, aSet, 0 );

        aDelPam.DeleteMark();
    }
    return 2 < nCnt;
}


BOOL SwAutoFormat::DoTable()
{
    if( !aFlags.bCreateTable || !aFlags.bAFmtByInput ||
        pAktTxtNd->FindTableNode() )
        return FALSE;

    const String& rTmp = pAktTxtNd->GetTxt();
    xub_StrLen nSttPlus = GetLeadingBlanks( rTmp );
    xub_StrLen nEndPlus = GetTrailingBlanks( rTmp );
    sal_Unicode cChar;

    if( 2 > nEndPlus - nSttPlus ||
        ( '+' != ( cChar = rTmp.GetChar( nSttPlus )) && '|' != cChar ) ||
        ( '+' != ( cChar = rTmp.GetChar( nEndPlus - 1)) && '|' != cChar ))
        return FALSE;

    SwTxtFrmInfo aInfo( pAktTxtFrm );

    xub_StrLen n = nSttPlus;
    const sal_Unicode* pStr = rTmp.GetBuffer() + n;
    SvUShorts aPosArr( 5, 5 );

    while( *pStr )
    {
        switch( *pStr )
        {
        case '-':
        case '_':
        case '=':
        case ' ':
        case '\t':
            break;

        case '+':
        case '|':
            aPosArr.Insert( static_cast<USHORT>(aInfo.GetCharPos(n)), aPosArr.Count() );
            break;

        default:
            return FALSE;
        }
        if( ++n == nEndPlus )
            break;

        ++pStr;
    }

    if( 1 < aPosArr.Count() )
    {
        // Ausrichtung vom Textnode besorgen:
        USHORT nColCnt = aPosArr.Count() - 1;
        SwTwips nSttPos = aPosArr[ 0 ];
        sal_Int16 eHori;
        switch( pAktTxtNd->GetSwAttrSet().GetAdjust().GetAdjust() )
        {
        case SVX_ADJUST_CENTER:     eHori = text::HoriOrientation::CENTER;    break;
        case SVX_ADJUST_RIGHT:      eHori = text::HoriOrientation::RIGHT;     break;

        default:
            if( nSttPos )
            {
                eHori = text::HoriOrientation::NONE;
                // dann muss als letztes noch die akt. FrameBreite
                // ins Array
                aPosArr.Insert( static_cast<USHORT>(pAktTxtFrm->Frm().Width()), aPosArr.Count() );
            }
            else
                eHori = text::HoriOrientation::LEFT;
            break;
        }

        // dann erzeuge eine Tabelle, die den Zeichen entspricht
        DelEmptyLine();
        SwNodeIndex aIdx( aDelPam.GetPoint()->nNode );
        aDelPam.Move( fnMoveForward );
        pDoc->InsertTable( SwInsertTableOptions( tabopts::ALL_TBL_INS_ATTR , 1 ),
                           *aDelPam.GetPoint(), 1, nColCnt, eHori,
                           0, &aPosArr );
        aDelPam.GetPoint()->nNode = aIdx;
    }
    return 1 < aPosArr.Count();
}


String& SwAutoFormat::DelLeadingBlanks( String& rStr ) const
{
    xub_StrLen nL;
    xub_StrLen n;

    for( nL = rStr.Len(), n = 0; n < nL && IsSpace( rStr.GetChar(n) ); ++n )
        ;
    if( n )		// keine Spaces
        rStr.Erase( 0, n );
    return rStr;
}


String& SwAutoFormat::DelTrailingBlanks( String& rStr ) const
{
    xub_StrLen nL = rStr.Len(), n = nL;
    if( !nL )
        return rStr;

    while( --n && IsSpace( rStr.GetChar( n ) )  )
        ;
    if( n+1 != nL )		// keine Spaces
        rStr.Erase( n+1 );
    return rStr;
}


xub_StrLen SwAutoFormat::GetLeadingBlanks( const String& rStr ) const
{
    xub_StrLen nL;
    xub_StrLen n;

    for( nL = rStr.Len(), n = 0; n < nL && IsSpace( rStr.GetChar( n ) ); ++n )
        ;
    return n;
}


xub_StrLen SwAutoFormat::GetTrailingBlanks( const String& rStr ) const
{
    xub_StrLen nL = rStr.Len(), n = nL;
    if( !nL )
        return 0;

    while( --n && IsSpace( rStr.GetChar( n ) )  )
        ;
    return ++n;
}


BOOL SwAutoFormat::IsFirstCharCapital( const SwTxtNode& rNd ) const
{
    const String& rTxt = rNd.GetTxt();
    for( xub_StrLen n = 0, nEnd = rTxt.Len(); n < nEnd; ++n )
        if( !IsSpace( rTxt.GetChar( n ) ) )
        {
            CharClass& rCC = GetCharClass( rNd.GetSwAttrSet().
                                        GetLanguage().GetLanguage() );
            sal_Int32 nCharType = rCC.getCharacterType( rTxt, n );
            return CharClass::isLetterType( nCharType ) &&
                   0 != ( i18n::KCharacterType::UPPER &
                                                    nCharType );
        }
    return FALSE;
}


USHORT SwAutoFormat::GetDigitLevel( const SwTxtNode& rNd, xub_StrLen& rPos,
        String* pPreFix, String* pPostFix, String* pNumTypes ) const
{
    // Teste auf 1.) / 1. / 1.1.1 / (1). / (1) / ....
    const String& rTxt = rNd.GetTxt();
    xub_StrLen nPos = rPos;
    int eScan = NONE;

    USHORT nStart = 0;
    BYTE nDigitLvl = 0, nDigitCnt = 0;
    //count number of parenthesis to assure a sensible order is found
    USHORT nOpeningParentheses = 0;
    USHORT nClosingParentheses = 0;

    CharClass& rCC = GetCharClass( rNd.GetSwAttrSet().GetLanguage().GetLanguage() );

    while( nPos < rTxt.Len() && nDigitLvl < MAXLEVEL - 1)
    {
        const sal_Unicode cCurrentChar = rTxt.GetChar( nPos );
        if( ('0' <= cCurrentChar &&  '9' >= cCurrentChar) ||
            (0xff10 <= cCurrentChar &&  0xff19 >= cCurrentChar) )
        {
            if( eScan & DELIM )
            {
                if( eScan & CHG )		// nicht wenns mit einer Zahl beginnt
                {
                    ++nDigitLvl;
                    if( pPostFix )
                        *pPostFix += (sal_Unicode)1;
                }

                if( pNumTypes )
                    *pNumTypes += (sal_Unicode)('0' + SVX_NUM_ARABIC);

                eScan = eScan | CHG;
            }
            else if( pNumTypes && !(eScan & DIGIT) )
                *pNumTypes += (sal_Unicode)('0' + SVX_NUM_ARABIC);

            eScan &= ~DELIM;		// Delim raus
            if( 0 != (eScan & ~CHG) && DIGIT != (eScan & ~CHG))
                return USHRT_MAX;

            eScan |= DIGIT;			// Digit rein
            if( 3 == ++nDigitCnt )	// mehr als 2 Nummern sind kein Enum mehr
                return USHRT_MAX;

            nStart *= 10;
            nStart += cCurrentChar <= '9' ? cCurrentChar - '0' : cCurrentChar - 0xff10;
        }
        else if( rCC.isAlpha( rTxt, nPos ) )
        {
            BOOL bIsUpper =
                0 != ( i18n::KCharacterType::UPPER &
                                        rCC.getCharacterType( rTxt, nPos ));
            sal_Unicode cLow = rCC.toLower( rTxt, nPos, 1 ).GetChar(0), cNumTyp;
            int eTmpScan;

            // roemische Zeichen sind "mdclxvi". Da man aber eher mal eine
            // Numerierung mit c oder d anfangen will, werden diese erstmal
            // zu chars und spaeter ggfs. zu romischen Zeichen!
//			if( strchr( "mdclxvi", cLow ))
#ifdef WITH_ALPHANUM_AS_NUMFMT
            //detection of 'c' and 'd' a ROMAN numbering should not be done here
            if( 256 > cLow  &&( (eScan & (LOWER_ROMAN|UPPER_ROMAN))
                                    ? strchr( "mdclxvi", cLow )
                                    : strchr( "mlxvi", cLow ) ))
#else
            if( 256 > cLow  && ( strchr( "mdclxvi", cLow ) ))
#endif
            {
                if( bIsUpper )
                    cNumTyp = '0' + SVX_NUM_ROMAN_UPPER, eTmpScan = UPPER_ROMAN;
                else
                    cNumTyp = '0' + SVX_NUM_ROMAN_LOWER, eTmpScan = LOWER_ROMAN;
            }
            else if( bIsUpper )
                cNumTyp = '0' + SVX_NUM_CHARS_UPPER_LETTER, eTmpScan = UPPER_ALPHA;
            else
                cNumTyp = '0' + SVX_NUM_CHARS_LOWER_LETTER, eTmpScan = LOWER_ALPHA;


            //ggfs. auf roemische Zeichen umschalten (nur bei c/d!)?
            if( 1 == nDigitCnt && ( eScan & (UPPER_ALPHA|LOWER_ALPHA) ) &&
                ( 3 == nStart || 4 == nStart) && 256 > cLow &&
                strchr( "mdclxvi", cLow ) &&
                (( eScan & UPPER_ALPHA ) ? (eTmpScan & (UPPER_ALPHA|UPPER_ROMAN))
                                         : (eTmpScan & (LOWER_ALPHA|LOWER_ROMAN))) )
            {
                sal_Unicode c = '0';
                nStart = 3 == nStart ? 100 : 500;
                if( UPPER_ALPHA == eTmpScan )
                    eTmpScan = UPPER_ROMAN, c += SVX_NUM_ROMAN_UPPER;
                else
                    eTmpScan = LOWER_ROMAN, c += SVX_NUM_ROMAN_LOWER;

                ( eScan &= ~(UPPER_ALPHA|LOWER_ALPHA)) |= eTmpScan;
                if( pNumTypes )
                    pNumTypes->SetChar( pNumTypes->Len() - 1, c );
            }

            if( eScan & DELIM )
            {
                if( eScan & CHG )		// nicht wenns mit einer Zahl beginnt
                {
                    ++nDigitLvl;
                    if( pPostFix )
                        *pPostFix += (sal_Unicode)1;
                }

                if( pNumTypes )
                    *pNumTypes += cNumTyp;
                eScan = eScan | CHG;
            }
            else if( pNumTypes && !(eScan & eTmpScan) )
                *pNumTypes += cNumTyp;

            eScan &= ~DELIM;		// Delim raus

            // falls ein andere Type gesetzt ist, brechen wir ab
            if( 0 != ( eScan & ~CHG ) && eTmpScan != ( eScan & ~CHG ))
                return USHRT_MAX;

            if( eTmpScan & (UPPER_ALPHA | LOWER_ALPHA) )
            {
                // Buchstaben nur zulassen, wenn sie einmalig vorkommen
                return USHRT_MAX;
            }
            else
            {
                // roemische Zahlen: checke ob das gueltige Zeichen sind
                USHORT nVal;
                BOOL bError = FALSE;
                switch( cLow )
                {
                case 'm':	nVal = 1000; goto CHECK_ROMAN_1;
                case 'd':	nVal =  500; goto CHECK_ROMAN_5;
                case 'c':	nVal =  100; goto CHECK_ROMAN_1;
                case 'l':	nVal =   50; goto CHECK_ROMAN_5;
                case 'x':	nVal =   10; goto CHECK_ROMAN_1;
                case 'v':	nVal =    5; goto CHECK_ROMAN_5;

CHECK_ROMAN_1:
                    {
                        int nMod5 = nStart % (nVal * 5);
                        int nLast = nStart % nVal;
                        int n10 = nVal / 10;

                        if( nMod5 == ((3 * nVal) + n10 ) ||
                            nMod5 == ((4 * nVal) + n10 ) ||
                            nLast == n10 )
                            nStart = static_cast<USHORT>(nStart + (n10 * 8));
                        else if( nMod5 == 0 ||
                                 nMod5 == (1 * nVal) ||
                                 nMod5 == (2 * nVal) )
                            nStart = nStart + nVal;
                        else
                            bError = TRUE;
                    }
                    break;

CHECK_ROMAN_5:
                    {
                        if( ( nStart / nVal ) & 1 )
                            bError = TRUE;
                        else
                        {
                            int nMod = nStart % nVal;
                            int n10 = nVal / 5;
                            if( n10 == nMod )
                                nStart = static_cast<USHORT>(nStart + (3 * n10));
                            else if( 0 == nMod )
                                nStart = nStart + nVal;
                            else
                                bError = TRUE;
                        }
                    }
                    break;

                case 'i':
                        if( nStart % 5 >= 3 )
                            bError = TRUE;
                        else
                            nStart += 1;
                        break;

                default:
                    bError = TRUE;
                }

                if( bError )
                    return USHRT_MAX;
            }
            eScan |= eTmpScan;			// Digit rein
            ++nDigitCnt;
        }
        else if( (256 > cCurrentChar &&
                 strchr( ".)(", cCurrentChar )) ||
                 0x3002 == cCurrentChar /* Chinese trad. dot */||
                 0xff0e == cCurrentChar /* Japanese dot */||
                 0xFF08 == cCurrentChar /* opening bracket Chin./Jap.*/||
                 0xFF09 == cCurrentChar )/* closing bracket Chin./Jap. */
        {
            if(cCurrentChar == '(' || cCurrentChar == 0xFF09)
                nOpeningParentheses++;
            else if(cCurrentChar == ')'|| cCurrentChar == 0xFF08)
                nClosingParentheses++;
            // nur wenn noch keine Zahlen gelesen wurden!
            if( pPreFix && !( eScan & ( NO_DELIM | CHG )) )
                *pPreFix += rTxt.GetChar( nPos );
            else if( pPostFix )
                *pPostFix += rTxt.GetChar( nPos );

            if( NO_DELIM & eScan )
            {
                eScan |= CHG;
                if( pPreFix )
                    (*pPreFix += (sal_Unicode)1)
                              += String::CreateFromInt32( nStart );
            }
            eScan &= ~NO_DELIM;		// Delim raus
            eScan |= DELIM;			// Digit rein
            nDigitCnt = 0;
            nStart = 0;
        }
        else
            break;
        ++nPos;
    }
    if( !( CHG & eScan ) || rPos == nPos ||
        nPos == rTxt.Len() || !IsSpace( rTxt.GetChar( nPos ) ) ||
        (nOpeningParentheses > nClosingParentheses))
        return USHRT_MAX;

    if( (NO_DELIM & eScan) && pPreFix )		// den letzen nicht vergessen
        (*pPreFix += (sal_Unicode)1) += String::CreateFromInt32( nStart );

    rPos = nPos;
    return nDigitLvl;		// 0 .. 9 (MAXLEVEL - 1)
}


void SwAutoFormat::SetColl( USHORT nId, BOOL bHdLineOrText )
{
    aDelPam.DeleteMark();
    aDelPam.GetPoint()->nNode = aNdIdx;
    aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, 0 );

    // behalte harte Tabs, Ausrichtung, Sprache, Silbentrennung,
    // DropCaps und fast alle Frame-Attribute
    SfxItemSet aSet( pDoc->GetAttrPool(),
                        RES_PARATR_ADJUST, RES_PARATR_ADJUST,
                        RES_PARATR_TABSTOP, RES_PARATR_DROP,
                        RES_CHRATR_LANGUAGE, RES_CHRATR_LANGUAGE,
                        RES_BACKGROUND, RES_SHADOW,
                        0 );

    if( pAktTxtNd->HasSwAttrSet() )
    {
        aSet.Put( *pAktTxtNd->GetpSwAttrSet() );
        // einige Sonderbedingungen:
        // HeaderLine/Textkoerper: nur zentriert oder rechts mitnehmem
        // sonst nur den Blocksatz
        SvxAdjustItem* pAdj;
        if( SFX_ITEM_SET == aSet.GetItemState( RES_PARATR_ADJUST,
                        FALSE, (const SfxPoolItem**)&pAdj ))
        {
            SvxAdjust eAdj = pAdj->GetAdjust();
            if( bHdLineOrText ? (SVX_ADJUST_RIGHT != eAdj &&
                                 SVX_ADJUST_CENTER != eAdj)
                              : SVX_ADJUST_BLOCK != eAdj )
                aSet.ClearItem( RES_PARATR_ADJUST );
        }
    }

    pDoc->SetTxtFmtCollByAutoFmt( *aDelPam.GetPoint(), nId, &aSet );
}


BOOL SwAutoFormat::HasSelBlanks( SwPaM& rPam ) const
{
    // noch ein Blank am Anfang oder Ende ?
    // nicht loeschen, wird wieder eingefuegt.
    SwPosition * pPos = rPam.End();
    xub_StrLen nBlnkPos = pPos->nContent.GetIndex();
    SwTxtNode* pTxtNd = pPos->nNode.GetNode().GetTxtNode();
    if( nBlnkPos && nBlnkPos-- < pTxtNd->GetTxt().Len() &&
        ( ' ' == pTxtNd->GetTxt().GetChar( nBlnkPos ) ))
// JP 23.08.95: keine Tabs stehen lassen, diese in Blanks wandeln
//        ( ' ' == ( cCh = pTxtNd->GetTxt()[ nBlnkPos ] ) || '\t' == cCh ))
        pPos->nContent--;
    else
    {
        pPos = rPam.GetPoint() == pPos ? rPam.GetMark() : rPam.GetPoint();
        nBlnkPos = pPos->nContent.GetIndex();
        pTxtNd = pPos->nNode.GetNode().GetTxtNode();
        if( nBlnkPos < pTxtNd->GetTxt().Len() &&
            ( ' ' == pTxtNd->GetTxt().GetChar( nBlnkPos )))
// JP 23.08.95: keine Tabs stehen lassen, diese in Blanks wandeln
//            ( ' ' == ( cCh = pTxtNd->GetTxt()[ nBlnkPos ] ) || '\t' == cCh ))
            pPos->nContent++;
        else
            return FALSE;
    }
    return TRUE;
}


BOOL SwAutoFormat::HasBreakAttr( const SwTxtNode& rTxtNd ) const
{
    const SfxItemSet* pSet = rTxtNd.GetpSwAttrSet();
    if( !pSet )
        return FALSE;

    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == pSet->GetItemState( RES_BREAK, FALSE, &pItem )
        && SVX_BREAK_NONE != ((SvxFmtBreakItem*)pItem)->GetBreak() )
        return TRUE;

    if( SFX_ITEM_SET == pSet->GetItemState( RES_PAGEDESC, FALSE, &pItem )
        && ((SwFmtPageDesc*)pItem)->GetPageDesc()
        && nsUseOnPage::PD_NONE != ((SwFmtPageDesc*)pItem)->GetPageDesc()->GetUseOn() )
        return TRUE;
    return FALSE;
}


// ist ein Punkt am Ende ??
BOOL SwAutoFormat::IsSentenceAtEnd( const SwTxtNode& rTxtNd ) const
{
    const String& rStr = rTxtNd.GetTxt();
    xub_StrLen n = rStr.Len();
    if( !n )
        return TRUE;

    while( --n && IsSpace( rStr.GetChar( n  ) ) )
        ;
    return '.' == rStr.GetChar( n );
}


// loesche im Node Anfang oder/und Ende
void SwAutoFormat::DeleteAktPara( BOOL bStart, BOOL bEnd )
{
    if( aFlags.bAFmtByInput
        ? aFlags.bAFmtByInpDelSpacesAtSttEnd
        : aFlags.bAFmtDelSpacesAtSttEnd )
    {
        // Loesche Blanks am Ende vom akt. und am Anfang vom naechsten
        aDelPam.DeleteMark();
        aDelPam.GetPoint()->nNode = aNdIdx;
        xub_StrLen nPos;
        if( bStart && 0 != ( nPos = GetLeadingBlanks( pAktTxtNd->GetTxt() )))
        {
            aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, 0 );
            aDelPam.SetMark();
            aDelPam.GetPoint()->nContent = nPos;
            DeleteSel( aDelPam );
            aDelPam.DeleteMark();
        }
        if( bEnd && pAktTxtNd->GetTxt().Len() !=
                    ( nPos = GetTrailingBlanks( pAktTxtNd->GetTxt() )) )
        {
            aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, pAktTxtNd->GetTxt().Len() );
            aDelPam.SetMark();
            aDelPam.GetPoint()->nContent = nPos;
            DeleteSel( aDelPam );
            aDelPam.DeleteMark();
        }
    }
}

void SwAutoFormat::DeleteSel( SwPaM& rDelPam )
{
    if( aFlags.bWithRedlining )
    {
        // damit der DelPam auch verschoben wird, in den Shell-Cursr-Ring
        // mit aufnehmen !!
        SwPaM* pShCrsr = pEditShell->_GetCrsr();
        SwPaM aTmp( *pAktTxtNd, 0, pShCrsr );

        Ring *pPrev = rDelPam.GetPrev();
        rDelPam.MoveRingTo( pShCrsr );

        pEditShell->DeleteSel( rDelPam );

        // und den Pam wieder herausnehmen:
        Ring *p, *pNext = (Ring*)&rDelPam;
        do {
            p = pNext;
            pNext = p->GetNext();
            p->MoveTo( &rDelPam );
        } while( p != pPrev );

        aNdIdx = aTmp.GetPoint()->nNode;
        pAktTxtNd = aNdIdx.GetNode().GetTxtNode();
    }
    else
        pEditShell->DeleteSel( rDelPam );
}

BOOL SwAutoFormat::DeleteAktNxtPara( const String& rNxtPara )
{
    // Loesche Blanks am Ende vom akt. und am Anfang vom naechsten
    aDelPam.DeleteMark();
    aDelPam.GetPoint()->nNode = aNdIdx;
    aDelPam.GetPoint()->nContent.Assign( pAktTxtNd,
                    GetTrailingBlanks( pAktTxtNd->GetTxt() ) );
    aDelPam.SetMark();

    aDelPam.GetPoint()->nNode++;
    SwTxtNode* pTNd = aDelPam.GetNode()->GetTxtNode();
    if( !pTNd )
    {
        // dann nur bis zum Ende von Absatz loeschen
        aDelPam.GetPoint()->nNode--;
        aDelPam.GetPoint()->nContent = pAktTxtNd->GetTxt().Len();
    }
    else
        aDelPam.GetPoint()->nContent.Assign( pTNd,
                            GetLeadingBlanks( rNxtPara ));

    // noch ein Blank am Anfang oder Ende ?
    // nicht loeschen, wird wieder eingefuegt.
    BOOL bHasBlnks = HasSelBlanks( aDelPam );

    if( *aDelPam.GetPoint() != *aDelPam.GetMark() )
        DeleteSel( aDelPam );
    aDelPam.DeleteMark();

    return !bHasBlnks;
}


void SwAutoFormat::DelEmptyLine( BOOL bTstNextPara )
{
    SetRedlineTxt( STR_AUTOFMTREDL_DEL_EMPTY_PARA );
    // Loesche Blanks den leeren Absatz
    aDelPam.DeleteMark();
    aDelPam.GetPoint()->nNode = aNdIdx;
    aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, pAktTxtNd->GetTxt().Len() );
    aDelPam.SetMark();

    aDelPam.GetMark()->nNode--;
    SwTxtNode* pTNd = aDelPam.GetNode( FALSE )->GetTxtNode();
    if( pTNd )
        // erstmal den vorherigen Textnode benutzen.
        aDelPam.GetMark()->nContent.Assign( pTNd, pTNd->GetTxt().Len() );
    else if( bTstNextPara )
    {
        // dann versuche den naechsten (am Anfang vom Dok, Tabellen-Zellen,
        // Rahmen, ...
        aDelPam.GetMark()->nNode += 2;
        pTNd = aDelPam.GetNode( FALSE )->GetTxtNode();
        if( pTNd )
        {
            aDelPam.GetMark()->nContent.Assign( pTNd, 0 );
            aDelPam.GetPoint()->nContent = 0;
        }
    }
    else
    {
        aDelPam.GetMark()->nNode = aNdIdx;
        aDelPam.GetMark()->nContent = 0;
        pTNd = pAktTxtNd;
    }
    if( pTNd )
        DeleteSel( aDelPam );

    aDelPam.DeleteMark();
    ClearRedlineTxt();
}


void SwAutoFormat::DelMoreLinesBlanks( BOOL bWithLineBreaks )
{
    if( aFlags.bAFmtByInput
        ? aFlags.bAFmtByInpDelSpacesBetweenLines
        : aFlags.bAFmtDelSpacesBetweenLines	)
    {
        // loesche alle "Blanks" Links und Rechts vom Einzug
        aDelPam.DeleteMark();
        aDelPam.GetPoint()->nNode = aNdIdx;
        aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, 0 );

        SwTxtFrmInfo aFInfo( pAktTxtFrm );
        aFInfo.GetSpaces( aDelPam, !aFlags.bAFmtByInput || bWithLineBreaks );

        SwPaM* pNxt;
        do {
            pNxt = (SwPaM*)aDelPam.GetNext();
            if( pNxt->HasMark() && *pNxt->GetPoint() != *pNxt->GetMark() )
            {
                BOOL bHasBlnks = HasSelBlanks( *pNxt );
                DeleteSel( *pNxt );
                if( !bHasBlnks )
                {
                    pDoc->InsertString( *pNxt, sal_Unicode(' ') );
                }
            }

            if( pNxt == &aDelPam )
                break;
            delete pNxt;
        } while( TRUE );

        aDelPam.DeleteMark();
    }
}


        // loesche den vorherigen Absatz
void SwAutoFormat::DelPrevPara()
{
    aDelPam.DeleteMark();
    aDelPam.GetPoint()->nNode = aNdIdx;
    aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, 0 );
    aDelPam.SetMark();

    aDelPam.GetPoint()->nNode--;
    SwTxtNode* pTNd = aDelPam.GetNode()->GetTxtNode();
    if( pTNd )
    {
        // erstmal den vorherigen Textnode benutzen.
        aDelPam.GetPoint()->nContent.Assign( pTNd, pTNd->GetTxt().Len() );
        DeleteSel( aDelPam );
    }
    aDelPam.DeleteMark();
}


void SwAutoFormat::BuildIndent()
{
    SetRedlineTxt( STR_AUTOFMTREDL_SET_TMPL_INDENT );

    // lese alle nachfolgenden Absaetze die zu diesem Einzug gehoeren
    BOOL bBreak = TRUE;
    if( bMoreLines )
        DelMoreLinesBlanks( TRUE );
    else
        bBreak = !IsFastFullLine( *pAktTxtNd ) ||
                IsBlanksInString( *pAktTxtNd ) ||
                IsSentenceAtEnd( *pAktTxtNd );
    SetColl( RES_POOLCOLL_TEXT_IDENT );
    if( !bBreak )
    {
        SetRedlineTxt( STR_AUTOFMTREDL_DEL_MORELINES );
        const SwTxtNode* pNxtNd = GetNextNode();
        if( pNxtNd && !bEnde )
        {
            do {
                bBreak = !IsFastFullLine( *pNxtNd ) ||
                        IsBlanksInString( *pNxtNd ) ||
                        IsSentenceAtEnd( *pNxtNd );
                if( DeleteAktNxtPara( pNxtNd->GetTxt() ))
                {
                    pDoc->InsertString( aDelPam, sal_Unicode(' ') );
                }
                if( bBreak )
                    break;
                pNxtNd = GetNextNode();
            } while( CanJoin( pNxtNd ) &&
                    !CalcLevel( *pNxtNd ) );
        }
    }
    DeleteAktPara( TRUE, TRUE );
    AutoCorrect();
}


void SwAutoFormat::BuildTextIndent()
{
    SetRedlineTxt( STR_AUTOFMTREDL_SET_TMPL_TEXT_INDENT);
    // lese alle nachfolgenden Absaetze die zu diesem Einzug gehoeren
    BOOL bBreak = TRUE;
    if( bMoreLines )
        DelMoreLinesBlanks( TRUE );
    else
        bBreak = !IsFastFullLine( *pAktTxtNd ) ||
                    IsBlanksInString( *pAktTxtNd ) ||
                    IsSentenceAtEnd( *pAktTxtNd );

    if( aFlags.bAFmtByInput )
        pAktTxtNd->SetAutoFmtLvl( (BYTE)CalcLevel( *pAktTxtNd ) );

    SetColl( RES_POOLCOLL_TEXT_MOVE );
    if( !bBreak )
    {
        SetRedlineTxt( STR_AUTOFMTREDL_DEL_MORELINES );
        const SwTxtNode* pNxtNd = GetNextNode();
        while(	CanJoin( pNxtNd ) &&
                CalcLevel( *pNxtNd ) )
        {
            bBreak = !IsFastFullLine( *pNxtNd ) || IsBlanksInString( *pNxtNd ) ||
                    IsSentenceAtEnd( *pNxtNd );
            if( DeleteAktNxtPara( pNxtNd->GetTxt() ) )
            {
                pDoc->InsertString( aDelPam, sal_Unicode(' ') );
            }
            if( bBreak )
                break;
            pNxtNd = GetNextNode();
        }
    }
    DeleteAktPara( TRUE, TRUE );
    AutoCorrect();
}


void SwAutoFormat::BuildText()
{
    SetRedlineTxt( STR_AUTOFMTREDL_SET_TMPL_TEXT );
    // lese alle nachfolgenden Absaetze die zu diesem Text
    // ohne Einzug gehoeren
    BOOL bBreak = TRUE;
    if( bMoreLines )
        DelMoreLinesBlanks();
    else
        bBreak = !IsFastFullLine( *pAktTxtNd ) ||
                    IsBlanksInString( *pAktTxtNd ) ||
                    IsSentenceAtEnd( *pAktTxtNd );
    SetColl( RES_POOLCOLL_TEXT, TRUE );
    if( !bBreak )
    {
        SetRedlineTxt( STR_AUTOFMTREDL_DEL_MORELINES );
        const SwTxtNode* pNxtNd = GetNextNode();
        while(	CanJoin( pNxtNd ) &&
                !CalcLevel( *pNxtNd ) )
        {
            bBreak = !IsFastFullLine( *pNxtNd ) || IsBlanksInString( *pNxtNd ) ||
                    IsSentenceAtEnd( *pNxtNd );
            if( DeleteAktNxtPara( pNxtNd->GetTxt() ) )
            {
                pDoc->InsertString( aDelPam, sal_Unicode(' ') );
            }
            if( bBreak )
                break;
            const SwTxtNode* pCurrNode = pNxtNd;
            pNxtNd = GetNextNode();
            if(!pNxtNd || pCurrNode == pNxtNd)
                break;
        }
    }
    DeleteAktPara( TRUE, TRUE );
    AutoCorrect();
}


void SwAutoFormat::BuildEnum( USHORT nLvl, USHORT nDigitLevel )
{
    SetRedlineTxt( STR_AUTOFMTREDL_SET_NUMBULET );

    BOOL bBreak = TRUE;

    // als erstes den akt. Einzug bestimmen und die Framebreite bestimmen
    SwTwips nFrmWidth = pAktTxtFrm->Prt().Width();;
    SwTwips nLeftTxtPos;
    {
        const sal_Unicode* pTxt = pAktTxtNd->GetTxt().GetBuffer(), *pSav = pTxt;
        while( IsSpace( *pTxt ) )
            ++pTxt;

        SwTxtFrmInfo aInfo( pAktTxtFrm );
        nLeftTxtPos = aInfo.GetCharPos( static_cast<xub_StrLen>(pTxt - pSav) );
        nLeftTxtPos -= pAktTxtNd->GetSwAttrSet().GetLRSpace().GetLeft();
    }

    if( bMoreLines )
        DelMoreLinesBlanks();
    else
        bBreak = !IsFastFullLine( *pAktTxtNd ) ||
                    IsBlanksInString( *pAktTxtNd ) ||
                    IsSentenceAtEnd( *pAktTxtNd );
    sal_Bool bRTL = pEditShell->IsInRightToLeftText();
//	SetColl( RES_POOLCOLL_NUM_LEVEL1 + ( nLvl * 4 ) );
    DeleteAktPara( TRUE, TRUE );

    BOOL bChgBullet = FALSE, bChgEnum = FALSE;
    xub_StrLen nAutoCorrPos = 0;

    // falls die Numerierung gesetzt werden, die akt. besorgen
    // --> OD 2008-02-11 #newlistlevelattrs#
    SwNumRule aRule( pDoc->GetUniqueNumRuleName(),
                     // --> OD 2008-06-06 #i89178#
                     numfunc::GetDefaultPositionAndSpaceMode() );
                     // <--
    // <--
    const SwNumRule* pCur = 0;
    if( aFlags.bSetNumRule && 0 != (pCur = pAktTxtNd->GetNumRule()) )
        aRule = *pCur;

    // ersetze das Bullet-Zeichen mit dem definiertem
    const String& rStr = pAktTxtNd->GetTxt();
    xub_StrLen nTxtStt = 0, nOrigTxtStt = 0;
    const sal_Unicode* pFndBulletChr;
//	if( aFlags.bAFmtByInput ? aFlags.bSetNumRule : aFlags.bChgEnumNum &&
    if( aFlags.bChgEnumNum &&
        2 < rStr.Len() &&
        0 != ( pFndBulletChr = StrChr( pBulletChar, rStr.GetChar( nTxtStt ) ))
        && IsSpace( rStr.GetChar( nTxtStt + 1 ) ) )
    {
        if( aFlags.bAFmtByInput )
        {
            if( aFlags.bSetNumRule )
            {
                SwCharFmt* pCFmt = pDoc->GetCharFmtFromPool(
                                            RES_POOLCHR_BUL_LEVEL );
                bChgBullet = TRUE;
                // wurde das Format schon mal angepasst?
                if( !aRule.GetNumFmt( nLvl ) )
                {
                    int nBulletPos = pFndBulletChr - pBulletChar;
                    sal_Unicode cBullChar;
                    const Font* pBullFnt( 0 );
                    if( nBulletPos < cnPosEnDash )
                    {
                        cBullChar = aFlags.cBullet;
                        pBullFnt = &aFlags.aBulletFont;
                    }
                    else
                    {
                        cBullChar = nBulletPos < cnPosEmDash
                                        ? cStarSymbolEnDash
                                        : cStarSymbolEmDash;
                        // --> OD 2008-06-03 #i63395#
                        // Only apply user defined default bullet font
                        if ( numfunc::IsDefBulletFontUserDefined() )
                        {
                            pBullFnt = &numfunc::GetDefBulletFont();
                        }
                        // <--
                    }

                    USHORT nAbsPos = lBullIndent;
                    USHORT nSpaceSteps = nLvl
                                            ? USHORT(nLeftTxtPos / nLvl)
                                            : lBullIndent;
                    for( BYTE n = 0; n < MAXLEVEL; ++n, nAbsPos = nAbsPos + nSpaceSteps )
                    {
                        SwNumFmt aFmt( aRule.Get( n ) );
                        aFmt.SetBulletFont( pBullFnt );
                        aFmt.SetBulletChar( cBullChar );
                        aFmt.SetNumberingType(SVX_NUM_CHAR_SPECIAL);
                        aFmt.SetFirstLineOffset( lBullFirstLineOffset );
                        aFmt.SetAbsLSpace( nAbsPos );
                        if( !aFmt.GetCharFmt() )
                            aFmt.SetCharFmt( pCFmt );
                        if( bRTL )
                            aFmt.SetNumAdjust( SVX_ADJUST_RIGHT );

                        aRule.Set( n, aFmt );

                        if( n == nLvl &&
                            nFrmWidth < ( nSpaceSteps * MAXLEVEL ) )
                            nSpaceSteps = static_cast<USHORT>(( nFrmWidth - nLeftTxtPos ) /
                                                ( MAXLEVEL - nLvl ));
                    }
                }
            }
        }
        else
        {
            bChgBullet = TRUE;
            SetColl( static_cast<USHORT>(RES_POOLCOLL_BUL_LEVEL1 + ( Min( nLvl, cnNumBullColls ) * 4 )) );
        }
    }
    else
    {
        // dann ist das eine Nummerierung

        //JP 21.11.97: Der NumLevel wird entweder der DigitLevel oder
        // 				wenn der nicht vorhanden oder 0 ist, durch den
        //				(Einrueckungs-)Level.

        String aPostFix, aPreFix, aNumTypes;
        if( USHRT_MAX != ( nDigitLevel = GetDigitLevel( *pAktTxtNd, nTxtStt,
                                        &aPreFix, &aPostFix, &aNumTypes )) )
        {
            bChgEnum = TRUE;

            // Ebene 0 und Einrueckung dann wird die Ebene durch den linken
            // Einzug und der default NumEinrueckung bestimmt.
            if( !nDigitLevel && nLeftTxtPos )
                nLvl = Min( USHORT( nLeftTxtPos / lNumIndent ),
                            USHORT( MAXLEVEL - 1 ) );
            else
                nLvl = nDigitLevel;
        }

        if( bChgEnum && aFlags.bSetNumRule )
        {
            if( !pCur )			// NumRule anpassen, wenn sie neu ist
            {
                SwCharFmt* pCFmt = pDoc->GetCharFmtFromPool(
                                            RES_POOLCHR_NUM_LEVEL );
                if( !nDigitLevel )
                {
                    SwNumFmt aFmt( aRule.Get( nLvl ) );
                    aFmt.SetStart( static_cast<USHORT>(aPreFix.GetToken( 1,
                                            (sal_Unicode)1 ).ToInt32()));
                    aFmt.SetPrefix( aPreFix.GetToken( 0, (sal_Unicode)1 ));
                    aFmt.SetSuffix( aPostFix.GetToken( 0, (sal_Unicode)1 ));
                    aFmt.SetIncludeUpperLevels( 0 );

                    if( !aFmt.GetCharFmt() )
                        aFmt.SetCharFmt( pCFmt );

                    if( aNumTypes.Len() )
                        aFmt.SetNumberingType(aNumTypes.GetChar( 0 ) - '0');

                    if( bRTL )
                        aFmt.SetNumAdjust( SVX_ADJUST_RIGHT );
                    aRule.Set( nLvl, aFmt );
                }
                else
                {
                    USHORT nSpaceSteps = nLvl ? USHORT(nLeftTxtPos / nLvl) : 0;
                    BYTE n;
                    for( n = 0; n <= nLvl; ++n )
                    {
                        SwNumFmt aFmt( aRule.Get( n ) );

                        aFmt.SetStart( static_cast<USHORT>(aPreFix.GetToken( n+1,
                                                    (sal_Unicode)1 ).ToInt32() ));
                        if( !n )
                            aFmt.SetPrefix( aPreFix.GetToken( n, (sal_Unicode)1 ));
                        aFmt.SetSuffix( aPostFix.GetToken( n, (sal_Unicode)1 ));
                        aFmt.SetIncludeUpperLevels( MAXLEVEL );
                        if( n < aNumTypes.Len() )
                            aFmt.SetNumberingType((aNumTypes.GetChar( n ) - '0'));

                        aFmt.SetAbsLSpace( USHORT( nSpaceSteps * n )
                                            + lNumIndent );

                        if( !aFmt.GetCharFmt() )
                            aFmt.SetCharFmt( pCFmt );
                        if( bRTL )
                            aFmt.SetNumAdjust( SVX_ADJUST_RIGHT );

                        aRule.Set( n, aFmt );
                    }

                    // passt alles vollstaendig in den Frame?
                    BOOL bDefStep = nFrmWidth < (nSpaceSteps * MAXLEVEL);
                    for( ; n < MAXLEVEL; ++n )
                    {
                        SwNumFmt aFmt( aRule.Get( n ) );
                        aFmt.SetIncludeUpperLevels( MAXLEVEL );
                        if( bDefStep )
                            aFmt.SetAbsLSpace( USHORT( (nLeftTxtPos +
                                SwNumRule::GetNumIndent(static_cast<BYTE>(n-nLvl)))));
                        else
                            aFmt.SetAbsLSpace( USHORT( nSpaceSteps * n )
                                                + lNumIndent );
                        aRule.Set( n, aFmt );
                    }
                }
            }
        }
        else if( !aFlags.bAFmtByInput )
            SetColl( static_cast<USHORT>(RES_POOLCOLL_NUM_LEVEL1 + ( Min( nLvl, cnNumBullColls ) * 4 ) ));
        else
            bChgEnum = FALSE;
    }

    if( bChgEnum || bChgBullet )
    {
        aDelPam.DeleteMark();
        aDelPam.GetPoint()->nNode = aNdIdx;

        if( aFlags.bSetNumRule )
        {
            if( aFlags.bAFmtByInput )
            {
                aDelPam.SetMark();
                aDelPam.GetMark()->nNode++;
                aDelPam.GetNode(FALSE)->GetTxtNode()->SetAttrListLevel( nLvl );
            }

            pAktTxtNd->SetAttrListLevel(nLvl);
            pAktTxtNd->SetNumLSpace( TRUE );

            // --> OD 2008-03-17 #refactorlists#
            // start new list
            pDoc->SetNumRule( aDelPam, aRule, true );
            // <--
            aDelPam.DeleteMark();

            aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, 0 );
        }
        else
            aDelPam.GetPoint()->nContent.Assign( pAktTxtNd,
                        bChgEnum ? (nTxtStt - nOrigTxtStt) : 0 );
        aDelPam.SetMark();

        if( bChgBullet )
            nTxtStt += 2;

        while( nTxtStt < rStr.Len() && IsSpace( rStr.GetChar( nTxtStt ) ))
            nTxtStt++;

        aDelPam.GetPoint()->nContent = nTxtStt - nOrigTxtStt;
        DeleteSel( aDelPam );

        if( !aFlags.bSetNumRule )
        {
            String sChgStr( '\t' );
            if( bChgBullet )
                sChgStr.Insert( aFlags.cBullet, 0 );
            pDoc->InsertString( aDelPam, sChgStr );

            SfxItemSet aSet( pDoc->GetAttrPool(), aTxtNodeSetRange );
            if( bChgBullet )
            {
                aDelPam.GetPoint()->nContent = 0;
                aDelPam.SetMark();
                aDelPam.GetMark()->nContent = 1;
                SetAllScriptItem( aSet,
                     SvxFontItem( aFlags.aBulletFont.GetFamily(),
                                  aFlags.aBulletFont.GetName(),
                                  aFlags.aBulletFont.GetStyleName(),
                                  aFlags.aBulletFont.GetPitch(),
                                  aFlags.aBulletFont.GetCharSet(),
                                  RES_CHRATR_FONT ) );
                pDoc->SetFmtItemByAutoFmt( aDelPam, aSet );
                aDelPam.DeleteMark();
                nAutoCorrPos = 2;
                aSet.ClearItem();
            }
            SvxTabStopItem aTStops( RES_PARATR_TABSTOP );    aTStops.Insert( SvxTabStop( 0 ));
            aSet.Put( aTStops );
            pDoc->SetFmtItemByAutoFmt( aDelPam, aSet );
        }
    }

    if( bBreak )
    {
        AutoCorrect( nAutoCorrPos );	   /* Offset wegen Bullet + Tab */
        return;
    }

    const SwTxtNode* pNxtNd = GetNextNode();
    while( CanJoin( pNxtNd ) &&
            nLvl == CalcLevel( *pNxtNd ) )
    {
        SetRedlineTxt( STR_AUTOFMTREDL_DEL_MORELINES );
        bBreak = !IsFastFullLine( *pNxtNd ) || IsBlanksInString( *pNxtNd ) ||
                IsSentenceAtEnd( *pNxtNd );
        if( DeleteAktNxtPara( pNxtNd->GetTxt() ) )
        {
            pDoc->InsertString( aDelPam, sal_Unicode(' ') );
        }
        if( bBreak )
            break;
        const SwTxtNode* pCurrNode = pNxtNd;
        pNxtNd = GetNextNode();
        if(!pNxtNd || pCurrNode == pNxtNd)
            break;
    }
    DeleteAktPara( FALSE, TRUE );
    AutoCorrect( nAutoCorrPos );
}


void SwAutoFormat::BuildNegIndent( SwTwips nSpaces )
{
    SetRedlineTxt( STR_AUTOFMTREDL_SET_TMPL_NEG_INDENT );
    // Test auf Gegenueberstellung:
    // (n Worte, durch Space/Tabs getrennt, mit gleicher
    //   Einrueckung in der 2.Zeile)

    // lese alle nachfolgenden Absaetze die zu dieser Aufzaehlung gehoeren
    BOOL bBreak = TRUE;
    xub_StrLen nSpacePos, nTxtPos = GetBigIndent( nSpacePos );
    if( bMoreLines )
        DelMoreLinesBlanks( TRUE );
    else
        bBreak = !IsFastFullLine( *pAktTxtNd ) ||
                    ( !nTxtPos && IsBlanksInString( *pAktTxtNd )) ||
                    IsSentenceAtEnd( *pAktTxtNd );

    SetColl( static_cast<USHORT>( nTxtPos
                ? RES_POOLCOLL_CONFRONTATION
                : RES_POOLCOLL_TEXT_NEGIDENT ) );

    if( nTxtPos )
    {
        const String& rStr = pAktTxtNd->GetTxt();
        BOOL bInsTab = TRUE;

        if( '\t' == rStr.GetChar( nSpacePos+1 ))       // ein Tab, das belassen wir
        {
            --nSpacePos;
            bInsTab = FALSE;
        }

        xub_StrLen nSpaceStt = nSpacePos;
        while( nSpaceStt && IsSpace( rStr.GetChar( --nSpaceStt ) ) )
            ;
        ++nSpaceStt;

        if( bInsTab && '\t' == rStr.GetChar( nSpaceStt ) )		// ein Tab, das belassen wir
        {
            ++nSpaceStt;
            bInsTab = FALSE;
        }


        aDelPam.DeleteMark();
        aDelPam.GetPoint()->nNode = aNdIdx;
        aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, nSpacePos );

        // alten Spaces, usw. loeschen
        if( nSpaceStt < nSpacePos )
        {
            aDelPam.SetMark();
            aDelPam.GetMark()->nContent = nSpaceStt;
            DeleteSel( aDelPam );
            if( bInsTab )
            {
                pDoc->InsertString( aDelPam, sal_Unicode('\t') );
            }
        }
    }

    if( !bBreak )
    {
        SetRedlineTxt( STR_AUTOFMTREDL_DEL_MORELINES );
        SwTxtFrmInfo aFInfo( pAktTxtFrm );
        const SwTxtNode* pNxtNd = GetNextNode();
        while(	CanJoin( pNxtNd ) &&
                20 < Abs( (long)(nSpaces - aFInfo.SetFrm(
                                GetFrm( *pNxtNd ) ).GetLineStart() ))
            )
        {
            bBreak = !IsFastFullLine( *pNxtNd ) ||
                    IsBlanksInString( *pNxtNd ) ||
                    IsSentenceAtEnd( *pNxtNd );
            if( DeleteAktNxtPara( pNxtNd->GetTxt() ) )
            {
                pDoc->InsertString( aDelPam, sal_Unicode(' ') );
            }
            if( bBreak )
                break;
            pNxtNd = GetNextNode();
        }
    }
    DeleteAktPara( TRUE, TRUE );
    AutoCorrect();
}


void SwAutoFormat::BuildHeadLine( USHORT nLvl )
{
    if( aFlags.bWithRedlining )
    {
        String sTxt( *ViewShell::GetShellRes()->GetAutoFmtNameLst()[
                                    STR_AUTOFMTREDL_SET_TMPL_HEADLINE ] );
        sTxt.SearchAndReplace( String::CreateFromAscii(
                                    RTL_CONSTASCII_STRINGPARAM( "$(ARG1)" )),
                                String::CreateFromInt32( nLvl + 1 ) );
        pDoc->SetAutoFmtRedlineComment( &sTxt );
    }

    SetColl( static_cast<USHORT>(RES_POOLCOLL_HEADLINE1 + nLvl ), TRUE );
    if( aFlags.bAFmtByInput )
    {
        SwTxtFmtColl& rNxtColl = pAktTxtNd->GetTxtColl()->GetNextTxtFmtColl();

        DelPrevPara();

        DeleteAktPara( TRUE, FALSE );
        DeleteAktNxtPara( aEmptyStr );

        aDelPam.DeleteMark();
        aDelPam.GetPoint()->nNode = aNdIdx.GetIndex() + 1;
        aDelPam.GetPoint()->nContent.Assign( aDelPam.GetCntntNode(), 0 );
        pDoc->SetTxtFmtColl( aDelPam, &rNxtColl );
    }
    else
    {
        DeleteAktPara( TRUE, TRUE );
        AutoCorrect();
    }
}


        // dann lasse doch mal das AutoCorrect auf den akt. TextNode los
void SwAutoFormat::AutoCorrect( xub_StrLen nPos )
{
    SvxAutoCorrect* pATst = SvxAutoCorrCfg::Get()->GetAutoCorrect();
    long aSvxFlags = pATst->GetFlags( );
    bool bReplaceQuote = ( aSvxFlags & ChgQuotes ) > 0;
    bool bReplaceSglQuote = ( aSvxFlags & ChgSglQuotes ) > 0;

    if( aFlags.bAFmtByInput ||
        (!aFlags.bAutoCorrect && !bReplaceQuote && !bReplaceSglQuote &&
        !aFlags.bCptlSttSntnc && !aFlags.bCptlSttWrd &&
        !aFlags.bChgOrdinalNumber &&
        !aFlags.bChgToEnEmDash && !aFlags.bSetINetAttr &&
        !aFlags.bChgWeightUnderl && !aFlags.bAddNonBrkSpace) )
        return;

    const String* pTxt = &pAktTxtNd->GetTxt();
    if( nPos >= pTxt->Len() )
        return;

    BOOL bGetLanguage = aFlags.bChgOrdinalNumber ||
                        aFlags.bChgToEnEmDash || aFlags.bSetINetAttr ||
                        aFlags.bCptlSttWrd || aFlags.bCptlSttSntnc ||
                        aFlags.bAddNonBrkSpace;


    aDelPam.DeleteMark();
    aDelPam.GetPoint()->nNode = aNdIdx;
    aDelPam.GetPoint()->nContent.Assign( pAktTxtNd, 0 );

    SwAutoCorrDoc aACorrDoc( *pEditShell, aDelPam );

    SwTxtFrmInfo aFInfo( 0 );

    xub_StrLen nSttPos, nLastBlank = nPos;
    BOOL bFirst = aFlags.bCptlSttSntnc, bFirstSent = bFirst;
    sal_Unicode cChar = 0;

    CharClass& rAppCC = GetAppCharClass();

    do {
        while( nPos < pTxt->Len() && IsSpace( cChar = pTxt->GetChar( nPos ) ))
            ++nPos;
        if( nPos == pTxt->Len() )
            break;		// das wars

        if( ( ( bReplaceQuote && '\"' == cChar ) || 
              ( bReplaceSglQuote && '\'' == cChar ) ) &&
            ( !nPos || ' ' == pTxt->GetChar( nPos-1 ) ) )
        {
            // --------------------------------------
            // beachte: Sonderfall Symbolfonts !!!
            if( !aFInfo.GetFrm() )
                aFInfo.SetFrm( GetFrm( *pAktTxtNd ) );
            if( !aFInfo.IsBullet( nPos ))
            {
                SetRedlineTxt( STR_AUTOFMTREDL_TYPO );
                aDelPam.GetPoint()->nContent = nPos;
                BOOL bSetHardBlank = FALSE;

                String sReplace( pATst->GetQuote( aACorrDoc,
                                    nPos, cChar, TRUE ));

                aDelPam.SetMark();
                aDelPam.GetPoint()->nContent = nPos+1;
                if( 2 == sReplace.Len() && ' ' == sReplace.GetChar( 1 ))
                {
                    sReplace.Erase( 1 );
                    bSetHardBlank = TRUE;
                }
                pDoc->ReplaceRange( aDelPam, sReplace, false );

                if( aFlags.bWithRedlining )
                {
                    aNdIdx = aDelPam.GetPoint()->nNode;
                    pAktTxtNd = aNdIdx.GetNode().GetTxtNode();
                    pTxt = &pAktTxtNd->GetTxt();
                    aDelPam.SetMark();
                    aFInfo.SetFrm( 0 );
                }

                nPos += sReplace.Len() - 1;
                aDelPam.DeleteMark();
                if( bSetHardBlank )
                {
                    pDoc->InsertString( aDelPam, CHAR_HARDBLANK );
                    ++nPos;
                }
            }
        }

        int bCallACorr = FALSE;
        int bBreak = 0;
        if( nPos && IsSpace( pTxt->GetChar( nPos-1 )))
            nLastBlank = nPos;
        for( nSttPos = nPos; !bBreak && nPos < pTxt->Len(); ++nPos )
            switch( cChar = pTxt->GetChar( nPos ) )
            {
            case '\"':
            case '\'':
                if( ( cChar == '\"' && bReplaceQuote ) || ( cChar == '\'' && bReplaceSglQuote ) )
                {
                    // --------------------------------------
                    // beachte: Sonderfall Symbolfonts !!!
                    if( !aFInfo.GetFrm() )
                        aFInfo.SetFrm( GetFrm( *pAktTxtNd ) );
                    if( !aFInfo.IsBullet( nPos ))
                    {
                        SetRedlineTxt( STR_AUTOFMTREDL_TYPO );
                        BOOL bSetHardBlank = FALSE;
                        aDelPam.GetPoint()->nContent = nPos;
                        String sReplace( pATst->GetQuote( aACorrDoc,
                                                    nPos, cChar, FALSE ));

                        if( 2 == sReplace.Len() && ' ' == sReplace.GetChar( 0 ))
                        {
                            sReplace.Erase( 0, 1 );
                            bSetHardBlank = TRUE;
                        }

                        aDelPam.SetMark();
                        aDelPam.GetPoint()->nContent = nPos+1;
                        pDoc->ReplaceRange( aDelPam, sReplace, false );

                        if( aFlags.bWithRedlining )
                        {
                            aNdIdx = aDelPam.GetPoint()->nNode;
                            pAktTxtNd = aNdIdx.GetNode().GetTxtNode();
                            pTxt = &pAktTxtNd->GetTxt();
                            aDelPam.SetMark();
                            aDelPam.DeleteMark();
                            aFInfo.SetFrm( 0 );
                        }

                        nPos += sReplace.Len() - 1;
                        aDelPam.DeleteMark();

                        if( bSetHardBlank )
                        {
                            aDelPam.GetPoint()->nContent = nPos;
                            pDoc->InsertString( aDelPam, CHAR_HARDBLANK );
                            aDelPam.GetPoint()->nContent = ++nPos;
                        }
                    }
                }
                break;
            case '*':
            case '_':
                if( aFlags.bChgWeightUnderl )
                {
                    // --------------------------------------
                    // beachte: Sonderfall Symbolfonts !!!
                    if( !aFInfo.GetFrm() )
                        aFInfo.SetFrm( GetFrm( *pAktTxtNd ) );
                    if( !aFInfo.IsBullet( nPos ))
                    {
                        SetRedlineTxt( '*' == cChar
                                            ? STR_AUTOFMTREDL_BOLD
                                            : STR_AUTOFMTREDL_UNDER );

                        sal_Unicode cBlank = nSttPos ? pTxt->GetChar(nSttPos - 1) : 0;
                        aDelPam.GetPoint()->nContent = nPos;

                        if( pATst->FnChgWeightUnderl( aACorrDoc, *pTxt,
                                                            nSttPos, nPos ))
                        {
                            if( aFlags.bWithRedlining )
                            {
                                aNdIdx = aDelPam.GetPoint()->nNode;
                                pAktTxtNd = aNdIdx.GetNode().GetTxtNode();
                                pTxt = &pAktTxtNd->GetTxt();
                                aDelPam.SetMark();
                                aDelPam.DeleteMark();
                                aFInfo.SetFrm( 0 );
                            }
                            //#125102# in case of the mode REDLINE_SHOW_DELETE the ** are still contained in pTxt
                            if(0 == (pDoc->GetRedlineMode() & nsRedlineMode_t::REDLINE_SHOW_DELETE))
                                nPos = aDelPam.GetPoint()->nContent.GetIndex() - 1;
                            // wurde vorm Start ein Zeichen entfernt?
                            if( cBlank && cBlank != pTxt->GetChar(nSttPos - 1) )
                                --nSttPos;
                        }
                    }
                }
                break;
            case '/':
                if ( aFlags.bAddNonBrkSpace )
                {
                    LanguageType eLang = (bGetLanguage && pAktTxtNd)
                                           ? pAktTxtNd->GetLang( nSttPos )
                                           : LANGUAGE_SYSTEM;
                
                    SetRedlineTxt( STR_AUTOFMTREDL_NON_BREAK_SPACE );
                    if ( pATst->FnAddNonBrkSpace( aACorrDoc, *pTxt, nSttPos, nPos, eLang ) )
                        --nPos;
                }
                break;

            case '.':
            case '!':
            case '?':
                if( aFlags.bCptlSttSntnc )
                    bFirstSent = TRUE;
//alle Wortrenner loesen die Autokorrektur aus!
//				break;
            default:
//alle Wortrenner loesen die Autokorrektur aus!
//			case ' ':
//			case '\t':
                if( !( rAppCC.isLetterNumeric( *pTxt, nPos )
                        || '/' == cChar )) //  '/' should not be a word seperator (e.g. '1/2' needs to be handled as one word for replacement)
                {
                    --nPos;		// ++nPos von dem for ungueltig machen !
                    ++bBreak;
                }
                break;
            }

        if( nPos == nSttPos )
        {
            if( ++nPos == pTxt->Len() )
                bCallACorr = TRUE;
        }
        else
            bCallACorr = TRUE;


        if( bCallACorr )
        {
            bCallACorr = FALSE;
            aDelPam.GetPoint()->nContent = nPos;
            SetRedlineTxt( STR_AUTOFMTREDL_USE_REPLACE );
            if( aFlags.bAutoCorrect &&
                aACorrDoc.ChgAutoCorrWord( nSttPos, nPos, *pATst, 0 ) )
            {
                nPos = aDelPam.GetPoint()->nContent.GetIndex();

                if( aFlags.bWithRedlining )
                {
                    aNdIdx = aDelPam.GetPoint()->nNode;
                    pAktTxtNd = aNdIdx.GetNode().GetTxtNode();
                    pTxt = &pAktTxtNd->GetTxt();
                    aDelPam.SetMark();
                    aDelPam.DeleteMark();
                }

                continue;		// nichts weiter mehr abpruefen
            }

            LanguageType eLang = (bGetLanguage && pAktTxtNd)
                                           ? pAktTxtNd->GetLang( nSttPos )
                                           : LANGUAGE_SYSTEM;
                
            if ( aFlags.bAddNonBrkSpace )
            {
                SetRedlineTxt( STR_AUTOFMTREDL_NON_BREAK_SPACE );
                pATst->FnAddNonBrkSpace( aACorrDoc, *pTxt, nSttPos, nPos, eLang );
            }

            if( ( aFlags.bChgOrdinalNumber &&
                    SetRedlineTxt( STR_AUTOFMTREDL_ORDINAL ) &&
                    pATst->FnChgOrdinalNumber( aACorrDoc, *pTxt, nSttPos, nPos, eLang ) ) ||
                ( aFlags.bChgToEnEmDash &&
                    SetRedlineTxt( STR_AUTOFMTREDL_DASH ) &&
                    pATst->FnChgToEnEmDash( aACorrDoc, *pTxt, nSttPos, nPos, eLang ) ) ||
                ( aFlags.bSetINetAttr &&
                    ( nPos == pTxt->Len() || IsSpace( pTxt->GetChar( nPos )) ) &&
                    SetRedlineTxt( STR_AUTOFMTREDL_DETECT_URL ) &&
                    pATst->FnSetINetAttr( aACorrDoc, *pTxt, nLastBlank, nPos, eLang ) ) )
                    nPos = aDelPam.GetPoint()->nContent.GetIndex();
            else
            {
                // Zwei Grossbuchstaben am Wort-Anfang ??
                if( aFlags.bCptlSttWrd )
                {
                    SetRedlineTxt( STR_AUTOFMTREDL_CPTL_STT_WORD );
                    pATst->FnCptlSttWrd( aACorrDoc, *pTxt, nSttPos, nPos, eLang );
                }
                // Grossbuchstabe am Satz-Anfang ??
                if( aFlags.bCptlSttSntnc && bFirst )
                {
                    SetRedlineTxt( STR_AUTOFMTREDL_CPTL_STT_SENT );
                    pATst->FnCptlSttSntnc( aACorrDoc, *pTxt, TRUE, nSttPos, nPos, eLang);
                    bFirst = FALSE;
                }

                bFirst = bFirstSent;
                bFirstSent = FALSE;

                if( aFlags.bWithRedlining )
                {
                    aNdIdx = aDelPam.GetPoint()->nNode;
                    pAktTxtNd = aNdIdx.GetNode().GetTxtNode();
                    pTxt = &pAktTxtNd->GetTxt();
                    aDelPam.SetMark();
                    aDelPam.DeleteMark();
                }
            }
        }
    } while( nPos < pTxt->Len() );
    ClearRedlineTxt();
}


SwAutoFormat::SwAutoFormat( SwEditShell* pEdShell, SvxSwAutoFmtFlags& rFlags,
                            SwNodeIndex* pSttNd, SwNodeIndex* pEndNd )
    : aFlags( rFlags ),
    aDelPam( pEdShell->GetDoc()->GetNodes().GetEndOfExtras() ),
    aNdIdx( pEdShell->GetDoc()->GetNodes().GetEndOfExtras(), +1 ),
    aEndNdIdx( pEdShell->GetDoc()->GetNodes().GetEndOfContent() ),
    pEditShell( pEdShell ),
    pDoc( pEdShell->GetDoc() ),
    pAktTxtNd( 0 ), pAktTxtFrm( 0 ),
    pCharClass( 0 ),
    nRedlAutoFmtSeqId( 0 )
{
    ASSERT( (pSttNd && pEndNd) || (!pSttNd && !pEndNd),
            "Kein Bereich angegeben" );

    if( aFlags.bSetNumRule && !aFlags.bAFmtByInput )
        aFlags.bSetNumRule = FALSE;

    BOOL bReplaceStyles = !aFlags.bAFmtByInput || aFlags.bReplaceStyles;

    const SwTxtNode* pNxtNd = 0;
    BOOL bNxtEmpty = FALSE;
    BOOL bNxtAlpha = FALSE;
    USHORT nNxtLevel = 0;

    // setze den Bereich zum Autoformatieren
    if( pSttNd )
    {
        aNdIdx = *pSttNd;
        aNdIdx--;			// fuer GoNextPara, ein Absatz davor
        aEndNdIdx = *pEndNd;
        aEndNdIdx++;

        // teste den vorhergehenden TextNode
        pNxtNd = aNdIdx.GetNode().GetTxtNode();
        bEmptyLine = !pNxtNd ||
                    IsEmptyLine( *pNxtNd ) ||
                    IsNoAlphaLine( *pNxtNd );
    }
    else
        bEmptyLine = TRUE;		// am Dokument Anfang

    bEnde = FALSE;

    // setze die Werte fuer die Prozent-Anzeige
    nEndNdIdx = aEndNdIdx.GetIndex();

    if( !aFlags.bAFmtByInput )
        ::StartProgress( STR_STATSTR_AUTOFORMAT, aNdIdx.GetIndex(),
                         nEndNdIdx = aEndNdIdx.GetIndex(),
                         pDoc->GetDocShell() );

    RedlineMode_t eRedlMode = pDoc->GetRedlineMode(), eOldMode = eRedlMode;
    if( aFlags.bWithRedlining )
    {
        pDoc->SetAutoFmtRedline( TRUE );
        eRedlMode = (RedlineMode_t)(nsRedlineMode_t::REDLINE_ON | nsRedlineMode_t::REDLINE_SHOW_INSERT);
    }
    else
      eRedlMode = (RedlineMode_t)(nsRedlineMode_t::REDLINE_SHOW_INSERT | nsRedlineMode_t::REDLINE_IGNORE);
    pDoc->SetRedlineMode( eRedlMode );

    // save undo state (might be turned off)
    sal_Bool bUndoState = pDoc->DoesUndo();

    // wenn mehrere Zeilen, dann erstmal nicht mit
    // dem nachfolgenden Absatz zusammenfassen.
    bMoreLines = FALSE;

    nLastCalcHeadLvl = nLastCalcEnumLvl = 0;
    nLastHeadLvl = nLastEnumLvl = USHRT_MAX;
    USHORT nLevel = 0;
    USHORT nDigitLvl = 0;

    // defaulten
    SwTxtFrmInfo aFInfo( 0 );

    // das ist unser Automat fuer die Auto-Formatierung
    eStat = READ_NEXT_PARA;
    while( !bEnde )
    {
        // #95884# limit redline array size to prevent overflow and to conserve
        // memory
        if( pDoc->HasTooManyUndos() )
        {
            DBG_ASSERT( bUndoState, "undo overflow without undo?" );

            //ask user
            short nResult = m_nActionWhileAutoformatUndoBufferOverflow; // TODO: #102007# read the last decision of the user from configuration
            if(m_bAskForCancelUndoWhileBufferOverflow) // #102007# TODO: read the last decision of the user from configuration
            {
                Window* pParent = pEditShell?pEditShell->GetWin():NULL;
                WarningBox aWarning( pParent,SW_RES(MSG_DISABLE_UNDO_QUESTION));
                aWarning.SetDefaultCheckBoxText();
                USHORT nDefaultButton = nResult==RET_YES?BUTTONID_YES:(nResult==RET_NO?BUTTONID_NO:BUTTONID_CANCEL);
                aWarning.SetFocusButton(nDefaultButton);
                nResult		= aWarning.Execute();
                m_bAskForCancelUndoWhileBufferOverflow = !aWarning.GetCheckBoxState();
                m_nActionWhileAutoformatUndoBufferOverflow = nResult;
                // TODO: #102007# store m_bAskForCancelUndoWhileBufferOverflow in configuration
                // TODO: #102007# store m_nActionWhileAutoformatUndoBufferOverflow in configuration
            }

            DBG_ASSERT( (nResult == RET_YES) || (nResult == RET_CANCEL) || (nResult == RET_NO),
                        "unexpected result" );

            if( nResult == RET_YES )
            {
                // turn off undo and continue
                pDoc->DoUndo( sal_False );
                pDoc->DelAllUndoObj();
            }
            else if( nResult == RET_NO )
            {
                //stop autoformatting and keep changes
                eStat = IS_ENDE;
            }
            else if( nResult == RET_CANCEL )
            {
                //cancel autoformatting and undo changes
                eStat = IS_ENDE;

                // TODO: #102004# undo changes
            }
        }

        switch( eStat )
        {
        case READ_NEXT_PARA:
            {
                GoNextPara();
                eStat = bEnde ? IS_ENDE : TST_EMPTY_LINE;
            }
            break;

        case TST_EMPTY_LINE:
            if( IsEmptyLine( *pAktTxtNd ) )
            {
                if( aFlags.bDelEmptyNode && !HasObjects( *pAktTxtNd ) )
                {
                    bEmptyLine = TRUE;
                    ULONG nOldCnt = pDoc->GetNodes().Count();
                    DelEmptyLine();
                    // wurde wiklich ein Node geloescht ?
                    if( nOldCnt != pDoc->GetNodes().Count() )
                        aNdIdx--;		// nicht den naechsten Absatz ueberspringen
                }
                eStat = READ_NEXT_PARA;
            }
            else
                eStat = TST_ALPHA_LINE;
            break;

        case TST_ALPHA_LINE:
            if( IsNoAlphaLine( *pAktTxtNd ))
            {
                // erkenne eine Tabellendefinition +---+---+
                if( aFlags.bAFmtByInput && aFlags.bCreateTable && DoTable() )
                {
                    //JP 30.09.96: das DoTable() verlaesst sich auf das
                    //				Pop und Move - Crsr nach dem AutoFormat!
                    pEdShell->Pop( FALSE );
                    *pEdShell->GetCrsr() = aDelPam;
                    pEdShell->Push();

                    eStat = IS_ENDE;
                    break;
                }

                // dann teste mal auf 3 "---" oder "===". In dem Fall
                // soll der vorherige Absatz unterstrichen und dieser
                // geloescht werden!
                if( !DoUnderline() && bReplaceStyles )
                {
                    SetColl( RES_POOLCOLL_STANDARD, TRUE );
                    bEmptyLine = TRUE;
                }
                eStat = READ_NEXT_PARA;
            }
            else
                eStat = GET_ALL_INFO;
            break;

        case GET_ALL_INFO:
            {
                if( pAktTxtNd->GetNumRule() )
                {
                    // in Numerierung nichts machen, zum naechsten
                    bEmptyLine = FALSE;
                    eStat = READ_NEXT_PARA;
                    // loesche alle Blanks am Anfang/Ende
                    // und alle mitten drin
                    //JP 29.04.98: erstmal nur alle "mitten drin".
                    DelMoreLinesBlanks( FALSE );
                    break;
                }

                aFInfo.SetFrm( pAktTxtFrm );

                // erstmal: wurden schon mal entsprechende Vorlagen
                //			vergeben, so behalte die bei, gehe zum
                //			naechsten Node.
                USHORT nPoolId = pAktTxtNd->GetTxtColl()->GetPoolFmtId();
                if( IsPoolUserFmt( nPoolId )
                        ? !aFlags.bChgUserColl
                        : ( RES_POOLCOLL_STANDARD != nPoolId &&
                           ( !aFlags.bAFmtByInput ||
                            (RES_POOLCOLL_TEXT_MOVE != nPoolId &&
                             RES_POOLCOLL_TEXT != nPoolId )) ))
                {
                    eStat = HAS_FMTCOLL;
                    break;
                }

                // teste auf Harte oder aus Vorlagen gesetzte LRSpaces
                if( IsPoolUserFmt( nPoolId ) ||
                    RES_POOLCOLL_STANDARD == nPoolId )
                {
                    short nSz;
                    SvxLRSpaceItem* pLRSpace;
                    if( SFX_ITEM_SET == pAktTxtNd->GetSwAttrSet().
                        GetItemState( RES_LR_SPACE, TRUE,
                                        (const SfxPoolItem**)&pLRSpace ) &&
                        ( 0 != (nSz = pLRSpace->GetTxtFirstLineOfst()) ||
                            0 != pLRSpace->GetTxtLeft() ) )
                    {
                        // Ausnahme: Numerierun/Aufzaehlung kann mit Einzug
                        //		existieren!!
                        if( IsEnumericChar( *pAktTxtNd ))
                        {
                            nLevel = CalcLevel( *pAktTxtNd, &nDigitLvl );
                            if( nLevel >= MAXLEVEL )
                                nLevel = MAXLEVEL-1;
                            BuildEnum( nLevel, nDigitLvl );
                            eStat = READ_NEXT_PARA;
                            break;
                        }


                        // nie zusammenfassen, so belassen
                        // (Opt. vielleicht als Ausnahmen nur Einzug)
                        bMoreLines = TRUE;

                        if( bReplaceStyles )
                        {
                            // dann setze doch eine unserer Vorlagen
                            if( 0 < nSz )			// positiver 1. Zeileneinzug
                                BuildIndent();
                            else if( 0 > nSz )		// negativer 1. Zeileneinzug
                                BuildNegIndent( aFInfo.GetLineStart() );
                            else if( pLRSpace->GetTxtLeft() )	// ist ein Einzug
                                BuildTextIndent();
                        }
                        eStat = READ_NEXT_PARA;
                        break;
                    }
                }

                nLevel = CalcLevel( *pAktTxtNd, &nDigitLvl );
                bMoreLines = !IsOneLine( *pAktTxtNd );
                pNxtNd = GetNextNode();
                if( pNxtNd )
                {
                    bNxtEmpty = IsEmptyLine( *pNxtNd );
                    bNxtAlpha = IsNoAlphaLine( *pNxtNd );
                    nNxtLevel = CalcLevel( *pNxtNd );

                    if( !bEmptyLine && HasBreakAttr( *pAktTxtNd ) )
                        bEmptyLine = TRUE;
                    if( !bNxtEmpty && HasBreakAttr( *pNxtNd ) )
                        bNxtEmpty = TRUE;

                    // fuer z.B. selbst definierte Einzuege oder
                    // rechts/zentierte Ausrichtung
//					if( !nLevel && 0 != aFInfo.GetLineStart() )
//						nLevel = 1;
                }
                else
                {
                    bNxtEmpty = FALSE; // TRUE;
                    bNxtAlpha = FALSE;
                    nNxtLevel = 0;
                }
                eStat = !bMoreLines ? IS_ONE_LINE : TST_ENUMERIC;
            }
            break;

        case IS_ONE_LINE:
            {
                eStat = TST_ENUMERIC;
                if( !bReplaceStyles )
                    break;

                String sClrStr( pAktTxtNd->GetTxt() );

                if( !DelLeadingBlanks( sClrStr ).Len() )
                {
                    bEmptyLine = TRUE;
                    eStat = READ_NEXT_PARA;
                    break;		// naechsten Absatz lesen
                }

                // Teste auf Ueberschrift
                if( !bEmptyLine || !IsFirstCharCapital( *pAktTxtNd ) ||
                    IsBlanksInString( *pAktTxtNd ) )
                    break;

                bEmptyLine = FALSE;
                String sEndClrStr( sClrStr );
                xub_StrLen nLen = DelTrailingBlanks( sEndClrStr ).Len();

                // nicht, dann teste auf Ueberschrift
                if( ':' == sEndClrStr.GetChar( nLen - 1 ) )
                {
//---------------------------------------------------------------------------
// Wie ist denn nun die Bedingung fuer die Ueberschrift auf Ebene 3 ??
// Zur Zeit: generell wenn am Ende ein ':' ist.
//
//					if( bNxtEmpty || bNxtAlpha )
//						!IsEnumericChar( *pNxtNd ) )
//---------------------------------------------------------------------------
                    {
                        BuildHeadLine( 2 );
                        eStat = READ_NEXT_PARA;
                        break;
                    }
                }
                else if( 256 <= sEndClrStr.GetChar( nLen-1 ) ||
                         !strchr( ",.;", sEndClrStr.GetChar( nLen-1 )) )
                {
                    if( bNxtEmpty || bNxtAlpha
                        || ( pNxtNd && IsEnumericChar( *pNxtNd ))

//---------------------------------------------------------------------------
// ist zum Verwechseln mit neg. Einzug !!
                        /*|| nLevel < nNxtLevel*/
//---------------------------------------------------------------------------

                        )
                    {
                        // wurde Level vom Text vorgegeben ?
//						if( USHRT_MAX != nDigitLvl )
//							nLevel = nDigitLvl;

                        // eine Ebene runter ?
                        if( nLevel >= MAXLEVEL )
                            nLevel = MAXLEVEL-1;

                        if( USHRT_MAX == nLastHeadLvl )
                            nLastHeadLvl = 0;
                        else if( nLastCalcHeadLvl < nLevel )
                        {
                            if( nLastHeadLvl+1 < MAXLEVEL )
                                ++nLastHeadLvl;
                        }
                        // eine Ebene hoch ?
                        else if( nLastCalcHeadLvl > nLevel )
                        {
                            if( nLastHeadLvl )
                                --nLastHeadLvl;
                        }
                        nLastCalcHeadLvl = nLevel;

                        if( aFlags.bAFmtByInput )
                            BuildHeadLine( nLevel );
                        else
                            BuildHeadLine( nLastHeadLvl );
                        eStat = READ_NEXT_PARA;
                        break;
                    }
                }
            }
            break;

        case TST_ENUMERIC:
            {
                bEmptyLine = FALSE;
                if( IsEnumericChar( *pAktTxtNd ))
                {
                    if( nLevel >= MAXLEVEL )
                        nLevel = MAXLEVEL-1;
                    BuildEnum( nLevel, nDigitLvl );
                    eStat = READ_NEXT_PARA;
                }
//JP 25.03.96: Vorlagen fuer Einzug zulassen
//				else if( aFlags.bAFmtByInput )
//					eStat = READ_NEXT_PARA;
                else if( bReplaceStyles )
                    eStat = nLevel ? TST_IDENT : TST_NEG_IDENT;
                else
                    eStat = READ_NEXT_PARA;
            }
            break;

        case TST_IDENT:
            // Spaces am Anfang, dann teste doch mal auf Einzuege
            if( bMoreLines && nLevel )
            {
                SwTwips nSz = aFInfo.GetFirstIndent();
                if( 0 < nSz )			// positiver 1. Zeileneinzug
                    BuildIndent();
                else if( 0 > nSz )		// negativer 1. Zeileneinzug
                    BuildNegIndent( aFInfo.GetLineStart() );
                else					// ist ein Einzug
                    BuildTextIndent();
                eStat = READ_NEXT_PARA;
            }
            else if( nLevel && pNxtNd && !bEnde &&
                     !bNxtEmpty && !bNxtAlpha && !nNxtLevel &&
                     !IsEnumericChar( *pNxtNd ) )
            {
                // ist ein Einzug
                BuildIndent();
                eStat = READ_NEXT_PARA;
            }
            else
                eStat = TST_TXT_BODY;
            break;

        case TST_NEG_IDENT:
            // keine Spaces am Anfang, dann teste doch mal auf neg. Einzuege
            {
                if( bMoreLines && !nLevel )
                {
                    SwTwips nSz = aFInfo.GetFirstIndent();
                    if( 0 < nSz )			// positiver 1. Zeileneinzug
                        BuildIndent();
                    else if( 0 > nSz )		// negativer 1. Zeileneinzug
                        BuildNegIndent( aFInfo.GetLineStart() );
                    else					// ist ein kein Einzug
                        BuildText();
                    eStat = READ_NEXT_PARA;
                }
                else if( !nLevel && pNxtNd && !bEnde &&
                         !bNxtEmpty && !bNxtAlpha && nNxtLevel &&
                         !IsEnumericChar( *pNxtNd ) )
                {
                    // ist ein neg. Einzug
                    BuildNegIndent( aFInfo.GetLineStart() );
                    eStat = READ_NEXT_PARA;
                }
                else
                    eStat = TST_TXT_BODY;
            }
            break;

        case TST_TXT_BODY:
            {
                if( bMoreLines )
                {
                    SwTwips nSz = aFInfo.GetFirstIndent();
                    if( 0 < nSz )			// positiver 1. Zeileneinzug
                        BuildIndent();
                    else if( 0 > nSz )		// negativer 1. Zeileneinzug
                        BuildNegIndent( aFInfo.GetLineStart() );
                    else if( nLevel )		// ist ein Einzug
                        BuildTextIndent();
                    else
                        BuildText();
                }
                else if( nLevel )
                    BuildTextIndent();
                else
                    BuildText();
                eStat = READ_NEXT_PARA;
            }
            break;

        case HAS_FMTCOLL:
            {
                // erstmal: wurden schon mal entsprechende Vorlagen
                //			vergeben, so behalte die bei, gehe zum
                //			naechsten Node.
                bEmptyLine = FALSE;
                eStat = READ_NEXT_PARA;
                // loesche alle Blanks am Anfang/Ende
                // und alle mitten drin
                //JP 29.04.98: erstmal nur alle "mitten drin".
                DelMoreLinesBlanks( FALSE );

                // behandel die harte Attributierung
                if( pAktTxtNd->HasSwAttrSet() )
                {
                    short nSz;
                    SvxLRSpaceItem* pLRSpace;
                    if( bReplaceStyles &&
                        SFX_ITEM_SET == pAktTxtNd->GetSwAttrSet().
                        GetItemState( RES_LR_SPACE, FALSE,
                                        (const SfxPoolItem**)&pLRSpace ) &&
                        ( 0 != (nSz = pLRSpace->GetTxtFirstLineOfst()) ||
                            0 != pLRSpace->GetTxtLeft() ) )
                    {
                        // dann setze doch eine unserer Vorlagen
                        if( 0 < nSz )			// positiver 1. Zeileneinzug
                            BuildIndent();
                        else if( 0 > nSz )		// negativer 1. Zeileneinzug
                        {
                            BuildNegIndent( aFInfo.GetLineStart() );
                        }
                        else if( pLRSpace->GetTxtLeft() )	// ist ein Einzug
                            BuildTextIndent();
                        else
                            BuildText();
                    }
                }
            }
            break;

        case IS_ENDE:
            bEnde = TRUE;
            break;
        }
    }

    if( aFlags.bWithRedlining )
        pDoc->SetAutoFmtRedline( FALSE );
    pDoc->SetRedlineMode( eOldMode );

    // restore undo (in case it has been changed)
    pDoc->DoUndo( bUndoState );

    // Prozent-Anzeige wieder abschalten
    if( !aFlags.bAFmtByInput )
        ::EndProgress( pDoc->GetDocShell() );
}

void SwEditShell::AutoFormat( const SvxSwAutoFmtFlags* pAFlags )
{
    SwWait* pWait = 0;

    SET_CURR_SHELL( this );
    StartAllAction();
    StartUndo( UNDO_AUTOFORMAT );

    SvxSwAutoFmtFlags aAFFlags;		// erst mal default - Werte
    if( pAFlags )					// oder doch angegeben ??
    {
        aAFFlags = *pAFlags;
        if( !aAFFlags.bAFmtByInput )
            pWait = new SwWait( *GetDoc()->GetDocShell(), TRUE );
    }

    SwPaM* pCrsr = GetCrsr();
    // es gibt mehr als einen oder ist eine Selektion offen
    if( pCrsr->GetNext() != pCrsr || pCrsr->HasMark() )
    {
        FOREACHPAM_START(this)
            if( PCURCRSR->HasMark() )
            {
                SwAutoFormat aFmt( this, aAFFlags, &PCURCRSR->Start()->nNode,
                                     &PCURCRSR->End()->nNode );
            }
        FOREACHPAM_END()
    }
    else
    {
        SwAutoFormat aFmt( this, aAFFlags );
    }

    EndUndo( UNDO_AUTOFORMAT );
    EndAllAction();

    delete pWait;
}


void SwEditShell::AutoFmtBySplitNode()
{
    SET_CURR_SHELL( this );
    SwPaM* pCrsr = GetCrsr();
    if( pCrsr->GetNext() == pCrsr && pCrsr->Move( fnMoveBackward, fnGoNode ) )
    {
        StartAllAction();
        StartUndo( UNDO_AUTOFORMAT );

        BOOL bRange = FALSE;
        pCrsr->SetMark();
        SwIndex* pCntnt = &pCrsr->GetMark()->nContent;
        if( pCntnt->GetIndex() )
        {
            *pCntnt = 0;
            bRange = TRUE;
        }
        else
        {
            // dann einen Node zurueckspringen
            SwNodeIndex aNdIdx( pCrsr->GetMark()->nNode, -1 );
            SwTxtNode* pTxtNd = aNdIdx.GetNode().GetTxtNode();
            if( pTxtNd && pTxtNd->GetTxt().Len() )
            {
                pCntnt->Assign( pTxtNd, 0 );
                pCrsr->GetMark()->nNode = aNdIdx;
                bRange = TRUE;
            }
        }

        if( bRange )
        {
            Push();		// Cursor sichern

            SvxSwAutoFmtFlags aAFFlags = *GetAutoFmtFlags();		// erst mal default - Werte

            SwAutoFormat aFmt( this, aAFFlags, &pCrsr->GetMark()->nNode,
                                    &pCrsr->GetPoint()->nNode );

            //JP 30.09.96: das DoTable() verlaesst sich auf das PopCrsr
            //				und MoveCrsr!
            Pop( FALSE );
            pCrsr = GetCrsr();
        }
        pCrsr->DeleteMark();
        pCrsr->Move( fnMoveForward, fnGoNode );

        EndUndo( UNDO_AUTOFORMAT );
        EndAllAction();
    }
}

SvxSwAutoFmtFlags* SwEditShell::GetAutoFmtFlags()
{
    if (!pAutoFmtFlags)
        pAutoFmtFlags = new SvxSwAutoFmtFlags;

    return pAutoFmtFlags;
}

void SwEditShell::SetAutoFmtFlags(SvxSwAutoFmtFlags * pFlags)
{
    SvxSwAutoFmtFlags* pEditFlags = GetAutoFmtFlags();

    pEditFlags->bSetNumRule		= pFlags->bSetNumRule;
    pEditFlags->bChgEnumNum		= pFlags->bChgEnumNum;
    pEditFlags->bSetBorder		= pFlags->bSetBorder;
    pEditFlags->bCreateTable	= pFlags->bCreateTable;
    pEditFlags->bReplaceStyles	= pFlags->bReplaceStyles;
    pEditFlags->bAFmtByInpDelSpacesAtSttEnd =
                                    pFlags->bAFmtByInpDelSpacesAtSttEnd;
    pEditFlags->bAFmtByInpDelSpacesBetweenLines =
                                    pFlags->bAFmtByInpDelSpacesBetweenLines;

    //JP 15.12.98: BulletZeichen und Font in die "normalen" kopieren,
    //			weil beim Autoformat nur mit diesen gearbeitet wird!
    pEditFlags->cBullet				= pFlags->cByInputBullet;
    pEditFlags->aBulletFont			= pFlags->aByInputBulletFont;
    pEditFlags->cByInputBullet		= pFlags->cByInputBullet;
    pEditFlags->aByInputBulletFont	= pFlags->aByInputBulletFont;
}

