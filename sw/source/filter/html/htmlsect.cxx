/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: htmlsect.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:45:39 $
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

#include "hintids.hxx"

#ifndef _APP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#ifndef _SVX_ADJITEM_HXX //autogen
#include <svx/adjitem.hxx>
#endif
#ifndef _SVX_ULSPITEM_HXX //autogen
#include <svx/ulspitem.hxx>
#endif
#ifndef _SVX_BRKITEM_HXX //autogen
#include <svx/brkitem.hxx>
#endif
#ifndef _HTMLTOKN_H
#include <svtools/htmltokn.h>
#endif
#ifndef _HTMLKYWD_H
#include <svtools/htmlkywd.hxx>
#endif
#ifndef _LINKMGR_HXX //autogen
#include <sfx2/linkmgr.hxx>
#endif
#ifndef _RTL_URI_HXX_
#include <rtl/uri.hxx>
#endif

#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
#ifndef _FMTHDFT_HXX //autogen
#include <fmthdft.hxx>
#endif
#ifndef _FMTCNTNT_HXX //autogen
#include <fmtcntnt.hxx>
#endif
#ifndef _FMTFSIZE_HXX //autogen
#include <fmtfsize.hxx>
#endif
#ifndef _FMTCLDS_HXX //autogen
#include <fmtclds.hxx>
#endif
#ifndef _FMTANCHR_HXX //autogen
#include <fmtanchr.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _FMTSRND_HXX //autogen
#include <fmtsrnd.hxx>
#endif
#ifndef _FMTFLCNT_HXX //autogen
#include <fmtflcnt.hxx>
#endif
#ifndef _FRMATR_HXX
#include "frmatr.hxx"
#endif
#include "doc.hxx"
#include "pam.hxx"
#include "ndtxt.hxx"
#include "shellio.hxx"
#include "section.hxx"
#include "poolfmt.hxx"
#include "pagedesc.hxx"
#include "swtable.hxx"
#include "viewsh.hxx"
#include "swcss1.hxx"
#include "swhtml.hxx"
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif

#define CONTEXT_FLAGS_MULTICOL (HTML_CNTXT_STRIP_PARA |  \
                                HTML_CNTXT_KEEP_NUMRULE | \
                                HTML_CNTXT_KEEP_ATTRS)
//#define CONTEXT_FLAGS_HDRFTR (HTML_CNTXT_STRIP_PARA|HTML_CNTXT_PROTECT_STACK)
#define CONTEXT_FLAGS_HDRFTR (CONTEXT_FLAGS_MULTICOL)
#define CONTEXT_FLAGS_FTN (CONTEXT_FLAGS_MULTICOL)

/*  */

void SwHTMLParser::NewDivision( int nToken )
{
    String aId, aHRef, aStyle, aClass, aLang, aDir;
    SvxAdjust eAdjust = HTML_CENTER_ON==nToken ? SVX_ADJUST_CENTER
                                               : SVX_ADJUST_END;

    sal_Bool bHeader=sal_False, bFooter=sal_False;
    const HTMLOptions *pOptions = GetOptions();
    for( sal_uInt16 i = pOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pOptions)[--i];
        switch( pOption->GetToken() )
        {
        case HTML_O_ID:
            aId = pOption->GetString();
            break;
        case HTML_O_ALIGN:
            if( HTML_DIVISION_ON==nToken )
                eAdjust = (SvxAdjust)pOption->GetEnum( aHTMLPAlignTable,
                                                        eAdjust );
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
        case HTML_O_HREF:
            aHRef =  pOption->GetString();
            break;
        case HTML_O_TYPE:
            {
                const String& rType = pOption->GetString();
                if( rType.EqualsIgnoreCaseAscii( "HEADER" ) )
                    bHeader = sal_True;
                else if( rType.EqualsIgnoreCaseAscii( "FOOTER" ) )
                    bFooter = sal_True;
            }
        }
    }

    sal_Bool bAppended = sal_False;
    if( pPam->GetPoint()->nContent.GetIndex() )
    {
        AppendTxtNode( bHeader||bFooter||aId.Len()||aHRef.Len() ? AM_NORMAL
                                                                : AM_NOSPACE );
        bAppended = sal_True;
    }

    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( nToken );

    sal_Bool bStyleParsed = sal_False, bPositioned = sal_False;
    SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aPropInfo;
    if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
    {
        bStyleParsed = ParseStyleOptions( aStyle, aId, aClass,
                                          aItemSet, aPropInfo, &aLang, &aDir );
        if( bStyleParsed )
        {
            bPositioned = HTML_DIVISION_ON == nToken && aClass.Len() &&
                          CreateContainer( aClass, aItemSet, aPropInfo,
                                           pCntxt );
            if( !bPositioned )
                bPositioned = DoPositioning( aItemSet, aPropInfo, pCntxt );
        }
    }

    if( !bPositioned && (bHeader || bFooter) && IsNewDoc() )
    {
        SwPageDesc *pPageDesc =	pCSS1Parser->GetMasterPageDesc();
        SwFrmFmt& rPageFmt = pPageDesc->GetMaster();

        SwFrmFmt *pHdFtFmt;
        sal_Bool bNew = sal_False;
        sal_uInt16 nFlags = CONTEXT_FLAGS_HDRFTR;
        if( bHeader )
        {
            pHdFtFmt = (SwFrmFmt*)rPageFmt.GetHeader().GetHeaderFmt();
            if( !pHdFtFmt )
            {
                // noch keine Header, dann erzeuge einen.
                rPageFmt.SetAttr( SwFmtHeader( sal_True ));
                pHdFtFmt = (SwFrmFmt*)rPageFmt.GetHeader().GetHeaderFmt();
                bNew = sal_True;
            }
            nFlags |= HTML_CNTXT_HEADER_DIST;
        }
        else
        {
            pHdFtFmt = (SwFrmFmt*)rPageFmt.GetFooter().GetFooterFmt();
            if( !pHdFtFmt )
            {
                // noch keine Footer, dann erzeuge einen.
                rPageFmt.SetAttr( SwFmtFooter( sal_True ));
                pHdFtFmt = (SwFrmFmt*)rPageFmt.GetFooter().GetFooterFmt();
                bNew = sal_True;
            }
            nFlags |= HTML_CNTXT_FOOTER_DIST;
        }

        const SwFmtCntnt& rFlyCntnt = pHdFtFmt->GetCntnt();
        const SwNodeIndex& rCntntStIdx = *rFlyCntnt.GetCntntIdx();
        SwCntntNode *pCNd;

        if( bNew )
        {
            pCNd = pDoc->GetNodes()[rCntntStIdx.GetIndex()+1]
                       ->GetCntntNode();
        }
        else
        {
            // Einen neuen Node zu Beginn der Section anlegen
            SwNodeIndex aSttIdx( rCntntStIdx, 1 );
            pCNd = pDoc->GetNodes().MakeTxtNode( aSttIdx,
                            pCSS1Parser->GetTxtCollFromPool(RES_POOLCOLL_TEXT));

            // Den bisherigen Inhalt der Section loeschen
            SwPaM aDelPam( aSttIdx );
            aDelPam.SetMark();

            const SwStartNode *pStNd =
                (const SwStartNode *)pDoc->GetNodes()[rCntntStIdx];
            aDelPam.GetPoint()->nNode = pStNd->EndOfSectionIndex() - 1;

            pDoc->DelFullPara( aDelPam );

            // Die Seitenvorlage aktualisieren
            for( sal_uInt16 i=0; i < pDoc->GetPageDescCnt(); i++ )
            {
                if( RES_POOLPAGE_HTML==const_cast<const SwDoc *>(pDoc)
                    ->GetPageDesc(i).GetPoolFmtId() )
                {
                    pDoc->ChgPageDesc( i, *pPageDesc );
                    break;
                }
            }
        }

        SwPosition aNewPos( SwNodeIndex( rCntntStIdx, 1 ), SwIndex( pCNd, 0 ) );
        SaveDocContext( pCntxt, nFlags, &aNewPos );
    }
    else if( !bPositioned && aId.Len() > 9 &&
             ('s' == aId.GetChar(0) || 'S' == aId.GetChar(0) ) &&
             ('d' == aId.GetChar(1) || 'D' == aId.GetChar(1) ) )
    {
        sal_Bool bEndNote = sal_False, bFootNote = sal_False;
        if( aId.CompareIgnoreCaseToAscii( sHTML_sdendnote, 9 ) == COMPARE_EQUAL )
            bEndNote = sal_True;
        else if( aId.CompareIgnoreCaseToAscii( sHTML_sdfootnote, 10 ) == COMPARE_EQUAL )
            bFootNote = sal_True;
        if( bFootNote || bEndNote )
        {
            SwNodeIndex *pSttNdIdx = GetFootEndNoteSection( aId );
            if( pSttNdIdx )
            {
                SwCntntNode *pCNd =
                    pDoc->GetNodes()[pSttNdIdx->GetIndex()+1]->GetCntntNode();
                SwNodeIndex aTmpSwNodeIndex = SwNodeIndex(*pCNd);
                SwPosition aNewPos( aTmpSwNodeIndex, SwIndex( pCNd, 0 ) );
                SaveDocContext( pCntxt, CONTEXT_FLAGS_FTN, &aNewPos );
                aId = aPropInfo.aId = aEmptyStr;
            }
        }
    }

    // Bereiche fuegen wir in Rahmen nur dann ein, wenn der Bereich gelinkt ist.
    if( (aId.Len() && !bPositioned) || aHRef.Len()  )
    {
        // Bereich einfuegen (muss vor dem Setzten von Attributen erfolgen,
        // weil die Section vor der PaM-Position eingefuegt.

        // wenn wir im ersten Node einer Section stehen, wir die neue
        // Section nicht in der aktuellen, sondern vor der aktuellen
        // Section eingefuegt. Deshalb muessen wir dann einen Node
        // einfuegen. UND IN LOESCHEN!!!
        if( !bAppended )
        {
            SwNodeIndex aPrvNdIdx( pPam->GetPoint()->nNode, -1 );
            if( (pDoc->GetNodes()[aPrvNdIdx])->IsSectionNode() )
            {
                AppendTxtNode();
                bAppended = sal_True;
            }
        }
        _HTMLAttrs *pPostIts = bAppended ? 0 : new _HTMLAttrs;
        SetAttr( sal_True, sal_True, pPostIts );

        // Namen der Section eindeutig machen
        String aName( pDoc->GetUniqueSectionName( aId.Len() ? &aId : 0 ) );

        if( aHRef.Len() )
        {
            sal_Unicode cDelim = 255U;
            String aURL;
            xub_StrLen nPos = aHRef.SearchBackward( cDelim );
            xub_StrLen nPos2 = STRING_NOTFOUND;
            if( STRING_NOTFOUND != nPos )
            {
                nPos2 = aHRef.SearchBackward( cDelim, nPos );
                if( STRING_NOTFOUND != nPos2 )
                {
                    xub_StrLen nTmp = nPos;
                    nPos = nPos2;
                    nPos2 = nTmp;
                }
            }
            if( STRING_NOTFOUND == nPos )
            {
                aURL = URIHelper::SmartRel2Abs(INetURLObject( sBaseURL ), aHRef, Link(), false);
            }
            else
            {
                aURL = URIHelper::SmartRel2Abs(INetURLObject( sBaseURL ), aHRef.Copy( 0, nPos ), Link(), false );
                aURL += sfx2::cTokenSeperator;
                if( STRING_NOTFOUND == nPos2 )
                {
                    aURL += aHRef.Copy( nPos+1 );
                }
                else
                {
                    aURL += aHRef.Copy( nPos+1, nPos2 - (nPos+1) );
                    aURL += sfx2::cTokenSeperator;
                    aURL += String(rtl::Uri::decode( aHRef.Copy( nPos2+1 ),
                                              rtl_UriDecodeWithCharset,
                                              RTL_TEXTENCODING_ISO_8859_1 ));
                }
            }
            aHRef = aURL;
        }

        SwSection aSection( aHRef.Len() ? FILE_LINK_SECTION
                                        : CONTENT_SECTION, aName );
        if( aHRef.Len() )
        {
            aSection.SetLinkFileName( aHRef );
            aSection.SetProtect();
        }

        SfxItemSet aFrmItemSet( pDoc->GetAttrPool(),
                                RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
        if( !IsNewDoc() )
            Reader::ResetFrmFmtAttrs(aFrmItemSet );

        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET == aItemSet.GetItemState( RES_BACKGROUND, sal_False,
                                                   &pItem ) )
        {
            aFrmItemSet.Put( *pItem );
            aItemSet.ClearItem( RES_BACKGROUND );
        }
        if( SFX_ITEM_SET == aItemSet.GetItemState( RES_FRAMEDIR, sal_False,
                                                   &pItem ) )
        {
            aFrmItemSet.Put( *pItem );
            aItemSet.ClearItem( RES_FRAMEDIR );
        }

        pDoc->Insert( *pPam, aSection, &aFrmItemSet, sal_False );

        // ggfs. einen Bereich anspringen
        if( JUMPTO_REGION == eJumpTo && aName == sJmpMark )
        {
            bChkJumpMark = sal_True;
            eJumpTo = JUMPTO_NONE;
        }

        SwTxtNode* pOldTxtNd =
            bAppended ? 0 : pDoc->GetNodes()[pPam->GetPoint()->nNode]
                                ->GetTxtNode();

        pPam->Move( fnMoveBackward );

        // PageDesc- und SwFmtBreak Attribute vom aktuellen Node in den
        // (ersten) Node des Bereich verschieben.
        if( pOldTxtNd )
            MovePageDescAttrs( pOldTxtNd, pPam->GetPoint()->nNode.GetIndex(),
                               sal_True	 );

        if( pPostIts )
        {
            // noch vorhandene PostIts in den ersten Absatz
            // der Tabelle setzen
            InsertAttrs( *pPostIts );
            delete pPostIts;
            pPostIts = 0;
        }

        pCntxt->SetSpansSection( sal_True );

        // keine ::com::sun::star::text::Bookmarks mit dem gleichen Namen wie Bereiche einfuegen
        if( aPropInfo.aId.Len() && aPropInfo.aId==aName )
            aPropInfo.aId.Erase();
    }
    else
    {
        pCntxt->SetAppendMode( AM_NOSPACE );
    }

    if( SVX_ADJUST_END != eAdjust )
    {
        InsertAttr( &aAttrTab.pAdjust, SvxAdjustItem(eAdjust), pCntxt );
    }

    // Style parsen
    if( bStyleParsed )
        InsertAttrs( aItemSet, aPropInfo, pCntxt, sal_True );

    PushContext( pCntxt );
}

void SwHTMLParser::EndDivision( int nToken )
{
    // Stack-Eintrag zu dem Token suchen (weil wir noch den Div-Stack
    // haben unterscheiden wir erst einmal nicht zwischen DIV und CENTER
    _HTMLAttrContext *pCntxt = 0;
    sal_uInt16 nPos = aContexts.Count();
    while( !pCntxt && nPos>nContextStMin )
    {
        switch( aContexts[--nPos]->GetToken() )
        {
        case HTML_CENTER_ON:
        case HTML_DIVISION_ON:
            pCntxt = aContexts[nPos];
            aContexts.Remove( nPos, 1 );
            break;
        }
    }

    if( pCntxt )
    {
        // Attribute beenden
        EndContext( pCntxt );
        SetAttr();	// Absatz-Atts wegen JavaScript moeglichst schnell setzen

        delete pCntxt;
    }
}

void SwHTMLParser::FixHeaderFooterDistance( sal_Bool bHeader,
                                            const SwPosition *pOldPos )
{
    SwPageDesc *pPageDesc =	pCSS1Parser->GetMasterPageDesc();
    SwFrmFmt& rPageFmt = pPageDesc->GetMaster();

    SwFrmFmt *pHdFtFmt =
        bHeader ? (SwFrmFmt*)rPageFmt.GetHeader().GetHeaderFmt()
                : (SwFrmFmt*)rPageFmt.GetFooter().GetFooterFmt();
    ASSERT( pHdFtFmt, "Doch keine Kopf- oder Fusszeile" );

    const SwFmtCntnt& rFlyCntnt = pHdFtFmt->GetCntnt();
    const SwNodeIndex& rCntntStIdx = *rFlyCntnt.GetCntntIdx();

    sal_uInt32 nPrvNxtIdx;
    if( bHeader )
    {
        nPrvNxtIdx = pDoc->GetNodes()[rCntntStIdx]->EndOfSectionIndex()-1;
    }
    else
    {
        nPrvNxtIdx = pOldPos->nNode.GetIndex() - 1;
    }

    sal_uInt16 nSpace = 0;
    SwTxtNode *pTxtNode = pDoc->GetNodes()[nPrvNxtIdx]->GetTxtNode();
    if( pTxtNode )
    {
        const SvxULSpaceItem& rULSpace =
            ((const SvxULSpaceItem&)pTxtNode
                ->SwCntntNode::GetAttr( RES_UL_SPACE ));

        // Der untere Absatz-Abstand wird zum Abstand zur
        // Kopf- oder Fusszeile
        nSpace = rULSpace.GetLower();

        // und anschliessend auf einen vernuenftigen Wert
        // gesetzt
        const SvxULSpaceItem& rCollULSpace =
            pTxtNode->GetAnyFmtColl().GetULSpace();
        if( rCollULSpace.GetUpper() == rULSpace.GetUpper() )
            pTxtNode->ResetAttr( RES_UL_SPACE );
        else
            pTxtNode->SwCntntNode::SetAttr(
                SvxULSpaceItem( rULSpace.GetUpper(),
                                rCollULSpace.GetLower() ) );
    }

    if( bHeader )
    {
        nPrvNxtIdx = pOldPos->nNode.GetIndex();
    }
    else
    {
        nPrvNxtIdx = rCntntStIdx.GetIndex() + 1;
    }

    pTxtNode = pDoc->GetNodes()[nPrvNxtIdx]
                    ->GetTxtNode();
    if( pTxtNode )
    {
        const SvxULSpaceItem& rULSpace =
            ((const SvxULSpaceItem&)pTxtNode
                ->SwCntntNode::GetAttr( RES_UL_SPACE ));

        // Der obere Absatz-Abstand wird zum Abstand zur
        // Kopf- oder Fusszeile, wenn er groesser ist als
        // der untere vom Absatz davor
        if( rULSpace.GetUpper() > nSpace )
            nSpace = rULSpace.GetUpper();

        // und anschliessend auf einen vernuenftigen Wert gesetzt
        const SvxULSpaceItem& rCollULSpace =
            pTxtNode->GetAnyFmtColl().GetULSpace();
        if( rCollULSpace.GetLower() == rULSpace.GetLower() )
            pTxtNode->ResetAttr( RES_UL_SPACE );
        else
            pTxtNode->SwCntntNode::SetAttr(
                SvxULSpaceItem( rCollULSpace.GetUpper(),
                                rULSpace.GetLower() ) );
    }

    SvxULSpaceItem aULSpace;
    if( bHeader )
        aULSpace.SetLower( nSpace );
    else
        aULSpace.SetUpper( nSpace );

    pHdFtFmt->SetAttr( aULSpace );
}

sal_Bool SwHTMLParser::EndSection( sal_Bool bLFStripped )
{
    SwEndNode *pEndNd = pDoc->GetNodes()[pPam->GetPoint()->nNode.GetIndex()+1]
                            ->GetEndNode();
    if( pEndNd && pEndNd->FindStartNode()->IsSectionNode() )
    {
        // den Bereich beenden
        if( !bLFStripped )
            StripTrailingPara();
        pPam->Move( fnMoveForward );
        return sal_True;
    }

    ASSERT( !this, "Falsche PaM Position Beenden eines Bereichs" );

    return sal_False;
}

sal_Bool SwHTMLParser::EndSections( sal_Bool bLFStripped )
{
    sal_Bool bSectionClosed = sal_False;
    sal_uInt16 nPos = aContexts.Count();
    while( nPos>nContextStMin )
    {
        _HTMLAttrContext *pCntxt = aContexts[--nPos];
        if( pCntxt->GetSpansSection() && EndSection( bLFStripped ) )
        {
            bSectionClosed = sal_True;
            pCntxt->SetSpansSection( sal_False );
            bLFStripped = sal_False;
        }
    }

    return bSectionClosed;
}

/*  */

void SwHTMLParser::NewMultiCol()
{
    String aId, aStyle, aClass, aLang, aDir;
    long nWidth = 100;
    sal_uInt16 nCols = 0, nGutter = 10;
    sal_Bool bPrcWidth = sal_True;

    const HTMLOptions *pOptions = GetOptions();
    sal_uInt16 i;

    for( i = pOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pOptions)[--i];
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
        case HTML_O_LANG:
            aLang = pOption->GetString();
            break;
        case HTML_O_DIR:
            aDir = pOption->GetString();
            break;
        case HTML_O_COLS:
            nCols = (sal_uInt16)pOption->GetNumber();
            break;
        case HTML_O_WIDTH:
            nWidth = pOption->GetNumber();
            bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
            if( bPrcWidth && nWidth>100 )
                nWidth = 100;
            break;
        case HTML_O_GUTTER:
            nGutter = (sal_uInt16)pOption->GetNumber();
            break;

        }
    }

    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( HTML_MULTICOL_ON );

    //.is the multicol elememt contained in a container? That may be the
    // case for 5.0 documents.
    sal_Bool bInCntnr = sal_False;
    i = aContexts.Count();
    while( !bInCntnr && i > nContextStMin )
        bInCntnr = 0 != aContexts[--i]->GetFrmItemSet();

    // Parse style sheets, but don't position anything by now.
    sal_Bool bStyleParsed = sal_False;
    SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aPropInfo;
    if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
        bStyleParsed = ParseStyleOptions( aStyle, aId, aClass,
                                          aItemSet, aPropInfo, &aLang, &aDir );

    // Calculate width.
    sal_uInt8 nPrcWidth = bPrcWidth ? (sal_uInt8)nWidth : 0;
    sal_uInt16 nTwipWidth = 0;
    if( !bPrcWidth && nWidth && Application::GetDefaultDevice() )
    {
        nTwipWidth = (sal_uInt16)Application::GetDefaultDevice()
                             ->PixelToLogic( Size(nWidth, 0),
                                             MapMode(MAP_TWIP) ).Width();
    }

    if( !nPrcWidth && nTwipWidth < MINFLY )
        nTwipWidth = MINFLY;

    // Do positioning.
    sal_Bool bPositioned = sal_False;
    if( bInCntnr || pCSS1Parser->MayBePositioned( aPropInfo, sal_True ) )
    {
        SfxItemSet aFrmItemSet( pDoc->GetAttrPool(),
                                RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
        if( !IsNewDoc() )
            Reader::ResetFrmFmtAttrs(aFrmItemSet );

        SetAnchorAndAdjustment( VERT_NONE, HORI_NONE, aItemSet, aPropInfo,
                                aFrmItemSet );

        // The width is either the WIDTH attribute's value or contained
        // in some style option.
        SetVarSize( aItemSet, aPropInfo, aFrmItemSet, nTwipWidth, nPrcWidth );

        SetSpace( Size(0,0), aItemSet, aPropInfo, aFrmItemSet );

        // Set some other frame attributes. If the background is set, its
        // it will be cleared here. That for, it won't be set at the section,
        // too.
        SetFrmFmtAttrs( aItemSet, aPropInfo,
                        HTML_FF_BOX|HTML_FF_BACKGROUND|HTML_FF_PADDING|HTML_FF_DIRECTION,
                        aFrmItemSet );

        // Insert fly frame. If the are columns, the fly frame's name is not
        // the sections name but a generated one.
        String aFlyName( aEmptyStr );
        if( nCols < 2 )
        {
            aFlyName = aId;
            aPropInfo.aId.Erase();
        }

        InsertFlyFrame( aFrmItemSet, pCntxt, aFlyName, CONTEXT_FLAGS_ABSPOS );

        pCntxt->SetPopStack( sal_True );
        bPositioned = sal_True;
    }

    sal_Bool bAppended = sal_False;
    if( !bPositioned )
    {
        if( pPam->GetPoint()->nContent.GetIndex() )
        {
            AppendTxtNode( AM_SPACE );
            bAppended = sal_True;
        }
        else
        {
            AddParSpace();
        }
    }

    // If there are less then 2 columns, no section is inserted.
    if( nCols >= 2 )
    {
        if( !bAppended )
        {
            // If the pam is at the start of a section, a additional text
            // node must be inserted. Otherwise, the new section will be
            // inserted in front of the old one.
            SwNodeIndex aPrvNdIdx( pPam->GetPoint()->nNode, -1 );
            if( (pDoc->GetNodes()[aPrvNdIdx])->IsSectionNode() )
            {
                AppendTxtNode();
                bAppended = sal_True;
            }
        }
        _HTMLAttrs *pPostIts = bAppended ? 0 : new _HTMLAttrs;
        SetAttr( sal_True, sal_True, pPostIts );

        // Make section name unique.
        String aName( pDoc->GetUniqueSectionName( aId.Len() ? &aId : 0 ) );
        SwSection aSection( CONTENT_SECTION, aName );

        SfxItemSet aFrmItemSet( pDoc->GetAttrPool(),
                                RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
        if( !IsNewDoc() )
            Reader::ResetFrmFmtAttrs(aFrmItemSet );

        if( nGutter && Application::GetDefaultDevice() )
        {
            nGutter = (sal_uInt16)Application::GetDefaultDevice()
                             ->PixelToLogic( Size(nGutter, 0),
                                             MapMode(MAP_TWIP) ).Width();
        }

        SwFmtCol aFmtCol;
#ifndef WIDTH_SUPPORTED_BY_SECTIONS
        nPrcWidth = 100;
#endif

        aFmtCol.Init( nCols, nGutter, nPrcWidth ? USHRT_MAX : nTwipWidth );
        aFrmItemSet.Put( aFmtCol );

        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET == aItemSet.GetItemState( RES_BACKGROUND, sal_False,
                                                   &pItem ) )
        {
            aFrmItemSet.Put( *pItem );
            aItemSet.ClearItem( RES_BACKGROUND );
        }
        if( SFX_ITEM_SET == aItemSet.GetItemState( RES_FRAMEDIR, sal_False,
                                                   &pItem ) )
        {
            aFrmItemSet.Put( *pItem );
            aItemSet.ClearItem( RES_FRAMEDIR );
        }
        pDoc->Insert( *pPam, aSection, &aFrmItemSet, sal_False );

        // Jump to section, if this is requested.
        if( JUMPTO_REGION == eJumpTo && aName == sJmpMark )
        {
            bChkJumpMark = sal_True;
            eJumpTo = JUMPTO_NONE;
        }

        SwTxtNode* pOldTxtNd =
            bAppended ? 0 : pDoc->GetNodes()[pPam->GetPoint()->nNode]
                                ->GetTxtNode();

        pPam->Move( fnMoveBackward );

        // Move PageDesc and SwFmtBreak attributes of the current node
        // to the section's first node.
        if( pOldTxtNd )
            MovePageDescAttrs( pOldTxtNd, pPam->GetPoint()->nNode.GetIndex(),
                               sal_True	 );

        if( pPostIts )
        {
            // Move pending PostIts into the section.
            InsertAttrs( *pPostIts );
            delete pPostIts;
            pPostIts = 0;
        }

        pCntxt->SetSpansSection( sal_True );

        // Insert a bookmark if its name differs from the section's name only.
        if( aPropInfo.aId.Len() && aPropInfo.aId==aName )
            aPropInfo.aId.Erase();
    }

    // Additional attributes must be set as hard ones.
    if( bStyleParsed )
        InsertAttrs( aItemSet, aPropInfo, pCntxt, sal_True );

    PushContext( pCntxt );
}

/*  */

void SwHTMLParser::InsertFlyFrame( const SfxItemSet& rItemSet,
                                   _HTMLAttrContext *pCntxt,
                                   const String& rName,
                                   sal_uInt16 nFlags )
{
    RndStdIds eAnchorId =
        ((const SwFmtAnchor&)rItemSet.Get( RES_ANCHOR )).GetAnchorId();

    // Den Rahmen anlegen
    SwFlyFrmFmt* pFlyFmt = pDoc->MakeFlySection( eAnchorId, pPam->GetPoint(),
                                                    &rItemSet );
    // Ggf. den Namen setzen
    if( rName.Len() )
        pFlyFmt->SetName( rName );

    RegisterFlyFrm( pFlyFmt );

    const SwFmtCntnt& rFlyCntnt = pFlyFmt->GetCntnt();
    const SwNodeIndex& rFlyCntIdx = *rFlyCntnt.GetCntntIdx();
    SwCntntNode *pCNd = pDoc->GetNodes()[rFlyCntIdx.GetIndex()+1]
                            ->GetCntntNode();

    SwPosition aNewPos( SwNodeIndex( rFlyCntIdx, 1 ), SwIndex( pCNd, 0 ) );
    SaveDocContext( pCntxt, nFlags, &aNewPos );
}


/*  */

void SwHTMLParser::MovePageDescAttrs( SwNode *pSrcNd,
                                      sal_uInt32 nDestIdx,
                                      sal_Bool bFmtBreak )
{
    SwCntntNode* pDestCntntNd =
        pDoc->GetNodes()[nDestIdx]->GetCntntNode();

    ASSERT( pDestCntntNd, "Wieso ist das Ziel kein Content-Node?" );

    if( pSrcNd->IsCntntNode() )
    {
        SwCntntNode* pSrcCntntNd = pSrcNd->GetCntntNode();

        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET ==	pSrcCntntNd->GetSwAttrSet()
                .GetItemState( RES_PAGEDESC, sal_False, &pItem ) &&
            ((SwFmtPageDesc *)pItem)->GetPageDesc() )
        {
            pDestCntntNd->SetAttr( *pItem );
            pSrcCntntNd->ResetAttr( RES_PAGEDESC );
        }
        if( SFX_ITEM_SET ==	pSrcCntntNd->GetSwAttrSet()
                .GetItemState( RES_BREAK, sal_False, &pItem ) )
        {
            switch( ((SvxFmtBreakItem *)pItem)->GetBreak() )
            {
            case SVX_BREAK_PAGE_BEFORE:
            case SVX_BREAK_PAGE_AFTER:
            case SVX_BREAK_PAGE_BOTH:
                if( bFmtBreak )
                    pDestCntntNd->SetAttr( *pItem );
                pSrcCntntNd->ResetAttr( RES_BREAK );
            }
        }
    }
    else if( pSrcNd->IsTableNode() )
    {
        SwFrmFmt *pFrmFmt = pSrcNd->GetTableNode()->GetTable().GetFrmFmt();

        const SfxPoolItem* pItem;
        if( SFX_ITEM_SET ==	pFrmFmt->GetAttrSet().
                GetItemState( RES_PAGEDESC, sal_False, &pItem ) )
        {
            pDestCntntNd->SetAttr( *pItem );
            pFrmFmt->ResetAttr( RES_PAGEDESC );
        }
    }
}

