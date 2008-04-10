/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: txtftn.hxx,v $
 * $Revision: 1.3 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _TXTFTN_HXX
#define _TXTFTN_HXX

#include <txatbase.hxx>

class SwNodeIndex;
class SwTxtNode;
class SwNodes;
class SwDoc;

// ATT_FTN **********************************************************

class SwTxtFtn : public SwTxtAttr
{
    SwNodeIndex *pStartNode;
    SwTxtNode* pMyTxtNd;
    USHORT nSeqNo;

    // Zuweisung und Copy-Ctor sind nicht erlaubt.
    SwTxtFtn& operator=( const SwTxtFtn &rTxtFtn );
    SwTxtFtn( const SwTxtFtn &rTxtFtn );

public:
    SwTxtFtn( const SwFmtFtn& rAttr, xub_StrLen nStart );
    ~SwTxtFtn();

    inline SwNodeIndex *GetStartNode() const { return pStartNode; }
    void SetStartNode( const SwNodeIndex *pNode, BOOL bDelNodes = TRUE );
    void SetNumber( const USHORT nNumber, const String* = 0 );
    void CopyFtn( SwTxtFtn *pDest );

    // erfrage und setze den TxtNode Pointer
    inline const SwTxtNode& GetTxtNode() const;
    void ChgTxtNode( const SwTxtNode* pNew ) { pMyTxtNd = (SwTxtNode*)pNew; }

        // lege eine neue leere TextSection fuer diese Fussnote an
    void MakeNewTextSection( SwNodes& rNodes );

        // loesche die FtnFrame aus der Seite
    void DelFrms();
        // bedingten Absatzvorlagen checken
    void CheckCondColl();

        // fuer die Querverweise auf Fussnoten
    USHORT SetSeqRefNo();
    void SetSeqNo( USHORT n )				{ nSeqNo = n; }	// fuer die Reader
    USHORT GetSeqRefNo() const 				{ return nSeqNo; }

    static void SetUniqueSeqRefNo( SwDoc& rDoc );
};

inline const SwTxtNode& SwTxtFtn::GetTxtNode() const
{
    ASSERT( pMyTxtNd, "SwTxtFtn:: wo ist mein TextNode?" );
    return *pMyTxtNd;
}

#endif

