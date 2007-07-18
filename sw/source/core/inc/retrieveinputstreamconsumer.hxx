/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: retrieveinputstreamconsumer.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-18 13:34:31 $
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
#ifndef _RETRIEVEINPUTSTREAMCONSUMER_HXX
#define _RETRIEVEINPUTSTREAMCONSUMER_HXX

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HXX_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _OSL_INTERLOCK_H_
#include <osl/interlck.h>
#endif

#ifndef _RETRIEVEDINPUTSTREAMDATA_HXX
#include <retrievedinputstreamdata.hxx>
#endif

class SwGrfNode;
class String;
class SwAsyncRetrieveInputStreamThread;

/** class to provide creation of a thread to retrieve an input stream given by
    an URL and to consume the retrieved input stream.

    OD 2007-01-29 #i73788#

    @author OD
*/
class SwAsyncRetrieveInputStreamThreadConsumer
{
    public:

        SwAsyncRetrieveInputStreamThreadConsumer( SwGrfNode& rGrfNode );
        ~SwAsyncRetrieveInputStreamThreadConsumer();

        /** method to create thread

            @author OD
        */
        void CreateThread( const String& rURL );

        /** method called to provide the retrieved input stream to the thread Consumer

            @author OD
        */
        void ApplyInputStream(
            com::sun::star::uno::Reference<com::sun::star::io::XInputStream> xInputStream,
            const sal_Bool bIsStreamReadOnly );

    private:

        SwGrfNode& mrGrfNode;
        oslInterlockedCount mnThreadID;
};
#endif
