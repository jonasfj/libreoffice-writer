/*************************************************************************
 *
 *  $RCSfile: txttypes.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-26 15:29:03 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef _TXTTYPES_HXX
#define _TXTTYPES_HXX

#include "swtypes.hxx"

// Ueberpruefte USHORTs, z.B. Indizes auf Arrays oder garantiert kleine
// Integerwerte, auch von aussen vorgegebene
#define MSHORT USHORT
#define MSHRT_MAX USHRT_MAX
// Koordinaten: Breite, Hoehe und Offsets in X-/Y-Richtung sowie Ascent etc.
#define KSHORT USHORT
#define KSHRT_MAX USHRT_MAX

// Portiontypen
#define POR_LIN			0x0000
#define POR_FLYCNT		0x0001

#define	POR_HOLE		0x0080
#define POR_TMPEND 		0x0081
#define POR_BRK 		0x0082
#define POR_KERN 		0x0083
#define POR_ARROW 		0x0084
#define POR_MULTI 		0x0085
#define POR_HIDDEN_TXT  0x0086

#define POR_TXT 		0x8000
#define POR_LAY 		0x8001
#define POR_PARA		0x8002
#define POR_URL			0x8003
#define POR_HNG 		0x8004

#define	POR_DROP		0x8080
#define POR_TOX 		0x8089
#define POR_ISOTOX		0x808a
#define POR_REF 		0x808b
#define	POR_ISOREF		0x808c

#define	POR_EXP 		0xc080
#define POR_BLANK		0xc081
#define POR_POSTITS     0xc082

#define POR_HYPH		0xd080
#define POR_HYPHSTR 	0xd081
#define POR_SOFTHYPH	0xd082
#define	POR_SOFTHYPHSTR 0xd083
#define POR_SOFTHYPH_COMP 0xd084

#define POR_FLD 		0xe080
#define POR_HIDDEN		0xe081
#define	POR_QUOVADIS	0xe082
#define	POR_ERGOSUM 	0xe083
#define POR_COMBINED 	0xe084
#define POR_FTN         0xe085

#define	POR_FTNNUM		0xe880
#define POR_NUMBER		0xe881
#define POR_BULLET		0xe882
#define POR_GRFNUM		0xe883

#define POR_GLUE		0x0480

#define POR_MARGIN		0x04c0

#define POR_FIX 		0x06c0
#define POR_FLY 		0x06c1

#define	POR_TAB 		0x0750

#define	POR_TABRIGHT	0x07d0
#define	POR_TABCENTER	0x07d1
#define	POR_TABDECIMAL	0x07d2

#define POR_TABLEFT 	0x0740

#endif	//_TXTTYPES_HXX
