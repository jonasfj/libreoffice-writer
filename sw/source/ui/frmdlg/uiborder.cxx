/*************************************************************************
 *
 *  $RCSfile: uiborder.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-23 08:56:07 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


#pragma hdrstop

//CHINA001 #ifndef _SVX_BORDER_HXX //autogen
//CHINA001 #include <svx/border.hxx>
//CHINA001 #endif
#include <svx/svxdlg.hxx> //CHINA001 
#include <svx/svxids.hrc> //CHINA001 
#include <svx/dialogs.hrc> //CHINA001 
#include <svtools/itemset.hxx> //CHINA001 
#include <svx/flagsdef.hxx> //CHINA001 
#include <sfx2/tabdlg.hxx> //CHINA001 
#ifndef _SFXINTITEM_HXX //CHINA001 
#include <svtools/intitem.hxx> //CHINA001 
#endif //CHINA001 

#include "swtypes.hxx"
#include "uiborder.hxx"
#include "frmui.hrc"




SwBorderDlg::SwBorderDlg(Window* pParent, SfxItemSet& rSet, USHORT nType) :

    SfxSingleTabDialog(pParent, rSet, 0)

{
    SetText(SW_RESSTR(STR_FRMUI_BORDER));

    // TabPage erzeugen
    //CHINA001 SvxBorderTabPage* pPage = (SvxBorderTabPage*) SvxBorderTabPage::Create(this, rSet);
    //CHINA001 pPage->SetSWMode(nType);
    //CHINA001 if(SW_BORDER_MODE_TABLE == nType)
        //CHINA001 pPage->HideShadowControls();
    //CHINA001 SetTabPage(pPage);
    SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();
    DBG_ASSERT(pFact, "Dialogdiet fail!");//CHINA001
    ::CreateTabPage fnCreatePage = pFact->GetTabPageCreatorFunc( RID_SVXPAGE_BORDER );
    if ( fnCreatePage )
    {
        SfxTabPage* pPage = (*fnCreatePage)( this, rSet );
        SfxAllItemSet aSet(*(rSet.GetPool()));
        aSet.Put (SfxUInt16Item(SID_SWMODE_TYPE,nType));
        if(SW_BORDER_MODE_TABLE == nType)
            aSet.Put (SfxUInt32Item(SID_FLAG_TYPE,SVX_HIDESHADOWCTL));
        pPage->PageCreated(aSet);
        SetTabPage(pPage);
    }		
}



SwBorderDlg::~SwBorderDlg()
{
}


