/*************************************************************************
 *
 *  $RCSfile: impfnote.hxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:45 $
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
#ifndef _IMPFNOTE_HXX
#define _IMPFNOTE_HXX

#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

class SwWrtShell;

class SwEndNoteOptionPage : public SfxTabPage
{
    RadioButton aPosPageBox;
    RadioButton aPosChapterBox;
    GroupBox aPosGrp;

    ListBox 		aNumViewBox;
    FixedText 		aOffsetLbl;
    NumericField 	aOffsetFld;
    ListBox 		aNumCountBox;
    FixedText		aPrefixFT;
    Edit			aPrefixED;
    FixedText		aSuffixFT;
    Edit			aSuffixED;
    GroupBox 		aNumGrp;

    FixedText 		aParaTemplLbl;
    ListBox 		aParaTemplBox;
    FixedText 		aPageTemplLbl;
    ListBox 		aPageTemplBox;
    GroupBox 		aTemplGrp;

    FixedText 		aFtnCharAnchorTemplLbl;
    ListBox 		aFtnCharAnchorTemplBox;
    FixedText 		aFtnCharTextTemplLbl;
    ListBox 		aFtnCharTextTemplBox;
    GroupBox 		aCharTemplGrp;

    FixedText aContLbl;
    Edit aContEdit;
    FixedText aContFromLbl;
    Edit aContFromEdit;
    GroupBox aContGrp;

    String aNumPage;
    SwWrtShell *pSh;
    BOOL	bPosDoc;
    BOOL 	bEndNote;

    inline void SelectNumbering(int eNum);
    int GetNumbering() const;

    DECL_LINK( PosPage, Button * );
    DECL_LINK( PosChapter, Button * );
    DECL_LINK( NumCount, ListBox * );


public:
    SwEndNoteOptionPage( Window *pParent, BOOL bEndNote,
                         const SfxItemSet &rSet );
    ~SwEndNoteOptionPage();

    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);
    virtual BOOL FillItemSet(SfxItemSet &rSet);
    virtual void Reset( const SfxItemSet& );

    void SetShell( SwWrtShell &rShell );
};

class SwFootNoteOptionPage : public SwEndNoteOptionPage
{
    SwFootNoteOptionPage( Window *pParent, const SfxItemSet &rSet );
    ~SwFootNoteOptionPage();

public:
    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);
};



#endif
