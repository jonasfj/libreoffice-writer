/*************************************************************************
 *
 *  $RCSfile: wdocsh.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: os $ $Date: 2002-12-05 12:59:35 $
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
#include "ui_pch.hxx"
#endif

#pragma hdrstop


#ifndef _SVX_SRCHITEM_HXX
#include <svx/srchitem.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC //autogen
#include <svx/svxids.hrc>
#endif

#ifndef _SO_CLSIDS_HXX
#include <so3/clsids.hxx>
#endif

#include "itemdef.hxx"

#include "cmdid.h"
#include "swtypes.hxx"

#include "shellio.hxx"
// nur wegen des Itemtypes
#include "wdocsh.hxx"
#include "web.hrc"

#define SwWebDocShell
#include "swslots.hxx"

#define C2S(cChar) UniString::CreateFromAscii(cChar)

SFX_IMPL_INTERFACE( SwWebDocShell, SfxObjectShell, SW_RES(0) )
{
    SwWebDocShell::Factory().RegisterHelpFile(C2S("swriter.svh"));
    SwWebDocShell::Factory().RegisterHelpPIFile(C2S("swriter.svh"));
}

/*--------------------------------------------------------------------
    Beschreibung:	Alle Filter registrieren
 --------------------------------------------------------------------*/

// 4.0:	0xf0caa840, 0x7821, 0x11d0, 0xa4, 0xa7, 0x0, 0xa0, 0x24, 0x9d, 0x57, 0xb1, Sw)
// 5.0: 0xc20cf9d2, 0x85ae, 0x11d1, 0xaa, 0xb4, 0x0, 0x60, 0x97, 0xda, 0x56, 0x1a

SFX_IMPL_OBJECTFACTORY_LOD(SwWebDocShell, SFXOBJECTSHELL_STD_NORMAL|SFXOBJECTSHELL_HASMENU, /*swriter4/web,*/ \
            SvGlobalName(SO3_SWWEB_CLASSID), Sw)
/*{
    ::RegisterWebFilterInSfxFactory( (SfxObjectFactory&)Factory(), RC_WEB_ICON );
} */


TYPEINIT1(SwWebDocShell, SwDocShell);

/*-----------------22.01.97 09.29-------------------

--------------------------------------------------*/

SwWebDocShell::SwWebDocShell(SfxObjectCreateMode eMode ) :
        SwDocShell(eMode),
        nSourcePara(0)
{
}

/*-----------------22.01.97 09.29-------------------

--------------------------------------------------*/

SwWebDocShell::~SwWebDocShell()
{
}



void SwWebDocShell::FillClass( SvGlobalName * pClassName,
                                   ULONG * pClipFormat,
                                   String * pAppName,
                                   String * pLongUserName,
                                   String * pUserName,
                                   long nVersion ) const
{
    SfxInPlaceObject::FillClass(pClassName, pClipFormat, pAppName, pLongUserName,
                                pUserName, nVersion);

    if (nVersion == SOFFICE_FILEFORMAT_40)
    {
        *pClassName = SvGlobalName( SO3_SWWEB_CLASSID_40 );
        *pClipFormat = SOT_FORMATSTR_ID_STARWRITERWEB_40;
        *pAppName = C2S("StarWriter/Web 4.0");

        *pLongUserName = SW_RESSTR(STR_WRITER_WEBDOC_FULLTYPE_40);
    }
    else if ( nVersion == SOFFICE_FILEFORMAT_50)
    {
        *pClassName = SvGlobalName( SO3_SWWEB_CLASSID_50 );
        *pClipFormat = SOT_FORMATSTR_ID_STARWRITERWEB_50;
        *pLongUserName = SW_RESSTR(STR_WRITER_WEBDOC_FULLTYPE_50);
    }
    else if (nVersion == SOFFICE_FILEFORMAT_60)
    {
        *pLongUserName = SW_RESSTR(STR_WRITER_WEBDOC_FULLTYPE);
    }
    *pUserName = SW_RESSTR(STR_HUMAN_SWWEBDOC_NAME);
}



