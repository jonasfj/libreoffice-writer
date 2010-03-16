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

#ifndef _BMPWIN_HXX
#define _BMPWIN_HXX

#ifndef _GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _BITMAPEX_HXX
#include <vcl/bitmapex.hxx>
#endif
#include <vcl/window.hxx>

/*--------------------------------------------------------------------
    Beschreibung:	Extended Page fuer Grafiken
 --------------------------------------------------------------------*/

class BmpWindow : public Window
{
private:
    Graphic     aGraphic;
    BitmapEx    aBmp;
    BitmapEx    aBmpHC;

    BOOL        bHorz : 1;
    BOOL        bVert : 1;
    BOOL        bGraphic : 1;
    BOOL        bLeftAlign : 1;

    void Paint(const Rectangle& rRect);

public:
    BmpWindow(Window* pPar, USHORT nId,
                const Graphic& rGraphic, const BitmapEx& rBmp, const BitmapEx& rBmpHC);
    BmpWindow(Window* pParent, const ResId rResId) :
        Window(pParent, rResId),
        bHorz(FALSE), bVert(FALSE),bGraphic(FALSE), bLeftAlign(TRUE) {}
    ~BmpWindow();
    void MirrorVert(BOOL bMirror) { bVert = bMirror; Invalidate(); }
    void MirrorHorz(BOOL bMirror) { bHorz = bMirror; Invalidate(); }
    void SetGraphic(const Graphic& rGrf);
};

#endif
