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
#ifndef _SWUNOHELPER_HXX
#define _SWUNOHELPER_HXX

#include <tools/solar.h>
#include <sal/types.h>
#include "swdllapi.h"

namespace com { namespace sun { namespace star {
    namespace uno {
        class Any;
    }
}}}

class String;
class SvStrings;
class SvPtrarr;
        
namespace SWUnoHelper {

// calls over the compherl the getEnumAsInt32 function and handle the
// exceptions.
sal_Int32 GetEnumAsInt32( const com::sun::star::uno::Any& rVal );


// methods for UCB actions:
    // delete the file under this URL
SW_DLLPUBLIC BOOL UCB_DeleteFile( const String& rURL );

    // copy/move the file to a new location
BOOL UCB_CopyFile( const String& rURL, const String& rNewURL,
                    BOOL bCopyIsMove = FALSE );

    // is the URL on the current system case sentive?
SW_DLLPUBLIC BOOL UCB_IsCaseSensitiveFileName( const String& rURL );

    // is the URL readonly?
SW_DLLPUBLIC BOOL UCB_IsReadOnlyFileName( const String& rURL );

    // get a list of files from the folder of the URL
    // options: pExtension = 0 -> all, else this specific extension
    // 			pDateTime != 0 -> returns also the modified date/time of
    //                       the files in a SvPtrarr -->
    //                       !! objects must be deleted from the caller!!
BOOL UCB_GetFileListOfFolder( const String& rURL, SvStrings& rList,
                                const String* pExtension = 0,
                                SvPtrarr* pDateTimeList = 0 );

    // is the URL an existing file?
SW_DLLPUBLIC BOOL UCB_IsFile( const String& rURL );

    // is the URL a existing directory?
BOOL UCB_IsDirectory( const String& rURL );
}

#endif
