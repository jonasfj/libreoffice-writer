#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.6 $
#
#   last change: $Author: tl $ $Date: 2001-05-02 16:58:48 $
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

#MKDEPENDSOLVER=YES
PROJECTPCH4DLL=TRUE

PRJ=..

PRJNAME=starmath
TARGET=starmath

PROJECTPCH=math_pch
PDBTARGET=math_pch
PROJECTPCHSOURCE=math_pch


# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

.INCLUDE :  $(UPD)minor.mk
RSCUPDVER=$(RSCREVISION)(SV$(UPD)$(UPDMINOR))

IMGLST_SRS=$(SRS)$/smres.srs

# --- Files --------------------------------------------------------

SMDLL=TRUE

SRS2NAME =smres
SRC2FILES = smres.src   \
            symbol.src	\
            commands.src

SLOFILES =  \
        $(SLO)$/register.obj \
        $(SLO)$/typemap.obj \
        $(SLO)$/symbol.obj \
        $(SLO)$/toolbox.obj \
        $(SLO)$/action.obj \
        $(SLO)$/cfgitem.obj \
        $(SLO)$/config.obj \
        $(SLO)$/dialog.obj \
        $(SLO)$/document.obj \
        $(SLO)$/mathtype.obj \
        $(SLO)$/mathml.obj \
        $(SLO)$/format.obj \
        $(SLO)$/node.obj \
        $(SLO)$/parse.obj \
        $(SLO)$/utility.obj \
        $(SLO)$/smdll.obj \
        $(SLO)$/smmod.obj \
        $(SLO)$/view.obj \
        $(SLO)$/edit.obj \
        $(SLO)$/rect.obj \
        $(SLO)$/unomodel.obj \
        $(SLO)$/xchar.obj

EXCEPTIONSFILES =   \
        $(SLO)$/register.obj  \
        $(SLO)$/mathml.obj \
        $(SLO)$/unomodel.obj


LIB2TARGET =    $(SLB)$/ysm.lib
LIB2ARCHIV =    $(LB)$/libysm.a
LIB2OBJFILES  =    $(SLO)$/smlib.obj

DEPOBJFILES = $(SLO)$/smlib.obj
# --- Targets -------------------------------------------------------

.IF "$(depend)" == ""

ALL:	\
        $(INCCOM)$/dllname.hxx	\
        ALLTAR

.IF "$(GUI)"=="UNX" || "$(GUI)"=="MAC"
CDEFS+=-DUSE_POLYGON
.ENDIF

$(INCCOM)$/dllname.hxx: makefile.mk
.IF "$(GUI)"=="OS2"
    echo #define DLL_NAME "sm$(UPD)$(DLLPOSTFIX)" >$@
.ELSE
.IF "$(GUI)"=="MAC"
    echo "$(HASHMARK)define DLL_NAME �"sm$(UPD)$(DLLPOSTFIX).dll�"" > $@
.ELSE
.IF "$(GUI)"=="UNX"
    $(RM) $@
    echo #define DLL_NAME \"libsm$(UPD)$(DLLPOSTFIX)$(DLLPOST)\" >$@
.ELSE
    echo #define DLL_NAME "sm$(UPD)$(DLLPOSTFIX)$(DLLPOST)" >$@
.ENDIF
.ENDIF
.ENDIF

.ENDIF

.INCLUDE :  target.mk

$(SRS)$/smres.srs: $(SOLARINCDIR)$/svx$/globlmn.hrc

