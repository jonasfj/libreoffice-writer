/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlatr.cxx,v $
 * $Revision: 1.41.140.1 $
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
#include <com/sun/star/i18n/ScriptType.hpp>
#include <vcl/svapp.hxx>
#ifndef _WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#include <tools/urlobj.hxx>

#if !defined _SVSTDARR_XUB_STRLEN_DECL || !defined _SVSTDARR_USHORTS_DECL
#define _SVSTDARR_XUB_STRLEN
#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>
#endif
#include <svtools/htmlout.hxx>
#include <svtools/htmlkywd.hxx>
#include <svtools/htmltokn.h>
#include <svtools/whiter.hxx>
#include <svx/htmlmode.hxx>
#include <svx/escpitem.hxx>
#include <svx/brkitem.hxx>
#include <svx/boxitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/crsditem.hxx>
#include <svx/blnkitem.hxx>
#include <svx/cmapitem.hxx>
#include <svx/colritem.hxx>
#include <svx/fontitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/postitem.hxx>
#include <svx/kernitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/lspcitem.hxx>
#include <svx/adjitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/brshitem.hxx>
#include <svx/langitem.hxx>
#include <svx/frmdiritem.hxx>
#include <fchrfmt.hxx>
#include <fmtautofmt.hxx>
#include <fmtfsize.hxx>
#include <fmtclds.hxx>
#include <fmtpdsc.hxx>
#include <fmtflcnt.hxx>
#include <fmtinfmt.hxx>
#include <fmtftn.hxx>
#include <txatbase.hxx>
#include <frmatr.hxx>
#include <charfmt.hxx>
#include <fmtfld.hxx>
#include <doc.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <paratr.hxx>
#include <poolfmt.hxx>
#include <pagedesc.hxx>
#include <swtable.hxx>
#include "fldbas.hxx"
#include <breakit.hxx>
#include <htmlnum.hxx>
#include <wrthtml.hxx>
#include <htmlfly.hxx>
#include <numrule.hxx>

using namespace ::com::sun::star;

/*
 * um nicht immer wieder nach einem Update festzustellen, das irgendwelche
 * Hint-Ids dazugekommen sind, wird hier definiert, die Groesse der Tabelle
 * definiert und mit der akt. verglichen. Bei unterschieden wird der
 * Compiler schon meckern.
 *
 * diese Section und die dazugeherigen Tabellen muessen in folgenden Files
 * gepflegt werden: rtf\rtfatr.cxx, sw6\sw6atr.cxx, w4w\w4watr.cxx
 */
#if !defined(UNX) && !defined(MSC) && !defined(PPC) && !defined(CSET) && !defined(__MWERKS__) && !defined(WTC) && !defined(__MINGW32__) && !defined(OS2)

#define ATTRFNTAB_SIZE 130
#if ATTRFNTAB_SIZE != POOLATTR_END - POOLATTR_BEGIN
#error Attribut-Tabelle ist ungueltigt. Wurden neue Hint-IDs zugefuegt ??
#endif

#ifdef FORMAT_TABELLE
// da sie nicht benutzt wird!
#define FORMATTAB_SIZE 7
#if FORMATTAB_SIZE != RES_FMT_END - RES_FMT_BEGIN
#error Format-Tabelle ist ungueltigt. Wurden neue Hint-IDs zugefuegt ??
#endif
#endif

#define NODETAB_SIZE 3
#if NODETAB_SIZE != RES_NODE_END - RES_NODE_BEGIN
#error Node-Tabelle ist ungueltigt. Wurden neue Hint-IDs zugefuegt ??
#endif

#endif

#define HTML_BULLETCHAR_DISC	34
#define HTML_BULLETCHAR_CIRCLE	38
#define HTML_BULLETCHAR_SQUARE	36

#define COLFUZZY 20

//-----------------------------------------------------------------------

HTMLOutEvent __FAR_DATA aAnchorEventTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_O_SDonclick,		OOO_STRING_SVTOOLS_HTML_O_onclick,		SFX_EVENT_MOUSECLICK_OBJECT	},
    { OOO_STRING_SVTOOLS_HTML_O_SDonmouseover,	OOO_STRING_SVTOOLS_HTML_O_onmouseover,	SFX_EVENT_MOUSEOVER_OBJECT	},
    { OOO_STRING_SVTOOLS_HTML_O_SDonmouseout,		OOO_STRING_SVTOOLS_HTML_O_onmouseout,		SFX_EVENT_MOUSEOUT_OBJECT	},
    { 0,						0,					  	0							}
};

static Writer& OutHTML_SvxAdjust( Writer& rWrt, const SfxPoolItem& rHt );

static Writer& OutHTML_HoriSpacer( Writer& rWrt, INT16 nSize )
{
    ASSERT( nSize>0, "horizontaler SPACER mit negativem Wert?" )
    if( nSize <= 0 )
        return rWrt;

    if( Application::GetDefaultDevice() )
    {
        nSize = (INT16)Application::GetDefaultDevice()
            ->LogicToPixel( Size(nSize,0), MapMode(MAP_TWIP) ).Width();
    }

    ByteString sOut( '<' );
    (((((((((sOut += OOO_STRING_SVTOOLS_HTML_spacer)
        += ' ') += OOO_STRING_SVTOOLS_HTML_O_type) += '=') += OOO_STRING_SVTOOLS_HTML_SPTYPE_horizontal)
        += ' ') += OOO_STRING_SVTOOLS_HTML_O_size) += '=')
                        +=ByteString::CreateFromInt32(nSize)) += '>';

    rWrt.Strm() << sOut.GetBuffer();

    return rWrt;
}

USHORT SwHTMLWriter::GetDefListLvl( const String& rNm, USHORT nPoolId )
{
    if( nPoolId == RES_POOLCOLL_HTML_DD )
    {
        return 1 | HTML_DLCOLL_DD;
    }
    else if( nPoolId == RES_POOLCOLL_HTML_DT )
    {
        return 1 | HTML_DLCOLL_DT;
    }

    String sDTDD( String::CreateFromAscii(OOO_STRING_SVTOOLS_HTML_dt) );
    sDTDD += ' ';
    if( COMPARE_EQUAL == sDTDD.CompareTo( rNm, sDTDD.Len() ) )
        // DefinitionList - term
        return (USHORT)rNm.Copy( sDTDD.Len() ).ToInt32() | HTML_DLCOLL_DT;

    sDTDD.AssignAscii( OOO_STRING_SVTOOLS_HTML_dd );
    sDTDD += ' ';
    if( COMPARE_EQUAL == sDTDD.CompareTo( rNm, sDTDD.Len() ) )
        // DefinitionList - definition
        return (USHORT)rNm.Copy( sDTDD.Len() ).ToInt32() | HTML_DLCOLL_DD;

    return 0;
}

void SwHTMLWriter::OutAndSetDefList( USHORT nNewLvl )
{
    // eventuell muss erst mal eine Liste aufgemacht werden
    if( nDefListLvl < nNewLvl )
    {
        // output </pre> for the previous(!) pararagraph, if required.
        // Preferable, the <pre> is exported by OutHTML_SwFmtOff for the
           // previous	paragraph already, but that's not possible, because a very
        // deep look at the next paragraph (this one) is required to figure
        // out that a def list starts here.

        ChangeParaToken( 0 );

        // entsprechend dem Level-Unterschied schreiben!
        for( USHORT i=nDefListLvl; i<nNewLvl; i++ )
        {
            if( bLFPossible )
                OutNewLine();
            HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_deflist, TRUE );
            IncIndentLevel();
            bLFPossible = TRUE;
        }
    }
    else if( nDefListLvl > nNewLvl )
    {
        for( USHORT i=nNewLvl ; i < nDefListLvl; i++ )
        {
            DecIndentLevel();
            if( bLFPossible )
                OutNewLine();
            HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_deflist, FALSE );
            bLFPossible = TRUE;
        }
    }

    nDefListLvl = nNewLvl;
}


void SwHTMLWriter::ChangeParaToken( USHORT nNew )
{
    if( nNew != nLastParaToken && HTML_PREFORMTXT_ON == nLastParaToken )
    {
        HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_preformtxt, FALSE );
        bLFPossible = TRUE;
    }
    nLastParaToken = nNew;
}

sal_uInt16 SwHTMLWriter::GetCSS1ScriptForScriptType( sal_uInt16 nScriptType )
{
    sal_uInt16 nRet = CSS1_OUTMODE_ANY_SCRIPT;

    switch( nScriptType )
    {
    case i18n::ScriptType::LATIN:
        nRet = CSS1_OUTMODE_WESTERN;
        break;
    case i18n::ScriptType::ASIAN:
        nRet = CSS1_OUTMODE_CJK;
        break;
    case i18n::ScriptType::COMPLEX:
        nRet = CSS1_OUTMODE_CTL;
        break;
    }

    return nRet;
}

// fuer die Formate muesste eine einzige Ausgabe-Funktion genuegen !
/*
 * Formate wie folgt ausgeben:
 * - fuer Formate, fuer die es entsprechende HTML-Tags gibt wird das
 *   Tag ausgegeben
 * - fuer alle anderen wird ein Absatz-Tag <P> ausgegeben und bUserFmt
 *   gesetzt
 * - Wenn eine Absatz-Ausrichtung am uebergebenen Item-Set des Nodes
 *   oder im Item-Set des Format gesetzt ist, wird ein ALIGN=xxx ausgegeben,
 *   sofern HTML es zulaesst
 * - in jedem Fall wird harte Attributierung als STYLE-Option geschrieben.
 *   Wenn bUserFmt nicht gesetzt ist, wird nur der uebergebene Item-Set
 *   betrachtet. Sonst werden auch Attribute des Formats ausgegeben.
 */

struct SwHTMLTxtCollOutputInfo
{
    ByteString aToken;				// auszugendens End-Token
    SfxItemSet *pItemSet;		// harte Attributierung

    BOOL bInNumBulList;			// in einer Aufzaehlungs-Liste;
    BOOL bParaPossible;			// ein </P> darf zusaetzlich ausgegeben werden
    BOOL bOutPara;				// ein </P> soll ausgegeben werden
    BOOL bOutDiv;				// write a </DIV>

    SwHTMLTxtCollOutputInfo() :
        pItemSet( 0 ),
        bInNumBulList( FALSE ),
        bParaPossible( FALSE ),
        bOutPara( FALSE ),
        bOutDiv( FALSE )
    {}

    ~SwHTMLTxtCollOutputInfo();

    BOOL HasParaToken() const { return aToken.Len()==1 && aToken.GetChar(0)=='P'; }
    BOOL ShouldOutputToken() const { return bOutPara || !HasParaToken(); }
};

SwHTMLTxtCollOutputInfo::~SwHTMLTxtCollOutputInfo()
{
    delete pItemSet;
}

struct SwHTMLFmtInfo
{
    const SwFmt *pFmt;		// das Format selbst
    const SwFmt *pRefFmt;	// das Vergleichs-Format

    ByteString aToken;			// das auszugebende Token
    String aClass;			// die auszugebende Klasse

    SfxItemSet *pItemSet;	// der auszugebende Attribut-Set

    sal_Int32 nLeftMargin;		// ein par default-Werte fuer
    sal_Int32 nRightMargin;	// Absatz-Vorlagen
    short nFirstLineIndent;

    USHORT nTopMargin;
    USHORT nBottomMargin;

    sal_Bool bScriptDependent;

    // Konstruktor fuer einen Dummy zum Suchen
    SwHTMLFmtInfo( const SwFmt *pF ) :
        pFmt( pF ), pItemSet( 0 )
    {}


    // Konstruktor zum Erstellen der Format-Info
    SwHTMLFmtInfo( const SwFmt *pFmt, SwDoc *pDoc, SwDoc *pTemlate,
                   BOOL bOutStyles, LanguageType eDfltLang=LANGUAGE_DONTKNOW,
                   sal_uInt16 nScript=CSS1_OUTMODE_ANY_SCRIPT,
                   BOOL bHardDrop=FALSE );
    ~SwHTMLFmtInfo();

    friend BOOL operator==( const SwHTMLFmtInfo& rInfo1,
                            const SwHTMLFmtInfo& rInfo2 )
    {
        return (long)rInfo1.pFmt == (long)rInfo2.pFmt;
    }

    friend BOOL operator<( const SwHTMLFmtInfo& rInfo1,
                            const SwHTMLFmtInfo& rInfo2 )
    {
        return (long)rInfo1.pFmt < (long)rInfo2.pFmt;
    }

};

SV_IMPL_OP_PTRARR_SORT( SwHTMLFmtInfos, SwHTMLFmtInfo* )

SwHTMLFmtInfo::SwHTMLFmtInfo( const SwFmt *pF, SwDoc *pDoc, SwDoc *pTemplate,
                              BOOL bOutStyles,
                              LanguageType eDfltLang,
                              sal_uInt16 nCSS1Script, BOOL bHardDrop ) :
    pFmt( pF ), pItemSet( 0 ), bScriptDependent( sal_False )
{
    USHORT nRefPoolId = 0;
    // Den Selektor des Formats holen
    USHORT nDeep = SwHTMLWriter::GetCSS1Selector( pFmt, aToken, aClass,
                                                  nRefPoolId );
    ASSERT( nDeep ? aToken.Len()>0 : aToken.Len()==0,
            "Hier stimmt doch was mit dem Token nicht!" );
    ASSERT( nDeep ? nRefPoolId : !nRefPoolId,
            "Hier stimmt doch was mit der Vergleichs-Vorlage nicht!" );

    BOOL bTxtColl = pFmt->Which() == RES_TXTFMTCOLL ||
                    pFmt->Which() == RES_CONDTXTFMTCOLL;

    const SwFmt *pReferenceFmt = 0;	// Vergleichs-Format
    sal_Bool bSetDefaults = sal_True, bClearSame = sal_True;
    if( nDeep != 0 )
    {
        // Es ist eine HTML-Tag-Vorlage oder die Vorlage ist von einer
        // solchen abgeleitet
        if( !bOutStyles )
        {
            // wenn keine Styles exportiert werden, muss evtl. zusaetlich
            // harte Attributierung geschrieben werden
            switch( nDeep )
            {
            case CSS1_FMT_ISTAG:
            case CSS1_FMT_CMPREF:
                // fuer HTML-Tag-Vorlagen die Unterscheide zum Original
                // (sofern verfuegbar)
                pReferenceFmt = SwHTMLWriter::GetTemplateFmt( nRefPoolId,
                                                        pTemplate );
                break;

            default:
                // sonst die zur HTML-Tag-Vorlage des Originals oder des
                // aktuellen Doks, wenn die nicht verfuegbar ist
                if( pTemplate )
                    pReferenceFmt = SwHTMLWriter::GetTemplateFmt( nRefPoolId,
                                                            pTemplate );
                else
                    pReferenceFmt = SwHTMLWriter::GetParentFmt( *pFmt, nDeep );
                break;
            }
        }
    }
    else if( bTxtColl )
    {
        // Nicht von einer HTML-Tag-Vorlage abgeleitete Absatz-Vorlagen
        // muessen als harte Attributierung relativ zur Textkoerper-Volage
        // exportiert werden. Fuer Nicht-Styles-Export sollte die der
        // HTML-Vorlage als Referenz dienen
        if( !bOutStyles && pTemplate )
            pReferenceFmt = pTemplate->GetTxtCollFromPool( RES_POOLCOLL_TEXT, false );
        else
            pReferenceFmt = pDoc->GetTxtCollFromPool( RES_POOLCOLL_TEXT, false );
    }

    if( pReferenceFmt || nDeep==0 )
    {
        pItemSet = new SfxItemSet( *pFmt->GetAttrSet().GetPool(),
                                       pFmt->GetAttrSet().GetRanges() );
        // wenn Unterschiede zu einer anderen Vorlage geschrieben werden
        // sollen ist harte Attributierung noetig. Fuer Vorlagen, die
        // nicht von HTML-Tag-Vorlagen abgeleitet sind, gilt das immer

        pItemSet->Set( pFmt->GetAttrSet(), TRUE );

        if( pReferenceFmt )
            SwHTMLWriter::SubtractItemSet( *pItemSet, pReferenceFmt->GetAttrSet(),
                                           bSetDefaults, bClearSame );

        // einen leeren Item-Set gleich loeschen, das spart speater
        // Arbeit
        if( !pItemSet->Count() )
        {
            delete pItemSet;
            pItemSet = 0;
        }
    }

    if( bTxtColl )
    {
        if( bOutStyles )
        {
            // We have to add hard attributes for any script dependent
            // item that is not accessed by the style
            static sal_uInt16 aWhichIds[3][4] =
            {
                { RES_CHRATR_FONT, RES_CHRATR_FONTSIZE,
                    RES_CHRATR_POSTURE, RES_CHRATR_WEIGHT },
                { RES_CHRATR_CJK_FONT, RES_CHRATR_CJK_FONTSIZE,
                    RES_CHRATR_CJK_POSTURE, RES_CHRATR_CJK_WEIGHT },
                { RES_CHRATR_CTL_FONT, RES_CHRATR_CTL_FONTSIZE,
                    RES_CHRATR_CTL_POSTURE, RES_CHRATR_CTL_WEIGHT }
            };

            sal_uInt16 nRef = 0;
            sal_uInt16 aSets[2] = {0,0};
            switch( nCSS1Script )
            {
            case CSS1_OUTMODE_WESTERN:
                nRef = 0;
                aSets[0] = 1;
                aSets[1] = 2;
                break;
            case CSS1_OUTMODE_CJK:
                nRef = 1;
                aSets[0] = 0;
                aSets[1] = 2;
                break;
            case CSS1_OUTMODE_CTL:
                nRef = 2;
                aSets[0] = 0;
                aSets[1] = 1;
                break;
            }
            for( sal_uInt16 i=0; i<4; i++ )
            {
                const SfxPoolItem& rRef = pFmt->GetFmtAttr( aWhichIds[nRef][i] );
                for( sal_uInt16 j=0; j<2; j++ )
                {
                    const SfxPoolItem& rSet = pFmt->GetFmtAttr( aWhichIds[aSets[j]][i] );
                    if( rSet != rRef )
                    {
                        if( !pItemSet )
                            pItemSet = new SfxItemSet( *pFmt->GetAttrSet().GetPool(),
                                                       pFmt->GetAttrSet().GetRanges() );
                        pItemSet->Put( rSet );
                    }
                }
            }
        }

        // Ggf. noch ein DropCap-Attribut uebernehmen
        if( bOutStyles && bHardDrop && nDeep != 0 )
        {
            const SfxPoolItem *pItem;
            if( SFX_ITEM_SET==pFmt->GetAttrSet().GetItemState(
                                    RES_PARATR_DROP, TRUE, &pItem ) )
            {
                BOOL bPut = TRUE;
                if( pTemplate )
                {
                    pReferenceFmt = SwHTMLWriter::GetTemplateFmt( nRefPoolId, pTemplate );
                    const SfxPoolItem *pRefItem;
                    BOOL bRefItemSet =
                        SFX_ITEM_SET==pReferenceFmt->GetAttrSet().GetItemState(
                                        RES_PARATR_DROP, TRUE, &pRefItem );
                    bPut = !bRefItemSet || *pItem!=*pRefItem;
                }
                if( bPut )
                {
                    if( !pItemSet )
                        pItemSet = new SfxItemSet( *pFmt->GetAttrSet().GetPool(),
                                                   pFmt->GetAttrSet().GetRanges() );
                    pItemSet->Put( *pItem );
                }
            }
        }


        // Die diversen default-Abstaende aus der Vorlage oder der
        // Vergleischs-Vorlage merken
        const SvxLRSpaceItem &rLRSpace =
            (pReferenceFmt ? pReferenceFmt : pFmt)->GetLRSpace();
        nLeftMargin = rLRSpace.GetTxtLeft();
        nRightMargin = rLRSpace.GetRight();
        nFirstLineIndent = rLRSpace.GetTxtFirstLineOfst();

        const SvxULSpaceItem &rULSpace =
            (pReferenceFmt ? pReferenceFmt : pFmt)->GetULSpace();
        nTopMargin = rULSpace.GetUpper();
        nBottomMargin = rULSpace.GetLower();

        // export language if it differs from the default language
        sal_uInt16 nWhichId =
            SwHTMLWriter::GetLangWhichIdFromScript( nCSS1Script );
        const SvxLanguageItem& rLang =
            (const SvxLanguageItem&)pFmt->GetFmtAttr( nWhichId );
        LanguageType eLang = rLang.GetLanguage();
        if( eLang != eDfltLang )
        {
            if( !pItemSet )
                pItemSet = new SfxItemSet( *pFmt->GetAttrSet().GetPool(),
                                           pFmt->GetAttrSet().GetRanges() );
            pItemSet->Put( rLang );
        }

        static sal_uInt16 aWhichIds[3] =
            { RES_CHRATR_LANGUAGE, RES_CHRATR_CJK_LANGUAGE,
                RES_CHRATR_CTL_LANGUAGE };
        for( sal_uInt16 i=0; i<3; i++ )
        {
            if( aWhichIds[i] != nWhichId )
            {
                const SvxLanguageItem& rTmpLang =
                    (const SvxLanguageItem&)pFmt->GetFmtAttr(aWhichIds[i]);
                if( rTmpLang.GetLanguage() != eLang )
                {
                    if( !pItemSet )
                        pItemSet = new SfxItemSet( *pFmt->GetAttrSet().GetPool(),
                                                   pFmt->GetAttrSet().GetRanges() );
                    pItemSet->Put( rTmpLang );
                }
            }
        }
    }
}

SwHTMLFmtInfo::~SwHTMLFmtInfo()
{
    delete pItemSet;
}

void OutHTML_SwFmt( Writer& rWrt, const SwFmt& rFmt,
                    const SfxItemSet *pNodeItemSet,
                    SwHTMLTxtCollOutputInfo& rInfo )
{
    ASSERT( RES_CONDTXTFMTCOLL==rFmt.Which() || RES_TXTFMTCOLL==rFmt.Which(),
            "keine Absatz-Vorlage" );

    SwHTMLWriter & rHWrt = (SwHTMLWriter&)rWrt;

    // Erstmal ein par Flags ...
    USHORT nNewDefListLvl = 0;
    USHORT nNumStart = USHRT_MAX;
    BOOL bForceDL = FALSE;
    BOOL bDT = FALSE;
    rInfo.bInNumBulList = FALSE;  	// Wir sind in einer Liste?
    BOOL bNumbered = FALSE;			// Der aktuelle Absatz ist numeriert
    BOOL bPara = FALSE;				// das aktuelle Token ist <P>
    rInfo.bParaPossible = FALSE;	// ein <P> darf zusaetzlich ausgegeben werden
    BOOL bNoEndTag = FALSE;			// kein End-Tag ausgeben

    rHWrt.bNoAlign = FALSE;			// kein ALIGN=... moeglich
    BOOL bNoStyle = FALSE;			// kein STYLE=... moeglich
    BYTE nBulletGrfLvl = 255;	  	// Die auszugebende Bullet-Grafik

    // Sind wir in einer Aufzaehlungs- oder Numerierungliste?
    const SwTxtNode* pTxtNd = rWrt.pCurPam->GetNode()->GetTxtNode();

    SwHTMLNumRuleInfo aNumInfo;
    if( rHWrt.GetNextNumInfo() )
    {
        aNumInfo = *rHWrt.GetNextNumInfo();
        rHWrt.ClearNextNumInfo();
    }
    else
    {
        aNumInfo.Set( *pTxtNd );
    }

    if( aNumInfo.GetNumRule() )
    {
        rInfo.bInNumBulList = TRUE;
        nNewDefListLvl = 0;

        // ist der aktuelle Absatz numeriert?
        bNumbered = aNumInfo.IsNumbered();
        BYTE nLvl = aNumInfo.GetLevel();

        ASSERT( pTxtNd->GetActualListLevel() == nLvl,
                "Gemerkter Num-Level ist falsch" );
        ASSERT( bNumbered == static_cast< BOOL >(pTxtNd->IsCountedInList()),
                "Gemerkter Numerierungs-Zustand ist falsch" );

        if( bNumbered )
        {
            nBulletGrfLvl = nLvl; // nur veruebergehend!!!
            // --> OD 2005-11-15 #i57919#
            // correction of re-factoring done by cws swnumtree:
            // - <nNumStart> has to contain the restart value, if the
            //   numbering is restarted at this text node. Value <USHRT_MAX>
            //   indicates, that no additional restart value has to be written.
            if ( pTxtNd->IsListRestart() )
            {
                nNumStart = static_cast< USHORT >(pTxtNd->GetActualListStartValue());
            }
            // <--
            DBG_ASSERT( rHWrt.nLastParaToken == 0,
                "<PRE> wurde nicht vor <LI> beendet." );
        }
    }

    // Jetzt holen wir das Token und ggf. die Klasse
    SwHTMLFmtInfo aFmtInfo( &rFmt );
    USHORT nArrayPos;
    const SwHTMLFmtInfo *pFmtInfo;
    if( rHWrt.aTxtCollInfos.Seek_Entry( &aFmtInfo, &nArrayPos ) )
    {
        pFmtInfo = rHWrt.aTxtCollInfos[nArrayPos];
    }
    else
    {
        pFmtInfo = new SwHTMLFmtInfo( &rFmt, rWrt.pDoc, rHWrt.pTemplate,
                                      rHWrt.bCfgOutStyles, rHWrt.eLang,
                                      rHWrt.nCSS1Script,
                                      !rHWrt.IsHTMLMode(HTMLMODE_DROPCAPS) );
        rHWrt.aTxtCollInfos.C40_PTR_INSERT( SwHTMLFmtInfo, pFmtInfo );
        String aName( rFmt.GetName() );
        if( rHWrt.aScriptParaStyles.Seek_Entry( &aName ) )
            ((SwHTMLFmtInfo *)pFmtInfo)->bScriptDependent = sal_True;
    }

    // Jetzt wird festgelegt, was aufgrund des Tokens so moeglich ist
    USHORT nToken = 0;			// Token fuer Tag-Wechsel
    BOOL bOutNewLine = FALSE;	// nur ein LF ausgeben?
    if( pFmtInfo->aToken.Len() )
    {
        // Es ist eine HTML-Tag-Vorlage oder die Vorlage ist von einer
        // solchen abgeleitet
        rInfo.aToken = pFmtInfo->aToken;

        // der erste Buchstabe reicht meistens
        switch( rInfo.aToken.GetChar( 0 ) )
        {
        case 'A':	ASSERT( rInfo.aToken.Equals(OOO_STRING_SVTOOLS_HTML_address),
                            "Doch kein ADDRESS?" );
                    rInfo.bParaPossible = TRUE;
                    rHWrt.bNoAlign = TRUE;
                    break;

        case 'B':	ASSERT( rInfo.aToken.Equals(OOO_STRING_SVTOOLS_HTML_blockquote),
                            "Doch kein BLOCKQUOTE?" );
                    rInfo.bParaPossible = TRUE;
                    rHWrt.bNoAlign = TRUE;
                    break;

        case 'P':	if( rInfo.aToken.Len() == 1 )
                    {
                        bPara = TRUE;
                    }
                    else
                    {
                        ASSERT( rInfo.aToken.Equals(OOO_STRING_SVTOOLS_HTML_preformtxt),
                                "Doch kein PRE?" );
                        if( HTML_PREFORMTXT_ON == rHWrt.nLastParaToken )
                        {
                            bOutNewLine = TRUE;
                        }
                        else
                        {
                            nToken = HTML_PREFORMTXT_ON;
                            rHWrt.bNoAlign = TRUE;
                            bNoEndTag = TRUE;
                        }
                    }
                    break;

        case 'D':	ASSERT( rInfo.aToken.Equals(OOO_STRING_SVTOOLS_HTML_dt) ||
                            rInfo.aToken.Equals(OOO_STRING_SVTOOLS_HTML_dd),
                            "Doch kein DD/DT?" );
                    bDT = rInfo.aToken.Equals(OOO_STRING_SVTOOLS_HTML_dt);
                    rInfo.bParaPossible = !bDT;
                    rHWrt.bNoAlign = TRUE;
                    bForceDL = TRUE;
                    break;
        }
    }
    else
    {
        // alle Vorlagen, die nicht einem HTML-Tag entsprechen oder von
        // diesem abgeleitet sind, werden als <P> exportiert

        rInfo.aToken = OOO_STRING_SVTOOLS_HTML_parabreak;
        bPara = TRUE;
    }

    // Falls noetig, die harte Attributierung der Vorlage uebernehmen
    if( pFmtInfo->pItemSet )
    {
        ASSERT( !rInfo.pItemSet, "Wo kommt der Item-Set her?" );
        rInfo.pItemSet = new SfxItemSet( *pFmtInfo->pItemSet );
    }

    // und noch die harte Attributierung des Absatzes dazunehmen
    if( pNodeItemSet )
    {
        if( rInfo.pItemSet )
            rInfo.pItemSet->Put( *pNodeItemSet );
        else
            rInfo.pItemSet = new SfxItemSet( *pNodeItemSet );
    }

    // den unteren Absatz-Abstand brauchen wir noch
    const SvxULSpaceItem& rULSpace =
        pNodeItemSet ? ((const SvxULSpaceItem &)pNodeItemSet->Get(RES_UL_SPACE))
                     : rFmt.GetULSpace();


    if( (rHWrt.bOutHeader &&
         rWrt.pCurPam->GetPoint()->nNode.GetIndex() ==
            rWrt.pCurPam->GetMark()->nNode.GetIndex()) ||
         rHWrt.bOutFooter )
    {
        if( rHWrt.bCfgOutStyles )
        {
            SvxULSpaceItem aULSpaceItem( rULSpace );
            if( rHWrt.bOutHeader )
                aULSpaceItem.SetLower( rHWrt.nHeaderFooterSpace );
            else
                aULSpaceItem.SetUpper( rHWrt.nHeaderFooterSpace );

            if( !rInfo.pItemSet )
                rInfo.pItemSet = new SfxItemSet(
                                    *rFmt.GetAttrSet().GetPool(),
                                    RES_UL_SPACE, RES_UL_SPACE );
            rInfo.pItemSet->Put( aULSpaceItem );
        }
        rHWrt.bOutHeader = FALSE;
        rHWrt.bOutFooter = FALSE;
    }

    if( bOutNewLine )
    {
        // nur einen Zeilen-Umbruch (ohne Einrueckung) am Absatz-Anfang
        // ausgeben
        rInfo.aToken.Erase();	// kein End-Tag ausgeben
        rWrt.Strm() << SwHTMLWriter::sNewLine;

        return;
    }


    // soll ein ALIGN=... geschrieben werden?
    const SfxPoolItem* pAdjItem = 0;
    const SfxPoolItem* pItem;

    if( rInfo.pItemSet &&
        SFX_ITEM_SET == rInfo.pItemSet->GetItemState( RES_PARATR_ADJUST,
                                                      FALSE, &pItem ) )
    {
        pAdjItem = pItem;
    }

    // Unteren Absatz-Abstand beachten ? (nie im letzen Absatz von
    // Tabellen)
    BOOL bUseParSpace =	!rHWrt.bOutTable ||
                        (rWrt.pCurPam->GetPoint()->nNode.GetIndex() !=
                         rWrt.pCurPam->GetMark()->nNode.GetIndex());
    // Wenn Styles exportiert werden, wird aus eingerueckten Absaetzen
    // eine Definitions-Liste
    const SvxLRSpaceItem& rLRSpace =
        pNodeItemSet ? ((const SvxLRSpaceItem &)pNodeItemSet->Get(RES_LR_SPACE))
                     : rFmt.GetLRSpace();
    if( (!rHWrt.bCfgOutStyles || bForceDL) && !rInfo.bInNumBulList )
    {
        sal_Int32 nLeftMargin;
        if( bForceDL )
            nLeftMargin = rLRSpace.GetTxtLeft();
        else
            nLeftMargin = rLRSpace.GetTxtLeft() > pFmtInfo->nLeftMargin
                ? rLRSpace.GetTxtLeft() - pFmtInfo->nLeftMargin
                : 0;

        if( nLeftMargin > 0 && rHWrt.nDefListMargin > 0 )
        {
            nNewDefListLvl = static_cast< USHORT >((nLeftMargin + (rHWrt.nDefListMargin/2)) /
                                                    rHWrt.nDefListMargin);
            if( nNewDefListLvl == 0 && bForceDL && !bDT )
                nNewDefListLvl = 1;
        }
        else
        {
            // If the left margin is 0 or negative, emulating indent
            // with <dd> does not work. We then set a def list only if
            // the dd style is used.
            nNewDefListLvl = (bForceDL&& !bDT) ? 1 : 0;
        }

        BOOL bIsNextTxtNode =
            rWrt.pDoc->GetNodes()[rWrt.pCurPam->GetPoint()->nNode.GetIndex()+1]
                     ->IsTxtNode();

        if( bForceDL && bDT )
        {
            // Statt eines DD muessen wir hier auch ein DT der Ebene
            // darueber nehmen
            nNewDefListLvl++;
        }
        else if( !nNewDefListLvl && !rHWrt.bCfgOutStyles && bPara &&
                 rULSpace.GetLower()==0 &&
                 ((bUseParSpace && bIsNextTxtNode) || rHWrt.nDefListLvl==1) &&
                 (!pAdjItem || SVX_ADJUST_LEFT==
                    ((const SvxAdjustItem *)pAdjItem)->GetAdjust()) )
        {
            // Absaetze ohne unteren Abstand als DT exportieren
            nNewDefListLvl = 1;
            bDT = TRUE;
            rInfo.bParaPossible = FALSE;
            rHWrt.bNoAlign = TRUE;
        }
    }

    if( nNewDefListLvl != rHWrt.nDefListLvl )
        rHWrt.OutAndSetDefList( nNewDefListLvl );

    // ggf. eine Aufzaehlung- oder Numerierungsliste beginnen
    if( rInfo.bInNumBulList )
    {
        ASSERT( !rHWrt.nDefListLvl, "DL in OL geht nicht!" );
        OutHTML_NumBulListStart( rHWrt, aNumInfo );

        if( bNumbered )
        {
            if( rHWrt.aBulletGrfs[nBulletGrfLvl].Len()  )
                bNumbered = FALSE;
            else
                nBulletGrfLvl = 255;
        }
    }

    // Die Defaults aus der Vorlage merken, denn sie muessen nicht
    // exportiert werden
    rHWrt.nDfltLeftMargin = pFmtInfo->nLeftMargin;
    rHWrt.nDfltRightMargin = pFmtInfo->nRightMargin;
    rHWrt.nDfltFirstLineIndent = pFmtInfo->nFirstLineIndent;

    if( rInfo.bInNumBulList )
    {
        if( !rHWrt.IsHTMLMode( HTMLMODE_LSPACE_IN_NUMBUL ) )
            rHWrt.nDfltLeftMargin = rLRSpace.GetTxtLeft();

        // In Numerierungs-Listen keinen Ertzeilen-Einzug ausgeben.
        rHWrt.nFirstLineIndent = rLRSpace.GetTxtFirstLineOfst();
    }

    if( rInfo.bInNumBulList && bNumbered && bPara && !rHWrt.bCfgOutStyles )
    {
        // ein einzelnes LI hat keinen Abstand
        rHWrt.nDfltTopMargin = 0;
        rHWrt.nDfltBottomMargin = 0;
    }
    else if( rHWrt.nDefListLvl && bPara )
    {
        // ein einzelnes DD hat auch keinen Abstand
        rHWrt.nDfltTopMargin = 0;
        rHWrt.nDfltBottomMargin = 0;
    }
    else
    {
        rHWrt.nDfltTopMargin = pFmtInfo->nTopMargin;
        // #60393#: Wenn im letzten Absatz einer Tabelle der
        // untere Absatz-Abstand veraendert wird, vertut sich
        // Netscape total. Deshalb exportieren wir hier erstmal
        // nichts, indem wir den Abstand aus dem Absatz als Default
        // setzen.
        if( rHWrt.bCfgNetscape4 && !bUseParSpace )
            rHWrt.nDfltBottomMargin = rULSpace.GetLower();
        else
            rHWrt.nDfltBottomMargin = pFmtInfo->nBottomMargin;
    }

    if( rHWrt.nDefListLvl )
    {
        rHWrt.nLeftMargin =
            (rHWrt.nDefListLvl-1) * rHWrt.nDefListMargin;
    }

    if( rHWrt.bLFPossible )
        rHWrt.OutNewLine(); // Absatz-Tag in neue Zeile
    rInfo.bOutPara = FALSE;

    // das ist jetzt unser neues Token
    rHWrt.ChangeParaToken( nToken );

    BOOL bHasParSpace = bUseParSpace && rULSpace.GetLower() > 0;

    // ggf ein List-Item aufmachen
    if( rInfo.bInNumBulList && bNumbered )
    {
        ByteString sOut( '<' );
        sOut += OOO_STRING_SVTOOLS_HTML_li;
        if( USHRT_MAX != nNumStart )
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_value) += '=')
                += ByteString::CreateFromInt32(nNumStart);
        sOut += '>';
        rWrt.Strm() << sOut.GetBuffer();
    }

    if( rHWrt.nDefListLvl > 0 && !bForceDL )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), bDT ? OOO_STRING_SVTOOLS_HTML_dt : OOO_STRING_SVTOOLS_HTML_dd );
    }

    if( pAdjItem &&
        rHWrt.IsHTMLMode( HTMLMODE_NO_CONTROL_CENTERING ) &&
        rHWrt.HasControls() )
    {
        // #64687#: The align=... attribute does behave strange in netscape
        // if there are controls in a paragraph, because the control and
        // all text behind the control does not recognize this attribute.
        ByteString sOut( '<' );
        sOut += OOO_STRING_SVTOOLS_HTML_division;
        rWrt.Strm() << sOut.GetBuffer();

        rHWrt.bTxtAttr = FALSE;
        rHWrt.bOutOpts = TRUE;
        OutHTML_SvxAdjust( rWrt, *pAdjItem );
        rWrt.Strm() << '>';
        pAdjItem = 0;
        rHWrt.bNoAlign = FALSE;
        rInfo.bOutDiv = TRUE;
        rHWrt.IncIndentLevel();
        rHWrt.bLFPossible = TRUE;
            rHWrt.OutNewLine();
    }

    // fuer BLOCKQUOTE, ADDRESS und DD wird noch ein Absatz-Token
    // ausgegeben, wenn,
    // - keine Styles geschrieben werden, und
    // - ein untere Abstand oder eine Absatz-Ausrichtung existiert
    ByteString aToken = rInfo.aToken;
    if( !rHWrt.bCfgOutStyles && rInfo.bParaPossible && !bPara &&
        (bHasParSpace || pAdjItem) )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), rInfo.aToken.GetBuffer() );
        aToken = OOO_STRING_SVTOOLS_HTML_parabreak;
        bPara = TRUE;
        rHWrt.bNoAlign = FALSE;
        bNoStyle = FALSE;
    }

    LanguageType eLang = rInfo.pItemSet
        ? ((const SvxLanguageItem&)rInfo.pItemSet->Get(SwHTMLWriter::GetLangWhichIdFromScript(rHWrt.nCSS1Script))).GetLanguage()
           : rHWrt.eLang;

    if( rInfo.pItemSet )
    {
        static sal_uInt16 aWhichIds[3] = { RES_CHRATR_LANGUAGE, RES_CHRATR_CJK_LANGUAGE, RES_CHRATR_CTL_LANGUAGE };

        for( sal_uInt16 i=0; i<3; i++ )
        {
            // export language if it differs from the default language only.
            const SfxPoolItem *pTmpItem;
            if( SFX_ITEM_SET == rInfo.pItemSet->GetItemState( aWhichIds[i],
                        sal_True, &pTmpItem ) &&
                ((const SvxLanguageItem *)pTmpItem)->GetLanguage() == eLang )
                rInfo.pItemSet->ClearItem( aWhichIds[i] );
        }
    }

    // and the text direction
    sal_uInt16 nDir = rHWrt.GetHTMLDirection(
            (pNodeItemSet ? static_cast < const SvxFrameDirectionItem& >(
                                    pNodeItemSet->Get( RES_FRAMEDIR ) )
                          : rFmt.GetFrmDir() ).GetValue() );

    // Ein <P> wird nur geschrieben, wenn
    // - wir in keiner OL/UL/DL sind, oder
    // - der Absatz einer OL/UL nicht numeriert ist, oder
    // - keine Styles exportiert werden und
    //      - ein unterer Abstand oder
    //      - eine Absatz-Ausrichtung existiert, ode
    // - Styles exportiert werden und,
    //		- die Textkoerper-Vorlage geaendert wurde, oder
    //   	- ein Benutzer-Format exportiert wird, oder
    //		- Absatz-Attribute existieren
    if( !bPara ||
        (!rInfo.bInNumBulList && !rHWrt.nDefListLvl) ||
        (rInfo.bInNumBulList && !bNumbered) ||
        (!rHWrt.bCfgOutStyles &&
         (bHasParSpace || pAdjItem ||
          (eLang != LANGUAGE_DONTKNOW && eLang != rHWrt.eLang))) ||
        nDir != rHWrt.nDirection ||
        rHWrt.bCfgOutStyles )
    {
        // jetzt werden Optionen ausgegeben
        rHWrt.bTxtAttr = FALSE;
        rHWrt.bOutOpts = TRUE;

        ByteString sOut( '<' );
        sOut += aToken;

        if( eLang != LANGUAGE_DONTKNOW && eLang != rHWrt.eLang )
        {
            rWrt.Strm() << sOut.GetBuffer();
            rHWrt.OutLanguage( eLang );
            sOut.Erase();
        }

        if( nDir != rHWrt.nDirection )
        {
            if( sOut.Len() )
            {
                rWrt.Strm() << sOut.GetBuffer();
                sOut.Erase();
            }
            rHWrt.OutDirection( nDir );
        }

        if( rHWrt.bCfgOutStyles &&
            (pFmtInfo->aClass.Len() || pFmtInfo->bScriptDependent) )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_class) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            String aClass( pFmtInfo->aClass );
            if( pFmtInfo->bScriptDependent )
            {
                if( aClass.Len() )
                   aClass += '-';
                switch( rHWrt.nCSS1Script )
                {
                case CSS1_OUTMODE_WESTERN:
                    aClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("western") );
                    break;
                case CSS1_OUTMODE_CJK:
                    aClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("cjk") );
                    break;
                case CSS1_OUTMODE_CTL:
                    aClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("ctl") );
                    break;
                }
            }
            HTMLOutFuncs::Out_String( rWrt.Strm(), aClass,
                                      rHWrt.eDestEnc, &rHWrt.aNonConvertableCharacters );
            sOut = '\"';
        }
        rWrt.Strm() << sOut.GetBuffer();

        // ggf. Ausrichtung ausgeben.
        if( !rHWrt.bNoAlign && pAdjItem )
            OutHTML_SvxAdjust( rWrt, *pAdjItem );

        // und nun ggf. noch die STYLE-Option
        if( rHWrt.bCfgOutStyles && rInfo.pItemSet && !bNoStyle)
        {
            OutCSS1_ParaTagStyleOpt( rWrt, *rInfo.pItemSet );
        }

        rWrt.Strm() << '>';

        // Soll ein </P> geschrieben wenrden
        rInfo.bOutPara =
            bPara &&
            ( rHWrt.bCfgOutStyles ||
                (!rHWrt.bCfgOutStyles && bHasParSpace) );

        // wenn kein End-Tag geschrieben werden soll, es loeschen
        if( bNoEndTag )
            rInfo.aToken.Erase();
    }

    // ??? Warum nicht ueber den Hint-Mechanismus ???
    if( rHWrt.IsHTMLMode(HTMLMODE_FIRSTLINE) )
    {
        const SvxLRSpaceItem& rLRSpaceTmp =
            pNodeItemSet ? ((const SvxLRSpaceItem &)pNodeItemSet->Get(RES_LR_SPACE))
                         : rFmt.GetLRSpace();
        if( rLRSpaceTmp.GetTxtFirstLineOfst() > 0 )
        {
            OutHTML_HoriSpacer( rWrt, rLRSpaceTmp.GetTxtFirstLineOfst() );
        }
    }

    if( nBulletGrfLvl != 255 )
    {
        ASSERT( aNumInfo.GetNumRule(), "Wo ist die Numerierung geblieben???" );
        ASSERT( nBulletGrfLvl < MAXLEVEL, "So viele Ebenen gibt's nicht" );
        const SwNumFmt& rNumFmt = aNumInfo.GetNumRule()->Get(nBulletGrfLvl);

        OutHTML_BulletImage( rWrt, OOO_STRING_SVTOOLS_HTML_image, 0,
                             rHWrt.aBulletGrfs[nBulletGrfLvl],
                             rNumFmt.GetGraphicSize(), rNumFmt.GetGraphicOrientation() );
    }

    rHWrt.GetNumInfo() = aNumInfo;

    // die Defaults zuruecksetzen
    rHWrt.nDfltLeftMargin = 0;
    rHWrt.nDfltRightMargin = 0;
    rHWrt.nDfltFirstLineIndent = 0;
    rHWrt.nDfltTopMargin = 0;
    rHWrt.nDfltBottomMargin = 0;
    rHWrt.nLeftMargin = 0;
    rHWrt.nFirstLineIndent = 0;
}

void OutHTML_SwFmtOff( Writer& rWrt, const SwHTMLTxtCollOutputInfo& rInfo )
{
    SwHTMLWriter & rHWrt = (SwHTMLWriter&)rWrt;

    // wenn es kein Token gibt haben wir auch nichts auszugeben
    if( !rInfo.aToken.Len() )
    {
        rHWrt.FillNextNumInfo();
        const SwHTMLNumRuleInfo& rNextInfo = *rHWrt.GetNextNumInfo();
        // Auch in PRE muss eine Bullet-Liste beendet werden
        if( rInfo.bInNumBulList )
        {

            const SwHTMLNumRuleInfo& rNRInfo = rHWrt.GetNumInfo();
            if( rNextInfo.GetNumRule() != rNRInfo.GetNumRule() ||
                rNextInfo.GetDepth() != rNRInfo.GetDepth() ||
                rNextInfo.IsNumbered() || rNextInfo.IsRestart() )
                rHWrt.ChangeParaToken( 0 );
            OutHTML_NumBulListEnd( rHWrt, rNextInfo );
        }
        else if( rNextInfo.GetNumRule() != 0 )
            rHWrt.ChangeParaToken( 0 );

        return;
    }

    if( rInfo.ShouldOutputToken() )
    {
        if( rHWrt.bLFPossible )
            rHWrt.OutNewLine( TRUE );

        // fuer BLOCKQUOTE, ADDRESS und DD wird ggf noch ein
        // Absatz-Token ausgegeben, wenn
        // - keine Styles geschrieben werden, und
        // - ein untere Abstand existiert
        if( rInfo.bParaPossible && rInfo.bOutPara )
            HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_parabreak, FALSE );

        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), rInfo.aToken.GetBuffer(),
                                    FALSE );
        rHWrt.bLFPossible = !rInfo.aToken.Equals( OOO_STRING_SVTOOLS_HTML_dt ) &&
                            !rInfo.aToken.Equals( OOO_STRING_SVTOOLS_HTML_dd ) &&
                            !rInfo.aToken.Equals( OOO_STRING_SVTOOLS_HTML_li );
    }
    if( rInfo.bOutDiv )
    {
        rHWrt.DecIndentLevel();
        if( rHWrt.bLFPossible )
            rHWrt.OutNewLine();
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_division, FALSE );
        rHWrt.bLFPossible = TRUE;
    }

    // ggf. eine Aufzaehlung- oder Numerierungsliste beenden
    if( rInfo.bInNumBulList )
    {
        rHWrt.FillNextNumInfo();
        OutHTML_NumBulListEnd( rHWrt, *rHWrt.GetNextNumInfo() );
    }
}


class HTMLSttEndPos
{
    xub_StrLen nStart;
    xub_StrLen nEnd;
    SfxPoolItem* pItem;

public:

    HTMLSttEndPos( const SfxPoolItem& rItem, xub_StrLen nStt, xub_StrLen nE );
    ~HTMLSttEndPos();

    const SfxPoolItem *GetItem() const { return pItem; }

    void SetStart( xub_StrLen nStt ) { nStart = nStt; }
    xub_StrLen GetStart() const { return nStart; }

    xub_StrLen GetEnd() const { return nEnd; }
    void SetEnd( xub_StrLen nE ) { nEnd = nE; }
};

HTMLSttEndPos::HTMLSttEndPos( const SfxPoolItem& rItem, xub_StrLen nStt,
                                                        xub_StrLen nE ) :
    nStart( nStt ),
    nEnd( nE ),
    pItem( rItem.Clone() )
{}

HTMLSttEndPos::~HTMLSttEndPos()
{
    delete pItem;
}

typedef HTMLSttEndPos *HTMLSttEndPosPtr;
SV_DECL_PTRARR( _HTMLEndLst, HTMLSttEndPosPtr, 5, 5 )

enum HTMLOnOffState { HTML_NOT_SUPPORTED, 	// nicht unterst. Attribut
                      HTML_REAL_VALUE, 		// Attribut mit Wert
                      HTML_ON_VALUE, 		// Attribut entspricht On-Tag
                      HTML_OFF_VALUE,		// Attribut entspricht Off-Tag
                      HTML_CHRFMT_VALUE,	// Attribut fuer Zeichenvorlage
                      HTML_COLOR_VALUE,		// Attribut fuer Vordergrundfarbe
                      HTML_STYLE_VALUE,		// Attribut muss als Style exp.
                      HTML_DROPCAP_VALUE, 	// DropCap-Attributs
                      HTML_AUTOFMT_VALUE };	// Attribute for automatic character styles


class HTMLEndPosLst
{
    _HTMLEndLst aStartLst;	// nach Anfangs-Psoitionen sortierte Liste
    _HTMLEndLst aEndLst;	// nach End-Psotionen sortierte Liste
    SvXub_StrLens aScriptChgLst;	// positions where script changes
                                    // 0 is not contained in this list,
                                    // but the text length
    SvUShorts aScriptLst;	// the script that is valif up to the position
                            // contained in aScriptChgList at the same index

    SwDoc *pDoc;			// das aktuelle Dokument
    SwDoc* pTemplate;		// die HTML-Vorlage (oder 0)
    const Color* pDfltColor;// die Default-Vordergrund-Farbe
    SvStringsSortDtor& rScriptTxtStyles;	//

    ULONG nHTMLMode;
    BOOL bOutStyles : 1;	// werden Styles exportiert


    // die Position eines Items in der Start-/Ende-Liste suchen
    USHORT _FindStartPos( const HTMLSttEndPos *pPos ) const;
    USHORT _FindEndPos( const HTMLSttEndPos *pPos ) const;

    // Eine SttEndPos in die Start- und Ende-Listen eintragen bzw. aus
    // ihnen loeschen, wobei die Ende-Position bekannt ist
    void _InsertItem( HTMLSttEndPos *pPos, USHORT nEndPos );
    void _RemoveItem( USHORT nEndPos );

    // die "Art" es Attributs ermitteln
    HTMLOnOffState GetHTMLItemState( const SfxPoolItem& rItem );

    // Existiert ein bestimmtes On-Tag-Item
    BOOL ExistsOnTagItem( USHORT nWhich, xub_StrLen nPos );

    // Existiert ein Item zum ausschalten eines Attributs, das genauso
    // exportiert wird wie das uebergebene Item im gleichen Bereich?
    BOOL ExistsOffTagItem( USHORT nWhich, xub_StrLen nStartPos,
                                          xub_StrLen nEndPos );


    // das Ende eines gesplitteten Items anpassen
    void FixSplittedItem( HTMLSttEndPos *pPos, USHORT nStartPos,
                          xub_StrLen nNewEnd );

    // Ein Attribut in die Listen eintragen und ggf. aufteilen
    void InsertItem( const SfxPoolItem& rItem, xub_StrLen nStart,
                                               xub_StrLen nEnd );

    // Ein bereits vorhandenes Attribut aufteilen
    void SplitItem( const SfxPoolItem& rItem, xub_StrLen nStart,
                                              xub_StrLen nEnd );

    // Insert without taking care of script
    void InsertNoScript( const SfxPoolItem& rItem, xub_StrLen nStart,
                          xub_StrLen nEnd, SwHTMLFmtInfos& rFmtInfos,
                         BOOL bParaAttrs=FALSE );

    const SwHTMLFmtInfo *GetFmtInfo( const SwFmt& rFmt,
                                     SwHTMLFmtInfos& rFmtInfos );

public:

    HTMLEndPosLst( SwDoc *pDoc, SwDoc* pTemplate, const Color* pDfltColor,
                   BOOL bOutStyles, ULONG nHTMLMode,
                   const String& rText, SvStringsSortDtor& rStyles );
    ~HTMLEndPosLst();

    // Ein Attribut einfuegen
    void Insert( const SfxPoolItem& rItem, xub_StrLen nStart,  xub_StrLen nEnd,
                 SwHTMLFmtInfos& rFmtInfos, BOOL bParaAttrs=FALSE );
    void Insert( const SfxItemSet& rItemSet, xub_StrLen nStart, xub_StrLen nEnd,
                 SwHTMLFmtInfos& rFmtInfos, BOOL bDeep,
                 BOOL bParaAttrs=FALSE );
    void Insert( const SwDrawFrmFmt& rFmt, xub_StrLen nPos,
                 SwHTMLFmtInfos& rFmtInfos );

    sal_uInt16 GetScriptAtPos( xub_StrLen nPos,
                               sal_uInt16 nWeak=CSS1_OUTMODE_ANY_SCRIPT );

    void OutStartAttrs( SwHTMLWriter& rHWrt, xub_StrLen nPos,
                        HTMLOutContext *pContext = 0 );
    void OutEndAttrs( SwHTMLWriter& rHWrt, xub_StrLen nPos,
                      HTMLOutContext *pContext = 0 );

    USHORT Count() const { return aEndLst.Count(); }

    BOOL IsHTMLMode( ULONG nMode ) const { return (nHTMLMode & nMode) != 0; }
};


USHORT HTMLEndPosLst::_FindStartPos( const HTMLSttEndPos *pPos ) const
{
    USHORT i;
    for( i = 0; i < aStartLst.Count() && aStartLst[i] != pPos;  i++ )
        ;

    ASSERT( i != aStartLst.Count(), "Item nicht in Start-Liste gefunden!" );

    return i==aStartLst.Count() ? USHRT_MAX : i;
}

USHORT HTMLEndPosLst::_FindEndPos( const HTMLSttEndPos *pPos ) const
{
    USHORT i;

    for( i = 0; i < aEndLst.Count() && aEndLst[i] != pPos;  i++ )
        ;

    ASSERT( i != aEndLst.Count(), "Item nicht in Ende-Liste gefunden" );

    return i==aEndLst.Count() ? USHRT_MAX : i;
}


void HTMLEndPosLst::_InsertItem( HTMLSttEndPos *pPos, USHORT nEndPos )
{
    // In der Start-Liste das Attribut hinter allen vorher und an
    // der gleichen Position gestarteten Attributen einfuegen
    xub_StrLen nStart = pPos->GetStart();
    USHORT i;

    for( i = 0; i < aStartLst.Count() &&
                     aStartLst[i]->GetStart() <= nStart; i++ )
        ;
    aStartLst.Insert( pPos, i );

    // die Position in der Ende-Liste wurde uebergeben
    aEndLst.Insert( pPos, nEndPos );
}

void HTMLEndPosLst::_RemoveItem( USHORT nEndPos )
{
    HTMLSttEndPos *pPos = aEndLst[nEndPos];

    // jetzt Suchen wir es in der Start-Liste
    USHORT nStartPos = _FindStartPos( pPos );
    if( nStartPos != USHRT_MAX )
        aStartLst.Remove( nStartPos, 1 );

    aEndLst.Remove( nEndPos, 1 );

    delete pPos;
}

HTMLOnOffState HTMLEndPosLst::GetHTMLItemState( const SfxPoolItem& rItem )
{
    HTMLOnOffState eState = HTML_NOT_SUPPORTED;
    switch( rItem.Which() )
    {
    case RES_CHRATR_POSTURE:
    case RES_CHRATR_CJK_POSTURE:
    case RES_CHRATR_CTL_POSTURE:
        switch( ((const SvxPostureItem&)rItem).GetPosture() )
        {
        case ITALIC_NORMAL:
            eState = HTML_ON_VALUE;
            break;
        case ITALIC_NONE:
            eState = HTML_OFF_VALUE;
            break;
        default:
            if( IsHTMLMode(HTMLMODE_SOME_STYLES) )
                eState = HTML_STYLE_VALUE;
            break;
        }
        break;

    case RES_CHRATR_CROSSEDOUT:
        switch( ((const SvxCrossedOutItem&)rItem).GetStrikeout() )
        {
        case STRIKEOUT_SINGLE:
        case STRIKEOUT_DOUBLE:
            eState = HTML_ON_VALUE;
            break;
        case STRIKEOUT_NONE:
            eState = HTML_OFF_VALUE;
            break;
        default:
            ;
        }
        break;

    case RES_CHRATR_ESCAPEMENT:
        switch( (const SvxEscapement)
                        ((const SvxEscapementItem&)rItem).GetEnumValue() )
        {
        case SVX_ESCAPEMENT_SUPERSCRIPT:
        case SVX_ESCAPEMENT_SUBSCRIPT:
            eState = HTML_ON_VALUE;
            break;
        case SVX_ESCAPEMENT_OFF:
            eState = HTML_OFF_VALUE;
            break;
        default:
            ;
        }
        break;

    case RES_CHRATR_UNDERLINE:
        switch( ((const SvxUnderlineItem&)rItem).GetLineStyle() )
        {
        case UNDERLINE_SINGLE:
            eState = HTML_ON_VALUE;
            break;
        case UNDERLINE_NONE:
            eState = HTML_OFF_VALUE;
            break;
        default:
            if( IsHTMLMode(HTMLMODE_SOME_STYLES) )
                eState = HTML_STYLE_VALUE;
            break;
        }
        break;

    case RES_CHRATR_OVERLINE:
        if( IsHTMLMode(HTMLMODE_SOME_STYLES) )
            eState = HTML_STYLE_VALUE;
        break;

    case RES_CHRATR_WEIGHT:
    case RES_CHRATR_CJK_WEIGHT:
    case RES_CHRATR_CTL_WEIGHT:
        switch( ((const SvxWeightItem&)rItem).GetWeight() )
        {
        case WEIGHT_BOLD:
            eState = HTML_ON_VALUE;
            break;
        case WEIGHT_NORMAL:
            eState = HTML_OFF_VALUE;
            break;
        default:
            if( IsHTMLMode(HTMLMODE_SOME_STYLES) )
                eState = HTML_STYLE_VALUE;
            break;
        }
        break;

    case RES_CHRATR_BLINK:
        if( IsHTMLMode(HTMLMODE_BLINK) )
            eState = ((const SvxBlinkItem&)rItem).GetValue() ? HTML_ON_VALUE
                                                             : HTML_OFF_VALUE;
        break;

    case RES_CHRATR_COLOR:
        eState = HTML_COLOR_VALUE;
        break;

    case RES_CHRATR_FONT:
    case RES_CHRATR_FONTSIZE:
    case RES_CHRATR_LANGUAGE:
    case RES_CHRATR_CJK_FONT:
    case RES_CHRATR_CJK_FONTSIZE:
    case RES_CHRATR_CJK_LANGUAGE:
    case RES_CHRATR_CTL_FONT:
    case RES_CHRATR_CTL_FONTSIZE:
    case RES_CHRATR_CTL_LANGUAGE:
    case RES_TXTATR_INETFMT:
        eState = HTML_REAL_VALUE;
        break;

    case RES_TXTATR_CHARFMT:
        eState = HTML_CHRFMT_VALUE;
        break;

    case RES_TXTATR_AUTOFMT:
        eState = HTML_AUTOFMT_VALUE;
        break;

    case RES_CHRATR_CASEMAP:
        if( IsHTMLMode(HTMLMODE_SMALL_CAPS) )
            eState = HTML_STYLE_VALUE;

    case RES_CHRATR_KERNING:
        if( IsHTMLMode(HTMLMODE_FULL_STYLES) )
            eState = HTML_STYLE_VALUE;
        break;

    case RES_CHRATR_BACKGROUND:
        if( IsHTMLMode(HTMLMODE_SOME_STYLES) )
            eState = HTML_STYLE_VALUE;
        break;

    case RES_PARATR_DROP:
        eState = HTML_DROPCAP_VALUE;
        break;

//	default:
//		eState = HTML_NOT_SUPPORTED;
//		break;
    }

    return eState;
}

BOOL HTMLEndPosLst::ExistsOnTagItem( USHORT nWhich, xub_StrLen nPos )
{
    for( USHORT i=0; i<aStartLst.Count(); i++ )
    {
        HTMLSttEndPos *pTest = aStartLst[i];

        if( pTest->GetStart() > nPos )
        {
            // dieses uns alle folgenden Attribute beginnen spaeter
            break;
        }
        else if( pTest->GetEnd() > nPos )
        {
            // das Attribut beginnt vor oder an der aktuellen Position
            // und endet hinter ihr
            const SfxPoolItem *pItem = pTest->GetItem();
            if( pItem->Which() == nWhich &&
                HTML_ON_VALUE == GetHTMLItemState(*pItem) )
            {
                // ein On-Tag-Attibut wurde gefunden
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL HTMLEndPosLst::ExistsOffTagItem( USHORT nWhich, xub_StrLen nStartPos,
                                      xub_StrLen nEndPos )
{
    if( nWhich != RES_CHRATR_CROSSEDOUT &&
        nWhich != RES_CHRATR_UNDERLINE &&
        nWhich != RES_CHRATR_BLINK )
    {
        return FALSE;
    }

    for( USHORT i=0; i<aStartLst.Count(); i++ )
    {
        HTMLSttEndPos *pTest = aStartLst[i];

        if( pTest->GetStart() > nStartPos )
        {
            // dieses uns alle folgenden Attribute beginnen spaeter
            break;
        }
        else if( pTest->GetStart()==nStartPos &&
                 pTest->GetEnd()==nEndPos )
        {
            // das Attribut beginnt vor oder an der aktuellen Position
            // und endet hinter ihr
            const SfxPoolItem *pItem = pTest->GetItem();
            USHORT nTstWhich = pItem->Which() ;
            if( (nTstWhich == RES_CHRATR_CROSSEDOUT ||
                 nTstWhich == RES_CHRATR_UNDERLINE ||
                 nTstWhich == RES_CHRATR_BLINK) &&
                HTML_OFF_VALUE == GetHTMLItemState(*pItem) )
            {
                // Ein Off-Tag-Attibut wurde gefunden, das genauso
                // exportiert wird, wie das aktuelle Item
                return TRUE;
            }
        }
    }

    return FALSE;
}

void HTMLEndPosLst::FixSplittedItem( HTMLSttEndPos *pPos, xub_StrLen nNewEnd,
                                     USHORT nStartPos )
{
    // die End-Position entsprechend fixen
    pPos->SetEnd( nNewEnd );

    // das Item aus der End-Liste entfernen
    USHORT nEndPos = _FindEndPos( pPos );
    if( nEndPos != USHRT_MAX )
        aEndLst.Remove( nEndPos, 1 );

    // es wird von nun an als letztes an der entsprechenden Position
    // beendet
    for( nEndPos=0; nEndPos < aEndLst.Count() &&
                    aEndLst[nEndPos]->GetEnd() <= nNewEnd; nEndPos++ )
        ;
    aEndLst.Insert( pPos, nEndPos );

    // jetzt noch die spaeter gestarteten Attribute anpassen
    for( USHORT i=nStartPos+1; i<aStartLst.Count(); i++ )
    {
        HTMLSttEndPos *pTest = aStartLst[i];
        xub_StrLen nTestEnd = pTest->GetEnd();
        if( pTest->GetStart() >= nNewEnd )
        {
            // das Test-Attribut und alle folgenden beginnen, nachdem das
            // gesplittete Attribut endet
            break;
        }
        else if( nTestEnd > nNewEnd )
        {
            // das Test-Attribut beginnt, bevor das gesplittete Attribut
            // endet und endet danach, muss also auch gesplittet werden

            // das neue Ende setzen
            pTest->SetEnd( nNewEnd );

            // das Attribut aus der End-Liste entfernen
            USHORT nEPos = _FindEndPos( pTest );
            if( nEPos != USHRT_MAX )
                aEndLst.Remove( nEPos, 1 );

            // es endet jetzt als erstes Attribut an der entsprechenden
            // Position. Diese Position in der Ende-Liste kennen wir schon.
            aEndLst.Insert(pTest, nEndPos );

            // den "Rest" des Attributs neu einfuegen
            InsertItem( *pTest->GetItem(), nNewEnd, nTestEnd );
        }
    }
}


void HTMLEndPosLst::InsertItem( const SfxPoolItem& rItem, xub_StrLen nStart,
                                                          xub_StrLen nEnd )
{
    USHORT i;
    for( i = 0; i < aEndLst.Count(); i++ )
    {
        HTMLSttEndPos *pTest = aEndLst[i];
        xub_StrLen nTestEnd = pTest->GetEnd();
        if( nTestEnd <= nStart )
        {
            // das Test-Attribut endet, bevor das neue beginnt
            continue;
        }
        else if( nTestEnd < nEnd )
        {
            // das Test-Attribut endet, bevor das neue endet. Das
            // neue Attribut muss deshalb aufgesplittet werden
            _InsertItem( new HTMLSttEndPos( rItem, nStart, nTestEnd ), i );
            nStart = nTestEnd;
        }
        else
        {
            // das Test-Attribut (und alle folgenden) endet, bevor das neue
            // endet
            break;
        }
    }

    // ein Attribut muss noch eingefuegt werden
    _InsertItem( new HTMLSttEndPos( rItem, nStart, nEnd ), i );
}



void HTMLEndPosLst::SplitItem( const SfxPoolItem& rItem, xub_StrLen nStart,
                                                           xub_StrLen nEnd )
{
    USHORT nWhich = rItem.Which();

    // erstmal muessen wir die alten Items anhand der Startliste suchen
    // und die neuen Item-Bereiche festlegen

    for( USHORT i=0; i<aStartLst.Count(); i++ )
    {
        HTMLSttEndPos *pTest = aStartLst[i];
        xub_StrLen nTestStart = pTest->GetStart();
        xub_StrLen nTestEnd = pTest->GetEnd();

        if( nTestStart >= nEnd )
        {
            // dieses und alle nachfolgenden Attribute beginnen spaeter
            break;
        }
        else if( nTestEnd > nStart )
        {
            // das Test Attribut endet im zu loeschenenden Bereich
            const SfxPoolItem *pItem = pTest->GetItem();

            // nur entsprechende On-Tag Attribute muessen beruecksichtigt
            // werden
            if( pItem->Which() == nWhich &&
                HTML_ON_VALUE == GetHTMLItemState( *pItem ) )
            {
                BOOL bDelete = TRUE;

                if( nTestStart < nStart )
                {
                    // der Start des neuen Attribut entspricht
                    // dem neuen Ende des Attribts
                    FixSplittedItem( pTest, nStart, i );
                    bDelete = FALSE;
                }
                else
                {
                    // das Test-Item beginnt erst hinter dem neuen
                    // Ende des Attribts und kann deshalb komplett
                    // geloescht werden
                    aStartLst.Remove( i, 1 );
                    i--;

                    USHORT nEndPos = _FindEndPos( pTest );
                    if( nEndPos != USHRT_MAX )
                        aEndLst.Remove( nEndPos, 1 );
                }

                // ggf den zweiten Teil des gesplitteten Attribts einfuegen
                if( nTestEnd > nEnd )
                {
                    InsertItem( *pTest->GetItem(), nEnd, nTestEnd );
                }

                if( bDelete )
                    delete pTest;
            }
        }
    }
}

const SwHTMLFmtInfo *HTMLEndPosLst::GetFmtInfo( const SwFmt& rFmt,
                                                SwHTMLFmtInfos& rFmtInfos )
{
    const SwHTMLFmtInfo *pFmtInfo;
    SwHTMLFmtInfo aFmtInfo( &rFmt );
    USHORT nPos;
    if( rFmtInfos.Seek_Entry( &aFmtInfo, &nPos ) )
    {
        pFmtInfo = rFmtInfos[nPos];
    }
    else
    {
        pFmtInfo = new SwHTMLFmtInfo( &rFmt, pDoc, pTemplate,
                                      bOutStyles );
        rFmtInfos.C40_PTR_INSERT( SwHTMLFmtInfo, pFmtInfo );
        String aName( rFmt.GetName() );
        if( rScriptTxtStyles.Seek_Entry( &aName ) )
            ((SwHTMLFmtInfo *)pFmtInfo)->bScriptDependent = sal_True;
    }

    return pFmtInfo;
}

HTMLEndPosLst::HTMLEndPosLst( SwDoc *pD, SwDoc* pTempl,
                              const Color* pDfltCol, BOOL bStyles,
                              ULONG nMode, const String& rText,
                              SvStringsSortDtor& rStyles ):
    pDoc( pD ),
    pTemplate( pTempl ),
    pDfltColor( pDfltCol ),
    rScriptTxtStyles( rStyles ),
    nHTMLMode( nMode ),
    bOutStyles( bStyles )
{
    xub_StrLen nEndPos = rText.Len();
    xub_StrLen nPos = 0;
    while( nPos < nEndPos )
    {
        sal_uInt16 nScript = pBreakIt->GetBreakIter()->getScriptType( rText, nPos );
        nPos = (xub_StrLen)pBreakIt->GetBreakIter()->endOfScript( rText, nPos, nScript );
        aScriptChgLst.Insert( nPos, aScriptChgLst.Count() );
        aScriptLst.Insert( nScript, aScriptLst.Count() );
    }
}

HTMLEndPosLst::~HTMLEndPosLst()
{
    ASSERT( !aStartLst.Count(), "Start-Liste im Destruktor nicht leer" );
    ASSERT( !aEndLst.Count(), "End-Liste im Destruktor nicht leer" );
}



void HTMLEndPosLst::InsertNoScript( const SfxPoolItem& rItem,
                            xub_StrLen nStart, xub_StrLen nEnd,
                            SwHTMLFmtInfos& rFmtInfos, BOOL bParaAttrs )
{
    // kein Bereich ?? dann nicht aufnehmen, wird nie wirksam !!
    if( nStart != nEnd )
    {
        BOOL bSet = FALSE, bSplit = FALSE;
        switch( GetHTMLItemState(rItem) )
        {
        case HTML_ON_VALUE:
            // das Attribut wird ausgegeben, wenn es nicht sowieso
            // schon an ist
            if( !ExistsOnTagItem( rItem.Which(), nStart ) )
                bSet = TRUE;
            break;

        case HTML_OFF_VALUE:
            // wenn das entsprechne Attribut an ist, wird es gesplittet,
            // Zusaetlich wird es aber als Style ausgegeben, wenn es nicht
            // am ganzen Absatz gesetzt ist, weil es dann ja schon mit dem
            // ABsatz-Tag ausgegeben wurde.
            if( ExistsOnTagItem( rItem.Which(), nStart ) )
                bSplit = TRUE;
            bSet = bOutStyles && !bParaAttrs &&
                   !ExistsOffTagItem( rItem.Which(), nStart, nEnd );
            break;

        case HTML_REAL_VALUE:
            // das Attribut kann immer ausgegeben werden
            bSet = TRUE;
            break;

        case HTML_STYLE_VALUE:
            // Das Attribut kann nur als CSS1 ausgegeben werden. Wenn
            // es am Absatz gesetzt ist, wurde es schon mit dem
            // Absatz-Tag ausgegeben. Einzige Ausnahme ist das
            // Zeichen-Hintergrund-Attribut. Es muss immer wie ein
            // Hint behandelt werden.
            bSet = bOutStyles &&
                   (!bParaAttrs
                  || rItem.Which()==RES_CHRATR_BACKGROUND
                  || rItem.Which()==RES_CHRATR_OVERLINE);
            break;

        case HTML_CHRFMT_VALUE:
            {
                ASSERT( RES_TXTATR_CHARFMT == rItem.Which(),
                        "Doch keine Zeichen-Vorlage" );
                const SwFmtCharFmt& rChrFmt = (const SwFmtCharFmt&)rItem;
                const SwCharFmt* pFmt = rChrFmt.GetCharFmt();

                const SwHTMLFmtInfo *pFmtInfo = GetFmtInfo( *pFmt, rFmtInfos );
                if( pFmtInfo->aToken.Len() )
                {
                    // das Zeichenvorlagen-Tag muss vor den harten
                    // Attributen ausgegeben werden
                    InsertItem( rItem, nStart, nEnd );
                }
                if( pFmtInfo->pItemSet )
                {
                    Insert( *pFmtInfo->pItemSet, nStart, nEnd,
                            rFmtInfos, TRUE, bParaAttrs );
                }
            }
            break;

        case HTML_AUTOFMT_VALUE:
            {
                const SwFmtAutoFmt& rAutoFmt = (const SwFmtAutoFmt&)rItem;
                const boost::shared_ptr<SfxItemSet> pSet = rAutoFmt.GetStyleHandle();
                if( pSet.get() )
                    Insert( *pSet.get(), nStart, nEnd, rFmtInfos, TRUE, bParaAttrs );
            }
            break;

        case HTML_COLOR_VALUE:
            // Eine Vordergrund-Farbe als Absatz-Attribut wird nur
            // exportiert, wenn sie nicht der Default-Farbe entspricht.
            {
                ASSERT( RES_CHRATR_COLOR == rItem.Which(),
                        "Doch keine Vordergrund-Farbe" );
                Color aColor( ((const SvxColorItem&)rItem).GetValue() );
                if( COL_AUTO == aColor.GetColor() )
                    aColor.SetColor( COL_BLACK );
                bSet = !bParaAttrs || !pDfltColor ||
                       !pDfltColor->IsRGBEqual( aColor );
            }
            break;

        case HTML_DROPCAP_VALUE:
            {
                ASSERT( RES_PARATR_DROP == rItem.Which(),
                        "Doch kein Drop-Cap" );
                const SwFmtDrop& rDrop = (const SwFmtDrop&)rItem;
                nEnd = nStart + rDrop.GetChars();
                if( !bOutStyles )
                {
                    // Zumindest die Attribute der Zeichenvorlage uebernehmen
                    const SwCharFmt *pCharFmt = rDrop.GetCharFmt();
                    if( pCharFmt )
                    {
                        Insert( pCharFmt->GetAttrSet(), nStart, nEnd,
                                rFmtInfos, TRUE, bParaAttrs );
                    }
                }
                else
                {
                    bSet = TRUE;
                }
            }
            break;
        default:
            ;
        }

        if( bSet )
            InsertItem( rItem, nStart, nEnd );
        if( bSplit )
            SplitItem( rItem, nStart, nEnd );
    }
}

void HTMLEndPosLst::Insert( const SfxPoolItem& rItem,
                            xub_StrLen nStart, xub_StrLen nEnd,
                            SwHTMLFmtInfos& rFmtInfos, BOOL bParaAttrs )
{
    sal_Bool bDependsOnScript = sal_False, bDependsOnAnyScript = sal_False;
    sal_uInt16 nScript = i18n::ScriptType::LATIN;
    switch( rItem.Which() )
    {
    case RES_CHRATR_FONT:
    case RES_CHRATR_FONTSIZE:
    case RES_CHRATR_LANGUAGE:
    case RES_CHRATR_POSTURE:
    case RES_CHRATR_WEIGHT:
        bDependsOnScript = sal_True;
        nScript = i18n::ScriptType::LATIN;
        break;

    case RES_CHRATR_CJK_FONT:
    case RES_CHRATR_CJK_FONTSIZE:
    case RES_CHRATR_CJK_LANGUAGE:
    case RES_CHRATR_CJK_POSTURE:
    case RES_CHRATR_CJK_WEIGHT:
        bDependsOnScript = sal_True;
        nScript = i18n::ScriptType::ASIAN;
        break;

    case RES_CHRATR_CTL_FONT:
    case RES_CHRATR_CTL_FONTSIZE:
    case RES_CHRATR_CTL_LANGUAGE:
    case RES_CHRATR_CTL_POSTURE:
    case RES_CHRATR_CTL_WEIGHT:
        bDependsOnScript = sal_True;
        nScript = i18n::ScriptType::COMPLEX;
        break;
    case RES_TXTATR_CHARFMT:
        {
            const SwFmtCharFmt& rChrFmt = (const SwFmtCharFmt&)rItem;
            const SwCharFmt* pFmt = rChrFmt.GetCharFmt();
            const SwHTMLFmtInfo *pFmtInfo = GetFmtInfo( *pFmt, rFmtInfos );
            if( pFmtInfo->bScriptDependent )
            {
                bDependsOnScript = sal_True;
                bDependsOnAnyScript = sal_True;
            }
        }
        break;
    case RES_TXTATR_INETFMT:
        {
            if( GetFmtInfo( *pDoc->GetCharFmtFromPool(
                     RES_POOLCHR_INET_NORMAL), rFmtInfos )->bScriptDependent ||
                GetFmtInfo( *pDoc->GetCharFmtFromPool(
                     RES_POOLCHR_INET_VISIT), rFmtInfos )->bScriptDependent )
            {
                bDependsOnScript = sal_True;
                bDependsOnAnyScript = sal_True;
            }
        }
        break;
    }

    if( bDependsOnScript )
    {
        sal_uInt16 nScriptChgs = aScriptChgLst.Count();
        xub_StrLen nPos = nStart;
        for( sal_uInt16 i=0; i < nScriptChgs; i++ )
        {
            xub_StrLen nChgPos = aScriptChgLst[i];
            if( nPos >= nChgPos )
            {
                // the hint starts behind or at the next script change,
                // so we may continue with this position.
                continue;
            }
            if( nEnd <= nChgPos )
            {
                // the (rest of) the hint ends before or at the next script
                // change, so we can insert it, but only if it belongs
                // to the current script.
                if( bDependsOnAnyScript || nScript == aScriptLst[i] )
                    InsertNoScript( rItem, nPos, nEnd, rFmtInfos,
                                    bParaAttrs );
                break;
            }

            // the hint starts before the next script change and ends behind
            // it, so we can insert a hint upto the next script change and
            // continue with the rest of the hint.
            if( bDependsOnAnyScript || nScript == aScriptLst[i] )
                InsertNoScript( rItem, nPos, nChgPos, rFmtInfos, bParaAttrs );
            nPos = nChgPos;
        }
    }
    else
    {
        InsertNoScript( rItem, nStart, nEnd, rFmtInfos, bParaAttrs );
    }
}

void HTMLEndPosLst::Insert( const SfxItemSet& rItemSet,
                            xub_StrLen nStart, xub_StrLen nEnd,
                            SwHTMLFmtInfos& rFmtInfos,
                            BOOL bDeep, BOOL bParaAttrs )
{
    SfxWhichIter aIter( rItemSet );

    USHORT nWhich = aIter.FirstWhich();
    while( nWhich )
    {
        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET == rItemSet.GetItemState( nWhich, bDeep, &pItem ) )
        {
            Insert( *pItem, nStart, nEnd, rFmtInfos, bParaAttrs );
        }

        nWhich = aIter.NextWhich();
    }
}

void HTMLEndPosLst::Insert( const SwDrawFrmFmt& rFmt, xub_StrLen nPos,
                            SwHTMLFmtInfos& rFmtInfos )
{
    // der Type-Cast ist nur noetig, um nicht seinetwegen
    // svdrwobt.hxx zu includem
    const SdrObject* pTextObj =
        (const SdrObject *)SwHTMLWriter::GetMarqueeTextObj( rFmt );

    if( pTextObj )
    {
        // die Edit-Engine-Attribute des Objekts als SW-Attribute holen
        // und als Hints einsortieren. Wegen der Menge der Hints werden
        // Styles hierbei nicht beruecksichtigt!
        const SfxItemSet& rFmtItemSet = rFmt.GetAttrSet();
        SfxItemSet aItemSet( *rFmtItemSet.GetPool(), RES_CHRATR_BEGIN,
                                                     RES_CHRATR_END );
        SwHTMLWriter::GetEEAttrsFromDrwObj( aItemSet, pTextObj, TRUE );
        BOOL bOutStylesOld = bOutStyles;
        bOutStyles = FALSE;
        Insert( aItemSet, nPos, nPos+1, rFmtInfos, FALSE, FALSE );
        bOutStyles = bOutStylesOld;
    }
}

sal_uInt16 HTMLEndPosLst::GetScriptAtPos( xub_StrLen nPos ,
                                             sal_uInt16 nWeak )
{
    sal_uInt16 nRet = CSS1_OUTMODE_ANY_SCRIPT;

    sal_uInt16 nScriptChgs = aScriptChgLst.Count();
    sal_uInt16 i=0;
    while( i < nScriptChgs && nPos >= aScriptChgLst[i] )
        i++;
    ASSERT( i < nScriptChgs, "script list is to short" );
    if( i < nScriptChgs )
    {
        if( i18n::ScriptType::WEAK == aScriptLst[i] )
            nRet = nWeak;
        else
            nRet = SwHTMLWriter::GetCSS1ScriptForScriptType( aScriptLst[i] );
    }

    return nRet;
}

void HTMLEndPosLst::OutStartAttrs( SwHTMLWriter& rHWrt, xub_StrLen nPos,
                                      HTMLOutContext *pContext	)
{
    rHWrt.bTagOn = TRUE;

    // die Attribute in der Start-Liste sind aufsteigend sortiert
    for( USHORT i=0; i< aStartLst.Count(); i++ )
    {
        HTMLSttEndPos *pPos = aStartLst[i];
        xub_StrLen nStart = pPos->GetStart();
        if( nStart > nPos )
        {
            // dieses und alle folgenden Attribute werden erst noch geoeffnet
            break;
        }
        else if( nStart == nPos )
        {
            // das Attribut ausgeben
            sal_uInt16 nCSS1Script = rHWrt.nCSS1Script;
            sal_uInt16 nWhich = pPos->GetItem()->Which();
            if( RES_TXTATR_CHARFMT == nWhich ||
                RES_TXTATR_INETFMT == nWhich ||
                 RES_PARATR_DROP == nWhich )
            {
                rHWrt.nCSS1Script = GetScriptAtPos( nPos, nCSS1Script );
            }
            if( pContext )
            {
                HTMLOutFuncs::FlushToAscii( rHWrt.Strm(), *pContext );
                pContext = 0; // one time ony
            }
            Out( aHTMLAttrFnTab, *pPos->GetItem(), rHWrt );
            rHWrt.nCSS1Script = nCSS1Script;
        }
    }
}

void HTMLEndPosLst::OutEndAttrs( SwHTMLWriter& rHWrt, xub_StrLen nPos,
                                     HTMLOutContext *pContext )
{
    rHWrt.bTagOn = FALSE;

    // die Attribute in der End-Liste sind aufsteigend sortiert
    USHORT i=0;
    while( i < aEndLst.Count() )
    {
        HTMLSttEndPos *pPos = aEndLst[i];
        xub_StrLen nEnd = pPos->GetEnd();

        if( STRING_MAXLEN==nPos || nEnd == nPos )
        {
            if( pContext )
            {
                HTMLOutFuncs::FlushToAscii( rHWrt.Strm(), *pContext );
                pContext = 0; // one time ony
            }
            Out( aHTMLAttrFnTab, *pPos->GetItem(), rHWrt );
            _RemoveItem( i );
        }
        else if( nEnd > nPos )
        {
            // dieses und alle folgenden Attribute werden erst spaeter beendet
            break;
        }
        else
        {
            // Das Attribut wird vor der aktuellen Position beendet. Das
            // darf nicht sein, aber wie koennen trotzdem damit umgehen
            ASSERT( nEnd >= nPos,
                    "Das Attribut sollte schon laengst beendet sein" );
            i++;
        }
    }
}


/* Ausgabe der Nodes */
Writer& OutHTML_SwTxtNode( Writer& rWrt, const SwCntntNode& rNode )
{
    SwTxtNode * pNd = &((SwTxtNode&)rNode);
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    const String& rStr = pNd->GetTxt();
    xub_StrLen nEnde = rStr.Len();

    // Besonderheit: leere Node und HR-Vorlage (horizontaler Strich)
    // 				nur ein <HR> ausgeben
    USHORT nPoolId = pNd->GetAnyFmtColl().GetPoolFmtId();

    if( !nEnde && (RES_POOLCOLL_HTML_HR==nPoolId ||
                   pNd->GetAnyFmtColl().GetName().EqualsAscii( OOO_STRING_SVTOOLS_HTML_horzrule) ) )
    {
        // dann die absatz-gebundenen Grafiken/OLE-Objekte im Absatz
        // MIB 8.7.97: Ein <PRE> spannen wir um die Linie auf. Dann stimmen
        // zwar die Abstaende nicht, aber sonst bekommen wir einen leeren
        // Absatz hinter dem <HR> und das ist noch unschoener.
        rHTMLWrt.ChangeParaToken( 0 );

        // Alle an dem Node verankerten Rahmen ausgeben
        rHTMLWrt.OutFlyFrm( rNode.GetIndex(), 0, HTML_POS_ANY );

        if( rHTMLWrt.bLFPossible )
            rHTMLWrt.OutNewLine(); // Absatz-Tag in eine neue Zeile

        rHTMLWrt.bLFPossible = TRUE;

        ByteString sOut( '<' );
        sOut += OOO_STRING_SVTOOLS_HTML_horzrule;

        const SfxItemSet* pItemSet = pNd->GetpSwAttrSet();
        if( !pItemSet )
        {
            rWrt.Strm() << sOut.GetBuffer() << '>';
            return rHTMLWrt;
        }
        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET == pItemSet->GetItemState( RES_LR_SPACE, FALSE, &pItem ))
        {
            sal_Int32 nLeft = ((SvxLRSpaceItem*)pItem)->GetLeft();
            sal_Int32 nRight = ((SvxLRSpaceItem*)pItem)->GetRight();
            if( nLeft || nRight )
            {
                const SwFrmFmt& rPgFmt =
                    rHTMLWrt.pDoc->GetPageDescFromPool
                    ( RES_POOLPAGE_HTML, false )->GetMaster();
                const SwFmtFrmSize& rSz   = rPgFmt.GetFrmSize();
                const SvxLRSpaceItem& rLR = rPgFmt.GetLRSpace();
                const SwFmtCol& rCol = rPgFmt.GetCol();

                long nPageWidth = rSz.GetWidth() - rLR.GetLeft() - rLR.GetRight();

                if( 1 < rCol.GetNumCols() )
                    nPageWidth /= rCol.GetNumCols();

                const SwTableNode* pTblNd = pNd->FindTableNode();
                if( pTblNd )
                {
                    const SwTableBox* pBox = pTblNd->GetTable().GetTblBox(
                                    pNd->StartOfSectionIndex() );
                    if( pBox )
                        nPageWidth = pBox->GetFrmFmt()->GetFrmSize().GetWidth();
                }

                ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_width) += '=';
                rWrt.Strm() << sOut.GetBuffer();
                rWrt.OutULong( rHTMLWrt.ToPixel(nPageWidth-nLeft-nRight) );

                ((sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_align) += '=';
                if( !nLeft )
                    sOut += OOO_STRING_SVTOOLS_HTML_AL_left;
                else if( !nRight )
                    sOut += OOO_STRING_SVTOOLS_HTML_AL_right;
                else
                    sOut += OOO_STRING_SVTOOLS_HTML_AL_center;
            }
        }
        rWrt.Strm() << sOut.GetBuffer();
        if( SFX_ITEM_SET == pItemSet->GetItemState( RES_BOX, FALSE, &pItem ))
        {
            const SvxBoxItem* pBoxItem = (const SvxBoxItem*)pItem;
            const SvxBorderLine* pBorderLine = pBoxItem->GetBottom();
            if( pBorderLine )
            {
                USHORT nWidth = pBorderLine->GetOutWidth() +
                                pBorderLine->GetInWidth() +
                                pBorderLine->GetDistance();
                ((sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_size) += '=';
                rWrt.Strm() << sOut.GetBuffer();
                rWrt.OutULong( rHTMLWrt.ToPixel(nWidth) );

                const Color& rBorderColor = pBorderLine->GetColor();
                if( !rBorderColor.IsRGBEqual( Color(COL_GRAY) ) )
                {
                    ((sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_color) += '=';
                    rWrt.Strm() << sOut.GetBuffer();
                    HTMLOutFuncs::Out_Color( rWrt.Strm(), rBorderColor,
                                             rHTMLWrt.eDestEnc );
                }

                if( !pBorderLine->GetInWidth() )
                {
                    (sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_noshade;
                    rWrt.Strm() << sOut.GetBuffer();
                }
            }
        }
        rWrt.Strm() << '>';
        return rHTMLWrt;
    }

    // Die leeren Nodes mit 2pt Font-Hoehe und der Stand-Vorlage, die
    // vor Tabellen und Bereichen eingefuegt werden, nicht exportieren,
    // Bookmarks oder absatzgebundene Grafiken aber schon.
    // MIB 21.7.97: Ausserdem auch keine leeren Tabellen-Zellen exportieren.
    if( !nEnde && (nPoolId == RES_POOLCOLL_STANDARD ||
                   nPoolId == RES_POOLCOLL_TABLE ||
                   nPoolId == RES_POOLCOLL_TABLE_HDLN) )
    {
        // Der aktuelle Node ist leer und enthaelt Standard-Vorlage ...
        const SfxPoolItem* pItem;
        const SfxItemSet *pItemSet = pNd->GetpSwAttrSet();
        if( pItemSet && pItemSet->Count() &&
            SFX_ITEM_SET == pItemSet->GetItemState( RES_CHRATR_FONTSIZE, FALSE, &pItem ) &&
            40 == ((const SvxFontHeightItem *)pItem)->GetHeight() )
        {
            // ... ausserdem ist die 2pt Schrift eingestellt ...
            ULONG nNdPos = rWrt.pCurPam->GetPoint()->nNode.GetIndex();
            const SwNode *pNextNd = rWrt.pDoc->GetNodes()[nNdPos+1];
            const SwNode *pPrevNd = rWrt.pDoc->GetNodes()[nNdPos-1];
            BOOL bStdColl = nPoolId == RES_POOLCOLL_STANDARD;
            if( ( bStdColl && (pNextNd->IsTableNode() ||
                               pNextNd->IsSectionNode()) ) ||
                ( !bStdColl && pNextNd->IsEndNode() &&
                               pPrevNd->IsStartNode() &&
                               SwTableBoxStartNode==
                                pPrevNd->GetStartNode()->GetStartNodeType() ) )
            {
                // ... und er steht vor einer Tabelle ohne einem Bereich
                rHTMLWrt.OutBookmarks();
                rHTMLWrt.bLFPossible = !rHTMLWrt.nLastParaToken;

                // Alle an dem Node verankerten Rahmen ausgeben
                rHTMLWrt.OutFlyFrm( rNode.GetIndex(), 0, HTML_POS_ANY );
                rHTMLWrt.bLFPossible = FALSE;

                return rWrt;
            }
        }
    }

    // PagePreaks uns PagDescs abfangen
    BOOL bPageBreakBehind = FALSE;
    if( rHTMLWrt.bCfgFormFeed &&
        !(rHTMLWrt.bOutTable || rHTMLWrt.bOutFlyFrame) &&
        rHTMLWrt.pStartNdIdx->GetIndex() !=
        rHTMLWrt.pCurPam->GetPoint()->nNode.GetIndex() )
    {
        BOOL bPageBreakBefore = FALSE;
        const SfxPoolItem* pItem;
        const SfxItemSet* pItemSet = pNd->GetpSwAttrSet();

        if( pItemSet )
        {
            if( SFX_ITEM_SET ==
                pItemSet->GetItemState( RES_PAGEDESC, TRUE, &pItem ) &&
                ((SwFmtPageDesc *)pItem)->GetPageDesc() )
                bPageBreakBefore = TRUE;
            else if( SFX_ITEM_SET ==
                     pItemSet->GetItemState( RES_BREAK, TRUE, &pItem ) )
            {
                switch( ((SvxFmtBreakItem *)pItem)->GetBreak() )
                {
                case SVX_BREAK_PAGE_BEFORE:
                    bPageBreakBefore = TRUE;
                    break;
                case SVX_BREAK_PAGE_AFTER:
                    bPageBreakBehind = TRUE;
                    break;
                case SVX_BREAK_PAGE_BOTH:
                    bPageBreakBefore = TRUE;
                    bPageBreakBehind = TRUE;
                    break;
                default:
                    ;
                }
            }
        }

        if( bPageBreakBefore )
            rWrt.Strm() << '\f';
    }

    // eventuell eine Form oeffnen
    rHTMLWrt.OutForm();

    // An dem Node "verankerte" Seitenegebunde Rahmen ausgeben
    BOOL bFlysLeft = rHTMLWrt.OutFlyFrm( rNode.GetIndex(),
                                         0, HTML_POS_PREFIX );
    // An dem Node verankerte Rahmen ausgeben, die vor dem
    // Absatz-Tag geschrieben werden sollen.
    if( bFlysLeft )
        bFlysLeft = rHTMLWrt.OutFlyFrm( rNode.GetIndex(),
                                        0, HTML_POS_BEFORE );

    if( rHTMLWrt.pCurPam->GetPoint()->nNode == rHTMLWrt.pCurPam->GetMark()->nNode )
        nEnde = rHTMLWrt.pCurPam->GetMark()->nContent.GetIndex();

    // gibt es harte Attribute, die als Optionen geschrieben werden muessen?
    rHTMLWrt.bTagOn = TRUE;

    // jetzt das Tag des Absatzes ausgeben
    const SwFmt& rFmt = pNd->GetAnyFmtColl();
    SwHTMLTxtCollOutputInfo aFmtInfo;
    BOOL bOldLFPossible = rHTMLWrt.bLFPossible;
    OutHTML_SwFmt( rWrt, rFmt, pNd->GetpSwAttrSet(), aFmtInfo );

    // Wenn vor dem Absatz-Tag keine neue Zeile aufgemacht wurde, dann
    // tun wir das jetzt
    rHTMLWrt.bLFPossible = !rHTMLWrt.nLastParaToken;
    if( !bOldLFPossible && rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine();


    // dann die Bookmarks (inkl. End-Tag)
    rHTMLWrt.bOutOpts = FALSE;
    rHTMLWrt.OutBookmarks();

    // jetzt ist noch mal eine gute Gelegenheit fuer ein LF, sofern es noch
    // erlaubt ist
    if( rHTMLWrt.bLFPossible &&
        rHTMLWrt.GetLineLen() >= rHTMLWrt.nWhishLineLen )
    {
        rHTMLWrt.OutNewLine();
    }
    rHTMLWrt.bLFPossible = FALSE;

    // Text, der aus einer Outline-Numerierung kommt ermitteln
    xub_StrLen nOffset = 0;
    String aOutlineTxt;
    String aFullText;
    // --> OD 2006-06-12 #b6435904#
    // export numbering string as plain text only for the outline numbering,
    // because the outline numbering isn't exported as a numbering - see <SwHTMLNumRuleInfo::Set(..)>
    if ( pNd->IsOutline() &&
         pNd->GetNumRule() == pNd->GetDoc()->GetOutlineNumRule() )
    // <--
    {
        aOutlineTxt = pNd->GetNumString();
        nOffset = nOffset + aOutlineTxt.Len();
        aFullText = aOutlineTxt;
    }
    String aFootEndNoteSym;
    if( rHTMLWrt.pFmtFtn )
    {
        aFootEndNoteSym = rHTMLWrt.GetFootEndNoteSym( *rHTMLWrt.pFmtFtn );
        nOffset = nOffset + aFootEndNoteSym.Len();
        aFullText += aFootEndNoteSym;
    }

    // gibt es harte Attribute, die als Tags geschrieben werden muessen?
    aFullText += rStr;
    HTMLEndPosLst aEndPosLst( rWrt.pDoc, rHTMLWrt.pTemplate,
                              rHTMLWrt.pDfltColor, rHTMLWrt.bCfgOutStyles,
                              rHTMLWrt.GetHTMLMode(), aFullText,
                                 rHTMLWrt.aScriptTextStyles );
    if( aFmtInfo.pItemSet )
    {
        aEndPosLst.Insert( *aFmtInfo.pItemSet, 0, nEnde + nOffset,
                           rHTMLWrt.aChrFmtInfos, FALSE, TRUE );
    }


    if( aOutlineTxt.Len() || rHTMLWrt.pFmtFtn )
    {
        // Absatz-Attribute ausgeben, damit der Text die Attribute des
        // Absatzes bekommt.
        aEndPosLst.OutStartAttrs( rHTMLWrt, 0 );

        // Theoretisch muesste man hier die Zeichen-Vorlage der Numerierung
        // beachten. Da man die ueber die UI nicht setzen kann, ignorieren
        // wir sie erstmal.

        if( aOutlineTxt.Len() )
            HTMLOutFuncs::Out_String( rWrt.Strm(), aOutlineTxt,
                                         rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters);

        if( rHTMLWrt.pFmtFtn )
        {
            rHTMLWrt.OutFootEndNoteSym( *rHTMLWrt.pFmtFtn, aFootEndNoteSym,
                                        aEndPosLst.GetScriptAtPos( aOutlineTxt.Len(), rHTMLWrt.nCSS1Script ) );
            rHTMLWrt.pFmtFtn = 0;
        }
    }

    // erstmal den Start berichtigen. D.h. wird nur ein Teil vom Satz
    // ausgegeben, so muessen auch da die Attribute stimmen!!
    rHTMLWrt.bTxtAttr = TRUE;


    USHORT nAttrPos = 0;
    xub_StrLen nStrPos = rHTMLWrt.pCurPam->GetPoint()->nContent.GetIndex();
    const SwTxtAttr * pHt = 0;
    USHORT nCntAttr = pNd->HasHints() ? pNd->GetSwpHints().Count() : 0;
    if( nCntAttr && nStrPos > *( pHt = pNd->GetSwpHints()[ 0 ] )->GetStart() )
    {
        // Ok, es gibt vorher Attribute, die ausgegeben werden muessen
        do {
            aEndPosLst.OutEndAttrs( rHTMLWrt, nStrPos + nOffset );

            nAttrPos++;
            if( RES_TXTATR_FIELD == pHt->Which() )		// Felder nicht
                continue;                               // ausgeben

            if ( pHt->GetEnd() && !pHt->HasDummyChar() )
            {
                xub_StrLen nHtEnd = *pHt->GetEnd(),
                       nHtStt = *pHt->GetStart();
                if( !rHTMLWrt.bWriteAll && nHtEnd <= nStrPos )
                    continue;

                // leere Hints am Anfang nicht beachten, oder ??
                if( nHtEnd == nHtStt )
                    continue;

                // Attribut in die Liste aufnehemen
                if( rHTMLWrt.bWriteAll )
                    aEndPosLst.Insert( pHt->GetAttr(), nHtStt + nOffset,
                                       nHtEnd + nOffset,
                                       rHTMLWrt.aChrFmtInfos );
                else
                {
                    xub_StrLen nTmpStt = nHtStt < nStrPos ? nStrPos : nHtStt;
                    xub_StrLen nTmpEnd = nHtEnd < nEnde ? nHtEnd : nEnde;
                    aEndPosLst.Insert( pHt->GetAttr(), nTmpStt + nOffset,
                                       nTmpEnd + nOffset,
                                       rHTMLWrt.aChrFmtInfos );
                }
                continue;
                // aber nicht ausgeben, das erfolgt spaeter !!
            }

        } while( nAttrPos < nCntAttr && nStrPos >
            *( pHt = pNd->GetSwpHints()[ nAttrPos ] )->GetStart() );

        // dann gebe mal alle gesammelten Attribute von der String-Pos aus
        aEndPosLst.OutEndAttrs( rHTMLWrt, nStrPos + nOffset );
        aEndPosLst.OutStartAttrs( rHTMLWrt, nStrPos + nOffset );
    }

    BOOL bWriteBreak = (HTML_PREFORMTXT_ON != rHTMLWrt.nLastParaToken);
    if( bWriteBreak && pNd->GetNumRule()  )
        bWriteBreak = FALSE;

    {
        HTMLOutContext aContext( rHTMLWrt.eDestEnc );

        xub_StrLen nPreSplitPos = 0;
        for( ; nStrPos < nEnde; nStrPos++ )
        {
            aEndPosLst.OutEndAttrs( rHTMLWrt, nStrPos + nOffset, &aContext );

            // Die an der aktuellen Position verankerten Rahmen ausgeben
            if( bFlysLeft )
                bFlysLeft =	rHTMLWrt.OutFlyFrm( rNode.GetIndex(),
                                                nStrPos, HTML_POS_INSIDE,
                                                &aContext );

            BOOL bOutChar = TRUE;
            const SwTxtAttr * pTxtHt = 0;
            if( nAttrPos < nCntAttr && *pHt->GetStart() == nStrPos
                && nStrPos != nEnde )
            {
                do {
                    if ( pHt->GetEnd() && !pHt->HasDummyChar() )
                    {
                        if( RES_CHRATR_KERNING == pHt->Which() &&
                            rHTMLWrt.IsHTMLMode(HTMLMODE_FIRSTLINE) &&
                            *pHt->GetEnd() - nStrPos == 1 &&
                            ' ' == rStr.GetChar(nStrPos) &&
                            ((const SvxKerningItem&)pHt->GetAttr()).GetValue() > 0 )
                        {
                            // Wenn erlaubt, wird das Ding als Spacer exportiert

                            bOutChar = FALSE;	// Space nicht ausgeben
                            bWriteBreak = FALSE;	// der Absatz ist aber auch nicht leer
                            HTMLOutFuncs::FlushToAscii( rWrt.Strm(), aContext );
                            OutHTML_HoriSpacer( rWrt,
                                ((const SvxKerningItem&)pHt->GetAttr()).GetValue() );

                            // Der Hint braucht nun doch nicht weiter
                            // beruecksichtigt werden.
                        }
                        else if( *pHt->GetEnd() != nStrPos )
                        {
                            // Hints mit Ende einsortieren, wenn sie keinen
                            // leeren Bereich aufspannen (Hints, die keinen
                            // Bereich aufspannen werden ignoriert
                            aEndPosLst.Insert( pHt->GetAttr(), nStrPos + nOffset,
                                               *pHt->GetEnd() + nOffset,
                                               rHTMLWrt.aChrFmtInfos );
                        }
                    }
                    else
                    {
                        // Hints ohne-Ende werden als letztes ausgebeben
                        ASSERT( !pTxtHt,
                                "Wieso gibt es da schon ein Attribut ohne Ende?" );
                        if( rHTMLWrt.nTxtAttrsToIgnore>0 )
                        {
                            rHTMLWrt.nTxtAttrsToIgnore--;
                        }
                        else
                        {
                            pTxtHt = pHt;
                            USHORT nFldWhich;
                            if( RES_TXTATR_FIELD != pHt->Which() ||
                                ( RES_POSTITFLD != (nFldWhich = ((const SwFmtFld&)pHt->GetAttr()).GetFld()->Which()) &&
                                RES_SCRIPTFLD != nFldWhich ) )
                                bWriteBreak = FALSE;
                        }
                        bOutChar = FALSE;		// keine 255 ausgeben
                    }
                } while( ++nAttrPos < nCntAttr && nStrPos ==
                    *( pHt = pNd->GetSwpHints()[ nAttrPos ] )->GetStart() );
            }

            // Manche Draw-Formate koennen auch noch Attribute mitbringen
            if( pTxtHt && RES_TXTATR_FLYCNT == pTxtHt->Which() )
            {
                const SwFrmFmt* pFrmFmt =
                    ((const SwFmtFlyCnt &)pTxtHt->GetAttr()).GetFrmFmt();

                if( RES_DRAWFRMFMT == pFrmFmt->Which() )
                    aEndPosLst.Insert( *((const SwDrawFrmFmt *)pFrmFmt),
                                        nStrPos + nOffset,
                                        rHTMLWrt.aChrFmtInfos );
            }

            aEndPosLst.OutEndAttrs( rHTMLWrt, nStrPos + nOffset, &aContext );
            aEndPosLst.OutStartAttrs( rHTMLWrt, nStrPos + nOffset, &aContext );

            if( pTxtHt )
            {
                rHTMLWrt.bLFPossible = !rHTMLWrt.nLastParaToken && nStrPos > 0 &&
                                       rStr.GetChar(nStrPos-1) == ' ';
                sal_uInt16 nCSS1Script = rHTMLWrt.nCSS1Script;
                rHTMLWrt.nCSS1Script = aEndPosLst.GetScriptAtPos(
                                                nStrPos + nOffset, nCSS1Script );
                HTMLOutFuncs::FlushToAscii( rWrt.Strm(), aContext );
                Out( aHTMLAttrFnTab, pTxtHt->GetAttr(), rHTMLWrt );
                rHTMLWrt.nCSS1Script = nCSS1Script;
                rHTMLWrt.bLFPossible = FALSE;
            }

            if( bOutChar )
            {
                sal_Unicode c = rStr.GetChar( nStrPos );
                // versuche nach ungefaehr 255 Zeichen eine neue Zeile zu
                // beginnen, aber nicht in PRE und nur bei Spaces
                if( ' '==c && !rHTMLWrt.nLastParaToken  )
                {
                    xub_StrLen nLineLen;
                    if( rHTMLWrt.nLastParaToken )
                        nLineLen = nStrPos - nPreSplitPos;
                    else
                        nLineLen = rHTMLWrt.GetLineLen();

                    xub_StrLen nWordLen = rStr.Search( ' ', nStrPos+1 );
                    if( nWordLen == STRING_NOTFOUND )
                        nWordLen = nEnde;
                    nWordLen = nWordLen - nStrPos;

                    if( nLineLen >= rHTMLWrt.nWhishLineLen ||
                        (nLineLen+nWordLen) >= rHTMLWrt.nWhishLineLen )
                    {
                        HTMLOutFuncs::FlushToAscii( rWrt.Strm(), aContext );
                        rHTMLWrt.OutNewLine();
                        bOutChar = FALSE;
                        if( rHTMLWrt.nLastParaToken )
                            nPreSplitPos = nStrPos+1;
                    }
                }

                if( bOutChar )
                {
                    if( 0x0a == c )
                    {
                        HTMLOutFuncs::FlushToAscii( rWrt.Strm(), aContext );
                        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_linebreak );
                    }
                    else
                        HTMLOutFuncs::Out_Char( rWrt.Strm(), c, aContext, &rHTMLWrt.aNonConvertableCharacters );

                    // Wenn das letzte Zeichen eines Absatzed ein harter
                    // Zeilen-Umbruch ist brauchen wir noch ein <BR> mehr, weil
                    // Netscape & Co in diesem Fall fuer den naechsten Absatz
                    // nicht in die naechste Zeile gehen.
                    bWriteBreak = (0x0a == c) &&
                                  (HTML_PREFORMTXT_ON != rHTMLWrt.nLastParaToken);
                }
            }
        }
        HTMLOutFuncs::FlushToAscii( rWrt.Strm(), aContext );
    }

    aEndPosLst.OutEndAttrs( rHTMLWrt, STRING_MAXLEN );

    // Die an der letzten Position verankerten Rahmen ausgeben
    if( bFlysLeft )
        bFlysLeft = rHTMLWrt.OutFlyFrm( rNode.GetIndex(),
                                       nEnde, HTML_POS_INSIDE );
    ASSERT( !bFlysLeft, "Es wurden nicht alle Rahmen gespeichert!" );

    rHTMLWrt.bTxtAttr = FALSE;

    if( bWriteBreak )
    {
        BOOL bEndOfCell = rHTMLWrt.bOutTable &&
                         rWrt.pCurPam->GetPoint()->nNode.GetIndex() ==
                         rWrt.pCurPam->GetMark()->nNode.GetIndex();

        if( bEndOfCell && !nEnde &&
            rHTMLWrt.IsHTMLMode(HTMLMODE_NBSP_IN_TABLES) )
        {
            // Wenn der letzte Absatz einer Tabellezelle leer ist und
            // wir fuer den MS-IE exportieren, schreiben wir statt eines
            // <BR> ein &nbsp;
            rWrt.Strm() << '&' << OOO_STRING_SVTOOLS_HTML_S_nbsp << ';';
        }
        else
        {
            HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_linebreak );
            const SvxULSpaceItem& rULSpace =
                (const SvxULSpaceItem &)pNd->GetSwAttrSet().Get(RES_UL_SPACE);
            if( rULSpace.GetLower() > 0 && !bEndOfCell &&
                !rHTMLWrt.IsHTMLMode(HTMLMODE_NO_BR_AT_PAREND) )
                HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_linebreak );
            rHTMLWrt.bLFPossible = TRUE;
        }
    }

    if( rHTMLWrt.bClearLeft || rHTMLWrt.bClearRight )
    {
        const sal_Char *pStr;
        if( rHTMLWrt.bClearLeft )
        {
            if( rHTMLWrt.bClearRight )
                pStr = OOO_STRING_SVTOOLS_HTML_AL_all;
            else
                pStr = OOO_STRING_SVTOOLS_HTML_AL_left;
        }
        else
            pStr = OOO_STRING_SVTOOLS_HTML_AL_right;

        ByteString sOut( OOO_STRING_SVTOOLS_HTML_linebreak );
        (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_clear) += '=') += pStr;

        HTMLOutFuncs::Out_AsciiTag( rHTMLWrt.Strm(), sOut.GetBuffer() );
        rHTMLWrt.bClearLeft = FALSE;
        rHTMLWrt.bClearRight = FALSE;

        rHTMLWrt.bLFPossible = TRUE;
    }

    // wenn ein LF nicht schon erlaubt ist wird es erlaubt, wenn der
    // Absatz mit einem ' ' endet
    if( !rHTMLWrt.bLFPossible && !rHTMLWrt.nLastParaToken &&
        nEnde > 0 && ' ' == rStr.GetChar(nEnde-1) )
        rHTMLWrt.bLFPossible = TRUE;

    rHTMLWrt.bTagOn = FALSE;
    OutHTML_SwFmtOff( rWrt, aFmtInfo );

    // eventuell eine Form schliessen
    rHTMLWrt.OutForm( FALSE );

    if( bPageBreakBehind )
        rWrt.Strm() << '\f';

    return rHTMLWrt;
}


sal_uInt32 SwHTMLWriter::ToPixel( sal_uInt32 nVal ) const
{
    if( Application::GetDefaultDevice() && nVal )
    {
        nVal = Application::GetDefaultDevice()->LogicToPixel(
                    Size( nVal, nVal ), MapMode( MAP_TWIP ) ).Width();
        if( !nVal )		// wo ein Twip ist sollte auch ein Pixel sein
            nVal = 1;
    }
    return nVal;
}


static Writer& OutHTML_CSS1Attr( Writer& rWrt, const SfxPoolItem& rHt )
{
    // wenn gerade Hints geschrieben werden versuchen wir den Hint als
    // CSS1-Attribut zu schreiben

    if( ((SwHTMLWriter&)rWrt).bCfgOutStyles && ((SwHTMLWriter&)rWrt).bTxtAttr )
        OutCSS1_HintSpanTag( rWrt, rHt );

    return rWrt;
}


/* File CHRATR.HXX: */

static Writer& OutHTML_SvxColor( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    // die Default-Farbe nur Schreiben, wenn sie als Hint vorkommt
    //if( rHTMLWrt.bTagOn && !rHTMLWrt.bTxtAttr && rHTMLWrt.pDfltColor
    //	&& rColor == *rHTMLWrt.pDfltColor )
    //	return rWrt;

    if( !rHTMLWrt.bTxtAttr && rHTMLWrt.bCfgOutStyles && rHTMLWrt.bCfgPreferStyles )
    {
        // Font-Farbe nicht als Tag schreiben, wenn Styles normalen Tags
        // vorgezogen werden
        return rWrt;
    }

    if( rHTMLWrt.bTagOn )
    {
        Color aColor( ((const SvxColorItem&)rHt).GetValue() );
        if( COL_AUTO == aColor.GetColor() )
            aColor.SetColor( COL_BLACK );

        ByteString sOut( '<' );
        (((sOut += OOO_STRING_SVTOOLS_HTML_font) += ' ') += OOO_STRING_SVTOOLS_HTML_O_color) += '=';
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_Color( rWrt.Strm(), aColor, rHTMLWrt.eDestEnc ) << '>';
    }
    else
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_font, FALSE );

    return rWrt;
}


static Writer& OutHTML_SwPosture( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    const FontItalic nPosture = ((const SvxPostureItem&)rHt).GetPosture();
    if( ITALIC_NORMAL == nPosture )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_italic, rHTMLWrt.bTagOn );
    }
    else if( rHTMLWrt.bCfgOutStyles && rHTMLWrt.bTxtAttr )
    {
        // vielleicht als CSS1-Attribut ?
        OutCSS1_HintSpanTag( rWrt, rHt );
    }

    return rWrt;
}

static Writer& OutHTML_SvxFont( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    if( rHTMLWrt.bTagOn )
    {
        String aNames;
        SwHTMLWriter::PrepareFontList( ((const SvxFontItem&)rHt), aNames, 0,
                           rHTMLWrt.IsHTMLMode(HTMLMODE_FONT_GENERIC) );
        ByteString sOut( '<' );
        (((sOut += OOO_STRING_SVTOOLS_HTML_font) += ' ') += OOO_STRING_SVTOOLS_HTML_O_face) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), aNames, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters )
            << "\">";
    }
    else
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_font , FALSE );

    return rWrt;
}

static Writer& OutHTML_SvxFontHeight( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    if( rHTMLWrt.bTagOn )
    {
        ByteString sOut( '<' );
        sOut += OOO_STRING_SVTOOLS_HTML_font;

        UINT32 nHeight = ((const SvxFontHeightItem&)rHt).GetHeight();
        USHORT nSize = rHTMLWrt.GetHTMLFontSize( nHeight );
        (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_size) += '=')
            += ByteString::CreateFromInt32( nSize );
        rWrt.Strm() << sOut.GetBuffer();

        if( rHTMLWrt.bCfgOutStyles && rHTMLWrt.bTxtAttr &&
            rHTMLWrt.aFontHeights[nSize-1] != nHeight )
        {
            // wenn die Groesse keiner HTML-Groesse entspricht,
            // wird sie noch zusatzlich als Style-Option exportiert
            OutCSS1_HintStyleOpt( rWrt, rHt );
        }
        rWrt.Strm() << '>';
    }
    else
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_font, FALSE );
    }

    return rWrt;
}

static Writer& OutHTML_SvxLanguage( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    LanguageType eLang = ((const SvxLanguageItem &)rHt).GetLanguage();
    if( LANGUAGE_DONTKNOW == eLang )
        return rWrt;

    if( rHTMLWrt.bTagOn )
    {
        ByteString sOut( '<' );
        sOut += OOO_STRING_SVTOOLS_HTML_span;
        rWrt.Strm() << sOut.GetBuffer();
        rHTMLWrt.OutLanguage( ((const SvxLanguageItem &)rHt).GetLanguage() );
        rWrt.Strm() << '>';
    }
    else
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_span, FALSE );
    }

    return rWrt;
}
static Writer& OutHTML_SwWeight( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    const FontWeight nBold = ((const SvxWeightItem&)rHt).GetWeight();
    if( WEIGHT_BOLD == nBold )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_bold, rHTMLWrt.bTagOn );
    }
    else if( rHTMLWrt.bCfgOutStyles && rHTMLWrt.bTxtAttr )
    {
        // vielleicht als CSS1-Attribut ?
        OutCSS1_HintSpanTag( rWrt, rHt );
    }

    return rWrt;
}


static Writer& OutHTML_SwCrossedOut( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    // Wegen Netscape schrieben wir hier STRIKE und nicht S raus!
    const FontStrikeout nStrike = ((const SvxCrossedOutItem&)rHt).GetStrikeout();
    if( STRIKEOUT_NONE != nStrike )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_strike, rHTMLWrt.bTagOn );
    }
    else if( rHTMLWrt.bCfgOutStyles && rHTMLWrt.bTxtAttr )
    {
        // vielleicht als CSS1-Attribut ?
        OutCSS1_HintSpanTag( rWrt, rHt );
    }

    return rWrt;
}


static Writer& OutHTML_SvxEscapement( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    const SvxEscapement eEscape =
        (const SvxEscapement)((const SvxEscapementItem&)rHt).GetEnumValue();
    const sal_Char *pStr = 0;
    switch( eEscape )
    {
    case SVX_ESCAPEMENT_SUPERSCRIPT: pStr = OOO_STRING_SVTOOLS_HTML_superscript; break;
    case SVX_ESCAPEMENT_SUBSCRIPT: pStr = OOO_STRING_SVTOOLS_HTML_subscript; break;
    default:
        ;
    }

    if( pStr )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), pStr, rHTMLWrt.bTagOn );
    }
    else if( rHTMLWrt.bCfgOutStyles && rHTMLWrt.bTxtAttr )
    {
        // vielleicht als CSS1-Attribut ?
        OutCSS1_HintSpanTag( rWrt, rHt );
    }

    return rWrt;
}



static Writer& OutHTML_SwUnderline( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    const FontUnderline eUnder = ((const SvxUnderlineItem&)rHt).GetLineStyle();
    if( UNDERLINE_NONE != eUnder )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_underline, rHTMLWrt.bTagOn );
    }
    else if( rHTMLWrt.bCfgOutStyles && rHTMLWrt.bTxtAttr )
    {
        // vielleicht als CSS1-Attribut ?
        OutCSS1_HintSpanTag( rWrt, rHt );
    }

    return rWrt;
}


static Writer& OutHTML_SwFlyCnt( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;
    SwFmtFlyCnt& rFlyCnt = (SwFmtFlyCnt&)rHt;

    const SwFrmFmt& rFmt = *rFlyCnt.GetFrmFmt();
    const SdrObject *pSdrObj = 0;

    SwHTMLFrmType eType =
        (SwHTMLFrmType)rHTMLWrt.GuessFrmType( rFmt, pSdrObj );
    BYTE nMode = aHTMLOutFrmAsCharTable[eType][rHTMLWrt.nExportMode];
    rHTMLWrt.OutFrmFmt( nMode, rFmt, pSdrObj );
    return rWrt;
}


// Das ist jetzt unser Blink-Item. Blinkend wird eingeschaltet, indem man
// das Item auf TRUE setzt!
static Writer& OutHTML_SwBlink( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts || !rHTMLWrt.IsHTMLMode(HTMLMODE_BLINK) )
        return rWrt;

    if( ((const SvxBlinkItem&)rHt).GetValue() )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_blink, rHTMLWrt.bTagOn );
    }
    else if( rHTMLWrt.bCfgOutStyles && rHTMLWrt.bTxtAttr )
    {
        // vielleicht als CSS1-Attribut ?
        OutCSS1_HintSpanTag( rWrt, rHt );
    }

    return rWrt;
}

Writer& OutHTML_INetFmt( Writer& rWrt, const SwFmtINetFmt& rINetFmt, BOOL bOn )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;

    String aURL( rINetFmt.GetValue() );
    const SvxMacroTableDtor *pMacTable = rINetFmt.GetMacroTbl();
    BOOL bEvents = pMacTable != 0 && pMacTable->Count() > 0;

    // Gibt es ueberhaupt etwas auszugeben?
    if( !aURL.Len() && !bEvents && !rINetFmt.GetName().Len() )
        return rWrt;

    // Tag aus? Dann nur ein </A> ausgeben.
    if( !bOn )
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_anchor, FALSE );
        return rWrt;
    }

    ByteString sOut( '<' );
    sOut += OOO_STRING_SVTOOLS_HTML_anchor;

    sal_Bool bScriptDependent = sal_False;
    {
        const SwCharFmt* pFmt = rWrt.pDoc->GetCharFmtFromPool(
                 RES_POOLCHR_INET_NORMAL );
        SwHTMLFmtInfo aFmtInfo( pFmt );
        USHORT nPos;
        if( rHTMLWrt.aChrFmtInfos.Seek_Entry( &aFmtInfo, &nPos ) )
        {
            bScriptDependent = rHTMLWrt.aChrFmtInfos[nPos]->bScriptDependent;
        }
    }
    if( !bScriptDependent )
    {
        const SwCharFmt* pFmt = rWrt.pDoc->GetCharFmtFromPool(
                 RES_POOLCHR_INET_VISIT );
        SwHTMLFmtInfo aFmtInfo( pFmt );
        USHORT nPos;
        if( rHTMLWrt.aChrFmtInfos.Seek_Entry( &aFmtInfo, &nPos ) )
        {
            bScriptDependent = rHTMLWrt.aChrFmtInfos[nPos]->bScriptDependent;
        }
    }

    if( bScriptDependent )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_class) += "=\"";
        switch( rHTMLWrt.nCSS1Script )
        {
        case CSS1_OUTMODE_WESTERN:
            sOut += "western";
            break;
        case CSS1_OUTMODE_CJK:
            sOut += "cjk";
            break;
        case CSS1_OUTMODE_CTL:
            sOut += "ctl";
            break;
        }
        sOut += '\"';
    }

    rWrt.Strm() << sOut.GetBuffer();

#define REL_HACK
#ifdef REL_HACK
    String sRel;
#endif

    if( aURL.Len() || bEvents )
    {
#ifdef REL_HACK
        String sTmp( aURL );
        sTmp.ToUpperAscii();
        xub_StrLen nPos = sTmp.SearchAscii( "\" REL=" );
        if( nPos!=STRING_NOTFOUND )
        {
            sRel = aURL.Copy( nPos+1 );
            aURL.Erase( nPos );
        }
#endif
        aURL.EraseLeadingChars().EraseTrailingChars();

        ((sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_href) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        rHTMLWrt.OutHyperlinkHRefValue( aURL );
        sOut = '\"';
    }
    else
        sOut.Erase();

    if( rINetFmt.GetName().Len() )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), rINetFmt.GetName(),
                                  rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
        sOut = '\"';
    }

    const String& rTarget = rINetFmt.GetTargetFrame();
    if( rTarget.Len() )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_target) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), rTarget, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
        sOut = '\"';
    }

#ifdef REL_HACK
    if( sRel.Len() )
        sOut += ByteString( sRel, RTL_TEXTENCODING_ASCII_US );
#endif
    if( sOut.Len() )
        rWrt.Strm() << sOut.GetBuffer();

    if( bEvents )
        HTMLOutFuncs::Out_Events( rWrt.Strm(), *pMacTable, aAnchorEventTable,
                                  rHTMLWrt.bCfgStarBasic, rHTMLWrt.eDestEnc,
                                     &rHTMLWrt.aNonConvertableCharacters	);
    rWrt.Strm() << ">";

    return rWrt;
}

static Writer& OutHTML_SwFmtINetFmt( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;

    if( rHTMLWrt.bOutOpts )
        return rWrt;

    const SwFmtINetFmt& rINetFmt = (const SwFmtINetFmt&)rHt;

    if( rHTMLWrt.bTagOn )
    {
        // ggf. ein noch offenes Attribut voruebergehend beenden
        if( rHTMLWrt.aINetFmts.Count() )
        {
            SwFmtINetFmt *pINetFmt =
                rHTMLWrt.aINetFmts[ rHTMLWrt.aINetFmts.Count()-1 ];
            OutHTML_INetFmt( rWrt, *pINetFmt, FALSE );
        }

        // jetzt das neue aufmachen
        OutHTML_INetFmt( rWrt, rINetFmt, TRUE );

        // und merken
        const SwFmtINetFmt *pINetFmt = new SwFmtINetFmt( rINetFmt );
        rHTMLWrt.aINetFmts.C40_INSERT( SwFmtINetFmt, pINetFmt,
                                       rHTMLWrt.aINetFmts.Count() );
    }
    else
    {
        // das
        OutHTML_INetFmt( rWrt, rINetFmt, FALSE );

        ASSERT( rHTMLWrt.aINetFmts.Count(), "da fehlt doch ein URL-Attribut" );
        if( rHTMLWrt.aINetFmts.Count() )
        {
            // das eigene Attribut vom Stack holen
            SwFmtINetFmt *pINetFmt =
                rHTMLWrt.aINetFmts[ rHTMLWrt.aINetFmts.Count()-1 ];

            rHTMLWrt.aINetFmts.Remove( rHTMLWrt.aINetFmts.Count()-1, 1 );
            delete pINetFmt;
        }

        if( rHTMLWrt.aINetFmts.Count() )
        {
            // es ist noch ein Attribut auf dem Stack, das wieder geoeffnet
            // werden muss
            SwFmtINetFmt *pINetFmt =
                rHTMLWrt.aINetFmts[ rHTMLWrt.aINetFmts.Count()-1 ];
            OutHTML_INetFmt( rWrt, *pINetFmt, TRUE );
        }
    }

    return rWrt;
}

static Writer& OutHTML_SwTxtCharFmt( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( rHTMLWrt.bOutOpts )
        return rWrt;

    const SwFmtCharFmt& rChrFmt = (const SwFmtCharFmt&)rHt;
    const SwCharFmt* pFmt = rChrFmt.GetCharFmt();

    if( !pFmt )
    {
        return rWrt;
    }

    SwHTMLFmtInfo aFmtInfo( pFmt );
    USHORT nPos;
    if( !rHTMLWrt.aChrFmtInfos.Seek_Entry( &aFmtInfo, &nPos ) )
        return rWrt;

    const SwHTMLFmtInfo *pFmtInfo = rHTMLWrt.aChrFmtInfos[nPos];
    ASSERT( pFmtInfo, "Wieso gint es keine Infos ueber die Zeichenvorlage?" );

    if( rHTMLWrt.bTagOn )
    {
        ByteString sOut( '<' );
        if( pFmtInfo->aToken.Len() > 0 )
            sOut += pFmtInfo->aToken;
        else
            sOut += OOO_STRING_SVTOOLS_HTML_span;
        if( rHTMLWrt.bCfgOutStyles &&
            (pFmtInfo->aClass.Len() || pFmtInfo->bScriptDependent) )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_class) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            String aClass( pFmtInfo->aClass );
            if( pFmtInfo->bScriptDependent )
            {
                if( aClass.Len() )
                   aClass += '-';
                switch( rHTMLWrt.nCSS1Script )
                {
                case CSS1_OUTMODE_WESTERN:
                    aClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("western") );
                    break;
                case CSS1_OUTMODE_CJK:
                    aClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("cjk") );
                    break;
                case CSS1_OUTMODE_CTL:
                    aClass.AppendAscii( RTL_CONSTASCII_STRINGPARAM("ctl") );
                    break;
                }
            }
            HTMLOutFuncs::Out_String( rWrt.Strm(), aClass,
                                          rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            sOut = '\"';
        }
        sOut += '>';
        rWrt.Strm() << sOut.GetBuffer();
    }
    else
    {
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(),
                pFmtInfo->aToken.Len() ? pFmtInfo->aToken.GetBuffer()
                                       : OOO_STRING_SVTOOLS_HTML_span,
                FALSE );
    }

    return rWrt;
}

static Writer& OutHTML_SvxAdjust( Writer& rWrt, const SfxPoolItem& rHt )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;
    if( !rHTMLWrt.bOutOpts || !rHTMLWrt.bTagOn )
        return  rWrt;

    SvxAdjustItem& rAdjust = (SvxAdjustItem&)rHt;
    const sal_Char* pStr = 0;
    switch( rAdjust.GetAdjust() )
    {
    case SVX_ADJUST_CENTER: pStr = OOO_STRING_SVTOOLS_HTML_AL_center; break;
    case SVX_ADJUST_LEFT: pStr = OOO_STRING_SVTOOLS_HTML_AL_left; break;
    case SVX_ADJUST_RIGHT: pStr = OOO_STRING_SVTOOLS_HTML_AL_right; break;
    case SVX_ADJUST_BLOCK: pStr = OOO_STRING_SVTOOLS_HTML_AL_justify; break;
    default:
        ;
    }
    if( pStr )
    {
        ByteString sOut( ' ' );
        ((sOut += OOO_STRING_SVTOOLS_HTML_O_align) += '=') += pStr;
        rWrt.Strm() << sOut.GetBuffer();
    }

    return rWrt;
}

/*
 * lege hier die Tabellen fuer die HTML-Funktions-Pointer auf
 * die Ausgabe-Funktionen an.
 * Es sind lokale Strukturen, die nur innerhalb der HTML-DLL
 * bekannt sein muessen.
 */


SwAttrFnTab aHTMLAttrFnTab = {
/* RES_CHRATR_CASEMAP	*/          OutHTML_CSS1Attr,
/* RES_CHRATR_CHARSETCOLOR	*/      0,
/* RES_CHRATR_COLOR	*/              OutHTML_SvxColor,
/* RES_CHRATR_CONTOUR	*/          0,
/* RES_CHRATR_CROSSEDOUT	*/      OutHTML_SwCrossedOut,
/* RES_CHRATR_ESCAPEMENT	*/      OutHTML_SvxEscapement,
/* RES_CHRATR_FONT	*/              OutHTML_SvxFont,
/* RES_CHRATR_FONTSIZE	*/          OutHTML_SvxFontHeight,
/* RES_CHRATR_KERNING	*/          OutHTML_CSS1Attr,
/* RES_CHRATR_LANGUAGE	*/          OutHTML_SvxLanguage,
/* RES_CHRATR_POSTURE	*/          OutHTML_SwPosture,
/* RES_CHRATR_PROPORTIONALFONTSIZE*/0,
/* RES_CHRATR_SHADOWED	*/          0,
/* RES_CHRATR_UNDERLINE	*/          OutHTML_SwUnderline,
/* RES_CHRATR_WEIGHT	*/          OutHTML_SwWeight,
/* RES_CHRATR_WORDLINEMODE	*/      0,
/* RES_CHRATR_AUTOKERN	*/          0,
/* RES_CHRATR_BLINK	*/          	OutHTML_SwBlink,
/* RES_CHRATR_NOHYPHEN	*/          0, // Neu: nicht trennen
/* RES_CHRATR_NOLINEBREAK */        0, // Neu: nicht umbrechen
/* RES_CHRATR_BACKGROUND */        	OutHTML_CSS1Attr, // Neu: Zeichenhintergrund
/* RES_CHRATR_CJK_FONT */			OutHTML_SvxFont,
/* RES_CHRATR_CJK_FONTSIZE */		OutHTML_SvxFontHeight,
/* RES_CHRATR_CJK_LANGUAGE */		OutHTML_SvxLanguage,
/* RES_CHRATR_CJK_POSTURE */		OutHTML_SwPosture,
/* RES_CHRATR_CJK_WEIGHT */			OutHTML_SwWeight,
/* RES_CHRATR_CTL_FONT */			OutHTML_SvxFont,
/* RES_CHRATR_CTL_FONTSIZE */		OutHTML_SvxFontHeight,
/* RES_CHRATR_CTL_LANGUAGE */		OutHTML_SvxLanguage,
/* RES_CHRATR_CTL_POSTURE */		OutHTML_SwPosture,
/* RES_CHRATR_CTL_WEIGHT */			OutHTML_SwWeight,
/* RES_CHRATR_ROTATE */             0,
/* RES_CHRATR_EMPHASIS_MARK */      0,
/* RES_CHRATR_TWO_LINES */          0,
/* RES_CHRATR_SCALEW */             0,
/* RES_CHRATR_RELIEF */             0,
/* RES_CHRATR_HIDDEN */             0,
/* RES_CHRATR_OVERLINE */           OutHTML_CSS1Attr,
/* RES_CHRATR_DUMMY1 */             0,
/* RES_CHRATR_DUMMY2 */             0,

/* RES_TXTATR_REFMARK */            0,
/* RES_TXTATR_TOXMARK */            0,
/* RES_TXTATR_META */               0,
/* RES_TXTATR_METAFIELD */          0,
/* RES_TXTATR_AUTOFMT */            0,
/* RES_TXTATR_INETFMT */            OutHTML_SwFmtINetFmt,
/* RES_TXTATR_CHARFMT */            OutHTML_SwTxtCharFmt,
/* RES_TXTATR_CJK_RUBY */           0,
/* RES_TXTATR_UNKNOWN_CONTAINER */  0,
/* RES_TXTATR_DUMMY5 */             0,

/* RES_TXTATR_FIELD	*/          	OutHTML_SwFmtFld,
/* RES_TXTATR_FLYCNT */ 			OutHTML_SwFlyCnt,
/* RES_TXTATR_FTN */				OutHTML_SwFmtFtn,
/* RES_TXTATR_DUMMY4 */             0,
/* RES_TXTATR_DUMMY3 */             0,
/* RES_TXTATR_DUMMY1 */        	    0, // Dummy:
/* RES_TXTATR_DUMMY2 */        	    0, // Dummy:

/* RES_PARATR_LINESPACING	*/      0,
/* RES_PARATR_ADJUST	*/          OutHTML_SvxAdjust,
/* RES_PARATR_SPLIT	*/				0,
/* RES_PARATR_WIDOWS	*/          0,
/* RES_PARATR_ORPHANS	*/          0,
/* RES_PARATR_TABSTOP	*/          0,
/* RES_PARATR_HYPHENZONE*/          0,
/* RES_PARATR_DROP */				OutHTML_CSS1Attr,
/* RES_PARATR_REGISTER */        	0, // neu:  Registerhaltigkeit
/* RES_PARATR_NUMRULE */      	    0, // Dummy:
/* RES_PARATR_SCRIPTSPACE */   	    0, // Dummy:
/* RES_PARATR_HANGINGPUNCTUATION */	0, // Dummy:
/* RES_PARATR_FORBIDDEN_RULES */    0, // new
/* RES_PARATR_VERTALIGN */          0, // new
/* RES_PARATR_SNAPTOGRID*/          0, // new
/* RES_PARATR_CONNECT_TO_BORDER */  0, // new

/* RES_PARATR_LIST_ID */            0, // new
/* RES_PARATR_LIST_LEVEL */         0, // new
/* RES_PARATR_LIST_ISRESTART */     0, // new
/* RES_PARATR_LIST_RESTARTVALUE */  0, // new
/* RES_PARATR_LIST_ISCOUNTED */     0, // new

/* RES_FILL_ORDER	*/				0,
/* RES_FRM_SIZE	*/					0,
/* RES_PAPER_BIN	*/              0,
/* RES_LR_SPACE	*/                  0,
/* RES_UL_SPACE	*/                  0,
/* RES_PAGEDESC */					0,
/* RES_BREAK */						0,
/* RES_CNTNT */						0,
/* RES_HEADER */		   			0,
/* RES_FOOTER */		   			0,
/* RES_PRINT */						0,
/* RES_OPAQUE */					0,
/* RES_PROTECT */					0,
/* RES_SURROUND */					0,
/* RES_VERT_ORIENT */				0,
/* RES_HORI_ORIENT */				0,
/* RES_ANCHOR */					0,
/* RES_BACKGROUND */				0,
/* RES_BOX	*/                      0,
/* RES_SHADOW */					0,
/* RES_FRMMACRO */					0,
/* RES_COL */						0,
/* RES_KEEP */						0,
/* RES_URL */        	    		0,
/* RES_EDIT_IN_READONLY */        	0,
/* RES_LAYOUT_SPLIT */ 	    		0,
/* RES_FRMATR_DUMMY1 */        	    0, // Dummy:
/* RES_FRMATR_DUMMY2 */        	    0, // Dummy:
/* RES_AUTO_STYLE */        	    0, // Dummy:
/* RES_FRMATR_DUMMY4 */        	    0, // Dummy:
/* RES_FRMATR_DUMMY5 */        	    0, // Dummy:
/* RES_FRMATR_DUMMY6 */        	    0, // Dummy:
/* RES_FRMATR_DUMMY7 */        	    0, // Dummy:
/* RES_FRMATR_DUMMY8 */        	    0, // Dummy:
/* RES_FRMATR_DUMMY9 */        	    0, // Dummy:
/* RES_FOLLOW_TEXT_FLOW */          0,
/* RES_WRAP_INFLUENCE_ON_OBJPOS */  0,
/* RES_FRMATR_DUMMY2 */             0, // Dummy:
/* RES_AUTO_STYLE */                0, // Dummy:
/* RES_FRMATR_DUMMY4 */             0, // Dummy:
/* RES_FRMATR_DUMMY5 */             0, // Dummy:

/* RES_GRFATR_MIRRORGRF	*/			0,
/* RES_GRFATR_CROPGRF	*/			0,
/* RES_GRFATR_ROTATION */			0,
/* RES_GRFATR_LUMINANCE */			0,
/* RES_GRFATR_CONTRAST */			0,
/* RES_GRFATR_CHANNELR */			0,
/* RES_GRFATR_CHANNELG */			0,
/* RES_GRFATR_CHANNELB */			0,
/* RES_GRFATR_GAMMA */				0,
/* RES_GRFATR_INVERT */				0,
/* RES_GRFATR_TRANSPARENCY */		0,
/* RES_GRFATR_DRWAMODE */			0,
/* RES_GRFATR_DUMMY1 */				0,
/* RES_GRFATR_DUMMY2 */				0,
/* RES_GRFATR_DUMMY3 */				0,
/* RES_GRFATR_DUMMY4 */				0,
/* RES_GRFATR_DUMMY5 */				0,

/* RES_BOXATR_FORMAT */				0,
/* RES_BOXATR_FORMULA */			0,
/* RES_BOXATR_VALUE */				0
};
