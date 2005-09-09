/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: shellid.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:08:53 $
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
#ifndef _SHELLID_HXX
#define _SHELLID_HXX

#ifndef _SFX_SHELL_HXX //autogen
#include <sfx2/shell.hxx>
#endif

/*--------------------------------------------------------------------
    Beschreibung:	Mapping fuer UI-Parameter
 --------------------------------------------------------------------*/

#define SW_BASESHELL			(SFX_INTERFACE_SW_START + 1)
#define SW_TEXTSHELL			(SFX_INTERFACE_SW_START + 2)
#define SW_LISTSHELL			(SFX_INTERFACE_SW_START + 3)
#define SW_TABSHELL 			(SFX_INTERFACE_SW_START + 4)
#define SW_FRAMESHELL			(SFX_INTERFACE_SW_START + 5)
#define SW_GRFSHELL 			(SFX_INTERFACE_SW_START + 6)
#define SW_OLESHELL 			(SFX_INTERFACE_SW_START + 7)
#define SW_DRAWSHELL			(SFX_INTERFACE_SW_START + 8)
#define SW_DRWTXTSHELL			(SFX_INTERFACE_SW_START + 9)

#define SW_INTERFACE_MODULE 	(SFX_INTERFACE_SW_START + 10)
#define SW_DOCSHELL 			(SFX_INTERFACE_SW_START + 11)
#define SW_VIEWSHELL			(SFX_INTERFACE_SW_START + 12)
#define SW_PAGEPREVIEW			(SFX_INTERFACE_SW_START + 13)
#define SW_GLOSDOCSHELL 		(SFX_INTERFACE_SW_START + 14)
#define SW_SRC_VIEWSHELL 		(SFX_INTERFACE_SW_START + 15)

#define SW_BEZIERSHELL 			(SFX_INTERFACE_SW_START + 16)
#define SW_DRAWBASESHELL		(SFX_INTERFACE_SW_START + 17)
#define SW_DRAWFORMSHELL		(SFX_INTERFACE_SW_START + 18)

#define SW_WEBDOCSHELL			(SFX_INTERFACE_SW_START + 19)
#define SW_WEBVIEWSHELL			(SFX_INTERFACE_SW_START + 20)
#define SW_WEBBASESHELL			(SFX_INTERFACE_SW_START + 21)
#define SW_WEBTEXTSHELL			(SFX_INTERFACE_SW_START + 22)
#define SW_WEBFRAMESHELL		(SFX_INTERFACE_SW_START + 23)
#define SW_WEBGLOSDOCSHELL 		(SFX_INTERFACE_SW_START + 24)
#define SW_WEBLISTSHELL			(SFX_INTERFACE_SW_START + 25)
#define SW_WEBTABSHELL 			(SFX_INTERFACE_SW_START + 26)
#define SW_WEBGRFSHELL 			(SFX_INTERFACE_SW_START + 27)
#define SW_WEBOLESHELL 			(SFX_INTERFACE_SW_START + 28)
#define SW_WEBDRAWBASESHELL		(SFX_INTERFACE_SW_START + 29)
#define SW_WEBDRAWFORMSHELL		(SFX_INTERFACE_SW_START + 30)

#define SW_MEDIASHELL 			(SFX_INTERFACE_SW_START + 31)

#endif // _UIPARAM_HXX
