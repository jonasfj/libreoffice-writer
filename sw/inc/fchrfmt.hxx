/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fchrfmt.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:45:04 $
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
#ifndef _FCHRFMT_HXX
#define _FCHRFMT_HXX


#ifndef _SFXPOOLITEM_HXX //autogen
#include <svtools/poolitem.hxx>
#endif
#ifndef _CALBCK_HXX //autogen
#include <calbck.hxx>
#endif
#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif

class SwCharFmt;
class SwTxtCharFmt;
class IntlWrapper;

// ATT_CHARFMT *********************************************


class SwFmtCharFmt: public SfxPoolItem, public SwClient
{
    friend class SwTxtCharFmt;
    SwTxtCharFmt* pTxtAttr;		// mein TextAttribut

public:
    SwFmtCharFmt() : pTxtAttr(0) {}

    // single argument ctors shall be explicit.
    explicit SwFmtCharFmt( SwCharFmt *pFmt );
    virtual ~SwFmtCharFmt();

    // @@@ public copy ctor, but no copy assignment?
    SwFmtCharFmt( const SwFmtCharFmt& rAttr );
private:
    // @@@ public copy ctor, but no copy assignment?
    SwFmtCharFmt & operator= (const SwFmtCharFmt &);
public:

    TYPEINFO();

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

    // an das SwTxtCharFmt weiterleiten (vom SwClient)
    virtual void    Modify( SfxPoolItem*, SfxPoolItem* );
    virtual BOOL 	GetInfo( SfxPoolItem& rInfo ) const;

    void SetCharFmt( SwFmt* pFmt ) { pFmt->Add(this); }
    SwCharFmt* GetCharFmt() const { return (SwCharFmt*)GetRegisteredIn(); }
};
#endif

