/*************************************************************************
 *
 *  $RCSfile: swxml.cxx,v $
 *
 *  $Revision: 1.36 $
 *
 *  last change: $Author: mib $ $Date: 2001-05-30 09:30:26 $
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
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _RSCSFX_HXX
#include <rsc/rscsfx.hxx>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_INPUTSOURCE_HPP_
#include <com/sun/star/xml/sax/InputSource.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XPARSER_HPP_
#include <com/sun/star/xml/sax/XParser.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XACTIVEDATACONTROL_HPP_
#include <com/sun/star/io/XActiveDataControl.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTRANGE_HPP_
#include <com/sun/star/text/XTextRange.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORFACTORY_HPP_
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>
#endif

#ifndef _SFXDOCFILE_HXX //autogen wg. SfxMedium
#include <sfx2/docfile.hxx>
#endif
#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif

#ifndef _SWSWERROR_H
#include <swerror.h>
#endif
#ifndef _ERRHDL_HXX //autogen wg. ASSERT
#include <errhdl.hxx>
#endif
#ifndef _FLTINI_HXX //autogen wg. XMLReader
#include <fltini.hxx>
#endif
#ifndef _DOC_HXX //autogen wg. SwDoc
#include <doc.hxx>
#endif
#ifndef _DOCSH_HXX //autogen wg. SwDoc
#include <docsh.hxx>
#endif
#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _XMLGRHLP_HXX
#include <svx/xmlgrhlp.hxx>
#endif
#ifndef _XMLEOHLP_HXX
#include <svx/xmleohlp.hxx>
#endif
#ifndef _COMPHELPER_GENERICPROPERTYSET_HXX_
#include <comphelper/genericpropertyset.hxx>
#endif

#ifndef _XMLIMP_HXX
#include "xmlimp.hxx"
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::lang;
using namespace ::rtl;

XMLReader::XMLReader()
{
}

int XMLReader::GetReaderType()
{
    return SW_STORAGE_READER | SW_STREAM_READER;
}

/// read a component (file + filter version)
sal_Int32 ReadThroughComponent(
    Reference<io::XInputStream> xInputStream,
    Reference<XComponent> xModelComponent,
    const String& rStreamName,
    Reference<lang::XMultiServiceFactory> & rFactory,
    const sal_Char* pFilterName,
    Sequence<Any> rFilterArguments,
    const OUString& rName,
    sal_Bool bMustBeSuccessfull,

    // parameters for special modes
    sal_Bool bBlockMode,
    Reference<XTextRange> & rInsertTextRange,
    sal_Bool bFormatsOnly,
    sal_uInt16 nStyleFamilyMask,
    sal_Bool bMergeStyles,
    sal_Bool bOrganizerMode )
{
    DBG_ASSERT(xInputStream.is(), "input stream missing");
    DBG_ASSERT(xModelComponent.is(), "document missing");
    DBG_ASSERT(rFactory.is(), "factory missing");
    DBG_ASSERT(NULL != pFilterName,"I need a service name for the component!");

    // prepare ParserInputSrouce
    xml::sax::InputSource aParserInput;
    aParserInput.sSystemId = rName;
    aParserInput.aInputStream = xInputStream; 

    // get parser
    Reference< xml::sax::XParser > xParser( 
        rFactory->createInstance(
            OUString::createFromAscii("com.sun.star.xml.sax.Parser") ),
        UNO_QUERY );
    DBG_ASSERT( xParser.is(), "Can't create parser" );
    if( !xParser.is() )
        return ERR_SWG_READ_ERROR;

    // get filter
    Reference< xml::sax::XDocumentHandler > xFilter(
        rFactory->createInstanceWithArguments(
            OUString::createFromAscii(pFilterName), rFilterArguments),
        UNO_QUERY );
    DBG_ASSERT( xFilter.is(), "Can't instantiate filter component." );
    if( !xFilter.is() )
        return ERR_SWG_READ_ERROR;

    // connect parser and filter
    xParser->setDocumentHandler( xFilter );

    // connect model and filter
    Reference < XImporter > xImporter( xFilter, UNO_QUERY );
    xImporter->setTargetDocument( xModelComponent );

    // prepare filter for special modes
    if( bBlockMode || bFormatsOnly || rInsertTextRange.is() || bOrganizerMode )
    {
        Reference<XUnoTunnel> xFilterTunnel( xFilter, UNO_QUERY );
        if (xFilterTunnel.is())
        {
            SwXMLImport* pFilter = (SwXMLImport *)xFilterTunnel->getSomething(
                SwXMLImport::getUnoTunnelId() );

            if ( NULL != pFilter )
            {
                // In formats only mode the reader's bInsertMode is set
                if ( bFormatsOnly )
                    pFilter->setStyleInsertMode( nStyleFamilyMask, 
                                                 !bMergeStyles );
                if ( rInsertTextRange.is() )
                    pFilter->setTextInsertMode( rInsertTextRange );

                if ( bBlockMode )
                    pFilter->setBlockMode();

                if ( bOrganizerMode )
                    pFilter->setOrganizerMode();
            }
        }
    }

    // finally, parser the stream
    try
    {
        xParser->parseStream( aParserInput );
    }
    catch( xml::sax::SAXParseException& r )
    {
#ifdef DEBUG
        ByteString aError( "SAX parse exception catched while importing:\n" );
        aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aError.GetBuffer() );
#endif

        String sErr( String::CreateFromInt32( r.LineNumber ));
        sErr += ',';
        sErr += String::CreateFromInt32( r.ColumnNumber );

        if( rStreamName.Len() )
        {
            return *new TwoStringErrorInfo(
                            (bMustBeSuccessfull ? ERR_FORMAT_FILE_ROWCOL
                                                    : WARN_FORMAT_FILE_ROWCOL),
                            rStreamName, sErr,
                            ERRCODE_BUTTON_OK | ERRCODE_MSG_ERROR );
        }
        else
        {
            ASSERT( bMustBeSuccessfull, "Warnings are not supported" );
            return *new StringErrorInfo( ERR_FORMAT_ROWCOL, sErr,
                             ERRCODE_BUTTON_OK | ERRCODE_MSG_ERROR );
        }
    }
    catch( xml::sax::SAXException& r )
    {
#ifdef DEBUG
        ByteString aError( "SAX exception catched while importing:\n" );
        aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aError.GetBuffer() );
#endif
        return ERR_SWG_READ_ERROR;
    }
    catch( io::IOException& r )
    {
#ifdef DEBUG
        ByteString aError( "IO exception catched while importing:\n" );
        aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aError.GetBuffer() );
#endif
        return ERR_SWG_READ_ERROR;
    }
    catch( uno::Exception& r )
    {
#ifdef DEBUG
        ByteString aError( "uno exception catched while importing:\n" );
        aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aError.GetBuffer() );
#endif
        return ERR_SWG_READ_ERROR;
    }

    // success!
    return 0;
}

/// read a component (storage version)
sal_Int32 ReadThroughComponent(
    SvStorage* pStorage,
    Reference<XComponent> xModelComponent,
    const sal_Char* pStreamName,
    const sal_Char* pCompatibilityStreamName,
    Reference<lang::XMultiServiceFactory> & rFactory,
    const sal_Char* pFilterName,
    Sequence<Any> rFilterArguments,
    const OUString& rName,
    sal_Bool bMustBeSuccessfull,

    // parameters for special modes
    sal_Bool bBlockMode,
    Reference<XTextRange> & rInsertTextRange,
    sal_Bool bFormatsOnly,
    sal_uInt16 nStyleFamilyMask,
    sal_Bool bMergeStyles,
    sal_Bool bOrganizerMode )
{
    DBG_ASSERT(NULL != pStorage, "Need storage!");
    DBG_ASSERT(NULL != pStreamName, "Please, please, give me a name!");

    // open stream (and set parser input)
    OUString sStreamName = OUString::createFromAscii(pStreamName);
    if (! pStorage->IsStream(sStreamName))
    {
        // stream name not found! Then try the compatibility name.
        // if no stream can be opened, return immediatly with OK signal

        // do we even have an alternative name?
        if ( NULL == pCompatibilityStreamName )
            return 0;

        // if so, does the stream exist?
        sStreamName = OUString::createFromAscii(pCompatibilityStreamName);
        if (! pStorage->IsStream(sStreamName) )
            return 0;
    }

    // get input stream
    SvStorageStreamRef xEventsStream;
    xEventsStream = pStorage->OpenStream( sStreamName, 
                                          STREAM_READ | STREAM_NOCREATE );
    xEventsStream->SetBufferSize( 16*1024 );
    Reference<io::XInputStream> xInputStream = 
        new utl::OInputStreamWrapper( *xEventsStream );

    // read from the stream
    return ReadThroughComponent(
        xInputStream, xModelComponent, sStreamName, rFactory,
        pFilterName, rFilterArguments,
        rName, bMustBeSuccessfull, bBlockMode, rInsertTextRange, bFormatsOnly, 
        nStyleFamilyMask, bMergeStyles, bOrganizerMode );
}


sal_uInt32 XMLReader::Read( SwDoc &rDoc, SwPaM &rPaM, const String & rName )
{
    // Get service factory
    Reference< lang::XMultiServiceFactory > xServiceFactory =
            comphelper::getProcessServiceFactory();
    ASSERT( xServiceFactory.is(),
            "XMLReader::Read: got no service manager" );
    if( !xServiceFactory.is() )
        return ERR_SWG_READ_ERROR;

    Reference< io::XActiveDataSource > xSource;
    Reference< XInterface > xPipe;
    Reference< document::XGraphicObjectResolver > xGraphicResolver;
    SvXMLGraphicHelper *pGraphicHelper = 0;
    Reference< document::XEmbeddedObjectResolver > xObjectResolver;
    SvXMLEmbeddedObjectHelper *pObjectHelper = 0;

    // get the input stream (storage or stream)
    SvStorageStreamRef xDocStream;
    Reference<io::XInputStream> xInputStream;
    SvStorage *pStorage = 0;
    if( pMedium )
        pStorage = pMedium->GetStorage();
    else
        pStorage = pStg;
    if( pStorage )
    {
        pGraphicHelper = SvXMLGraphicHelper::Create( *pStorage,
                                                     GRAPHICHELPER_MODE_READ,
                                                     sal_False );
        xGraphicResolver = pGraphicHelper;

    }
    else if( pMedium )
    {
        // if there is a medium and if this medium has a load environment,
        // we get an active data source from the medium.
        pMedium->GetInStream()->Seek( 0 );
        xSource = pMedium->GetDataSource();
        ASSERT( xSource.is(), "XMLReader:: got no data source from medium" );
        if( !xSource.is() )
            return ERR_SWG_READ_ERROR;

        // get a pipe for connecting the data source to the parser
        xPipe = xServiceFactory->createInstance(
                OUString::createFromAscii("com.sun.star.io.Pipe") );
        ASSERT( xPipe.is(),
                "XMLReader::Read: com.sun.star.io.Pipe service missing" );
        if( !xPipe.is() )
            return ERR_SWG_READ_ERROR;

        // connect pipe's output stream to the data source
        Reference< io::XOutputStream > xPipeOutput( xPipe, UNO_QUERY );
        xSource->setOutputStream( xPipeOutput );

        xInputStream = Reference< io::XInputStream >( xPipe, UNO_QUERY );
    }
    else
    {
        pStrm->SetBufferSize( 16*1024 );
        xInputStream = new utl::OInputStreamWrapper( *pStrm );
    }
    SvPersist *pPersist = rDoc.GetPersist();
    if( pPersist )
    {
        if( pStorage )
            pObjectHelper = SvXMLEmbeddedObjectHelper::Create(
                                        *pStorage, *pPersist,
                                        EMBEDDEDOBJECTHELPER_MODE_READ,
                                        sal_False );
#if SUPD > 632
        else
            pObjectHelper = SvXMLEmbeddedObjectHelper::Create(
                                        *pPersist,
                                        EMBEDDEDOBJECTHELPER_MODE_READ );
#endif

        xObjectResolver = pObjectHelper;
    }

    // Get the docshell, the model, and finally the model's component
    SwDocShell *pDocSh = rDoc.GetDocShell();
    ASSERT( pDocSh, "XMLReader::Read: got no doc shell" );
    if( !pDocSh )
        return ERR_SWG_READ_ERROR;
    Reference< lang::XComponent > xModelComp( pDocSh->GetModel(), UNO_QUERY );
    ASSERT( xModelComp.is(),
            "XMLReader::Read: got no model" );
    if( !xModelComp.is() )
        return ERR_SWG_READ_ERROR;


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
        { "NumberStyles", sizeof("NumberStyles")-1, 0,
              &::getCppuType( (Reference<container::XNameContainer> *) 0),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "RecordChanges", sizeof("RecordChanges")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "ShowChanges", sizeof("ShowChanges")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "RedlineProtectionKey", sizeof("RedlineProtectionKey")-1, 0,
#if (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)) || (defined(__GNUC__) && defined(__APPLE__))
              new uno::Type(::getCppuType((Sequence<sal_Int8>*)0)),
#else
              &::getCppuType((Sequence<sal_Int8>*)0),
#endif
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { NULL, 0, 0, NULL, 0, 0 }
    };
    uno::Reference< beans::XPropertySet > xInfoSet( 
                comphelper::GenericPropertySet_CreateInstance( 
                            new comphelper::PropertySetInfo( aInfoMap ) ) );

    // create XStatusIndicator
    uno::Reference<task::XStatusIndicator> xStatusIndicator;
    if ( !SW_MOD()->IsEmbeddedLoadSave() )
    {
        uno::Reference<frame::XModel> xModel( pDocSh->GetModel() );
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
                        xStatusIndicator = xFactory->createStatusIndicator();
                    }
                }
            }
        }
    }

    // set progress range and start status indicator
    sal_Int32 nProgressRange(1000000);
    if (xStatusIndicator.is())
    {
        OUString sLoading(RTL_CONSTASCII_USTRINGPARAM("Loading ..."));
        xStatusIndicator->start(sLoading, nProgressRange);
    }
    uno::Any aProgRange;
    aProgRange <<= nProgressRange;
    OUString sProgressRange(RTL_CONSTASCII_USTRINGPARAM("ProgressRange"));
    xInfoSet->setPropertyValue(sProgressRange, aProgRange);

    // prepare filter arguments
    Sequence<Any> aFilterArgs( 5 );
    Any *pArgs = aFilterArgs.getArray();
    *pArgs++ <<= xGraphicResolver;
    *pArgs++ <<= xObjectResolver;
    *pArgs++ <<= xStatusIndicator;
    *pArgs++ <<= xInfoSet;
    Sequence<Any> aEmptyArgs( 3 );
    pArgs = aEmptyArgs.getArray();
    *pArgs++ <<= xStatusIndicator;
    *pArgs++ <<= xInfoSet;

    // prepare for special modes
    sal_uInt16 nStyleFamilyMask = 0U;
    Reference<XTextRange> xInsertTextRange = NULL;
    if( aOpt.IsFmtsOnly() )
    {
        if( aOpt.IsFrmFmts() )
            nStyleFamilyMask |= SFX_STYLE_FAMILY_FRAME;
        if( aOpt.IsPageDescs() )
            nStyleFamilyMask |= SFX_STYLE_FAMILY_PAGE;
        if( aOpt.IsTxtFmts() )
            nStyleFamilyMask |= (SFX_STYLE_FAMILY_CHAR|SFX_STYLE_FAMILY_PARA);
        if( aOpt.IsNumRules() )
            nStyleFamilyMask |= SFX_STYLE_FAMILY_PSEUDO;
    }
    else if( bInsertMode )
    {
        xInsertTextRange = SwXTextRange::CreateTextRangeFromPosition( 
            &rDoc, *rPaM.GetPoint(), 0 );
    }

    rDoc.AddLink();	// prevent deletion
    sal_uInt32 nRet = 0;

    // save redline mode into import info property set
    Any aAny;
    sal_Bool bTmp;
    OUString sShowChanges( RTL_CONSTASCII_USTRINGPARAM("ShowChanges") );
    bTmp = IsShowChanges( rDoc.GetRedlineMode() );
    aAny.setValue( &bTmp, ::getBooleanCppuType() );
    xInfoSet->setPropertyValue( sShowChanges, aAny );
    OUString sRecordChanges( RTL_CONSTASCII_USTRINGPARAM("RecordChanges") );
    bTmp = IsRedlineOn(rDoc.GetRedlineMode());
    aAny.setValue( &bTmp, ::getBooleanCppuType() );
    xInfoSet->setPropertyValue( sRecordChanges, aAny );
    OUString sRedlineProtectionKey( RTL_CONSTASCII_USTRINGPARAM("RedlineProtectionKey") );
    aAny <<= rDoc.GetRedlinePasswd();
    xInfoSet->setPropertyValue( sRedlineProtectionKey, aAny );

    // force redline mode to "none"
    rDoc.SetRedlineMode_intern( REDLINE_NONE );


    if ( NULL != pStorage )
    {
        sal_uInt32 nWarn = 0;
        sal_uInt32 nWarn2 = 0;
        // read storage streams
        if( !(IsOrganizerMode() || IsBlockMode() || aOpt.IsFmtsOnly() ||
              bInsertMode) )
        {
            nWarn = ReadThroughComponent(
                pStorage, xModelComp, "meta.xml", "Meta.xml", xServiceFactory,
                "com.sun.star.comp.Writer.XMLMetaImporter",
                aEmptyArgs, rName, sal_False, IsBlockMode(), xInsertTextRange,
                aOpt.IsFmtsOnly(), nStyleFamilyMask, aOpt.IsMerge(),
                sal_False );

            nWarn2 = ReadThroughComponent(
                pStorage, xModelComp, "settings.xml", NULL, xServiceFactory,
                "com.sun.star.comp.Writer.XMLSettingsImporter",
                aFilterArgs, rName, sal_False, IsBlockMode(), xInsertTextRange,
                aOpt.IsFmtsOnly(), nStyleFamilyMask, aOpt.IsMerge(),
                IsOrganizerMode() );
        }

        nRet = ReadThroughComponent(
            pStorage, xModelComp, "styles.xml", NULL, xServiceFactory,
            "com.sun.star.comp.Writer.XMLStylesImporter",
            aFilterArgs, rName, sal_True, IsBlockMode(), xInsertTextRange,
            aOpt.IsFmtsOnly(), nStyleFamilyMask, aOpt.IsMerge(),
            IsOrganizerMode() );

        if( !nRet && !(IsOrganizerMode() || aOpt.IsFmtsOnly()) )
            nRet = ReadThroughComponent(
               pStorage, xModelComp, "content.xml", "Content.xml", xServiceFactory,
               "com.sun.star.comp.Writer.XMLContentImporter",
               aFilterArgs, rName, sal_True, IsBlockMode(), xInsertTextRange,
               aOpt.IsFmtsOnly(), nStyleFamilyMask, aOpt.IsMerge(),
               sal_False );

        if( !(IsOrganizerMode() || IsBlockMode() || bInsertMode ||
              aOpt.IsFmtsOnly() ) )
        {
            OUString sStreamName( RTL_CONSTASCII_USTRINGPARAM("layout-cache") );
            SvStorageStreamRef xStrm = pStorage->OpenStream( sStreamName, 
                                          STREAM_READ | STREAM_NOCREATE );
            if( xStrm.Is() && !xStrm->GetError() )
            {
                xStrm->SetBufferSize( 16*1024 );
                rDoc.ReadLayoutCache( *xStrm );
            }
        }
        if( !nRet )
        {
            if( nWarn )
                nRet = nWarn;
            else if( nWarn2 )
                nRet = nWarn2;
        }
    }
    else
    {
        // read plain file
        
        // parse    
        if( xSource.is() )
        {
            Reference< io::XActiveDataControl > xSourceControl( xSource, 
                                                                UNO_QUERY );
            xSourceControl->start();
        }

        nRet = ReadThroughComponent(
            xInputStream, xModelComp, aEmptyStr, xServiceFactory,
            "com.sun.star.comp.Writer.XMLImporter",
            aFilterArgs, rName, sal_True, IsBlockMode(), xInsertTextRange,
            aOpt.IsFmtsOnly(), nStyleFamilyMask, aOpt.IsMerge(),
            IsOrganizerMode() );
    }

    aOpt.ResetAllFmtsOnly();	

    // redline password
    aAny = xInfoSet->getPropertyValue( sRedlineProtectionKey );
    Sequence<sal_Int8> aKey;
    aAny >>= aKey;
    rDoc.SetRedlinePasswd( aKey );

    // restore redline mode from import info property set
    sal_Int16 nRedlineMode = REDLINE_SHOW_INSERT;
    aAny = xInfoSet->getPropertyValue( sShowChanges );
    if ( *(sal_Bool*)aAny.getValue() )
        nRedlineMode |= REDLINE_SHOW_DELETE;
    aAny = xInfoSet->getPropertyValue( sRecordChanges );
    if ( *(sal_Bool*)aAny.getValue() || (aKey.getLength() > 0) )
        nRedlineMode |= REDLINE_ON;
    else
        nRedlineMode |= REDLINE_NONE;

    // ... restore redline mode
    // (First set bogus mode to make sure the mode in SetRedlineMode()
    //  is different from it's previous mode.)
    rDoc.SetRedlineMode_intern( ~nRedlineMode );
    rDoc.SetRedlineMode( nRedlineMode );


    if( pGraphicHelper )
        SvXMLGraphicHelper::Destroy( pGraphicHelper );
    xGraphicResolver = 0;
    if( pObjectHelper )
        SvXMLEmbeddedObjectHelper::Destroy( pObjectHelper );
    xObjectResolver = 0;
    rDoc.RemoveLink();

    if (xStatusIndicator.is())
    {
        xStatusIndicator->end();
    }

    return nRet;
}
