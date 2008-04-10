/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: txtcfg.hxx,v $
 * $Revision: 1.5 $
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

#ifndef _TXTCFG_HXX
#define _TXTCFG_HXX

#if OSL_DEBUG_LEVEL > 1
#include "dbgloop.hxx"  // DBG_LOOP
#else
#ifdef DBG_LOOP		//kann per precompiled hereinkommen
#undef DBG_LOOP
#undef DBG_LOOP_RESET
#endif
#define DBG_LOOP
#define DBG_LOOP_RESET
#endif

// Toleranzwert in der Formatierung und Textausgabe.
#define SLOPPY_TWIPS    5

#define CONSTCHAR( name, string ) static const sal_Char __FAR_DATA name[] = string

// Allgemeines ...

#ifndef CONST
#define CONST const
#endif


#endif
