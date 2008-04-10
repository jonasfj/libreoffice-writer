/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: insfnote.hxx,v $
 * $Revision: 1.5 $
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
#ifndef _INSFNOTE_HXX
#define _INSFNOTE_HXX

#include <svx/stddlg.hxx>

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#include <vcl/fixed.hxx>

class SwWrtShell;

class SwInsFootNoteDlg: public SvxStandardDialog
{
    SwWrtShell	   &rSh;

    //Alles fuer das/die Zeichen
    String			aFontName;
    CharSet			eCharSet;
    char			cExtChar;
    BOOL			bExtCharAvailable;
    BOOL			bEdit;
    RadioButton		aNumberAutoBtn;
    RadioButton		aNumberCharBtn;
    Edit 			aNumberCharEdit;
    PushButton		aNumberExtChar;
    FixedLine       aNumberFL;

    //Alles fuer die Auswahl Fussnote/Endnote
    RadioButton		aFtnBtn;
    RadioButton		aEndNoteBtn;
    FixedLine        aTypeFL;

    OKButton		aOkBtn;
    CancelButton	aCancelBtn;
    HelpButton		aHelpBtn;
    ImageButton 	aPrevBT;
    ImageButton		aNextBT;

    DECL_LINK( NumberCharHdl, Button * );
    DECL_LINK( NumberEditHdl, void * );
    DECL_LINK( NumberAutoBtnHdl, Button *);
    DECL_LINK( NumberExtCharHdl, Button *);
    DECL_LINK( NextPrevHdl, Button * );

    virtual void	Apply();

    void			Init();

public:
    SwInsFootNoteDlg(Window * pParent, SwWrtShell &rSh, BOOL bEd = FALSE);
    ~SwInsFootNoteDlg();

    CharSet         GetCharSet() { return eCharSet; }
    BOOL            IsExtCharAvailable() { return bExtCharAvailable; }
    String          GetFontName() { return aFontName; }
    BOOL            IsEndNote() { return aEndNoteBtn.IsChecked(); }
    String          GetStr()
                    {
                        if ( aNumberCharBtn.IsChecked() )
                            return aNumberCharEdit.GetText();
                        else
                            return String();
                    }
};

#endif
