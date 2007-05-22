#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.18 $
#
#   last change: $Author: vg $ $Date: 2007-05-22 16:33:36 $
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

PRJ=..$/..$/..

PRJNAME=sw
TARGET=unocore

# --- Settings -----------------------------------------------------
#ENABLE_EXCEPTIONS=TRUE

.INCLUDE :	$(PRJ)$/inc$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------
EXCEPTIONSFILES=	\
    $(SLO)$/swunohelper.obj\
    $(SLO)$/SwXTextDefaults.obj\
    $(SLO)$/unobkm.obj\
    $(SLO)$/unochart.obj\
    $(SLO)$/unoevent.obj\
    $(SLO)$/unocrsrhelper.obj\
    $(SLO)$/unoevtlstnr.obj\
    $(SLO)$/unoftn.obj\
    $(SLO)$/unorefmk.obj\
    $(SLO)$/unosect.obj\
    $(SLO)$/unosett.obj\
    $(SLO)$/unocoll.obj\
    $(SLO)$/unodraw.obj\
    $(SLO)$/unofield.obj\
    $(SLO)$/unoframe.obj\
    $(SLO)$/unoidx.obj\
    $(SLO)$/unoobj.obj\
    $(SLO)$/unoobj2.obj\
    $(SLO)$/unoparagraph.obj\
    $(SLO)$/unoport.obj\
    $(SLO)$/unoredline.obj\
    $(SLO)$/unoredlines.obj\
    $(SLO)$/unosrch.obj\
    $(SLO)$/unostyle.obj\
    $(SLO)$/unotbl.obj \
    $(SLO)$/TextCursorHelper.obj \
    $(SLO)$/unotext.obj

.IF "$(GUI)$(COM)$(CPU)" == "WNTMSCI"
EXCEPTIONSNOOPTFILES =$(SLO)$/unoportenum.obj
.ELSE
EXCEPTIONSFILES +=$(SLO)$/unoportenum.obj
.ENDIF


SRS1NAME=$(TARGET)
SRC1FILES =  \
        unocore.src

SLOFILES =	\
    $(SLO)$/swunohelper.obj\
    $(SLO)$/SwXTextDefaults.obj\
    $(SLO)$/unoportenum.obj\
    $(SLO)$/unobkm.obj\
    $(SLO)$/unochart.obj\
    $(SLO)$/unoevent.obj\
    $(SLO)$/unocrsrhelper.obj\
    $(SLO)$/unoevtlstnr.obj\
    $(SLO)$/unoftn.obj\
    $(SLO)$/unorefmk.obj\
    $(SLO)$/unosect.obj\
    $(SLO)$/unosett.obj\
    $(SLO)$/unocoll.obj\
    $(SLO)$/unodraw.obj\
    $(SLO)$/unofield.obj\
    $(SLO)$/unoframe.obj\
    $(SLO)$/unoidx.obj\
    $(SLO)$/unoobj.obj\
    $(SLO)$/unoobj2.obj\
    $(SLO)$/unoparagraph.obj\
    $(SLO)$/unoport.obj\
    $(SLO)$/unoredline.obj\
    $(SLO)$/unoredlines.obj\
    $(SLO)$/unosrch.obj\
    $(SLO)$/unostyle.obj\
    $(SLO)$/unotbl.obj \
    $(SLO)$/TextCursorHelper.obj \
    $(SLO)$/unotext.obj\
    $(SLO)$/unoclbck.obj\
    $(SLO)$/unomap.obj\
    $(SLO)$/unoprnms.obj\
    $(SLO)$/XMLRangeHelper.obj



# --- Targets -------------------------------------------------------


.INCLUDE :	target.mk

