/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ndtyp.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:02:50 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _NDTYP_HXX
#define _NDTYP_HXX

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

// Ids fuer die verschiedenden Nodes; in der Basisklasse steht der Member,
// der angibt, um was fuer einen es sich handelt
const BYTE ND_ENDNODE		= 0x01;
const BYTE ND_STARTNODE		= 0x02;
const BYTE ND_TABLENODE		= 0x06;
const BYTE ND_TEXTNODE 		= 0x08;
const BYTE ND_GRFNODE 		= 0x10;
const BYTE ND_OLENODE 		= 0x20;

const BYTE ND_CONTENTNODE	= 0x38;	// ContentNode (eines von den 3 Bits)
const BYTE ND_NOTXTNODE		= 0x30;	// NoTxtNode (eines von den 2 Bits)

const BYTE ND_SECTIONNODE   = 0x42;
// nur fuer internen Gebrauch!!
const BYTE ND_SECTIONDUMMY  = 0x40; //(ND_SECTIONNODE & ~ND_STARTNODE);

// spezielle Types der StartNodes, die keine Ableitungen sind, aber
// "Bereiche" zusammenhalten.
enum SwStartNodeType
{
    SwNormalStartNode = 0,
    SwTableBoxStartNode,
    SwFlyStartNode,
    SwFootnoteStartNode,
    SwHeaderStartNode,
    SwFooterStartNode
};

// is the node the first and/or last node of a section?
// This information is used for the export filters. Our layout never have a
// distance before or after if the node is the first or last in a section.
const BYTE ND_HAS_PREV_LAYNODE = 0x01;
const BYTE ND_HAS_NEXT_LAYNODE = 0x02;



#endif
