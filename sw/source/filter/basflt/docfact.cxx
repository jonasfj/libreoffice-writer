/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docfact.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:34:58 $
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

#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif

#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif


/******************************************************************************
 *	Methode		:	SwDocFac::SwDocFac( SwDoc *pDoc )
 *	Beschreibung:
 *	Erstellt	:	OK 01-24-94 11:32am
 *	Aenderung	:	OK 01-24-94 11:32am
 ******************************************************************************/


SwDocFac::SwDocFac( SwDoc *pDc )
    : pDoc( pDc )
{
    if( pDoc )
        pDoc->AddLink();
}

/******************************************************************************
 *	Methode		:   SwDocFac::~SwDocFac()
 *	Beschreibung:
 *	Erstellt	:	OK 01-24-94 11:33am
 *	Aenderung	:	OK 01-24-94 11:33am
 ******************************************************************************/


SwDocFac::~SwDocFac()
{
    if( pDoc && !pDoc->RemoveLink() )
        delete pDoc;
}

/******************************************************************************
 *	Methode		:	SwDoc *SwDocFac::GetDoc()
 *	Beschreibung:	Diese Methode legt immer einen Drucker an.
 *	Erstellt	:	OK 01-24-94 11:34am
 *	Aenderung	:	OK 01-24-94 11:34am
 ******************************************************************************/


SwDoc *SwDocFac::GetDoc()
{
    if( !pDoc )
    {
        pDoc = new SwDoc;
        pDoc->AddLink();
    }
    return pDoc;
}
