/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: hfspacingitem.hxx,v $
 * $Revision: 1.4 $
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
#ifndef _SW_HF_EAT_SPACINGITEM_HXX
#define _SW_HF_EAT_SPACINGITEM_HXX


#include <svtools/eitem.hxx>
#include <hintids.hxx>
#include <format.hxx>

class IntlWrapper;

class SW_DLLPUBLIC SwHeaderAndFooterEatSpacingItem : public SfxBoolItem
{
public:
    SwHeaderAndFooterEatSpacingItem( USHORT nId = RES_HEADER_FOOTER_EAT_SPACING,
                                     BOOL bPrt = FALSE ) : SfxBoolItem( nId, bPrt ) {}

    // "pure virtual Methoden" vom SfxPoolItem
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;
};

inline const SwHeaderAndFooterEatSpacingItem &SwAttrSet::GetHeaderAndFooterEatSpacing(BOOL bInP) const
    { return (const SwHeaderAndFooterEatSpacingItem&)Get( RES_HEADER_FOOTER_EAT_SPACING,bInP); }

inline const SwHeaderAndFooterEatSpacingItem &SwFmt::GetHeaderAndFooterEatSpacing(BOOL bInP) const
    { return aSet.GetHeaderAndFooterEatSpacing(bInP); }

#endif

