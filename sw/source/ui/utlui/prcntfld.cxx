/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: prcntfld.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:46:29 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

// include ---------------------------------------------------------------


#include "prcntfld.hxx"

// STATIC DATA -----------------------------------------------------------

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

PercentField::PercentField( Window* pWin, const ResId& rResId ) :
        MetricField	( pWin, rResId ),

        nOldMax     (0),
        nOldMin		(0),
        nLastPercent(-1),
        nLastValue  (-1),
        eOldUnit    (FUNIT_NONE),
        bLockAutoCalculation(sal_False)
{

    nOldSpinSize = GetSpinSize();
    nRefValue = DenormalizePercent(MetricField::GetMax(FUNIT_TWIP));
    nOldDigits = GetDecimalDigits();
    SetCustomUnitText('%');
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void PercentField::SetRefValue(sal_Int64 nValue)
{
    sal_Int64 nRealValue = GetRealValue(eOldUnit);

    nRefValue = nValue;

    if (!bLockAutoCalculation && (GetUnit() == FUNIT_CUSTOM))
        SetPrcntValue(nRealValue, eOldUnit);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void PercentField::ShowPercent(BOOL bPercent)
{
    if ((bPercent && GetUnit() == FUNIT_CUSTOM) ||
        (!bPercent && GetUnit() != FUNIT_CUSTOM))
        return;

    sal_Int64 nOldValue;

    if (bPercent)
    {
        sal_Int64 nAktWidth, nPercent;

        nOldValue = GetValue();

        eOldUnit = GetUnit();
        nOldDigits = GetDecimalDigits();
        nOldMin = GetMin();
        nOldMax = GetMax();
        nOldSpinSize = GetSpinSize();
        nOldBaseValue = GetBaseValue();
        SetUnit(FUNIT_CUSTOM);
        SetDecimalDigits( 0 );

        nAktWidth = ConvertValue(nOldMin, 0, nOldDigits, eOldUnit, FUNIT_TWIP);
        // Um 0.5 Prozent aufrunden
        nPercent = ((nAktWidth * 10) / nRefValue + 5) / 10;

        MetricField::SetMin(Max(static_cast< sal_Int64 >(1), nPercent));
        MetricField::SetMax(100);
        SetSpinSize(5);
        MetricField::SetBaseValue(0);
        if (nOldValue != nLastValue)
        {
            nAktWidth = ConvertValue(nOldValue, 0, nOldDigits, eOldUnit, FUNIT_TWIP);
            nPercent = ((nAktWidth * 10) / nRefValue + 5) / 10;
            MetricFormatter::SetValue(nPercent);
            nLastPercent = nPercent;
            nLastValue = nOldValue;
        }
        else
            MetricFormatter::SetValue(nLastPercent);
//		SetValue(100, FUNIT_CUSTOM);
    }
    else
    {
        sal_Int64 nOldPercent = GetValue(FUNIT_CUSTOM);

        nOldValue = Convert(GetValue(), GetUnit(), eOldUnit);

        SetUnit(eOldUnit);
        SetDecimalDigits(nOldDigits);
        MetricField::SetMin(nOldMin);
        MetricField::SetMax(nOldMax);
        SetSpinSize(nOldSpinSize);
        MetricField::SetBaseValue(nOldBaseValue);

        if (nOldPercent != nLastPercent)
        {
            SetPrcntValue(nOldValue, eOldUnit);
            nLastPercent = nOldPercent;
            nLastValue = nOldValue;
        }
        else
            SetPrcntValue(nLastValue, eOldUnit);
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void PercentField::SetValue(sal_Int64 nNewValue, FieldUnit eInUnit)
{
   MetricFormatter::SetValue(nNewValue, eInUnit);
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/
void PercentField::SetPrcntValue(sal_Int64 nNewValue, FieldUnit eInUnit)
{
    if (GetUnit() != FUNIT_CUSTOM || eInUnit == FUNIT_CUSTOM)
        MetricFormatter::SetValue(Convert(nNewValue, eInUnit, GetUnit()));

    else
    {
        // Ausgangswert ueberschreiben, nicht spaeter restaurieren
        sal_Int64 nPercent, nAktWidth;
        if(eInUnit == FUNIT_TWIP)
        {
            nAktWidth = ConvertValue(nNewValue, 0, nOldDigits, FUNIT_TWIP, FUNIT_TWIP);
        }
        else
        {
            sal_Int64 nValue = Convert(nNewValue, eInUnit, eOldUnit);
            nAktWidth = ConvertValue(nValue, 0, nOldDigits, eOldUnit, FUNIT_TWIP);
        }
        nPercent = ((nAktWidth * 10) / nRefValue + 5) / 10;
        MetricFormatter::SetValue(nPercent);
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void PercentField::SetUserValue( sal_Int64 nNewValue, FieldUnit eInUnit )
{
    if (GetUnit() != FUNIT_CUSTOM || eInUnit == FUNIT_CUSTOM)
        MetricField::SetUserValue(Convert(nNewValue, eInUnit, GetUnit()),FUNIT_NONE);

    else
    {
        // Ausgangswert ueberschreiben, nicht spaeter restaurieren
        sal_Int64 nPercent, nAktWidth;
        if(eInUnit == FUNIT_TWIP)
        {
            nAktWidth = ConvertValue(nNewValue, 0, nOldDigits, FUNIT_TWIP, FUNIT_TWIP);
        }
        else
        {
            sal_Int64 nValue = Convert(nNewValue, eInUnit, eOldUnit);
            nAktWidth = ConvertValue(nValue, 0, nOldDigits, eOldUnit, FUNIT_TWIP);
        }
        nPercent = ((nAktWidth * 10) / nRefValue + 5) / 10;
        MetricField::SetUserValue(nPercent,FUNIT_NONE);
    }

}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void PercentField::SetBaseValue(sal_Int64 nNewValue, FieldUnit eInUnit)
{
    if (GetUnit() == FUNIT_CUSTOM)
        nOldBaseValue = ConvertValue(nNewValue, 0, nOldDigits, eInUnit, eOldUnit);
    else
        MetricField::SetBaseValue(nNewValue, eInUnit);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Int64 PercentField::GetValue( FieldUnit eOutUnit )
{
    return Convert(MetricField::GetValue(), GetUnit(), eOutUnit);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void PercentField::SetMin(sal_Int64 nNewMin, FieldUnit eInUnit)
{
    if (GetUnit() != FUNIT_CUSTOM)
        MetricField::SetMin(nNewMin, eInUnit);
    else
    {
        if (eInUnit == FUNIT_NONE)
            eInUnit = eOldUnit;
        nOldMin = Convert(nNewMin, eInUnit, eOldUnit);

        sal_Int64 nPercent = Convert(nNewMin, eInUnit, FUNIT_CUSTOM);
        MetricField::SetMin(Max( static_cast< sal_Int64 >(1), nPercent));
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void PercentField::SetMax(sal_Int64 nNewMax, FieldUnit eInUnit)
{
    if (GetUnit() != FUNIT_CUSTOM)
        MetricField::SetMax(nNewMax, eInUnit);
    else
    {
        if (eInUnit == FUNIT_NONE)
            eInUnit = eOldUnit;
//		SetRefValue(Convert(nNewMax, eInUnit, FUNIT_TWIP));
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Int64 PercentField::NormalizePercent(sal_Int64 nValue)
{
    if (GetUnit() != FUNIT_CUSTOM)
        nValue = MetricField::Normalize(nValue);
    else
        nValue = nValue * ImpPower10(nOldDigits);

    return nValue;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Int64 PercentField::DenormalizePercent(sal_Int64 nValue)
{
    if (GetUnit() != FUNIT_CUSTOM)
        nValue = MetricField::Denormalize(nValue);
    else
    {
        sal_Int64 nFactor = ImpPower10(nOldDigits);
        nValue = ((nValue+(nFactor/2)) / nFactor);
    }

    return nValue;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

BOOL PercentField::IsValueModified()
{
    if (GetUnit() == FUNIT_CUSTOM)
        return TRUE;
    else
        return MetricField::IsValueModified();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Int64 PercentField::ImpPower10( USHORT n )
{
    USHORT i;
    sal_Int64   nValue = 1;

    for ( i=0; i < n; i++ )
        nValue *= 10;

    return nValue;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Int64 PercentField::GetRealValue(FieldUnit eOutUnit)
{
    if (GetUnit() != FUNIT_CUSTOM)
        return GetValue(eOutUnit);
    else
        return Convert(GetValue(), GetUnit(), eOutUnit);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

sal_Int64 PercentField::Convert(sal_Int64 nValue, FieldUnit eInUnit, FieldUnit eOutUnit)
{
    if (eInUnit == eOutUnit ||
        (eInUnit == FUNIT_NONE && eOutUnit == GetUnit()) ||
        (eOutUnit == FUNIT_NONE && eInUnit == GetUnit()))
        return nValue;

    if (eInUnit == FUNIT_CUSTOM)
    {
        // Umrechnen in Metrik
        sal_Int64 nTwipValue = (nRefValue * nValue + 50) / 100;

        if (eOutUnit == FUNIT_TWIP)	// Nur wandeln, wenn unbedingt notwendig
            return NormalizePercent(nTwipValue);
        else
            return ConvertValue(NormalizePercent(nTwipValue), 0, nOldDigits, FUNIT_TWIP, eOutUnit);
    }

    if (eOutUnit == FUNIT_CUSTOM)
    {
        // Umrechnen in Prozent
        sal_Int64 nAktWidth;
        nValue = DenormalizePercent(nValue);

        if (eInUnit == FUNIT_TWIP)	// Nur wandeln, wenn unbedingt notwendig
            nAktWidth = nValue;
        else
            nAktWidth = ConvertValue(nValue, 0, nOldDigits, eInUnit, FUNIT_TWIP);
        // Um 0.5 Prozent runden
        return ((nAktWidth * 1000) / nRefValue + 5) / 10;
    }

    return ConvertValue(nValue, 0, nOldDigits, eInUnit, eOutUnit);
}


