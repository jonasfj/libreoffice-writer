#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: makefile.mk,v $
#
# $Revision: 1.3 $
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
PRJ=..$/..

PRJNAME=sw
TARGET=test-bigptrarray
# this is removed at the moment because we need some enhancements
# TESTDIR=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# BEGIN ----------------------------------------------------------------
# auto generated Target:testjob by codegen.pl 

.IF "$(GUI)" == "WNT"
    CFLAGS+=/Ob1
.ENDIF

SHL1OBJS=$(SLO)$/Test-BigPtrArray.obj \
    $(SLO)$/bparr.obj

SHL1TARGET=$(TARGET)
SHL1STDLIBS=$(SALLIB) $(TOOLSLIB) 
   
.IF "$(GUI)" == "WNT"
SHL1STDLIBS+=	$(SOLARLIBDIR)$/cppunit.lib
.ENDIF
.IF "$(GUI)" == "UNX"
SHL1STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
# .IF "$(OS)" == "SOLARIS"
# SHL1STDLIBS += -lrt -laio
# .ENDIF
.ENDIF

SHL1IMPLIB= i$(SHL1TARGET)

# SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)

# DEF1EXPORTFILE= export.exp
SHL1VERSIONMAP = export.map

# END ------------------------------------------------------------------

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : _cppunit.mk
