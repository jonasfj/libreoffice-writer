/*************************************************************************
 *
 *  $RCSfile: dbtree.hxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:39 $
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
#ifndef _DBTREE_HXX
#define _DBTREE_HXX

#ifndef _SVTREEBOX_HXX //autogen
#include <svtools/svtreebx.hxx>
#endif

#ifdef REPLACE_OFADBMGR
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
namespace com{namespace sun{namespace star{namespace container{
    class XNameAccess;
}}}}
#endif

#include "swtypes.hxx"

class SwDBTreeList : public SvTreeListBox
{
    Bitmap			aRootOpened;
    Bitmap 			aRootClosed;
    Image			aDBBMP;
    Image			aTableBMP;
    Image			aQueryBMP;

    String			sDefDBName;
    BOOL			bInitialized;
    BOOL			bShowColumns;

#ifdef REPLACE_OFADBMGR
    com::sun::star::uno::Reference<com::sun::star::container::XNameAccess> xDBContext;
#endif
    DECL_LINK( DBCompare, SvSortData* );

    void			InitTreeList();
    virtual void	RequestingChilds( SvLBoxEntry* pParent );

    virtual void	Command( const CommandEvent& rCEvt );
    virtual BOOL	QueryDrop( DropEvent& rEvt);

    void			StartExecuteDrag();

public:
     SwDBTreeList(Window* pParent, const ResId& rResId, const String& rDefDBName = aEmptyStr, const BOOL bShowCol = FALSE );
    ~SwDBTreeList();

#ifdef REPLACE_OFADBMGR
    String  GetDBName(String& rTableName, String& rColumnName, BOOL* pbIsTable = 0);
    void	Select(const String& rDBName, const String& rTableName, const String& rColumnName);
#else
    String	GetDBName() const;
    void	Select(const String& rDBName);		// Format: Datenbank.Tabelle
#endif

    void	Show();
    void	ShowColumns(BOOL bShowCol);
};

#endif

