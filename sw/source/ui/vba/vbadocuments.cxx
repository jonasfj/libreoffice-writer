/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: vbadocuments.cxx,v $
 * $Revision: 1.4 $
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
#include <comphelper/processfactory.hxx>

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>

#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/beans/PropertyVetoException.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/document/XTypeDetection.hpp>
#include <com/sun/star/uri/XUriReference.hpp>
#include <com/sun/star/uri/XUriReferenceFactory.hpp>

#include <sfx2/objsh.hxx>
#include <tools/urlobj.hxx>

#include "vbaglobals.hxx"
#include "vbadocument.hxx"
#include "vbadocuments.hxx"
#include <vbahelper/vbahelper.hxx>

#include <hash_map>
#include <osl/file.hxx>
using namespace ::ooo::vba;
using namespace ::com::sun::star;

static uno::Any
getDocument( uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< text::XTextDocument > &xDoc, const uno::Any& aApplication )
{
    // FIXME: fine as long as SwVbaDocument is stateless ...
    uno::Reference< frame::XModel > xModel( xDoc, uno::UNO_QUERY );
    if( !xModel.is() )
        return uno::Any();
    
    SwVbaDocument *pWb = new SwVbaDocument(  uno::Reference< XHelperInterface >( aApplication, uno::UNO_QUERY_THROW ), xContext, xModel );
    return uno::Any( uno::Reference< word::XDocument > (pWb) );
}

class DocumentEnumImpl : public EnumerationHelperImpl
{
    uno::Any m_aApplication;
public:
    DocumentEnumImpl( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< container::XEnumeration >& xEnumeration, const uno::Any& aApplication ) throw ( uno::RuntimeException ) : EnumerationHelperImpl( xContext, xEnumeration ), m_aApplication( aApplication ) {}

    virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException) 
    { 
        uno::Reference< text::XTextDocument > xDoc( m_xEnumeration->nextElement(), uno::UNO_QUERY_THROW );
        return getDocument( m_xContext, xDoc, m_aApplication );
    }
};

SwVbaDocuments::SwVbaDocuments( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext ) : SwVbaDocuments_BASE( xParent, xContext, VbaDocumentsBase::WORD_DOCUMENT )
{
}
// XEnumerationAccess
uno::Type
SwVbaDocuments::getElementType() throw (uno::RuntimeException)
{
    return word::XDocument::static_type(0);
}
uno::Reference< container::XEnumeration >
SwVbaDocuments::createEnumeration() throw (uno::RuntimeException)
{
    // #FIXME its possible the DocumentEnumImpl here doens't reflect
    // the state of this object ( although it should ) would be 
    // safer to create an enumeration based on this objects state
    // rather than one effectively based of the desktop component
    uno::Reference< container::XEnumerationAccess > xEnumerationAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );
    return new DocumentEnumImpl( mxContext, xEnumerationAccess->createEnumeration(), Application() );
}

uno::Any
SwVbaDocuments::createCollectionObject( const uno::Any& aSource )
{
    uno::Reference< text::XTextDocument > xDoc( aSource, uno::UNO_QUERY_THROW );
    return getDocument( mxContext, xDoc, Application() );
}

uno::Any SAL_CALL
SwVbaDocuments::Add( const uno::Any& Template, const uno::Any& /*NewTemplate*/, const uno::Any& /*DocumentType*/, const uno::Any& /*Visible*/ ) throw (uno::RuntimeException)
{
    rtl::OUString sFileName;
    if( Template.hasValue() && ( Template >>= sFileName ) )
    {
        return  Open( sFileName, uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any(), uno::Any());
    }
    uno::Reference <text::XTextDocument> xTextDoc( VbaDocumentsBase::Add() , uno::UNO_QUERY_THROW );

    if( xTextDoc.is() )
        return getDocument( mxContext, xTextDoc, Application() );
    return uno::Any();
}

// #TODO# #FIXME# can any of the unused params below be used?
void SAL_CALL
SwVbaDocuments::Close( const uno::Any& /*SaveChanges*/, const uno::Any& /*OriginalFormat*/, const uno::Any& /*RouteDocument*/ ) throw (uno::RuntimeException)
{
    VbaDocumentsBase::Close();
}

// #TODO# #FIXME# can any of the unused params below be used?
uno::Any SAL_CALL
SwVbaDocuments::Open( const ::rtl::OUString& Filename, const uno::Any& /*ConfirmConversions*/, const uno::Any& ReadOnly, const uno::Any& /*AddToRecentFiles*/, const uno::Any& /*PasswordDocument*/, const uno::Any& /*PasswordTemplate*/, const uno::Any& /*Revert*/, const uno::Any& /*WritePasswordDocument*/, const uno::Any& /*WritePasswordTemplate*/, const uno::Any& /*Format*/, const uno::Any& /*Encoding*/, const uno::Any& /*Visible*/, const uno::Any& /*OpenAndRepair*/, const uno::Any& /*DocumentDirection*/, const uno::Any& /*NoEncodingDialog*/, const uno::Any& /*XMLTransform*/ ) throw (uno::RuntimeException)
{
    // we need to detect if this is a URL, if not then assume its a file path
    rtl::OUString aURL;
    INetURLObject aObj;
    aObj.SetURL( Filename );
    bool bIsURL = aObj.GetProtocol() != INET_PROT_NOT_VALID;
    if ( bIsURL )
        aURL = Filename;
    else
        osl::FileBase::getFileURLFromSystemPath( Filename, aURL );

    uno::Sequence< beans::PropertyValue > sProps(0);

    uno::Reference <text::XTextDocument> xSpreadDoc( VbaDocumentsBase::Open( Filename, ReadOnly, sProps ), uno::UNO_QUERY_THROW );
    uno::Any aRet = getDocument( mxContext, xSpreadDoc, Application() );
    uno::Reference< word::XDocument > xDocument( aRet, uno::UNO_QUERY );
    if ( xDocument.is() )
        xDocument->Activate();
    return aRet;
}

rtl::OUString& 
SwVbaDocuments::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("SwVbaDocuments") );
    return sImplName;
}

uno::Sequence<rtl::OUString> 
SwVbaDocuments::getServiceNames()
{
    static uno::Sequence< rtl::OUString > sNames;
    if ( sNames.getLength() == 0 )
    {
        sNames.realloc( 1 );
        sNames[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Documents") );
    }
    return sNames;
}
