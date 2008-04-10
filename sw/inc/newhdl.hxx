/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: newhdl.hxx,v $
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
#ifndef _NEWHDL_HXX
#define _NEWHDL_HXX

#include <tools/link.hxx>

#include "errhdl.hxx"		// hier kommt auch swtypes rein, was ich brauche

// ----------------------------------------------------------------------------

extern void SW_NEW_HDL();

class SwNewHdl
{
        friend void SW_NEW_HDL();
private:
        char*			pWarnMem;
        char*			pExceptMem;
        Link 			aWarnLnk;
        Link			aErrLnk;
public:
        void	SetWarnLnk( const Link &rLink );
        void	SetErrLnk( const Link &rLink );

        BOOL	TryWarnMem();

        BOOL 	Flush_WarnMem();
        BOOL 	Flush_ExceptMem();

        SwNewHdl();
        ~SwNewHdl();
};

/******************************************************************************
 *	INLINE
 ******************************************************************************/
inline void SwNewHdl::SetWarnLnk( const Link &rLink )
{
    aWarnLnk = rLink;
}

inline void SwNewHdl::SetErrLnk( const Link &rLink )
{
    aErrLnk = rLink;
}

#endif

