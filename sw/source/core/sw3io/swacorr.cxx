/*************************************************************************
 *
 *  $RCSfile: swacorr.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: kz $ $Date: 2004-10-04 19:11:24 $
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

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _SWACORR_HXX
#include <swacorr.hxx>
#endif
#ifndef _SWBLOCKS_HXX
#include <swblocks.hxx>
#endif
#ifndef _SW_XMLTEXTBLOCKS_HXX
#include <SwXMLTextBlocks.hxx>
#endif
#ifndef _SWSERROR_H
#include <swerror.h>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#include <sot/storage.hxx>

TYPEINIT1( SwAutoCorrect, SvxAutoCorrect );


    //	- return den Ersetzungstext (nur fuer SWG-Format, alle anderen
    //		koennen aus der Wortliste herausgeholt werden!)
    //		rShort ist der Stream-Name - gecryptet!

BOOL SwAutoCorrect::GetLongText( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >& rStg, const String& rShort, String& rLong )
{
    ULONG nRet;
    if (rStg.is())
    {
        //TODO/MBA: can't pass filename of storage; the only place where it seems to be used is in SwImpBlocks::IsFileChanged()
        SwXMLTextBlocks aBlk( rStg, String() );
        nRet = aBlk.GetText( rShort, rLong );
    }
    else
        ASSERT ( rStg.is(), "Someone passed SwAutoCorrect::GetLongText a dud storage!");
    return !IsError( nRet ) && rLong.Len();
}

    //	- Text mit Attributierung (kann nur der SWG - SWG-Format!)
    //		rShort ist der Stream-Name - gecryptet!
BOOL SwAutoCorrect::PutText( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >&  rStg, const String& rShort,
                            SfxObjectShell& rObjSh, String& rLong )
{
    if( !rObjSh.IsA( TYPE(SwDocShell) ) )
        return FALSE;

    SwDocShell& rDShell = (SwDocShell&)rObjSh;
    // Bis es eine Option dafuer gibt, base URL loeschen
    const String aOldURL( INetURLObject::GetBaseURL() );
    INetURLObject::SetBaseURL( aEmptyStr );
    ULONG nRet = 0;

    //TODO/MBA: can't pass filename of storage; the only place where it seems to be used is in SwImpBlocks::IsFileChanged()
    SwXMLTextBlocks aBlk( rStg, String() );
    SwDoc* pDoc = aBlk.GetDoc();

    nRet = aBlk.BeginPutDoc( rShort, rShort );
    if( !IsError( nRet ) )
    {
        ((SwEditShell*)rDShell.GetWrtShell())->_CopySelToDoc( pDoc );
        nRet = aBlk.PutDoc();
        aBlk.AddName ( rShort, rShort, FALSE );
        if( !IsError( nRet ) )
            nRet = aBlk.GetText( rShort, rLong );
    }

    INetURLObject::SetBaseURL( aOldURL );
    return !IsError( nRet );
}


SwAutoCorrect::SwAutoCorrect( const SvxAutoCorrect& rACorr )
    : SvxAutoCorrect( rACorr )
{
    SwEditShell::SetAutoFmtFlags(&GetSwFlags());
}

SwAutoCorrect::~SwAutoCorrect()
{
}
