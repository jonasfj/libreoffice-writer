/*************************************************************************
 *
 *  $RCSfile: swxml.cxx,v $
 *
 *  $Revision: 1.60 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-11 12:37:05 $
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


#define _SVSTDARR_STRINGS

#ifndef _RSCSFX_HXX
#include <rsc/rscsfx.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
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
#ifndef _COM_SUN_STAR_IO_XACTIVEDATASOURCE_HPP_
#include <com/sun/star/io/XActiveDataSource.hpp>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPIOEXCEPTION_HPP_
#include <com/sun/star/packages/zip/ZipIOException.hpp>
#endif
#include <svtools/svstdarr.hxx>

#ifndef _SFXDOCFILE_HXX //autogen wg. SfxMedium
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFXECODE_HXX
#include <svtools/sfxecode.hxx>
#endif
#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
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
#ifndef _COMPHELPER_GENERICPROPERTYSET_HXX_
#include <comphelper/genericpropertyset.hxx>
#endif
#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#include <sfx2/frame.hxx>
#include <unotools/ucbstreamhelper.hxx>

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
#ifndef _SW_XMLSECTIONLIST_HXX
#include <SwXMLSectionList.hxx>
#endif

#ifndef _STATSTR_HRC
#include <statstr.hrc>
#endif

#define LOGFILE_AUTHOR "mb93740"


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::lang;
using namespace ::rtl;


void lcl_EnsureValidPam( SwPaM& rPam )
{
    if( rPam.GetCntntNode() != NULL )
    {
        // set proper point content
        if( rPam.GetCntntNode() != rPam.GetPoint()->nContent.GetIdxReg() )
        {
            rPam.GetPoint()->nContent.Assign( rPam.GetCntntNode(), 0 );
        }
        // else: point was already valid

        // if mark is invalid, we delete it
        if( ( rPam.GetCntntNode( FALSE ) == NULL ) ||
            ( rPam.GetCntntNode( FALSE ) != rPam.GetMark()->nContent.GetIdxReg() ) )
        {
            rPam.DeleteMark();
        }
    }
    else
    {
        // point is not valid, so move it into the first content
        rPam.DeleteMark();
        rPam.GetPoint()->nNode =
            *rPam.GetDoc()->GetNodes().GetEndOfContent().StartOfSectionNode();
        ++ rPam.GetPoint()->nNode;
        rPam.Move( fnMoveForward, fnGoCntnt ); // go into content
    }
}

XMLReader::XMLReader()
{
}

int XMLReader::GetReaderType()
{
    return SW_STORAGE_READER;
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
    sal_Bool bEncrypted )
{
    DBG_ASSERT(xInputStream.is(), "input stream missing");
    DBG_ASSERT(xModelComponent.is(), "document missing");
    DBG_ASSERT(rFactory.is(), "factory missing");
    DBG_ASSERT(NULL != pFilterName,"I need a service name for the component!");

    RTL_LOGFILE_CONTEXT_AUTHOR( aLog, "sw", LOGFILE_AUTHOR, "ReadThroughComponent" );

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
    RTL_LOGFILE_CONTEXT_TRACE( aLog, "parser created" );

    // get filter
    Reference< xml::sax::XDocumentHandler > xFilter(
        rFactory->createInstanceWithArguments(
            OUString::createFromAscii(pFilterName), rFilterArguments),
        UNO_QUERY );
    DBG_ASSERT( xFilter.is(), "Can't instantiate filter component." );
    if( !xFilter.is() )
        return ERR_SWG_READ_ERROR;
    RTL_LOGFILE_CONTEXT_TRACE1( aLog, "%s created", pFilterName );

    // connect parser and filter
    xParser->setDocumentHandler( xFilter );

    // connect model and filter
    Reference < XImporter > xImporter( xFilter, UNO_QUERY );
    xImporter->setTargetDocument( xModelComponent );


#ifdef TIMELOG
    // if we do profiling, we want to know the stream
    ByteString aString( (String)rStreamName, RTL_TEXTENCODING_ASCII_US );
    RTL_LOGFILE_TRACE_AUTHOR1( "sw", LOGFILE_AUTHOR,
                               "ReadThroughComponent : parsing \"%s\"", aString.GetBuffer() );
#endif

    // finally, parser the stream
    try
    {
        xParser->parseStream( aParserInput );
    }
    catch( xml::sax::SAXParseException& r )
    {
        if( bEncrypted )
            return ERRCODE_SFX_WRONGPASSWORD;

#if OSL_DEBUG_LEVEL > 1
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
        if( bEncrypted )
            return ERRCODE_SFX_WRONGPASSWORD;

#if OSL_DEBUG_LEVEL > 1
        ByteString aError( "SAX exception catched while importing:\n" );
        aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aError.GetBuffer() );
#endif
        return ERR_SWG_READ_ERROR;
    }
    catch( packages::zip::ZipIOException& r )
    {
#if OSL_DEBUG_LEVEL > 1
        ByteString aError( "Zip exception catched while importing:\n" );
        aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aError.GetBuffer() );
#endif
        return ERRCODE_IO_BROKENPACKAGE;
    }
    catch( io::IOException& r )
    {
#if OSL_DEBUG_LEVEL > 1
        ByteString aError( "IO exception catched while importing:\n" );
        aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aError.GetBuffer() );
#endif
        return ERR_SWG_READ_ERROR;
    }
    catch( uno::Exception& r )
    {
#if OSL_DEBUG_LEVEL > 1
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
    Reference<embed::XStorage> xStorage,
    Reference<XComponent> xModelComponent,
    const sal_Char* pStreamName,
    const sal_Char* pCompatibilityStreamName,
    Reference<lang::XMultiServiceFactory> & rFactory,
    const sal_Char* pFilterName,
    Sequence<Any> rFilterArguments,
    const OUString& rName,
    sal_Bool bMustBeSuccessfull)
{
    DBG_ASSERT(xStorage.is(), "Need storage!");
    DBG_ASSERT(NULL != pStreamName, "Please, please, give me a name!");

    // open stream (and set parser input)
    OUString sStreamName = OUString::createFromAscii(pStreamName);
    sal_Bool bContainsStream = sal_False;
    try
    {
        bContainsStream = xStorage->isStreamElement(sStreamName);
    }
    catch( container::NoSuchElementException& )
    {
    }

    if (!bContainsStream )
    {
        // stream name not found! Then try the compatibility name.
        // if no stream can be opened, return immediatly with OK signal

        // do we even have an alternative name?
        if ( NULL == pCompatibilityStreamName )
            return 0;

        // if so, does the stream exist?
        sStreamName = OUString::createFromAscii(pCompatibilityStreamName);
        try
        {
            bContainsStream = xStorage->isStreamElement(sStreamName);
        }
        catch( container::NoSuchElementException& )
        {
        }

        if (! bContainsStream )
            return 0;
    }

    // set Base URL
    uno::Reference< beans::XPropertySet > xInfoSet;
    if( rFilterArguments.getLength() > 0 )
        rFilterArguments.getConstArray()[0] >>= xInfoSet;
    DBG_ASSERT( xInfoSet.is(), "missing property set" );
    if( xInfoSet.is() )
    {
        OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("StreamName") );
        xInfoSet->setPropertyValue( sPropName, makeAny( sStreamName ) );
    }

    try
    {
        // get input stream
        Reference <io::XStream> xStream = xStorage->openStreamElement( sStreamName, embed::ElementModes::READ );
        Reference <beans::XPropertySet > xProps( xStream, uno::UNO_QUERY );

        Any aAny = xProps->getPropertyValue(
                OUString( RTL_CONSTASCII_USTRINGPARAM("Encrypted") ) );

        sal_Bool bEncrypted = aAny.getValueType() == ::getBooleanCppuType() &&
                *(sal_Bool *)aAny.getValue();

        Reference <io::XInputStream> xInputStream = xStream->getInputStream();

        // read from the stream
        return ReadThroughComponent(
            xInputStream, xModelComponent, sStreamName, rFactory,
            pFilterName, rFilterArguments,
            rName, bMustBeSuccessfull, bEncrypted );
    }
    catch ( packages::WrongPasswordException& )
    {
        return ERRCODE_SFX_WRONGPASSWORD;
    }
    catch ( uno::Exception& )
    {
        OSL_ENSURE( sal_False, "Error on import!\n" );
        // TODO/LATER: error handling
    }
}

sal_uInt32 XMLReader::Read( SwDoc &rDoc, const String& rBaseURL, SwPaM &rPaM, const String & rName )
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
    Reference<io::XInputStream> xInputStream;
    Reference<embed::XStorage> xStorage;
    if( pMedium )
        xStorage = pMedium->GetStorage();
    else
        xStorage = xStg;

    if( !xStorage.is() )
        return ERR_SWG_READ_ERROR;

    pGraphicHelper = SvXMLGraphicHelper::Create( xStorage,
                                                 GRAPHICHELPER_MODE_READ,
                                                 sal_False );
    xGraphicResolver = pGraphicHelper;
    SfxObjectShell *pPersist = rDoc.GetPersist();
    if( pPersist )
    {
        pObjectHelper = SvXMLEmbeddedObjectHelper::Create(
                                        xStorage, *pPersist,
                                        EMBEDDEDOBJECTHELPER_MODE_READ,
                                        sal_False );
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
#if (defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500))
              new uno::Type(::getCppuType((Sequence<sal_Int8>*)0)),
#else
              &::getCppuType((Sequence<sal_Int8>*)0),
#endif
              beans::PropertyAttribute::MAYBEVOID, 0 },
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
        // properties for insert modes
        { "StyleInsertModeFamilies", sizeof("StyleInsertModeFamilies")-1, 0,
              &::getCppuType((Sequence<OUString>*)0),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "StyleInsertModeOverwrite", sizeof("StyleInsertModeOverwrite")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "TextInsertModeRange", sizeof("TextInsertModeRange")-1, 0,
              &::getCppuType( (Reference<text::XTextRange> *) 0),
              beans::PropertyAttribute::MAYBEVOID, 0},
        { "AutoTextMode", sizeof("AutoTextMode")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        { "OrganizerMode", sizeof("OrganizerMode")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        // --> OD 2004-08-10 #i28749# - Add property, which indicates, if the
        // shape position attributes are given in horizontal left-to-right layout.
        // This is the case for the OpenOffice.org file format.
        { "ShapePositionInHoriL2R", sizeof("ShapePositionInHoriL2R")-1, 0,
              &::getBooleanCppuType(),
              beans::PropertyAttribute::MAYBEVOID, 0 },
        // <--
        { NULL, 0, 0, NULL, 0, 0 }
    };
    uno::Reference< beans::XPropertySet > xInfoSet(
                comphelper::GenericPropertySet_CreateInstance(
                            new comphelper::PropertySetInfo( aInfoMap ) ) );

    // try to get an XStatusIndicator from the Medium
    uno::Reference<task::XStatusIndicator> xStatusIndicator;

    if (pDocSh->GetMedium())
    {
        SfxItemSet* pSet = pDocSh->GetMedium()->GetItemSet();
        if (pSet)
        {
            const SfxUnoAnyItem* pItem = static_cast<const SfxUnoAnyItem*>(
                pSet->GetItem(SID_PROGRESS_STATUSBAR_CONTROL) );
            if (pItem)
            {
                pItem->GetValue() >>= xStatusIndicator;
            }
        }
    }


    // set progress range and start status indicator
    sal_Int32 nProgressRange(1000000);
    if (xStatusIndicator.is())
    {
        xStatusIndicator->start(SW_RESSTR(STR_STATSTR_SWGREAD), nProgressRange);
    }
    uno::Any aProgRange;
    aProgRange <<= nProgressRange;
    OUString sProgressRange(RTL_CONSTASCII_USTRINGPARAM("ProgressRange"));
    xInfoSet->setPropertyValue(sProgressRange, aProgRange);

    // prepare filter arguments, WARNING: the order is important!
    Sequence<Any> aFilterArgs( 5 );
    Any *pArgs = aFilterArgs.getArray();
    *pArgs++ <<= xInfoSet;
    *pArgs++ <<= xStatusIndicator;
    *pArgs++ <<= xGraphicResolver;
    *pArgs++ <<= xObjectResolver;
    Sequence<Any> aEmptyArgs( 3 );
    pArgs = aEmptyArgs.getArray();
    *pArgs++ <<= xInfoSet;
    *pArgs++ <<= xStatusIndicator;

    // prepare for special modes
    if( aOpt.IsFmtsOnly() )
    {
        sal_Int32 nCount = 
            (aOpt.IsFrmFmts() ? 1 : 0) +
            (aOpt.IsPageDescs() ? 1 : 0) +
            (aOpt.IsTxtFmts() ? 2 : 0) +
            (aOpt.IsNumRules() ? 1 : 0);

        Sequence< OUString> aFamiliesSeq( nCount );
        OUString *pSeq = aFamiliesSeq.getArray();
        if( aOpt.IsFrmFmts() )
            // SFX_STYLE_FAMILY_FRAME;
            *pSeq++ = OUString::createFromAscii("FrameStyles");
        if( aOpt.IsPageDescs() )
            // SFX_STYLE_FAMILY_PAGE;
            *pSeq++ = OUString::createFromAscii("PageStyles");
        if( aOpt.IsTxtFmts() )
        {
            // (SFX_STYLE_FAMILY_CHAR|SFX_STYLE_FAMILY_PARA);
            *pSeq++ = OUString::createFromAscii("CharacterStyles");
            *pSeq++ = OUString::createFromAscii("ParagraphStyles");
        }
        if( aOpt.IsNumRules() )
            // SFX_STYLE_FAMILY_PSEUDO;
            *pSeq++ = OUString::createFromAscii("NumberingStyles");

        OUString sStyleInsertModeFamilies(
                RTL_CONSTASCII_USTRINGPARAM("StyleInsertModeFamilies"));
        xInfoSet->setPropertyValue( sStyleInsertModeFamilies, 
                                    makeAny(aFamiliesSeq) );

        OUString sStyleInsertModeOverwrite(
                RTL_CONSTASCII_USTRINGPARAM("StyleInsertModeOverwrite"));
        sal_Bool bTmp = !aOpt.IsMerge();
        Any aAny;
        aAny.setValue( &bTmp, ::getBooleanCppuType() );
        xInfoSet->setPropertyValue( sStyleInsertModeOverwrite, aAny );
    }
    else if( bInsertMode )
    {
        Reference<XTextRange> xInsertTextRange = 
            SwXTextRange::CreateTextRangeFromPosition( &rDoc, *rPaM.GetPoint(),
                                                           0 );
        OUString sTextInsertModeRange(
                RTL_CONSTASCII_USTRINGPARAM("TextInsertModeRange"));
        xInfoSet->setPropertyValue( sTextInsertModeRange, 
                                    makeAny(xInsertTextRange) );
    }
    else
    {
        rPaM.GetBound(true).nContent.Assign(0, 0);
        rPaM.GetBound(false).nContent.Assign(0, 0);
    }

    if( IsBlockMode() )
    {
        OUString sAutoTextMode(
                RTL_CONSTASCII_USTRINGPARAM("AutoTextMode"));
        sal_Bool bTmp = sal_True;
        Any aAny;
        aAny.setValue( &bTmp, ::getBooleanCppuType() );
        xInfoSet->setPropertyValue( sAutoTextMode, aAny );
    }
    if( IsOrganizerMode() )
    {
        OUString sOrganizerMode(
                RTL_CONSTASCII_USTRINGPARAM("OrganizerMode"));
        sal_Bool bTmp = sal_True;
        Any aAny;
        aAny.setValue( &bTmp, ::getBooleanCppuType() );
        xInfoSet->setPropertyValue( sOrganizerMode, aAny );
    }

    // Set base URI
    // there is ambiguity which medium should be used here
    // for now the own medium has a preference
    SfxMedium* pMedDescrMedium = pMedium ? pMedium : pDocSh->GetMedium();
    OSL_ENSURE( pMedDescrMedium, "There is no medium to get MediaDescriptor from!\n" );

    ::rtl::OUString aBaseURL( rBaseURL );
    OUString sPropName( RTL_CONSTASCII_USTRINGPARAM("BaseURI") );
    xInfoSet->setPropertyValue( sPropName, makeAny( aBaseURL ) );

    // TODO/LATER: separate links from usual embedded objects
    if( SFX_CREATE_MODE_EMBEDDED == rDoc.GetDocShell()->GetCreateMode() )
    {
        OUString aName = ::rtl::OUString::createFromAscii( "dummyObjectName" );
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

    sal_Bool bOASIS = ( SotStorage::GetVersion( xStorage ) > SOFFICE_FILEFORMAT_60 );
    // --> OD 2004-08-10 #i28749# - set property <ShapePositionInHoriL2R>
    {
        const sal_Bool bShapePositionInHoriL2R = !bOASIS;
        xInfoSet->setPropertyValue(
                OUString(RTL_CONSTASCII_USTRINGPARAM("ShapePositionInHoriL2R")),
                makeAny( bShapePositionInHoriL2R ) );
    }
    // <--
    sal_uInt32 nWarn = 0;
    sal_uInt32 nWarn2 = 0;
    // read storage streams
    if( !(IsOrganizerMode() || IsBlockMode() || aOpt.IsFmtsOnly() ||
          bInsertMode) )
    {
        nWarn = ReadThroughComponent(
            xStorage, xModelComp, "meta.xml", "Meta.xml", xServiceFactory,
            (bOASIS ? "com.sun.star.comp.Writer.XMLOasisMetaImporter"
                    : "com.sun.star.comp.Writer.XMLMetaImporter"),
            aEmptyArgs, rName, sal_False );

        nWarn2 = ReadThroughComponent(
            xStorage, xModelComp, "settings.xml", NULL, xServiceFactory,
            (bOASIS ? "com.sun.star.comp.Writer.XMLOasisSettingsImporter"
                    : "com.sun.star.comp.Writer.XMLSettingsImporter"),
            aFilterArgs, rName, sal_False );
    }

    nRet = ReadThroughComponent(
        xStorage, xModelComp, "styles.xml", NULL, xServiceFactory,
        (bOASIS ? "com.sun.star.comp.Writer.XMLOasisStylesImporter"
                : "com.sun.star.comp.Writer.XMLStylesImporter"),
        aFilterArgs, rName, sal_True );

    if( !nRet && !(IsOrganizerMode() || aOpt.IsFmtsOnly()) )
        nRet = ReadThroughComponent(
           xStorage, xModelComp, "content.xml", "Content.xml", xServiceFactory,
            (bOASIS ? "com.sun.star.comp.Writer.XMLOasisContentImporter"
                    : "com.sun.star.comp.Writer.XMLContentImporter"),
           aFilterArgs, rName, sal_True );

    if( !(IsOrganizerMode() || IsBlockMode() || bInsertMode ||
          aOpt.IsFmtsOnly() ) )
    {
        OUString sStreamName( RTL_CONSTASCII_USTRINGPARAM("layout-cache") );
        try
        {
            Reference < io::XStream > xStm = xStorage->openStreamElement( sStreamName, embed::ElementModes::READ );
            SvStream* pStrm = utl::UcbStreamHelper::CreateStream( xStm );
            if( !pStrm->GetError() )
                rDoc.ReadLayoutCache( *pStrm );
            delete pStrm;
        }
        catch ( uno::Exception& )
        {
        }
    }

    // Notify math objects
    if( bInsertMode )
        rDoc.PrtOLENotify( FALSE );
    else if ( rDoc.IsOLEPrtNotifyPending() )
        rDoc.PrtOLENotify( TRUE );

    if( !nRet )
    {
        if( nWarn )
            nRet = nWarn;
        else if( nWarn2 )
            nRet = nWarn2;
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

    // #103728# move Pam into valid content
    lcl_EnsureValidPam( rPaM );

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

    // read the sections of the document, which is equal to the medium.
    // returns the count of it
USHORT XMLReader::GetSectionList( SfxMedium& rMedium,
                                    SvStrings& rStrings ) const
{
    Reference< lang::XMultiServiceFactory > xServiceFactory =
            comphelper::getProcessServiceFactory();
    ASSERT( xServiceFactory.is(),
            "XMLReader::Read: got no service manager" );
    Reference < embed::XStorage > xStg;
    if( xServiceFactory.is() && ( xStg = rMedium.GetStorage() ).is() )
    {
        xml::sax::InputSource aParserInput;
        OUString sDocName( RTL_CONSTASCII_USTRINGPARAM( "content.xml" ) );
        aParserInput.sSystemId = sDocName;

        Reference < io::XStream > xStm = xStg->openStreamElement( sDocName, embed::ElementModes::READ );
        aParserInput.aInputStream = xStm->getInputStream();

        // get parser
        Reference< XInterface > xXMLParser = xServiceFactory->createInstance(
            OUString::createFromAscii("com.sun.star.xml.sax.Parser") );
        ASSERT( xXMLParser.is(),
            "XMLReader::Read: com.sun.star.xml.sax.Parser service missing" );
        if( xXMLParser.is() )
        {
            // get filter
            // #110680#
            // Reference< xml::sax::XDocumentHandler > xFilter = new SwXMLSectionList( rStrings );
            Reference< xml::sax::XDocumentHandler > xFilter = new SwXMLSectionList( xServiceFactory, rStrings );

            // connect parser and filter
            Reference< xml::sax::XParser > xParser( xXMLParser, UNO_QUERY );
            xParser->setDocumentHandler( xFilter );

            // parse
            try
            {
                xParser->parseStream( aParserInput );
            }
            catch( xml::sax::SAXParseException&  )
            {
                // re throw ?
            }
            catch( xml::sax::SAXException&  )
            {
                // re throw ?
            }
            catch( io::IOException& )
            {
                // re throw ?
            }
        }
    }
    return rStrings.Count();
}

