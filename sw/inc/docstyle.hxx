/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docstyle.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:42:17 $
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
#ifndef _DOCSTYLE_HXX
#define _DOCSTYLE_HXX

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#endif

#ifndef _SFXSTYLE_HXX //autogen
#include <svtools/style.hxx>
#endif

#ifndef _SFXITEMSET_HXX //autogen
#include <svtools/itemset.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif

class SwDoc;
class SwDocStyleSheetPool;
class SwPageDesc;
class SwCharFmt;
class SwTxtFmtColl;
class SwFrmFmt;
class SwNumRule;

/*--------------------------------------------------------------------
    Beschreibung:	Lokale Hilfsklasse
 --------------------------------------------------------------------*/
class SwPoolFmtList : public SvStringsDtor
{
public:
    SwPoolFmtList() {}
    void Append( char cChar, const String& rStr );
    void Erase();
};


/*--------------------------------------------------------------------
    Beschreibung:	temp. StyleSheet
 --------------------------------------------------------------------*/
class SW_DLLPUBLIC SwDocStyleSheet : public SfxStyleSheetBase
{
    friend class SwDocStyleSheetPool;
    friend class SwStyleSheetIterator;

    SwCharFmt*			pCharFmt;
    SwTxtFmtColl* 		pColl;
    SwFrmFmt* 			pFrmFmt;
    const SwPageDesc* 	pDesc;
    const SwNumRule*	pNumRule;

    SwDoc&				rDoc;
    SfxItemSet			aCoreSet;

    BOOL 				bPhysical;


    // leere Huelse zum richtigen StyleSheet (Core) machen
    SW_DLLPRIVATE void				Create();

    // den StyleSheet mit Daten fuellen
    enum FillStyleType {
        FillOnlyName,
        FillAllInfo,
        FillPhysical
    };

    SW_DLLPRIVATE BOOL FillStyleSheet( FillStyleType eFType );

public:
    SwDocStyleSheet( SwDoc& 				rDoc,
                     const String& 			rName,
                     SwDocStyleSheetPool& 	rPool,
                     SfxStyleFamily 		eFam,
                     USHORT 				nMask);

    SwDocStyleSheet( const SwDocStyleSheet& );
    virtual ~SwDocStyleSheet();

    void					Reset();

    void 					SetMask(USHORT nMsk)			{ nMask = nMsk;   	}
    void 					SetFamily(SfxStyleFamily eFam)	{ nFamily = eFam;	}

    BOOL		   			IsPhysical() const				{ return bPhysical; }
    void					SetPhysical(BOOL bPhys);

    void					SetItemSet(const SfxItemSet& rSet);

    virtual SfxItemSet& 	GetItemSet();
    virtual const String& 	GetParent() const;
    virtual const String& 	GetFollow() const;

    virtual ULONG GetHelpId( String& rFile );
    virtual void SetHelpId( const String& r, ULONG nId );

    // Vorbelegen der member ohne physikalischen Zugriff
    // wird vom StyleSheetPool benutzt
    //
    void			  		PresetName(const String& rName)  { aName   = rName; }
    void			  		PresetNameAndFamily(const String& rName);
    void					PresetParent(const String& rName){ aParent = rName; }
    void					PresetFollow(const String& rName){ aFollow = rName; }

    virtual BOOL 			SetName( const String& rStr);
    virtual BOOL 			SetParent( const String& rStr);
    virtual BOOL 			SetFollow( const String& rStr);

    virtual BOOL 			HasFollowSupport() const;
    virtual BOOL 			HasParentSupport() const;
    virtual BOOL 			HasClearParentSupport() const;
    virtual String 			GetDescription();
    virtual String 			GetDescription(SfxMapUnit eUnit);

    SwCharFmt*		   		GetCharFmt();
    SwTxtFmtColl* 			GetCollection();
    SwFrmFmt* 				GetFrmFmt();
    const SwPageDesc* 		GetPageDesc();
    const SwNumRule*		GetNumRule();
    void					SetNumRule(const SwNumRule& rRule);

    virtual BOOL 			IsUsed() const;
};

/*--------------------------------------------------------------------
    Beschreibung:	Iterator fuer den Pool
 --------------------------------------------------------------------*/

class SwStyleSheetIterator : public SfxStyleSheetIterator, public SfxListener
{
    SwDocStyleSheet 	aIterSheet;
    SwDocStyleSheet 	aStyleSheet;
    SwPoolFmtList 		aLst;
    USHORT 				nLastPos;
    BOOL 				bFirstCalled;

    void				AppendStyleList(const SvStringsDtor& rLst,
                                        BOOL 	bUsed,
                                        USHORT 	nSection,
                                        char	cType);

public:
    SwStyleSheetIterator( SwDocStyleSheetPool* pBase,
                          SfxStyleFamily eFam, USHORT n=0xFFFF );
    virtual ~SwStyleSheetIterator();

    virtual USHORT Count();
    virtual SfxStyleSheetBase *operator[](USHORT nIdx);
    virtual SfxStyleSheetBase* First();
    virtual SfxStyleSheetBase* Next();
    virtual SfxStyleSheetBase* Find(const UniString& rStr);

    virtual void Notify( SfxBroadcaster&, const SfxHint& );
};

/*--------------------------------------------------------------------
    Beschreibung:	Pool fuer
 --------------------------------------------------------------------*/

class SwDocStyleSheetPool : public SfxStyleSheetBasePool
{
    SwDocStyleSheet 	aStyleSheet;
    SwDoc&				rDoc;
    BOOL				bOrganizer : 1;		// TRUE: fuer den Organizer


    virtual SfxStyleSheetBase* Create( const String&, SfxStyleFamily, USHORT nMask);
    virtual SfxStyleSheetBase* Create( const SfxStyleSheetBase& );
public:
    SwDocStyleSheetPool( SwDoc&, BOOL bOrganizer = FALSE );
    virtual ~SwDocStyleSheetPool();

    virtual void Replace( SfxStyleSheetBase& rSource,
                          SfxStyleSheetBase& rTarget );
    virtual SfxStyleSheetBase& Make(const String&, SfxStyleFamily, USHORT nMask, USHORT nPos = 0xffff);
    virtual SfxStyleSheetBase* Find( const String&, SfxStyleFamily eFam,
                                    USHORT n=0xFFFF );
    virtual BOOL SetParent( SfxStyleFamily eFam, const String &rStyle,
                            const String &rParent );

    virtual void Erase( SfxStyleSheetBase* pStyle);
    void	SetItemSet(const SfxItemSet& rSet) { aStyleSheet.SetItemSet(rSet); }

    void	SetOrganizerMode( BOOL bMode )	{ bOrganizer = bMode; }
    BOOL 	IsOrganizerMode() const 		{ return bOrganizer; }

    virtual SfxStyleSheetIterator* CreateIterator( SfxStyleFamily,
                                                    USHORT nMask );

    SwDoc& GetDoc() const { return rDoc; }

    //Fuer die daemlicheren Compiler
private:
    SwDocStyleSheetPool( const SwDocStyleSheetPool& );
};


#endif
