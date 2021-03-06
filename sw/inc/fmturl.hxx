/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#ifndef _FMTURL_HXX
#define _FMTURL_HXX

#include <svl/poolitem.hxx>
#include "swdllapi.h"
#include <hintids.hxx>
#include <format.hxx>

class ImageMap;
class IntlWrapper;

// URL, ServerMap und ClientMap

class SW_DLLPUBLIC SwFmtURL: public SfxPoolItem
{
    String	  sTargetFrameName;	// in diesen Frame soll die URL
    String	  sURL;				//Einfache URL
    String	  sName;			// Name des Anchors
    ImageMap *pMap;				//ClientSide Images

    BOOL      bIsServerMap;		//mit der URL eine ServerSideImageMap

    SwFmtURL& operator=( const SwFmtURL& );

public:
    SwFmtURL();

    // @@@ copy construction allowed, but assigment is not? @@@
    SwFmtURL( const SwFmtURL& );

    virtual ~SwFmtURL();

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;
    virtual	BOOL        	 QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

    void SetTargetFrameName( const String& rStr ) { sTargetFrameName = rStr; }
    void SetURL( const String &rURL, BOOL bServerMap );
    void SetMap( const ImageMap *pM );	//Pointer wird kopiert!

    const String   &GetTargetFrameName()const { return sTargetFrameName; }
    const String   &GetURL()			const { return sURL; }
          BOOL      IsServerMap()		const { return bIsServerMap; }
    const ImageMap *GetMap() 		    const { return pMap; }
          ImageMap *GetMap()				  { return pMap; }

    const String& GetName() const 			{ return sName; }
    void SetName( const String& rNm )		{ sName = rNm; }
};


inline const SwFmtURL &SwAttrSet::GetURL(BOOL bInP) const
    { return (const SwFmtURL&)Get( RES_URL,bInP); }

inline const SwFmtURL &SwFmt::GetURL(BOOL bInP) const
    { return aSet.GetURL(bInP); }

#endif

