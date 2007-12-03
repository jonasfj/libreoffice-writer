/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docufld.cxx,v $
 *
 *  $Revision: 1.48 $
 *
 *  last change: $Author: ihi $ $Date: 2007-12-03 14:25:25 $
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


#include <tools/pstm.hxx>

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _COM_SUN_STAR_SCRIPT_XTYPECONVERTER_HPP_
#include <com/sun/star/script/XTypeConverter.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_SETVARIABLETYPE_HPP_
#include <com/sun/star/text/SetVariableType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELDSSUPPLIER_HPP_
#include <com/sun/star/text/XTextFieldsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_USERDATAPART_HPP_
#include <com/sun/star/text/UserDataPart.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_CHAPTERFORMAT_HPP_
#include <com/sun/star/text/ChapterFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELD_HPP_
#include <com/sun/star/text/XTextField.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_PLACEHOLDERTYPE_HPP_
#include <com/sun/star/text/PlaceholderType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TEMPLATEDISPLAYFORMAT_HPP_
#include <com/sun/star/text/TemplateDisplayFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_USERFIELDFORMAT_HPP_
#include <com/sun/star/text/UserFieldFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_PAGENUMBERTYPE_HPP_
#include <com/sun/star/text/PageNumberType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_REFERENCEFIELDPART_HPP_
#include <com/sun/star/text/ReferenceFieldPart.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_FilenameDisplayFormat_HPP_
#include <com/sun/star/text/FilenameDisplayFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XDEPENDENTTEXTFIELD_HPP_
#include <com/sun/star/text/XDependentTextField.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_DOCUMENTSTATISTIC_HPP_
#include <com/sun/star/text/DocumentStatistic.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATE_HPP_
#include <com/sun/star/util/Date.hpp>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _UNO_LINGU_HXX
#include <svx/unolingu.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#include <comphelper/types.hxx>

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _APP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif

#include <sfx2/docinf.hxx>

#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFXDOCTEMPL_HXX //autogen
#include <sfx2/doctempl.hxx>
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#ifndef _DOCSTAT_HXX //autogen
#include <docstat.hxx>
#endif
#ifndef _PAGEDESC_HXX //autogen
#include <pagedesc.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _ROOTFRM_HXX
#include <rootfrm.hxx>		// AuthorField
#endif
#ifndef _PAGEFRM_HXX
#include <pagefrm.hxx>		//
#endif
#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>		//
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _VIEWSH_HXX
#include <viewsh.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _SHELLRES_HXX
#include <shellres.hxx>
#endif
#ifndef _DOCUFLD_HXX
#include <docufld.hxx>
#endif
#ifndef _FLDDAT_HXX
#include <flddat.hxx>
#endif
#ifndef _DOCFLD_HXX
#include <docfld.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _UNOFLDMID_H
#include <unofldmid.h>
#endif
#ifndef _SWUNOHELPER_HXX
#include <swunohelper.hxx>
#endif
#ifndef _COMCORE_HRC
#include <comcore.hrc>
#endif

#define URL_DECODE 	INetURLObject::DECODE_UNAMBIGUOUS

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::rtl;
using namespace nsSwDocInfoSubType;

/*--------------------------------------------------------------------
    Beschreibung: SwPageNumberFieldType
 --------------------------------------------------------------------*/

SwPageNumberFieldType::SwPageNumberFieldType()
    : SwFieldType( RES_PAGENUMBERFLD ),
    nNumberingType( SVX_NUM_ARABIC ),
    nNum( 0 ),
    nMax( USHRT_MAX ),
    bVirtuell( sal_False )
{
}

String& SwPageNumberFieldType::Expand( sal_uInt32 nFmt, short nOff,
                                const String& rUserStr, String& rRet ) const
{
    sal_uInt32 nTmpFmt = (SVX_NUM_PAGEDESC == nFmt) ? (sal_uInt32)nNumberingType : nFmt;
    long nTmp = nNum + nOff;

    if( 0 >= nTmp || SVX_NUM_NUMBER_NONE == nTmpFmt || (!bVirtuell && nTmp > nMax) )
        rRet = aEmptyStr;
    else if( SVX_NUM_CHAR_SPECIAL == nTmpFmt )
        rRet = rUserStr;
    else
        rRet = FormatNumber( (sal_uInt16)nTmp, nTmpFmt );
    return rRet;
}

SwFieldType* SwPageNumberFieldType::Copy() const
{
    SwPageNumberFieldType *pTmp = new SwPageNumberFieldType();

    pTmp->nNum 		 = nNum;
    pTmp->nMax 		 = nMax;
    pTmp->nNumberingType = nNumberingType;
    pTmp->bVirtuell  = bVirtuell;

    return pTmp;
}

/*--------------------------------------------------------------------
    Beschreibung: Verschiedene Expandierung
 --------------------------------------------------------------------*/

void SwPageNumberFieldType::ChangeExpansion( SwDoc* pDoc, sal_uInt16 nPage,
                                            sal_uInt16 nNumPages, sal_Bool bVirt,
                                            const sal_Int16* pNumFmt )
{
    nNum = nPage;
    nMax = nNumPages;
    if( pNumFmt )
        nNumberingType = *pNumFmt;

    bVirtuell = sal_False;
    if( bVirt )
    {
        // dann muss das Flag ueberprueft werden, denn das Layout setzt
        // es NIE zurueck
        const SfxItemPool &rPool = pDoc->GetAttrPool();
        const SwFmtPageDesc *pDesc;
        sal_uInt16 nMaxItems = rPool.GetItemCount( RES_PAGEDESC );
        for( sal_uInt16 n = 0; n < nMaxItems; ++n )
            if( 0 != (pDesc = (SwFmtPageDesc*)rPool.GetItem( RES_PAGEDESC, n ) )
                && pDesc->GetNumOffset() && pDesc->GetDefinedIn() )
            {
                if( pDesc->GetDefinedIn()->ISA( SwCntntNode ))
                {
                    SwClientIter aIter( *(SwModify*)pDesc->GetDefinedIn() );
                    if( aIter.First( TYPE( SwFrm ) ) )
                    {
                        bVirtuell = sal_True;
                        break;
                    }
                }
                else if( pDesc->GetDefinedIn()->ISA( SwFmt ))
                {
                    SwAutoFmtGetDocNode aGetHt( &pDoc->GetNodes() );
                    bVirtuell = !pDesc->GetDefinedIn()->GetInfo( aGetHt );
                    break;
                }
            }
    }
}

/*--------------------------------------------------------------------
    Beschreibung: SwPageNumberField
 --------------------------------------------------------------------*/

SwPageNumberField::SwPageNumberField(SwPageNumberFieldType* pTyp,
                                     sal_uInt16 nSub, sal_uInt32 nFmt, short nOff)
    : SwField(pTyp, nFmt), nSubType(nSub), nOffset(nOff)
{
}

String SwPageNumberField::Expand() const
{
    String sRet;
    SwPageNumberFieldType* pFldType = (SwPageNumberFieldType*)GetTyp();

    if( PG_NEXT == nSubType && 1 != nOffset )
    {
        if( pFldType->Expand( GetFormat(), 1, sUserStr, sRet ).Len() )
            pFldType->Expand( GetFormat(), nOffset, sUserStr, sRet );
    }
    else if( PG_PREV == nSubType && -1 != nOffset )
    {
        if( pFldType->Expand( GetFormat(), -1, sUserStr, sRet ).Len() )
            pFldType->Expand( GetFormat(), nOffset, sUserStr, sRet );
    }
    else
        pFldType->Expand( GetFormat(), nOffset, sUserStr, sRet );
    return sRet;
}

SwField* SwPageNumberField::Copy() const
{
    SwPageNumberField *pTmp =
        new SwPageNumberField((SwPageNumberFieldType*)GetTyp(), nSubType, GetFormat(), nOffset);
    pTmp->SetLanguage( GetLanguage() );
    pTmp->SetUserString( sUserStr );
    return pTmp;
}

String SwPageNumberField::GetPar2() const
{
    return String::CreateFromInt32(nOffset);
}

void SwPageNumberField::SetPar2(const String& rStr)
{
    nOffset = (short)rStr.ToInt32();
}

sal_uInt16 SwPageNumberField::GetSubType() const
{
    return nSubType;
}

/*-----------------05.03.98 10:25-------------------

--------------------------------------------------*/
BOOL SwPageNumberField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_FORMAT:
        rAny <<= (sal_Int16)GetFormat();
        break;
    case FIELD_PROP_USHORT1:
        rAny <<= nOffset;
        break;
    case FIELD_PROP_SUBTYPE:
        {
             text::PageNumberType eType;
            eType = text::PageNumberType_CURRENT;
            if(nSubType == PG_PREV)
                eType = text::PageNumberType_PREV;
            else if(nSubType == PG_NEXT)
                eType = text::PageNumberType_NEXT;
            rAny.setValue(&eType, ::getCppuType((const text::PageNumberType*)0));
        }
        break;
    case FIELD_PROP_PAR1:
        rAny <<= OUString(sUserStr);
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 10:25-------------------

--------------------------------------------------*/
BOOL SwPageNumberField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    BOOL bRet = TRUE;
    sal_Int16 nSet = 0;
    switch( nWhichId )
    {
    case FIELD_PROP_FORMAT:
        rAny >>= nSet;

        // TODO: woher kommen die defines?
        if(nSet <= SVX_NUM_PAGEDESC )
            SetFormat(nSet);
        else
            //exception(wrong_value)
            ;
        break;
    case FIELD_PROP_USHORT1:
        rAny >>= nSet;
        nOffset = nSet;
        break;
    case FIELD_PROP_SUBTYPE:
        switch( SWUnoHelper::GetEnumAsInt32( rAny ) )
        {
            case text::PageNumberType_CURRENT:
                nSubType = PG_RANDOM;
            break;
            case text::PageNumberType_PREV:
                nSubType = PG_PREV;
            break;
            case text::PageNumberType_NEXT:
                nSubType = PG_NEXT;
            break;
            default:
                bRet = FALSE;
        }
        break;
    case FIELD_PROP_PAR1:
        ::GetString( rAny, sUserStr );
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return bRet;
}
/*--------------------------------------------------------------------
    Beschreibung: SwAuthorFieldType
 --------------------------------------------------------------------*/

SwAuthorFieldType::SwAuthorFieldType()
    : SwFieldType( RES_AUTHORFLD )
{
}

String SwAuthorFieldType::Expand(ULONG nFmt) const
{
    String sRet;
    SvtUserOptions&  rOpt = SW_MOD()->GetUserOptions();
    if((nFmt & 0xff) == AF_NAME)
        sRet = rOpt.GetFullName();
    else
        sRet = rOpt.GetID();
    return sRet;
}

SwFieldType* SwAuthorFieldType::Copy() const
{
    return new SwAuthorFieldType;
}

/*--------------------------------------------------------------------
    Beschreibung: SwAuthorField
 --------------------------------------------------------------------*/

SwAuthorField::SwAuthorField(SwAuthorFieldType* pTyp, sal_uInt32 nFmt)
    : SwField(pTyp, nFmt)
{
    aContent = ((SwAuthorFieldType*)GetTyp())->Expand(GetFormat());
}

String SwAuthorField::Expand() const
{
    if (!IsFixed())
        ((SwAuthorField*)this)->aContent =
                    ((SwAuthorFieldType*)GetTyp())->Expand(GetFormat());

    return aContent;
}

SwField* SwAuthorField::Copy() const
{
    SwAuthorField *pTmp = new SwAuthorField( (SwAuthorFieldType*)GetTyp(),
                                                GetFormat());
    pTmp->SetExpansion(aContent);
    return pTmp;
}

/*-----------------05.03.98 11:15-------------------

--------------------------------------------------*/
BOOL SwAuthorField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    sal_Bool bVal;
    switch( nWhichId )
    {
    case FIELD_PROP_BOOL1:
        bVal = (GetFormat() & 0xff) == AF_NAME;
        rAny.setValue(&bVal, ::getBooleanCppuType());
        break;

    case FIELD_PROP_BOOL2:
        bVal = IsFixed();
        rAny.setValue(&bVal, ::getBooleanCppuType());
        break;

    case FIELD_PROP_PAR1:
        rAny <<= rtl::OUString(GetContent());
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 11:15-------------------

--------------------------------------------------*/
BOOL SwAuthorField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_BOOL1:
        SetFormat( *(sal_Bool*)rAny.getValue() ? AF_NAME : AF_SHORTCUT );
        break;

    case FIELD_PROP_BOOL2:
        if( *(sal_Bool*)rAny.getValue() )
            SetFormat( GetFormat() | AF_FIXED);
        else
            SetFormat( GetFormat() & ~AF_FIXED);
        break;

    case FIELD_PROP_PAR1:
        ::GetString( rAny, aContent );
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}

/*--------------------------------------------------------------------
    Beschreibung: SwFileNameFieldType
 --------------------------------------------------------------------*/

SwFileNameFieldType::SwFileNameFieldType(SwDoc *pDocument)
    : SwFieldType( RES_FILENAMEFLD )
{
    pDoc = pDocument;
}

String SwFileNameFieldType::Expand(ULONG nFmt) const
{
    String aRet;
    const SwDocShell* pDShell = pDoc->GetDocShell();
    if( pDShell && pDShell->HasName() )
    {
        const INetURLObject& rURLObj = pDShell->GetMedium()->GetURLObject();
        switch( nFmt & ~FF_FIXED )
        {
            case FF_PATH:
                {
                    if( INET_PROT_FILE == rURLObj.GetProtocol() )
                    {
                        INetURLObject aTemp(rURLObj);
                        aTemp.removeSegment();
                        // #101947# last slash should belong to the pathname
                        aRet = aTemp.PathToFileName();//GetFull();
                    }
                    else
                    {
                        aRet = URIHelper::removePassword(
                                    rURLObj.GetMainURL( INetURLObject::NO_DECODE ),
                                    INetURLObject::WAS_ENCODED, URL_DECODE );
                        aRet.Erase( aRet.Search( String(rURLObj.GetLastName(
                                                    URL_DECODE )) ) );
                    }
                }
                break;

            case FF_NAME:
                aRet = rURLObj.GetLastName( URL_DECODE );
                break;

            case FF_NAME_NOEXT:
                aRet = rURLObj.GetBase();
                break;

            default:
                if( INET_PROT_FILE == rURLObj.GetProtocol() )
                    aRet = rURLObj.GetFull();
                else
                    aRet = URIHelper::removePassword(
                                    rURLObj.GetMainURL( INetURLObject::NO_DECODE ),
                                    INetURLObject::WAS_ENCODED, URL_DECODE );
        }
    }
    return aRet;
}

SwFieldType* SwFileNameFieldType::Copy() const
{
    SwFieldType *pTmp = new SwFileNameFieldType(pDoc);
    return pTmp;
}
/*--------------------------------------------------------------------
    Beschreibung: SwFileNameField
 --------------------------------------------------------------------*/

SwFileNameField::SwFileNameField(SwFileNameFieldType* pTyp, sal_uInt32 nFmt)
    : SwField(pTyp, nFmt)
{
    aContent = ((SwFileNameFieldType*)GetTyp())->Expand(GetFormat());
}

String SwFileNameField::Expand() const
{
    if (!IsFixed())
        ((SwFileNameField*)this)->aContent = ((SwFileNameFieldType*)GetTyp())->Expand(GetFormat());

    return aContent;
}

SwField* SwFileNameField::Copy() const
{
    SwFileNameField *pTmp =
        new SwFileNameField((SwFileNameFieldType*)GetTyp(), GetFormat());
    pTmp->SetExpansion(aContent);

    return pTmp;
}

/*-----------------05.03.98 08:59-------------------

--------------------------------------------------*/
BOOL SwFileNameField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_FORMAT:
        {
            sal_Int16 nRet;
            switch( GetFormat() &(~FF_FIXED) )
            {
                case FF_PATH:
                    nRet = text::FilenameDisplayFormat::PATH;
                break;
                case FF_NAME_NOEXT:
                    nRet = text::FilenameDisplayFormat::NAME;
                break;
                case FF_NAME:
                    nRet = text::FilenameDisplayFormat::NAME_AND_EXT;
                break;
                default:	nRet = text::FilenameDisplayFormat::FULL;
            }
            rAny <<= nRet;
        }
        break;

    case FIELD_PROP_BOOL2:
        {
            BOOL bVal = IsFixed();
            rAny.setValue(&bVal, ::getBooleanCppuType());
        }
        break;

    case FIELD_PROP_PAR3:
        rAny <<= OUString(GetContent());
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 09:01-------------------

--------------------------------------------------*/
BOOL SwFileNameField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_FORMAT:
        {
            //JP 24.10.2001: int32 because in UnoField.cxx a putvalue is
            //				called with a int32 value! But normally we need
            //				here only a int16
            sal_Int32 nType = 0;
            rAny >>= nType;
            BOOL bFixed = IsFixed();
            switch( nType )
            {
                case text::FilenameDisplayFormat::PATH:
                    nType = FF_PATH;
                break;
                case text::FilenameDisplayFormat::NAME:
                    nType = FF_NAME_NOEXT;
                break;
                case text::FilenameDisplayFormat::NAME_AND_EXT:
                    nType = FF_NAME;
                break;
                default:	nType = FF_PATHNAME;
            }
            if(bFixed)
                nType |= FF_FIXED;
            SetFormat(nType);
        }
        break;

    case FIELD_PROP_BOOL2:
        if( *(sal_Bool*)rAny.getValue() )
            SetFormat( GetFormat() | FF_FIXED);
        else
            SetFormat( GetFormat() & ~FF_FIXED);
        break;

    case FIELD_PROP_PAR3:
        ::GetString( rAny, aContent );
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*--------------------------------------------------------------------
    Beschreibung: SwTemplNameFieldType
 --------------------------------------------------------------------*/

SwTemplNameFieldType::SwTemplNameFieldType(SwDoc *pDocument)
    : SwFieldType( RES_TEMPLNAMEFLD )
{
    pDoc = pDocument;
}

String SwTemplNameFieldType::Expand(ULONG nFmt) const
{
    ASSERT( nFmt < FF_END, "Expand: kein guelt. Fmt!" );

    String aRet;
    const SfxDocumentInfo* pDInfo = pDoc->GetpInfo();

    if( pDInfo )
    {
        if( FF_UI_NAME == nFmt )
            aRet = pDInfo->GetTemplateName();
        else if( pDInfo->GetTemplateFileName().Len() )
        {
            if( FF_UI_RANGE == nFmt )
            {
                // fuers besorgen vom RegionNamen !!
                SfxDocumentTemplates aFac;
                aFac.Construct();
                String sTmp;
                aFac.GetLogicNames( pDInfo->GetTemplateFileName(), aRet, sTmp );
            }
            else
            {
                INetURLObject aPathName( pDInfo->GetTemplateFileName() );
                if( FF_NAME == nFmt )
                    aRet = aPathName.GetName(URL_DECODE);
                else if( FF_NAME_NOEXT == nFmt )
                    aRet = aPathName.GetBase();
                else
                {
                    if( FF_PATH == nFmt )
                    {
                        aPathName.removeSegment();
                        aRet = aPathName.GetFull();
                    }
                    else
                        aRet = aPathName.GetFull();
                }
            }
        }
    }
    return aRet;
}

SwFieldType* SwTemplNameFieldType::Copy() const
{
    SwFieldType *pTmp = new SwTemplNameFieldType(pDoc);
    return pTmp;
}
/*--------------------------------------------------------------------
    Beschreibung: SwTemplNameField
 --------------------------------------------------------------------*/

SwTemplNameField::SwTemplNameField(SwTemplNameFieldType* pTyp, sal_uInt32 nFmt)
    : SwField(pTyp, nFmt)
{}

String SwTemplNameField::Expand() const
{
    return((SwTemplNameFieldType*)GetTyp())->Expand(GetFormat());
}

SwField* SwTemplNameField::Copy() const
{
    SwTemplNameField *pTmp =
        new SwTemplNameField((SwTemplNameFieldType*)GetTyp(), GetFormat());
    return pTmp;
}

/*-----------------05.03.98 08:59-------------------

--------------------------------------------------*/
BOOL SwTemplNameField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch ( nWhichId )
    {
    case FIELD_PROP_FORMAT:
        {
            sal_Int16 nRet;
            switch( GetFormat() )
            {
                case FF_PATH:		nRet = text::FilenameDisplayFormat::PATH; break;
                case FF_NAME_NOEXT: nRet = text::FilenameDisplayFormat::NAME; break;
                case FF_NAME: 		nRet = text::FilenameDisplayFormat::NAME_AND_EXT; break;
                case FF_UI_RANGE: 	nRet = text::TemplateDisplayFormat::AREA; break;
                case FF_UI_NAME:    nRet = text::TemplateDisplayFormat::TITLE;  break;
                default:	nRet = text::FilenameDisplayFormat::FULL;

            }
            rAny <<= nRet;
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 09:01-------------------

--------------------------------------------------*/
BOOL SwTemplNameField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch ( nWhichId )
    {
    case FIELD_PROP_FORMAT:
        {
            //JP 24.10.2001: int32 because in UnoField.cxx a putvalue is
            //				called with a int32 value! But normally we need
            //				here only a int16
            sal_Int32 nType = 0;
            rAny >>= nType;
            switch( nType )
            {
            case text::FilenameDisplayFormat::PATH:
                SetFormat(FF_PATH);
            break;
            case text::FilenameDisplayFormat::NAME:
                SetFormat(FF_NAME_NOEXT);
            break;
            case text::FilenameDisplayFormat::NAME_AND_EXT:
                SetFormat(FF_NAME);
            break;
            case text::TemplateDisplayFormat::AREA	:
                SetFormat(FF_UI_RANGE);
            break;
            case text::TemplateDisplayFormat::TITLE  :
                SetFormat(FF_UI_NAME);
            break;
            default:	SetFormat(FF_PATHNAME);
            }
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*--------------------------------------------------------------------
    Beschreibung: SwDocStatFieldType
 --------------------------------------------------------------------*/

SwDocStatFieldType::SwDocStatFieldType(SwDoc* pDocument)
    : SwFieldType( RES_DOCSTATFLD ), nNumberingType( SVX_NUM_ARABIC )
{
    pDoc = pDocument;
}

String SwDocStatFieldType::Expand(sal_uInt16 nSubType, sal_uInt32 nFmt) const
{
    sal_uInt32 nVal = 0;
    const SwDocStat& rDStat = pDoc->GetDocStat();
    switch( nSubType )
    {
        case DS_TBL:  nVal = rDStat.nTbl;   break;
        case DS_GRF:  nVal = rDStat.nGrf;   break;
        case DS_OLE:  nVal = rDStat.nOLE;   break;
        case DS_PARA: nVal = rDStat.nPara;  break;
        case DS_WORD: nVal = rDStat.nWord;  break;
        case DS_CHAR: nVal = rDStat.nChar;  break;
        case DS_PAGE:
            if( pDoc->GetRootFrm() )
                ((SwDocStat	&)rDStat).nPage = pDoc->GetRootFrm()->GetPageNum();
            nVal = rDStat.nPage;
            if( SVX_NUM_PAGEDESC == nFmt )
                nFmt = (sal_uInt32)nNumberingType;
            break;
        default:
            ASSERT( sal_False, "SwDocStatFieldType::Expand: unbekannter SubType" );
    }

    String sRet;
    if( nVal <= SHRT_MAX )
        sRet = FormatNumber( (sal_uInt16)nVal, nFmt );
    else
        sRet = String::CreateFromInt32( nVal );
    return sRet;
}

SwFieldType* SwDocStatFieldType::Copy() const
{
    SwDocStatFieldType *pTmp = new SwDocStatFieldType(pDoc);
    return pTmp;
}

/*--------------------------------------------------------------------
    Beschreibung: SwDocStatFieldType
                  Aus historischen Gruenden steht in nFormat der
                  SubType
 --------------------------------------------------------------------*/

SwDocStatField::SwDocStatField(SwDocStatFieldType* pTyp, sal_uInt16 nSub, sal_uInt32 nFmt)
    : SwField(pTyp, nFmt),
    nSubType(nSub)
{}

String SwDocStatField::Expand() const
{
    return((SwDocStatFieldType*)GetTyp())->Expand(nSubType, GetFormat());
}

SwField* SwDocStatField::Copy() const
{
    SwDocStatField *pTmp = new SwDocStatField(
                    (SwDocStatFieldType*)GetTyp(), nSubType, GetFormat() );
    return pTmp;
}

sal_uInt16 SwDocStatField::GetSubType() const
{
    return nSubType;
}

void SwDocStatField::SetSubType(sal_uInt16 nSub)
{
    nSubType = nSub;
}

void SwDocStatField::ChangeExpansion( const SwFrm* pFrm )
{
    if( DS_PAGE == nSubType && SVX_NUM_PAGEDESC == GetFormat() )
        ((SwDocStatFieldType*)GetTyp())->SetNumFormat(
                pFrm->FindPageFrm()->GetPageDesc()->GetNumType().GetNumberingType() );
}

/*-----------------05.03.98 11:38-------------------

--------------------------------------------------*/
BOOL SwDocStatField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch ( nWhichId )
    {
    case FIELD_PROP_USHORT2:
        rAny <<= (sal_Int16)GetFormat();
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 11:38-------------------

--------------------------------------------------*/
BOOL SwDocStatField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    BOOL bRet = FALSE;
    switch ( nWhichId )
    {
    case FIELD_PROP_USHORT2:
        {
            sal_Int16 nSet = 0;
            rAny >>= nSet;
            if(nSet <= SVX_NUM_CHARS_LOWER_LETTER_N &&
                nSet != SVX_NUM_CHAR_SPECIAL &&
                    nSet != SVX_NUM_BITMAP)
            {
                SetFormat(nSet);
                bRet = TRUE;
            }
        }
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung: DokumentinfoFields
 --------------------------------------------------------------------*/

SwDocInfoFieldType::SwDocInfoFieldType(SwDoc* pDc)
    : SwValueFieldType( pDc, RES_DOCINFOFLD )
{
}

/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwFieldType* SwDocInfoFieldType::Copy() const
{
    SwDocInfoFieldType* pTyp = new SwDocInfoFieldType(GetDoc());
    return pTyp;
}

void lcl_GetLocalDataWrapper( ULONG nLang,
                              LocaleDataWrapper **ppAppLocalData,
                              LocaleDataWrapper **ppLocalData )
{
    *ppAppLocalData = &GetAppLocaleData();
    *ppLocalData = *ppAppLocalData;
    if( nLang != SvxLocaleToLanguage( (*ppLocalData)->getLocale() ) )
        *ppLocalData = new LocaleDataWrapper(
                        ::comphelper::getProcessServiceFactory(),
                        SvxCreateLocale( static_cast<LanguageType>(nLang) ) );
}

/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwDocInfoFieldType::Expand( sal_uInt16 nSub, sal_uInt32 nFormat,
                                    sal_uInt16 nLang, const String& rName ) const
{
    String aStr;
    LocaleDataWrapper *pAppLocalData = 0, *pLocalData = 0;
    const SfxDocumentInfo*	pInf = GetDoc()->GetDocumentInfo();

    sal_uInt16 nExtSub = nSub & 0xff00;
    nSub &= 0xff;	// ExtendedSubTypes nicht beachten

    switch(nSub)
    {
    case DI_TITEL:	aStr = pInf->GetTitle();	break;
    case DI_THEMA:	aStr = pInf->GetTheme();	break;
    case DI_KEYS:	aStr = pInf->GetKeywords(); break;
    case DI_COMMENT:aStr = pInf->GetComment();	break;
    case DI_INFO1:
    case DI_INFO2:
    case DI_INFO3:
    case DI_INFO4:	aStr = pInf->GetUserKeyWord(nSub - DI_INFO1);break;
    case DI_DOCNO:	aStr = String::CreateFromInt32(
                                                pInf->GetDocumentNumber() );
                    break;
    case DI_EDIT:
        if ( !nFormat )
        {
            lcl_GetLocalDataWrapper( nLang, &pAppLocalData, &pLocalData );
            aStr = pLocalData->getTime( pInf->GetTime(), sal_False, sal_False);
        }
        else
        {
            double fVal = Time(pInf->GetTime()).GetTimeInDays();
            aStr = ExpandValue(fVal, nFormat, nLang);
        }
        break;
    case DI_CUSTOM: 
        {
            ::rtl::OUString sVal;
            try
            {
                uno::Any aAny;
                uno::Reference < beans::XPropertySet > xSet( pInf->GetInfo(), uno::UNO_QUERY );
                aAny = xSet->getPropertyValue( rName );

                uno::Reference < script::XTypeConverter > xConverter( comphelper::getProcessServiceFactory()
                    ->createInstance(::rtl::OUString::createFromAscii("com.sun.star.script.Converter")), uno::UNO_QUERY );
                uno::Any aNew;
                    aNew = xConverter->convertToSimpleType( aAny, uno::TypeClass_STRING );
                aNew >>= sVal;
            }
            catch (uno::Exception&) {}
            return sVal;
        }

    default:
        {
            String aName( pInf->GetAuthor() );
            DateTime aDate( pInf->GetCreationDate() );
            if( nSub == DI_CREATE )
                ;		// das wars schon!!
            else if( nSub == DI_CHANGE &&
                    (pInf->GetModificationDate() != aDate ||
                    (nExtSub & ~DI_SUB_FIXED) == DI_SUB_AUTHOR &&
                    pInf->GetDocumentNumber() > 1) )
            {
                aName = pInf->GetModificationAuthor();
                aDate = pInf->GetModificationDate();
            }
            else if( nSub == DI_PRINT &&
                    pInf->GetPrintDate() != aDate )
            {
                aName = pInf->GetPrintedBy();
                aDate = pInf->GetPrintDate();
            }
            else
                break;

            if (aDate.IsValid())
            {
                switch (nExtSub & ~DI_SUB_FIXED)
                {
                case DI_SUB_AUTHOR:
                    aStr = aName;
                    break;

                case DI_SUB_TIME:
                    if (!nFormat)
                    {
                        lcl_GetLocalDataWrapper( nLang, &pAppLocalData,
                                                        &pLocalData );
                        aStr = pLocalData->getTime( aDate,
                                                    sal_False, sal_False);
                    }
                    else
                    {
                        // Numberformatter anwerfen!
                        double fVal = SwDateTimeField::GetDateTime( GetDoc(),
                                                    aDate);
                        aStr = ExpandValue(fVal, nFormat, nLang);
                    }
                    break;

                case DI_SUB_DATE:
                    if (!nFormat)
                    {
                        lcl_GetLocalDataWrapper( nLang, &pAppLocalData,
                                                 &pLocalData );
                        aStr = pLocalData->getDate( aDate );
                    }
                    else
                    {
                        // Numberformatter anwerfen!
                        double fVal = SwDateTimeField::GetDateTime( GetDoc(),
                                                    aDate);
                        aStr = ExpandValue(fVal, nFormat, nLang);
                    }
                    break;
                }
            }
        }
        break;
    }

    if( pAppLocalData != pLocalData )
        delete pLocalData;

    return aStr;
}

/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwDocInfoField::SwDocInfoField(SwDocInfoFieldType* pTyp, sal_uInt16 nSub, const String& rName, sal_uInt32 nFmt) :
    SwValueField(pTyp, nFmt), nSubType(nSub)
{
    aName = rName;
    aContent = ((SwDocInfoFieldType*)GetTyp())->Expand(nSubType, nFmt, GetLanguage(), aName);
}

/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwDocInfoField::Expand() const
{
    if (!IsFixed())	// aContent fuer Umschaltung auf fixed mitpflegen
        ((SwDocInfoField*)this)->aContent = ((SwDocInfoFieldType*)GetTyp())->Expand(nSubType, GetFormat(), GetLanguage(), aName);

    return aContent;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwDocInfoField::GetCntnt(sal_Bool bName) const
{
    if ( bName )
    {
        String aStr(SwFieldType::GetTypeStr(GetTypeId()));
        aStr += ':';

        sal_uInt16 nSub = nSubType & 0xff;

        switch(nSub)
        {
            case DI_INFO1:
            case DI_INFO2:
            case DI_INFO3:
            case DI_INFO4:
            {
                const SfxDocumentInfo*	pInf = GetDoc()->GetDocumentInfo();
                aStr += pInf->GetUserKeyTitle(nSub - DI_INFO1);
                break;
            }
            case DI_CUSTOM:
                aStr += aName;
                break;

            default:
                aStr += *ViewShell::GetShellRes()->aDocInfoLst[ nSub - DI_SUBTYPE_BEGIN ];
                break;
        }
        if( IsFixed() )
            ( aStr += ' ' ) += ViewShell::GetShellRes()->aFixedStr;
        return aStr;
    }
    return Expand();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwDocInfoField::Copy() const
{
    SwDocInfoField* pFld = new SwDocInfoField((SwDocInfoFieldType*)GetTyp(), nSubType, aName, GetFormat());
    pFld->SetAutomaticLanguage(IsAutomaticLanguage());
    pFld->aContent = aContent;

    return pFld;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
sal_uInt16 SwDocInfoField::GetSubType() const
{
    return nSubType;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwDocInfoField::SetSubType(sal_uInt16 nSub)
{
    nSubType = nSub;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwDocInfoField::SetLanguage(sal_uInt16 nLng)
{
    if (!GetFormat())
        SwField::SetLanguage(nLng);
    else
        SwValueField::SetLanguage(nLng);
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwDocInfoField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        rAny <<= OUString(aContent);
        break;

    case FIELD_PROP_PAR4:
        rAny <<= OUString(aName);
        break;

    case FIELD_PROP_USHORT1:
        rAny  <<= (sal_Int16)aContent.ToInt32();
        break;

    case FIELD_PROP_BOOL1:
        {
            sal_Bool bVal = 0 != (nSubType & DI_SUB_FIXED);
            rAny.setValue(&bVal, ::getBooleanCppuType());
        }
        break;
    case FIELD_PROP_FORMAT:
        rAny  <<= (sal_Int32)GetFormat();
        break;

    case FIELD_PROP_DOUBLE:
        {
            double fVal = GetValue();
            rAny.setValue(&fVal, ::getCppuType(&fVal));
        }
        break;
    case FIELD_PROP_PAR3:
        rAny <<= rtl::OUString(Expand());
        break;
    case FIELD_PROP_BOOL2:
        {
            sal_uInt16 nExtSub = (nSubType & 0xff00) & ~DI_SUB_FIXED;
            sal_Bool bVal = (nExtSub == DI_SUB_DATE);
            rAny.setValue(&bVal, ::getBooleanCppuType());
        }
        break;
    default:
        return SwField::QueryValue(rAny, nWhichId);
    }
    return sal_True;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwDocInfoField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    sal_Int32 nValue = 0;
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        if( nSubType & DI_SUB_FIXED )
            ::GetString( rAny, aContent );
        break;

    case FIELD_PROP_USHORT1:
        if( nSubType & DI_SUB_FIXED )
        {
            rAny >>= nValue;
            aContent = String::CreateFromInt32(nValue);
        }
        break;

    case FIELD_PROP_BOOL1:
        if(*(sal_Bool*)rAny.getValue())
            nSubType |= DI_SUB_FIXED;
        else
            nSubType &= ~DI_SUB_FIXED;
        break;
    case FIELD_PROP_FORMAT:
        {
            rAny >>= nValue;
            if( nValue >= 0)
                SetFormat(nValue);
        }
        break;

    case FIELD_PROP_PAR3:
        ::GetString( rAny, aContent );
        break;
    case FIELD_PROP_BOOL2:
        nSubType &= 0xf0ff;
        if(*(sal_Bool*)rAny.getValue())
            nSubType |= DI_SUB_DATE;
        else
            nSubType |= DI_SUB_TIME;
        break;
    default:
        return SwField::PutValue(rAny, nWhichId);
    }
    return sal_True;
}

/*--------------------------------------------------------------------
    Beschreibung: SwHiddenTxtFieldType by JP
 --------------------------------------------------------------------*/

SwHiddenTxtFieldType::SwHiddenTxtFieldType( sal_Bool bSetHidden )
    : SwFieldType( RES_HIDDENTXTFLD ), bHidden( bSetHidden )
{}

SwFieldType* SwHiddenTxtFieldType::Copy() const
{
    return new SwHiddenTxtFieldType( bHidden );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwHiddenTxtFieldType::SetHiddenFlag( sal_Bool bSetHidden )
{
    if( bHidden != bSetHidden )
    {
        bHidden = bSetHidden;
        UpdateFlds();		// alle HiddenText benachrichtigen
    }
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwHiddenTxtField::SwHiddenTxtField( SwHiddenTxtFieldType* pFldType,
                                    sal_Bool 	bConditional,
                                    const 	String& rCond,
                                    const	String& rStr,
                                    sal_Bool 	bHidden,
                                    sal_uInt16  nSub) :
    SwField( pFldType ), aCond(rCond), nSubType(nSub),
    bCanToggle(bConditional), bIsHidden(bHidden), bValid(sal_False)
{
    if(nSubType == TYP_CONDTXTFLD)
    {
        sal_uInt16 nPos = 0;
        aTRUETxt = rStr.GetToken(0, '|', nPos);

        if(nPos != STRING_NOTFOUND)
        {
            aFALSETxt = rStr.GetToken(0, '|', nPos);
            if(nPos != STRING_NOTFOUND)
            {
                aContent = rStr.GetToken(0, '|', nPos);
                bValid = sal_True;
            }
        }
    }
    else
        aTRUETxt = rStr;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwHiddenTxtField::SwHiddenTxtField( SwHiddenTxtFieldType* pFldType,
                                    const String& rCond,
                                    const String& rTrue,
                                    const String& rFalse,
                                    sal_uInt16 nSub)
    : SwField( pFldType ), aTRUETxt(rTrue), aFALSETxt(rFalse), aCond(rCond), nSubType(nSub),
      bIsHidden(sal_True), bValid(sal_False)
{
    bCanToggle	= aCond.Len() > 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwHiddenTxtField::Expand() const
{
    // Type: !Hidden  -> immer anzeigen
    // 		  Hide	  -> Werte die Bedingung aus

    if( TYP_CONDTXTFLD == nSubType )
    {
        if( bValid )
            return aContent;

        if( bCanToggle && !bIsHidden )
            return aTRUETxt;
    }
    else if( !((SwHiddenTxtFieldType*)GetTyp())->GetHiddenFlag() ||
        ( bCanToggle && bIsHidden ))
        return aTRUETxt;

    return aFALSETxt;
}

/*--------------------------------------------------------------------
    Beschreibung: Aktuellen Field-Value holen und cachen
 --------------------------------------------------------------------*/

void SwHiddenTxtField::Evaluate(SwDoc* pDoc)
{
    ASSERT(pDoc, Wo ist das Dokument Seniore);

    if( TYP_CONDTXTFLD == nSubType )
    {
        SwNewDBMgr* pMgr = pDoc->GetNewDBMgr();

        bValid = sal_False;
        String sTmpName;

        if (bCanToggle && !bIsHidden)
            sTmpName = aTRUETxt;
        else
            sTmpName = aFALSETxt;

// OS 21.08.97: #42943# Datenbankausdruecke muessen sich von
// 				einfachem Text unterscheiden. also wird der einfache Text
// 				bevorzugt in Anfuehrungszeichen gesetzt.
// 				Sind diese vorhanden werden umschliessende entfernt.
//				Wenn nicht, dann wird auf die Tauglichkeit als Datenbankname
//				geprueft. Nur wenn zwei oder mehr Punkte vorhanden sind und kein
//				Anfuehrungszeichen enthalten ist, gehen wir von einer DB aus.
        if(sTmpName.Len() > 1 && sTmpName.GetChar(0) == '\"' &&
            sTmpName.GetChar((sTmpName.Len() - 1))== '\"')
        {
            aContent = sTmpName.Copy(1, sTmpName.Len() - 2);
            bValid = sal_True;
        }
        else if(sTmpName.Search('\"') == STRING_NOTFOUND &&
            sTmpName.GetTokenCount('.') > 2)
        {
            ::ReplacePoint(sTmpName);
            if(sTmpName.GetChar(0) == '[' && sTmpName.GetChar(sTmpName.Len()-1) == ']')
            {	// Eckige Klammern entfernen
                sTmpName.Erase(0, 1);
                sTmpName.Erase(sTmpName.Len()-1, 1);
            }

            if( pMgr)
            {
                String sDBName( GetDBName( sTmpName, pDoc ));
                String sDataSource(sDBName.GetToken(0, DB_DELIM));
                String sDataTableOrQuery(sDBName.GetToken(1, DB_DELIM));
                if( pMgr->IsInMerge() && sDBName.Len() &&
                    pMgr->IsDataSourceOpen( sDataSource,
                                                sDataTableOrQuery, sal_False))
                {
                    double fNumber;
                    sal_uInt32 nTmpFormat;
                    pMgr->GetMergeColumnCnt(GetColumnName( sTmpName ),
                        GetLanguage(), aContent, &fNumber, &nTmpFormat );
                    bValid = sal_True;
                }
                else if( sDBName.Len() && sDataSource.Len() &&
                         sDataTableOrQuery.Len() )
                    bValid = sal_True;
            }
        }
    }
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwHiddenTxtField::GetCntnt(sal_Bool bName) const
{
    if ( bName )
    {
        String aStr(SwFieldType::GetTypeStr(nSubType));
        aStr += ' ';
        aStr += aCond;
        aStr += ' ';
        aStr += aTRUETxt;

        if(nSubType == TYP_CONDTXTFLD)
        {
static char __READONLY_DATA cTmp[] = " : ";
            aStr.AppendAscii(cTmp);
            aStr += aFALSETxt;
        }
        return aStr;
    }
    return Expand();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwHiddenTxtField::Copy() const
{
    SwHiddenTxtField* pFld =
        new SwHiddenTxtField((SwHiddenTxtFieldType*)GetTyp(), aCond,
                              aTRUETxt, aFALSETxt);
    pFld->bIsHidden = bIsHidden;
    pFld->bValid	= bValid;
    pFld->aContent	= aContent;
    pFld->SetFormat(GetFormat());
    pFld->nSubType 	= nSubType;
    return pFld;
}


/*--------------------------------------------------------------------
    Beschreibung: Bedingung setzen
 --------------------------------------------------------------------*/

void SwHiddenTxtField::SetPar1(const String& rStr)
{
    aCond = rStr;
    bCanToggle = aCond.Len() > 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
const String& SwHiddenTxtField::GetPar1() const
{
    return aCond;
}

/*--------------------------------------------------------------------
    Beschreibung: True/False Text
 --------------------------------------------------------------------*/

void SwHiddenTxtField::SetPar2(const String& rStr)
{
    if(nSubType == TYP_CONDTXTFLD)
    {
        sal_uInt16 nPos = rStr.Search('|');
        aTRUETxt = rStr.Copy(0, nPos);

        if(nPos != STRING_NOTFOUND)
            aFALSETxt = rStr.Copy(nPos + 1);
    }
    else
        aTRUETxt = rStr;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwHiddenTxtField::GetPar2() const
{
    String aRet(aTRUETxt);
    if(nSubType == TYP_CONDTXTFLD)
    {
        aRet += '|';
        aRet += aFALSETxt;
    }
    return aRet;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
sal_uInt16 SwHiddenTxtField::GetSubType() const
{
    return nSubType;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwHiddenTxtField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    const String* pOut = 0;
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        pOut = &aCond;
        break;
    case FIELD_PROP_PAR2:
        pOut = &aTRUETxt;
        break;
    case FIELD_PROP_PAR3:
        pOut = &aFALSETxt;
        break;
    case FIELD_PROP_PAR4 :
        pOut = &aContent;
    break;
    case FIELD_PROP_BOOL1:
        {
            sal_Bool bHidden = bIsHidden;
            rAny.setValue(&bHidden, ::getBooleanCppuType());
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    if( pOut )
        rAny <<= OUString( *pOut );
    return sal_True;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwHiddenTxtField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        {
            String sVal;
            SetPar1(::GetString( rAny, sVal ));
        }
        break;
    case FIELD_PROP_PAR2:
        ::GetString( rAny, aTRUETxt );
        break;
    case FIELD_PROP_PAR3:
        ::GetString( rAny, aFALSETxt );
        break;
    case FIELD_PROP_BOOL1:
        bIsHidden = *(sal_Bool*)rAny.getValue();
        break;
    case FIELD_PROP_PAR4:
        ::GetString( rAny, aContent);
        bValid = TRUE;
    break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}

//------------------------------------------------------------------------------

String SwHiddenTxtField::GetColumnName(const String& rName)
{
    sal_uInt16 nPos = rName.Search(DB_DELIM);
    if( STRING_NOTFOUND != nPos )
    {
        nPos = rName.Search(DB_DELIM, nPos + 1);

        if( STRING_NOTFOUND != nPos )
            return rName.Copy(nPos + 1);
    }
    return rName;
}

//------------------------------------------------------------------------------

String SwHiddenTxtField::GetDBName(const String& rName, SwDoc *pDoc)
{
    sal_uInt16 nPos = rName.Search(DB_DELIM);
    if( STRING_NOTFOUND != nPos )
    {
        nPos = rName.Search(DB_DELIM, nPos + 1);

        if( STRING_NOTFOUND != nPos )
            return rName.Copy( 0, nPos );
    }
    SwDBData aData = pDoc->GetDBData();
    String sRet = aData.sDataSource;
    sRet += DB_DELIM;
    sRet += String(aData.sCommand);
    return sRet;
}

/*--------------------------------------------------------------------
    Beschreibung: Der Feldtyp fuer Zeilenhoehe 0
 --------------------------------------------------------------------*/

SwHiddenParaFieldType::SwHiddenParaFieldType()
    : SwFieldType( RES_HIDDENPARAFLD )
{
}

SwFieldType* SwHiddenParaFieldType::Copy() const
{
    SwHiddenParaFieldType* pTyp = new SwHiddenParaFieldType();
    return pTyp;
}

/*--------------------------------------------------------------------
    Beschreibung: Das Feld Zeilenhoehe 0
 --------------------------------------------------------------------*/

SwHiddenParaField::SwHiddenParaField(SwHiddenParaFieldType* pTyp, const String& rStr)
    : SwField(pTyp), aCond(rStr)
{
    bIsHidden = sal_False;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwHiddenParaField::Expand() const
{
    return aEmptyStr;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwHiddenParaField::Copy() const
{
    SwHiddenParaField* pFld = new SwHiddenParaField((SwHiddenParaFieldType*)GetTyp(), aCond);
    pFld->bIsHidden = bIsHidden;

    return pFld;
}
/*-----------------05.03.98 13:25-------------------

--------------------------------------------------*/
BOOL SwHiddenParaField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch ( nWhichId )
    {
    case FIELD_PROP_PAR1:
        rAny <<= OUString(aCond);
        break;
    case  FIELD_PROP_BOOL1:
        {
            sal_Bool bHidden = bIsHidden;
            rAny.setValue(&bHidden, ::getBooleanCppuType());
        }
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 13:25-------------------

--------------------------------------------------*/
BOOL SwHiddenParaField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch ( nWhichId )
    {
    case FIELD_PROP_PAR1:
        ::GetString( rAny, aCond );
        break;
    case FIELD_PROP_BOOL1:
        bIsHidden = *(sal_Bool*)rAny.getValue();
        break;

    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}

/*--------------------------------------------------------------------
    Beschreibung: Bedingung setzen
 --------------------------------------------------------------------*/

void SwHiddenParaField::SetPar1(const String& rStr)
{
    aCond = rStr;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
const String& SwHiddenParaField::GetPar1() const
{
    return aCond;
}

/*--------------------------------------------------------------------
    Beschreibung: PostIt
 --------------------------------------------------------------------*/

SwPostItFieldType::SwPostItFieldType()
    : SwFieldType( RES_POSTITFLD )
{}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwFieldType* SwPostItFieldType::Copy() const
{
    return new SwPostItFieldType;
}

/*--------------------------------------------------------------------
    Beschreibung: SwPostItFieldType
 --------------------------------------------------------------------*/

SwPostItField::SwPostItField( SwPostItFieldType* pTyp,
        const String& rAuthor, const String& rTxt, const Date& rDate )
    : SwField( pTyp ), sTxt( rTxt ), sAuthor( rAuthor ), aDate( rDate )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwPostItField::Expand() const
{
    return aEmptyStr;
}


String SwPostItField::GetDescription() const
{
    return SW_RES(STR_NOTE);
}

/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwPostItField::Copy() const
{
    return new SwPostItField( (SwPostItFieldType*)GetTyp(), sAuthor,
                                sTxt, aDate );
}
/*--------------------------------------------------------------------
    Beschreibung: Author setzen
 --------------------------------------------------------------------*/

void SwPostItField::SetPar1(const String& rStr)
{
    sAuthor = rStr;
}

const String& SwPostItField::GetPar1() const
{
    return sAuthor;
}

/*--------------------------------------------------------------------
    Beschreibung: Text fuers PostIt setzen
 --------------------------------------------------------------------*/

void SwPostItField::SetPar2(const String& rStr)
{
    sTxt = rStr;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwPostItField::GetPar2() const
{
    return sTxt;
}

/*-----------------05.03.98 13:42-------------------

--------------------------------------------------*/
BOOL SwPostItField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        rAny <<= OUString(sAuthor);
        break;
    case FIELD_PROP_PAR2:
        rAny <<= OUString(sTxt);
        break;
    case FIELD_PROP_DATE:
        {
            util::Date aSetDate;
            aSetDate.Day = aDate.GetDay();
            aSetDate.Month = aDate.GetMonth();
            aSetDate.Year = aDate.GetYear();
            rAny.setValue(&aSetDate, ::getCppuType((util::Date*)0));
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 13:42-------------------

--------------------------------------------------*/
BOOL SwPostItField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        ::GetString( rAny, sAuthor );
        break;
    case FIELD_PROP_PAR2:
        ::GetString( rAny, sTxt );
        break;
    case FIELD_PROP_DATE:
        if( rAny.getValueType() == ::getCppuType((util::Date*)0) )
        {
            util::Date aSetDate = *(util::Date*)rAny.getValue();
            aDate = Date(aSetDate.Day, aSetDate.Month, aSetDate.Year);
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*--------------------------------------------------------------------
    Beschreibung: DokumentinfoFields
 --------------------------------------------------------------------*/

SwExtUserFieldType::SwExtUserFieldType()
    : SwFieldType( RES_EXTUSERFLD )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwFieldType* SwExtUserFieldType::Copy() const
{
    SwExtUserFieldType* pTyp = new SwExtUserFieldType;
    return pTyp;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwExtUserFieldType::Expand(sal_uInt16 nSub, sal_uInt32 ) const
{
    String aRet;
    sal_uInt16 nRet = USHRT_MAX;
    switch(nSub)
    {
    case EU_FIRSTNAME:      nRet = USER_OPT_FIRSTNAME; break;
    case EU_NAME:           nRet = USER_OPT_LASTNAME;  break;
    case EU_SHORTCUT:       nRet = USER_OPT_ID; break;

    case EU_COMPANY:        nRet = USER_OPT_COMPANY;        break;
    case EU_STREET:         nRet = USER_OPT_STREET;         break;
    case EU_TITLE:          nRet = USER_OPT_TITLE;          break;
    case EU_POSITION:       nRet = USER_OPT_POSITION;       break;
    case EU_PHONE_PRIVATE:  nRet = USER_OPT_TELEPHONEHOME;    break;
    case EU_PHONE_COMPANY:  nRet = USER_OPT_TELEPHONEWORK;    break;
    case EU_FAX:            nRet = USER_OPT_FAX;            break;
    case EU_EMAIL:          nRet = USER_OPT_EMAIL;          break;
    case EU_COUNTRY:        nRet = USER_OPT_COUNTRY;        break;
    case EU_ZIP:            nRet = USER_OPT_ZIP;            break;
    case EU_CITY:           nRet = USER_OPT_CITY;           break;
    case EU_STATE:          nRet = USER_OPT_STATE;          break;
    case EU_FATHERSNAME:    nRet = USER_OPT_FATHERSNAME;    break;
    case EU_APARTMENT:      nRet = USER_OPT_APARTMENT;      break;
    default:				ASSERT( !this, "Field unknown");
    }
    if( USHRT_MAX != nRet )
    {
        SvtUserOptions&  rUserOpt = SW_MOD()->GetUserOptions();
        aRet = rUserOpt.GetToken( nRet );
    }
    return aRet;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwExtUserField::SwExtUserField(SwExtUserFieldType* pTyp, sal_uInt16 nSubTyp, sal_uInt32 nFmt) :
    SwField(pTyp, nFmt), nType(nSubTyp)
{
    aContent = ((SwExtUserFieldType*)GetTyp())->Expand(nType, GetFormat());
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwExtUserField::Expand() const
{
    if (!IsFixed())
        ((SwExtUserField*)this)->aContent = ((SwExtUserFieldType*)GetTyp())->Expand(nType, GetFormat());

    return aContent;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwExtUserField::Copy() const
{
    SwExtUserField* pFld = new SwExtUserField((SwExtUserFieldType*)GetTyp(), nType, GetFormat());
    pFld->SetExpansion(aContent);

    return pFld;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
sal_uInt16 SwExtUserField::GetSubType() const
{
    return nType;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwExtUserField::SetSubType(sal_uInt16 nSub)
{
    nType = nSub;
}

/*-----------------05.03.98 14:14-------------------

--------------------------------------------------*/
BOOL SwExtUserField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        rAny <<= OUString(aContent);
        break;

    case FIELD_PROP_USHORT1:
        {
            sal_Int16 nTmp = nType;
            rAny <<= nTmp;
        }
        break;
    case FIELD_PROP_BOOL1:
        {
            sal_Bool bTmp = IsFixed();
            rAny.setValue(&bTmp, ::getBooleanCppuType());
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 14:14-------------------

--------------------------------------------------*/
BOOL SwExtUserField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        ::GetString( rAny, aContent );
        break;

    case FIELD_PROP_USHORT1:
        {
            sal_Int16 nTmp = 0;
            rAny >>= nTmp;
            nType = nTmp;
        }
        break;
    case FIELD_PROP_BOOL1:
        if( *(sal_Bool*)rAny.getValue() )
            SetFormat(GetFormat() | AF_FIXED);
        else
            SetFormat(GetFormat() & ~AF_FIXED);
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
//-------------------------------------------------------------------------

/*--------------------------------------------------------------------
    Beschreibung: Relatives Seitennummern - Feld
 --------------------------------------------------------------------*/

SwRefPageSetFieldType::SwRefPageSetFieldType()
    : SwFieldType( RES_REFPAGESETFLD )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwFieldType* SwRefPageSetFieldType::Copy() const
{
    return new SwRefPageSetFieldType;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
// ueberlagert, weil es nichts zum Updaten gibt!
void SwRefPageSetFieldType::Modify( SfxPoolItem *, SfxPoolItem * )
{
}

/*--------------------------------------------------------------------
    Beschreibung: Relative Seitennummerierung
 --------------------------------------------------------------------*/

SwRefPageSetField::SwRefPageSetField( SwRefPageSetFieldType* pTyp,
                    short nOff, sal_Bool bFlag )
    : SwField( pTyp ), nOffset( nOff ), bOn( bFlag )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwRefPageSetField::Expand() const
{
    return aEmptyStr;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwRefPageSetField::Copy() const
{
    return new SwRefPageSetField( (SwRefPageSetFieldType*)GetTyp(), nOffset, bOn );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwRefPageSetField::GetPar2() const
{
    return String::CreateFromInt32( GetOffset() );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwRefPageSetField::SetPar2(const String& rStr)
{
    SetOffset( (short) rStr.ToInt32() );
}

/*-----------------05.03.98 14:52-------------------

--------------------------------------------------*/
BOOL SwRefPageSetField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_BOOL1:
        rAny.setValue(&bOn, ::getBooleanCppuType());
        break;
    case FIELD_PROP_USHORT1:
        rAny <<= (sal_Int16)nOffset;
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 14:52-------------------

--------------------------------------------------*/
BOOL SwRefPageSetField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_BOOL1:
        bOn = *(sal_Bool*)rAny.getValue();
        break;
    case FIELD_PROP_USHORT1:
        rAny >>=nOffset;
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*--------------------------------------------------------------------
    Beschreibung: relatives Seitennummern - Abfrage Feld
 --------------------------------------------------------------------*/

SwRefPageGetFieldType::SwRefPageGetFieldType( SwDoc* pDc )
    : SwFieldType( RES_REFPAGEGETFLD ), pDoc( pDc ), nNumberingType( SVX_NUM_ARABIC )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwFieldType* SwRefPageGetFieldType::Copy() const
{
    SwRefPageGetFieldType* pNew = new SwRefPageGetFieldType( pDoc );
    pNew->nNumberingType = nNumberingType;
    return pNew;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwRefPageGetFieldType::Modify( SfxPoolItem* pOld, SfxPoolItem* pNew )
{
    // Update auf alle GetReferenz-Felder
    if( !pNew && !pOld && GetDepends() )
    {
        // sammel erstmal alle SetPageRefFelder ein.
        _SetGetExpFlds aTmpLst( 10, 5 );
        if( MakeSetList( aTmpLst ) )
        {
            SwClientIter aIter( *this );
            if( aIter.GoStart() )
                do {
                    // nur die GetRef-Felder Updaten
                    SwFmtFld* pFmtFld = (SwFmtFld*)aIter();
                    if( pFmtFld->GetTxtFld() )
                        UpdateField( pFmtFld->GetTxtFld(), aTmpLst );
                } while( aIter++ );
        }
    }

    // weiter an die Text-Felder, diese "Expandieren" den Text
    SwModify::Modify( pOld, pNew );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
sal_uInt16 SwRefPageGetFieldType::MakeSetList( _SetGetExpFlds& rTmpLst )
{
    SwClientIter aIter( *pDoc->GetSysFldType( RES_REFPAGESETFLD));
    if( aIter.GoStart() )
        do {
            // nur die GetRef-Felder Updaten
            SwFmtFld* pFmtFld = (SwFmtFld*)aIter();
            const SwTxtFld* pTFld = pFmtFld->GetTxtFld();
            if( pTFld )
            {
                const SwTxtNode& rTxtNd = pTFld->GetTxtNode();

                // immer den ersten !! (in Tab-Headline, Kopf-/Fuss )
                Point aPt;
                const SwCntntFrm* pFrm = rTxtNd.GetFrm( &aPt, 0, sal_False );

                _SetGetExpFld* pNew;

                if( !pFrm ||
                     pFrm->IsInDocBody() ||
                    // --> FME 2004-07-27 #i31868#
                    // Check if pFrm is not yet connected to the layout.
                    !pFrm->FindPageFrm() )
                    // <--
                {
                    // einen sdbcx::Index fuers bestimmen vom TextNode anlegen
                    SwNodeIndex aIdx( rTxtNd );
                    pNew = new _SetGetExpFld( aIdx, pTFld );
                }
                else
                {
                    // einen sdbcx::Index fuers bestimmen vom TextNode anlegen
                    SwPosition aPos( pDoc->GetNodes().GetEndOfPostIts() );
#ifndef PRODUCT
                    ASSERT( GetBodyTxtNode( *pDoc, aPos, *pFrm ),
                            "wo steht das Feld" );
#else
                    GetBodyTxtNode( *pDoc, aPos, *pFrm );
#endif
                    pNew = new _SetGetExpFld( aPos.nNode, pTFld,
                                                &aPos.nContent );
                }

                if( !rTmpLst.Insert( pNew ))
                    delete pNew;
            }
        } while( aIter++ );

    return rTmpLst.Count();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwRefPageGetFieldType::UpdateField( SwTxtFld* pTxtFld,
                                        _SetGetExpFlds& rSetList )
{
    SwRefPageGetField* pGetFld = (SwRefPageGetField*)pTxtFld->GetFld().GetFld();
    pGetFld->SetText( aEmptyStr );

    // dann suche mal das richtige RefPageSet-Field
    SwTxtNode* pTxtNode = (SwTxtNode*)&pTxtFld->GetTxtNode();
    if( pTxtNode->StartOfSectionIndex() >
        pDoc->GetNodes().GetEndOfExtras().GetIndex() )
    {
        SwNodeIndex aIdx( *pTxtNode );
        _SetGetExpFld aEndFld( aIdx, pTxtFld );

        sal_uInt16 nLast;
        rSetList.Seek_Entry( &aEndFld, &nLast );

        if( nLast-- )
        {
            const SwTxtFld* pRefTxtFld = rSetList[ nLast ]->GetFld();
            const SwRefPageSetField* pSetFld =
                        (SwRefPageSetField*)pRefTxtFld->GetFld().GetFld();
            if( pSetFld->IsOn() )
            {
                // dann bestimme mal den entsp. Offset
                Point aPt;
                const SwCntntFrm* pFrm = pTxtNode->GetFrm( &aPt, 0, sal_False );
                const SwCntntFrm* pRefFrm = pRefTxtFld->GetTxtNode().GetFrm( &aPt, 0, sal_False );
                const SwPageFrm* pPgFrm = 0;
                sal_uInt16 nDiff = ( pFrm && pRefFrm )
                        ? 	(pPgFrm = pFrm->FindPageFrm())->GetPhyPageNum() -
                            pRefFrm->FindPageFrm()->GetPhyPageNum() + 1
                        : 1;

                sal_uInt32 nTmpFmt = SVX_NUM_PAGEDESC == pGetFld->GetFormat()
                        ? ( !pPgFrm
                                ? (sal_uInt32)SVX_NUM_ARABIC
                                : pPgFrm->GetPageDesc()->GetNumType().GetNumberingType() )
                        : pGetFld->GetFormat();
                short nPageNum = static_cast<short>(Max(0, pSetFld->GetOffset() + (short)nDiff));
                pGetFld->SetText( FormatNumber( nPageNum, nTmpFmt ) );
            }
        }
    }
    // dann die Formatierung anstossen
    ((SwFmtFld&)pTxtFld->GetFld()).Modify( 0, 0 );
}

/*--------------------------------------------------------------------
    Beschreibung: Relative Seitennummerierung Abfragen
 --------------------------------------------------------------------*/

SwRefPageGetField::SwRefPageGetField( SwRefPageGetFieldType* pTyp,
                                    sal_uInt32 nFmt )
    : SwField( pTyp, nFmt )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwRefPageGetField::Expand() const
{
    return sTxt;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwRefPageGetField::Copy() const
{
    SwRefPageGetField* pCpy = new SwRefPageGetField(
                        (SwRefPageGetFieldType*)GetTyp(), GetFormat() );
    pCpy->SetText( sTxt );
    return pCpy;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwRefPageGetField::ChangeExpansion( const SwFrm* pFrm,
                                        const SwTxtFld* pFld )
{
    // nur Felder in Footer, Header, FootNote, Flys
    SwTxtNode* pTxtNode = (SwTxtNode*)&pFld->GetTxtNode();
    SwRefPageGetFieldType* pGetType = (SwRefPageGetFieldType*)GetTyp();
    SwDoc* pDoc = pGetType->GetDoc();
    if( pFld->GetTxtNode().StartOfSectionIndex() >
        pDoc->GetNodes().GetEndOfExtras().GetIndex() )
        return;

    sTxt.Erase();

    ASSERT( !pFrm->IsInDocBody(), "Flag ist nicht richtig, Frame steht im DocBody" );

    // sammel erstmal alle SetPageRefFelder ein.
    _SetGetExpFlds aTmpLst( 10, 5 );
    if( !pGetType->MakeSetList( aTmpLst ) )
        return ;

    // einen sdbcx::Index fuers bestimmen vom TextNode anlegen
    SwPosition aPos( SwNodeIndex( pDoc->GetNodes() ) );
    pTxtNode = (SwTxtNode*) GetBodyTxtNode( *pDoc, aPos, *pFrm );

    // Wenn kein Layout vorhanden, kommt es in Kopf und Fusszeilen dazu
    // das ChangeExpansion uebers Layout-Formatieren aufgerufen wird
    // aber kein TxtNode vorhanden ist
    //
    if(!pTxtNode)
        return;

    _SetGetExpFld aEndFld( aPos.nNode, pFld, &aPos.nContent );

    sal_uInt16 nLast;
    aTmpLst.Seek_Entry( &aEndFld, &nLast );

    if( !nLast-- )
        return ;		// es gibt kein entsprechendes Set - Feld vor mir

    const SwTxtFld* pRefTxtFld = aTmpLst[ nLast ]->GetFld();
    const SwRefPageSetField* pSetFld =
                        (SwRefPageSetField*)pRefTxtFld->GetFld().GetFld();
    Point aPt;
    const SwCntntFrm* pRefFrm = pRefTxtFld ? pRefTxtFld->GetTxtNode().GetFrm( &aPt, 0, sal_False ) : 0;
    if( pSetFld->IsOn() && pRefFrm )
    {
        // dann bestimme mal den entsp. Offset
        const SwPageFrm* pPgFrm = pFrm->FindPageFrm();
        sal_uInt16 nDiff = pPgFrm->GetPhyPageNum() -
                            pRefFrm->FindPageFrm()->GetPhyPageNum() + 1;

        SwRefPageGetField* pGetFld = (SwRefPageGetField*)pFld->GetFld().GetFld();
        sal_uInt32 nTmpFmt = SVX_NUM_PAGEDESC == pGetFld->GetFormat()
                            ? pPgFrm->GetPageDesc()->GetNumType().GetNumberingType()
                            : pGetFld->GetFormat();
        short nPageNum = static_cast<short>(Max(0, pSetFld->GetOffset() + (short)nDiff ));
        pGetFld->SetText( FormatNumber( nPageNum, nTmpFmt ) );
    }
}
/*-----------------05.03.98 14:52-------------------

--------------------------------------------------*/
BOOL SwRefPageGetField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
        case FIELD_PROP_USHORT1:
            rAny <<= (sal_Int16)GetFormat();
        break;
        case FIELD_PROP_PAR1:
            rAny <<= OUString(sTxt);
        break;
        default:
            DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 14:52-------------------

--------------------------------------------------*/
BOOL SwRefPageGetField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
        case FIELD_PROP_USHORT1:
        {
            sal_Int16 nSet = 0;
            rAny >>= nSet;
            if(nSet <= SVX_NUM_PAGEDESC )
                SetFormat(nSet);
            else
                //exception(wrong_value)
                ;
        }
        break;
        case FIELD_PROP_PAR1:
        {
            OUString sTmp;
            rAny >>= sTmp;
            sTxt = sTmp;
        }
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}

/*--------------------------------------------------------------------
    Beschreibung: Feld zum Anspringen und Editieren
 --------------------------------------------------------------------*/

SwJumpEditFieldType::SwJumpEditFieldType( SwDoc* pD )
    : SwFieldType( RES_JUMPEDITFLD ), pDoc( pD ), aDep( this, 0 )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwFieldType* SwJumpEditFieldType::Copy() const
{
    return new SwJumpEditFieldType( pDoc );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwCharFmt* SwJumpEditFieldType::GetCharFmt()
{
    SwCharFmt* pFmt = pDoc->GetCharFmtFromPool( RES_POOLCHR_JUMPEDIT );

    // noch nicht registriert ?
    if( !aDep.GetRegisteredIn() )
        pFmt->Add( &aDep );		// anmelden

    return pFmt;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwJumpEditField::SwJumpEditField( SwJumpEditFieldType* pTyp, sal_uInt32 nForm,
                                const String& rTxt, const String& rHelp )
    : SwField( pTyp, nForm ), sTxt( rTxt ), sHelp( rHelp )
{
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwJumpEditField::Expand() const
{
    String sTmp( '<' );
    sTmp += sTxt;
    return sTmp += '>';
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwField* SwJumpEditField::Copy() const
{
    return new SwJumpEditField( (SwJumpEditFieldType*)GetTyp(), GetFormat(),
                                sTxt, sHelp );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
// Platzhalter-Text

const String& SwJumpEditField::GetPar1() const
{
    return sTxt;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwJumpEditField::SetPar1(const String& rStr)
{
    sTxt = rStr;
}

// HinweisText
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
String SwJumpEditField::GetPar2() const
{
    return sHelp;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwJumpEditField::SetPar2(const String& rStr)
{
    sHelp = rStr;
}

/*-----------------05.03.98 15:00-------------------

--------------------------------------------------*/
BOOL SwJumpEditField::QueryValue( uno::Any& rAny, USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_USHORT1:
        {
            sal_Int16 nRet;
            switch( GetFormat() )
            {
            case JE_FMT_TABLE:  nRet = text::PlaceholderType::TABLE; break;
            case JE_FMT_FRAME:  nRet = text::PlaceholderType::TEXTFRAME; break;
            case JE_FMT_GRAPHIC:nRet = text::PlaceholderType::GRAPHIC; break;
            case JE_FMT_OLE:	nRet = text::PlaceholderType::OBJECT; break;
//			case JE_FMT_TEXT:
            default:
                nRet = text::PlaceholderType::TEXT; break;
            }
            rAny <<= nRet;
        }
        break;
    case FIELD_PROP_PAR1 :
        rAny <<= OUString(sHelp);
        break;
    case FIELD_PROP_PAR2 :
         rAny <<= OUString(sTxt);
         break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}
/*-----------------05.03.98 15:00-------------------

--------------------------------------------------*/
BOOL SwJumpEditField::PutValue( const uno::Any& rAny, USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_USHORT1:
        {
            //JP 24.10.2001: int32 because in UnoField.cxx a putvalue is
            //				called with a int32 value! But normally we need
            //				here only a int16
            sal_Int32 nSet = 0;
            rAny >>= nSet;
            switch( nSet )
            {
                case text::PlaceholderType::TEXT 	 : SetFormat(JE_FMT_TEXT); break;
                case text::PlaceholderType::TABLE 	 : SetFormat(JE_FMT_TABLE); break;
                case text::PlaceholderType::TEXTFRAME: SetFormat(JE_FMT_FRAME); break;
                case text::PlaceholderType::GRAPHIC  : SetFormat(JE_FMT_GRAPHIC); break;
                case text::PlaceholderType::OBJECT 	 : SetFormat(JE_FMT_OLE); break;
            }
        }
        break;
    case FIELD_PROP_PAR1 :
        ::GetString( rAny, sHelp );
        break;
    case FIELD_PROP_PAR2 :
         ::GetString( rAny, sTxt);
         break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}


/*--------------------------------------------------------------------
    Beschreibung: Combined Character Fieldtype / Field
 --------------------------------------------------------------------*/

SwCombinedCharFieldType::SwCombinedCharFieldType()
    : SwFieldType( RES_COMBINED_CHARS )
{
}

SwFieldType* SwCombinedCharFieldType::Copy() const
{
    return new SwCombinedCharFieldType;
}

/* --------------------------------------------------------------------*/

SwCombinedCharField::SwCombinedCharField( SwCombinedCharFieldType* pFTyp,
                                            const String& rChars )
    : SwField( pFTyp, 0 ),
    sCharacters( rChars.Copy( 0, MAX_COMBINED_CHARACTERS ))
{
}

String	SwCombinedCharField::Expand() const
{
    return sCharacters;
}

SwField* SwCombinedCharField::Copy() const
{
    return new SwCombinedCharField( (SwCombinedCharFieldType*)GetTyp(),
                                        sCharacters );
}

const String& SwCombinedCharField::GetPar1() const
{
    return sCharacters;
}

void SwCombinedCharField::SetPar1(const String& rStr)
{
    sCharacters = rStr.Copy( 0, MAX_COMBINED_CHARACTERS );
}

BOOL SwCombinedCharField::QueryValue( uno::Any& rAny,
                                        USHORT nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        rAny <<= rtl::OUString( sCharacters );
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}

BOOL SwCombinedCharField::PutValue( const uno::Any& rAny,
                                        USHORT nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        ::GetString( rAny, sCharacters ).Erase( MAX_COMBINED_CHARACTERS );
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}

