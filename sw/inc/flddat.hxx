/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: flddat.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:45:45 $
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

#ifndef _FLDDAT_HXX
#define _FLDDAT_HXX

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#include "fldbas.hxx"

class DateTime;

/*--------------------------------------------------------------------
    Beschreibung: Formate
 --------------------------------------------------------------------*/

enum SwDateFormat
{
    DF_BEGIN,
    //neue Formate:
    DFF_SSYS = DF_BEGIN,
    DFF_LSYS ,
    DFF_DMY ,
    DFF_DMYY ,
    DFF_DMMY ,
    DFF_DMMYY,
    DFF_DMMMY ,
    DFF_DMMMYY,
    DFF_DDMMY ,
    DFF_DDMMMY ,
    DFF_DDMMMYY ,
    DFF_DDDMMMY ,
    DFF_DDDMMMYY ,
    DFF_MY ,
    DFF_MD ,
    DFF_YMD ,
    DFF_YYMD ,
    DF_END ,
//	Kompatibilitaet:
    DF_SSYS 	= DFF_SSYS,
    DF_LSYS 	= DFF_LSYS,
    DF_SHORT 	= DFF_DMY,
    DF_SCENT 	= DFF_DMYY,
    DF_LMON		= DFF_DMMYY,
    DF_LMONTH	= DFF_DMMMYY,
    DF_LDAYMON	= DFF_DDMMMYY,
    DF_LDAYMONTH= DFF_DDDMMMYY
};

enum SwTimeFormat
{
    TF_BEGIN,
    TF_SYSTEM=TF_BEGIN,
    TF_SSMM_24,
    TF_SSMM_12,
    TF_END
};

enum SwTimeSubFormat
{
    TIME_FIX,
    TIME_VAR
};

enum SwDateSubFormat
{
    DATE_FIX,
    DATE_VAR
};

/*--------------------------------------------------------------------
    Beschreibung: Datum/Uhrzeitfeld
 --------------------------------------------------------------------*/

class SwDateTimeFieldType : public SwValueFieldType
{
public:
        SwDateTimeFieldType(SwDoc* pDoc);

        virtual SwFieldType*    Copy() const;
};

/*--------------------------------------------------------------------
    Beschreibung: Datum/Uhrzeitfeld
 --------------------------------------------------------------------*/

class SwDateTimeField : public SwValueField
{
        USHORT				nSubType;
        long				nOffset;	// Offset in Minuten

public:
        SwDateTimeField(SwDateTimeFieldType* pType, USHORT nSubType = DATEFLD,
                    ULONG nFmt = 0, USHORT nLng = 0);

        virtual USHORT			GetSubType() const;
        virtual void			SetSubType(USHORT nSub);

        virtual double			GetValue() const;

        virtual void			SetPar2(const String& rStr);
        virtual String			GetPar2() const;

        inline BOOL				IsDate() const
                                { return (nSubType & DATEFLD) != 0; }

        inline void				SetOffset(long nMinutes)	{ nOffset = nMinutes; }
        inline long				GetOffset() const			{ return nOffset; }

        Date                    GetDate(BOOL bUseOffset = FALSE) const;
        Time                    GetTime(BOOL bUseOffset = FALSE) const;
        void                    SetDateTime(const DateTime& rDT);
        static double           GetDateTime(SwDoc* pDoc, const DateTime& rDT);

        virtual String 			Expand() const;
        virtual SwField* 		Copy() const;

    virtual	BOOL       	QueryValue( com::sun::star::uno::Any& rVal, BYTE nMId ) const;
    virtual	BOOL		PutValue( const com::sun::star::uno::Any& rVal, BYTE nMId );
};

#endif // _FLDDAT_HXX
