#*************************************************************************
#
#   $RCSfile: sw.mk,v $
#
#   $Revision: 1.11 $
#
#   last change: $Author: pjunck $ $Date: 2004-10-22 10:57:57 $
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

# CDEFS=$(CDEFS) -DXXX

# Bereiche aktivieren

#CDEFS+=-DXML_CORE_API
CDEFS+=-DNUM_RELSPACE
CDEFS+=-DVERTICAL_LAYOUT
CDEFS+=-DACCESSIBLE_LAYOUT
CDEFS+=-DBIDI

# define SW_DLLIMPLEMENTATION (see @ swdllapi.h)
CDEFS += -DSW_DLLIMPLEMENTATION

# set default symbol visibility / scope to hidden
.IF "$(COMNAME)" == "gcc3"
.IF "$(CCNUMVER)" >= "000300040000"
CFLAGS += -fvisibility=hidden
.ENDIF # 3.4.0
.ENDIF # gcc3

.IF "$(COMNAME)" == "sunpro5"
.IF "$(CCNUMVER)" >= "00050005"
CFLAGS += -xldscope=hidden
.ENDIF # 5.5
.ENDIF # sunpro5

# ------------------------------------------------------------------------
