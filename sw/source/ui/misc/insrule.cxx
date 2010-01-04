/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: insrule.cxx,v $
 * $Revision: 1.15 $
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

#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif



#include "hintids.hxx"
#include <svx/gallery.hxx>
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#include <svx/brshitem.hxx>
#include <svl/urihelper.hxx>
#include <vcl/graph.hxx>

#include "swtypes.hxx"
#include "docsh.hxx"
#include "insrule.hxx"
#include "swvset.hxx"
#include "insrule.hrc"
#include "misc.hrc"
#include "helpid.h"

/*------------------------------------------------------------------------
 Beschreibung:
------------------------------------------------------------------------*/


SwInsertGrfRulerDlg::SwInsertGrfRulerDlg( Window* pParent ) :
    SfxModalDialog(pParent, SW_RES(DLG_INSERT_RULER)),
    aSelectionFL(this, SW_RES(FL_SEL     )),
    aOkPB       (this, SW_RES(PB_OK     )),
    aCancelPB   (this, SW_RES(PB_CANCEL )),
    aHelpPB     (this, SW_RES(PB_HELP   )),

    sSimple (SW_RES(ST_SIMPLE)),
    nSelPos(USHRT_MAX),
    pExampleVS  (new SwRulerValueSet(this, SW_RES(VS_EXAMPLE )))
{
    FreeResource();
    pExampleVS->SetLineCount(6);
    pExampleVS->SetColCount(1);
    pExampleVS->SetSelectHdl(LINK(this, SwInsertGrfRulerDlg, SelectHdl));
    pExampleVS->SetDoubleClickHdl(LINK(this, SwInsertGrfRulerDlg, DoubleClickHdl));
    pExampleVS->GrabFocus();

    // Grafiknamen ermitteln
    GalleryExplorer::BeginLocking(GALLERY_THEME_RULERS);
    GalleryExplorer::FillObjList( GALLERY_THEME_RULERS, aGrfNames );
    pExampleVS->SetHelpId(HID_VS_RULER);
    Color aColor(COL_WHITE);
    pExampleVS->InsertItem( 1, 1);
    pExampleVS->SetItemText( 1, sSimple);

    for(USHORT i = 1; i <= aGrfNames.Count(); i++)
    {
        pExampleVS->InsertItem( i + 1, i);
        pExampleVS->SetItemText( i + 1, *((String*)aGrfNames.GetObject(i-1)));
    }
    pExampleVS->Show();

}
/*-----------------14.02.97 13.18-------------------

--------------------------------------------------*/

SwInsertGrfRulerDlg::~SwInsertGrfRulerDlg()
{
    GalleryExplorer::EndLocking(GALLERY_THEME_RULERS);
    delete pExampleVS;
}

/*-----------------14.02.97 13.17-------------------

--------------------------------------------------*/

String SwInsertGrfRulerDlg::GetGraphicName()
{
    String sRet;
    USHORT nSel = nSelPos - 2; //align selection position with ValueSet index
    if(nSel < aGrfNames.Count())
        sRet = URIHelper::SmartRel2Abs(
            INetURLObject(), *(String*) aGrfNames.GetObject(nSel),
            URIHelper::GetMaybeFileHdl());
    return sRet;
}

/*-----------------14.02.97 13.20-------------------

--------------------------------------------------*/

IMPL_LINK(SwInsertGrfRulerDlg, SelectHdl, ValueSet*, pVS)
{
    nSelPos = pVS->GetSelectItemId();
    aOkPB.Enable();
    return 0;
}

/*-----------------14.02.97 14.17-------------------

--------------------------------------------------*/
SwRulerValueSet::SwRulerValueSet(   Window* pParent, const ResId& rResId ) :
    SvxBmpNumValueSet(pParent, rResId)
{
    SetStyle(  GetStyle() & ~WB_ITEMBORDER     );
}
/*-----------------14.02.97 14.17-------------------

--------------------------------------------------*/
SwRulerValueSet::~SwRulerValueSet()
{
}
/*-----------------14.02.97 13.42-------------------

--------------------------------------------------*/
void __EXPORT SwRulerValueSet::UserDraw( const UserDrawEvent& rUDEvt )
{
    Rectangle aRect = rUDEvt.GetRect();
    OutputDevice*  pDev = rUDEvt.GetDevice();
    USHORT	nItemId = rUDEvt.GetItemId();
    Point aBLPos = aRect.TopLeft();

    // Itemzaehlung beginnt bei 1, und die 1. ist die einfache Linie
    if(nItemId > 1)
    {
        Graphic aGraphic;
        if(GalleryExplorer::GetGraphicObj( GALLERY_THEME_RULERS, nItemId - 2,
                                                            &aGraphic))
        {
            Size aGrfSize = aGraphic.GetPrefSize();
            if(aGrfSize.Width() && aGrfSize.Height())
            {
                int nRelGrf = aGrfSize.Height() * 100 / aGrfSize.Width();
                Size aWinSize = aRect.GetSize();
                Size aPaintSize = aWinSize;
                int nRelWin = aWinSize.Height() * 100 / aWinSize.Width();
                if(nRelGrf > nRelWin)
                {
                    aPaintSize.Width() = aWinSize.Height() * 100 / nRelGrf;
                    aBLPos.X() += (aWinSize.Width() - aPaintSize.Width()) /2;
                }
                else
                {
                    aPaintSize.Height() = aWinSize.Width() * nRelGrf/100;
                    aBLPos.Y() += (aWinSize.Height() - aPaintSize.Height()) /2;
                }
                aBLPos.X() -= aPaintSize.Width() /2;
                aBLPos.Y() -= aPaintSize.Height() /2;

                aPaintSize.Width() *= 2;
                aPaintSize.Height() *= 2;
                if(aPaintSize.Height() < 2)
                    aPaintSize.Height() = 2;
                Region aRegion = pDev->GetClipRegion();
                pDev->SetClipRegion(aRect);
                aGraphic.Draw(pDev, aBLPos, aPaintSize);
                pDev->SetClipRegion(aRegion);
            }
        }
        else
        {
            SetGrfNotFound(TRUE);
        }
    }
    else
    {
        // Text fuer einfache Linie painten
        Font aOldFont = pDev->GetFont();
        Font aFont = pDev->GetFont();
        Size aSize = aFont.GetSize();
        int nRectHeight = aRect.GetHeight();
        aSize.Height() = nRectHeight * 2 / 3;
        aFont.SetSize(aSize);
        pDev->SetFont(aFont);
        String aText(GetItemText(nItemId));
        aSize.Width() = pDev->GetTextWidth(aText);
        aSize.Height() = pDev->GetTextHeight();
        Point aPos(aBLPos);
        aPos.Y() += (nRectHeight - aSize.Height()) / 2;
        aPos.X() += (aRect.GetWidth() - aSize.Width()) / 2;
        pDev->DrawText(aPos, aText);

        pDev->SetFont(aOldFont);
    }
}

/*-----------------15.02.97 10.03-------------------

--------------------------------------------------*/

IMPL_LINK(SwInsertGrfRulerDlg, DoubleClickHdl, ValueSet*, EMPTYARG)
{
    EndDialog(RET_OK);
    return 0;
}

