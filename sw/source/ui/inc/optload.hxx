/*************************************************************************
 *
 *  $RCSfile: optload.hxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:41 $
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
#ifndef _OPTLOAD_HXX
#define _OPTLOAD_HXX

#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif

#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

class SwWrtShell;

/*-----------------31.08.96 10.09-------------------

--------------------------------------------------*/

class SwLoadOptPage : public SfxTabPage
{
private:
    RadioButton	aAlwaysRB;
    RadioButton	aRequestRB;
    RadioButton	aNeverRB;
    CheckBox	aLinkDocOnlyCB;
    GroupBox	aLinkGB;

    CheckBox	aAutoUpdateFields;
    CheckBox	aAutoUpdateCharts;
    CheckBox	aFldDocOnlyCB;
    GroupBox	aFldGB;

    CheckBox 	aMergeDistCB;
    CheckBox 	aMergeDistPageStartCB;
    GroupBox 	aCompatGB;

    SwWrtShell*	pWrtShell;
    USHORT		nOldLinkMode;

    DECL_LINK(	UpdateHdl, CheckBox* );

public:
                        SwLoadOptPage( Window* pParent,
                                         const SfxItemSet& rSet );
                        ~SwLoadOptPage();

    static SfxTabPage*	Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual	BOOL 		FillItemSet( SfxItemSet& rSet );
    virtual	void 		Reset( const SfxItemSet& rSet );
};

#endif


