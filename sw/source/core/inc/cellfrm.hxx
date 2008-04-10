/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: cellfrm.hxx,v $
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
#ifndef _CELLFRM_HXX
#define _CELLFRM_HXX

#include <tools/mempool.hxx>

#include "layfrm.hxx"

class SwTableBox;
struct SwCrsrMoveState;
class SwBorderAttrs;

class SwCellFrm: public SwLayoutFrm
{
    const SwTableBox* pTabBox;

protected:
    virtual void Format( const SwBorderAttrs *pAttrs = 0 );

public:
    SwCellFrm( const SwTableBox &, bool bInsertContent = true );
    ~SwCellFrm();

    virtual BOOL GetCrsrOfst( SwPosition *, Point&, SwCrsrMoveState* = 0 ) const;
    virtual void Modify( SfxPoolItem*, SfxPoolItem* );
    virtual void Paint( const SwRect& ) const;
    virtual void CheckDirection( BOOL bVert );

    const SwTableBox *GetTabBox() const { return pTabBox; }

    // used for breaking table rows:
    SwCellFrm* GetFollowCell() const;
    SwCellFrm* GetPreviousCell() const;

    // used for rowspan stuff:
    const SwCellFrm& FindStartEndOfRowSpanCell( bool bStart, bool bCurrentTab ) const;
    long GetLayoutRowSpan() const;

    DECL_FIXEDMEMPOOL_NEWDEL(SwCellFrm)
};

#endif
