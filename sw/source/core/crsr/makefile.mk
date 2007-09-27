#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: hr $ $Date: 2007-09-27 08:30:07 $
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
TARGET=crsr

AUTOSEG=true

# --- Settings -----------------------------------------------------

.INCLUDE :	$(PRJ)$/inc$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/sw.mk

.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

# --- Files --------------------------------------------------------

EXCEPTIONSFILES=    \
    $(SLO)$/crsrsh.obj \
    $(SLO)$/viscrs.obj

SLOFILES =	\
    $(SLO)$/bookmrk.obj \
    $(SLO)$/callnk.obj \
    $(SLO)$/crbm.obj \
    $(SLO)$/crsrsh.obj \
    $(SLO)$/crstrvl.obj \
    $(SLO)$/crstrvl1.obj \
    $(SLO)$/findattr.obj \
    $(SLO)$/findcoll.obj \
    $(SLO)$/findfmt.obj \
    $(SLO)$/findtxt.obj \
    $(SLO)$/pam.obj \
    $(SLO)$/paminit.obj \
    $(SLO)$/swcrsr.obj \
    $(SLO)$/trvlcol.obj \
    $(SLO)$/trvlfnfl.obj \
    $(SLO)$/trvlreg.obj \
    $(SLO)$/trvltbl.obj \
    $(SLO)$/unocrsr.obj \
    $(SLO)$/viscrs.obj

.IF "$(mydebug)" != ""
SLOFILES +=  \
        $(SLO)$/pamio.obj
.ENDIF

# --- Tagets -------------------------------------------------------

.INCLUDE :	target.mk
