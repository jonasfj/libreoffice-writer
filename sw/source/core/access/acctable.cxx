 /*************************************************************************
 *
 *  $RCSfile: acctable.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mib $ $Date: 2002-04-17 14:07:39 $
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

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#include <list>
#include <set>

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEROLE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleRole.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLESTATETYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleStateType.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEEVENTID_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleEventId.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLETABLEMODELCHANGE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleTableModelChange.hpp>
#endif
#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLETABLEMODELCHANGETYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleTableModelChangeType.hpp>
#endif

#ifndef _UTL_ACCESSIBLESTATESETHELPER_HXX_
#include <unotools/accessiblestatesethelper.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _TABFRM_HXX
#include <tabfrm.hxx>
#endif
#ifndef _SWTABLE_HXX
#include <swtable.hxx>
#endif
#ifndef _CRSRSH_HXX
#include "crsrsh.hxx"
#endif
#ifndef _VISCRS_HXX
#include "viscrs.hxx"
#endif
#ifndef _ACCFRMOBJSLIST_HXX
#include <accfrmobjslist.hxx>
#endif
#ifndef _ACCMAP_HXX
#include "accmap.hxx"
#endif
#ifndef _ACCESS_HRC
#include "access.hrc"
#endif
#ifndef _ACCTABLE_HXX
#include <acctable.hxx>
#endif

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::accessibility;
using namespace ::rtl;

const sal_Char sServiceName[] = "com.sun.star.table.AccessibleTableView";
const sal_Char sImplementationName[] = "SwAccessibleTable";

class SwAccessibleTableEvent_Impl 
{
    const SwFrm *mpFrm;
    sal_Int32 mnRow;
    sal_Int32 mnColumn;
    sal_Int32 mnRowExtent;
    sal_Int32 mnColumnExtent;

public:
    
    inline SwAccessibleTableEvent_Impl( const SwFrm *pFrm, 
                                 sal_Int32 nRow, sal_Int32 nColumn,
                                 sal_Int32 nRowExtent,
                                 sal_Int32 nColumnExtent );
    const SwFrm *GetFrm() const { return mpFrm; }

    const sal_Int32 GetRow() const { return mnRow; }
    const sal_Int32 GetColumn() const { return mnColumn; }

    const sal_Int32 GetRowExtent() const { return mnRowExtent; }
    const sal_Int32 GetColumnExtent() const { return mnColumnExtent; }
};

inline SwAccessibleTableEvent_Impl::SwAccessibleTableEvent_Impl(
        const SwFrm *pFrm, 
        sal_Int32 nRow, sal_Int32 nColumn,
        sal_Int32 nRowExtent,
        sal_Int32 nColumnExtent ) :
    mpFrm( pFrm ), mnRow( nRow ), mnColumn( nColumn ), 
    mnRowExtent( nRowExtent ), mnColumnExtent( nColumnExtent )
{
}

typedef ::std::list < SwAccessibleTableEvent_Impl > _SwAccessibleTableEventList_Impl;
class SwAccessibleTableEventList_Impl : public _SwAccessibleTableEventList_Impl
{
public:
    SwAccessibleTableEventList_Impl() : _SwAccessibleTableEventList_Impl() {}
};

//------------------------------------------------------------------------------

typedef ::std::less < sal_Int32 > Int32Less_Impl;
typedef ::std::set < sal_Int32, Int32Less_Impl > Int32Set_Impl;

class SwAccTableSelHander_Impl
{
public:
    
    virtual void Unselect( sal_Int32 nRowOrCol, sal_Int32 nExt ) = 0;
};

//------------------------------------------------------------------------------

class SwAccessibleTableData_Impl
{
    Int32Set_Impl	maRows;
    Int32Set_Impl	maColumns;
    Point 	maTabFrmPos;		
    const SwTabFrm *mpTabFrm;

    void CollectData( const Point& rPos, const SwFrm *pFrm );

    sal_Bool FindCell( const Point& rPos, const SwFrm *pFrm ,
                           sal_Bool bExact, const SwFrm *& rFrm ) const;

    void GetSelection( const Point& rTabPos, const SwRect& rArea, 
                       const SwSelBoxes& rSelBoxes, const SwFrm *pFrm, 
                       SwAccTableSelHander_Impl& rSelHdl,
                       sal_Bool bColumns ) const;

public:
    SwAccessibleTableData_Impl( const SwTabFrm *pTabFrm );

    const Int32Set_Impl& GetRows() const { return maRows; }
    const Int32Set_Impl& GetColumns() const { return maColumns; }

    inline Int32Set_Impl::const_iterator GetRowIter( sal_Int32 nRow ) const;
    inline Int32Set_Impl::const_iterator GetColumnIter( sal_Int32 nCol ) const;

    const SwFrm *GetCell( sal_Int32 nRow, sal_Int32 nColumn, sal_Bool bExact,
                          SwAccessibleTable *pThis ) const
        throw(IndexOutOfBoundsException );
    const SwFrm *GetCellAtPos( sal_Int32 nLeft, sal_Int32 nTop,
                                         sal_Bool bExact ) const;
    inline sal_Int32 GetRowCount() const;
    inline sal_Int32 GetColumnCount() const;

    void GetSelection( sal_Int32 nStart, sal_Int32 nEnd,
                       const SwSelBoxes& rSelBoxes,
                          SwAccTableSelHander_Impl& rSelHdl,
                       sal_Bool bColumns ) const;

    void CheckRowAndCol( sal_Int32 nRow, sal_Int32 nCol, 
                         SwAccessibleTable *pThis ) const
        throw(IndexOutOfBoundsException );

    void GetRowColumnAndExtent( const SwRect& rBox,
                                  sal_Int32& rRow, sal_Int32& rColumn,
                                  sal_Int32& rRowExtend,
                                  sal_Int32& rColumnExtend );

    const Point& GetTablePos() const { return maTabFrmPos; }
    void SetTablePos( const Point& rPos ) { maTabFrmPos = rPos; }
};

void SwAccessibleTableData_Impl::CollectData( const Point& rPos,
                                              const SwFrm *pFrm )
{
    const SwFrmOrObjSList aList( pFrm );
    SwFrmOrObjSList::const_iterator aIter( aList.begin() );
    SwFrmOrObjSList::const_iterator aEndIter( aList.end() );
    while( aIter != aEndIter )
    {
        const SwFrmOrObj& rLower = *aIter;
        const SwFrm *pLower = rLower.GetSwFrm();
        if( pLower )
        {
            if( pLower->IsRowFrm() )
            {
                maRows.insert( pLower->Frm().Top() - rPos.Y() );
                CollectData( rPos, pLower );
            }
            else if( pLower->IsCellFrm() && rLower.IsAccessible() )
            {
                maColumns.insert( pLower->Frm().Left() - rPos.X() );
            }
            else
            {
                CollectData( rPos, pLower );
            }
        }
        ++aIter;
    }
}

sal_Bool SwAccessibleTableData_Impl::FindCell(
        const Point& rPos, const SwFrm *pFrm, sal_Bool bExact,
        const SwFrm *& rRet	) const
{
    sal_Bool bFound = sal_False;

    const SwFrmOrObjSList aList( pFrm );
    SwFrmOrObjSList::const_iterator aIter( aList.begin() );
    SwFrmOrObjSList::const_iterator aEndIter( aList.end() );
    while( !bFound && aIter != aEndIter )
    {
        const SwFrmOrObj& rLower = *aIter;
        const SwFrm *pLower = rLower.GetSwFrm();
        ASSERT( pLower, "child should be a frame" );
        if( pLower )
        {
            if( rLower.IsAccessible() )
            {
                ASSERT( pLower->IsCellFrm(), "lower is not a cell frame" );
                const SwRect& rFrm = pLower->Frm();
                if( rFrm.Right() >= rPos.X() && rFrm.Bottom() >= rPos.Y() )
                {
                    // We have found the cell
                    ASSERT( rFrm.Left() <= rPos.X() && rFrm.Top() <= rPos.Y(),
                            "find frame moved to far!" );
                    bFound = sal_True;
                    if( !bExact || 
                        (rFrm.Top == rPos.Y() && rFrm.Left() == rPos.Y() ) )
                    {
                        rRet = pLower;
                    }
                }
            }
            else
            {
                bFound = FindCell( rPos, pLower, bExact, rRet );
            }
        }
        ++aIter;
    }

    return bFound;
}

void SwAccessibleTableData_Impl::GetSelection(
            const Point& rTabPos,
            const SwRect& rArea, 
            const SwSelBoxes& rSelBoxes,
            const SwFrm *pFrm,
            SwAccTableSelHander_Impl& rSelHdl,
            sal_Bool bColumns ) const
{
    const SwFrmOrObjSList aList( pFrm );
    SwFrmOrObjSList::const_iterator aIter( aList.begin() );
    SwFrmOrObjSList::const_iterator aEndIter( aList.end() );
    while( aIter != aEndIter )
    {
        const SwFrmOrObj& rLower = *aIter;
        const SwFrm *pLower = rLower.GetSwFrm();
        ASSERT( pLower, "child should be a frame" );
        const SwRect& rBox = rLower.GetBox();
        if( pLower && rBox.IsOver( rArea ) )
        {
            if( rLower.IsAccessible() )
            {
                ASSERT( pLower->IsCellFrm(), "lower is not a cell frame" );
                const SwCellFrm *pCFrm =
                        static_cast < const SwCellFrm * >( pLower );
                SwTableBox *pBox =
                    const_cast< SwTableBox *>( pCFrm->GetTabBox() ); //SVPtrArr!
                if( !rSelBoxes.Seek_Entry( pBox ) )
                {
                    const Int32Set_Impl	rRowsOrCols =
                        bColumns ? maColumns : maRows;

                    sal_Int32 nPos = bColumns ? (rBox.Left() - rTabPos.X())
                                              : (rBox.Top() - rTabPos.Y());
                    Int32Set_Impl::const_iterator aSttRowOrCol( 
                        rRowsOrCols.lower_bound( nPos ) );
                    sal_Int32 nRowOrCol =
                        static_cast< sal_Int32 >( ::std::distance( 
                            rRowsOrCols.begin(), aSttRowOrCol ) );

                    nPos = bColumns ? (rBox.Right() - rTabPos.X())
                                    : (rBox.Bottom() - rTabPos.Y());
                    Int32Set_Impl::const_iterator aEndRowOrCol( 
                        rRowsOrCols.upper_bound( nPos ) );
                    sal_Int32 nExt =
                        static_cast< sal_Int32 >( ::std::distance( 
                            aSttRowOrCol, aEndRowOrCol ) );

                    rSelHdl.Unselect( nRowOrCol, nExt );
                }
            }
            else
            {
                GetSelection( rTabPos, rArea, rSelBoxes, pLower, rSelHdl,
                              bColumns );
            }
        }
        ++aIter;
    }
}

const SwFrm *SwAccessibleTableData_Impl::GetCell(
        sal_Int32 nRow, sal_Int32 nColumn, sal_Bool bExact,
        SwAccessibleTable *pThis ) const
    throw(IndexOutOfBoundsException )
{
    CheckRowAndCol( nRow, nColumn, pThis );

    Int32Set_Impl::const_iterator aSttCol( GetColumnIter( nColumn ) );
    Int32Set_Impl::const_iterator aSttRow( GetRowIter( nRow ) );
    const SwFrm *pCellFrm = GetCellAtPos( *aSttCol, *aSttRow, sal_False );

    return pCellFrm;
}

void SwAccessibleTableData_Impl::GetSelection( 
                       sal_Int32 nStart, sal_Int32 nEnd,
                       const SwSelBoxes& rSelBoxes,
                          SwAccTableSelHander_Impl& rSelHdl,
                       sal_Bool bColumns ) const
{
    SwRect aArea( mpTabFrm->Frm() );
    Point aPos( aArea.Pos() );

    const Int32Set_Impl& rRowsOrColumns = bColumns ? maColumns : maRows; 
    if( nStart > 0 )
    {
        Int32Set_Impl::const_iterator aStt( rRowsOrColumns.begin() );
        ::std::advance( aStt, 
            static_cast< Int32Set_Impl::difference_type >( nStart ) );
        if( bColumns )
            aArea.Left( *aStt + aPos.X() );
        else
            aArea.Top( *aStt + aPos.Y() );
    }
    if( nEnd < static_cast< sal_Int32 >( rRowsOrColumns.size() ) )
    {
        Int32Set_Impl::const_iterator aEnd( rRowsOrColumns.begin() );
        ::std::advance( aEnd, 
            static_cast< Int32Set_Impl::difference_type >( nEnd ) );
        if( bColumns )
            aArea.Right( *aEnd + aPos.X() - 1 );
        else
            aArea.Bottom( *aEnd + aPos.Y() - 1 );
    }

    GetSelection( aPos, aArea, rSelBoxes, mpTabFrm, rSelHdl, bColumns );
}

const SwFrm *SwAccessibleTableData_Impl::GetCellAtPos(
        sal_Int32 nLeft, sal_Int32 nTop, sal_Bool bExact ) const
{
    Point aPos( mpTabFrm->Frm().Pos() );
    aPos.Move( nLeft, nTop );
    const SwFrm *pRet = 0;
    FindCell( aPos, mpTabFrm, bExact, pRet );

    return pRet;
}

inline sal_Int32 SwAccessibleTableData_Impl::GetRowCount() const
{
    return static_cast< sal_Int32 >( maRows.size() );
}

inline sal_Int32 SwAccessibleTableData_Impl::GetColumnCount() const
{
    return static_cast< sal_Int32 >( maColumns.size() );
}

SwAccessibleTableData_Impl::SwAccessibleTableData_Impl(
        const SwTabFrm *pTabFrm ) :
    mpTabFrm( pTabFrm ),
    maTabFrmPos( pTabFrm->Frm().Pos() )
{
    CollectData( mpTabFrm->Frm().Pos(), mpTabFrm );
}

inline Int32Set_Impl::const_iterator SwAccessibleTableData_Impl::GetRowIter(
        sal_Int32 nRow ) const
{
    Int32Set_Impl::const_iterator aCol( GetRows().begin() );
    if( nRow > 0 )
    {
        ::std::advance( aCol, 
                    static_cast< Int32Set_Impl::difference_type >( nRow ) );
    }
    return aCol;
}

inline Int32Set_Impl::const_iterator SwAccessibleTableData_Impl::GetColumnIter(
        sal_Int32 nColumn ) const
{
    Int32Set_Impl::const_iterator aCol = GetColumns().begin();
    if( nColumn > 0 )
    {
        ::std::advance( aCol, 
                    static_cast< Int32Set_Impl::difference_type >( nColumn ) );
    }
    return aCol;
}

void SwAccessibleTableData_Impl::CheckRowAndCol( 
        sal_Int32 nRow, sal_Int32 nCol, SwAccessibleTable *pThis ) const
    throw(IndexOutOfBoundsException )
{
    if( ( nRow < 0 || nRow >= static_cast< sal_Int32 >( maRows.size() ) ) ||
        ( nCol < 0 || nCol >= static_cast< sal_Int32 >( maColumns.size() ) ) )
    {
        Reference < XAccessibleTable > xThis( pThis );
        IndexOutOfBoundsException aExcept(
               OUString( RTL_CONSTASCII_USTRINGPARAM(
                       "row or column index out of range") ),
               xThis );
        throw aExcept;
    }
}

void SwAccessibleTableData_Impl::GetRowColumnAndExtent(
        const SwRect& rBox,
        sal_Int32& rRow, sal_Int32& rColumn,
        sal_Int32& rRowExtent, sal_Int32& rColumnExtent )
{
    Int32Set_Impl::const_iterator aStt( 
                maRows.lower_bound( rBox.Top() - maTabFrmPos.Y() ) );
    Int32Set_Impl::const_iterator aEnd( 
                maRows.upper_bound( rBox.Bottom() - maTabFrmPos.Y() ) );
    rRow = 
         static_cast< sal_Int32 >( ::std::distance( maRows.begin(), aStt ) );
    rRowExtent =
         static_cast< sal_Int32 >( ::std::distance( aStt, aEnd ) );

    aStt = maColumns.lower_bound( rBox.Left() - maTabFrmPos.X() );
    aEnd = maColumns.upper_bound( rBox.Right() - maTabFrmPos.X() );
    rColumn =
         static_cast< sal_Int32 >( ::std::distance( maColumns.begin(), aStt ) );
    rColumnExtent =
         static_cast< sal_Int32 >( ::std::distance( aStt, aEnd ) );
}

//------------------------------------------------------------------------------

class SwAccSingleTableSelHander_Impl : public SwAccTableSelHander_Impl
{
    sal_Bool bSelected;

public:
    
    inline SwAccSingleTableSelHander_Impl();

    inline sal_Bool IsSelected() const { return bSelected; }

    virtual void Unselect( sal_Int32, sal_Int32 );
};

inline SwAccSingleTableSelHander_Impl::SwAccSingleTableSelHander_Impl() :
    bSelected( sal_True )
{
}

void SwAccSingleTableSelHander_Impl::Unselect( sal_Int32, sal_Int32 )
{
    bSelected = sal_False;
}

//------------------------------------------------------------------------------

class SwAccAllTableSelHander_Impl : public SwAccTableSelHander_Impl

{
    ::std::vector< sal_Bool > aSelected;
    sal_Int32 nCount;

public:
    
    inline SwAccAllTableSelHander_Impl( sal_Int32 nSize );

    Sequence < sal_Int32 > GetSelSequence();

    virtual void Unselect( sal_Int32 nRowOrCol, sal_Int32 nExt );
};

inline SwAccAllTableSelHander_Impl::SwAccAllTableSelHander_Impl( sal_Int32 nSize ) :
    aSelected( nSize, sal_True ),
    nCount( nSize )
{
}

Sequence < sal_Int32 > SwAccAllTableSelHander_Impl::GetSelSequence()
{
    ASSERT( nCount >= 0, "underflow" );
    Sequence < sal_Int32 > aRet( nCount );
    sal_Int32 *pRet = aRet.getArray();
    sal_Int32 nPos = 0;
    size_t nSize = aSelected.size();
    for( size_t i=0; i < nSize && nPos < nCount; i++ )
    {
        if( aSelected[i] )
        {
            *pRet++ = i;
            nPos++;
        }
    }

    ASSERT( nPos == nCount, "count is wrong" );

    return aRet;
}

void SwAccAllTableSelHander_Impl::Unselect( sal_Int32 nRowOrCol,
                                            sal_Int32 nExt )
{
    ASSERT( static_cast< size_t >( nRowOrCol ) < aSelected.size(),
             "index to large" );
    ASSERT( static_cast< size_t >( nRowOrCol+nExt ) <= aSelected.size(),
             "extent to large" );
    while( nExt )
    {
        if( aSelected[static_cast< size_t >( nRowOrCol )] )
        {
            aSelected[static_cast< size_t >( nRowOrCol )] = sal_False;
            nCount--;
        }
        nExt--;
        nRowOrCol++;
    }
}

//------------------------------------------------------------------------------

const SwSelBoxes *SwAccessibleTable::GetSelBoxes() const
{
    const SwSelBoxes *pSelBoxes = 0;
    const ViewShell *pVSh = GetMap()->GetShell();
    if( pVSh && pVSh->ISA( SwCrsrShell ) )
    {
        const SwCrsrShell *pCSh = static_cast< const SwCrsrShell * >( pVSh );
        if( pCSh->IsTableMode() )
        {
            pSelBoxes = &pCSh->GetTableCrsr()->GetBoxes();
        }
    }

    return pSelBoxes;
}

void SwAccessibleTable::AppendEvent( const SwFrm *pFrm, const SwRect& rBox )
{
    sal_Int32 nRow, nCol, nRowExt, nColExt;
    mpTableData->GetRowColumnAndExtent( rBox, nRow, nCol, nRowExt, nColExt );

    if( !mpEvents )
        mpEvents = new SwAccessibleTableEventList_Impl;
    SwAccessibleTableEvent_Impl aEvent( pFrm, nRow, nCol, nRowExt, nColExt );
    mpEvents->push_back( aEvent );
}

void SwAccessibleTable::FireTableChangeEvent( 
        const SwAccessibleTableData_Impl *pTableData )
{
    if( !pTableData )
        pTableData = &GetTableData();

    AccessibleTableModelChange aModelChange;
    aModelChange.Type = AccessibleTableModelChangeType::UPDATE;
    aModelChange.FirstRow = 0;
    aModelChange.LastRow = pTableData->GetRowCount() - 1;
    aModelChange.FirstColumn = 0;
    aModelChange.LastColumn = pTableData->GetColumnCount() - 1;

    AccessibleEventObject aEvent;
    aEvent.EventId = AccessibleEventId::ACCESSIBLE_TABLE_MODEL_CHANGED;
    aEvent.NewValue <<= aModelChange;

    FireAccessibleEvent( aEvent );
}

void SwAccessibleTable::GetStates(
        ::utl::AccessibleStateSetHelper& rStateSet )
{
    SwAccessibleContext::GetStates( rStateSet );
}

SwAccessibleTable::SwAccessibleTable( 
        SwAccessibleMap *pMap,
        const SwTabFrm *pTabFrm	) :
    SwAccessibleContext( pMap, AccessibleRole::TABLE, pTabFrm ),
    mpTableData( 0 ),
    mpEvents( 0 )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    const String& rName = pTabFrm->GetFmt()->GetName();

    OUStringBuffer aBuffer( rName.Len() + 4 );
    aBuffer.append( OUString(rName) );
    aBuffer.append( static_cast<sal_Unicode>( '-' ) );
    aBuffer.append( static_cast<sal_Int32>( pTabFrm->GetPhyPageNum() ) );

    SetName( aBuffer.makeStringAndClear() );
}

SwAccessibleTable::~SwAccessibleTable()
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    delete mpTableData;
    ASSERT( !mpEvents, "events are existing" );
    delete mpEvents;
}

Any SwAccessibleTable::queryInterface( const Type& rType ) 
    throw (RuntimeException)
{
    Any aRet;
    if ( rType == ::getCppuType((Reference<XAccessibleTable> *)0) )
    {
//        Reference<XAccessibleTable> xThis( this );
//       aRet <<= xThis;
    }
    else
    {
        aRet = SwAccessibleContext::queryInterface(rType);
    }

    return aRet;
}

void SwAccessibleTable::UpdateTableData()
{
    const SwTabFrm* pTabFrm = static_cast<const SwTabFrm*>( GetFrm() );

    delete mpTableData;
    mpTableData = new SwAccessibleTableData_Impl( pTabFrm );
    delete mpEvents;
    mpEvents = 0;
}

void SwAccessibleTable::ClearTableData()
{
    delete mpTableData;
    mpTableData = 0;
    delete mpEvents;
    mpEvents = 0;
}

OUString SAL_CALL SwAccessibleTable::getAccessibleDescription (void)
        throw (::com::sun::star::uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleContext )

    OUString sArg1( static_cast< const SwTabFrm * >( GetFrm() )
                                        ->GetFmt()->GetName() );
    OUString sArg2( GetFormattedPageNumber() );

    return GetResource( STR_ACCESS_TABLE_DESC, &sArg1, &sArg2 );
}

sal_Int32 SAL_CALL SwAccessibleTable::getAccessibleRowCount() 
    throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    return  GetTableData().GetRowCount();
}

sal_Int32 SAL_CALL SwAccessibleTable::getAccessibleColumnCount(  )
    throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    return GetTableData().GetColumnCount();
}

OUString SAL_CALL SwAccessibleTable::getAccessibleRowDescription( 
            sal_Int32 nRow ) 
    throw (IndexOutOfBoundsException, RuntimeException)
{
    // TODO: Is there any reasonable we can do here?
    OUString sDesc;
    return sDesc;
}

OUString SAL_CALL SwAccessibleTable::getAccessibleColumnDescription(
            sal_Int32 nColumn )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    // TODO: Is there any reasonable we can do here?
    OUString sDesc;
    return sDesc;
}

sal_Int32 SAL_CALL SwAccessibleTable::getAccessibleRowExtentAt(
            sal_Int32 nRow, sal_Int32 nColumn )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    sal_Int32 nExtend = -1;

    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    GetTableData().CheckRowAndCol( nRow, nColumn, this );

    Int32Set_Impl::const_iterator aSttCol(
                                    GetTableData().GetColumnIter( nColumn ) );
    Int32Set_Impl::const_iterator aSttRow(
                                    GetTableData().GetRowIter( nRow ) );
    const SwFrm *pCellFrm = GetTableData().GetCellAtPos( *aSttCol, *aSttRow,
                                                         sal_False );
    if( pCellFrm )
    {
        sal_Int32 nBottom = pCellFrm->Frm().Bottom();
        nBottom -= GetFrm()->Frm().Top();
        Int32Set_Impl::const_iterator aEndRow( 
                GetTableData().GetRows().upper_bound( nBottom ) );
        nExtend =
             static_cast< sal_Int32 >( ::std::distance( aSttRow, aEndRow ) );
    }
        
    return nExtend;
}

sal_Int32 SAL_CALL SwAccessibleTable::getAccessibleColumnExtentAt(
               sal_Int32 nRow, sal_Int32 nColumn ) 
    throw (IndexOutOfBoundsException, RuntimeException)
{
    sal_Int32 nExtend = -1;

    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    GetTableData().CheckRowAndCol( nRow, nColumn, this );

    Int32Set_Impl::const_iterator aSttCol(
                                    GetTableData().GetColumnIter( nColumn ) );
    Int32Set_Impl::const_iterator aSttRow(
                                    GetTableData().GetRowIter( nRow ) );
    const SwFrm *pCellFrm = GetTableData().GetCellAtPos( *aSttCol, *aSttRow,
                                                         sal_False );
    if( pCellFrm )
    {
        sal_Int32 nRight = pCellFrm->Frm().Right();
        nRight -= GetFrm()->Frm().Left();
        Int32Set_Impl::const_iterator aEndCol( 
                GetTableData().GetColumns().upper_bound( nRight ) );
        nExtend =
             static_cast< sal_Int32 >( ::std::distance( aSttCol, aEndCol ) );
    }
        
    return nExtend;
}

Reference< XAccessibleTable > SAL_CALL
        SwAccessibleTable::getAccessibleRowHeaders(  )
    throw (RuntimeException)
{
    // Column headers aren't supported
    Reference< XAccessibleTable > xRet;
    return xRet;
}

Reference< XAccessibleTable > SAL_CALL
        SwAccessibleTable::getAccessibleColumnHeaders(  )
    throw (RuntimeException)
{
    // TODO Column headers aren't supported, arent they?
    Reference< XAccessibleTable > xRet;
    return xRet;
}

Sequence< sal_Int32 > SAL_CALL SwAccessibleTable::getSelectedAccessibleRows()
    throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    const SwSelBoxes *pSelBoxes = GetSelBoxes();
    if( pSelBoxes )
    {
        sal_Int32 nRows = GetTableData().GetRowCount();
        SwAccAllTableSelHander_Impl aSelRows( nRows  );

        GetTableData().GetSelection( 0, nRows, *pSelBoxes, aSelRows,
                                      sal_False );

        return aSelRows.GetSelSequence();
    }
    else
    {
        Sequence< sal_Int32 > aRet( 0 );
        return aRet;
    }
}

Sequence< sal_Int32 > SAL_CALL SwAccessibleTable::getSelectedAccessibleColumns()
    throw (RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    const SwSelBoxes *pSelBoxes = GetSelBoxes();
    if( pSelBoxes )
    {
        sal_Int32 nCols = GetTableData().GetColumnCount();
        SwAccAllTableSelHander_Impl aSelCols( nCols );

        GetTableData().GetSelection( 0, nCols, *pSelBoxes, aSelCols, sal_True );

        return aSelCols.GetSelSequence();
    }
    else
    {
        Sequence< sal_Int32 > aRet( 0 );
        return aRet;
    }
}

sal_Bool SAL_CALL SwAccessibleTable::isAccessibleRowSelected( sal_Int32 nRow )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    GetTableData().CheckRowAndCol( nRow, 0, this );

    sal_Bool bRet;
    const SwSelBoxes *pSelBoxes = GetSelBoxes();
    if( pSelBoxes )
    {
        SwAccSingleTableSelHander_Impl aSelRow;
        GetTableData().GetSelection( nRow, nRow+1, *pSelBoxes, aSelRow,
                                     sal_False );
        bRet = aSelRow.IsSelected();
    }
    else
    {
        bRet = sal_False;
    }

    return bRet;
}

sal_Bool SAL_CALL SwAccessibleTable::isAccessibleColumnSelected(
        sal_Int32 nColumn )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    GetTableData().CheckRowAndCol( 0, nColumn, this );

    sal_Bool bRet;
    const SwSelBoxes *pSelBoxes = GetSelBoxes();
    if( pSelBoxes )
    {
        SwAccSingleTableSelHander_Impl aSelCol;

        GetTableData().GetSelection( nColumn, nColumn+1, *pSelBoxes, aSelCol,
                                     sal_True );
        bRet = aSelCol.IsSelected();
    }
    else
    {
        bRet = sal_False;
    }

    return bRet;
}

Reference< XAccessible > SAL_CALL SwAccessibleTable::getAccessibleCellAt(
        sal_Int32 nRow, sal_Int32 nColumn )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    Reference< XAccessible > xRet;

    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    const SwFrm *pCellFrm = 
                    GetTableData().GetCell( nRow, nColumn, sal_False, this );
    if( pCellFrm )
        xRet = GetMap()->GetContext( pCellFrm, sal_True );

    return xRet;
}

Reference< XAccessible > SAL_CALL SwAccessibleTable::getAccessibleCaption()
    throw (RuntimeException)
{
    // captions aren't supported
    Reference< XAccessible > xRet;
    return xRet;
}

Reference< XAccessible > SAL_CALL SwAccessibleTable::getAccessibleSummary() 
    throw (RuntimeException)
{
    // summaries aren't supported
    Reference< XAccessible > xRet;
    return xRet;
}

sal_Bool SAL_CALL SwAccessibleTable::isAccessibleSelected(
            sal_Int32 nRow, sal_Int32 nColumn )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    sal_Bool bRet = sal_False;

    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    const SwFrm *pFrm =
                    GetTableData().GetCell( nRow, nColumn, sal_False, this );
    if( pFrm && pFrm->IsCellFrm() )
    {
        const SwSelBoxes *pSelBoxes = GetSelBoxes();
        if( pSelBoxes )
        {
            const SwCellFrm *pCFrm = static_cast < const SwCellFrm * >( pFrm );
            SwTableBox *pBox =
                const_cast< SwTableBox *>( pCFrm->GetTabBox() ); //SVPtrArr!
            bRet = pSelBoxes->Seek_Entry( pBox );
        }
    }

    return bRet;
}

sal_Int32 SAL_CALL SwAccessibleTable::getAccessibleIndex(
            sal_Int32 nRow, sal_Int32 nColumn )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    sal_Int32 nRet = -1;

    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    SwFrmOrObj aCell( GetTableData().GetCell( nRow, nColumn, sal_False, this ));
    if( aCell.IsValid() )
        nRet = GetChildIndex( aCell );

    return nRet;
}

sal_Int32 SAL_CALL SwAccessibleTable::getAccessibleRow( sal_Int32 nChildIndex )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    sal_Int32 nRet = -1;

    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    SwFrmOrObj aCell( GetChild( nChildIndex ) );
    if( aCell.GetSwFrm()  )
    {
        sal_Int32 nTop = aCell.GetSwFrm()->Frm().Top();
        nTop -= GetFrm()->Frm().Top();
        Int32Set_Impl::const_iterator aRow( 
                GetTableData().GetRows().lower_bound( nTop ) );
        nRet = static_cast< sal_Int32 >( ::std::distance( 
                    GetTableData().GetRows().begin(), aRow ) );
    }

    return nRet;
}

sal_Int32 SAL_CALL SwAccessibleTable::getAccessibleColumn(
        sal_Int32 nChildIndex )
    throw (IndexOutOfBoundsException, RuntimeException)
{
    sal_Int32 nRet = -1;

    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleTable )

    SwFrmOrObj aCell( GetChild( nChildIndex ) );
    if( aCell.GetSwFrm()  )
    {
        sal_Int32 nLeft = aCell.GetSwFrm()->Frm().Left();
        nLeft -= GetFrm()->Frm().Left();
        Int32Set_Impl::const_iterator aCol( 
                GetTableData().GetColumns().lower_bound( nLeft ) );
        nRet = static_cast< sal_Int32 >( ::std::distance( 
                    GetTableData().GetColumns().begin(), aCol ) );
    }

    return nRet;
}


OUString SAL_CALL SwAccessibleTable::getImplementationName()
        throw( RuntimeException )
{
    return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationName));
}

sal_Bool SAL_CALL SwAccessibleTable::supportsService(
        const ::rtl::OUString& sTestServiceName)
    throw (::com::sun::star::uno::RuntimeException)
{
    return sTestServiceName.equalsAsciiL( sServiceName, sizeof(sServiceName)-1 );
}

Sequence< OUString > SAL_CALL SwAccessibleTable::getSupportedServiceNames()
        throw( ::com::sun::star::uno::RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceName) );
    return aRet;
}

void SwAccessibleTable::InvalidatePosOrSize( const SwRect& rOldBox )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if( HasTableData() )
    {
        GetTableData().SetTablePos( GetFrm()->Frm().Pos() );

        const SwTabFrm* pTabFrm = static_cast<const SwTabFrm*>( GetFrm() );

        SwAccessibleTableData_Impl *pOldTableData = mpTableData;

        if( mpEvents )
        {
            mpTableData = new SwAccessibleTableData_Impl( pTabFrm );
            sal_Bool bFireEvent = sal_False;

            SwAccessibleTableEventList_Impl::const_iterator aIter( mpEvents->begin() );
            SwAccessibleTableEventList_Impl::const_iterator aEndIter( mpEvents->end() );
            while( !bFireEvent && aIter != aEndIter )
            {
                const SwAccessibleTableEvent_Impl& rEvent = *aIter;
                if( rEvent.GetFrm() && rEvent.GetRowExtent() > 0 &&
                    rEvent.GetColumnExtent() > 0 )
                {
                    // the frame did extist before.
                    sal_Int32 nRow, nCol, nRowExt, nColExt;
                    mpTableData->GetRowColumnAndExtent( 
                            rEvent.GetFrm()->Frm(), nRow, nCol, nRowExt, nColExt );
                    if( nRow != rEvent.GetRow() || nCol != rEvent.GetColumn() ||
                        nRowExt != rEvent.GetRowExtent() ||
                        nColExt != rEvent.GetColumnExtent() )
                    {
                        bFireEvent = sal_True;
                    }
                }
                else
                {
                    // in any other case currently change the whole model.
                    bFireEvent = sal_True;
                }

                ++aIter;
            }

            if( bFireEvent )
                FireTableChangeEvent( pOldTableData );
            
            delete mpEvents;
            mpEvents = 0;
        }
        else
        {
            mpTableData = 0;
        }

        delete pOldTableData;
    }

    SwAccessibleContext::InvalidatePosOrSize( rOldBox );
}

void SwAccessibleTable::DisposeChild( const SwFrm *pFrm, 
                                    sal_Bool bRecursive )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if( HasTableData() )
    {
        if( GetMap()->GetShell()->ActionPend() )
        {
            AppendDisposeEvent( pFrm );
        }
        else
        {
            const SwTabFrm* pTabFrm = static_cast<const SwTabFrm*>( GetFrm() );
            SwAccessibleTableData_Impl *pOldTableData = mpTableData;
            mpTableData = new SwAccessibleTableData_Impl( pTabFrm );
            FireTableChangeEvent( pOldTableData );
            delete pOldTableData;
        }
    }

    // There are two reason why this method has been called. The first one
    // is there is no context for pFrm. The method is them called by
    // the map, and we have to call our superclass.
    // The other situation is that we have been call by a call to get notified
    // about its change. We then must not call the superclass
    Reference< XAccessible > xAcc( GetMap()->GetContext( pFrm, sal_False ) );
    if( !xAcc.is() )
        SwAccessibleContext::DisposeChild( pFrm, bRecursive );
}

void SwAccessibleTable::InvalidateChildPosOrSize( const SwFrm *pFrm,
                                                  const SwRect& rOldBox )
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    if( HasTableData() )
    {
        if( GetMap()->GetShell()->ActionPend() )
        {
            AppendPosOrSizeEvent( pFrm, rOldBox );
        }
        else
        {
            const SwTabFrm* pTabFrm = static_cast<const SwTabFrm*>( GetFrm() );
            SwAccessibleTableData_Impl *pOldTableData = mpTableData;
            mpTableData = new SwAccessibleTableData_Impl( pTabFrm );
            FireTableChangeEvent( pOldTableData );
            delete pOldTableData;
        }
    }

    // There are two reason why this method has been called. The first one
    // is there is no context for pFrm. The method is them called by
    // the map, and we have to call our superclass.
    // The other situation is that we have been call by a call to get notified
    // about its change. We then must not call the superclass
    Reference< XAccessible > xAcc( GetMap()->GetContext( pFrm, sal_False ) );
    if( !xAcc.is() )
        SwAccessibleContext::InvalidateChildPosOrSize( pFrm, rOldBox );
}
