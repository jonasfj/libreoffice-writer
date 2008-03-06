#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.21 $
#
#   last change: $Author: kz $ $Date: 2008-03-06 12:32:41 $
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
TARGET=dialog
LIBTARGET=no

# future: DEMO\dialog.srs

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/sw.mk

.IF "$(ENABLE_LAYOUT)" == "TRUE"
CFLAGS+= -DENABLE_LAYOUT=1 -I../$(PRJ)/layout/inc -I../$(PRJ)/layout/$(INPATH)/inc
.ENDIF # ENABLE_LAYOUT == TRUE

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =  \
        abstract.src \
        ascfldlg.src \
        dialog.src \
        docstdlg.src \
        regionsw.src \
        wordcountdialog.src

EXCEPTIONSFILES = \
        $(SLO)$/SwSpellDialogChildWindow.obj

SLOFILES =  \
        $(SLO)$/abstract.obj \
        $(SLO)$/addrdlg.obj \
        $(SLO)$/ascfldlg.obj \
        $(SLO)$/docstdlg.obj \
        $(SLO)$/macassgn.obj \
        $(SLO)$/SwSpellDialogChildWindow.obj \
        $(SLO)$/regionsw.obj	\
        $(SLO)$/uiregionsw.obj	\
        $(SLO)$/swabstdlg.obj	\
        $(SLO)$/swuiexp.obj		\
        $(SLO)$/swwrtshitem.obj \
        $(SLO)$/swdialmgr.obj  \
        $(SLO)$/wordcountdialog.obj \
        $(SLO)$/swdlgfact.obj

LIB1TARGET = $(SLB)$/$(TARGET).lib

LIB1OBJFILES =  \
        $(SLO)$/regionsw.obj \
        $(SLO)$/swabstdlg.obj \
        $(SLO)$/SwSpellDialogChildWindow.obj \
        $(SLO)$/swwrtshitem.obj

#		$(SLO)$/macassgn.obj \

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk
$(INCCOM)$/swuilib.hxx: makefile.mk
.IF "$(GUI)"=="UNX"
    $(RM) $@
    echo \#define DLL_NAME \"libswui$(DLLPOSTFIX)$(DLLPOST)\" >$@
.ELSE
.IF "$(USE_SHELL)"!="4nt"
    echo \#define DLL_NAME \"swui$(DLLPOSTFIX)$(DLLPOST)\" >$@
.ELSE          # "$(USE_SHELL)"!="4nt"
    echo #define DLL_NAME "swui$(DLLPOSTFIX)$(DLLPOST)" >$@
.ENDIF          # "$(USE_SHELL)"!="4nt"
.ENDIF

$(SLO)$/swabstdlg.obj : $(INCCOM)$/swuilib.hxx







