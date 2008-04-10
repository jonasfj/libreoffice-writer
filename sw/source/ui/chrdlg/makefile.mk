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
# $Revision: 1.6 $
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
TARGET=chrdlg
LIBTARGET=NO
# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =	\
        swbreak.src \
        chardlg.src \
        chrdlg.src \
        drpcps.src \
        ccoll.src \
        paradlg.src \
        numpara.src

SLOFILES =	\
        $(SLO)$/break.obj \
        $(SLO)$/chardlg.obj \
        $(SLO)$/drpcps.obj \
        $(SLO)$/ccoll.obj \
        $(SLO)$/swuiccoll.obj \
        $(SLO)$/pardlg.obj \
        $(SLO)$/tblnumfm.obj \
        $(SLO)$/numpara.obj
        
LIB1TARGET = $(SLB)$/$(TARGET).lib

LIB1OBJFILES =  \
    $(SLO)$/ccoll.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

