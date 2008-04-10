/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: glshell.cxx,v $
 * $Revision: 1.17 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#include <tools/list.hxx>
#include <svtools/eitem.hxx>
#include <svtools/stritem.hxx>
#include <sfx2/printer.hxx>
#include <sfx2/request.hxx>
#include <sfx2/sfxsids.hrc>
#include <svx/srchitem.hxx>
#include <svtools/macitem.hxx>
#include <gloshdl.hxx>

#include <svx/acorrcfg.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/viewfrm.hxx>
#include <uitool.hxx>
#include <wrtsh.hxx>
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _GLSHELL_HXX
#include <glshell.hxx>
#endif
#include <doc.hxx>
#include <glosdoc.hxx>
#include <shellio.hxx>
#include <initui.hxx>					// fuer ::GetGlossaries()
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _SWERROR_H
#include <swerror.h>
#endif
#ifndef _MISC_HRC
#include <misc.hrc>
#endif


#define SwWebGlosDocShell
#define SwGlosDocShell
#ifndef _ITEMDEF_HXX
#include <itemdef.hxx>
#endif
#ifndef _SWSLOTS_HXX
#include <swslots.hxx>
#endif


SFX_IMPL_INTERFACE( SwGlosDocShell, SwDocShell, SW_RES(0) )
{
}

SFX_IMPL_INTERFACE( SwWebGlosDocShell, SwWebDocShell, SW_RES(0) )
{
}


TYPEINIT1( SwGlosDocShell, SwDocShell );
TYPEINIT1( SwWebGlosDocShell, SwWebDocShell );


void lcl_Execute( SwDocShell& rSh, SfxRequest& rReq )
{
    if ( rReq.GetSlot() == SID_SAVEDOC )
    {
        if( !rSh.HasName() )
        {
            rReq.SetReturnValue( SfxBoolItem( 0, rSh.Save() ) );
        }
        else
        {
            const SfxBoolItem* pRes = ( const SfxBoolItem* )
                                        rSh.ExecuteSlot( rReq,
                                        rSh.SfxObjectShell::GetInterface() );
            if( pRes->GetValue() )
                rSh.GetDoc()->ResetModified();
        }
    }
}


void lcl_GetState( SwDocShell& rSh, SfxItemSet& rSet )
{
    if( SFX_ITEM_AVAILABLE >= rSet.GetItemState( SID_SAVEDOC, FALSE ))
    {
        if( !rSh.GetDoc()->IsModified() )
            rSet.DisableItem( SID_SAVEDOC );
        else
            rSet.Put( SfxStringItem( SID_SAVEDOC, SW_RESSTR(STR_SAVE_GLOSSARY)));
    }
}


BOOL lcl_Save( SwWrtShell& rSh, const String& rGroupName,
                const String& rShortNm, const String& rLongNm )
{
    const SvxAutoCorrCfg* pCfg = SvxAutoCorrCfg::Get();
    SwTextBlocks * pBlock = ::GetGlossaries()->GetGroupDoc( rGroupName );

    SvxMacro aStart(aEmptyStr, aEmptyStr);
    SvxMacro aEnd(aEmptyStr, aEmptyStr);
    SwGlossaryHdl* pGlosHdl;

    pGlosHdl = rSh.GetView().GetGlosHdl();
    pGlosHdl->GetMacros( rShortNm, aStart, aEnd, pBlock );

    USHORT nRet = rSh.SaveGlossaryDoc( *pBlock, rLongNm, rShortNm,
                                pCfg->IsSaveRelFile(),
                                pBlock->IsOnlyTextBlock( rShortNm ) );

    if(aStart.GetMacName().Len() || aEnd.GetMacName().Len() )
    {
        SvxMacro* pStart = aStart.GetMacName().Len() ? &aStart : 0;
        SvxMacro* pEnd = aEnd.GetMacName().Len() ? &aEnd : 0;
        pGlosHdl->SetMacros( rShortNm, pStart, pEnd, pBlock );
    }

    rSh.EnterStdMode();
    if( USHRT_MAX != nRet )
        rSh.ResetModified();
    delete pBlock;
    return nRet != USHRT_MAX;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwGlosDocShell::SwGlosDocShell( sal_Bool bNewShow)
    :
    SwDocShell( bShow ? SFX_CREATE_MODE_STANDARD : SFX_CREATE_MODE_INTERNAL )
    ,bShow ( bNewShow )
{
    SetHelpId(SW_GLOSDOCSHELL);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwGlosDocShell::~SwGlosDocShell(  )
{
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwGlosDocShell::Execute( SfxRequest& rReq )
{
    ::lcl_Execute( *this, rReq );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwGlosDocShell::GetState( SfxItemSet& rSet )
{
    ::lcl_GetState( *this, rSet );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


BOOL SwGlosDocShell::Save()
{
    // In case of an API object which holds this document, it is possible that the WrtShell is already
    // dead. For instance, if the doc is modified via this API object, and then, upon office shutdown,
    // the document's view is closed (by the SFX framework) _before_ the API object is release and
    // tries to save the doc, again.
    // 96380 - 2002-03-03 - fs@openoffice.org
    if ( GetWrtShell() )
        return ::lcl_Save( *GetWrtShell(), aGroupName, aShortName, aLongName );
    else
    {
        SetModified( FALSE );
        return FALSE;
    }
}


/**/

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwWebGlosDocShell::SwWebGlosDocShell()
    : SwWebDocShell( SFX_CREATE_MODE_STANDARD )
{
    SetHelpId(SW_WEBGLOSDOCSHELL);
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


SwWebGlosDocShell::~SwWebGlosDocShell(  )
{
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwWebGlosDocShell::Execute( SfxRequest& rReq )
{
    ::lcl_Execute( *this, rReq );
}
/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


void SwWebGlosDocShell::GetState( SfxItemSet& rSet )
{
    ::lcl_GetState( *this, rSet );
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/


BOOL SwWebGlosDocShell::Save()
{
    // same comment as in SwGlosDocShell::Save - see there
    if ( GetWrtShell() )
        return ::lcl_Save( *GetWrtShell(), aGroupName, aShortName, aLongName );
    else
    {
        SetModified( FALSE );
        return FALSE;
    }
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SV_IMPL_REF ( SwDocShell )

SwDocShellRef SwGlossaries::EditGroupDoc( const String& rGroup, const String& rShortName, BOOL bShow )
{
    SwDocShellRef xDocSh;

    SwTextBlocks* pGroup = GetGroupDoc( rGroup );
    if( pGroup && pGroup->GetCount() )
    {
        // erfrage welche View registriert ist. Im WebWriter gibts es keine
        // normale View
        USHORT nViewId = 0 != &SwView::Factory() ? 2 : 6;
        String sLongName = pGroup->GetLongName(pGroup->GetIndex( rShortName ));

        if( 6 == nViewId )
        {
            SwWebGlosDocShell* pDocSh = new SwWebGlosDocShell();
            xDocSh = pDocSh;
            pDocSh->DoInitNew( 0 );
            pDocSh->SetLongName( sLongName );
            pDocSh->SetShortName( rShortName);
            pDocSh->SetGroupName( rGroup );
        }
        else
        {
            SwGlosDocShell* pDocSh = new SwGlosDocShell(bShow);
            xDocSh = pDocSh;
            pDocSh->DoInitNew( 0 );
            pDocSh->SetLongName( sLongName );
            pDocSh->SetShortName( rShortName );
            pDocSh->SetGroupName( rGroup );
        }

        // Dokumenttitel setzen
        SfxViewFrame* pFrame = SfxViewFrame::CreateViewFrame( *xDocSh, nViewId, !bShow );
        String aDocTitle(SW_RES( STR_GLOSSARY ));
        aDocTitle += ' ';
        aDocTitle += sLongName;

        BOOL bDoesUndo = xDocSh->GetDoc()->DoesUndo();
        xDocSh->GetDoc()->DoUndo( FALSE );

        xDocSh->GetWrtShell()->InsertGlossary( *pGroup, rShortName );
        if( !xDocSh->GetDoc()->getPrinter( false ) )
        {
            // wir erzeugen einen default SfxPrinter.
            // Das ItemSet wird vom Sfx geloescht!
            SfxItemSet *pSet = new SfxItemSet( xDocSh->GetDoc()->GetAttrPool(),
                        FN_PARAM_ADDPRINTER, FN_PARAM_ADDPRINTER,
                        SID_PRINTER_NOTFOUND_WARN, SID_PRINTER_NOTFOUND_WARN,
                        SID_PRINTER_CHANGESTODOC, SID_PRINTER_CHANGESTODOC,
                        0 );
            SfxPrinter* pPrinter = new SfxPrinter( pSet );

            // und haengen ihn ans Dokument.
            xDocSh->GetDoc()->setPrinter( pPrinter, true, true );
        }

        xDocSh->SetTitle( aDocTitle );
        xDocSh->GetDoc()->DoUndo( bDoesUndo );
        xDocSh->GetDoc()->ResetModified();
        if ( bShow )
            pFrame->GetFrame()->Appear();

        delete pGroup;
    }
    return xDocSh;
}


