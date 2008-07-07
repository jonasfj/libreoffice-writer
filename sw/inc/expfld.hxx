/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: expfld.hxx,v $
 * $Revision: 1.12 $
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
#ifndef _EXPFLD_HXX
#define _EXPFLD_HXX

#include <svtools/svarray.hxx>
#include "swdllapi.h"
#include <fldbas.hxx>
#include <cellfml.hxx>

class SfxPoolItem;
class SwTxtNode;
class SwFrm;
struct SwPosition;
class SwTxtFld;
class SwDoc;
class SwFmtFld;
class _SetGetExpFlds;
class SwEditShell;

// Vorwaertsdeklaration: besorge den "Body-TextNode", fuer Exp.Fld in Fly's
//						Header/Footers/Footnodes
const SwTxtNode* GetBodyTxtNode( const SwDoc& pDoc, SwPosition& rPos,
                                 const SwFrm& rFrm );
// Wandlung Address -> Adressen
void ReplacePoint(String& sTmpName, BOOL bWithCommandType = FALSE);

struct _SeqFldLstElem
{
    String sDlgEntry;
    USHORT nSeqNo;

    _SeqFldLstElem( const String& rStr, USHORT nNo )
        : sDlgEntry( rStr ), nSeqNo( nNo )
    {}
};
SV_DECL_PTRARR_DEL( _SwSeqFldList, _SeqFldLstElem*, 10, 10 )

class SW_DLLPUBLIC SwSeqFldList : public _SwSeqFldList
{
public:
    SwSeqFldList()	: _SwSeqFldList( 10, 10 ) {}

    BOOL InsertSort( _SeqFldLstElem* );
    BOOL SeekEntry( const _SeqFldLstElem& , USHORT* pPos = 0 );
};

/*--------------------------------------------------------------------
    Beschreibung: Ausdruck
 --------------------------------------------------------------------*/

class SwGetExpFieldType : public SwValueFieldType
{
public:
        SwGetExpFieldType(SwDoc* pDoc);
        virtual SwFieldType*    Copy() const;

        // ueberlagert, weil das Get-Field nicht veraendert werden kann
        // und dann auch nicht aktualisiert werden muss. Aktualisierung
        // erfolgt beim Aendern von Set-Werten !

        virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew );
};

/*--------------------------------------------------------------------
    Beschreibung: GetExperession
 --------------------------------------------------------------------*/

class SwGetExpField : public SwFormulaField
{
    String 			sExpand;
    BOOL 			bIsInBodyTxt;
    USHORT			nSubType;

    bool            bLateInitialization; // #i82544# 

public:
    SwGetExpField( SwGetExpFieldType*, const String& rFormel,
                   USHORT nSubType = nsSwGetSetExpType::GSE_EXPR, ULONG nFmt = 0);

    virtual void 				SetValue( const double& rVal );
    virtual void				SetLanguage(USHORT nLng);

    virtual String	 			Expand() const;
    virtual SwField* 			Copy() const;

    inline const String& 		GetExpStr() const;
    inline void  				ChgExpStr(const String& rExpand);

    // wird von der Formatierung abgefragt
    inline BOOL 				IsInBodyTxt() const;
    // wird von UpdateExpFlds gesetzt (dort ist die Node-Position bekannt)
    inline void 				ChgBodyTxtFlag( BOOL bIsInBody );
    // fuer Felder in Header/Footer/Footnotes/Flys:
    // (wird nur von der Formatierung aufgerufen!!)
    void 						ChangeExpansion( const SwFrm&, const SwTxtFld& );

    virtual	String				GetCntnt(BOOL bName = FALSE) const;

    // Die Formel aendern
    virtual String				GetPar2() const;
    virtual void				SetPar2(const String& rStr);

    virtual USHORT				GetSubType() const;
    virtual void				SetSubType(USHORT nType);
    virtual BOOL        QueryValue( com::sun::star::uno::Any& rVal, USHORT nWhich ) const;
    virtual BOOL        PutValue( const com::sun::star::uno::Any& rVal, USHORT nWhich );

    static USHORT 		GetReferenceTextPos( const SwFmtFld& rFmt, SwDoc& rDoc);
    // #i82544# 
    void				SetLateInitialization() { bLateInitialization = true;}
};

inline void SwGetExpField::ChgExpStr(const String& rExpand)
    { sExpand = rExpand;}

inline const String& SwGetExpField::GetExpStr() const
    { return sExpand; 	}

// wird von der Formatierung abgefragt
inline BOOL SwGetExpField::IsInBodyTxt() const
    { return bIsInBodyTxt; }

// wird von UpdateExpFlds gesetzt (dort ist die Node-Position bekannt)
inline void SwGetExpField::ChgBodyTxtFlag( BOOL bIsInBody )
    { bIsInBodyTxt = bIsInBody; }


/*--------------------------------------------------------------------
    Beschreibung: Ausdruck setzen
 --------------------------------------------------------------------*/

class SwSetExpField;

class SW_DLLPUBLIC SwSetExpFieldType : public SwValueFieldType
{
    String 		sName;
    const SwNode* pOutlChgNd;
//	sal_Unicode cDelim;
    String		sDelim;
    USHORT 		nType;
    BYTE		nLevel;
    BOOL		bDeleted;

public:
    SwSetExpFieldType( SwDoc* pDoc, const String& rName,
                        USHORT nType = nsSwGetSetExpType::GSE_EXPR );
    virtual SwFieldType*    Copy() const;
    virtual const String&	GetName() const;

    inline void				SetType(USHORT nTyp);
    inline USHORT			GetType() const;

    void					SetSeqFormat(ULONG nFormat);
    ULONG					GetSeqFormat();

    BOOL					IsDeleted() const 		{ return bDeleted; }
    void					SetDeleted( BOOL b )	{ bDeleted = b; }

    // ueberlagert, weil das Set-Field selbst dafuer sorgt, das
    // es aktualisiert wird.
    virtual void 			Modify( SfxPoolItem *pOld, SfxPoolItem *pNew );
    inline const String& 	GetSetRefName() const;

    USHORT SetSeqRefNo( SwSetExpField& rFld );

    USHORT GetSeqFldList( SwSeqFldList& rList );
    String MakeSeqName( USHORT nSeqNo );

    // Seqencefelder ggfs. Kapitelweise numerieren
//	sal_Unicode GetDelimiter() const 		{ return cDelim; }
//	void SetDelimiter( sal_Unicode c ) 		{ cDelim = c; }
    const String& GetDelimiter() const 		{ return sDelim; }
    void SetDelimiter( const String& s ) 	{ sDelim = s; }
    BYTE GetOutlineLvl() const 				{ return nLevel; }
    void SetOutlineLvl( BYTE n ) 			{ nLevel = n; }
    void SetChapter( SwSetExpField& rFld, const SwNode& rNd );
    // Member nur fuers SwDoc::UpdateExpFld - wird nur waehrend der Laufzeit
    // von SequencefeldTypen benoetigt!!!
    const SwNode* GetOutlineChgNd() const	{ return pOutlChgNd; }
    void SetOutlineChgNd( const SwNode* p )	{ pOutlChgNd = p; }

    virtual BOOL        QueryValue( com::sun::star::uno::Any& rVal, USHORT nWhich ) const;
    virtual BOOL        PutValue( const com::sun::star::uno::Any& rVal, USHORT nWhich );
};

inline void SwSetExpFieldType::SetType( USHORT nTyp )
{
        nType = nTyp;
        EnableFormat( !(nType & (nsSwGetSetExpType::GSE_SEQ|nsSwGetSetExpType::GSE_STRING)));
}

inline USHORT SwSetExpFieldType::GetType() const
    { return nType;	  }

inline const String& SwSetExpFieldType::GetSetRefName() const
    { return sName; }


/*--------------------------------------------------------------------
    Beschreibung: Ausdruck
 --------------------------------------------------------------------*/

class SwSetExpField : public SwFormulaField
{
    String 			sExpand;
    String			aPText;
    String			aSeqText;
    BOOL			bInput;
    USHORT 			nSeqNo;
    USHORT			nSubType;

public:
    SwSetExpField(SwSetExpFieldType*, const String& rFormel, ULONG nFmt = 0);

    virtual void 				SetValue( const double& rVal );

    virtual String	 			Expand() const;
    virtual SwField* 			Copy() const;

    inline const String& 		GetExpStr() const;

    inline void 				ChgExpStr( const String& rExpand );

    inline void  				SetPromptText(const String& rStr);
    inline const 				String& GetPromptText() const;

    inline void  				SetInputFlag(BOOL bInp);
    inline BOOL					GetInputFlag() const;

    virtual String              GetCntnt(BOOL bName = FALSE) const;
    virtual USHORT				GetSubType() const;
    virtual void				SetSubType(USHORT nType);

    inline BOOL 				IsSequenceFld() const;

    // fuer SequenceFelder - logische Nummer
    inline void					SetSeqNumber( USHORT n )	{ nSeqNo = n; }
    inline USHORT				GetSeqNumber() const		{ return nSeqNo; }

    // Der Name nur erfragen
    virtual const String&		GetPar1() 	const;

    // Die Formel
    virtual String				GetPar2() 	const;
    virtual void				SetPar2(const String& rStr);
    virtual BOOL        QueryValue( com::sun::star::uno::Any& rVal, USHORT nWhich ) const;
    virtual BOOL        PutValue( const com::sun::star::uno::Any& rVal, USHORT nWhich );
};

inline const String& SwSetExpField::GetExpStr() const
    { return sExpand; 		}

inline void SwSetExpField::ChgExpStr( const String& rExpand )
    { sExpand = rExpand; 	}

inline void  SwSetExpField::SetPromptText(const String& rStr)
    { aPText = rStr;  		}

inline const String& SwSetExpField::GetPromptText() const
    { return aPText;  		}

inline void SwSetExpField::SetInputFlag(BOOL bInp)
    { bInput = bInp; }

inline BOOL	SwSetExpField::GetInputFlag() const
    { return bInput; }

inline BOOL SwSetExpField::IsSequenceFld() const
    { return 0 != (nsSwGetSetExpType::GSE_SEQ & ((SwSetExpFieldType*)GetTyp())->GetType()); }

/*--------------------------------------------------------------------
    Beschreibung: Eingabe im Text/Variable setzen
 --------------------------------------------------------------------*/

class SwInputFieldType : public SwFieldType
{
    SwDoc* pDoc;
public:
    SwInputFieldType( SwDoc* pDoc );

    virtual SwFieldType* Copy() const;

    SwDoc* GetDoc() const { return pDoc; }
};

/*--------------------------------------------------------------------
    Beschreibung: Eingabefeld
 --------------------------------------------------------------------*/

class SwInputField : public SwField
{
    String  aContent;
    String	aPText;
    String  aHelp;
    String  aToolTip;
    USHORT  nSubType;
public:
    // Direkte Eingabe ueber Dialog alten Wert loeschen
    SwInputField(SwInputFieldType*, const String& rContent ,
                 const String& rPrompt, USHORT nSubType = 0,
                 ULONG nFmt = 0);

    virtual	String			GetCntnt(BOOL bName = FALSE) const;
    virtual String	 		Expand() const;
    virtual SwField* 		Copy() const;

    // Content
    virtual const String&	GetPar1() const;
    virtual void			SetPar1(const String& rStr);

    // aPromptText
    virtual String			GetPar2() const;
    virtual void			SetPar2(const String& rStr);

    virtual String          GetHelp() const;
    virtual void            SetHelp(const String & rStr);

    virtual String          GetToolTip() const;
    virtual void            SetToolTip(const String & rStr);

    virtual BOOL            isFormField() const;

    virtual USHORT			GetSubType() const;
    virtual void			SetSubType(USHORT nSub);
    virtual BOOL        QueryValue( com::sun::star::uno::Any& rVal, USHORT nWhich ) const;
    virtual BOOL        PutValue( const com::sun::star::uno::Any& rVal, USHORT nWhich );
};

/*--------------------------------------------------------------------
    Description: Sorted list of input fields and DropDown fields
 --------------------------------------------------------------------*/

class SwInputFieldList
{
public:
    SwInputFieldList( SwEditShell* pShell, BOOL bBuildTmpLst = FALSE );
    ~SwInputFieldList();

    USHORT		Count() const;
    SwField*    GetField(USHORT nId);

    void		GotoFieldPos(USHORT nId);
    void		PushCrsr();
    void	 	PopCrsr();

    // vergleiche TmpLst mit akt Feldern. Alle neue kommen in die SortLst
    // damit sie geupdatet werden koennen. Returnt die Anzahl.
    // (Fuer Textbausteine: nur seine Input-Felder aktualisieren)
    USHORT		BuildSortLst();

    // Alle unselektierten Felder aus Liste entfernen
    void		RemoveUnselectedFlds();

private:
    SwEditShell*	pSh;
    _SetGetExpFlds*	pSrtLst;
    SvPtrarr		aTmpLst;
};

/*--------------------------------------------------------------------
    Beschreibung: Tabellen-Formelfeld
                  (Implementierung steht in tblcalc.cxx)
 --------------------------------------------------------------------*/

class SwTblFieldType : public SwValueFieldType
{
public:
    SwTblFieldType(SwDoc* pDocPtr);
    virtual SwFieldType* Copy() const;
};


// MSC will den hier nicht
//typedef void (SwField:: *FnScanFormel)( const SwTable&, String&,
//									 String&, String* = 0, void* = 0 );


class SwTblField : public SwValueField, public SwTableFormula
{
    String 		sExpand;
    USHORT		nSubType;

    // suche den TextNode, in dem das Feld steht
    virtual const SwNode* GetNodeOfFormula() const;

public:
    SwTblField( SwTblFieldType*, const String& rFormel,
                USHORT nSubType = 0, ULONG nFmt = 0);

    virtual void 		SetValue( const double& rVal );
    virtual USHORT		GetSubType() const;
    virtual void		SetSubType(USHORT nType);
    virtual String	 	Expand() const;
    virtual SwField* 	Copy() const;

    const String& 		GetExpStr() const 				{ return sExpand; }
    void 				ChgExpStr(const String& rStr) 	{ sExpand = rStr; }

    // berechne sich selbst
    void 				CalcField( SwTblCalcPara& rCalcPara );

    virtual	String		GetCntnt(BOOL bName = FALSE) const;
    // Die Formel
    virtual String		GetPar2() 	const;
    virtual void		SetPar2(const String& rStr);
    virtual BOOL        QueryValue( com::sun::star::uno::Any& rVal, USHORT nWhich ) const;
    virtual BOOL        PutValue( const com::sun::star::uno::Any& rVal, USHORT nWhich );
};


#endif // _EXPFLD_HXX
