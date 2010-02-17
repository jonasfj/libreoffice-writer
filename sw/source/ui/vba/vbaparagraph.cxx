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
#include "vbaparagraph.hxx"
#include <vbahelper/vbahelper.hxx>
#include <tools/diagnose_ex.h>
#include "vbarange.hxx"
#include <com/sun/star/lang/XServiceInfo.hpp>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaParagraph::SwVbaParagraph( const uno::Reference< ooo::vba::XHelperInterface >& rParent, const uno::Reference< uno::XComponentContext >& rContext, const uno::Reference< text::XTextDocument >& xDocument, const uno::Reference< text::XTextRange >& xTextRange ) throw ( uno::RuntimeException ) :
    SwVbaParagraph_BASE( rParent, rContext ), mxTextDocument( xDocument ), mxTextRange( xTextRange )
{
}

SwVbaParagraph::~SwVbaParagraph()
{
}

uno::Reference< word::XRange > SAL_CALL 
SwVbaParagraph::getRange( ) throw ( uno::RuntimeException )
{
    return uno::Reference< word::XRange >( new SwVbaRange( this, mxContext, mxTextDocument, mxTextRange->getStart(), mxTextRange->getEnd(), mxTextRange->getText(), sal_True ) );
}

rtl::OUString& 
SwVbaParagraph::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("SwVbaParagraph") );
    return sImplName;
}

uno::Sequence< rtl::OUString > 
SwVbaParagraph::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Paragraph" ) );
    }
    return aServiceNames;
}


//typedef ::cppu::WeakImplHelper1< container::XEnumeration > ParagraphEnumeration_BASE;
typedef ::cppu::WeakImplHelper2< container::XIndexAccess, container::XEnumerationAccess > ParagraphCollectionHelper_BASE;

class ParagraphCollectionHelper : public ParagraphCollectionHelper_BASE
{
private:
    uno::Reference< text::XTextDocument > mxTextDocument;
    
    uno::Reference< container::XEnumeration > getEnumeration() throw (uno::RuntimeException)
    {
        uno::Reference< container::XEnumerationAccess > xParEnumAccess( mxTextDocument->getText(), uno::UNO_QUERY_THROW );
        return xParEnumAccess->createEnumeration();
    }

public:
    ParagraphCollectionHelper( const uno::Reference< text::XTextDocument >& xDocument ) throw (uno::RuntimeException): mxTextDocument( xDocument )
    {
    }
    // XElementAccess
    virtual uno::Type SAL_CALL getElementType(  ) throw (uno::RuntimeException) { return  text::XTextRange::static_type(0); }
    virtual ::sal_Bool SAL_CALL hasElements(  ) throw (uno::RuntimeException) { return sal_True; }
    // XIndexAccess
    virtual ::sal_Int32 SAL_CALL getCount(  ) throw (uno::RuntimeException)
    {
        sal_Int32 nCount = 0;
        uno::Reference< container::XEnumeration > xParEnum = getEnumeration();
        while( xParEnum->hasMoreElements() )
        {
            uno::Reference< lang::XServiceInfo > xServiceInfo( xParEnum->nextElement(), uno::UNO_QUERY_THROW );
            if( xServiceInfo->supportsService( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Paragraph") ) ) )
            {
                nCount++;
            }
        }
        return nCount;
    }	
    virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw (lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
    {
        if( Index < getCount() )
        {
            sal_Int32 nCount = 0;
            uno::Reference< container::XEnumeration > xParEnum = getEnumeration();
            while( xParEnum->hasMoreElements() )
            {
                uno::Reference< lang::XServiceInfo > xServiceInfo( xParEnum->nextElement(), uno::UNO_QUERY_THROW );
                if( xServiceInfo->supportsService( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Paragraph") ) ) )
                {
                    if( Index == nCount )
                        return uno::makeAny( xServiceInfo );
                    nCount++;
                }        
            }
        }
        throw lang::IndexOutOfBoundsException();
    }
    // XEnumerationAccess
    virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration(  ) throw (uno::RuntimeException)
    {
        return getEnumeration();
    }    
};

SwVbaParagraphs::SwVbaParagraphs( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< ::com::sun::star::uno::XComponentContext > & xContext, const uno::Reference< text::XTextDocument >& xDocument ) throw (uno::RuntimeException) : SwVbaParagraphs_BASE( xParent, xContext, new ParagraphCollectionHelper( xDocument ) ), mxTextDocument( xDocument )
{
}

// XEnumerationAccess
uno::Type
SwVbaParagraphs::getElementType() throw (uno::RuntimeException)
{
    return word::XParagraph::static_type(0);
}
uno::Reference< container::XEnumeration >
SwVbaParagraphs::createEnumeration() throw (uno::RuntimeException)
{
    uno::Reference< container::XEnumerationAccess > xEnumerationAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );
    return xEnumerationAccess->createEnumeration();
}

uno::Any
SwVbaParagraphs::createCollectionObject( const css::uno::Any& aSource )
{
    uno::Reference< text::XTextRange > xTextRange( aSource, uno::UNO_QUERY_THROW );
    return uno::makeAny( uno::Reference< word::XParagraph >( new SwVbaParagraph( this, mxContext, mxTextDocument, xTextRange ) ) );
}

rtl::OUString& 
SwVbaParagraphs::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("SwVbaParagraphs") );
    return sImplName;
}

css::uno::Sequence<rtl::OUString> 
SwVbaParagraphs::getServiceNames()
{
    static uno::Sequence< rtl::OUString > sNames;
    if ( sNames.getLength() == 0 )
    {
        sNames.realloc( 1 );
        sNames[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Paragraphs") );
    }
    return sNames;
}
