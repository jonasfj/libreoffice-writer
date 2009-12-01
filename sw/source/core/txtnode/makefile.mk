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
# $Revision: 1.14 $
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
TARGET=txtnode

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

# --- Files --------------------------------------------------------

SLOFILES =  \
        $(SLO)$/atrfld.obj \
        $(SLO)$/atrflyin.obj \
        $(SLO)$/atrftn.obj \
        $(SLO)$/atrref.obj \
        $(SLO)$/atrtox.obj \
        $(SLO)$/chrfmt.obj \
        $(SLO)$/fmtatr2.obj \
        $(SLO)$/fntcap.obj \
        $(SLO)$/fntcache.obj \
        $(SLO)$/swfntcch.obj \
        $(SLO)$/SwGrammarContact.obj \
                $(SLO)$/modeltoviewhelper.obj \
                $(SLO)$/ndhints.obj \
        $(SLO)$/ndtxt.obj \
        $(SLO)$/swfont.obj \
        $(SLO)$/thints.obj \
        $(SLO)$/txatbase.obj \
        $(SLO)$/txatritr.obj \
        $(SLO)$/txtatr2.obj \
                $(SLO)$/txtedt.obj


EXCEPTIONSFILES = \
        $(SLO)$/fmtatr2.obj \
        $(SLO)$/atrtox.obj \
        $(SLO)$/atrflyin.obj \
        $(SLO)$/docnew.obj \
        $(SLO)$/fntcache.obj \
        $(SLO)$/ndtxt.obj \
        $(SLO)$/thints.obj \
        $(SLO)$/txtedt.obj


.IF "$(dbutil)" != ""
OFILES+=$(SLO)$/dbchratr.$(QBJX)
.ENDIF

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

