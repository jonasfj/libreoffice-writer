#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: ihi $ $Date: 2007-11-26 15:29:18 $
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
TARGET=fields

AUTOSEG=true

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

CXXFILES = \
        authfld.cxx \
        cellfml.cxx \
        chpfld.cxx \
        dbfld.cxx \
        ddefld.cxx \
        ddetbl.cxx \
        docufld.cxx \
        expfld.cxx \
        fldbas.cxx \
        flddat.cxx \
        flddropdown.cxx \
        scrptfld.cxx \
        macrofld.cxx \
        fldlst.cxx \
        reffld.cxx \
        tblcalc.cxx \
        usrfld.cxx



SLOFILES =	\
        $(SLO)$/authfld.obj \
        $(SLO)$/cellfml.obj \
        $(SLO)$/chpfld.obj \
        $(SLO)$/dbfld.obj \
        $(SLO)$/ddefld.obj \
        $(SLO)$/ddetbl.obj \
        $(SLO)$/docufld.obj \
        $(SLO)$/expfld.obj \
        $(SLO)$/fldbas.obj \
        $(SLO)$/flddat.obj \
        $(SLO)$/flddropdown.obj \
        $(SLO)$/fldlst.obj \
        $(SLO)$/scrptfld.obj \
        $(SLO)$/macrofld.obj \
        $(SLO)$/reffld.obj \
        $(SLO)$/tblcalc.obj \
        $(SLO)$/usrfld.obj

EXCEPTIONSFILES = \
        $(SLO)$/flddropdown.obj \
        $(SLO)$/docufld.obj 

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

