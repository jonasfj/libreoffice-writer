#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.30 $
#
#   last change: $Author: rt $ $Date: 2003-11-26 08:39:38 $
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

PRJNAME=sw
TARGET=sw
GEN_HID=TRUE
GEN_HID_OTHER=TRUE

# --- Settings ------------------------------------------------------------

.INCLUDE :  settings.mk

# --- Allgemein -----------------------------------------------------------

.IF "$(OS)"=="IRIX"
LINKFLAGS+=-Wl,-LD_LAYOUT:lgot_buffer=40
.ENDIF

sw_res_files= \
    $(SRS)$/app.srs          \
    $(SRS)$/dialog.srs       \
    $(SRS)$/chrdlg.srs       \
    $(SRS)$/config.srs       \
    $(SRS)$/dbui.srs	    \
    $(SRS)$/dochdl.srs       \
    $(SRS)$/docvw.srs        \
    $(SRS)$/envelp.srs       \
    $(SRS)$/fldui.srs        \
    $(SRS)$/fmtui.srs        \
    $(SRS)$/frmdlg.srs       \
    $(SRS)$/globdoc.srs      \
    $(SRS)$/index.srs        \
    $(SRS)$/lingu.srs        \
    $(SRS)$/misc.srs         \
    $(SRS)$/ribbar.srs       \
    $(SRS)$/shells.srs       \
    $(SRS)$/swslots.srs     \
    $(SRS)$/table.srs        \
    $(SRS)$/uiview.srs       \
    $(SRS)$/utlui.srs        \
    $(SRS)$/web.srs          \
    $(SRS)$/wizard.srs       \
    $(SRS)$/wrtsh.srs        \
    $(SOLARRESDIR)$/sfx.srs

RESLIB1NAME=sw

RESLIB1SRSFILES= \
    $(sw_res_files)

SHL1TARGET= $(TARGET)$(UPD)$(DLLPOSTFIX)
SHL1VERSIONMAP= $(TARGET).map
SHL1IMPLIB= _$(TARGET)
SHL1LIBS= \
    $(SLB)$/core1.lib\
    $(SLB)$/core2.lib\
    $(SLB)$/filter.lib\
    $(SLB)$/ui1.lib\
    $(SLB)$/ui2.lib

.IF "$(OS)"!="MACOSX"
# static libraries
SHL1STDLIBS+= $(SCHLIB)
.ENDIF

# dynamic libraries
SHL1STDLIBS+= \
    $(OFALIB) \
    $(SVXLIB) \
    $(SFXLIB) \
    $(XMLOFFLIB) \
    $(BASICLIB) \
    $(GOODIESLIB) \
    $(SO2LIB) \
    $(SVTOOLLIB) \
    $(TKLIB) \
    $(VCLLIB) \
    $(SVLLIB)	\
    $(SOTLIB) \
    $(UNOTOOLSLIB) \
    $(TOOLSLIB) \
    $(COMPHELPERLIB) \
    $(UCBHELPERLIB) \
    $(CPPUHELPERLIB) \
    $(CPPULIB) \
    $(VOSLIB) \
    $(SALLIB) \
        $(ICUUCLIB) \
    $(I18NUTILLIB)

.IF "$(OS)"=="MACOSX"
# static libraries at end for OS X
SHL1STDLIBS+= $(SCHLIB)
.ENDIF

.IF "$(GUI)"=="WNT"
SHL1STDLIBS+= advapi32.lib
.ENDIF # WNT

SHL1DEPN=   \
    $(SLB)$/core1.lib\
    $(SLB)$/core2.lib\
    $(SLB)$/filter.lib\
    $(SLB)$/ui1.lib\
    $(SLB)$/ui2.lib


SHL1OBJS= \
    $(OUT)$/slo$/swmodule.obj \
    $(OUT)$/slo$/swdll.obj
#	$(SLO)$/.obj		  ^ \ nicht vergessen!


SHL1DEF=    $(MISC)$/$(SHL1TARGET).def
SHL1BASE=	0x1e000000

DEF1NAME	=$(SHL1TARGET)

SHL2TARGET= swd$(UPD)$(DLLPOSTFIX)
SHL2IMPLIB= swdimp
SHL2VERSIONMAP= swd.map
SHL2DEF=$(MISC)$/$(SHL2TARGET).def
DEF2NAME=       $(SHL2TARGET)

SHL2STDLIBS= \
            $(SFX2LIB) \
            $(SVTOOLLIB) \
            $(SVLLIB) \
            $(VCLLIB) \
            $(SOTLIB) \
            $(TOOLSLIB) \
        $(UCBHELPERLIB) \
            $(COMPHELPERLIB) \
            $(CPPUHELPERLIB) \
            $(CPPULIB) \
            $(SALLIB)

SHL2OBJS=   $(SLO)$/swdetect.obj \
        $(SLO)$/swdet2.obj \
        $(SLO)$/detreg.obj  \
            $(OUT)$/obj$/w4wflt.obj

SHL2DEPN+=  makefile.mk

.INCLUDE :  target.mk

