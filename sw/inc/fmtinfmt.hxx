/*************************************************************************
 *
 *  $RCSfile: fmtinfmt.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: er $ $Date: 2001-05-13 03:32:47 $
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
#ifndef _FMTINFMT_HXX
#define _FMTINFMT_HXX


#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX
#include <svtools/poolitem.hxx>
#endif

class SvxMacro;
class SvxMacroTableDtor;
class IntlWrapper;
// ATT_INETFMT *********************************************

class SwFmtINetFmt: public SfxPoolItem
{
    friend class SwTxtINetFmt;

    String aURL;					// die URL
    String aTargetFrame;			// in diesen Frame soll die URL
    String aINetFmt;
    String aVisitedFmt;
    String aName;					// Name des Links
    SvxMacroTableDtor* pMacroTbl;
    SwTxtINetFmt* pTxtAttr;			// mein TextAttribut
    USHORT nINetId;
    USHORT nVisitedId;
public:
    SwFmtINetFmt( const String& rURL, const String& rTarget );
    SwFmtINetFmt( const SwFmtINetFmt& rAttr );
    virtual ~SwFmtINetFmt();

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
    virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper* pIntl = 0 ) const;

    virtual	BOOL        	QueryValue( com::sun::star::uno::Any& rVal,
                                        BYTE nMemberId = 0 ) const;
    virtual	BOOL			PutValue( const com::sun::star::uno::Any& rVal,
                                        BYTE nMemberId = 0 );

    virtual USHORT			 GetVersion( USHORT nFFVer ) const;

    const SwTxtINetFmt* GetTxtINetFmt() const	{ return pTxtAttr; }
    SwTxtINetFmt* GetTxtINetFmt()				{ return pTxtAttr; }

    const String& GetValue() const 			{ return aURL; }
    void SetValue( const String& rS ) 		{ aURL = rS; }

    const String& GetName() const 			{ return aName; }
    void SetName( const String& rNm )		{ aName = rNm; }

    const String& GetTargetFrame() const 	{ return aTargetFrame; }
    void SetTargetFrame( const String& rNm ){ aTargetFrame = rNm; }

    const String& GetINetFmt() const		{ return aINetFmt; }
    void SetINetFmt( const String& rNm )	{ aINetFmt = rNm; }

    const String& GetVisitedFmt() const		{ return aVisitedFmt; }
    void SetVisitedFmt( const String& rNm )	{ aVisitedFmt = rNm; }

    USHORT GetINetFmtId() const				{ return nINetId; }
    void SetINetFmtId( USHORT nNew )		{ nINetId = nNew; }

    USHORT GetVisitedFmtId() const			{ return nVisitedId; }
    void SetVisitedFmtId( USHORT nNew )		{ nVisitedId = nNew; }

    // setze eine neue oder loesche die akt. MakroTabelle
    void SetMacroTbl( const SvxMacroTableDtor* pTbl = 0 );
    const SvxMacroTableDtor* GetMacroTbl() const	{ return pMacroTbl; }

    // setze / erfrage ein Makro
    void SetMacro( USHORT nEvent, const SvxMacro& rMacro );
    const SvxMacro* GetMacro( USHORT nEvent ) const;
};


#endif

