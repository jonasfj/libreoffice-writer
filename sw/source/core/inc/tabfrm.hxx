/*************************************************************************
 *
 *  $RCSfile: tabfrm.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: obo $ $Date: 2004-06-01 07:43:18 $
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
#ifndef _TABFRM_HXX
#define _TABFRM_HXX

#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif
#include "layfrm.hxx"
#include "flowfrm.hxx"

class SwTable;
class SwBorderAttrs;
class SwAttrSetChg;

class SwTabFrm: public SwLayoutFrm, public SwFlowFrm
{
    // OD 14.03.2003 #i11760# - adjustment, because of method signature change
    //darf mit den Flags spielen.
    friend void CalcCntnt( SwLayoutFrm *pLay, bool bNoColl, bool bNoCalcFollow );

    //Fuert Spezialbehandlung fuer _Get[Next|Prev]Leaf() durch.
    SwLayoutFrm *GetLeaf( MakePageType eMakePage, BOOL bFwd );

    SwTable *pTable;

    BOOL bComplete	  		:1;	//Eintrage als Repaint ohne das CompletePaint
                                //der Basisklasse gesetzt werden muss. Damit
                                //sollen unertraegliche Tabellen-Repaints
                                //vermieden werden.
    BOOL bCalcLowers  		:1;	//Im MakeAll auf jedenfall auch fuer Stabilitaet
                                //des Inhaltes sorgen.
    BOOL bLowersFormatted	:1;//Kommunikation zwischen MakeAll und Layact
    BOOL bLockBackMove		:1;	//BackMove-Test hat der Master erledigt.
    BOOL bResizeHTMLTable	:1;	//Resize des HTMLTableLayout rufen im MakeAll
                                //Zur Optimierung, damit dies nicht im
                                //CntntFrm::Grow gerufen werden muss, denn dann
                                //wird es ggf. fuer jede Zelle gerufen #47483#
    BOOL bONECalcLowers		:1;	//Primaer fuer die StarONE-SS. Beim MakeAll werden
                                //die Cntnts auf jedenfall per Calc() formatiert.
                                //es finden keine zusaetzlichen Invalidierungen
                                //statt und dieser Weg kann auch kaum garantien
                                //geben.

    BOOL bHasFollowFlowLine :1; // Means that the first line in the follow
                                // is indented to contain content from a broken
                                // cell
    BOOL bIsRebuildLastLine :1; // Means that currently the last line of the
                                // TabFrame is rebuilded. In this case we
                                // do not want any notification to the master
                                // table
    BOOL bRestrictTableGrowth :1;       // Usually, the table may grow infinite,
                                        // because the table can be split in
                                        // SwTabFrm::MakeAll. In MakeAll, this
                                        // flag is set to indicate that the table
                                        // may only grow inside its upper. This
                                        // is necessary, in order to let the text
                                        // flow into the FollowFlowLine
    BOOL bRemoveFollowFlowLinePending :1;
    BOOL bDummy2 :1;
    BOOL bDummy3 :1;
    BOOL bDummy4 :1;

    //Split() spaltet den Frm an der angegebenen Stelle, es wird ein
    //Follow erzeugt und aufgebaut und direkt hinter this gepastet.
    //Join() Holt sich den Inhalt aus dem Follow und vernichtet diesen.
    bool Split( const SwTwips nCutPos );
    bool Join();

    void _UpdateAttr( SfxPoolItem*, SfxPoolItem*, BYTE &,
                      SwAttrSetChg *pa = 0, SwAttrSetChg *pb = 0 );

    virtual BOOL ShouldBwdMoved( SwLayoutFrm *pNewUpper, BOOL bHead, BOOL &rReformat );

protected:
    virtual void MakeAll();
    virtual void Format( const SwBorderAttrs *pAttrs = 0 );
        //Aendert nur die Framesize, nicht die PrtArea-SSize
    virtual SwTwips GrowFrm  ( SwTwips, SZPTR
                               BOOL bTst = FALSE, BOOL bInfo = FALSE );
public:
    SwTabFrm( SwTable & );	//Immer nach dem erzeugen _und_ pasten das
                            //Regist Flys rufen!
    SwTabFrm( SwTabFrm & );	//_Nur_ zum erzeugen von Follows
    ~SwTabFrm();

    void JoinAndDelFollows();	//Fuer DelFrms des TableNodes!

    //Ruft das RegistFlys der Zeilen.
    void RegistFlys();

    inline const SwTabFrm *GetFollow() const;
    inline		 SwTabFrm *GetFollow();
    SwTabFrm* FindMaster( bool bFirstMaster = false ) const;

    virtual	void Modify( SfxPoolItem*, SfxPoolItem* );
    virtual BOOL GetInfo( SfxPoolItem &rHnt ) const;
    virtual void Paint( const SwRect& ) const;
    virtual void  CheckDirection( BOOL bVert );

    virtual void Cut();
    virtual void Paste( SwFrm* pParent, SwFrm* pSibling = 0 );

    virtual void Prepare( const PrepareHint ePrep = PREP_CLEAR,
                          const void *pVoid = 0, sal_Bool bNotify = sal_True );

                 SwCntntFrm *FindLastCntnt();
    inline const SwCntntFrm *FindLastCntnt() const;

    const SwTable *GetTable() const { return pTable; }
          SwTable *GetTable() 		{ return pTable; }

    BOOL IsComplete()  { return bComplete; }
    void SetComplete() { bComplete = TRUE; }
    void ResetComplete() { bComplete = FALSE; }

    BOOL IsLowersFormatted() const 		{ return bLowersFormatted; }
    void SetLowersFormatted( BOOL b )	{ bLowersFormatted = b;    }

    void SetCalcLowers()		{ bCalcLowers = TRUE; 	   } //Sparsam einsetzen!
    void SetResizeHTMLTable()   { bResizeHTMLTable = TRUE; } //dito
    void SetONECalcLowers()		{ bONECalcLowers = TRUE;   }

    //
    // Start: New stuff for breaking table rows
    //
    BOOL HasFollowFlowLine() const { return bHasFollowFlowLine; }
    void SetFollowFlowLine( BOOL bNew ) { bHasFollowFlowLine = bNew; }

    BOOL IsRebuildLastLine() const { return bIsRebuildLastLine; }
    void SetRebuildLastLine( BOOL bNew ) { bIsRebuildLastLine = bNew; }

    BOOL IsRestrictTableGrowth() const { return bRestrictTableGrowth; }
    void SetRestrictTableGrowth( BOOL bNew ) { bRestrictTableGrowth = bNew; }

    BOOL IsRemoveFollowFlowLinePending() const { return bRemoveFollowFlowLinePending; }
    void SetRemoveFollowFlowLinePending( BOOL bNew ) { bRemoveFollowFlowLinePending = bNew; }

    bool SwTabFrm::RemoveFollowFlowLine();
    //
    // End: New stuff for breaking table rows
    //

    BOOL CalcFlyOffsets( SwTwips& rUpper, long& rLeftOffset,
                         long& rRightOffset ) const;

    SwTwips CalcHeightOfFirstContentLine() const;
    
    bool SwTabFrm::IsInHeadline( const SwFrm& rFrm ) const;
    SwRowFrm* GetFirstNonHeadlineRow() const;

    bool IsLayoutSplitAllowed() const;

    DECL_FIXEDMEMPOOL_NEWDEL(SwTabFrm)
};

inline const SwCntntFrm *SwTabFrm::FindLastCntnt() const
{
    return ((SwTabFrm*)this)->FindLastCntnt();
}

inline const SwTabFrm *SwTabFrm::GetFollow() const
{
    return (const SwTabFrm*)SwFlowFrm::GetFollow();
}
inline SwTabFrm *SwTabFrm::GetFollow()
{
    return (SwTabFrm*)SwFlowFrm::GetFollow();
}

#endif	//_TABFRM_HXX
