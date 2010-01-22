/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: warnpassword.cxx,v $
 * $Revision: 1.7 $
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
+ ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
// ============================================================================
#include "warnpassword.hxx"
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <svl/itemset.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/sfxsids.hrc>
#include <ucbhelper/simpleinteractionrequest.hxx>
#include <com/sun/star/ucb/InteractiveAppException.hpp>
#include <svx/svxerr.hxx>

using ::rtl::OUString;

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::task;

bool SwWarnPassword::WarningOnPassword( SfxMedium& rMedium )
{
    bool bReturn = true;
    uno::Reference< task::XInteractionHandler > xHandler( rMedium.GetInteractionHandler());
    if( xHandler.is() )
    {

        OUString empty;
        uno::Any xException( makeAny(InteractiveAppException(empty,
                uno::Reference <XInterface> (),
            InteractionClassification_QUERY,
                 ERRCODE_SVX_EXPORT_FILTER_CRYPT)));

        uno::Reference< ucbhelper::SimpleInteractionRequest > xRequest
                    = new ucbhelper::SimpleInteractionRequest(
                        xException,
                 ucbhelper::CONTINUATION_APPROVE
                           | ucbhelper::CONTINUATION_DISAPPROVE );

        xHandler->handle( xRequest.get() );

        const sal_Int32 nResp = xRequest->getResponse();

        switch ( nResp )
        {
        case ucbhelper::CONTINUATION_UNKNOWN:
                break;

        case ucbhelper::CONTINUATION_APPROVE:
                // Continue
                break;

        case ucbhelper::CONTINUATION_DISAPPROVE:
                bReturn = false;
                break;
        }
    }
    return bReturn;
}

