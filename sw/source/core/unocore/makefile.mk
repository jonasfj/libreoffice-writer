#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: os $ $Date: 2000-12-15 12:30:53 $
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
TARGET=unocore

AUTOSEG=true

PROJECTPCH=core_pch
PDBTARGET=core_pch
PROJECTPCHSOURCE=..$/core_1st$/core_pch

# --- Settings -----------------------------------------------------
ENABLE_EXCEPTIONS=TRUE

.INCLUDE :	$(PRJ)$/inc$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/sw.mk

.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF



# --- Files --------------------------------------------------------

CXXFILES = \
        unobkm.cxx \
        unoclbck.cxx \
        unocrsrhelper.cxx\
        unocoll.cxx \
        unodraw.cxx \
        unofield.cxx \
        unoframe.cxx \
        unoidx.cxx \
        unomap.cxx \
        unoobj.cxx \
        unoobj2.cxx \
        unoparagraph.cxx \
        unoport.cxx \
        unoportenum.cxx \
        unoprnms.cxx \
        unorefmk.cxx \
        unosett.cxx \
        unosrch.cxx \
        unostyle.cxx \
        unotbl.cxx	\
         unotext.cxx


SLOFILES =	\
        $(SLO)$/unobkm.obj\
        $(SLO)$/unoclbck.obj\
        $(SLO)$/unocrsrhelper.obj\
        $(SLO)$/unoftn.obj\
        $(SLO)$/unorefmk.obj\
        $(SLO)$/unosect.obj\
        $(SLO)$/unosett.obj\
        $(SLO)$/unocoll.obj\
        $(SLO)$/unodraw.obj\
        $(SLO)$/unofield.obj\
        $(SLO)$/unoframe.obj\
        $(SLO)$/unoidx.obj\
        $(SLO)$/unomap.obj\
        $(SLO)$/unoobj.obj\
        $(SLO)$/unoobj2.obj\
        $(SLO)$/unoparagraph.obj\
        $(SLO)$/unoport.obj\
        $(SLO)$/unoportenum.obj\
        $(SLO)$/unoprnms.obj\
        $(SLO)$/unosrch.obj\
        $(SLO)$/unostyle.obj\
        $(SLO)$/unotbl.obj \
        $(SLO)$/unotext.obj


# --- Targets -------------------------------------------------------

.INCLUDE :	target.mk

