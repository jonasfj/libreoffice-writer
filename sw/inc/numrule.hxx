/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: numrule.hxx,v $
 * $Revision: 1.39.78.1 $
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
#ifndef _NUMRULE_HXX
#define _NUMRULE_HXX

#include <tools/link.hxx>
#include <tools/gen.hxx>
#include <tools/string.hxx>
#include <svx/svxenum.hxx>
#include <svx/numitem.hxx>
#include "swdllapi.h"
#include <swtypes.hxx>
#include <calbck.hxx>
#include <errhdl.hxx> 		// Fuer die inline-ASSERTs
#include <error.h>			// Fuer die inline-ASSERTs
#include <hints.hxx>
#include <hash_map>
#include <stringhash.hxx>
// --> OD 2008-02-21 #refactorlists#
class SwNodeNum;
#include <SwNumberTreeTypes.hxx>
// <--
// --> OD 2008-02-19 #refactorlists#
#include <vector>
class SwTxtFmtColl;
// <--
// --> OD 2008-07-08 #i91400#
class IDocumentListsAccess;
// <--

class Font;
class SvxBrushItem;
class SvxNumRule;
class SwCharFmt;
class SwDoc;
class SwFmtVertOrient;
class SwTxtNode;

const sal_Unicode cBulletChar	= 0x2022;	// Charakter fuer Aufzaehlungen

class SW_DLLPUBLIC SwNumFmt : public SvxNumberFormat, public SwClient
{
    SwFmtVertOrient* pVertOrient;

    SW_DLLPRIVATE void UpdateNumNodes( SwDoc* pDoc );
    SW_DLLPRIVATE virtual void NotifyGraphicArrived();

    using SvxNumberFormat::operator ==;
    using SvxNumberFormat::operator !=;

public:
    SwNumFmt();
    SwNumFmt( const SwNumFmt& );
    SwNumFmt( const SvxNumberFormat&, SwDoc* pDoc);

    virtual ~SwNumFmt();

    SwNumFmt& operator=( const SwNumFmt& );

    BOOL operator==( const SwNumFmt& ) const;
    BOOL operator!=( const SwNumFmt& r ) const { return !(*this == r); }

    SwCharFmt* GetCharFmt() const { return (SwCharFmt*)pRegisteredIn; }
    void SetCharFmt( SwCharFmt* );
    virtual void Modify( SfxPoolItem* pOld, SfxPoolItem* pNew );

    virtual void			SetCharFmtName(const String& rSet);
    virtual const String&	GetCharFmtName()const;

    virtual void    SetGraphicBrush( const SvxBrushItem* pBrushItem, const Size* pSize = 0, const sal_Int16* pOrient = 0);

    virtual void                SetVertOrient(sal_Int16 eSet);
    virtual sal_Int16   GetVertOrient() const;
    const SwFmtVertOrient*      GetGraphicOrientation() const;

    BOOL IsEnumeration() const; // #i22362#
    BOOL IsItemize() const; // #i29560#
};

class SwPaM;
enum SwNumRuleType { OUTLINE_RULE = 0, NUM_RULE = 1, RULE_END = 2 };
class SW_DLLPUBLIC SwNumRule
{
// --> OD 2008-02-19 #refactorlists#
public:
    typedef std::vector< SwTxtNode* > tTxtNodeList;
    typedef std::vector< SwTxtFmtColl* > tParagraphStyleList;
// <--
private:
    friend void _FinitCore();

#ifndef PRODUCT
    long int nSerial;
    static long int nInstances;
#endif

    static SwNumFmt* aBaseFmts [ RULE_END ][ MAXLEVEL ];
    static USHORT aDefNumIndents[ MAXLEVEL ];
    // --> OD 2008-02-11 #newlistlevelattrs#
    // default list level properties for position-and-space mode LABEL_ALIGNMENT
    static SwNumFmt* aLabelAlignmentBaseFmts [ RULE_END ][ MAXLEVEL ];
    // <--
    static USHORT nRefCount;
    static char* pDefOutlineName;

    SwNumFmt* aFmts[ MAXLEVEL ];

    /** container for associated text nodes

    */
    // --> OD 2008-02-19 #refactorlists#
//    SwTxtNodeTable* pTxtNodeList;
    tTxtNodeList maTxtNodeList;
    // <--

    /** container for associated paragraph styles

        OD 2008-03-03 #refactorlists#
    */
    tParagraphStyleList maParagraphStyleList;

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
    bool mbCountPhantoms;

    // --> OD 2008-02-11 #newlistlevelattrs#
    const SvxNumberFormat::SvxNumPositionAndSpaceMode meDefaultNumberFormatPositionAndSpaceMode;
    // <--

    // --> OD 2008-04-03 #refactorlists#
    String msDefaultListId;
    // <--

    // forbidden and not implemented.
    SwNumRule();

public:
    // --> OD 2008-02-08 #newlistlevelattrs#
    // add parameter <eDefaultNumberFormatPositionAndSpaceMode>
    SwNumRule( const String& rNm,
               const SvxNumberFormat::SvxNumPositionAndSpaceMode eDefaultNumberFormatPositionAndSpaceMode,
               SwNumRuleType = NUM_RULE,
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
    // --> OD 2005-10-17 #126238#
    // - add optional parameter <_nRestrictToThisLevel> in order to
    //   restrict returned string to this level.
    String MakeNumString( const SwNumberTree::tNumberVector & rNumVector,
                          const BOOL bInclStrings = TRUE,
                          const BOOL bOnlyArabic = FALSE,
                          const unsigned int _nRestrictToThisLevel = MAXLEVEL ) const;
    // <--
    // --> OD 2007-08-24 #i81002#
    String MakeRefNumString( const SwNodeNum& rNodeNum,
                             const bool bInclSuperiorNumLabels = false,
                             const sal_uInt8 nRestrictInclToThisLevel = 0 ) const;
    // <--

    /** Returns list of associated text nodes.

       OD 2008-02-19 #refactorlists#

       @return list of associated text nodes
    */
//    const SwTxtNodeTable * GetTxtNodeList() const { return pTxtNodeList; }
    void GetTxtNodeList( SwNumRule::tTxtNodeList& rTxtNodeList ) const;
    SwNumRule::tTxtNodeList::size_type GetTxtNodeListSize() const;

    // --> OD 2008-02-19 #refactorlists#
    void AddTxtNode( SwTxtNode& rTxtNode );
    void RemoveTxtNode( SwTxtNode& rTxtNode );
    // <--

    // --> OD 2008-03-03 #refactorlists#
    SwNumRule::tParagraphStyleList::size_type GetParagraphStyleListSize() const;
    void AddParagraphStyle( SwTxtFmtColl& rTxtFmtColl );
    void RemoveParagraphStyle( SwTxtFmtColl& rTxtFmtColl );
    // <--

    // --> OD 2008-04-03 #refactorlists#
    inline void SetDefaultListId( const String sDefaultListId )
    {
        msDefaultListId = sDefaultListId;
    }
    inline String GetDefaultListId() const
    {
        return msDefaultListId;
    }
    // <--
    // #i36749#
    /**
       Register this rule in a "name->numrule" map.

       @param pNumRuleMap      map to register in
     */
    void SetNumRuleMap(
                std::hash_map<String, SwNumRule *, StringHash>* pNumRuleMap );

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

    const String& GetName() const       { return sName; }
    // --> OD 2008-07-08 #i91400#
    void SetName( const String& rNm,
                  IDocumentListsAccess& rDocListAccess ); // #i36749#
    // <--

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

    bool IsCountPhantoms() const;
    void SetCountPhantoms(bool bCountPhantoms);

    // erfragen und setzen der Poolvorlagen-Id's
    USHORT GetPoolFmtId() const			{ return nPoolFmtId; }
    void SetPoolFmtId( USHORT nId ) 	{ nPoolFmtId = nId; }

    // erfragen und setzen der Hilfe-Id's fuer die Document-Vorlagen
    USHORT GetPoolHelpId() const 		{ return nPoolHelpId; }
    void SetPoolHelpId( USHORT nId ) 	{ nPoolHelpId = nId; }
    BYTE GetPoolHlpFileId() const 		{ return nPoolHlpFileId; }
    void SetPoolHlpFileId( BYTE nId ) 	{ nPoolHlpFileId = nId; }

    void		SetSvxRule(const SvxNumRule&, SwDoc* pDoc);
    SvxNumRule	MakeSvxNumRule() const;

    // #i23726#, #i23725#
    // --> OD 2008-06-09 #i90078#
    // refactoring: provide certain method for certain purpose
//    void        Indent(short aAmount, int nLevel = -1,
//                       int nReferenceLevel = -1, BOOL bRelative = TRUE,
//                       BOOL bFirstLine = TRUE, BOOL bCheckGtZero = TRUE);
    // change indent of all list levels by given difference
    void ChangeIndent( const short nDiff );
    // set indent of certain list level to given value
    void SetIndent( const short nNewIndent,
                    const USHORT nListLevel );
    // set indent of first list level to given value and change other list level's
    // indents accordingly
    void SetIndentOfFirstListLevelAndChangeOthers( const short nNewIndent );
    // <--

    void Validate();
};

// --> OD 2006-06-27 #b6440955#
// namespace for static functions and methods for numbering and bullets
namespace numfunc
{
    /** retrieve font family name used for the default bullet list characters

        @author OD
    */
    const String& GetDefBulletFontname();

    /** determine if default bullet font is user defined

        OD 2008-06-06 #i63395#
        The default bullet font is user defined, if it is given in the user configuration

        @author OD
    */
    bool IsDefBulletFontUserDefined();

    /** retrieve font used for the default bullet list characters

        @author OD
    */
    const Font& GetDefBulletFont();

    /** retrieve unicode of character used for the default bullet list for the given list level

        @author OD
    */
    sal_Unicode GetBulletChar( BYTE nLevel );

    /** configuration, if at first position of the first list item the <TAB>-key
        increased the indent of the complete list or only demotes this list item.
        The same for <SHIFT-TAB>-key at the same position for decreasing the
        indent of the complete list or only promotes this list item.

        OD 2007-10-01 #b6600435#

        @author OD
    */
    sal_Bool ChangeIndentOnTabAtFirstPosOfFirstListItem();

    /**
        OD 2008-06-06 #i89178#

        @author OD
    */
    SvxNumberFormat::SvxNumPositionAndSpaceMode GetDefaultPositionAndSpaceMode();
}

#endif	// _NUMRULE_HXX
