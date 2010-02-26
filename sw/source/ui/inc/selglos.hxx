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
#ifndef _SELGLOS_HXX
#define _SELGLOS_HXX

#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#include <vcl/fixed.hxx>

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

class SwSelGlossaryDlg : public ModalDialog
{
protected:
    ListBox 		aGlosBox;
    FixedLine       aGlosFL;
    OKButton 		aOKBtn;
    CancelButton 	aCancelBtn;
    HelpButton 		aHelpBtn;

    DECL_LINK(DoubleClickHdl, ListBox*);
public:
    SwSelGlossaryDlg(Window * pParent, const String &rShortName);
    ~SwSelGlossaryDlg();
    void InsertGlos(const String &rRegion, const String &rGlosName);	// inline
    USHORT GetSelectedIdx() const;	// inline
    void SelectEntryPos(USHORT nIdx); 	// inline
};

inline void SwSelGlossaryDlg::InsertGlos(const String &rRegion,
                                const String &rGlosName)
{
    String aTmp( rRegion );
    aTmp += ':';
    aTmp += rGlosName;
    aGlosBox.InsertEntry( aTmp );
}
inline USHORT SwSelGlossaryDlg::GetSelectedIdx() const
{	return aGlosBox.GetSelectEntryPos(); }
inline void SwSelGlossaryDlg::SelectEntryPos(USHORT nIdx)
{	aGlosBox.SelectEntryPos(nIdx); }

#endif
