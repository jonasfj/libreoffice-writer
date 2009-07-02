/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: txttxmrk.hxx,v $
 * $Revision: 1.4 $
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
#ifndef _TXTTXMRK_HXX
#define _TXTTXMRK_HXX

#include <txatbase.hxx>

class SwTxtNode;
class SwDoc;

// SWTXT_TOXMARK *******************************
// Attribut fuer die Verzeichnisse

class SwTxtTOXMark : public SwTxtAttrEnd
{
    SwTxtNode * m_pTxtNode;
    xub_StrLen * m_pEnd;   // 0 if SwTOXMark without AlternativeText

public:
    SwTxtTOXMark( SwTOXMark& rAttr, xub_StrLen nStart, xub_StrLen * pEnd = 0 );
    virtual ~SwTxtTOXMark();

    virtual xub_StrLen *GetEnd();     // SwTxtAttr
    inline const xub_StrLen* GetEnd() const { return m_pEnd; }

    void CopyTOXMark( SwDoc* pDestDoc );

    // get and set TxtNode pointer
    inline const SwTxtNode* GetpTxtNd() const { return m_pTxtNode; }
    inline const SwTxtNode& GetTxtNode() const;
    void ChgTxtNode( SwTxtNode* pNew ) { m_pTxtNode = pNew; }
};

inline const SwTxtNode& SwTxtTOXMark::GetTxtNode() const
{
    ASSERT( m_pTxtNode, "SwTxtTOXMark: where is my TxtNode?" );
    return *m_pTxtNode;
}

#endif

