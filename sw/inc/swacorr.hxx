/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: swacorr.hxx,v $
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
#ifndef _SWACORR_HXX
#define _SWACORR_HXX

#include <svx/svxacorr.hxx>

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/embed/XStorage.hpp>

class SwAutoCorrect : public SvxAutoCorrect
{
    using  SvxAutoCorrect::PutText;

protected:
    //	- return den Ersetzungstext (nur fuer SWG-Format, alle anderen
    //		koennen aus der Wortliste herausgeholt werden!)
    //		rShort ist der Stream-Name - gecryptet!
    virtual BOOL GetLongText( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >& , const String& rFileName, const String& rShort, String& rLong );
    
    //	- Text mit Attributierung (kann nur der SWG - SWG-Format!)
    //		rShort ist der Stream-Name - gecryptet!
    virtual BOOL PutText( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >&, const String& rFileName, const String& rShort, SfxObjectShell& ,
                            String& );

public:
    TYPEINFO();

    SwAutoCorrect( const SvxAutoCorrect& rACorr );
    virtual ~SwAutoCorrect();
};


#endif

