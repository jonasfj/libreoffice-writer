/*************************************************************************
 *
 *  $RCSfile: wrtxml.cxx,v $
 *
 *  $Revision: 1.47 $
 *
 *  last change: $Author: obo $ $Date: 2005-01-05 11:48:04 $
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


#pragma hdrstop

#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORFACTORY_HPP_
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XACTIVEDATASOURCE_HPP_
#include <com/sun/star/io/XActiveDataSource.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEXPORTER_HPP_
#include <com/sun/star/document/XExporter.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XFILTER_HPP_
#include <com/sun/star/document/XFilter.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COMPHELPER_GENERICPROPERTYSET_HXX_
#include <comphelper/genericpropertyset.hxx>
#endif
#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif
#ifndef _XMLGRHLP_HXX
#include <svx/xmlgrhlp.hxx>
#endif
#ifndef _XMLEOHLP_HXX
#include <svx/xmleohlp.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_SAVEOPT_HXX
#include <svtools/saveopt.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
#endif

#ifndef _SFXDOCFILE_HXX //autogen wg. SfxMedium
#include <sfx2/docfile.hxx>
#endif
#ifndef _PAM_HXX //autogen wg. SwPaM
#include <pam.hxx>
#endif
#ifndef _DOC_HXX //autogen wg. SwDoc
#include <doc.hxx>
#endif
#ifndef _DOCSTAT_HXX //autogen wg. SwDoc
#include <docstat.hxx>
#endif
#ifndef _DOCSH_HXX //autogen wg. SwDoc
#include <docsh.hxx>
#endif

#include <unotools/ucbstreamhelper.hxx>

#ifndef _ERRHDL_HXX //autogen wg. ASSERT
#include <errhdl.hxx>
#endif
#ifndef _SWSWERROR_H
#include <swerror.h>
#endif
#ifndef _WRTXML_HXX
#include <wrtxml.hxx>
#endif
#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif
#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif


using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;

#define LOGFILE_AUTHOR "mb93740"

SwXMLWriter::SwXMLWriter()
{
}


__EXPORT SwXMLWriter::~SwXMLWriter()
{
}


sal_uInt32 SwXMLWriter::_Write()
{
    // Get service factory
    Reference< lang::XMultiServiceFactory > xServiceFactory =
            comphelper::getProcessServiceFactory();
    ASSERT( xServiceFactory.is(),
            "SwXMLWriter::Write: got no service manager" );
    if( !xServiceFactory.is() )
        return ERR_SWG_WRITE_ERROR;

    // Get data sink ...
    Reference< io::XOutputStream > xOut;
    SvStorageStreamRef xDocStream;
    Reference< document::XGraphicObjectResolver > xGraphicResolver;
    SvXMLGraphicHelper *pGraphicHelper = 0;
    Reference< document::XEmbeddedObjectResolver > xObjectResolver;
    SvXMLEmbeddedObjectHelper *pObjectHelper = 0;

    ASSERT( xStg.is(), "Where is my storage?" );
    pGraphicHelper = SvXMLGraphicHelper::Create( xStg,
                                                 GRAPHICHELPER_MODE_WRITE,
                                                 sal_False );
    xGraphicResolver = pGraphicHelper;

    SfxObjectShell *pPersist = pDoc->GetPersist();
    if( pPersist )
    {
        pObjectHelper = SvXMLEmbeddedObjectHelper::Create(
                                         xStg, *pPersist,
                                         EMBEDDEDOBJECTHELPER_MODE_WRITE,
                                         sal_False );
        xObjectResolver = pObjectHelper;
    }

    // create and prepare the XPropertySet that gets passed through
    // the components, and the XStatusIndicator that shows progress to
    // the user.

    // create XPropertySet with three properties for status indicator
    comphelper::PropertyMapEntry aInfoMap[] =
    {
        { "ProgressRange", sizeof("ProgressRange")-1, 0,
              &::getCppuType((sal_Int32*)0),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "ProgressMax", sizeof("ProgressMax")-1, 0,
              &::getCppuType((sal_Int32*)0),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "ProgressCurrent", sizeof("ProgressCurrent")-1, 0,
              &::getCppuType((sal_Int32*)0),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "WrittenNumberStyles", sizeof("WrittenNumberStyles")-1, 0,
              &::getCppuType((uno::Sequence<sal_Int32> *)0),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "UsePrettyPrinting", sizeof("UsePrettyPrinting")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "ShowChanges", sizeof("ShowChanges")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "RedlineProtectionKey", sizeof("RedlineProtectionKey")-1, 0,
#if (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500))
              new uno::Type(::getCppuType((Sequence<sal_Int8>*)0)),
#else
              &::getCppuType((Sequence<sal_Int8>*)0),
#endif
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
        { "AutoTextMode", sizeof("AutoTextMode")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "StyleNames", sizeof("StyleNames")-1, 0,
              &::getCppuType( (Sequence<OUString>*)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "StyleFamilies", sizeof("StyleFamilies")-1, 0,
              &::getCppuType( (Sequence<sal_Int32>*)0 ),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { NULL, 0, 0, NULL, 0, 0 }
    };
    uno::Reference< beans::XPropertySet > xInfoSet(
                comphelper::GenericPropertySet_CreateInstance(
                            new comphelper::PropertySetInfo( aInfoMap ) ) );

    // create XStatusIndicator
    uno::Reference<task::XStatusIndicator> xStatusIndicator;

    uno::Any aAny;
    if (bShowProgress)
    {
        try
        {
            uno::Reference<frame::XModel> xModel( pDoc->GetDocShell()->GetModel());
            if (xModel.is())
            {
                uno::Reference<frame::XController> xController(
                    xModel->getCurrentController());
                if( xController.is())
                {
                    uno::Reference<frame::XFrame> xFrame( xController->getFrame());
                    if( xFrame.is())
                    {
                        uno::Reference<task::XStatusIndicatorFactory> xFactory(
                            xFrame, uno::UNO_QUERY );
                        if( xFactory.is())
                        {
                            xStatusIndicator =
                                xFactory->createStatusIndicator();
                        }
                    }
                }
            }
        }
        catch( const RuntimeException& )
        {
            xStatusIndicator = 0;
        }

        // set progress range and start status indicator
        sal_Int32 nProgressRange(1000000);
        if (xStatusIndicator.is())
        {
            xStatusIndicator->start(SW_RESSTR( STR_STATSTR_SWGWRITE),
                                    nProgressRange);
        }
        aAny <<= nProgressRange;
        OUString sProgressRange(RTL_CONSTASCII_USTRINGPARAM("ProgressRange"));
        xInfoSet->setPropertyValue(sProgressRange, aAny);

        aAny <<= static_cast < sal_Int32 >( -1 );
        OUString sProgressMax(RTL_CONSTASCII_USTRINGPARAM("ProgressMax"));
        xInfoSet->setPropertyValue(sProgressMax, aAny);
    }
    SvtSaveOptions aSaveOpt;
    OUString sUsePrettyPrinting(RTL_CONSTASCII_USTRINGPARAM("UsePrettyPrinting"));
    sal_Bool bUsePrettyPrinting( aSaveOpt.IsPrettyPrinting() );
    aAny.setValue( &bUsePrettyPrinting, ::getBooleanCppuType() );
    xInfoSet->setPropertyValue( sUsePrettyPrinting, aAny );

    // save show redline mode ...
    OUString sShowChanges(RTL_CONSTASCII_USTRINGPARAM("ShowChanges"));
    sal_uInt16 nRedlineMode = pDoc->GetRedlineMode();
    sal_Bool bShowChanges( IsShowChanges( nRedlineMode ) );
    aAny.setValue( &bShowChanges, ::getBooleanCppuType() );
    xInfoSet->setPropertyValue( sShowChanges, aAny );
    // ... and hide redlines for export
    nRedlineMode &= ~REDLINE_SHOW_MASK;
    nRedlineMode |= REDLINE_SHOW_INSERT;
    pDoc->SetRedlineMode( nRedlineMode );
    
    // TODO/LATER : either target medium or mediadescriptor for the current storing must be accessible here
    SfxMedium* pMedDescrMedium = NULL;

    // Set base URI
    OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("BaseURI") );
    ::rtl::OUString aBaseURL;
    sal_Bool bBaseURLSet = sal_False;
    if ( pMedDescrMedium && pMedDescrMedium->GetItemSet() )
    {
        const SfxStringItem* pBaseURLItem = static_cast<const SfxStringItem*>(
                   pMedDescrMedium->GetItemSet()->GetItem(SID_DOC_BASEURL) );
        if ( pBaseURLItem )
        {
            aBaseURL = pBaseURLItem->GetValue();
            bBaseURLSet = sal_True;
        }
    }
    xInfoSet->setPropertyValue( sPropName, makeAny( aBaseURL ) );

    // TODO/LATER: separate links from normal embedded objects
    if( SFX_CREATE_MODE_EMBEDDED == pDoc->GetDocShell()->GetCreateMode() )
    {
        OUString aName;
        if ( pMedDescrMedium && pMedDescrMedium->GetItemSet() )
        {
            const SfxStringItem* pDocHierarchItem = static_cast<const SfxStringItem*>(
                pMedDescrMedium->GetItemSet()->GetItem(SID_DOC_HIERARCHICALNAME) );
            if ( pDocHierarchItem )
                aName = pDocHierarchItem->GetValue();
        }

        if( aName.getLength() )
        {
            sPropName = OUString(RTL_CONSTASCII_USTRINGPARAM("StreamRelPath")); 
            xInfoSet->setPropertyValue( sPropName, makeAny( aName ) ); 
        }
    }
    else if ( !bBaseURLSet )
        xInfoSet->setPropertyValue( sPropName, makeAny( ::rtl::OUString( INetURLObject::GetBaseURL() ) ) );

    if( bBlock )
    {
        OUString sAutoTextMode(
                RTL_CONSTASCII_USTRINGPARAM("AutoTextMode"));
        sal_Bool bTmp = sal_True;
        Any aAny;
        aAny.setValue( &bTmp, ::getBooleanCppuType() );
        xInfoSet->setPropertyValue( sAutoTextMode, aAny );
    }


    // filter arguments
    // - graphics + object resolver for styles + content
    // - status indicator
    // - info property set
    // - else empty
    sal_Int32 nArgs = 1;
    if( xStatusIndicator.is() )
        nArgs++;

    Sequence < Any > aEmptyArgs( nArgs );
    Any *pArgs = aEmptyArgs.getArray();
    *pArgs++ <<= xInfoSet;
    if( xStatusIndicator.is() )
        *pArgs++ <<= xStatusIndicator;

    if( xGraphicResolver.is() )
        nArgs++;
    if( xObjectResolver.is() )
        nArgs++;

    Sequence < Any > aFilterArgs( nArgs );
    pArgs = aFilterArgs.getArray();
    *pArgs++ <<= xInfoSet;
    if( xGraphicResolver.is() )
        *pArgs++ <<= xGraphicResolver;
    if( xObjectResolver.is() )
        *pArgs++ <<= xObjectResolver;
    if( xStatusIndicator.is() )
        *pArgs++ <<= xStatusIndicator;

    //Get model
    Reference< lang::XComponent > xModelComp(
        pDoc->GetDocShell()->GetModel(), UNO_QUERY );
    ASSERT( xModelComp.is(), "XMLWriter::Write: got no model" );
    if( !xModelComp.is() )
        return ERR_SWG_WRITE_ERROR;

    PutNumFmtFontsInAttrPool();
    PutEditEngFontsInAttrPool();

    // properties
    Sequence < PropertyValue > aProps( pOrigFileName ? 1 : 0 );
    if( pOrigFileName )
    {
        PropertyValue *pProps = aProps.getArray();
        pProps->Name = OUString( RTL_CONSTASCII_USTRINGPARAM("FileName") );
        (pProps++)->Value <<= OUString( *pOrigFileName  );
    }

    // export sub streams for package, else full stream into a file
    sal_Bool bWarn = sal_False, bErr = sal_False;
    String sWarnFile, sErrFile;
    sal_Bool bOASIS = ( SotStorage::GetVersion( xStg ) > SOFFICE_FILEFORMAT_60 );

    if( !bOrganizerMode && !bBlock &&
        SFX_CREATE_MODE_EMBEDDED != pDoc->GetDocShell()->GetCreateMode() )
    {
        if( !WriteThroughComponent(
                xModelComp, "meta.xml", xServiceFactory,
                (bOASIS ? "com.sun.star.comp.Writer.XMLOasisMetaExporter"
                        : "com.sun.star.comp.Writer.XMLMetaExporter"),
                aEmptyArgs, aProps, sal_True ) )
        {
            bWarn = sal_True;
            sWarnFile = String( RTL_CONSTASCII_STRINGPARAM("meta.xml"),
                                RTL_TEXTENCODING_ASCII_US );
        }
    }

    if( !bErr )
    {
        if( !bBlock )
        {
            if( !WriteThroughComponent(
                xModelComp, "settings.xml", xServiceFactory,
                (bOASIS ? "com.sun.star.comp.Writer.XMLOasisSettingsExporter"
                        : "com.sun.star.comp.Writer.XMLSettingsExporter"),
                aEmptyArgs, aProps, sal_False ) )
            {
                if( !bWarn )
                {
                    bWarn = sal_True;
                    sWarnFile = String( RTL_CONSTASCII_STRINGPARAM("settings.xml"),
                                        RTL_TEXTENCODING_ASCII_US );
                }
            }
        }
    }

    if( !WriteThroughComponent(
            xModelComp, "styles.xml", xServiceFactory,
            (bOASIS ? "com.sun.star.comp.Writer.XMLOasisStylesExporter"
                    : "com.sun.star.comp.Writer.XMLStylesExporter"),
            aFilterArgs, aProps, sal_False ) )
    {
        bErr = sal_True;
        sErrFile = String( RTL_CONSTASCII_STRINGPARAM("styles.xml"),
                           RTL_TEXTENCODING_ASCII_US );
    }


    if( !bOrganizerMode && !bErr )
    {
        if( !WriteThroughComponent(
                xModelComp, "content.xml", xServiceFactory,
                (bOASIS ? "com.sun.star.comp.Writer.XMLOasisContentExporter"
                        : "com.sun.star.comp.Writer.XMLContentExporter"),
                aFilterArgs, aProps, sal_False ) )
        {
            bErr = sal_True;
            sErrFile = String( RTL_CONSTASCII_STRINGPARAM("content.xml"),
                               RTL_TEXTENCODING_ASCII_US );
        }
    }

    if( pDoc->GetRootFrm() && pDoc->GetDocStat().nPage > 1 &&
        !(bOrganizerMode || bBlock || bErr) )
    {
//			DBG_ASSERT( !pDoc->GetDocStat().bModified,
//						"doc stat is modified!" );
        OUString sStreamName( RTL_CONSTASCII_USTRINGPARAM("layout-cache") );
        try
        {
            Reference < io::XStream > xStm = xStg->openStreamElement( sStreamName, embed::ElementModes::READWRITE | embed::ElementModes::TRUNCATE );
            SvStream* pStrm = utl::UcbStreamHelper::CreateStream( xStm );
            if( !pStrm->GetError() )
            {
                Reference < beans::XPropertySet > xSet( xStm, UNO_QUERY );
                String aPropName( String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM("MediaType") ) );
                OUString aMime( RTL_CONSTASCII_USTRINGPARAM("application/binary") );
                uno::Any aAny;
                aAny <<= aMime;
                xSet->setPropertyValue( aPropName, aAny );
                pDoc->WriteLayoutCache( *pStrm );
            }

            delete pStrm;
        }
        catch ( uno::Exception& )
        {
        }
    }

    if( pGraphicHelper )
        SvXMLGraphicHelper::Destroy( pGraphicHelper );
    xGraphicResolver = 0;

    if( pObjectHelper )
        SvXMLEmbeddedObjectHelper::Destroy( pObjectHelper );
    xObjectResolver = 0;

    // restore redline mode
    aAny = xInfoSet->getPropertyValue( sShowChanges );
    nRedlineMode = pDoc->GetRedlineMode();
    nRedlineMode &= ~REDLINE_SHOW_MASK;
    nRedlineMode |= REDLINE_SHOW_INSERT;
    if ( *(sal_Bool*)aAny.getValue() )
        nRedlineMode |= REDLINE_SHOW_DELETE;
    pDoc->SetRedlineMode( nRedlineMode );

    if (xStatusIndicator.is())
    {
        xStatusIndicator->end();
    }

    if( bErr )
    {
        if( sErrFile.Len() )
            return *new StringErrorInfo( ERR_WRITE_ERROR_FILE, sErrFile,
                                         ERRCODE_BUTTON_OK | ERRCODE_MSG_ERROR );
        else
            return ERR_SWG_WRITE_ERROR;
    }
    else if( bWarn )
    {
        if( sWarnFile.Len() )
            return *new StringErrorInfo( WARN_WRITE_ERROR_FILE, sWarnFile,
                                         ERRCODE_BUTTON_OK | ERRCODE_MSG_ERROR );
        else
            return WARN_SWG_FEATURES_LOST;
    }

    return 0;
}

sal_uInt32 SwXMLWriter::WriteStorage()
{
    return _Write();
}

sal_uInt32 SwXMLWriter::Write( SwPaM& rPaM, SfxMedium& rMed,
                               const String* pFileName )
{
    return IsStgWriter()
            ? ((StgWriter *)this)->Write( rPaM, rMed.GetOutputStorage(), pFileName )
            : ((Writer *)this)->Write( rPaM, *rMed.GetOutStream(), pFileName );
}

sal_Bool SwXMLWriter::WriteThroughComponent(
    const Reference<XComponent> & xComponent,
    const sal_Char* pStreamName,
    const Reference<lang::XMultiServiceFactory> & rFactory,
    const sal_Char* pServiceName,
    const Sequence<Any> & rArguments,
    const Sequence<beans::PropertyValue> & rMediaDesc,
    sal_Bool bPlainStream )
{
    DBG_ASSERT( xStg.is(), "Need storage!" );
    DBG_ASSERT( NULL != pStreamName, "Need stream name!" );
    DBG_ASSERT( NULL != pServiceName, "Need service name!" );

    RTL_LOGFILE_TRACE_AUTHOR1( "sw", LOGFILE_AUTHOR,
                               "SwXMLWriter::WriteThroughComponent : stream %s",
                               pStreamName );

    // open stream
    sal_Bool bRet = sal_False;
    try
    {
        OUString sStreamName = OUString::createFromAscii( pStreamName );
        uno::Reference<io::XStream> xStream =
                xStg->openStreamElement( sStreamName,
                embed::ElementModes::READWRITE | embed::ElementModes::TRUNCATE );

        Reference <beans::XPropertySet > xSet( xStream, uno::UNO_QUERY );
        if( !xSet.is() )
            return sal_False;

        String aPropName( String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM("MediaType") ) );
        OUString aMime( RTL_CONSTASCII_USTRINGPARAM("text/xml") );
        uno::Any aAny;
        aAny <<= aMime;
        xSet->setPropertyValue( aPropName, aAny );

        if( bPlainStream )
        {
            OUString aPropName( RTL_CONSTASCII_USTRINGPARAM("Compressed") );
            sal_Bool bFalse = sal_False;
            aAny.setValue( &bFalse, ::getBooleanCppuType() );
            xSet->setPropertyValue( aPropName, aAny );
        }
        else
        {
//REMOVE	            OUString aPropName( RTL_CONSTASCII_USTRINGPARAM("Encrypted") );
            OUString aPropName( RTL_CONSTASCII_USTRINGPARAM("UseCommonStoragePasswordEncryption") );
            sal_Bool bTrue = sal_True;
            aAny.setValue( &bTrue, ::getBooleanCppuType() );
            xSet->setPropertyValue( aPropName, aAny );
        }

        // set buffer and create outputstream
        Reference< io::XOutputStream > xOutputStream = xStream->getOutputStream();

        // set Base URL
        uno::Reference< beans::XPropertySet > xInfoSet;
        if( rArguments.getLength() > 0 )
            rArguments.getConstArray()[0] >>= xInfoSet;
        DBG_ASSERT( xInfoSet.is(), "missing property set" );
        if( xInfoSet.is() )
        {
            OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("StreamName") );
            xInfoSet->setPropertyValue( sPropName, makeAny( sStreamName ) );
        }

        // write the stuff
        bRet = WriteThroughComponent(
            xOutputStream, xComponent, rFactory,
            pServiceName, rArguments, rMediaDesc );
    }
    catch ( uno::Exception& )
    {
    }

    return bRet;

}

sal_Bool SwXMLWriter::WriteThroughComponent(
    const Reference<io::XOutputStream> & xOutputStream,
    const Reference<XComponent> & xComponent,
    const Reference<XMultiServiceFactory> & rFactory,
    const sal_Char* pServiceName,
    const Sequence<Any> & rArguments,
    const Sequence<PropertyValue> & rMediaDesc )
{
    ASSERT( xOutputStream.is(), "I really need an output stream!" );
    ASSERT( xComponent.is(), "Need component!" );
    ASSERT( NULL != pServiceName, "Need component name!" );

    RTL_LOGFILE_CONTEXT_AUTHOR( aFilterLog, "sw", LOGFILE_AUTHOR,
                                "SwXMLWriter::WriteThroughComponent" );

    // get component
    Reference< io::XActiveDataSource > xSaxWriter(
        rFactory->createInstance(
            String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM(
                "com.sun.star.xml.sax.Writer")) ),
        UNO_QUERY );
    ASSERT( xSaxWriter.is(), "can't instantiate XML writer" );
    if(!xSaxWriter.is())
        return sal_False;

    RTL_LOGFILE_CONTEXT_TRACE( aFilterLog, "SAX-Writer created" );

    // connect XML writer to output stream
    xSaxWriter->setOutputStream( xOutputStream );

    // prepare arguments (prepend doc handler to given arguments)
    Reference<xml::sax::XDocumentHandler> xDocHandler( xSaxWriter,UNO_QUERY);
    Sequence<Any> aArgs( 1 + rArguments.getLength() );
    aArgs[0] <<= xDocHandler;
    for(sal_Int32 i = 0; i < rArguments.getLength(); i++)
        aArgs[i+1] = rArguments[i];

    // get filter component
    Reference< document::XExporter > xExporter(
        rFactory->createInstanceWithArguments(
            OUString::createFromAscii(pServiceName), aArgs), UNO_QUERY);
    ASSERT( xExporter.is(),
            "can't instantiate export filter component" );
    if( !xExporter.is() )
        return sal_False;
    RTL_LOGFILE_CONTEXT_TRACE1( aFilterLog, "%s instantiated.", pServiceName );

    // connect model and filter
    xExporter->setSourceDocument( xComponent );

    // filter!
    RTL_LOGFILE_CONTEXT_TRACE( aFilterLog, "call filter()" );
    Reference<XFilter> xFilter( xExporter, UNO_QUERY );
    return xFilter->filter( rMediaDesc );
}


// -----------------------------------------------------------------------

void GetXMLWriter( const String& rName, WriterRef& xRet )
{
    xRet = new SwXMLWriter();
}

// -----------------------------------------------------------------------
