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
#ifndef _CFGDESC_HXX
#define _CFGDESC_HXX

#include <tools/string.hxx>

class SwCfgDesc
{
protected:
    String aFuncText;
    String aLibName;
    USHORT nEvent;

    SwCfgDesc(const String &rFuncName, USHORT nId);
    SwCfgDesc(const String &rLib,const String &rMacro);
    ~SwCfgDesc();

public:
    void SetEvent(USHORT nE);				// inline
    void SetFuncText(const String &rStr);	// inline
    void SetMacro(const String &rLib, const String &rMac);
    BOOL IsMacro() const;
    USHORT GetEvent() const;			 	// inline
    String GetFuncText() const;			 	// inline
    String GetMacro() const;			 	// inline
    String GetLib() const;			 	// inline
};

// INLINE METHODE --------------------------------------------------------
inline void SwCfgDesc::SetEvent(USHORT nE) {
    nEvent = nE;
}

inline void SwCfgDesc::SetFuncText(const String &rStr) {
    aFuncText = rStr;
}

inline 	USHORT SwCfgDesc::GetEvent() const { return nEvent; }
inline 	String SwCfgDesc::GetFuncText() const { return aFuncText; }
inline 	String SwCfgDesc::GetMacro() const { return aFuncText; }
inline 	String SwCfgDesc::GetLib() const { return aLibName; }


#endif
