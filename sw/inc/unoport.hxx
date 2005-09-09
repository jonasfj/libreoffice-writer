/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unoport.hxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:27:45 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#ifndef _UNOPORT_HXX
#define _UNOPORT_HXX

#ifndef _UNOEVTLSTNR_HXX
#include <unoevtlstnr.hxx>
#endif
#ifndef _CALBCK_HXX //autogen
#include <calbck.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE6_HXX_
#include <cppuhelper/implbase6.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTRANGE_HPP_
#include <com/sun/star/text/XTextRange.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTENTENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XEnumerationAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XMULTIPROPERTYSET_HPP_
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#endif
//#ifndef _COM_SUN_STAR_BEANS_XTOLERANTMULTIPROPERTYSET_HPP_
//#include <com/sun/star/beans/XTolerantMultiPropertySet.hpp>
//#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELD_HPP_
#include <com/sun/star/text/XTextField.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE8_HXX_
#include <cppuhelper/implbase8.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE9_HXX_
#include <cppuhelper/implbase9.hxx>
#endif
#ifndef _SFX_ITEMPROP_HXX
#include <svtools/itemprop.hxx>
#endif

class SwFmtFld;
class SwFrmFmt;
class SwUnoCrsr;
class SwRedline;
class SwTxtRuby;
/* -----------------29.05.98 14:42-------------------
 *
 * --------------------------------------------------*/
enum SwTextPortionType
{
    PORTION_TEXT,
    PORTION_FIELD,
    PORTION_FRAME,
    PORTION_FOOTNOTE,
    PORTION_CONTROL_CHAR,
    PORTION_REFMARK_START,
    PORTION_REFMARK_END,
    PORTION_TOXMARK_START,
    PORTION_TOXMARK_END,
    PORTION_BOOKMARK_START,
    PORTION_BOOKMARK_END,
    PORTION_REDLINE_START,
    PORTION_REDLINE_END,
    PORTION_RUBY_START,
    PORTION_RUBY_END
};

class SwXRubyPortion;

class SwXTextPortion : public cppu::WeakImplHelper8
<
//    ::com::sun::star::beans::XTolerantMultiPropertySet,
    ::com::sun::star::beans::XMultiPropertySet,
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::text::XTextRange,
    ::com::sun::star::text::XTextField,
    ::com::sun::star::beans::XPropertyState,
    ::com::sun::star::container::XContentEnumerationAccess,
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::lang::XServiceInfo
>,
    public SwClient
{
    friend class SwXRubyPortion;
    SwEventListenerContainer    aLstnrCntnr;
    SfxItemPropertySet			aPropSet;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > 					xParentText;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >  	xRefMark;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >  	xTOXMark;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent > 	xBookmark;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent > 	xFootnote;
    ::com::sun::star::uno::Any* pRubyText;
    ::com::sun::star::uno::Any* pRubyStyle;
    ::com::sun::star::uno::Any* pRubyAdjust;
    ::com::sun::star::uno::Any* pRubyIsAbove;

    const SwFmtFld*             pFmtFld;
    SwDepend 					aFrameDepend;
    SwFrmFmt*					pFrameFmt;
    SwTextPortionType			ePortionType;
    sal_Int16					nControlChar;

    BOOL						bIsCollapsed;

    SwFmtFld* 			GetFldFmt(BOOL bInit = sal_False);
protected:
    SfxItemPropertySet& GetPropSet() { return aPropSet; }

    void SAL_CALL SetPropertyValues_Impl( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL GetPropertyValues_Impl( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

//    ::com::sun::star::uno::Any  GetPropertyValue( const SfxItemPropertyMap *pEntry,
//                                    SwUnoCrsr *pUnoCrsr,
//                                    SfxItemSet *pSet );
    void        GetPropertyValues( const ::rtl::OUString *pPropertyNames,
                                    ::com::sun::star::uno::Any *pValues,
                                    sal_Int32 nLength );

    void SwXTextPortion::GetPropertyValue( ::com::sun::star::uno::Any &rVal,
                const SfxItemPropertyMap *pEntry, SwUnoCrsr *pUnoCrsr, SfxItemSet *&pSet );

//    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetDirectPropertyTolerantResult > SAL_CALL GetPropertyValuesTolerant_Impl(
//        const ::com::sun::star::uno::Sequence< rtl::OUString >& rPropertyNames,
//        sal_Bool bDirectValuesOnly ) throw (::com::sun::star::uno::RuntimeException);

    virtual ~SwXTextPortion();
public:
    SwXTextPortion(const SwUnoCrsr* pPortionCrsr, ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > & rParent, SwTextPortionType	eType	);
    SwXTextPortion(const SwUnoCrsr* pPortionCrsr, ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > & rParent, SwFrmFmt& rFmt );


    //XTextRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >  SAL_CALL getText(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL  getStart(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL   getEnd(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual rtl::OUString SAL_CALL  getString(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL  setString(const rtl::OUString& aString) throw( ::com::sun::star::uno::RuntimeException );

    //XTolerantMultiPropertySet
//    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::SetPropertyTolerantFailed > SAL_CALL setPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
//    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetPropertyTolerantResult > SAL_CALL getPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) throw (::com::sun::star::uno::RuntimeException);
//    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetDirectPropertyTolerantResult > SAL_CALL getDirectPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) throw (::com::sun::star::uno::RuntimeException);

    //XMultiPropertySet
//    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues ) throw(::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) throw(::com::sun::star::uno::RuntimeException);

    //XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    //XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    //XTextField
    virtual rtl::OUString SAL_CALL getPresentation(BOOL bShowCommand) throw( ::com::sun::star::uno::RuntimeException );

    //XTextContent
    virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getAnchor(  ) throw(::com::sun::star::uno::RuntimeException);

    //XComponent
    virtual void SAL_CALL dispose(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );

    //XUnoTunnel
    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual BOOL SAL_CALL supportsService(const rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

    //XContentEnumerationAccess
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createContentEnumeration(const rtl::OUString& aServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getAvailableServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

    //SwClient
    virtual void 				Modify( SfxPoolItem *pOld, SfxPoolItem *pNew);

    void 				SetRefMark( ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >  xMark)
                            {xRefMark = xMark;}

    void 				SetTOXMark( ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >  xMark)
                            {xTOXMark = xMark;}

    void 				SetBookmark( ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >  xMark)
                            {xBookmark = xMark;}

    void 				SetFootnote( ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >  xMark)
                            {xFootnote = xMark;}

    void				SetControlChar(sal_Int16 nSet) {nControlChar = nSet;}

    BOOL				IsCollapsed() const { return bIsCollapsed;}
    void				SetCollapsed(BOOL bSet) { bIsCollapsed = bSet;}

    SwTextPortionType	GetTextPortionType() const {return ePortionType;}

    SwUnoCrsr*          GetCrsr() const { return (SwUnoCrsr*)GetRegisteredIn(); }
};
/* -----------------------------19.02.01 10:46--------------------------------

 ---------------------------------------------------------------------------*/
class SwXRubyPortion : public SwXTextPortion
{
public:
    SwXRubyPortion(const SwUnoCrsr* pPortionCrsr,
                    SwTxtRuby& rAttr,
                    ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > & rParent,
                    sal_Bool bEnd	);
    ~SwXRubyPortion();
};
#endif
