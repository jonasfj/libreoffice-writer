/*************************************************************************
 *
 *  $RCSfile: grfshex.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2001-07-23 10:04:56 $
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

#ifdef PRECOMPILED
#include "ui_pch.hxx"
#endif

#pragma hdrstop

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _GRFSH_HXX
#include <grfsh.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _TEXTSH_HXX
#include <textsh.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _SWUNDO_HXX
#include <swundo.hxx>
#endif
#ifndef _SHELLS_HRC
#include <shells.hrc>
#endif
#ifndef _CAPTION_HXX
#include <caption.hxx>
#endif
#define _SVSTDARR_STRINGSSORTDTOR
#include <svtools/svstdarr.hxx>
#ifndef _FILTER_HXX
#include <svtools/filter.hxx>
#endif
#ifndef _SVX_IMPGRF_HXX
#include <svx/impgrf.hxx>
#endif
#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _FRMFMT_HXX
#include <frmfmt.hxx>
#endif
#ifndef _FRMMGR_HXX
#include <frmmgr.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif

#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERCONTROLACCESS_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_EXTENDEDFILEPICKERELEMENTIDS_HPP_
#include <com/sun/star/ui/dialogs/ExtendedFilePickerElementIds.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_LISTBOXCONTROLACTIONS_HPP_
#include <com/sun/star/ui/dialogs/ListboxControlActions.hpp>
#endif
#ifndef _POOLFMT_HRC
#include <poolfmt.hrc>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ui::dialogs;
using namespace ::sfx2;
using namespace ::rtl;

BOOL SwTextShell::InsertGraphicDlg()
{
#ifndef ENABLE_PROP_WITHOUTLINK
#define ENABLE_PROP_WITHOUTLINK 0x08
#endif

    BOOL bReturn = FALSE;
    SwView &rVw = GetView();
    SwDocShell* pDocShell = rVw.GetDocShell();
    USHORT nHtmlMode = ::GetHtmlMode(pDocShell);
    // im HTML-Mode nur verknuepft einfuegen
    FileDialogHelper* pFileDlg = new FileDialogHelper( SFXWB_GRAPHIC );
    pFileDlg->SetTitle(SW_RESSTR(STR_INSERT_GRAPHIC ));
    Reference < XFilePicker > xFP = pFileDlg->GetFilePicker();
    Reference < XFilePickerControlAccess > xCtrlAcc(xFP, UNO_QUERY);
    if(nHtmlMode & HTMLMODE_ON)
    {
        sal_Bool bTrue = sal_True;
        Any aVal(&bTrue, ::getBooleanCppuType());
        xCtrlAcc->setValue( ExtendedFilePickerElementIds::CHECKBOX_LINK, 0, aVal);
        xCtrlAcc->enableControl( ExtendedFilePickerElementIds::CHECKBOX_LINK, sal_False);
    }

    SvStringsSortDtor aFormats;
    SwDoc* pDoc = pDocShell->GetDoc();
    const USHORT nArrLen = pDoc->GetFrmFmts()->Count();
    USHORT i;
    for( i = 0; i < nArrLen; i++ )
    {
        SwFrmFmt* pFmt = (*pDoc->GetFrmFmts())[ i ];
        if(pFmt->IsDefault() || pFmt->IsAuto())
            continue;
        String *pFormat = new String(pFmt->GetName());
        aFormats.Insert(pFormat);
    }

    // pool formats
    //
    const SvStringsDtor& rFrmPoolArr = SwStyleNameMapper::GetFrmFmtUINameArray();
    for( i = 0; i < rFrmPoolArr.Count(); i++ )
    {
        String *pFormat = new String(*rFrmPoolArr[i]);
        if (!aFormats.Insert(pFormat))
            delete pFormat;
    }

    Sequence<OUString> aListBoxEntries(aFormats.Count());
    OUString* pEntries = aListBoxEntries.getArray();
    sal_Int16 nSelect = 0;
    String sGraphicFormat = SW_RESSTR(STR_POOLFRM_GRAPHIC);
    for(i = 0; i < aFormats.Count(); ++i)
    {
        pEntries[i] = *aFormats[i];
        if(pEntries[i].equals(sGraphicFormat))
            nSelect = i;
    }
    try
    {
        Any aTemplates(&aListBoxEntries, ::getCppuType(&aListBoxEntries));

        xCtrlAcc->setValue( ExtendedFilePickerElementIds::LISTBOX_IMAGE_TEMPLATE,
            ListboxControlActions::ADD_ITEMS , aTemplates );

        Any aSelectPos(&nSelect, ::getCppuType(&nSelect));
        xCtrlAcc->setValue( ExtendedFilePickerElementIds::LISTBOX_IMAGE_TEMPLATE,
            ListboxControlActions::SET_SELECT_ITEM, aSelectPos );
    }
    catch(Exception& )
    {
        DBG_ERROR("control acces failed")
    }

    if( ERRCODE_NONE == pFileDlg->Execute() )
    {
        SwWrtShell& rSh = GetShell();
        rSh.StartAction();
        rSh.StartUndo(UNDO_INSERT);
        sal_Bool bAsLink;
        if(nHtmlMode & HTMLMODE_ON)
            bAsLink = sal_True;
        else
        {
            Any aVal = xCtrlAcc->getValue( ExtendedFilePickerElementIds::CHECKBOX_LINK, 0);
            DBG_ASSERT(aVal.hasValue(), "Value CBX_INSERT_AS_LINK not found")
            bAsLink = aVal.hasValue() ? *(sal_Bool*) aVal.getValue() : sal_True;
            Any aTemplateValue = xCtrlAcc->getValue(
                ExtendedFilePickerElementIds::LISTBOX_IMAGE_TEMPLATE,
                ListboxControlActions::GET_SELECTED_ITEM );
            OUString sTmpl;
            aTemplateValue >>= sTmpl;
            sGraphicFormat = sTmpl;
        }
        USHORT nError = InsertGraphic( pFileDlg->GetPath(), pFileDlg->GetCurrentFilter(),
                                bAsLink, ::GetGrfFilter() );

        // Format ist ungleich Current Filter, jetzt mit auto. detection
        if( nError == GRFILTER_FORMATERROR )
            nError = InsertGraphic( pFileDlg->GetPath(), aEmptyStr, bAsLink,
                                    ::GetGrfFilter() );
        if ( rSh.IsFrmSelected() )
        {
            SwFrmFmt* pFmt = pDoc->FindFrmFmtByName( sGraphicFormat );
            if(!pFmt)
                pFmt = pDoc->MakeFrmFmt(sGraphicFormat, 0);
            rSh.SetFrmFmt( pFmt );
        }

        RESOURCE_TYPE nResId = 0;
        switch( nError )
        {
            case GRFILTER_OPENERROR:
                nResId = STR_GRFILTER_OPENERROR;
                break;
            case GRFILTER_IOERROR:
                nResId = STR_GRFILTER_IOERROR;
                break;
            case GRFILTER_FORMATERROR:
                nResId = STR_GRFILTER_FORMATERROR;
                break;
            case GRFILTER_VERSIONERROR:
                nResId = STR_GRFILTER_VERSIONERROR;
                break;
            case GRFILTER_FILTERERROR:
                nResId = STR_GRFILTER_FILTERERROR;
                break;
            case GRFILTER_TOOBIG:
                nResId = STR_GRFILTER_TOOBIG;
                break;
        }

        if( nResId )
        {
            rSh.EndAction();
            InfoBox aInfoBox( rVw.GetWindow(), SW_RESSTR( nResId ));
            aInfoBox.Execute();
        }
        else
        {
            // set the specific graphic attrbutes to the graphic
            rSh.EndAction();
            bReturn = TRUE;
            rVw.AutoCaption( GRAPHIC_CAP );
        }
        rSh.EndUndo(UNDO_INSERT); // wegen moegl. Shellwechsel
    }

    DELETEZ( pFrmMgr );
    delete pFileDlg;

    return bReturn;
}


