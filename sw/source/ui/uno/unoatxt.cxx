/*************************************************************************
 *
 *  $RCSfile: unoatxt.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: jp $ $Date: 2001-10-18 15:06:55 $
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
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#define _SVSTDARR_STRINGS

#ifndef _COM_SUN_STAR_BEANS_PropertyAttribute_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#include <svtools/svstdarr.hxx>
#ifndef _SVTOOLS_UNOEVENT_HXX_
#include <svtools/unoevent.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _OFF_APP_HXX //autogen
#include <offmgr/app.hxx>
#endif
#ifndef _OFAACCFG_HXX //autogen
#include <offmgr/ofaaccfg.hxx>
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _GLOSDOC_HXX
#include <glosdoc.hxx>
#endif
#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _INITUI_HXX
#include <initui.hxx>
#endif
#ifndef _GLOSLST_HXX
#include <gloslst.hxx>
#endif
#ifndef _UNOATXT_HXX
#include <unoatxt.hxx>
#endif
#ifndef _UNOMAP_HXX
#include <unomap.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _SWEVENT_HXX
#include <swevent.hxx>
#endif
#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _UNOCRSR_HXX //autogen
#include <unocrsr.hxx>
#endif
#ifndef _BOOKMRK_HXX //autogen
#include <bookmrk.hxx>
#endif
#ifndef _UNOPRNMS_HXX
#include <unoprnms.hxx>
#endif
#ifndef _SWDOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _SWUNODEF_HXX
#include <swunodef.hxx>
#endif


SV_IMPL_REF ( SwDocShell )
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::rtl;

using ::com::sun::star::container::NoSuchElementException;

#define PROPERTY_NONE	0


/* -----------------16.06.98 09:15-------------------
 *
 * --------------------------------------------------*/
String lcl_FindGroupName(SwGlossaries* pGlossaries, const OUString& GroupName)
{
    sal_uInt16 nCount = pGlossaries->GetGroupCnt();
    //wenn der Gruppenname intern erzeugt wurde, dann steht auch hier der Pfad drin
    String sGroup(GroupName);
    String sGroupName(sGroup.GetToken(0, GLOS_DELIM));
    String sPath = sGroup.GetToken(1, GLOS_DELIM);
    sal_Bool bPathLen = sPath.Len() > 0;
    for ( sal_uInt16 i = 0; i < nCount; i++ )
    {
        String sGrpName = pGlossaries->GetGroupName(i);
        if(bPathLen ? sGroup == sGrpName : sGroupName == sGrpName.GetToken(0, GLOS_DELIM))
        {
            return sGrpName;
        }
    }
    return aEmptyStr;
}

/******************************************************************
 *
 ******************************************************************/
/* -----------------30.03.99 14:31-------------------
 *
 * --------------------------------------------------*/
Reference< uno::XInterface > SAL_CALL SwXAutoTextContainer_createInstance(
    const Reference< XMultiServiceFactory > & rSMgr) throw( Exception )
{
    static Reference< uno::XInterface > xAText = (cppu::OWeakObject*)new SwXAutoTextContainer();;
    return xAText;
}
/* -----------------------------17.04.01 13:17--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SAL_CALL SwXAutoTextContainer_getSupportedServiceNames() throw()
{
    OUString sService( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.AutoTextContainer"));
    const Sequence< OUString > aSeq( &sService, 1 );
    return aSeq;
}
/* -----------------------------17.04.01 13:18--------------------------------

 ---------------------------------------------------------------------------*/
OUString SAL_CALL SwXAutoTextContainer_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM("SwXAutoTextContainer" ) );
}

/*-- 21.12.98 12:42:16---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoTextContainer::SwXAutoTextContainer()
{
    pGlossaries = ::GetGlossaries();

}
/*-- 21.12.98 12:42:17---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoTextContainer::~SwXAutoTextContainer()
{

}
/*-- 21.12.98 12:42:17---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXAutoTextContainer::getCount(void) throw( uno::RuntimeException )
{
    return pGlossaries->GetGroupCnt();
}
/*-- 21.12.98 12:42:18---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoTextContainer::getByIndex(sal_Int32 nIndex)
    throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    sal_uInt16 nCount = pGlossaries->GetGroupCnt();
    if ( nIndex >= 0 && nIndex <= nCount )
        aRet = getByName(pGlossaries->GetGroupName(nIndex));
    else
        throw lang::IndexOutOfBoundsException();
    return aRet;
}
/*-- 21.12.98 12:42:18---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXAutoTextContainer::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((const Reference<text::XAutoTextGroup>*)0);

}
/*-- 21.12.98 12:42:18---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXAutoTextContainer::hasElements(void) throw( uno::RuntimeException )
{
    //zumindest Standard sollte es immer geben!
    return sal_True;
}
/*-- 21.12.98 12:42:18---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoTextContainer::getByName(const OUString& GroupName)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    XAutoTextGroupPtrArr &rGlosGroupArr = pGlossaries->GetUnoGroupArray();
    //zunaechst den Namen mit Pfad-Extension finden
    String sGroupName = lcl_FindGroupName(pGlossaries, GroupName);

    if(sGroupName.Len())
    {
        Reference< text::XAutoTextGroup >  aRef;
        for ( sal_uInt16 i = 0; i < rGlosGroupArr.Count(); i++ )
        {
            Reference< text::XAutoTextGroup > * pxGroup = rGlosGroupArr.GetObject(i);
            Reference< lang::XUnoTunnel > xGroupTunnel(*pxGroup, uno::UNO_QUERY);

            SwXAutoTextGroup* pSwGroup = 0;
            if(xGroupTunnel.is())
            {
                pSwGroup = (SwXAutoTextGroup*)xGroupTunnel->getSomething(SwXAutoTextGroup::getUnoTunnelId());
            }
            if(pSwGroup && GroupName == pSwGroup->getName())
            {	                            // Die Gruppe gibt es schon
                aRef = *pxGroup;
                break;
            }
        }
        if ( !aRef.is() )
        {
            Reference< text::XAutoTextGroup > * pxGroup = new Reference< text::XAutoTextGroup > ;
            *pxGroup = new SwXAutoTextGroup(sGroupName, pGlossaries/*pGlosGroup*/);
            aRef = *pxGroup;
            rGlosGroupArr.Insert(pxGroup, rGlosGroupArr.Count());
        }
        aRet.setValue(&aRef, ::getCppuType((const Reference< text::XAutoTextGroup >*)0));
    }
    else
        throw container::NoSuchElementException();
    return aRet;
}
/*-- 21.12.98 12:42:19---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXAutoTextContainer::getElementNames(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_uInt16 nCount = pGlossaries->GetGroupCnt();

    uno::Sequence< OUString > aGroupNames(nCount);
    OUString *pArr = aGroupNames.getArray();

    for ( sal_uInt16 i = 0; i < nCount; i++ )
    {
        // Die Namen werden ohne Pfad-Extension weitergegeben
        String sGroupName(pGlossaries->GetGroupName(i));
        pArr[i] = sGroupName.GetToken(0, GLOS_DELIM);
    }
    return aGroupNames;
}
/*-- 21.12.98 12:42:19---------------------------------------------------
    findet Gruppennamen mit und ohne Pfadindex
  -----------------------------------------------------------------------*/
sal_Bool SwXAutoTextContainer::hasByName(const OUString& Name)
    throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    String sGroupName = lcl_FindGroupName(pGlossaries, Name);
    if(sGroupName.Len())
        return sal_True;
    return sal_False;
}
/*-- 21.12.98 12:42:19---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XAutoTextGroup >  SwXAutoTextContainer::insertNewByName(
    const OUString& aGroupName)
    throw( lang::IllegalArgumentException, container::ElementExistException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(hasByName(aGroupName))
        throw container::ElementExistException();
    //check for non-ASCII characters
    if(!aGroupName.getLength())
    {
        IllegalArgumentException aIllegal;
        aIllegal.Message = C2U("group name must not be empty");
        throw aIllegal;
    }
    for(sal_Int32 nPos = 0; nPos < aGroupName.getLength(); nPos++)
    {
        sal_Unicode cChar = aGroupName[nPos];
        if(	((cChar >= 'A') && (cChar <= 'Z')) ||
                ((cChar >= 'a') && (cChar <= 'z')) ||
                    ((cChar >= '0') && (cChar <= '9')) ||
                        (cChar == '_') ||
                    cChar == 0x20 )
        {
            continue;
        }
        IllegalArgumentException aIllegal;
        aIllegal.Message = C2U("group name must not contain non-ASCII characters");
        throw aIllegal;
    }
    String sGroup(aGroupName);
    if(STRING_NOTFOUND == sGroup.Search(GLOS_DELIM))
    {
        sGroup += GLOS_DELIM;
        sGroup += UniString::CreateFromInt32(0);
    }
    pGlossaries->NewGroupDoc(sGroup, sGroup.GetToken(0, GLOS_DELIM));
    Reference< text::XAutoTextGroup >  aRet = new SwXAutoTextGroup(sGroup, pGlossaries);
    return aRet;
}
/*-- 21.12.98 12:42:19---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextContainer::removeByName(const OUString& aGroupName)
    throw( container::NoSuchElementException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    //zunaechst den Namen mit Pfad-Extension finden
    String sGroupName = lcl_FindGroupName(pGlossaries, aGroupName);
    if(!sGroupName.Len())
        throw container::NoSuchElementException();
    pGlossaries->DelGroupDoc(sGroupName);
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXAutoTextContainer::getImplementationName(void) throw( RuntimeException )
{
    return SwXAutoTextContainer_getImplementationName();
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXAutoTextContainer::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    const Sequence< OUString > aNames = SwXAutoTextContainer_getSupportedServiceNames();
    for(sal_Int32 nService = 0; nService < aNames.getLength(); nService++)
    {
        if(aNames.getConstArray()[nService] == rServiceName)
            return TRUE;
    }
    return FALSE;
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXAutoTextContainer::getSupportedServiceNames(void) throw( RuntimeException )
{
    return SwXAutoTextContainer_getSupportedServiceNames();
}
/******************************************************************
 *
 ******************************************************************/
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXAutoTextGroup::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXAutoTextGroup::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}

/*-- 21.12.98 12:42:24---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoTextGroup::SwXAutoTextGroup(const OUString& rName,
            SwGlossaries*	pGlos) :
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_AUTO_TEXT_GROUP)),
    sName(rName),
    sGroupName(rName),
    pGlossaries(pGlos)
{
}
/*-- 21.12.98 12:42:24---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoTextGroup::~SwXAutoTextGroup()
{
}
/*-- 21.12.98 12:42:24---------------------------------------------------

  -----------------------------------------------------------------------*/
/*-- 21.12.98 12:42:25---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXAutoTextGroup::getTitles(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_uInt16 nCount = 0;
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
        nCount = pGlosGroup->GetCount();
    else
        throw uno::RuntimeException();

    uno::Sequence< OUString > aEntryTitles(nCount);
    OUString *pArr = aEntryTitles.getArray();

    for ( sal_uInt16 i = 0; i < nCount; i++ )
        pArr[i] = pGlosGroup->GetLongName(i);
    delete pGlosGroup;
    return aEntryTitles;
}
/*-- 21.12.98 12:42:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::renameByName(const OUString& aElementName,
    const OUString& aNewElementName, const OUString& aNewElementTitle)
    throw( lang::IllegalArgumentException, container::ElementExistException, io::IOException,
                                                     uno::RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    // throw exception only if the programmatic name is to be changed into an existing name
    if(aNewElementName != aElementName && hasByName(aNewElementName))
        throw container::ElementExistException();
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
    {
        sal_uInt16 nIdx = pGlosGroup->GetIndex( aElementName);
        if(USHRT_MAX == nIdx)
            throw lang::IllegalArgumentException();
        String aNewShort( aNewElementName);
        String aNewName( aNewElementTitle);
        sal_uInt16 nOldLongIdx = pGlosGroup->GetLongIndex( aNewShort );
        sal_uInt16 nOldIdx = pGlosGroup->GetIndex( aNewName );

        if( nIdx != USHRT_MAX &&
                (nOldLongIdx == USHRT_MAX || nOldLongIdx == nIdx )&&
                    (nOldIdx == USHRT_MAX || nOldIdx == nIdx ))
        {
            pGlosGroup->Rename( nIdx, &aNewShort, &aNewName );
            if(pGlosGroup->GetError() != 0)
                throw io::IOException();
        }
        delete pGlosGroup;
    }
    else
        throw uno::RuntimeException();
}
/* -----------------04.05.99 11:57-------------------
 *
 * --------------------------------------------------*/
sal_Bool lcl_CopySelToDoc( SwDoc* pInsDoc, SwXTextCursor* pxCursor, SwXTextRange* pxRange )
{
    ASSERT( pInsDoc, "kein Ins.Dokument"  );

    SwNodes& rNds = pInsDoc->GetNodes();

    SwNodeIndex aIdx( rNds.GetEndOfContent(), -1 );
    SwCntntNode * pNd = aIdx.GetNode().GetCntntNode();
    SwPosition aPos( aIdx, SwIndex( pNd, pNd->Len() ));

    sal_Bool bRet = sal_False;
    pInsDoc->LockExpFlds();
    {
        if(pxCursor)
        {
            SwUnoCrsr* pUnoCrsr = pxCursor->GetCrsr();
            bRet |= pUnoCrsr->GetDoc()->Copy( *pUnoCrsr, aPos );
        }
        else
        {
            SwBookmark* pBkm = pxRange->GetBookmark();
            if(pBkm->GetOtherPos())
            {
                SwPaM aTmp(*pBkm->GetOtherPos(), pBkm->GetPos());
                bRet |= pxRange->GetDoc()->Copy(aTmp, aPos);
            }
        }
    }

    pInsDoc->UnlockExpFlds();
    if( !pInsDoc->IsExpFldsLocked() )
        pInsDoc->UpdateExpFlds();

    return bRet;
}
/*-- 21.12.98 12:42:25---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XAutoTextEntry >  SwXAutoTextGroup::insertNewByName(const OUString& aName,
        const OUString& aTitle, const Reference< text::XTextRange > & xTextRange)
        throw( container::ElementExistException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if(hasByName(aName))
        throw container::ElementExistException();
    if(!xTextRange.is())
        throw uno::RuntimeException();

    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    String sShortName(aName);
    String sLongName(aTitle);
    if(pGlosGroup && !pGlosGroup->GetError())
    {
        if( pGlosGroup->IsOld() && pGlosGroup->ConvertToNew())
        {
            throw uno::RuntimeException();
        }
        Reference<lang::XUnoTunnel> xRangeTunnel( xTextRange, uno::UNO_QUERY);
        SwXTextRange* pxRange = 0;
        SwXTextCursor* pxCursor = 0;
        if(xRangeTunnel.is())
        {
            pxRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                    SwXTextRange::getUnoTunnelId());
            pxCursor = (SwXTextCursor*)xRangeTunnel->getSomething(
                                    SwXTextCursor::getUnoTunnelId());
        }

        String sOnlyTxt;
        String* pOnlyTxt = 0;
        sal_Bool bNoAttr = !pxCursor && !pxRange;
        if(bNoAttr)
        {
            sOnlyTxt = UniString(xTextRange->getString());
            pOnlyTxt = &sOnlyTxt;
        }

        const OfaAutoCorrCfg* pCfg = OFF_APP()->GetAutoCorrConfig();

        SwDoc* pGDoc = pGlosGroup->GetDoc();

        // Bis es eine Option dafuer gibt, base util::URL loeschen
        const String aOldURL( INetURLObject::GetBaseURL() );
        if(pCfg->IsSaveRelFile())
        {
            INetURLObject::SetBaseURL( URIHelper::SmartRelToAbs(pGlosGroup->GetFileName()));
        }
        else
            INetURLObject::SetBaseURL( aEmptyStr );

        sal_uInt16 nRet;
        if( pOnlyTxt )
            nRet = pGlosGroup->PutText( sShortName, sLongName, *pOnlyTxt );
        else
        {
            pGlosGroup->ClearDoc();
            if( pGlosGroup->BeginPutDoc( sShortName, sLongName ) )
            {
                pGDoc->SetRedlineMode_intern( REDLINE_DELETE_REDLINES );
                lcl_CopySelToDoc( pGDoc, pxCursor, pxRange );
                pGDoc->SetRedlineMode_intern( 0 );
                nRet = pGlosGroup->PutDoc();
            }
            else
                nRet = (sal_uInt16) -1;
        }

        INetURLObject::SetBaseURL( aOldURL );

        if(nRet == (sal_uInt16) -1 )
        {
            throw uno::RuntimeException();
        }
        pGlossaries->PutGroupDoc( pGlosGroup );
    }
    Reference< text::XAutoTextEntry >  xRet = new SwXAutoTextEntry(pGlossaries, sGroupName, sShortName);
    return xRet;
}
/*-- 21.12.98 12:42:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::removeByName(const OUString& aEntryName) throw( container::NoSuchElementException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
    {
        sal_uInt16 nIdx = pGlosGroup->GetIndex(aEntryName);
        if ( nIdx != USHRT_MAX )
            pGlosGroup->Delete(nIdx);
        delete pGlosGroup;
    }
    else
        throw container::NoSuchElementException();
}
/*-- 21.12.98 12:42:25---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXAutoTextGroup::getName(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    return sName;
}
/*-- 21.12.98 12:42:25---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::setName(const OUString& rName) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    String sNewGroup(rName);
    if( !pGlossaries )
        throw uno::RuntimeException();
    if(STRING_NOTFOUND == sNewGroup.Search(GLOS_DELIM))
    {
        sNewGroup += GLOS_DELIM;
        sNewGroup += UniString::CreateFromInt32(0);
    }
    // check value after delimiter...
    OUString aNewSuffix (rName.copy ( 1 + rName.lastIndexOf ( GLOS_DELIM ) ) );
    OUString aOldSuffix (sName.copy ( 1 + sName.lastIndexOf ( GLOS_DELIM ) ) );

    sal_Int32 nNewNumeric = aNewSuffix.toInt32();
    sal_Int32 nOldNumeric = aOldSuffix.toInt32();

    OUString aNewPrefix (rName.copy ( 0, rName.lastIndexOf ( GLOS_DELIM ) ) );
    OUString aOldPrefix (sName.copy ( 0, sName.lastIndexOf ( GLOS_DELIM ) ) );

    if ( sName == rName ||
       ( nNewNumeric == nOldNumeric && aNewPrefix == aOldPrefix ) )
        return;
    //the name must be saved, the group may be invalidated while in RenameGroupDoc()
    SwGlossaries* pTempGlossaries = pGlossaries;

    String sGrpName(sName);
    String sTitle(pGlossaries->GetGroupTitle(sGrpName));
    if(!pGlossaries->RenameGroupDoc(sGrpName, sNewGroup, sTitle))
        throw uno::RuntimeException();
    else
    {
        sName = rName;
        sGroupName = sNewGroup;
        pGlossaries = pTempGlossaries;
    }
}
/*-- 21.12.98 12:42:26---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SwXAutoTextGroup::getCount(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    int nCount = 0;
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
        nCount = pGlosGroup->GetCount();
    else
        throw uno::RuntimeException();
    delete pGlosGroup;
    return nCount;
}
/*-- 21.12.98 12:42:26---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoTextGroup::getByIndex(sal_Int32 nIndex)
    throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    sal_uInt16 nCount = 0;
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
        nCount = pGlosGroup->GetCount();
    else
        throw uno::RuntimeException();
    if(nCount > nIndex)
        aRet = getByName(pGlosGroup->GetShortName((sal_uInt16) nIndex));
    else
        throw lang::IndexOutOfBoundsException();
    delete pGlosGroup;
    return aRet;
}
/*-- 21.12.98 12:42:26---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Type SwXAutoTextGroup::getElementType(void) throw( uno::RuntimeException )
{
    return ::getCppuType((Reference<text::XAutoTextEntry>*)0);

}
/*-- 21.12.98 12:42:26---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXAutoTextGroup::hasElements(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    sal_uInt16 nCount = 0;
    if(pGlosGroup && !pGlosGroup->GetError())
        nCount = pGlosGroup->GetCount();
    else
        throw uno::RuntimeException();
    return nCount > 0;

}
/*-- 21.12.98 12:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoTextGroup::getByName(const OUString& Name)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    uno::Any aRet;
    //standard must be created
    sal_Bool bCreate = sGroupName == SwGlossaries::GetDefName();
    SwTextBlocks* pGlosGroup = pGlossaries ?
        pGlossaries->GetGroupDoc(sGroupName, bCreate) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
    {
        sal_uInt16 nIdx = pGlosGroup->GetIndex(Name);
        if( nIdx != USHRT_MAX )
        {
            Reference< text::XAutoTextEntry >  xRef;
            SwGlossaries* pGlossaries = ::GetGlossaries();
            XAutoTextEntryPtrArr& rArr = pGlossaries->GetUnoEntryArray();
            for(sal_uInt16 i = 0; i < rArr.Count(); i++)
            {
                Reference< uno::XInterface > * pxEntry = rArr.GetObject(i);
                Reference< lang::XUnoTunnel > xEntryTunnel(*pxEntry, uno::UNO_QUERY);
                SwXAutoTextEntry* pEntry = 0;
                if(xEntryTunnel.is())
                {
                    pEntry = (SwXAutoTextEntry*)xEntryTunnel->getSomething(SwXAutoTextEntry::getUnoTunnelId());
                }
                if(pEntry &&
                    COMPARE_EQUAL == pEntry->GetGroupName().CompareTo(String(sName)) &&
                        COMPARE_EQUAL == pEntry->GetEntryName().CompareTo(String(Name)))
                {
                    xRef = pEntry;
                    break;
                }
            }
            if(!xRef.is())
                xRef = new SwXAutoTextEntry(pGlossaries, sName, Name);
            aRet.setValue(&xRef, ::getCppuType((Reference< text::XAutoTextEntry>*)0));
        }
        else
            throw container::NoSuchElementException();
        delete pGlosGroup;
    }
    else
        throw uno::RuntimeException();
    return aRet;
}
/*-- 21.12.98 12:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SwXAutoTextGroup::getElementNames(void)
    throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_uInt16 nCount = 0;
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
        nCount = pGlosGroup->GetCount();
    else
        throw uno::RuntimeException();

    uno::Sequence< OUString > aEntryNames(nCount);
    OUString *pArr = aEntryNames.getArray();

    for ( sal_uInt16 i = 0; i < nCount; i++ )
        pArr[i] = pGlosGroup->GetShortName(i);
    delete pGlosGroup;
    return aEntryNames;
}
/*-- 21.12.98 12:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SwXAutoTextGroup::hasByName(const OUString& rName)
    throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;
    sal_uInt16 nCount = 0;
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(pGlosGroup && !pGlosGroup->GetError())
        nCount = pGlosGroup->GetCount();
    else
        throw uno::RuntimeException();

    for( sal_uInt16 i = 0; i < nCount; i++ )
    {
        String sCompare(pGlosGroup->GetShortName(i));
        if(COMPARE_EQUAL == sCompare.CompareIgnoreCaseToAscii(String(rName)))
        {
            bRet = sal_True;
            break;
        }
    }
    delete pGlosGroup;
    return bRet;
}

/*-- 09.02.00 15:33:30---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< beans::XPropertySetInfo >  SwXAutoTextGroup::getPropertySetInfo(void)
    throw( uno::RuntimeException )
{
    static Reference< beans::XPropertySetInfo >  xRet = aPropSet.getPropertySetInfo();
    return xRet;
}
/*-- 09.02.00 15:33:31---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::setPropertyValue(
    const OUString& rPropertyName, const uno::Any& aValue)
    throw( beans::UnknownPropertyException, beans::PropertyVetoException,
         lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                    aPropSet.getPropertyMap(), rPropertyName);

    if(!pMap)
        throw beans::UnknownPropertyException();

    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(!pGlosGroup && !pGlosGroup->GetError())
        throw uno::RuntimeException();
    switch(pMap->nWID)
    {
        case  WID_GROUP_TITLE:
        {
            OUString sNewTitle;
            aValue >>= sNewTitle;
            if(!sNewTitle.getLength())
                throw lang::IllegalArgumentException();
            sal_Bool bChanged = !sNewTitle.equals(pGlosGroup->GetName());
            pGlosGroup->SetName(sNewTitle);
            if(bChanged && HasGlossaryList())
                GetGlossaryList()->ClearGroups();
        }
        break;
    }
    delete pGlosGroup;
}
/*-- 09.02.00 15:33:31---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SwXAutoTextGroup::getPropertyValue(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    const SfxItemPropertyMap*	pMap = SfxItemPropertyMap::GetByName(
                                    aPropSet.getPropertyMap(), rPropertyName);

    if(!pMap)
        throw beans::UnknownPropertyException();
    SwTextBlocks* pGlosGroup = pGlossaries ? pGlossaries->GetGroupDoc(sGroupName, sal_False) : 0;
    if(!pGlosGroup  || pGlosGroup->GetError())
        throw uno::RuntimeException();

    uno::Any aAny;
    switch(pMap->nWID)
    {
        case  WID_GROUP_PATH:
            aAny <<= OUString(pGlosGroup->GetFileName());
        break;
        case  WID_GROUP_TITLE:
            aAny <<= OUString(pGlosGroup->GetName());
        break;
    }
    delete pGlosGroup;
    return aAny;
}
/*-- 09.02.00 15:33:31---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::addPropertyChangeListener(
    const OUString& PropertyName, const Reference< beans::XPropertyChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
}
/*-- 09.02.00 15:33:31---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::removePropertyChangeListener(
    const OUString& PropertyName, const Reference< beans::XPropertyChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
}
/*-- 09.02.00 15:33:32---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::addVetoableChangeListener(
    const OUString& PropertyName, const Reference< beans::XVetoableChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
}
/*-- 09.02.00 15:33:32---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::removeVetoableChangeListener(
    const OUString& PropertyName, const Reference< beans::XVetoableChangeListener > & aListener)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
}
/*-- 21.12.98 12:42:27---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextGroup::Invalidate()
{
    pGlossaries = 0;
    sName = aEmptyStr;
    sGroupName = aEmptyStr;
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXAutoTextGroup::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXAutoTextGroup");
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXAutoTextGroup::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.AutoTextGroup") == rServiceName;
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXAutoTextGroup::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.AutoTextGroup");
    return aRet;
}
/******************************************************************
 *
 ******************************************************************/
/* -----------------------------10.03.00 18:02--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SwXAutoTextEntry::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::CreateUnoTunnelId();
    return aSeq;
}
/* -----------------------------10.03.00 18:04--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SwXAutoTextEntry::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}
/*-- 21.12.98 12:42:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoTextEntry::SwXAutoTextEntry(SwGlossaries* pGlss, const String& rGroupName,
                                            const String& rEntryName) :
    pGlossaries(pGlss),
    sGroupName(rGroupName),
    sEntryName(rEntryName)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    xDocSh = pGlossaries->EditGroupDoc ( rGroupName, rEntryName, FALSE );
    pBodyText = new SwXBodyText ( xDocSh->GetDoc() );
    xBodyText = Reference < XServiceInfo > ( *pBodyText, UNO_QUERY);
}
/*-- 21.12.98 12:42:33---------------------------------------------------

  -----------------------------------------------------------------------*/
SwXAutoTextEntry::~SwXAutoTextEntry()
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    if ( xDocSh->GetDoc()->IsModified () )
        xDocSh->Save();
}
/*-- 21.12.98 12:42:33---------------------------------------------------

  -----------------------------------------------------------------------*/
/*-- 21.12.98 12:42:34---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextCursor >  SwXAutoTextEntry::createTextCursor(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    return pBodyText->createTextCursor();
}
/*-- 21.12.98 12:42:34---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextCursor >  SwXAutoTextEntry::createTextCursorByRange(
    const Reference< text::XTextRange > & aTextPosition) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    return pBodyText->createTextCursorByRange ( aTextPosition );
}
/*-- 21.12.98 12:42:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextEntry::insertString(const Reference< text::XTextRange > & xRange, const OUString& aString, sal_Bool bAbsorb) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    pBodyText->insertString ( xRange, aString, bAbsorb );
}
/*-- 21.12.98 12:42:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextEntry::insertControlCharacter(const Reference< text::XTextRange > & xRange,
    sal_Int16 nControlCharacter, sal_Bool bAbsorb)
        throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    pBodyText->insertControlCharacter ( xRange, nControlCharacter, bAbsorb );
}
/*-- 21.12.98 12:42:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextEntry::insertTextContent(
    const Reference< text::XTextRange > & xRange,
    const Reference< text::XTextContent > & xContent, sal_Bool bAbsorb)
        throw( lang::IllegalArgumentException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    pBodyText->insertTextContent ( xRange, xContent, bAbsorb );
}
/*-- 21.12.98 12:42:34---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextEntry::removeTextContent(
    const Reference< text::XTextContent > & xContent)
        throw( container::NoSuchElementException, uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    pBodyText->removeTextContent ( xContent );
}
/*-- 21.12.98 12:42:35---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XText >  SwXAutoTextEntry::getText(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    Reference< text::XText >  xRet =  (text::XText*)this;
    return xRet;
}
/*-- 21.12.98 12:42:35---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextRange >  SwXAutoTextEntry::getStart(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    return pBodyText->getStart();
}
/*-- 21.12.98 12:42:36---------------------------------------------------

  -----------------------------------------------------------------------*/
Reference< text::XTextRange >  SwXAutoTextEntry::getEnd(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    return pBodyText->getEnd();
}
/*-- 21.12.98 12:42:36---------------------------------------------------

  -----------------------------------------------------------------------*/
OUString SwXAutoTextEntry::getString(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    return pBodyText->getString();
}
/*-- 21.12.98 12:42:36---------------------------------------------------

  -----------------------------------------------------------------------*/
void SwXAutoTextEntry::setString(const OUString& aString) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    pBodyText->setString( aString );
}
/* -----------------15.07.99 10:11-------------------

 --------------------------------------------------*/
void SwXAutoTextEntry::applyTo(const Reference< text::XTextRange > & xTextRange)throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    sal_Bool bRet = sal_False;

    Reference<lang::XUnoTunnel> xTunnel( xTextRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    SwXTextCursor* pCursor = 0;
    SwXText *pText = 0;

    if(xTunnel.is())
    {
        pRange = reinterpret_cast < SwXTextRange* >
                ( xTunnel->getSomething( SwXTextRange::getUnoTunnelId() ) );
        pCursor = reinterpret_cast < SwXTextCursor*>
                ( xTunnel->getSomething( SwXTextCursor::getUnoTunnelId() ) );
        pText = reinterpret_cast < SwXText* >
                ( xTunnel->getSomething( SwXText::getUnoTunnelId() ) );
    }

    SwDoc* pDoc = 0;
    if ( pRange && pRange->GetBookmark())
        pDoc = pRange->GetDoc();
    else if ( pCursor && pCursor->GetCrsr() )
        pDoc = pCursor->GetDoc();
    else if ( pText && pText->GetDoc() )
    {
        xTunnel = Reference < lang::XUnoTunnel > (pText->getStart(), uno::UNO_QUERY);
        if (xTunnel.is())
        {
            pCursor = reinterpret_cast < SwXTextCursor* >
                ( xTunnel->getSomething( SwXTextCursor::getUnoTunnelId() ) );
            if (pCursor)
                pDoc = pText->GetDoc();
        }
    }

    if(!pDoc)
        throw uno::RuntimeException();
    SwPaM* pInsertPaM = 0;
    if(pRange)
    {
        SwBookmark* pBkm = pRange->GetBookmark();
        if(pBkm->GetOtherPos())
            pInsertPaM = new SwPaM(*pBkm->GetOtherPos(), pBkm->GetPos());
        else
            pInsertPaM = new SwPaM(pBkm->GetPos());
    }
    else
    {
        SwUnoCrsr* pCrsr = pCursor->GetCrsr();
        if(pCrsr->HasMark())
            pInsertPaM = new SwPaM(*pCrsr->GetPoint(), *pCrsr->GetMark());
        else
            pInsertPaM = new SwPaM(*pCrsr->GetPoint());
    }

    SwTextBlocks* pBlock = pGlossaries->GetGroupDoc(sGroupName);
    sal_Bool bResult = pBlock && !pBlock->GetError() &&
                pDoc->InsertGlossary( *pBlock, sEntryName, *pInsertPaM);
    delete pInsertPaM;

    if(!bResult)
        throw uno::RuntimeException();
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwXAutoTextEntry::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXAutoTextEntry");
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwXAutoTextEntry::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.AutoTextEntry") == rServiceName;
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
Sequence< OUString > SwXAutoTextEntry::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.AutoTextEntry");
    return aRet;
}
/* -----------------------------06.04.00 11:11--------------------------------

 ---------------------------------------------------------------------------*/
Reference< container::XNameReplace > SwXAutoTextEntry::getEvents()
    throw( uno::RuntimeException )
{
    return new SwAutoTextEventDescriptor( *this );
}
/* -----------------------------30.01.01 18:40--------------------------------

 ---------------------------------------------------------------------------*/
const struct SvEventDescription aAutotextEvents[] =
{
    { SW_EVENT_START_INS_GLOSSARY,	"OnInsertStart" },
    { SW_EVENT_END_INS_GLOSSARY,	"OnInsertDone" },
    { 0, NULL }
};

/* -----------------------------30.01.01 18:40--------------------------------

 ---------------------------------------------------------------------------*/
SwAutoTextEventDescriptor::SwAutoTextEventDescriptor(
    SwXAutoTextEntry& rAutoText ) :
        SvBaseEventDescriptor(aAutotextEvents),
        sSwAutoTextEventDescriptor(RTL_CONSTASCII_USTRINGPARAM(
            "SwAutoTextEventDescriptor")),
        rAutoTextEntry(rAutoText)
{
}
/* -----------------------------30.01.01 18:40--------------------------------

 ---------------------------------------------------------------------------*/
SwAutoTextEventDescriptor::~SwAutoTextEventDescriptor()
{
}
/* -----------------------------30.01.01 18:40--------------------------------

 ---------------------------------------------------------------------------*/
OUString SwAutoTextEventDescriptor::getImplementationName()
    throw( uno::RuntimeException )
{
    return sSwAutoTextEventDescriptor;
}
/* -----------------------------30.01.01 18:40--------------------------------

 ---------------------------------------------------------------------------*/
void SwAutoTextEventDescriptor::replaceByName(
    const USHORT nEvent,
    const SvxMacro& rMacro)
            throw(
                IllegalArgumentException,
                NoSuchElementException,
                WrappedTargetException,
                RuntimeException)
{
    DBG_ASSERT( NULL != rAutoTextEntry.GetGlossaries(),
                "Strangely enough, the AutoText vanished!" );
    DBG_ASSERT( (nEvent == SW_EVENT_END_INS_GLOSSARY) ||
                (nEvent == SW_EVENT_START_INS_GLOSSARY) ,
                "Unknown event ID" );

    const SwGlossaries* pGlossaries = rAutoTextEntry.GetGlossaries();
    SwTextBlocks* pBlocks =
        pGlossaries->GetGroupDoc( rAutoTextEntry.GetGroupName() );
    DBG_ASSERT( NULL != pBlocks,
                "can't get autotext group; SwAutoTextEntry has illegal name?");

    if( pBlocks && !pBlocks->GetError())
    {
        USHORT nIndex = pBlocks->GetIndex( rAutoTextEntry.GetEntryName() );
        if( nIndex != USHRT_MAX )
        {
            SvxMacroTableDtor aMacroTable;
            if( pBlocks->GetMacroTable( nIndex, aMacroTable ) )
            {
                SvxMacro* pNewMacro = new SvxMacro(rMacro);
                aMacroTable.Replace( nEvent, pNewMacro );
                pBlocks->SetMacroTable( nIndex, aMacroTable );
            }
        }
    }
    // else: ignore
}
/* -----------------------------30.01.01 18:40--------------------------------

 ---------------------------------------------------------------------------*/
void SwAutoTextEventDescriptor::getByName(
    SvxMacro& rMacro,
    const USHORT nEvent )
            throw(
                NoSuchElementException,
                WrappedTargetException,
                RuntimeException)
{
    DBG_ASSERT( NULL != rAutoTextEntry.GetGlossaries(), "no AutoText" );
    DBG_ASSERT( (nEvent == SW_EVENT_END_INS_GLOSSARY) ||
                (nEvent == SW_EVENT_START_INS_GLOSSARY) ,
                "Unknown event ID" );

    const SwGlossaries* pGlossaries = rAutoTextEntry.GetGlossaries();
    SwTextBlocks* pBlocks =
        pGlossaries->GetGroupDoc( rAutoTextEntry.GetGroupName() );
    DBG_ASSERT( NULL != pBlocks,
                "can't get autotext group; SwAutoTextEntry has illegal name?");

    // return empty macro, unless macro is found
    OUString sEmpty;
    SvxMacro aEmptyMacro(sEmpty, sEmpty);
    rMacro = aEmptyMacro;

    if ( pBlocks &&  !pBlocks->GetError())
    {
        USHORT nIndex = pBlocks->GetIndex( rAutoTextEntry.GetEntryName() );
        if( nIndex != USHRT_MAX )
        {
            SvxMacroTableDtor aMacroTable;
            if( pBlocks->GetMacroTable( nIndex, aMacroTable ) )
            {
                SvxMacro *pMacro = aMacroTable.Get( nEvent );
                if( pMacro )
                    rMacro = *pMacro;
            }
        }
    }
}



void SwGlossaries::RemoveFileFromList( const String& rGroup )
{
    if(pGlosArr)
    {
        const sal_uInt16 nCount = pGlosArr->Count();
        for(sal_uInt16 i = 0; i < nCount; ++i)
        {
            String *pTmp = (*pGlosArr)[i];
            if(*pTmp == rGroup)
            {
                //UNO-Objekt fuer die Gruppe aus dem Array loeschen
                rtl::OUString aUName = rGroup;
                sal_uInt16 nXCount = aGlosGroupArr.Count();
                for(sal_uInt16 j = 0; j < nXCount; ++j)
                {
                    STAR_REFERENCE( text::XAutoTextGroup ) * pxGroup =
                                                aGlosGroupArr.GetObject(j);
                    STAR_REFERENCE( container::XNamed )  xNamed( *pxGroup,
                                                    UNO_NMSPC::UNO_QUERY);

                    if( xNamed->getName() == aUName )
                    {
                        STAR_NMSPC::text::XAutoTextGroup* pGroup = pxGroup->get();
                        ((SwXAutoTextGroup*)pGroup)->Invalidate();
                        aGlosGroupArr.Remove(j);
                        delete pxGroup;
                        break;
                    }
                }
                // alle UNO-Objekte fuer enthaltene Entries loeschen - rueckwaerts!
                nXCount = aGlosEntryArr.Count();
                for(j = nXCount; j; --j)
                {
                    STAR_REFERENCE( uno::XInterface ) * pxEntry =
                                                aGlosEntryArr.GetObject(j);
                    STAR_REFERENCE( lang::XUnoTunnel )  xTunnel(*pxEntry,
                                                        UNO_NMSPC::UNO_QUERY);
                    SwXAutoTextEntry* pEntry = (SwXAutoTextEntry*)
                                xTunnel->getSomething(SwXAutoTextEntry::getUnoTunnelId());
                    if(pEntry->GetGroupName() == rGroup)
                    {
                        pEntry->Invalidate();
                        aGlosEntryArr.Remove(j);
                        delete pxEntry;
                    }
                }

                pGlosArr->Remove(i);
                delete pTmp;
                break;
            }
        }
    }
}

void SwGlossaries::InvalidateUNOOjects()
{
    USHORT i, nCount = aGlosGroupArr.Count();
    for( i = 0; i < nCount; ++i )
    {
        STAR_NMSPC::text::XAutoTextGroup* pGroup = aGlosGroupArr.GetObject(i)->get();
        ((SwXAutoTextGroup*)pGroup)->Invalidate();
    }
    nCount = aGlosEntryArr.Count();
    for(i = 0; i < nCount; ++i)
    {
        STAR_REFERENCE( uno::XInterface ) * pxEntry = aGlosEntryArr.GetObject(i);
        STAR_REFERENCE( lang::XUnoTunnel ) xTunnel(*pxEntry, UNO_NMSPC::UNO_QUERY);
        DBG_ASSERT(xTunnel.is(), "No tunnel for SwXAutoTextEntry?");
        SwXAutoTextEntry* pEntry = (SwXAutoTextEntry*)
                    xTunnel->getSomething(SwXAutoTextEntry::getUnoTunnelId());
        pEntry->Invalidate();
    }
}
