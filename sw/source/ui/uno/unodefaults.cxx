/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unodefaults.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 23:29:37 $
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



#ifndef _UNODEFAULTS_HXX
#include <unodefaults.hxx>
#endif

#ifndef _SVDMODEL_HXX
#include <svx/svdmodel.hxx>
#endif
#include <svx/unoprov.hxx>
#include <doc.hxx>

/* -----------------------------13.03.01 14:16--------------------------------

 ---------------------------------------------------------------------------*/
SwSvxUnoDrawPool::SwSvxUnoDrawPool( SwDoc* pDoc ) throw() :
    SvxUnoDrawPool(pDoc->GetDrawModel(), SVXUNO_SERVICEID_COM_SUN_STAR_DRAWING_DEFAULTS_WRITER),
    m_pDoc(pDoc)
{
}
/* -----------------------------13.03.01 14:16--------------------------------

 ---------------------------------------------------------------------------*/
SwSvxUnoDrawPool::~SwSvxUnoDrawPool() throw()
{
}
/* -----------------------------13.03.01 14:16--------------------------------

 ---------------------------------------------------------------------------*/
SfxItemPool* SwSvxUnoDrawPool::getModelPool( sal_Bool bReadOnly ) throw()
{
    if(m_pDoc)
    {

        // DVO, OD 01.10.2003 #i18732# - return item pool of writer document;
        // it contains draw model item pool as secondary pool.
        //SdrModel* pModel = m_pDoc->MakeDrawModel();
        //return &pModel->GetItemPool();
        // --> OD 2005-08-08 #i52858# - method name changed
        m_pDoc->GetOrCreateDrawModel();
        // <--
        return &(m_pDoc->GetAttrPool());
    }
    return 0;
}



