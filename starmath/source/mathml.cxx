/*************************************************************************
 *
 *  $RCSfile: mathml.cxx,v $
 *
 *  $Revision: 1.73 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-11 13:08:41 $
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

/*
 Warning: The SvXMLElementExport helper class creates the beginning and
 closing tags of xml elements in its constructor and destructor, so theres
 hidden stuff going on, on occasion the ordering of these classes declarations
 may be significant
*/

/*todo: Change characters and tcharacters to accumulate the characters together
into one string, xml parser hands them to us line by line rather than all in
one go*/

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif

#ifndef _TOOLS_DEBUG_H
#include <tools/debug.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _SFXECODE_HXX
#include <svtools/sfxecode.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_SAVEOPT_HXX
#include <svtools/saveopt.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
#endif

#ifndef UNOMODEL_HXX
#include <unomodel.hxx>
#endif
#ifndef MATHML_HXX
#include <mathml.hxx>
#endif
#ifndef DOCUMENT_HXX
#include <document.hxx>
#endif
#ifndef UTILITY_HXX
#include <utility.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_XMLKYWD_HXX
#include <xmloff/xmlkywd.hxx>
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_ATTRLIST_HXX
#include <xmloff/attrlist.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif
#ifndef _XMLOFF_XMLMETAI_HXX
#include <xmloff/xmlmetai.hxx>
#endif

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif
#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif

#include <com/sun/star/xml/sax/XErrorHandler.hpp>
#include <com/sun/star/xml/sax/XEntityResolver.hpp>
#include <com/sun/star/xml/sax/InputSource.hpp>
#include <com/sun/star/xml/sax/XDTDHandler.hpp>
#include <com/sun/star/xml/sax/XParser.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/io/XActiveDataControl.hpp>

#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPIOEXCEPTION_HPP_
#include <com/sun/star/packages/zip/ZipIOException.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORFACTORY_HPP_
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COMPHELPER_GENERICPROPERTYSET_HXX_
#include <comphelper/genericpropertyset.hxx>
#endif

#ifndef _SFX_ITEMPROP_HXX
#include <svtools/itemprop.hxx>
#endif

#include <sfx2/frame.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::document;
using namespace com::sun::star::container;
using namespace com::sun::star::beans;
using namespace com::sun::star;
using namespace ::xmloff::token;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;

#ifndef MATHTYPE_HXX
#include "mathtype.hxx"
#endif

#ifndef STARMATH_HRC
#include <starmath.hrc>
#endif
#ifndef PARSE_HXX
#include <parser.hxx>
#endif

#define IMPORT_SVC_NAME RTL_CONSTASCII_USTRINGPARAM("com.sun.star.xml.XMLImportFilter")
#define EXPORT_SVC_NAME RTL_CONSTASCII_USTRINGPARAM("com.sun.star.xml.XMLExportFilter")

#undef WANTEXCEPT

/// read a component (file + filter version)
ULONG SmXMLWrapper::ReadThroughComponent(
    Reference<io::XInputStream> xInputStream,
    Reference<XComponent> xModelComponent,
    Reference<lang::XMultiServiceFactory> & rFactory,
    Reference<beans::XPropertySet> & rPropSet,
    const sal_Char* pFilterName,
    sal_Bool bEncrypted )
{
    ULONG nError = ERRCODE_SFX_DOLOADFAILED;
    DBG_ASSERT(xInputStream.is(), "input stream missing");
    DBG_ASSERT(xModelComponent.is(), "document missing");
    DBG_ASSERT(rFactory.is(), "factory missing");
    DBG_ASSERT(NULL != pFilterName,"I need a service name for the component!");

    // prepare ParserInputSrouce
    xml::sax::InputSource aParserInput;
    aParserInput.aInputStream = xInputStream;

    // get parser
    Reference< xml::sax::XParser > xParser(
        rFactory->createInstance(
            OUString::createFromAscii("com.sun.star.xml.sax.Parser") ),
        UNO_QUERY );
    DBG_ASSERT( xParser.is(), "Can't create parser" );
    if( !xParser.is() )
        return nError;

    Sequence<Any> aArgs( 1 );
    aArgs[0] <<= rPropSet;

    // get filter
    Reference< xml::sax::XDocumentHandler > xFilter(
        rFactory->createInstanceWithArguments(
            OUString::createFromAscii(pFilterName), aArgs ),
        UNO_QUERY );
    DBG_ASSERT( xFilter.is(), "Can't instantiate filter component." );
    if( !xFilter.is() )
        return nError;

    // connect parser and filter
    xParser->setDocumentHandler( xFilter );

    // connect model and filter
    Reference < XImporter > xImporter( xFilter, UNO_QUERY );
    xImporter->setTargetDocument( xModelComponent );

    // finally, parser the stream
    try
    {
        xParser->parseStream( aParserInput );

        uno::Reference<lang::XUnoTunnel> xFilterTunnel;
        xFilterTunnel = uno::Reference<lang::XUnoTunnel>
            ( xFilter, uno::UNO_QUERY );
        SmXMLImport *pFilter = (SmXMLImport *)xFilterTunnel->getSomething(
            SmXMLImport::getUnoTunnelId() );
        if( pFilter && pFilter->GetSuccess() )
            nError = 0;
    }
    catch( xml::sax::SAXParseException& )
    {
        if( bEncrypted )
            nError = ERRCODE_SFX_WRONGPASSWORD;
    }
    catch( xml::sax::SAXException& )
    {
        if( bEncrypted )
            nError = ERRCODE_SFX_WRONGPASSWORD;
    }
    catch( packages::zip::ZipIOException& )
    {
        nError = ERRCODE_IO_BROKENPACKAGE;
    }
    catch( io::IOException& )
    {
    }

    return nError;
}

ULONG SmXMLWrapper::ReadThroughComponent(
    const uno::Reference< embed::XStorage >& xStorage,
    Reference<XComponent> xModelComponent,
    const sal_Char* pStreamName,
    const sal_Char* pCompatibilityStreamName,
    Reference<lang::XMultiServiceFactory> & rFactory,
    Reference<beans::XPropertySet> & rPropSet,
    const sal_Char* pFilterName )
{
    DBG_ASSERT(xStorage.is(), "Need storage!");
    DBG_ASSERT(NULL != pStreamName, "Please, please, give me a name!");

    // open stream (and set parser input)
    OUString sStreamName = OUString::createFromAscii(pStreamName);
    uno::Reference < container::XNameAccess > xAccess( xStorage, uno::UNO_QUERY );
    if ( !xAccess->hasByName(sStreamName) || !xStorage->isStreamElement(sStreamName) )
    {
        // stream name not found! Then try the compatibility name.
        // do we even have an alternative name?
        if ( pCompatibilityStreamName )
            sStreamName = OUString::createFromAscii(pCompatibilityStreamName);
    }

    // get input stream
    try
    {
        uno::Reference < io::XStream > xEventsStream = xStorage->openStreamElement( sStreamName, embed::ElementModes::READ );

        // determine if stream is encrypted or not
        uno::Reference < beans::XPropertySet > xProps( xEventsStream, uno::UNO_QUERY );
        Any aAny = xProps->getPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM("Encrypted") ) );
        sal_Bool bEncrypted = sal_False;
        if ( aAny.getValueType() == ::getBooleanCppuType() )
            aAny >>= bEncrypted;

        // set Base URL
        if( rPropSet.is() )
        {
            OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("StreamName") );
            rPropSet->setPropertyValue( sPropName, makeAny( sStreamName ) );
        }


        Reference < io::XInputStream > xStream = xEventsStream->getInputStream();
        return ReadThroughComponent( xStream, xModelComponent, rFactory, rPropSet, pFilterName, bEncrypted );
    }
    catch ( packages::WrongPasswordException& )
    {
        return ERRCODE_SFX_WRONGPASSWORD;
    }
    catch ( uno::Exception& )
    {
    }

    return ERRCODE_SFX_DOLOADFAILED;
}

ULONG SmXMLWrapper::Import(SfxMedium &rMedium)
{
    ULONG nError = ERRCODE_SFX_DOLOADFAILED;

    uno::Reference<lang::XMultiServiceFactory> xServiceFactory(
        utl::getProcessServiceFactory());
    DBG_ASSERT(xServiceFactory.is(), "XMLReader::Read: got no service manager");
    if( !xServiceFactory.is() )
        return nError;

    //Make a model component from our SmModel
    uno::Reference< lang::XComponent > xModelComp( xModel, uno::UNO_QUERY );
    DBG_ASSERT( xModelComp.is(), "XMLReader::Read: got no model" );

    // try to get an XStatusIndicator from the Medium
    uno::Reference<task::XStatusIndicator> xStatusIndicator;

    sal_Bool bEmbedded = sal_False;
    uno::Reference <lang::XUnoTunnel> xTunnel;
    xTunnel = uno::Reference <lang::XUnoTunnel> (xModel,uno::UNO_QUERY);
    SmModel *pModel = reinterpret_cast<SmModel *>
        (xTunnel->getSomething(SmModel::getUnoTunnelId()));

    SmDocShell *pDocShell = pModel ?
            static_cast<SmDocShell*>(pModel->GetObjectShell()) : 0;
    if (pDocShell)
    {
//        if (pDocShell->GetMedium())
        {
            DBG_ASSERT( pDocShell->GetMedium() == &rMedium,
                    "different SfxMedium found" );

            SfxItemSet* pSet = rMedium.GetItemSet();
            if (pSet)
            {
                const SfxUnoAnyItem* pItem = static_cast<const SfxUnoAnyItem*>(
                    pSet->GetItem(SID_PROGRESS_STATUSBAR_CONTROL) );
                if (pItem)
                    pItem->GetValue() >>= xStatusIndicator;
            }
        }

        if( SFX_CREATE_MODE_EMBEDDED == pDocShell->GetCreateMode() )
            bEmbedded = sal_True;
    }

    comphelper::PropertyMapEntry aInfoMap[] =
    {
        { "PrivateData", sizeof("PrivateData")-1, 0,
              &::getCppuType( (Reference<XInterface> *)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "BaseURI", sizeof("BaseURI")-1, 0,
              &::getCppuType( (OUString *)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "StreamRelPath", sizeof("StreamRelPath")-1, 0,
              &::getCppuType( (OUString *)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "StreamName", sizeof("StreamName")-1, 0,
              &::getCppuType( (OUString *)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { NULL, 0, 0, NULL, 0, 0 }
    };
    uno::Reference< beans::XPropertySet > xInfoSet(
                comphelper::GenericPropertySet_CreateInstance(
                            new comphelper::PropertySetInfo( aInfoMap ) ) );

    // Set base URI
    OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("BaseURI") );
    xInfoSet->setPropertyValue( sPropName, makeAny( rMedium.GetBaseURL() ) );

    sal_Int32 nSteps=3;
    if( !(rMedium.IsStorage()))
        nSteps = 1;

    sal_Int32 nProgressRange(nSteps);
    if (xStatusIndicator.is())
    {
        xStatusIndicator->start(String(SmResId(STR_STATSTR_READING)),
            nProgressRange);
    }

    nSteps=0;
    if (xStatusIndicator.is())
        xStatusIndicator->setValue(nSteps++);

    if( rMedium.IsStorage())
    {
        // TODO/LATER: handle the case of embedded links gracefully
        if( bEmbedded ) // && !rMedium.GetStorage()->IsRoot() )
        {
            OUString aName( RTL_CONSTASCII_USTRINGPARAM( "dummyObjName" ) );
            if ( rMedium.GetItemSet() )
            {
                const SfxStringItem* pDocHierarchItem = static_cast<const SfxStringItem*>(
                    rMedium.GetItemSet()->GetItem(SID_DOC_HIERARCHICALNAME) );
                if ( pDocHierarchItem )
                    aName = pDocHierarchItem->GetValue();
            }

            if( aName.getLength() )
            {
                sPropName = OUString(RTL_CONSTASCII_USTRINGPARAM("StreamRelPath"));
                xInfoSet->setPropertyValue( sPropName, makeAny( aName ) );
            }
        }

        sal_Bool bOASIS = ( SotStorage::GetVersion( rMedium.GetStorage() ) > SOFFICE_FILEFORMAT_60 );
        if (xStatusIndicator.is())
            xStatusIndicator->setValue(nSteps++);

        ULONG nWarn = ReadThroughComponent(
            rMedium.GetStorage(), xModelComp, "meta.xml", "Meta.xml",
            xServiceFactory, xInfoSet,
                (bOASIS ? "com.sun.star.comp.Math.XMLOasisMetaImporter"
                        : "com.sun.star.comp.Math.XMLMetaImporter")	);

        if ( nWarn != ERRCODE_IO_BROKENPACKAGE )
        {
            if (xStatusIndicator.is())
                xStatusIndicator->setValue(nSteps++);

            nWarn = ReadThroughComponent(
                rMedium.GetStorage(), xModelComp, "settings.xml", 0,
                xServiceFactory, xInfoSet,
                (bOASIS ? "com.sun.star.comp.Math.XMLOasisSettingsImporter"
                        : "com.sun.star.comp.Math.XMLSettingsImporter" ) );

            if ( nWarn != ERRCODE_IO_BROKENPACKAGE )
            {
                if (xStatusIndicator.is())
                    xStatusIndicator->setValue(nSteps++);

                nError = ReadThroughComponent(
                    rMedium.GetStorage(), xModelComp, "content.xml", "Content.xml",
                    xServiceFactory, xInfoSet, "com.sun.star.comp.Math.XMLImporter" );
            }
            else
                nError = ERRCODE_IO_BROKENPACKAGE;
        }
        else
            nError = ERRCODE_IO_BROKENPACKAGE;
    }
    else
    {
        Reference<io::XInputStream> xInputStream =
            new utl::OInputStreamWrapper(rMedium.GetInStream());

        if (xStatusIndicator.is())
            xStatusIndicator->setValue(nSteps++);

        nError = ReadThroughComponent( xInputStream, xModelComp,
            xServiceFactory, xInfoSet, "com.sun.star.comp.Math.XMLImporter", FALSE );
    }

    if (xStatusIndicator.is())
        xStatusIndicator->end();
    return nError;
}

SmXMLImport::SmXMLImport(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
    sal_uInt16 nImportFlags)
:	SvXMLImport( xServiceFactory, nImportFlags ),
    pPresLayoutElemTokenMap(0),
    pPresElemTokenMap(0),
    pPresScriptEmptyElemTokenMap(0),
    pPresTableElemTokenMap(0),
    pPresLayoutAttrTokenMap(0),
    pFencedAttrTokenMap(0),
    pOperatorAttrTokenMap(0),
    pColorTokenMap(0),
    pAnnotationAttrTokenMap(0),
    bSuccess(sal_False)
{
}

SmXMLImport::SmXMLImport(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
    com::sun::star::uno::Reference<com::sun::star::frame::XModel> &rModel,
    const rtl::OUString &rFileName)
:	SvXMLImport( xServiceFactory, rModel ) ,
    pPresLayoutElemTokenMap(0),
    pPresElemTokenMap(0),
    pPresScriptEmptyElemTokenMap(0),
    pPresTableElemTokenMap(0),
    pPresLayoutAttrTokenMap(0),
    pFencedAttrTokenMap(0),
    pOperatorAttrTokenMap(0),
    pColorTokenMap(0),
    pAnnotationAttrTokenMap(0),
    bSuccess(sal_False)
{
}

const uno::Sequence< sal_Int8 > & SmXMLImport::getUnoTunnelId() throw()
{
    static uno::Sequence< sal_Int8 > * pSeq = 0;
    if( !pSeq )
    {
        osl::Guard< osl::Mutex > aGuard( osl::Mutex::getGlobalMutex() );
        if( !pSeq )
        {
            static uno::Sequence< sal_Int8 > aSeq( 16 );
            rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0, sal_True );
            pSeq = &aSeq;
        }
    }
    return *pSeq;
}

// #110680#
SmXMLExport::SmXMLExport(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
    sal_uInt16 nExportFlags)
:	SvXMLExport( xServiceFactory, MAP_INCH, XML_MATH, nExportFlags ) ,
    pTree(0) ,
    bSuccess(sal_False)
{
}

// #110680#
SmXMLExport::SmXMLExport(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
    const SmNode *pIn,
    const rtl::OUString &rFileName,
    com::sun::star::uno::Reference<	com::sun::star::xml::sax::XDocumentHandler> &rHandler)
:	SvXMLExport( xServiceFactory, rFileName, rHandler ),
    pTree(pIn),
    bSuccess(sal_False)
{
}

const uno::Sequence< sal_Int8 > & SmXMLExport::getUnoTunnelId() throw()
{
    static uno::Sequence< sal_Int8 > * pSeq = 0;
    if( !pSeq )
    {
        osl::Guard< osl::Mutex > aGuard( osl::Mutex::getGlobalMutex() );
        if( !pSeq )
        {
            static uno::Sequence< sal_Int8 > aSeq( 16 );
            rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0, sal_True );
            pSeq = &aSeq;
        }
    }
    return *pSeq;
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLImport_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLImporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLImport_getSupportedServiceNames()
        throw()
{
    const OUString aServiceName( IMPORT_SVC_NAME );
    const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
        return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLImport_createInstance(
    const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
    throw( uno::Exception )
{
    // #110680#
    // return (cppu::OWeakObject*)new SmXMLImport(IMPORT_ALL);
    return (cppu::OWeakObject*)new SmXMLImport(rSMgr, IMPORT_ALL);
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLExport_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLExporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLExport_getSupportedServiceNames()
        throw()
{
    const OUString aServiceName( EXPORT_SVC_NAME );
    const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
        return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLExport_createInstance(
    const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
    throw( uno::Exception )
{
    // #110680#
    // return (cppu::OWeakObject*)new SmXMLExport( EXPORT_ALL );
    // EXPORT_OASIS is required here allthough there is no differrence between
    // OOo and OASIS, because without the flag, a transformation to OOo would
    // be chained in.
    return (cppu::OWeakObject*)new SmXMLExport( rSMgr, EXPORT_OASIS|EXPORT_ALL );
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLImportMeta_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLOasisMetaImporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLImportMeta_getSupportedServiceNames()
        throw()
{
    const OUString aServiceName( IMPORT_SVC_NAME );
    const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
        return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLImportMeta_createInstance(
    const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
    throw( uno::Exception )
{
    // #110680#
    // return (cppu::OWeakObject*)new SmXMLImport( IMPORT_META );
    return (cppu::OWeakObject*)new SmXMLImport( rSMgr, IMPORT_META );
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLExportMetaOOO_getImplementationName() throw()
{
return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLMetaExporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLExportMetaOOO_getSupportedServiceNames()
    throw()
{
const OUString aServiceName( EXPORT_SVC_NAME );
const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLExportMetaOOO_createInstance(
const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
throw( uno::Exception )
{
// #110680#
// return (cppu::OWeakObject*)new SmXMLExport( EXPORT_META );
return (cppu::OWeakObject*)new SmXMLExport( rSMgr, EXPORT_META );
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLExportMeta_getImplementationName() throw()
{
return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLOasisMetaExporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLExportMeta_getSupportedServiceNames()
    throw()
{
const OUString aServiceName( EXPORT_SVC_NAME );
const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLExportMeta_createInstance(
const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
throw( uno::Exception )
{
// #110680#
// return (cppu::OWeakObject*)new SmXMLExport( EXPORT_META );
return (cppu::OWeakObject*)new SmXMLExport( rSMgr, EXPORT_OASIS|EXPORT_META );
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLImportSettings_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLOasisSettingsImporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLImportSettings_getSupportedServiceNames()
        throw()
{
    const OUString aServiceName( IMPORT_SVC_NAME );
    const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
        return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLImportSettings_createInstance(
    const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
    throw( uno::Exception )
{
    // #110680#
    // return (cppu::OWeakObject*)new SmXMLImport( IMPORT_SETTINGS );
    return (cppu::OWeakObject*)new SmXMLImport( rSMgr, IMPORT_SETTINGS );
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLExportSettingsOOO_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLSettingsExporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLExportSettingsOOO_getSupportedServiceNames()
        throw()
{
    const OUString aServiceName( EXPORT_SVC_NAME );
    const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
        return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLExportSettingsOOO_createInstance(
    const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
    throw( uno::Exception )
{
    // #110680#
    // return (cppu::OWeakObject*)new SmXMLExport( EXPORT_SETTINGS );
    return (cppu::OWeakObject*)new SmXMLExport( rSMgr, EXPORT_SETTINGS );
}

//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLExportSettings_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLOasisSettingsExporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLExportSettings_getSupportedServiceNames()
        throw()
{
    const OUString aServiceName( EXPORT_SVC_NAME );
    const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
        return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLExportSettings_createInstance(
    const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
    throw( uno::Exception )
{
    // #110680#
    // return (cppu::OWeakObject*)new SmXMLExport( EXPORT_SETTINGS );
    return (cppu::OWeakObject*)new SmXMLExport( rSMgr, EXPORT_OASIS|EXPORT_SETTINGS );
}


//------------------------------------------------------------------------------

OUString SAL_CALL SmXMLExportContent_getImplementationName() throw()
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Math.XMLContentExporter" ) );
}

uno::Sequence< OUString > SAL_CALL SmXMLExportContent_getSupportedServiceNames()
        throw()
{
    const OUString aServiceName( EXPORT_SVC_NAME );
    const uno::Sequence< OUString > aSeq( &aServiceName, 1 );
        return aSeq;
}

uno::Reference< uno::XInterface > SAL_CALL SmXMLExportContent_createInstance(
    const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
    throw( uno::Exception )
{
    // #110680#
    // return (cppu::OWeakObject*)new SmXMLExport( EXPORT_CONTENT );
    // The EXPORT_OASIS flag is only required to avoid that a transformer is
    // chanied in
    return (cppu::OWeakObject*)new SmXMLExport( rSMgr, EXPORT_OASIS|EXPORT_CONTENT );
}

//------------------------------------------------------------------------------

// XServiceInfo
// override empty method from parent class
rtl::OUString SAL_CALL SmXMLExport::getImplementationName()
    throw(uno::RuntimeException)
{
    switch( getExportFlags() )
    {
        default:
        case EXPORT_ALL:
            return SmXMLExport_getImplementationName();
            break;
        case EXPORT_META:
            return SmXMLExportMeta_getImplementationName();
            break;
        case EXPORT_SETTINGS:
            return SmXMLExportSettings_getImplementationName();
            break;
        case EXPORT_CONTENT:
            return SmXMLExportContent_getImplementationName();
            break;
    }
}

// XServiceInfo
// override empty method from parent class
rtl::OUString SAL_CALL SmXMLImport::getImplementationName()
    throw(uno::RuntimeException)
{
    switch( getImportFlags() )
    {
        default:
        case IMPORT_ALL:
            return SmXMLImport_getImplementationName();
            break;
        case IMPORT_META:
            return SmXMLImportMeta_getImplementationName();
            break;
        case IMPORT_SETTINGS:
            return SmXMLImportSettings_getImplementationName();
            break;
    }
}


sal_Int64 SAL_CALL SmXMLImport::getSomething( const uno::Sequence< sal_Int8 >&
    rId ) throw(uno::RuntimeException)
{
    if( rId.getLength() == 16 &&
        0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
        rId.getConstArray(), 16 ) )
    return (sal_Int64)this;

    return SvXMLImport::getSomething( rId );
}

sal_Int64 SAL_CALL SmXMLExport::getSomething( const uno::Sequence< sal_Int8 >&
    rId ) throw(uno::RuntimeException)
{
    if( rId.getLength() == 16 &&
        0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
        rId.getConstArray(), 16 ) )
    return (sal_Int64)this;

    return SvXMLExport::getSomething( rId );
}


void SmXMLImport::endDocument(void)
    throw(xml::sax::SAXException, uno::RuntimeException)
{
    //Set the resulted tree into the SmDocShell where it belongs
    SmNode *pTree;
    if (pTree = GetTree())
    {
        uno::Reference <frame::XModel> xModel = GetModel();
        uno::Reference <lang::XUnoTunnel> xTunnel;
        xTunnel = uno::Reference <lang::XUnoTunnel> (xModel,uno::UNO_QUERY);
        SmModel *pModel = reinterpret_cast<SmModel *>
            (xTunnel->getSomething(SmModel::getUnoTunnelId()));

        if (pModel)
        {
            SmDocShell *pDocShell =
                static_cast<SmDocShell*>(pModel->GetObjectShell());
            pDocShell->SetFormulaTree(pTree);
            if (0 == aText.Len())  //If we picked up no annotation text
            {
                //Make up some editable text
                aText = pDocShell->GetText();
                pTree->CreateTextFromNode(aText);
                aText.EraseTrailingChars();
                if((aText.GetChar(0) == '{') &&
                    (aText.GetChar(aText.Len()-1) == '}'))
                {
                    aText.Erase(0,1);
                    aText.Erase(aText.Len()-1,1);
                }
            }
            pDocShell->SetText( String() );

            // Convert symbol names
            SmParser &rParser = pDocShell->GetParser();
            BOOL bVal = rParser.IsImportSymbolNames();
            rParser.SetImportSymbolNames( TRUE );
            SmNode *pTree = rParser.Parse( aText );
            aText = rParser.GetText();
            delete pTree;
            rParser.SetImportSymbolNames( bVal );

            pDocShell->SetText( aText );
        }
        DBG_ASSERT(pModel,"So there *was* a uno problem after all");

        bSuccess = sal_True;
    }

    SvXMLImport::endDocument();
}

/// export through an XML exporter component (output stream version)
sal_Bool SmXMLWrapper::WriteThroughComponent(
    Reference<io::XOutputStream> xOutputStream,
    Reference<XComponent> xComponent,
    Reference<lang::XMultiServiceFactory> & rFactory,
    Reference<beans::XPropertySet> & rPropSet,
    const sal_Char* pComponentName )
{
    DBG_ASSERT(xOutputStream.is(), "I really need an output stream!");
    DBG_ASSERT(xComponent.is(), "Need component!");
    DBG_ASSERT(NULL != pComponentName, "Need component name!");

    // get component
    Reference< io::XActiveDataSource > xSaxWriter(
        rFactory->createInstance(
            OUString::createFromAscii("com.sun.star.xml.sax.Writer") ),
        UNO_QUERY );
    DBG_ASSERT( xSaxWriter.is(), "can't instantiate XML writer" );
    if(!xSaxWriter.is())
        return sal_False;

    // connect XML writer to output stream
    xSaxWriter->setOutputStream( xOutputStream );

    // prepare arguments (prepend doc handler to given arguments)
    Reference<xml::sax::XDocumentHandler> xDocHandler( xSaxWriter,UNO_QUERY);

    Sequence<Any> aArgs( 2 );
    aArgs[0] <<= xDocHandler;
    aArgs[1] <<= rPropSet;

    // get filter component
    Reference< document::XExporter > xExporter(
        rFactory->createInstanceWithArguments(
            OUString::createFromAscii(pComponentName), aArgs), UNO_QUERY);
    DBG_ASSERT( xExporter.is(),
            "can't instantiate export filter component" );
    if( !xExporter.is() )
        return sal_False;


    // connect model and filter
    xExporter->setSourceDocument( xComponent );

    // filter!
    Reference < XFilter > xFilter( xExporter, UNO_QUERY );
    uno::Sequence< PropertyValue > aProps(0);
    xFilter->filter( aProps );

    uno::Reference<lang::XUnoTunnel> xFilterTunnel;
    xFilterTunnel = uno::Reference<lang::XUnoTunnel>
        ( xFilter, uno::UNO_QUERY );
    SmXMLExport *pFilter = (SmXMLExport *)xFilterTunnel->getSomething(
        SmXMLExport::getUnoTunnelId() );
    return pFilter ? pFilter->GetSuccess() : sal_True;
}

/// export through an XML exporter component (storage version)
sal_Bool SmXMLWrapper::WriteThroughComponent(
    const Reference < embed::XStorage >& xStorage,
    Reference<XComponent> xComponent,
    const sal_Char* pStreamName,
    Reference<lang::XMultiServiceFactory> & rFactory,
    Reference<beans::XPropertySet> & rPropSet,
    const sal_Char* pComponentName,
    sal_Bool bCompress
    )
{
    DBG_ASSERT(xStorage.is(), "Need storage!");
    DBG_ASSERT(NULL != pStreamName, "Need stream name!");

    // open stream
    Reference < io::XStream > xStream;
    OUString sStreamName = OUString::createFromAscii(pStreamName);
    try
    {
        xStream = xStorage->openStreamElement( sStreamName,
            embed::ElementModes::READWRITE | embed::ElementModes::TRUNCATE );
    }
    catch ( uno::Exception& )
    {
        DBG_ERROR( "Can't create output stream in package!" );
        return sal_False;
    }

    String aPropName( String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM("MediaType") ) );
    OUString aMime( RTL_CONSTASCII_USTRINGPARAM("text/xml") );
    uno::Any aAny;
    aAny <<= aMime;

    uno::Reference < beans::XPropertySet > xSet( xStream, uno::UNO_QUERY );
    xSet->setPropertyValue( aPropName, aAny );

    if( !bCompress )
    {
        aPropName = String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM("Compressed") );
        sal_Bool bFalse = sal_False;
        aAny.setValue( &bFalse, ::getBooleanCppuType() );
        xSet->setPropertyValue( aPropName, aAny );
    }
    else
    {
//REMOVE	        OUString aPropName( RTL_CONSTASCII_USTRINGPARAM("Encrypted") );
        OUString aPropName( RTL_CONSTASCII_USTRINGPARAM("UseCommonStoragePasswordEncryption") );
        sal_Bool bTrue = sal_True;
        aAny.setValue( &bTrue, ::getBooleanCppuType() );
        xSet->setPropertyValue( aPropName, aAny );
    }

    // set Base URL
    if( rPropSet.is() )
    {
        OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("StreamName") );
        rPropSet->setPropertyValue( sPropName, makeAny( sStreamName ) );
    }

    // write the stuff
    sal_Bool bRet = WriteThroughComponent( xStream->getOutputStream(), xComponent, rFactory,
        rPropSet, pComponentName );

    // stream is closed by SAX parser
    //if( bRet )
    //    xStream->getOutputStream()->closeOutput();

    return bRet;
}

sal_Bool SmXMLWrapper::Export(SfxMedium &rMedium)
{
    sal_Bool bRet=sal_True;
    uno::Reference<lang::XMultiServiceFactory>
        xServiceFactory(utl::getProcessServiceFactory());
    DBG_ASSERT(xServiceFactory.is(),"got no service manager");

    //Get model
    uno::Reference< lang::XComponent > xModelComp(xModel, uno::UNO_QUERY );

    sal_Bool bEmbedded = sal_False;
    uno::Reference <lang::XUnoTunnel> xTunnel;
    xTunnel = uno::Reference <lang::XUnoTunnel> (xModel,uno::UNO_QUERY);
    SmModel *pModel = reinterpret_cast<SmModel *>
        (xTunnel->getSomething(SmModel::getUnoTunnelId()));

    SmDocShell *pDocShell = pModel ?
            static_cast<SmDocShell*>(pModel->GetObjectShell()) : 0;
    if( pDocShell &&
        SFX_CREATE_MODE_EMBEDDED == pDocShell->GetCreateMode() )
        bEmbedded = sal_True;

    uno::Reference<task::XStatusIndicator> xStatusIndicator;
    if (!bEmbedded)
    {
        if (pDocShell /*&& pDocShell->GetMedium()*/)
        {
            DBG_ASSERT( pDocShell->GetMedium() == &rMedium,
                    "different SfxMedium found" );

            SfxItemSet* pSet = rMedium.GetItemSet();
            if (pSet)
            {
                const SfxUnoAnyItem* pItem = static_cast<const SfxUnoAnyItem*>(
                    pSet->GetItem(SID_PROGRESS_STATUSBAR_CONTROL) );
                if (pItem)
                    pItem->GetValue() >>= xStatusIndicator;
            }
        }

        // set progress range and start status indicator
        if (xStatusIndicator.is())
        {
            sal_Int32 nProgressRange = bFlat ? 1 : 3;
            xStatusIndicator->start(String(SmResId(STR_STATSTR_WRITING)),
                nProgressRange);
        }
    }


    // create XPropertySet with three properties for status indicator
    comphelper::PropertyMapEntry aInfoMap[] =
    {
        { "UsePrettyPrinting", sizeof("UsePrettyPrinting")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "BaseURI", sizeof("BaseURI")-1, 0,
              &::getCppuType( (OUString *)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "StreamRelPath", sizeof("StreamRelPath")-1, 0,
              &::getCppuType( (OUString *)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "StreamName", sizeof("StreamName")-1, 0,
              &::getCppuType( (OUString *)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { NULL, 0, 0, NULL, 0, 0 }
    };
    uno::Reference< beans::XPropertySet > xInfoSet(
                comphelper::GenericPropertySet_CreateInstance(
                            new comphelper::PropertySetInfo( aInfoMap ) ) );

    SvtSaveOptions aSaveOpt;
    OUString sUsePrettyPrinting(RTL_CONSTASCII_USTRINGPARAM("UsePrettyPrinting"));
    sal_Bool bUsePrettyPrinting( bFlat || aSaveOpt.IsPrettyPrinting() );
    Any aAny;
    aAny.setValue( &bUsePrettyPrinting, ::getBooleanCppuType() );
    xInfoSet->setPropertyValue( sUsePrettyPrinting, aAny );

    // Set base URI
    OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("BaseURI") );
    xInfoSet->setPropertyValue( sPropName, makeAny( rMedium.GetBaseURL( true ) ) );

    sal_Int32 nSteps=0;
    if (xStatusIndicator.is())
            xStatusIndicator->setValue(nSteps++);
    if (!bFlat) //Storage (Package) of Stream
    {
        uno::Reference < embed::XStorage > xStg = rMedium.GetOutputStorage();
        sal_Bool bOASIS = ( SotStorage::GetVersion( xStg ) > SOFFICE_FILEFORMAT_60 );

        // TODO/LATER: handle the case of embedded links gracefully
        if( bEmbedded ) //&& !pStg->IsRoot() )
        {
            OUString aName;
            if ( rMedium.GetItemSet() )
            {
                const SfxStringItem* pDocHierarchItem = static_cast<const SfxStringItem*>(
                    rMedium.GetItemSet()->GetItem(SID_DOC_HIERARCHICALNAME) );
                if ( pDocHierarchItem )
                    aName = pDocHierarchItem->GetValue();
            }

            if( aName.getLength() )
            {
                sPropName = OUString(RTL_CONSTASCII_USTRINGPARAM("StreamRelPath"));
                xInfoSet->setPropertyValue( sPropName, makeAny( aName ) );
            }
        }

        if( !bEmbedded )
        {
            if (xStatusIndicator.is())
                xStatusIndicator->setValue(nSteps++);

            bRet = WriteThroughComponent(
                    xStg, xModelComp, "meta.xml", xServiceFactory, xInfoSet,
                    (bOASIS ? "com.sun.star.comp.Math.XMLOasisMetaExporter"
                             : "com.sun.star.comp.Math.XMLMetaExporter"),
                    sal_False);
        }
        if( bRet )
        {
           if (xStatusIndicator.is())
                xStatusIndicator->setValue(nSteps++);

            bRet = WriteThroughComponent(
                    xStg, xModelComp, "content.xml", xServiceFactory, xInfoSet,
                    "com.sun.star.comp.Math.XMLContentExporter");
        }

        if( bRet )
        {
            if (xStatusIndicator.is())
                xStatusIndicator->setValue(nSteps++);

            bRet = WriteThroughComponent(
                    xStg, xModelComp, "settings.xml", xServiceFactory, xInfoSet,
                    (bOASIS ? "com.sun.star.comp.Math.XMLOasisSettingsExporter"
                            : "com.sun.star.comp.Math.XMLSettingsExporter") );
        }
    }
    else
    {
        SvStream *pStream = rMedium.GetOutStream();
        uno::Reference<io::XOutputStream> xOut(
            new utl::OOutputStreamWrapper(*pStream) );

        if (xStatusIndicator.is())
            xStatusIndicator->setValue(nSteps++);

        bRet = WriteThroughComponent(
            xOut, xModelComp, xServiceFactory, xInfoSet,
            "com.sun.star.comp.Math.XMLContentExporter");
    }

    if (xStatusIndicator.is())
        xStatusIndicator->end();

    return bRet;
}

sal_uInt32 SmXMLExport::exportDoc(enum XMLTokenEnum eClass)
{
    if( (getExportFlags() & EXPORT_CONTENT) == 0 )
    {
        SvXMLExport::exportDoc( eClass );
    }
    else
    {
        uno::Reference <frame::XModel> xModel = GetModel();
        uno::Reference <lang::XUnoTunnel> xTunnel;
        xTunnel = uno::Reference <lang::XUnoTunnel> (xModel,uno::UNO_QUERY);
        SmModel *pModel = reinterpret_cast<SmModel *>
            (xTunnel->getSomething(SmModel::getUnoTunnelId()));

        if (pModel)
        {
            SmDocShell *pDocShell =
                static_cast<SmDocShell*>(pModel->GetObjectShell());
            pTree = pDocShell->GetFormulaTree();
            aText = pDocShell->GetText();
        }

        GetDocHandler()->startDocument();

        if( (getExportFlags() & EXPORT_NODOCTYPE) == 0 &&
            GetExtDocHandler().is() )
        {
            OUString aDocType( RTL_CONSTASCII_USTRINGPARAM( "<!DOCTYPE math:math PUBLIC \"-//OpenOffice.org//DTD Modified W3C MathML 1.01//EN\" \"math.dtd\">" ) );
            GetExtDocHandler()->unknown( aDocType );
        }

        /*Add xmlns line*/
        SvXMLAttributeList &rList = GetAttrList();
        rList.AddAttribute(GetNamespaceMap().GetAttrNameByKey(
            XML_NAMESPACE_MATH_IDX),GetNamespaceMap().GetNameByKey(
            XML_NAMESPACE_MATH_IDX));

        //I think we need something like ImplExportEntities();
        _ExportContent();
        GetDocHandler()->endDocument();
    }

    bSuccess=sal_True;
    return 0;
}

class SmXMLImportContext: public SvXMLImportContext
{
public:
    SmXMLImportContext( SmXMLImport &rImport, sal_uInt16 nPrfx,
        const OUString& rLName)
        : SvXMLImportContext(rImport, nPrfx, rLName) {}
    const SmXMLImport& GetSmImport() const
    {
        return (const SmXMLImport&)GetImport();
    }
    SmXMLImport& GetSmImport()
    {
        return (SmXMLImport&)GetImport();
    }
    virtual void TCharacters(const OUString &rChars) {}
    virtual void Characters(const OUString &rChars)
    {
        /*
        Whitespace occurring within the content of token elements is "trimmed"
        from the ends (i.e. all whitespace at the beginning and end of the
        content is removed), and "collapsed" internally (i.e. each sequence of
        1 or more whitespace characters is replaced with one blank character).
        */
        //collapsing not done yet!
        const OUString &rChars2 = rChars.trim();
        if (rChars2.getLength())
            TCharacters(rChars2/*.collapse()*/);
    }
    virtual SvXMLImportContext *CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList) {return 0;}
};

class SmXMLDocContext_Impl : public SmXMLImportContext
{
public:
    SmXMLDocContext_Impl( SmXMLImport &rImport, sal_uInt16 nPrfx,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrfx,rLName) {}
    virtual SvXMLImportContext *CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList);
    void EndElement();
};


/*avert thy gaze from the proginator*/
class SmXMLRowContext_Impl : public SmXMLDocContext_Impl
{
public:
    SmXMLRowContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLDocContext_Impl(rImport,nPrefix,rLName)
        { nElementCount = GetSmImport().GetNodeStack().Count(); }
    virtual SvXMLImportContext *CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList);
    SvXMLImportContext *StrictCreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList);
    void EndElement();
protected:
    ULONG nElementCount;
};

class SmXMLFracContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLFracContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement();
};

class SmXMLSqrtContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLSqrtContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement();
};

class SmXMLRootContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLRootContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement();
};

struct SmXMLContext_Helper
{
    SmXMLContext_Helper(SmXMLImportContext &rImport) : rContext(rImport),
    nIsBold(-1), nIsItalic(-1),nFontSize(0.0) {}
    void RetrieveAttrs(const uno::Reference< xml::sax::XAttributeList > &
        xAttrList );
    void ApplyAttrs();

    sal_Int8 nIsBold;
    sal_Int8 nIsItalic;
    double nFontSize;
    sal_Bool bFontNodeNeeded;
    OUString sFontFamily;
    OUString sColor;

    SmXMLImportContext rContext;
};

void SmXMLContext_Helper::RetrieveAttrs(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList )
{
    sal_Int8 nOldIsBold=nIsBold;
    sal_Int8 nOldIsItalic=nIsItalic;
    double nOldFontSize=nFontSize;
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    OUString sOldFontFamily = sFontFamily;
    for (sal_Int16 i=0;i<nAttrCount;i++)
    {
        OUString sAttrName = xAttrList->getNameByIndex(i);
        OUString aLocalName;
        sal_uInt16 nPrefix = rContext.GetSmImport().GetNamespaceMap().
            GetKeyByAttrName(sAttrName,&aLocalName);
        OUString sValue = xAttrList->getValueByIndex(i);
        const SvXMLTokenMap &rAttrTokenMap =
            rContext.GetSmImport().GetPresLayoutAttrTokenMap();
        switch(rAttrTokenMap.Get(nPrefix,aLocalName))
        {
            case XML_TOK_FONTWEIGHT:
                nIsBold = sValue.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    sXML_bold)));
                break;
            case XML_TOK_FONTSTYLE:
                nIsItalic = sValue.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    sXML_italic)));
                break;
            case XML_TOK_FONTSIZE:
                SvXMLUnitConverter::convertDouble(nFontSize,sValue);
                rContext.GetSmImport().GetMM100UnitConverter().
                    setXMLMeasureUnit(MAP_POINT);
                if (-1 == sValue.indexOf(OUString(
                        RTL_CONSTASCII_USTRINGPARAM(sXML_unit_pt))))
                    if (-1 == sValue.indexOf('%'))
                        nFontSize=0.0;
                    else
                    {
                        rContext.GetSmImport().GetMM100UnitConverter().
                            setXMLMeasureUnit(MAP_RELATIVE);
                    }
                break;
            case XML_TOK_FONTFAMILY:
                sFontFamily = sValue;
                break;
            case XML_TOK_COLOR:
                sColor = sValue;
                break;
            default:
                break;
        }
    }

    if ((nOldIsBold!=nIsBold) || (nOldIsItalic!=nIsItalic) ||
        (nOldFontSize!=nFontSize) || (sOldFontFamily!=sFontFamily)
        || sColor.getLength())
        bFontNodeNeeded=sal_True;
    else
        bFontNodeNeeded=sal_False;
}

void SmXMLContext_Helper::ApplyAttrs()
{
    SmNodeStack &rNodeStack = rContext.GetSmImport().GetNodeStack();

    if (bFontNodeNeeded)
    {
        SmToken aToken;
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 5;

        if (nIsBold != -1)
        {
            if (nIsBold)
                aToken.eType = TBOLD;
            else
                aToken.eType = TNBOLD;
            SmStructureNode *pFontNode = static_cast<SmStructureNode *>
                (new SmFontNode(aToken));
            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (nIsItalic != -1)
        {
            if (nIsItalic)
                aToken.eType = TITALIC;
            else
                aToken.eType = TNITALIC;
            SmStructureNode *pFontNode = static_cast<SmStructureNode *>
                (new SmFontNode(aToken));
            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (nFontSize != 0.0)
        {
            aToken.eType = TSIZE;
            SmFontNode *pFontNode = new SmFontNode(aToken);

            if (MAP_RELATIVE == rContext.GetSmImport().GetMM100UnitConverter().
                getXMLMeasureUnit())
            {
                if (nFontSize < 100.00)
                    pFontNode->SetSizeParameter(Fraction(100.00/nFontSize),
                        FNTSIZ_DIVIDE);
                else
                    pFontNode->SetSizeParameter(Fraction(nFontSize/100.00),
                        FNTSIZ_MULTIPLY);
            }
            else
                pFontNode->SetSizeParameter(Fraction(nFontSize),FNTSIZ_ABSOLUT);

            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (sFontFamily.getLength())
        {
            if (sFontFamily.equalsIgnoreAsciiCase(OUString(
                RTL_CONSTASCII_USTRINGPARAM(sXML_fixed))))
                aToken.eType = TFIXED;
            else if (sFontFamily.equalsIgnoreAsciiCase(OUString(
                RTL_CONSTASCII_USTRINGPARAM("sans"))))
                aToken.eType = TSANS;
            else if (sFontFamily.equalsIgnoreAsciiCase(OUString(
                RTL_CONSTASCII_USTRINGPARAM("serif"))))
                aToken.eType = TSERIF;
            else //Just give up, we need to extend our font mechanism to be
                //more general
                return;

            aToken.aText = sFontFamily;
            SmFontNode *pFontNode = new SmFontNode(aToken);
            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (sColor.getLength())
        {
            //Again we can only handle a small set of colours in
            //StarMath for now.
            const SvXMLTokenMap& rTokenMap =
                rContext.GetSmImport().GetColorTokenMap();
            aToken.eType = static_cast<SmTokenType>(rTokenMap.Get(
                XML_NAMESPACE_MATH, sColor));
            if (aToken.eType != -1)
            {
                SmFontNode *pFontNode = new SmFontNode(aToken);
                pFontNode->SetSubNodes(0,rNodeStack.Pop());
                rNodeStack.Push(pFontNode);
            }
        }

    }
}

class SmXMLStyleContext_Impl : public SmXMLRowContext_Impl
{
public:
    /*Right now the style tag is completely ignored*/
    SmXMLStyleContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName) : SmXMLRowContext_Impl(rImport,nPrefix,rLName),
        aStyleHelper(*this) {}
    void EndElement();
    void StartElement(const uno::Reference< xml::sax::XAttributeList > &
        xAttrList );
protected:
    SmXMLContext_Helper aStyleHelper;
};

void SmXMLStyleContext_Impl::StartElement(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList )
{
#if 1
    aStyleHelper.RetrieveAttrs(xAttrList);
#else
    sal_Int8 nOldIsBold=nIsBold;
    sal_Int8 nOldIsItalic=nIsItalic;
    double nOldFontSize=nFontSize;
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    OUString sOldFontFamily = sFontFamily;
    for (sal_Int16 i=0;i<nAttrCount;i++)
    {
        OUString sAttrName = xAttrList->getNameByIndex(i);
        OUString aLocalName;
        sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
            GetKeyByAttrName(sAttrName,&aLocalName);
        OUString sValue = xAttrList->getValueByIndex(i);
        const SvXMLTokenMap &rAttrTokenMap =
            GetSmImport().GetPresLayoutAttrTokenMap();
        switch(rAttrTokenMap.Get(nPrefix,aLocalName))
        {
            case XML_TOK_FONTWEIGHT:
                nIsBold = sValue.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    sXML_bold)));
                break;
            case XML_TOK_FONTSTYLE:
                nIsItalic = sValue.equals(OUString(RTL_CONSTASCII_USTRINGPARAM(
                    sXML_italic)));
                break;
            case XML_TOK_FONTSIZE:
                SvXMLUnitConverter::convertDouble(nFontSize,sValue);
                GetSmImport().GetMM100UnitConverter().
                    setXMLMeasureUnit(MAP_POINT);
                if (-1 == sValue.indexOf(OUString(
                        RTL_CONSTASCII_USTRINGPARAM(sXML_unit_pt))))
                    if (-1 == sValue.indexOf('%'))
                        nFontSize=0.0;
                    else
                    {
                        GetSmImport().GetMM100UnitConverter().
                            setXMLMeasureUnit(MAP_RELATIVE);
                    }
                break;
            case XML_TOK_FONTFAMILY:
                sFontFamily = sValue;
                break;
            case XML_TOK_COLOR:
                sColor = sValue;
                break;
            default:
                break;
        }
    }

    if ((nOldIsBold!=nIsBold) || (nOldIsItalic!=nIsItalic) ||
        (nOldFontSize!=nFontSize) || (sOldFontFamily!=sFontFamily)
        || sColor.getLength())
        bFontNodeNeeded=sal_True;
    else
        bFontNodeNeeded=sal_False;
#endif
}


void SmXMLStyleContext_Impl::EndElement()
{
    /*
    <mstyle> accepts any number of arguments; if this number is not 1, its
    contents are treated as a single "inferred <mrow>" containing its
    arguments
    */
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    if (rNodeStack.Count() - nElementCount > 1)
        SmXMLRowContext_Impl::EndElement();
#if 1
    aStyleHelper.ApplyAttrs();
#else
    if (bFontNodeNeeded)
    {
        SmToken aToken;
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 5;

        if (nIsBold != -1)
        {
            if (nIsBold)
                aToken.eType = TBOLD;
            else
                aToken.eType = TNBOLD;
            SmStructureNode *pFontNode = static_cast<SmStructureNode *>
                (new SmFontNode(aToken));
            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (nIsItalic != -1)
        {
            if (nIsItalic)
                aToken.eType = TITALIC;
            else
                aToken.eType = TNITALIC;
            SmStructureNode *pFontNode = static_cast<SmStructureNode *>
                (new SmFontNode(aToken));
            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (nFontSize != 0.0)
        {
            aToken.eType = TSIZE;
            SmFontNode *pFontNode = new SmFontNode(aToken);

            if (MAP_RELATIVE == GetSmImport().GetMM100UnitConverter().
                getXMLMeasureUnit())
            {
                if (nFontSize < 100.00)
                    pFontNode->SetSizeParameter(Fraction(100.00/nFontSize),
                        FNTSIZ_DIVIDE);
                else
                    pFontNode->SetSizeParameter(Fraction(nFontSize/100.00),
                        FNTSIZ_MULTIPLY);
            }
            else
                pFontNode->SetSizeParameter(Fraction(nFontSize),FNTSIZ_ABSOLUT);

            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (sFontFamily.getLength())
        {
            if (sFontFamily.equalsIgnoreCase(OUString(
                RTL_CONSTASCII_USTRINGPARAM(sXML_fixed))))
                aToken.eType = TFIXED;
            else if (sFontFamily.equalsIgnoreCase(OUString(
                RTL_CONSTASCII_USTRINGPARAM("sans"))))
                aToken.eType = TSANS;
            else if (sFontFamily.equalsIgnoreCase(OUString(
                RTL_CONSTASCII_USTRINGPARAM("serif"))))
                aToken.eType = TSERIF;
            else //Just give up, we need to extend our font mechanism to be
                //more general
                return;

            aToken.aText = sFontFamily;
            SmFontNode *pFontNode = new SmFontNode(aToken);
            pFontNode->SetSubNodes(0,rNodeStack.Pop());
            rNodeStack.Push(pFontNode);
        }
        if (sColor.getLength())
        {
            //Again we can only handle a small set of colours in
            //StarMath for now.
            const SvXMLTokenMap& rTokenMap =
                GetSmImport().GetColorTokenMap();
            aToken.eType = static_cast<SmTokenType>(rTokenMap.Get(
                XML_NAMESPACE_MATH, sColor));
            if (aToken.eType != -1)
            {
                SmFontNode *pFontNode = new SmFontNode(aToken);
                pFontNode->SetSubNodes(0,rNodeStack.Pop());
                rNodeStack.Push(pFontNode);
            }
        }

    }
#endif
}

class SmXMLPaddedContext_Impl : public SmXMLRowContext_Impl
{
public:
    /*Right now the style tag is completely ignored*/
    SmXMLPaddedContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement();
};

void SmXMLPaddedContext_Impl::EndElement()
{
    /*
    <mpadded> accepts any number of arguments; if this number is not 1, its
    contents are treated as a single "inferred <mrow>" containing its
    arguments
    */
    if (GetSmImport().GetNodeStack().Count() - nElementCount > 1)
        SmXMLRowContext_Impl::EndElement();
}

class SmXMLPhantomContext_Impl : public SmXMLRowContext_Impl
{
public:
    /*Right now the style tag is completely ignored*/
    SmXMLPhantomContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement();
};

void SmXMLPhantomContext_Impl::EndElement()
{
    /*
    <mphantom> accepts any number of arguments; if this number is not 1, its
    contents are treated as a single "inferred <mrow>" containing its
    arguments
    */
    if (GetSmImport().GetNodeStack().Count() - nElementCount > 1)
        SmXMLRowContext_Impl::EndElement();

    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.nLevel = 5;
    aToken.eType = TPHANTOM;

    SmStructureNode *pPhantom = static_cast<SmStructureNode *>
        (new SmFontNode(aToken));
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    pPhantom->SetSubNodes(0,rNodeStack.Pop());
    rNodeStack.Push(pPhantom);
}

class SmXMLFencedContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLFencedContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName),
        cBegin('('), cEnd(')') {}
    void StartElement(const uno::Reference<
        xml::sax::XAttributeList > & xAttrList );
    void EndElement();
protected:
    sal_Unicode cBegin;
    sal_Unicode cEnd;
};


void SmXMLFencedContext_Impl::StartElement(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList )
{
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for (sal_Int16 i=0;i<nAttrCount;i++)
    {
        OUString sAttrName = xAttrList->getNameByIndex(i);
        OUString aLocalName;
        sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
            GetKeyByAttrName(sAttrName,&aLocalName);
        OUString sValue = xAttrList->getValueByIndex(i);
        const SvXMLTokenMap &rAttrTokenMap =
            GetSmImport().GetFencedAttrTokenMap();
        switch(rAttrTokenMap.Get(nPrefix,aLocalName))
        {
            //temp, starmath cannot handle multichar brackets (I think)
            case XML_TOK_OPEN:
                cBegin = sValue[0];
                break;
            case XML_TOK_CLOSE:
                cEnd = sValue[0];
                break;
            default:
                /*Go to superclass*/
                break;
        }
    }
}


void SmXMLFencedContext_Impl::EndElement()
{
    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.aText = ',';
    aToken.eType = TLEFT;
    aToken.nLevel = 5;

    aToken.eType = TLPARENT;
    aToken.cMathChar = cBegin;
    SmStructureNode *pSNode = new SmBraceNode(aToken);
    SmNode *pLeft = new SmMathSymbolNode(aToken);

    aToken.cMathChar = cEnd;
    aToken.eType = TRPARENT;
    SmNode *pRight = new SmMathSymbolNode(aToken);

    SmNodeArray aRelationArray;
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();

    aToken.cMathChar = '\0';
    aToken.aText = ',';
    aToken.eType = TIDENT;

    ULONG i=rNodeStack.Count()-nElementCount;
    if (rNodeStack.Count()-nElementCount > 1)
        i+=rNodeStack.Count()-1-nElementCount;
    aRelationArray.SetSize(i);
    while(rNodeStack.Count() > nElementCount)
    {
        aRelationArray.Put(--i,rNodeStack.Pop());
        if (rNodeStack.Count() > 1)
            aRelationArray.Put(--i,new SmGlyphSpecialNode(aToken));
    }

    SmToken aDummy;
    SmStructureNode *pBody = new SmExpressionNode(aDummy);
    pBody->SetSubNodes(aRelationArray);


    pSNode->SetSubNodes(pLeft,pBody,pRight);
    pSNode->SetScaleMode(SCALE_HEIGHT);
    GetSmImport().GetNodeStack().Push(pSNode);
}


class SmXMLErrorContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLErrorContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement();
};

void SmXMLErrorContext_Impl::EndElement()
{
    /*Right now the error tag is completely ignored, what
     can I do with it in starmath, ?, maybe we need a
     report window ourselves, do a test for validity of
     the xml input, use merrors, and then generate
     the markup inside the merror with a big red colour
     of something. For now just throw them all away.
     */
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    while(rNodeStack.Count() > nElementCount)
    {
        SmNode *pNode = rNodeStack.Pop();
        delete pNode;
    }
}

class SmXMLNumberContext_Impl : public SmXMLImportContext
{
public:
    SmXMLNumberContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName)
    {
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 5;
        aToken.eType = TNUMBER;
    }
    virtual void TCharacters(const OUString &rChars);
    void EndElement();
protected:
    SmToken aToken;
};

void SmXMLNumberContext_Impl::TCharacters(const OUString &rChars)
{
    aToken.aText = rChars;
}

void SmXMLNumberContext_Impl::EndElement()
{
    GetSmImport().GetNodeStack().Push(new SmTextNode(aToken,FNT_NUMBER));
}

class SmXMLAnnotationContext_Impl : public SmXMLImportContext
{
public:
    SmXMLAnnotationContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName), bIsStarMath(sal_False) {}
    virtual void Characters(const OUString &rChars);
    void StartElement(const uno::Reference<xml::sax::XAttributeList > &
        xAttrList );
private:
    sal_Bool bIsStarMath;
};

void SmXMLAnnotationContext_Impl::StartElement(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList )
{
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for (sal_Int16 i=0;i<nAttrCount;i++)
    {
        OUString sAttrName = xAttrList->getNameByIndex(i);
        OUString aLocalName;
        sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
            GetKeyByAttrName(sAttrName,&aLocalName);

        OUString sValue = xAttrList->getValueByIndex(i);
        const SvXMLTokenMap &rAttrTokenMap =
            GetSmImport().GetAnnotationAttrTokenMap();
        switch(rAttrTokenMap.Get(nPrefix,aLocalName))
        {
            case XML_TOK_ENCODING:
                bIsStarMath= sValue.equals(
                    OUString(RTL_CONSTASCII_USTRINGPARAM("StarMath 5.0")));
                break;
            default:
                break;
        }
    }
}

void SmXMLAnnotationContext_Impl::Characters(const OUString &rChars)
{
    if (bIsStarMath)
        GetSmImport().GetText().Append(String(rChars));
}

class SmXMLTextContext_Impl : public SmXMLImportContext
{
public:
    SmXMLTextContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName)
    {
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 5;
        aToken.eType = TTEXT;
    }
    virtual void TCharacters(const OUString &rChars);
    void EndElement();
protected:
    SmToken aToken;
};

void SmXMLTextContext_Impl::TCharacters(const OUString &rChars)
{
    aToken.aText = rChars;
}

void SmXMLTextContext_Impl::EndElement()
{
    GetSmImport().GetNodeStack().Push(new SmTextNode(aToken,FNT_TEXT));
}

class SmXMLStringContext_Impl : public SmXMLImportContext
{
public:
    SmXMLStringContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName)
    {
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 5;
        aToken.eType = TTEXT;
    }
    virtual void TCharacters(const OUString &rChars);
    void EndElement();
protected:
    SmToken aToken;
};

void SmXMLStringContext_Impl::TCharacters(const OUString &rChars)
{
    /*
    The content of <ms> elements should be rendered with visible "escaping" of
    certain characters in the content, including at least "double quote"
    itself, and preferably whitespace other than individual blanks. The intent
    is for the viewer to see that the expression is a string literal, and to
    see exactly which characters form its content. For example, <ms>double
    quote is "</ms> might be rendered as "double quote is \"".

    Obviously this isn't fully done here.
    */
    aToken.aText.Erase();
    aToken.aText += '\"';
    aToken.aText += String(rChars);
    aToken.aText += '\"';
}

void SmXMLStringContext_Impl::EndElement()
{
    GetSmImport().GetNodeStack().Push(new SmTextNode(aToken,FNT_FIXED));
}

class SmXMLIdentifierContext_Impl : public SmXMLImportContext
{
public:
    SmXMLIdentifierContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName),aStyleHelper(*this)
    {
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 5;
        aToken.eType = TIDENT;
    }
    void TCharacters(const OUString &rChars);
    void StartElement(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList ) {aStyleHelper.RetrieveAttrs(xAttrList);};
    void EndElement();
protected:
    SmXMLContext_Helper aStyleHelper;
    SmToken aToken;
};

void SmXMLIdentifierContext_Impl::EndElement()
{
    SmTextNode *pNode = 0;
    //we will handle identifier italic/normal here instead of with a standalone
    //font node
    if (((aStyleHelper.nIsItalic == -1) && (aToken.aText.Len() > 1))
        || ((aStyleHelper.nIsItalic == 0) && (aToken.aText.Len() == 1)))
    {
        pNode = new SmTextNode(aToken,FNT_FUNCTION);
        pNode->GetFont().SetItalic(ITALIC_NONE);
        aStyleHelper.nIsItalic = -1;
    }
    else
        pNode = new SmTextNode(aToken,FNT_VARIABLE);
    if (aStyleHelper.bFontNodeNeeded && aStyleHelper.nIsItalic != -1)
    {
        if (aStyleHelper.nIsItalic)
            pNode->GetFont().SetItalic(ITALIC_NORMAL);
        else
            pNode->GetFont().SetItalic(ITALIC_NONE);
    }

    if ((-1!=aStyleHelper.nIsBold) || (0.0!=aStyleHelper.nFontSize) ||
        (aStyleHelper.sFontFamily.getLength()) ||
        aStyleHelper.sColor.getLength())
        aStyleHelper.bFontNodeNeeded=sal_True;
    else
        aStyleHelper.bFontNodeNeeded=sal_False;
    if (aStyleHelper.bFontNodeNeeded)
        aStyleHelper.ApplyAttrs();
    GetSmImport().GetNodeStack().Push(pNode);
}

void SmXMLIdentifierContext_Impl::TCharacters(const OUString &rChars)
{
    aToken.aText = rChars;
}

class SmXMLOperatorContext_Impl : public SmXMLImportContext
{
public:
    SmXMLOperatorContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName), bIsStretchy(sal_False)
    {
        aToken.nGroup = 0;
        aToken.eType = TSPECIAL;
        aToken.nLevel = 5;
    }
    void TCharacters(const OUString &rChars);
    void StartElement(const uno::Reference<
        xml::sax::XAttributeList > &xAttrList );
    void EndElement();
protected:
    SmToken aToken;
private:
    sal_Bool bIsStretchy;
};

void SmXMLOperatorContext_Impl::TCharacters(const OUString &rChars)
{
    aToken.cMathChar = rChars[0];
}

void SmXMLOperatorContext_Impl::EndElement()
{
    SmMathSymbolNode *pNode = new SmMathSymbolNode(aToken);
    //For stretchy scaling the scaling must be retrieved from this node
    //and applied to the expression itself so as to get the expression
    //to scale the operator to the height of the expression itself
    if (bIsStretchy)
        pNode->SetScaleMode(SCALE_HEIGHT);
    GetSmImport().GetNodeStack().Push(pNode);
}



void SmXMLOperatorContext_Impl::StartElement(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList )
{
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for (sal_Int16 i=0;i<nAttrCount;i++)
    {
        OUString sAttrName = xAttrList->getNameByIndex(i);
        OUString aLocalName;
        sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
            GetKeyByAttrName(sAttrName,&aLocalName);

        OUString sValue = xAttrList->getValueByIndex(i);
        const SvXMLTokenMap &rAttrTokenMap =
            GetSmImport().GetOperatorAttrTokenMap();
        switch(rAttrTokenMap.Get(nPrefix,aLocalName))
        {
            case XML_TOK_STRETCHY:
                bIsStretchy = sValue.equals(
                    OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
                break;
            default:
                break;
        }
    }
}


class SmXMLSpaceContext_Impl : public SmXMLImportContext
{
public:
    SmXMLSpaceContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName) {}
    void StartElement(const uno::Reference<
        xml::sax::XAttributeList >& xAttrList );
};

void SmXMLSpaceContext_Impl::StartElement(
    const uno::Reference<xml::sax::XAttributeList > &xAttrList )
{
    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.eType = TBLANK;
    aToken.nLevel = 5;
    SmBlankNode *pBlank = new SmBlankNode(aToken);
    pBlank->IncreaseBy(aToken);
    GetSmImport().GetNodeStack().Push(pBlank);
}

class SmXMLSubContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLSubContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement() { GenericEndElement(TRSUB,RSUB); }
protected:
    void GenericEndElement(SmTokenType eType,SmSubSup aSubSup);
};


void SmXMLSubContext_Impl::GenericEndElement(SmTokenType eType,SmSubSup aSubSup)
{
    /*The <msub> element requires exactly 2 arguments.*/
    DBG_ASSERT(GetSmImport().GetNodeStack().Count() - nElementCount == 2,
        "Sub has not two arguments");
    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.nLevel = 0;
    aToken.eType = eType;
    SmSubSupNode *pNode = new SmSubSupNode(aToken);
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();

    // initialize subnodes array
    SmNodeArray  aSubNodes;
    aSubNodes.SetSize(1 + SUBSUP_NUM_ENTRIES);
    for (ULONG i = 1;  i < aSubNodes.GetSize();  i++)
        aSubNodes.Put(i, NULL);

    aSubNodes.Put(aSubSup+1,rNodeStack.Pop());
    aSubNodes.Put(0, rNodeStack.Pop());
    pNode->SetSubNodes(aSubNodes);
    rNodeStack.Push(pNode);
}

class SmXMLSupContext_Impl : public SmXMLSubContext_Impl
{
public:
    SmXMLSupContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLSubContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement() {GenericEndElement(TRSUP,RSUP);}
};

class SmXMLSubSupContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLSubSupContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLRowContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement() { GenericEndElement(TRSUB,RSUB,RSUP); }
protected:
    void GenericEndElement(SmTokenType eType,
        SmSubSup aSub,SmSubSup aSup);

};

void SmXMLSubSupContext_Impl::GenericEndElement(SmTokenType eType,
        SmSubSup aSub,SmSubSup aSup)
{
    /*The <msub> element requires exactly 3 arguments.*/
    DBG_ASSERT(GetSmImport().GetNodeStack().Count() - nElementCount == 3,
        "SubSup has not three arguments");

    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.nLevel = 0;
    aToken.eType = eType;
    SmSubSupNode *pNode = new SmSubSupNode(aToken);
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();

    // initialize subnodes array
    SmNodeArray  aSubNodes;
    aSubNodes.SetSize(1 + SUBSUP_NUM_ENTRIES);
    for (ULONG i = 1;  i < aSubNodes.GetSize();  i++)
        aSubNodes.Put(i, NULL);

    aSubNodes.Put(aSup+1,rNodeStack.Pop());
    aSubNodes.Put(aSub+1,rNodeStack.Pop());
    aSubNodes.Put(0, rNodeStack.Pop());
    pNode->SetSubNodes(aSubNodes);
    rNodeStack.Push(pNode);
}

class SmXMLUnderContext_Impl : public SmXMLSubContext_Impl
{
public:
    SmXMLUnderContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLSubContext_Impl(rImport,nPrefix,rLName) {}
    void StartElement(const uno::Reference< xml::sax::XAttributeList > &
        xAttrList );
    void EndElement();
    void HandleAccent();
protected:
    sal_Int16 nAttrCount;
};

void SmXMLUnderContext_Impl::StartElement(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList )
{
    nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
}

void SmXMLUnderContext_Impl::HandleAccent()
{
    DBG_ASSERT(GetSmImport().GetNodeStack().Count() - nElementCount == 2,
        "Sub has not two arguments");

    /*Just one special case for the underline thing*/
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    SmNode *pTest = rNodeStack.Pop();
    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.nLevel = 0;
    aToken.eType = TUNDERLINE;


    SmNodeArray aSubNodes;
    aSubNodes.SetSize(2);

    SmStructureNode *pNode = new SmAttributNode(aToken);
    if ((pTest->GetToken().cMathChar & 0x0FFF) == 0x0332)
    {
        aSubNodes.Put(0, new SmRectangleNode(aToken));
        delete pTest;
    }
    else
        aSubNodes.Put(0, pTest);

    aSubNodes.Put(1, rNodeStack.Pop());
    pNode->SetSubNodes(aSubNodes);
    pNode->SetScaleMode(SCALE_WIDTH);
    rNodeStack.Push(pNode);
}


void SmXMLUnderContext_Impl::EndElement()
{
    if (!nAttrCount)
        GenericEndElement(TCSUB,CSUB);
    else
        HandleAccent();
#if 0
    //UnderBrace trick
    SmStructureNode *pNode = rNodeStack.Pop();
    if (pNode->GetSubNode(1)->GetToken().cMathChar == (0x0332|0xf000))
    if (pNode->GetSubNode(0)->GetToken().cMathChar == (0x0332|0xf000))
#endif
}

class SmXMLOverContext_Impl : public SmXMLSubContext_Impl
{
public:
    SmXMLOverContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLSubContext_Impl(rImport,nPrefix,rLName), nAttrCount(0) {}
    void EndElement();
    void StartElement(const uno::Reference< xml::sax::XAttributeList > &
        xAttrList );
    void HandleAccent();
protected:
    sal_Int16 nAttrCount;
};


void SmXMLOverContext_Impl::StartElement(const uno::Reference<
    xml::sax::XAttributeList > & xAttrList )
{
    nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
}


void SmXMLOverContext_Impl::EndElement()
{
    if (!nAttrCount)
        GenericEndElement(TCSUP,CSUP);
    else
        HandleAccent();
}


void SmXMLOverContext_Impl::HandleAccent()
{
    DBG_ASSERT(GetSmImport().GetNodeStack().Count() - nElementCount == 2,
        "Sub has not two arguments");
    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.nLevel = 0;
    aToken.eType = TACUTE;

    SmAttributNode *pNode = new SmAttributNode(aToken);
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();

    SmNodeArray aSubNodes;
    aSubNodes.SetSize(2);
    aSubNodes.Put(0, rNodeStack.Pop());
    aSubNodes.Put(1, rNodeStack.Pop());
    pNode->SetSubNodes(aSubNodes);
    pNode->SetScaleMode(SCALE_WIDTH);
    rNodeStack.Push(pNode);

}

class SmXMLUnderOverContext_Impl : public SmXMLSubSupContext_Impl
{
public:
    SmXMLUnderOverContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLSubSupContext_Impl(rImport,nPrefix,rLName) {}
    void EndElement() { GenericEndElement(TCSUB,CSUB,CSUP); }
};

class SmXMLMultiScriptsContext_Impl : public SmXMLSubSupContext_Impl
{
public:
    SmXMLMultiScriptsContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName) :
        SmXMLSubSupContext_Impl(rImport,nPrefix,rLName),
        bHasPrescripts(FALSE) {}
    void EndElement();
    void MiddleElement();
    SvXMLImportContext *CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList);
private:
    sal_Bool bHasPrescripts;
};

class SmXMLNoneContext_Impl : public SmXMLImportContext
{
public:
    SmXMLNoneContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName) {}
    void EndElement();
};


void SmXMLNoneContext_Impl::EndElement(void)
{
    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.aText.Erase();
    aToken.nLevel = 5;
    aToken.eType = TIDENT;
    GetSmImport().GetNodeStack().Push(
        new SmTextNode(aToken,FNT_VARIABLE));
}

class SmXMLPrescriptsContext_Impl : public SmXMLImportContext
{
public:
    SmXMLPrescriptsContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName)
        : SmXMLImportContext(rImport,nPrefix,rLName) {}
};

class SmXMLTableRowContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLTableRowContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName) :
        SmXMLRowContext_Impl(rImport,nPrefix,rLName)
        {}
    SvXMLImportContext *CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList);
};


class SmXMLTableContext_Impl : public SmXMLTableRowContext_Impl
{
public:
    SmXMLTableContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName) :
        SmXMLTableRowContext_Impl(rImport,nPrefix,rLName)
        {}
    void EndElement();
    SvXMLImportContext *CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList);
};


class SmXMLTableCellContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLTableCellContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName) :
        SmXMLRowContext_Impl(rImport,nPrefix,rLName)
        {}
};

class SmXMLAlignGroupContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLAlignGroupContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName) :
        SmXMLRowContext_Impl(rImport,nPrefix,rLName)
        {}
    /*Don't do anything with alignment for now*/
    void EndElement() {}
};

class SmXMLActionContext_Impl : public SmXMLRowContext_Impl
{
public:
    SmXMLActionContext_Impl(SmXMLImport &rImport,sal_uInt16 nPrefix,
        const OUString& rLName) :
        SmXMLRowContext_Impl(rImport,nPrefix,rLName)
        {}
    void EndElement();
};

class SmXMLOfficeContext_Impl : public SvXMLImportContext
{
public:
    SmXMLOfficeContext_Impl( SmXMLImport &rImport, sal_uInt16 nPrfx,
        const OUString& rLName)
        : SvXMLImportContext(rImport,nPrfx,rLName) {}
    virtual SvXMLImportContext *CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList);
};

SvXMLImportContext *SmXMLOfficeContext_Impl::CreateChildContext(sal_uInt16 nPrefix,
        const OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > &xAttrList)
{
    SvXMLImportContext *pContext = 0;
    if( XML_NAMESPACE_OFFICE == nPrefix &&
        rLocalName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(sXML_meta) ) )
        pContext = new SfxXMLMetaContext( GetImport(),
                                    XML_NAMESPACE_OFFICE, rLocalName,
                                    GetImport().GetModel() );
    else if( XML_NAMESPACE_OFFICE == nPrefix &&
        rLocalName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(sXML_settings) ) )
        pContext = new XMLDocumentSettingsContext( GetImport(),
                                    XML_NAMESPACE_OFFICE, rLocalName,
                                    xAttrList );
    else
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );

    return pContext;
}

static __FAR_DATA SvXMLTokenMapEntry aPresLayoutElemTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_SEMANTICS, XML_TOK_SEMANTICS },
    { XML_NAMESPACE_MATH,   XML_MATH,      XML_TOK_MATH   },
    { XML_NAMESPACE_MATH,   XML_MSTYLE,    XML_TOK_MSTYLE  },
    { XML_NAMESPACE_MATH,   XML_MERROR,    XML_TOK_MERROR },
    { XML_NAMESPACE_MATH,   XML_MPHANTOM,  XML_TOK_MPHANTOM },
    { XML_NAMESPACE_MATH,   XML_MROW,      XML_TOK_MROW },
    { XML_NAMESPACE_MATH,   XML_MFRAC,     XML_TOK_MFRAC },
    { XML_NAMESPACE_MATH,   XML_MSQRT,     XML_TOK_MSQRT },
    { XML_NAMESPACE_MATH,   XML_MROOT,     XML_TOK_MROOT },
    { XML_NAMESPACE_MATH,   XML_MSUB,      XML_TOK_MSUB },
    { XML_NAMESPACE_MATH,   XML_MSUP,      XML_TOK_MSUP },
    { XML_NAMESPACE_MATH,   XML_MSUBSUP,   XML_TOK_MSUBSUP },
    { XML_NAMESPACE_MATH,   XML_MUNDER,    XML_TOK_MUNDER },
    { XML_NAMESPACE_MATH,   XML_MOVER,     XML_TOK_MOVER },
    { XML_NAMESPACE_MATH,   XML_MUNDEROVER,    XML_TOK_MUNDEROVER },
    { XML_NAMESPACE_MATH,   XML_MMULTISCRIPTS, XML_TOK_MMULTISCRIPTS },
    { XML_NAMESPACE_MATH,   XML_MTABLE,    XML_TOK_MTABLE },
    { XML_NAMESPACE_MATH,   XML_MACTION,   XML_TOK_MACTION },
    { XML_NAMESPACE_MATH,   XML_MFENCED,   XML_TOK_MFENCED },
    { XML_NAMESPACE_MATH,   XML_MPADDED,   XML_TOK_MPADDED },
    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aPresLayoutAttrTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_FONTWEIGHT,      XML_TOK_FONTWEIGHT    },
    { XML_NAMESPACE_MATH,   XML_FONTSTYLE,       XML_TOK_FONTSTYLE     },
    { XML_NAMESPACE_MATH,   XML_FONTSIZE,        XML_TOK_FONTSIZE      },
    { XML_NAMESPACE_MATH,   XML_FONTFAMILY,      XML_TOK_FONTFAMILY    },
    { XML_NAMESPACE_MATH,   XML_COLOR,           XML_TOK_COLOR },
    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aFencedAttrTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_OPEN,       XML_TOK_OPEN },
    { XML_NAMESPACE_MATH,   XML_CLOSE,      XML_TOK_CLOSE },
    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aOperatorAttrTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_STRETCHY,      XML_TOK_STRETCHY },
    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aAnnotationAttrTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_ENCODING,      XML_TOK_ENCODING },
    XML_TOKEN_MAP_END
};


static __FAR_DATA SvXMLTokenMapEntry aPresElemTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_ANNOTATION,    XML_TOK_ANNOTATION },
    { XML_NAMESPACE_MATH,   XML_MI,    XML_TOK_MI },
    { XML_NAMESPACE_MATH,   XML_MN,    XML_TOK_MN },
    { XML_NAMESPACE_MATH,   XML_MO,    XML_TOK_MO },
    { XML_NAMESPACE_MATH,   XML_MTEXT, XML_TOK_MTEXT },
    { XML_NAMESPACE_MATH,   XML_MSPACE,XML_TOK_MSPACE },
    { XML_NAMESPACE_MATH,   XML_MS,    XML_TOK_MS },
    { XML_NAMESPACE_MATH,   XML_MALIGNGROUP,   XML_TOK_MALIGNGROUP },
    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aPresScriptEmptyElemTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_MPRESCRIPTS,   XML_TOK_MPRESCRIPTS },
    { XML_NAMESPACE_MATH,   XML_NONE,  XML_TOK_NONE },
    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aPresTableElemTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_MTR,       XML_TOK_MTR },
    { XML_NAMESPACE_MATH,   XML_MTD,       XML_TOK_MTD },
    XML_TOKEN_MAP_END
};

static __FAR_DATA SvXMLTokenMapEntry aColorTokenMap[] =
{
    { XML_NAMESPACE_MATH,   XML_BLACK,        TBLACK},
    { XML_NAMESPACE_MATH,   XML_WHITE,        TWHITE},
    { XML_NAMESPACE_MATH,   XML_RED,          TRED},
    { XML_NAMESPACE_MATH,   XML_GREEN,        TGREEN},
    { XML_NAMESPACE_MATH,   XML_BLUE,         TBLUE},
    { XML_NAMESPACE_MATH,   XML_AQUA,         TCYAN},
    { XML_NAMESPACE_MATH,   XML_FUCHSIA,      TMAGENTA},
    { XML_NAMESPACE_MATH,   XML_YELLOW,       TYELLOW},
    XML_TOKEN_MAP_END
};


const SvXMLTokenMap& SmXMLImport::GetPresLayoutElemTokenMap()
{
    if(!pPresLayoutElemTokenMap)
        pPresLayoutElemTokenMap = new SvXMLTokenMap(aPresLayoutElemTokenMap);
    return *pPresLayoutElemTokenMap;
}

const SvXMLTokenMap& SmXMLImport::GetPresLayoutAttrTokenMap()
{
    if(!pPresLayoutAttrTokenMap)
        pPresLayoutAttrTokenMap = new SvXMLTokenMap(aPresLayoutAttrTokenMap);
    return *pPresLayoutAttrTokenMap;
}


const SvXMLTokenMap& SmXMLImport::GetFencedAttrTokenMap()
{
    if(!pFencedAttrTokenMap)
        pFencedAttrTokenMap = new SvXMLTokenMap(aFencedAttrTokenMap);
    return *pFencedAttrTokenMap;
}

const SvXMLTokenMap& SmXMLImport::GetOperatorAttrTokenMap()
{
    if(!pOperatorAttrTokenMap)
        pOperatorAttrTokenMap = new SvXMLTokenMap(aOperatorAttrTokenMap);
    return *pOperatorAttrTokenMap;
}

const SvXMLTokenMap& SmXMLImport::GetAnnotationAttrTokenMap()
{
    if(!pAnnotationAttrTokenMap)
        pAnnotationAttrTokenMap = new SvXMLTokenMap(aAnnotationAttrTokenMap);
    return *pAnnotationAttrTokenMap;
}

const SvXMLTokenMap& SmXMLImport::GetPresElemTokenMap()
{
    if(!pPresElemTokenMap)
        pPresElemTokenMap = new SvXMLTokenMap(aPresElemTokenMap);
    return *pPresElemTokenMap;
}

const SvXMLTokenMap& SmXMLImport::GetPresScriptEmptyElemTokenMap()
{
    if(!pPresScriptEmptyElemTokenMap)
        pPresScriptEmptyElemTokenMap = new
            SvXMLTokenMap(aPresScriptEmptyElemTokenMap);
    return *pPresScriptEmptyElemTokenMap;
}

const SvXMLTokenMap& SmXMLImport::GetPresTableElemTokenMap()
{
    if(!pPresTableElemTokenMap)
        pPresTableElemTokenMap = new SvXMLTokenMap(aPresTableElemTokenMap);
    return *pPresTableElemTokenMap;
}

const SvXMLTokenMap& SmXMLImport::GetColorTokenMap()
{
    if(!pColorTokenMap)
        pColorTokenMap = new SvXMLTokenMap(aColorTokenMap);
    return *pColorTokenMap;
}

SvXMLImportContext *SmXMLDocContext_Impl::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const uno::Reference<xml::sax::XAttributeList>& xAttrList)
{
    SvXMLImportContext* pContext = 0L;

    const SvXMLTokenMap& rTokenMap = GetSmImport().GetPresLayoutElemTokenMap();

    UINT32 nTest = rTokenMap.Get(nPrefix, rLocalName);

    switch(rTokenMap.Get(nPrefix, rLocalName))
    {
        //Consider semantics a dummy except for any starmath annotations
        case XML_TOK_SEMANTICS:
            pContext = GetSmImport().CreateRowContext(nPrefix,rLocalName,
                xAttrList);
            break;
        /*General Layout Schemata*/
        case XML_TOK_MROW:
            pContext = GetSmImport().CreateRowContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MFRAC:
            pContext = GetSmImport().CreateFracContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MSQRT:
            pContext = GetSmImport().CreateSqrtContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MROOT:
            pContext = GetSmImport().CreateRootContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MSTYLE:
            pContext = GetSmImport().CreateStyleContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MERROR:
            pContext = GetSmImport().CreateErrorContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MPADDED:
            pContext = GetSmImport().CreatePaddedContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MPHANTOM:
            pContext = GetSmImport().CreatePhantomContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MFENCED:
            pContext = GetSmImport().CreateFencedContext(nPrefix,rLocalName,
                xAttrList);
            break;
        /*Script and Limit Schemata*/
        case XML_TOK_MSUB:
            pContext = GetSmImport().CreateSubContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MSUP:
            pContext = GetSmImport().CreateSupContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MSUBSUP:
            pContext = GetSmImport().CreateSubSupContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MUNDER:
            pContext = GetSmImport().CreateUnderContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MOVER:
            pContext = GetSmImport().CreateOverContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MUNDEROVER:
            pContext = GetSmImport().CreateUnderOverContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MMULTISCRIPTS:
            pContext = GetSmImport().CreateMultiScriptsContext(nPrefix,
                rLocalName, xAttrList);
            break;
        case XML_TOK_MTABLE:
            pContext = GetSmImport().CreateTableContext(nPrefix,
                rLocalName, xAttrList);
            break;
        case XML_TOK_MACTION:
            pContext = GetSmImport().CreateActionContext(nPrefix,
                rLocalName, xAttrList);
            break;
        default:
            /*Basically theres an implicit mrow around certain bare
             *elements, use a RowContext to see if this is one of
             *those ones*/
            SmXMLRowContext_Impl aTempContext(GetSmImport(),nPrefix,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_mrow)));

            pContext = aTempContext.StrictCreateChildContext(nPrefix,
                rLocalName, xAttrList);
            break;
    }
    return pContext;
}

void SmXMLDocContext_Impl::EndElement()
{
    SmNodeArray ContextArray;
    ContextArray.SetSize(1);
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();

    for(ULONG i=0;i< 1;i++)
        ContextArray.Put(i, rNodeStack.Pop());

    SmToken aDummy;
    SmStructureNode *pSNode = new SmLineNode(aDummy);
    pSNode->SetSubNodes(ContextArray);
    rNodeStack.Push(pSNode);

    SmNodeArray  LineArray;
    ULONG n = rNodeStack.Count();
    LineArray.SetSize(n);
    for (ULONG j = 0; j < n; j++)
        LineArray.Put(n - (j + 1), rNodeStack.Pop());
    SmStructureNode *pSNode2 = new SmTableNode(aDummy);
    pSNode2->SetSubNodes(LineArray);
    rNodeStack.Push(pSNode2);
}

void SmXMLFracContext_Impl::EndElement()
{
    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = 0;
    aToken.nLevel = 0;
    aToken.eType = TOVER;
    SmStructureNode *pSNode = new SmBinVerNode(aToken);
    SmNode *pOper = new SmRectangleNode(aToken);
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    DBG_ASSERT(rNodeStack.Count() - nElementCount == 2,
        "Fraction (mfrac) tag is missing component");

    if (rNodeStack.Count() - nElementCount == 2)
    {
        SmNode *pSecond = rNodeStack.Pop();
        SmNode *pFirst = rNodeStack.Pop();
        pSNode->SetSubNodes(pFirst,pOper,pSecond);
        rNodeStack.Push(pSNode);
    }
}

void SmXMLRootContext_Impl::EndElement()
{
    /*The <mroot> element requires exactly 2 arguments.*/
    DBG_ASSERT(GetSmImport().GetNodeStack().Count() - nElementCount == 2,
        "Root tag is missing component");

    SmToken aToken;
    aToken.cMathChar = MS_SQRT;  //Temporary: alert, based on StarSymbol font
    aToken.nGroup = 0;
    aToken.nLevel = 0;
    aToken.eType = TNROOT;
    SmStructureNode *pSNode = new SmRootNode(aToken);
    SmNode *pOper = new SmRootSymbolNode(aToken);
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    SmNode *pIndex = rNodeStack.Pop();
    SmNode *pBase = rNodeStack.Pop();
    pSNode->SetSubNodes(pIndex,pOper,pBase);
    rNodeStack.Push(pSNode);
}

void SmXMLSqrtContext_Impl::EndElement()
{
    /*
    <msqrt> accepts any number of arguments; if this number is not 1, its
    contents are treated as a single "inferred <mrow>" containing its
    arguments
    */
    if (GetSmImport().GetNodeStack().Count() - nElementCount > 1)
        SmXMLRowContext_Impl::EndElement();

    SmToken aToken;
    aToken.cMathChar = MS_SQRT;  //Temporary: alert, based on StarSymbol font
    aToken.nGroup = 0;
    aToken.nLevel = 0;
    aToken.eType = TSQRT;
    SmStructureNode *pSNode = new SmRootNode(aToken);
    SmNode *pOper = new SmRootSymbolNode(aToken);
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    pSNode->SetSubNodes(0,pOper,rNodeStack.Pop());
    rNodeStack.Push(pSNode);
}

void SmXMLRowContext_Impl::EndElement()
{
    SmNodeArray aRelationArray;
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    ULONG nSize = rNodeStack.Count()-nElementCount;

    if (nSize)
    {
        aRelationArray.SetSize(nSize);
        for(ULONG j=rNodeStack.Count()-nElementCount;j > 0;j--)
            aRelationArray.Put(j-1,rNodeStack.Pop());


        //If the first or last element is an operator with stretchyness
        //set then we must create a brace node here from those elements,
        //removing the stretchness from the operators and applying it to
        //ourselves, and creating the appropiate dummy StarMath none bracket
        //to balance the arrangement
        if (((aRelationArray.Get(0)->GetScaleMode() == SCALE_HEIGHT)
            && (aRelationArray.Get(0)->GetType() == NMATH))
        || ((aRelationArray.Get(nSize-1)->GetScaleMode() == SCALE_HEIGHT)
            && (aRelationArray.Get(nSize-1)->GetType() == NMATH)))
        {
            SmToken aToken;
            aToken.cMathChar = '\0';
            aToken.nGroup = 0;
            aToken.nLevel = 5;

            int nLeft=0,nRight=0;
            if ((aRelationArray.Get(0)->GetScaleMode() == SCALE_HEIGHT)
                && (aRelationArray.Get(0)->GetType() == NMATH))
            {
                aToken = aRelationArray.Get(0)->GetToken();
                nLeft=1;
            }
            else
                aToken.cMathChar = '\0';

            aToken.eType = TLPARENT;
            SmNode *pLeft = new SmMathSymbolNode(aToken);

            if ((aRelationArray.Get(nSize-1)->GetScaleMode() == SCALE_HEIGHT)
                && (aRelationArray.Get(nSize-1)->GetType() == NMATH))
            {
                aToken = aRelationArray.Get(nSize-1)->GetToken();
                nRight=1;
            }
            else
                aToken.cMathChar = '\0';

            aToken.eType = TRPARENT;
            SmNode *pRight = new SmMathSymbolNode(aToken);

            SmNodeArray aRelationArray2;

            //!! nSize-nLeft-nRight may be < 0 !!
            int nRelArrSize = nSize-nLeft-nRight;
            if (nRelArrSize > 0)
            {
                aRelationArray2.SetSize(nRelArrSize);
                for(int i=0;i < nRelArrSize;i++)
                    aRelationArray2.Put(i,aRelationArray.Get(i+nLeft));
            }

            SmToken aDummy;
            SmStructureNode *pSNode = new SmBraceNode(aToken);
            SmStructureNode *pBody = new SmExpressionNode(aDummy);
            pBody->SetSubNodes(aRelationArray2);

            pSNode->SetSubNodes(pLeft,pBody,pRight);
            pSNode->SetScaleMode(SCALE_HEIGHT);
            rNodeStack.Push(pSNode);
            return;
        }
    }
    else //Multiple newlines result in empty row elements
    {
        aRelationArray.SetSize(1);
        SmToken aToken;
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 5;
        aToken.eType = TNEWLINE;
        aRelationArray.Put(0,new SmLineNode(aToken));
    }

    SmToken aDummy;
    SmStructureNode *pSNode = new SmExpressionNode(aDummy);
    pSNode->SetSubNodes(aRelationArray);
    rNodeStack.Push(pSNode);
}





SvXMLImportContext *SmXMLRowContext_Impl::StrictCreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const uno::Reference<xml::sax::XAttributeList>& xAttrList)
{
    SvXMLImportContext* pContext = 0L;

    const SvXMLTokenMap& rTokenMap = GetSmImport().GetPresElemTokenMap();
    switch(rTokenMap.Get(nPrefix, rLocalName))
    {
        /*Note that these should accept malignmark subelements, but do not*/
        case XML_TOK_MN:
            pContext = GetSmImport().CreateNumberContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MI:
            pContext = GetSmImport().CreateIdentifierContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MO:
            pContext = GetSmImport().CreateOperatorContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MTEXT:
            pContext = GetSmImport().CreateTextContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MSPACE:
            pContext = GetSmImport().CreateSpaceContext(nPrefix,rLocalName,
                xAttrList);
            break;
        case XML_TOK_MS:
            pContext = GetSmImport().CreateStringContext(nPrefix,rLocalName,
                xAttrList);
            break;

        /*Note: The maligngroup should only be seen when the row
         * (or decendants) are in a table*/
        case XML_TOK_MALIGNGROUP:
            pContext = GetSmImport().CreateAlignGroupContext(nPrefix,rLocalName,
                xAttrList);
            break;

        case XML_TOK_ANNOTATION:
            pContext = GetSmImport().CreateAnnotationContext(nPrefix,rLocalName,
                xAttrList);
            break;

        default:
            break;
    }
    return pContext;
}



SvXMLImportContext *SmXMLRowContext_Impl::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const uno::Reference<xml::sax::XAttributeList>& xAttrList)
{
    SvXMLImportContext* pContext = StrictCreateChildContext(nPrefix,
    rLocalName, xAttrList);

    if (!pContext)
    {
        //Hmm, unrecognized for this level, check to see if its
        //an element that can have an implicit schema around it
        pContext = SmXMLDocContext_Impl::CreateChildContext(nPrefix,
            rLocalName,xAttrList);
    }
    return pContext;
}


SvXMLImportContext *SmXMLMultiScriptsContext_Impl::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const uno::Reference<xml::sax::XAttributeList>& xAttrList)
{
    SvXMLImportContext* pContext = 0L;

    const SvXMLTokenMap& rTokenMap = GetSmImport().
        GetPresScriptEmptyElemTokenMap();
    switch(rTokenMap.Get(nPrefix, rLocalName))
    {
        case XML_TOK_MPRESCRIPTS:
            MiddleElement();
            pContext = GetSmImport().CreatePrescriptsContext(nPrefix,
                rLocalName, xAttrList);
            break;
        case XML_TOK_NONE:
            pContext = GetSmImport().CreateNoneContext(nPrefix,rLocalName,
                xAttrList);
            break;
        default:
            pContext = SmXMLRowContext_Impl::CreateChildContext(nPrefix,
                rLocalName,xAttrList);
            break;
    }
    return pContext;
}

void SmXMLMultiScriptsContext_Impl::MiddleElement()
{
    bHasPrescripts=sal_True;

    DBG_ASSERT(GetSmImport().GetNodeStack().Count() - nElementCount > 0,
        "Sub has no arguments");
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    if (rNodeStack.Count()-nElementCount > 1)
    {
        SmToken aToken;
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 0;
        aToken.eType = TRSUB;
        ULONG nFinalCount = rNodeStack.Count()-nElementCount-1;

        SmNodeStack aReverseStack;
        while (rNodeStack.Count()-nElementCount)
        {
            SmNode *pThing = rNodeStack.Pop();
            aReverseStack.Push(pThing);
        }

        for (ULONG nCount=0;nCount < nFinalCount;nCount+=2)
        {
            SmSubSupNode *pNode = new SmSubSupNode(aToken);

            // initialize subnodes array
            SmNodeArray  aSubNodes;
            aSubNodes.SetSize(1 + SUBSUP_NUM_ENTRIES);
            for (ULONG i = 1;  i < aSubNodes.GetSize();  i++)
                aSubNodes.Put(i, NULL);

            /*On each loop the base and its sub sup pair becomes the
             base for the next loop to which the next sub sup pair is
             attached, i.e. wheels within wheels*/
            //if (nCount == 0)
            aSubNodes.Put(0, aReverseStack.Pop());

            SmNode *pScriptNode = aReverseStack.Pop();

            if ((pScriptNode->GetToken().eType != TIDENT) ||
                (pScriptNode->GetToken().aText.Len()))
                aSubNodes.Put(RSUB+1,pScriptNode);
            pScriptNode = aReverseStack.Pop();
            if ((pScriptNode->GetToken().eType != TIDENT) ||
                (pScriptNode->GetToken().aText.Len()))
                aSubNodes.Put(RSUP+1,pScriptNode);

            pNode->SetSubNodes(aSubNodes);
            aReverseStack.Push(pNode);
        }
    rNodeStack.Push(aReverseStack.Pop());
    }
}


void SmXMLTableContext_Impl::EndElement()
{
    SmNodeArray aExpressionArray;
    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    SmNodeStack aReverseStack;
    aExpressionArray.SetSize(rNodeStack.Count()-nElementCount);

    ULONG nRows = rNodeStack.Count()-nElementCount;
    USHORT nCols = 0;

    SmStructureNode *pArray;
    for(ULONG i=rNodeStack.Count()-nElementCount;i > 0;i--)
    {
        pArray = (SmStructureNode *)rNodeStack.Pop();
        if (pArray->GetNumSubNodes() == 0)
        {
            //This is a little tricky, it is possible that there was
            //be elements that were not inside a <mtd> pair, in which
            //case they will not be in a row, i.e. they will not have
            //SubNodes, so we have to wait until here before we can
            //resolve the situation. Implicitsurrounding tags are
            //surprisingly difficult to get right within this
            //architecture

            SmNodeArray aRelationArray;
            aRelationArray.SetSize(1);
            aRelationArray.Put(0,pArray);
            SmToken aDummy;
            pArray = new SmExpressionNode(aDummy);
            pArray->SetSubNodes(aRelationArray);
        }

        if (pArray->GetNumSubNodes() > nCols)
            nCols = pArray->GetNumSubNodes();
        aReverseStack.Push(pArray);
    }
    aExpressionArray.SetSize(nCols*nRows);
    ULONG j=0;
    while (aReverseStack.Count())
    {
        pArray = (SmStructureNode *)aReverseStack.Pop();
        for (USHORT i=0;i<pArray->GetNumSubNodes();i++)
            aExpressionArray.Put(j++,pArray->GetSubNode(i));
    }

    SmToken aToken;
    aToken.cMathChar = '\0';
    aToken.nGroup = TRGROUP;
    aToken.nLevel = 0;
    aToken.eType = TMATRIX;
    SmMatrixNode *pSNode = new SmMatrixNode(aToken);
    pSNode->SetSubNodes(aExpressionArray);
    pSNode->SetRowCol(static_cast<USHORT>(nRows),nCols);
    rNodeStack.Push(pSNode);
}

SvXMLImportContext *SmXMLTableRowContext_Impl::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const uno::Reference<xml::sax::XAttributeList>& xAttrList)
{
    SvXMLImportContext* pContext = 0L;

    const SvXMLTokenMap& rTokenMap = GetSmImport().
        GetPresTableElemTokenMap();
    switch(rTokenMap.Get(nPrefix, rLocalName))
    {
        case XML_TOK_MTD:
            pContext = GetSmImport().CreateTableCellContext(nPrefix,
                rLocalName, xAttrList);
            break;
        default:
            pContext = SmXMLRowContext_Impl::CreateChildContext(nPrefix,
                rLocalName,xAttrList);
            break;
    }
    return pContext;
}

SvXMLImportContext *SmXMLTableContext_Impl::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const uno::Reference<xml::sax::XAttributeList>& xAttrList)
{
    SvXMLImportContext* pContext = 0L;

    const SvXMLTokenMap& rTokenMap = GetSmImport().
        GetPresTableElemTokenMap();
    switch(rTokenMap.Get(nPrefix, rLocalName))
    {
        case XML_TOK_MTR:
            pContext = GetSmImport().CreateTableRowContext(nPrefix,rLocalName,
                xAttrList);
            break;
        default:
            pContext = SmXMLTableRowContext_Impl::CreateChildContext(nPrefix,
                rLocalName,xAttrList);
            break;
    }
    return pContext;
}

void SmXMLMultiScriptsContext_Impl::EndElement()
{
    if (!bHasPrescripts)
        MiddleElement();

    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    if (rNodeStack.Count()-nElementCount > 1)
    {
        SmToken aToken;
        aToken.cMathChar = '\0';
        aToken.nGroup = 0;
        aToken.nLevel = 0;
        aToken.eType = TLSUB;
        ULONG nFinalCount = rNodeStack.Count()-nElementCount-1;

        SmNodeStack aReverseStack;
        while (rNodeStack.Count()-nElementCount)
            aReverseStack.Push(rNodeStack.Pop());
        for (ULONG nCount=0;nCount < nFinalCount;nCount+=2)
        {
            SmSubSupNode *pNode = new SmSubSupNode(aToken);

            // initialize subnodes array
            SmNodeArray  aSubNodes;
            aSubNodes.SetSize(1 + SUBSUP_NUM_ENTRIES);
            for (ULONG i = 1;  i < aSubNodes.GetSize();  i++)
                aSubNodes.Put(i, NULL);

            /*On each loop the base and its sub sup pair becomes the
             base for the next loop to which the next sub sup pair is
             attached, i.e. wheels within wheels*/
            //if (nCount == 0)
            aSubNodes.Put(0, aReverseStack.Pop());

            SmNode *pScriptNode = aReverseStack.Pop();
            if (pScriptNode->GetToken().aText.Len())
                aSubNodes.Put(LSUB+1,pScriptNode);
            pScriptNode = aReverseStack.Pop();
            if (pScriptNode->GetToken().aText.Len())
                aSubNodes.Put(LSUP+1,pScriptNode);

            pNode->SetSubNodes(aSubNodes);
            aReverseStack.Push(pNode);
        }
    rNodeStack.Push(aReverseStack.Pop());
    }

}
void SmXMLActionContext_Impl::EndElement()
{
    /*For now we will just assume that the
     selected attribute is one, and then just display
     that expression alone, i.e. remove all expect the
     first pushed one*/

    SmNodeStack &rNodeStack = GetSmImport().GetNodeStack();
    for(ULONG i=rNodeStack.Count()-nElementCount;i > 1;i--)
    {
        delete rNodeStack.Pop();
    }
}

SvXMLImportContext *SmXMLImport::CreateContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
    if( XML_NAMESPACE_OFFICE == nPrefix )
        return new SmXMLOfficeContext_Impl( *this,nPrefix,rLocalName);
    else
        return new SmXMLDocContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateRowContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLRowContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateTextContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLTextContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateAnnotationContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLAnnotationContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateStringContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLStringContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateNumberContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLNumberContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateIdentifierContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLIdentifierContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateOperatorContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
    return new SmXMLOperatorContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateSpaceContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
    return new SmXMLSpaceContext_Impl(*this,nPrefix,rLocalName);
}


SvXMLImportContext *SmXMLImport::CreateFracContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLFracContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateSqrtContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLSqrtContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateRootContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLRootContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateStyleContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLStyleContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreatePaddedContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLPaddedContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreatePhantomContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLPhantomContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateFencedContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLFencedContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateErrorContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLErrorContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateSubContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLSubContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateSubSupContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLSubSupContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateSupContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLSupContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateUnderContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLUnderContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateOverContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLOverContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateUnderOverContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLUnderOverContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateMultiScriptsContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLMultiScriptsContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateTableContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLTableContext_Impl(*this,nPrefix,rLocalName);
}
SvXMLImportContext *SmXMLImport::CreateTableRowContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLTableRowContext_Impl(*this,nPrefix,rLocalName);
}
SvXMLImportContext *SmXMLImport::CreateTableCellContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLTableCellContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateNoneContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLNoneContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreatePrescriptsContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLPrescriptsContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateAlignGroupContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLAlignGroupContext_Impl(*this,nPrefix,rLocalName);
}

SvXMLImportContext *SmXMLImport::CreateActionContext(sal_uInt16 nPrefix,
    const OUString &rLocalName,
    const uno::Reference <xml::sax::XAttributeList> &xAttrList)
{
        return new SmXMLActionContext_Impl(*this,nPrefix,rLocalName);
}

SmXMLImport::~SmXMLImport() throw ()
{
    delete pPresLayoutElemTokenMap;
    delete pPresElemTokenMap;
    delete pPresScriptEmptyElemTokenMap;
    delete pPresTableElemTokenMap;
    delete pPresLayoutAttrTokenMap;
    delete pFencedAttrTokenMap;
    delete pColorTokenMap;
    delete pOperatorAttrTokenMap;
    delete pAnnotationAttrTokenMap;
}

void SmXMLImport::SetViewSettings(const Sequence<PropertyValue>& aViewProps)
{
    uno::Reference <frame::XModel> xModel = GetModel();
    if( !xModel.is() )
        return;

    uno::Reference <lang::XUnoTunnel> xTunnel;
    xTunnel = uno::Reference <lang::XUnoTunnel> (xModel,uno::UNO_QUERY);
    SmModel *pModel = reinterpret_cast<SmModel *>
        (xTunnel->getSomething(SmModel::getUnoTunnelId()));

    if( !pModel )
        return;

    SmDocShell *pDocShell =
        static_cast<SmDocShell*>(pModel->GetObjectShell());
    if( !pDocShell )
        return;

    Rectangle aRect( pDocShell->GetVisArea() );

    sal_Int32 nCount = aViewProps.getLength();
    const PropertyValue *pValue = aViewProps.getConstArray();

    long nTmp;
    sal_Bool bShowDeletes = sal_False, bShowInserts = sal_False, bShowFooter = sal_False, bShowHeader = sal_False;

    for (sal_Int32 i = 0; i < nCount ; i++)
    {
        if (pValue->Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "ViewAreaTop" ) ) )
        {
            pValue->Value >>= nTmp;
            aRect.setY( nTmp );
        }
        else if (pValue->Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "ViewAreaLeft" ) ) )
        {
            pValue->Value >>= nTmp;
            aRect.setX( nTmp );
        }
        else if (pValue->Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "ViewAreaWidth" ) ) )
        {
            pValue->Value >>= nTmp;
            Size aSize( aRect.GetSize() );
            aSize.Width() = nTmp;
            aRect.SetSize( aSize );
        }
        else if (pValue->Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( "ViewAreaHeight" ) ) )
        {
            pValue->Value >>= nTmp;
            Size aSize( aRect.GetSize() );
            aSize.Height() = nTmp;
            aRect.SetSize( aSize );
        }
        pValue++;
    }

    pDocShell->SetVisArea ( aRect );
}

void SmXMLImport::SetConfigurationSettings(const Sequence<PropertyValue>& aConfProps)
{
    uno::Reference < XPropertySet > xProps ( GetModel(), UNO_QUERY );
    if ( xProps.is() )
    {
        Reference < XPropertySetInfo > xInfo ( xProps->getPropertySetInfo() );
        if (xInfo.is() )
        {
            sal_Int32 nCount = aConfProps.getLength();
            const PropertyValue* pValues = aConfProps.getConstArray();

            const OUString sFormula ( RTL_CONSTASCII_USTRINGPARAM ( "Formula" ) );
            const OUString sBasicLibraries ( RTL_CONSTASCII_USTRINGPARAM ( "BasicLibraries" ) );
            while( nCount-- )
            {
                if (pValues->Name != sFormula &&
                    pValues->Name != sBasicLibraries)
                {
                    try
                    {
                        if( xInfo->hasPropertyByName( pValues->Name ) )
                            xProps->setPropertyValue( pValues->Name, pValues->Value );
                    }
                    catch( Exception& )
                    {
                        DBG_ERROR( "SmXMLImport::SetConfigurationSettings: Exception!" );
                    }
                }

                pValues++;
            }
        }
    }
}
void SmXMLExport::_ExportContent()
{
    SvXMLElementExport aEquation(*this,XML_NAMESPACE_MATH,sXML_math, sal_True,
        sal_True);
    SvXMLElementExport *pSemantics=0;

    if (aText.Len())
    {
        pSemantics = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
            sXML_semantics, sal_True, sal_True);
    }

    ExportNodes(pTree,0);

    if (aText.Len())
    {
        // Convert symbol names
        uno::Reference <frame::XModel> xModel = GetModel();
        uno::Reference <lang::XUnoTunnel> xTunnel;
        xTunnel = uno::Reference <lang::XUnoTunnel> (xModel,uno::UNO_QUERY);
        SmModel *pModel = reinterpret_cast<SmModel *>
            (xTunnel->getSomething(SmModel::getUnoTunnelId()));
        SmDocShell *pDocShell = pModel ?
            static_cast<SmDocShell*>(pModel->GetObjectShell()) : 0;
        DBG_ASSERT( pDocShell, "doc shell missing" );
        if (pDocShell)
        {
            SmParser &rParser = pDocShell->GetParser();
            BOOL bVal = rParser.IsExportSymbolNames();
            rParser.SetExportSymbolNames( TRUE );
            SmNode *pTree = rParser.Parse( aText );
            aText = rParser.GetText();
            delete pTree;
            rParser.SetExportSymbolNames( bVal );
        }

        AddAttribute(XML_NAMESPACE_MATH,sXML_encoding,
            OUString(RTL_CONSTASCII_USTRINGPARAM("StarMath 5.0")));
        SvXMLElementExport aAnnotation(*this,XML_NAMESPACE_MATH,
            sXML_annotation,sal_True, sal_False);
        GetDocHandler()->characters(OUString( aText ));
    }
    delete pSemantics;
}

void SmXMLExport::GetViewSettings( Sequence < PropertyValue >& aProps)
{
    uno::Reference <frame::XModel> xModel = GetModel();
    if( !xModel.is() )
        return;

    uno::Reference <lang::XUnoTunnel> xTunnel;
    xTunnel = uno::Reference <lang::XUnoTunnel> (xModel,uno::UNO_QUERY);
    SmModel *pModel = reinterpret_cast<SmModel *>
        (xTunnel->getSomething(SmModel::getUnoTunnelId()));

    if( !pModel )
        return;

    SmDocShell *pDocShell =
        static_cast<SmDocShell*>(pModel->GetObjectShell());
    if( !pDocShell )
        return;

    aProps.realloc( 4 );
    PropertyValue *pValue = aProps.getArray();
    sal_Int32 nIndex = 0;

    Rectangle aRect( pDocShell->GetVisArea() );

    pValue[nIndex].Name = OUString( RTL_CONSTASCII_USTRINGPARAM ( "ViewAreaTop") );
    pValue[nIndex++].Value <<= aRect.Top();

    pValue[nIndex].Name = OUString( RTL_CONSTASCII_USTRINGPARAM ( "ViewAreaLeft") );
    pValue[nIndex++].Value <<= aRect.Left();

    pValue[nIndex].Name = OUString( RTL_CONSTASCII_USTRINGPARAM ( "ViewAreaWidth") );
    pValue[nIndex++].Value <<= aRect.GetWidth();

    pValue[nIndex].Name = OUString( RTL_CONSTASCII_USTRINGPARAM ( "ViewAreaHeight") );
    pValue[nIndex++].Value <<= aRect.GetHeight();
}

void SmXMLExport::GetConfigurationSettings( Sequence < PropertyValue > & rProps)
{
    Reference < XPropertySet > xProps ( GetModel(), UNO_QUERY );
    if ( xProps.is() )
    {
        Reference< XPropertySetInfo > xPropertySetInfo = xProps->getPropertySetInfo();
        if (xPropertySetInfo.is())
        {
            Sequence< Property > aProps = xPropertySetInfo->getProperties();
            sal_Int32 nCount(aProps.getLength());
            if (nCount)
            {
                rProps.realloc(nCount);
                PropertyValue* pProps = rProps.getArray();
                if (pProps)
                {
                    const OUString sFormula ( RTL_CONSTASCII_USTRINGPARAM ( "Formula" ) );
                    const OUString sBasicLibraries ( RTL_CONSTASCII_USTRINGPARAM ( "BasicLibraries" ) );
                    const OUString sRuntimeUID ( RTL_CONSTASCII_USTRINGPARAM ( "RuntimeUID" ) );
                    for (sal_Int32 i = 0; i < nCount; i++, pProps++)
                    {
                        const OUString &rPropName = aProps[i].Name;
                        if (rPropName != sFormula &&
                            rPropName != sBasicLibraries &&
                            rPropName != sRuntimeUID)
                        {
                            pProps->Name = rPropName;
                            pProps->Value = xProps->getPropertyValue(rPropName);
                        }
                    }
                }
            }
        }
    }
}

void SmXMLExport::ExportLine(const SmNode *pNode,int nLevel)
{
    ExportExpression(pNode,nLevel);
}

void SmXMLExport::ExportBinaryHorizontal(const SmNode *pNode,int nLevel)
{
    ExportExpression(pNode,nLevel);
}

void SmXMLExport::ExportUnaryHorizontal(const SmNode *pNode,int nLevel)
{
    ExportExpression(pNode,nLevel);
}

void SmXMLExport::ExportExpression(const SmNode *pNode,int nLevel)
{
    SvXMLElementExport *pRow=0;
    ULONG  nSize = pNode->GetNumSubNodes();

    if (nSize > 1)
        pRow = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mrow,
        sal_True, sal_True);

    //if (nSize)
    //{
        for (USHORT i = 0; i < nSize; i++)
            if (const SmNode *pTemp = pNode->GetSubNode(i))
                ExportNodes(pTemp,nLevel+1);
    //}
#if 0
    else
    {
        //This saves us from situations like "a newline" where the
        //lack of a term following the newline would otherwise create
        //a incorrect token like <mtr/>
        SvXMLElementExport aDummy(*this,XML_NAMESPACE_MATH,sXML_mi,
            sal_True,sal_True);
        sal_Unicode nArse[2] = {'\n','\0'};
        GetDocHandler()->characters(nArse);
    }
#endif

    delete pRow;
}

void SmXMLExport::ExportBinaryVertical(const SmNode *pNode,int nLevel)
{
    DBG_ASSERT(pNode->GetNumSubNodes()==3,"Bad Fraction");
    SvXMLElementExport aFraction(*this,XML_NAMESPACE_MATH,sXML_mfrac, sal_True,
        sal_True);
    ExportNodes(pNode->GetSubNode(0),nLevel);
    ExportNodes(pNode->GetSubNode(2),nLevel);
}

void SmXMLExport::ExportTable(const SmNode *pNode, int nLevel)
{
    SvXMLElementExport *pTable=0;

    USHORT nSize = pNode->GetNumSubNodes();

    //If the list ends in newline then the last entry has
    //no subnodes, the newline is superfulous so we just drop
    //the last node, inclusion would create a bad MathML
    //table
    if (pNode->GetSubNode(nSize-1)->GetNumSubNodes() == 0)
        nSize--;

    if ( nLevel || (nSize >1))
        pTable = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mtable,
        sal_True, sal_True);

    for (USHORT i = 0; i < nSize; i++)
        if (const SmNode *pTemp = pNode->GetSubNode(i))
        {
            SvXMLElementExport *pRow=0;
            if (pTable)
                pRow = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                    sXML_mtr, sal_True, sal_True);
            ExportNodes(pTemp,nLevel+1);
            delete pRow;
        }

    delete pTable;
}

void SmXMLExport::ExportMath(const SmNode *pNode, int nLevel)
{
    const SmMathSymbolNode *pTemp = static_cast<const SmMathSymbolNode *>
        (pNode);
    SvXMLElementExport aMath(*this,XML_NAMESPACE_MATH,sXML_mo,
        sal_True,sal_False);
    sal_Unicode nArse[2];
    nArse[0] = pTemp->GetText().GetChar(0);
    DBG_ASSERT(nArse[0] != 0xffff,"Non existant symbol");
    nArse[1] = 0;
    GetDocHandler()->characters(nArse);
}

void SmXMLExport::ExportText(const SmNode *pNode, int nLevel)
{
    SvXMLElementExport *aText;
    const SmTextNode *pTemp = static_cast<const SmTextNode *>(pNode);
    switch (pNode->GetToken().eType)
    {
        default:
        case TIDENT:
            //Note that we change the fontstyle to italic for strings that
            //are italic and longer than a single character.
            if ((pTemp->GetText().Len() > 1) &&
                (pTemp->GetFont().GetItalic() > ITALIC_NONE))
                AddAttribute(XML_NAMESPACE_MATH,sXML_fontstyle,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_italic)));
            else if ((pTemp->GetText().Len() == 1) &&
                (pTemp->GetFont().GetItalic() == ITALIC_NONE))
                AddAttribute(XML_NAMESPACE_MATH,sXML_fontstyle,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_normal)));
            aText = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mi,
            sal_True,sal_False);
            break;
        case TNUMBER:
            aText = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mn,
            sal_True,sal_False);
            break;
        case TTEXT:
            aText = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mtext,
            sal_True,sal_False);
            break;
        }
    GetDocHandler()->characters(OUString(pTemp->GetText().GetBuffer()));
    delete aText;
}

void SmXMLExport::ExportSubSupScript(const SmNode *pNode,int nLevel)
{
    const SmNode *pSub,*pSup,*pCSub,*pCSup,*pLSub,*pLSup;
    SvXMLElementExport *pThing=0,*pThing2=0;

    //if we have prescripts at all then we must use the tensor notation

    //This is one of those excellent locations where scope is vital to
    //arrange the construction and destruction of the element helper
    //classes correctly
    pLSub = pNode->GetSubNode(LSUB+1);
    pLSup = pNode->GetSubNode(LSUP+1);
    if (pLSub || pLSup)
    {
        SvXMLElementExport aMultiScripts(*this,XML_NAMESPACE_MATH,
            sXML_mmultiscripts, sal_True, sal_True);


        if ((pCSub = pNode->GetSubNode(CSUB+1))
            && (pCSup=pNode->GetSubNode(CSUP+1)))
        {
            pThing2 = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_munderover, sal_True,sal_True);
        }
        else if (pCSub = pNode->GetSubNode(CSUB+1))
        {
            pThing2 = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_munder, sal_True,sal_True);
        }
        else if (pCSup = pNode->GetSubNode(CSUP+1))
        {
            pThing2 = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_mover, sal_True,sal_True);
        }

        ExportNodes(pNode->GetSubNode(0), nLevel+1);    //Main Term

        if (pCSub)
            ExportNodes(pCSub, nLevel+1);
        if (pCSup)
            ExportNodes(pCSup, nLevel+1);
        delete pThing2;

        pSub = pNode->GetSubNode(RSUB+1);
        pSup = pNode->GetSubNode(RSUP+1);
        if (pSub || pSup)
        {
            if (pSub)
                ExportNodes(pSub, nLevel+1);
            else
            {
                SvXMLElementExport aNone(*this,XML_NAMESPACE_MATH, sXML_none,
                    sal_True,sal_True);
            }
            if (pSup)
                ExportNodes(pSup, nLevel+1);
            else
            {
                SvXMLElementExport aNone(*this,XML_NAMESPACE_MATH, sXML_none,
                    sal_True,sal_True);
            }
        }

        //Seperator element between suffix and prefix sub/sup pairs
        {
            SvXMLElementExport aPrescripts(*this,XML_NAMESPACE_MATH,
                sXML_mprescripts, sal_True,sal_True);
        }

        if (pLSub)
            ExportNodes(pLSub, nLevel+1);
        else
        {
            SvXMLElementExport aNone(*this,XML_NAMESPACE_MATH, sXML_none,
                sal_True,sal_True);

        }
        if (pLSup)
            ExportNodes(pLSup, nLevel+1);
        else
        {
            SvXMLElementExport aNone(*this,XML_NAMESPACE_MATH, sXML_none,
                sal_True,sal_True);

        }
    }
    else
    {
        if ((pSub = pNode->GetSubNode(RSUB+1)) &&
            (pSup = pNode->GetSubNode(RSUP+1)))
        {
            pThing = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_msubsup, sal_True,sal_True);
        }
        else if (pSub = pNode->GetSubNode(RSUB+1))
        {
            pThing = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_msub,
                sal_True,sal_True);
        }
        else if (pSup = pNode->GetSubNode(RSUP+1))
        {
            pThing = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_msup,
                sal_True,sal_True);
        }

        if ((pCSub = pNode->GetSubNode(CSUB+1))
            && (pCSup=pNode->GetSubNode(CSUP+1)))
        {
            pThing2 = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_munderover, sal_True,sal_True);
        }
        else if (pCSub = pNode->GetSubNode(CSUB+1))
        {
            pThing2 = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_munder, sal_True,sal_True);
        }
        else if (pCSup = pNode->GetSubNode(CSUP+1))
        {
            pThing2 = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_mover, sal_True,sal_True);
        }
        ExportNodes(pNode->GetSubNode(0), nLevel+1);    //Main Term

        if (pCSub)
            ExportNodes(pCSub, nLevel+1);
        if (pCSup)
            ExportNodes(pCSup, nLevel+1);
        delete pThing2;

        if (pSub)
            ExportNodes(pSub, nLevel+1);
        if (pSup)
            ExportNodes(pSup, nLevel+1);
        delete pThing;
    }
}

void SmXMLExport::ExportBrace(const SmNode *pNode, int nLevel)
{
    const SmNode *pTemp;
    const SmNode *pLeft=pNode->GetSubNode(0);
    const SmNode *pRight=pNode->GetSubNode(2);
    SvXMLElementExport *pFences=0,*pRow=0;
    if ( ((pLeft) && (pLeft->GetToken().eType != TNONE)) &&
        ((pRight) && (pRight->GetToken().eType != TNONE)) &&
        (pNode->GetScaleMode() == SCALE_HEIGHT))
    {
        sal_Unicode nArse[2];
        nArse[1] = 0;
        nArse[0] = static_cast<
            const SmMathSymbolNode* >(pLeft)->GetText().GetChar(0);
        DBG_ASSERT(nArse[0] != 0xffff,"Non existant symbol");
        AddAttribute(XML_NAMESPACE_MATH,sXML_open,nArse);
        nArse[0] = static_cast<
            const SmMathSymbolNode* >(pRight)->GetText().GetChar(0);
        DBG_ASSERT(nArse[0] != 0xffff,"Non existant symbol");
        AddAttribute(XML_NAMESPACE_MATH,sXML_close,nArse);
        pFences = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mfenced,
            sal_True,sal_True);
    }
    else if (pLeft && (pLeft->GetToken().eType != TNONE))
    {
        pRow = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mrow,
            sal_True, sal_True);
        if (pNode->GetScaleMode() == SCALE_HEIGHT)
            AddAttribute(XML_NAMESPACE_MATH,sXML_stretchy,OUString(
            RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        else
            AddAttribute(XML_NAMESPACE_MATH,sXML_stretchy,OUString(
            RTL_CONSTASCII_USTRINGPARAM(sXML_false)));
        ExportNodes(pLeft,nLevel+1);
    }
    else
        pRow = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mrow,
            sal_True, sal_True);

    if (pTemp = pNode->GetSubNode(1))
        ExportNodes(pTemp,nLevel+1);
    if (pFences)
        delete pFences;
    else if (pRight && (pRight->GetToken().eType != TNONE))
    {
        if (pNode->GetScaleMode() == SCALE_HEIGHT)
            AddAttribute(XML_NAMESPACE_MATH,sXML_stretchy,OUString(
            RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        else
            AddAttribute(XML_NAMESPACE_MATH,sXML_stretchy,OUString(
            RTL_CONSTASCII_USTRINGPARAM(sXML_false)));
        ExportNodes(pRight,nLevel+1);
    }
    delete pRow;
}

void SmXMLExport::ExportRoot(const SmNode *pNode, int nLevel)
{
    if (pNode->GetSubNode(0))
    {
        SvXMLElementExport aRoot(*this,XML_NAMESPACE_MATH,sXML_mroot,sal_True,
            sal_True);
        ExportNodes(pNode->GetSubNode(2),nLevel+1);
        ExportNodes(pNode->GetSubNode(0),nLevel+1);
    }
    else
    {
        SvXMLElementExport aSqrt(*this,XML_NAMESPACE_MATH,sXML_msqrt,sal_True,
            sal_True);
        ExportNodes(pNode->GetSubNode(2),nLevel+1);
    }
}

void SmXMLExport::ExportOperator(const SmNode *pNode, int nLevel)
{
    /*we need to either use content or font and size attributes
     *here*/
#if 0
    {
    SvXMLElementExport aMath(*this,XML_NAMESPACE_MATH,sXML_mo,
        sal_True,sal_False);
    SmTextNode *pTemp = (SmTextNode *)pNode->GetSubNode(0);
    GetDocHandler()->characters(pTemp->GetText());
    }
#endif
    SvXMLElementExport aRow(*this,XML_NAMESPACE_MATH,sXML_mrow,
        sal_True, sal_True);
    ExportNodes(pNode->GetSubNode(0),nLevel+1);
    ExportNodes(pNode->GetSubNode(1),nLevel+1);
}

void SmXMLExport::ExportAttributes(const SmNode *pNode, int nLevel)
{
    SvXMLElementExport *pElement=0;

    if (pNode->GetToken().eType == TUNDERLINE)
    {
        AddAttribute(XML_NAMESPACE_MATH,sXML_accentunder,
            OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        pElement = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_munder,
            sal_True,sal_True);
    }
    else if (pNode->GetToken().eType != TOVERSTRIKE)
    {
        AddAttribute(XML_NAMESPACE_MATH,sXML_accent,
            OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        pElement = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,sXML_mover,
            sal_True,sal_True);
    }

    ExportNodes(pNode->GetSubNode(1),nLevel+1);
    switch (pNode->GetToken().eType)
    {
        case TOVERLINE:
            {
            //proper entity support required
            SvXMLElementExport aMath(*this,XML_NAMESPACE_MATH,sXML_mo,
                sal_True,sal_False);
#if 0
            GetDocHandler()->characters(
                OUString(RTL_CONSTASCII_USTRINGPARAM("&overbar;")));
#else
            sal_Unicode nArse[2] = {0xAF,0x00};
#endif
            GetDocHandler()->characters(nArse);
            }
            break;
        case TUNDERLINE:
            {
            //proper entity support required
            SvXMLElementExport aMath(*this,XML_NAMESPACE_MATH,sXML_mo,
                sal_True,sal_False);
#if 0
            GetDocHandler()->characters(
                OUString(RTL_CONSTASCII_USTRINGPARAM("&underbar;")));
#else
            sal_Unicode nArse[2] = {0x0332,0x00};
#endif
            GetDocHandler()->characters(nArse);
            }
            break;
        case TOVERSTRIKE:
            break;
        default:
            ExportNodes(pNode->GetSubNode(0),nLevel+1);
            break;
    }
    delete pElement;
}

void SmXMLExport::ExportFont(const SmNode *pNode, int nLevel)
{
    SvXMLElementExport *pElement=0;
    switch (pNode->GetToken().eType)
    {
        //wrap a phantom element around everything*/
        case TPHANTOM:
            pElement = new SvXMLElementExport(*this,XML_NAMESPACE_MATH,
                sXML_mphantom, sal_True,sal_True);
            break;
        case TBOLD:
            AddAttribute(XML_NAMESPACE_MATH,sXML_fontweight,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_bold)));
            break;
        case TITALIC:
            AddAttribute(XML_NAMESPACE_MATH,sXML_fontstyle,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_italic)));
            break;
        case TNBOLD:
            AddAttribute(XML_NAMESPACE_MATH,sXML_fontweight,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_weight_normal)));
            break;
        case TNITALIC:
            AddAttribute(XML_NAMESPACE_MATH,sXML_fontstyle,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_weight_normal)));
            break;
        case TBLACK:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_black)));
            break;
        case TWHITE:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_white)));
            break;
        case TRED:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_red)));
            break;
        case TGREEN:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_green)));
            break;
        case TBLUE:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_blue)));
            break;
        case TCYAN:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_aqua)));
            break;
        case TMAGENTA:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_fuchsia)));
            break;
        case TYELLOW:
            AddAttribute(XML_NAMESPACE_MATH,sXML_color,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_yellow)));
            break;
        case TSIZE:
            {
            const SmFontNode *pFontNode = static_cast<const SmFontNode *>
                (pNode);
            const Fraction &aFrac = pFontNode->GetSizeParameter();

            OUStringBuffer sStrBuf;
            switch(pFontNode->GetSizeType())
            {
                case FNTSIZ_MULTIPLY:
                    SvXMLUnitConverter::convertDouble(sStrBuf,
                        static_cast<double>(aFrac*Fraction(100.00)));
                    sStrBuf.append(static_cast<sal_Unicode>('%'));
                    break;
                case FNTSIZ_DIVIDE:
                    SvXMLUnitConverter::convertDouble(sStrBuf,
                        static_cast<double>(Fraction(100.00)/aFrac));
                    sStrBuf.append(static_cast<sal_Unicode>('%'));
                    break;
                case FNTSIZ_ABSOLUT:
                    SvXMLUnitConverter::convertDouble(sStrBuf,
                        static_cast<double>(aFrac));
                    sStrBuf.append(
                        OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_unit_pt)));
                    break;
                default:
                    {
                        //The problem here is that the wheels fall off because
                        //font size is stored in 100th's of a mm not pts, and
                        //rounding errors take their toll on the original
                        //value specified in points.

                        //Must fix StarMath to retain the original pt values
                        Fraction aTemp = Sm100th_mmToPts(pFontNode->GetFont().
                            GetSize().Height());

                        if (pFontNode->GetSizeType() == FNTSIZ_MINUS)
                            aTemp-=aFrac;
                        else
                            aTemp+=aFrac;

                        double mytest = static_cast<double>(aTemp);

                        mytest = ::rtl::math::round(mytest,1);
                        SvXMLUnitConverter::convertDouble(sStrBuf,mytest);
                        sStrBuf.append(OUString(
                            RTL_CONSTASCII_USTRINGPARAM(sXML_unit_pt)));
                    }
                    break;
            }

            OUString sStr(sStrBuf.makeStringAndClear());
            AddAttribute(XML_NAMESPACE_MATH,sXML_fontsize,sStr);
            }
            break;
        case TFIXED:
        case TSANS:
        case TSERIF:
            AddAttribute(XML_NAMESPACE_MATH,sXML_fontfamily,
                OUString(pNode->GetToken().aText.GetBuffer()));
            break;

    }
#if 0
    if (pNode->GetNumSubNodes() > 1) //or in the future is a node that
                                     //cannot take the currently supported
                                     //properties
#endif
    //for now we will just always export with a style and not worry about
    //anyone else for the moment.
    {
        //wrap a style around it
        SvXMLElementExport aStyle(*this,XML_NAMESPACE_MATH,
                sXML_mstyle, sal_True,sal_True);
        ExportExpression(pNode,nLevel);
    }
#if 0
    else
        ExportNodes(pNode->GetSubNode(0),nLevel+1);
#endif

    delete pElement;
}


void SmXMLExport::ExportVerticalBrace(const SmNode *pNode, int nLevel)
{
    //Place the overbrace value OVER a vertical brace and then place that
    //expression OVER the overbrace value, If someone can find a
    //dedicated term in MathML to handle this overbrace/underbrace concept
    //let me know. C.
    const sal_Char *pWhich;

    switch (pNode->GetToken().eType)
    {
        case TOVERBRACE:
        default:
            pWhich = sXML_mover;
            break;
        case TUNDERBRACE:
            pWhich = sXML_munder;
            break;
    }

    DBG_ASSERT(pNode->GetNumSubNodes()==3,"Bad Vertical Brace");
    SvXMLElementExport aOver1(*this,XML_NAMESPACE_MATH,pWhich, sal_True,
        sal_True);
    {//Scoping
        AddAttribute(XML_NAMESPACE_MATH,sXML_accent,OUString(
            RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        SvXMLElementExport aOver2(*this,XML_NAMESPACE_MATH,pWhich, sal_True,
            sal_True);
        ExportNodes(pNode->GetSubNode(0),nLevel);
        ExportNodes(pNode->GetSubNode(1),nLevel);
    }
    ExportNodes(pNode->GetSubNode(2),nLevel);
}

void SmXMLExport::ExportMatrix(const SmNode *pNode, int nLevel)
{
    SvXMLElementExport aTable(*this,XML_NAMESPACE_MATH,sXML_mtable,
        sal_True, sal_True);
    const SmMatrixNode *pMatrix = static_cast<const SmMatrixNode *>(pNode);
    USHORT i=0;
    for (ULONG y = 0; y < pMatrix->GetNumRows(); y++)
    {
        SvXMLElementExport aRow(*this,XML_NAMESPACE_MATH,sXML_mtr,
            sal_True, sal_True);
        for (ULONG x = 0; x < pMatrix->GetNumCols(); x++)
            if (const SmNode *pTemp = pNode->GetSubNode(i++))
            {
                SvXMLElementExport aCell(*this,XML_NAMESPACE_MATH,
                        sXML_mtd, sal_True, sal_True);
                ExportNodes(pTemp,nLevel+1);
            }
    }
}

void SmXMLExport::ExportNodes(const SmNode *pNode, int nLevel)
{
    if (!pNode)
        return;
    switch(pNode->GetType())
    {
        case NTABLE:
            ExportTable(pNode,nLevel);
            break;
        case NALIGN:
        case NBRACEBODY:
        case NEXPRESSION:
            ExportExpression(pNode,nLevel);
            break;
        case NLINE:
            ExportLine(pNode,nLevel);
            break;
        case NTEXT:
            ExportText(pNode,nLevel);
            break;
        case NSPECIAL: //NSPECIAL requires some sort of Entity preservation in
                    //the XML engine.
        case NMATH:
            {
            //To fully handle generic MathML we need to implement the full
            //operator dictionary, we will generate MathML with explicit
            //stretchiness for now.
            sal_Int16 nLength = GetAttrList().getLength();
            sal_Bool bAddStretch=sal_True;
            for( sal_Int16 i = 0; i < nLength; i++ )
            {
                OUString sLocalName;
                sal_uInt16 nPrefix = GetNamespaceMap().GetKeyByAttrName(
                    GetAttrList().getNameByIndex(i), &sLocalName );

                if ( ( XML_NAMESPACE_MATH == nPrefix ) &&
                    IsXMLToken(sLocalName, XML_STRETCHY) )
                {
                    bAddStretch = sal_False;
                    break;
                }
            }
            if (bAddStretch)
            {
                AddAttribute(XML_NAMESPACE_MATH,sXML_stretchy,OUString(
                    RTL_CONSTASCII_USTRINGPARAM(sXML_false)));
            }
            ExportMath(pNode,nLevel);
            }
            break;
        case NBINHOR:
            ExportBinaryHorizontal(pNode,nLevel);
            break;
        case NUNHOR:
            ExportUnaryHorizontal(pNode,nLevel);
            break;
        case NBRACE:
            ExportBrace(pNode,nLevel);
            break;
        case NBINVER:
            ExportBinaryVertical(pNode,nLevel);
            break;
        case NSUBSUP:
            ExportSubSupScript(pNode,nLevel);
            break;
        case NROOT:
            ExportRoot(pNode,nLevel);
            break;
        case NOPER:
            ExportOperator(pNode,nLevel);
            break;
        case NATTRIBUT:
            ExportAttributes(pNode,nLevel);
            break;
        case NFONT:
            ExportFont(pNode,nLevel);
            break;
        case NVERTICAL_BRACE:
            ExportVerticalBrace(pNode,nLevel);
            break;
        case NMATRIX:
            ExportMatrix(pNode,nLevel);
            break;
#if 0
        default:
            {
            ULONG  nSize = pNode->GetNumSubNodes();
            for (ULONG i = 0; i < nSize; i++)
                if (SmNode *pTemp = pNode->GetSubNode(i))
                    ExportNodes(pTemp,nLevel+1);
            }
            break;
#endif
    }
}
