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
# $Revision: 1.13 $
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
TARGET=edit

AUTOSEG=true

# --- Settings -----------------------------------------------------

.INCLUDE :	$(PRJ)$/inc$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

EXCEPTIONSFILES=\
        $(SLO)$/eddel.obj \
        $(SLO)$/edlingu.obj \
        $(SLO)$/edfldexp.obj \
        $(SLO)$/edtab.obj

SLOFILES =	\
        $(EXCEPTIONSFILES)\
        $(SLO)$/acorrect.obj \
        $(SLO)$/autofmt.obj \
        $(SLO)$/edatmisc.obj \
        $(SLO)$/edattr.obj \
        $(SLO)$/edfcol.obj \
        $(SLO)$/edfld.obj \
        $(SLO)$/edfmt.obj \
        $(SLO)$/edglbldc.obj \
        $(SLO)$/edglss.obj \
        $(SLO)$/editsh.obj \
        $(SLO)$/ednumber.obj \
        $(SLO)$/edredln.obj \
        $(SLO)$/edtox.obj \
        $(SLO)$/edundo.obj \
        $(SLO)$/edws.obj \
        $(SLO)$/edsect.obj

# --- Targets -------------------------------------------------------

.INCLUDE :	target.mk

