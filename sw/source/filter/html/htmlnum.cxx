/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: htmlnum.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-10 15:03:43 $
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

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _HTMLTOKN_H
#include <svtools/htmltokn.h>
#endif
#ifndef _HTMLKYWD_HXX
#include <svtools/htmlkywd.hxx>
#endif
#ifndef _HTMLOUT_HXX //autogen
#include <svtools/htmlout.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif

#ifndef _SVX_BRSHITEM_HXX //autogen
#include <svx/brshitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif

#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _PARATR_HXX
#include <paratr.hxx>
#endif

#include "htmlnum.hxx"
#include "swcss1.hxx"
#include "swhtml.hxx"
#include "wrthtml.hxx"

// TODO: Unicode: Are these characters the correct ones?
#define HTML_BULLETCHAR_DISC	(0xe008)
#define HTML_BULLETCHAR_CIRCLE	(0xe009)
#define HTML_BULLETCHAR_SQUARE	(0xe00b)


// <UL TYPE=...>
static HTMLOptionEnum __FAR_DATA aHTMLULTypeTable[] =
{
    { sHTML_ULTYPE_disc,	HTML_BULLETCHAR_DISC		},
    { sHTML_ULTYPE_circle,	HTML_BULLETCHAR_CIRCLE		},
    { sHTML_ULTYPE_square,	HTML_BULLETCHAR_SQUARE		},
    { 0,					0							}
};

/*  */

void SwHTMLNumRuleInfo::Set( const SwTxtNode& rTxtNd )
{
    // --> OD 2006-06-12 #b6435904#
    // export all numberings, except the outline numbering.
//    if( rTxtNd.GetNumRule() && ! rTxtNd.IsOutline())
    const SwNumRule* pTxtNdNumRule( rTxtNd.GetNumRule() );
    if ( pTxtNdNumRule &&
         pTxtNdNumRule != rTxtNd.GetDoc()->GetOutlineNumRule() )
    {
        pNumRule = const_cast<SwNumRule*>(pTxtNdNumRule);
        nDeep = pNumRule ? rTxtNd.GetLevel() + 1 : 0;
        bNumbered = rTxtNd.IsCounted();
        // --> OD 2005-11-16 #i57919#
        // correction of refactoring done by cws swnumtree:
        // <bRestart> has to be set to <true>, if numbering is restarted at this
        // text node and the start value equals <USHRT_MAX>.
        // Start value <USHRT_MAX> indicates, that at this text node the numbering
        // is restarted with the value given at the corresponding level.
        bRestart = rTxtNd.IsRestart() &&
                   rTxtNd.GetNum() && rTxtNd.GetNum()->GetStartValue() == USHRT_MAX;
        // <--
    }
    // <--
    else
    {
        pNumRule = 0;
        nDeep = 0;
        bNumbered = bRestart = sal_False;
    }
}

/*  */

void SwHTMLParser::NewNumBulList( int nToken )
{
    SwHTMLNumRuleInfo& rInfo = GetNumInfo();

    // Erstmal einen neuen Absatz aufmachen
    sal_Bool bSpace = (rInfo.GetDepth() + nDefListDeep) == 0;
    if( pPam->GetPoint()->nContent.GetIndex() )
        AppendTxtNode( bSpace ? AM_SPACE : AM_NOSPACE, sal_False );
    else if( bSpace )
        AddParSpace();

    // Die Numerierung-Ebene erhoehen
    rInfo.IncDepth();
    sal_uInt8 nLevel = (sal_uInt8)( (rInfo.GetDepth() <= MAXLEVEL ? rInfo.GetDepth()
                                                        : MAXLEVEL) - 1 );

    // ggf. ein Regelwerk anlegen
    if( !rInfo.GetNumRule() )
    {
        sal_uInt16 nPos = pDoc->MakeNumRule( pDoc->GetUniqueNumRuleName() );
        rInfo.SetNumRule( pDoc->GetNumRuleTbl()[nPos] );
    }

    // das Format anpassen, falls es fuer den Level noch nicht
    // geschehen ist!
    sal_Bool bNewNumFmt = rInfo.GetNumRule()->GetNumFmt( nLevel ) == 0;
    sal_Bool bChangeNumFmt = sal_False;

    // das default Numerierungsformat erstellen
    SwNumFmt aNumFmt( rInfo.GetNumRule()->Get(nLevel) );
    rInfo.SetNodeStartValue( nLevel );
    if( bNewNumFmt )
    {
        sal_uInt16 nChrFmtPoolId = 0;
        if( HTML_ORDERLIST_ON == nToken )
        {
            aNumFmt.SetNumberingType(SVX_NUM_ARABIC);
            nChrFmtPoolId = RES_POOLCHR_NUM_LEVEL;
        }
        else
        {
            // Wir setzen hier eine Zeichenvorlage, weil die UI das auch
            // so macht. Dadurch wurd immer auch eine 9pt-Schrift
            // eingestellt, was in Netscape nicht der Fall ist. Bisher hat
            // das noch niemanden gestoert.
            aNumFmt.SetBulletFont( &rInfo.GetNumRule()->GetDefBulletFont() );
            aNumFmt.SetNumberingType(SVX_NUM_CHAR_SPECIAL);
            aNumFmt.SetBulletChar( cBulletChar );		// das Bulletzeichen !!
            nChrFmtPoolId = RES_POOLCHR_BUL_LEVEL;
        }

        sal_uInt16 nAbsLSpace = HTML_NUMBUL_MARGINLEFT;
        short nFirstLineIndent  = HTML_NUMBUL_INDENT;
        if( nLevel > 0 )
        {
            const SwNumFmt& rPrevNumFmt = rInfo.GetNumRule()->Get( nLevel-1 );
            nAbsLSpace += rPrevNumFmt.GetAbsLSpace();
            nFirstLineIndent = rPrevNumFmt.GetFirstLineOffset();
        }
        aNumFmt.SetAbsLSpace( nAbsLSpace );
        aNumFmt.SetFirstLineOffset( nFirstLineIndent );
        aNumFmt.SetCharFmt( pCSS1Parser->GetCharFmtFromPool(nChrFmtPoolId) );
        bChangeNumFmt = sal_True;
    }
    else if( 1 != aNumFmt.GetStart() )
    {
        // Wenn die Ebene schon mal benutzt wurde, muss der Start-Wert
        // ggf. hart am Absatz gesetzt werden.
        rInfo.SetNodeStartValue( nLevel, 1 );
    }

    // und es ggf. durch die Optionen veraendern
    String aId, aStyle, aClass, aBulletSrc, aLang, aDir;
    SvxFrameVertOrient eVertOri = SVX_VERT_NONE;
    sal_uInt16 nWidth=USHRT_MAX, nHeight=USHRT_MAX;
    const HTMLOptions *pOptions = GetOptions();
    for( sal_uInt16 i = pOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pOptions)[--i];
        switch( pOption->GetToken() )
        {
        case HTML_O_ID:
            aId = pOption->GetString();
            break;
        case HTML_O_TYPE:
            if( bNewNumFmt && pOption->GetString().Len() )
            {
                switch( nToken )
                {
                case HTML_ORDERLIST_ON:
                    bChangeNumFmt = sal_True;
                    switch( pOption->GetString().GetChar(0) )
                    {
                    case 'A':   aNumFmt.SetNumberingType(SVX_NUM_CHARS_UPPER_LETTER); break;
                    case 'a':   aNumFmt.SetNumberingType(SVX_NUM_CHARS_LOWER_LETTER); break;
                    case 'I':   aNumFmt.SetNumberingType(SVX_NUM_ROMAN_UPPER);        break;
                    case 'i':   aNumFmt.SetNumberingType(SVX_NUM_ROMAN_LOWER);        break;
                    default:	bChangeNumFmt = sal_False;
                    }
                    break;

                case HTML_UNORDERLIST_ON:
                    aNumFmt.SetBulletChar( (sal_Unicode)pOption->GetEnum(
                                    aHTMLULTypeTable,aNumFmt.GetBulletChar() ) );
                    bChangeNumFmt = sal_True;
                    break;
                }
            }
            break;
        case HTML_O_START:
            {
                sal_uInt16 nStart = (sal_uInt16)pOption->GetNumber();
                if( bNewNumFmt )
                {
                    aNumFmt.SetStart( nStart );
                    bChangeNumFmt = sal_True;
                }
                else
                {
                    rInfo.SetNodeStartValue( nLevel, nStart );
                }
            }
            break;
        case HTML_O_STYLE:
            aStyle = pOption->GetString();
            break;
        case HTML_O_CLASS:
            aClass = pOption->GetString();
            break;
        case HTML_O_LANG:
            aLang = pOption->GetString();
            break;
        case HTML_O_DIR:
            aDir = pOption->GetString();
            break;
        case HTML_O_SRC:
            if( bNewNumFmt )
            {
                aBulletSrc = pOption->GetString();
                if( !InternalImgToPrivateURL(aBulletSrc) )
                    aBulletSrc = URIHelper::SmartRel2Abs( INetURLObject( sBaseURL ), aBulletSrc, Link(), false );
            }
            break;
        case HTML_O_WIDTH:
            nWidth = (sal_uInt16)pOption->GetNumber();
            break;
        case HTML_O_HEIGHT:
            nHeight = (sal_uInt16)pOption->GetNumber();
            break;
        case HTML_O_ALIGN:
            eVertOri =
                (SvxFrameVertOrient)pOption->GetEnum( aHTMLImgVAlignTable,
                                                eVertOri );
            break;
        }
    }

    if( aBulletSrc.Len() )
    {
        // Eine Bullet-Liste mit Grafiken
        aNumFmt.SetNumberingType(SVX_NUM_BITMAP);

        // Die Grafik als Brush anlegen
        SvxBrushItem aBrushItem;
        aBrushItem.SetGraphicLink( aBulletSrc );
        aBrushItem.SetGraphicPos( GPOS_AREA );

        // Die Groesse nur beachten, wenn Breite und Hoehe vorhanden sind
        Size aTwipSz( nWidth, nHeight), *pTwipSz=0;
        if( nWidth!=USHRT_MAX && nHeight!=USHRT_MAX )
        {
            aTwipSz =
                Application::GetDefaultDevice()->PixelToLogic( aTwipSz,
                                                    MapMode(MAP_TWIP) );
            pTwipSz = &aTwipSz;
        }

        // Die Ausrichtung auch nur beachten, wenn eine Ausrichtung
        // angegeben wurde
        aNumFmt.SetGraphicBrush( &aBrushItem, pTwipSz,
                            SVX_VERT_NONE!=eVertOri ? &eVertOri : 0);

        // Und noch die Grafik merken, um sie in den Absaetzen nicht
        // einzufuegen
        aBulletGrfs[nLevel] = aBulletSrc;
        bChangeNumFmt = sal_True;
    }
    else
        aBulletGrfs[nLevel].Erase();

    // den aktuellen Absatz erst einmal nicht numerieren
    {
        BYTE nLvl = nLevel;
        SetNoNum(&nLvl, TRUE); // #115962#
        SetNodeNum( nLvl );
    }

    // einen neuen Kontext anlegen
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( nToken );

    // Styles parsen
    if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
    {
        SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
        SvxCSS1PropertyInfo aPropInfo;

        if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
        {
            if( bNewNumFmt )
            {
                if( aPropInfo.bLeftMargin )
                {
                    // Der Der Default-Einzug wurde schon eingefuegt.
                    sal_uInt16 nAbsLSpace =
                        aNumFmt.GetAbsLSpace() - HTML_NUMBUL_MARGINLEFT;
                    if( aPropInfo.nLeftMargin < 0 &&
                        nAbsLSpace < -aPropInfo.nLeftMargin )
                        nAbsLSpace = 0U;
                    else if( aPropInfo.nLeftMargin > USHRT_MAX ||
                             (long)nAbsLSpace +
                                            aPropInfo.nLeftMargin > USHRT_MAX )
                        nAbsLSpace = USHRT_MAX;
                    else
                        nAbsLSpace = nAbsLSpace + (sal_uInt16)aPropInfo.nLeftMargin;

                    aNumFmt.SetAbsLSpace( nAbsLSpace );
                    bChangeNumFmt = sal_True;
                }
                if( aPropInfo.bTextIndent )
                {
                    short nTextIndent =
                        ((const SvxLRSpaceItem &)aItemSet.Get( RES_LR_SPACE ))
                                                        .GetTxtFirstLineOfst();
                    aNumFmt.SetFirstLineOffset( nTextIndent );
                    bChangeNumFmt = sal_True;
                }
            }
            aPropInfo.bLeftMargin = aPropInfo.bTextIndent = sal_False;
            if( !aPropInfo.bRightMargin )
                aItemSet.ClearItem( RES_LR_SPACE );
            DoPositioning( aItemSet, aPropInfo, pCntxt );
            InsertAttrs( aItemSet, aPropInfo, pCntxt );
        }
    }

    if( bChangeNumFmt )
    {
        rInfo.GetNumRule()->Set( nLevel, aNumFmt );
        pDoc->ChgNumRuleFmts( *rInfo.GetNumRule() );
    }

    PushContext( pCntxt );

    // die Attribute der neuen Vorlage setzen
    SetTxtCollAttrs( pCntxt );
}

void SwHTMLParser::EndNumBulList( int nToken )
{
    SwHTMLNumRuleInfo& rInfo = GetNumInfo();

    // Ein neuer Absatz muss aufgemacht werden, wenn
    // - der aktuelle nicht leer ist, also Text oder absatzgebundene Objekte
    //   enthaelt.
    // - der aktuelle Absatz numeriert ist.
    sal_Bool bAppend = pPam->GetPoint()->nContent.GetIndex() > 0;
    if( !bAppend )
    {
        SwTxtNode* pTxtNode = pPam->GetNode()->GetTxtNode();
        bAppend = (pTxtNode && ! pTxtNode->IsOutline() && pTxtNode->IsCounted()) ||
            HasCurrentParaFlys();
    }

    sal_Bool bSpace = (rInfo.GetDepth() + nDefListDeep) == 1;
    if( bAppend )
        AppendTxtNode( bSpace ? AM_SPACE : AM_NOSPACE, sal_False );
    else if( bSpace )
        AddParSpace();

    // den aktuellen Kontext vom Stack holen
    _HTMLAttrContext *pCntxt = nToken!=0 ? PopContext( nToken & ~1 ) : 0;

    // Keine Liste aufgrund eines Tokens beenden, wenn der Kontext
    // nie angelgt wurde oder nicht beendet werden darf.
    if( rInfo.GetDepth()>0 && (!nToken || pCntxt) )
    {
        rInfo.DecDepth();
        if( !rInfo.GetDepth() )		// wars der letze Level ?
        {
            // Die noch nicht angepassten Formate werden jetzt noch
            // angepasst, damit es sich besser Editieren laesst.
            const SwNumFmt *pRefNumFmt = 0;
            sal_Bool bChanged = sal_False;
            for( sal_uInt16 i=0; i<MAXLEVEL; i++ )
            {
                const SwNumFmt *pNumFmt = rInfo.GetNumRule()->GetNumFmt(i);
                if( pNumFmt )
                {
                    pRefNumFmt = pNumFmt;
                }
                else if( pRefNumFmt )
                {
                    SwNumFmt aNumFmt( rInfo.GetNumRule()->Get(i) );
                    aNumFmt.SetNumberingType(pRefNumFmt->GetNumberingType() != SVX_NUM_BITMAP
                                        ? pRefNumFmt->GetNumberingType() : SVX_NUM_CHAR_SPECIAL);
                    if( SVX_NUM_CHAR_SPECIAL == aNumFmt.GetNumberingType() )
                    {
                        aNumFmt.SetBulletFont(
                                &rInfo.GetNumRule()->GetDefBulletFont() );
                        aNumFmt.SetBulletChar( cBulletChar );
                    }
                    aNumFmt.SetAbsLSpace( (i+1) * HTML_NUMBUL_MARGINLEFT );
                    aNumFmt.SetFirstLineOffset( HTML_NUMBUL_INDENT );
                    aNumFmt.SetCharFmt( pRefNumFmt->GetCharFmt() );
                    rInfo.GetNumRule()->Set( i, aNumFmt );
                    bChanged = sal_True;
                }
            }
            if( bChanged )
                pDoc->ChgNumRuleFmts( *rInfo.GetNumRule() );

            // Beim letzen Append wurde das NumRule-Item und das
            // NodeNum-Objekt mit kopiert. Beides muessen wir noch
            // loeschen. Das ResetAttr loescht das NodeNum-Objekt mit!
            pPam->GetNode()->GetTxtNode()->ResetAttr( RES_PARATR_NUMRULE );

            rInfo.Clear();
        }
        else
        {
            // den naechsten Absatz erstmal nicht numerieren
            SetNodeNum( rInfo.GetLevel() | NO_NUMLEVEL );
        }
    }

    // und noch Attribute beenden
    sal_Bool bSetAttrs = sal_False;
    if( pCntxt )
    {
        EndContext( pCntxt );
        delete pCntxt;
        bSetAttrs = sal_True;
    }

    if( nToken )
        SetTxtCollAttrs();

    if( bSetAttrs )
        SetAttr();	// Absatz-Atts wegen JavaScript moeglichst schnell setzen

}

/*  */

void SwHTMLParser::NewNumBulListItem( int nToken )
{
    sal_uInt8 nLevel = GetNumInfo().GetLevel();
    String aId, aStyle, aClass, aLang, aDir;
    sal_uInt16 nStart = HTML_LISTHEADER_ON != nToken
                        ? GetNumInfo().GetNodeStartValue( nLevel )
                        : USHRT_MAX;
    if( USHRT_MAX != nStart )
        GetNumInfo().SetNodeStartValue( nLevel );

    const HTMLOptions *pOptions = GetOptions();
    for( sal_uInt16 i = pOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pOptions)[--i];
        switch( pOption->GetToken() )
        {
            case HTML_O_VALUE:
                nStart = (sal_uInt16)pOption->GetNumber();
                break;
            case HTML_O_ID:
                aId = pOption->GetString();
                break;
            case HTML_O_STYLE:
                aStyle = pOption->GetString();
                break;
            case HTML_O_CLASS:
                aClass = pOption->GetString();
                break;
            case HTML_O_LANG:
                aLang = pOption->GetString();
                break;
            case HTML_O_DIR:
                aDir = pOption->GetString();
                break;
        }
    }

    // einen neuen Absatz aufmachen
    if( pPam->GetPoint()->nContent.GetIndex() )
        AppendTxtNode( AM_NOSPACE, sal_False );
    bNoParSpace = sal_False;	// In <LI> wird kein Abstand eingefuegt!

    if( HTML_LISTHEADER_ON==nToken )
        SetNoNum(&nLevel, TRUE);

    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( nToken );

    String aNumRuleName;
    if( GetNumInfo().GetNumRule() )
    {
        aNumRuleName = GetNumInfo().GetNumRule()->GetName();
    }
    else
    {
        aNumRuleName = pDoc->GetUniqueNumRuleName();
        SwNumRule aNumRule( aNumRuleName );
        SwNumFmt aNumFmt( aNumRule.Get( 0 ) );
        aNumFmt.SetBulletFont( &SwNumRule::GetDefBulletFont() );
        aNumFmt.SetNumberingType(SVX_NUM_CHAR_SPECIAL);
        aNumFmt.SetBulletChar( cBulletChar );	// das Bulletzeichen !!
        aNumFmt.SetCharFmt( pCSS1Parser->GetCharFmtFromPool(RES_POOLCHR_BUL_LEVEL) );
        aNumFmt.SetLSpace( (sal_uInt16)(-HTML_NUMBUL_INDENT) );
        aNumFmt.SetFirstLineOffset( HTML_NUMBUL_INDENT );
        aNumRule.Set( 0, aNumFmt );

        pDoc->MakeNumRule( aNumRuleName, &aNumRule );

        ASSERT( !nOpenParaToken,
                "Jetzt geht ein offenes Absatz-Element verloren" );
        // Wir tun so, als ob wir in einem Absatz sind. Dann wird
        // beim naechsten Absatz wenigstens die Numerierung
        // weggeschmissen, die nach dem naechsten AppendTxtNode uebernommen
        // wird.
        nOpenParaToken = nToken;
    }

    SwTxtNode* pTxtNode = pPam->GetNode()->GetTxtNode();
    ((SwCntntNode *)pTxtNode)->SetAttr( SwNumRuleItem(aNumRuleName) );
    pTxtNode->SetLevel(nLevel);
    // --> OD 2005-11-14 #i57656#
    // <IsCounted()> state of text node has to be adjusted accordingly.
    if ( nLevel >= 0 && nLevel < MAXLEVEL )
    {
        pTxtNode->SetCounted( true );
    }
    // <--
    // --> OD 2005-11-15 #i57919#
    // correction of refactoring done by cws swnumtree
    // - <nStart> contains the start value, if the numbering has to be restarted
    //   at this text node. Value <USHRT_MAX> indicates, that numbering isn't
    //   restarted at this text node
    if ( nStart != USHRT_MAX )
    {
        pTxtNode->SetRestart( true );
        pTxtNode->SetStart( nStart );
    }
    // <--

    if( GetNumInfo().GetNumRule() )
        GetNumInfo().GetNumRule()->SetInvalidRule( sal_True );

    // Styles parsen
    if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
    {
        SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
        SvxCSS1PropertyInfo aPropInfo;

        if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
        {
            DoPositioning( aItemSet, aPropInfo, pCntxt );
            InsertAttrs( aItemSet, aPropInfo, pCntxt );
        }
    }

    PushContext( pCntxt );

    // die neue Vorlage setzen
    SetTxtCollAttrs( pCntxt );

    // Laufbalkenanzeige aktualisieren
    ShowStatline();
}

void SwHTMLParser::EndNumBulListItem( int nToken, sal_Bool bSetColl,
                                      sal_Bool bLastPara )
{
    // einen neuen Absatz aufmachen
    if( !nToken && pPam->GetPoint()->nContent.GetIndex() )
        AppendTxtNode( AM_NOSPACE );

    // Kontext zu dem Token suchen und vom Stack holen
    _HTMLAttrContext *pCntxt = 0;
    sal_uInt16 nPos = aContexts.Count();
    nToken &= ~1;
    while( !pCntxt && nPos>nContextStMin )
    {
        sal_uInt16 nCntxtToken = aContexts[--nPos]->GetToken();
        switch( nCntxtToken )
        {
        case HTML_LI_ON:
        case HTML_LISTHEADER_ON:
            if( !nToken || nToken == nCntxtToken  )
            {
                pCntxt = aContexts[nPos];
                aContexts.Remove( nPos, 1 );
            }
            break;
        case HTML_ORDERLIST_ON:
        case HTML_UNORDERLIST_ON:
        case HTML_MENULIST_ON:
        case HTML_DIRLIST_ON:
            // keine LI/LH ausserhalb der aktuellen Liste betrachten
            nPos = nContextStMin;
            break;
        }
    }

    // und noch Attribute beenden
    if( pCntxt )
    {
        EndContext( pCntxt );
        SetAttr();	// Absatz-Atts wegen JavaScript moeglichst schnell setzen
        delete pCntxt;
    }

    // und die bisherige Vorlage setzen
    if( bSetColl )
        SetTxtCollAttrs();
}

/*  */

void SwHTMLParser::SetNodeNum( sal_uInt8 nLevel )
{
    SwTxtNode* pTxtNode = pPam->GetNode()->GetTxtNode();
    ASSERT( pTxtNode, "Kein Text-Node an PaM-Position" );

    ASSERT( GetNumInfo().GetNumRule(), "Kein Numerierungs-Regel" );
    const String& rName = GetNumInfo().GetNumRule()->GetName();
    ((SwCntntNode *)pTxtNode)->SetAttr( SwNumRuleItem(rName) );

    // --> OD 2005-11-14 #i57656#
    // consider usage of NO_NUMLEVEL - see implementation of <SwTxtNode::SetLevel(..)>
    if ( nLevel >= 0 && ( nLevel & NO_NUMLEVEL ) )
    {
        pTxtNode->SetLevel( nLevel & ~NO_NUMLEVEL );
        pTxtNode->SetCounted( false );
    }
    else
    {
        pTxtNode->SetLevel( nLevel );
        pTxtNode->SetCounted( true );
    }

    // NumRule invalidieren, weil sie durch ein EndAction bereits
    // auf valid geschaltet worden sein kann.
    GetNumInfo().GetNumRule()->SetInvalidRule( sal_False );
}


/*  */

void SwHTMLWriter::FillNextNumInfo()
{
    pNextNumRuleInfo = 0;

    ULONG nPos = pCurPam->GetPoint()->nNode.GetIndex() + 1;

    sal_Bool bDone = sal_False;
    sal_Bool bTable = sal_False;
    do
    {
        const SwNode* pNd = pDoc->GetNodes()[nPos];
        if( pNd->IsTxtNode() )
        {
            // Der naechste wird als naechstes ausgegeben.
            pNextNumRuleInfo = new SwHTMLNumRuleInfo( *pNd->GetTxtNode() );

            // Vor einer Tabelle behalten wir erst einmal die alte Ebene bei,
            // wenn die gleiche Numerierung hinter der Tabelle
            // fortgesetzt wird und dort nicht von vorne numeriert
            // wird. Die Tabelle wird ann beim Import so weit eingeruckt,
            // wie es der Num-Ebene entspricht.
            if( bTable &&
                pNextNumRuleInfo->GetNumRule()==GetNumInfo().GetNumRule() &&
                !pNextNumRuleInfo->IsRestart() )
            {
                pNextNumRuleInfo->SetDepth( GetNumInfo().GetDepth() );
            }
        }
        else if( pNd->IsTableNode() )
        {
            // Eine Tabelle wird uebersprungen, also den Node
            // hinter der Tabelle betrachten.
            nPos = pNd->EndOfSectionIndex() + 1;
            bTable = sal_True;
        }
        else
        {
            // In allen anderen Faellen ist die Numerierung erstmal
            // zu Ende.
            pNextNumRuleInfo = new SwHTMLNumRuleInfo;
        }
    }
    while( !pNextNumRuleInfo );
}

void SwHTMLWriter::ClearNextNumInfo()
{
    delete pNextNumRuleInfo;
    pNextNumRuleInfo = 0;
}

Writer& OutHTML_NumBulListStart( SwHTMLWriter& rWrt,
                                 const SwHTMLNumRuleInfo& rInfo )
{
    SwHTMLNumRuleInfo& rPrevInfo = rWrt.GetNumInfo();
    sal_Bool bSameRule = rPrevInfo.GetNumRule() == rInfo.GetNumRule();
    if( bSameRule && rPrevInfo.GetDepth() >= rInfo.GetDepth() &&
        !rInfo.IsRestart() )
    {
        return rWrt;
    }

    sal_Bool bStartValue = sal_False;
    if( !bSameRule && rInfo.GetDepth() )
    {
        String aName( rInfo.GetNumRule()->GetName() );
        if( rWrt.aNumRuleNames.Seek_Entry( &aName ) )
        {
            // The rule has been applied before
            sal_Int16 eType = rInfo.GetNumRule()
                ->Get( rInfo.GetDepth()-1 ).GetNumberingType();
            if( SVX_NUM_CHAR_SPECIAL != eType && SVX_NUM_BITMAP != eType )
            {
                // If its a numbering rule, the current number should be
                // exported as start value, but only if there are no nodes
                // within the numbering that have a lower level
                bStartValue = sal_True;
                if( rInfo.GetDepth() > 1 )
                {
                    ULONG nPos =
                        rWrt.pCurPam->GetPoint()->nNode.GetIndex() + 1;
                    do
                    {
                        const SwNode* pNd = rWrt.pDoc->GetNodes()[nPos];
                        if( pNd->IsTxtNode() )
                        {
                            const SwTxtNode *pTxtNd = pNd->GetTxtNode();
                            if( !pTxtNd->GetNumRule() )
                            {
                                // node isn't numbered => check completed
                                break;
                            }

                            ASSERT(! pTxtNd->IsOutline(),
                                   "outline not expected");

                            if( pTxtNd->GetLevel() + 1 <
                                rInfo.GetDepth() )
                            {
                                // node is numbered, but level is lower
                                // => check completed
                                bStartValue = sal_False;
                                break;
                            }
                            nPos++;
                        }
                        else if( pNd->IsTableNode() )
                        {
                            // skip table
                            nPos = pNd->EndOfSectionIndex() + 1;
                        }
                        else
                        {
                            // end node or sections start node -> check
                            // completed
                            break;
                        }
                    }
                    while( sal_True );
                }
            }
        }
        else
        {
            rWrt.aNumRuleNames.Insert( new String( aName ) );
        }
    }


    DBG_ASSERT( rWrt.nLastParaToken == 0,
                "<PRE> wurde nicht vor <OL> beendet." );
    sal_uInt16 nPrevDepth =
        (bSameRule && !rInfo.IsRestart()) ? rPrevInfo.GetDepth() : 0;

    for( sal_uInt16 i=nPrevDepth; i<rInfo.GetDepth(); i++ )
    {
        rWrt.OutNewLine(); // <OL>/<UL> in eine neue Zeile

        rWrt.aBulletGrfs[i].Erase();
        ByteString sOut( '<' );
        const SwNumFmt& rNumFmt = rInfo.GetNumRule()->Get( i );
        sal_Int16 eType = rNumFmt.GetNumberingType();
        if( SVX_NUM_CHAR_SPECIAL == eType )
        {
            // Aufzaehlungs-Liste: <OL>
            sOut += sHTML_unorderlist;

            // den Typ ueber das Bullet-Zeichen bestimmen
            const sal_Char *pStr = 0;
            switch( rNumFmt.GetBulletChar() )
            {
            case HTML_BULLETCHAR_DISC:
                pStr = sHTML_ULTYPE_disc;
                break;
            case HTML_BULLETCHAR_CIRCLE:
                pStr = sHTML_ULTYPE_circle;
                break;
            case HTML_BULLETCHAR_SQUARE:
                pStr = sHTML_ULTYPE_square;
                break;
            }

            if( pStr )
                (((sOut += ' ') += sHTML_O_type) += '=') += pStr;
        }
        else if( SVX_NUM_BITMAP == eType )
        {
            // Aufzaehlungs-Liste: <OL>
            sOut += sHTML_unorderlist;
            rWrt.Strm() << sOut.GetBuffer();
            sOut.Erase();

            OutHTML_BulletImage( rWrt,
                                    0,
                                    rNumFmt.GetBrush(),
                                    rWrt.aBulletGrfs[i],
                                    rNumFmt.GetGraphicSize(),
                                    rNumFmt.GetGraphicOrientation() );
        }
        else
        {
            // Numerierungs-Liste: <UL>
            sOut += sHTML_orderlist;

            // den Typ ueber das Format bestimmen
            sal_Char cType = 0;
            switch( eType )
            {
            case SVX_NUM_CHARS_UPPER_LETTER:	cType = 'A'; break;
            case SVX_NUM_CHARS_LOWER_LETTER:	cType = 'a'; break;
            case SVX_NUM_ROMAN_UPPER:			cType = 'I'; break;
            case SVX_NUM_ROMAN_LOWER:			cType = 'i'; break;
            }
            if( cType )
                (((sOut += ' ') += sHTML_O_type) += '=') += cType;

            sal_uInt16 nStartVal = rNumFmt.GetStart();
            if( bStartValue && 1 == nStartVal && i == rInfo.GetDepth()-1 )
            {
                // --> OD 2005-11-02 #i51089 - TUNING#
                if ( rWrt.pCurPam->GetNode()->GetTxtNode()->GetNum() )
                {
                    nStartVal = rWrt.pCurPam->GetNode()->GetTxtNode()->GetNum()->
                        GetNumberVector()[i];
                }
                else
                {
                    ASSERT( false,
                            "<OutHTML_NumBulListStart(..) - text node has no number." );
                }
            }
            if( nStartVal != 1 )
            {
                (((sOut += ' ') += sHTML_O_start) += '=')
                    += ByteString::CreateFromInt32( nStartVal );
            }
        }

        if( sOut.Len() )
            rWrt.Strm() << sOut.GetBuffer();

        if( rWrt.bCfgOutStyles )
            OutCSS1_NumBulListStyleOpt( rWrt, *rInfo.GetNumRule(), (BYTE)i );

        rWrt.Strm() << '>';

        rWrt.IncIndentLevel(); // Inhalt von <OL> einruecken
    }

    return rWrt;
}

Writer& OutHTML_NumBulListEnd( SwHTMLWriter& rWrt,
                               const SwHTMLNumRuleInfo& rNextInfo )
{
    SwHTMLNumRuleInfo& rInfo = rWrt.GetNumInfo();
    sal_Bool bSameRule = rNextInfo.GetNumRule() == rInfo.GetNumRule();
    if( bSameRule && rNextInfo.GetDepth() >= rInfo.GetDepth() &&
        !rNextInfo.IsRestart() )
    {
        return rWrt;
    }

    DBG_ASSERT( rWrt.nLastParaToken == 0,
                "<PRE> wurde nicht vor </OL> beendet." );
    sal_uInt16 nNextDepth =
        (bSameRule && !rNextInfo.IsRestart()) ? rNextInfo.GetDepth() : 0;

    // MIB 23.7.97: Die Schleife muss doch rueckwaerts durchlaufen
    // werden, weil die Reihenfolge von </OL>/</UL> stimmen muss
    for( sal_uInt16 i=rInfo.GetDepth(); i>nNextDepth; i-- )
    {
        rWrt.DecIndentLevel(); // Inhalt von <OL> einruecken
        if( rWrt.bLFPossible )
            rWrt.OutNewLine(); // </OL>/</UL> in eine neue Zeile

        // es wird also eine Liste angefangen oder beendet:
        sal_Int16 eType = rInfo.GetNumRule()->Get( i-1 ).GetNumberingType();
        const sal_Char *pStr;
        if( SVX_NUM_CHAR_SPECIAL == eType || SVX_NUM_BITMAP == eType)
            pStr = sHTML_unorderlist;
        else
            pStr = sHTML_orderlist;
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), pStr, sal_False );
        rWrt.bLFPossible = sal_True;
    }

    return rWrt;
}
