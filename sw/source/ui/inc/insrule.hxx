/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: insrule.hxx,v $
 * $Revision: 1.6 $
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
#ifndef _INSRULE_HXX
#define _INSRULE_HXX
#include "num.hxx"

class SwRulerValueSet;
class ValueSet;
/*-----------------14.02.97 12.30-------------------

--------------------------------------------------*/
class SwInsertGrfRulerDlg  : public SfxModalDialog
{
    FixedLine       aSelectionFL;
    OKButton		aOkPB;
    CancelButton	aCancelPB;
    HelpButton		aHelpPB;

    List			aGrfNames;
    String			sSimple;
    String          sRulers;
    USHORT 			nSelPos;

    SwRulerValueSet* pExampleVS;

protected:
    DECL_LINK(SelectHdl, ValueSet*);
    DECL_LINK(DoubleClickHdl, ValueSet*);

public:
    SwInsertGrfRulerDlg( Window* pParent );
    ~SwInsertGrfRulerDlg();

    String 			GetGraphicName();
    BOOL			IsSimpleLine() {return nSelPos == 1;}
    BOOL 			HasImages() const {return 0 != aGrfNames.Count();}
};

#endif



