/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ndhints.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2006-12-01 15:33:12 $
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
#ifndef _NDHINTS_HXX
#define _NDHINTS_HXX


#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif

#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "numrule.hxx"

class SwTxtNode;
class SwRegHistory;                 // steht im RolBck.hxx
class SwTxtAttr;

/*
 * Ableitung der Klasse SwpHints ueber den Umweg ueber SwpHts, da
 * lediglich die Klasse SwTxtNode Attribute einfuegen und
 * loeschen koennen soll. Anderen Klassen wie den Frames steht
 * lediglich ein lesender Zugriff ueber den Index-Operator zur
 * Verfuegung.
 * Groesse beim Anlegen gleich 1, weil nur dann ein Array erzeug wird, wenn
 * auch ein Hint eingefuegt wird.
 */

/*************************************************************************
 *                      class SwpHtStart/End
 *************************************************************************/

SV_DECL_PTRARR_SORT(SwpHtStart,SwTxtAttr*,1,1)
SV_DECL_PTRARR_SORT(SwpHtEnd,SwTxtAttr*,1,1)

/*************************************************************************
 *                      class SwpHintsArr
 *************************************************************************/

// Das neue Hintsarray:
class SwpHintsArr : private SwpHtStart
{

protected:
    SwpHtEnd aHtEnd;

public:
    void Insert( const SwTxtAttr *pHt );
    void DeleteAtPos( const USHORT nPosInStart );
    BOOL Resort();
    SwTxtAttr *Cut( const USHORT nPosInStart );

    inline const SwTxtAttr *GetStart( const USHORT nPos ) const { return (*this)[nPos]; }
    inline const SwTxtAttr *GetEnd( const USHORT nPos ) const { return aHtEnd[nPos]; }
    inline SwTxtAttr *GetStart( const USHORT nPos ) { return GetHt(nPos); }
    inline SwTxtAttr *GetEnd( const USHORT nPos ) { return aHtEnd[nPos]; }
    inline USHORT GetEndCount() const { return aHtEnd.Count(); }
    inline USHORT GetStartCount() const { return Count(); }
    inline USHORT GetStartOf( const SwTxtAttr *pHt ) const;
    inline USHORT GetPos( const SwTxtAttr *pHt ) const
//	OS: in svmem.hxx wird fuer TCPP GetPos ohne const gerufen
#ifdef TCPP
        { return SwpHtStart::GetPos( (SwTxtAttr *)pHt ); }
#else
        { return SwpHtStart::GetPos( pHt ); }
#endif

    inline SwTxtAttr *GetHt( const USHORT nIdx ) { return SwpHtStart::operator[](nIdx); }
    inline const SwTxtAttr *operator[]( const USHORT nIdx )const
        { return SwpHtStart::operator[](nIdx); }
    inline USHORT Count() const { return SwpHtStart::Count(); }

#ifndef PRODUCT
    BOOL Check() const;
#endif
};

/*************************************************************************
 *                      class SwpHints
 *************************************************************************/

// Die public-Schnittstelle zum Node hin
class SwpHints: public SwpHintsArr
{
private:
    SwRegHistory* pHistory;
    // Numerierung
    BOOL    bHasHiddenParaField    :1;  // HiddenParaFld
    BOOL    bFntChg :1;  // Fontwechsel
    BOOL    bFtn    :1;  // Fussnoten
    BOOL 	bInSplitNode: 1;	// TRUE: der Node ist im Split und die Frames
                                // werden verschoben!
    BOOL	bDDEFlds : 1;		// es sind DDE-Felder am TextNode vorhanden
    BOOL    bCalcHiddenParaField : 1; // bHasHiddenParaField ist invalid, CalcHiddenParaField() rufen

    BOOL Resort( const USHORT nPos );
    // Haelt ein neues Attribut in pHistory fest.
    void NoteInHistory( SwTxtAttr *pAttr, const BOOL bNew = FALSE );

    void CalcFlags( );

    // die Delete Methoden duerfen nur vom TextNode gerufen werden !!
    // Dieser sorgt dafuer, das bei Attributen ohne Ende auch das
    // entsp. Zeichen entfernt wird !!
    friend class SwTxtNode;
    void DeleteAtPos( const USHORT nPos );
    // Ist der Hint schon bekannt, dann suche die Position und loesche ihn.
    // Ist er nicht im Array, so gibt es ein ASSERT !!
    void Delete( SwTxtAttr* pTxtHt );

    inline void SetCalcHiddenParaField(){ bCalcHiddenParaField = TRUE; }
    inline void SetHiddenParaField( const BOOL bNew )  { bHasHiddenParaField = bNew; }
    inline BOOL HasHiddenParaField() const
        { if( bCalcHiddenParaField ) ((SwpHints*)this)->CalcHiddenParaField(); return bHasHiddenParaField; }

    void BuildPortions( SwTxtNode& rNode, SwTxtAttr& rNewHint, USHORT nMode );
    bool MergePortions( SwTxtNode& rNode );

public:
    inline BOOL CanBeDeleted() const { return !Count(); }

    // damit das SwDoc::Undo ueber alle Attributaenderungen informiert
    // wird, gibt es hier einen Pointer auf die History. Nur wenn dieser
    // != 0 ist, muessen alle Attributaenderungen "gemeldet" werden.
    void Register( SwRegHistory* pHist ) { pHistory = pHist; }
    void DeRegister() { Register(0); }

    void Insert( SwTxtAttr*  pHt,      SwTxtNode &rNode, USHORT nMode = 0 );

    inline BOOL HasFtn() const { return bFtn; }
    inline BOOL IsInSplitNode() const { return bInSplitNode; }

    // Konstruktor  (wird im nur einmal benutzt!)
    SwpHints()
    {
        pHistory = 0;
        bFntChg = TRUE;
        bDDEFlds = bFtn = bInSplitNode = bCalcHiddenParaField = bHasHiddenParaField = FALSE;
    }

    // Berechnung von bHasHiddenParaField, return-Wert TRUE bei Aenderung
    BOOL CalcHiddenParaField();

    DECL_FIXEDMEMPOOL_NEWDEL(SwpHints)
};

// Ausgabeoperator fuer die Texthints
SvStream &operator<<(SvStream &aS, const SwpHints &rHints); //$ ostream

/*************************************************************************
 *						 Inline-Implementationen
 *************************************************************************/

inline USHORT SwpHintsArr::GetStartOf( const SwTxtAttr *pHt ) const
{
    USHORT nPos;
    if( !Seek_Entry( pHt, &nPos ) )
        nPos = USHRT_MAX;
    return nPos;
}

inline SwTxtAttr *SwpHintsArr::Cut( const USHORT nPosInStart )
{
    SwTxtAttr *pHt = GetHt(nPosInStart);
    DeleteAtPos( nPosInStart );
    return pHt;
}


#endif
