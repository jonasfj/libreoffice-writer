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
#ifndef _FMTROWSPLT_HXX
#define _FMTROWSPLT_HXX

#include <svl/eitem.hxx>
#include "swdllapi.h"
#include <hintids.hxx>
#include <format.hxx>

class IntlWrapper;

class SW_DLLPUBLIC SwFmtRowSplit : public SfxBoolItem
{
public:
    SwFmtRowSplit( BOOL bSplit = TRUE ) : SfxBoolItem( RES_ROW_SPLIT, bSplit ) {}

    // "pure virtual Methoden" vom SfxPoolItem
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;
};

inline const SwFmtRowSplit &SwAttrSet::GetRowSplit(BOOL bInP) const
    { return (const SwFmtRowSplit&)Get( RES_ROW_SPLIT,bInP); }

inline const SwFmtRowSplit &SwFmt::GetRowSplit(BOOL bInP) const
    { return aSet.GetRowSplit(bInP); }

#endif

