/*************************************************************************
 *
 *  $RCSfile: ftnidx.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-08-12 12:02:45 $
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
#ifndef _FTNIDX_HXX
#define _FTNIDX_HXX


#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>

class SwTxtFtn;
class SwNodeIndex;
class SwSectionNode;

// ueberall, wo der NodeIndex gebraucht wird, werden die hier fehlenden
// Headerfiles schon includes. Darum hier nur als define und nicht als
// inline Methode (spart Compile-Zeit)
#define _SwTxtFtn_GetIndex( pFIdx )	(pFIdx->GetTxtNode().GetIndex())


typedef SwTxtFtn* SwTxtFtnPtr;
SV_DECL_PTRARR_SORT( _SwFtnIdxs, SwTxtFtnPtr, 0, 10 )

class SwFtnIdxs : public _SwFtnIdxs
{
public:
    SwFtnIdxs() {}

    void UpdateFtn( const SwNodeIndex& rStt );   	// ab Pos. alle Updaten
    void UpdateAllFtn();					// alle Fussnoten updaten

    SwTxtFtn* SeekEntry( const SwNodeIndex& rIdx, USHORT* pPos = 0 ) const;
};


class SwUpdFtnEndNtAtEnd
{
    SvPtrarr aFtnSects, aEndSects;
    SvUShorts aFtnNums, aEndNums;

public:
    SwUpdFtnEndNtAtEnd() : aFtnSects( 0, 4 ), aEndSects( 0, 4 ),
                           aFtnNums( 0, 4 ), aEndNums( 0, 4 )
        {}

    static const SwSectionNode* FindSectNdWithEndAttr(
                                            const SwTxtFtn& rTxtFtn );

    USHORT GetNumber( const SwTxtFtn& rTxtFtn, const SwSectionNode& rNd );
    USHORT ChkNumber( const SwTxtFtn& rTxtFtn );
};



#endif // _FTNIDX_HXX

