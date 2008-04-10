/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: condedit.hxx,v $
 * $Revision: 1.8 $
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
#ifndef _CONDEDTI_HXX
#define _CONDEDTI_HXX

#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#include <svtools/transfer.hxx>
#include "swdllapi.h"

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

class SW_DLLPUBLIC ConditionEdit : public Edit, public DropTargetHelper
{
    BOOL bBrackets, bEnableDrop;

    SW_DLLPRIVATE virtual sal_Int8 	AcceptDrop( const AcceptDropEvent& rEvt );
    SW_DLLPRIVATE virtual sal_Int8 	ExecuteDrop( const ExecuteDropEvent& rEvt );

public:
    ConditionEdit( Window* pParent, const ResId& rResId );

    inline void ShowBrackets(BOOL bShow) 		{ bBrackets = bShow; }

    inline void SetDropEnable( BOOL bFlag )		{ bEnableDrop = bFlag; }
    inline BOOL IsDropEnable() const 			{ return bEnableDrop; }
};

#endif
















