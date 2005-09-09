/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: edfldexp.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:27:15 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#pragma hdrstop

#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _DBFLD_HXX
#include <dbfld.hxx>
#endif
#ifndef _DBMGR_HXX
#include <dbmgr.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _NDTXT_HXX
#include <ndtxt.hxx>		// GetCurFld
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>		// SwRefMarkFldUpdate
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _EDIMP_HXX
#include <edimp.hxx>
#endif
#ifndef _FLDDAT_HXX
#include <flddat.hxx>
#endif

using namespace com::sun::star::uno;
using namespace com::sun::star::container;
using namespace com::sun::star::lang;
using namespace ::rtl;

/* -----------------28.11.2002 17:53-----------------
 * 
 * --------------------------------------------------*/
BOOL SwEditShell::IsFieldDataSourceAvailable(String& rUsedDataSource) const
{
    const SwFldTypes * pFldTypes = GetDoc()->GetFldTypes();
    const USHORT nSize = pFldTypes->Count();
    Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
    if( !xMgr.is() )
        return FALSE;
    Reference<XInterface> xInstance = xMgr->createInstance( OUString::createFromAscii( "com.sun.star.sdb.DatabaseContext" ));
    Reference<XNameAccess>  xDBContext = Reference<XNameAccess>(xInstance, UNO_QUERY) ;
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
                            catch(Exception)
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


