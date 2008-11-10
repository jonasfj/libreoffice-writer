/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: expfld.cxx,v $
 * $Revision: 1.35 $
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
#include <unotools/collatorwrapper.hxx>
#include <unotools/charclass.hxx>
#include <svx/unolingu.hxx>
#include <svx/pageitem.hxx>
#include <svx/langitem.hxx>
#include <svx/fontitem.hxx>
#include <com/sun/star/text/SetVariableType.hpp>
#include <unofield.hxx>
#include <fmtfld.hxx>
#include <txtfld.hxx>
#include <fmtanchr.hxx>
#include <txtftn.hxx>
#include <doc.hxx>
#include <layfrm.hxx>
#include <pagefrm.hxx>
#include <cntfrm.hxx>
#include <rootfrm.hxx>
#include <tabfrm.hxx>
#include <flyfrm.hxx>
#include <ftnfrm.hxx>
#include <rowfrm.hxx>
#include <expfld.hxx>
#include <usrfld.hxx>
#include <ndtxt.hxx>
#include <calc.hxx>
#include <pam.hxx>
#include <docfld.hxx>
#include <swcache.hxx>
#include <swtable.hxx>
#include <breakit.hxx>
#include <SwStyleNameMapper.hxx>
#ifndef _UNOFLDMID_H
#include <unofldmid.h>
#endif
#include <numrule.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::text;
using ::rtl::OUString;

SV_IMPL_PTRARR( _SwSeqFldList, _SeqFldLstElem* )

//-----------------------------------------------------------------------------
sal_Int16 lcl_SubTypeToAPI(USHORT nSubType)
{
        sal_Int16 nRet = 0;
        switch(nSubType)
        {
            case nsSwGetSetExpType::GSE_EXPR	:  	nRet = SetVariableType::VAR /*0*/; break;
            case nsSwGetSetExpType::GSE_SEQ	    :	nRet = SetVariableType::SEQUENCE /*1*/; break;
            case nsSwGetSetExpType::GSE_FORMULA :	nRet = SetVariableType::FORMULA /*2*/; break;
            case nsSwGetSetExpType::GSE_STRING  : 	nRet = SetVariableType::STRING /*3*/; break;
        }
        return nRet;
}
//-----------------------------------------------------------------------------
sal_Int32 lcl_APIToSubType(const uno::Any& rAny)
{
        sal_Int16 nVal = 0;
        rAny >>= nVal;
        sal_Int32 nSet = 0;
        switch(nVal)
        {
            case SetVariableType::VAR:		nSet = nsSwGetSetExpType::GSE_EXPR;  break;
            case SetVariableType::SEQUENCE: nSet = nsSwGetSetExpType::GSE_SEQ;  break;
            case SetVariableType::FORMULA:  nSet = nsSwGetSetExpType::GSE_FORMULA; break;
            case SetVariableType::STRING:	nSet = nsSwGetSetExpType::GSE_STRING;	break;
            default:
                DBG_ERROR("wrong value");
                nSet = -1;
        }
        return nSet;
}

//-----------------------------------------------------------------------------

void ReplacePoint( String& rTmpName, BOOL bWithCommandType )
{
    // replace first and last (if bWithCommandType: last two) dot Ersten und letzten Punkt ersetzen, da in Tabellennamen Punkte erlaubt sind
    // since table names may contain dots

    xub_StrLen nLen = rTmpName.Len();
    sal_Unicode *pStr = rTmpName.GetBufferAccess(), *pBackStr = pStr + nLen;

    long nBackCount = bWithCommandType ? 2 : 1;
    xub_StrLen i;

    for( i = nLen; i; --i, pBackStr-- )
        if( '.' == *pBackStr )
        {
            *pBackStr = DB_DELIM;
            if(!--nBackCount)
                break;
        }
    for( i = 0; i < nLen; ++i, ++pStr )
        if( '.' == *pStr )
        {
            *pStr = DB_DELIM;
            break;
        }
}

SwTxtNode* GetFirstTxtNode( const SwDoc& rDoc, SwPosition& rPos,
                            const SwCntntFrm *pCFrm, Point &rPt )
{
    SwTxtNode* pTxtNode = 0;
    if ( !pCFrm )
    {
        const SwNodes& rNodes = rDoc.GetNodes();
        rPos.nNode = *rNodes.GetEndOfContent().StartOfSectionNode();
        SwCntntNode* pCNd;
        while( 0 != (pCNd = rNodes.GoNext( &rPos.nNode ) ) &&
                0 == ( pTxtNode = pCNd->GetTxtNode() ) )
                        ;
        ASSERT( pTxtNode, "wo ist der 1.TextNode" );
        rPos.nContent.Assign( pTxtNode, 0 );
    }
    else if ( !pCFrm->IsValid() )
    {
        pTxtNode = (SwTxtNode*)pCFrm->GetNode();
        rPos.nNode = *pTxtNode;
        rPos.nContent.Assign( pTxtNode, 0 );
    }
    else
    {
        pCFrm->GetCrsrOfst( &rPos, rPt );
        pTxtNode = rPos.nNode.GetNode().GetTxtNode();
    }
    return pTxtNode;
}

const SwTxtNode* GetBodyTxtNode( const SwDoc& rDoc, SwPosition& rPos,
                                const SwFrm& rFrm )
{
    const SwLayoutFrm* pLayout = (SwLayoutFrm*)rFrm.GetUpper();
    const SwTxtNode* pTxtNode = 0;

    while( pLayout )
    {
        if( pLayout->IsFlyFrm() )
        {
            // hole das FlyFormat
            SwFrmFmt* pFlyFmt = ((SwFlyFrm*)pLayout)->GetFmt();
            ASSERT( pFlyFmt, "kein FlyFormat gefunden, wo steht das Feld" );

            const SwFmtAnchor &rAnchor = pFlyFmt->GetAnchor();

            if( FLY_AT_FLY == rAnchor.GetAnchorId() )
            {
                // und der Fly muss irgendwo angehaengt sein, also
                // den befragen
                pLayout = (SwLayoutFrm*)((SwFlyFrm*)pLayout)->GetAnchorFrm();
                continue;
            }
            else if( FLY_AT_CNTNT == rAnchor.GetAnchorId() ||
                     FLY_AUTO_CNTNT == rAnchor.GetAnchorId() ||
                     FLY_IN_CNTNT == rAnchor.GetAnchorId() )
            {
                ASSERT( rAnchor.GetCntntAnchor(), "keine gueltige Position" );
                rPos = *rAnchor.GetCntntAnchor();
                pTxtNode = rPos.nNode.GetNode().GetTxtNode();
                if( FLY_AT_CNTNT == rAnchor.GetAnchorId() )
                    ((SwTxtNode*)pTxtNode)->MakeStartIndex( &rPos.nContent );
// oder doch besser das Ende vom (Anker-)TextNode nehmen ??
//					((SwTxtNode*)pTxtNode)->MakeEndIndex( &rPos.nContent );

                // noch nicht abbrechen, kann ja auch noch im
                // Header/Footer/Footnote/Fly stehen !!
                pLayout = ((SwFlyFrm*)pLayout)->GetAnchorFrm()
                            ? ((SwFlyFrm*)pLayout)->GetAnchorFrm()->GetUpper() : 0;
                continue;
            }
            else
            {
                pLayout->FindPageFrm()->GetCntntPosition(
                                                pLayout->Frm().Pos(), rPos );
                pTxtNode = rPos.nNode.GetNode().GetTxtNode();
            }
        }
        else if( pLayout->IsFtnFrm() )
        {
            // hole den Node vom Anker
            const SwTxtFtn* pFtn = ((SwFtnFrm*)pLayout)->GetAttr();
            pTxtNode = &pFtn->GetTxtNode();
            rPos.nNode = *pTxtNode;
            rPos.nContent = *pFtn->GetStart();
        }
        else if( pLayout->IsHeaderFrm() || pLayout->IsFooterFrm() )
        {
            const SwCntntFrm* pCntFrm;
            const SwPageFrm* pPgFrm = pLayout->FindPageFrm();
            if( pLayout->IsHeaderFrm() )
            {
                const SwTabFrm *pTab;
                if( 0 != ( pCntFrm = pPgFrm->FindFirstBodyCntnt()) &&
                    0 != (pTab = pCntFrm->FindTabFrm()) && pTab->IsFollow() &&
                    pTab->GetTable()->GetRowsToRepeat() > 0 &&
                    pTab->IsInHeadline( *pCntFrm ) )
                {
                    // take the next line
                    const SwLayoutFrm* pRow = pTab->GetFirstNonHeadlineRow();
                    pCntFrm = pRow->ContainsCntnt();
                }
            }
            else
                pCntFrm = pPgFrm->FindLastBodyCntnt();

            if( pCntFrm )
            {
                pTxtNode = pCntFrm->GetNode()->GetTxtNode();
                rPos.nNode = *pTxtNode;
                ((SwTxtNode*)pTxtNode)->MakeEndIndex( &rPos.nContent );
            }
            else
            {
                Point aPt( pLayout->Frm().Pos() );
                aPt.Y()++;		// aus dem Header raus
                pCntFrm = pPgFrm->GetCntntPos( aPt, FALSE, TRUE, FALSE );
                pTxtNode = GetFirstTxtNode( rDoc, rPos, pCntFrm, aPt );
            }
        }
        else
        {
            pLayout = pLayout->GetUpper();
            continue;
        }
        break;		// gefunden und beende die Schleife
    }
    return pTxtNode;
}

/*--------------------------------------------------------------------
    Beschreibung: SwSetExpFieldType by JP
 --------------------------------------------------------------------*/

SwGetExpFieldType::SwGetExpFieldType(SwDoc* pDc)
    : SwValueFieldType( pDc, RES_GETEXPFLD )
{
}

SwFieldType* SwGetExpFieldType::Copy() const
{
    return new SwGetExpFieldType(GetDoc());
}

void SwGetExpFieldType::Modify( SfxPoolItem*, SfxPoolItem* pNew )
{
    if( pNew && RES_DOCPOS_UPDATE == pNew->Which() )
        SwModify::Modify( 0, pNew );
    // sonst nichts weiter expandieren
}

/*--------------------------------------------------------------------
    Beschreibung: SwGetExpField by JP
 --------------------------------------------------------------------*/

SwGetExpField::SwGetExpField(SwGetExpFieldType* pTyp, const String& rFormel,
                            USHORT nSub, ULONG nFmt)
    : SwFormulaField( pTyp, nFmt, 0.0 ),
    bIsInBodyTxt( TRUE ),
    nSubType(nSub),
    bLateInitialization( false )
{
    SetFormula( rFormel );
}

String SwGetExpField::Expand() const
{
    if(nSubType & nsSwExtendedSubType::SUB_CMD)
        return GetFormula();
    else
        return sExpand;
}

String SwGetExpField::GetCntnt(BOOL bName) const
{
    if ( bName )
    {
        String aStr( SwFieldType::GetTypeStr( static_cast<USHORT>(nsSwGetSetExpType::GSE_FORMULA & nSubType
                                                ? TYP_FORMELFLD
                                                : TYP_GETFLD ) ));
        aStr += ' ';
        aStr += GetFormula();
        return aStr;
    }
    return Expand();
}

SwField* SwGetExpField::Copy() const
{
    SwGetExpField *pTmp = new SwGetExpField((SwGetExpFieldType*)GetTyp(),
                                            GetFormula(), nSubType, GetFormat());
    pTmp->SetLanguage(GetLanguage());
    pTmp->SwValueField::SetValue(GetValue());
    pTmp->sExpand 		= sExpand;
    pTmp->bIsInBodyTxt 	= bIsInBodyTxt;
    pTmp->SetAutomaticLanguage(IsAutomaticLanguage());
    if( bLateInitialization )
        pTmp->SetLateInitialization();

    return pTmp;
}

void SwGetExpField::ChangeExpansion( const SwFrm& rFrm, const SwTxtFld& rFld )
{
    if( bIsInBodyTxt )		// nur Felder in Footer, Header, FootNote, Flys
        return;

    ASSERT( !rFrm.IsInDocBody(), "Flag ist nicht richtig, Frame steht im DocBody" );

    // bestimme mal das Dokument (oder geht es noch einfacher?)
    const SwTxtNode* pTxtNode = &rFld.GetTxtNode();
    SwDoc& rDoc = *(SwDoc*)pTxtNode->GetDoc();

    // einen Index fuers bestimmen vom TextNode anlegen
    SwPosition aPos( SwNodeIndex( rDoc.GetNodes() ) );
    pTxtNode = GetBodyTxtNode( rDoc, aPos, rFrm );

    // Wenn kein Layout vorhanden, kommt es in Kopf und Fusszeilen dazu
    // das ChnageExpansion uebers Layout-Formatieren aufgerufen wird
    // aber kein TxtNode vorhanden ist
    //
    if(!pTxtNode)
        return;
    // #i82544# 
    if( bLateInitialization )
    {
        SwFieldType* pSetExpFld = rDoc.GetFldType(RES_SETEXPFLD, GetFormula(), sal_False);
        if( pSetExpFld )
        {
            bLateInitialization = false;
            if( !(GetSubType() & nsSwGetSetExpType::GSE_STRING) &&
                static_cast< SwSetExpFieldType* >(pSetExpFld)->GetType() == nsSwGetSetExpType::GSE_STRING )
            SetSubType( nsSwGetSetExpType::GSE_STRING );
        }
    }
    
    _SetGetExpFld aEndFld( aPos.nNode, &rFld, &aPos.nContent );
    if(GetSubType() & nsSwGetSetExpType::GSE_STRING)
    {
        SwHash** ppHashTbl;
        USHORT nSize;
        rDoc.FldsToExpand( ppHashTbl, nSize, aEndFld );
        LookString( ppHashTbl, nSize, GetFormula(), sExpand );
        ::DeleteHashTable( ppHashTbl, nSize );		// HashTabelle loeschen
    }
    else
    {
        // fuelle den Calculator mit den Werten
        SwCalc aCalc( rDoc );
        rDoc.FldsToCalc(aCalc, aEndFld);

        // Wert berechnen
        SetValue(aCalc.Calculate(GetFormula()).GetDouble());

        // Auswertung nach Format
        sExpand = ((SwValueFieldType*)GetTyp())->ExpandValue(
                                GetValue(), GetFormat(), GetLanguage());
    }
}

String SwGetExpField::GetPar2() const
{
    return GetFormula();
}

void SwGetExpField::SetPar2(const String& rStr)
{
    SetFormula(rStr);
}

USHORT SwGetExpField::GetSubType() const
{
    return nSubType;
}

void SwGetExpField::SetSubType(USHORT nType)
{
    nSubType = nType;
}

void SwGetExpField::SetLanguage(USHORT nLng)
{
    if (nSubType & nsSwExtendedSubType::SUB_CMD)
        SwField::SetLanguage(nLng);
    else
        SwValueField::SetLanguage(nLng);
}

/*-----------------07.03.98 16:08-------------------

--------------------------------------------------*/
BOOL SwGetExpField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_DOUBLE:
        rAny <<= GetValue();
        break;
    case FIELD_PROP_FORMAT:
        rAny <<= (sal_Int32)GetFormat();
        break;
    case FIELD_PROP_USHORT1:
         rAny <<= (sal_Int16)nSubType;
        break;
    case FIELD_PROP_PAR1:
         rAny <<= OUString( GetFormula() );
        break;
    case FIELD_PROP_SUBTYPE:
        {
            sal_Int16 nRet = lcl_SubTypeToAPI(GetSubType() & 0xff);
            rAny <<= nRet;
        }
        break;
    case FIELD_PROP_BOOL2:
        {
            BOOL bTmp = 0 != (nSubType & nsSwExtendedSubType::SUB_CMD);
            rAny.setValue(&bTmp, ::getBooleanCppuType());
        }
        break;
    case FIELD_PROP_PAR4:
        rAny <<= rtl::OUString(GetExpStr());
        break;
    default:
        return SwField::QueryValue(rAny, nWhichId);
    }
    return TRUE;
}
/*-----------------07.03.98 16:08-------------------

--------------------------------------------------*/
BOOL SwGetExpField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    sal_Int32 nTmp = 0;
    String sTmp;
    switch( nWhichId )
    {
    case FIELD_PROP_DOUBLE:
        SwValueField::SetValue(*(double*) rAny.getValue());
        break;
    case FIELD_PROP_FORMAT:
        rAny >>= nTmp;
        SetFormat(nTmp);
        break;
    case FIELD_PROP_USHORT1:
         rAny >>= nTmp;
         nSubType = static_cast<USHORT>(nTmp);
        break;
    case FIELD_PROP_PAR1:
         SetFormula( ::GetString( rAny, sTmp ));
        break;
    case FIELD_PROP_SUBTYPE:
        nTmp = lcl_APIToSubType(rAny);
        if( nTmp >=0 )
            SetSubType( static_cast<USHORT>((GetSubType() & 0xff00) | nTmp));
        break;
    case FIELD_PROP_BOOL2:
        if(*(sal_Bool*) rAny.getValue())
            nSubType |= nsSwExtendedSubType::SUB_CMD;
        else
            nSubType &= (~nsSwExtendedSubType::SUB_CMD);
        break;
    case FIELD_PROP_PAR4:
        ChgExpStr(::GetString( rAny, sTmp ));
        break;
    default:
        return SwField::PutValue(rAny, nWhichId);
    }
    return TRUE;
}

/*-----------------JP: 17.06.93 -------------------
 Set-Expression-Type
 --------------------------------------------------*/

SwSetExpFieldType::SwSetExpFieldType( SwDoc* pDc, const String& rName, USHORT nTyp )
    : SwValueFieldType( pDc, RES_SETEXPFLD ),
    sName( rName ),
    pOutlChgNd( 0 ),
    sDelim( String::CreateFromAscii( "." ) ),
    nType(nTyp), nLevel( UCHAR_MAX ),
    bDeleted( FALSE )
{
    if( ( nsSwGetSetExpType::GSE_SEQ | nsSwGetSetExpType::GSE_STRING ) & nType )
        EnableFormat(FALSE);	// Numberformatter nicht einsetzen
}

SwFieldType* SwSetExpFieldType::Copy() const
{
    SwSetExpFieldType* pNew = new SwSetExpFieldType(GetDoc(), sName, nType);
    pNew->bDeleted = bDeleted;
    pNew->sDelim = sDelim;
    pNew->nLevel = nLevel;

    return pNew;
}

const String& SwSetExpFieldType::GetName() const
{
    return sName;
}

void SwSetExpFieldType::Modify( SfxPoolItem*, SfxPoolItem* )
{
    return;		// nicht weiter expandieren
}

void SwSetExpFieldType::SetSeqFormat(ULONG nFmt)
{
    SwClientIter aIter(*this);
    for( SwFmtFld* pFld = (SwFmtFld*)aIter.First( TYPE(SwFmtFld) );
            pFld; pFld = (SwFmtFld*)aIter.Next() )
        pFld->GetFld()->ChangeFormat( nFmt );
}

ULONG SwSetExpFieldType::GetSeqFormat()
{
    if( !GetDepends() )
        return SVX_NUM_ARABIC;

    SwField *pFld = ((SwFmtFld*)GetDepends())->GetFld();
    return pFld->GetFormat();
}

USHORT SwSetExpFieldType::SetSeqRefNo( SwSetExpField& rFld )
{
    if( !GetDepends() || !(nsSwGetSetExpType::GSE_SEQ & nType) )
        return USHRT_MAX;

extern void InsertSort( SvUShorts& rArr, USHORT nIdx, USHORT* pInsPos = 0 );
    SvUShorts aArr( 64 );

    USHORT n;

    // dann testmal, ob die Nummer schon vergeben ist oder ob eine neue
    // bestimmt werden muss.
    SwClientIter aIter( *this );
    const SwTxtNode* pNd;
    for( SwFmtFld* pF = (SwFmtFld*)aIter.First( TYPE( SwFmtFld )); pF;
            pF = (SwFmtFld*)aIter.Next() )
        if( pF->GetFld() != &rFld && pF->GetTxtFld() &&
            0 != ( pNd = pF->GetTxtFld()->GetpTxtNode() ) &&
            pNd->GetNodes().IsDocNodes() )
            InsertSort( aArr, ((SwSetExpField*)pF->GetFld())->GetSeqNumber() );


    // teste erstmal ob die Nummer schon vorhanden ist:
    USHORT nNum = rFld.GetSeqNumber();
    if( USHRT_MAX != nNum )
    {
        for( n = 0; n < aArr.Count(); ++n )
            if( aArr[ n ] > nNum )
                return nNum;			// nicht vorhanden -> also benutzen
            else if( aArr[ n ] == nNum )
                break;					// schon vorhanden -> neue erzeugen

        if( n == aArr.Count() )
            return nNum;			// nicht vorhanden -> also benutzen
    }

    // alle Nummern entsprechend geflag, also bestimme die richtige Nummer
    for( n = 0; n < aArr.Count(); ++n )
        if( n != aArr[ n ] )
            break;

    rFld.SetSeqNumber( n );
    return n;
}

USHORT SwSetExpFieldType::GetSeqFldList( SwSeqFldList& rList )
{
    if( rList.Count() )
        rList.Remove( 0, rList.Count() );

    SwClientIter aIter( *this );
    const SwTxtNode* pNd;
    for( SwFmtFld* pF = (SwFmtFld*)aIter.First( TYPE( SwFmtFld )); pF;
            pF = (SwFmtFld*)aIter.Next() )
        if( pF->GetTxtFld() &&
            0 != ( pNd = pF->GetTxtFld()->GetpTxtNode() ) &&
            pNd->GetNodes().IsDocNodes() )
        {
            _SeqFldLstElem* pNew = new _SeqFldLstElem(
                    pNd->GetExpandTxt( 0, (*pF->GetTxtFld()->GetStart()) + 1 ),
                    ((SwSetExpField*)pF->GetFld())->GetSeqNumber() );
            rList.InsertSort( pNew );
        }

    return rList.Count();
}


void SwSetExpFieldType::SetChapter( SwSetExpField& rFld, const SwNode& rNd )
{
    const SwTxtNode* pTxtNd = rNd.FindOutlineNodeOfLevel( nLevel );
    if( pTxtNd )
    {
        SwNumRule * pRule = pTxtNd->GetNumRule();

        if (pRule)
        {
            // --> OD 2005-11-02 #i51089 - TUNING#
            if ( pTxtNd->GetNum() )
            {
                const SwNodeNum & aNum = *(pTxtNd->GetNum());

                // nur die Nummer besorgen, ohne Pre-/Post-fixstrings
                String sNumber( pRule->MakeNumString(aNum, FALSE ));

                if( sNumber.Len() )
                    rFld.ChgExpStr(  ( sNumber += sDelim ) += rFld.GetExpStr() );
            }
            else
            {
                ASSERT( false,
                        "<SwSetExpFieldType::SetChapter(..)> - text node with numbering rule, but without number. This is a serious defect -> inform OD" );
            }
        }
    }
}

/* -----------------24.03.99 09:44-------------------
 *
 * --------------------------------------------------*/
BOOL SwSetExpFieldType::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_SUBTYPE:
        {
            sal_Int16 nRet = lcl_SubTypeToAPI(GetType());
            rAny <<= nRet;
        }
        break;
    case FIELD_PROP_PAR2:
        rAny <<= OUString(GetDelimiter());
        break;
    case FIELD_PROP_SHORT1:
        {
            sal_Int8 nRet = nLevel < MAXLEVEL? nLevel : -1;
            rAny <<= nRet;
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return TRUE;
}

BOOL SwSetExpFieldType::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_SUBTYPE:
        {
            sal_Int32 nSet = lcl_APIToSubType(rAny);
            if(nSet >=0)
                SetType(static_cast<USHORT>(nSet));
        }
        break;
    case FIELD_PROP_PAR2:
        {
            String sTmp;
            if( ::GetString( rAny, sTmp ).Len() )
//				SetDelimiter( sTmp.GetChar( 0 ));
                SetDelimiter( sTmp );
            else
                SetDelimiter(String::CreateFromAscii( " "));
        }
        break;
    case FIELD_PROP_SHORT1:
        {
            sal_Int8 nLvl = 0;
            rAny >>= nLvl;
            if(nLvl < 0 || nLvl >= MAXLEVEL)
                SetOutlineLvl(UCHAR_MAX);
            else
                SetOutlineLvl(nLvl);
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return TRUE;
}

BOOL SwSeqFldList::InsertSort( _SeqFldLstElem* pNew )
{
    sal_Unicode* p = pNew->sDlgEntry.GetBufferAccess();
    while( *p )
    {
        if( *p < 0x20 )
            *p = 0x20;
        ++p;
    }

    USHORT nPos;
    BOOL bRet = SeekEntry( *pNew, &nPos );
    if( !bRet )
        C40_INSERT( _SeqFldLstElem, pNew, nPos );
    return bRet;
}

BOOL SwSeqFldList::SeekEntry( const _SeqFldLstElem& rNew, USHORT* pP )
{
    USHORT nO = Count(), nM, nU = 0;
    if( nO > 0 )
    {
        CollatorWrapper & rCaseColl = ::GetAppCaseCollator(),
                        & rColl = ::GetAppCollator();
        const CharClass& rCC = GetAppCharClass();

        //#59900# Die Sortierung soll die Nummer korrekt einordnen
        //also "10" nach "9" und nicht "10" nach "1"
        const String& rTmp2 = rNew.sDlgEntry;
        xub_StrLen nFndPos2 = 0;
        String sNum2( rTmp2.GetToken( 0, ' ', nFndPos2 ));
        BOOL bIsNum2IsNumeric = rCC.isAsciiNumeric( sNum2 );
        sal_Int32 nNum2 = bIsNum2IsNumeric ? sNum2.ToInt32() : 0;

        nO--;
        while( nU <= nO )
        {
            nM = nU + ( nO - nU ) / 2;

            //#59900# Die Sortierung soll die Nummer korrekt einordnen
            //also "10" nach "9" und nicht "10" nach "1"
            const String& rTmp1 = (*((_SeqFldLstElem**)pData + nM))->sDlgEntry;
            xub_StrLen nFndPos1 = 0;
            String sNum1( rTmp1.GetToken( 0, ' ', nFndPos1 ));
            sal_Int32 nCmp;

            if( bIsNum2IsNumeric && rCC.isNumeric( sNum1 ) )
            {
                sal_Int32 nNum1 = sNum1.ToInt32();
                nCmp = nNum2 - nNum1;
                if( 0 == nCmp )
                    nCmp = rCaseColl.compareString( rTmp2.Copy( nFndPos2 ),
                                                       rTmp1.Copy( nFndPos1 ));
            }
            else
                nCmp = rColl.compareString( rTmp2, rTmp1 );

            if( 0 == nCmp )
            {
                if( pP ) *pP = nM;
                return TRUE;
            }
            else if( 0 < nCmp )
                nU = nM + 1;
            else if( nM == 0 )
                break;
            else
                nO = nM - 1;
        }
    }
    if( pP ) *pP = nU;
    return FALSE;
}

/*--------------------------------------------------------------------
    Beschreibung: SwSetExpField by JP
 --------------------------------------------------------------------*/

SwSetExpField::SwSetExpField(SwSetExpFieldType* pTyp, const String& rFormel,
                                        ULONG nFmt)
    : SwFormulaField( pTyp, nFmt, 0.0 ), nSeqNo( USHRT_MAX ),
    nSubType(0)
{
    SetFormula(rFormel);
    // SubType ignorieren !!!
    bInput = FALSE;
    if( IsSequenceFld() )
    {
        SwValueField::SetValue(1.0);
        if( !rFormel.Len() )
        {
            String sFormel(rFormel);
            sFormel += pTyp->GetName();
            sFormel += '+';
            sFormel += '1';
            SetFormula(sFormel);
        }
    }
}

String SwSetExpField::Expand() const
{
    String aStr;
    if (nSubType & nsSwExtendedSubType::SUB_CMD)
    {	// Der CommandString ist gefragt
        aStr = GetTyp()->GetName();
        aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " = " ));
        aStr += GetFormula();
    }
    else if(!(nSubType & nsSwExtendedSubType::SUB_INVISIBLE))
    {   // Der Wert ist sichtbar
        aStr = sExpand;
    }
    return aStr;
}

/*--------------------------------------------------------------------
    Beschreibung: liefert den Namen oder den Inhalt
 --------------------------------------------------------------------*/

String SwSetExpField::GetCntnt(BOOL bName) const
{
    if( bName )
    {
        USHORT nStrType;

        if( IsSequenceFld() )
            nStrType = TYP_SEQFLD;
        else if( bInput )
            nStrType = TYP_SETINPFLD;
        else
            nStrType = TYP_SETFLD;

        String aStr( SwFieldType::GetTypeStr( nStrType ) );
        aStr += ' ';
        aStr += GetTyp()->GetName();

        if( TYP_SEQFLD != nStrType )
        {
            // Sequence nicht die Formel ausgeben
            aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " = " ));
            aStr += GetFormula();
        }
        return aStr;
    }
    return Expand();
}

SwField* SwSetExpField::Copy() const
{
    SwSetExpField *pTmp = new SwSetExpField((SwSetExpFieldType*)GetTyp(),
                                            GetFormula(), GetFormat());
    pTmp->SwValueField::SetValue(GetValue());
    pTmp->sExpand   	= sExpand;
    pTmp->SetAutomaticLanguage(IsAutomaticLanguage());
    pTmp->SetLanguage(GetLanguage());
    pTmp->aPText		= aPText;
    pTmp->bInput		= bInput;
    pTmp->nSeqNo		= nSeqNo;
    pTmp->SetSubType(GetSubType());

    return pTmp;
}

void SwSetExpField::SetSubType(USHORT nSub)
{
    ((SwSetExpFieldType*)GetTyp())->SetType(nSub & 0xff);
    nSubType = nSub & 0xff00;

    DBG_ASSERT( (nSub & 0xff) != 3, "SubType ist illegal!" );
}

USHORT SwSetExpField::GetSubType() const
{
    return ((SwSetExpFieldType*)GetTyp())->GetType() | nSubType;
}

void SwSetExpField::SetValue( const double& rAny )
{
    SwValueField::SetValue(rAny);

    if( IsSequenceFld() )
        sExpand = FormatNumber( (USHORT)GetValue(), GetFormat() );
    else
        sExpand = ((SwValueFieldType*)GetTyp())->ExpandValue( rAny,
                                                GetFormat(), GetLanguage());
}

void SwGetExpField::SetValue( const double& rAny )
{
    SwValueField::SetValue(rAny);
    sExpand = ((SwValueFieldType*)GetTyp())->ExpandValue( rAny, GetFormat(),
                                                            GetLanguage());
}
/* -----------------14.07.99 12:21-------------------
    Description: Find the index of the reference text
    following the current field
 --------------------------------------------------*/
xub_StrLen SwGetExpField::GetReferenceTextPos( const SwFmtFld& rFmt, SwDoc& rDoc)
{
    //
    const SwTxtFld* pTxtFld = rFmt.GetTxtFld();
    const SwTxtNode& rTxtNode = pTxtFld->GetTxtNode();
    //
    xub_StrLen nRet = *pTxtFld->GetStart() + 1;
    String sNodeText = rTxtNode.GetTxt();
    sNodeText.Erase(0, nRet);
    if(sNodeText.Len())
    {
        //now check if sNodeText starts with a non-alphanumeric character plus a blank
        USHORT nSrcpt = pBreakIt->GetRealScriptOfText( sNodeText, 0 );

        static USHORT nIds[] =
        {
            RES_CHRATR_LANGUAGE, RES_CHRATR_LANGUAGE,
            RES_CHRATR_FONT, RES_CHRATR_FONT,
            RES_CHRATR_CJK_LANGUAGE, RES_CHRATR_CJK_LANGUAGE,
            RES_CHRATR_CJK_FONT, RES_CHRATR_CJK_FONT,
            RES_CHRATR_CTL_LANGUAGE, RES_CHRATR_CTL_LANGUAGE,
            RES_CHRATR_CTL_FONT, RES_CHRATR_CTL_FONT,
            0, 0
        };
        SwAttrSet aSet(rDoc.GetAttrPool(), nIds);
        rTxtNode.GetAttr(aSet, nRet, nRet+1);

        if( RTL_TEXTENCODING_SYMBOL != ((SvxFontItem&)aSet.Get(
                GetWhichOfScript( RES_CHRATR_FONT, nSrcpt )) ).GetCharSet() )
        {
            LanguageType eLang = ((SvxLanguageItem&)aSet.Get(
                GetWhichOfScript( RES_CHRATR_LANGUAGE, nSrcpt )) ).GetLanguage();
            CharClass aCC( SvxCreateLocale( eLang ));
            sal_Unicode c0 = sNodeText.GetChar(0);
            BOOL bIsAlphaNum = aCC.isAlphaNumeric( sNodeText, 0 );
            if( !bIsAlphaNum ||
                (c0 == ' ' || c0 == '\t'))
            {
                nRet++;
                if( sNodeText.Len() > 1 &&
                    (sNodeText.GetChar(1) == ' ' ||
                     sNodeText.GetChar(1) == '\t'))
                    nRet++;
            }
        }
    }
    return nRet;
}


/*--------------------------------------------------------------------
    Beschreibung: Parameter setzen
 --------------------------------------------------------------------*/

const String& SwSetExpField::GetPar1() const
{
    return ((SwSetExpFieldType*)GetTyp())->GetName();
}

String SwSetExpField::GetPar2() const
{
    USHORT nType = ((SwSetExpFieldType*)GetTyp())->GetType();

    if (nType & nsSwGetSetExpType::GSE_STRING)
        return GetFormula();
    return GetExpandedFormula();
}

void SwSetExpField::SetPar2(const String& rStr)
{
    USHORT nType = ((SwSetExpFieldType*)GetTyp())->GetType();

    if( !(nType & nsSwGetSetExpType::GSE_SEQ) || rStr.Len() )
    {
        if (nType & nsSwGetSetExpType::GSE_STRING)
            SetFormula(rStr);
        else
            SetExpandedFormula(rStr);
    }
}

/*--------------------------------------------------------------------
    Beschreibung: Eingabefeld Type
 ---------------------------------------------------------------------*/

SwInputFieldType::SwInputFieldType( SwDoc* pD )
    : SwFieldType( RES_INPUTFLD ), pDoc( pD )
{
}

SwFieldType* SwInputFieldType::Copy() const
{
    SwInputFieldType* pType = new SwInputFieldType( pDoc );
    return pType;
}

/*--------------------------------------------------------------------
    Beschreibung: Eingabefeld
 --------------------------------------------------------------------*/

SwInputField::SwInputField(SwInputFieldType* pTyp, const String& rContent,
                           const String& rPrompt, USHORT nSub, ULONG nFmt) :
    SwField(pTyp, nFmt), aContent(rContent), aPText(rPrompt), nSubType(nSub)
{
}

String SwInputField::GetCntnt(BOOL bName) const
{
    if ( bName )
    {
        String aStr(SwField::GetCntnt(bName));
        if ((nSubType & 0x00ff) == INP_USR)
        {
            aStr += GetTyp()->GetName();
            aStr += ' ';
            aStr += aContent;
        }
        return aStr;
    }
    return Expand();
}

SwField* SwInputField::Copy() const
{
    SwInputField* pFld = new SwInputField((SwInputFieldType*)GetTyp(), aContent,
                                          aPText, GetSubType(), GetFormat());

    pFld->SetHelp(aHelp);
    pFld->SetToolTip(aToolTip);

    pFld->SetAutomaticLanguage(IsAutomaticLanguage());
    return pFld;
}

String SwInputField::Expand() const
{
    String sRet;
    if((nSubType & 0x00ff) == INP_TXT)
        sRet = aContent;

    else if( (nSubType & 0x00ff) == INP_USR )
    {
        SwUserFieldType* pUserTyp = (SwUserFieldType*)
                            ((SwInputFieldType*)GetTyp())->GetDoc()->
                            GetFldType( RES_USERFLD, aContent, false );
        if( pUserTyp )
            sRet = pUserTyp->GetContent();
    }
    return sRet;
}

/*-----------------06.03.98 11:12-------------------

--------------------------------------------------*/
BOOL SwInputField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
         rAny <<= OUString( aContent );
        break;
    case FIELD_PROP_PAR2:
        rAny <<= OUString( aPText );
        break;
    case FIELD_PROP_PAR3:
        rAny <<= OUString( aHelp );
        break;
    case FIELD_PROP_PAR4:
        rAny <<= OUString( aToolTip );
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return TRUE;
}
/*-----------------06.03.98 11:12-------------------

--------------------------------------------------*/
BOOL SwInputField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
         ::GetString( rAny, aContent );
        break;
    case FIELD_PROP_PAR2:
        ::GetString( rAny, aPText );
        break;
    case FIELD_PROP_PAR3:
        ::GetString( rAny, aHelp );
        break;
    case FIELD_PROP_PAR4:
        ::GetString( rAny, aToolTip );
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return TRUE;
}
/*--------------------------------------------------------------------
    Beschreibung: Bedingung setzen
 --------------------------------------------------------------------*/

void SwInputField::SetPar1(const String& rStr)
{
    aContent = rStr;
}

const String& SwInputField::GetPar1() const
{
    return aContent;
}

/*--------------------------------------------------------------------
    Beschreibung: True/False Text
 --------------------------------------------------------------------*/

void SwInputField::SetPar2(const String& rStr)
{
    aPText = rStr;
}

String SwInputField::GetPar2() const
{
    return aPText;
}

void SwInputField::SetHelp(const String & rStr)
{
    aHelp = rStr;
}

String SwInputField::GetHelp() const
{
    return aHelp;
}

void SwInputField::SetToolTip(const String & rStr)
{
    aToolTip = rStr;
}

String SwInputField::GetToolTip() const
{
    return aToolTip;
}

BOOL SwInputField::isFormField() const
{
    return aHelp.Len() > 0 || aToolTip.Len() > 0;
}

USHORT SwInputField::GetSubType() const
{
    return nSubType;
}

void SwInputField::SetSubType(USHORT nSub)
{
    nSubType = nSub;
}
/*-----------------05.03.98 17:22-------------------

--------------------------------------------------*/
BOOL SwSetExpField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_BOOL2:
        {
            sal_Bool bVal = 0 == (nSubType & nsSwExtendedSubType::SUB_INVISIBLE);
            rAny.setValue(&bVal, ::getBooleanCppuType());
        }
        break;
    case FIELD_PROP_FORMAT:
        rAny <<= (sal_Int32)GetFormat();
        break;
    case FIELD_PROP_USHORT2:
        rAny <<= (sal_Int16)GetFormat();
        break;
    case FIELD_PROP_USHORT1:
        rAny <<= (sal_Int16)nSeqNo;
        break;
    case FIELD_PROP_PAR1:
        rAny <<= OUString ( SwStyleNameMapper::GetProgName(GetPar1(), nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL ) );
        break;
    case FIELD_PROP_PAR2:
        {
            //I18N - if the formula contains only "TypeName+1"
            //and it's one of the initially created sequence fields
            //then the localized names has to be replaced by a programmatic name
            OUString sMyFormula = SwXFieldMaster::LocalizeFormula(*this, GetFormula(), TRUE);
            rAny <<= OUString( sMyFormula );
        }
        break;
    case FIELD_PROP_DOUBLE:
        rAny <<= (double)GetValue();
        break;
    case FIELD_PROP_SUBTYPE:
        {
            sal_Int16 nRet = 0;
                nRet = lcl_SubTypeToAPI(GetSubType() & 0xff);
            rAny <<= nRet;
        }
        break;
    case FIELD_PROP_PAR3:
        rAny <<= OUString( aPText );
        break;
    case FIELD_PROP_BOOL3:
        {
            BOOL bTmp = 0 != (nSubType & nsSwExtendedSubType::SUB_CMD);
            rAny.setValue(&bTmp, ::getBooleanCppuType());
        }
        break;
    case FIELD_PROP_BOOL1:
        {
            BOOL bTmp = GetInputFlag();
            rAny.setValue(&bTmp, ::getBooleanCppuType());
        }
        break;
    case FIELD_PROP_PAR4:
        rAny <<= rtl::OUString(GetExpStr());
        break;
    default:
        return SwField::QueryValue(rAny, nWhichId);
    }
    return TRUE;
}
/*-----------------05.03.98 17:22-------------------

--------------------------------------------------*/
BOOL SwSetExpField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    sal_Int32 nTmp32 = 0;
    sal_Int16 nTmp16 = 0;
    String sTmp;
    switch( nWhichId )
    {
    case FIELD_PROP_BOOL2:
        if(*(sal_Bool*)rAny.getValue())
            nSubType &= ~nsSwExtendedSubType::SUB_INVISIBLE;
        else
            nSubType |= nsSwExtendedSubType::SUB_INVISIBLE;
        break;
    case FIELD_PROP_FORMAT:
        rAny >>= nTmp32;
        SetFormat(nTmp32);
        break;
    case FIELD_PROP_USHORT2:
        {
            rAny >>= nTmp16;
            if(nTmp16 <= SVX_NUMBER_NONE )
                SetFormat(nTmp16);
            else {
                //exception(wrong_value)
                ;
            }
        }
        break;
    case FIELD_PROP_USHORT1:
        rAny >>= nTmp16;
        nSeqNo = nTmp16;
        break;
    case FIELD_PROP_PAR1:
        SetPar1( SwStyleNameMapper::GetUIName(
                            ::GetString( rAny, sTmp ), nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL ) );
        break;
    case FIELD_PROP_PAR2:
        {
            OUString uTmp;
            rAny >>= uTmp;
            //I18N - if the formula contains only "TypeName+1"
            //and it's one of the initially created sequence fields
            //then the localized names has to be replaced by a programmatic name
            OUString sMyFormula = SwXFieldMaster::LocalizeFormula(*this, uTmp, FALSE);
            SetFormula( sMyFormula );
        }
        break;
    case FIELD_PROP_DOUBLE:
        {
             double fVal;
             rAny >>= fVal;
             SetValue(fVal);
        }
        break;
    case FIELD_PROP_SUBTYPE:
        nTmp32 = lcl_APIToSubType(rAny);
        if(nTmp32 >= 0)
            SetSubType(static_cast<USHORT>((GetSubType() & 0xff00) | nTmp32));
        break;
    case FIELD_PROP_PAR3:
        ::GetString( rAny, aPText );
        break;
    case FIELD_PROP_BOOL3:
        if(*(sal_Bool*) rAny.getValue())
            nSubType |= nsSwExtendedSubType::SUB_CMD;
        else
            nSubType &= (~nsSwExtendedSubType::SUB_CMD);
        break;
    case FIELD_PROP_BOOL1:
        SetInputFlag(*(sal_Bool*) rAny.getValue());
        break;
    case FIELD_PROP_PAR4:
        ChgExpStr( ::GetString( rAny, sTmp ));
        break;
    default:
        return SwField::PutValue(rAny, nWhichId);
    }
    return TRUE;
}



