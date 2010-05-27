/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile:  $
 *
 * $Revision:  $
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

#ifndef _SIDEBARTXTCONTROL_HXX
#define _SIDEBARTXTCONTROL_HXX

#include <vcl/ctrl.hxx>

class OutlinerView;
class SwView;
class SwPostItMgr;
class SpellCallbackInfo;

namespace sw { namespace sidebarwindows {

class SwSidebarWin;

class SidebarTxtControl : public Control
{
    private:
        SwSidebarWin& mrSidebarWin;
        SwView& mrDocView;
        SwPostItMgr& mrPostItMgr;

        bool mbMouseOver;

    protected:
        virtual void	Paint( const Rectangle& rRect);
        virtual void    KeyInput( const KeyEvent& rKeyEvt );
        virtual void	MouseMove( const MouseEvent& rMEvt );
        virtual void	MouseButtonDown( const MouseEvent& rMEvt );
        virtual void	MouseButtonUp( const MouseEvent& rMEvt );
        virtual void	Command( const CommandEvent& rCEvt );
        virtual void 	LoseFocus();
        virtual void	RequestHelp(const HelpEvent &rEvt);
        virtual XubString GetSurroundingText() const;
        virtual Selection GetSurroundingTextSelection() const;

        DECL_LINK( Select, Menu* );

    public:
        SidebarTxtControl( SwSidebarWin& rSidebarWin,
                           WinBits nBits,
                           SwView& rDocView,
                           SwPostItMgr& rPostItMgr );
        ~SidebarTxtControl();

        virtual void GetFocus();

        OutlinerView* GetTextView() const;

        DECL_LINK( OnlineSpellCallback, SpellCallbackInfo*);

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > CreateAccessible();
};

} } // end of namespace sw::sidebarwindows

#endif
