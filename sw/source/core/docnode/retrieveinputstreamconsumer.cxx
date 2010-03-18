/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include "precompiled_sw.hxx"
#include <retrieveinputstreamconsumer.hxx>
#include <ndgrf.hxx>
#include <retrieveinputstream.hxx>
#include <swthreadmanager.hxx>

/** class to provide creation of a thread to retrieve an input stream given by
    an URL and to consume the retrieved input stream.

    OD 2007-01-29 #i73788#

    @author OD
*/
SwAsyncRetrieveInputStreamThreadConsumer::SwAsyncRetrieveInputStreamThreadConsumer(
                                                            SwGrfNode& rGrfNode )
    : mrGrfNode( rGrfNode ),
      mnThreadID( 0 )
{
}

SwAsyncRetrieveInputStreamThreadConsumer::~SwAsyncRetrieveInputStreamThreadConsumer()
{
    SwThreadManager::GetThreadManager().RemoveThread( mnThreadID );
}

void SwAsyncRetrieveInputStreamThreadConsumer::CreateThread( const String& rURL )
{
    // Get new data container for input stream data
    SwRetrievedInputStreamDataManager::tDataKey nDataKey =
            SwRetrievedInputStreamDataManager::GetManager().ReserveData(
                                                mrGrfNode.GetThreadConsumer() );

    rtl::Reference< ObservableThread > pNewThread =
        SwAsyncRetrieveInputStreamThread::createThread( nDataKey, rURL );

    // Add thread to thread manager and pass ownership of thread to thread manager.
    mnThreadID = SwThreadManager::GetThreadManager().AddThread( pNewThread );
}

void SwAsyncRetrieveInputStreamThreadConsumer::ApplyInputStream(
    com::sun::star::uno::Reference<com::sun::star::io::XInputStream> xInputStream,
    const sal_Bool bIsStreamReadOnly )
{
    mrGrfNode.ApplyInputStream( xInputStream, bIsStreamReadOnly );
}

