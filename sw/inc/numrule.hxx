/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: numrule.hxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:03:31 $
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
#ifndef _NUMRULE_HXX
#define _NUMRULE_HXX

#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _SV_GEN_HXX //autogen wg. Size
#include <tools/gen.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _SVX_SVXENUM_HXX //autogen
#include <svx/svxenum.hxx>
#endif
#ifndef _SVX_NUMITEM_HXX
#include <svx/numitem.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx> 		// Fuer die inline-ASSERTs
#endif
#ifndef _SWERROR_H
#include <error.h>			// Fuer die inline-ASSERTs
#endif
#ifndef _SW_BIT_ARRAY_HXX
#include <SwBitArray.hxx> // #i27615#
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#include <hash_map>
#include <stringhash.hxx>

class Font;
class SvxBrushItem;
class SvxNumRule;
class SwCharFmt;
class SwDoc;
class SwFmtVertOrient;
class SwNodeNum;
class SwTxtNode;

extern char __FAR_DATA sOutlineStr[];	// SWG-Filter

BYTE SW_DLLPUBLIC GetRealLevel( const BYTE nLvl );

BOOL SW_DLLPUBLIC IsNum( BYTE nLvl );

BOOL SW_DLLPUBLIC IsShowNum( BYTE nLvl );

void SW_DLLPUBLIC SetNoNum( BYTE * nLvl, BOOL nVal = TRUE );

void SW_DLLPUBLIC SetLevel( BYTE * nLvl, BYTE nNewLvl);

const sal_Unicode cBulletChar	= 0x2022;	// Charakter fuer Aufzaehlungen

class SW_DLLPUBLIC SwNumFmt : public SvxNumberFormat, public SwClient
{
    SwFmtVertOrient* pVertOrient;

    SW_DLLPRIVATE void UpdateNumNodes( SwDoc* pDoc );
    SW_DLLPRIVATE virtual void NotifyGraphicArrived();

public:
    SwNumFmt();
    SwNumFmt( const SwNumFmt& );
    SwNumFmt( const SvxNumberFormat&, SwDoc* pDoc);

    virtual ~SwNumFmt();

    SwNumFmt& operator=( const SwNumFmt& );
    BOOL operator==( const SwNumFmt& ) const;
    BOOL operator!=( const SwNumFmt& r ) const { return !(*this == r); }

    const Graphic* GetGraphic() const;

    SwCharFmt* GetCharFmt() const { return (SwCharFmt*)pRegisteredIn; }
    void SetCharFmt( SwCharFmt* );
    virtual void Modify( SfxPoolItem* pOld, SfxPoolItem* pNew );

    virtual void			SetCharFmtName(const String& rSet);
    virtual const String&	GetCharFmtName()const;

    virtual void	SetGraphicBrush( const SvxBrushItem* pBrushItem, const Size* pSize = 0, const SvxFrameVertOrient* pOrient = 0);

    virtual void				SetVertOrient(SvxFrameVertOrient eSet);
    virtual SvxFrameVertOrient 	GetVertOrient() const;
    const SwFmtVertOrient*      GetGraphicOrientation() const;

    BOOL IsEnumeration() const; // #i22362#
    BOOL IsItemize() const; // #i29560#
};

enum SwNumRuleType { OUTLINE_RULE = 0, NUM_RULE = 1, RULE_END = 2 };
class SW_DLLPUBLIC SwNumRule
{
    friend void _FinitCore();

#ifndef PRODUCT
    long int nSerial;
    static long int nInstances;
#endif

    static SwNumFmt* aBaseFmts [ RULE_END ][ MAXLEVEL ];
    static USHORT aDefNumIndents[ MAXLEVEL ];
    static USHORT nRefCount;
    static Font* pDefBulletFont;
    static char* pDefOutlineName;

    SwNumFmt* aFmts[ MAXLEVEL ];

    /**
       cache for associated text nodes
    */
    SwTxtNodeTable * pList;
    
    /**
       marked levels
     */
    SwBitArray aMarkedLevels;

    // #i36749#
    /**
       hash_map containing "name->rule" relation
     */
    std::hash_map<String, SwNumRule *, StringHash> * pNumRuleMap;

    String sName;
    SwNumRuleType eRuleType;
    USHORT nPoolFmtId;		// Id-fuer "automatich" erzeugte NumRules
    USHORT nPoolHelpId;		// HelpId fuer diese Pool-Vorlage
    BYTE nPoolHlpFileId; 	// FilePos ans Doc auf die Vorlagen-Hilfen
    BOOL bAutoRuleFlag : 1;
    BOOL bInvalidRuleFlag : 1;
    BOOL bContinusNum : 1;	// Fortlaufende Numerierung - ohne Ebenen
    BOOL bAbsSpaces : 1;	// die Ebenen repraesentieren absol. Einzuege

    SW_DLLPRIVATE static void _MakeDefBulletFont();

    // forbidden and not implemented.
    SwNumRule();

public:
    // single argument constructors shall be explicit.
    explicit SwNumRule( const String& rNm, SwNumRuleType = NUM_RULE,
                BOOL bAutoFlg = TRUE );

    SwNumRule( const SwNumRule& );
    ~SwNumRule();

    SwNumRule& operator=( const SwNumRule& );
    BOOL operator==( const SwNumRule& ) const;
    BOOL operator!=( const SwNumRule& r ) const { return !(*this == r); }

    const SwNumFmt* GetNumFmt( USHORT i ) const;
    const SwNumFmt& Get( USHORT i ) const;

    void Set( USHORT i, const SwNumFmt* );
    void Set( USHORT i, const SwNumFmt& );
    String MakeNumString( const SwNodeNum&, BOOL bInclStrings = TRUE,
                            BOOL bOnlyArabic = FALSE ) const;

    /**
       Returns list of associated text nodes.

       @return list of associated text nodes, or NULL if none present
    */
    const SwTxtNodeTable * GetList() const { return pList; }

    /**
       Set list of associated text nodes.

       @param _pList  the list of associated text nodes
    */
    void SetList(SwTxtNodeTable * _pList);

    // #i36749#
    /**
       Register this rule in a "name->numrule" map.

       @param pNumRuleMap      map to register in
     */
    void SetNumRuleMap(std::hash_map<String, SwNumRule *, StringHash> * 
                       pNumRuleMap);
    
    static const Font& GetDefBulletFont();

    static char* GetOutlineRuleName() { return pDefOutlineName; }

    static USHORT GetNumIndent( BYTE nLvl );
    static USHORT GetBullIndent( BYTE nLvl );

    SwNumRuleType GetRuleType() const 			{ return eRuleType; }
    void SetRuleType( SwNumRuleType eNew ) 		{ eRuleType = eNew;
                                                  bInvalidRuleFlag = TRUE; }

    // eine Art Copy-Constructor, damit die Num-Formate auch an den
    // richtigen CharFormaten eines Dokumentes haengen !!
    // (Kopiert die NumFormate und returnt sich selbst)
    SwNumRule& CopyNumRule( SwDoc*, const SwNumRule& );

    // testet ob die CharFormate aus dem angegeben Doc sind und kopiert
    // die gegebenfalls
    void CheckCharFmts( SwDoc* pDoc );

#ifndef NUM_RELSPACE
    // test ob der Einzug von dieser Numerierung kommt.
    BOOL IsRuleLSpace( SwTxtNode& rNd ) const;
#endif

    const String& GetName() const 		{ return sName; }
    void SetName( const String& rNm ); // #i36749#

    BOOL IsAutoRule() const 			{ return bAutoRuleFlag; }
    void SetAutoRule( BOOL bFlag )		{ bAutoRuleFlag = bFlag; }

    BOOL IsInvalidRule() const 			{ return bInvalidRuleFlag; }
    void SetInvalidRule( BOOL bFlag );	

    BOOL IsContinusNum() const 			{ return bContinusNum; }
    void SetContinusNum( BOOL bFlag )	{ bContinusNum = bFlag; }

    BOOL IsAbsSpaces() const 			{ return bAbsSpaces; }
    void SetAbsSpaces( BOOL bFlag )		{ bAbsSpaces = bFlag; }

    // #115901#
    BOOL IsOutlineRule() const { return eRuleType == OUTLINE_RULE; }

    // erfragen und setzen der Poolvorlagen-Id's
    USHORT GetPoolFmtId() const			{ return nPoolFmtId; }
    void SetPoolFmtId( USHORT nId ) 	{ nPoolFmtId = nId; }

    // erfragen und setzen der Hilfe-Id's fuer die Document-Vorlagen
    USHORT GetPoolHelpId() const 		{ return nPoolHelpId; }
    void SetPoolHelpId( USHORT nId ) 	{ nPoolHelpId = nId; }
    BYTE GetPoolHlpFileId() const 		{ return nPoolHlpFileId; }
    void SetPoolHlpFileId( BYTE nId ) 	{ nPoolHlpFileId = nId; }

    /**  
        #109308# Sets adjustment in all formats of the numbering rule. 

        @param eNum adjustment to be set
    */
    void SetNumAdjust(SvxAdjust eNum);

    void		SetSvxRule(const SvxNumRule&, SwDoc* pDoc);
    SvxNumRule	MakeSvxNumRule() const;

    // -> #i27615#
    /**
       Returns if a level is marked.

       @param nLvl     level to check

       @retval TRUE    level is marked
       @retval FALSE   level is not marked
    */
    BOOL IsLevelMarked(BYTE nLvl) const { return aMarkedLevels.Get(nLvl); }

    /**
       Mark/unmark a level.

       @param nLvl     level to mark/unmark
       @param bVal     - TRUE    mark
                       - FALSE   unmark

       @return bit array in which the altered levels are marked.
    */
    SwBitArray SetLevelMarked(BYTE nLvl, BOOL bVal);

    /**
       Unmarks all levels.
    */
    void ResetMarkedLevels() { aMarkedLevels.Reset(); }
    // <- #i27615#
    // #i23726#, #i23725#
    void        Indent(short aAmount, int nLevel = -1, 
                       int nReferenceLevel = -1, BOOL bRelative = TRUE, 
                       BOOL bFirstLine = TRUE, BOOL bCheckGtZero = TRUE);
};


class SW_DLLPUBLIC SwNodeNum
{
#ifndef PRODUCT
    static long nSerial;
    long nMySerial;
#endif

    USHORT nLevelVal[ MAXLEVEL ];		// Nummern aller Levels
    USHORT nSetValue;					// vorgegeben Nummer
    BYTE nMyLevel;						// akt. Level
    BOOL bStartNum;						// Numerierung neu starten
    BOOL bContNum;                      // #111955#
                                        // TRUE -> in continuous numbering
public:
    SwNodeNum( BYTE nLevel = NO_NUMBERING, USHORT nSetVal = USHRT_MAX );
    SwNodeNum& operator=( const SwNodeNum& rCpy );

    BOOL operator==( const SwNodeNum& ) const;

    BYTE GetLevel() const 					{ return nMyLevel; }
    void SetLevel( BYTE nVal )  			{ ::SetLevel(&nMyLevel, nVal); }

    BOOL IsStart() const					{ return bStartNum; }
    void SetStart( BOOL bFlag = TRUE ) 		{ bStartNum = bFlag; }

    // -> #111955#
    BOOL IsContinuousNum() const { return bContNum; }
    void SetContinuousNum( BOOL bFlag = TRUE ) { bContNum = bFlag; }
    // <- #111955#

    BOOL HasSetValue() const { return USHRT_MAX != nSetValue; } 
    USHORT GetSetValue() const 				{ return nSetValue; }
    void SetSetValue( USHORT nVal )  		{ nSetValue = nVal; }

    const USHORT* GetLevelVal() const 		{ return nLevelVal; }
          USHORT* GetLevelVal() 	 		{ return nLevelVal; }

    BYTE GetRealLevel() const { return ::GetRealLevel(nMyLevel); }
    BOOL IsNum() const { return ::IsNum(nMyLevel); }
    BOOL IsShowNum() const { return ::IsShowNum(nMyLevel); }

    void SetNoNum(BOOL nVal = TRUE) 
    { 
        nMyLevel = nVal ? nMyLevel | NO_NUMLEVEL : nMyLevel & ~NO_NUMLEVEL; 
    }
        
};

sal_Unicode GetBulletChar(BYTE nLevel);

#endif	// _NUMRULE_HXX
