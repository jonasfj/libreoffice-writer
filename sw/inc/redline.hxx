/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: redline.hxx,v $
 * $Revision: 1.14 $
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
#ifndef _REDLINE_HXX
#define _REDLINE_HXX

#include <tools/datetime.hxx>
#include <tools/string.hxx>

#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>
#include <pam.hxx>

#ifndef IDOCUMENTREDLINEACCESS_HXX_INCLUDED
#include <IDocumentRedlineAccess.hxx> 
#endif

class SfxItemSet;

class SwRedlineExtraData
{
    SwRedlineExtraData( const SwRedlineExtraData& );
    SwRedlineExtraData& operator=( const SwRedlineExtraData& );

protected:
    SwRedlineExtraData() {}

public:
    virtual ~SwRedlineExtraData();
    virtual SwRedlineExtraData* CreateNew() const = 0;

    virtual void Accept( SwPaM& rPam ) const;
    virtual void Reject( SwPaM& rPam ) const;
    virtual int operator == ( const SwRedlineExtraData& ) const;
};

class SwRedlineExtraData_FmtColl : public SwRedlineExtraData
{
    String sFmtNm;
    SfxItemSet* pSet;
    USHORT nPoolId;
public:
    SwRedlineExtraData_FmtColl( const String& rColl, USHORT nPoolFmtId,
                                const SfxItemSet* pSet = 0 );
    virtual ~SwRedlineExtraData_FmtColl();
    virtual SwRedlineExtraData* CreateNew() const;
    virtual void Reject( SwPaM& rPam ) const;
    virtual int operator == ( const SwRedlineExtraData& ) const;

    void SetItemSet( const SfxItemSet& rSet );
};

class SwRedlineExtraData_Format : public SwRedlineExtraData
{
    SvUShorts aWhichIds;

    SwRedlineExtraData_Format( const SwRedlineExtraData_Format& rCpy );

public:
    SwRedlineExtraData_Format( const SfxItemSet& rSet );
    virtual ~SwRedlineExtraData_Format();
    virtual SwRedlineExtraData* CreateNew() const;
    virtual void Reject( SwPaM& rPam ) const;
    virtual int operator == ( const SwRedlineExtraData& ) const;
};


class SwRedlineData
{
    friend class SwRedline;
    SwRedlineData* pNext;		// Verweis auf weitere Daten
    SwRedlineExtraData* pExtraData;

    String sComment;
    DateTime aStamp;
    RedlineType_t eType;
    USHORT nAuthor, nSeqNo;

public:
    SwRedlineData( RedlineType_t eT, USHORT nAut );
    SwRedlineData( const SwRedlineData& rCpy, BOOL bCpyNext = TRUE );

    // fuer sw3io: pNext/pExtraData gehen in eigenen Besitz ueber!
    SwRedlineData( RedlineType_t eT, USHORT nAut, const DateTime& rDT,
                   const String& rCmnt, SwRedlineData* pNxt,
                    SwRedlineExtraData* pExtraData = 0 );

    ~SwRedlineData();

    int operator==( const SwRedlineData& rCmp ) const
        {
            return nAuthor == rCmp.nAuthor &&
                    eType == rCmp.eType &&
                    sComment == rCmp.sComment &&
                    (( !pNext && !rCmp.pNext ) ||
                        ( pNext && rCmp.pNext && *pNext == *rCmp.pNext )) &&
                    (( !pExtraData && !rCmp.pExtraData ) ||
                        ( pExtraData && rCmp.pExtraData &&
                            *pExtraData == *rCmp.pExtraData ));
        }
    int operator!=( const SwRedlineData& rCmp ) const
        {	return !operator==( rCmp ); }

    RedlineType_t GetType() const
  { return ((RedlineType_t)(eType & nsRedlineType_t::REDLINE_NO_FLAG_MASK)); }
    RedlineType_t GetRealType() const { return eType; }
    USHORT GetAuthor() const 				{ return nAuthor; }
    const String& GetComment() const 		{ return sComment; }
    const DateTime& GetTimeStamp() const 	{ return aStamp; }
    inline const SwRedlineData* Next() const{ return pNext;	}

    void SetComment( const String& rS )     { sComment = rS; }
    void SetTimeStamp( const DateTime& rDT ) { aStamp = rDT; }
    
    void SetAutoFmtFlag()
  { eType = (RedlineType_t)(eType | nsRedlineType_t::REDLINE_FORM_AUTOFMT); }
    int CanCombine( const SwRedlineData& rCmp ) const
        {
            return nAuthor == rCmp.nAuthor &&
                    eType == rCmp.eType &&
                    sComment == rCmp.sComment &&
                    GetTimeStamp() == rCmp.GetTimeStamp() &&
                    (( !pNext && !rCmp.pNext ) ||
                        ( pNext && rCmp.pNext &&
                        pNext->CanCombine( *rCmp.pNext ))) &&
                    (( !pExtraData && !rCmp.pExtraData ) ||
                        ( pExtraData && rCmp.pExtraData &&
                            *pExtraData == *rCmp.pExtraData ));
        }

    // ExtraData wird kopiert, der Pointer geht also NICHT in den Besitz
    // des RedlineObjectes!
    void SetExtraData( const SwRedlineExtraData* pData );
    const SwRedlineExtraData* GetExtraData() const { return pExtraData; }

    // fuers UI-seitige zusammenfassen von Redline-Actionen. Wird z.Z. nur
    // fuers Autoformat mit Redline benoetigt. Der Wert != 0 bedeutet dabei,
    // das es noch weitere geben kann!
    USHORT GetSeqNo() const 					{ return nSeqNo; }
    void SetSeqNo( USHORT nNo ) 				{ nSeqNo = nNo; }

    String GetDescr() const;
};


class SwRedline : public SwPaM
{
    SwRedlineData* pRedlineData;
    SwNodeIndex* pCntntSect;
    BOOL bDelLastPara : 1;
    BOOL bIsLastParaDelete : 1;
    BOOL bIsVisible : 1;

    void MoveToSection();
    void CopyToSection();
    void DelCopyOfSection();
    void MoveFromSection();

public:
    SwRedline( RedlineType_t eType, const SwPaM& rPam );
    SwRedline( const SwRedlineData& rData, const SwPaM& rPam );
    SwRedline( const SwRedlineData& rData, const SwPosition& rPos );
    // fuer sw3io: pData geht in eigenen Besitz ueber!
    SwRedline(SwRedlineData* pData, const SwPosition& rPos, BOOL bVsbl,
               BOOL bDelLP, BOOL bIsPD) :
        SwPaM( rPos ), pRedlineData( pData ), pCntntSect( 0 ),
        bDelLastPara( bDelLP ), bIsLastParaDelete( bIsPD ), bIsVisible( bVsbl )
    {}
    SwRedline( const SwRedline& );
    virtual ~SwRedline();

    SwNodeIndex* GetContentIdx() const { return pCntntSect; }
    // fuers Undo
    void SetContentIdx( const SwNodeIndex* );

    BOOL IsVisible() const { return bIsVisible; }
    BOOL IsDelLastPara() const { return bDelLastPara; }

    // das BOOL besagt, ob nach dem setzen der Pos kein Bereich mehr
    // aufgespannt ist. -> TRUE, ansonten Bereich und FALSE
    void SetStart( const SwPosition& rPos, SwPosition* pSttPtr = 0 )
    {
        if( !pSttPtr ) pSttPtr = Start();
        *pSttPtr = rPos;
    }
    void SetEnd( const SwPosition& rPos, SwPosition* pEndPtr = 0 )
    {
        if( !pEndPtr ) pEndPtr = End();
        *pEndPtr = rPos;
    }
    // liegt eine gueltige Selektion vor?
    BOOL HasValidRange() const;

    const SwRedlineData& GetRedlineData(USHORT nPos = 0) const;
    int operator==( const SwRedlineData& rCmp ) const
        { return *pRedlineData == rCmp; }
    int operator!=( const SwRedlineData& rCmp ) const
        { return *pRedlineData != rCmp; }
    void SetAutoFmtFlag()				{ pRedlineData->SetAutoFmtFlag(); }

    USHORT GetStackCount() const;
    USHORT GetAuthor( USHORT nPos = 0) const;
    const String& GetAuthorString( USHORT nPos = 0 ) const;
    const DateTime& GetTimeStamp( USHORT nPos = 0) const;
    RedlineType_t GetRealType( USHORT nPos = 0 ) const;
    RedlineType_t GetType( USHORT nPos = 0) const
  { return ( (RedlineType_t)(GetRealType( nPos ) & nsRedlineType_t::REDLINE_NO_FLAG_MASK)); }
    const String& GetComment( USHORT nPos = 0 ) const;

    void SetComment( const String& rS ) { pRedlineData->SetComment( rS ); }

    // ExtraData wird kopiert, der Pointer geht also NICHT in den Besitz
    // des RedlineObjectes!
    void SetExtraData( const SwRedlineExtraData* pData )
        { pRedlineData->SetExtraData( pData ); }
    const SwRedlineExtraData* GetExtraData() const
        { return pRedlineData->GetExtraData(); }

    // fuers UI-seitige zusammenfassen von Redline-Actionen. Wird z.Z. nur
    // fuers Autoformat mit Redline benoetigt. Der Wert != 0 bedeutet dabei,
    // das es noch weitere geben kann!
    USHORT GetSeqNo() const 			{ return pRedlineData->GetSeqNo(); }
    void SetSeqNo( USHORT nNo ) 		{ pRedlineData->SetSeqNo( nNo ); }

    // Beim Hide/ShowOriginal wird 2 mal ueber die Liste gelaufen, damit
    //	die Del-Redlines per Copy und Delete versteckt werden. Beim Move
    //	wird sonst die Attributierung falsch behandelt.
    // Alle anderen Aufrufer muessen immer 0 angeben.
    void CallDisplayFunc( USHORT nLoop = 0 );
    void Show( USHORT nLoop = 0 );
    void Hide( USHORT nLoop = 0 );
    void ShowOriginal( USHORT nLoop = 0 );

    // calculates the intersection with text node number nNdIdx
    void CalcStartEnd( ULONG nNdIdx, USHORT& nStart, USHORT& nEnd ) const;

    void InvalidateRange();     // das Layout anstossen

    BOOL IsOwnRedline( const SwRedline& rRedl ) const
        { return GetAuthor() == rRedl.GetAuthor(); }
    BOOL CanCombine( const SwRedline& rRedl ) const;

    void PushData( const SwRedline& rRedl, BOOL bOwnAsNext = TRUE );
    BOOL PopData();

    // #111827#
    /**
       Returns textual description of this a redline data element of
       this redline.

       @param nPos index of the redline data element to describe

       The textual description of the selected element contains the
       kind of redline and the possibly shortened text of the redline.

       @return textual description of the selected redline data element
     */
    String GetDescr(USHORT nPos = 0);

    int operator==( const SwRedline& ) const;
    int operator<( const SwRedline& ) const;
};

#endif
