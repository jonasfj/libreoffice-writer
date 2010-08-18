/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "vbadocument.hxx"
#include "vbarange.hxx"
#include "vbarangehelper.hxx"
#include "vbadocumentproperties.hxx"
#include "vbabookmarks.hxx"
#include "vbavariables.hxx"
#include <com/sun/star/text/XBookmarksSupplier.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#include <com/sun/star/document/XDocumentProperties.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/drawing/XControlShape.hpp>
#include <com/sun/star/drawing/XControlShape.hpp>
#include <com/sun/star/form/XFormsSupplier.hpp>
#include <ooo/vba/XControlProvider.hpp>

#include <vbahelper/helperdecl.hxx>
#include <wordvbahelper.hxx>
#include <docsh.hxx>
#include "vbatemplate.hxx"
#include "vbaparagraph.hxx"
#include "vbastyles.hxx"
#include "vbatables.hxx"
#include "vbafield.hxx"
#include "vbapagesetup.hxx"
#include "vbasections.hxx"
#include <vbahelper/vbashapes.hxx>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaDocument::SwVbaDocument( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, uno::Reference< frame::XModel > xModel ): SwVbaDocument_BASE( xParent, xContext, xModel )
{
    Initialize();
}
SwVbaDocument::SwVbaDocument( uno::Sequence< uno::Any > const& aArgs, uno::Reference< uno::XComponentContext >const& xContext ) : SwVbaDocument_BASE( aArgs, xContext )
{
    Initialize();
} 

SwVbaDocument::~SwVbaDocument()
{
}

void SwVbaDocument::Initialize()
{
    mxTextDocument.set( getModel(), uno::UNO_QUERY_THROW );    
}

uno::Reference< word::XRange > SAL_CALL 
SwVbaDocument::getContent() throw ( uno::RuntimeException )
{
    uno::Reference< text::XTextRange > xStart = mxTextDocument->getText()->getStart();
    uno::Reference< text::XTextRange > xEnd;
    return uno::Reference< word::XRange >( new SwVbaRange( this, mxContext, mxTextDocument, xStart, xEnd, sal_True ) );
}

uno::Reference< word::XRange > SAL_CALL 
SwVbaDocument::Range( const uno::Any& rStart, const uno::Any& rEnd ) throw ( uno::RuntimeException )
{
    if( !rStart.hasValue() && !rEnd.hasValue() )
        return getContent();
    
    sal_Int32 nStart = 0;
    sal_Int32 nEnd = 0;
    rStart >>= nStart;
    rEnd >>= nEnd;
    nStart--;
    nEnd--;

    uno::Reference< text::XTextRange > xStart;
    uno::Reference< text::XTextRange > xEnd;
    if( nStart != -1 || nEnd != -1 )
    {
        if( nStart == -1 )
            xStart = mxTextDocument->getText()->getStart();
        else
            xStart = SwVbaRangeHelper::getRangeByPosition( mxTextDocument->getText(), nStart );

        if( nEnd == -1 )
            xEnd = mxTextDocument->getText()->getEnd();
        else
            xEnd = SwVbaRangeHelper::getRangeByPosition( mxTextDocument->getText(), nEnd );
    }

    if( !xStart.is() && !xEnd.is() )
    {
        try
        {
            // FIXME
            xStart = mxTextDocument->getText()->getStart();
            xEnd = mxTextDocument->getText()->getEnd();
        }
        catch( uno::Exception )
        {
            DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
        }    
    }
    return uno::Reference< word::XRange >( new SwVbaRange( this, mxContext, mxTextDocument, xStart, xEnd ) );
}

uno::Any SAL_CALL
SwVbaDocument::BuiltInDocumentProperties( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaBuiltinDocumentProperties( mxParent, mxContext, getModel() ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaDocument::CustomDocumentProperties( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaCustomDocumentProperties( mxParent, mxContext, getModel() ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL 
SwVbaDocument::Bookmarks( const uno::Any& rIndex ) throw ( uno::RuntimeException )
{
    uno::Reference< text::XBookmarksSupplier > xBookmarksSupplier( getModel(),uno::UNO_QUERY_THROW );
    uno::Reference<container::XIndexAccess > xBookmarks( xBookmarksSupplier->getBookmarks(), uno::UNO_QUERY_THROW );
    uno::Reference< XCollection > xBookmarksVba( new SwVbaBookmarks( this, mxContext, xBookmarks, getModel() ) );
    if (  rIndex.getValueTypeClass() == uno::TypeClass_VOID )
        return uno::makeAny( xBookmarksVba );
    
    return uno::Any( xBookmarksVba->Item( rIndex, uno::Any() ) );
}

uno::Any SAL_CALL 
SwVbaDocument::Variables( const uno::Any& rIndex ) throw ( uno::RuntimeException )
{
    uno::Reference< document::XDocumentPropertiesSupplier > xDocumentPropertiesSupplier( getModel(),uno::UNO_QUERY_THROW );
    uno::Reference< document::XDocumentProperties > xDocumentProperties =  xDocumentPropertiesSupplier->getDocumentProperties();
    uno::Reference< beans::XPropertyAccess > xUserDefined( xDocumentProperties->getUserDefinedProperties(), uno::UNO_QUERY_THROW );

    uno::Reference< XCollection > xVariables( new SwVbaVariables( this, mxContext, xUserDefined ) );
    if (  rIndex.getValueTypeClass() == uno::TypeClass_VOID )
        return uno::makeAny( xVariables );
    
    return uno::Any( xVariables->Item( rIndex, uno::Any() ) );
}

uno::Any SAL_CALL
SwVbaDocument::Paragraphs( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaParagraphs( mxParent, mxContext, mxTextDocument ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaDocument::Styles( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaStyles( mxParent, mxContext, getModel() ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaDocument::Fields( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaFields( mxParent, mxContext, getModel() ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaDocument::Shapes( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< drawing::XDrawPageSupplier > xDrawPageSupplier( getModel(), uno::UNO_QUERY_THROW );
    //uno::Reference< drawing::XShapes > xShapes( xDrawPageSupplier->getDrawPage(), uno::UNO_QUERY_THROW );
    uno::Reference< container::XIndexAccess > xIndexAccess( xDrawPageSupplier->getDrawPage(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XModel > xModel( mxTextDocument, uno::UNO_QUERY_THROW );
    uno::Reference< XCollection > xCol( new ScVbaShapes( this, mxContext, xIndexAccess, xModel ) );

    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaDocument::Sections( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaSections( mxParent, mxContext, getModel() ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaDocument::PageSetup( ) throw (uno::RuntimeException)
{
    uno::Reference< beans::XPropertySet > xPageProps( word::getCurrentPageStyle( mxModel ), uno::UNO_QUERY_THROW );
    return uno::makeAny( uno::Reference< word::XPageSetup >( new SwVbaPageSetup( this, mxContext, mxModel, xPageProps ) ) );
}

rtl::OUString& 
SwVbaDocument::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("SwVbaDocument") );
    return sImplName;
}
uno::Any SAL_CALL 
SwVbaDocument::getAttachedTemplate() throw (uno::RuntimeException)
{
    uno::Reference< word::XTemplate > xTemplate;
    uno::Reference< document::XDocumentInfoSupplier > xDocInfoSupp( getModel(), uno::UNO_QUERY_THROW );
    uno::Reference< document::XDocumentPropertiesSupplier > xDocPropSupp( xDocInfoSupp->getDocumentInfo(), uno::UNO_QUERY_THROW );
    uno::Reference< document::XDocumentProperties > xDocProps( xDocPropSupp->getDocumentProperties(), uno::UNO_QUERY_THROW );
    rtl::OUString sTemplateName = xDocProps->getTemplateName();

    xTemplate = new SwVbaTemplate( this, mxContext, getModel(), sTemplateName );
    return uno::makeAny( xTemplate );
}

void SAL_CALL 
SwVbaDocument::setAttachedTemplate( const css::uno::Any& /*_attachedtemplate*/ ) throw (uno::RuntimeException)
{
    throw uno::RuntimeException();
}

uno::Any SAL_CALL 
SwVbaDocument::Tables( const css::uno::Any& aIndex ) throw (uno::RuntimeException)
{
    uno::Reference< frame::XModel > xModel( mxTextDocument, uno::UNO_QUERY_THROW );
    uno::Reference< XCollection > xColl( new SwVbaTables( mxParent, mxContext, xModel ) );
    
    if ( aIndex.hasValue() )
        return xColl->Item( aIndex, uno::Any() );
    return uno::makeAny( xColl );
}

void SAL_CALL SwVbaDocument::Activate() throw (uno::RuntimeException)
{
    VbaDocumentBase::Activate();
}

uno::Any
SwVbaDocument::getControlShape( const ::rtl::OUString& sName )
{
    uno::Reference< drawing::XDrawPageSupplier > xDrawPageSupplier( mxTextDocument, uno::UNO_QUERY_THROW );
    uno::Reference< container::XIndexAccess > xIndexAccess( xDrawPageSupplier->getDrawPage(), uno::UNO_QUERY_THROW );

    sal_Int32 nCount = xIndexAccess->getCount();
    for( int index = 0; index < nCount; index++ )
    {
        uno::Any aUnoObj =  xIndexAccess->getByIndex( index );
        // It seems there are some drawing objects that can not query into Control shapes?
        uno::Reference< drawing::XControlShape > xControlShape( aUnoObj, uno::UNO_QUERY );
        if( xControlShape.is() )
        {
             uno::Reference< container::XNamed > xNamed( xControlShape->getControl(), uno::UNO_QUERY_THROW );
            if( sName.equals( xNamed->getName() ))
            {
                return aUnoObj;
            }
        }
    }
    return uno::Any();
}

uno::Reference< beans::XIntrospectionAccess > SAL_CALL 
SwVbaDocument::getIntrospection(  ) throw (uno::RuntimeException)
{
    return uno::Reference< beans::XIntrospectionAccess >();
}

uno::Any SAL_CALL 
SwVbaDocument::invoke( const ::rtl::OUString& aFunctionName, const uno::Sequence< uno::Any >& /*aParams*/, uno::Sequence< ::sal_Int16 >& /*aOutParamIndex*/, uno::Sequence< uno::Any >& /*aOutParam*/ ) throw (lang::IllegalArgumentException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
    OSL_TRACE("** SwVbaDocument::invoke( %s ), will barf",
        rtl::OUStringToOString( aFunctionName, RTL_TEXTENCODING_UTF8 ).getStr() );
    
    throw uno::RuntimeException(); // unsupported operation
}

void SAL_CALL 
SwVbaDocument::setValue( const ::rtl::OUString& /*aPropertyName*/, const uno::Any& /*aValue*/ ) throw (beans::UnknownPropertyException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
    throw uno::RuntimeException(); // unsupported operation
}
uno::Any SAL_CALL 
SwVbaDocument::getValue( const ::rtl::OUString& aPropertyName ) throw (beans::UnknownPropertyException, uno::RuntimeException)
{
    uno::Reference< drawing::XControlShape > xControlShape( getControlShape( aPropertyName ), uno::UNO_QUERY_THROW );
    
    uno::Reference<lang::XMultiComponentFactory > xServiceManager( mxContext->getServiceManager(), uno::UNO_QUERY_THROW );
    uno::Reference< XControlProvider > xControlProvider( xServiceManager->createInstanceWithContext( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ooo.vba.ControlProvider" ) ), mxContext ), uno::UNO_QUERY_THROW );
    uno::Reference< msforms::XControl > xControl( xControlProvider->createControl(  xControlShape, getModel() ) );
    return uno::makeAny( xControl );
}

::sal_Bool SAL_CALL 
SwVbaDocument::hasMethod( const ::rtl::OUString& /*aName*/ ) throw (uno::RuntimeException)
{
    return sal_False;
}

::sal_Bool SAL_CALL 
SwVbaDocument::hasProperty( const ::rtl::OUString& aName ) throw (uno::RuntimeException)
{
    uno::Reference< container::XNameAccess > xFormControls( getFormControls() );
    if ( xFormControls.is() )
        return xFormControls->hasByName( aName );
    return sal_False;
}

uno::Reference< container::XNameAccess > 
SwVbaDocument::getFormControls()
{
    uno::Reference< container::XNameAccess > xFormControls;
    try
    {
        uno::Reference< drawing::XDrawPageSupplier > xDrawPageSupplier( mxTextDocument, uno::UNO_QUERY_THROW );
        uno::Reference< form::XFormsSupplier >  xFormSupplier( xDrawPageSupplier->getDrawPage(), uno::UNO_QUERY_THROW );
            uno::Reference< container::XIndexAccess > xIndexAccess( xFormSupplier->getForms(), uno::UNO_QUERY_THROW );
        // get the www-standard container ( maybe we should access the 
        // 'www-standard' by name rather than index, this seems an
        // implementation detail
        xFormControls.set( xIndexAccess->getByIndex(0), uno::UNO_QUERY_THROW );
    }
    catch( uno::Exception& )
    {
    }
    return xFormControls;
}

uno::Sequence< rtl::OUString > 
SwVbaDocument::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Document" ) );
    }
    return aServiceNames;
}

namespace document
{
namespace sdecl = comphelper::service_decl;
sdecl::vba_service_class_<SwVbaDocument, sdecl::with_args<true> > serviceImpl;
extern sdecl::ServiceDecl const serviceDecl(
    serviceImpl,
    "SwVbaDocument",
    "ooo.vba.word.Document" );
}

