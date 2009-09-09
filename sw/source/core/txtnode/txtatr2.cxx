/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: txtatr2.cxx,v $
 * $Revision: 1.21 $
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
#include <hints.hxx>
#include <sfx2/objsh.hxx>
#include <svx/xmlcnitm.hxx>
#include <svx/twolinesitem.hxx>
#include <txtinet.hxx>
#include <txtatr.hxx>
#include <fchrfmt.hxx>
#include <fmtinfmt.hxx>
#include <charfmt.hxx>
#include <ndtxt.hxx>        // SwCharFmt, SwTxtNode
#include <poolfmt.hxx>		// RES_POOLCHR_INET_...
#include <doc.hxx>			// SwDoc
#include <fmtruby.hxx>
#include <fmthbsh.hxx>

TYPEINIT1(SwTxtINetFmt,SwClient);
TYPEINIT1(SwTxtRuby,SwClient);

/*************************************************************************
 *						class SwTxtHardBlank
 *************************************************************************/

SwTxtHardBlank::SwTxtHardBlank( const SwFmtHardBlank& rAttr, xub_StrLen nStt )
  : SwTxtAttr( rAttr, nStt )
  , m_Char( rAttr.GetChar() )
{
    ASSERT( ' ' != m_Char && '-' != m_Char,
            "Invalid character for the HardBlank attribute - "
            "must be a normal unicode character" );
}


/*************************************************************************
 *						class SwTxtCharFmt
 *************************************************************************/

SwTxtCharFmt::SwTxtCharFmt( SwFmtCharFmt& rAttr,
                    xub_StrLen nStt, xub_StrLen nEnde )
    : SwTxtAttrEnd( rAttr, nStt, nEnde )
    , m_pTxtNode( 0 )
    , m_nSortNumber( 0 )
{
    rAttr.pTxtAttr = this;
    SetCharFmtAttr( TRUE );
}

SwTxtCharFmt::~SwTxtCharFmt( )
{
}

void SwTxtCharFmt::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    USHORT nWhich = pOld ? pOld->Which() : pNew ? pNew->Which() : 0;
    ASSERT(  isCHRATR(nWhich) || (RES_OBJECTDYING == nWhich)
             || (RES_ATTRSET_CHG == nWhich) || (RES_FMT_CHG == nWhich),
        "SwTxtCharFmt::Modify(): unknown Modify");

    if ( m_pTxtNode )
    {
        SwUpdateAttr aUpdateAttr( *GetStart(), *GetEnd(), nWhich );
        m_pTxtNode->Modify( &aUpdateAttr, &aUpdateAttr );
    }
}

    // erfrage vom Modify Informationen
BOOL SwTxtCharFmt::GetInfo( SfxPoolItem& rInfo ) const
{
    if ( RES_AUTOFMT_DOCNODE != rInfo.Which() || !m_pTxtNode ||
        &m_pTxtNode->GetNodes() != static_cast<SwAutoFmtGetDocNode&>(rInfo).pNodes )
    {
        return TRUE;
    }

    static_cast<SwAutoFmtGetDocNode&>(rInfo).pCntntNode = m_pTxtNode;
    return FALSE;
}

/*************************************************************************
 *						class SwTxtINetFmt
 *************************************************************************/

SwTxtINetFmt::SwTxtINetFmt( SwFmtINetFmt& rAttr,
                            xub_StrLen nStt, xub_StrLen nEnde )
    : SwTxtAttrEnd( rAttr, nStt, nEnde )
    , SwClient( 0 )
    , m_pTxtNode( 0 )
    , m_bVisited( false )
    , m_bVisitedValid( false )
{
    rAttr.pTxtAttr  = this;
    SetCharFmtAttr( TRUE );
}

SwTxtINetFmt::~SwTxtINetFmt( )
{
}

SwCharFmt* SwTxtINetFmt::GetCharFmt()
{
    const SwFmtINetFmt& rFmt = SwTxtAttrEnd::GetINetFmt();
    SwCharFmt* pRet = NULL;

    if( rFmt.GetValue().Len() )
    {
        const SwDoc* pDoc = GetTxtNode().GetDoc();
        if( !IsVisitedValid() )
        {
            SetVisited( pDoc->IsVisitedURL( rFmt.GetValue() ) );
            SetVisitedValid( true );
        }
        USHORT nId;
        const String& rStr = IsVisited() ? rFmt.GetVisitedFmt()
                                           : rFmt.GetINetFmt();
        if( rStr.Len() )
            nId = IsVisited() ? rFmt.GetVisitedFmtId() : rFmt.GetINetFmtId();
        else
            nId = static_cast<USHORT>(IsVisited() ? RES_POOLCHR_INET_VISIT : RES_POOLCHR_INET_NORMAL);

        // JP 10.02.2000, Bug 72806: dont modify the doc for getting the
        //		correct charstyle.
        BOOL bResetMod = !pDoc->IsModified();
        Link aOle2Lnk;
        if( bResetMod )
        {
            aOle2Lnk = pDoc->GetOle2Link();
            ((SwDoc*)pDoc)->SetOle2Link( Link() );
        }

        pRet = IsPoolUserFmt( nId )
                ? ((SwDoc*)pDoc)->FindCharFmtByName( rStr )
                : ((SwDoc*)pDoc)->GetCharFmtFromPool( nId );

        if( bResetMod )
        {
            ((SwDoc*)pDoc)->ResetModified();
            ((SwDoc*)pDoc)->SetOle2Link( aOle2Lnk );
        }
    }

    if( pRet )
        pRet->Add( this );
    else if( GetRegisteredIn() )
        pRegisteredIn->Remove( this );

    return pRet;
}

void SwTxtINetFmt::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    USHORT nWhich = pOld ? pOld->Which() : pNew ? pNew->Which() : 0;
    ASSERT(  isCHRATR(nWhich) || (RES_OBJECTDYING == nWhich)
             || (RES_ATTRSET_CHG == nWhich) || (RES_FMT_CHG == nWhich),
        "SwTxtINetFmt::Modify(): unknown Modify");

    if ( m_pTxtNode )
    {
        SwUpdateAttr aUpdateAttr( *GetStart(), *GetEnd(), nWhich );
        m_pTxtNode->Modify( &aUpdateAttr, &aUpdateAttr );
    }
}

    // erfrage vom Modify Informationen
BOOL SwTxtINetFmt::GetInfo( SfxPoolItem& rInfo ) const
{
    if ( RES_AUTOFMT_DOCNODE != rInfo.Which() || !m_pTxtNode ||
        &m_pTxtNode->GetNodes() != static_cast<SwAutoFmtGetDocNode&>(rInfo).pNodes )
    {
        return TRUE;
    }

    static_cast<SwAutoFmtGetDocNode&>(rInfo).pCntntNode = m_pTxtNode;
    return FALSE;
}

BOOL SwTxtINetFmt::IsProtect( ) const
{
    return m_pTxtNode && m_pTxtNode->IsProtect();
}

// ATT_XNLCONTAINERITEM ******************************

SwTxtXMLAttrContainer::SwTxtXMLAttrContainer(
                            const SvXMLAttrContainerItem& rAttr,
                            xub_StrLen nStt, xub_StrLen nEnde )
    : SwTxtAttrEnd( rAttr, nStt, nEnde )
{}


/*************************************************************************
 *						class SwTxtRuby
 *************************************************************************/

SwTxtRuby::SwTxtRuby( SwFmtRuby& rAttr,
                      xub_StrLen nStart, xub_StrLen nEnd )
    : SwTxtAttrEnd( rAttr, nStart, nEnd )
    , SwClient( 0 )
    , m_pTxtNode( 0 )
{
    rAttr.pTxtAttr  = this;
    SetDontExpand( true );  // never expand this attribute
    SetLockExpandFlag( true );
    SetDontMergeAttr( true );
    SetDontExpandStartAttr( true );
}

SwTxtRuby::~SwTxtRuby()
{
}

void SwTxtRuby::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew )
{
    USHORT nWhich = pOld ? pOld->Which() : pNew ? pNew->Which() : 0;
    ASSERT(  isCHRATR(nWhich) || (RES_OBJECTDYING == nWhich)
             || (RES_ATTRSET_CHG == nWhich) || (RES_FMT_CHG == nWhich),
        "SwTxtRuby::Modify(): unknown Modify");

    if ( m_pTxtNode )
    {
        SwUpdateAttr aUpdateAttr( *GetStart(), *GetEnd(), nWhich );
        m_pTxtNode->Modify( &aUpdateAttr, &aUpdateAttr );
    }
}

BOOL SwTxtRuby::GetInfo( SfxPoolItem& rInfo ) const
{
    if( RES_AUTOFMT_DOCNODE != rInfo.Which() || !m_pTxtNode ||
        &m_pTxtNode->GetNodes() != static_cast<SwAutoFmtGetDocNode&>(rInfo).pNodes )
    {
        return TRUE;
    }

    static_cast<SwAutoFmtGetDocNode&>(rInfo).pCntntNode = m_pTxtNode;
    return FALSE;
}

SwCharFmt* SwTxtRuby::GetCharFmt()
{
    const SwFmtRuby& rFmt = SwTxtAttrEnd::GetRuby();
    SwCharFmt* pRet = 0;

    if( rFmt.GetText().Len() )
    {
        const SwDoc* pDoc = GetTxtNode().GetDoc();
        const String& rStr = rFmt.GetCharFmtName();
        USHORT nId = RES_POOLCHR_RUBYTEXT;
        if ( rStr.Len() )
            nId = rFmt.GetCharFmtId();

        // JP 10.02.2000, Bug 72806: dont modify the doc for getting the
        //				correct charstyle.
        BOOL bResetMod = !pDoc->IsModified();
        Link aOle2Lnk;
        if( bResetMod )
        {
            aOle2Lnk = pDoc->GetOle2Link();
            ((SwDoc*)pDoc)->SetOle2Link( Link() );
        }

        pRet = IsPoolUserFmt( nId )
                ? ((SwDoc*)pDoc)->FindCharFmtByName( rStr )
                : ((SwDoc*)pDoc)->GetCharFmtFromPool( nId );

        if( bResetMod )
        {
            ((SwDoc*)pDoc)->ResetModified();
            ((SwDoc*)pDoc)->SetOle2Link( aOle2Lnk );
        }
    }

    if( pRet )
        pRet->Add( this );
    else if( GetRegisteredIn() )
        pRegisteredIn->Remove( this );

    return pRet;
}

// ******************************

SwTxt2Lines::SwTxt2Lines( const SvxTwoLinesItem& rAttr,
                        xub_StrLen nStt, xub_StrLen nEnde )
    : SwTxtAttrEnd( rAttr, nStt, nEnde )
{
}

