/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dbtree.hxx,v $
 * $Revision: 1.14 $
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
#ifndef _DBTREE_HXX
#define _DBTREE_HXX

#include <svtools/svtreebx.hxx>
#include <com/sun/star/uno/Reference.h>

namespace com{namespace sun{namespace star{namespace container{
    class XNameAccess;
}}}}
#include "swdllapi.h"
#include <swtypes.hxx>
#include <swunodef.hxx>

class SwDBTreeList_Impl;
class SwWrtShell;

class SW_DLLPUBLIC SwDBTreeList : public SvTreeListBox
{
    ImageList       aImageList;
    ImageList       aImageListHC;
    Image			aDBBMP;
    Image			aTableBMP;
    Image			aQueryBMP;

    String			sDefDBName;
    BOOL			bInitialized;
    BOOL			bShowColumns;

    SwDBTreeList_Impl* pImpl;

    SW_DLLPRIVATE DECL_LINK( DBCompare, SvSortData* );

    SW_DLLPRIVATE void			InitTreeList();
    SW_DLLPRIVATE virtual void	RequestingChilds( SvLBoxEntry* pParent );

    SW_DLLPRIVATE virtual sal_Int8 AcceptDrop( const AcceptDropEvent& rEvt );
    SW_DLLPRIVATE virtual void 	StartDrag( sal_Int8 nAction, const Point& rPosPixel );

    using SvTreeListBox::Select;

public:
     SwDBTreeList( Window* pParent, const ResId& rResId,
                    SwWrtShell* pSh,
                    const String& rDefDBName = aEmptyStr,
                    const BOOL bShowCol = FALSE );
    virtual ~SwDBTreeList();

    String  GetDBName( String& rTableName, String& rColumnName, BOOL* pbIsTable = 0);

    void	Select( const String& rDBName, const String& rTableName,
                    const String& rColumnName );

    void	ShowColumns(BOOL bShowCol);
    void    SetWrtShell(SwWrtShell& rSh);
    
    void    AddDataSource(const String& rSource);
};

#endif

