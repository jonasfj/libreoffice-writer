/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: edtox.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:29:46 $
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
#ifndef _COM_SUN_STAR_UTIL_SEARCHOPTIONS_HPP_
#include <com/sun/star/util/SearchOptions.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_SEARCHFLAGS_HPP_
#include <com/sun/star/util/SearchFlags.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_TRANSLITERATIONMODULES_HPP_
#include <com/sun/star/i18n/TransliterationModules.hpp>
#endif


#pragma hdrstop

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _TXTCMP_HXX
#include <svtools/txtcmp.hxx>
#endif
#ifndef SVTOOLS_FSTATHELPER_HXX
#include <svtools/fstathelper.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include "svx/unolingu.hxx"
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif
#ifndef _UNDOBJ_HXX
#include <undobj.hxx>
#endif
#ifndef _TXTTXMRK_HXX //autogen
#include <txttxmrk.hxx>
#endif
#ifndef _EDIMP_HXX
#include <edimp.hxx>
#endif
#ifndef _TOX_HXX
#include <tox.hxx>
#endif
#ifndef _DOCTXM_HXX
#include <doctxm.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _MDIEXP_HXX
#include <mdiexp.hxx>
#endif
#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif

using namespace com::sun::star::i18n;
using namespace com::sun::star::lang;
using namespace com::sun::star::util;

/*--------------------------------------------------------------------
     Beschreibung: Verzeichnismarkierung ins Dokument einfuegen/loeschen
 --------------------------------------------------------------------*/


void SwEditShell::Insert(const SwTOXMark& rMark)
{
    BOOL bInsAtPos = rMark.IsAlternativeText();
    StartAllAction();
    FOREACHPAM_START(this)

        const SwPosition *pStt = PCURCRSR->Start(),
                         *pEnd = PCURCRSR->End();
        if( bInsAtPos )
        {
            SwPaM aTmp( *pStt );
            GetDoc()->Insert( aTmp, rMark, 0 );
        }
        else if( *pEnd != *pStt )
            GetDoc()->Insert( *PCURCRSR, rMark, SETATTR_DONTEXPAND );

    FOREACHPAM_END()
    EndAllAction();
}



void SwEditShell::DeleteTOXMark( SwTOXMark* pMark )
{
    SET_CURR_SHELL( this );
    StartAllAction();

    pDoc->Delete( pMark );

    EndAllAction();
}


/*--------------------------------------------------------------------
     Beschreibung: Alle Verzeichnismarkierungen am SPoint zusammensuchen
 --------------------------------------------------------------------*/

USHORT SwEditShell::GetCurTOXMarks(SwTOXMarks& rMarks) const
{
    return GetDoc()->GetCurTOXMark( *GetCrsr()->Start(), rMarks );
}

/* -----------------01.09.99 16:05-------------------

 --------------------------------------------------*/
BOOL SwEditShell::IsTOXBaseReadonly(const SwTOXBase& rTOXBase) const
{
    ASSERT( rTOXBase.ISA( SwTOXBaseSection ), "no TOXBaseSection!" );
    const SwTOXBaseSection& rTOXSect = (const SwTOXBaseSection&)rTOXBase;
    return 	rTOXSect.IsProtect();
}
/* -----------------18.10.99 15:53-------------------

 --------------------------------------------------*/
void SwEditShell::SetTOXBaseReadonly(const SwTOXBase& rTOXBase, BOOL bReadonly)
{
    ASSERT( rTOXBase.ISA( SwTOXBaseSection ), "no TOXBaseSection!" );
    const SwTOXBaseSection& rTOXSect = (const SwTOXBaseSection&)rTOXBase;
    ((SwTOXBase&)rTOXBase).SetProtected(bReadonly);
    ASSERT( rTOXSect.SwSection::GetType() == TOX_CONTENT_SECTION, "not a TOXContentSection" );

    SwSection aSect(TOX_CONTENT_SECTION, rTOXSect.GetName());
    aSect = rTOXSect;
    aSect.SetProtect(bReadonly);
    ChgSection( GetSectionFmtPos( *rTOXSect.GetFmt()  ), aSect, 0 );
}

/* -----------------02.09.99 07:47-------------------

 --------------------------------------------------*/
const SwTOXBase* 	SwEditShell::GetDefaultTOXBase( TOXTypes eTyp, BOOL bCreate )
{
    return GetDoc()->GetDefaultTOXBase( eTyp, bCreate );
}
/* -----------------02.09.99 08:05-------------------

 --------------------------------------------------*/
void	SwEditShell::SetDefaultTOXBase(const SwTOXBase& rBase)
{
    GetDoc()->SetDefaultTOXBase(rBase);
}

/*--------------------------------------------------------------------
     Beschreibung: Verzeichnis einfuegen, und Inhalt erzeugen
 --------------------------------------------------------------------*/

void SwEditShell::InsertTableOf( const SwTOXBase& rTOX, const SfxItemSet* pSet )
{
    SET_CURR_SHELL( this );
    StartAllAction();

    SwDocShell* pDocSh = GetDoc()->GetDocShell();
    ::StartProgress( STR_STATSTR_TOX_INSERT, 0, 0, pDocSh );
    ::SetProgressText( STR_STATSTR_TOX_INSERT, pDocSh );

    // Einfuegen des Verzeichnisses
    const SwTOXBaseSection* pTOX = pDoc->InsertTableOf(
                                        *GetCrsr()->GetPoint(), rTOX, pSet, TRUE );
    ASSERT(pTOX, "Kein aktuelles Verzeichnis");

    // Formatierung anstossen
    CalcLayout();

    // Seitennummern eintragen
    ((SwTOXBaseSection*)pTOX)->UpdatePageNum();

    pTOX->SetPosAtStartEnd( *GetCrsr()->GetPoint() );

    // Fix fuer leere Verzeichnisse
    InvalidateWindows( aVisArea );
    ::EndProgress( pDocSh );
    EndAllAction();
}

/*--------------------------------------------------------------------
     Beschreibung: Verzeichnisinhalt erneuern
 --------------------------------------------------------------------*/

BOOL SwEditShell::UpdateTableOf( const SwTOXBase& rTOX, const SfxItemSet* pSet )
{
    BOOL bRet = FALSE;

    ASSERT( rTOX.ISA( SwTOXBaseSection ),  "keine TOXBaseSection!" );
    SwTOXBaseSection* pTOX = (SwTOXBaseSection*)&rTOX;
    ASSERT(pTOX, "Keine aktuelles Verzeichnis");
    const SwSectionNode* pSectNd;
    if( pTOX && 0 != ( pSectNd = pTOX->GetFmt()->GetSectionNode() ) )
    {
        SwDoc* pDoc = GetDoc();
        SwDocShell* pDocSh = pDoc->GetDocShell();

        BOOL bInIndex = pTOX == GetCurTOX();
        SET_CURR_SHELL( this );
        StartAllAction();

        ::StartProgress( STR_STATSTR_TOX_UPDATE, 0, 0, pDocSh );
        ::SetProgressText( STR_STATSTR_TOX_UPDATE, pDocSh );

        pDoc->StartUndo(UNDO_TOXCHANGE);

        // Verzeichnisrumpf erzeugen
        pTOX->Update(pSet);

        // Cursor korrigieren
        if( bInIndex )
            pTOX->SetPosAtStartEnd( *GetCrsr()->GetPoint() );

        // Formatierung anstossen
        CalcLayout();

        // Seitennummern eintragen
        pTOX->UpdatePageNum();

        pDoc->EndUndo(UNDO_TOXCHANGE);        

        ::EndProgress( pDocSh );
        EndAllAction();
    }
    return bRet;
}

/*--------------------------------------------------------------------
     Beschreibung: Aktuelles Verzeichnis vor oder in dem der Cursor
                                   steht
 --------------------------------------------------------------------*/

const SwTOXBase* SwEditShell::GetCurTOX() const
{
    return GetDoc()->GetCurTOX( *GetCrsr()->GetPoint() );
}

BOOL SwEditShell::DeleteTOX( const SwTOXBase& rTOXBase, BOOL bDelNodes )
{
    return GetDoc()->DeleteTOX( (SwTOXBase&)rTOXBase, bDelNodes );
}

/*--------------------------------------------------------------------
     Beschreibung: Typen der Verzeichnisse verwalten
 --------------------------------------------------------------------*/

const SwTOXType* SwEditShell::GetTOXType(TOXTypes eTyp, USHORT nId) const
{
    return pDoc->GetTOXType(eTyp, nId);
}

/*--------------------------------------------------------------------
     Beschreibung: Schluessel fuer Stichwortverzeichnisse verwalten
 --------------------------------------------------------------------*/

USHORT SwEditShell::GetTOIKeys( SwTOIKeyType eTyp, SvStringsSort& rArr ) const
{
    return GetDoc()->GetTOIKeys( eTyp, rArr );
}


USHORT SwEditShell::GetTOXCount() const
{
    const SwSectionFmts& rFmts = GetDoc()->GetSections();
    USHORT nRet = 0;
    for( USHORT n = rFmts.Count(); n; )
    {
        const SwSection* pSect = rFmts[ --n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode() )
            ++nRet;
    }
    return nRet;
}


const SwTOXBase* SwEditShell::GetTOX( USHORT nPos ) const
{
    const SwSectionFmts& rFmts = GetDoc()->GetSections();
    for( USHORT n = 0, nCnt = 0; n < rFmts.Count(); ++n )
    {
        const SwSection* pSect = rFmts[ n ]->GetSection();
        if( TOX_CONTENT_SECTION == pSect->GetType() &&
            pSect->GetFmt()->GetSectionNode() &&
            nCnt++ == nPos )
        {
            ASSERT( pSect->ISA( SwTOXBaseSection ), "keine TOXBaseSection!" );
            return (SwTOXBaseSection*)pSect;
        }
    }
    return 0;
}


    // nach einlesen einer Datei alle Verzeichnisse updaten
void SwEditShell::SetUpdateTOX( BOOL bFlag )
{
    GetDoc()->SetUpdateTOX( bFlag );
}


BOOL SwEditShell::IsUpdateTOX() const
{
    return GetDoc()->IsUpdateTOX();
}

/* -----------------26.08.99 13:49-------------------

 --------------------------------------------------*/
const String&	SwEditShell::GetTOIAutoMarkURL() const
{
    return GetDoc()->GetTOIAutoMarkURL();
}
/* -----------------26.08.99 13:49-------------------

 --------------------------------------------------*/
void SwEditShell::SetTOIAutoMarkURL(const String& rSet)
{
    GetDoc()->SetTOIAutoMarkURL(rSet);
}
/* -----------------26.08.99 09:29-------------------

 --------------------------------------------------*/
void SwEditShell::ApplyAutoMark()
{
    StartAllAction();
    BOOL bDoesUndo = DoesUndo();
    DoUndo(FALSE);
    //1. remove all automatic generated index entries if AutoMarkURL has a
    //	 length and the file exists
    //2. load file
    //3. select all occurrences of the searched words
    //4. apply index entries

    String sAutoMarkURL(GetDoc()->GetTOIAutoMarkURL());
    if( sAutoMarkURL.Len() && FStatHelper::IsDocument( sAutoMarkURL ))
    {
        //1.
        const SwTOXType* pTOXType = GetTOXType(TOX_INDEX, 0);
        SwClientIter aIter(*(SwTOXType*)pTOXType);
        SwTOXMark* pMark = (SwTOXMark*)aIter.First(TYPE(SwTOXMark));
        while( pMark )
        {
            if(pMark->IsAutoGenerated() && pMark->GetTxtTOXMark())
                DeleteTOXMark(pMark);
            pMark = (SwTOXMark*)aIter.Next();
        }

        //2.
        SfxMedium aMedium( sAutoMarkURL, STREAM_STD_READ, TRUE );
        SvStream& rStrm = *aMedium.GetInStream();
        const String sZero('0');
        Push();
        rtl_TextEncoding eChrSet = ::gsl_getSystemTextEncoding();

        //
        // SearchOptions to be used in loop below
        //
        //SearchAlgorithms eSrchType	= SearchAlgorithms_ABSOLUTE;
        //OUString aSrchStr	= rText;
        BOOL bCaseSensitive	= TRUE;
        BOOL bWordOnly		= FALSE;
        BOOL bSrchInSel		= FALSE;
        BOOL bLEV_Relaxed	= TRUE;
        INT32 nLEV_Other	= 2;	//  -> changedChars;
        INT32 nLEV_Longer	= 3;	//! -> deletedChars;
        INT32 nLEV_Shorter	= 1;	//!	-> insertedChars;
        INT32 nTransliterationFlags = 0;
        //
        INT32 nSrchFlags = 0;
        if (!bCaseSensitive)
        {
            nSrchFlags |= SearchFlags::ALL_IGNORE_CASE;
            nTransliterationFlags |= TransliterationModules_IGNORE_CASE;
        }
        if ( bWordOnly)
            nSrchFlags |= SearchFlags::NORM_WORD_ONLY;
        if ( bLEV_Relaxed)
            nSrchFlags |= SearchFlags::LEV_RELAXED;
        if ( bSrchInSel)
            nSrchFlags |= (SearchFlags::REG_NOT_BEGINOFLINE |
                            SearchFlags::REG_NOT_ENDOFLINE );
        //
        rtl::OUString sEmpty;
        SearchOptions aSearchOpt(
                            SearchAlgorithms_ABSOLUTE, nSrchFlags,
                            sEmpty, sEmpty,
                            SvxCreateLocale( LANGUAGE_SYSTEM ),
                            nLEV_Other, nLEV_Longer, nLEV_Shorter,
                            nTransliterationFlags );

        while( !rStrm.GetError() && !rStrm.IsEof() )
        {
            ByteString aRdLine;
            rStrm.ReadLine( aRdLine );

            // # -> comment
            // ; -> delimiter between entries ->
            // Format: TextToSearchFor;AlternativeString;PrimaryKey;SecondaryKey;CaseSensitive;WordOnly
            // Leading and trailing blanks are ignored
            if( aRdLine.Len() && '#' != aRdLine.GetChar(0) )
            {
                String sLine( aRdLine, eChrSet );

                xub_StrLen nTokenPos = 0;
                String sToSelect( sLine.GetToken(0, ';', nTokenPos ) );
                if( sToSelect.Len() )
                {
                    String sAlternative = sLine.GetToken(0, ';', nTokenPos);
                    String sPrimary 	= sLine.GetToken(0, ';', nTokenPos);
                    String sSecondary 	= sLine.GetToken(0, ';', nTokenPos);
                    String sCase        = sLine.GetToken(0, ';', nTokenPos);
                    String sWordOnly 	= sLine.GetToken(0, ';', nTokenPos);

                    //3.
                    bCaseSensitive	= sCase.Len() && sCase != sZero;
                    bWordOnly		= sWordOnly.Len() && sWordOnly != sZero;
                    //
                    if (!bCaseSensitive)
                    {
                        //nSrchFlags |= SearchFlags::ALL_IGNORE_CASE;
                        aSearchOpt.transliterateFlags |=
                                     TransliterationModules_IGNORE_CASE;
                    }
                    else
                    {
                        //aSearchOpt.searchFlag &= ~SearchFlags::ALL_IGNORE_CASE;
                        aSearchOpt.transliterateFlags &=
                                    ~TransliterationModules_IGNORE_CASE;
                    }
                    if ( bWordOnly)
                        aSearchOpt.searchFlag |=  SearchFlags::NORM_WORD_ONLY;
                    else
                        aSearchOpt.searchFlag &= ~SearchFlags::NORM_WORD_ONLY;
                    //
                    aSearchOpt.searchString = sToSelect;

                    KillPams();
                    BOOL bCancel;
                    ULONG nRet = Find( aSearchOpt,  DOCPOS_START, DOCPOS_END, bCancel,
                                    (FindRanges)(FND_IN_SELALL|FND_IN_BODYONLY),
                                    FALSE );

                    if(nRet)
                    {
                        SwTOXMark* pMark = new SwTOXMark(pTOXType);
                        if( sPrimary.Len() )
                        {
                            pMark->SetPrimaryKey( sPrimary );
                            if( sSecondary.Len() )
                                pMark->SetSecondaryKey( sSecondary );
                        }
                        if(sAlternative.Len())
                            pMark->SetAlternativeText(sAlternative);
                        pMark->SetMainEntry(FALSE);
                        pMark->SetAutoGenerated(TRUE);
                        //4.
                        SwEditShell::Insert(*pMark);
                    }
                }
            }
        }
        KillPams();
        Pop(FALSE);
    }
    DoUndo(bDoesUndo);
    EndAllAction();
}



