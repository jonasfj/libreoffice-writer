/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: bodyfrm.hxx,v $
 * $Revision: 1.6 $
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
#ifndef _BODYFRM_HXX
#define _BODYFRM_HXX

#include <tools/mempool.hxx>
#include "layfrm.hxx"

class SwBorderAttrs;

class SwBodyFrm: public SwLayoutFrm
{
protected:
    virtual void Format( const SwBorderAttrs *pAttrs = 0 );

#if defined ( GCC) && defined ( C272 )
    ~SwBodyFrm();
#endif
public:
    SwBodyFrm( SwFrmFmt* );

    DECL_FIXEDMEMPOOL_NEWDEL(SwBodyFrm)
};

#endif	//_BODYFRM_HXX
