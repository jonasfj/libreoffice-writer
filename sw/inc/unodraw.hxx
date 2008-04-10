/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: unodraw.hxx,v $
 * $Revision: 1.16 $
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
#ifndef _UNODRAW_HXX
#define _UNODRAW_HXX

#include <svx/fmdpage.hxx>
#include <calbck.hxx>
#include <frmfmt.hxx>
#include <com/sun/star/text/XTextContent.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <cppuhelper/implbase3.hxx> // helper for implementations
// --> OD 2004-07-22 #i31698#
#include <cppuhelper/implbase6.hxx> // helper for implementations
#include <com/sun/star/drawing/HomogenMatrix3.hpp>
// <--
#include <svtools/itemprop.hxx>

class SdrMarkList;
class SdrView;
class SwDoc;
/******************************************************************************
 *
 ******************************************************************************/
class SwFmDrawPage : public SvxFmDrawPage
{
    SdrPageView*		pPageView;
protected:

    // Erzeugen eines SdrObjects anhand einer Description. Kann von
    // abgeleiteten Klassen dazu benutzt werden, eigene ::com::sun::star::drawing::Shapes zu
    // unterstuetzen (z.B. Controls)
    virtual SdrObject *_CreateSdrObject( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > & xShape ) throw ();

public:
    SwFmDrawPage( SdrPage* pPage );
    virtual ~SwFmDrawPage() throw ();

    const SdrMarkList& 	PreGroup(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > & xShapes);
    void 				PreUnGroup(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup >   xShapeGroup);
//	void 				PostGroup(); ?? wird es noch gebraucht ??

    SdrView* 			GetDrawView() {return mpView;}
    SdrPageView*		GetPageView();
    void				RemovePageView();
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  		GetInterface( SdrObject* pObj );

    // Die folgende Methode wird gerufen, wenn ein SvxShape-Objekt angelegt
    // werden soll. abgeleitete Klassen koennen hier eine Ableitung oder
    // ein ein SvxShape aggregierendes Objekt anlegen.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >  _CreateShape( SdrObject *pObj ) const throw ();
};

/* -----------------09.12.98 08:57-------------------
 *
 * --------------------------------------------------*/
typedef cppu::WeakAggImplHelper3
<
    ::com::sun::star::drawing::XDrawPage,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::drawing::XShapeGrouper
>
SwXDrawPageBaseClass;
class SwXDrawPage : public SwXDrawPageBaseClass
{
    SwDoc* 			pDoc;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation > 	xPageAgg;
    SwFmDrawPage*   pDrawPage;
public:
    SwXDrawPage(SwDoc* pDoc);
    ~SwXDrawPage();

    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);

    //XIndexAccess
    virtual sal_Int32 SAL_CALL getCount(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 nIndex) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException );

    //XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  ) throw(::com::sun::star::uno::RuntimeException);

    //XShapes
    virtual void SAL_CALL add(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > & xShape) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL remove(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > & xShape) throw( ::com::sun::star::uno::RuntimeException );

    //XShapeGrouper
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup >  SAL_CALL group(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > & xShapes) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL ungroup(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup > & aGroup) throw( ::com::sun::star::uno::RuntimeException );

    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual BOOL SAL_CALL supportsService(const rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

    SwFmDrawPage* 	GetSvxPage();
    void	Invalidate() {pDoc = 0;}
};
/* -----------------22.01.99 10:20-------------------
 *
 * --------------------------------------------------*/
class SwShapeDescriptor_Impl;
class SwXGroupShape;
typedef
cppu::WeakAggImplHelper6
<
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::beans::XPropertyState,
    ::com::sun::star::text::XTextContent,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::lang::XUnoTunnel,
    // --> OD 2004-07-22 #i31698#
    ::com::sun::star::drawing::XShape
    // <--
>
SwXShapeBaseClass;
class SwXShape : public SwXShapeBaseClass,
    public SwClient
{
    friend class SwHTMLImageWatcher;
    friend class SwHTMLParser;
    friend class SwXGroupShape;
    friend class SwXDrawPage;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation > xShapeAgg;
    // --> OD 2004-07-23 #i31698# - reference to <XShape>, determined in the
    // constructor by <queryAggregation> at <xShapeAgg>.
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxShape;
    // <--

    SfxItemPropertySet          aPropSet;
    const SfxItemPropertyMap* 	_pMap;
    com::sun::star::uno::Sequence< sal_Int8 >* pImplementationId;

    SwShapeDescriptor_Impl*		pImpl;

    sal_Bool 						m_bDescriptor;

    SwFrmFmt* 				GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }

    SvxShape*				GetSvxShape();

    /** method to determine top group object

        OD 2004-08-03 #i31698#

        @author OD
    */
    SdrObject* _GetTopGroupObj( SvxShape* _pSvxShape = 0L );

    /** method to determine position according to the positioning attributes

        OD 2004-08-03 #i31698#

        @author OD
    */
    com::sun::star::awt::Point _GetAttrPosition();

    /** method to convert the position (translation) of the drawing object to
        the layout direction horizontal left-to-right.

        OD 2004-07-27 #i31698#

        @author OD
    */
    ::com::sun::star::awt::Point _ConvertPositionToHoriL2R(
                                    const ::com::sun::star::awt::Point _aObjPos,
                                    const ::com::sun::star::awt::Size _aObjSize );

    /** method to convert the transformation of the drawing object to the layout
        direction, the drawing object is in

        OD 2004-07-27 #i31698#

        @author OD
    */
    ::com::sun::star::drawing::HomogenMatrix3 _ConvertTransformationToLayoutDir(
                ::com::sun::star::drawing::HomogenMatrix3 _aMatrixInHoriL2R );

    /** method to adjust the positioning properties

        OD 2004-08-02 #i31698#

        @author OD

        @param _aPosition
        input parameter - point representing the new shape position. The position
        has to be given in the layout direction the shape is in and relative to
        its position alignment areas.
    */
    void _AdjustPositionProperties( const ::com::sun::star::awt::Point _aPosition );

    /** method to convert start and end position of the drawing object to the
        Writer specific position, which is the attribute position in layout direction

        OD 2004-10-28 #i36248#

        @author OD
    */
    void __ConvertStartEndPosToLayoutDir(
                                    ::com::sun::star::awt::Point& _rioStartPos,
                                    ::com::sun::star::awt::Point& _rioEndPos );

    /** method to convert start position of the drawing object to the
        Writer specific position, which is the attribute position in layout direction

        OD 2004-10-28 #i36248#

        @author OD
    */
    ::com::sun::star::awt::Point _ConvertStartPosToLayoutDir(
                                const ::com::sun::star::awt::Point& _aStartPos );

    /** method to convert end position of the drawing object to the
        Writer specific position, which is the attribute position in layout direction

        OD 2004-10-28 #i36248#

        @author OD
    */
    ::com::sun::star::awt::Point _ConvertEndPosToLayoutDir(
                                const ::com::sun::star::awt::Point& _aEndPos );

    /** method to get property from aggregation object

        OD 2004-10-28 #i36248#

        @author OD
    */
    ::com::sun::star::uno::Any _getPropAtAggrObj( const ::rtl::OUString& _rPropertyName )
            throw( ::com::sun::star::beans::UnknownPropertyException,
                   ::com::sun::star::lang::WrappedTargetException,
                   ::com::sun::star::uno::RuntimeException);

protected:
    virtual ~SwXShape();
public:
    SwXShape(::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & xShape);


    TYPEINFO();
    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw(::com::sun::star::uno::RuntimeException);

    //XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);


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

    //XTextContent
    virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >  SAL_CALL getAnchor(void) throw( ::com::sun::star::uno::RuntimeException );

    //XComponent
    virtual void SAL_CALL dispose(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );

    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual BOOL SAL_CALL supportsService(const rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

    // --> OD 2004-07-22 #i31698# XShape
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize ) throw (::com::sun::star::beans::PropertyVetoException, ::com::sun::star::uno::RuntimeException);
    // <--
    // --> OD 2004-07-22 #i31698# XShapeDescriptor - superclass of XShape
    virtual ::rtl::OUString SAL_CALL getShapeType(  ) throw (::com::sun::star::uno::RuntimeException);
    // <--

    //SwClient
    virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew);

    SwShapeDescriptor_Impl*		GetDescImpl() {return pImpl;}
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation > 				GetAggregationInterface() {return xShapeAgg;}
};
/* -----------------------------31.05.01 09:54--------------------------------

 ---------------------------------------------------------------------------*/
class SwXGroupShape :
    public SwXShape,
    public ::com::sun::star::drawing::XShapes
{
protected:
    virtual ~SwXGroupShape();
public:
    SwXGroupShape(::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & xShape);


    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

    //XShapes
    virtual void SAL_CALL add( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL remove( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw (::com::sun::star::uno::RuntimeException);

    //XIndexAccess
    virtual sal_Int32 SAL_CALL getCount(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 nIndex) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException );

    //XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  ) throw(::com::sun::star::uno::RuntimeException);
};
#endif


