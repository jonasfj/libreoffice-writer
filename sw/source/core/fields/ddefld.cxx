/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ddefld.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-05 17:01:49 $
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


#ifndef _SVXLINKMGR_HXX
#include <svx/linkmgr.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _DDEFLD_HXX
#include <ddefld.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _SWBASLNK_HXX
#include <swbaslnk.hxx>
#endif
#ifndef _SWDDETBL_HXX
#include <swddetbl.hxx>
#endif
#ifndef _UNOFLDMID_H
#include <unofldmid.h>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif

using namespace rtl;
using namespace ::com::sun::star;

#ifdef PM2
#define DDE_TXT_ENCODING    RTL_TEXTENCODING_IBM_850
#else
#define DDE_TXT_ENCODING 	RTL_TEXTENCODING_MS_1252
#endif

/*--------------------------------------------------------------------
    Beschreibung: Globale Variablen
 --------------------------------------------------------------------*/

class SwIntrnlRefLink : public SwBaseLink
{
    SwDDEFieldType& rFldType;
public:
    SwIntrnlRefLink( SwDDEFieldType& rType, USHORT nUpdateType, USHORT nFmt )
        : SwBaseLink( nUpdateType, nFmt ),
        rFldType( rType )
    {}

    virtual void Closed();
    virtual void DataChanged( const String& rMimeType,
                                const uno::Any & rValue );

    virtual const SwNode* GetAnchor() const;
    virtual BOOL IsInRange( ULONG nSttNd, ULONG nEndNd, xub_StrLen nStt = 0,
                            xub_StrLen nEnd = STRING_NOTFOUND ) const;
};


void SwIntrnlRefLink::DataChanged( const String& rMimeType,
                                const uno::Any & rValue )
{
    switch( SotExchange::GetFormatIdFromMimeType( rMimeType ) )
    {
    case FORMAT_STRING:
        if( !IsNoDataFlag() )
        {
            uno::Sequence< sal_Int8 > aSeq;
            rValue >>= aSeq;
            String sStr( (sal_Char*)aSeq.getConstArray(), static_cast<xub_StrLen>(aSeq.getLength()),
                               DDE_TXT_ENCODING	 );

            // CR-LF am Ende entfernen, ist ueberfluessig!
            xub_StrLen n = sStr.Len();
            while( n && 0 == sStr.GetChar( n-1 ) )
                --n;
            if( n && 0x0a == sStr.GetChar( n-1 ) )
                --n;
            if( n && 0x0d == sStr.GetChar( n-1 ) )
                --n;

            BOOL bDel = n != sStr.Len();
            if( bDel )
                sStr.Erase( n );

            rFldType.SetExpansion( sStr );
            // erst Expansion setzen! (sonst wird das Flag geloescht!)
            rFldType.SetCRLFDelFlag( bDel );
        }
        break;

    // weitere Formate ...
    default:
        return;
    }

    ASSERT( rFldType.GetDoc(), "Kein pDoc" );

    // keine Abhaengigen mehr?
    if( rFldType.GetDepends() && !rFldType.IsModifyLocked() && !ChkNoDataFlag() )
    {
        ViewShell* pSh;
        SwEditShell* pESh = rFldType.GetDoc()->GetEditShell( &pSh );

        // dann suchen wir uns mal alle Felder. Wird kein gueltiges
        // gefunden, dann Disconnecten wir uns!
        SwMsgPoolItem aUpdateDDE( RES_UPDATEDDETBL );
        int bCallModify = FALSE;
        rFldType.LockModify();

        SwClientIter aIter( rFldType );
        SwClient * pLast = aIter.GoStart();
        if( pLast ) 	// konnte zum Anfang gesprungen werden ??
            do {
                // eine DDE-Tabelle oder ein DDE-FeldAttribut im Text
                if( !pLast->IsA( TYPE( SwFmtFld ) ) ||
                    ((SwFmtFld*)pLast)->GetTxtFld() )
                {
                    if( !bCallModify )
                    {
                        if( pESh )
                            pESh->StartAllAction();
                        else if( pSh )
                            pSh->StartAction();
                    }
                    pLast->Modify( 0, &aUpdateDDE );
                    bCallModify = TRUE;
                }
            } while( 0 != ( pLast = aIter++ ));

        rFldType.UnlockModify();

        if( bCallModify )
        {
            if( pESh )
                pESh->EndAllAction();
            else if( pSh )
                pSh->EndAction();

            if( pSh )
                pSh->GetDoc()->SetModified();
        }
    }
}

void SwIntrnlRefLink::Closed()
{
    if( rFldType.GetDoc() && !rFldType.GetDoc()->IsInDtor() )
    {
        // Advise verabschiedet sich, alle Felder in Text umwandeln ?
        ViewShell* pSh;
        SwEditShell* pESh = rFldType.GetDoc()->GetEditShell( &pSh );
        if( pESh )
        {
            pESh->StartAllAction();
            pESh->FieldToText( &rFldType );
            pESh->EndAllAction();
        }
        else
        {
            pSh->StartAction();
            // am Doc aufrufen ??
            pSh->EndAction();
        }
    }
    SvBaseLink::Closed();
}

const SwNode* SwIntrnlRefLink::GetAnchor() const
{
    // hier sollte irgend ein Anchor aus dem normalen Nodes-Array reichen
    const SwNode* pNd = 0;
    SwClientIter aIter( rFldType );
    SwClient * pLast = aIter.GoStart();
    if( pLast ) 	// konnte zum Anfang gesprungen werden ??
        do {
            // eine DDE-Tabelle oder ein DDE-FeldAttribut im Text
            if( !pLast->IsA( TYPE( SwFmtFld ) ))
            {
                SwDepend* pDep = (SwDepend*)pLast;
                SwDDETable* pDDETbl = (SwDDETable*)pDep->GetToTell();
                pNd = pDDETbl->GetTabSortBoxes()[0]->GetSttNd();
            }
            else if( ((SwFmtFld*)pLast)->GetTxtFld() )
                pNd = ((SwFmtFld*)pLast)->GetTxtFld()->GetpTxtNode();

            if( pNd && &rFldType.GetDoc()->GetNodes() == &pNd->GetNodes() )
                break;
            pNd = 0;
        } while( 0 != ( pLast = aIter++ ));

    return pNd;
}

BOOL SwIntrnlRefLink::IsInRange( ULONG nSttNd, ULONG nEndNd,
                                xub_StrLen nStt, xub_StrLen nEnd ) const
{
    // hier sollte irgend ein Anchor aus dem normalen Nodes-Array reichen
    SwNodes* pNds = &rFldType.GetDoc()->GetNodes();
    SwClientIter aIter( rFldType );
    SwClient * pLast = aIter.GoStart();
    if( pLast ) 	// konnte zum Anfang gesprungen werden ??
        do {
            // eine DDE-Tabelle oder ein DDE-FeldAttribut im Text
            if( !pLast->IsA( TYPE( SwFmtFld ) ))
            {
                SwDepend* pDep = (SwDepend*)pLast;
                SwDDETable* pDDETbl = (SwDDETable*)pDep->GetToTell();
                const SwTableNode* pTblNd = pDDETbl->GetTabSortBoxes()[0]->
                                GetSttNd()->FindTableNode();
                if( pTblNd->GetNodes().IsDocNodes() &&
                    nSttNd < pTblNd->EndOfSectionIndex() &&
                    nEndNd > pTblNd->GetIndex() )
                    return TRUE;
            }
            else if( ((SwFmtFld*)pLast)->GetTxtFld() )
            {
                const SwTxtFld* pTFld = ((SwFmtFld*)pLast)->GetTxtFld();
                const SwTxtNode* pNd = pTFld->GetpTxtNode();
                if( pNd && pNds == &pNd->GetNodes() )
                {
                    ULONG nNdPos = pNd->GetIndex();
                    if( nSttNd <= nNdPos && nNdPos <= nEndNd &&
                        ( nNdPos != nSttNd || *pTFld->GetStart() >= nStt ) &&
                        ( nNdPos != nEndNd || *pTFld->GetStart() < nEnd ))
                        return TRUE;
                }
            }
        } while( 0 != ( pLast = aIter++ ));

    return FALSE;
}

SwDDEFieldType::SwDDEFieldType(const String& rName,
                                const String& rCmd, USHORT nUpdateType )
    : SwFieldType( RES_DDEFLD ),
    aName( rName ), pDoc( 0 ), nRefCnt( 0 )
{
    bCRLFFlag = bDeleted = FALSE;
    refLink = new SwIntrnlRefLink( *this, nUpdateType, FORMAT_STRING );
    SetCmd( rCmd );
}

SwDDEFieldType::~SwDDEFieldType()
{
    if( pDoc && !pDoc->IsInDtor() )
        pDoc->GetLinkManager().Remove( refLink );
    refLink->Disconnect();
}

SwFieldType* SwDDEFieldType::Copy() const
{
    SwDDEFieldType* pType = new SwDDEFieldType( aName, GetCmd(), GetType() );
    pType->aExpansion = aExpansion;
    pType->bCRLFFlag = bCRLFFlag;
    pType->bDeleted = bDeleted;
    pType->SetDoc( pDoc );
    return pType;
}

const String& SwDDEFieldType::GetName() const
{
    return aName;
}

void SwDDEFieldType::SetCmd( const String& rStr )
{
    String sCmd( rStr );
    xub_StrLen nPos;
    while( STRING_NOTFOUND != (nPos = sCmd.SearchAscii( "  " )) )
        sCmd.Erase( nPos, 1 );
    refLink->SetLinkSourceName( sCmd );
}

String SwDDEFieldType::GetCmd() const
{
    return refLink->GetLinkSourceName();
}

void SwDDEFieldType::SetDoc( SwDoc* pNewDoc )
{
    if( pNewDoc == pDoc )
        return;

    if( pDoc && refLink.Is() )
    {
        ASSERT( !nRefCnt, "wie kommen die Referenzen rueber?" );
        pDoc->GetLinkManager().Remove( refLink );
    }

    pDoc = pNewDoc;
    if( pDoc && nRefCnt )
    {
        refLink->SetVisible( pDoc->IsVisibleLinks() );
        pDoc->GetLinkManager().InsertDDELink( refLink );
    }
}


void SwDDEFieldType::_RefCntChgd()
{
    if( nRefCnt )
    {
        refLink->SetVisible( pDoc->IsVisibleLinks() );
        pDoc->GetLinkManager().InsertDDELink( refLink );
        if( pDoc->GetRootFrm() )
            UpdateNow();
    }
    else
    {
        Disconnect();
        pDoc->GetLinkManager().Remove( refLink );
    }
}
/* -----------------------------28.08.00 16:23--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwDDEFieldType::QueryValue( uno::Any& rVal, USHORT nWhichId ) const
{
    BYTE nPart = 0;
    switch( nWhichId )
    {
    case FIELD_PROP_PAR2:      nPart = 3; break;
    case FIELD_PROP_PAR4:      nPart = 2; break;
    case FIELD_PROP_SUBTYPE:   nPart = 1; break;
    case FIELD_PROP_BOOL1:
        {
            sal_Bool bSet = GetType() == sfx2::LINKUPDATE_ALWAYS ? TRUE : FALSE;
            rVal.setValue(&bSet, ::getBooleanCppuType());
        }
        break;
    case FIELD_PROP_PAR5:
        rVal <<= ::rtl::OUString(aExpansion);
    break;
    default:
        DBG_ERROR("illegal property");
    }
    if( nPart )
        rVal <<= OUString(GetCmd().GetToken(nPart-1, sfx2::cTokenSeperator));
    return TRUE;
}
/* -----------------------------28.08.00 16:23--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwDDEFieldType::PutValue( const uno::Any& rVal, USHORT nWhichId )
{
    BYTE nPart = 0;
    switch( nWhichId )
    {
    case FIELD_PROP_PAR2:      nPart = 3; break;
    case FIELD_PROP_PAR4:      nPart = 2; break;
    case FIELD_PROP_SUBTYPE:   nPart = 1; break;
    case FIELD_PROP_BOOL1:
        SetType( static_cast<USHORT>(*(sal_Bool*)rVal.getValue() ?
                                     sfx2::LINKUPDATE_ALWAYS :
                                     sfx2::LINKUPDATE_ONCALL ) );
        break;
    case FIELD_PROP_PAR5:
    {
        ::rtl::OUString sTemp;
        rVal >>= sTemp;
        aExpansion = sTemp;
    }
    break;
    default:
        DBG_ERROR("illegal property");
    }
    if( nPart )
    {
        String sTmp, sCmd( GetCmd() );
        while(3 > sCmd.GetTokenCount(sfx2::cTokenSeperator))
            sCmd += sfx2::cTokenSeperator;
        sCmd.SetToken( nPart-1, sfx2::cTokenSeperator, ::GetString( rVal, sTmp ) );
        SetCmd( sCmd );
    }
    return TRUE;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwDDEField::SwDDEField( SwDDEFieldType* pInitType )
    : SwField(pInitType)
{
}

SwDDEField::~SwDDEField()
{
    if( GetTyp()->IsLastDepend() )  					// der Letzte mach das
        ((SwDDEFieldType*)GetTyp())->Disconnect();		// Licht aus
}

String SwDDEField::Expand() const
{
    xub_StrLen nPos;
    String aStr( ((SwDDEFieldType*)GetTyp())->GetExpansion() );

    aStr.EraseAllChars( '\r' );
    while( (nPos = aStr.Search( '\t' )) != STRING_NOTFOUND )
        aStr.SetChar( nPos, ' ' );
    while( (nPos = aStr.Search( '\n' )) != STRING_NOTFOUND )
        aStr.SetChar( nPos, '|' );
    if( aStr.Len() && ( aStr.GetChar( aStr.Len()-1 ) == '|') )
        aStr.Erase( aStr.Len()-1, 1 );
    return aStr;
}

SwField* SwDDEField::Copy() const
{
    return new SwDDEField((SwDDEFieldType*)GetTyp());
}

/*--------------------------------------------------------------------
    Beschreibung: Parameter des Typen erfragen
                  Name
 --------------------------------------------------------------------*/
const String& SwDDEField::GetPar1() const
{
    return ((SwDDEFieldType*)GetTyp())->GetName();
}

/*--------------------------------------------------------------------
    Beschreibung: Parameter des Typen erfragen
                  Commando
 --------------------------------------------------------------------*/
String SwDDEField::GetPar2() const
{
    return ((SwDDEFieldType*)GetTyp())->GetCmd();
}

void SwDDEField::SetPar2(const String& rStr)
{
    ((SwDDEFieldType*)GetTyp())->SetCmd(rStr);
}

