#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: sw.mk,v $
#
#   $Revision: 1.14 $
#
#   last change: $Author: hr $ $Date: 2006-08-14 15:31:42 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

# CDEFS=$(CDEFS) -DXXX

# Bereiche aktivieren

#CDEFS+=-DXML_CORE_API
CDEFS+=-DACCESSIBLE_LAYOUT

# define SW_DLLIMPLEMENTATION (see @ swdllapi.h)
CDEFS += -DSW_DLLIMPLEMENTATION

# set default symbol visibility / scope to hidden
.IF "$(COMNAME)" == "gcc3"
.IF "$(HAVE_GCC_VISIBILITY_FEATURE)" == "TRUE"
CFLAGS += -fvisibility=hidden
.ENDIF # "$(HAVE_GCC_VISIBILITY_FEATURE)" == "TRUE"
.ENDIF # gcc3

.IF "$(COMNAME)" == "sunpro5"
.IF "$(CCNUMVER)" >= "00050005"
CFLAGS += -xldscope=hidden
.ENDIF # 5.5
.ENDIF # sunpro5

# ------------------------------------------------------------------------
