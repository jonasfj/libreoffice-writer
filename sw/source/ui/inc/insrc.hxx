/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: insrc.hxx,v $
 * $Revision: 1.4 $
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
#ifndef _INSRC_HXX
#define _INSRC_HXX

#include <svx/stddlg.hxx>

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#include <tools/string.hxx>

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

class SwView;
class SwInsRowColDlg : public SvxStandardDialog
{
    FixedText 		aCount;
    NumericField 	aCountEdit;
    FixedLine        aInsFL;

    RadioButton 	aBeforeBtn;
    RadioButton 	aAfterBtn;
    FixedLine        aPosFL;

    String 			aRow;
    String 			aCol;

    OKButton 		aOKBtn;
    CancelButton 	aCancelBtn;
    HelpButton 		aHelpBtn;

    SwView& 		rView;
    BOOL  			bColumn;

protected:
    virtual void Apply();

public:
    SwInsRowColDlg( SwView& rView, BOOL bCol );
};

#endif

