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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <tools/debug.hxx>
#include <unotools/charclass.hxx>
#include <swtypes.hxx>
#include <swlbox.hxx>

using namespace nsSwComboBoxStyle;


SV_IMPL_PTRARR(SwEntryLst, SwBoxEntry*)

/*--------------------------------------------------------------------
     Beschreibung: Ein ListboxElement
 --------------------------------------------------------------------*/


SwBoxEntry::SwBoxEntry() :
    bModified(FALSE),
    bNew(FALSE),
    nId(LISTBOX_APPEND)
{
}


SwBoxEntry::SwBoxEntry(const String& aNam, USHORT nIdx) :
    bModified(FALSE),
    bNew(FALSE),
    aName(aNam),
    nId(nIdx)
{
}


SwBoxEntry::SwBoxEntry(const SwBoxEntry& rOld) :
    bModified(rOld.bModified),
    bNew(rOld.bNew),
    aName(rOld.aName),
    nId(rOld.nId)
{

}



SwComboBox::SwComboBox(Window* pParent, const ResId& rId, USHORT nStyleBits ):
    ComboBox(pParent, rId),
    nStyle(nStyleBits)
{
    // Verwaltung fuer die Stringlist aus der Resource aufbauen
    USHORT nSize = GetEntryCount();
    for( USHORT i=0; i < nSize; ++i )
    {
        const SwBoxEntry* pTmp = new SwBoxEntry(ComboBox::GetEntry(i), i);
        aEntryLst.Insert(pTmp, aEntryLst.Count() );
    }
}

/*--------------------------------------------------------------------
     Beschreibung: Basisklasse Dtor
 --------------------------------------------------------------------*/


SwComboBox::~SwComboBox()
{
// das erledigen die Listen doch schon selbst im DTOR!
//	aEntryLst.DeleteAndDestroy(0, 	aEntryLst.Count());
//	aDelEntryLst.DeleteAndDestroy(0, aDelEntryLst.Count());
}

/*--------------------------------------------------------------------
     Beschreibung: Eintrag in die ComboBox aufnehmen
 --------------------------------------------------------------------*/


void SwComboBox::InsertEntry(const SwBoxEntry& rEntry)
{
    InsertSorted(new SwBoxEntry(rEntry));
}

/*--------------------------------------------------------------------
     Beschreibung: Eintrag aus der Liste loeschen
 --------------------------------------------------------------------*/


void SwComboBox::RemoveEntry(USHORT nPos)
{
    if(nPos >= aEntryLst.Count())
        return;

    // Altes Element austragen
    SwBoxEntry* pEntry = aEntryLst[nPos];
    aEntryLst.Remove(nPos, 1);
    ComboBox::RemoveEntry(nPos);

    // keine neuen Eintraege in die Liste mit aufnehmen
    if(pEntry->bNew)
        return;

    // in DeleteListe eintragen
    aDelEntryLst.C40_INSERT(SwBoxEntry, pEntry, aDelEntryLst.Count());
}



/*--------------------------------------------------------------------
     Beschreibung: Position by Name
 --------------------------------------------------------------------*/

USHORT SwComboBox::GetEntryPos(const SwBoxEntry& rEntry) const
{
    return ComboBox::GetEntryPos(rEntry.aName);
}

/*--------------------------------------------------------------------
     Beschreibung: Rund um die Entries
 --------------------------------------------------------------------*/


const SwBoxEntry& SwComboBox::GetEntry(USHORT nPos) const
{
    if(nPos < aEntryLst.Count())
        return *aEntryLst[nPos];

    return aDefault;
}

/*--------------------------------------------------------------------
     Beschreibung: geloeschte Eintraege
 --------------------------------------------------------------------*/


USHORT SwComboBox::GetRemovedCount() const
{
    return aDelEntryLst.Count();
}


const SwBoxEntry& SwComboBox::GetRemovedEntry(USHORT nPos) const
{
    if(nPos < aDelEntryLst.Count())
        return *aDelEntryLst[nPos];

    return aDefault;
}

/*--------------------------------------------------------------------
     Beschreibung: Sortiert einfuegen
 --------------------------------------------------------------------*/


void SwComboBox::InsertSorted(SwBoxEntry* pEntry)
{
    ComboBox::InsertEntry(pEntry->aName);
    USHORT nPos = ComboBox::GetEntryPos(pEntry->aName);
    aEntryLst.C40_INSERT(SwBoxEntry, pEntry, nPos);
}


/*--------------------------------------------------------------------
    Beschreibung: Je nach Option bestimmte Zeichen ausblenden
 --------------------------------------------------------------------*/


void SwComboBox::KeyInput( const KeyEvent& rKEvt )
{
    USHORT nChar = rKEvt.GetCharCode();

    if(nStyle & CBS_FILENAME)
    {
#if defined UNX
        if(nChar == '/' || nChar == ' ' )
            return;
#else
        if(nChar == ':' || nChar == '\\' || nChar == '.' || nChar == ' ')
            return;
#endif
    }
    ComboBox::KeyInput(rKEvt);
}



/*--------------------------------------------------------------------
    Beschreibung: Text nach Option konvertieren
 --------------------------------------------------------------------*/


String SwComboBox::GetText() const
{
    String aTxt( ComboBox::GetText() );

    if(nStyle & CBS_LOWER)
        GetAppCharClass().toLower( aTxt );
    else if( nStyle & CBS_UPPER )
        GetAppCharClass().toUpper( aTxt );

    return aTxt;
}



