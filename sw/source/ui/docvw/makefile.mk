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
TARGET=docvw
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =  \
        access.src \
                docvw.src \
                annotation.src

SLOFILES =  \
        $(SLO)$/edtdd.obj \
        $(SLO)$/edtwin.obj \
        $(SLO)$/edtwin2.obj \
        $(SLO)$/edtwin3.obj \
        $(SLO)$/romenu.obj \
        $(SLO)$/srcedtw.obj \
                $(SLO)$/PostItMgr.obj \
                $(SLO)$/frmsidebarwincontainer.obj \
                $(SLO)$/AnchorOverlayObject.obj \
                $(SLO)$/ShadowOverlayObject.obj \
                $(SLO)$/SidebarWin.obj \
                $(SLO)$/SidebarWinAcc.obj \
                $(SLO)$/AnnotationWin.obj \
                $(SLO)$/AnnotationMenuButton.obj \
                $(SLO)$/SidebarTxtControl.obj \
                $(SLO)$/SidebarTxtControlAcc.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

