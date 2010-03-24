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

#ifndef _UNOMAILMERGE_HXX_
#define _UNOMAILMERGE_HXX_

#include <functional>
#include <cppuhelper/implbase5.hxx> // WeakImplHelper4
#include <cppuhelper/interfacecontainer.hxx>    // OMultiTypeInterfaceContainerHelperVar
#include <unotools/configitem.hxx>  // !! needed for OMultiTypeInterfaceContainerHelperVar !!


#include <com/sun/star/task/XJob.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/PropertyChangeEvent.hpp>
#include <com/sun/star/text/XMailMergeBroadcaster.hpp>
#include <svl/itemprop.hxx>
#include <sfx2/objsh.hxx>   // SfxObjectShellRef

#include <functional>

namespace com { namespace sun { namespace star {

    namespace sdbc {
        class XResultSet;
        class XConnection;
    }
    namespace frame {
        class XModel;
    }
    namespace lang {
        class XMultiServiceFactory;
    }
    namespace text {
        class XMailMergeListener;
        struct MailMergeEvent;
    }
    namespace beans{
        struct PropertyValue;
    }

}}}

namespace rtl {
    class OUString;
}

///////////////////////////////////////////////////////////////////////////

// uses templates from <cppuhelper/interfacecontainer.h>
// and <unotools/configitem.hxx>

// helper function call class
struct PropHashType_Impl
{
    size_t operator()(const INT32 &s) const { return s; }
};

typedef cppu::OMultiTypeInterfaceContainerHelperVar
    <
        INT32,
        PropHashType_Impl,
        std::equal_to< INT32 >
    > OPropertyListenerContainerHelper;

////////////////////////////////////////////////////////////

class SwXMailMerge :
    public cppu::WeakImplHelper5
    <
        com::sun::star::task::XJob,
        com::sun::star::beans::XPropertySet,
        com::sun::star::text::XMailMergeBroadcaster,
        com::sun::star::lang::XComponent,
        com::sun::star::lang::XServiceInfo
    >
{
    cppu::OInterfaceContainerHelper     aEvtListeners;
    cppu::OInterfaceContainerHelper     aMergeListeners;
    OPropertyListenerContainerHelper    aPropListeners;

    //SfxItemPropertySet          aPropSet;
    const SfxItemPropertySet*   pPropSet;

    SfxObjectShellRef xDocSh;   // the document

    String          aTmpFileName;

    // properties of mail merge service
    com::sun::star::uno::Sequence< com::sun::star::uno::Any >           aSelection;
    com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet >  xResultSet;
    com::sun::star::uno::Reference< com::sun::star::sdbc::XConnection > xConnection;
    com::sun::star::uno::Reference< com::sun::star::frame::XModel >     xModel;
    rtl::OUString   aDataSourceName;
    rtl::OUString   aDataCommand;
    rtl::OUString   aFilter;
    rtl::OUString   aDocumentURL;
    rtl::OUString   aOutputURL;
    rtl::OUString   aFileNamePrefix;
    sal_Int32       nDataCommandType;
    sal_Int16       nOutputType;
    sal_Bool        bEscapeProcessing;
    sal_Bool        bSinglePrintJobs;
    sal_Bool        bFileNameFromColumn;


    ::rtl::OUString                                         sInServerPassword;
    ::rtl::OUString                                         sOutServerPassword;
    ::rtl::OUString                                         sSubject;
    ::rtl::OUString                                         sAddressFromColumn;
    ::rtl::OUString                                         sMailBody;
    ::rtl::OUString                                         sAttachmentName;
    ::rtl::OUString                                         sAttachmentFilter;
    com::sun::star::uno::Sequence< ::rtl::OUString >        aCopiesTo;
    com::sun::star::uno::Sequence< ::rtl::OUString >        aBlindCopiesTo;
    sal_Bool                                                bSendAsHTML;
    sal_Bool                                                bSendAsAttachment;

    com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aPrintSettings;

    sal_Bool                                                bSaveAsSingleFile;
    ::rtl::OUString                                         sSaveFilter;
    ::rtl::OUString                                         sSaveFilterOptions;
    com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aSaveFilterData;



    sal_Bool        bDisposing;


    void    launchEvent( const com::sun::star::beans::PropertyChangeEvent &rEvt ) const;

    // disallow use of copy-constructor and assignment-operator for now
    SwXMailMerge( const SwXMailMerge & );
    SwXMailMerge & operator = ( const SwXMailMerge & );
protected:
    virtual ~SwXMailMerge();
public:
    SwXMailMerge();


    void LaunchMailMergeEvent( const com::sun::star::text::MailMergeEvent &rData ) const;

    // XJob
    virtual ::com::sun::star::uno::Any SAL_CALL execute( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& Arguments ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    // XMailMergeBroadcaster
    virtual void SAL_CALL addMailMergeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XMailMergeListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeMailMergeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XMailMergeListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);

    // XComponent
    virtual void SAL_CALL dispose(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);
};


extern com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL SwXMailMerge_getSupportedServiceNames() throw();
extern rtl::OUString SAL_CALL SwXMailMerge_getImplementationName() throw();
extern com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL SwXMailMerge_createInstance(const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory > & rSMgr) throw( com::sun::star::uno::Exception );

////////////////////////////////////////////////////////////

#endif


