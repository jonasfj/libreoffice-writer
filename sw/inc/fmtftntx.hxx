/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fmtftntx.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:50:36 $
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
#ifndef _FMTFTNTX_HXX
#define _FMTFTNTX_HXX

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif

enum SwFtnEndPosEnum
{
    FTNEND_ATPGORDOCEND,				// at page or document end
    FTNEND_ATTXTEND,					// at end of the current text end
    FTNEND_ATTXTEND_OWNNUMSEQ,			// -""- and with own number sequence
    FTNEND_ATTXTEND_OWNNUMANDFMT,		// -""- and with onw numberformat
    FTNEND_ATTXTEND_END
};


class SW_DLLPUBLIC SwFmtFtnEndAtTxtEnd : public SfxEnumItem
{
    String 		sPrefix;
    String 		sSuffix;
    SvxNumberType aFmt;
    USHORT 	  	nOffset;

protected:
    SwFmtFtnEndAtTxtEnd( USHORT nWhich, SwFtnEndPosEnum ePos )
        : SfxEnumItem( nWhich, ePos ), nOffset( 0 )
    {}
    SwFmtFtnEndAtTxtEnd( const SwFmtFtnEndAtTxtEnd& rAttr )
        : SfxEnumItem( rAttr ), sPrefix( rAttr.sPrefix ), 
        sSuffix( rAttr.sSuffix ), aFmt( rAttr.aFmt ), 
        nOffset( rAttr.nOffset )
    {}

public:
    virtual USHORT 			GetValueCount() const;

    virtual int				operator==( const SfxPoolItem& ) const;

    virtual	BOOL        	 QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual	BOOL			 PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

// will be used at time??
//	void					FillVariable( SbxVariable &rVar,
//										  SfxMapUnit eCoreMetric,
//										  SfxMapUnit eUserMetric ) const;
//	virtual SfxArgumentError SetVariable( const SbxVariable &rVal,
//										  SfxMapUnit eCoreMetric,
//										  SfxMapUnit eUserMetric );
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper* pIntl = 0 ) const;

    inline BOOL IsAtEnd() const { return FTNEND_ATPGORDOCEND != GetValue(); }

    SwFmtFtnEndAtTxtEnd & operator=( const SwFmtFtnEndAtTxtEnd & rAttr );

    sal_Int16 GetNumType() const 		{ return aFmt.GetNumberingType(); }
    void SetNumType( sal_Int16 eType )	{ aFmt.SetNumberingType(eType); }

    const SvxNumberType& GetSwNumType() const 	{ return aFmt; }

    USHORT GetOffset() const 				{ return nOffset; }
    void SetOffset( USHORT nOff ) 			{ nOffset = nOff; }

    const String& GetPrefix() const 		{ return sPrefix; }
    void SetPrefix(const String& rSet)		{ sPrefix = rSet; }

    const String& GetSuffix() const 		{ return sSuffix; }
    void SetSuffix(const String& rSet)		{ sSuffix = rSet; }
};

class SW_DLLPUBLIC SwFmtFtnAtTxtEnd : public SwFmtFtnEndAtTxtEnd
{
public:
    SwFmtFtnAtTxtEnd( SwFtnEndPosEnum ePos = FTNEND_ATPGORDOCEND )
        : SwFmtFtnEndAtTxtEnd( RES_FTN_AT_TXTEND, ePos )
    {}

    virtual SfxPoolItem* Clone( SfxItemPool *pPool = 0 ) const;
};

class SW_DLLPUBLIC SwFmtEndAtTxtEnd : public SwFmtFtnEndAtTxtEnd
{
public:
    SwFmtEndAtTxtEnd( SwFtnEndPosEnum ePos = FTNEND_ATPGORDOCEND )
        : SwFmtFtnEndAtTxtEnd( RES_END_AT_TXTEND, ePos )
    {
        SetNumType( SVX_NUM_ROMAN_LOWER );
    }

    virtual SfxPoolItem* Clone( SfxItemPool *pPool = 0 ) const;
};

inline const SwFmtFtnAtTxtEnd &SwAttrSet::GetFtnAtTxtEnd(BOOL bInP) const
    { return (const SwFmtFtnAtTxtEnd&)Get( RES_FTN_AT_TXTEND, bInP); }
inline const SwFmtEndAtTxtEnd &SwAttrSet::GetEndAtTxtEnd(BOOL bInP) const
    { return (const SwFmtEndAtTxtEnd&)Get( RES_END_AT_TXTEND, bInP); }


inline const SwFmtFtnAtTxtEnd &SwFmt::GetFtnAtTxtEnd(BOOL bInP) const
    { return aSet.GetFtnAtTxtEnd(bInP); }
inline const SwFmtEndAtTxtEnd &SwFmt::GetEndAtTxtEnd(BOOL bInP) const
    { return aSet.GetEndAtTxtEnd(bInP); }

#endif

