/*************************************************************************
 *
 *  $RCSfile: fldpage.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:14:36 $
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

#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif
#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif

#ifndef _DBFLD_HXX //autogen
#include <dbfld.hxx>
#endif
#ifndef _FLDDAT_HXX
#include <flddat.hxx>
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _FLDEDT_HXX
#include <fldedt.hxx>
#endif
#ifndef _DOCSH_HXX
#include <docsh.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>
#endif
#ifndef _FLDTDLG_HXX
#include <fldtdlg.hxx>
#endif
#ifndef _FLDPAGE_HXX
#include <fldpage.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _GLOBALS_HRC
#include <globals.hrc>
#endif

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwFldPage::SwFldPage(Window *pParent, const ResId &rId, const SfxItemSet &rAttrSet ) :
    SfxTabPage		(pParent, rId, rAttrSet),
    nPageId			(rId.GetId()),
    bFldDlgHtmlMode	(FALSE),
    nFldDlgAktGrpSel(0),
    nTypeSel		(LISTBOX_ENTRY_NOTFOUND),
    nSelectionSel	(LISTBOX_ENTRY_NOTFOUND),
    bFldEdit		(FALSE),
    pCurFld			(0),
    bInsert			(TRUE),
    bRefresh		(FALSE)
{
//	FreeResource();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

SwFldPage::~SwFldPage()
{
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

USHORT SwFldPage::GetGroup()
{
    ASSERT (0, "GetGroup not implemented!");
    return 0;
}

/*--------------------------------------------------------------------
    Beschreibung: TabPage initialisieren
 --------------------------------------------------------------------*/

void SwFldPage::Init()
{
    SwDocShell* pDocSh = (SwDocShell*)SfxObjectShell::Current();
    BOOL bNewMode = (::GetHtmlMode(pDocSh) & HTMLMODE_ON) != 0;

    bFldEdit = GetTabDialog() == 0;
    pCurFld = aMgr.GetCurFld();

    nFldDlgAktGrpSel = GetGroup();

    if (bNewMode != bFldDlgHtmlMode)
    {
        bFldDlgHtmlMode = bNewMode;

        // Bereichslistbox initialisieren
        if (bFldDlgHtmlMode)
        {
            SwWrtShell *pSh = ::GetActiveView()->GetWrtShellPtr();

            (SwSetExpFieldType*)pSh->InsertFldType(SwSetExpFieldType(pDocSh->GetDoc(),
                                                    String::CreateFromAscii("HTML_ON"), 1));
            (SwSetExpFieldType*)pSh->InsertFldType(SwSetExpFieldType(pDocSh->GetDoc(),
                                                    String::CreateFromAscii("HTML_OFF"), 1));
        }
    }

    aMgr.GetCurFld();	// FieldManager neu initialisieren
                        // wichtig fuer Dok-Wechsel (fldtdlg:ReInitTabPage)
}

/*--------------------------------------------------------------------
     Beschreibung: Seite neu initialisieren
 --------------------------------------------------------------------*/

void SwFldPage::ActivatePage()
{
    EnableInsert(bInsert);
}

/*--------------------------------------------------------------------
     Beschreibung: Kompletter Reset; neues Feld editieren
 --------------------------------------------------------------------*/

void SwFldPage::EditNewField()
{
    nFldDlgAktGrpSel = 0;
    nTypeSel = LISTBOX_ENTRY_NOTFOUND;
    nSelectionSel = LISTBOX_ENTRY_NOTFOUND;
    bRefresh = TRUE;
    Reset(*(SfxItemSet*)0);
    bRefresh = FALSE;
}

/*--------------------------------------------------------------------
     Beschreibung: Feld einfuegen
 --------------------------------------------------------------------*/

BOOL SwFldPage::InsertFld(USHORT nTypeId, USHORT nSubType, const String& rPar1,
                            const String& rPar2, ULONG nFormatId, sal_Unicode cSeparator)
{
    BOOL bRet = FALSE;
    SwWrtShell *pSh = ::GetActiveView()->GetWrtShellPtr();

    if (!IsFldEdit())	// Neues Feld einfuegen
    {
        bRet = aMgr.InsertFld( nTypeId, nSubType, rPar1, rPar2, nFormatId, 0, cSeparator );

    }
    else	// Feld aendern
    {
        String sPar1(rPar1);
        String sPar2(rPar2);
        BOOL bDBChanged = FALSE;

        switch( nTypeId )
        {
        case TYP_DATEFLD:
        case TYP_TIMEFLD:
            nSubType = ((nTypeId == TYP_DATEFLD) ? DATEFLD : TIMEFLD) |
                       ((nSubType == DATE_VAR) ? 0 : FIXEDFLD);
            break;

        case TYP_DBNAMEFLD:
        case TYP_DBNEXTSETFLD:
        case TYP_DBNUMSETFLD:
        case TYP_DBSETNUMBERFLD:
            {
                USHORT nPos, nTablePos, nExpPos;
                String sDBName;

                // DBName aus rPar1 extrahieren. Format: DBName.TableName.ExpStrg
                if ((nTablePos = rPar1.Search(DB_DELIM)) != STRING_NOTFOUND)
                    sDBName = rPar1.Copy(0, nTablePos++);
                if ((nExpPos = rPar1.Search(DB_DELIM, nTablePos)) != STRING_NOTFOUND)
                {
                    sDBName += DB_DELIM;
                    sDBName += rPar1.Copy(nTablePos, nExpPos++ - nTablePos);
                }
                if (nExpPos != STRING_NOTFOUND)
                    nPos = nExpPos;
                else if (nTablePos != STRING_NOTFOUND)
                    nPos = nTablePos;
                else
                    nPos = 0;
                sPar1 = rPar1.Copy(nPos);

                ((SwDBNameInfField*)pCurFld)->SetDBName(sDBName);
                bDBChanged = TRUE;
            }
            break;

        case TYP_DBFLD:
            {
                String sDBName = rPar1.GetToken(0, DB_DELIM);
                sDBName += DB_DELIM;
                sDBName += rPar1.GetToken(1, DB_DELIM);
                String sColumn = rPar1.GetToken(2, DB_DELIM);

                SwDBFieldType* pOldTyp = (SwDBFieldType*)pCurFld->GetTyp();
                SwDBFieldType* pTyp = (SwDBFieldType*)pSh->InsertFldType(
                        SwDBFieldType(pSh->GetDoc(), sColumn, sDBName));

                SwClientIter aIter( *pOldTyp );

                for( SwFmtFld* pFmtFld = (SwFmtFld*)aIter.First( TYPE(SwFmtFld) );
                    pFmtFld; pFmtFld = (SwFmtFld*)aIter.Next() )
                {
                    if( pFmtFld->GetFld() == pCurFld)
                    {
                        pTyp->Add(pFmtFld);	// Feld auf neuen Typ umhaengen
                        pCurFld->ChgTyp(pTyp);
                        break;
                    }
                }
                bDBChanged = TRUE;
            }
            break;

        case TYP_SEQFLD:
            {
                SwSetExpFieldType* pTyp = (SwSetExpFieldType*)pCurFld->GetTyp();
                pTyp->SetOutlineLvl(nSubType & 0xff);
                pTyp->SetDelimiter((char)((nSubType & 0xff00) >> 8));

                nSubType = GSE_SEQ;
            }
            break;

        case TYP_INPUTFLD:
            {
                // User- oder SetField ?
                if (aMgr.GetFldType(RES_USERFLD, sPar1) == 0 &&
                !(pCurFld->GetSubType() & INP_TXT))	// SETEXPFLD
                {
                    SwSetExpField* pFld = (SwSetExpField*)pCurFld;
                    pFld->SetPromptText(sPar2);
                    sPar2 = pFld->GetPar2();
                }
            }
            break;
        }

        pSh->StartAllAction();

        pCurFld->SetSubType(nSubType);

        aMgr.UpdateCurFld( nFormatId, sPar1, sPar2 );

        switch (nTypeId)
        {
            case TYP_HIDDENTXTFLD:
            case TYP_HIDDENPARAFLD:
                aMgr.EvalExpFlds(pSh);
                break;
        }

        if (bDBChanged)	// Datenbank geaendert
        {
            switch(nTypeId)
            {
                case TYP_DBFLD:
                    ((SwDBField*)pCurFld)->ClearInitialized();
                    ((SwDBField*)pCurFld)->InitContent();
                case TYP_DBNAMEFLD:
                case TYP_DBNEXTSETFLD:
                case TYP_DBNUMSETFLD:
                case TYP_DBSETNUMBERFLD:
                    pCurFld->GetTyp()->UpdateFlds();
                    break;
            }
        }

        pSh->SetUndoNoResetModified();
        pSh->EndAllAction();
    }

    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwFldPage::SavePos(ListBox* pLst1, ListBox* pLst2, ListBox* pLst3)
{
    if (pLst1 && pLst1->GetEntryCount())
        sLst1 = pLst1->GetSelectEntry();
    else
        sLst1.Erase();

    if (pLst2 && pLst2->GetEntryCount())
        sLst2 = pLst2->GetSelectEntry();
    else
        sLst2.Erase();

    if (pLst3 && pLst3->GetEntryCount())
        sLst3 = pLst3->GetSelectEntry();
    else
        sLst3.Erase();
}

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

void SwFldPage::RestorePos(ListBox* pLst1, ListBox* pLst2, ListBox* pLst3)
{
    USHORT nPos = 0;

    if (pLst1 && pLst1->GetEntryCount() && sLst1.Len() && (nPos = pLst1->GetEntryPos(sLst1)) != LISTBOX_ENTRY_NOTFOUND)
        pLst1->SelectEntryPos(nPos);
    if (pLst2 && pLst2->GetEntryCount() && sLst1.Len() && (nPos = pLst2->GetEntryPos(sLst1)) != LISTBOX_ENTRY_NOTFOUND)
        pLst2->SelectEntryPos(nPos);
    if (pLst3 && pLst3->GetEntryCount() && sLst1.Len() && (nPos = pLst3->GetEntryPos(sLst1)) != LISTBOX_ENTRY_NOTFOUND)
        pLst3->SelectEntryPos(nPos);
}

/*--------------------------------------------------------------------
     Beschreibung: Einfuegen von neuen Feldern
 --------------------------------------------------------------------*/

IMPL_LINK( SwFldPage, InsertHdl, Button *, pBtn )
{
    SwFldDlg *pDlg = (SwFldDlg*)GetTabDialog();

    if (pDlg)
    {
        pDlg->InsertHdl();

        if (pBtn)
            pBtn->GrabFocus();	// Wegen InputField-Dlg
    }
    else
    {
        SwFldEditDlg *pEditDlg = (SwFldEditDlg *)GetParent();
        pEditDlg->InsertHdl();
    }

    return 0;
}

/*--------------------------------------------------------------------
     Beschreibung: "Einfuegen"-Button Enablen/Disablen
 --------------------------------------------------------------------*/

void SwFldPage::EnableInsert(BOOL bEnable)
{
    SwFldDlg *pDlg = (SwFldDlg*)GetTabDialog();

    if (pDlg)
    {
        if (pDlg->GetCurPageId() == nPageId)
            pDlg->EnableInsert(bEnable);
    }
    else
    {
        SwFldEditDlg *pEditDlg = (SwFldEditDlg *)GetParent();
        pEditDlg->EnableInsert(bEnable);
    }

    bInsert = bEnable;
}

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwFldPage, NumFormatHdl, ListBox *, pLst )
{
    InsertHdl();

    return 0;
}

/*------------------------------------------------------------------------

    $Log: not supported by cvs2svn $
    Revision 1.25  2000/09/18 16:05:29  willem.vandorp
    OpenOffice header added.
    
    Revision 1.24  2000/06/30 08:52:53  os
    #76541# string assertions removed
    
    Revision 1.23  2000/05/10 11:55:13  os
    Basic API removed

    Revision 1.22  2000/04/18 15:17:32  os
    UNICODE

    Revision 1.21  2000/02/11 14:46:39  hr
    #70473# changes for unicode ( patched by automated patchtool )

    Revision 1.20  1999/01/20 13:16:52  JP
    Task #58677#: Crsr in Readonly Bereichen zulassen


      Rev 1.19   20 Jan 1999 14:16:52   JP
   Task #58677#: Crsr in Readonly Bereichen zulassen

      Rev 1.18   29 Oct 1998 14:08:32   OM
   #58544# Butzerfeld: richtigen Inhalt anzeigen

      Rev 1.17   15 Oct 1998 16:02:52   OM
   #57965# Variablennamen fuer Eingabefeld verwenden

      Rev 1.16   18 Jun 1998 11:16:44   OM
   Kapitelebenen fuer Nummernkreise

      Rev 1.15   27 Mar 1998 16:26:18   OM
   #48007# UpdateExpFlds bei Aenderung von HiddenText und HiddenParaField

      Rev 1.14   18 Mar 1998 10:33:56   OM
   #48197# Focus restaurieren nach InputDlg

      Rev 1.13   06 Mar 1998 14:54:24   OM
   Nur bei Aenderung Feld aktualisieren

      Rev 1.12   08 Jan 1998 13:36:20   OM
   Feldbefehl-Traveling

      Rev 1.11   08 Jan 1998 10:19:04   OM
   Referenzen editieren

      Rev 1.10   06 Jan 1998 18:13:34   OM
   Felbefehl-Dlg

      Rev 1.9   19 Dec 1997 18:25:00   OM
   Feldbefehl-bearbeiten Dlg

      Rev 1.8   16 Dec 1997 17:01:44   OM
   Feldbefehle bearbeiten

      Rev 1.7   12 Dec 1997 16:10:08   OM
   AutoUpdate bei FocusWechsel u.a.

      Rev 1.6   11 Dec 1997 16:58:50   OM
   Feldumstellung

      Rev 1.5   09 Dec 1997 17:16:46   OM
   Kein alter Feldbefehl-Dialog mehr

      Rev 1.4   28 Nov 1997 19:51:32   MA
   includes

      Rev 1.3   20 Nov 1997 17:02:04   OM
   Neuer Felddialog

      Rev 1.2   19 Nov 1997 16:30:42   OM
   Datenbank-TP Drag&Drop

      Rev 1.1   18 Nov 1997 10:34:36   OM
   Neuer Feldbefehldialog

      Rev 1.0   17 Nov 1997 09:07:00   OM
   Initial revision.

------------------------------------------------------------------------*/


