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
#ifndef _ABSTRACT_HXX
#define _ABSTRACT_HXX

#include <sfx2/basedlgs.hxx>

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif


/*-----------------22.02.97 15.06-------------------

--------------------------------------------------*/
class SwInsertAbstractDlg  : public SfxModalDialog
{
    FixedLine       aFL;
    FixedText		aLevelFT;
    NumericField	aLevelNF;
    FixedText		aParaFT;
    NumericField	aParaNF;
    FixedText		aDescFT;
    OKButton		aOkPB;
    CancelButton	aCancelPB;
    HelpButton		aHelpPB;

protected:

public:
    SwInsertAbstractDlg( Window* pParent );
    ~SwInsertAbstractDlg();

    BYTE	GetLevel() const;
    BYTE	GetPara() const;
};

#endif



