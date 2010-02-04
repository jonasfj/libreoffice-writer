/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlgrin.cxx,v $
 * $Revision: 1.26 $
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

#include "hintids.hxx"
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <i18npool/mslangid.hxx>
#include <svl/stritem.hxx>
#include <svl/urihelper.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/lrspitem.hxx>
#include <svx/adjitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/brshitem.hxx>
#include <svx/colritem.hxx>
#include <svx/boxitem.hxx>
#include <svx/ulspitem.hxx>
#include <svx/langitem.hxx>
#include <svx/scripttypeitem.hxx>
#include <sfx2/docfile.hxx>
#include <svtools/imap.hxx>
#include <svtools/htmltokn.h>
#include <svtools/htmlkywd.hxx>
#include <unotools/eventcfg.hxx>

#include <fmtornt.hxx>
#include <fmturl.hxx>
#include <fmtanchr.hxx>
#include <fmtsrnd.hxx>
#include <fmtinfmt.hxx>
#include <fmtcntnt.hxx>
#include <fmtanchr.hxx>
#include <fmtfsize.hxx>
#include <fmtinfmt.hxx>
#include "frmatr.hxx"
#include "charatr.hxx"
#include <frmfmt.hxx>
#include <charfmt.hxx>
#include <docary.hxx>
#include <docsh.hxx>
#include <pam.hxx>
#include <doc.hxx>
#include <ndtxt.hxx>
#include <shellio.hxx>
#include <poolfmt.hxx>
#include <IMark.hxx>
#include <ndgrf.hxx>
#include <htmlnum.hxx>
#include <swcss1.hxx>
#include <swhtml.hxx>
#include <numrule.hxx>
#include <boost/shared_ptr.hpp>

using namespace ::com::sun::star;


HTMLOptionEnum __FAR_DATA aHTMLImgHAlignTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_AL_left,    text::HoriOrientation::LEFT       },
    { OOO_STRING_SVTOOLS_HTML_AL_right,   text::HoriOrientation::RIGHT      },
    { 0,				0				}
};


HTMLOptionEnum __FAR_DATA aHTMLImgVAlignTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_VA_top,         text::VertOrientation::LINE_TOP       },
    { OOO_STRING_SVTOOLS_HTML_VA_texttop,     text::VertOrientation::CHAR_TOP       },
    { OOO_STRING_SVTOOLS_HTML_VA_middle,      text::VertOrientation::CENTER         },
    { OOO_STRING_SVTOOLS_HTML_AL_center,      text::VertOrientation::CENTER         },
    { OOO_STRING_SVTOOLS_HTML_VA_absmiddle,   text::VertOrientation::LINE_CENTER    },
    { OOO_STRING_SVTOOLS_HTML_VA_bottom,      text::VertOrientation::TOP            },
    { OOO_STRING_SVTOOLS_HTML_VA_baseline,    text::VertOrientation::TOP            },
    { OOO_STRING_SVTOOLS_HTML_VA_absbottom,   text::VertOrientation::LINE_BOTTOM    },
    { 0,					0					}
};

SV_IMPL_PTRARR( ImageMaps, ImageMapPtr )

ImageMap *SwHTMLParser::FindImageMap( const String& rName ) const
{
    ImageMap *pMap = 0;

    ASSERT( rName.GetChar(0) != '#', "FindImageName: Name beginnt mit #!" );

    if( pImageMaps )
    {
        for( USHORT i=0; i<pImageMaps->Count(); i++ )
        {
            ImageMap *pIMap = (*pImageMaps)[i];
            if( rName.EqualsIgnoreCaseAscii( pIMap->GetName() ) )
            {
                pMap = pIMap;
                break;
            }
        }
    }
    return pMap;
}

void SwHTMLParser::ConnectImageMaps()
{
    SwNodes& rNds = pDoc->GetNodes();
    // auf den Start-Node der 1. Section
    ULONG nIdx = rNds.GetEndOfAutotext().StartOfSectionIndex() + 1;
    ULONG nEndIdx = rNds.GetEndOfAutotext().GetIndex();

    SwGrfNode* pGrfNd;
    while( nMissingImgMaps > 0 && nIdx < nEndIdx )
    {
        SwNode *pNd = rNds[nIdx + 1];
        if( 0 != (pGrfNd = pNd->GetGrfNode()) )
        {
            SwFrmFmt *pFmt = pGrfNd->GetFlyFmt();
            SwFmtURL aURL( pFmt->GetURL() );
            const ImageMap *pIMap = aURL.GetMap();
            if( pIMap && pIMap->GetIMapObjectCount()==0 )
            {
                // Die (leere) Image-Map des Nodes wird entweder
                // durch die jetzt gefundene Image-Map ersetzt
                // oder geloescht.
                ImageMap *pNewIMap =
                    FindImageMap( pIMap->GetName() );
                aURL.SetMap( pNewIMap );
                pFmt->SetFmtAttr( aURL );
                if( !pGrfNd->IsScaleImageMap() )
                {
                    // die Grafikgroesse ist mitlerweile da oder dir
                    // Grafik muss nicht skaliert werden
                    pGrfNd->ScaleImageMap();
                }
                nMissingImgMaps--;	// eine Map weniger suchen
            }
        }
        nIdx = rNds[nIdx]->EndOfSectionIndex() + 1;
    }
}


/*  */

void SwHTMLParser::SetAnchorAndAdjustment( sal_Int16 eVertOri,
                                           sal_Int16 eHoriOri,
                                           const SfxItemSet &rCSS1ItemSet,
                                           const SvxCSS1PropertyInfo &rCSS1PropInfo,
                                           SfxItemSet& rFrmItemSet )
{
    const SfxItemSet *pCntnrItemSet = 0;
    USHORT i = aContexts.Count();
    while( !pCntnrItemSet && i > nContextStMin )
        pCntnrItemSet = aContexts[--i]->GetFrmItemSet();

    if( pCntnrItemSet )
    {
        // Wenn wir und in einem Container befinden wird die Verankerung
        // des Containers uebernommen.
        rFrmItemSet.Put( *pCntnrItemSet );
    }
    else if( pCSS1Parser->MayBePositioned( rCSS1PropInfo, TRUE ) )
    {
        // Wenn die Ausrichtung anhand der CSS1-Optionen gesetzt werden kann
        // werden die benutzt.
        SetAnchorAndAdjustment( rCSS1ItemSet, rCSS1PropInfo, rFrmItemSet );
    }
    else
    {
        // Sonst wird die Ausrichtung entsprechend der normalen HTML-Optionen
        // gesetzt.
        SetAnchorAndAdjustment( eVertOri, eHoriOri, rFrmItemSet );
    }
}

void SwHTMLParser::SetAnchorAndAdjustment( sal_Int16 eVertOri,
                                           sal_Int16 eHoriOri,
                                           SfxItemSet& rFrmSet,
                                           BOOL bDontAppend )
{
    BOOL bMoveBackward = FALSE;
    SwFmtAnchor aAnchor( FLY_AS_CHAR );
    sal_Int16 eVertRel = text::RelOrientation::FRAME;

    if( text::HoriOrientation::NONE != eHoriOri )
    {
        // den Absatz-Einzug bestimmen
        USHORT nLeftSpace = 0, nRightSpace = 0;
        short nIndent = 0;
        GetMarginsFromContextWithNumBul( nLeftSpace, nRightSpace, nIndent );

        // Horizonale Ausrichtung und Umlauf bestimmen.
        sal_Int16 eHoriRel;
        SwSurround eSurround;
        switch( eHoriOri )
        {
        case text::HoriOrientation::LEFT:
            eHoriRel = nLeftSpace ? text::RelOrientation::PRINT_AREA : text::RelOrientation::FRAME;
            eSurround = SURROUND_RIGHT;
            break;
        case text::HoriOrientation::RIGHT:
            eHoriRel = nRightSpace ? text::RelOrientation::PRINT_AREA : text::RelOrientation::FRAME;
            eSurround = SURROUND_LEFT;
            break;
        case text::HoriOrientation::CENTER:   // fuer Tabellen
            eHoriRel = text::RelOrientation::FRAME;
            eSurround = SURROUND_NONE;
            break;
        default:
            eHoriRel = text::RelOrientation::FRAME;
            eSurround = SURROUND_PARALLEL;
            break;
        }

        // Einen neuen Absatz aufmachen, wenn der aktuelle
        // absatzgebundene Rahmen ohne Umlauf enthaelt.
        if( !bDontAppend && HasCurrentParaFlys( TRUE ) )
        {
            // Wenn der Absatz nur Grafiken enthaelt, braucht er
            // auch keinen unteren Absatz-Abstand. Da hier auch bei
            // Verwendung von Styles kein Abstand enstehen soll, wird
            // hier auch geweohnlich attributiert !!!
            USHORT nUpper=0, nLower=0;
            GetULSpaceFromContext( nUpper, nLower );
            InsertAttr( SvxULSpaceItem( nUpper, 0, RES_UL_SPACE ), FALSE, TRUE );

            AppendTxtNode( AM_NOSPACE );

            if( nUpper )
            {
                NewAttr( &aAttrTab.pULSpace, SvxULSpaceItem( 0, nLower, RES_UL_SPACE ) );
                aParaAttrs.Insert( aAttrTab.pULSpace, aParaAttrs.Count() );
                EndAttr( aAttrTab.pULSpace, 0, FALSE );
            }
        }

        // Vertikale Ausrichtung und Verankerung bestimmen.
        xub_StrLen nCntnt = pPam->GetPoint()->nContent.GetIndex();
        if( nCntnt )
        {
            aAnchor.SetType( FLY_AT_CHAR );
            bMoveBackward = TRUE;
            eVertOri = text::VertOrientation::CHAR_BOTTOM;
            eVertRel = text::RelOrientation::CHAR;
        }
        else
        {
            aAnchor.SetType( FLY_AT_PARA );
            eVertOri = text::VertOrientation::TOP;
            eVertRel = text::RelOrientation::PRINT_AREA;
        }

        rFrmSet.Put( SwFmtHoriOrient( 0, eHoriOri, eHoriRel) );

        rFrmSet.Put( SwFmtSurround( eSurround ) );
    }
    rFrmSet.Put( SwFmtVertOrient( 0, eVertOri, eVertRel) );

    if( bMoveBackward )
        pPam->Move( fnMoveBackward );

    aAnchor.SetAnchor( pPam->GetPoint() );

    if( bMoveBackward )
        pPam->Move( fnMoveForward );

    rFrmSet.Put( aAnchor );
}

void SwHTMLParser::RegisterFlyFrm( SwFrmFmt *pFlyFmt )
{
    // automatisch verankerte Rahmen muessen noch um eine Position
    // nach vorne verschoben werden.
    if( RES_DRAWFRMFMT != pFlyFmt->Which() &&
        (FLY_AT_PARA == pFlyFmt->GetAnchor().GetAnchorId()) &&
        SURROUND_THROUGHT == pFlyFmt->GetSurround().GetSurround() )
    {
        aMoveFlyFrms.Insert( pFlyFmt, aMoveFlyFrms.Count() );
        aMoveFlyCnts.Insert( pPam->GetPoint()->nContent.GetIndex(),
                             aMoveFlyCnts.Count() );
    }
}

/*  */

void SwHTMLParser::GetDefaultScriptType( ScriptType& rType,
                                         String& rTypeStr ) const
{
    SwDocShell *pDocSh = pDoc->GetDocShell();
    SvKeyValueIterator* pHeaderAttrs = pDocSh ? pDocSh->GetHeaderAttributes()
                                              : 0;
    rType = GetScriptType( pHeaderAttrs );
    rTypeStr = GetScriptTypeString( pHeaderAttrs );
}

/*  */

void SwHTMLParser::InsertImage()
{
    // und jetzt auswerten
    String sGrfNm, sAltNm, aId, aClass, aStyle, aMap, sHTMLGrfName;
    sal_Int16 eVertOri = text::VertOrientation::TOP;
    sal_Int16 eHoriOri = text::HoriOrientation::NONE;
    long nWidth=0, nHeight=0;
    long nVSpace=0, nHSpace=0;

    USHORT nBorder = (aAttrTab.pINetFmt ? 1 : 0);
    BOOL bIsMap = FALSE;
    BOOL bPrcWidth = FALSE;
    BOOL bPrcHeight = FALSE;
    SvxMacroItem aMacroItem(RES_FRMMACRO);

    ScriptType eDfltScriptType;
    String sDfltScriptType;
    GetDefaultScriptType( eDfltScriptType, sDfltScriptType );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( USHORT i = pHTMLOptions->Count(); i; )
    {
        USHORT nEvent = 0;
        ScriptType eScriptType2 = eDfltScriptType;
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        switch( pOption->GetToken() )
        {
            case HTML_O_ID:
                aId = pOption->GetString();
                break;
            case HTML_O_STYLE:
                aStyle = pOption->GetString();
                break;
            case HTML_O_CLASS:
                aClass = pOption->GetString();
                break;
            case HTML_O_SRC:
                sGrfNm = pOption->GetString();
                if( !InternalImgToPrivateURL(sGrfNm) )
                    sGrfNm = INetURLObject::GetAbsURL( sBaseURL, sGrfNm );
                break;
            case HTML_O_ALIGN:
                eVertOri =
                    pOption->GetEnum( aHTMLImgVAlignTable,
                                                    text::VertOrientation::TOP );
                eHoriOri =
                    pOption->GetEnum( aHTMLImgHAlignTable,
                                                    text::HoriOrientation::NONE );
                break;
            case HTML_O_WIDTH:
                // erstmal nur als Pixelwerte merken!
                nWidth = pOption->GetNumber();
                bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
                if( bPrcWidth && nWidth>100 )
                    nWidth = 100;
                break;
            case HTML_O_HEIGHT:
                // erstmal nur als Pixelwerte merken!
                nHeight = pOption->GetNumber();
                bPrcHeight = (pOption->GetString().Search('%') != STRING_NOTFOUND);
                if( bPrcHeight && nHeight>100 )
                    nHeight = 100;
                break;
            case HTML_O_VSPACE:
                nVSpace = pOption->GetNumber();
                break;
            case HTML_O_HSPACE:
                nHSpace = pOption->GetNumber();
                break;
            case HTML_O_ALT:
                sAltNm = pOption->GetString();
                break;
            case HTML_O_BORDER:
                nBorder = (USHORT)pOption->GetNumber();
                break;
            case HTML_O_ISMAP:
                bIsMap = TRUE;
                break;
            case HTML_O_USEMAP:
                aMap = pOption->GetString();
                break;
            case HTML_O_NAME:
                sHTMLGrfName = pOption->GetString();
                break;

            case HTML_O_SDONLOAD:
                eScriptType2 = STARBASIC;
            case HTML_O_ONLOAD:
                nEvent = SVX_EVENT_IMAGE_LOAD;
                goto IMAGE_SETEVENT;

            case HTML_O_SDONABORT:
                eScriptType2 = STARBASIC;
            case HTML_O_ONABORT:
                nEvent = SVX_EVENT_IMAGE_ABORT;
                goto IMAGE_SETEVENT;

            case HTML_O_SDONERROR:
                eScriptType2 = STARBASIC;
            case HTML_O_ONERROR:
                nEvent = SVX_EVENT_IMAGE_ERROR;
                goto IMAGE_SETEVENT;
IMAGE_SETEVENT:
                {
                    String sTmp( pOption->GetString() );
                    if( sTmp.Len() )
                    {
                        sTmp.ConvertLineEnd();
                        String sScriptType;
                        if( EXTENDED_STYPE == eScriptType2 )
                            sScriptType = sDfltScriptType;
                        aMacroItem.SetMacro( nEvent,
                            SvxMacro( sTmp, sScriptType, eScriptType2 ));
                    }
                }
                break;
        }
    }

    if( !sGrfNm.Len() )
        return;

    // Wenn wir in einer Numerierung stehen und der Absatz noch leer und
    // nicht numeriert ist, handelt es sich vielleicht um die Grafik
    // einer Bullet-Liste
    if( !pPam->GetPoint()->nContent.GetIndex() &&
        GetNumInfo().GetDepth() > 0 && GetNumInfo().GetDepth() <= MAXLEVEL &&
        aBulletGrfs[GetNumInfo().GetDepth()-1].Len() &&
        aBulletGrfs[GetNumInfo().GetDepth()-1]==sGrfNm )
    {
        SwTxtNode* pTxtNode = pPam->GetNode()->GetTxtNode();

        if( pTxtNode && ! pTxtNode->IsCountedInList())
        {
            ASSERT( pTxtNode->GetActualListLevel() == GetNumInfo().GetLevel(),
                    "Numerierungs-Ebene stimmt nicht" );

            pTxtNode->SetCountedInList( true );

            // Rule invalisieren ist noetig, weil zwischem dem einlesen
            // des LI und der Grafik ein EndAction gerufen worden sein kann.
            if( GetNumInfo().GetNumRule() )
                GetNumInfo().GetNumRule()->SetInvalidRule( TRUE );

            // Die Vorlage novh mal setzen. Ist noetig, damit der
            // Erstzeilen-Einzug stimmt.
            SetTxtCollAttrs();

            return;
        }
    }

    SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aPropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
        ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo );

    SfxItemSet aFrmSet( pDoc->GetAttrPool(),
                        RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
    if( !IsNewDoc() )
        Reader::ResetFrmFmtAttrs( aFrmSet );

    // Umrandung setzen
    long nHBorderWidth = 0, nVBorderWidth = 0;
    if( nBorder )
    {
        nHBorderWidth = (long)nBorder;
        nVBorderWidth = (long)nBorder;
        SvxCSS1Parser::PixelToTwip( nVBorderWidth, nHBorderWidth );

        SvxBorderLine aHBorderLine;
        SvxBorderLine aVBorderLine;

        SvxCSS1Parser::SetBorderWidth( aHBorderLine,
                                       (USHORT)nHBorderWidth, FALSE );
        if( nHBorderWidth == nVBorderWidth )
            aVBorderLine.SetOutWidth( aHBorderLine.GetOutWidth() );
        else
            SvxCSS1Parser::SetBorderWidth( aVBorderLine,
                                           (USHORT)nVBorderWidth, FALSE );

        // die tatsaechlich gesetzter Rahmenbreite benutzen und nicht die
        // Wunschbreite!
        nHBorderWidth = aHBorderLine.GetOutWidth();
        nVBorderWidth = aVBorderLine.GetOutWidth();

        if( aAttrTab.pINetFmt )
        {
            const String& rURL =
                ((const SwFmtINetFmt&)aAttrTab.pINetFmt->GetItem()).GetValue();

            pCSS1Parser->SetATagStyles();
            USHORT nPoolId =  static_cast< USHORT >(pDoc->IsVisitedURL( rURL )
                                    ? RES_POOLCHR_INET_VISIT
                                    : RES_POOLCHR_INET_NORMAL);
            const SwCharFmt *pCharFmt =	pCSS1Parser->GetCharFmtFromPool( nPoolId );
            aHBorderLine.SetColor( pCharFmt->GetColor().GetValue() );
            aVBorderLine.SetColor( aHBorderLine.GetColor() );
        }
        else
        {
            const SvxColorItem& rColorItem = aAttrTab.pFontColor ?
              (const SvxColorItem &)aAttrTab.pFontColor->GetItem() :
              (const SvxColorItem &)pDoc->GetDefault(RES_CHRATR_COLOR);
            aHBorderLine.SetColor( rColorItem.GetValue() );
            aVBorderLine.SetColor( aHBorderLine.GetColor() );
        }


        SvxBoxItem aBoxItem( RES_BOX );
        aBoxItem.SetLine( &aHBorderLine, BOX_LINE_TOP );
        aBoxItem.SetLine( &aHBorderLine, BOX_LINE_BOTTOM );
        aBoxItem.SetLine( &aVBorderLine, BOX_LINE_LEFT );
        aBoxItem.SetLine( &aVBorderLine, BOX_LINE_RIGHT );
        aFrmSet.Put( aBoxItem );
    }

    // Ausrichtung setzen
    SetAnchorAndAdjustment( eVertOri, eHoriOri, aItemSet, aPropInfo, aFrmSet );

    // Abstaende setzen
    SetSpace( Size( nHSpace, nVSpace), aItemSet, aPropInfo, aFrmSet );

    // Sonstige CSS1-Attribute Setzen
    SetFrmFmtAttrs( aItemSet, aPropInfo, HTML_FF_BOX, aFrmSet );

    Size aTwipSz( bPrcWidth ? 0 : nWidth, bPrcHeight ? 0 : nHeight );
    if( (aTwipSz.Width() || aTwipSz.Height()) && Application::GetDefaultDevice() )
    {
        aTwipSz = Application::GetDefaultDevice()
                    ->PixelToLogic( aTwipSz, MapMode( MAP_TWIP ) );
    }

    // CSS1-Groesse auf "normale" Groesse umrechnen
    switch( aPropInfo.eWidthType )
    {
        case SVX_CSS1_LTYPE_TWIP:
            aTwipSz.Width() = aPropInfo.nWidth;
            nWidth = 1;	// != 0
            bPrcWidth = FALSE;
            break;
        case SVX_CSS1_LTYPE_PERCENTAGE:
            aTwipSz.Width() = 0;
            nWidth = aPropInfo.nWidth;
            bPrcWidth = TRUE;
            break;
        default:
            ;
    }
    switch( aPropInfo.eHeightType )
    {
        case SVX_CSS1_LTYPE_TWIP:
            aTwipSz.Height() = aPropInfo.nHeight;
            nHeight = 1;	// != 0
            bPrcHeight = FALSE;
            break;
        case SVX_CSS1_LTYPE_PERCENTAGE:
            aTwipSz.Height() = 0;
            nHeight = aPropInfo.nHeight;
            bPrcHeight = TRUE;
            break;
        default:
            ;
    }

    Size aGrfSz( 0, 0 );
    BOOL bSetTwipSize = TRUE;		// Twip-Size am Node setzen?
    BOOL bChangeFrmSize = FALSE;	// Frame-Format nachtraeglich anpassen?
    BOOL bRequestGrfNow = FALSE;
    BOOL bSetScaleImageMap = FALSE;
    BYTE nPrcWidth = 0, nPrcHeight = 0;

    if( !nWidth || !nHeight )
    {
        // Es fehlt die Breite oder die Hoehe
        // Wenn die Grfik in einer Tabelle steht, wird sie gleich
        // angefordert, damit sie eventuell schon da ist, bevor die
        // Tabelle layoutet wird.
        if( pTable!=0 && !nWidth )
        {
            bRequestGrfNow = TRUE;
            IncGrfsThatResizeTable();
        }

        // Die Groesse des Rahmens wird nachtraeglich gesetzt
        bChangeFrmSize = TRUE;
        aGrfSz = aTwipSz;
        if( !nWidth && !nHeight )
        {
            aTwipSz.Width() = HTML_DFLT_IMG_WIDTH;
            aTwipSz.Height() = HTML_DFLT_IMG_HEIGHT;
        }
        else if( nWidth )
        {
            // eine %-Angabe
            if( bPrcWidth )
            {
                nPrcWidth = (BYTE)nWidth;
                nPrcHeight = 255;
            }
            else
            {
                aTwipSz.Height() = HTML_DFLT_IMG_HEIGHT;
            }
        }
        else if( nHeight )
        {
            if( bPrcHeight )
            {
                nPrcHeight = (BYTE)nHeight;
                nPrcWidth = 255;
            }
            else
            {
                aTwipSz.Width() = HTML_DFLT_IMG_WIDTH;
            }
        }
    }
    else
    {
        // Breite und Hoehe wurden angegeben und brauchen nicht gesetzt
        // zu werden
        bSetTwipSize = FALSE;

        if( bPrcWidth )
            nPrcWidth = (BYTE)nWidth;

        if( bPrcHeight )
            nPrcHeight = (BYTE)nHeight;
    }

    // Image-Map setzen
    aMap.EraseTrailingChars();
    if( aMap.Len() )
    {
        // Da wir nur lokale Image-Maps kennen nehmen wireinfach alles
        // hinter dem # als Namen
        xub_StrLen nPos = aMap.Search( '#' );
        String aName;
        if ( STRING_NOTFOUND==nPos )
            aName = aMap ;
        else
            aName = aMap.Copy(nPos+1);

        ImageMap *pImgMap = FindImageMap( aName );
        if( pImgMap )
        {
            SwFmtURL aURL; aURL.SetMap( pImgMap );//wird kopieiert

            bSetScaleImageMap = !nPrcWidth || !nPrcHeight;
            aFrmSet.Put( aURL );
        }
        else
        {
            ImageMap aEmptyImgMap( aName );
            SwFmtURL aURL; aURL.SetMap( &aEmptyImgMap );//wird kopieiert
            aFrmSet.Put( aURL );
            nMissingImgMaps++;			// es fehlen noch Image-Maps

            // die Grafik muss beim SetTwipSize skaliert werden, wenn
            // wir keine Groesse am Node gesetzt haben oder die Groesse
            // nicht der Grafikgroesse entsprach.
            bSetScaleImageMap = sal_True;
        }
    }

    // min. Werte einhalten !!
    if( nPrcWidth )
    {
        ASSERT( !aTwipSz.Width(),
                "Wieso ist da trotz %-Angabe eine Breite gesetzt?" );
        aTwipSz.Width() = aGrfSz.Width() ? aGrfSz.Width()
                                         : HTML_DFLT_IMG_WIDTH;
    }
    else
    {
        aTwipSz.Width() += 2*nVBorderWidth;
        if( aTwipSz.Width() < MINFLY )
            aTwipSz.Width() = MINFLY;
    }
    if( nPrcHeight )
    {
        ASSERT( !aTwipSz.Height(),
                "Wieso ist da trotz %-Angabe eine Hoehe gesetzt?" );
        aTwipSz.Height() = aGrfSz.Height() ? aGrfSz.Height()
                                           : HTML_DFLT_IMG_HEIGHT;
    }
    else
    {
        aTwipSz.Height() += 2*nHBorderWidth;
        if( aTwipSz.Height() < MINFLY )
            aTwipSz.Height() = MINFLY;
    }

    SwFmtFrmSize aFrmSize( ATT_FIX_SIZE, aTwipSz.Width(), aTwipSz.Height() );
    aFrmSize.SetWidthPercent( nPrcWidth );
    aFrmSize.SetHeightPercent( nPrcHeight );
    aFrmSet.Put( aFrmSize );

    Graphic aEmptyGrf;
    aEmptyGrf.SetDefaultType();
    SwFrmFmt *pFlyFmt = pDoc->Insert( *pPam, sGrfNm, aEmptyStr, &aEmptyGrf,
                                      &aFrmSet, NULL, NULL );
    SwGrfNode *pGrfNd = pDoc->GetNodes()[ pFlyFmt->GetCntnt().GetCntntIdx()
                                  ->GetIndex()+1 ]->GetGrfNode();

    if( sHTMLGrfName.Len() )
    {
        pFlyFmt->SetName( sHTMLGrfName );

        // ggfs. eine Grafik anspringen
        if( JUMPTO_GRAPHIC == eJumpTo && sHTMLGrfName == sJmpMark )
        {
            bChkJumpMark = TRUE;
            eJumpTo = JUMPTO_NONE;
        }
    }

    if( sAltNm.Len() )
        pGrfNd->SetTitle( sAltNm );

    if( bSetTwipSize )
        pGrfNd->SetTwipSize( aGrfSz );

    pGrfNd->SetChgTwipSize( bChangeFrmSize, bChangeFrmSize );

    if( bSetScaleImageMap )
        pGrfNd->SetScaleImageMap( TRUE );

    if( aAttrTab.pINetFmt )
    {
        const SwFmtINetFmt &rINetFmt =
            (const SwFmtINetFmt&)aAttrTab.pINetFmt->GetItem();

        SwFmtURL aURL( pFlyFmt->GetURL() );

        aURL.SetURL( rINetFmt.GetValue(), bIsMap );
        aURL.SetTargetFrameName( rINetFmt.GetTargetFrame() );
        aURL.SetName( rINetFmt.GetName() );
        pFlyFmt->SetFmtAttr( aURL );

        {
            const SvxMacro *pMacro;
            static USHORT __READONLY_DATA aEvents[] = {
                SFX_EVENT_MOUSEOVER_OBJECT,
                SFX_EVENT_MOUSECLICK_OBJECT,
                SFX_EVENT_MOUSEOUT_OBJECT,
                0 };

            for( USHORT n = 0; aEvents[ n ]; ++n )
                if( 0 != ( pMacro = rINetFmt.GetMacro( aEvents[ n ] ) ))
                    aMacroItem.SetMacro( aEvents[ n ], *pMacro );
        }

        if ((FLY_AS_CHAR == pFlyFmt->GetAnchor().GetAnchorId()) &&
            aAttrTab.pINetFmt->GetSttPara() ==
                        pPam->GetPoint()->nNode &&
            aAttrTab.pINetFmt->GetSttCnt() ==
                        pPam->GetPoint()->nContent.GetIndex() - 1 )
        {
            // das Attribut wurde unmitellbar vor einer zeichengeb.
            // Grafik eingefuegt, also verschieben wir es
            aAttrTab.pINetFmt->SetStart( *pPam->GetPoint() );

            // Wenn das Attribut auch ein Sprungziel ist, fuegen
            // wir noch eine Bookmark vor der Grafik ein, weil das
            // SwFmtURL kein Sprungziel ist.
            if( rINetFmt.GetName().Len() )
            {
                pPam->Move( fnMoveBackward );
                InsertBookmark( rINetFmt.GetName() );
                pPam->Move( fnMoveForward );
            }
        }

    }

    if( aMacroItem.GetMacroTable().Count() )
        pFlyFmt->SetFmtAttr( aMacroItem );

    // Wenn die Grafik gleich angeforder wird, muss dies geschehen,
    // nachdem das Format vollstaendig aufgebaut ist, weil es evtl.
    // gleich (synchron) angepasst wird (war bug #40983#)
    if( bRequestGrfNow )
    {
        pGrfNd->SwapIn();
    }

    // Ggf. Frames anlegen und Auto-gebundenen Rahmen registrieren
    RegisterFlyFrm( pFlyFmt );

    if( aId.Len() )
        InsertBookmark( aId );
}

/*  */

void SwHTMLParser::InsertBodyOptions()
{
    pDoc->SetTxtFmtColl( *pPam,
                         pCSS1Parser->GetTxtCollFromPool( RES_POOLCOLL_TEXT ) );

    String aBackGround, aId, aStyle, aLang, aDir;
    Color aBGColor, aTextColor, aLinkColor, aVLinkColor;
    BOOL bBGColor=FALSE, bTextColor=FALSE;
    BOOL bLinkColor=FALSE, bVLinkColor=FALSE;

    ScriptType eDfltScriptType;
    String sDfltScriptType;
    GetDefaultScriptType( eDfltScriptType, sDfltScriptType );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( USHORT i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        ScriptType eScriptType2 = eDfltScriptType;
        rtl::OUString aEvent;
        BOOL bSetEvent = FALSE;

        switch( pOption->GetToken() )
        {
            case HTML_O_ID:
                aId = pOption->GetString();
                break;
            case HTML_O_BACKGROUND:
                aBackGround = pOption->GetString();
                break;
            case HTML_O_BGCOLOR:
                pOption->GetColor( aBGColor );
                bBGColor = TRUE;
                break;
            case HTML_O_TEXT:
                pOption->GetColor( aTextColor );
                bTextColor = TRUE;
                break;
            case HTML_O_LINK:
                pOption->GetColor( aLinkColor );
                bLinkColor = TRUE;
                break;
            case HTML_O_VLINK:
                pOption->GetColor( aVLinkColor );
                bVLinkColor = TRUE;
                break;

            case HTML_O_SDONLOAD:
                eScriptType2 = STARBASIC;
            case HTML_O_ONLOAD:
                aEvent = GlobalEventConfig::GetEventName( STR_EVENT_OPENDOC );
                bSetEvent = TRUE;
                break;

            case HTML_O_SDONUNLOAD:
                eScriptType2 = STARBASIC;
            case HTML_O_ONUNLOAD:
                aEvent = GlobalEventConfig::GetEventName( STR_EVENT_PREPARECLOSEDOC );
                bSetEvent = TRUE;
                break;

            case HTML_O_SDONFOCUS:
                eScriptType2 = STARBASIC;
            case HTML_O_ONFOCUS:
                aEvent = GlobalEventConfig::GetEventName( STR_EVENT_ACTIVATEDOC );
                bSetEvent = TRUE;
                break;

            case HTML_O_SDONBLUR:
                eScriptType2 = STARBASIC;
            case HTML_O_ONBLUR:
                aEvent = GlobalEventConfig::GetEventName( STR_EVENT_DEACTIVATEDOC );
                bSetEvent = TRUE;
                break;

            case HTML_O_ONERROR:
//				if( bAnyStarBasic )
//					InsertBasicDocEvent( SFX_EVENT_ACTIVATEDOC,
//										 pOption->GetString() );
                break;

            case HTML_O_STYLE:
                aStyle = pOption->GetString();
                bTextColor = TRUE;
                break;
            case HTML_O_LANG:
                aLang = pOption->GetString();
                break;
            case HTML_O_DIR:
                aDir = pOption->GetString();
                break;
        }

        if( bSetEvent )
        {
            const String& rEvent = pOption->GetString();
            if( rEvent.Len() )
                InsertBasicDocEvent( aEvent, rEvent, eScriptType2,
                                     sDfltScriptType );
        }
    }

    if( bTextColor && !pCSS1Parser->IsBodyTextSet() )
    {
        // Die Textfarbe wird an der Standard-Vorlage gesetzt
        pCSS1Parser->GetTxtCollFromPool( RES_POOLCOLL_STANDARD )
            ->SetFmtAttr( SvxColorItem(aTextColor, RES_CHRATR_COLOR) );
        pCSS1Parser->SetBodyTextSet();
    }


    // Die Item fuer die Seitenvorlage vorbereiten (Hintergrund, Umrandung)
    // Beim BrushItem muessen schon gesetzte werte erhalten bleiben!
    SvxBrushItem aBrushItem( pCSS1Parser->GetPageDescBackground() );
    BOOL bSetBrush = FALSE;

    if( bBGColor && !pCSS1Parser->IsBodyBGColorSet() )
    {
        // Hintergrundfarbe aus "BGCOLOR"
        String aLink;
        if( aBrushItem.GetGraphicLink() )
            aLink = *aBrushItem.GetGraphicLink();
        SvxGraphicPosition ePos = aBrushItem.GetGraphicPos();

        aBrushItem.SetColor( aBGColor );

        if( aLink.Len() )
        {
            aBrushItem.SetGraphicLink( aLink );
            aBrushItem.SetGraphicPos( ePos );
        }
        bSetBrush = TRUE;
        pCSS1Parser->SetBodyBGColorSet();
    }

    if( aBackGround.Len() && !pCSS1Parser->IsBodyBackgroundSet() )
    {
        // Hintergrundgrafik aus "BACKGROUND"
        aBrushItem.SetGraphicLink( INetURLObject::GetAbsURL( sBaseURL, aBackGround ) );
        aBrushItem.SetGraphicPos( GPOS_TILED );
        bSetBrush = TRUE;
        pCSS1Parser->SetBodyBackgroundSet();
    }

    if( aStyle.Len() || aDir.Len() )
    {
        SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
        SvxCSS1PropertyInfo aPropInfo;
        String aDummy;
        ParseStyleOptions( aStyle, aDummy, aDummy, aItemSet, aPropInfo, 0, &aDir );

        // Ein par Attribute muessen an der Seitenvorlage gesetzt werden,
        // und zwar die, die nicht vererbit werden
        pCSS1Parser->SetPageDescAttrs( bSetBrush ? &aBrushItem : 0,
                                       &aItemSet );

        const SfxPoolItem *pItem;
        static USHORT aWhichIds[3] = { RES_CHRATR_FONTSIZE,
                                       RES_CHRATR_CJK_FONTSIZE,
                                       RES_CHRATR_CTL_FONTSIZE };
        for( USHORT i=0; i<3; i++ )
        {
            if( SFX_ITEM_SET == aItemSet.GetItemState( aWhichIds[i], FALSE,
                                                       &pItem ) &&
                static_cast <const SvxFontHeightItem * >(pItem)->GetProp() != 100)
            {
                sal_uInt32 nHeight =
                    ( aFontHeights[2] *
                     static_cast <const SvxFontHeightItem * >(pItem)->GetProp() ) / 100;
                SvxFontHeightItem aNewItem( nHeight, 100, aWhichIds[i] );
                aItemSet.Put( aNewItem );
            }
        }

        // alle noch uebrigen Optionen koennen an der Standard-Vorlage
        // gesetzt werden und gelten dann automatisch als defaults
        pCSS1Parser->GetTxtCollFromPool( RES_POOLCOLL_STANDARD )
            ->SetFmtAttr( aItemSet );
    }
    else if( bSetBrush )
    {
        pCSS1Parser->SetPageDescAttrs( &aBrushItem );
    }

    if( bLinkColor && !pCSS1Parser->IsBodyLinkSet() )
    {
        SwCharFmt *pCharFmt =
            pCSS1Parser->GetCharFmtFromPool(RES_POOLCHR_INET_NORMAL);
        pCharFmt->SetFmtAttr( SvxColorItem(aLinkColor, RES_CHRATR_COLOR) );
        pCSS1Parser->SetBodyLinkSet();
    }
    if( bVLinkColor && !pCSS1Parser->IsBodyVLinkSet() )
    {
        SwCharFmt *pCharFmt =
            pCSS1Parser->GetCharFmtFromPool(RES_POOLCHR_INET_VISIT);
        pCharFmt->SetFmtAttr( SvxColorItem(aVLinkColor, RES_CHRATR_COLOR) );
        pCSS1Parser->SetBodyVLinkSet();
    }
    if( aLang.Len() )
    {
        LanguageType eLang = MsLangId::convertIsoStringToLanguage( aLang );
        sal_uInt16 nWhich = 0;
        if( LANGUAGE_DONTKNOW != eLang )
        {
            switch( SvtLanguageOptions::GetScriptTypeOfLanguage( eLang ) )
            {
            case SCRIPTTYPE_LATIN:
                nWhich = RES_CHRATR_LANGUAGE;
                break;
            case SCRIPTTYPE_ASIAN:
                nWhich = RES_CHRATR_CJK_LANGUAGE;
                break;
            case SCRIPTTYPE_COMPLEX:
                nWhich = RES_CHRATR_CTL_LANGUAGE;
                break;
            }
            if( nWhich )
            {
                SvxLanguageItem aLanguage( eLang, nWhich );
                aLanguage.SetWhich( nWhich );
                pDoc->SetDefault( aLanguage );
            }
        }
    }

    if( aId.Len() )
        InsertBookmark( aId );
}

/*  */

void SwHTMLParser::NewAnchor()
{
    // den voherigen Link beenden, falls es einen gab
    _HTMLAttrContext *pOldCntxt = PopContext( HTML_ANCHOR_ON );
    if( pOldCntxt )
    {
        // und ggf. die Attribute beenden
        EndContext( pOldCntxt );
        delete pOldCntxt;
    }

    SvxMacroTableDtor aMacroTbl;
    String sHRef, aName, sTarget;
    String aId, aStyle, aClass, aLang, aDir;
    BOOL bHasHRef = FALSE, bFixed = FALSE;

    ScriptType eDfltScriptType;
    String sDfltScriptType;
    GetDefaultScriptType( eDfltScriptType, sDfltScriptType );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( USHORT i = pHTMLOptions->Count(); i; )
    {
        USHORT nEvent = 0;
        ScriptType eScriptType2 = eDfltScriptType;
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        switch( pOption->GetToken() )
        {
            case HTML_O_NAME:
                aName = pOption->GetString();
                break;

            case HTML_O_HREF:
                sHRef = pOption->GetString();
                bHasHRef = TRUE;
                break;
            case HTML_O_TARGET:
                sTarget = pOption->GetString();
                break;

            case HTML_O_STYLE:
                aStyle = pOption->GetString();
                break;
            case HTML_O_ID:
                aId = pOption->GetString();
                break;
            case HTML_O_CLASS:
                aClass = pOption->GetString();
                break;
            case HTML_O_SDFIXED:
                bFixed = TRUE;
                break;
            case HTML_O_LANG:
                aLang = pOption->GetString();
                break;
            case HTML_O_DIR:
                aDir = pOption->GetString();
                break;

            case HTML_O_SDONCLICK:
                eScriptType2 = STARBASIC;
            case HTML_O_ONCLICK:
                nEvent = SFX_EVENT_MOUSECLICK_OBJECT;
                goto ANCHOR_SETEVENT;

            case HTML_O_SDONMOUSEOVER:
                eScriptType2 = STARBASIC;
            case HTML_O_ONMOUSEOVER:
                nEvent = SFX_EVENT_MOUSEOVER_OBJECT;
                goto ANCHOR_SETEVENT;

            case HTML_O_SDONMOUSEOUT:
                eScriptType2 = STARBASIC;
            case HTML_O_ONMOUSEOUT:
                nEvent = SFX_EVENT_MOUSEOUT_OBJECT;
                goto ANCHOR_SETEVENT;
ANCHOR_SETEVENT:
                {
                    String sTmp( pOption->GetString() );
                    if( sTmp.Len() )
                    {
                        sTmp.ConvertLineEnd();
                        String sScriptType;
                        if( EXTENDED_STYPE == eScriptType2 )
                            sScriptType = sDfltScriptType;
                        aMacroTbl.Insert( nEvent,
                            new SvxMacro( sTmp, sScriptType, eScriptType2 ));
                    }
                }
                break;

        }
    }

    // Sprungziele, die unseren ipmliziten Zielen entsprechen, schmeissen
    // wir hier ganz rigoros raus.
    if( aName.Len() )
    {
        String sDecoded( INetURLObject::decode( aName, INET_HEX_ESCAPE,
                                           INetURLObject::DECODE_UNAMBIGUOUS,
                                        RTL_TEXTENCODING_UTF8 ));
        xub_StrLen nPos = sDecoded.SearchBackward( cMarkSeperator );
        if( STRING_NOTFOUND != nPos )
        {
            String sCmp( sDecoded.Copy( nPos+1 ) );
            sCmp.EraseAllChars();
            if( sCmp.Len() )
            {
                sCmp.ToLowerAscii();
                if( sCmp.EqualsAscii( pMarkToRegion ) ||
                    sCmp.EqualsAscii( pMarkToFrame ) ||
                    sCmp.EqualsAscii( pMarkToGraphic ) ||
                    sCmp.EqualsAscii( pMarkToOLE ) ||
                    sCmp.EqualsAscii( pMarkToTable ) ||
                    sCmp.EqualsAscii( pMarkToOutline ) ||
                    sCmp.EqualsAscii( pMarkToText ) )
                {
                    aName.Erase();
                }
            }
        }
    }

    // einen neuen Kontext anlegen
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( HTML_ANCHOR_ON );

    BOOL bEnAnchor = FALSE, bFtnAnchor = FALSE, bFtnEnSymbol = FALSE;
    String aFtnName;
    String aStrippedClass( aClass );
    SwCSS1Parser::GetScriptFromClass( aStrippedClass, sal_False );
    if( aStrippedClass.Len() >=9  && bHasHRef && sHRef.Len() > 1 &&
        ('s' == aStrippedClass.GetChar(0) || 'S' == aStrippedClass.GetChar(0)) &&
        ('d' == aStrippedClass.GetChar(1) || 'D' == aStrippedClass.GetChar(1)) )
    {
        if( aStrippedClass.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_sdendnote_anc ) )
            bEnAnchor = TRUE;
        else if( aStrippedClass.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote_anc ) )
            bFtnAnchor = TRUE;
        else if( aStrippedClass.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_sdendnote_sym ) ||
                 aStrippedClass.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_sdfootnote_sym ) )
            bFtnEnSymbol = TRUE;
        if( bEnAnchor || bFtnAnchor || bFtnEnSymbol )
        {
            aFtnName = sHRef.Copy( 1 );
            aClass = aStrippedClass = aName = aEmptyStr;
            bHasHRef = FALSE;
        }
    }

    // Styles parsen
    if( HasStyleOptions( aStyle, aId, aStrippedClass, &aLang, &aDir ) )
    {
        SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
        SvxCSS1PropertyInfo aPropInfo;

        if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
        {
            DoPositioning( aItemSet, aPropInfo, pCntxt );
            InsertAttrs( aItemSet, aPropInfo, pCntxt, TRUE );
        }
    }

    if( bHasHRef )
    {
        if( sHRef.Len() )
        {
            sHRef = URIHelper::SmartRel2Abs( INetURLObject(sBaseURL), sHRef, Link(), false );
        }
        else
        {
            // Bei leerer URL das Directory nehmen
            INetURLObject aURLObj( aPathToFile );
            sHRef = aURLObj.GetPartBeforeLastName();
        }

        pCSS1Parser->SetATagStyles();
        SwFmtINetFmt aINetFmt( sHRef, sTarget );
        aINetFmt.SetName( aName );

        if( aMacroTbl.Count() )
            aINetFmt.SetMacroTbl( &aMacroTbl );

        // das Default-Attribut setzen
        InsertAttr( &aAttrTab.pINetFmt, aINetFmt, pCntxt );
    }
    else if( aName.Len() )
    {
        InsertBookmark( aName );
    }

    if( bEnAnchor || bFtnAnchor )
    {
        InsertFootEndNote( aFtnName, bEnAnchor, bFixed );
        bInFootEndNoteAnchor = bCallNextToken = TRUE;
    }
    else if( bFtnEnSymbol )
    {
        bInFootEndNoteSymbol = bCallNextToken = TRUE;
    }

    // den Kontext merken
    PushContext( pCntxt );
}

void SwHTMLParser::EndAnchor()
{
    if( bInFootEndNoteAnchor )
    {
        FinishFootEndNote();
        bInFootEndNoteAnchor = FALSE;
    }
    else if( bInFootEndNoteSymbol )
    {
        bInFootEndNoteSymbol = FALSE;
    }

    EndTag( HTML_ANCHOR_OFF );
}

/*  */

void SwHTMLParser::InsertBookmark( const String& rName )
{
    _HTMLAttr* pTmp = new _HTMLAttr( *pPam->GetPoint(),
            SfxStringItem( RES_FLTR_BOOKMARK, rName ));
    aSetAttrTab.Insert( pTmp, aSetAttrTab.Count() );
}

BOOL SwHTMLParser::HasCurrentParaBookmarks( BOOL bIgnoreStack ) const
{
    BOOL bHasMarks = FALSE;
    ULONG nNodeIdx = pPam->GetPoint()->nNode.GetIndex();

    // first step: are there still bookmark in the attribute-stack?
    // bookmarks are added to the end of the stack - thus we only have
    // to check the last bookmark
    if( !bIgnoreStack )
    {
        _HTMLAttr* pAttr;
        for( USHORT i = aSetAttrTab.Count(); i; )
        {
            pAttr = aSetAttrTab[ --i ];
            if( RES_FLTR_BOOKMARK == pAttr->pItem->Which() )
            {
                if( pAttr->GetSttParaIdx() == nNodeIdx )
                    bHasMarks = TRUE;
                break;
            }
        }
    }

    if( !bHasMarks )
    {
        // second step: when we didnt find a bookmark, check if there is one
        // set already
        IDocumentMarkAccess* const pMarkAccess = pDoc->getIDocumentMarkAccess();
        for(IDocumentMarkAccess::const_iterator_t ppMark = pMarkAccess->getMarksBegin();
            ppMark != pMarkAccess->getMarksEnd();
            ppMark++)
        {
            const ::sw::mark::IMark* pBookmark = ppMark->get();
            ULONG nBookNdIdx = pBookmark->GetMarkPos().nNode.GetIndex();
            if( nBookNdIdx==nNodeIdx )
            {
                bHasMarks = TRUE;
                break;
            }
            else if( nBookNdIdx > nNodeIdx )
                break;
        }
    }

    return bHasMarks;
}

/*  */

void SwHTMLParser::StripTrailingPara()
{
    BOOL bSetSmallFont = FALSE;

    SwCntntNode* pCNd = pPam->GetCntntNode();
    if( !pPam->GetPoint()->nContent.GetIndex() )
    {
        if( pCNd && pCNd->StartOfSectionIndex()+2 <
            pCNd->EndOfSectionIndex() )
        {
            ULONG nNodeIdx = pPam->GetPoint()->nNode.GetIndex();

            const SwSpzFrmFmts& rFrmFmtTbl = *pDoc->GetSpzFrmFmts();

            for( USHORT i=0; i<rFrmFmtTbl.Count(); i++ )
            {
                SwFrmFmt const*const pFmt = rFrmFmtTbl[i];
                SwFmtAnchor const*const pAnchor = &pFmt->GetAnchor();
                SwPosition const*const pAPos = pAnchor->GetCntntAnchor();
                if (pAPos &&
                    ((FLY_AT_PARA == pAnchor->GetAnchorId()) ||
                     (FLY_AT_CHAR == pAnchor->GetAnchorId())) &&
                    pAPos->nNode == nNodeIdx )

                    return;     // den Knoten duerfen wir nicht loeschen
            }

            SetAttr( FALSE );   // die noch offenen Attribute muessen
                                // beendet werden, bevor der Node
                                // geloescht wird, weil sonst der
                                // End-Index in die Botanik zeigt

            if( pCNd->Len() && pCNd->IsTxtNode() )
            {
                // es wurden Felder in den Node eingefuegt, die muessen
                // wir jetzt verschieben
                SwTxtNode *pPrvNd = pDoc->GetNodes()[nNodeIdx-1]->GetTxtNode();
                if( pPrvNd )
                {
                    SwIndex aSrc( pCNd, 0 );
                    pCNd->GetTxtNode()->CutText( pPrvNd, aSrc, pCNd->Len() );
                }
            }

            // jetz muessen wir noch eventuell vorhandene Bookmarks
            // verschieben
            IDocumentMarkAccess* const pMarkAccess = pDoc->getIDocumentMarkAccess();
            for(IDocumentMarkAccess::const_iterator_t ppMark = pMarkAccess->getMarksBegin();
                ppMark != pMarkAccess->getMarksEnd();
                ppMark++)
            {
                ::sw::mark::IMark* pMark = ppMark->get();
                ULONG nBookNdIdx = pMark->GetMarkPos().nNode.GetIndex();
                if(nBookNdIdx==nNodeIdx)
                {
                    SwNodeIndex nNewNdIdx(pPam->GetPoint()->nNode);
                    SwCntntNode* pNd = pDoc->GetNodes().GoPrevious(&nNewNdIdx);
                    if(!pNd)
                    {
                        ASSERT(!this, "Hoppla, wo ist mein Vorgaenger-Node");
                        return;
                    }
                    // --> OD 2007-09-27 #i81002# - refactoring
                    // Do not directly manipulate member of <SwBookmark>
                    {
                        SwPosition aNewPos(*pNd);
                        aNewPos.nContent.Assign(pNd, pNd->Len());
                        const SwPaM aPaM(aNewPos);
                        pMarkAccess->repositionMark(ppMark->get(), aPaM);
                    }
                    // <--
                }
                else if( nBookNdIdx > nNodeIdx )
                    break;
            }

            pPam->GetPoint()->nContent.Assign( 0, 0 );
            pPam->SetMark();
            pPam->DeleteMark();
            pDoc->GetNodes().Delete( pPam->GetPoint()->nNode );
            pPam->Move( fnMoveBackward, fnGoNode );
        }
        else if( pCNd && pCNd->IsTxtNode() && pTable )
        {
            // In leeren Zellen stellen wir einen kleinen Font ein, damit die
            // Zelle nicht hoeher wird als die Grafik bzw. so niedrig wie
            // moeglich bleibt.
            bSetSmallFont = TRUE;
        }
    }
    else if( pCNd && pCNd->IsTxtNode() && pTable &&
             pCNd->StartOfSectionIndex()+2 ==
             pCNd->EndOfSectionIndex() )
    {
        // Wenn die Zelle nur zeichengebundene Grafiken/Rahmen enthaelt
        // stellen wir ebenfalls einen kleinen Font ein.
        bSetSmallFont = TRUE;
        SwTxtNode* pTxtNd = pCNd->GetTxtNode();

        xub_StrLen nPos = pPam->GetPoint()->nContent.GetIndex();
        while( bSetSmallFont && nPos>0 )
        {
            --nPos;
            bSetSmallFont =
                (CH_TXTATR_BREAKWORD == pTxtNd->GetTxt().GetChar( nPos )) &&
                (0 != pTxtNd->GetTxtAttrForCharAt( nPos, RES_TXTATR_FLYCNT ));
        }
    }

    if( bSetSmallFont )
    {
        SvxFontHeightItem aFontHeight( 40, 100, RES_CHRATR_FONTSIZE );
        pCNd->SetAttr( aFontHeight );
        aFontHeight.SetWhich( RES_CHRATR_CJK_FONTSIZE );
        pCNd->SetAttr( aFontHeight );
        aFontHeight.SetWhich( RES_CHRATR_CTL_FONTSIZE );
        pCNd->SetAttr( aFontHeight );
    }
}

