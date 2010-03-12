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
#ifndef _MDIEXP_HXX
#define _MDIEXP_HXX

#include <tools/solar.h>
#include <tblenum.hxx>
#include <layout/layout.hxx>
#include <swdllapi.h>

class UniString;
class SwRect;
class Size;
class Dialog;
class ViewShell;
class SwDoc;
class SwDocShell;

extern void ScrollMDI(ViewShell* pVwSh, const SwRect &, USHORT nRangeX, USHORT nRangeY);
extern BOOL IsScrollMDI(ViewShell* pVwSh, const SwRect &);
extern void SizeNotify(ViewShell* pVwSh, const Size &);

//Update der Statusleiste, waehrend einer Action.
extern void PageNumNotify( ViewShell* pVwSh,
                            USHORT nPhyNum,
                            USHORT nVirtNum,
                           const UniString& rPg );

enum FlyMode { FLY_DRAG_START, FLY_DRAG, FLY_DRAG_END };
extern void FrameNotify( ViewShell* pVwSh, FlyMode eMode = FLY_DRAG );

SW_DLLPUBLIC void StartProgress	   ( USHORT nMessId, long nStartVal, long nEndVal, SwDocShell *pDocSh = 0 );
SW_DLLPUBLIC void EndProgress  	   ( SwDocShell *pDocSh = 0 );
SW_DLLPUBLIC void SetProgressState  ( long nPosition, SwDocShell *pDocShell );
void SetProgressText   ( USHORT nMessId, SwDocShell *pDocShell );
void RescheduleProgress( SwDocShell *pDocShell );

void EnableCmdInterface(BOOL bEnable = TRUE);

LAYOUT_NS Dialog* GetSearchDialog();

void RepaintPagePreview( ViewShell* pVwSh, const SwRect& rRect );

// ndgrf.cxx
// alle QuickDraw-Bitmaps des speziellen Docs loeschen
void DelAllGrfCacheEntries( SwDoc* pDoc );

// ChgMode fuer Tabellen aus der Konfiguration lesen
TblChgMode GetTblChgDefaultMode();

BOOL JumpToSwMark( ViewShell* pVwSh, const UniString& rMark );


#endif
