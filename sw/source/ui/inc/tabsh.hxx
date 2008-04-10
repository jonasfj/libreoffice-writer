/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: tabsh.hxx,v $
 * $Revision: 1.5 $
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
#ifndef _SWTABSH_HXX
#define _SWTABSH_HXX

#include "basesh.hxx"
#include "swdllapi.h"

class SfxItemSet;
class SwWrtShell;

SW_DLLPUBLIC void ItemSetToTableParam( const SfxItemSet& rSet, SwWrtShell &rSh );

extern const USHORT __FAR_DATA aUITableAttrRange[];
SW_DLLPUBLIC const USHORT* SwuiGetUITableAttrRange();

class SwTableShell: public SwBaseShell
{
public:
    SFX_DECL_INTERFACE(SW_TABSHELL)
    TYPEINFO();

    void	Execute(SfxRequest &);
    void	GetState(SfxItemSet &);
    void	GetFrmBorderState(SfxItemSet &rSet);
    void 	GetLineStyleState(SfxItemSet &rSet);
    void	ExecTableStyle(SfxRequest& rReq);

    void	ExecNumberFormat(SfxRequest& rReq);

            SwTableShell(SwView &rView);
};

#endif
