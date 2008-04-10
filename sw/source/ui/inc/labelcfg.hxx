/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: labelcfg.hxx,v $
 * $Revision: 1.5 $
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
#ifndef _LABELCFG_HXX
#define _LABELCFG_HXX

#include <unotools/configitem.hxx>
#include "swdllapi.h"

class SwLabRecs;
class SwLabRec;

class SW_DLLPUBLIC SwLabelConfig : public utl::ConfigItem
{
    com::sun::star::uno::Sequence<rtl::OUString> aNodeNames;

    SW_DLLPRIVATE com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();

public:
    SwLabelConfig();
    virtual ~SwLabelConfig();

    virtual void			Commit();

    void	FillLabels(const rtl::OUString& rManufacturer, SwLabRecs& rLabArr);
    const com::sun::star::uno::Sequence<rtl::OUString>&
            GetManufacturers() const {return aNodeNames;}

    sal_Bool	HasLabel(const rtl::OUString& rManufacturer, const rtl::OUString& rType);
    void		SaveLabel(const rtl::OUString& rManufacturer, const rtl::OUString& rType,
                            const SwLabRec& rRec);
};

#endif

