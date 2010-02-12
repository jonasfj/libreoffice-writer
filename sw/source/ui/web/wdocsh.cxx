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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"




#include <svx/srchitem.hxx>
#include <sfx2/app.hxx>
#ifndef _SVX_SVXIDS_HRC //autogen
#include <svx/svxids.hrc>
#endif

#include <sot/clsids.hxx>
#include <sfx2/objface.hxx>

#include <sfx2/msg.hxx>
#include "cfgid.h"
#include "cmdid.h"
#include "swtypes.hxx"

#include "shellio.hxx"
// nur wegen des Itemtypes
#include "wdocsh.hxx"
#include "web.hrc"

#define SwWebDocShell
#include "swslots.hxx"

#include <unomid.h>


SFX_IMPL_INTERFACE( SwWebDocShell, SfxObjectShell, SW_RES(0) )
{
}

TYPEINIT1(SwWebDocShell, SwDocShell);

SFX_IMPL_OBJECTFACTORY(SwWebDocShell, SvGlobalName(SO3_SWWEB_CLASSID), SFXOBJECTSHELL_STD_NORMAL|SFXOBJECTSHELL_HASMENU, "swriter/web" )

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
                                   sal_uInt32 * pClipFormat,
                                   String * /*pAppName*/,
                                   String * pLongUserName,
                                   String * pUserName,
                                   sal_Int32 nVersion,
                                   sal_Bool bTemplate /* = sal_False */) const
{
    (void)bTemplate;
    DBG_ASSERT( bTemplate == sal_False, "No template for Writer Web" );

    if (nVersion == SOFFICE_FILEFORMAT_60)
    {
        *pClassName = SvGlobalName( SO3_SWWEB_CLASSID_60 );
        *pClipFormat = SOT_FORMATSTR_ID_STARWRITERWEB_60;
        *pLongUserName = SW_RESSTR(STR_WRITER_WEBDOC_FULLTYPE);
    }
    else if (nVersion == SOFFICE_FILEFORMAT_8)
    {
        *pClassName		= SvGlobalName( SO3_SWWEB_CLASSID_60 );
        *pClipFormat	= SOT_FORMATSTR_ID_STARWRITERWEB_8;
        *pLongUserName = SW_RESSTR(STR_WRITER_WEBDOC_FULLTYPE);
    }
    *pUserName = SW_RESSTR(STR_HUMAN_SWWEBDOC_NAME);
}



