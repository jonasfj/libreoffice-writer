/*************************************************************************
 *
 *  $RCSfile: redlnitr.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: fme $ $Date: 2001-12-14 12:12:05 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#include "hintids.hxx"

#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif
#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif

#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>		// SwRedline
#endif
#ifndef _TXTATR_HXX
#include <txtatr.hxx>		// SwTxt ...
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>		// SwRedlineTbl
#endif
#ifndef _ITRATR_HXX
#include <itratr.hxx>		// SwAttrIter
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>		// SwTxtNode
#endif
#ifndef _SWFNTCCH_HXX
#include <swfntcch.hxx>		// SwFontAccess
#endif
#ifndef _DOC_HXX
#include <doc.hxx>			// SwDoc
#endif
#ifndef _TXATBASE_HXX
#include <txatbase.hxx>		// SwTxtAttr
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>
#endif
#ifndef _FRMSH_HXX
#include <frmsh.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _ATRHNDL_HXX
#include <atrhndl.hxx>
#endif

//////////////////////////

#ifndef _SV_KEYCODES_HXX //autogen
#include <vcl/keycodes.hxx>
#endif
#ifndef _VCL_CMDEVT_HXX //autogen
#include <vcl/cmdevt.hxx>
#endif

#ifndef _SV_SETTINGS_HXX //autogen
#include <vcl/settings.hxx>
#endif

#ifndef _TXTFRM_HXX
#include <txtfrm.hxx>		// SwTxtFrm
#endif
#ifndef _APP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _REDLNITR_HXX
#include <redlnitr.hxx>
#endif
#ifndef _EXTINPUT_HXX
#include <extinput.hxx>
#endif

#ifndef _SFX_PRINTER_HXX //autogen
#include <sfx2/printer.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>	// ViewShell
#endif

using namespace ::com::sun::star;

extern BYTE WhichFont( xub_StrLen nIdx, const String* pTxt,
                       const SwScriptInfo* pSI );

/*************************************************************************
 *						SwAttrIter::CtorInit()
 *************************************************************************/
#ifdef VERTICAL_LAYOUT
void SwAttrIter::CtorInit( SwTxtNode& rTxtNode, SwScriptInfo& rScrInf, SwTxtFrm* pFrm )
#else
void SwAttrIter::CtorInit( SwTxtNode& rTxtNode, SwScriptInfo& rScrInf )
#endif
{
    // Beim HTML-Import kann es vorkommen, dass kein Layout existiert.
    SwRootFrm *pRootFrm = rTxtNode.GetDoc()->GetRootFrm();
    pShell = pRootFrm ? pRootFrm->GetShell() : 0;

    // get output device for later font checking for weak characters
    OutputDevice* pOut = rTxtNode.GetDoc()->GetPrt();
    if( !pOut || !((Printer*)pOut)->IsValid() )
    {
        if( pShell )
        {
            if( 0 == ( pOut = pShell->GetReferenzDevice() ) )
                pOut = pShell->GetWin();
        }
        if( !pOut )
            pOut = GetpApp()->GetDefaultDevice();
    }

    pScriptInfo = &rScrInf;
    pAttrSet = &rTxtNode.GetSwAttrSet();
    pHints = rTxtNode.GetpSwpHints();

#ifndef VERTICAL_LAYOUT
    const sal_Bool bAttrSet = rTxtNode.HasSwAttrSet();
#endif

    SwFontAccess aFontAccess( &rTxtNode.GetAnyFmtColl(), pShell );

    delete pFnt;
    pFnt = new SwFont( *aFontAccess.Get()->GetFont() );

#ifdef VERTICAL_LAYOUT
    // set font to vertical if frame layout is vertical
    sal_Bool bVertLayout = sal_False;
    if ( pFrm && pFrm->IsVertical() )
    {
        bVertLayout = sal_True;
        pFnt->SetVertical( pFnt->GetOrientation(), sal_True );
    }
#endif

    // Initialize the default attribute of the attribute handler
    // based on the attribute array cached together with the font.
    // If any further attributes for the paragraph are given in pAttrSet
    // consider them during construction of the default array, and apply
    // them to the font
#ifdef VERTICAL_LAYOUT
    if ( rTxtNode.HasSwAttrSet() )
        aAttrHandler.Init( aFontAccess.Get()->GetDefault(), pAttrSet,
                           *rTxtNode.GetDoc(), *pFnt, bVertLayout );
    else
        aAttrHandler.Init( aFontAccess.Get()->GetDefault(), 0,
                           *rTxtNode.GetDoc(), *pFnt, bVertLayout );
#else
    aAttrHandler.Init( aFontAccess.Get()->GetDefault(), *pAttrSet,
                       *rTxtNode.GetDoc(), *pFnt, bAttrSet );
#endif

    aMagicNo[SW_LATIN] = aMagicNo[SW_CJK] = aMagicNo[SW_CTL] = NULL;

    // determine script changes if not already done for current paragraph
    ASSERT( pScriptInfo, "No script info available");
    if ( pScriptInfo->GetInvalidity() != STRING_LEN )
         pScriptInfo->InitScriptInfo( rTxtNode, aAttrHandler, *pOut );

    if ( pBreakIt->xBreak.is() )
    {
        pFnt->SetActual( WhichFont( 0, 0, pScriptInfo ) );

        xub_StrLen nChg = 0;
        USHORT nCnt = 0;

        do
        {
            nChg = pScriptInfo->GetScriptChg( nCnt );
            USHORT nScript = pScriptInfo->GetScriptType( nCnt++ );
            BYTE nTmp = 4;
            switch ( nScript ) {
                case i18n::ScriptType::ASIAN :
                    if( !aMagicNo[SW_CJK] ) nTmp = SW_CJK; break;
                case i18n::ScriptType::COMPLEX :
                    if( !aMagicNo[SW_CTL] ) nTmp = SW_CTL; break;
                default:
                    if( !aMagicNo[SW_LATIN ] ) nTmp = SW_LATIN;
            }
            if( nTmp < 4 )
            {
                pFnt->ChkMagic( pShell, nTmp );
                pFnt->GetMagic( aMagicNo[ nTmp ], aFntIdx[ nTmp ], nTmp );
            }
        } while( nChg < rTxtNode.GetTxt().Len() );
    }
    else
    {
        pFnt->ChkMagic( pShell, SW_LATIN );
        pFnt->GetMagic( aMagicNo[ SW_LATIN ], aFntIdx[ SW_LATIN ], SW_LATIN );
    }

    nStartIndex = nEndIndex = nPos = nChgCnt = 0;
    nPropFont = 0;
    SwDoc* pDoc = rTxtNode.GetDoc();

    const SwExtTextInput* pExtInp = pDoc->GetExtTextInput( rTxtNode );
    sal_Bool bShow = ::IsShowChanges( pDoc->GetRedlineMode() );
    if( pExtInp || bShow )
    {
        MSHORT nRedlPos = pDoc->GetRedlinePos( rTxtNode );
        if( pExtInp || MSHRT_MAX != nRedlPos )
        {
            const SvUShorts* pArr = 0;
            xub_StrLen nInputStt = 0;
            if( pExtInp )
            {
                pArr = &pExtInp->GetAttrs();
                nInputStt = pExtInp->Start()->nContent.GetIndex();
            }

            pRedln = new SwRedlineItr( rTxtNode, *pFnt, aAttrHandler, nRedlPos,
                                        bShow, pArr, nInputStt );

            if( pRedln->IsOn() )
                ++nChgCnt;
        }
    }
}

/*************************************************************************
 * SwRedlineItr - Der Redline-Iterator
 *
 * Folgende Informationen/Zustaende gibt es im RedlineIterator:
 *
 * nFirst ist der erste Index der RedlineTbl, der mit dem Absatz ueberlappt.
 *
 * nAct ist der zur Zeit aktive ( wenn bOn gesetzt ist ) oder der naechste
 * in Frage kommende Index.
 * nStart und nEnd geben die Grenzen des Objekts innerhalb des Absatzes an.
 *
 * Wenn bOn gesetzt ist, ist der Font entsprechend manipuliert worden.
 *
 * Wenn nAct auf MSHRT_MAX gesetzt wurde ( durch Reset() ), so ist zur Zeit
 * kein Redline aktiv, nStart und nEnd sind invalid.
 *************************************************************************/

SwRedlineItr::SwRedlineItr( const SwTxtNode& rTxtNd, SwFont& rFnt,
    SwAttrHandler& rAH, MSHORT nRed, sal_Bool bShw, const SvUShorts *pArr,
    xub_StrLen nExtStart )
    : rDoc( *rTxtNd.GetDoc() ), rNd( rTxtNd ), rAttrHandler( rAH ),
      nNdIdx( rTxtNd.GetIndex() ), nFirst( nRed ),
      nAct( MSHRT_MAX ), bOn( sal_False ), pSet(0), bShow( bShw )
{
    if( pArr )
        pExt = new SwExtend( *pArr, nExtStart );
    else
        pExt = NULL;
    Seek( rFnt, 0, STRING_LEN );
}

SwRedlineItr::~SwRedlineItr()
{
    Clear( NULL );
    delete pSet;
    delete pExt;
}

// Der Return-Wert von SwRedlineItr::Seek gibt an, ob der aktuelle Font
// veraendert wurde durch Verlassen (-1) oder Betreten eines Bereichs (+1)

short SwRedlineItr::_Seek( SwFont& rFnt, xub_StrLen nNew, xub_StrLen nOld )
{
    short nRet = 0;
    if( ExtOn() )
        return 0; // Abkuerzung: wenn wir innerhalb eines ExtendTextInputs sind
            // kann es keine anderen Attributwechsel (auch nicht durch Redlining) geben
    if( bShow )
    {
        if( bOn )
        {
            if( nNew >= nEnd )
            {
                --nRet;
                _Clear( &rFnt );	// Wir gehen hinter den aktuellen Bereich
                ++nAct;		   		// und pruefen gleich den naechsten
            }
            else if( nNew < nStart )
            {
                --nRet;
                _Clear( &rFnt );	// Wir gehen vor den aktuellen Bereich
                if( nAct > nFirst )
                    nAct = nFirst;	// Die Pruefung muss von vorne beginnen
                else
                    return nRet + EnterExtend( rFnt, nNew ); // Es gibt keinen vor uns.
            }
            else
                return nRet + EnterExtend( rFnt, nNew ); // Wir sind im gleichen Bereich geblieben.
        }
        if( MSHRT_MAX == nAct || nOld > nNew )
            nAct = nFirst;

        nStart = STRING_LEN;
        nEnd = STRING_LEN;

        for( ; nAct < rDoc.GetRedlineTbl().Count() ; ++nAct )
        {
            rDoc.GetRedlineTbl()[ nAct ]->CalcStartEnd( nNdIdx, nStart, nEnd );

            if( nNew < nEnd )
            {
                if( nNew >= nStart ) // der einzig moegliche Kandidat
                {
                    bOn = sal_True;
                    const SwRedline *pRed = rDoc.GetRedlineTbl()[ nAct ];

                    if (pSet)
                        pSet->ClearItem();
                    else
                    {
                        SwAttrPool& rPool = (SwAttrPool& )rDoc.GetAttrPool();
                        pSet = new SfxItemSet(rPool, RES_CHRATR_BEGIN, RES_CHRATR_END-1);
                    }

                    if( 1 < pRed->GetStackCount() )
                        FillHints( pRed->GetAuthor( 1 ), pRed->GetType( 1 ) );
                    FillHints( pRed->GetAuthor(), pRed->GetType() );

                    SfxWhichIter aIter( *pSet );
                    register MSHORT nWhich = aIter.FirstWhich();
                    while( nWhich )
                    {
                        const SfxPoolItem* pItem;
                        if( ( nWhich < RES_CHRATR_END ) &&
                            ( SFX_ITEM_SET == pSet->GetItemState( nWhich, sal_True,	&pItem ) ) )
                        {
                            SwTxtAttr* pAttr = ((SwTxtNode&)rNd).MakeTmpTxtAttr(*pItem);
                            pAttr->SetPriorityAttr( sal_True );
                            aHints.C40_INSERT( SwTxtAttr, pAttr, aHints.Count());
                            rAttrHandler.PushAndChg( *pAttr, rFnt );
                            if( RES_CHRATR_COLOR == nWhich )
                                rFnt.SetNoCol( sal_True );
                        }
                        nWhich = aIter.NextWhich();
                    }

                    ++nRet;
                }
                break;
            }
            nStart = STRING_LEN;
            nEnd = STRING_LEN;
        }
    }
    return nRet + EnterExtend( rFnt, nNew );
}

void SwRedlineItr::FillHints( MSHORT nAuthor, SwRedlineType eType )
{
    switch ( eType )
    {
        case REDLINE_INSERT:
            SW_MOD()->GetInsertAuthorAttr(nAuthor, *pSet);
            break;
        case REDLINE_DELETE:
            SW_MOD()->GetDeletedAuthorAttr(nAuthor, *pSet);
            break;
        case REDLINE_FORMAT:
        case REDLINE_FMTCOLL:
            SW_MOD()->GetFormatAuthorAttr(nAuthor, *pSet);
            break;
    }
}

void SwRedlineItr::ChangeTxtAttr( SwFont* pFnt, SwTxtAttr &rHt, sal_Bool bChg )
{
    ASSERT( IsOn(), "SwRedlineItr::ChangeTxtAttr: Off?" );

    if( !bShow && !pExt )
        return;

    if( bChg )
        rAttrHandler.PushAndChg( rHt, *pFnt );
    else
        rAttrHandler.PopAndChg( rHt, *pFnt );
}

void SwRedlineItr::_Clear( SwFont* pFnt )
{
    ASSERT( bOn, "SwRedlineItr::Clear: Off?" );
    bOn = sal_False;
    while( aHints.Count() )
    {
        SwTxtAttr *pPos = aHints[ 0 ];
        aHints.Remove(0);
        if( pFnt )
            rAttrHandler.PopAndChg( *pPos, *pFnt );
        else
            rAttrHandler.Pop( *pPos );
        delete pPos;
    }
    if( pFnt )
        pFnt->SetNoCol( sal_False );
}

xub_StrLen SwRedlineItr::_GetNextRedln( xub_StrLen nNext )
{
    nNext = NextExtend( nNext );
    if( !bShow || MSHRT_MAX == nFirst )
        return nNext;
    if( MSHRT_MAX == nAct )
    {
        nAct = nFirst;
        rDoc.GetRedlineTbl()[ nAct ]->CalcStartEnd( nNdIdx, nStart, nEnd );
    }
    if( bOn || !nStart )
    {
        if( nEnd < nNext )
            nNext = nEnd;
    }
    else if( nStart < nNext )
        nNext = nStart;
    return nNext;
}

sal_Bool SwRedlineItr::_ChkSpecialUnderline() const
{
    // Wenn die Unterstreichung oder das Escapement vom Redling kommt,
    // wenden wir immer das SpecialUnderlining, d.h. die Unterstreichung
    // unter der Grundlinie an.
    for( MSHORT i = 0; i < aHints.Count(); ++i )
    {
        SwTxtAttr *pPos = aHints[ i ];
        MSHORT nWhich = aHints[i]->Which();
        if( RES_CHRATR_UNDERLINE == nWhich ||
            RES_CHRATR_ESCAPEMENT == nWhich )
            return sal_True;
    }
    return sal_False;
}

sal_Bool SwRedlineItr::CheckLine( xub_StrLen nChkStart, xub_StrLen nChkEnd )
{
    if( nFirst == MSHRT_MAX )
        return sal_False;
    if( nChkEnd == nChkStart ) // Leerzeilen gucken ein Zeichen weiter.
        ++nChkEnd;
    xub_StrLen nOldStart = nStart;
    xub_StrLen nOldEnd = nEnd;
    xub_StrLen nOldAct = nAct;
    sal_Bool bRet = sal_False;

    for( nAct = nFirst; nAct < rDoc.GetRedlineTbl().Count() ; ++nAct )
    {
        rDoc.GetRedlineTbl()[ nAct ]->CalcStartEnd( nNdIdx, nStart, nEnd );
        if( nChkEnd < nStart )
            break;
        if( nChkStart <= nEnd && ( nChkEnd > nStart || STRING_LEN == nEnd ) )
        {
            bRet = sal_True;
            break;
        }
    }

    nStart = nOldStart;
    nEnd = nOldEnd;
    nAct = nOldAct;
    return bRet;
}

void SwExtend::ActualizeFont( SwFont &rFnt, MSHORT nAttr )
{
    if ( nAttr & EXTTEXTINPUT_ATTR_UNDERLINE )
        rFnt.SetUnderline( UNDERLINE_SINGLE );
    else if ( nAttr & EXTTEXTINPUT_ATTR_BOLDUNDERLINE )
        rFnt.SetUnderline( UNDERLINE_BOLD );
    else if ( nAttr & EXTTEXTINPUT_ATTR_DOTTEDUNDERLINE )
        rFnt.SetUnderline( UNDERLINE_DOTTED );
    else if ( nAttr & EXTTEXTINPUT_ATTR_DASHDOTUNDERLINE )
        rFnt.SetUnderline( UNDERLINE_DOTTED );

    if ( nAttr & EXTTEXTINPUT_ATTR_REDTEXT )
        rFnt.SetColor( Color( COL_RED ) );

    if ( nAttr & EXTTEXTINPUT_ATTR_HIGHLIGHT )
    {
        const StyleSettings& rStyleSettings = GetpApp()->GetSettings().GetStyleSettings();
        rFnt.SetColor( rStyleSettings.GetHighlightTextColor() );
        rFnt.SetFillColor( rStyleSettings.GetHighlightColor() );
        rFnt.SetTransparent( sal_False );
    }
    if ( nAttr & EXTTEXTINPUT_ATTR_GRAYWAVELINE )
        rFnt.SetGreyWave( sal_True );
}

short SwExtend::Enter( SwFont& rFnt, xub_StrLen nNew )
{
    ASSERT( !Inside(), "SwExtend: Enter without Leave" );
    ASSERT( !pFnt, "SwExtend: Enter with Font" );
    nPos = nNew;
    if( Inside() )
    {
        pFnt = new SwFont( rFnt );
        ActualizeFont( rFnt, rArr[ nPos - nStart ] );
        return 1;
    }
    return 0;
}

sal_Bool SwExtend::_Leave( SwFont& rFnt, xub_StrLen nNew )
{
    ASSERT( Inside(), "SwExtend: Leave without Enter" );
    MSHORT nOldAttr = rArr[ nPos - nStart ];
    nPos = nNew;
    if( Inside() )
    {	// Wir sind innerhalb des ExtendText-Bereichs geblieben
        MSHORT nAttr = rArr[ nPos - nStart ];
        if( nOldAttr != nAttr ) // Gibt es einen (inneren) Attributwechsel?
        {
            rFnt = *pFnt;
            ActualizeFont( rFnt, nAttr );
        }
    }
    else
    {
        rFnt = *pFnt;
        delete pFnt;
        pFnt = NULL;
        return sal_True;
    }
    return sal_False;
}

xub_StrLen SwExtend::Next( xub_StrLen nNext )
{
    if( nPos < nStart )
    {
        if( nNext > nStart )
            nNext = nStart;
    }
    else if( nPos < nEnd )
    {
        MSHORT nIdx = nPos - nStart;
        MSHORT nAttr = rArr[ nIdx ];
        while( ++nIdx < rArr.Count() && nAttr == rArr[ nIdx ] )
            ; //nothing
        nIdx += nStart;
        if( nNext > nIdx )
            nNext = nIdx;
    }
    return nNext;
}
