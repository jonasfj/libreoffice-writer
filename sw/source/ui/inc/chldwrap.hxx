/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: chldwrap.hxx,v $
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
#ifndef _CHLDWRAP_HXX
#define _CHLDWRAP_HXX

#include <sfx2/childwin.hxx>
#include <vcl/timer.hxx>

class SwDocShell;

class SwChildWinWrapper : public SfxChildWindow
{
    Timer       m_aUpdateTimer;
    SwDocShell* m_pDocSh;

    DECL_LINK( UpdateHdl, void* );

    // Implementation in fldtdlg.cxx
protected:
    SwChildWinWrapper(Window *pParentWindow, USHORT nId);

public:
    virtual BOOL	ReInitDlg(SwDocShell *pDocSh);

    inline SwDocShell*  GetOldDocShell()                    { return m_pDocSh; }
    inline void         SetOldDocShell(SwDocShell *pDcSh)   { m_pDocSh = pDcSh; }
};


#endif

