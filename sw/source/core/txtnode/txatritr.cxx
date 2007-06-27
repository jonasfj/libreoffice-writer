/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: txatritr.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 13:21:35 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif

#ifndef _TXATRITR_HXX
#include <txatritr.hxx>
#endif
#ifndef _FCHRFMT_HXX
#include <fchrfmt.hxx>
#endif
#ifndef _CHARFMT_HXX
#include <charfmt.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _TXATBASE_HXX
#include <txatbase.hxx>
#endif

using namespace ::com::sun::star::i18n;


SwScriptIterator::SwScriptIterator( const String& rStr, xub_StrLen nStt, sal_Bool bFrwrd )
    : rText( rStr ),
      nChgPos( rStr.Len() ),
      nCurScript( ScriptType::WEAK ),
      bForward( bFrwrd )
{
    if( pBreakIt->xBreak.is() )
    {
        if ( ! bFrwrd && nStt )
            --nStt;

        xub_StrLen nPos = nStt;
        nCurScript = pBreakIt->xBreak->getScriptType( rText, nPos );
        if( ScriptType::WEAK == nCurScript )
        {
            if( nPos )
            {
                nPos = (xub_StrLen)pBreakIt->xBreak->beginOfScript(
                                                rText, nPos, nCurScript );
                if( nPos && nPos < rText.Len() )
                {
                    nStt = --nPos;
                    nCurScript = pBreakIt->xBreak->getScriptType( rText,nPos);
                }
            }
        }

        nChgPos = bForward ?
                  (xub_StrLen)pBreakIt->xBreak->endOfScript( rText, nStt, nCurScript ) :
                  (xub_StrLen)pBreakIt->xBreak->beginOfScript( rText, nStt, nCurScript );
    }
}

sal_Bool SwScriptIterator::Next()
{
    sal_Bool bRet = sal_False;
    if( pBreakIt->xBreak.is() )
    {
        if ( bForward && nChgPos < rText.Len() )
        {
            nCurScript = pBreakIt->xBreak->getScriptType( rText, nChgPos );
            nChgPos = (xub_StrLen)pBreakIt->xBreak->endOfScript(
                                                rText, nChgPos, nCurScript );
            bRet = sal_True;
        }
        else if ( ! bForward && nChgPos )
        {
            --nChgPos;
            nCurScript = pBreakIt->xBreak->getScriptType( rText, nChgPos );
            nChgPos = (xub_StrLen)pBreakIt->xBreak->beginOfScript(
                                                rText, nChgPos, nCurScript );
            bRet = sal_True;
        }
    }
    else
        nChgPos = rText.Len();
    return bRet;
}

// --------------------------------------------------------------------

SwTxtAttrIterator::SwTxtAttrIterator( const SwTxtNode& rTNd, USHORT nWhchId,
                                        xub_StrLen nStt,
                                        sal_Bool bUseGetWhichOfScript )
    : aSIter( rTNd.GetTxt(), nStt ), rTxtNd( rTNd ),
    nChgPos( nStt ), nAttrPos( 0 ), pParaItem( 0 ), nWhichId( nWhchId ),
    bIsUseGetWhichOfScript( bUseGetWhichOfScript )
{
    SearchNextChg();
}

sal_Bool SwTxtAttrIterator::Next()
{
    sal_Bool bRet = sal_False;
    if( nChgPos < aSIter.GetText().Len() )
    {
        bRet = sal_True;
        if( aStack.Count() )
        {
            do {
                const SwTxtAttr* pHt = (SwTxtAttr*)aStack[ 0 ];
                USHORT nEndPos = *pHt->GetEnd();
                if( nChgPos >= nEndPos )
                    aStack.Remove( 0 );
                else
                    break;
            } while( aStack.Count() );
        }

        if( aStack.Count() )
        {
            sal_uInt16 nSavePos = nAttrPos;
            SearchNextChg();
            if( aStack.Count() )
            {
                const SwTxtAttr* pHt = (SwTxtAttr*)aStack[ 0 ];
                USHORT nEndPos = *pHt->GetEnd();
                if( nChgPos >= nEndPos )
                {
                    nChgPos = nEndPos;
                    nAttrPos = nSavePos;

                    if( RES_TXTATR_CHARFMT == pHt->Which() )
                    {
                        sal_uInt16 nWId = bIsUseGetWhichOfScript ?
                                GetWhichOfScript( nWhichId,
                                                  aSIter.GetCurrScript() ) : nWhichId;
                        pCurItem = &pHt->GetCharFmt().GetCharFmt()->GetAttr(nWId);
                    }
                    else
                        pCurItem = &pHt->GetAttr();

                    aStack.Remove( 0 );
                }
            }
        }
        else
            SearchNextChg();
    }
    return bRet;
}

void SwTxtAttrIterator::AddToStack( const SwTxtAttr& rAttr )
{
    void* pAdd = (void*)&rAttr;
    USHORT nIns = 0, nEndPos = *rAttr.GetEnd();
    for( ; nIns < aStack.Count(); ++nIns )
        if( *((SwTxtAttr*)aStack[ nIns ] )->GetEnd() > nEndPos )
            break;

    aStack.Insert( pAdd, nIns );
}

void SwTxtAttrIterator::SearchNextChg()
{
    USHORT nWh = 0;
    if( nChgPos == aSIter.GetScriptChgPos() )
    {
        aSIter.Next();
        pParaItem = 0;
        nAttrPos = 0; 		// must be restart at the beginning, because
                            // some attributes can start before or inside
                            // the current scripttype!
        aStack.Remove( 0, aStack.Count() );
    }
    if( !pParaItem )
    {
        nWh = bIsUseGetWhichOfScript ?
                GetWhichOfScript( nWhichId,
                                  aSIter.GetCurrScript() ) : nWhichId;
        pParaItem = &rTxtNd.GetSwAttrSet().Get( nWh );
    }

    xub_StrLen nStt = nChgPos;
    nChgPos = aSIter.GetScriptChgPos();
    pCurItem = pParaItem;

    const SwpHints* pHts = rTxtNd.GetpSwpHints();
    if( pHts )
    {
        if( !nWh )
        {
            nWh =  bIsUseGetWhichOfScript ?
                        GetWhichOfScript( nWhichId,
                                          aSIter.GetCurrScript() ) : nWhichId;
        }

        const SfxPoolItem* pItem = 0;
        for( ; nAttrPos < pHts->Count(); ++nAttrPos )
        {
            const SwTxtAttr* pHt = (*pHts)[ nAttrPos ];
            const USHORT* pEnd = pHt->GetEnd();
            const USHORT nHtStt = *pHt->GetStart();
            if( nHtStt < nStt && ( !pEnd || *pEnd <= nStt ))
                continue;

            if( nHtStt >= nChgPos )
                break;

            pItem = CharFmt::GetItem( *pHt, nWh );
            if ( pItem )
            {
                if( nHtStt > nStt )
                {
                    if( nChgPos > nHtStt )
                        nChgPos = nHtStt;
                    break;
                }
                AddToStack( *pHt );
                pCurItem = pItem;
                if( *pEnd < nChgPos )
                    nChgPos = *pEnd;
            }
        }
    }
}


