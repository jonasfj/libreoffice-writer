/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: edfldexp.cxx,v $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
#include <editsh.hxx>
#include <dbfld.hxx>
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#include <com/sun/star/container/XNameAccess.hpp>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <doc.hxx>
#include <docary.hxx>
#include <ndtxt.hxx>		// GetCurFld
#include <txtfld.hxx>
#include <fmtfld.hxx>
#include <edimp.hxx>
#include <flddat.hxx>

using namespace com::sun::star;
using ::rtl::OUString;

/* -----------------28.11.2002 17:53-----------------
 *
 * --------------------------------------------------*/
BOOL SwEditShell::IsFieldDataSourceAvailable(String& rUsedDataSource) const
{
    const SwFldTypes * pFldTypes = GetDoc()->GetFldTypes();
    const USHORT nSize = pFldTypes->Count();
    uno::Reference< lang::XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
    if( !xMgr.is() )
        return FALSE;
    uno::Reference<uno::XInterface> xInstance = xMgr->createInstance( OUString::createFromAscii( "com.sun.star.sdb.DatabaseContext" ));
    uno::Reference<container::XNameAccess> xDBContext(xInstance, uno::UNO_QUERY) ;
    if(!xDBContext.is())
        return FALSE;
    for(USHORT i = 0; i < nSize; ++i)
    {
        SwFieldType& rFldType = *((*pFldTypes)[i]);
        USHORT nWhich = rFldType.Which();
        if(IsUsed(rFldType))
        {
            switch(nWhich)
            {
                case RES_DBFLD:
                {
                    SwClientIter aIter( rFldType );
                    SwFmtFld* pFld = (SwFmtFld*)aIter.First( TYPE( SwFmtFld ));
                    while(pFld)
                    {
                        if(pFld->IsFldInDoc())
                        {
                            const SwDBData& rData =
                                    ((SwDBFieldType*)pFld->GetFld()->GetTyp())->GetDBData();
                            try
                            {
                                return xDBContext->getByName(rData.sDataSource).hasValue();
                            }
                            catch(uno::Exception const &)
                            {
                                rUsedDataSource = rData.sDataSource;
                                return FALSE;
                            }
                        }
                        pFld = (SwFmtFld*)aIter.Next();
                    }
                }
                break;
            }
        }
    }
    return TRUE;
}


