/*************************************************************************
 *
 *  $RCSfile: numpara.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: os $ $Date: 2002-11-06 10:16:34 $
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
#ifndef _NUMPARA_HXX
#define _NUMPARA_HXX


#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif


/* --- class SwParagraphNumTabPage ----------------------------------------
    Mit dieser TabPage werden Numerierungseinstellungen am Absatz bzw. der
    Absatzvorlage vorgenommen.

 ---------------------------------------------------------------------------*/
class SwParagraphNumTabPage : public SfxTabPage
{
    FixedText               aNumberStyleFT;
    ListBox					aNumberStyleLB;

    FixedLine               aNewStartFL;
    TriStateBox				aNewStartCB;
    TriStateBox             aNewStartNumberCB;
    NumericField			aNewStartNF;

    FixedLine               aCountParaFL;
    TriStateBox				aCountParaCB;
    TriStateBox				aRestartParaCountCB;
    FixedText				aRestartFT;
    NumericField			aRestartNF;

    BOOL					bModified : 1;
    BOOL					bCurNumrule : 1;

    DECL_LINK( NewStartHdl_Impl, CheckBox* );
    DECL_LINK( StyleHdl_Impl, ListBox* );
    DECL_LINK( LineCountHdl_Impl, CheckBox* );

protected:
        SwParagraphNumTabPage(Window* pParent, const SfxItemSet& rSet );

public:
        ~SwParagraphNumTabPage();

    static SfxTabPage*	Create(	Window* pParent,
                                const SfxItemSet& rSet );
    static USHORT*		GetRanges();

    virtual BOOL		FillItemSet( SfxItemSet& rSet );
    virtual void		Reset( const SfxItemSet& rSet );

    void				EnableNewStart();

    ListBox&			GetStyleBox() {return aNumberStyleLB;};

};


#endif

