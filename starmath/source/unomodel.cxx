/*************************************************************************
 *
 *  $RCSfile: unomodel.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: tl $ $Date: 2001-05-17 10:02:06 $
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

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _SFX_PRINTER_HXX
#include <sfx2/printer.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _CTRLTOOL_HXX
#include <svtools/ctrltool.hxx>
#endif
#ifndef _SFX_ITEMPROP_HXX
#include <svtools/itemprop.hxx>
#endif
#ifndef UNOMODEL_HXX
#include "unomodel.hxx"
#endif
#ifndef DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSTATE_HPP_
#include <com/sun/star/beans/PropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_FORMULA_SYMBOLDESCRIPTOR_HPP_
#include <com/sun/star/formula/SymbolDescriptor.hpp>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _COMPHELPER_PROPERTSETINFO_HXX_ 
#include <comphelper/propertysetinfo.hxx>
#endif
#ifndef SYMBOL_HXX
#include <symbol.hxx>
#endif


using namespace ::vos;
using namespace ::rtl;
using namespace ::cppu;
using namespace ::std;
using namespace ::comphelper;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::formula;

#define TWIP_TO_MM100(TWIP) 	((TWIP) >= 0 ? (((TWIP)*127L+36L)/72L) : (((TWIP)*127L-36L)/72L))
#define MM100_TO_TWIP(MM100)	((MM100) >= 0 ? (((MM100)*72L+63L)/127L) : (((MM100)*72L-63L)/127L))

#define C2U(cChar) 	rtl::OUString::createFromAscii(cChar)

////////////////////////////////////////
//
// class SmModel
//
#define PROPERTY_NONE 0
enum SmModelPropertyHandles
{
    HANDLE_FORMULA,
    HANDLE_FONT_NAME_VARIABLES,
    HANDLE_FONT_NAME_FUNCTIONS,
    HANDLE_FONT_NAME_NUMBERS,
    HANDLE_FONT_NAME_TEXT,
    HANDLE_CUSTOM_FONT_NAME_SERIF,
    HANDLE_CUSTOM_FONT_NAME_SANS,
    HANDLE_CUSTOM_FONT_NAME_FIXED,
    HANDLE_CUSTOM_FONT_FIXED_POSTURE,
    HANDLE_CUSTOM_FONT_FIXED_WEIGHT,
    HANDLE_CUSTOM_FONT_SANS_POSTURE,
    HANDLE_CUSTOM_FONT_SANS_WEIGHT,
    HANDLE_CUSTOM_FONT_SERIF_POSTURE,
    HANDLE_CUSTOM_FONT_SERIF_WEIGHT,
    HANDLE_FONT_VARIABLES_POSTURE,
    HANDLE_FONT_VARIABLES_WEIGHT,
    HANDLE_FONT_FUNCTIONS_POSTURE,
    HANDLE_FONT_FUNCTIONS_WEIGHT,
    HANDLE_FONT_NUMBERS_POSTURE,
    HANDLE_FONT_NUMBERS_WEIGHT,
    HANDLE_FONT_TEXT_POSTURE,
    HANDLE_FONT_TEXT_WEIGHT,
    HANDLE_BASE_FONT_HEIGHT,
    HANDLE_RELATIVE_FONT_HEIGHT_TEXT,
    HANDLE_RELATIVE_FONT_HEIGHT_INDICES,
    HANDLE_RELATIVE_FONT_HEIGHT_FUNCTIONS,
    HANDLE_RELATIVE_FONT_HEIGHT_OPERATORS,
    HANDLE_RELATIVE_FONT_HEIGHT_LIMITS,
    HANDLE_IS_TEXT_MODE,
    HANDLE_ALIGNMENT,
    HANDLE_RELATIVE_SPACING,
    HANDLE_RELATIVE_LINE_SPACING,
    HANDLE_RELATIVE_ROOT_SPACING,
    HANDLE_RELATIVE_INDEX_SUPERSCRIPT,
    HANDLE_RELATIVE_INDEX_SUBSCRIPT,
    HANDLE_RELATIVE_FRACTION_NUMERATOR_HEIGHT,
    HANDLE_RELATIVE_FRACTION_DENOMINATOR_DEPTH,
    HANDLE_RELATIVE_FRACTION_BAR_EXCESS_LENGTH,
    HANDLE_RELATIVE_FRACTION_BAR_LINE_WEIGHT,
    HANDLE_RELATIVE_UPPER_LIMIT_DISTANCE,
    HANDLE_RELATIVE_LOWER_LIMIT_DISTANCE,
    HANDLE_RELATIVE_BRACKET_EXCESS_SIZE,
    HANDLE_RELATIVE_BRACKET_DISTANCE,
    HANDLE_IS_SCALE_ALL_BRACKETS,
    HANDLE_RELATIVE_SCALE_BRACKET_EXCESS_SIZE,
    HANDLE_RELATIVE_MATRIX_LINE_SPACING,
    HANDLE_RELATIVE_MATRIX_COLUMN_SPACING,
    HANDLE_RELATIVE_SYMBOL_PRIMARY_HEIGHT,
    HANDLE_RELATIVE_SYMBOL_MINIMUM_HEIGHT,
    HANDLE_RELATIVE_OPERATOR_EXCESS_SIZE,
    HANDLE_RELATIVE_OPERATOR_SPACING,
    HANDLE_LEFT_MARGIN,
    HANDLE_RIGHT_MARGIN,
    HANDLE_TOP_MARGIN,
    HANDLE_BOTTOM_MARGIN,
    HANDLE_PRINTER_NAME,
    HANDLE_PRINTER_SETUP,
    HANDLE_SYMBOLS
};

PropertySetInfo * lcl_createModelPropertyInfo ()
{
    static PropertyMapEntry aModelPropertyInfoMap[] = 
    {
        { RTL_CONSTASCII_STRINGPARAM( "Alignment"                          ), HANDLE_ALIGNMENT                          , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, 0},
        { RTL_CONSTASCII_STRINGPARAM( "BaseFontHeight"                  ), HANDLE_BASE_FONT_HEIGHT                   , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, 0},
        { RTL_CONSTASCII_STRINGPARAM( "BottomMargin"            		  ), HANDLE_BOTTOM_MARGIN              		   , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_BOTTOMSPACE			   },
        { RTL_CONSTASCII_STRINGPARAM( "CustomFontNameFixed"            ), HANDLE_CUSTOM_FONT_NAME_FIXED             , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, FNT_FIXED		  },
        { RTL_CONSTASCII_STRINGPARAM( "CustomFontNameSans"              ), HANDLE_CUSTOM_FONT_NAME_SANS              , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, FNT_SANS		   },
        { RTL_CONSTASCII_STRINGPARAM( "CustomFontNameSerif"             ), HANDLE_CUSTOM_FONT_NAME_SERIF             , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, FNT_SERIF		  },
        { RTL_CONSTASCII_STRINGPARAM( "FontFixedIsBold"),	  HANDLE_CUSTOM_FONT_FIXED_WEIGHT    ,  &::getBooleanCppuType(),  PROPERTY_NONE, FNT_FIXED},
        { RTL_CONSTASCII_STRINGPARAM( "FontFixedIsItalic"), HANDLE_CUSTOM_FONT_FIXED_POSTURE   ,  &::getBooleanCppuType(),  PROPERTY_NONE, FNT_FIXED},
        { RTL_CONSTASCII_STRINGPARAM( "FontFunctionsIsBold"),	 HANDLE_FONT_FUNCTIONS_WEIGHT    ,  &::getBooleanCppuType(),  			PROPERTY_NONE, FNT_FUNCTION},
        { RTL_CONSTASCII_STRINGPARAM( "FontFunctionsIsItalic"),   HANDLE_FONT_FUNCTIONS_POSTURE   ,  &::getBooleanCppuType(),  PROPERTY_NONE, FNT_FUNCTION},
        { RTL_CONSTASCII_STRINGPARAM( "FontNameFunctions"                ), HANDLE_FONT_NAME_FUNCTIONS                , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, FNT_FUNCTION	},
        { RTL_CONSTASCII_STRINGPARAM( "FontNameNumbers"                  ), HANDLE_FONT_NAME_NUMBERS                  , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, FNT_NUMBER		 },
        { RTL_CONSTASCII_STRINGPARAM( "FontNameText"                     ), HANDLE_FONT_NAME_TEXT                     , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, FNT_TEXT		   },
        { RTL_CONSTASCII_STRINGPARAM( "FontNameVariables"                ), HANDLE_FONT_NAME_VARIABLES                , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, FNT_VARIABLE },
        { RTL_CONSTASCII_STRINGPARAM( "FontNumbersIsBold"),	 HANDLE_FONT_NUMBERS_WEIGHT    ,  &::getBooleanCppuType(),  			PROPERTY_NONE, FNT_NUMBER},
        { RTL_CONSTASCII_STRINGPARAM( "FontNumbersIsItalic"),   HANDLE_FONT_NUMBERS_POSTURE   ,  &::getBooleanCppuType(),  PROPERTY_NONE, FNT_NUMBER},
        { RTL_CONSTASCII_STRINGPARAM( "FontSansIsBold"),	 HANDLE_CUSTOM_FONT_SANS_WEIGHT    ,  &::getBooleanCppuType(),  			PROPERTY_NONE, FNT_SANS},
        { RTL_CONSTASCII_STRINGPARAM( "FontSansIsItalic"),   HANDLE_CUSTOM_FONT_SANS_POSTURE   ,  &::getBooleanCppuType(),  PROPERTY_NONE, FNT_SANS},
        { RTL_CONSTASCII_STRINGPARAM( "FontSerifIsBold"),	 HANDLE_CUSTOM_FONT_SERIF_WEIGHT    ,  &::getBooleanCppuType(),  			PROPERTY_NONE,  FNT_SERIF},
        { RTL_CONSTASCII_STRINGPARAM( "FontSerifIsItalic"),   HANDLE_CUSTOM_FONT_SERIF_POSTURE   ,  &::getBooleanCppuType(),  PROPERTY_NONE, FNT_SERIF},
        { RTL_CONSTASCII_STRINGPARAM( "FontTextIsBold"),	 HANDLE_FONT_TEXT_WEIGHT    ,  &::getBooleanCppuType(),  			PROPERTY_NONE, },
        { RTL_CONSTASCII_STRINGPARAM( "FontTextIsItalic"),   HANDLE_FONT_TEXT_POSTURE   ,  &::getBooleanCppuType(),  PROPERTY_NONE, },
        { RTL_CONSTASCII_STRINGPARAM( "FontVariablesIsBold"),	 HANDLE_FONT_VARIABLES_WEIGHT    ,  &::getBooleanCppuType(),  			PROPERTY_NONE, },
        { RTL_CONSTASCII_STRINGPARAM( "FontVariablesIsItalic"),   HANDLE_FONT_VARIABLES_POSTURE,  &::getBooleanCppuType(),  PROPERTY_NONE, },
        { RTL_CONSTASCII_STRINGPARAM( "Formula"							  ),	HANDLE_FORMULA							   , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, 0},
        { RTL_CONSTASCII_STRINGPARAM( "IsScaleAllBrackets"              ), HANDLE_IS_SCALE_ALL_BRACKETS              , 		&::getBooleanCppuType(), 	PROPERTY_NONE, 0},
        { RTL_CONSTASCII_STRINGPARAM( "IsTextMode"                       ), HANDLE_IS_TEXT_MODE                       , 		&::getBooleanCppuType(), 	PROPERTY_NONE, 0},
        { RTL_CONSTASCII_STRINGPARAM( "LeftMargin"               		  ), HANDLE_LEFT_MARGIN        		           , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_LEFTSPACE			     },
        { RTL_CONSTASCII_STRINGPARAM( "PrinterName"                	   ), HANDLE_PRINTER_NAME               		 , 		&::getCppuType((const OUString*)0), 	PROPERTY_NONE, 0			      },
        { RTL_CONSTASCII_STRINGPARAM( "PrinterSetup"                	   ), HANDLE_PRINTER_SETUP               		 , 		&::getCppuType((const Sequence < sal_Int8 >*)0), 	PROPERTY_NONE, 0			      },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeBracketDistance"          ), HANDLE_RELATIVE_BRACKET_DISTANCE          , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_BRACKETSPACE	},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeBracketExcessSize"       ), HANDLE_RELATIVE_BRACKET_EXCESS_SIZE       , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_BRACKETSIZE 	},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFontHeightFunctions"     ), HANDLE_RELATIVE_FONT_HEIGHT_FUNCTIONS     , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, SIZ_FUNCTION},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFontHeightIndices"       ), HANDLE_RELATIVE_FONT_HEIGHT_INDICES       , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, SIZ_INDEX	  },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFontHeightLimits"        ), HANDLE_RELATIVE_FONT_HEIGHT_LIMITS        , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, SIZ_LIMITS	 },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFontHeightOperators"     ), HANDLE_RELATIVE_FONT_HEIGHT_OPERATORS     , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, SIZ_OPERATOR},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFontHeightText"       	  ), HANDLE_RELATIVE_FONT_HEIGHT_TEXT          , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, SIZ_TEXT	  },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFractionBarExcessLength"), HANDLE_RELATIVE_FRACTION_BAR_EXCESS_LENGTH, 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_FRACTION		   },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFractionBarLineWeight"  ), HANDLE_RELATIVE_FRACTION_BAR_LINE_WEIGHT  , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_STROKEWIDTH 	},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFractionDenominatorDepth"), HANDLE_RELATIVE_FRACTION_DENOMINATOR_DEPTH, 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_DENOMINATOR 	},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeFractionNumeratorHeight" ), HANDLE_RELATIVE_FRACTION_NUMERATOR_HEIGHT , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_NUMERATOR		  },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeIndexSubscript"           ), HANDLE_RELATIVE_INDEX_SUBSCRIPT           , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_SUBSCRIPT		  },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeIndexSuperscript"         ), HANDLE_RELATIVE_INDEX_SUPERSCRIPT         , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_SUPERSCRIPT 	},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeLineSpacing"              ), HANDLE_RELATIVE_LINE_SPACING              , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_VERTICAL		   },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeLowerLimitDistance"      ), HANDLE_RELATIVE_LOWER_LIMIT_DISTANCE      , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_LOWERLIMIT		 },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeMatrixColumnSpacing"     ), HANDLE_RELATIVE_MATRIX_COLUMN_SPACING     , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_MATRIXCOL},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeMatrixLineSpacing"       ), HANDLE_RELATIVE_MATRIX_LINE_SPACING       , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_MATRIXROW},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeOperatorExcessSize"      ), HANDLE_RELATIVE_OPERATOR_EXCESS_SIZE      , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_OPERATORSIZE		   },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeOperatorSpacing"          ), HANDLE_RELATIVE_OPERATOR_SPACING          , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_OPERATORSPACE},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeRootSpacing"              ), HANDLE_RELATIVE_ROOT_SPACING              , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_ROOT			      },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeScaleBracketExcessSize" ), HANDLE_RELATIVE_SCALE_BRACKET_EXCESS_SIZE , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_NORMALBRACKETSIZE},
        { RTL_CONSTASCII_STRINGPARAM( "RelativeSpacing"                   ), HANDLE_RELATIVE_SPACING                   , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_HORIZONTAL		 },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeSymbolMinimumHeight"     ), HANDLE_RELATIVE_SYMBOL_MINIMUM_HEIGHT     , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_ORNAMENTSPACE		 },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeSymbolPrimaryHeight"     ), HANDLE_RELATIVE_SYMBOL_PRIMARY_HEIGHT     , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_ORNAMENTSIZE		   },
        { RTL_CONSTASCII_STRINGPARAM( "RelativeUpperLimitDistance"      ), 	HANDLE_RELATIVE_UPPER_LIMIT_DISTANCE     , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_UPPERLIMIT		 },
        { RTL_CONSTASCII_STRINGPARAM( "RightMargin"              		  ), 	HANDLE_RIGHT_MARGIN              	 , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_RIGHTSPACE			    },
        { RTL_CONSTASCII_STRINGPARAM( "Symbols"              		  ), 		HANDLE_SYMBOLS              		 , 		&::getCppuType((const Sequence < SymbolDescriptor > *)0), 	PROPERTY_NONE, 0  },
        { RTL_CONSTASCII_STRINGPARAM( "TopMargin"                		  ), 	HANDLE_TOP_MARGIN                	 , 		&::getCppuType((const sal_Int16*)0), 	PROPERTY_NONE, DIS_TOPSPACE			      },
        { NULL, 0, 0, NULL, 0, 0 }
    };
    PropertySetInfo *pInfo = new PropertySetInfo ( aModelPropertyInfoMap );
    return pInfo;
}
//-----------------------------------------------------------------------
SmModel::SmModel( SfxObjectShell *pObjSh )
: SfxBaseModel(pObjSh)
, PropertySetHelper ( lcl_createModelPropertyInfo () )  
{
}
//-----------------------------------------------------------------------
SmModel::~SmModel()
{
}
/*-- 28.03.00 14:18:17---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Any SAL_CALL SmModel::queryInterface( const uno::Type& rType ) throw(uno::RuntimeException)
{
    uno::Any aRet =  ::cppu::queryInterface ( rType										,
                                    // OWeakObject interfaces
                                    reinterpret_cast< XInterface*		> ( this )	,
                                    static_cast< XWeak*			> ( this )	,
                                    // PropertySetHelper interfaces
                                    static_cast< XPropertySet*		> ( this )	,
                                    static_cast< XMultiPropertySet*		> ( this )	,
                                    static_cast< XPropertyState*		> ( this )	,
                                    // my own interfaces
                                    static_cast< XUnoTunnel*		> ( this )	,
                                    static_cast< XServiceInfo*	> ( this ) );
    if (!aRet.hasValue())
        aRet = SfxBaseModel::queryInterface ( rType );
    return aRet;
}
/*-- 28.03.00 14:18:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SmModel::acquire() throw(uno::RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    OWeakObject::acquire();
}
/*-- 28.03.00 14:18:18---------------------------------------------------

  -----------------------------------------------------------------------*/
void SAL_CALL SmModel::release() throw(uno::RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    OWeakObject::release();
}
/*-- 28.03.00 14:18:19---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< uno::Type > SAL_CALL SmModel::getTypes(  ) throw(uno::RuntimeException)
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());
    return SfxBaseModel::getTypes();
}
/* -----------------------------28.03.00 14:23--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & SmModel::getUnoTunnelId()
{
    static osl::Mutex aCreateMutex;
    osl::Guard<osl::Mutex> aGuard( aCreateMutex );

    static uno::Sequence< sal_Int8 > aSeq;
    if(!aSeq.getLength())
    {
        aSeq.realloc( 16 );
        rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0,	sal_True );
    }
    return aSeq;
} /* -----------------------------28.03.00 14:23--------------------------------

 ---------------------------------------------------------------------------*/
sal_Int64 SAL_CALL SmModel::getSomething( const uno::Sequence< sal_Int8 >& rId )
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
/*-- 07.01.00 16:32:59---------------------------------------------------

  -----------------------------------------------------------------------*/
/*-- 07.01.00 16:33:00---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int16 lcl_AnyToINT16(const uno::Any& rAny)
{
    uno::TypeClass eType = rAny.getValueType().getTypeClass();

    sal_Int16 nRet = 0;
    if( eType == uno::TypeClass_DOUBLE )
        nRet = (sal_Int16)*(double*)rAny.getValue();
    else if( eType == uno::TypeClass_FLOAT )
        nRet = (sal_Int16)*(float*)rAny.getValue();
    else
        rAny >>= nRet;
    return nRet;
}
//-----------------------------------------------------------------------------

OUString SmModel::getImplementationName(void) throw( uno::RuntimeException )
{
    return C2U("SmModel");
}
/*-- 07.02.00 13:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool SmModel::supportsService(const OUString& rServiceName) throw( uno::RuntimeException )
{
    return rServiceName == C2U("com.sun.star.formula.FormulaProperties");
}
/*-- 07.02.00 13:24:09---------------------------------------------------

  -----------------------------------------------------------------------*/
uno::Sequence< OUString > SmModel::getSupportedServiceNames(void) throw( uno::RuntimeException )
{
    ::vos::OGuard aGuard(Application::GetSolarMutex());

    uno::Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.formula.FormulaProperties");
    return aRet;
}

void SmModel::_setPropertyValues(const PropertyMapEntry** ppEntries, const Any* pValues)
    throw( UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException)
{
    SmDocShell *pDocSh = static_cast < SmDocShell * > (GetObjectShell());

    if ( NULL == pDocSh )
        throw UnknownPropertyException();

    SmFormat & aFormat = pDocSh->GetFormat();

    for (; *ppEntries; ppEntries++, pValues++ )
    {
        switch ( (*ppEntries)->mnHandle )
        {
            case HANDLE_FORMULA:
            {
                OUString aText;
                *pValues >>= aText;
                pDocSh->SetText(aText);
            }
            break;
            case HANDLE_FONT_NAME_VARIABLES                :
            case HANDLE_FONT_NAME_FUNCTIONS                :
            case HANDLE_FONT_NAME_NUMBERS                  :
            case HANDLE_FONT_NAME_TEXT                     :
            case HANDLE_CUSTOM_FONT_NAME_SERIF             :
            case HANDLE_CUSTOM_FONT_NAME_SANS              :
            case HANDLE_CUSTOM_FONT_NAME_FIXED             :
            {
                OUString aText;
                *pValues >>= aText;
                String sFontName = aText;
                if(!sFontName.Len())
                    throw IllegalArgumentException();
                if(aFormat.GetFont((*ppEntries)->mnMemberId).GetName() != sFontName)
                {
                    OutputDevice *pDev = pDocSh->GetPrinter();
                    if (!pDev || pDev->GetDevFontCount() == 0)
                        pDev = (OutputDevice *) GetpApp()->GetDefaultDevice();

                    FontList aFontList(pDev);

                    sal_uInt16	nCount = aFontList.GetFontNameCount();
                    sal_Bool bSet = sal_False;
                    for (sal_uInt16 i = 0;  i < nCount;  i++)
                    {
                        const FontInfo& rInfo = aFontList.GetFontName( i );
                        if(rInfo.GetName() == sFontName)
                        {
                            SmFace aSet(rInfo);
                            const SmFace rOld = aFormat.GetFont((*ppEntries)->mnMemberId);
                            aSet.SetBorderWidth(rOld.GetBorderWidth());
                            aSet.SetSize(rOld.GetSize());
                            aSet.SetAlign(ALIGN_BASELINE);
                            aFormat.SetFont((*ppEntries)->mnMemberId, aSet);
                            bSet = sal_True;
                            break;
                        }
                    }
                    if(!bSet)
                        throw IllegalArgumentException();
                }
            }
            break;
            case HANDLE_CUSTOM_FONT_FIXED_POSTURE:
            case HANDLE_CUSTOM_FONT_SANS_POSTURE :
            case HANDLE_CUSTOM_FONT_SERIF_POSTURE:
            case HANDLE_FONT_VARIABLES_POSTURE   :
            case HANDLE_FONT_FUNCTIONS_POSTURE   :
            case HANDLE_FONT_NUMBERS_POSTURE     :
            case HANDLE_FONT_TEXT_POSTURE        :
            {
                if((*pValues).getValueType() != ::getBooleanCppuType())
                    throw IllegalArgumentException();
                BOOL bVal = *(sal_Bool*)(*pValues).getValue();
                Font aNewFont(aFormat.GetFont((*ppEntries)->mnMemberId));
                aNewFont.SetItalic((bVal) ? ITALIC_NORMAL : ITALIC_NONE);
                aFormat.SetFont((*ppEntries)->mnMemberId, aNewFont);
            }
            break;
            case HANDLE_CUSTOM_FONT_FIXED_WEIGHT :
            case HANDLE_CUSTOM_FONT_SANS_WEIGHT  :
            case HANDLE_CUSTOM_FONT_SERIF_WEIGHT :
            case HANDLE_FONT_VARIABLES_WEIGHT    :
            case HANDLE_FONT_FUNCTIONS_WEIGHT    :
            case HANDLE_FONT_NUMBERS_WEIGHT      :
            case HANDLE_FONT_TEXT_WEIGHT         :
            {
                if((*pValues).getValueType() != ::getBooleanCppuType())
                    throw IllegalArgumentException();
                BOOL bVal = *(sal_Bool*)(*pValues).getValue();
                Font aNewFont(aFormat.GetFont((*ppEntries)->mnMemberId));
                aNewFont.SetWeight((bVal) ? WEIGHT_BOLD : WEIGHT_NORMAL);
                aFormat.SetFont((*ppEntries)->mnMemberId, aNewFont);
            }
            break;
            case HANDLE_BASE_FONT_HEIGHT                   :
            {
                // Point!
                sal_Int16 nVal = lcl_AnyToINT16(*pValues);
                if(nVal < 1)
                    throw IllegalArgumentException();
                Size aSize = aFormat.GetBaseSize();
                nVal *= 20;
                nVal = static_cast < sal_Int16 > ( TWIP_TO_MM100(nVal) );
                aSize.Height() = nVal;
                aFormat.SetBaseSize(aSize);
            }
            break;
            case HANDLE_RELATIVE_FONT_HEIGHT_TEXT       	:
            case HANDLE_RELATIVE_FONT_HEIGHT_INDICES       :
            case HANDLE_RELATIVE_FONT_HEIGHT_FUNCTIONS     :
            case HANDLE_RELATIVE_FONT_HEIGHT_OPERATORS     :
            case HANDLE_RELATIVE_FONT_HEIGHT_LIMITS        :
            {
                sal_Int16 nVal;
                *pValues >>= nVal;
                if(nVal < 1)
                    throw IllegalArgumentException();
                aFormat.SetRelSize((*ppEntries)->mnMemberId, nVal);
            }
            break;

            case HANDLE_IS_TEXT_MODE                       :
            {
                aFormat.SetTextmode(*(sal_Bool*)(*pValues).getValue());
            }
            break;

            case HANDLE_ALIGNMENT                          :
            {
                // SmHorAlign uses the same values as HorizontalAlignment
                sal_Int16 nVal;
                *pValues >>= nVal;
                if(nVal < 0 || nVal > 2)
                    throw IllegalArgumentException();
                aFormat.SetHorAlign((SmHorAlign)nVal);
            }
            break;

            case HANDLE_RELATIVE_SPACING                   :
            case HANDLE_RELATIVE_LINE_SPACING              :
            case HANDLE_RELATIVE_ROOT_SPACING              :
            case HANDLE_RELATIVE_INDEX_SUPERSCRIPT         :
            case HANDLE_RELATIVE_INDEX_SUBSCRIPT           :
            case HANDLE_RELATIVE_FRACTION_NUMERATOR_HEIGHT :
            case HANDLE_RELATIVE_FRACTION_DENOMINATOR_DEPTH:
            case HANDLE_RELATIVE_FRACTION_BAR_EXCESS_LENGTH:
            case HANDLE_RELATIVE_FRACTION_BAR_LINE_WEIGHT  :
            case HANDLE_RELATIVE_UPPER_LIMIT_DISTANCE      :
            case HANDLE_RELATIVE_LOWER_LIMIT_DISTANCE      :
            case HANDLE_RELATIVE_BRACKET_EXCESS_SIZE       :
            case HANDLE_RELATIVE_BRACKET_DISTANCE          :
            case HANDLE_RELATIVE_SCALE_BRACKET_EXCESS_SIZE :
            case HANDLE_RELATIVE_MATRIX_LINE_SPACING       :
            case HANDLE_RELATIVE_MATRIX_COLUMN_SPACING     :
            case HANDLE_RELATIVE_SYMBOL_PRIMARY_HEIGHT     :
            case HANDLE_RELATIVE_SYMBOL_MINIMUM_HEIGHT     :
            case HANDLE_RELATIVE_OPERATOR_EXCESS_SIZE      :
            case HANDLE_RELATIVE_OPERATOR_SPACING          :
            case HANDLE_LEFT_MARGIN               :
            case HANDLE_RIGHT_MARGIN              :
            case HANDLE_TOP_MARGIN                :
            case HANDLE_BOTTOM_MARGIN             :
            {
                sal_Int16 nVal;
                *pValues >>= nVal;
                if(nVal < 0)
                    throw IllegalArgumentException();
                aFormat.SetDistance((*ppEntries)->mnMemberId, nVal);
            }
            break;
            case HANDLE_IS_SCALE_ALL_BRACKETS              :
                aFormat.SetScaleNormalBrackets(*(sal_Bool*)(*pValues).getValue());
            break;
            case HANDLE_PRINTER_NAME:
            {
                SfxPrinter *pPrinter = pDocSh->GetPrinter ( );
                if (pPrinter)
                {
                    OUString sPrinterName;
                    if (*pValues >>= sPrinterName )
                    {
                        SfxPrinter *pNewPrinter = new SfxPrinter ( pPrinter->GetOptions().Clone(), sPrinterName );
                        if (pNewPrinter->IsKnown())
                            pDocSh->SetPrinter ( pNewPrinter );
                        else
                            delete pNewPrinter;
                    }
                    else
                        throw IllegalArgumentException();
                }
            }
            break;
            case HANDLE_PRINTER_SETUP:
            {
                Sequence < sal_Int8 > aSequence;
                if ( *pValues >>= aSequence )
                {
                    sal_uInt32 nSize = aSequence.getLength();
                    SvMemoryStream aStream ( aSequence.getArray(), nSize, STREAM_READ );
                    aStream.Seek ( STREAM_SEEK_TO_BEGIN );
                    static sal_uInt16 __READONLY_DATA nRange[] =
                    {
                        SID_PRINTER_NOTFOUND_WARN, SID_PRINTER_NOTFOUND_WARN,
                        SID_PRINTER_CHANGESTODOC, SID_PRINTER_CHANGESTODOC,
                        0
                    };
                    SfxItemSet *pItemSet = new SfxItemSet( pDocSh->GetPool(), nRange );
                    SfxPrinter *pPrinter = SfxPrinter::Create ( aStream, pItemSet );

                    pDocSh->SetPrinter( pPrinter );
                }
                else
                    throw IllegalArgumentException();
            }
            break;
            case HANDLE_SYMBOLS:
            {
                // this is set
                Sequence < SymbolDescriptor > aSequence;
                if ( *pValues >>= aSequence )
                {
                    sal_uInt32 nSize = aSequence.getLength();
                    SmSymSetManager &rManager = pDocSh->GetSymSetManager();
                    SymbolDescriptor *pDescriptor = aSequence.getArray();
                    for (sal_uInt32 i = 0; i < nSize ; i++, pDescriptor++)
                    {
                        Font aFont;
                        aFont.SetName ( pDescriptor->sFontName );
                        aFont.SetCharSet ( static_cast < rtl_TextEncoding > (pDescriptor->nCharSet) );
                        aFont.SetFamily ( static_cast < FontFamily > (pDescriptor->nFamily ) );
                        aFont.SetPitch  ( static_cast < FontPitch >  (pDescriptor->nPitch ) );
                        aFont.SetWeight ( static_cast < FontWeight > (pDescriptor->nWeight ) );
                        aFont.SetItalic ( static_cast < FontItalic > (pDescriptor->nItalic ) );
                        SmSym aSymbol ( pDescriptor->sName, aFont, static_cast < sal_Unicode > (pDescriptor->nCharacter),
                                        pDescriptor->sSymbolSet );
                        aSymbol.SetExportName ( pDescriptor->sExportName );
                        aSymbol.SetDocSymbol( TRUE );
                        rManager.AddReplaceSymbol ( aSymbol );
                    }
                }
                else
                    throw IllegalArgumentException();
            }
            break;
        }
    }
}

void SmModel::_getPropertyValues( const PropertyMapEntry **ppEntries, Any *pValue )
    throw( UnknownPropertyException, WrappedTargetException )
{
    SmDocShell *pDocSh = static_cast < SmDocShell * > (GetObjectShell());

    if ( NULL == pDocSh )
        throw UnknownPropertyException();

    SmFormat & aFormat = pDocSh->GetFormat();

    for (; *ppEntries; ppEntries++, pValue++ )
    {
        switch ( (*ppEntries)->mnHandle )
        {
            case HANDLE_FORMULA:
                *pValue <<= OUString(pDocSh->GetText());
            break;
            case HANDLE_FONT_NAME_VARIABLES                :
            case HANDLE_FONT_NAME_FUNCTIONS                :
            case HANDLE_FONT_NAME_NUMBERS                  :
            case HANDLE_FONT_NAME_TEXT                     :
            case HANDLE_CUSTOM_FONT_NAME_SERIF             :
            case HANDLE_CUSTOM_FONT_NAME_SANS              :
            case HANDLE_CUSTOM_FONT_NAME_FIXED             :
            {
                const SmFace &  rFace = aFormat.GetFont((*ppEntries)->mnMemberId);
                *pValue <<= OUString(rFace.GetName());
            }
            break;
            case HANDLE_CUSTOM_FONT_FIXED_POSTURE:
            case HANDLE_CUSTOM_FONT_SANS_POSTURE :
            case HANDLE_CUSTOM_FONT_SERIF_POSTURE:
            case HANDLE_FONT_VARIABLES_POSTURE   :
            case HANDLE_FONT_FUNCTIONS_POSTURE   :
            case HANDLE_FONT_NUMBERS_POSTURE     :
            case HANDLE_FONT_TEXT_POSTURE        :
            {
                const SmFace &  rFace = aFormat.GetFont((*ppEntries)->mnMemberId);
                BOOL bVal = (rFace.GetItalic() != ITALIC_NONE);
                (*pValue).setValue(&bVal, *(*ppEntries)->mpType);
            }
            break;
            case HANDLE_CUSTOM_FONT_FIXED_WEIGHT :
            case HANDLE_CUSTOM_FONT_SANS_WEIGHT  :
            case HANDLE_CUSTOM_FONT_SERIF_WEIGHT :
            case HANDLE_FONT_VARIABLES_WEIGHT    :
            case HANDLE_FONT_FUNCTIONS_WEIGHT    :
            case HANDLE_FONT_NUMBERS_WEIGHT      :
            case HANDLE_FONT_TEXT_WEIGHT         :
            {
                const SmFace &  rFace = aFormat.GetFont((*ppEntries)->mnMemberId);
                BOOL bVal = (rFace.GetWeight() == WEIGHT_BOLD);
                (*pValue).setValue(&bVal, *(*ppEntries)->mpType);
            }
            break;
            case HANDLE_BASE_FONT_HEIGHT                   :
            {
                // Point!
                sal_Int16 nVal = static_cast < sal_Int16 > (aFormat.GetBaseSize().Height());
                nVal = static_cast < sal_Int16 > (MM100_TO_TWIP(nVal));
                nVal = (nVal + 10) / 20;
                *pValue <<= nVal;
            }
            break;
            case HANDLE_RELATIVE_FONT_HEIGHT_TEXT       	:
            case HANDLE_RELATIVE_FONT_HEIGHT_INDICES       :
            case HANDLE_RELATIVE_FONT_HEIGHT_FUNCTIONS     :
            case HANDLE_RELATIVE_FONT_HEIGHT_OPERATORS     :
            case HANDLE_RELATIVE_FONT_HEIGHT_LIMITS        :
                *pValue <<= (sal_Int16) aFormat.GetRelSize((*ppEntries)->mnMemberId);
            break;

            case HANDLE_IS_TEXT_MODE                       :
            {
                sal_Bool bVal = aFormat.IsTextmode();
                (*pValue).setValue(&bVal, ::getBooleanCppuType());
            }
            break;

            case HANDLE_ALIGNMENT                          :
                // SmHorAlign uses the same values as HorizontalAlignment
                *pValue <<= (sal_Int16)aFormat.GetHorAlign();
            break;

            case HANDLE_RELATIVE_SPACING                   :
            case HANDLE_RELATIVE_LINE_SPACING              :
            case HANDLE_RELATIVE_ROOT_SPACING              :
            case HANDLE_RELATIVE_INDEX_SUPERSCRIPT         :
            case HANDLE_RELATIVE_INDEX_SUBSCRIPT           :
            case HANDLE_RELATIVE_FRACTION_NUMERATOR_HEIGHT :
            case HANDLE_RELATIVE_FRACTION_DENOMINATOR_DEPTH:
            case HANDLE_RELATIVE_FRACTION_BAR_EXCESS_LENGTH:
            case HANDLE_RELATIVE_FRACTION_BAR_LINE_WEIGHT  :
            case HANDLE_RELATIVE_UPPER_LIMIT_DISTANCE      :
            case HANDLE_RELATIVE_LOWER_LIMIT_DISTANCE      :
            case HANDLE_RELATIVE_BRACKET_EXCESS_SIZE       :
            case HANDLE_RELATIVE_BRACKET_DISTANCE          :
            case HANDLE_RELATIVE_SCALE_BRACKET_EXCESS_SIZE :
            case HANDLE_RELATIVE_MATRIX_LINE_SPACING       :
            case HANDLE_RELATIVE_MATRIX_COLUMN_SPACING     :
            case HANDLE_RELATIVE_SYMBOL_PRIMARY_HEIGHT     :
            case HANDLE_RELATIVE_SYMBOL_MINIMUM_HEIGHT     :
            case HANDLE_RELATIVE_OPERATOR_EXCESS_SIZE      :
            case HANDLE_RELATIVE_OPERATOR_SPACING          :
            case HANDLE_LEFT_MARGIN               :
            case HANDLE_RIGHT_MARGIN              :
            case HANDLE_TOP_MARGIN                :
            case HANDLE_BOTTOM_MARGIN             :
                *pValue <<= (sal_Int16)aFormat.GetDistance((*ppEntries)->mnMemberId);
            break;
            case HANDLE_IS_SCALE_ALL_BRACKETS              :
            {
                sal_Bool bVal = aFormat.IsScaleNormalBrackets();
                (*pValue).setValue(&bVal, ::getBooleanCppuType());
            }
            break;
            case HANDLE_PRINTER_NAME:
            {
                SfxPrinter *pPrinter = pDocSh->GetPrinter ( );
                *pValue <<= pPrinter ? OUString ( pPrinter->GetName()) : OUString();
            }
            break;
            case HANDLE_PRINTER_SETUP:
            {
                SfxPrinter *pPrinter = pDocSh->GetPrinter ();
                if (pPrinter)
                {
                    SvMemoryStream aStream;
                    pPrinter->Store( aStream );
                    aStream.Seek ( STREAM_SEEK_TO_END );
                    sal_uInt32 nSize = aStream.Tell();
                    aStream.Seek ( STREAM_SEEK_TO_BEGIN );
                    Sequence < sal_Int8 > aSequence ( nSize );
                    aStream.Read ( aSequence.getArray(), nSize );
                    *pValue <<= aSequence;
                }
            }
            break;
            case HANDLE_SYMBOLS:
            {
                // this is get
                SmSymSetManager &rManager = pDocSh->GetSymSetManager();
                vector < const SmSym * > aVector;
                
                USHORT nCount = 0;
                for (USHORT i = 0, nEnd = rManager.GetSymbolCount(); i < nEnd; i++)
                {
                    const SmSym * pSymbol = rManager.GetSymbol ( i );
                    if (!pSymbol->IsPredefined () )
                    {
                        aVector.push_back ( pSymbol );
                        nCount++;
                    }
                }
                Sequence < SymbolDescriptor > aSequence ( nCount );
                SymbolDescriptor * pDescriptor = aSequence.getArray();
            
                vector <const SmSym * >::const_iterator aIter = aVector.begin(), aEnd = aVector.end();
                for(; aIter != aEnd; pDescriptor++, aIter++)
                {
                    pDescriptor->sName = (*aIter)->GetName();
                    pDescriptor->sExportName = (*aIter)->GetExportName();
                    pDescriptor->sSymbolSet = (*aIter)->GetSetName();
                    pDescriptor->nCharacter = static_cast < sal_Int32 > ((*aIter)->GetCharacter());
                    
                    Font rFont = (*aIter)->GetFace();
                    pDescriptor->sFontName = rFont.GetName();
                    pDescriptor->nCharSet  = rFont.GetCharSet();
                    pDescriptor->nFamily   = rFont.GetFamily();
                    pDescriptor->nPitch    = rFont.GetPitch();
                    pDescriptor->nWeight   = rFont.GetWeight();
                    pDescriptor->nItalic   = rFont.GetItalic();
                }
                *pValue <<= aSequence;
            }
            break;
        }
    }
}
