/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: viewopt.cxx,v $
 *
 *  $Revision: 1.29 $
 *
 *  last change: $Author: ihi $ $Date: 2007-11-26 17:33:44 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#ifndef _SVX_HTMLMODE_HXX //autogen
#include <svx/htmlmode.hxx>
#endif
#include <svx/htmlcfg.hxx>

#ifndef _SVX_SVXIDS_HRC //autogen
#include <svx/svxids.hrc>
#endif
#ifndef _SVX_SVXENUM_HXX //autogen
#include <svx/svxenum.hxx>
#endif
#ifndef _MySVXACORR_HXX //autogen
#include <svx/svxacorr.hxx>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _REGION_HXX //autogen
#include <vcl/region.hxx>
#endif
#ifndef _OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _SWMODULE_HXX //autogen
#include <swmodule.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _VIEWOPT_HXX
#include <viewopt.hxx>
#endif
#ifndef _WDOCSH_HXX
#include <wdocsh.hxx>
#endif
#ifndef _SWRECT_HXX
#include <swrect.hxx>
#endif
#ifndef _CRSTATE_HXX
#include <crstate.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_COLORCFG_HXX
#include <svtools/colorcfg.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX
#include <svtools/accessibilityoptions.hxx>
#endif

#include <svx/acorrcfg.hxx>

#ifndef PRODUCT
BOOL   SwViewOption::bTest9 = FALSE;		//DrawingLayerNotLoading
#endif
Color SwViewOption::aDocBoundColor(COL_LIGHTGRAY);
Color SwViewOption::aObjectBoundColor(COL_LIGHTGRAY);
Color SwViewOption::aDocColor(COL_LIGHTGRAY);
Color SwViewOption::aAppBackgroundColor(COL_LIGHTGRAY);
Color SwViewOption::aTableBoundColor(COL_LIGHTGRAY);
Color SwViewOption::aIndexShadingsColor(COL_LIGHTGRAY);
Color SwViewOption::aLinksColor(COL_BLUE);
Color SwViewOption::aVisitedLinksColor(COL_RED);
Color SwViewOption::aDirectCursorColor(COL_BLUE);
Color SwViewOption::aTextGridColor(COL_LIGHTGRAY);
Color SwViewOption::aSpellColor(COL_LIGHTRED);
Color SwViewOption::aSmarttagColor(COL_LIGHTMAGENTA);
Color SwViewOption::aFontColor(COL_BLACK);
Color SwViewOption::aFieldShadingsColor(COL_LIGHTGRAY);
Color SwViewOption::aSectionBoundColor(COL_LIGHTGRAY);
Color SwViewOption::aPageBreakColor(COL_BLUE);
Color SwViewOption::aNotesIndicatorColor(COL_YELLOW);
Color SwViewOption::aScriptIndicatorColor(COL_GREEN);

sal_Int32 SwViewOption::nAppearanceFlags = VIEWOPT_DOC_BOUNDARIES|VIEWOPT_OBJECT_BOUNDARIES;
USHORT SwViewOption::nPixelTwips = 0;   //ein Pixel auf dem Bildschirm


#define LINEBREAK_SIZE 12, 8
#define TAB_SIZE 12, 6

#define MIN_BLANKWIDTH		 40
#define MIN_BLANKHEIGHT 	 40
#define MIN_TABWIDTH		120
#define MIN_TABHEIGHT		200

static const char __FAR_DATA aPostItStr[] = "  ";

/*************************************************************************
 *					  SwViewOption::IsEqualFlags()
 *************************************************************************/

BOOL SwViewOption::IsEqualFlags( const SwViewOption &rOpt ) const
{
    return	nCoreOptions == rOpt.nCoreOptions
            && nCore2Options == rOpt.nCore2Options
            && aSnapSize	== rOpt.aSnapSize
            && nDivisionX	== rOpt.GetDivisionX()
            && nDivisionY	== rOpt.GetDivisionY()
            && nPagePrevRow == rOpt.GetPagePrevRow()
            && nPagePrevCol == rOpt.GetPagePrevCol()
            && aRetoucheColor == rOpt.GetRetoucheColor()
            && bFormView == rOpt.IsFormView()
#ifndef PRODUCT
            // korrespondieren zu den Angaben in ui/config/cfgvw.src
            && bTest1 == rOpt.IsTest1()
            && bTest2 == rOpt.IsTest2()
            && bTest3 == rOpt.IsTest3()
            && bTest4 == rOpt.IsTest4()
            && bTest5 == rOpt.IsTest5()
            && bTest6 == rOpt.IsTest6()
            && bTest7 == rOpt.IsTest7()
            && bTest8 == rOpt.IsTest8()
            && bTest10 == rOpt.IsTest10()
#endif
            ;
}

/*************************************************************************
 *					  SwViewOption::DrawRect()
 *************************************************************************/

void SwViewOption::DrawRect( OutputDevice *pOut,
                             const SwRect &rRect, long nCol ) const
{
    if ( pOut->GetOutDevType() != OUTDEV_PRINTER )
    {
        const Color aCol( nCol );
        const Color aOldColor( pOut->GetFillColor() );
        pOut->SetFillColor( aCol );
        pOut->DrawRect( rRect.SVRect() );
        pOut->SetFillColor( aOldColor );
    }
    else
        DrawRectPrinter( pOut, rRect );
}

/*************************************************************************
 *					  SwViewOption::DrawRectPrinter()
 *************************************************************************/

void SwViewOption::DrawRectPrinter( OutputDevice *pOut,
                                    const SwRect &rRect ) const
{
    Color aOldColor(pOut->GetLineColor());
    Color aOldFillColor( pOut->GetFillColor() );
    pOut->SetLineColor( Color(COL_BLACK) );
    pOut->SetFillColor( Color(COL_TRANSPARENT ));
    pOut->DrawRect( rRect.SVRect() );
    pOut->SetFillColor( aOldFillColor );
    pOut->SetLineColor( aOldColor );
}

/*************************************************************************
 *					  SwViewOption::GetPostItsWidth()
 *************************************************************************/

USHORT SwViewOption::GetPostItsWidth( const OutputDevice *pOut ) const
{
    ASSERT( pOut, "no Outdev" );
    return USHORT(pOut->GetTextWidth( String::CreateFromAscii(aPostItStr )));
}

/*************************************************************************
 *					  SwViewOption::PaintPostIts()
 *************************************************************************/

void SwViewOption::PaintPostIts( OutputDevice *pOut, const SwRect &rRect,
                                 sal_Bool bIsScript ) const
{
    if( pOut )
    {
        Color aOldLineColor( pOut->GetLineColor() );
        pOut->SetLineColor( Color(COL_GRAY ) );
        // Wir ziehen ueberall zwei Pixel ab, damit es schick aussieht
        USHORT nPix = GetPixelTwips() * 2;
        if( rRect.Width() <= 2 * nPix || rRect.Height() <= 2 * nPix )
            nPix = 0;
        const Point aTopLeft(  rRect.Left()  + nPix, rRect.Top()	+ nPix );
        const Point aBotRight( rRect.Right() - nPix, rRect.Bottom() - nPix );
        const SwRect aRect( aTopLeft, aBotRight );
        sal_Int32 nColor = bIsScript ? aScriptIndicatorColor.GetColor() : aNotesIndicatorColor.GetColor();
        DrawRect( pOut, aRect, nColor );
        pOut->SetLineColor( aOldLineColor );
    }
}


/*************************************************************************
|*
|*	ViewOption::ViewOption()
|*
|*	Letzte Aenderung	MA 04. Aug. 93
|*
|*************************************************************************/

SwViewOption::SwViewOption() :
    aRetoucheColor( COL_TRANSPARENT ),
    nPagePrevRow( 1 ),
    nPagePrevCol( 2 ),
    nShdwCrsrFillMode( FILL_TAB ),
    bReadonly(FALSE),
    bStarOneSetting(FALSE),
    bIsPagePreview(FALSE),
    bSelectionInReadonly(FALSE),
    // --> FME 2004-06-29 #114856# Formular view
    bFormView(FALSE),
    // <--
    bBookview(FALSE),
    bShowPlaceHolderFields( sal_True ),

    nZoom( 100 ),
    eZoom( SVX_ZOOM_PERCENT ),
    nTblDest(TBL_DEST_CELL)
{
    // Initialisierung ist jetzt etwas einfacher
    // alle Bits auf 0
    nCoreOptions = 	VIEWOPT_1_IDLE | VIEWOPT_1_HARDBLANK | VIEWOPT_1_SOFTHYPH |
                    VIEWOPT_1_REF |
                    VIEWOPT_1_GRAPHIC |
                    VIEWOPT_1_TABLE	   | VIEWOPT_1_DRAW | VIEWOPT_1_CONTROL	|
                    VIEWOPT_1_PAGEBACK |
                    VIEWOPT_1_SOLIDMARKHDL | VIEWOPT_1_POSTITS;
    nCore2Options = VIEWOPT_CORE2_BLACKFONT | VIEWOPT_CORE2_HIDDENPARA;
    nUIOptions    = VIEWOPT_2_MODIFIED | VIEWOPT_2_GRFKEEPZOOM |VIEWOPT_2_ANY_RULER;

    if(MEASURE_METRIC != GetAppLocaleData().getMeasurementSystemEnum())
        aSnapSize.Width() = aSnapSize.Height() = 720;	// 1/2"
    else
        aSnapSize.Width() = aSnapSize.Height() = 567;	// 1 cm
    nDivisionX = nDivisionY = 1;

    bSelectionInReadonly = SW_MOD()->GetAccessibilityOptions().IsSelectionInReadonly();

#ifndef PRODUCT
    // korrespondieren zu den Angaben in ui/config/cfgvw.src
    bTest1 = bTest2 = bTest3 = bTest4 =
             bTest5 = bTest6 = bTest7 = bTest8 = bTest10 = FALSE;
#endif
}


SwViewOption::SwViewOption(const SwViewOption& rVOpt)
{
    bReadonly = FALSE;
    bSelectionInReadonly = FALSE;
    // --> FME 2004-06-29 #114856# Formular view
    bFormView       = rVOpt.bFormView;
    // <--
    nZoom           = rVOpt.nZoom       ;
    aSnapSize   	= rVOpt.aSnapSize   ;
    nDivisionX  	= rVOpt.nDivisionX  ;
    nDivisionY  	= rVOpt.nDivisionY  ;
    nPagePrevRow	= rVOpt.nPagePrevRow;
    nPagePrevCol	= rVOpt.nPagePrevCol;
    bIsPagePreview  = rVOpt.bIsPagePreview;
    eZoom      		= rVOpt.eZoom       ;
    nTblDest    	= rVOpt.nTblDest    ;
    nUIOptions		= rVOpt.nUIOptions  ;
    nCoreOptions	= rVOpt.nCoreOptions  ;
    nCore2Options	= rVOpt.nCore2Options  ;
    aRetoucheColor	= rVOpt.GetRetoucheColor();
    sSymbolFont 	= rVOpt.sSymbolFont;
    nShdwCrsrFillMode = rVOpt.nShdwCrsrFillMode;
    bStarOneSetting = rVOpt.bStarOneSetting;
    bBookview		= rVOpt.bBookview;
    bShowPlaceHolderFields = rVOpt.bShowPlaceHolderFields;

#ifndef PRODUCT
    bTest1          = rVOpt.bTest1      ;
    bTest2          = rVOpt.bTest2      ;
    bTest3          = rVOpt.bTest3      ;
    bTest4          = rVOpt.bTest4      ;
    bTest5          = rVOpt.bTest5      ;
    bTest6          = rVOpt.bTest6      ;
    bTest7          = rVOpt.bTest7      ;
    bTest8          = rVOpt.bTest8      ;
    bTest10         = rVOpt.bTest10     ;
#endif
}


SwViewOption& SwViewOption::operator=( const SwViewOption &rVOpt )
{
    // --> DVO FME 2004-06-29 #114856# Formular view
    bFormView       = rVOpt.bFormView   ;
    // <--
    nZoom   		= rVOpt.nZoom   	;
    aSnapSize   	= rVOpt.aSnapSize   ;
    nDivisionX  	= rVOpt.nDivisionX  ;
    nDivisionY  	= rVOpt.nDivisionY  ;
    nPagePrevRow	= rVOpt.nPagePrevRow;
    nPagePrevCol	= rVOpt.nPagePrevCol;
    bIsPagePreview  = rVOpt.bIsPagePreview;
    eZoom      		= rVOpt.eZoom       ;
    nTblDest    	= rVOpt.nTblDest    ;
    nUIOptions		= rVOpt.nUIOptions  ;
    nCoreOptions	= rVOpt.nCoreOptions;
    nCore2Options	= rVOpt.nCore2Options;
    aRetoucheColor	= rVOpt.GetRetoucheColor();
    sSymbolFont 	= rVOpt.sSymbolFont;
    nShdwCrsrFillMode = rVOpt.nShdwCrsrFillMode;
    bStarOneSetting = rVOpt.bStarOneSetting;
    bBookview		= rVOpt.bBookview;
    bShowPlaceHolderFields = rVOpt.bShowPlaceHolderFields;

#ifndef PRODUCT
    bTest1          = rVOpt.bTest1      ;
    bTest2          = rVOpt.bTest2      ;
    bTest3          = rVOpt.bTest3      ;
    bTest4          = rVOpt.bTest4      ;
    bTest5          = rVOpt.bTest5      ;
    bTest6          = rVOpt.bTest6      ;
    bTest7          = rVOpt.bTest7      ;
    bTest8          = rVOpt.bTest8      ;
    bTest10         = rVOpt.bTest10     ;
#endif
    return *this;
}


SwViewOption::~SwViewOption()
{
}

/*************************************************************************
|*
|*	ViewOption::Init()
|*
|*	Letzte Aenderung	MA 04. Aug. 93
|*
|*************************************************************************/

void SwViewOption::Init( Window *pWin )
{
    if( !nPixelTwips && pWin )
    {
        nPixelTwips = (USHORT)pWin->PixelToLogic( Size(1,1) ).Height();
    }
}

BOOL SwViewOption::IsAutoCompleteWords() const
{
    const SvxSwAutoFmtFlags& rFlags = SvxAutoCorrCfg::Get()->GetAutoCorrect()->GetSwFlags();
    return /*rFlags.bAutoCompleteWords &&*/ rFlags.bAutoCmpltCollectWords;
}

/*************************************************************************/
/*                                                                       */
/*************************************************************************/

AuthorCharAttr::AuthorCharAttr() :
    nItemId	(SID_ATTR_CHAR_UNDERLINE),
    nAttr	(UNDERLINE_SINGLE),
    nColor	(COL_TRANSPARENT)
{
}

/*-----------------07.01.97 13.50-------------------

--------------------------------------------------*/

USHORT		GetHtmlMode(const SwDocShell* pShell)
{
    USHORT nRet = 0;
    if(!pShell || PTR_CAST(SwWebDocShell, pShell))
    {
        nRet = HTMLMODE_ON;
        SvxHtmlOptions* pHtmlOpt = SvxHtmlOptions::Get();
        switch ( pHtmlOpt->GetExportMode() )
        {
            case HTML_CFG_MSIE_40:
                nRet |= HTMLMODE_PARA_BORDER|HTMLMODE_SMALL_CAPS|
                        HTMLMODE_SOME_STYLES|
                        HTMLMODE_FULL_STYLES|HTMLMODE_GRAPH_POS|
                        HTMLMODE_FULL_ABS_POS|HTMLMODE_SOME_ABS_POS;
            break;
            case HTML_CFG_NS40:
                nRet |= HTMLMODE_PARA_BORDER|HTMLMODE_SOME_STYLES|
                        HTMLMODE_FRM_COLUMNS|HTMLMODE_BLINK|HTMLMODE_GRAPH_POS|
                        HTMLMODE_SOME_ABS_POS;
            break;
            case HTML_CFG_WRITER:
                nRet |= HTMLMODE_PARA_BORDER|HTMLMODE_SMALL_CAPS|
                        HTMLMODE_SOME_STYLES|
                        HTMLMODE_FRM_COLUMNS|HTMLMODE_FULL_STYLES|
                        HTMLMODE_BLINK|HTMLMODE_DROPCAPS|HTMLMODE_GRAPH_POS|
                        HTMLMODE_FULL_ABS_POS|HTMLMODE_SOME_ABS_POS;
            break;
            case HTML_CFG_HTML32:
            break;
        }
    }
    return nRet;
}
/* -----------------------------24.04.2002 10:20------------------------------

 ---------------------------------------------------------------------------*/
Color&   SwViewOption::GetDocColor()
{
    return aDocColor;
}
/* -----------------------------23.04.2002 17:18------------------------------

 ---------------------------------------------------------------------------*/
Color&   SwViewOption::GetDocBoundariesColor()
{
    return aDocBoundColor;
}
/* -----------------------------23.04.2002 17:53------------------------------

 ---------------------------------------------------------------------------*/
Color&   SwViewOption::GetObjectBoundariesColor()
{
    return aObjectBoundColor;
}
/* -----------------------------24.04.2002 10:41------------------------------

 ---------------------------------------------------------------------------*/
Color& SwViewOption::GetAppBackgroundColor()
{
    return aAppBackgroundColor;
}
/*-- 24.04.2002 10:50:11---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetTableBoundariesColor()
{
    return aTableBoundColor;
}
/*-- 24.04.2002 10:50:12---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetIndexShadingsColor()
{
    return aIndexShadingsColor;
}
/*-- 24.04.2002 10:50:12---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetLinksColor()
{
    return aLinksColor;
}
/*-- 24.04.2002 10:50:13---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetVisitedLinksColor()
{
    return aVisitedLinksColor;
}
/*-- 24.04.2002 10:50:13---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetDirectCursorColor()
{
    return aDirectCursorColor;
}
/*-- 24.04.2002 10:50:14---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetTextGridColor()
{
    return aTextGridColor;
}
/*-- 24.04.2002 10:50:14---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetSpellColor()
{
    return aSpellColor;
}
/*-- 24.04.2007 10:50:14---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetSmarttagColor()
{
    return aSmarttagColor;
}
/*-- 06.12.2002 10:50:11---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetFontColor()
{
    return aFontColor;
}
/*-- 24.04.2002 10:50:15---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetFieldShadingsColor()
{
    return aFieldShadingsColor;
}
/*-- 24.04.2002 10:50:15---------------------------------------------------

  -----------------------------------------------------------------------*/
Color&   SwViewOption::GetSectionBoundColor()
{
    return aSectionBoundColor;
}
/* -----------------------------2002/07/31 14:00------------------------------

 ---------------------------------------------------------------------------*/
Color& SwViewOption::GetPageBreakColor()
{
    return aPageBreakColor;
}

/* -----------------------------23.04.2002 17:41------------------------------

 ---------------------------------------------------------------------------*/
void SwViewOption::ApplyColorConfigValues(const svtools::ColorConfig& rConfig )
{
    aDocColor.SetColor(rConfig.GetColorValue(svtools::DOCCOLOR).nColor);

    svtools::ColorConfigValue aValue = rConfig.GetColorValue(svtools::DOCBOUNDARIES);
    aDocBoundColor.SetColor(aValue.nColor);
    nAppearanceFlags = 0;
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_DOC_BOUNDARIES;

    aAppBackgroundColor.SetColor(rConfig.GetColorValue(svtools::APPBACKGROUND).nColor);

    aValue = rConfig.GetColorValue(svtools::OBJECTBOUNDARIES);
    aObjectBoundColor.SetColor(aValue.nColor);
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_OBJECT_BOUNDARIES;

    aValue = rConfig.GetColorValue(svtools::TABLEBOUNDARIES);
    aTableBoundColor.SetColor(aValue.nColor);
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_TABLE_BOUNDARIES;

    aValue = rConfig.GetColorValue(svtools::WRITERIDXSHADINGS);
    aIndexShadingsColor.SetColor(aValue.nColor);
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_INDEX_SHADINGS;

    aValue = rConfig.GetColorValue(svtools::LINKS);
    aLinksColor.SetColor(aValue.nColor);
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_LINKS;

    aValue = rConfig.GetColorValue(svtools::LINKSVISITED);
    aVisitedLinksColor.SetColor(aValue.nColor);
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_VISITED_LINKS;

    aDirectCursorColor.SetColor(rConfig.GetColorValue(svtools::WRITERDIRECTCURSOR).nColor);

    aTextGridColor.SetColor(rConfig.GetColorValue(svtools::WRITERTEXTGRID).nColor);

    aSpellColor.SetColor(rConfig.GetColorValue(svtools::SPELL).nColor);

    aSmarttagColor.SetColor(rConfig.GetColorValue(svtools::SMARTTAGS).nColor);

    aFontColor.SetColor(rConfig.GetColorValue(svtools::FONTCOLOR).nColor);

    aValue = rConfig.GetColorValue(svtools::WRITERFIELDSHADINGS);
    aFieldShadingsColor.SetColor(aValue.nColor);
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_FIELD_SHADINGS;

    aValue = rConfig.GetColorValue(svtools::WRITERSECTIONBOUNDARIES);
    aSectionBoundColor.SetColor(aValue.nColor);
    if(aValue.bIsVisible)
        nAppearanceFlags |= VIEWOPT_SECTION_BOUNDARIES;

    aValue = rConfig.GetColorValue(svtools::WRITERPAGEBREAKS);
    aPageBreakColor.SetColor(aValue.nColor);

    aNotesIndicatorColor.SetColor(rConfig.GetColorValue(svtools::WRITERNOTESINDICATOR).nColor);
    aScriptIndicatorColor.SetColor(rConfig.GetColorValue(svtools::WRITERSCRIPTINDICATOR).nColor);
}
/* -----------------------------23.04.2002 17:48------------------------------

 ---------------------------------------------------------------------------*/
void SwViewOption::SetAppearanceFlag(sal_Int32 nFlag, BOOL bSet, BOOL bSaveInConfig )
{
    if(bSet)
        nAppearanceFlags |= nFlag;
    else
        nAppearanceFlags &= ~nFlag;
    if(bSaveInConfig)
    {
        //create an editable svtools::ColorConfig and store the change
        svtools::EditableColorConfig aEditableConfig;
        struct FlagToConfig_Impl
        {
            sal_Int32               nFlag;
            svtools::ColorConfigEntry   eEntry;
        };
        static const FlagToConfig_Impl aFlags[] =
        {
            { VIEWOPT_DOC_BOUNDARIES     ,   svtools::DOCBOUNDARIES },
            { VIEWOPT_OBJECT_BOUNDARIES  ,   svtools::OBJECTBOUNDARIES },
            { VIEWOPT_TABLE_BOUNDARIES   ,   svtools::TABLEBOUNDARIES },
            { VIEWOPT_INDEX_SHADINGS     ,   svtools::WRITERIDXSHADINGS },
            { VIEWOPT_LINKS              ,   svtools::LINKS },
            { VIEWOPT_VISITED_LINKS      ,   svtools::LINKSVISITED },
            { VIEWOPT_FIELD_SHADINGS     ,   svtools::WRITERFIELDSHADINGS },
            { VIEWOPT_SECTION_BOUNDARIES ,   svtools::WRITERSECTIONBOUNDARIES },
            { 0                          ,   svtools::ColorConfigEntryCount }
        };
        sal_uInt16 nPos = 0;
        while(aFlags[nPos].nFlag)
        {
            if(0 != (nFlag&aFlags[nPos].nFlag))
            {
                svtools::ColorConfigValue aValue = aEditableConfig.GetColorValue(aFlags[nPos].eEntry);
                aValue.bIsVisible = bSet;
                aEditableConfig.SetColorValue(aFlags[nPos].eEntry, aValue);
            }
            nPos++;
        }
    }
}
/* -----------------------------24.04.2002 10:42------------------------------

 ---------------------------------------------------------------------------*/
BOOL SwViewOption::IsAppearanceFlag(sal_Int32 nFlag)
{
    return 0 != (nAppearanceFlags & nFlag);
}

