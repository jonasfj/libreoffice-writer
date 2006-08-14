/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sortedobjs.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 16:22:21 $
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
#ifndef _SORTEDOBJS_HXX
#define _SORTEDOBJS_HXX
class SwSortedObjsImpl;
class SwAnchoredObject;

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

/** class for collecting anchored objects

    for #i28701#
    Anchored objects can be inserted and deleted. The entries can be directly
    accessed via index.
    An anchored object is inserted sorted. The sort criteria are:
    - anchor type
      - order 1: to-page, 2: to-fly, 3: to-paragraph|to-character|as-character
    - anchor node
    - wrapping style (inclusive layer)
      - order 1: wrapping style != SURROUND_THROUGHT and not in hell layer,
        2: wrapping style = SURROUND_THROUGHT or in hell layer
    - wrapping style influence
      - order 1: NONE_SUCCESSIVE_POSITIONED, 2: NONE_CONCURRENT_POSITIONED
    - again anchor type
      - order 1: to-paragraph, 2: to-character, 3: as-character
    - anchor node position
    - internal anchor order number
    If one of the sort criteria attributes of an anchored object changes,
    the sorting has to be updated - use method <Update(..)>

    @author OD
*/
class SwSortedObjs
{
    private:
        SwSortedObjsImpl* mpImpl;

    public:
        SwSortedObjs();
        ~SwSortedObjs();

        sal_uInt32 Count() const;

        /** direct access to the entries

            @param _nIndex
            input parameter - index of entry, valid value range [0..Count()-1]

            @author OD
        */
        SwAnchoredObject* operator[]( sal_uInt32 _nIndex ) const;

        bool Insert( SwAnchoredObject& _rAnchoredObj );

        bool Remove( SwAnchoredObject& _rAnchoredObj );

        bool Contains( const SwAnchoredObject& _rAnchoredObj ) const;

        /** method to update the position of the given anchored object in the
            sorted list

            OD 2004-07-01

            @author OD

            @return boolean, indicating success of the update.
        */
        bool Update( SwAnchoredObject& _rAnchoredObj );

        /** Position of object <_rAnchoredObj> in sorted list

            OD 2004-05-07
            Returns the number of the list position of object <_rAnchoredObj>.
            Returns <Count()>, if object isn't contained in list.

            @author OD

            @return sal_uInt32
            Number of the list position of object <_rAnchoredObj>
        */
        sal_uInt32 ListPosOf( const SwAnchoredObject& _rAnchoredObj ) const;
};

#endif
