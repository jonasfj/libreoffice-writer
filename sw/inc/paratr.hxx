/*************************************************************************
 *
 *  $RCSfile: paratr.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-23 08:37:48 $
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
#ifndef _PARATR_HXX
#define _PARATR_HXX

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _CALBCK_HXX
#include <calbck.hxx>
#endif
#ifndef _SWATRSET_HXX
#include <swatrset.hxx>
#endif
#ifndef _FORMAT_HXX
#include <format.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif

class SwCharFmt;
class IntlWrapper;

/*************************************************************************
|*    class		SwFmtDrop
*************************************************************************/

#define DROP_WHOLEWORD ((USHORT)0x0001)

//Wenn ein SwFmtDrop Client ist, so ist dies das CharFmt welches den Font fuer
//die DropCaps beschreibt. Ist es kein Client, so wird von der Formatierung
//das CharFmt des Absatzes benutzt.
//Wird das CharFmt verandert, so wird diese Aenderung ueber das Modify des
//SwFmtDrop an die Absatze propagiert.
class SW_DLLPUBLIC SwFmtDrop: public SfxPoolItem, public SwClient
{
    SwModify* pDefinedIn;	// Modify-Object, in dem der DropCaps steht
                            // kann nur TxtFmtCollection/TxtNode sein
    USHORT nDistance;   // Abstand zum Textbeginn
    USHORT nReadFmt;   	// fuer den Sw3-Reader: CharFormat-Id (Pool laden!)
    BYTE   nLines;		 // Anzahl der Zeilen
    BYTE   nChars;		 // Anzahl der Zeichen
    BOOL   bWholeWord;	 // Erstes Wort als Initialen
public:
    TYPEINFO(); //Bereits in der Basisklasse SwClient

    SwFmtDrop();
    virtual ~SwFmtDrop();

    // @@@ public copy ctor, but no copy assignment?
    SwFmtDrop( const SwFmtDrop & );
private:
    // @@@ public copy ctor, but no copy assignment?
    SwFmtDrop & operator= (const SwFmtDrop &);
public:

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
    virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0) const;
    virtual	BOOL        	 QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );
    virtual USHORT			 GetVersion( USHORT nFFVer ) const;

    inline BYTE GetLines() const { return nLines; }
    inline BYTE &GetLines() { return nLines; }

    inline BYTE GetChars() const { return nChars; }
    inline BYTE &GetChars() { return nChars; }

    inline BOOL GetWholeWord() const { return bWholeWord; }
    inline BYTE &GetWholeWord() { return bWholeWord; }

    inline USHORT GetDistance() const { return nDistance; }
    inline USHORT &GetDistance() { return nDistance; }

    // fuer den Sw3-Reader: CharFormat-Id (Pool laden!)
    inline USHORT GetReadFmt() const 		{ return nReadFmt; }
    inline void SetReadFmt( USHORT n ) 		{ nReadFmt = n; }

    inline const SwCharFmt *GetCharFmt() const { return (SwCharFmt*)pRegisteredIn; }
    inline SwCharFmt *GetCharFmt()       { return (SwCharFmt*)pRegisteredIn; }
    void SetCharFmt( SwCharFmt *pNew );
    virtual void Modify( SfxPoolItem*, SfxPoolItem* );
        // erfrage vom Client Informationen
    virtual BOOL GetInfo( SfxPoolItem& ) const;

    // erfrage und setze den Modify-Pointer
    inline const SwModify* GetDefinedIn() const { return pDefinedIn; }
    inline void ChgDefinedIn( const SwModify* pNew )
    { pDefinedIn = (SwModify*)pNew; }
};

class SwRegisterItem : public SfxBoolItem
{
public:
    TYPEINFO();

    inline SwRegisterItem( const BOOL bRegister = FALSE );

    // @@@ public copy assignment, but no copy ctor?
    inline SwRegisterItem& operator=( const SwRegisterItem& rRegister );

    // "pure virtual Methoden" vom SfxPoolItem
    virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*    Create( SvStream&, USHORT ) const;
    virtual SvStream&       Store( SvStream&, USHORT nIVer ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;
    virtual USHORT           GetVersion( USHORT nFFVer ) const;
};

inline SwRegisterItem::SwRegisterItem( const BOOL bRegister ) :
    SfxBoolItem( RES_PARATR_REGISTER, bRegister )
{}

inline SwRegisterItem& SwRegisterItem::operator=(
    const SwRegisterItem& rRegister )
{
    SetValue( rRegister.GetValue() );
    return *this;
}

class SwNumRuleItem : public SfxStringItem
{
    SwModify* pDefinedIn;	// Modify-Object, in dem das NumRule steht
                            // kann nur TxtFmtCollection/TxtNode sein
public:
    TYPEINFO();

    SwNumRuleItem()
        : SfxStringItem( RES_PARATR_NUMRULE, aEmptyStr ), pDefinedIn(0) {}

    SwNumRuleItem( const String& rRuleName )
        : SfxStringItem( RES_PARATR_NUMRULE, rRuleName ), pDefinedIn(0) {}

    SwNumRuleItem( const SwNumRuleItem& rCpy )
        : SfxStringItem( RES_PARATR_NUMRULE, rCpy.GetValue() ), pDefinedIn(0) {}


    SwNumRuleItem& operator=( const SwNumRuleItem& rCpy )
    { SetValue( rCpy.GetValue() ); pDefinedIn=0; return *this; }

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*	Create( SvStream&, USHORT ) const;
    virtual SvStream&		Store( SvStream&, USHORT nIVer ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;
    virtual USHORT			 GetVersion( USHORT nFFVer ) const;

    virtual	BOOL        	 QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId );

    // erfrage und setze den Modify-Pointer
    inline const SwModify* GetDefinedIn() const { return pDefinedIn; }
    inline void ChgDefinedIn( const SwModify* pNew )
    { pDefinedIn = (SwModify*)pNew; }
};

class SwParaConnectBorderItem : public SfxBoolItem
{
public:
    TYPEINFO();

    inline SwParaConnectBorderItem( const BOOL bConnect = TRUE );

    // @@@ public copy assignment, but no copy ctor?
    inline SwParaConnectBorderItem& operator=( const SwParaConnectBorderItem& rConnect );

    // "pure virtual Methoden" vom SfxPoolItem
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;
};

inline SwParaConnectBorderItem::SwParaConnectBorderItem( const BOOL bConnect ) :
    SfxBoolItem( RES_PARATR_CONNECT_BORDER, bConnect )
{}

inline SwParaConnectBorderItem& SwParaConnectBorderItem::operator=(
    const SwParaConnectBorderItem& rConnect )
{
    SetValue( rConnect.GetValue() );
    return *this;
}



/******************************************************************************
 *	Implementierung der Paragraph-Attribut Methoden vom SwAttrSet
 ******************************************************************************/

inline const SvxLineSpacingItem &SwAttrSet::GetLineSpacing(BOOL bInP) const
    {	return (const SvxLineSpacingItem&)Get( RES_PARATR_LINESPACING,bInP); }
inline const SvxAdjustItem &SwAttrSet::GetAdjust(BOOL bInP) const
    {	return (const SvxAdjustItem&)Get( RES_PARATR_ADJUST,bInP); }
inline const SvxFmtSplitItem &SwAttrSet::GetSplit(BOOL bInP) const
    {	return (const SvxFmtSplitItem&)Get( RES_PARATR_SPLIT,bInP); }
inline const SwRegisterItem &SwAttrSet::GetRegister(BOOL bInP) const
    {	return (const SwRegisterItem&)Get( RES_PARATR_REGISTER,bInP); }
inline const SvxWidowsItem &SwAttrSet::GetWidows(BOOL bInP) const
    {	return (const SvxWidowsItem&)Get( RES_PARATR_WIDOWS,bInP); }
inline const SvxOrphansItem &SwAttrSet::GetOrphans(BOOL bInP) const
    {	return (const SvxOrphansItem&)Get( RES_PARATR_ORPHANS,bInP); }
inline const SvxTabStopItem &SwAttrSet::GetTabStops(BOOL bInP) const
    {	return (const SvxTabStopItem&)Get( RES_PARATR_TABSTOP,bInP); }
inline const SvxHyphenZoneItem &SwAttrSet::GetHyphenZone(BOOL bInP) const
    {	return (const SvxHyphenZoneItem&)Get(RES_PARATR_HYPHENZONE,bInP); }
inline const SwFmtDrop &SwAttrSet::GetDrop(BOOL bInP) const
    {	return (const SwFmtDrop&)Get(RES_PARATR_DROP,bInP); }
inline const SwNumRuleItem &SwAttrSet::GetNumRule(BOOL bInP) const
    {	return (const SwNumRuleItem&)Get(RES_PARATR_NUMRULE,bInP); }
inline const SvxScriptSpaceItem& SwAttrSet::GetScriptSpace(BOOL bInP) const
    {	return (const SvxScriptSpaceItem&)Get(RES_PARATR_SCRIPTSPACE,bInP); }
inline const SvxHangingPunctuationItem &SwAttrSet::GetHangingPunctuation(BOOL bInP) const
    {	return (const SvxHangingPunctuationItem&)Get(RES_PARATR_HANGINGPUNCTUATION,bInP); }
inline const SvxForbiddenRuleItem &SwAttrSet::GetForbiddenRule(BOOL bInP) const
    {	return (const SvxForbiddenRuleItem&)Get(RES_PARATR_FORBIDDEN_RULES, bInP); }
inline const SvxParaVertAlignItem &SwAttrSet::GetParaVertAlign(BOOL bInP) const
    {	return (const SvxParaVertAlignItem&)Get( RES_PARATR_VERTALIGN, bInP ); }
inline const SvxParaGridItem &SwAttrSet::GetParaGrid(BOOL bInP) const
    {   return (const SvxParaGridItem&)Get( RES_PARATR_SNAPTOGRID, bInP ); }
inline const SwParaConnectBorderItem &SwAttrSet::GetParaConnectBorder(BOOL bInP) const
    {   return (const SwParaConnectBorderItem&)Get( RES_PARATR_CONNECT_BORDER, bInP ); }

/******************************************************************************
 *	Implementierung der Paragraph-Attribut Methoden vom SwFmt
 ******************************************************************************/

inline const SvxLineSpacingItem &SwFmt::GetLineSpacing(BOOL bInP) const
    {	return aSet.GetLineSpacing(bInP); }
inline const SvxAdjustItem &SwFmt::GetAdjust(BOOL bInP) const
    {	return aSet.GetAdjust(bInP); }
inline const SvxFmtSplitItem &SwFmt::GetSplit(BOOL bInP) const
    {	return aSet.GetSplit(bInP); }
inline const SwRegisterItem &SwFmt::GetRegister(BOOL bInP) const
    {	return aSet.GetRegister(bInP); }
inline const SvxWidowsItem &SwFmt::GetWidows(BOOL bInP) const
    {	return aSet.GetWidows(bInP); }
inline const SvxOrphansItem &SwFmt::GetOrphans(BOOL bInP) const
    {	return aSet.GetOrphans(bInP); }
inline const SvxTabStopItem &SwFmt::GetTabStops(BOOL bInP) const
    {	return aSet.GetTabStops(bInP); }
inline const SvxHyphenZoneItem &SwFmt::GetHyphenZone(BOOL bInP) const
    {	return aSet.GetHyphenZone(bInP); }
inline const SwFmtDrop &SwFmt::GetDrop(BOOL bInP) const
    {	return aSet.GetDrop(bInP); }
inline const SwNumRuleItem &SwFmt::GetNumRule(BOOL bInP) const
    {	return aSet.GetNumRule(bInP); }
inline const SvxScriptSpaceItem& SwFmt::GetScriptSpace(BOOL bInP) const
    {	return aSet.GetScriptSpace(bInP) ; }
inline const SvxHangingPunctuationItem &SwFmt::GetHangingPunctuation(BOOL bInP) const
    {	return aSet.GetHangingPunctuation(bInP) ; }
inline const SvxForbiddenRuleItem &SwFmt::GetForbiddenRule(BOOL bInP) const
    {	return (const SvxForbiddenRuleItem&)aSet.Get(RES_PARATR_FORBIDDEN_RULES, bInP); }
inline const SvxParaVertAlignItem &SwFmt::GetParaVertAlign(BOOL bInP) const
    {	return (const SvxParaVertAlignItem&)aSet.Get( RES_PARATR_VERTALIGN, bInP ); }
inline const SvxParaGridItem &SwFmt::GetParaGrid(BOOL bInP) const
    {   return (const SvxParaGridItem&)aSet.Get( RES_PARATR_SNAPTOGRID, bInP ); }
inline const SwParaConnectBorderItem &SwFmt::GetParaConnectBorder(BOOL bInP) const
    {   return (const SwParaConnectBorderItem&)aSet.Get( RES_PARATR_CONNECT_BORDER, bInP ); }

#endif
