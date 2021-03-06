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
#ifndef _MAILMERGEPREPAREMERGEPAGE_HXX
#define _MAILMERGEPREPAREMERGEPAGE_HXX

#include <svtools/wizardmachine.hxx>
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#include <vcl/field.hxx>
#include <svtools/stdctrl.hxx>
#include <mailmergehelper.hxx>
#include "actctrl.hxx"

class SwMailMergeWizard;
/*-- 02.04.2004 09:21:06---------------------------------------------------

  -----------------------------------------------------------------------*/
class SwMailMergePrepareMergePage : public svt::OWizardPage
{
    SwBoldFixedInfo m_aHeaderFI;
    FixedInfo       m_aPreviewFI;
    FixedText       m_aRecipientFT;
    PushButton      m_aFirstPB;
    PushButton      m_aPrevPB;
    NumEditAction   m_aRecordED;
    PushButton      m_aNextPB;
    PushButton      m_aLastPB;
    CheckBox        m_ExcludeCB;
                    
    FixedLine       m_aNoteHeaderFL;
    FixedInfo       m_aEditFI;
    PushButton      m_aEditPB;
                    

    SwMailMergeWizard*  m_pWizard;
    
    DECL_LINK(EditDocumentHdl_Impl, PushButton*);
    DECL_LINK(ExcludeHdl_Impl, CheckBox*);
    DECL_LINK(MoveHdl_Impl, void*);
    
    virtual void        ActivatePage();
    virtual sal_Bool    commitPage( ::svt::WizardTypes::CommitPageReason _eReason );

public:     
        SwMailMergePrepareMergePage( SwMailMergeWizard* _pParent);
        ~SwMailMergePrepareMergePage();

};

#endif


