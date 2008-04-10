/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: srcview.hxx,v $
 * $Revision: 1.9 $
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
#ifndef _SRCVIEW_HXX
#define _SRCIEW_HXX

#include <sfx2/viewfac.hxx>
#include <sfx2/viewsh.hxx>
#include "srcedtw.hxx"
#include "shellid.hxx"

class SwDocShell;
class SvxSearchItem;
class SfxMedium;

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

class SwSrcView: public SfxViewShell
{
    SwSrcEditWindow 	aEditWin;

    SvxSearchItem*		pSearchItem;

    BOOL				bSourceSaved	:1;
    rtl_TextEncoding    eLoadEncoding;
    void                Init();

    // fuer Readonly-Umschaltung
    virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );


protected:
    USHORT 				StartSearchAndReplace(const SvxSearchItem& rItem,
                                                        BOOL bFromStart,
                                                        BOOL bApi,
                                                        BOOL bRecursive = FALSE);


public:

    SFX_DECL_VIEWFACTORY(SwSrcView);
    SFX_DECL_INTERFACE(SW_SRC_VIEWSHELL)
    TYPEINFO();


    SwSrcView(SfxViewFrame* pFrame, SfxViewShell*);

    ~SwSrcView();

    SwDocShell*			GetDocShell();
    SwSrcEditWindow&    GetEditWin() {return aEditWin;}
    void				SaveContent(const String& rTmpFile);
    void				SaveContentTo(SfxMedium& rMed);

    BOOL				IsModified() {return aEditWin.IsModified();}



    void			Execute(SfxRequest&);
    void			GetState(SfxItemSet&);

    SvxSearchItem* 	GetSearchItem();
    void 			SetSearchItem( const SvxSearchItem& rItem );

    void			Load(SwDocShell* pDocShell);

    virtual USHORT	SetPrinter(	SfxPrinter* pNew,
                                        USHORT nDiff = SFX_PRINTER_ALL, bool bIsAPI=false );
    virtual ErrCode DoPrint( SfxPrinter *pPrinter, PrintDialog *pPrintDialog, BOOL bSilent, BOOL bIsAPI );
    virtual			SfxPrinter* 	GetPrinter( BOOL bCreate = FALSE );

    void			SourceSaved() {bSourceSaved = TRUE;}
    BOOL			HasSourceSaved() const {return bSourceSaved;}

};


#endif





















