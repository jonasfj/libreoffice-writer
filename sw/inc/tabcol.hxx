/*************************************************************************
 *
 *  $RCSfile: tabcol.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-26 11:37:59 $
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
#ifndef _TABCOL_HXX
#define _TABCOL_HXX

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_LONGS
#define _SVSTDARR_BOOLS
#include <svtools/svstdarr.hxx>
#endif

#include <vector>

struct SwTabColsEntry
{
    long    nPos;
    long    nMin;
    long    nMax;
    BOOL    bHidden;    //Fuer jeden Eintrag ein Flag, Hidden oder nicht.
                        //Wenn das Flag Hidden TRUE ist liegt der Spalten-
                        //trenner nicht in der aktuellen Zeile; er muss
                        //mit gepflegt werden, darf aber nicht angezeigt
                        //werden.
};

typedef std::vector< SwTabColsEntry > SwTabColsEntries;

class SwTabCols
{
    long nLeftMin,		//Linker aeusserer Rand (Bezugspunkt) in
                        //Dokumentkordinaten.
                        //Alle anderen Werte relativ zu diesem Punkt!
            nLeft,		//Linker Rand der Tabelle.
           nRight,		//Rechter Rand der Tabelle.
           nRightMax;	//Maximaler rechter Rand der Tabelle.

    bool bLastRowAllowedToChange;       // if the last row of the table frame
                                        // is split across pages, it may not
                                        // change its size

    SwTabColsEntries aData;

    //fuer den CopyCTor
    const SwTabColsEntries& GetData() const { return aData; }

public:
    SwTabCols( USHORT nSize = 0 );
    SwTabCols( const SwTabCols& );
    SwTabCols &operator=( const SwTabCols& );
    BOOL operator==( const SwTabCols& rCmp ) const;
    long& operator[]( USHORT nPos ) { return aData[nPos].nPos; }
    long operator[]( USHORT nPos ) const { return aData[nPos].nPos; }
    USHORT Count() const { return aData.size(); }

    BOOL IsHidden( USHORT nPos ) const         { return aData[nPos].bHidden; }
    void SetHidden( USHORT nPos, BOOL bValue ) { aData[nPos].bHidden = bValue; }

    void Insert( long nValue, BOOL bValue, USHORT nPos );
    void Insert( long nValue, long nMin, long nMax, BOOL bValue, USHORT nPos );
    void Remove( USHORT nPos, USHORT nAnz = 1 );

    const SwTabColsEntry& GetEntry( USHORT nPos ) const { return aData[nPos]; }
          SwTabColsEntry& GetEntry( USHORT nPos )  { return aData[nPos]; }

    long GetLeftMin() const { return nLeftMin; }
    long GetLeft()	const { return nLeft;	 }
    long GetRight()	const { return nRight;	 }
    long GetRightMax()const { return nRightMax;}

    void SetLeftMin ( long nNew )	{ nLeftMin = nNew; }
    void SetLeft	( long nNew )	{ nLeft = nNew;	   }
    void SetRight	( long nNew )	{ nRight = nNew;   }
    void SetRightMax( long nNew )	{ nRightMax = nNew;}

    bool IsLastRowAllowedToChange() const { return bLastRowAllowedToChange; }
    void SetLastRowAllowedToChange( bool bNew ) { bLastRowAllowedToChange = bNew; }
};

#endif	//_TABCOL_HXX
