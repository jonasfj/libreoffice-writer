#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1.1.1 $
#
#   last change: $Author: hr $ $Date: 2000-09-19 00:08:17 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************

PRJ=..$/..$/..

PRJNAME=sw
TARGET=crsr

AUTOSEG=true

PROJECTPCH=core_pch
PDBTARGET=core_pch
PROJECTPCHSOURCE=..$/core_1st$/core_pch

# --- Settings -----------------------------------------------------

.INCLUDE :	$(PRJ)$/inc$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/sw.mk

.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

# --- Files --------------------------------------------------------

CXXFILES = \
        bookmrk.cxx \
        callnk.cxx \
        crbm.cxx \
        crsrsh.cxx \
        crstrvl.cxx \
        crstrvl1.cxx \
        findattr.cxx \
        findcoll.cxx \
        findfmt.cxx \
        findtxt.cxx \
        pam.cxx \
        paminit.cxx \
        swcrsr.cxx \
        trvlcol.cxx \
        trvlfnfl.cxx \
        trvlreg.cxx \
        trvltbl.cxx \
        unocrsr.cxx \
        viscrs.cxx

.IF "$(GUI)"=="MAC"
CXXFILES += \
        crsrsh1.cxx
.ENDIF
.IF "$(mydebug)" != ""
CXXFILES += \
        pamio.cxx
.ENDIF

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

.IF "$(GUI)"=="MAC"
SLOFILES +=  \
        $(SLO)$/crsrsh1.obj
.ENDIF
.IF "$(mydebug)" != ""
SLOFILES +=  \
        $(SLO)$/pamio.obj
.ENDIF

# --- Tagets -------------------------------------------------------

.INCLUDE :	target.mk
