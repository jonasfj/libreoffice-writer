 /*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: acchypertextdata.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 20:36:58 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifndef _ACCHYPERLINK_HXX
#include <acchyperlink.hxx>
#endif
#ifndef _ACCHYPERTEXTDATA_HXX
#include <acchypertextdata.hxx>
#endif

using namespace ::com::sun::star::accessibility;
using namespace ::com::sun::star::uno;

SwAccessibleHyperTextData::SwAccessibleHyperTextData() :
    _SwAccessibleHyperlinkMap_Impl()
{
}

SwAccessibleHyperTextData::~SwAccessibleHyperTextData()
{
    iterator aIter = begin();
    while( aIter != end() )
    {
        Reference < XAccessibleHyperlink > xTmp = (*aIter).second;
        if( xTmp.is() )
        {
            SwAccessibleHyperlink *pTmp =
                static_cast< SwAccessibleHyperlink * >( xTmp.get() );
            pTmp->Invalidate();
        }
        ++aIter;
    }
}
