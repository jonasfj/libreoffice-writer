/*************************************************************************
 *
 *  $RCSfile: envimg.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-23 08:58:26 $
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
#ifndef _ENVIMG_HXX
#define _ENVIMG_HXX

#ifndef _SFXPOOLITEM_HXX //autogen
#include <svtools/poolitem.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif

// MakeSender -----------------------------------------------------------------

SW_DLLPUBLIC String MakeSender();

// enum -----------------------------------------------------------------------

enum SwEnvAlign
{
    ENV_HOR_LEFT = 0,
    ENV_HOR_CNTR,
    ENV_HOR_RGHT,
    ENV_VER_LEFT,
    ENV_VER_CNTR,
    ENV_VER_RGHT
};

// class SwEnvItem ------------------------------------------------------------

class SW_DLLPUBLIC SwEnvItem : public SfxPoolItem
{
public:

    rtl::OUString   aAddrText;       // Text fuer Empfaenger
    sal_Bool      	bSend;           // Absender?
    rtl::OUString  	aSendText;       // Text fuer Absender
    sal_Int32       lAddrFromLeft;   // Linker Abstand fuer Empfaenger (twips)
    sal_Int32       lAddrFromTop;    // Oberer Abstand fuer Empfaenger (twips)
    sal_Int32       lSendFromLeft;   // Linker Abstand fuer Absender   (twips)
    sal_Int32       lSendFromTop;    // Oberer Abstand fuer Absender   (twips)
    sal_Int32       lWidth;          // Breite des Umschlags           (twips)
    sal_Int32       lHeight;         // Hoehe  des Umschlags           (twips)
    SwEnvAlign 		eAlign;          // Ausrichtung beim Einzug
    sal_Bool  		bPrintFromAbove; // Von oben drucken?
    sal_Int32       lShiftRight;     // Verschiebung nach rechts       (twips)
    sal_Int32       lShiftDown;      // Verschiebung nach unten        (twips)

    SwEnvItem();
    SwEnvItem(const SwEnvItem& rItem);

    TYPEINFO();

    SwEnvItem& operator =(const SwEnvItem& rItem);

    virtual int operator ==(const SfxPoolItem& rItem) const;

    virtual SfxPoolItem*     Clone(SfxItemPool* = 0) const;
    virtual BOOL             QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );
};

// class SwEnvCfgItem -------------------------------------------------------
class SwEnvCfgItem : public utl::ConfigItem
{
    SwEnvItem aEnvItem;

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();
public:
    SwEnvCfgItem();
    ~SwEnvCfgItem();

    SwEnvItem& GetItem() {return aEnvItem;}

    virtual void			Commit();
};
#endif

