/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: acccontext.hxx,v $
 *
 *  $Revision: 1.41 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 08:19:13 $
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
#ifndef _ACCBASE_HXX
#define _ACCBASE_HXX
#ifndef _ACCFRAME_HXX
#include <accframe.hxx>
#endif

#ifndef _ACCMAP_HXX
#include <accmap.hxx>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECOMPONENT_HPP_
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEEVENTBROADCASTER_HPP_
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE5_HXX_
#include <cppuhelper/implbase5.hxx>
#endif
#ifndef _CPPUHELPER_INTERFACECONTAINER_HXX_
#include <cppuhelper/interfacecontainer.hxx>
#endif

class Window;
class SwAccessibleMap;
class SwCrsrShell;
class SdrObject;
class SwPaM;
namespace utl { class AccessibleStateSetHelper; }
namespace accessibility { class AccessibleShape; }

const sal_Char sAccessibleServiceName[] = "com.sun.star.accessibility.Accessible";

class SwAccessibleContext :
    public ::cppu::WeakImplHelper5<
                ::com::sun::star::accessibility::XAccessible,
                ::com::sun::star::accessibility::XAccessibleContext,
                ::com::sun::star::accessibility::XAccessibleComponent,
                ::com::sun::star::accessibility::XAccessibleEventBroadcaster,
                ::com::sun::star::lang::XServiceInfo
                >,
    public SwAccessibleFrame
{
    // The implements for the XAccessibleSelection interface has been
    // 'externalized' and wants access to the protected members like
    // GetMap, GetChild, GetParent, and GetFrm.
    friend class SwAccessibleSelectionHelper;


protected:

    mutable ::osl::Mutex aListenerMutex;
    mutable ::vos::OMutex aMutex;

private:

    ::rtl::OUString sName;	// immutable outside constructor

    // The parent if it has been retrieved. This is always an
    // SwAccessibleContext. (protected by Mutex)
    ::com::sun::star::uno::WeakReference <
        ::com::sun::star::accessibility::XAccessible > xWeakParent;

    SwAccessibleMap *pMap; 	// must be protected by solar mutex

    sal_uInt32 nClientId;   // client id in the AccessibleEventNotifier queue
    sal_Int16 nRole;		// immutable outside constructor

    // The current states (protected by mutex)
    sal_Bool bIsShowingState : 1;
    sal_Bool bIsEditableState : 1;
    sal_Bool bIsOpaqueState : 1;
    sal_Bool bIsDefuncState : 1;

    // Are we currently disposing that object (protected by solar mutex)?
    sal_Bool bDisposing : 1;

    void InitStates();

protected:
    void SetName( const ::rtl::OUString& rName ) { sName = rName; }
    inline sal_Int16 GetRole() const
    {
        return nRole;
    }

    void SetParent( SwAccessibleContext *pParent );
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> GetWeakParent() const;

    sal_Bool IsDisposing() const { return bDisposing; }

    Window *GetWindow();
    SwAccessibleMap *GetMap() { return pMap; }
    const SwAccessibleMap *GetMap() const { return pMap; }

    /** convenience method to get the SwViewShell through accessibility map */
    inline ViewShell* GetShell()
    {
        return GetMap()->GetShell();
    }
    inline const ViewShell* GetShell() const
    {
        return GetMap()->GetShell();
    }

    /** convenience method to get SwCrsrShell through accessibility map
     * @returns SwCrsrShell, or NULL if none is found */
    SwCrsrShell* GetCrsrShell();
    const SwCrsrShell* GetCrsrShell() const;

    // Notify all children that the vis araea has changed.
    // The SwFrm might belong to the current object or to any other child or
    // grandchild.
    void ChildrenScrolled( const SwFrm *pFrm, const SwRect& rOldVisArea );

    // The context's showing state changed. May only be called for context that
    // exist even if they aren't visible.
    void Scrolled( const SwRect& rOldVisArea );

    // A child has been moved while setting the vis area
    void ScrolledWithin( const SwRect& rOldVisArea );

    // The has been added while setting the vis area
    void ScrolledIn();

    // The context has to be removed while setting the vis area
    void ScrolledOut( const SwRect& rOldVisArea );

    // Invalidate the states of all children of the specified SwFrm. The
    // SwFrm might belong the the current object or to any child or grandchild!
    // --> OD 2005-12-12 #i27301# - use new type definition for <_nStates>
    void InvalidateChildrenStates( const SwFrm* _pFrm,
                                   tAccessibleStates _nStates );
    // <--

    // Dispose children of the specified SwFrm. The SwFrm might belong to
    // the current object or to any other child or grandchild.
    void DisposeChildren( const SwFrm *pFrm,
                                       sal_Bool bRecursive );

    void DisposeShape( const SdrObject *pObj,
                                ::accessibility::AccessibleShape *pAccImpl );
    void ScrolledInShape( const SdrObject *pObj,
                                ::accessibility::AccessibleShape *pAccImpl );

    virtual void _InvalidateContent( sal_Bool bVisibleDataFired );

    virtual void _InvalidateCursorPos();
    virtual void _InvalidateFocus();

public:

    void FireAccessibleEvent( ::com::sun::star::accessibility::AccessibleEventObject& rEvent );

protected:

    // broadcast visual data event
    void FireVisibleDataEvent();

    // broadcast state change event
    void FireStateChangedEvent( sal_Int16 nState, sal_Bool bNewState );

    // Set states for getAccessibleStateSet.
    // This base class sets DEFUNC(0/1), EDITABLE(0/1), ENABLED(1),
    // SHOWING(0/1), OPAQUE(0/1) and VISIBLE(1).
    virtual void GetStates( ::utl::AccessibleStateSetHelper& rStateSet );

     sal_Bool IsEditableState();

    virtual ::com::sun::star::awt::Rectangle SAL_CALL
        getBoundsImpl(sal_Bool bRelative)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ~SwAccessibleContext();

public:

    SwAccessibleContext( SwAccessibleMap *pMap, sal_Int16 nRole,
                         const SwFrm *pFrm );

    //=====  XAccessible  =====================================================

    ///	Return the XAccessibleContext.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL
        getAccessibleContext (void) throw (com::sun::star::uno::RuntimeException);

    //=====  XAccessibleContext  ==============================================

    ///	Return the number of currently visible children.
    virtual sal_Int32 SAL_CALL getAccessibleChildCount (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Return the specified child or NULL if index is invalid.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleChild (sal_Int32 nIndex)
        throw (::com::sun::star::uno::RuntimeException,
                ::com::sun::star::lang::IndexOutOfBoundsException);

    ///	Return a reference to the parent.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleParent (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Return this objects index among the parents children.
    virtual	sal_Int32 SAL_CALL
        getAccessibleIndexInParent (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Return this object's role.
    virtual sal_Int16 SAL_CALL
        getAccessibleRole (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Return this object's description.
    virtual ::rtl::OUString SAL_CALL
        getAccessibleDescription (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Return the object's current name.
    virtual ::rtl::OUString SAL_CALL
        getAccessibleName (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Return NULL to indicate that an empty relation set.
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleRelationSet> SAL_CALL
        getAccessibleRelationSet (void)
        throw (::com::sun::star::uno::RuntimeException);

    ///	Return the set of current states.
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleStateSet> SAL_CALL
        getAccessibleStateSet (void)
        throw (::com::sun::star::uno::RuntimeException);

    /**	Return the parents locale or throw exception if this object has no
        parent yet/anymore.
    */
    virtual ::com::sun::star::lang::Locale SAL_CALL
        getLocale (void)
        throw (::com::sun::star::accessibility::IllegalAccessibleComponentStateException, ::com::sun::star::uno::RuntimeException);

    //=====  XAccessibleEventBroadcaster  =====================================

    virtual void SAL_CALL addEventListener(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::accessibility::XAccessibleEventListener >& xListener )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::accessibility::XAccessibleEventListener >& xListener )
        throw (::com::sun::star::uno::RuntimeException);

    //=====  XAccessibleComponent  ============================================
    virtual sal_Bool SAL_CALL containsPoint(
            const ::com::sun::star::awt::Point& aPoint )
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint(
                const ::com::sun::star::awt::Point& aPoint )
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds()
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Point SAL_CALL getLocation()
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen()
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Size SAL_CALL getSize()
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL grabFocus()
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding()
        throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getForeground()
        throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getBackground()
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XServiceInfo  ====================================================

    /**	Returns an identifier for the implementation of this object.
    */
    virtual ::rtl::OUString SAL_CALL
        getImplementationName (void)
        throw (::com::sun::star::uno::RuntimeException);

    /**	Return whether the specified service is supported by this class.
    */
    virtual sal_Bool SAL_CALL
        supportsService (const ::rtl::OUString& sServiceName)
        throw (::com::sun::star::uno::RuntimeException);

    /** Returns a list of all supported services.  In this case that is just
        the AccessibleContext service.
    */
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
        getSupportedServiceNames (void)
             throw (::com::sun::star::uno::RuntimeException);

    //====== thread safe C++ interface ========================================

    // The object is not visible an longer and should be destroyed
    virtual void Dispose( sal_Bool bRecursive = sal_False );

    // The child object is not visible an longer and should be destroyed
    virtual void DisposeChild( const SwFrmOrObj& rFrmOrObj, sal_Bool bRecursive );

    // The object has been moved by the layout
    virtual void InvalidatePosOrSize( const SwRect& rFrm );

    // The vhild object has been moved by the layout
    virtual void InvalidateChildPosOrSize( const SwFrmOrObj& rFrmOrObj,
                                           const SwRect& rFrm );

    // The content may have changed (but it hasn't tohave changed)
    void InvalidateContent();

    // The caretPos has changed
    void InvalidateCursorPos();

    // The Focus state has changed
    void InvalidateFocus();

    // Check states
    // --> OD 2005-12-12 #i27301# - use new type definition for <_nStates>
    void InvalidateStates( tAccessibleStates _nStates );
    // <--

    // the XAccessibleRelationSet may have changed
    void InvalidateRelation( sal_uInt16 nType );

    /** text selection has changed

        OD 2005-12-14 #i27301#

        @author OD
    */
    void InvalidateTextSelection();

    const ::rtl::OUString& GetName() const { return sName; }

    virtual sal_Bool HasCursor();	// required by map to remember that object

    sal_Bool Select( SwPaM *pPaM, SdrObject *pObj, sal_Bool bAdd );
    inline sal_Bool Select( SwPaM& rPaM )
    {
        return Select( &rPaM, 0, sal_False );
    }
    inline sal_Bool Select( SdrObject *pObj, sal_Bool bAdd )
    {
        return Select( 0, pObj, bAdd );
    }

    static ::rtl::OUString GetResource( sal_uInt16 nResId,
                                        const ::rtl::OUString *pArg1 = 0,
                                        const ::rtl::OUString *pArg2 = 0 );


};

// some heaviliy used exception support
const sal_Char sDefunc[] = "object is defunctional";
const sal_Char sMissingWindow[] = "window is missing";

#define THROW_RUNTIME_EXCEPTION( ifc, msg ) 								\
    ::com::sun::star::uno::Reference < ifc > xThis( this );					\
    ::com::sun::star::uno::RuntimeException aExcept(                        \
        ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(msg) ), xThis );		\
    throw aExcept;

#define CHECK_FOR_DEFUNC_THIS( ifc, ths )									\
    if( !(GetFrm() && GetMap()) )											\
    {																		\
        ::com::sun::star::uno::Reference < ifc > xThis( ths );				\
        ::com::sun::star::lang::DisposedException aExcept( 					\
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(sDefunc) ),        \
            xThis );												        \
        throw aExcept;														\
    }

#define CHECK_FOR_DEFUNC( ifc )												\
    CHECK_FOR_DEFUNC_THIS( ifc, this )

#define CHECK_FOR_WINDOW( i, w ) 											\
    if( !(w) )																\
    {																		\
        THROW_RUNTIME_EXCEPTION( i, sMissingWindow );						\
    }
#endif

