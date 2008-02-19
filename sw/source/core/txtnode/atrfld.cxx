/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: atrfld.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: rt $ $Date: 2008-02-19 13:48:10 $
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

#include "fldbas.hxx"          // fuer FieldType

#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _DOCUFLD_HXX //autogen
#include <docufld.hxx>
#endif

#include "doc.hxx"
#include "docsh.hxx"
#include "../../../inc/PostItMgr.hxx"
#include "../../ui/inc/view.hxx"


#include "reffld.hxx"
#include "ddefld.hxx"
#include "usrfld.hxx"
#include "expfld.hxx"
#include "swfont.hxx"       // fuer GetFldsColor
#include "ndtxt.hxx"        // SwTxtNode
#include "calc.hxx"         // Update fuer UserFields
#include "hints.hxx"
#include <IDocumentFieldsAccess.hxx>

#include <svtools/smplhint.hxx>

TYPEINIT2( SwFmtFld, SfxPoolItem, SwClient )
TYPEINIT1(SwFmtFldHint, SfxHint);

/****************************************************************************
 *
 *  class SwFmtFld
 *
 ****************************************************************************/

    // Konstruktor fuers Default vom Attribut-Pool
SwFmtFld::SwFmtFld()
    : SfxPoolItem( RES_TXTATR_FIELD ),
    SwClient( 0 ),
    pField( 0 ),
    pTxtAttr( 0 )
{
}

SwFmtFld::SwFmtFld( const SwField &rFld )
    : SfxPoolItem( RES_TXTATR_FIELD ),
    SwClient( rFld.GetTyp() ),
    pTxtAttr( 0 )
{
    pField = rFld.Copy();
}

// #i24434#
// Since Items are used in ItemPool and in default constructed ItemSets with
// full pool range, all items need to be clonable. Thus, this one needed to be
// corrected
SwFmtFld::SwFmtFld( const SwFmtFld& rAttr )
    : SfxPoolItem( RES_TXTATR_FIELD ), SwClient(), SfxBroadcaster(),
    pField( 0 ),
    pTxtAttr( 0 )
{
    if(rAttr.GetFld())
    {
        rAttr.GetFld()->GetTyp()->Add(this);
        pField = rAttr.GetFld()->Copy();
    }
}

SwFmtFld::~SwFmtFld()
{
    SwFieldType* pType = pField ? pField->GetTyp() : 0;

    if (pType && pType->Which() == RES_DBFLD)
        pType = 0;	// DB-Feldtypen zerstoeren sich selbst

    Broadcast( SwFmtFldHint( this, SWFMTFLD_REMOVED ) );
    delete pField;

    // bei einige FeldTypen muessen wir den FeldTypen noch loeschen
    if( pType && pType->IsLastDepend() )
    {
        BOOL bDel = FALSE;
        switch( pType->Which() )
        {
        case RES_USERFLD:
            bDel = ((SwUserFieldType*)pType)->IsDeleted();
            break;

        case RES_SETEXPFLD:
            bDel = ((SwSetExpFieldType*)pType)->IsDeleted();
            break;

        case RES_DDEFLD:
            bDel = ((SwDDEFieldType*)pType)->IsDeleted();
            break;
        }

        if( bDel )
        {
            // vorm loeschen erstmal austragen
            pType->Remove( this );
            delete pType;
        }
    }
}

// #111840#
void SwFmtFld::SetFld(SwField * _pField)
{
    if (NULL != pField)
        delete pField;

    pField = _pField;
    Broadcast( SwFmtFldHint( this, SWFMTFLD_CHANGED ) );
}

int SwFmtFld::operator==( const SfxPoolItem& rAttr ) const
{
    ASSERT( SfxPoolItem::operator==( rAttr ), "keine gleichen Attribute" );
    // OD 2004-05-14 #i29146# - correction: check, if <pField> and
    // <((SwFmtFld&)rAttr).GetFld()> are set.
    // OD 2004-05-14 #i29146# - items are equal, if both fields aren't set.
    return ( pField && ((SwFmtFld&)rAttr).GetFld() &&
             pField->GetTyp() == ((SwFmtFld&)rAttr).GetFld()->GetTyp() &&
             pField->GetFormat() == ((SwFmtFld&)rAttr).GetFld()->GetFormat() ) ||
           ( !pField && !((SwFmtFld&)rAttr).GetFld() );
}

SfxPoolItem* SwFmtFld::Clone( SfxItemPool* ) const
{
    return new SwFmtFld( *this );
}

void SwFmtFld::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    if( !pTxtAttr )
        return;

    SwTxtNode* pTxtNd = (SwTxtNode*)&pTxtAttr->GetTxtNode();
    ASSERT( pTxtNd, "wo ist denn mein Node?" );
    if( pNew )
    {
        switch( pNew->Which() )
        {
        case RES_TXTATR_FLDCHG:
                // "Farbe hat sich geaendert !"
                // this, this fuer "nur Painten"
                pTxtNd->Modify( this, this );
                return;
        case RES_REFMARKFLD_UPDATE:
                // GetReferenz-Felder aktualisieren
                if( RES_GETREFFLD == GetFld()->GetTyp()->Which() )
                    ((SwGetRefField*)GetFld())->UpdateField();
                break;
        case RES_DOCPOS_UPDATE:
                // Je nach DocPos aktualisieren (SwTxtFrm::Modify())
                pTxtNd->Modify( pNew, this );
                return;

        case RES_ATTRSET_CHG:
        case RES_FMT_CHG:
                pTxtNd->Modify( pOld, pNew );
                return;
        }
    }

    switch (GetFld()->GetTyp()->Which())
    {
        case RES_HIDDENPARAFLD:
            if( !pOld || RES_HIDDENPARA_PRINT != pOld->Which() )
                break;
        case RES_DBSETNUMBERFLD:
        case RES_DBNUMSETFLD:
        case RES_DBNEXTSETFLD:
        case RES_DBNAMEFLD:
            pTxtNd->Modify( 0, pNew);
            return;
    }

    if( RES_USERFLD == GetFld()->GetTyp()->Which() )
    {
        SwUserFieldType* pType = (SwUserFieldType*)GetFld()->GetTyp();
        if(!pType->IsValid())
        {
            SwCalc aCalc( *pTxtNd->GetDoc() );
            pType->GetValue( aCalc );
        }
    }
    pTxtAttr->Expand();
}

BOOL SwFmtFld::GetInfo( SfxPoolItem& rInfo ) const
{
    const SwTxtNode* pTxtNd;
    if( RES_AUTOFMT_DOCNODE != rInfo.Which() ||
        !pTxtAttr || 0 == ( pTxtNd = pTxtAttr->GetpTxtNode() ) ||
        &pTxtNd->GetNodes() != ((SwAutoFmtGetDocNode&)rInfo).pNodes )
        return TRUE;

    ((SwAutoFmtGetDocNode&)rInfo).pCntntNode = pTxtNd;
    return FALSE;
}


BOOL SwFmtFld::IsFldInDoc() const
{
    const SwTxtNode* pTxtNd;
    return pTxtAttr && 0 != ( pTxtNd = pTxtAttr->GetpTxtNode() ) &&
            pTxtNd->GetNodes().IsDocNodes();
}

BOOL SwFmtFld::IsProtect() const
{
    const SwTxtNode* pTxtNd;
    return pTxtAttr && 0 != ( pTxtNd = pTxtAttr->GetpTxtNode() ) &&
            pTxtNd->IsProtect();
}

/*************************************************************************
|*
|*                SwTxtFld::SwTxtFld()
|*
|*    Beschreibung      Attribut fuer automatischen Text, Ctor
|*    Ersterstellung    BP 30.04.92
|*    Letzte Aenderung	JP 15.08.94
|*
*************************************************************************/

SwTxtFld::SwTxtFld( const SwFmtFld& rAttr, xub_StrLen nStartPos )
    : SwTxtAttr( rAttr, nStartPos ),
    aExpand( rAttr.GetFld()->Expand() ),
    pMyTxtNd( 0 )
{
    ((SwFmtFld&)rAttr).pTxtAttr = this;
}

SwTxtFld::~SwTxtFld( )
{
}

/*************************************************************************
|*
|*                SwTxtFld::Expand()
|*
|*    Beschreibung      exandiert das Feld und tauscht den Text im Node
|*    Ersterstellung    BP 30.04.92
|*    Letzte Aenderung	JP 15.08.94
|*
*************************************************************************/

void SwTxtFld::Expand() const
{
    // Wenn das expandierte Feld sich nicht veraendert hat, wird returnt
    ASSERT( pMyTxtNd, "wo ist denn mein Node?" );

    const SwField* pFld = GetFld().GetFld();
    XubString aNewExpand( pFld->Expand() );

    if( aNewExpand == aExpand )
    {
        // Bei Seitennummernfeldern
        const USHORT nWhich = pFld->GetTyp()->Which();
        if( RES_CHAPTERFLD != nWhich && RES_PAGENUMBERFLD != nWhich &&
            RES_REFPAGEGETFLD != nWhich &&
            // --> FME 2005-05-23 #122919# Page count fields to not use aExpand
            // during formatting, therefore an invalidation of the text frame
            // has to be triggered even if aNewExpand == aExpand:
            ( RES_DOCSTATFLD != nWhich || DS_PAGE != static_cast<const SwDocStatField*>(pFld)->GetSubType() ) &&
            // <--
            ( RES_GETEXPFLD != nWhich || ((SwGetExpField*)pFld)->IsInBodyTxt() ) )
        {
            // BP: das muesste man noch optimieren!
            //JP 12.06.97: stimmt, man sollte auf jedenfall eine Status-
            //				aenderung an die Frames posten
            if( pMyTxtNd->CalcHiddenParaField() )
                pMyTxtNd->Modify( 0, 0 );
            return;
        }
    }

    aExpand = aNewExpand;

    // 0, this fuer Formatieren
    pMyTxtNd->Modify( 0, (SfxPoolItem*)&GetFld() );
}

/*************************************************************************
 *                      SwTxtFld::CopyFld()
 *************************************************************************/

void SwTxtFld::CopyFld( SwTxtFld *pDest ) const
{
    ASSERT( pMyTxtNd, "wo ist denn mein Node?" );
    ASSERT( pDest->pMyTxtNd, "wo ist denn mein Node?" );

    IDocumentFieldsAccess* pIDFA = pMyTxtNd->getIDocumentFieldsAccess();
    IDocumentFieldsAccess* pDestIDFA = pDest->pMyTxtNd->getIDocumentFieldsAccess();

    SwFmtFld& rFmtFld = (SwFmtFld&)pDest->GetFld();
    const USHORT nFldWhich = rFmtFld.GetFld()->GetTyp()->Which();

    if( pIDFA != pDestIDFA )
    {
        // Die Hints stehen in unterschiedlichen Dokumenten,
        // der Feldtyp muss im neuen Dokument angemeldet werden.
        // Z.B: Kopieren ins ClipBoard.
        SwFieldType* pFldType;
        if( nFldWhich != RES_DBFLD && nFldWhich != RES_USERFLD &&
            nFldWhich != RES_SETEXPFLD && nFldWhich != RES_DDEFLD &&
            RES_AUTHORITY != nFldWhich )
            pFldType = pDestIDFA->GetSysFldType( nFldWhich );
        else
            pFldType = pDestIDFA->InsertFldType( *rFmtFld.GetFld()->GetTyp() );

        // Sonderbehandlung fuer DDE-Felder
        if( RES_DDEFLD == nFldWhich )
        {
            if( rFmtFld.GetTxtFld() )
                ((SwDDEFieldType*)rFmtFld.GetFld()->GetTyp())->DecRefCnt();
            ((SwDDEFieldType*)pFldType)->IncRefCnt();
        }

        ASSERT( pFldType, "unbekannter FieldType" );
        pFldType->Add( &rFmtFld );          // ummelden
        rFmtFld.GetFld()->ChgTyp( pFldType );
    }

    // Expressionfelder Updaten
    if( nFldWhich == RES_SETEXPFLD || nFldWhich == RES_GETEXPFLD ||
        nFldWhich == RES_HIDDENTXTFLD )
    {
        SwTxtFld* pFld = (SwTxtFld*)this;
        pDestIDFA->UpdateExpFlds( pFld, true );
    }
    // Tabellenfelder auf externe Darstellung
    else if( RES_TABLEFLD == nFldWhich &&
        ((SwTblField*)rFmtFld.GetFld())->IsIntrnlName() )
    {
        // erzeuge aus der internen (fuer CORE) die externe (fuer UI) Formel
        const SwTableNode* pTblNd = pMyTxtNd->FindTableNode();
        if( pTblNd )		// steht in einer Tabelle
            ((SwTblField*)rFmtFld.GetFld())->PtrToBoxNm( &pTblNd->GetTable() );
    }
}


