/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: threadlistener.hxx,v $
 * $Revision: 1.3 $
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
#ifndef _THREADLISTENER_HXX
#define _THREADLISTENER_HXX

#include <ifinishedthreadlistener.hxx>
#include <ithreadlistenerowner.hxx>
#include <observablethread.hxx>

/** helper class to observe threads

    OD 2007-03-30 #i73788#
    An instance of this class can be used to observe a thread in order to
    be notified, if the thread has finished its work. The notification is
    directly forward to its owner - an instance of IThreadListenerOwner
    Note:
    - A thread can only have one or none listener.
    - The notification is performed via the ThreadID

    @author OD
*/
class ThreadListener : public IFinishedThreadListener
{
    public:

        ThreadListener( IThreadListenerOwner& rThreadListenerOwner );
        virtual ~ThreadListener();

        void ListenToThread( const oslInterlockedCount nThreadID,
                             ObservableThread& rThread );

        virtual void NotifyAboutFinishedThread( const oslInterlockedCount nThreadID );

    private:

        IThreadListenerOwner& mrThreadListenerOwner;
};
#endif
