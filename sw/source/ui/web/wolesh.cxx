/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: wolesh.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 23:38:10 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"



#include "itemdef.hxx"

#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SFXOBJFACE_HXX //autogen
#include <sfx2/objface.hxx>
#endif
#ifndef _SVX_SRCHITEM_HXX //autogen
#include <svx/srchitem.hxx>
#endif

#include "wrtsh.hxx"
#include "view.hxx"
#include "helpid.h"
#include "globals.hrc"
#include "web.hrc"
#include "popup.hrc"
#include "shells.hrc"
#include "wolesh.hxx"
#include "cmdid.h"

#define SwWebOleShell
#include "swslots.hxx"

SFX_IMPL_INTERFACE(SwWebOleShell, SwOleShell, SW_RES(STR_SHELLNAME_OBJECT))
{
    SFX_POPUPMENU_REGISTRATION(SW_RES(MN_OLE_POPUPMENU));
    SFX_OBJECTBAR_REGISTRATION(SFX_OBJECTBAR_OBJECT, SW_RES(RID_WEBOLE_TOOLBOX));
}

SwWebOleShell::SwWebOleShell(SwView &rView) :
    SwOleShell(rView)

{
    SetName(String::CreateFromAscii("Object"));
    SetHelpId(SW_OLESHELL);
}

__EXPORT SwWebOleShell::~SwWebOleShell()
{
}


