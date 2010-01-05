/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fmthdft.hxx,v $
 * $Revision: 1.8 $
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
#ifndef _FMTHDFT_HXX
#define _FMTHDFT_HXX

#include <hintids.hxx>
#include <format.hxx>
#include <svl/poolitem.hxx>
#include <calbck.hxx>

class SwFrmFmt;
class IntlWrapper;


//Kopfzeile, fuer Seitenformate
//Client von FrmFmt das den Header beschreibt.

class SW_DLLPUBLIC SwFmtHeader: public SfxPoolItem, public SwClient
{
    BOOL bActive;		//Nur zur Steuerung (Erzeugung des Inhaltes)

public:
    SwFmtHeader( BOOL bOn = FALSE );
    SwFmtHeader( SwFrmFmt *pHeaderFmt );
    SwFmtHeader( const SwFmtHeader &rCpy );
    ~SwFmtHeader();
    SwFmtHeader& operator=( const SwFmtHeader &rCpy );

    TYPEINFO();

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;

    const SwFrmFmt *GetHeaderFmt() const { return (SwFrmFmt*)pRegisteredIn; }
          SwFrmFmt *GetHeaderFmt()		 { return (SwFrmFmt*)pRegisteredIn; }

    BOOL IsActive() const { return bActive; }
    void SetActive( BOOL bNew = TRUE ) { bActive = bNew; }
};

//Fusszeile, fuer Seitenformate
//Client von FrmFmt das den Footer beschreibt.

class SW_DLLPUBLIC SwFmtFooter: public SfxPoolItem, public SwClient
{
    BOOL bActive;		//Nur zur Steuerung (Erzeugung des Inhaltes)

public:
    SwFmtFooter( BOOL bOn = FALSE );
    SwFmtFooter( SwFrmFmt *pFooterFmt );
    SwFmtFooter( const SwFmtFooter &rCpy );
    ~SwFmtFooter();
    SwFmtFooter& operator=( const SwFmtFooter &rCpy );

    TYPEINFO();

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper*    pIntl = 0 ) const;

    const SwFrmFmt *GetFooterFmt() const { return (SwFrmFmt*)pRegisteredIn; }
          SwFrmFmt *GetFooterFmt()		 { return (SwFrmFmt*)pRegisteredIn; }

    BOOL IsActive() const { return bActive; }
    void SetActive( BOOL bNew = TRUE ) { bActive = bNew; }
};

inline const SwFmtHeader &SwAttrSet::GetHeader(BOOL bInP) const
    { return (const SwFmtHeader&)Get( RES_HEADER,bInP); }
inline const SwFmtFooter &SwAttrSet::GetFooter(BOOL bInP) const
    { return (const SwFmtFooter&)Get( RES_FOOTER,bInP); }

inline const SwFmtHeader &SwFmt::GetHeader(BOOL bInP) const
    { return aSet.GetHeader(bInP); }
inline const SwFmtFooter &SwFmt::GetFooter(BOOL bInP) const
    { return aSet.GetFooter(bInP); }

#endif

