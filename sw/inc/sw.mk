#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: sw.mk,v $
#
# $Revision: 1.15 $
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
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
