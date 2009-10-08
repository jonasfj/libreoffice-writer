/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: txtinet.hxx,v $
 * $Revision: 1.7.214.1 $
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
#ifndef _TXTINET_HXX
#define _TXTINET_HXX
#include <txatbase.hxx>
#include <calbck.hxx>

class SwTxtNode;
class SwCharFmt;
class Color;

// ATT_INETFMT *********************************************

class SW_DLLPUBLIC SwTxtINetFmt : public SwTxtAttrEnd, public SwClient
{
    SwTxtNode* pMyTxtNd;
    BOOL bVisited		: 1; // Besuchter Link?
    BOOL bValidVis		: 1; // Ist das bVisited-Flag gueltig?
    BOOL bColor         : 1;

    // forbidden and not implemented.
    SwTxtINetFmt();

public:
    SwTxtINetFmt( const SwFmtINetFmt& rAttr, xub_StrLen nStart, xub_StrLen nEnd );
    virtual ~SwTxtINetFmt();
    TYPEINFO();

    virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew);
    virtual BOOL GetInfo( SfxPoolItem& rInfo ) const;

    // erfrage und setze den TxtNode Pointer
    const SwTxtNode* GetpTxtNode() const { return pMyTxtNd; }
    inline const SwTxtNode& GetTxtNode() const;
    void ChgTxtNode( const SwTxtNode* pNew ) { pMyTxtNd = (SwTxtNode*)pNew; }

          SwCharFmt* GetCharFmt();
    const SwCharFmt* GetCharFmt() const
            { return ((SwTxtINetFmt*)this)->GetCharFmt(); }

    BOOL IsVisited() const { return bVisited; }
    void SetVisited( BOOL bNew ) { bVisited = bNew; }

    BOOL IsValidVis() const { return bValidVis; }
    void SetValidVis( BOOL bNew ) { bValidVis = bNew; }

    BOOL IsProtect() const;
};
inline const SwTxtNode& SwTxtINetFmt::GetTxtNode() const
{
    ASSERT( pMyTxtNd, "SwTxtINetFmt:: wo ist mein TextNode?" );
    return *pMyTxtNd;
}

#endif

