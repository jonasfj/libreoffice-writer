#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1.1.1 $
#
#   last change: $Author: hr $ $Date: 2000-09-18 16:57:27 $
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

PRJ=..

PRJNAME=SM
TARGET=smath3
LIBTARGET=NO
GEN_HID=TRUE
USE_DEFFILE=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk


# --- Files --------------------------------------------------------

RESLIB1NAME=sm
RESLIB1SRSFILES=\
    $(SRS)$/smres.srs \
    $(SRS)$/smslots.srs \
    $(SOLARVERSION)$/$(INPATH)$/res$(EXT_UPDMINOR)$/sfx.srs

SHL1TARGET= sm$(UPD)$(DLLPOSTFIX)
SHL1VERSIONMAP= sm.map
.IF "$(GUI)" == "WNT"
SHL1RES=	$(RCTARGET)
.ENDIF

SHL1IMPLIB= smimp
SHL1LIBS=   $(SLB)$/starmath.lib
SHL1STDLIBS= \
            $(TOOLSLIB) \
            $(SVTOOLLIB) \
            $(SVLLIB)	\
            $(SVMEMLIB) \
            $(SVLIB) \
            $(SOTLIB) \
            $(SO2LIB) \
            $(SFX2LIB) \
                        $(CPPULIB) \
                        $(CPPUHELPERLIB) \
            $(SVXLIB) \
            $(SALLIB) \
            $(VOSLIB) \
            $(UCBHELPERLIB)

SHL1DEPN=               makefile.mk \
                        $(MISC)$/$(SHL1TARGET).flt

SHL1DEF=		$(MISC)$/$(SHL1TARGET).def

.IF "$(GUI)" != "UNX"
SHL1OBJS=   $(SLO)$/smdll.obj
.ENDIF

DEF1NAME=		$(SHL1TARGET)
DEF1EXPORTFILE=	exports.dxp

# --- Targets -------------------------------------------------------------

.IF "$(depend)" == ""

ALL: \
    $(SRS)$/hidother.hid\
    ALLTAR
.ENDIF

.INCLUDE :  target.mk

.IF "$(depend)" == ""

$(MISC)$/$(SHL1TARGET).flt:
    @echo ------------------------------
    @echo Making: $@
    @echo WEP>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@

.ENDIF

$(SRS)$/hidother.hid: hidother.src
.IF "$(GUI)$(CPU)"=="WNTI"
    +mhids hidother.src ..\$(INPATH)$/srs starmath hidother
.ELSE
    @echo nix
.ENDIF
