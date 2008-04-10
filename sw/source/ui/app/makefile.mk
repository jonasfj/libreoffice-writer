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
# $Revision: 1.21 $
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

PRJ=..$/..$/..

PRJNAME=sw
TARGET=app

LIBTARGET=NO

# future: DEMO\...

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =\
        app.src     \
        mn.src      \
        error.src


SLOFILES = \
        $(SLO)$/appenv.obj   \
        $(SLO)$/apphdl.obj   \
        $(SLO)$/applab.obj   \
        $(SLO)$/appopt.obj   \
        $(SLO)$/docsh.obj    \
        $(SLO)$/docsh2.obj   \
        $(SLO)$/docshdrw.obj \
        $(SLO)$/docshini.obj \
        $(SLO)$/docst.obj    \
        $(SLO)$/docstyle.obj \
        $(SLO)$/mainwn.obj   \
        $(SLO)$/swmodule.obj \
        $(SLO)$/swmodul1.obj \
        $(SLO)$/swdll.obj	 \
        $(SLO)$/swwait.obj

EXCEPTIONSFILES= \
        $(SLO)$/docsh.obj    \
        $(SLO)$/docst.obj    \
        $(SLO)$/swmodule.obj \
        $(SLO)$/swmodul1.obj \
            $(SLO)$/apphdl.obj   \
        $(SLO)$/docsh2.obj

LIB1TARGET= $(SLB)$/app.lib

LIB1OBJFILES= \
        $(SLO)$/appenv.obj   \
        $(SLO)$/apphdl.obj   \
        $(SLO)$/applab.obj   \
        $(SLO)$/appopt.obj   \
        $(SLO)$/docsh.obj    \
        $(SLO)$/docsh2.obj   \
        $(SLO)$/docshdrw.obj \
        $(SLO)$/docshini.obj \
        $(SLO)$/docst.obj    \
        $(SLO)$/docstyle.obj \
        $(SLO)$/mainwn.obj   \
        $(SLO)$/swmodul1.obj \
        $(SLO)$/swwait.obj


# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

$(SRS)$/app.srs: $(SOLARINCDIR)$/svx$/globlmn.hrc

