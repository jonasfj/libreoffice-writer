/*************************************************************************
 *
 *  $RCSfile: actctrl.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-27 11:55:59 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef _ACTCTRL_HXX
#define _ACTCTRL_HXX

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif

/*--------------------------------------------------------------------
    Beschreibung:	numerische Eingabe
 --------------------------------------------------------------------*/

class SW_DLLPUBLIC NumEditAction: public NumericField
{
    Link aActionLink;

protected:
    virtual	void Action();
    virtual long	Notify( NotifyEvent& rNEvt );
public:
    NumEditAction( Window* pParent, const ResId& rResId ) :
                                NumericField(pParent, rResId) {}

    void 		SetActionHdl( const Link& rLink )	{ aActionLink = rLink;}
    const Link& GetActionHdl() const 				{ return aActionLink; }
};


/* -----------------21.04.98 08:11-------------------
 * Edit, dass keine Spaces akzeptiert
 * --------------------------------------------------*/
class SW_DLLPUBLIC NoSpaceEdit : public Edit
{
    String sForbiddenChars;
protected:
    virtual void KeyInput( const KeyEvent& );
    virtual void		Modify();

public:
    NoSpaceEdit( Window* pParent, const ResId& rResId)
        : Edit(pParent, rResId),
        sForbiddenChars(String::CreateFromAscii(" "))
    {}
    void			SetForbiddenChars(const String& rSet){sForbiddenChars = rSet;}
    const String& 	GetForbiddenChars(){return sForbiddenChars;}
};

/* -----------------21.04.98 08:33-------------------
 * Kein Space und kein Punkt
 * --------------------------------------------------*/
class TableNameEdit : public NoSpaceEdit
{
public:
    TableNameEdit(Window* pWin, const ResId& rResId) :
                            NoSpaceEdit(pWin, rResId)
                                {SetForbiddenChars(String::CreateFromAscii(" .<>"));}
};
/* -----------------25.06.2003 15:55-----------------
    call a link when KEY_RETURN is pressed
 --------------------------------------------------*/
class SW_DLLPUBLIC ReturnActionEdit : public Edit
{
    Link    aReturnActionLink;
public:
    ReturnActionEdit( Window* pParent, const ResId& rResId)
        : Edit(pParent, rResId){}
    ~ReturnActionEdit();
    virtual void KeyInput( const KeyEvent& );

    void SetReturnActionLink(Link& rLink)
            { aReturnActionLink = rLink;}
};        

#endif
