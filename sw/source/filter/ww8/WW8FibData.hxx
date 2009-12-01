/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile:$
 * $Revision:$
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
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
#ifndef INCLUDED_WW8_FIB_DATA_HXX
#define INCLUDED_WW8_FIB_DATA_HXX
#include <IDocumentExternalData.hxx>

namespace ww8
{
class WW8FibData : public ::sw::ExternalData
{
    bool m_bReadOnlyRecommended;
    bool m_bWriteReservation;

public:
    WW8FibData();
    virtual ~WW8FibData();

    void setReadOnlyRecommended(bool bReadOnlyRecommended);
    void setWriteReservation(bool bWriteReservation);
    
    bool getReadOnlyRecommended() const;
    bool getWriteReservation() const;
};
}

#endif // INCLUDED_WW8_FIB_DATA_HXX
