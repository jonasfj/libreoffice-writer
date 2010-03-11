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
#include <finalthreadmanager.hxx>

#ifndef _OSL_THREAD_HXX_
#include <osl/thread.hxx>
#endif
#include <errhdl.hxx>
#include <pausethreadstarting.hxx>
#include <swthreadjoiner.hxx>

#include <com/sun/star/frame/XDesktop.hpp>
#include <rtl/ustring.hxx>
#include <com/sun/star/frame/XFramesSupplier.hpp>

namespace css = ::com::sun::star;

/** thread to cancel a give list of cancellable jobs

    helper class for FinalThreadManager

    @author OD
*/
class CancelJobsThread : public osl::Thread
{
    public:
        CancelJobsThread( std::list< css::uno::Reference< css::util::XCancellable > > aJobs )
            : osl::Thread(),
              maMutex(),
              maJobs( aJobs ),
              mbAllJobsCancelled( false ),
              mbStopped( false )
        {
        }

        virtual ~CancelJobsThread() {}

        void addJobs( std::list< css::uno::Reference< css::util::XCancellable > >& rJobs );

        bool allJobsCancelled() const;

        void stopWhenAllJobsCancelled();

    private:

        bool existJobs() const;

        css::uno::Reference< css::util::XCancellable > getNextJob();

        bool stopped() const;

        virtual void SAL_CALL run();

        mutable osl::Mutex maMutex;

        std::list< css::uno::Reference< css::util::XCancellable > > maJobs;

        bool mbAllJobsCancelled;
        bool mbStopped;
};

void CancelJobsThread::addJobs( std::list< css::uno::Reference< css::util::XCancellable > >& rJobs )
{
    osl::MutexGuard aGuard(maMutex);

    maJobs.insert( maJobs.end(), rJobs.begin(), rJobs.end() );
    mbAllJobsCancelled = !maJobs.empty();
}

bool CancelJobsThread::existJobs() const
{
    osl::MutexGuard aGuard(maMutex);

    return !maJobs.empty();
}

bool CancelJobsThread::allJobsCancelled() const
{
    osl::MutexGuard aGuard(maMutex);

    return maJobs.empty() && mbAllJobsCancelled;
}
void CancelJobsThread::stopWhenAllJobsCancelled()
{
    osl::MutexGuard aGuard(maMutex);

    mbStopped = true;
}

css::uno::Reference< css::util::XCancellable > CancelJobsThread::getNextJob()
{
    css::uno::Reference< css::util::XCancellable > xRet;

    {
        osl::MutexGuard aGuard(maMutex);

        if ( !maJobs.empty() )
        {
            xRet = maJobs.front();
            maJobs.pop_front();
        }
    }

    return xRet;
}

bool CancelJobsThread::stopped() const
{
    osl::MutexGuard aGuard(maMutex);

    return mbStopped;
}

void SAL_CALL CancelJobsThread::run()
{
    while ( !stopped() )
    {
        while ( existJobs() )
        {
            css::uno::Reference< css::util::XCancellable > aJob( getNextJob() );
            if ( aJob.is() )
            {
                aJob->cancel();
            }
        }

        mbAllJobsCancelled = true;

        {
            TimeValue aSleepTime;
            aSleepTime.Seconds = 1;
            aSleepTime.Nanosec = 0;
            osl_waitThread( &aSleepTime );
        }
    }
}

/** thread to terminate office, when all jobs are cancelled.

    helper class for FinalThreadManager

    @author OD
*/
class TerminateOfficeThread : public osl::Thread
{
    public:
        TerminateOfficeThread( CancelJobsThread& rCancelJobsThread,
                               css::uno::Reference< css::uno::XComponentContext > const & xContext )
            : osl::Thread(),
              maMutex(),
              mrCancelJobsThread( rCancelJobsThread ),
              mbStopOfficeTermination( false ),
              mxContext( xContext )
        {
        }

        virtual ~TerminateOfficeThread() {}

        void StopOfficeTermination();

    private:

        virtual void SAL_CALL run();
        virtual void SAL_CALL onTerminated();

        bool OfficeTerminationStopped();

        void PerformOfficeTermination();

        osl::Mutex maMutex;

        const CancelJobsThread& mrCancelJobsThread;

        bool mbStopOfficeTermination;

        css::uno::Reference< css::uno::XComponentContext > mxContext;
};

void TerminateOfficeThread::StopOfficeTermination()
{
    osl::MutexGuard aGuard(maMutex);

    mbStopOfficeTermination = true;
}

bool TerminateOfficeThread::OfficeTerminationStopped()
{
    osl::MutexGuard aGuard(maMutex);

    return mbStopOfficeTermination;
}

void SAL_CALL TerminateOfficeThread::run()
{
    while ( !OfficeTerminationStopped() )
    {
        osl::MutexGuard aGuard(maMutex);

        if ( mrCancelJobsThread.allJobsCancelled() )
        {
            break;
        }
    }

    if ( !OfficeTerminationStopped() )
    {
        PerformOfficeTermination();
    }
}

void TerminateOfficeThread::PerformOfficeTermination()
{
    css::uno::Reference< css::frame::XFramesSupplier > xTasksSupplier(
        mxContext->getServiceManager()->createInstanceWithContext(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop") ),
            mxContext ),
        css::uno::UNO_QUERY );
    if ( !xTasksSupplier.is() )
    {
        ASSERT( false, "<TerminateOfficeThread::PerformOfficeTermination()> - no XFramesSupplier!" );
        return;
    }

    css::uno::Reference< css::container::XElementAccess > xList( xTasksSupplier->getFrames(), css::uno::UNO_QUERY );
    if ( !xList.is() )
    {
        ASSERT( false, "<TerminateOfficeThread::PerformOfficeTermination()> - no XElementAccess!" );
        return;
    }

    if ( !xList->hasElements() )
    {
        css::uno::Reference< css::frame::XDesktop > xDesktop( xTasksSupplier, css::uno::UNO_QUERY );
        if ( xDesktop.is() && !OfficeTerminationStopped() )
        {
            xDesktop->terminate();
        }
    }
}

void SAL_CALL TerminateOfficeThread::onTerminated()
{
    if ( OfficeTerminationStopped() )
    {
        delete this;
    }
}


/** class FinalThreadManager

    @author OD
*/
FinalThreadManager::FinalThreadManager(css::uno::Reference< css::uno::XComponentContext > const & context)
    : m_xContext(context),
      maMutex(),
      maThreads(),
      mpCancelJobsThread( 0 ),
      mpTerminateOfficeThread( 0 ),
      mpPauseThreadStarting( 0 ),
      mbRegisteredAtDesktop( false )
{

}

void FinalThreadManager::registerAsListenerAtDesktop()
{
    css::uno::Reference< css::frame::XDesktop > xDesktop(
        m_xContext->getServiceManager()->createInstanceWithContext(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop") ),
            m_xContext ),
        css::uno::UNO_QUERY );

    if ( xDesktop.is() )
    {
        xDesktop->addTerminateListener( css::uno::Reference< css::frame::XTerminateListener >( static_cast< cppu::OWeakObject* >( this ), css::uno::UNO_QUERY ) );
    }
}

FinalThreadManager::~FinalThreadManager()
{
    if ( mpPauseThreadStarting != 0 )
    {
        delete mpPauseThreadStarting;
        mpPauseThreadStarting = 0;
    }

    if ( mpTerminateOfficeThread != 0 )
    {
        mpTerminateOfficeThread->StopOfficeTermination(); // thread kills itself.
        mpTerminateOfficeThread = 0;
    }

    if ( !maThreads.empty() )
    {
        ASSERT( false, "<FinalThreadManager::~FinalThreadManager()> - still registered jobs are existing -> perform cancellation" );
        cancelAllJobs();
    }

    if ( mpCancelJobsThread != 0 )
    {
        if ( !mpCancelJobsThread->allJobsCancelled() )
        {
            ASSERT( false, "<FinalThreadManager::~FinalThreadManager()> - cancellation of registered jobs not yet finished -> wait for its finish" );
        }

        mpCancelJobsThread->stopWhenAllJobsCancelled();
        mpCancelJobsThread->join();
        delete mpCancelJobsThread;
        mpCancelJobsThread = 0;
    }
}

// com.sun.star.uno.XServiceInfo:
::rtl::OUString SAL_CALL FinalThreadManager::getImplementationName() throw (css::uno::RuntimeException)
{
    return comp_FinalThreadManager::_getImplementationName();
}

::sal_Bool SAL_CALL FinalThreadManager::supportsService(::rtl::OUString const & serviceName) throw (css::uno::RuntimeException)
{
    css::uno::Sequence< ::rtl::OUString > serviceNames = comp_FinalThreadManager::_getSupportedServiceNames();
    for (::sal_Int32 i = 0; i < serviceNames.getLength(); ++i) {
        if (serviceNames[i] == serviceName)
            return sal_True;
    }
    return sal_False;
}

css::uno::Sequence< ::rtl::OUString > SAL_CALL FinalThreadManager::getSupportedServiceNames() throw (css::uno::RuntimeException)
{
    return comp_FinalThreadManager::_getSupportedServiceNames();
}

// ::com::sun::star::util::XJobManager:
void SAL_CALL FinalThreadManager::registerJob(const css::uno::Reference< css::util::XCancellable > & Job) throw (css::uno::RuntimeException)
{
    osl::MutexGuard aGuard(maMutex);

    maThreads.push_back( Job );

    if ( !mbRegisteredAtDesktop )
    {
        registerAsListenerAtDesktop();
        mbRegisteredAtDesktop = true;
    }
}

void SAL_CALL FinalThreadManager::releaseJob(const css::uno::Reference< css::util::XCancellable > & Job) throw (css::uno::RuntimeException)
{
    osl::MutexGuard aGuard(maMutex);

    maThreads.remove( Job );
}

void SAL_CALL FinalThreadManager::cancelAllJobs() throw (css::uno::RuntimeException)
{
    std::list< css::uno::Reference< css::util::XCancellable > > aThreads;
    {
        osl::MutexGuard aGuard(maMutex);

        aThreads.insert( aThreads.end(), maThreads.begin(), maThreads.end() );
        maThreads.clear();
    }

    if ( !aThreads.empty() )
    {
        osl::MutexGuard aGuard(maMutex);

        if ( mpCancelJobsThread == 0 )
        {
            mpCancelJobsThread = new CancelJobsThread( aThreads );;
            if ( !mpCancelJobsThread->create() )
            {
                // error handling
                // ASSERT( false, "<FinalThreadManager::cancelAllJobs()> - thread to cancel jobs can't be setup --> synchron cancellation of jobs" );
                delete mpCancelJobsThread;
                mpCancelJobsThread = 0;
                while ( !aThreads.empty() )
                {
                    aThreads.front()->cancel();
                    aThreads.pop_front();
                }
            }
        }
        else
        {
            mpCancelJobsThread->addJobs( aThreads );
        }
    }
}

// ::com::sun::star::frame::XTerminateListener
void SAL_CALL FinalThreadManager::queryTermination( const css::lang::EventObject& ) throw (css::frame::TerminationVetoException, css::uno::RuntimeException)
{
    osl::MutexGuard aGuard(maMutex);

    cancelAllJobs();
    // Sleep 1 second to give the thread for job cancellation some time.
    // Probably, all started threads have already finished its work.
    if ( mpCancelJobsThread != 0 &&
         !mpCancelJobsThread->allJobsCancelled() )
    {
        TimeValue aSleepTime;
        aSleepTime.Seconds = 1;
        aSleepTime.Nanosec = 0;
        osl_waitThread( &aSleepTime );
    }

    if ( mpCancelJobsThread != 0 &&
         !mpCancelJobsThread->allJobsCancelled() )
    {
        if ( mpTerminateOfficeThread != 0 )
        {
            if ( mpTerminateOfficeThread->isRunning() )
            {
                mpTerminateOfficeThread->StopOfficeTermination(); // thread kills itself.
            }
            else
            {
                delete mpTerminateOfficeThread;
            }
            mpTerminateOfficeThread = 0;
        }
        mpTerminateOfficeThread = new TerminateOfficeThread( *mpCancelJobsThread,
                                                 m_xContext );
        if ( !mpTerminateOfficeThread->create() )
        {
            // ASSERT( false, "FinalThreadManager::queryTermination(..) - thread to terminate office can't be started!" );
            delete mpTerminateOfficeThread;
            mpTerminateOfficeThread = 0;
        }

        throw css::frame::TerminationVetoException();
    }

    mpPauseThreadStarting = new SwPauseThreadStarting();

    return;
}

void SAL_CALL FinalThreadManager::cancelTermination( const css::lang::EventObject& ) throw (css::uno::RuntimeException)
{
    if ( mpPauseThreadStarting != 0 )
    {
        delete mpPauseThreadStarting;
        mpPauseThreadStarting = 0;
    }

    return;
}

void SAL_CALL FinalThreadManager::notifyTermination( const css::lang::EventObject& ) throw (css::uno::RuntimeException)
{
    if ( mpTerminateOfficeThread != 0 )
    {
        if ( mpTerminateOfficeThread->isRunning() )
        {
            // ASSERT( false, "<FinalThreadManager::notifyTermination()> - office termination thread still running!" );
            mpTerminateOfficeThread->StopOfficeTermination(); // thread kills itself.
        }
        else
        {
            delete mpTerminateOfficeThread;
        }
        mpTerminateOfficeThread = 0;
    }

    if ( !maThreads.empty() )
    {
        // ASSERT( false, "<FinalThreadManager::notifyTermination()> - still registered jobs are existing" );
        cancelAllJobs();
    }

    if ( mpCancelJobsThread != 0 )
    {
        if ( !mpCancelJobsThread->allJobsCancelled() )
        {
            // ASSERT( false, "<FinalThreadManager::notifyTermination()> - cancellation of registered jobs not yet finished -> wait for its finish" );
        }

        mpCancelJobsThread->stopWhenAllJobsCancelled();
        mpCancelJobsThread->join();
        delete mpCancelJobsThread;
        mpCancelJobsThread = 0;
    }

    // get reference of this
    css::uno::Reference< css::uno::XInterface > aOwnRef( static_cast< cppu::OWeakObject* >( this ));
    // notify <SwThreadJoiner> to release its reference
    SwThreadJoiner::ReleaseThreadJoiner();
}

// ::com::sun:star::lang::XEventListener (inherited via com::sun::star::frame::XTerminateListener)
void SAL_CALL FinalThreadManager::disposing( const css::lang::EventObject& ) throw (css::uno::RuntimeException)
{
    // nothing to do, because instance doesn't hold any references of observed objects
}

// component helper namespace
namespace comp_FinalThreadManager {

    ::rtl::OUString SAL_CALL _getImplementationName()
    {
        return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.util.comp.FinalThreadManager"));
    }

    css::uno::Sequence< ::rtl::OUString > SAL_CALL _getSupportedServiceNames()
    {
        css::uno::Sequence< ::rtl::OUString > s(1);
        s[0] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.util.JobManager"));
        return s;
    }

    css::uno::Reference< css::uno::XInterface > SAL_CALL _create(
        const css::uno::Reference< css::uno::XComponentContext > & context)
            SAL_THROW((css::uno::Exception))
    {
        return static_cast< ::cppu::OWeakObject * >(new FinalThreadManager(context));
    }

} // closing component helper namespace
