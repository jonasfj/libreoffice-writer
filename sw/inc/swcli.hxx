/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: swcli.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:12:19 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#ifndef _SWCLI_HXX
#define _SWCLI_HXX

#ifndef _COM_SUN_STAR_EMBED_XEMBEDDEDOBJECT_HPP_
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#endif

#include <svtools/embedhlp.hxx>

#ifndef _SFX_CLIENTSH_HXX //autogen
#include <sfx2/ipclient.hxx>
#endif

class SwView;
class SwEditWin;

class SwOleClient : public SfxInPlaceClient
{
    BOOL bInDoVerb;
    BOOL bOldCheckForOLEInCaption;

    virtual void ObjectAreaChanged();
    virtual void RequestNewObjectArea( Rectangle& );
    virtual void ViewChanged();
    virtual void MakeVisible();

public:
    SwOleClient( SwView *pView, SwEditWin *pWin, const svt::EmbeddedObjectRef& );

    void SetInDoVerb( BOOL bFlag )				{ bInDoVerb = bFlag; }
    BOOL IsInDoVerb() const						{ return bInDoVerb; }

    BOOL IsCheckForOLEInCaption() const			{ return bOldCheckForOLEInCaption; }
};

#endif
