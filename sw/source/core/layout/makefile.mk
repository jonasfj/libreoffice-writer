#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: ama $ $Date: 2001-05-29 12:38:15 $
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
TARGET=layout

AUTOSEG=true

PROJECTPCH=core_pch
PDBTARGET=core_pch
PROJECTPCHSOURCE=..$/core_1st$/core_pch

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

.IF "$(madebug)" != ""
CDEFS+=-DDEBUG
.ENDIF

#		"Querdarstellung des Dokumentes"
#		CDEFS=$(CDEFS) -DQUER

# 		CDEFS=$(CDEFS) -DPAGE

.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF


# --- Files --------------------------------------------------------

CXXFILES = \
        atrfrm.cxx \
        calcmove.cxx \
        colfrm.cxx \
        findfrm.cxx \
        flowfrm.cxx \
        fly.cxx \
        flycnt.cxx \
        flyincnt.cxx \
        flylay.cxx \
        flypos.cxx \
        frmtool.cxx \
        ftnfrm.cxx \
        hffrm.cxx \
        layact.cxx \
                laycache.cxx \
        layouter.cxx \
        newfrm.cxx \
        pagechg.cxx \
        pagedesc.cxx \
        pageiter.cxx \
        paintfrm.cxx \
        sectfrm.cxx \
        ssfrm.cxx \
        tabfrm.cxx \
        trvlfrm.cxx \
        unusedf.cxx \
        virtoutp.cxx \
        wsfrm.cxx



SLOFILES =  \
        $(SLO)$/atrfrm.obj \
        $(SLO)$/calcmove.obj \
        $(SLO)$/colfrm.obj \
        $(SLO)$/findfrm.obj \
        $(SLO)$/flowfrm.obj \
        $(SLO)$/fly.obj \
        $(SLO)$/flycnt.obj \
        $(SLO)$/flyincnt.obj \
        $(SLO)$/flylay.obj \
        $(SLO)$/flypos.obj \
        $(SLO)$/frmtool.obj \
        $(SLO)$/ftnfrm.obj \
        $(SLO)$/hffrm.obj \
        $(SLO)$/layact.obj \
                $(SLO)$/laycache.obj \
        $(SLO)$/layouter.obj \
        $(SLO)$/newfrm.obj \
        $(SLO)$/pagechg.obj \
        $(SLO)$/pagedesc.obj \
        $(SLO)$/pageiter.obj \
        $(SLO)$/paintfrm.obj \
        $(SLO)$/sectfrm.obj \
        $(SLO)$/ssfrm.obj \
        $(SLO)$/tabfrm.obj \
        $(SLO)$/trvlfrm.obj \
        $(SLO)$/unusedf.obj \
        $(SLO)$/virtoutp.obj \
        $(SLO)$/wsfrm.obj

EXCEPTIONSFILES= \
        $(SLO)$/atrfrm.obj

.IF "$(product)"==""
.IF "$(cap)"==""
CXXFILES += \
        dbg_lay.cxx
SLOFILES +=  \
        $(SLO)$/dbg_lay.obj
.ENDIF
.ENDIF

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

