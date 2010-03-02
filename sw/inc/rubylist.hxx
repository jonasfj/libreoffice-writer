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
#ifndef _RUBYLIST_HXX
#define _RUBYLIST_HXX

#include <svl/svarray.hxx>
#include <swtypes.hxx>
#include <fmtruby.hxx>

class SwRubyListEntry
{
    String sText;
    SwFmtRuby aRubyAttr;
public:
    SwRubyListEntry() : aRubyAttr( aEmptyStr ) {}
    ~SwRubyListEntry();

    const String& GetText() const 				{ return sText; }
    void SetText( const String& rStr ) 			{ sText = rStr; }

    const SwFmtRuby& GetRubyAttr() const		{ return aRubyAttr; }
          SwFmtRuby& GetRubyAttr() 				{ return aRubyAttr; }
    void SetRubyAttr( const SwFmtRuby& rAttr )	{ aRubyAttr = rAttr; }
};

typedef SwRubyListEntry* SwRubyListEntryPtr;
SV_DECL_PTRARR_DEL( SwRubyList, SwRubyListEntryPtr, 0, 32 )


#endif	//_RUBYLIST_HXX
