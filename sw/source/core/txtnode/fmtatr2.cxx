/*************************************************************************
 *
 *  $RCSfile: fmtatr2.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: mtg $ $Date: 2001-08-16 12:31:52 $
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
#include "unomid.h"

#ifndef __SBX_SBXVARIABLE_HXX //autogen
#include <svtools/sbxvar.hxx>
#endif
#ifndef _SFXMACITEM_HXX //autogen
#include <svtools/macitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif

#ifndef _FCHRFMT_HXX //autogen
#include <fchrfmt.hxx>
#endif
#ifndef _FMTINFMT_HXX //autogen
#include <fmtinfmt.hxx>
#endif
#ifndef _TXTATR_HXX //autogen
#include <txtatr.hxx>
#endif
#ifndef _FMTRUBY_HXX
#include <fmtruby.hxx>
#endif
#ifndef _CHARFMT_HXX
#include <charfmt.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>        // SwUpdateAttr
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _UNOSTYLE_HXX
#include <unostyle.hxx>
#endif
#ifndef _UNOEVENT_HXX
#include <unoevent.hxx>		// SwHyperlinkEventDescriptor
#endif
#ifndef _COM_SUN_STAR_TEXT_RUBYADJUST_HDL_
#include <com/sun/star/text/RubyAdjust.hdl>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::rtl;

/*************************************************************************
|*
|*    class SwFmtCharFmt
|*    Beschreibung
|*    Ersterstellung    JP 23.11.90
|*    Letzte Aenderung  JP 09.08.94
|*
*************************************************************************/



SwFmtCharFmt::SwFmtCharFmt( SwCharFmt *pFmt )
    : SfxPoolItem( RES_TXTATR_CHARFMT ),
    SwClient(pFmt),
    pTxtAttr( 0 )
{
}



SwFmtCharFmt::SwFmtCharFmt( const SwFmtCharFmt& rAttr )
    : SfxPoolItem( RES_TXTATR_CHARFMT ),
    SwClient( rAttr.GetCharFmt() ),
    pTxtAttr( 0 )
{
}



SwFmtCharFmt::~SwFmtCharFmt() {}



int SwFmtCharFmt::operator==( const SfxPoolItem& rAttr ) const
{
    ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
    return GetCharFmt() == ((SwFmtCharFmt&)rAttr).GetCharFmt();
}



SfxPoolItem* SwFmtCharFmt::Clone( SfxItemPool* ) const
{
    return new SwFmtCharFmt( *this );
}



// weiterleiten an das TextAttribut
void SwFmtCharFmt::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    if( pTxtAttr )
        pTxtAttr->Modify( pOld, pNew );
}



// weiterleiten an das TextAttribut
BOOL SwFmtCharFmt::GetInfo( SfxPoolItem& rInfo ) const
{
    return pTxtAttr ? pTxtAttr->GetInfo( rInfo ) : FALSE;
}
BOOL SwFmtCharFmt::QueryValue( uno::Any& rVal, BYTE nMemberId ) const
{
    String sCharFmtName;
    if(GetCharFmt())
        SwStyleNameMapper::FillProgName(GetCharFmt()->GetName(), sCharFmtName,  GET_POOLID_CHRFMT, sal_True );
    rVal <<= OUString( sCharFmtName );
    return TRUE;
}
BOOL SwFmtCharFmt::PutValue( const uno::Any& rVal, BYTE nMemberId  )
{
    DBG_ERROR("Zeichenvorlage kann mit PutValue nicht gesetzt werden!")
    return FALSE;
}

/*************************************************************************
|*
|*    class SwFmtINetFmt
|*    Beschreibung
|*    Ersterstellung    AMA 02.08.96
|*    Letzte Aenderung  AMA 02.08.96
|*
*************************************************************************/



SwFmtINetFmt::SwFmtINetFmt( const XubString& rURL, const XubString& rTarget )
    : SfxPoolItem( RES_TXTATR_INETFMT ),
    aURL( rURL ),
    aTargetFrame( rTarget ),
    pTxtAttr( 0 ),
    pMacroTbl( 0 ),
    nINetId( 0 ),
    nVisitedId( 0 )
{
}



SwFmtINetFmt::SwFmtINetFmt( const SwFmtINetFmt& rAttr )
    : SfxPoolItem( RES_TXTATR_INETFMT ),
    aURL( rAttr.GetValue() ),
    aName( rAttr.aName ),
    aTargetFrame( rAttr.aTargetFrame ),
    aINetFmt( rAttr.aINetFmt ),
    aVisitedFmt( rAttr.aVisitedFmt ),
    pTxtAttr( 0 ),
    pMacroTbl( 0 ),
    nINetId( rAttr.nINetId ),
    nVisitedId( rAttr.nVisitedId )
{
    if( rAttr.GetMacroTbl() )
        pMacroTbl = new SvxMacroTableDtor( *rAttr.GetMacroTbl() );
}



SwFmtINetFmt::~SwFmtINetFmt()
{
    delete pMacroTbl;
}



int SwFmtINetFmt::operator==( const SfxPoolItem& rAttr ) const
{
    ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
    BOOL bRet = SfxPoolItem::operator==( (SfxPoolItem&) rAttr )
                && aURL == ((SwFmtINetFmt&)rAttr).aURL
                && aName == ((SwFmtINetFmt&)rAttr).aName
                && aTargetFrame == ((SwFmtINetFmt&)rAttr).aTargetFrame
                && aINetFmt == ((SwFmtINetFmt&)rAttr).aINetFmt
                && aVisitedFmt == ((SwFmtINetFmt&)rAttr).aVisitedFmt
                && nINetId == ((SwFmtINetFmt&)rAttr).nINetId
                && nVisitedId == ((SwFmtINetFmt&)rAttr).nVisitedId;

    if( !bRet )
        return FALSE;

    const SvxMacroTableDtor* pOther = ((SwFmtINetFmt&)rAttr).pMacroTbl;
    if( !pMacroTbl )
        return ( !pOther || !pOther->Count() );
    if( !pOther )
        return 0 == pMacroTbl->Count();

    const SvxMacroTableDtor& rOwn = *pMacroTbl;
    const SvxMacroTableDtor& rOther = *pOther;

    // Anzahl unterschiedlich => auf jeden Fall ungleich
    if( rOwn.Count() != rOther.Count() )
        return FALSE;

    // einzeln vergleichen; wegen Performance ist die Reihenfolge wichtig
    for( USHORT nNo = 0; nNo < rOwn.Count(); ++nNo )
    {
        const SvxMacro *pOwnMac = rOwn.GetObject(nNo);
        const SvxMacro *pOtherMac = rOther.GetObject(nNo);
        if ( 	rOwn.GetKey(pOwnMac) != rOther.GetKey(pOtherMac)  ||
                pOwnMac->GetLibName() != pOtherMac->GetLibName() ||
                pOwnMac->GetMacName() != pOtherMac->GetMacName() )
            return FALSE;
    }
    return TRUE;
}



SfxPoolItem* SwFmtINetFmt::Clone( SfxItemPool* ) const
{
    return new SwFmtINetFmt( *this );
}



void SwFmtINetFmt::SetMacroTbl( const SvxMacroTableDtor* pNewTbl )
{
    if( pNewTbl )
    {
        if( pMacroTbl )
            *pMacroTbl = *pNewTbl;
        else
            pMacroTbl = new SvxMacroTableDtor( *pNewTbl );
    }
    else if( pMacroTbl )
        delete pMacroTbl, pMacroTbl = 0;
}



void SwFmtINetFmt::SetMacro( USHORT nEvent, const SvxMacro& rMacro )
{
    if( !pMacroTbl )
        pMacroTbl = new SvxMacroTableDtor;

    SvxMacro *pOldMacro;
    if( 0 != ( pOldMacro = pMacroTbl->Get( nEvent )) )
    {
        delete pOldMacro;
        pMacroTbl->Replace( nEvent, new SvxMacro( rMacro ) );
    }
    else
        pMacroTbl->Insert( nEvent, new SvxMacro( rMacro ) );
}



const SvxMacro* SwFmtINetFmt::GetMacro( USHORT nEvent ) const
{
    const SvxMacro* pRet = 0;
    if( pMacroTbl && pMacroTbl->IsKeyValid( nEvent ) )
        pRet = pMacroTbl->Get( nEvent );
    return pRet;
}



BOOL SwFmtINetFmt::QueryValue( uno::Any& rVal, BYTE nMemberId ) const
{
    BOOL bRet = TRUE;
    XubString sVal;
    switch(nMemberId)
    {
        case MID_URL_URL:
            sVal = aURL;
        break;
        case MID_URL_TARGET:
            sVal = aTargetFrame;
        break;
        case MID_URL_HYPERLINKNAME:
            sVal = aName;
        break;
        case MID_URL_VISITED_FMT:
            sVal = aVisitedFmt;
            if( !sVal.Len() && nVisitedId != 0 )
                SwStyleNameMapper::FillUIName( nVisitedId, sVal );
            if( sVal.Len() )
                SwStyleNameMapper::FillProgName( sVal, sVal, GET_POOLID_CHRFMT, sal_True );
        break;
        case MID_URL_UNVISITED_FMT:
            sVal = aINetFmt;
            if( !sVal.Len() && nINetId != 0 )
                SwStyleNameMapper::FillUIName( nINetId, sVal );
            if( sVal.Len() )
                SwStyleNameMapper::FillProgName( sVal, sVal, GET_POOLID_CHRFMT, sal_True );
        break;
        case MID_URL_HYPERLINKEVENTS:
        {
            // create (and return) event descriptor
            SwHyperlinkEventDescriptor* pEvents =
                new SwHyperlinkEventDescriptor();
            pEvents->copyMacrosFromINetFmt(*this);
            uno::Reference<container::XNameReplace> xNameReplace(pEvents);

            // all others return a string; so we just set rVal here and exit
            rVal <<= xNameReplace;
            return bRet;
        }
        break;
        default:
            bRet = FALSE;
    }
    rVal <<= OUString(sVal);
    return bRet;
}
BOOL SwFmtINetFmt::PutValue( const uno::Any& rVal, BYTE nMemberId  )
{
    BOOL bRet = TRUE;

    // all properties except HyperlinkEvents are of type string, hence
    // we treat HyperlinkEvents specially
    if (MID_URL_HYPERLINKEVENTS == nMemberId)
    {
        uno::Reference<container::XNameReplace> xReplace;
        rVal >>= xReplace;
        if (xReplace.is())
        {
            // Create hyperlink event descriptor. Then copy events
            // from argument into descriptor. Then copy events from
            // the descriptor into the format.
            SwHyperlinkEventDescriptor aEvents;
            aEvents.copyMacrosFromNameReplace(xReplace);
            aEvents.copyMacrosIntoINetFmt(*this);
        }
        else
        {
            // wrong type!
            bRet = FALSE;
        }
    }
    else
    {
        // all string properties:
        if(rVal.getValueType() != ::getCppuType((rtl::OUString*)0))
            return FALSE;
        XubString sVal = *(rtl::OUString*)rVal.getValue();
        switch(nMemberId)
        {
            case MID_URL_URL:
                aURL = sVal;
                break;
            case MID_URL_TARGET:
                aTargetFrame = sVal;
                break;
            case MID_URL_HYPERLINKNAME:
                aName = sVal;
                break;
            case MID_URL_VISITED_FMT:
            {
                String aString;
                SwStyleNameMapper::FillUIName( sVal, aString, GET_POOLID_CHRFMT, sal_True );
                aVisitedFmt = OUString ( aString ); 
                nVisitedId = SwStyleNameMapper::GetPoolIdFromUIName( aVisitedFmt,
                                               GET_POOLID_CHRFMT );
            }
            break;
            case MID_URL_UNVISITED_FMT:
            {
                String aString;
                SwStyleNameMapper::FillUIName( sVal, aString, GET_POOLID_CHRFMT, sal_True );
                aINetFmt = OUString ( aString ); 
                nINetId = SwStyleNameMapper::GetPoolIdFromUIName( aINetFmt,	GET_POOLID_CHRFMT );
            }
            break;
            default:
                bRet = FALSE;
        }
    }
    return bRet;
}


/*************************************************************************
|*    class SwFmtRuby
*************************************************************************/

SwFmtRuby::SwFmtRuby( const String& rRubyTxt )
    : SfxPoolItem( RES_TXTATR_CJK_RUBY ),
    sRubyTxt( rRubyTxt ),
    nCharFmtId( 0 ),
    nPosition( 0 ), nAdjustment( 0 ),
    pTxtAttr( 0 )
{
}

SwFmtRuby::SwFmtRuby( const SwFmtRuby& rAttr )
    : SfxPoolItem( RES_TXTATR_CJK_RUBY ),
    sRubyTxt( rAttr.sRubyTxt ),
    sCharFmtName( rAttr.sCharFmtName ),
    nCharFmtId( rAttr.nCharFmtId),
    nPosition( rAttr.nPosition ), nAdjustment( rAttr.nAdjustment ),
    pTxtAttr( 0 )
{
}

SwFmtRuby::~SwFmtRuby()
{
}

SwFmtRuby& SwFmtRuby::operator=( const SwFmtRuby& rAttr )
{
    sRubyTxt = rAttr.sRubyTxt;
    sCharFmtName = rAttr.sCharFmtName;
    nCharFmtId = rAttr.nCharFmtId;
    nPosition = rAttr.nPosition;
    nAdjustment = rAttr.nAdjustment;
    pTxtAttr =  0;
    return *this;
}

int SwFmtRuby::operator==( const SfxPoolItem& rAttr ) const
{
    ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
    return sRubyTxt == ((SwFmtRuby&)rAttr).sRubyTxt &&
           sCharFmtName == ((SwFmtRuby&)rAttr).sCharFmtName &&
           nCharFmtId == ((SwFmtRuby&)rAttr).nCharFmtId &&
           nPosition == ((SwFmtRuby&)rAttr).nPosition &&
           nAdjustment == ((SwFmtRuby&)rAttr).nAdjustment;
}

SfxPoolItem* SwFmtRuby::Clone( SfxItemPool* ) const
{
    return new SwFmtRuby( *this );
}

BOOL SwFmtRuby::QueryValue( com::sun::star::uno::Any& rVal,
                            BYTE nMemberId ) const
{
    BOOL bRet = TRUE;
    switch( nMemberId )
    {
        case MID_RUBY_TEXT: rVal <<= (OUString)sRubyTxt; 					break;
         case MID_RUBY_ADJUST:	rVal <<= (sal_Int16)nAdjustment;	break;
        case MID_RUBY_CHARSTYLE:
        {
            String aString;
            SwStyleNameMapper::FillProgName(sCharFmtName, aString, GET_POOLID_CHRFMT, sal_True );
            rVal <<= OUString ( aString );
        }
        break;
        case MID_RUBY_ABOVE:
        {
            sal_Bool bAbove = !nPosition;
            rVal.setValue(&bAbove, ::getBooleanCppuType());
        }
        break;
        default:
            bRet = FALSE;
    }
    return bRet;
}
BOOL SwFmtRuby::PutValue( const com::sun::star::uno::Any& rVal,
                            BYTE nMemberId  )
{
    BOOL bRet;
    switch( nMemberId )
    {
        case MID_RUBY_TEXT:
        {
            OUString sTmp;
            bRet = rVal >>= sTmp;
            sRubyTxt = sTmp;
        }
        break;
         case MID_RUBY_ADJUST:
        {
            sal_Int16 nSet; rVal >>= nSet;
            if(nSet >= 0 && nSet <= com::sun::star::text::RubyAdjust_INDENT_BLOCK)
                nAdjustment = nSet;
            else
                bRet = sal_False;
        }
        break;
        case MID_RUBY_ABOVE:
        {
            const uno::Type& rType = ::getBooleanCppuType();
            if(rVal.hasValue() && rVal.getValueType() == rType)
            {
                sal_Bool bAbove = *(sal_Bool*)rVal.getValue();
                nPosition = bAbove ? 0 : 1;
            }
        }
        break;
        case MID_RUBY_CHARSTYLE:
            DBG_ERROR("char style name must be handled outside")
//			bRet = rVal >>= sTmp;
//			sCharFmtName <<= SwStyleNameMapper::GetUIName(sTmp, GET_POOLID_CHRFMT );
        //no break;
        default:
            bRet = FALSE;
    }
    return bRet;
}

