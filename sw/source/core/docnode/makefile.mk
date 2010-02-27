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
TARGET=docnode

# --- Settings -----------------------------------------------------

.INCLUDE :	$(PRJ)$/inc$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

EXCEPTIONSFILES=        \
        $(SLO)$/cancellablejob.obj \
        $(SLO)$/finalthreadmanager.obj \
        $(SLO)$/ndcopy.obj \
        $(SLO)$/ndtbl.obj \
        $(SLO)$/node.obj \
        $(SLO)$/observablethread.obj \
        $(SLO)$/retrieveinputstream.obj \
        $(SLO)$/retrievedinputstreamdata.obj \
        $(SLO)$/swbaslnk.obj \
        $(SLO)$/threadmanager.obj

SLOFILES =	\
        $(SLO)$/ndindex.obj \
        $(SLO)$/ndcopy.obj \
        $(SLO)$/ndnotxt.obj \
        $(SLO)$/ndnum.obj \
        $(SLO)$/ndsect.obj \
        $(SLO)$/ndtbl.obj \
        $(SLO)$/ndtbl1.obj \
        $(SLO)$/node.obj \
        $(SLO)$/node2lay.obj \
        $(SLO)$/nodes.obj \
        $(SLO)$/section.obj \
        $(SLO)$/swbaslnk.obj \
        $(SLO)$/retrievedinputstreamdata.obj \
        $(SLO)$/observablethread.obj \
        $(SLO)$/cancellablejob.obj \
        $(SLO)$/retrieveinputstream.obj \
        $(SLO)$/retrieveinputstreamconsumer.obj \
        $(SLO)$/threadlistener.obj \
        $(SLO)$/threadmanager.obj \
        $(SLO)$/swthreadmanager.obj \
        $(SLO)$/swthreadjoiner.obj \
        $(SLO)$/pausethreadstarting.obj \
        $(SLO)$/finalthreadmanager.obj


# --- Targets -------------------------------------------------------

.INCLUDE :	target.mk

