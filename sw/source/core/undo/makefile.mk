#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
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
TARGET=undo

AUTOSEG=true

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =  \
        undo.src

EXCEPTIONSFILES = \
        $(SLO)$/SwRewriter.obj \
        $(SLO)$/unattr.obj \
        $(SLO)$/undobj.obj \
        $(SLO)$/undraw.obj \
        $(SLO)$/unovwr.obj \
        $(SLO)$/untbl.obj

SLOFILES =	\
        $(SLO)$/SwRewriter.obj \
        $(SLO)$/SwUndoField.obj \
        $(SLO)$/SwUndoPageDesc.obj \
        $(SLO)$/SwUndoFmt.obj \
        $(SLO)$/SwUndoTOXChange.obj \
        $(SLO)$/docundo.obj \
        $(SLO)$/rolbck.obj \
        $(SLO)$/unattr.obj \
        $(SLO)$/unbkmk.obj \
        $(SLO)$/undel.obj \
        $(SLO)$/undobj.obj \
        $(SLO)$/undobj1.obj \
                $(SLO)$/undoflystrattr.obj \
        $(SLO)$/undraw.obj \
        $(SLO)$/unfmco.obj \
        $(SLO)$/unins.obj \
        $(SLO)$/unmove.obj \
        $(SLO)$/unnum.obj \
        $(SLO)$/unoutl.obj \
        $(SLO)$/unovwr.obj \
        $(SLO)$/unredln.obj \
        $(SLO)$/unsect.obj \
        $(SLO)$/unsort.obj \
        $(SLO)$/unspnd.obj \
        $(SLO)$/untbl.obj \
        $(SLO)$/untblk.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

