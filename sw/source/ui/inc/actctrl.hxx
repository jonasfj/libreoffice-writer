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
#ifndef _ACTCTRL_HXX
#define _ACTCTRL_HXX

#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#include "swdllapi.h"

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
    NoSpaceEdit( Window* pParent, const ResId& rResId);
    virtual ~NoSpaceEdit();
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

    void SetReturnActionLink(const Link& rLink)
            { aReturnActionLink = rLink;}
};        

#endif
