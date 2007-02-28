/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rowfrm.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: vg $ $Date: 2007-02-28 15:46:00 $
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
#ifndef _ROWFRM_HXX
#define _ROWFRM_HXX

#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "layfrm.hxx"

class SwTableLine;
class SwBorderAttrs;

class SwRowFrm: public SwLayoutFrm
{
    virtual void Format( const SwBorderAttrs *pAttrs = 0 );
        //Aendern nur die Framesize, nicht die PrtArea-SSize
    virtual SwTwips ShrinkFrm( SwTwips, BOOL bTst = FALSE, BOOL bInfo = FALSE );
    virtual SwTwips GrowFrm  ( SwTwips, BOOL bTst = FALSE, BOOL bInfo = FALSE );

    const SwTableLine *pTabLine;
    SwRowFrm* pFollowRow;
    // --> collapsing borders FME 2005-05-27 #i29550#
    USHORT mnTopMarginForLowers;
    USHORT mnBottomMarginForLowers;
    USHORT mnBottomLineSize;
    // <-- collapsing
    bool bIsFollowFlowRow;
    bool bIsRepeatedHeadline;
    bool mbIsRowSpanLine;

protected:
    virtual void MakeAll();

public:
    SwRowFrm( const SwTableLine &, bool bInsertContent = true );
    ~SwRowFrm();

    virtual void Cut();
    virtual	void  Modify( SfxPoolItem*, SfxPoolItem* );

    //Zum Anmelden der Flys nachdem eine Zeile erzeugt _und_ eingefuegt wurde.
    //Muss vom Erzeuger gerufen werden, denn erst nach dem Konstruieren wird
    //Das Teil gepastet; mithin ist auch erst dann die Seite zum Anmelden der
    //Flys erreichbar.
    void RegistFlys( SwPageFrm *pPage = 0 );

    const SwTableLine *GetTabLine() const { return pTabLine; }

    //Passt die Zellen auf die aktuelle Hoehe an, invalidiert die Zellen
    //wenn die Direction nicht der Hoehe entspricht.
    void AdjustCells( const SwTwips nHeight, const BOOL bHeight );

    //
    //
    SwRowFrm* GetFollowRow() const { return pFollowRow; }
    void SetFollowRow( SwRowFrm* pNew ) { pFollowRow = pNew; }

    // --> collapsing borders FME 2005-05-27 #i29550#
    USHORT GetTopMarginForLowers() const { return mnTopMarginForLowers; }
    void   SetTopMarginForLowers( USHORT nNew ) { mnTopMarginForLowers = nNew; }
    USHORT GetBottomMarginForLowers() const { return mnBottomMarginForLowers; }
    void   SetBottomMarginForLowers( USHORT nNew ) { mnBottomMarginForLowers = nNew; }
    USHORT GetBottomLineSize() const { return mnBottomLineSize; }
    void   SetBottomLineSize( USHORT nNew ) { mnBottomLineSize = nNew; }
    // <-- collapsing

    bool IsRepeatedHeadline() const { return bIsRepeatedHeadline; }
    void SetRepeatedHeadline( bool bNew ) { bIsRepeatedHeadline = bNew; }

    // --> split table rows
    bool IsRowSplitAllowed() const;
    bool IsFollowFlowRow() const { return bIsFollowFlowRow; }
    void SetFollowFlowRow( bool bNew ) { bIsFollowFlowRow = bNew; }
    // <-- split table rows

    // --> FME 2006-02-10 #131283# Table row keep feature
    bool ShouldRowKeepWithNext() const;
    // <--

    // --> FME 2006-10-30 #i4032# NEW TABLES
    bool IsRowSpanLine() const { return mbIsRowSpanLine; }
    void SetRowSpanLine( bool bNew ) { mbIsRowSpanLine = bNew; }

    DECL_FIXEDMEMPOOL_NEWDEL(SwRowFrm)
};

#endif
