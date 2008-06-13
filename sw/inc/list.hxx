/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: list.hxx,v $
 * $Revision: 1.2 $
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

#ifndef LIST_HXX_INCLUDED
#define LIST_HXX_INCLUDED

#include <tools/string.hxx>

class SwNumRule;
class SwNodes;
class SwNodeNum;

class SwListImpl;

class SwList
{
    public:
        SwList( const String sListId,
                SwNumRule& rDefaultListStyle,
                const SwNodes& rNodes );
        ~SwList();

        const String GetListId() const;

        const String GetDefaultListStyleName() const;

        void InsertListItem( SwNodeNum& rNodeNum,
                             const int nLevel );
        void RemoveListItem( SwNodeNum& rNodeNum );

        void InvalidateListTree();
        void ValidateListTree();

        void MarkListLevel( const int nListLevel,
                            const BOOL bValue );

        bool IsListLevelMarked( const int nListLevel ) const;

//        void ContinueList( SwList& rList );
//        const SwList* GetContinuedList() const;
//        void ClearContinuation();

    private:
        // no copy constructor and no assignment operator
        SwList( const SwList& );
        SwList& operator=( const SwList& );

        SwListImpl* mpListImpl;
};
#endif // LIST_HXX_INCLUDED