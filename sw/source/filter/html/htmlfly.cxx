/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlfly.cxx,v $
 * $Revision: 1.28 $
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

#include <com/sun/star/text/HoriOrientation.hpp>
#include <com/sun/star/text/VertOrientation.hpp>
#include <com/sun/star/text/RelOrientation.hpp>
#include <svx/svxids.hrc>
#include "hintids.hxx"
#include <tools/string.hxx>
#include <svl/urihelper.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <svtools/htmlkywd.hxx>
#include <svtools/htmlout.hxx>
#include <svtools/imap.hxx>
#include <svtools/imapobj.hxx>
#include <svtools/htmlcfg.hxx>
#include <svx/xoutbmp.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/brshitem.hxx>


#include <fmtanchr.hxx>
#include <fmtornt.hxx>
#include <fmturl.hxx>
#include <fmtfsize.hxx>
#include <fmtclds.hxx>
#include <fmtcntnt.hxx>
#include <fmtsrnd.hxx>
#include <fmtinfmt.hxx>
#include <txtinet.hxx>
#include "frmatr.hxx"
#include <grfatr.hxx>
#include <flypos.hxx>
#include <docary.hxx>
#include <ndgrf.hxx>

#include "doc.hxx"
#include "ndtxt.hxx"
#include "pam.hxx"
#include "swerror.h"
#include "frmfmt.hxx"
#include "wrthtml.hxx"
#include "css1kywd.hxx"
#include "htmlfly.hxx"

using namespace ::com::sun::star;

////////////////////////////////////////////////////////////

const ULONG HTML_FRMOPTS_IMG_ALL		=
    HTML_FRMOPT_ALT |
    HTML_FRMOPT_SIZE |
    HTML_FRMOPT_ANYSIZE |
    HTML_FRMOPT_BORDER |
    HTML_FRMOPT_NAME;
const ULONG HTML_FRMOPTS_IMG_CNTNR		=
    HTML_FRMOPTS_IMG_ALL |
    HTML_FRMOPT_ABSSIZE;
const ULONG HTML_FRMOPTS_IMG			=
    HTML_FRMOPTS_IMG_ALL |
    HTML_FRMOPT_ALIGN |
    HTML_FRMOPT_SPACE |
    HTML_FRMOPT_BRCLEAR;
const ULONG HTML_FRMOPTS_IMG_CSS1		=
    HTML_FRMOPT_S_ALIGN |
    HTML_FRMOPT_S_SPACE;

const ULONG HTML_FRMOPTS_DIV 			=
    HTML_FRMOPT_ID |
    HTML_FRMOPT_S_ALIGN |
    HTML_FRMOPT_S_SIZE |
    HTML_FRMOPT_ANYSIZE |
    HTML_FRMOPT_ABSSIZE |
    HTML_FRMOPT_S_SPACE |
    HTML_FRMOPT_S_BORDER |
    HTML_FRMOPT_S_BACKGROUND |
    HTML_FRMOPT_BRCLEAR |
    HTML_FRMOPT_DIR;

const ULONG HTML_FRMOPTS_MULTICOL		=
    HTML_FRMOPT_ID |
    HTML_FRMOPT_WIDTH |
    HTML_FRMOPT_ANYSIZE |
    HTML_FRMOPT_ABSSIZE |
    HTML_FRMOPT_DIR;
const ULONG HTML_FRMOPTS_MULTICOL_CNTNR	=
    HTML_FRMOPTS_MULTICOL;
const ULONG HTML_FRMOPTS_MULTICOL_CSS1	=
    HTML_FRMOPT_S_ALIGN |
    HTML_FRMOPT_S_SIZE |
    HTML_FRMOPT_S_SPACE |
    HTML_FRMOPT_S_BORDER|
    HTML_FRMOPT_S_BACKGROUND;

const ULONG HTML_FRMOPTS_SPACER			=
    HTML_FRMOPT_ALIGN |
    HTML_FRMOPT_SIZE |
    HTML_FRMOPT_ANYSIZE |
    HTML_FRMOPT_BRCLEAR |
    HTML_FRMOPT_MARGINSIZE |
    HTML_FRMOPT_ABSSIZE;

const ULONG HTML_FRMOPTS_CNTNR			=
    HTML_FRMOPT_S_ALIGN |
    HTML_FRMOPT_S_SPACE |
    HTML_FRMOPT_S_WIDTH |
    HTML_FRMOPT_ANYSIZE |
    HTML_FRMOPT_ABSSIZE |
    HTML_FRMOPT_S_PIXSIZE;


static Writer& OutHTML_FrmFmtTableNode( Writer& rWrt, const SwFrmFmt& rFrmFmt );
static Writer& OutHTML_FrmFmtAsMulticol( Writer& rWrt, const SwFrmFmt& rFmt,
                                         BOOL bInCntnr );
static Writer& OutHTML_FrmFmtAsSpacer( Writer& rWrt, const SwFrmFmt& rFmt );
static Writer& OutHTML_FrmFmtAsDivOrSpan( Writer& rWrt,
                                          const SwFrmFmt& rFrmFmt, BOOL bSpan );
static Writer& OutHTML_FrmFmtAsImage( Writer& rWrt, const SwFrmFmt& rFmt,
                                      BOOL bInCntnr	);

static Writer& OutHTML_FrmFmtGrfNode( Writer& rWrt, const SwFrmFmt& rFmt,
                                      BOOL bInCntnr	);

static Writer& OutHTML_FrmFmtAsMarquee( Writer& rWrt, const SwFrmFmt& rFrmFmt,
                                        const SdrObject& rSdrObj	);
//-----------------------------------------------------------------------

extern HTMLOutEvent __FAR_DATA aAnchorEventTable[];

static HTMLOutEvent __FAR_DATA aImageEventTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_O_SDonload,			OOO_STRING_SVTOOLS_HTML_O_onload,		SVX_EVENT_IMAGE_LOAD		},
    { OOO_STRING_SVTOOLS_HTML_O_SDonabort,		OOO_STRING_SVTOOLS_HTML_O_onabort,	SVX_EVENT_IMAGE_ABORT		},
    { OOO_STRING_SVTOOLS_HTML_O_SDonerror,		OOO_STRING_SVTOOLS_HTML_O_onerror,	SVX_EVENT_IMAGE_ERROR		},
    { 0,						0,					0						}
};

static HTMLOutEvent __FAR_DATA aIMapEventTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_O_SDonmouseover,	OOO_STRING_SVTOOLS_HTML_O_onmouseover,	SFX_EVENT_MOUSEOVER_OBJECT	},
    { OOO_STRING_SVTOOLS_HTML_O_SDonmouseout,		OOO_STRING_SVTOOLS_HTML_O_onmouseout,		SFX_EVENT_MOUSEOUT_OBJECT	},
    { 0,						0,					  	0							}
};



SV_IMPL_OP_PTRARR_SORT( SwHTMLPosFlyFrms, SwHTMLPosFlyFrmPtr )

USHORT SwHTMLWriter::GuessFrmType( const SwFrmFmt& rFrmFmt,
                                   const SdrObject*& rpSdrObj )
{
    SwHTMLFrmType eType;

    if( RES_DRAWFRMFMT == rFrmFmt.Which() )
    {
        // Als Default irgendein Zeichen-Objekt
        eType = HTML_FRMTYPE_DRAW;

        const SdrObject *pObj =
            SwHTMLWriter::GetMarqueeTextObj( (const SwDrawFrmFmt &)rFrmFmt );
        if( pObj )
        {
            // Laufschrift
            rpSdrObj = pObj;
            eType = HTML_FRMTYPE_MARQUEE;
        }
        else
        {
            pObj = GetHTMLControl( (const SwDrawFrmFmt &)rFrmFmt );

            if( pObj )
            {
                // Form-Control
                rpSdrObj = pObj;
                eType = HTML_FRMTYPE_CONTROL;
            }
        }
    }
    else
    {
        // Als Default ein Textrahmen
        eType = HTML_FRMTYPE_TEXT;

        const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
        ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex()+1;
        const SwNode* pNd = pDoc->GetNodes()[ nStt ];

        if( pNd->IsGrfNode() )
        {
            // Grafik - Node
            eType = HTML_FRMTYPE_GRF;
        }
        else if( pNd->IsOLENode() )
        {
            // Applet, Plugin, Floating-Frame
            eType = (SwHTMLFrmType)GuessOLENodeFrmType( *pNd );
        }
        else
        {
            ULONG nEnd = pDoc->GetNodes()[nStt-1]->EndOfSectionIndex();

            const SfxPoolItem* pItem;
            const SfxItemSet& rItemSet = rFrmFmt.GetAttrSet();
            if( SFX_ITEM_SET == rItemSet.GetItemState( RES_COL,
                                                       TRUE, &pItem ) &&
                ((const SwFmtCol *)pItem)->GetNumCols() > 1 )
            {
                // spaltiger Rahmen
                eType = HTML_FRMTYPE_MULTICOL;
            }
            else if( pNd->IsTableNode() )
            {
                const SwTableNode *pTblNd = pNd->GetTableNode();
                ULONG nTblEnd = pTblNd->EndOfSectionIndex();

                if( nTblEnd+1 == nEnd )
                {
                    // Tabelle
                    eType = HTML_FRMTYPE_TABLE;
                }
                else if( nTblEnd+2 == nEnd )
                {
                    // Tabelle mit Unterschrft
                    eType = HTML_FRMTYPE_TABLE_CAP;
                }
            }
            else if( pNd->IsTxtNode() )
            {
                const SwTxtNode *pTxtNd = pNd->GetTxtNode();

                BOOL bEmpty = FALSE;
                if( nStt==nEnd-1 && !pTxtNd->Len() )
                {
                    // leerer Rahmen? Nur wenn kein Rahmen am
                    // Text- oder Start-Node verankert ist.
                    bEmpty = TRUE;
                    if( pHTMLPosFlyFrms )
                    {
                        for( USHORT i=0; i<pHTMLPosFlyFrms->Count(); i++ )
                        {
                            ULONG nIdx = (*pHTMLPosFlyFrms)[i]
                                                ->GetNdIndex().GetIndex();
                            bEmpty = (nIdx != nStt) && (nIdx != nStt-1);
                            if( !bEmpty || nIdx > nStt )
                                break;
                        }
                    }
                }
                if( bEmpty )
                {
                    const SvxBrushItem& rBrush = rFrmFmt.GetBackground();
                    /// OD 02.09.2002 #99657#
                    /// background is not empty, if it has a background graphic
                    /// or its background color is not "no fill"/"auto fill".
                    if( GPOS_NONE != rBrush.GetGraphicPos() ||
                        rBrush.GetColor() != COL_TRANSPARENT )
                        bEmpty = FALSE;
                }
                if( bEmpty )
                {
                    // leerer Rahmen
                    eType = HTML_FRMTYPE_EMPTY;
                }
                else if( pDoc->GetNodes()[nStt+1]->IsTableNode() )
                {
                    const SwTableNode *pTblNd =
                        pDoc->GetNodes()[nStt+1]->GetTableNode();
                    if( pTblNd->EndOfSectionIndex()+1 == nEnd )
                    {
                        // Tabelle mit Ueberschrift
                        eType = HTML_FRMTYPE_TABLE_CAP;
                    }
                }
            }
        }
    }

    return static_cast< USHORT >(eType);
}

void SwHTMLWriter::CollectFlyFrms()
{
    ASSERT( HTML_CFG_MAX+1 == MAX_BROWSERS,
            "number of browser configurations has changed" );

    BYTE nSz = (BYTE)Min( pDoc->GetSpzFrmFmts()->Count(), USHORT(255) );
    SwPosFlyFrms aFlyPos( nSz, nSz );
    pDoc->GetAllFlyFmts( aFlyPos, bWriteAll ? 0 : pCurPam, TRUE );

    for( USHORT i=0; i< aFlyPos.Count(); i++ )
    {
        const SwFrmFmt& rFrmFmt = aFlyPos[i]->GetFmt();
        const SdrObject *pSdrObj = 0;
        const SwPosition *pAPos;
        const SwCntntNode *pACNd;
        SwHTMLFrmType eType = (SwHTMLFrmType)GuessFrmType( rFrmFmt, pSdrObj );

        BYTE nMode;
        const SwFmtAnchor& rAnchor = rFrmFmt.GetAnchor();
        sal_Int16 eHoriRel = rFrmFmt.GetHoriOrient().GetRelationOrient();
        switch( rAnchor.GetAnchorId() )
        {
        case FLY_PAGE:
        case FLY_AT_FLY:
            nMode = aHTMLOutFrmPageFlyTable[eType][nExportMode];
            break;

        case FLY_AT_CNTNT:
            // Absatz-gebundene Rahmen werden nur dann vor den
            // Absatz geschrieben, wenn der Absatz einen Abstand
            // hat.
            if( text::RelOrientation::FRAME == eHoriRel &&
                (pAPos = rAnchor.GetCntntAnchor()) != 0 &&
                (pACNd = pAPos->nNode.GetNode().GetCntntNode()) != 0 )
            {
                const SvxLRSpaceItem& rLRItem =
                    (const SvxLRSpaceItem&)pACNd->GetAttr(RES_LR_SPACE);
                if( rLRItem.GetTxtLeft() || rLRItem.GetRight() )
                {
                    nMode = aHTMLOutFrmParaFrameTable[eType][nExportMode];
                    break;
                }
            }
            nMode = aHTMLOutFrmParaPrtAreaTable[eType][nExportMode];
            break;

        case FLY_AUTO_CNTNT:
            if( text::RelOrientation::FRAME == eHoriRel || text::RelOrientation::PRINT_AREA == eHoriRel )
                nMode = aHTMLOutFrmParaPrtAreaTable[eType][nExportMode];
            else
                nMode = aHTMLOutFrmParaOtherTable[eType][nExportMode];
            break;

        default:
            nMode = aHTMLOutFrmParaPrtAreaTable[eType][nExportMode];
            break;
        }

        if( !pHTMLPosFlyFrms )
            pHTMLPosFlyFrms = new SwHTMLPosFlyFrms;

        SwHTMLPosFlyFrm *pNew =
            new SwHTMLPosFlyFrm( *aFlyPos[i], pSdrObj, nMode );
        pHTMLPosFlyFrms->Insert( pNew );
    }
}

BOOL SwHTMLWriter::OutFlyFrm( ULONG nNdIdx, xub_StrLen nCntntIdx, BYTE nPos,
                              HTMLOutContext *pContext )
{
    BOOL bFlysLeft = FALSE;	// Noch Flys an aktueller Node-Position da?

    // OutFlyFrm kan rekursiv aufgerufen werden. Deshalb muss man
    // manchmal wieder von vorne anfangen, nachdem ein Fly ausgegeben
    // wurde.
    BOOL bRestart = TRUE;
    while( pHTMLPosFlyFrms && bRestart )
    {
        bFlysLeft = bRestart = FALSE;

        // suche nach dem Anfang der FlyFrames
        USHORT i;

        for( i = 0; i < pHTMLPosFlyFrms->Count() &&
            (*pHTMLPosFlyFrms)[i]->GetNdIndex().GetIndex() < nNdIdx; i++ )
            ;
        for( ; !bRestart && i < pHTMLPosFlyFrms->Count() &&
            (*pHTMLPosFlyFrms)[i]->GetNdIndex().GetIndex() == nNdIdx; i++ )
        {
            SwHTMLPosFlyFrm *pPosFly = (*pHTMLPosFlyFrms)[i];
            if( ( HTML_POS_ANY == nPos ||
                  pPosFly->GetOutPos() == nPos ) &&
                pPosFly->GetCntntIndex() == nCntntIdx )
            {
                // Erst entfernen ist wichtig, weil in tieferen
                // Rekursionen evtl. weitere Eintraege oder das
                // ganze Array geloscht werden koennte.
                pHTMLPosFlyFrms->Remove( i, 1 );
                i--;
                if( !pHTMLPosFlyFrms->Count() )
                {
                    delete pHTMLPosFlyFrms;
                    pHTMLPosFlyFrms = 0;
                    bRestart = TRUE; 	// nicht wirklich, nur raus
                                        // aus der Schleife
                }

                if( pContext )
                {
                    HTMLOutFuncs::FlushToAscii(Strm(), *pContext );
                    pContext = 0; // one time only
                }

                OutFrmFmt( pPosFly->GetOutMode(), pPosFly->GetFmt(),
                           pPosFly->GetSdrObject() );
                switch( pPosFly->GetOutFn() )
                {
                case HTML_OUT_DIV:
                case HTML_OUT_SPAN:
                case HTML_OUT_MULTICOL:
                case HTML_OUT_TBLNODE:
                    bRestart = TRUE; // Hier wird's evtl rekursiv
                    break;
                }
                delete pPosFly;
            }
            else
            {
                bFlysLeft = TRUE;
            }
        }
    }

    return bFlysLeft;
}

void SwHTMLWriter::OutFrmFmt( BYTE nMode, const SwFrmFmt& rFrmFmt,
                              const SdrObject *pSdrObject )
{
    BYTE nCntnrMode = SwHTMLPosFlyFrm::GetOutCntnr( nMode );
    BYTE nOutMode = SwHTMLPosFlyFrm::GetOutFn(nMode);
    const sal_Char *pCntnrStr = 0;
    if( HTML_CNTNR_NONE != nCntnrMode )
    {

        if( bLFPossible && HTML_CNTNR_DIV == nCntnrMode )
            OutNewLine();

        ByteString sOut( '<' );
        pCntnrStr = (HTML_CNTNR_DIV == nCntnrMode)
                            ? OOO_STRING_SVTOOLS_HTML_division
                            : OOO_STRING_SVTOOLS_HTML_span;
        sOut += pCntnrStr;
        ((((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_class) += "=\"")
            += sCSS1_class_abs_pos) += '\"';
        Strm() << sOut.GetBuffer();

        // Fuer Nicht-Zeichenobekte eine Breite ausgeben
        ULONG nFrmFlags = HTML_FRMOPTS_CNTNR;

        // Fuer spaltige Rahmen koennen wir auch noch den Hintergrund ausgeben.
        if( HTML_OUT_MULTICOL == nOutMode )
            nFrmFlags |= HTML_FRMOPT_S_BACKGROUND|HTML_FRMOPT_S_BORDER;

        if( IsHTMLMode( HTMLMODE_BORDER_NONE ) )
           nFrmFlags |= HTML_FRMOPT_S_NOBORDER;
        OutCSS1_FrmFmtOptions( rFrmFmt, nFrmFlags, pSdrObject );
        Strm() << '>';

        if( HTML_CNTNR_DIV == nCntnrMode )
        {
            IncIndentLevel();
            bLFPossible = TRUE;
        }
    }

    switch( nOutMode )
    {
    case HTML_OUT_TBLNODE:		// OK
        ASSERT( !pCntnrStr, "Table: Container ist hier nicht vorgesehen" );
        OutHTML_FrmFmtTableNode( *this, rFrmFmt );
        break;
    case HTML_OUT_GRFNODE:		// OK
        OutHTML_FrmFmtGrfNode( *this, rFrmFmt, pCntnrStr != 0 );
        break;
    case HTML_OUT_OLENODE:		// OK
        OutHTML_FrmFmtOLENode( *this, rFrmFmt, pCntnrStr != 0 );
        break;
    case HTML_OUT_OLEGRF:		// OK
        OutHTML_FrmFmtOLENodeGrf( *this, rFrmFmt, pCntnrStr != 0 );
        break;
    case HTML_OUT_DIV:
    case HTML_OUT_SPAN:
        ASSERT( !pCntnrStr, "Div: Container ist hier nicht vorgesehen" );
        OutHTML_FrmFmtAsDivOrSpan( *this, rFrmFmt, HTML_OUT_SPAN==nOutMode );
        break;
    case HTML_OUT_MULTICOL:		// OK
        OutHTML_FrmFmtAsMulticol( *this, rFrmFmt, pCntnrStr != 0 );
        break;
    case HTML_OUT_SPACER:		// OK
        ASSERT( !pCntnrStr, "Spacer: Container ist hier nicht vorgesehen" );
        OutHTML_FrmFmtAsSpacer( *this, rFrmFmt );
        break;
    case HTML_OUT_CONTROL:		// OK
        OutHTML_DrawFrmFmtAsControl( *this,
                                    (const SwDrawFrmFmt &)rFrmFmt, *pSdrObject,
                                    pCntnrStr != 0 );
        break;
    case HTML_OUT_AMARQUEE:
        OutHTML_FrmFmtAsMarquee( *this, rFrmFmt, *pSdrObject );
        break;
    case HTML_OUT_MARQUEE:
        ASSERT( !pCntnrStr, "Marquee: Container ist hier nicht vorgesehen" );
        OutHTML_DrawFrmFmtAsMarquee( *this,
                    (const SwDrawFrmFmt &)rFrmFmt, *pSdrObject );
        break;
    case HTML_OUT_GRFFRM:
        OutHTML_FrmFmtAsImage( *this, rFrmFmt, pCntnrStr != 0 );
        break;
    }

    if( HTML_CNTNR_DIV == nCntnrMode )
    {
        DecIndentLevel();
        if( bLFPossible )
            OutNewLine();
        HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_division, FALSE );
        bLFPossible = TRUE;
    }
    else if( HTML_CNTNR_SPAN == nCntnrMode )
        HTMLOutFuncs::Out_AsciiTag( Strm(), OOO_STRING_SVTOOLS_HTML_span, FALSE );
}


void SwHTMLWriter::OutFrmFmtOptions( const SwFrmFmt &rFrmFmt,
                                     const String& rAlternateTxt,
                                     ByteString &rEndTags,
                                     sal_uInt32 nFrmOpts )
{
    ByteString sOut;
    const SfxPoolItem* pItem;
    const SfxItemSet& rItemSet = rFrmFmt.GetAttrSet();

    // Name
    if( (nFrmOpts & (HTML_FRMOPT_ID|HTML_FRMOPT_NAME)) &&
        rFrmFmt.GetName().Len() )
    {
        const sal_Char *pStr =
            (nFrmOpts & HTML_FRMOPT_ID) ? OOO_STRING_SVTOOLS_HTML_O_id : OOO_STRING_SVTOOLS_HTML_O_name;
        ((sOut += ' ') += pStr) += "=\"";
        Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( Strm(), rFrmFmt.GetName(), eDestEnc, &aNonConvertableCharacters );
        sOut = '\"';
    }

    // Name
    if( nFrmOpts & HTML_FRMOPT_DIR )
    {
        sal_uInt16 nDir = GetHTMLDirection( rItemSet );
        Strm() << sOut.GetBuffer();
        sOut.Erase();
        OutDirection( nDir );
    }


    // ALT
    if( (nFrmOpts & HTML_FRMOPT_ALT) && rAlternateTxt.Len() )
    {
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_alt) += "=\"";
        Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( Strm(), rAlternateTxt, eDestEnc, &aNonConvertableCharacters );
        sOut = '\"';
    }

    // ALIGN
    const sal_Char *pStr = 0;
    RndStdIds eAnchorId = rFrmFmt.GetAnchor().GetAnchorId();
    if( (nFrmOpts & HTML_FRMOPT_ALIGN) &&
        (FLY_AT_CNTNT == eAnchorId || FLY_AUTO_CNTNT == eAnchorId) )
    {
        // MIB 12.3.98: Ist es nicht schlauer, absatzgebundene
        // Rahmen notfalls links auszurichten als sie
        // zeichengebunden einzufuegen???
        const SwFmtHoriOrient& rHoriOri = rFrmFmt.GetHoriOrient();
        if( !(nFrmOpts & HTML_FRMOPT_S_ALIGN) ||
            text::RelOrientation::FRAME == rHoriOri.GetRelationOrient() ||
            text::RelOrientation::PRINT_AREA == rHoriOri.GetRelationOrient() )
        {
            pStr = text::HoriOrientation::RIGHT == rHoriOri.GetHoriOrient()
                        ? OOO_STRING_SVTOOLS_HTML_AL_right
                        : OOO_STRING_SVTOOLS_HTML_AL_left;
        }
    }
    if( (nFrmOpts & HTML_FRMOPT_ALIGN) && !pStr &&
        ( (nFrmOpts & HTML_FRMOPT_S_ALIGN) == 0 ||
          FLY_IN_CNTNT == eAnchorId ) &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_VERT_ORIENT, TRUE, &pItem ))
    {
        switch( ((SwFmtVertOrient*)pItem)->GetVertOrient() )
        {
        case text::VertOrientation::LINE_TOP:     pStr = OOO_STRING_SVTOOLS_HTML_VA_top;        break;
        case text::VertOrientation::CHAR_TOP:
        case text::VertOrientation::BOTTOM:       pStr = OOO_STRING_SVTOOLS_HTML_VA_texttop;    break;  // geht nicht
        case text::VertOrientation::LINE_CENTER:
        case text::VertOrientation::CHAR_CENTER:  pStr = OOO_STRING_SVTOOLS_HTML_VA_absmiddle;  break;  // geht nicht
        case text::VertOrientation::CENTER:       pStr = OOO_STRING_SVTOOLS_HTML_VA_middle;     break;
        case text::VertOrientation::LINE_BOTTOM:
        case text::VertOrientation::CHAR_BOTTOM:  pStr = OOO_STRING_SVTOOLS_HTML_VA_absbottom;  break;  // geht nicht
        case text::VertOrientation::TOP:          pStr = OOO_STRING_SVTOOLS_HTML_VA_bottom;     break;
        case text::VertOrientation::NONE:     break;
        }
    }
    if( pStr )
        (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_align) += '=') += pStr;


    // HSPACE und VSPACE
    Size aTwipSpc( 0, 0 );
    if( (nFrmOpts & (HTML_FRMOPT_SPACE|HTML_FRMOPT_MARGINSIZE)) &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_LR_SPACE, TRUE, &pItem ))
    {
        aTwipSpc.Width() =
            ( ((SvxLRSpaceItem*)pItem)->GetLeft() +
                ((SvxLRSpaceItem*)pItem)->GetRight() ) / 2;
        nDfltLeftMargin = nDfltRightMargin = aTwipSpc.Width();
    }
    if( (nFrmOpts & (HTML_FRMOPT_SPACE|HTML_FRMOPT_MARGINSIZE)) &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_UL_SPACE, TRUE, &pItem ))
    {
        aTwipSpc.Height()  =
            ( ((SvxULSpaceItem*)pItem)->GetUpper() +
                ((SvxULSpaceItem*)pItem)->GetLower() ) / 2;
        nDfltTopMargin = nDfltBottomMargin = (USHORT)aTwipSpc.Height();
    }

    if( (nFrmOpts & HTML_FRMOPT_SPACE) &&
        (aTwipSpc.Width() || aTwipSpc.Height()) &&
        Application::GetDefaultDevice() )
    {
        Size aPixelSpc =
            Application::GetDefaultDevice()->LogicToPixel( aTwipSpc,
                                                MapMode(MAP_TWIP) );
        if( !aPixelSpc.Width() && aTwipSpc.Width() )
            aPixelSpc.Width() = 1;
        if( !aPixelSpc.Height() && aTwipSpc.Height() )
            aPixelSpc.Height() = 1;

        if( aPixelSpc.Width() )
        {
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_hspace) += '=')
                += ByteString::CreateFromInt32( aPixelSpc.Width() );
        }

        if( aPixelSpc.Height() )
        {
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_vspace) += '=')
                += ByteString::CreateFromInt32( aPixelSpc.Height() );
        }
    }

    // Der Abstand muss bei der Groesse beruecksichtigt, wenn das entsprechende
    // Flag gesetzt ist.
    if( (nFrmOpts & HTML_FRMOPT_MARGINSIZE) )
    {
        aTwipSpc.Width() *= -2;
        aTwipSpc.Height() *= -2;
    }
    else
    {
        aTwipSpc.Width() = 0;
        aTwipSpc.Height() = 0;
    }

    if( !(nFrmOpts & HTML_FRMOPT_ABSSIZE) &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_BOX, TRUE, &pItem ))
    {
        const SvxBoxItem* pBoxItem = (const SvxBoxItem*)pItem;

        aTwipSpc.Width() += pBoxItem->CalcLineSpace( BOX_LINE_LEFT );
        aTwipSpc.Width() += pBoxItem->CalcLineSpace( BOX_LINE_RIGHT );
        aTwipSpc.Height() += pBoxItem->CalcLineSpace( BOX_LINE_TOP );
        aTwipSpc.Height() += pBoxItem->CalcLineSpace( BOX_LINE_BOTTOM );
    }

    // WIDTH und/oder HEIGHT
    // ATT_VAR_SIZE/ATT_MIN_SIZE nur ausgeben, wenn ANYSIZE gesezut ist
    if( (nFrmOpts & HTML_FRMOPT_SIZE) &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_FRM_SIZE, TRUE, &pItem ) &&
        ( (nFrmOpts & HTML_FRMOPT_ANYSIZE) ||
          ATT_FIX_SIZE == ((const SwFmtFrmSize *)pItem)->GetHeightSizeType()) )
    {
        const SwFmtFrmSize *pFSItem = (const SwFmtFrmSize *)pItem;
        BYTE nPrcWidth = pFSItem->GetWidthPercent();
        BYTE nPrcHeight = pFSItem->GetHeightPercent();

        // Groesse des Objekts Twips ohne Raender
        Size aTwipSz( (nPrcWidth ? 0
                                 : pFSItem->GetWidth()-aTwipSpc.Width()),
                      (nPrcHeight ? 0
                                  : pFSItem->GetHeight()-aTwipSpc.Height()) );

        ASSERT( aTwipSz.Width() >= 0 && aTwipSz.Height() >= 0,
                "Rahmengroesse minus Abstand < 0!!!???" );
        if( aTwipSz.Width() < 0 )
            aTwipSz.Width() = 0;
        if( aTwipSz.Height() < 0 )
            aTwipSz.Height() = 0;

        Size aPixelSz( 0, 0 );
        if( (aTwipSz.Width() || aTwipSz.Height()) &&
            Application::GetDefaultDevice() )
        {
            aPixelSz =
                Application::GetDefaultDevice()->LogicToPixel( aTwipSz,
                                                    MapMode(MAP_TWIP) );
            if( !aPixelSz.Width() && aTwipSz.Width() )
                aPixelSz.Width() = 1;
            if( !aPixelSz.Height() && aTwipSz.Height() )
                aPixelSz.Height() = 1;
        }

        if( (nFrmOpts & HTML_FRMOPT_WIDTH) &&
            ((nPrcWidth && nPrcWidth!=255) || aPixelSz.Width()) )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_width) += '=';
            if( nPrcWidth )
                (sOut += ByteString::CreateFromInt32( nPrcWidth )) += '%';
            else
                sOut += ByteString::CreateFromInt32( aPixelSz.Width() );
        }

        if( (nFrmOpts & HTML_FRMOPT_HEIGHT) &&
            ((nPrcHeight && nPrcHeight!=255) || aPixelSz.Height()) )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_height) += '=';
            if( nPrcHeight )
                (sOut += ByteString::CreateFromInt32( nPrcHeight )) += '%';
            else
                sOut += ByteString::CreateFromInt32( aPixelSz.Height() );
        }
    }

    if( sOut.Len() )
        Strm() << sOut.GetBuffer();

    // Umlauf fuer absatzgeb. Grafiken als <BR CLEAR=...> in den String
    // schreiben
    if( (nFrmOpts & HTML_FRMOPT_BRCLEAR) &&
        (FLY_AT_CNTNT== rFrmFmt.GetAnchor().GetAnchorId() ||
         FLY_AUTO_CNTNT== rFrmFmt.GetAnchor().GetAnchorId()) &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_SURROUND, TRUE, &pItem ))
    {
        const SwFmtSurround* pSurround = (const SwFmtSurround*)pItem;
        sal_Int16 eHoriOri =    rFrmFmt.GetHoriOrient().GetHoriOrient();
        pStr = 0;
        SwSurround eSurround = pSurround->GetSurround();
        BOOL bAnchorOnly = pSurround->IsAnchorOnly();
        switch( eHoriOri )
        {
        case text::HoriOrientation::RIGHT:
            {
                switch( eSurround )
                {
                case SURROUND_NONE:
                case SURROUND_RIGHT:
                    pStr = OOO_STRING_SVTOOLS_HTML_AL_right;
                    break;
                case SURROUND_LEFT:
                case SURROUND_PARALLEL:
                    if( bAnchorOnly )
                        bClearRight = TRUE;
                    break;
                default:
                    ;
                }
            }
            break;

        default:
            // #67508#: If a frame is centered, it gets left aligned. This
            // should be taken into account here, too.
            {
                switch( eSurround )
                {
                case SURROUND_NONE:
                case SURROUND_LEFT:
                    pStr = OOO_STRING_SVTOOLS_HTML_AL_left;
                    break;
                case SURROUND_RIGHT:
                case SURROUND_PARALLEL:
                    if( bAnchorOnly )
                        bClearLeft = TRUE;
                    break;
                default:
                    ;
                }
            }
            break;

        }

        if( pStr )
        {
            (((((((sOut = '<') += OOO_STRING_SVTOOLS_HTML_linebreak) += ' ')
                += OOO_STRING_SVTOOLS_HTML_O_clear) += '=') += pStr) += '>') += rEndTags;
            rEndTags = sOut;
        }
    }
}


Writer& OutHTML_Image( Writer& rWrt, const SwFrmFmt &rFrmFmt,
                       const String &rGrfName, const String& rAlternateTxt,
                       const Size &rRealSize, sal_uInt32 nFrmOpts,
                       const sal_Char *pMarkType,
                       const ImageMap *pAltImgMap )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    // ggf. ein noch offenes Attribut voruebergehend beenden
    if( rHTMLWrt.aINetFmts.Count() )
    {
        SwFmtINetFmt *pINetFmt =
            rHTMLWrt.aINetFmts[ rHTMLWrt.aINetFmts.Count()-1 ];
        OutHTML_INetFmt( rWrt, *pINetFmt, FALSE );
    }

    String aGrfNm( rGrfName );
    if( !HTMLOutFuncs::PrivateURLToInternalImg(aGrfNm) )
        aGrfNm = URIHelper::simpleNormalizedMakeRelative( rWrt.GetBaseURL(), aGrfNm);

    const SfxPoolItem* pItem;
    const SfxItemSet& rItemSet = rFrmFmt.GetAttrSet();

    const SwFmtURL *pURLItem = 0;

    // das URL-Attribut nur beruecksichtigen, wenn keine Image-Map
    // uebergeben wurde
    if( !pAltImgMap &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_URL, TRUE, &pItem ))
    {
        pURLItem = (const SwFmtURL *)pItem;
    }

    // Image-Map rausschreiben
    const ImageMap *pIMap = pAltImgMap;
    if( !pIMap && pURLItem )
    {
        pIMap = pURLItem->GetMap();
    }

    String aIMapName;
    if( pIMap )
    {
        // den Namen eindeutig machen
        aIMapName = pIMap->GetName();
        String aNameBase;
        if( aIMapName.Len() )
            aNameBase = aIMapName;
        else
            aNameBase.AssignAscii( OOO_STRING_SVTOOLS_HTML_map );
        if( !aIMapName.Len() )
            (aIMapName = aNameBase)
                += String::CreateFromInt32( rHTMLWrt.nImgMapCnt );

        BOOL bFound;
        do
        {
            bFound = FALSE;
            for( USHORT i=0; i<rHTMLWrt.aImgMapNames.Count(); i++ )
            {
                // TODO: Unicode: Comparison is case insensitive for ASCII
                // characters only now!
                if( aIMapName.EqualsIgnoreCaseAscii(*rHTMLWrt.aImgMapNames[i]) )
                {
                    bFound = TRUE;
                    break;
                }
            }
            if( bFound )
            {
                rHTMLWrt.nImgMapCnt++;
                (aIMapName = aNameBase)
                    += String::CreateFromInt32( rHTMLWrt.nImgMapCnt );
            }

        } while( bFound );

        BOOL bScale = FALSE;
        //Size aGrfSize( rNode.GetTwipSize() );
        Fraction aScaleX( 1, 1 );
        Fraction aScaleY( 1, 1 );

        const SwFmtFrmSize& rFrmSize = rFrmFmt.GetFrmSize();
        const SvxBoxItem& rBox = rFrmFmt.GetBox();

        if( !rFrmSize.GetWidthPercent() && rRealSize.Width() )
        {
            SwTwips nWidth = rFrmSize.GetWidth();
            nWidth -= ( rBox.CalcLineSpace(BOX_LINE_LEFT) +
                        rBox.CalcLineSpace(BOX_LINE_RIGHT) );

            ASSERT( nWidth>0, "Gibt es 0 twip breite Grafiken!?" );
            if( nWidth<=0 )	// sollte nicht passieren
                nWidth = 1;

            if( rRealSize.Width() != nWidth )
            {
                aScaleX = Fraction( nWidth, rRealSize.Width() );
                bScale = TRUE;
            }
        }
        if( !rFrmSize.GetHeightPercent() && rRealSize.Height() )
        {
            SwTwips nHeight = rFrmSize.GetHeight();
            nHeight -= ( rBox.CalcLineSpace(BOX_LINE_TOP) +
                         rBox.CalcLineSpace(BOX_LINE_BOTTOM) );

            ASSERT( nHeight>0, "Gibt es 0 twip hohe Grafiken!?" );
            if( nHeight<=0 )
                nHeight = 1;

            if( rRealSize.Height() != nHeight )
            {
                aScaleY = Fraction( nHeight, rRealSize.Height() );
                bScale = TRUE;
            }
        }

        rHTMLWrt.aImgMapNames.Insert( new String(aIMapName),
                                          rHTMLWrt.aImgMapNames.Count() );

        ByteString aIndMap, aIndArea;
        const sal_Char *pLF = 0, *pIndArea = 0, *pIndMap = 0;
#if defined(UNX)
        sal_Char aLF[2] = "\x00";
#endif

        if( rHTMLWrt.bLFPossible )
        {
            rHTMLWrt.OutNewLine( TRUE );
            rHTMLWrt.GetIndentString( aIndMap );
            rHTMLWrt.GetIndentString( aIndArea, 1 );
#if defined(UNX)
            aLF[0]  = SwHTMLWriter::sNewLine;
            pLF = aLF;
#else
            pLF = SwHTMLWriter::sNewLine;
#endif
            pIndArea = aIndArea.GetBuffer();
            pIndMap = aIndMap.GetBuffer();
        }

        if( bScale )
        {
            ImageMap aScaledIMap( *pIMap );
            aScaledIMap.Scale( aScaleX, aScaleY );
            HTMLOutFuncs::Out_ImageMap( rWrt.Strm(), rWrt.GetBaseURL(), aScaledIMap, aIMapName,
                                        aIMapEventTable,
                                        rHTMLWrt.bCfgStarBasic,
                                        pLF, pIndArea, pIndMap,
                                        rHTMLWrt.eDestEnc,
                                        &rHTMLWrt.aNonConvertableCharacters );
        }
        else
        {
            HTMLOutFuncs::Out_ImageMap( rWrt.Strm(), rWrt.GetBaseURL(), *pIMap, aIMapName,
                                        aIMapEventTable,
                                        rHTMLWrt.bCfgStarBasic,
                                        pLF, pIndArea, pIndMap,
                                         rHTMLWrt.eDestEnc,
                                        &rHTMLWrt.aNonConvertableCharacters );
        }
    }

    // wenn meoglich vor der Grafik einen Zeilen-Umbruch ausgeben
    if( rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine( TRUE );

    // Attribute die ausserhelb der Grafik geschreiben werden muessen sammeln
    ByteString sOut;
    ByteString aEndTags;

    // implizite Sprungmarke -> <A NAME=...></A>...<IMG ...>
    if( pMarkType && rFrmFmt.GetName().Len() )
        rHTMLWrt.OutImplicitMark( rFrmFmt.GetName(), pMarkType );

    // URL -> <A>...<IMG ... >...</A>
    const SvxMacroItem *pMacItem = 0;
    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_FRMMACRO, TRUE, &pItem ))
        pMacItem = (const SvxMacroItem *)pItem;

    if( pURLItem || pMacItem )
    {
        String aMapURL;
        String aName;
        String aTarget;
        if( pURLItem )
        {
            aMapURL = pURLItem->GetURL();
            aName = pURLItem->GetName();
            aTarget = pURLItem->GetTargetFrameName();
        }
        BOOL bEvents = pMacItem && pMacItem->GetMacroTable().Count();

        if( aMapURL.Len() || aName.Len() || aTarget.Len() || bEvents )
        {
            (sOut = '<') += OOO_STRING_SVTOOLS_HTML_anchor;

            // Ein HREF nur Ausgaben, wenn es einen Link oder Makros gibt
            if( aMapURL.Len() || bEvents )
            {
                ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_href) += "=\"";
                rWrt.Strm() << sOut.GetBuffer();
                rHTMLWrt.OutHyperlinkHRefValue( aMapURL );
                sOut = '\"';
            }

            if( aName.Len() )
            {
                ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"";
                rWrt.Strm() << sOut.GetBuffer();
                HTMLOutFuncs::Out_String( rWrt.Strm(), aName,
                                          rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
                sOut = '\"';
            }

            if( aTarget.Len() )
            {
                ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_target) += "=\"";
                rWrt.Strm() << sOut.GetBuffer();
                HTMLOutFuncs::Out_String( rWrt.Strm(), aTarget,
                                          rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
                sOut = '\"';
            }
            if( sOut.Len() )
            {
                rWrt.Strm() << sOut.GetBuffer();
                sOut.Erase();
            }

            if( pMacItem )
            {
                const SvxMacroTableDtor& rMacTable = pMacItem->GetMacroTable();
                if( rMacTable.Count() )
                    HTMLOutFuncs::Out_Events( rWrt.Strm(), rMacTable,
                                              aAnchorEventTable,
                                              rHTMLWrt.bCfgStarBasic,
                                                 rHTMLWrt.eDestEnc,
                                        &rHTMLWrt.aNonConvertableCharacters );
            }

            rWrt.Strm() << ">";
            (((sOut = "</") += OOO_STRING_SVTOOLS_HTML_anchor) += ">") += aEndTags;
            aEndTags = sOut;
        }
    }

    // Umrandung -> <FONT COLOR = ...>...<IMG ... >...</FONT>
    USHORT nBorderWidth = 0;
    if( (nFrmOpts & HTML_FRMOPT_BORDER) &&
        SFX_ITEM_SET == rItemSet.GetItemState( RES_BOX, TRUE, &pItem ))
    {
        Size aTwipBorder( 0, 0 );
        const SvxBoxItem* pBoxItem = (const SvxBoxItem*)pItem;

        const SvxBorderLine *pColBorderLine = 0;
        const SvxBorderLine *pBorderLine = pBoxItem->GetLeft();
        if( pBorderLine )
        {
            pColBorderLine = pBorderLine;
            aTwipBorder.Width() += pBorderLine->GetOutWidth();
        }

        pBorderLine = pBoxItem->GetRight();
        if( pBorderLine )
        {
            pColBorderLine = pBorderLine;
            aTwipBorder.Width() += pBorderLine->GetOutWidth();
        }

        pBorderLine = pBoxItem->GetTop();
        if( pBorderLine )
        {
            pColBorderLine = pBorderLine;
            aTwipBorder.Height() += pBorderLine->GetOutWidth();
        }

        pBorderLine = pBoxItem->GetBottom();
        if( pBorderLine )
        {
            pColBorderLine = pBorderLine;
            aTwipBorder.Height() += pBorderLine->GetOutWidth();
        }

        aTwipBorder.Width() /= 2;
        aTwipBorder.Height() /= 2;

        if( (aTwipBorder.Width() || aTwipBorder.Height()) &&
            Application::GetDefaultDevice() )
        {
            Size aPixelBorder =
                Application::GetDefaultDevice()->LogicToPixel( aTwipBorder,
                                                    MapMode(MAP_TWIP) );
            if( !aPixelBorder.Width() && aTwipBorder.Width() )
                aPixelBorder.Width() = 1;
            if( !aPixelBorder.Height() && aTwipBorder.Height() )
                aPixelBorder.Height() = 1;

            if( aPixelBorder.Width() )
                aPixelBorder.Height() = 0;

            nBorderWidth =
                (USHORT)(aPixelBorder.Width() + aPixelBorder.Height());
        }

        if( pColBorderLine )
        {
            sOut = '<';
            (((sOut += OOO_STRING_SVTOOLS_HTML_font) += ' ') += OOO_STRING_SVTOOLS_HTML_O_color) += '=';
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_Color( rWrt.Strm(),
                                     pColBorderLine->GetColor(), rHTMLWrt.eDestEnc ) << '>';

            (((sOut = "</" ) += OOO_STRING_SVTOOLS_HTML_font) += '>') += aEndTags;
            aEndTags = sOut;
        }
    }

    sOut = '<';
    (((sOut += OOO_STRING_SVTOOLS_HTML_image) += ' ') += OOO_STRING_SVTOOLS_HTML_O_src) += "=\"";
    rWrt.Strm() << sOut.GetBuffer();
    HTMLOutFuncs::Out_String( rWrt.Strm(), aGrfNm, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters ) << '\"';

    // Events
    sOut.Erase();
    if( SFX_ITEM_SET == rItemSet.GetItemState( RES_FRMMACRO, TRUE, &pItem ))
    {
        const SvxMacroTableDtor& rMacTable =
            ((const SvxMacroItem *)pItem)->GetMacroTable();
        if( rMacTable.Count() )
            HTMLOutFuncs::Out_Events( rWrt.Strm(), rMacTable, aImageEventTable,
                                      rHTMLWrt.bCfgStarBasic, rHTMLWrt.eDestEnc,
                                        &rHTMLWrt.aNonConvertableCharacters );
    }

    // ALT, ALIGN, WIDTH, HEIGHT, HSPACE, VSPACE
    rHTMLWrt.OutFrmFmtOptions( rFrmFmt, rAlternateTxt, aEndTags, nFrmOpts );
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_ABS_POS_FLY ) )
        rHTMLWrt.OutCSS1_FrmFmtOptions( rFrmFmt, nFrmOpts );


    if( nFrmOpts & HTML_FRMOPT_BORDER )
    {
        (((sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_border) += '=')
            += ByteString::CreateFromInt32( nBorderWidth );
        rWrt.Strm() << sOut.GetBuffer();
    }

    if( pURLItem && pURLItem->IsServerMap() )
    {
        (sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_ismap;
        rWrt.Strm() << sOut.GetBuffer();
    }
    if( aIMapName.Len() )
    {
        ((sOut = ' ') += OOO_STRING_SVTOOLS_HTML_O_usemap) += "=\"#";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), aIMapName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters ) << '\"';
    }

    rHTMLWrt.Strm() << '>';

    if( aEndTags.Len() )
        rWrt.Strm() << aEndTags.GetBuffer();

    if( rHTMLWrt.aINetFmts.Count() )
    {
        // es ist noch ein Attribut auf dem Stack, das wieder geoeffnet
        // werden muss
        SwFmtINetFmt *pINetFmt =
            rHTMLWrt.aINetFmts[ rHTMLWrt.aINetFmts.Count()-1 ];
        OutHTML_INetFmt( rWrt, *pINetFmt, TRUE );
    }

    return rHTMLWrt;
}

Writer& OutHTML_BulletImage( Writer& rWrt,
                             const sal_Char *pTag,
                             const SvxBrushItem* pBrush,
                             String &rGrfName,
                             const Size &rSize,
                             const SwFmtVertOrient* pVertOrient )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    // Wenn es ein BrushItem gibt, muss die Grafiknoch exportiert werden
    const String *pLink = 0;
    if( pBrush )
    {
        pLink = pBrush->GetGraphicLink();

        // embeddete Grafik -> WriteEmbedded schreiben
        if( !pLink )
        {
            const Graphic* pGrf = pBrush->GetGraphic();
            if( pGrf )
            {
                // Grafik als (JPG-)File speichern
                if( rHTMLWrt.GetOrigFileName() )
                    rGrfName = *rHTMLWrt.GetOrigFileName();
                USHORT nErr = XOutBitmap::WriteGraphic(	*pGrf,  rGrfName,
                        String::CreateFromAscii("JPG"),
                        (XOUTBMP_USE_GIF_IF_SENSIBLE |
                         XOUTBMP_USE_NATIVE_IF_POSSIBLE));
                if( !nErr )
                {
                    rGrfName = URIHelper::SmartRel2Abs(
                        INetURLObject( rWrt.GetBaseURL() ), rGrfName,
                        URIHelper::GetMaybeFileHdl() );
                    pLink = &rGrfName;
                }
                else
                {
                    rHTMLWrt.nWarn = WARN_SWG_POOR_LOAD | WARN_SW_WRITE_BASE;
                }
            }
        }
        else
        {
            rGrfName = *pLink;
            if( rHTMLWrt.bCfgCpyLinkedGrfs )
            {
                rHTMLWrt.CopyLocalFileToINet( rGrfName );
                pLink = &rGrfName;
            }
        }
    }
    else
    {
        pLink = &rGrfName;
    }

    ByteString sOut;
    if( pTag )
        (sOut += '<') += pTag;

    if( pLink )
    {
        sOut += ' ';
        String s( *pLink );
        if( !HTMLOutFuncs::PrivateURLToInternalImg(s) )
            s = URIHelper::simpleNormalizedMakeRelative( rWrt.GetBaseURL(), s);
        (sOut += OOO_STRING_SVTOOLS_HTML_O_src) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), s, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
        sOut = '\"';

        // Groesse des Objekts Twips ohne Raender
        Size aPixelSz( 0, 0 );
        if( (rSize.Width() || rSize.Height()) && Application::GetDefaultDevice() )
        {
            aPixelSz =
                Application::GetDefaultDevice()->LogicToPixel( rSize,
                                                    MapMode(MAP_TWIP) );
            if( !aPixelSz.Width() && rSize.Width() )
                aPixelSz.Width() = 1;
            if( !aPixelSz.Height() && rSize.Height() )
                aPixelSz.Height() = 1;
        }

        if( aPixelSz.Width() )
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_width) += '=')
                += ByteString::CreateFromInt32( aPixelSz.Width() );

        if( aPixelSz.Height() )
            (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_height) += '=')
                += ByteString::CreateFromInt32( aPixelSz.Height() );

        if( pVertOrient )
        {
            const sal_Char *pStr = 0;
            switch( pVertOrient->GetVertOrient() )
            {
            case text::VertOrientation::LINE_TOP:     pStr = OOO_STRING_SVTOOLS_HTML_VA_top;        break;
            case text::VertOrientation::CHAR_TOP:
            case text::VertOrientation::BOTTOM:       pStr = OOO_STRING_SVTOOLS_HTML_VA_texttop;    break;  // geht nicht
            case text::VertOrientation::LINE_CENTER:
            case text::VertOrientation::CHAR_CENTER:  pStr = OOO_STRING_SVTOOLS_HTML_VA_absmiddle;  break;  // geht nicht
            case text::VertOrientation::CENTER:       pStr = OOO_STRING_SVTOOLS_HTML_VA_middle;     break;
            case text::VertOrientation::LINE_BOTTOM:
            case text::VertOrientation::CHAR_BOTTOM:  pStr = OOO_STRING_SVTOOLS_HTML_VA_absbottom;  break;  // geht nicht
            case text::VertOrientation::TOP:          pStr = OOO_STRING_SVTOOLS_HTML_VA_bottom;     break;
            case text::VertOrientation::NONE:     break;
            }
            if( pStr )
                (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_align) += '=') += pStr;
        }
    }

    if( pTag )
        sOut += '>';
    rWrt.Strm() << sOut.GetBuffer();

    return rWrt;
}


//-----------------------------------------------------------------------

static Writer& OutHTML_FrmFmtTableNode( Writer& rWrt, const SwFrmFmt& rFrmFmt )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
    ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex()+1;
    ULONG nEnd = rHTMLWrt.pDoc->GetNodes()[nStt-1]->EndOfSectionIndex();

    String aCaption;
    BOOL bTopCaption = FALSE;

    // Nicht const, weil GetTable spater mal nicht const ist
    SwNode *pNd = rHTMLWrt.pDoc->GetNodes()[ nStt ];
    SwTableNode *pTblNd = pNd->GetTableNode();
    const SwTxtNode *pTxtNd = pNd->GetTxtNode();
    if( !pTblNd && pTxtNd )
    {
        // Tabelle mit Ueberschrift
        bTopCaption = TRUE;
        pTblNd = rHTMLWrt.pDoc->GetNodes()[nStt+1]->GetTableNode();
    }
    ASSERT( pTblNd, "Rahmen enthaelt keine Tabelle" );
    if( pTblNd )
    {
        ULONG nTblEnd = pTblNd->EndOfSectionIndex();
        ASSERT( nTblEnd == nEnd - 1 ||
                (nTblEnd == nEnd - 2 && !bTopCaption),
                "Ungeuelter Rahmen-Inhalt fuer Tabelle" );

        if( nTblEnd == nEnd - 2 )
            pTxtNd = rHTMLWrt.pDoc->GetNodes()[nTblEnd+1]->GetTxtNode();
    }
    if( pTxtNd )
        aCaption = pTxtNd->GetTxt();

    {
        HTMLSaveData aSaveData( rHTMLWrt, pTblNd->GetIndex()+1,
                                pTblNd->EndOfSectionIndex(),
                                   sal_True, &rFrmFmt );
        rHTMLWrt.bOutFlyFrame = sal_True;
        OutHTML_SwTblNode( rHTMLWrt, *pTblNd, &rFrmFmt, &aCaption,
                           bTopCaption );
    }

    return rWrt;
}

static Writer & OutHTML_FrmFmtAsMulticol( Writer& rWrt,
                                          const SwFrmFmt& rFrmFmt,
                                          BOOL bInCntnr	)
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    rHTMLWrt.ChangeParaToken( 0 );

    // Die aktulle <DL> beenden!
    rHTMLWrt.OutAndSetDefList( 0 );

    // als Multicol ausgeben
    if( rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine();

    ByteString sOut( '<' );
    sOut += OOO_STRING_SVTOOLS_HTML_multicol;

    const SwFmtCol& rFmtCol = rFrmFmt.GetCol();

    // die Anzahl der Spalten als COLS ausgeben
    USHORT nCols = rFmtCol.GetNumCols();
    if( nCols )
        (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_cols) += '=')
            += ByteString::CreateFromInt32( nCols );

    // die Gutter-Breite (Minimalwert) als GUTTER
    USHORT nGutter = rFmtCol.GetGutterWidth( TRUE );
    if( nGutter!=USHRT_MAX )
    {
        if( nGutter && Application::GetDefaultDevice() )
        {
            nGutter = (USHORT)Application::GetDefaultDevice()
                            ->LogicToPixel( Size(nGutter,0),
                                            MapMode(MAP_TWIP) ).Width();
        }
        (((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_gutter) += '=')
            += ByteString::CreateFromInt32( nGutter );
    }

    rWrt.Strm() << sOut.GetBuffer();

    // WIDTH
    ULONG nFrmFlags = bInCntnr ? HTML_FRMOPTS_MULTICOL_CNTNR
                                : HTML_FRMOPTS_MULTICOL;
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_ABS_POS_FLY ) && !bInCntnr )
        nFrmFlags |= HTML_FRMOPTS_MULTICOL_CSS1;
    ByteString aEndTags;
    rHTMLWrt.OutFrmFmtOptions( rFrmFmt, aEmptyStr, aEndTags, nFrmFlags );
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_ABS_POS_FLY ) && !bInCntnr )
        rHTMLWrt.OutCSS1_FrmFmtOptions( rFrmFmt, nFrmFlags );

    rWrt.Strm() << '>';

    rHTMLWrt.bLFPossible = TRUE;
    rHTMLWrt.IncIndentLevel();	// den Inhalt von Multicol einruecken;

    const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
    ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex();
    const SwStartNode* pSttNd = rWrt.pDoc->GetNodes()[nStt]->GetStartNode();
    ASSERT( pSttNd, "Wo ist der Start-Node" );

    {
        // in einem Block damit rechtzeitig vor dem Ende der alte Zustand
        // wieder hergestellt wird.
        HTMLSaveData aSaveData( rHTMLWrt, nStt+1,
                                pSttNd->EndOfSectionIndex(),
                                   sal_True, &rFrmFmt );
        rHTMLWrt.bOutFlyFrame = sal_True;
        rHTMLWrt.Out_SwDoc( rWrt.pCurPam );
    }

//	rHTMLWrt.ChangeParaToken( 0 );	// MIB 8.7.97: Passiert jetzt in Out_SwDoc

    rHTMLWrt.DecIndentLevel();	// den Inhalt von Multicol einruecken;
    if( rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine();
    HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_multicol, FALSE );
    rHTMLWrt.bLFPossible = TRUE;

    return rWrt;
}

static Writer& OutHTML_FrmFmtAsSpacer( Writer& rWrt, const SwFrmFmt& rFrmFmt )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    // wenn meoglich vor der Grafik einen Zeilen-Umbruch ausgeben
    if( rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine( TRUE );

    ByteString sOut('<');
    ((((sOut += OOO_STRING_SVTOOLS_HTML_spacer) += ' ') += OOO_STRING_SVTOOLS_HTML_O_type) += '=')
        += OOO_STRING_SVTOOLS_HTML_SPTYPE_block;
    rWrt.Strm() << sOut.GetBuffer();

    // ALIGN, WIDTH, HEIGHT
    ByteString aEndTags;
    rHTMLWrt.OutFrmFmtOptions( rFrmFmt, aEmptyStr, aEndTags,
                                HTML_FRMOPTS_SPACER );

    rWrt.Strm() << '>';
    if( aEndTags.Len() )
        rWrt.Strm() << aEndTags.GetBuffer();

    return rWrt;
}

static Writer& OutHTML_FrmFmtAsDivOrSpan( Writer& rWrt,
                                          const SwFrmFmt& rFrmFmt, BOOL bSpan)
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    const sal_Char *pStr = 0;
    if( !bSpan )
    {
        rHTMLWrt.ChangeParaToken( 0 );

        // Die aktulle <DL> beenden!
        rHTMLWrt.OutAndSetDefList( 0 );
        pStr = OOO_STRING_SVTOOLS_HTML_division;
    }
    else
        pStr = OOO_STRING_SVTOOLS_HTML_span;

    // als DIV ausgeben
    if( rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine();

    ByteString sOut( '<' );
    sOut += pStr;

    rWrt.Strm() << sOut.GetBuffer();
    ByteString aEndTags;
    ULONG nFrmFlags = HTML_FRMOPTS_DIV;
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_BORDER_NONE ) )
       nFrmFlags |= HTML_FRMOPT_S_NOBORDER;
    rHTMLWrt.OutFrmFmtOptions( rFrmFmt, aEmptyStr, aEndTags, nFrmFlags );
    rHTMLWrt.OutCSS1_FrmFmtOptions( rFrmFmt, nFrmFlags );
    rWrt.Strm() << '>';

    rHTMLWrt.IncIndentLevel();	// den Inhalt einruecken
    rHTMLWrt.bLFPossible = TRUE;

    const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
    ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex();

    // Am Start-Node verankerte Rahmen-gebundene Rahmen ausgeben
    rHTMLWrt.OutFlyFrm( nStt, 0, HTML_POS_ANY );

    const SwStartNode* pSttNd = rWrt.pDoc->GetNodes()[nStt]->GetStartNode();
    ASSERT( pSttNd, "Wo ist der Start-Node" );

    {
        // in einem Block damit rechtzeitig vor dem Ende der alte Zustand
        // wieder hergestellt wird.
        HTMLSaveData aSaveData( rHTMLWrt, nStt+1,
                                pSttNd->EndOfSectionIndex(),
                                   sal_True, &rFrmFmt );
        rHTMLWrt.bOutFlyFrame = sal_True;
        rHTMLWrt.Out_SwDoc( rWrt.pCurPam );
    }

    rHTMLWrt.DecIndentLevel();	// den Inhalt von Multicol einruecken;
    if( rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine();
    HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), pStr, FALSE );

    if( aEndTags.Len() )
        rWrt.Strm() << aEndTags.GetBuffer();

    return rWrt;
}

static Writer & OutHTML_FrmFmtAsImage( Writer& rWrt, const SwFrmFmt& rFrmFmt,
                                       BOOL /*bInCntnr*/ )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    ImageMap aIMap;
    Graphic aGrf( ((SwFrmFmt &)rFrmFmt).MakeGraphic( &aIMap ) );

    String aGrfNm;
    if( rHTMLWrt.GetOrigFileName() )
        aGrfNm = *rHTMLWrt.GetOrigFileName();
    if( aGrf.GetType() == GRAPHIC_NONE ||
        XOutBitmap::WriteGraphic( aGrf, aGrfNm,
                                  String::CreateFromAscii( "JPG" ),
                                  (XOUTBMP_USE_GIF_IF_POSSIBLE|
                                   XOUTBMP_USE_NATIVE_IF_POSSIBLE) ) != 0 )
    {
        // leer oder fehlerhaft, da ist nichts auszugeben
        rHTMLWrt.nWarn = WARN_SWG_POOR_LOAD | WARN_SW_WRITE_BASE;
        return rWrt;
    }

    aGrfNm = URIHelper::SmartRel2Abs(
        INetURLObject(rWrt.GetBaseURL()), aGrfNm,
        URIHelper::GetMaybeFileHdl() );
    Size aSz( 0, 0 );
    OutHTML_Image( rWrt, rFrmFmt, aGrfNm, rFrmFmt.GetName(), aSz,
                    HTML_FRMOPTS_GENIMG, pMarkToFrame,
                    aIMap.GetIMapObjectCount() ? &aIMap : 0 );
    return rWrt;
}


static Writer& OutHTML_FrmFmtGrfNode( Writer& rWrt, const SwFrmFmt& rFrmFmt,
                                      BOOL bInCntnr )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;

    const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
    ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex()+1;
    SwGrfNode *pGrfNd = rHTMLWrt.pDoc->GetNodes()[ nStt ]->GetGrfNode();
    ASSERT( pGrfNd, "Grf-Node erwartet" );
    if( !pGrfNd )
        return rWrt;

    const SwMirrorGrf& rMirror = pGrfNd->GetSwAttrSet().GetMirrorGrf();

    String aGrfNm;
    if( !pGrfNd->IsLinkedFile() || RES_MIRROR_GRAPH_DONT != rMirror.GetValue() )
    {
        // Grafik als File-Referenz speichern (als JPEG-Grafik speichern)
        if( rHTMLWrt.GetOrigFileName() )
            aGrfNm = *rHTMLWrt.GetOrigFileName();
        pGrfNd->SwapIn( TRUE );

        ULONG nFlags = XOUTBMP_USE_GIF_IF_SENSIBLE |
                       XOUTBMP_USE_NATIVE_IF_POSSIBLE;
        switch( rMirror.GetValue() )
        {
        case RES_MIRROR_GRAPH_VERT: nFlags = XOUTBMP_MIRROR_HORZ; break;
        case RES_MIRROR_GRAPH_HOR:    nFlags = XOUTBMP_MIRROR_VERT; break;
        case RES_MIRROR_GRAPH_BOTH:
            nFlags = XOUTBMP_MIRROR_VERT | XOUTBMP_MIRROR_HORZ;
            break;
        }

        Size aMM100Size;
        const SwFmtFrmSize& rSize = rFrmFmt.GetFrmSize();
        aMM100Size = OutputDevice::LogicToLogic( rSize.GetSize(),
                        MapMode( MAP_TWIP ), MapMode( MAP_100TH_MM ));

        USHORT nErr = XOutBitmap::WriteGraphic( pGrfNd->GetGrf(), aGrfNm,
                String::CreateFromAscii("JPG"), nFlags, &aMM100Size );
        if( nErr )     			// fehlerhaft, da ist nichts auszugeben
        {
            rHTMLWrt.nWarn = WARN_SWG_POOR_LOAD | WARN_SW_WRITE_BASE;
            return rWrt;
        }
        aGrfNm = URIHelper::SmartRel2Abs(
            INetURLObject(rWrt.GetBaseURL()), aGrfNm,
            URIHelper::GetMaybeFileHdl() );
    }
    else
    {
        pGrfNd->GetFileFilterNms( &aGrfNm, 0 );
        if( rHTMLWrt.bCfgCpyLinkedGrfs )
            rWrt.CopyLocalFileToINet( aGrfNm );
    }

    ULONG nFrmFlags = bInCntnr ? HTML_FRMOPTS_IMG_CNTNR : HTML_FRMOPTS_IMG;
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_ABS_POS_FLY ) && !bInCntnr )
        nFrmFlags |= HTML_FRMOPTS_IMG_CSS1;
    OutHTML_Image( rWrt, rFrmFmt, aGrfNm, pGrfNd->GetTitle(),
                   pGrfNd->GetTwipSize(), nFrmFlags, pMarkToGraphic );

    return rWrt;
}


static Writer& OutHTML_FrmFmtAsMarquee( Writer& rWrt, const SwFrmFmt& rFrmFmt,
                                  const SdrObject& rSdrObj	)
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    // die Edit-Engine-Attribute des Objekts als SW-Attribute holen
    // und als Hints einsortieren
    const SfxItemSet& rFmtItemSet = rFrmFmt.GetAttrSet();
    SfxItemSet aItemSet( *rFmtItemSet.GetPool(), RES_CHRATR_BEGIN,
                                                 RES_CHRATR_END );
    SwHTMLWriter::GetEEAttrsFromDrwObj( aItemSet, &rSdrObj, TRUE );
    BOOL bCfgOutStylesOld = rHTMLWrt.bCfgOutStyles;
    rHTMLWrt.bCfgOutStyles = FALSE;
    rHTMLWrt.bTxtAttr = TRUE;
    rHTMLWrt.bTagOn = TRUE;
    Out_SfxItemSet( aHTMLAttrFnTab, rWrt, aItemSet, FALSE );
    rHTMLWrt.bTxtAttr = FALSE;

    OutHTML_DrawFrmFmtAsMarquee( rHTMLWrt,
                                 (const SwDrawFrmFmt &)rFrmFmt,
                                 rSdrObj );
    rHTMLWrt.bTxtAttr = TRUE;
    rHTMLWrt.bTagOn = FALSE;
    Out_SfxItemSet( aHTMLAttrFnTab, rWrt, aItemSet, FALSE );
    rHTMLWrt.bTxtAttr = FALSE;
    rHTMLWrt.bCfgOutStyles = bCfgOutStylesOld;

    return rWrt;
}

//-----------------------------------------------------------------------

Writer& OutHTML_HeaderFooter( Writer& rWrt, const SwFrmFmt& rFrmFmt,
                              BOOL bHeader )
{
    SwHTMLWriter & rHTMLWrt = (SwHTMLWriter&)rWrt;

    // als Multicol ausgeben
    rHTMLWrt.OutNewLine();
    ByteString sOut( OOO_STRING_SVTOOLS_HTML_division );
    ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_type) += '=';
    sOut += (bHeader ? "HEADER" : "FOOTER" );
    HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), sOut.GetBuffer() );

    rHTMLWrt.IncIndentLevel();	// den Inhalt von Multicol einruecken;

    // Einen Spacer fuer den Absatnd zusammenbasteln. Da durch das
    // <DL> bzw. </DL> immer einer Absatz-Abstand entsteht, wird der
    // ggf. abgezogen.
    const SvxULSpaceItem& rULSpace = rFrmFmt.GetULSpace();
    USHORT nSize = bHeader ? rULSpace.GetLower() : rULSpace.GetUpper();
    rHTMLWrt.nHeaderFooterSpace = nSize;

    ByteString aSpacer;
    if( rHTMLWrt.IsHTMLMode(HTMLMODE_VERT_SPACER) &&
        nSize > HTML_PARSPACE && Application::GetDefaultDevice() )
    {
        nSize -= HTML_PARSPACE;
        nSize = (INT16)Application::GetDefaultDevice()
            ->LogicToPixel( Size(nSize,0), MapMode(MAP_TWIP) ).Width();

        ((((((((aSpacer = OOO_STRING_SVTOOLS_HTML_spacer) += ' ')
            += OOO_STRING_SVTOOLS_HTML_O_type) += '=') += OOO_STRING_SVTOOLS_HTML_SPTYPE_vertical) += ' ')
            += OOO_STRING_SVTOOLS_HTML_O_size) += '=') += ByteString::CreateFromInt32(nSize);
    }

    const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
    ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex();
    const SwStartNode* pSttNd = rWrt.pDoc->GetNodes()[nStt]->GetStartNode();
    ASSERT( pSttNd, "Wo ist der Start-Node" );

    if( !bHeader && aSpacer.Len() )
    {
        rHTMLWrt.OutNewLine();
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), aSpacer.GetBuffer() );
    }

    {
        // in einem Block damit rechtzeitig vor dem Ende der alte Zustand
        // wieder hergestellt wird. pFlyFmt braucht hier nicht gestzt zu
        // werden, denn PageDesc-Attribute koennen hier nicht vorkommen
        HTMLSaveData aSaveData( rHTMLWrt, nStt+1,
                                pSttNd->EndOfSectionIndex() );

        if( bHeader )
            rHTMLWrt.bOutHeader = TRUE;
        else
            rHTMLWrt.bOutFooter = TRUE;

        rHTMLWrt.Out_SwDoc( rWrt.pCurPam );
    }

    if( bHeader && aSpacer.Len() )
    {
        rHTMLWrt.OutNewLine();
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), aSpacer.GetBuffer() );
    }

    rHTMLWrt.DecIndentLevel();	// den Inhalt von Multicol einruecken;
    rHTMLWrt.OutNewLine();
    HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_division, FALSE );

    rHTMLWrt.nHeaderFooterSpace = 0;

    return rWrt;
}


void SwHTMLWriter::AddLinkTarget( const String& rURL )
{
    if( !rURL.Len() || rURL.GetChar(0) != '#' )
        return;

    // There might be a '|' as delimiter (if the link has been inserted
    // freshly) or a '%7c' or a '%7C' if the document has been saved and
    // loaded already.
    xub_StrLen nPos = rURL.Len();
    sal_Bool bFound = sal_False, bEncoded = sal_False;
    while( !bFound && nPos > 0 )
    {
        sal_Unicode c = rURL.GetChar( --nPos );
        switch( c )
        {
        case cMarkSeperator:
            bFound = sal_True;
            break;
        case '%':
            bFound = (rURL.Len() - nPos) >=3 &&
                     rURL.GetChar( nPos+1 ) == '7' &&
                     ((c =rURL.GetChar( nPos+2 )) == 'C' || c == 'c');
            if( bFound )
                bEncoded = sal_True;
        }
    }
    if( !bFound || nPos < 2 ) // mindetsens "#a|..."
        return;

    String aURL( rURL.Copy( 1 ) );

    String sCmp( aURL.Copy( bEncoded ? nPos+2 : nPos ) ); // nPos-1+1/3 (-1 wg. Erase)
    sCmp.EraseAllChars();
    if( !sCmp.Len() )
        return;

    sCmp.ToLowerAscii();

    if( sCmp.EqualsAscii( pMarkToRegion ) ||
        sCmp.EqualsAscii( pMarkToFrame ) ||
        sCmp.EqualsAscii( pMarkToGraphic ) ||
        sCmp.EqualsAscii( pMarkToOLE ) ||
        sCmp.EqualsAscii( pMarkToTable ) )
    {
        // Einfach nur in einem sortierten Array merken
        if( bEncoded )
        {
            aURL.Erase( nPos, 2 );
            aURL.SetChar( nPos-1, cMarkSeperator );
        }
        aImplicitMarks.Insert( new String( aURL ) );
    }
    else if( sCmp.EqualsAscii( pMarkToOutline ) )
    {
        // Hier brauchen wir Position und Name. Deshalb sortieren wir
        // ein USHORT und ein String-Array selbst
        String aOutline( aURL.Copy( 0, nPos-1 ) );
        SwPosition aPos( *pCurPam->GetPoint() );
        if( pDoc->GotoOutline( aPos, aOutline ) )
        {
            ULONG nIdx = aPos.nNode.GetIndex();

            USHORT nIns=0;
            while( nIns < aOutlineMarkPoss.Count() &&
                   aOutlineMarkPoss[nIns] < nIdx )
                nIns++;

            aOutlineMarkPoss.Insert( nIdx, nIns );
            if( bEncoded )
            {
                aURL.Erase( nPos, 2 );
                aURL.SetChar( nPos-1, cMarkSeperator );
            }
            aOutlineMarks.Insert( new String( aURL ), nIns );
        }
    }
    else if( sCmp.EqualsAscii( pMarkToText ) )
    {
        //
    }
}

void SwHTMLWriter::CollectLinkTargets()
{
    const SwFmtINetFmt* pINetFmt;
    const SwTxtINetFmt* pTxtAttr;
    const SwTxtNode* pTxtNd;

    USHORT n, nMaxItems = pDoc->GetAttrPool().GetItemCount( RES_TXTATR_INETFMT );
    for( n = 0; n < nMaxItems; ++n )
    {
        if( 0 != (pINetFmt = (SwFmtINetFmt*)pDoc->GetAttrPool().GetItem(
            RES_TXTATR_INETFMT, n ) ) &&
            0 != ( pTxtAttr = pINetFmt->GetTxtINetFmt()) &&
            0 != ( pTxtNd = pTxtAttr->GetpTxtNode() ) &&
            pTxtNd->GetNodes().IsDocNodes() )
        {
            AddLinkTarget( pINetFmt->GetValue() );
        }
    }

    const SwFmtURL *pURL;
    nMaxItems = pDoc->GetAttrPool().GetItemCount( RES_URL );
    for( n = 0; n < nMaxItems; ++n )
    {
        if( 0 != (pURL = (SwFmtURL*)pDoc->GetAttrPool().GetItem(
            RES_URL, n ) ) )
        {
            AddLinkTarget( pURL->GetURL() );
            const ImageMap *pIMap = pURL->GetMap();
            if( pIMap )
            {
                for( USHORT i=0; i<pIMap->GetIMapObjectCount(); i++ )
                {
                    const IMapObject* pObj = pIMap->GetIMapObject( i );
                    if( pObj )
                    {
                        AddLinkTarget( pObj->GetURL() );
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------

SwHTMLPosFlyFrm::SwHTMLPosFlyFrm( const SwPosFlyFrm& rPosFly,
                                  const SdrObject *pSdrObj,
                                  BYTE nOutMode ) :
    pFrmFmt( &rPosFly.GetFmt() ),
    pSdrObject( pSdrObj ),
    pNdIdx( new SwNodeIndex( rPosFly.GetNdIndex() ) ),
    nOrdNum( rPosFly.GetOrdNum() ),
    nCntntIdx( 0 ),
    nOutputMode( nOutMode )
{
    const SwFmtAnchor& rAnchor = rPosFly.GetFmt().GetAnchor();
    if( FLY_AUTO_CNTNT==rAnchor.GetAnchorId() &&
        HTML_POS_INSIDE == GetOutPos() )
    {
        // Auto-gebundene Rahmen werden ein Zeichen weiter hinten
        // ausgegeben, weil dann die Positionierung mit Netscape
        // uebereinstimmt.
        ASSERT( rAnchor.GetCntntAnchor(), "Keine Anker-Position?" );
        if( rAnchor.GetCntntAnchor() )
        {
            nCntntIdx = rAnchor.GetCntntAnchor()->nContent.GetIndex();
            sal_Int16 eHoriRel = rPosFly.GetFmt().GetHoriOrient().
                                                GetRelationOrient();
            if( text::RelOrientation::FRAME == eHoriRel || text::RelOrientation::PRINT_AREA == eHoriRel )
            {
                const SwCntntNode *pCNd = pNdIdx->GetNode().GetCntntNode();
                ASSERT( pCNd, "Kein Content-Node an PaM-Position" );
                if( pCNd && nCntntIdx < pCNd->Len() )
                    nCntntIdx++;
            }
        }
    }
}

BOOL SwHTMLPosFlyFrm::operator<( const SwHTMLPosFlyFrm& rFrm ) const
{
    if( pNdIdx->GetIndex() == rFrm.pNdIdx->GetIndex() )
    {
        if( nCntntIdx == rFrm.nCntntIdx )
        {
            if( GetOutPos() == rFrm.GetOutPos() )
                return nOrdNum < rFrm.nOrdNum;
            else
                return GetOutPos() < rFrm.GetOutPos();
        }
        else
            return nCntntIdx < rFrm.nCntntIdx;
    }
    else
        return pNdIdx->GetIndex() < rFrm.pNdIdx->GetIndex();
}


