/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: accfrmobj.hxx,v $
 * $Revision: 1.16 $
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
#ifndef _ACCFRMOBJ_HXX
#define _ACCFRMOBJ_HXX

#include <sal/types.h>

class SwAccessibleMap;
class SwFrm;
class SdrObject;
class Window;
class SwRect;

namespace sw { namespace access {

class SwAccessibleChild
{
    public:
        SwAccessibleChild();
        explicit SwAccessibleChild( const SdrObject* pDrawObj );
        explicit SwAccessibleChild( const SwFrm* pFrm );
        explicit SwAccessibleChild( Window* pWindow );
        SwAccessibleChild( const SwFrm* pFrm,
                           const SdrObject* pDrawObj );

        SwAccessibleChild( const SwAccessibleChild& r );
        SwAccessibleChild& operator=( const SwAccessibleChild& r );

        SwAccessibleChild& operator=( const SdrObject* pDrawObj );
        SwAccessibleChild& operator=( const SwFrm* pFrm );
        SwAccessibleChild& operator=( Window* pWindow );

        bool operator==( const SwAccessibleChild& r ) const;

        bool IsValid() const;

        const SwFrm* GetSwFrm() const;
        const SdrObject* GetDrawObject() const;
        Window* GetWindow() const;

        const SwFrm* GetParent( const sal_Bool bInPagePreview ) const;

        bool IsAccessible( sal_Bool bPagePreview ) const;
        bool IsBoundAsChar() const;

        bool IsVisibleChildrenOnly() const;
        SwRect GetBox( const SwAccessibleMap& rAccMap ) const;
        SwRect GetBounds( const SwAccessibleMap& rAccMap ) const;

    private:
        const SwFrm* mpFrm;
        const SdrObject* mpDrawObj;
        Window* mpWindow;

        void Init( const SdrObject* pDrawObj );
        void Init( const SwFrm* pFrm );
        void Init( Window* pWindow );
};


} } // eof of namespace sw::access

#endif
