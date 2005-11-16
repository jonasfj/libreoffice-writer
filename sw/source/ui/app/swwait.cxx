/*************************************************************************
 *
 *  $RCSfile: swwait.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2005-11-16 09:48:01 $
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


#pragma hdrstop

#ifndef _SFXVIEWFRM_HXX //autogen wg. SfxViewFrame
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen wg. SfxDispatcher
#include <sfx2/dispatch.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _SWDOCSH_HXX //autogen wg. SwDocShell
#include <docsh.hxx>
#endif
#ifndef _SWWAIT_HXX
#include <swwait.hxx>
#endif


void SwDocShell::EnterWait( BOOL bLockDispatcher )
{
    SfxViewFrame *pFrame = SfxViewFrame::GetFirst( this, 0, FALSE );
    while ( pFrame )
    {
        pFrame->GetWindow().EnterWait();
        if ( bLockDispatcher )
            pFrame->GetDispatcher()->Lock( TRUE );
        pFrame = SfxViewFrame::GetNext( *pFrame, this, 0, FALSE );
    }
}

void SwDocShell::LeaveWait( BOOL bLockDispatcher )
{
    SfxViewFrame *pFrame = SfxViewFrame::GetFirst( this, 0, FALSE );
    while ( pFrame )
    {
        pFrame->GetWindow().LeaveWait();
        if ( bLockDispatcher )
            pFrame->GetDispatcher()->Lock( FALSE );
        pFrame = SfxViewFrame::GetNext( *pFrame, this, 0, FALSE );
    }
}

SwWait::SwWait( SwDocShell &rDocShell, BOOL bLockDispatcher ) :
    rDoc ( rDocShell ),
    bLock( bLockDispatcher )
{
    rDoc.EnterWait( bLock );
}

SwWait::~SwWait()
{
    rDoc.LeaveWait( bLock );
}


