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
#include "vbastyles.hxx"
#include "vbastyle.hxx"
#include <cppuhelper/implbase3.hxx>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <ooo/vba/word/WdBuiltinStyle.hpp>
#include <ooo/vba/word/WdStyleType.hpp>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

struct BuiltinStyleTable
{
    sal_Int32 wdBuiltinStyle;
    const sal_Char* pOOoStyleName;
    sal_Int32 wdStyleType;
};

const BuiltinStyleTable aBuiltinStyleTable[] =
{
    { word::WdBuiltinStyle::wdStyleBlockQuotation, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyText, "Text body", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyText2, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyText3, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyTextFirstIndent, "First line indent", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyTextFirstIndent2, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyTextIndent, "Text body indent", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyTextIndent2, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleBodyTextIndent3, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleCaption, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleClosing, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleCommentReference, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleCommentText, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleDate, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleDefaultParagraphFont, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleEmphasis, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleEndnoteReference, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleEndnoteText, "Endnote", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleEnvelopeAddress, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleEnvelopeReturn, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleFooter, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleFootnoteReference, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleFootnoteText, "Footnote", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeader, "Header", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading1, "Heading 1", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading2, "Heading 2", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading3, "Heading 3", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading4, "Heading 4", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading5, "Heading 5", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading6, "Heading 6", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading7, "Heading 7", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading8, "Heading 8", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHeading9, "Heading 9", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlAcronym, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlAddress, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlCite, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlCode, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlDfn, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlKbd, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlNormal, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlPre, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlSamp, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlTt, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHtmlVar, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHyperlink, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleHyperlinkFollowed, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex1, "Index 1", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex2, "Index 2", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex3, "Index 3", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex4, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex5, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex6, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex7, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex8, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndex9, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleIndexHeading, "Index Heading", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleLineNumber, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleList, "List", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleList2, "List 2", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleList3, "List 3", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleList4, "List 4", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleList5, "List 5", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleListBullet, "List 1", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListBullet2, "List 2", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListBullet3, "List 3", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListBullet4, "List 4", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListBullet5, "List 5", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListContinue, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleListContinue2, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleListContinue3, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleListContinue4, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleListContinue5, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleListNumber, "Numbering 1", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListNumber2, "Numbering 2", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListNumber3, "Numbering 3", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListNumber4, "Numbering 4", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleListNumber5, "Numbering 5", word::WdStyleType::wdStyleTypeList },
    { word::WdBuiltinStyle::wdStyleMacroText, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleMessageHeader, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleNavPane, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleNormal, "Default", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleNormalIndent, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleNormalTable, "Table", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleNoteHeading, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStylePageNumber, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStylePlainText, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleSalutation, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleSignature, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleStrong, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleSubtitle, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTableOfAuthorities, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTableOfFigures, "", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTitle, "Title", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOAHeading, "Contents Heading", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC1, "Contents 1", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC2, "Contents 2", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC3, "Contents 3", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC4, "Contents 4", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC5, "Contents 5", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC6, "Contents 6", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC7, "Contents 7", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC8, "Contents 8", word::WdStyleType::wdStyleTypeParagraph },
    { word::WdBuiltinStyle::wdStyleTOC9, "Contents 9", word::WdStyleType::wdStyleTypeParagraph },
    { 0, 0, 0 }
};


static uno::Sequence< rtl::OUString > getStyleTypes()
{
    uno::Sequence< rtl::OUString > aRet(3);
    rtl::OUString* pArray = aRet.getArray();
    pArray[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ParagraphStyles") );
    pArray[1] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("CharacterStyles") );
    pArray[2] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("NumberingStyles") );
    return aRet;
}

typedef ::cppu::WeakImplHelper1< container::XEnumeration > StyleEnumeration_BASE;
typedef ::cppu::WeakImplHelper3< container::XNameAccess, container::XIndexAccess, container::XEnumerationAccess > StyleCollectionHelper_BASE;
/*
class StylesEnumeration : public StyleEnumeration_BASE
{
public:
    StylesEnumeration( const SheetMap& sMap ) : mSheetMap( sMap ), mIt( mSheetMap.begin() ) {}
    virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (uno::RuntimeException)
    {
        return ( mIt != mSheetMap.end() );
    }
    virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException)
    {
        if ( !hasMoreElements() )
            throw container::NoSuchElementException();
        uno::Reference< sheet::XSpreadsheet > xSheet( *mIt++ );
        return uno::makeAny( xSheet ) ;
    }
};
*/
class StyleCollectionHelper : public StyleCollectionHelper_BASE
{
private:
    uno::Reference< frame::XModel > mxModel;
    uno::Reference< container::XNameAccess > mxStyleFamilies;
    uno::Reference< container::XNameContainer > mxCurrentStyleFamily;
    uno::Any cachePos;
public:
    StyleCollectionHelper( const uno::Reference< frame::XModel >& _xModel ) : mxModel( _xModel )
    {
        uno::Reference< style::XStyleFamiliesSupplier > xStyleSupplier( _xModel, uno::UNO_QUERY_THROW);
        mxStyleFamilies = xStyleSupplier->getStyleFamilies();
    }
    uno::Reference< container::XNameContainer > getCurrentStyleFamily(){ return mxCurrentStyleFamily; }
    // XElementAccess
    virtual uno::Type SAL_CALL getElementType(  ) throw (uno::RuntimeException) { return  style::XStyle::static_type(0); }
    virtual ::sal_Bool SAL_CALL hasElements(  ) throw (uno::RuntimeException) { return getCount() > 0; }
    // XNameAcess 
    virtual uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException)
    {
        if ( !hasByName(aName) )
            throw container::NoSuchElementException();
        return cachePos;
    }
    virtual uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< rtl::OUString > sNames( getCount() );
        rtl::OUString* pString = sNames.getArray();
        uno::Sequence< rtl::OUString > aStyleTypes = getStyleTypes();
        sal_Int32 nCount = 0;
        for( sal_Int32 i = 0; i < aStyleTypes.getLength(); i++ )
        {
            uno::Reference< container::XNameAccess > xNameAccess( mxStyleFamilies->getByName( aStyleTypes[i] ), uno::UNO_QUERY_THROW );
            uno::Sequence< rtl::OUString > sElementNames = xNameAccess->getElementNames();
            for( sal_Int32 j = 0; j < sElementNames.getLength(); j++ )
            {
                pString[nCount++] = sElementNames[j];
            }
        }
        return sNames;
    }
    virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (uno::RuntimeException)
    {
        uno::Sequence< rtl::OUString > aStyleTypes = getStyleTypes();
        for( sal_Int32 i = 0; i < aStyleTypes.getLength(); i++ )
        {
            uno::Reference< container::XNameAccess > xNameAccess( mxStyleFamilies->getByName( aStyleTypes[i] ), uno::UNO_QUERY_THROW );
            if( xNameAccess->hasByName( aName ) )
            {
                cachePos = xNameAccess->getByName( aName );
                mxCurrentStyleFamily.set( xNameAccess, uno::UNO_QUERY_THROW );
                return sal_True;
            }
        }
        return sal_False;
    }

    // XIndexAccess
    virtual ::sal_Int32 SAL_CALL getCount(  ) throw (uno::RuntimeException)
    { 
        uno::Sequence< rtl::OUString > aStyleTypes = getStyleTypes();
        sal_Int32 nCount = 0;
        for( sal_Int32 i = 0; i < aStyleTypes.getLength(); i++ )
        {
            uno::Reference< container::XIndexAccess > xIndexAccess( mxStyleFamilies->getByName( aStyleTypes[i] ), uno::UNO_QUERY_THROW );
            nCount += xIndexAccess->getCount();
        }
        return nCount;
    }	
    virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw (lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
    {
        if ( Index < 0 || Index >= getCount() )
            throw lang::IndexOutOfBoundsException();
    
        // FIXME: need to make a alphabetically sorted list of style names
        uno::Sequence< rtl::OUString > aStyleTypes = getStyleTypes();
        for( sal_Int32 i = 0; i < aStyleTypes.getLength(); i++ )
        {
            uno::Reference< container::XIndexAccess > xIndexAccess( mxStyleFamilies->getByName( aStyleTypes[i] ), uno::UNO_QUERY_THROW );
            sal_Int32 nCount = xIndexAccess->getCount();
            if( Index >= nCount )
                Index -= nCount;
            else
            {
                mxCurrentStyleFamily.set( xIndexAccess, uno::UNO_QUERY_THROW );
                return xIndexAccess->getByIndex( Index );
            }
        }
        throw lang::IndexOutOfBoundsException();
    }
    // XEnumerationAccess
    virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration(  ) throw (uno::RuntimeException)
    {
        // FIXME:
        throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Not implemented") ), uno::Reference< uno::XInterface >() );
    }
};

SwVbaStyles::SwVbaStyles( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< css::uno::XComponentContext > & xContext, const uno::Reference< frame::XModel >& xModel ) throw ( script::BasicErrorException ) : SwVbaStyles_BASE( xParent, xContext, uno::Reference< container::XIndexAccess >( new StyleCollectionHelper( xModel )  ) ), mxModel( xModel )
{
    mxMSF.set( mxModel, uno::UNO_QUERY_THROW );
}

uno::Any 
SwVbaStyles::createCollectionObject(const uno::Any& aObject)
{
    uno::Reference< beans::XPropertySet > xStyleProp( aObject, uno::UNO_QUERY_THROW );
    return uno::makeAny( uno::Reference< word::XStyle >( new SwVbaStyle( this, mxContext, xStyleProp ) ) );
}

uno::Type SAL_CALL 
SwVbaStyles::getElementType() throw (uno::RuntimeException)
{
    return word::XStyle::static_type(0);
}

uno::Reference< container::XEnumeration > SAL_CALL 
SwVbaStyles::createEnumeration() throw (uno::RuntimeException)
{
    throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Not implemented") ), uno::Reference< uno::XInterface >() );
}

uno::Any SAL_CALL
SwVbaStyles::Item( const uno::Any& Index1, const uno::Any& Index2 ) throw (uno::RuntimeException)
{
    //handle WdBuiltinStyle
    sal_Int32 nIndex = 0;
    if( ( Index1 >>= nIndex ) && ( nIndex < 0 ) )
    {
        for( const BuiltinStyleTable* pTable = aBuiltinStyleTable; pTable != NULL; pTable++ )
        {
            if( nIndex == pTable->wdBuiltinStyle )
            {
                rtl::OUString aStyleName = rtl::OUString::createFromAscii( pTable->pOOoStyleName );
                if( aStyleName.getLength() > 0 )
                {
                    rtl::OUString aStyleType = SwVbaStyle::getOOoStyleTypeFromMSWord( pTable->wdStyleType );
                    switch( pTable->wdStyleType )
                    {
                        case word::WdStyleType::wdStyleTypeParagraph:
                        case word::WdStyleType::wdStyleTypeTable:
                        {
                            aStyleType = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ParagraphStyles") );
                            break;
                        }
                        case word::WdStyleType::wdStyleTypeCharacter:
                        {
                            aStyleType = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("CharacterStyles") );
                            break;
                        }
                        case word::WdStyleType::wdStyleTypeList:
                        {
                            aStyleType = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("NumberingStyles") );
                            break;
                        }
                        default:
                            DebugHelper::exception( SbERR_INTERNAL_ERROR, rtl::OUString() );
                    }
                    uno::Reference< style::XStyleFamiliesSupplier > xStyleSupplier( mxModel, uno::UNO_QUERY_THROW);
                    uno::Reference< container::XNameAccess > xStylesAccess( xStyleSupplier->getStyleFamilies()->getByName( aStyleType ), uno::UNO_QUERY_THROW );
                    uno::Reference< beans::XPropertySet > xStyleProps( xStylesAccess->getByName( aStyleName ), uno::UNO_QUERY_THROW );
                    return uno::makeAny( uno::Reference< word::XStyle >( new SwVbaStyle( this, mxContext, xStyleProps ) ) );
                }
                else
                {
                    OSL_TRACE("SwVbaStyles::Item: the builtin style type is not implemented");
                    throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Not implemented") ), uno::Reference< uno::XInterface >() );
                }
            }
        }
    }
    return SwVbaStyles_BASE::Item( Index1, Index2 );
}

rtl::OUString&
SwVbaStyles::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("SwVbaStyles") );
    return sImplName;
}

uno::Sequence< rtl::OUString >
SwVbaStyles::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.XStyles" ) );
    }
    return aServiceNames;
}
