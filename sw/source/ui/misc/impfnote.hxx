/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: impfnote.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 10:32:25 $
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
#ifndef _IMPFNOTE_HXX
#define _IMPFNOTE_HXX

#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
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
#ifndef _NUMBERINGTYPELISTBOX_HXX
#include <numberingtypelistbox.hxx>
#endif

class SwWrtShell;

class SwEndNoteOptionPage : public SfxTabPage
{
    FixedText		aNumTypeFT;
    SwNumberingTypeListBox	 aNumViewBox;
    FixedText 		aOffsetLbl;
    NumericField 	aOffsetFld;
    FixedText		aNumCountFT;
    ListBox 		aNumCountBox;
    FixedText		aPrefixFT;
    Edit			aPrefixED;
    FixedText		aSuffixFT;
    Edit			aSuffixED;
    FixedText		aPosFT;
    RadioButton 	aPosPageBox;
    RadioButton 	aPosChapterBox;
    FixedLine       aNumFL;

    FixedText 		aParaTemplLbl;
    ListBox 		aParaTemplBox;
    FixedText 		aPageTemplLbl;
    ListBox 		aPageTemplBox;
    FixedLine        aTemplFL;

    FixedText 		aFtnCharAnchorTemplLbl;
    ListBox 		aFtnCharAnchorTemplBox;
    FixedText 		aFtnCharTextTemplLbl;
    ListBox 		aFtnCharTextTemplBox;
    FixedLine        aCharTemplFL;

    FixedText aContLbl;
    Edit aContEdit;
    FixedText aContFromLbl;
    Edit aContFromEdit;
    FixedLine aContFL;

    String aNumDoc;
    String aNumPage;
    String aNumChapter;
    SwWrtShell *pSh;
    BOOL	bPosDoc;
    BOOL 	bEndNote;

    inline void SelectNumbering(int eNum);
    int GetNumbering() const;

    DECL_LINK( PosPageHdl, Button * );
    DECL_LINK( PosChapterHdl, Button * );
    DECL_LINK( NumCountHdl, ListBox * );


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
