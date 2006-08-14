/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sectfrm.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 16:22:08 $
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
#ifndef _SECTFRM_HXX
#define _SECTFRM_HXX
#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "layfrm.hxx"
#include "flowfrm.hxx"

class SwSection;
class SwSectionFmt;
class SwAttrSetChg;
class SwFtnContFrm;
class SwLayouter;

#define FINDMODE_ENDNOTE 1
#define FINDMODE_LASTCNT 2
#define FINDMODE_MYLAST  4

class SwSectionFrm: public SwLayoutFrm, public SwFlowFrm
{
    SwSection* pSection;

    void _UpdateAttr( SfxPoolItem*, SfxPoolItem*, BYTE &,
                      SwAttrSetChg *pa = 0, SwAttrSetChg *pb = 0 );
    void _Cut( BOOL bRemove );
    // Is there a FtnContainer?
    // An empty sectionfrm without FtnCont is superfluous
    BOOL IsSuperfluous() const { return !ContainsAny() && !ContainsFtnCont(); }
    void CalcFtnAtEndFlag();
    void CalcEndAtEndFlag();
    const SwSectionFmt* _GetEndSectFmt() const;
    BOOL IsEndnoteAtMyEnd() const;
protected:
    virtual void MakeAll();
    virtual BOOL ShouldBwdMoved( SwLayoutFrm *pNewUpper, BOOL bHead, BOOL &rReformat );
    virtual void Format( const SwBorderAttrs *pAttrs = 0 );
public:
    SwSectionFrm( SwSection & );				 //Inhalt wird nicht erzeugt!
    SwSectionFrm( SwSectionFrm &, BOOL bMaster );//_Nur_ zum Erzeugen von Master/Follows
    ~SwSectionFrm();

    void Init();
    virtual void  CheckDirection( BOOL bVert );

    virtual void Cut();
    virtual void Paste( SwFrm* pParent, SwFrm* pSibling = 0 );
    virtual	void Modify( SfxPoolItem*, SfxPoolItem* );

    inline const SwSectionFrm *GetFollow() const;
    inline		 SwSectionFrm *GetFollow();
    SwSectionFrm* FindMaster() const;

                 SwCntntFrm *FindLastCntnt( BYTE nMode = 0 );
    inline const SwCntntFrm *FindLastCntnt( BYTE nMode = 0 ) const;
    inline SwSection* GetSection() { return pSection; }
    inline const SwSection* GetSection() const { return pSection; }
    inline void ColLock()		{ bColLocked = TRUE; }
    inline void ColUnlock()   	{ bColLocked = FALSE; }

    void CalcFtnCntnt();
    void SimpleFormat();
    BOOL IsDescendantFrom( const SwSectionFmt* pSect ) const;
    BOOL HasToBreak( const SwFrm* pFrm ) const;
    void MergeNext( SwSectionFrm* pNxt );
    //Zerlegt den pFrm umgebenden SectionFrm in zwei Teile,
    //pFrm an den Anfang des 2. Teils
    BOOL SplitSect( SwFrm* pFrm, BOOL bApres );
    void DelEmpty( BOOL bRemove ); 	// wie Cut(), Follow-Verkettung wird aber mitgepflegt
    BOOL IsToIgnore() const			// Keine Groesse, kein Inhalt, muss ignoriert werden
    { return !Frm().Height() && !ContainsCntnt(); }
    SwFtnContFrm* ContainsFtnCont( const SwFtnContFrm* pCont = NULL ) const;
    BOOL Growable() const;
    SwTwips _Shrink( SwTwips, SZPTR BOOL bTst );
    SwTwips _Grow  ( SwTwips, SZPTR BOOL bTst );

    // A sectionfrm has to maximize, if he has a follow or a ftncontainer at
    // the end of the page. A superfluous follow will be ignored,
    // if bCheckFollow is set.
    BOOL ToMaximize( BOOL bCheckFollow ) const;
    inline BOOL _ToMaximize() const
        { if( !pSection ) return FALSE; return ToMaximize( FALSE );	}
    BOOL MoveAllowed( const SwFrm* ) const;
    BOOL CalcMinDiff( SwTwips& rMinDiff ) const;
    // Uebergibt man kein bOverSize bzw. FALSE, so ist der Returnwert
    // >0 fuer Undersized-Frames, ==0 sonst.
    // Uebergibt man TRUE, so kann es auch einen negativen Returnwert geben,
    // wenn der SectionFrm nicht vollstaendig ausgefuellt ist, was z.B. bei
    // SectionFrm mit Follow meist vorkommt. Benoetigt wird dies im
    // FormatWidthCols, um "Luft" aus den Spalten zu lassen.
    long Undersize( BOOL bOverSize = FALSE );
    // Groesse an die Umgebung anpassen
    void _CheckClipping( BOOL bGrow, BOOL bMaximize );

    void InvalidateFtnPos();
    void CollectEndnotes( SwLayouter* pLayouter );
    const SwSectionFmt* GetEndSectFmt() const
        { if( IsEndnAtEnd() ) return _GetEndSectFmt(); return NULL; }

    static void MoveCntntAndDelete( SwSectionFrm* pDel, BOOL bSave );

    bool IsBalancedSection() const;

    DECL_FIXEDMEMPOOL_NEWDEL(SwSectionFrm)
};

typedef SwSectionFrm* SwSectionFrmPtr;
SV_DECL_PTRARR_SORT( SwDestroyList, SwSectionFrmPtr, 1, 5)

inline const SwSectionFrm *SwSectionFrm::GetFollow() const
{
    return (const SwSectionFrm*)SwFlowFrm::GetFollow();
}
inline SwSectionFrm *SwSectionFrm::GetFollow()
{
    return (SwSectionFrm*)SwFlowFrm::GetFollow();
}
inline const SwCntntFrm *SwSectionFrm::FindLastCntnt( BYTE nMode ) const
{
    return ((SwSectionFrm*)this)->FindLastCntnt( nMode );
}


#endif	//_SECTFRM_HXX
