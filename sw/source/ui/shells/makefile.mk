#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: kz $ $Date: 2004-10-04 19:30:28 $
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
TARGET=shells

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =  \
        shells.src

SLOFILES =  \
        $(SLO)$/basesh.obj \
        $(SLO)$/beziersh.obj \
        $(SLO)$/drawdlg.obj \
        $(SLO)$/drawsh.obj \
        $(SLO)$/drformsh.obj \
        $(SLO)$/drwbassh.obj \
        $(SLO)$/drwtxtex.obj \
        $(SLO)$/drwtxtsh.obj \
        $(SLO)$/frmsh.obj \
        $(SLO)$/grfsh.obj \
        $(SLO)$/grfshex.obj \
        $(SLO)$/mediash.obj \
        $(SLO)$/listsh.obj \
        $(SLO)$/olesh.obj \
        $(SLO)$/slotadd.obj \
        $(SLO)$/tabsh.obj \
        $(SLO)$/textdrw.obj \
        $(SLO)$/textfld.obj \
        $(SLO)$/textglos.obj \
        $(SLO)$/textidx.obj \
        $(SLO)$/textsh.obj \
        $(SLO)$/textsh1.obj \
        $(SLO)$/textsh2.obj \
        $(SLO)$/txtattr.obj \
        $(SLO)$/txtcrsr.obj \
        $(SLO)$/txtnum.obj

EXCEPTIONSFILES =  \
        $(SLO)$/textsh.obj \
        $(SLO)$/textsh2.obj \
        $(SLO)$/grfshex.obj	\
        $(SLO)$/textsh1.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

$(SRS)$/shells.srs: $(SOLARINCDIR)$/svx$/globlmn.hrc

