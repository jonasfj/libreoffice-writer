/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mergetbl.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 11:01:02 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif


#pragma hdrstop

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _MERGETBL_HXX
#include <mergetbl.hxx>
#endif
#ifndef _MERGETBL_HRC
#include <mergetbl.hrc>
#endif
#ifndef _TABLE_HRC
#include <table.hrc>
#endif


SwMergeTblDlg::SwMergeTblDlg( Window *pParent, BOOL& rWithPrev )
    : SvxStandardDialog(pParent, SW_RES(DLG_MERGE_TABLE)),
    aOKPB(					this, ResId(PB_OK		)),
    aCancelPB(      		this, ResId(PB_CANCEL	)),
    aHelpPB(        		this, ResId(PB_HELP		)),
    aMergeFL(               this, ResId(FL_MERGE    )),
    aMergePrevRB(   		this, ResId(RB_MERGE_PREV)),
    aMergeNextRB(  			this, ResId(RB_MERGE_NEXT)),
    rMergePrev( 			rWithPrev )
{
    FreeResource();
    aMergePrevRB.Check();
}

void SwMergeTblDlg::Apply()
{
    rMergePrev = aMergePrevRB.IsChecked();
}



