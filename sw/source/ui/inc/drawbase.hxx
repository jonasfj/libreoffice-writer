/*************************************************************************
 *
 *  $RCSfile: drawbase.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: os $ $Date: 2002-06-24 14:53:54 $
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
#ifndef _SW_DRAWBASE_HXX
#define _SW_DRAWBASE_HXX

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif

class SwView;
class SwWrtShell;
class SwEditWin;
class KeyEvent;
class MouseEvent;

#define MIN_FREEHAND_DISTANCE	10

/*************************************************************************
|*
|* Basisklasse fuer alle Funktionen
|*
\************************************************************************/

class SwDrawBase
{
protected:
    SwView* 		pView;
    SwWrtShell*		pSh;
    SwEditWin*		pWin;
    Point			aStartPos; 				// Position von BeginCreate
    Point			aMDPos; 				// Position von MouseButtonDown
    USHORT          nSlotId;
    BOOL            bCreateObj  :1;
    BOOL			bInsForm	:1;

public:
    SwDrawBase(SwWrtShell *pSh, SwEditWin* pWin, SwView* pView);
    virtual ~SwDrawBase();

    void		 SetDrawPointer();
    void		 EnterSelectMode(const MouseEvent& rMEvt);
    inline BOOL  IsInsertForm() const { return bInsForm; }
    inline BOOL  IsCreateObj() const { return bCreateObj; }

    // Mouse- & Key-Events; Returnwert=TRUE: Event wurde bearbeitet
    virtual BOOL KeyInput(const KeyEvent& rKEvt);
    virtual BOOL MouseMove(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonUp(const MouseEvent& rMEvt);
    virtual BOOL MouseButtonDown(const MouseEvent& rMEvt);

    void		 BreakCreate();
    void         SetSlotId(USHORT nSlot) {nSlotId = nSlot;}

    virtual void Activate(const USHORT nSlotId);	// Function aktivieren
    virtual void Deactivate();						// Function deaktivieren

    virtual void CreateDefaultObject();
};



#endif		// _SW_DRAWBASE_HXX

