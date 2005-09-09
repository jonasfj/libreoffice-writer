/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: txtinet.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:22:49 $
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
#ifndef _TXTINET_HXX
#define _TXTINET_HXX

#ifndef _TXATBASE_HXX //autogen
#include <txatbase.hxx>
#endif
#ifndef _CALBCK_HXX //autogen
#include <calbck.hxx>
#endif

class SvxFont;
class SwTxtNode;
class SwCharFmt;
class Color;

// ATT_INETFMT *********************************************

class SwTxtINetFmt : public SwTxtAttrEnd, public SwClient
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

    const BOOL IsVisited() const { return bVisited; }
    void SetVisited( BOOL bNew ) { bVisited = bNew; }

    const BOOL IsValidVis() const { return bValidVis; }
    void SetValidVis( BOOL bNew ) { bValidVis = bNew; }

    BOOL IsProtect() const;
};
inline const SwTxtNode& SwTxtINetFmt::GetTxtNode() const
{
    ASSERT( pMyTxtNd, "SwTxtINetFmt:: wo ist mein TextNode?" );
    return *pMyTxtNd;
}

#endif

