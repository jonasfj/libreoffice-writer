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
#ifndef _SWSERV_HXX
#define _SWSERV_HXX

#include <sfx2/linksrc.hxx>
#include <IMark.hxx>

class SwSectionNode;
class SwBaseLink;
class SwTableNode;
struct SwPosition;
class SwPaM;

class SwServerObject : public ::sfx2::SvLinkSource
{
    using sfx2::SvLinkSource::SendDataChanged;

protected:
    enum ServerModes { BOOKMARK_SERVER, TABLE_SERVER, SECTION_SERVER, NONE_SERVER } eType;
    union {
        ::sw::mark::IMark* pBkmk;
        SwTableNode* pTblNd;
        SwSectionNode* pSectNd;
    } CNTNT_TYPE;

    SwServerObject();

public:
    SwServerObject( ::sw::mark::IMark& rBookmark )
        : eType( BOOKMARK_SERVER )
    {
        CNTNT_TYPE.pBkmk = &rBookmark;
    }
    SwServerObject( SwTableNode& rTableNd )
        : eType( TABLE_SERVER )
    {
        CNTNT_TYPE.pTblNd = &rTableNd;
    }
    SwServerObject( SwSectionNode& rSectNd )
        : eType( SECTION_SERVER )
    {
        CNTNT_TYPE.pSectNd = &rSectNd;
    }
    virtual ~SwServerObject();

    virtual BOOL GetData( ::com::sun::star::uno::Any & rData,
                            const String & rMimeType,
                            BOOL bSynchron = FALSE );

    BOOL SetData( const String & rMimeType,
                    const ::com::sun::star::uno::Any& rData );

    virtual void SendDataChanged( const SwPosition& rPos );
    virtual void SendDataChanged( const SwPaM& rRange );

    BOOL IsLinkInServer( const SwBaseLink* ) const;

    void SetNoServer();
    void SetDdeBookmark( ::sw::mark::IMark& rBookmark);
};

#ifndef SW_DECL_SWSERVEROBJECT_DEFINED
#define SW_DECL_SWSERVEROBJECT_DEFINED
SV_DECL_REF( SwServerObject )
#endif

#endif  // _SWSERV_HXX

