/*************************************************************************
 *
 *  $RCSfile: maildispatcher.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: kz $ $Date: 2005-03-01 15:25:48 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef INCLUDED_MAILDISPATCHER_HXX
#include "maildispatcher.hxx"
#endif

#ifndef INCLUDED_IMAILDSPLISTENER_HXX
#include "imaildsplistener.hxx"
#endif

#include <algorithm>

using namespace ::com::sun::star;
using ::rtl::OUString;

typedef std::list< uno::Reference<mail::XMailMessage> > MailMessageContainer_t;
typedef std::list< ::rtl::Reference<IMailDispatcherListener> > MailDispatcherListenerContainer_t;

namespace /* private */
{
    /* Generic event notifier for started, 
       stopped, and idle events which are 
       very similary */
    class GenericEventNotifier
    {
    public:
        // pointer to virtual function typedef
        typedef void (IMailDispatcherListener::*GenericNotificationFunc_t)(::rtl::Reference<MailDispatcher>);
        
        GenericEventNotifier(
            GenericNotificationFunc_t notification_function, 
            ::rtl::Reference<MailDispatcher> mail_dispatcher) :
            notification_function_(notification_function),
            mail_dispatcher_(mail_dispatcher)
        {}
        
        void operator() (::rtl::Reference<IMailDispatcherListener> listener) const
        { (listener.get()->*notification_function_)(mail_dispatcher_); }
                        
    private:
        GenericNotificationFunc_t notification_function_;
        ::rtl::Reference<MailDispatcher> mail_dispatcher_;
    };
                                           
    class MailDeliveryNotifier
    {
    public:
        MailDeliveryNotifier(::rtl::Reference<MailDispatcher> xMailDispatcher, uno::Reference<mail::XMailMessage> message) :
            mail_dispatcher_(xMailDispatcher),
            message_(message)
        {}
        
        void operator() (::rtl::Reference<IMailDispatcherListener> listener) const
        { listener->mailDelivered(mail_dispatcher_, message_); }        
        
    private:
        ::rtl::Reference<MailDispatcher> mail_dispatcher_;
        uno::Reference<mail::XMailMessage> message_;
    };
    
    class MailDeliveryErrorNotifier
    {
    public:
        MailDeliveryErrorNotifier(
            ::rtl::Reference<MailDispatcher> xMailDispatcher,
            uno::Reference<mail::XMailMessage> message, 
            const ::rtl::OUString& error_message) : 
            mail_dispatcher_(xMailDispatcher),
            message_(message),
            error_message_(error_message)
        {}
        
        void operator() (::rtl::Reference<IMailDispatcherListener> listener) const
        { listener->mailDeliveryError(mail_dispatcher_, message_, error_message_); }
                
    private:
        ::rtl::Reference<MailDispatcher> mail_dispatcher_;
        uno::Reference<mail::XMailMessage> message_;
        ::rtl::OUString error_message_;
    };
    
} // namespace private

    
MailDispatcher::MailDispatcher(uno::Reference<mail::XSmtpService> mailserver) :
    mailserver_ (mailserver),
    run_(false),    
    shutdown_requested_(false),
    bIsInRun(false)
{            
    wakening_call_.reset();
    mail_dispatcher_active_.reset();
    
    if (!create())        
        throw uno::RuntimeException();   
    
    // wait until the mail dispatcher thread is really alive    
    // and has aquired a reference to this instance of the 
    // class    
    mail_dispatcher_active_.wait();                                             
}

MailDispatcher::~MailDispatcher()
{    
}

void MailDispatcher::enqueueMailMessage(uno::Reference<mail::XMailMessage> message)
{   
    ::osl::MutexGuard thread_status_guard(thread_status_mutex_);     
    ::osl::MutexGuard message_container_guard(message_container_mutex_);
    
    OSL_PRECOND(!shutdown_requested_, "MailDispatcher thread is shuting down already");
        
    messages_.push_back(message);                    
    if (run_)
        wakening_call_.set();             
}

uno::Reference<mail::XMailMessage> MailDispatcher::dequeueMailMessage()
{        
    ::osl::MutexGuard guard(message_container_mutex_);
    uno::Reference<mail::XMailMessage> message;
    if(!messages_.empty())
    {        
        message = messages_.front();
        messages_.pop_front();
    }
    return message;
}

void MailDispatcher::start()
{            
    OSL_PRECOND(!isStarted(), "MailDispatcher is already started!");    
    
    ::osl::ClearableMutexGuard thread_status_guard(thread_status_mutex_);        
        
    OSL_PRECOND(!shutdown_requested_, "MailDispatcher thread is shuting down already");
    
    if (!shutdown_requested_)
    {
        run_ = true;
        wakening_call_.set();
        thread_status_guard.clear();
        
        MailDispatcherListenerContainer_t listeners_cloned(cloneListener());    
        std::for_each(listeners_cloned.begin(), listeners_cloned.end(), GenericEventNotifier(&IMailDispatcherListener::started, this));        
    }        
}

void MailDispatcher::stop()
{
    OSL_PRECOND(isStarted(), "MailDispatcher not started!");    
    
    ::osl::ClearableMutexGuard thread_status_guard(thread_status_mutex_);                
        
    OSL_PRECOND(!shutdown_requested_, "MailDispatcher thread is shuting down already");
    
    if (!shutdown_requested_)
    {
        run_ = false;
        wakening_call_.reset();
        thread_status_guard.clear();
        
        MailDispatcherListenerContainer_t listeners_cloned(cloneListener());    
        std::for_each(listeners_cloned.begin(), listeners_cloned.end(), GenericEventNotifier(&IMailDispatcherListener::stopped, this));   
    }
}

void MailDispatcher::shutdown()
{        
    ::osl::MutexGuard thread_status_guard(thread_status_mutex_);            
    
    OSL_PRECOND(!shutdown_requested_, "MailDispatcher thread is shuting down already");
    
    shutdown_requested_ = true;
    wakening_call_.set();        
}

bool MailDispatcher::isStarted() const
{
    return run_;
}

bool MailDispatcher::isRunning() const
{
    return bIsInRun;
} 


void MailDispatcher::addListener(::rtl::Reference<IMailDispatcherListener> listener)
{
    OSL_PRECOND(!shutdown_requested_, "MailDispatcher thread is shuting down already");
    
    ::osl::MutexGuard guard(listener_container_mutex_);
    listeners_.push_back(listener);
}

void MailDispatcher::removeListener(::rtl::Reference<IMailDispatcherListener> listener)
{
    OSL_PRECOND(!shutdown_requested_, "MailDispatcher thread is shuting down already");
    
    ::osl::MutexGuard guard(listener_container_mutex_);
    listeners_.remove(listener);
}

std::list< ::rtl::Reference<IMailDispatcherListener> > MailDispatcher::cloneListener()
{
    ::osl::MutexGuard guard(listener_container_mutex_);
    return listeners_;
}

void MailDispatcher::sendMailMessageNotifyListener(uno::Reference<mail::XMailMessage> message)
{
    try
    {
        mailserver_->sendMailMessage(message);                        
        MailDispatcherListenerContainer_t listeners_cloned(cloneListener());            
        std::for_each(listeners_cloned.begin(), listeners_cloned.end(), MailDeliveryNotifier(this, message));                   
    }
    catch (mail::MailException& ex)
    {            
        MailDispatcherListenerContainer_t listeners_cloned(cloneListener());            
        std::for_each(listeners_cloned.begin(), listeners_cloned.end(), MailDeliveryErrorNotifier(this, message, ex.Message));                   
    }
    catch (uno::RuntimeException& ex)
    {   
        MailDispatcherListenerContainer_t listeners_cloned(cloneListener());            
        std::for_each(listeners_cloned.begin(), listeners_cloned.end(), MailDeliveryErrorNotifier(this, message, ex.Message));                   
    }
}

void MailDispatcher::run()
{    
    // aquire a self reference in order to avoid race
    // conditions. The last client of this class must
    // call shutdown before releasing his last reference 
    // to this class in order to shutdown this thread
    // which will release his (the very last reference
    // to the class and so force their destruction        
    m_xSelfReference = this; 
    
    // signal that the mail dispatcher thread is now alive
    mail_dispatcher_active_.set();
    
    bIsInRun = true;
    for(;;)
    {       
        wakening_call_.wait();
                                                                                                                            
        ::osl::ClearableMutexGuard thread_status_guard(thread_status_mutex_);        
        if (shutdown_requested_)
           break;
    
        ::osl::ClearableMutexGuard message_container_guard(message_container_mutex_);
                   
        if (messages_.size())
        {   
            thread_status_guard.clear();         
            uno::Reference<mail::XMailMessage> message = messages_.front();
            messages_.pop_front();
            message_container_guard.clear();
            sendMailMessageNotifyListener(message);
        }
        else // idle - put ourself to sleep 
        {
            wakening_call_.reset();
            message_container_guard.clear();
            thread_status_guard.clear();
            MailDispatcherListenerContainer_t listeners_cloned(cloneListener());    
            std::for_each(listeners_cloned.begin(), listeners_cloned.end(), GenericEventNotifier(&IMailDispatcherListener::idle, this));               
        }                                                    
    } // end for        SSH ALI
    bIsInRun = false;
}
/*-- 27.08.2004 12:04:46---------------------------------------------------

  -----------------------------------------------------------------------*/
void MailDispatcher::onTerminated() 
{
    //keep the reference until the end of onTerminated() because of the call order in the 
    //_threadFunc() from osl/thread.hxx
    m_xSelfReference = 0;
}
