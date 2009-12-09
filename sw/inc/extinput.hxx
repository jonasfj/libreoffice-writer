/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: extinput.hxx,v $
 * $Revision: 1.10 $
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
#ifndef _EXTINPUT_HXX
#define _EXTINPUT_HXX

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_USHORTS
#include <svl/svstdarr.hxx>
#endif
#include <pam.hxx>
#include <i18npool/lang.h>

class CommandExtTextInputData;

class SwExtTextInput : public SwPaM
{
    SvUShorts aAttrs;
    String sOverwriteText;
    BOOL bInsText : 1;
    BOOL bIsOverwriteCursor : 1;
    LanguageType eInputLanguage;
public:
    SwExtTextInput( const SwPaM& rPam, Ring* pRing = 0 );
    virtual ~SwExtTextInput();

    void SetInputData( const CommandExtTextInputData& rData );
    const SvUShorts& GetAttrs() const  	{ return aAttrs; }
    void SetInsText( BOOL bFlag )       { bInsText = bFlag; }
    BOOL IsOverwriteCursor() const 		{ return bIsOverwriteCursor; }
    void SetOverwriteCursor( BOOL bFlag );
    void SetLanguage(LanguageType eSet) { eInputLanguage = eSet;}
};

#endif	//_EXTINPUT_HXX

