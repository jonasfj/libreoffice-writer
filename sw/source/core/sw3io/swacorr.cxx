/*************************************************************************
 *
 *  $RCSfile: swacorr.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-19 00:08:23 $
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

#ifdef PRECOMPILED
#include "core_pch.hxx"
#endif

#pragma hdrstop

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif

#include "swacorr.hxx"
#include "swblocks.hxx"
#include "swerror.h"
#include "docsh.hxx"
#include "editsh.hxx"



TYPEINIT1( SwAutoCorrect, SvxAutoCorrect );


    //	- return den Ersetzungstext (nur fuer SWG-Format, alle anderen
    //		koennen aus der Wortliste herausgeholt werden!)
    //		rShort ist der Stream-Name - gecryptet!
BOOL SwAutoCorrect::GetLongText( SvStorage& rStg,
                            const String& rShort, String& rLong )
{
    Sw3TextBlocks aBlk( rStg );
    ULONG nRet = aBlk.GetText( rShort, rLong );
    return !IsError( nRet ) && rLong.Len();
}


    //	- Text mit Attributierung (kann nur der SWG - SWG-Format!)
    //		rShort ist der Stream-Name - gecryptet!
BOOL SwAutoCorrect::PutText( SvStorage& rStg, const String& rShort,
                            SfxObjectShell& rObjSh, String& rLong )
{
    if( !rObjSh.IsA( TYPE(SwDocShell) ) )
        return FALSE;

    SwDocShell& rDShell = (SwDocShell&)rObjSh;
    Sw3TextBlocks aBlk( rStg );

    SwDoc* pDoc = aBlk.GetDoc();

    // Bis es eine Option dafuer gibt, base URL loeschen
    const String aOldURL( INetURLObject::GetBaseURL() );
    INetURLObject::SetBaseURL( aEmptyStr );

    ULONG nRet = aBlk.BeginPutDoc( rShort, rShort );
    if( !IsError( nRet ) )
    {
        ((SwEditShell*)rDShell.GetWrtShell())->_CopySelToDoc( pDoc );
        nRet = aBlk.PutDoc();
        if( !IsError( nRet ) )
            nRet = aBlk.GetText( rShort, rLong );
    }

    INetURLObject::SetBaseURL( aOldURL );

    return !IsError( nRet );
}


SwAutoCorrect::SwAutoCorrect( const String& rAutocorrFile )
    : SvxAutoCorrect( rAutocorrFile )
{
    SwEditShell::SetAutoFmtFlags(&GetSwFlags());
    // Konvertierung ??
}

SwAutoCorrect::SwAutoCorrect( const SvxAutoCorrect& rACorr )
    : SvxAutoCorrect( rACorr )
{
    SwEditShell::SetAutoFmtFlags(&GetSwFlags());
}

SwAutoCorrect::~SwAutoCorrect()
{
}



