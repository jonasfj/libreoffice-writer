/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: maildispatcher.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:39:38 $
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

#ifndef INCLUDED_MAILDISPATCHER_HXX
#define INCLUDED_MAILDISPATCHER_HXX

//#ifndef _COM_SUN_STAR_MAIL_XMAILSERVER_HPP_
//#include "com/sun/star/mail/XMailServer.hpp"
//#endif
#ifndef _COM_SUN_STAR_MAIL_XSMTPSERVICE_HPP_
#include "com/sun/star/mail/XSmtpService.hpp"
#endif

#ifndef _COM_SUN_STAR_MAIL_XMAILMESSAGE_HPP_
#include "com/sun/star/mail/XMailMessage.hpp"
#endif

#ifndef _THREAD_HXX_
#include <osl/thread.hxx>
#endif

#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif

#ifndef _SALHELPER_REFOBJ_HXX_
#include <salhelper/refobj.hxx>
#endif

#include <list>

class IMailDispatcherListener;

/**
    A MailDispatcher should be used for sending a bunch a mail messages
    asynchronously. Usually a client enqueues a number of mail messages
    and then calls start to begin sending them. An instance of this class
    must not be shared among different client threads. Instead each client 
    thread should create an own instance of this class.
*/ 
class MailDispatcher : public ::salhelper::ReferenceObject, private ::osl::Thread
{
public:
    // bringing operator new/delete into scope    
    using osl::Thread::operator new;
    using osl::Thread::operator delete;
    using osl::Thread::join;
            
public:

    /**                 
        @param xSmtpService
        [in] a reference to a mail server. A user must be
        connected to the mail server otherwise errors occur
        during the delivery of mail messages.
        
        @throws ::com::sun::star::uno::RuntimeException
        on errors during construction of an instance of this class.
    */
    MailDispatcher(::com::sun::star::uno::Reference< ::com::sun::star::mail::XSmtpService> xMailService);

    /**
        Shutdown the mail dispatcher. Every mail messages
        not yet sent will be discarded.
    */        
    virtual ~MailDispatcher();
    
    /**
        Enqueue a mail message for delivery. A client must
        start the mail dispatcher in order to send the 
        enqueued mail messages.
        
        @param xMailMessage
        [in] a mail message that should be send.
    */
    void enqueueMailMessage(::com::sun::star::uno::Reference< ::com::sun::star::mail::XMailMessage> xMailMessage);
    /** 
        Dequeues a mail message. 
        This enables the caller to remove attachments when sending mails is to be cancelled.
    */
    ::com::sun::star::uno::Reference< ::com::sun::star::mail::XMailMessage> MailDispatcher::dequeueMailMessage();
    
    /**
        Start sending mail messages asynchronously. A client may register
        a listener for mail dispatcher events. For every mail message sent 
        the notification will be sent. While handling such notification a 
        client may enqueue new mail messages. If there are no more mail
        messages to send an respective notification is sent and the mail
        dispatcher waits for more mail messages.
        
        @precond not isStarted()
    */
    void start();
    
    /**
        Stop sending mail messages.
        
        @precond isStarted()
    */
    void stop();

    /**
        Request shutdown of the mail dispatcher thread.
        NOTE: You must call this method before you release
        your last reference to this class otherwise the
        mail dispatcher thread will never end.
    */
    void shutdown();
                
    /**
        Check whether the mail dispatcher is started or not.
        
        @return 
        <TRUE/> if the sending thread is running.
    */
    bool isStarted() const;
            
    /** returns if the thread is still running
    */
    bool isRunning() const;
    /** returns if shutdown has already been called
    */
    bool isShutdownRequested() const  
        { return shutdown_requested_; }  
    /**
        Register a listener for mail dispatcher events.
    */
    void addListener(::rtl::Reference<IMailDispatcherListener> listener);
    
    /**
        Unregister a listener for mail dispatcher events
    */
    void removeListener(::rtl::Reference<IMailDispatcherListener> listener);
        
protected:
    virtual void SAL_CALL run();
    virtual void SAL_CALL onTerminated();

private:
    std::list< ::rtl::Reference<IMailDispatcherListener> > cloneListener();
    void sendMailMessageNotifyListener(::com::sun::star::uno::Reference< ::com::sun::star::mail::XMailMessage> message);           
    
private:    
    ::com::sun::star::uno::Reference< ::com::sun::star::mail::XSmtpService> mailserver_;    
    ::std::list< ::com::sun::star::uno::Reference< ::com::sun::star::mail::XMailMessage > > messages_;
    ::std::list< ::rtl::Reference<IMailDispatcherListener> > listeners_;    
    ::osl::Mutex message_container_mutex_;
    ::osl::Mutex listener_container_mutex_;    
    ::osl::Mutex thread_status_mutex_;
    ::osl::Condition mail_dispatcher_active_;
    ::osl::Condition wakening_call_;    
    ::rtl::Reference<MailDispatcher> m_xSelfReference;
    bool run_;    
    bool shutdown_requested_;
    bool bIsInRun;    
};

#endif // INCLUDED_MAILDISPATCHER_HXX
