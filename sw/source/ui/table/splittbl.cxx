/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: splittbl.cxx,v $
 * $Revision: 1.9 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


#include <wrtsh.hxx>
#include <splittbl.hxx>
#include <splittbl.hrc>
#include <table.hrc>
#include <tblenum.hxx>
/*-----------------17.03.98 10:56-------------------

--------------------------------------------------*/
SwSplitTblDlg::SwSplitTblDlg( Window *pParent, SwWrtShell &rSh ) :
    SvxStandardDialog(pParent, SW_RES(DLG_SPLIT_TABLE)),
    aOKPB(					this, SW_RES(PB_OK		)),
    aCancelPB(      		this, SW_RES(PB_CANCEL	)),
    aHelpPB(        		this, SW_RES(PB_HELP		)),
    aSplitFL(               this, SW_RES(FL_SPLIT    )),
    aCntntCopyRB(   		this, SW_RES(RB_CNTNT	)),
    aBoxAttrCopyWithParaRB( this, SW_RES(RB_BOX_PARA	)),
    aBoxAttrCopyNoParaRB(	this, SW_RES(RB_BOX_NOPARA)),
    aBorderCopyRB(  		this, SW_RES(RB_BORDER	)),
    rShell(rSh)
{
    FreeResource();
    aCntntCopyRB.Check();
}

/*-----------------17.03.98 10:56-------------------

--------------------------------------------------*/
void SwSplitTblDlg::Apply()
{
    USHORT nSplit = HEADLINE_CNTNTCOPY;
    if(aBoxAttrCopyWithParaRB.IsChecked())
        nSplit = HEADLINE_BOXATRCOLLCOPY;
    if(aBoxAttrCopyNoParaRB.IsChecked())
        nSplit = HEADLINE_BOXATTRCOPY;
    else if(aBorderCopyRB.IsChecked())
        nSplit = HEADLINE_BORDERCOPY;

    rShell.SplitTable( nSplit );

}



