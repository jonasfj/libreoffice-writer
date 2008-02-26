/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dialog.cxx,v $
 *
 *  $Revision: 1.42 $
 *
 *  last change: $Author: obo $ $Date: 2008-02-26 14:43:42 $
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
#include "precompiled_starmath.hxx"


#define SMDLL 1

#ifndef _TOOLS_RCID_H
#include "tools/rcid.h"
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _CTRLTOOL_HXX //autogen
#include <svtools/ctrltool.hxx>
#endif
#ifndef _SFX_PRINTER_HXX
#include <sfx2/printer.hxx>
#endif
#ifndef _SV_SOUND_HXX //autogen
#include <vcl/sound.hxx>
#endif
#ifndef _SV_SNDSTYLE_HXX //autogen
#include <vcl/sndstyle.hxx>
#endif
#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif
#ifndef _SV_SETTINGS_HXX
#include <vcl/settings.hxx>
#endif
#ifndef _SV_WALL_HXX
#include <vcl/wall.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SFX_HRC //autogen
#include <sfx2/sfx.hrc>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _SVX_SUBSETMAP_HXX
#include <svx/ucsubset.hxx>
#endif


#ifndef CONFIG_HXX
#include "config.hxx"
#endif
#ifndef DIALOG_HXX
#include "dialog.hxx"
#endif
#ifndef _STARMATH_HRC
#include "starmath.hrc"
#endif

#include "dialog.hrc"

#ifndef _SMMOD_HXX
#include "smmod.hxx"
#endif
#ifndef SYMBOL_HXX
#include "symbol.hxx"
#endif
#ifndef VIEW_HXX
#include "view.hxx"
#endif
#ifndef DOCUMENT_HXX
#include "document.hxx"
#endif

////////////////////////////////////////
//
// Da der FontStyle besser ueber die Attribute gesetzt/abgefragt wird als ueber
// den StyleName bauen wir uns hier unsere eigene Uebersetzung
// Attribute <-> StyleName
//

class SmFontStyles
{
    String  aNormal;
    String  aBold;
    String  aItalic;
    String  aBoldItalic;
    String  aEmpty;

public:
    SmFontStyles();

    USHORT          GetCount() const    { return 4; }
    const String &  GetStyleName( const Font &rFont ) const;
    const String &  GetStyleName( USHORT nIdx ) const;
};


SmFontStyles::SmFontStyles() :
    aNormal ( ResId( RID_FONTREGULAR, *SM_MOD()->GetResMgr() ) ),
    aBold   ( ResId( RID_FONTBOLD,    *SM_MOD()->GetResMgr() ) ),
    aItalic ( ResId( RID_FONTITALIC,  *SM_MOD()->GetResMgr() ) )
{
//    SM_MOD()->GetResMgr().FreeResource();

    aBoldItalic = aBold;
    aBoldItalic.AppendAscii( ", " );
    aBoldItalic += aItalic;
}


const String & SmFontStyles::GetStyleName( const Font &rFont ) const
{
    //! compare also SmSpecialNode::Prepare
    BOOL bBold   = IsBold( rFont ),
         bItalic = IsItalic( rFont );

    if (bBold && bItalic)
        return aBoldItalic;
    else if (bItalic)
        return aItalic;
    else if (bBold)
        return aBold;
    else
        return aNormal;
}


const String & SmFontStyles::GetStyleName( USHORT nIdx ) const
{
    // 0 = "normal",  1 = "italic",
    // 2 = "bold",    3 = "bold italic"

#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT( nIdx < GetCount(), "index out of range" );
#endif
    switch (nIdx)
    {
        case 0 : return aNormal;
        case 1 : return aItalic;
        case 2 : return aBold;
        case 3 : return aBoldItalic;
    }
    return aEmpty;
}


const SmFontStyles & GetFontStyles()
{
    static const SmFontStyles aImpl;
    return aImpl;
}

/////////////////////////////////////////////////////////////////

void SetFontStyle(const XubString &rStyleName, Font &rFont)
{
    // finden des Index passend zum StyleName fuer den leeren StyleName wird
    // 0 (nicht bold nicht italic) angenommen.
    USHORT  nIndex = 0;
    if (rStyleName.Len())
    {
        USHORT i;
        const SmFontStyles &rStyles = GetFontStyles();
        for (i = 0;  i < rStyles.GetCount();  i++)
            if (rStyleName.CompareTo( rStyles.GetStyleName(i) ) == COMPARE_EQUAL)
                break;
#if OSL_DEBUG_LEVEL > 1
        DBG_ASSERT(i < rStyles.GetCount(), "style-name unknown");
#endif
        nIndex = i;
    }

    rFont.SetItalic((nIndex & 0x1) ? ITALIC_NORMAL : ITALIC_NONE);
    rFont.SetWeight((nIndex & 0x2) ? WEIGHT_BOLD : WEIGHT_NORMAL);
}


/**************************************************************************/

IMPL_LINK_INLINE_START( SmPrintOptionsTabPage, SizeButtonClickHdl, Button *, EMPTYARG/*pButton*/ )
{
    aZoom.Enable(aSizeZoomed.IsChecked());
    return 0;
}
IMPL_LINK_INLINE_END( SmPrintOptionsTabPage, SizeButtonClickHdl, Button *, pButton )


SmPrintOptionsTabPage::SmPrintOptionsTabPage(Window *pParent, const SfxItemSet &rOptions)
    : SfxTabPage(pParent, SmResId(RID_PRINTOPTIONPAGE), rOptions),
    aFixedLine1     (this, SmResId( FL_PRINTOPTIONS )),
    aTitle		   	(this, SmResId( CB_TITLEROW )),
    aText		   	(this, SmResId( CB_EQUATION_TEXT )),
    aFrame		   	(this, SmResId( CB_FRAME )),
    aFixedLine2     (this, SmResId( FL_PRINT_FORMAT )),
    aSizeNormal    	(this, SmResId( RB_ORIGINAL_SIZE )),
    aSizeScaled    	(this, SmResId( RB_FIT_TO_PAGE )),
    aSizeZoomed    	(this, SmResId( RB_ZOOM )),
    aZoom		   	(this, SmResId( MF_ZOOM )),
    aFixedLine3      (this, SmResId( FL_MISC_OPTIONS )),
    aNoRightSpaces	(this, SmResId( CB_IGNORE_SPACING ))
{
    FreeResource();

    aSizeNormal.SetClickHdl(LINK(this, SmPrintOptionsTabPage, SizeButtonClickHdl));
    aSizeScaled.SetClickHdl(LINK(this, SmPrintOptionsTabPage, SizeButtonClickHdl));
    aSizeZoomed.SetClickHdl(LINK(this, SmPrintOptionsTabPage, SizeButtonClickHdl));

    Reset(rOptions);
}


BOOL SmPrintOptionsTabPage::FillItemSet(SfxItemSet& rSet)
{
    UINT16  nPrintSize;
    if (aSizeNormal.IsChecked())
        nPrintSize = PRINT_SIZE_NORMAL;
    else if (aSizeScaled.IsChecked())
        nPrintSize = PRINT_SIZE_SCALED;
    else
        nPrintSize = PRINT_SIZE_ZOOMED;

    rSet.Put(SfxUInt16Item(GetWhich(SID_PRINTSIZE), (UINT16) nPrintSize));
    rSet.Put(SfxUInt16Item(GetWhich(SID_PRINTZOOM), (UINT16) aZoom.GetValue()));
    rSet.Put(SfxBoolItem(GetWhich(SID_PRINTTITLE), aTitle.IsChecked()));
    rSet.Put(SfxBoolItem(GetWhich(SID_PRINTTEXT), aText.IsChecked()));
    rSet.Put(SfxBoolItem(GetWhich(SID_PRINTFRAME), aFrame.IsChecked()));
    rSet.Put(SfxBoolItem(GetWhich(SID_NO_RIGHT_SPACES), aNoRightSpaces.IsChecked()));

    return TRUE;
}


void SmPrintOptionsTabPage::Reset(const SfxItemSet& rSet)
{
    SmPrintSize ePrintSize = (SmPrintSize)((const SfxUInt16Item &)rSet.Get(GetWhich(SID_PRINTSIZE))).GetValue();

    aSizeNormal.Check(ePrintSize == PRINT_SIZE_NORMAL);
    aSizeScaled.Check(ePrintSize == PRINT_SIZE_SCALED);
    aSizeZoomed.Check(ePrintSize == PRINT_SIZE_ZOOMED);

    aZoom.Enable(aSizeZoomed.IsChecked());

    aZoom.SetValue(((const SfxUInt16Item &)rSet.Get(GetWhich(SID_PRINTZOOM))).GetValue());

    aTitle.Check(((const SfxBoolItem &)rSet.Get(GetWhich(SID_PRINTTITLE))).GetValue());
    aText.Check(((const SfxBoolItem &)rSet.Get(GetWhich(SID_PRINTTEXT))).GetValue());
    aFrame.Check(((const SfxBoolItem &)rSet.Get(GetWhich(SID_PRINTFRAME))).GetValue());
    aNoRightSpaces.Check(((const SfxBoolItem &)rSet.Get(GetWhich(SID_NO_RIGHT_SPACES))).GetValue());
}


SfxTabPage* SmPrintOptionsTabPage::Create(Window* pWindow, const SfxItemSet& rSet)
{
    return (new SmPrintOptionsTabPage(pWindow, rSet));
}

/**************************************************************************/


void SmShowFont::Paint(const Rectangle& rRect )
{
    Control::Paint( rRect );

    XubString	Text (GetFont().GetName());
    Size	TextSize(GetTextWidth(Text), GetTextHeight());

    DrawText(Point((GetOutputSize().Width()  - TextSize.Width())  / 2,
                   (GetOutputSize().Height() - TextSize.Height()) / 2), Text);
}


void SmShowFont::SetFont(const Font& rFont)
{
    Color aTxtColor( GetTextColor() );
    Font aFont (rFont);

    Invalidate();
    aFont.SetSize(Size(0, 24));
    aFont.SetAlign(ALIGN_TOP);
    Control::SetFont(aFont);

    // keep old text color (new font may have different color)
    SetTextColor( aTxtColor );
}


IMPL_LINK_INLINE_START( SmFontDialog, FontSelectHdl, ComboBox *, pComboBox )
{
    Face.SetName(pComboBox->GetText());
    aShowFont.SetFont(Face);
    return 0;
}
IMPL_LINK_INLINE_END( SmFontDialog, FontSelectHdl, ComboBox *, pComboBox )


IMPL_LINK( SmFontDialog, FontModifyHdl, ComboBox *, pComboBox )
{
    // if font is available in list then use it
    USHORT nPos = pComboBox->GetEntryPos( pComboBox->GetText() );
    if (COMBOBOX_ENTRY_NOTFOUND != nPos)
    {
        FontSelectHdl( pComboBox );
    }
    return 0;
}


IMPL_LINK( SmFontDialog, AttrChangeHdl, CheckBox *, EMPTYARG /*pCheckBox*/ )
{
    if (aBoldCheckBox.IsChecked())
        Face.SetWeight(FontWeight(WEIGHT_BOLD));
    else
        Face.SetWeight(FontWeight(WEIGHT_NORMAL));

    if (aItalicCheckBox.IsChecked())
        Face.SetItalic(ITALIC_NORMAL);
    else
        Face.SetItalic(ITALIC_NONE);

    aShowFont.SetFont(Face);
    return 0;
}


void SmFontDialog::SetFont(const Font &rFont)
{
    Face = rFont;

    aFontBox.SetText( Face.GetName() );
    aBoldCheckBox.Check( IsBold( Face ) );
    aItalicCheckBox.Check( IsItalic( Face ) );

    aShowFont.SetFont(Face);
}


SmFontDialog::SmFontDialog(Window * pParent,
        OutputDevice *pFntListDevice, BOOL bHideCheckboxes, BOOL bFreeRes)
    : ModalDialog(pParent,SmResId(RID_FONTDIALOG)),
    aFixedText1 	(this, SmResId(1)),
    aFontBox		(this, SmResId(1)),
    aBoldCheckBox	(this, SmResId(1)),
    aItalicCheckBox (this, SmResId(2)),
    aOKButton1		(this, SmResId(1)),
    aCancelButton1	(this, SmResId(1)),
    aShowFont		(this, SmResId(1)),
    aFixedText2     (this, SmResId(2))
{
    if (bFreeRes)
        FreeResource();

    {
        WaitObject( this );

        FontList aFontList( pFntListDevice );

        USHORT  nCount = aFontList.GetFontNameCount();
        for (USHORT i = 0;  i < nCount;  i++)
            aFontBox.InsertEntry( aFontList.GetFontName(i).GetName() );

        Face.SetSize(Size(0, 24));
        Face.SetWeight(WEIGHT_NORMAL);
        Face.SetItalic(ITALIC_NONE);
        Face.SetFamily(FAMILY_DONTKNOW);
        Face.SetPitch(PITCH_DONTKNOW);
        Face.SetCharSet(RTL_TEXTENCODING_DONTKNOW);
        Face.SetTransparent(TRUE);

        InitColor_Impl();

        // preview like controls should have a 2D look
        aShowFont.SetBorderStyle( WINDOW_BORDER_MONO );
    }

    aFontBox.SetSelectHdl(LINK(this, SmFontDialog, FontSelectHdl));
    aFontBox.SetModifyHdl(LINK(this, SmFontDialog, FontModifyHdl));
    aBoldCheckBox.SetClickHdl(LINK(this, SmFontDialog, AttrChangeHdl));
    aItalicCheckBox.SetClickHdl(LINK(this, SmFontDialog, AttrChangeHdl));

    if (bHideCheckboxes)
    {
        aBoldCheckBox.Check( FALSE );
        aBoldCheckBox.Enable( FALSE );
        aBoldCheckBox.Show( FALSE );
        aItalicCheckBox.Check( FALSE );
        aItalicCheckBox.Enable( FALSE );
        aItalicCheckBox.Show( FALSE );
        aFixedText2.Show( FALSE );

        Size  aSize( aFontBox.GetSizePixel() );
        long nComboBoxBottom = aFontBox.GetPosPixel().Y() + aFontBox.GetSizePixel().Height();
        long nCheckBoxBottom = aItalicCheckBox.GetPosPixel().Y() + aItalicCheckBox.GetSizePixel().Height();
        aSize.Height() += nCheckBoxBottom - nComboBoxBottom;
        aFontBox.SetSizePixel( aSize );
    }
}

void SmFontDialog::InitColor_Impl()
{
#if OSL_DEBUG_LEVEL > 1
    Color aBC( GetDisplayBackground().GetColor() );
#endif
    ColorData   nBgCol  = COL_WHITE,
                nTxtCol = COL_BLACK;
    if (GetDisplayBackground().GetColor().IsDark())
    {
        const StyleSettings &rS = GetSettings().GetStyleSettings();
        nBgCol  = rS.GetFieldColor().GetColor();
        nTxtCol = rS.GetFieldTextColor().GetColor();
    }

    Color aTmpColor( nBgCol );
    Wallpaper aWall( aTmpColor );
    Color aTxtColor( nTxtCol );
    aShowFont.SetBackground( aWall );
    aShowFont.SetTextColor( aTxtColor );
}


void SmFontDialog::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( rDCEvt.GetType() == DATACHANGED_SETTINGS  &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            InitColor_Impl();

    ModalDialog::DataChanged( rDCEvt );
}

/**************************************************************************/


IMPL_LINK( SmFontSizeDialog, DefaultButtonClickHdl, Button *, EMPTYARG /*pButton*/ )
{
    QueryBox *pQueryBox = new QueryBox(this, SmResId(RID_DEFAULTSAVEQUERY));

    if (pQueryBox->Execute() == RET_YES)
    {
        SmModule *pp = SM_MOD1();
        SmFormat aFmt( pp->GetConfig()->GetStandardFormat() );
        WriteTo( aFmt );
        pp->GetConfig()->SetStandardFormat( aFmt );
    }

    delete pQueryBox;
    return 0;
}


SmFontSizeDialog::SmFontSizeDialog(Window * pParent, BOOL bFreeRes)
    : ModalDialog(pParent, SmResId(RID_FONTSIZEDIALOG)),
    aFixedText1(this, SmResId(1)),
    aBaseSize(this, SmResId(1)),
    aFixedText4(this, SmResId(4)),
    aTextSize(this, SmResId(4)),
    aFixedText5(this, SmResId(5)),
    aIndexSize(this, SmResId(5)),
    aFixedText6(this, SmResId(6)),
    aFunctionSize(this, SmResId(6)),
    aFixedText7(this, SmResId(7)),
    aOperatorSize(this, SmResId(7)),
    aFixedText8(this, SmResId(8)),
    aBorderSize(this, SmResId(8)),
    aFixedLine1(this, SmResId(1)),
    aOKButton1(this, SmResId(1)),
    aCancelButton1(this, SmResId(1)),
    aDefaultButton(this, SmResId(1))
{
    if (bFreeRes)
        FreeResource();

    aDefaultButton.SetClickHdl(LINK(this, SmFontSizeDialog, DefaultButtonClickHdl));
}


void SmFontSizeDialog::ReadFrom(const SmFormat &rFormat)
{
    //! aufpassen: richtig runden!
    aBaseSize.SetValue( SmRoundFraction(
        Sm100th_mmToPts( rFormat.GetBaseSize().Height() ) ) );

    aTextSize    .SetValue( rFormat.GetRelSize(SIZ_TEXT) );
    aIndexSize   .SetValue( rFormat.GetRelSize(SIZ_INDEX) );
    aFunctionSize.SetValue( rFormat.GetRelSize(SIZ_FUNCTION) );
    aOperatorSize.SetValue( rFormat.GetRelSize(SIZ_OPERATOR) );
    aBorderSize  .SetValue( rFormat.GetRelSize(SIZ_LIMITS) );
}


void SmFontSizeDialog::WriteTo(SmFormat &rFormat) const
{
    rFormat.SetBaseSize( Size(0, SmPtsTo100th_mm( static_cast< long >(aBaseSize.GetValue()))) );

    rFormat.SetRelSize(SIZ_TEXT, 	 (USHORT) aTextSize    .GetValue());
    rFormat.SetRelSize(SIZ_INDEX,	 (USHORT) aIndexSize   .GetValue());
    rFormat.SetRelSize(SIZ_FUNCTION, (USHORT) aFunctionSize.GetValue());
    rFormat.SetRelSize(SIZ_OPERATOR, (USHORT) aOperatorSize.GetValue());
    rFormat.SetRelSize(SIZ_LIMITS,	 (USHORT) aBorderSize  .GetValue());

    const Size aTmp (rFormat.GetBaseSize());
    for (USHORT  i = FNT_BEGIN;  i <= FNT_END;  i++)
        rFormat.SetFontSize(i, aTmp);

    rFormat.RequestApplyChanges();
}


/**************************************************************************/


IMPL_LINK( SmFontTypeDialog, MenuSelectHdl, Menu *, pMenu )
{
    SmFontPickListBox *pActiveListBox;

    BOOL bHideCheckboxes = FALSE;
    switch (pMenu->GetCurItemId())
    {
        case 1: pActiveListBox = &aVariableFont; break;
        case 2: pActiveListBox = &aFunctionFont; break;
        case 3: pActiveListBox = &aNumberFont;	 break;
        case 4: pActiveListBox = &aTextFont;	 break;
        case 5: pActiveListBox = &aSerifFont; bHideCheckboxes = TRUE;   break;
        case 6: pActiveListBox = &aSansFont;  bHideCheckboxes = TRUE;   break;
        case 7: pActiveListBox = &aFixedFont; bHideCheckboxes = TRUE;   break;
        default:pActiveListBox = NULL;
    }

    if (pActiveListBox)
    {
        SmFontDialog *pFontDialog = new SmFontDialog(this, pFontListDev, bHideCheckboxes);

        pActiveListBox->WriteTo(*pFontDialog);
        if (pFontDialog->Execute() == RET_OK)
            pActiveListBox->ReadFrom(*pFontDialog);
        delete pFontDialog;
    }
    return 0;
}


IMPL_LINK_INLINE_START( SmFontTypeDialog, DefaultButtonClickHdl, Button *, EMPTYARG /*pButton*/ )
{
    QueryBox *pQueryBox = new QueryBox(this, SmResId(RID_DEFAULTSAVEQUERY));
    if (pQueryBox->Execute() == RET_YES)
    {
        SmModule *pp = SM_MOD1();
        SmFormat aFmt( pp->GetConfig()->GetStandardFormat() );
        WriteTo( aFmt );
        pp->GetConfig()->SetStandardFormat( aFmt );
    }

    delete pQueryBox;
    return 0;
}
IMPL_LINK_INLINE_END( SmFontTypeDialog, DefaultButtonClickHdl, Button *, pButton )


SmFontTypeDialog::SmFontTypeDialog(Window * pParent, OutputDevice *pFntListDevice, BOOL bFreeRes)
    : ModalDialog(pParent, SmResId(RID_FONTTYPEDIALOG)),
    aFixedText1    (this, SmResId(1)),
    aVariableFont  (this, SmResId(1)),
    aFixedText2    (this, SmResId(2)),
    aFunctionFont  (this, SmResId(2)),
    aFixedText3    (this, SmResId(3)),
    aNumberFont    (this, SmResId(3)),
    aFixedText4    (this, SmResId(4)),
    aTextFont	   (this, SmResId(4)),
    aFixedText5    (this, SmResId(5)),
    aSerifFont	   (this, SmResId(5)),
    aFixedText6    (this, SmResId(6)),
    aSansFont	   (this, SmResId(6)),
    aFixedText7    (this, SmResId(7)),
    aFixedFont	   (this, SmResId(7)),
    aFixedLine1    (this, SmResId(1)),
    aFixedLine2    (this, SmResId(2)),
    aOKButton1	   (this, SmResId(1)),
    aCancelButton1 (this, SmResId(1)),
    aMenuButton    (this, SmResId(1)),
    aDefaultButton (this, SmResId(2)),
    pFontListDev    (pFntListDevice)
{
    if (bFreeRes)
        FreeResource();

    aDefaultButton.SetClickHdl(LINK(this, SmFontTypeDialog, DefaultButtonClickHdl));

    aMenuButton.GetPopupMenu()->SetSelectHdl(LINK(this, SmFontTypeDialog, MenuSelectHdl));
}

void SmFontTypeDialog::ReadFrom(const SmFormat &rFormat)
{
    SmModule *pp = SM_MOD1();

    aVariableFont = pp->GetConfig()->GetFontPickList(FNT_VARIABLE);
    aFunctionFont = pp->GetConfig()->GetFontPickList(FNT_FUNCTION);
    aNumberFont   = pp->GetConfig()->GetFontPickList(FNT_NUMBER);
    aTextFont	  = pp->GetConfig()->GetFontPickList(FNT_TEXT);
    aSerifFont	  = pp->GetConfig()->GetFontPickList(FNT_SERIF);
    aSansFont	  = pp->GetConfig()->GetFontPickList(FNT_SANS);
    aFixedFont	  = pp->GetConfig()->GetFontPickList(FNT_FIXED);

    aVariableFont.Insert( rFormat.GetFont(FNT_VARIABLE) );
    aFunctionFont.Insert( rFormat.GetFont(FNT_FUNCTION) );
    aNumberFont  .Insert( rFormat.GetFont(FNT_NUMBER) );
    aTextFont    .Insert( rFormat.GetFont(FNT_TEXT) );
    aSerifFont   .Insert( rFormat.GetFont(FNT_SERIF) );
    aSansFont    .Insert( rFormat.GetFont(FNT_SANS) );
    aFixedFont   .Insert( rFormat.GetFont(FNT_FIXED) );
}


void SmFontTypeDialog::WriteTo(SmFormat &rFormat) const
{
    SmModule *pp = SM_MOD1();

    pp->GetConfig()->GetFontPickList(FNT_VARIABLE) = aVariableFont;
    pp->GetConfig()->GetFontPickList(FNT_FUNCTION) = aFunctionFont;
    pp->GetConfig()->GetFontPickList(FNT_NUMBER)   = aNumberFont;
    pp->GetConfig()->GetFontPickList(FNT_TEXT)	   = aTextFont;
    pp->GetConfig()->GetFontPickList(FNT_SERIF)    = aSerifFont;
    pp->GetConfig()->GetFontPickList(FNT_SANS)	   = aSansFont;
    pp->GetConfig()->GetFontPickList(FNT_FIXED)    = aFixedFont;

    rFormat.SetFont( FNT_VARIABLE, aVariableFont.Get(0) );
    rFormat.SetFont( FNT_FUNCTION, aFunctionFont.Get(0) );
    rFormat.SetFont( FNT_NUMBER,   aNumberFont  .Get(0) );
    rFormat.SetFont( FNT_TEXT,     aTextFont    .Get(0) );
    rFormat.SetFont( FNT_SERIF,    aSerifFont   .Get(0) );
    rFormat.SetFont( FNT_SANS,     aSansFont    .Get(0) );
    rFormat.SetFont( FNT_FIXED,    aFixedFont   .Get(0) );

    rFormat.RequestApplyChanges();
}

/**************************************************************************/

struct FieldMinMax
{
    USHORT nMin, nMax;
};

// Data for min and max values of the 4 metric fields
// for each of the 10 categories
static const FieldMinMax pMinMaxData[10][4] =
{
    // 0
    {{ 0, 200 },	{ 0, 200 },		{ 0, 100 },		{ 0, 0 }},
    // 1
    {{ 0, 100 },	{ 0, 100 },		{ 0, 0 },		{ 0, 0 }},
    // 2
    {{ 0, 100 },	{ 0, 100 },		{ 0, 0 },		{ 0, 0 }},
    // 3
    {{ 0, 100 },	{ 1, 100 },		{ 0, 0 },		{ 0, 0 }},
    // 4
    {{ 0, 100 },	{ 0, 100 },		{ 0, 0 },		{ 0, 0 }},
    // 5
    {{ 0, 100 },	{ 0, 100 },		{ 0, 0 },		{ 0, 100 }},
    // 6
    {{ 0, 300 },	{ 0, 300 },		{ 0, 0 },		{ 0, 0 }},
    // 7
    {{ 0, 100 },	{ 0, 100 },		{ 0, 0 },		{ 0, 0 }},
    // 8
    {{ 0, 100 },	{ 0, 100 },		{ 0, 0 },		{ 0, 0 }},
    // 9
    {{ 0, 10000 },	{ 0, 10000 },	{ 0, 10000 },	{ 0, 10000 }}
};

SmCategoryDesc::SmCategoryDesc(const ResId& rResId, USHORT nCategoryIdx) :
    Resource(rResId),
    bIsHighContrast(FALSE)
{
    if (IsAvailableRes(ResId(1,*rResId.GetResMgr()).SetRT(RSC_STRING)))
    {
        Name = XubString(ResId(1,*rResId.GetResMgr()));

        int i;
        for (i = 0; i < 4; i++)
        {
            int nI2 = i + 2;

            if (IsAvailableRes(ResId(nI2,*rResId.GetResMgr()).SetRT(RSC_STRING)))
            {
                Strings  [i] = new XubString(ResId(nI2,*rResId.GetResMgr()));
                Graphics [i] = new Bitmap(ResId(10*nI2,*rResId.GetResMgr()));
                GraphicsH[i] = new Bitmap(ResId(10*nI2+1,*rResId.GetResMgr()));
            }
            else
            {
                Strings  [i] = 0;
                Graphics [i] = 0;
                GraphicsH[i] = 0;
            }
        }

        for (i = 0; i < 4; i++)
        {
            const FieldMinMax &rMinMax = pMinMaxData[ nCategoryIdx ][i];
            Value[i] = Minimum[i] = rMinMax.nMin;
            Maximum[i] = rMinMax.nMax;
        }
    }

    FreeResource();
}


SmCategoryDesc::~SmCategoryDesc()
{
    for (int i = 0; i < 4; i++)
    {
        delete Strings  [i];
        delete Graphics [i];
        delete GraphicsH[i];
    }
}

/**************************************************************************/

IMPL_LINK( SmDistanceDialog, GetFocusHdl, Control *, pControl )
{
    if (Categories[nActiveCategory])
    {
        USHORT	i;

        if (pControl == &aMetricField1)
            i = 0;
        else if (pControl == &aMetricField2)
            i = 1;
        else if (pControl == &aMetricField3)
            i = 2;
        else if (pControl == &aMetricField4)
            i = 3;
        else
            return 0;
        aBitmap.SetBitmap(*(Categories[nActiveCategory]->GetGraphic(i)));
    }
    return 0;
}

IMPL_LINK( SmDistanceDialog, MenuSelectHdl, Menu *, pMenu )
{
    SetCategory(pMenu->GetCurItemId() - 1);
    return 0;
}


IMPL_LINK( SmDistanceDialog, DefaultButtonClickHdl, Button *, EMPTYARG /*pButton*/ )
{
    QueryBox *pQueryBox = new QueryBox(this, SmResId(RID_DEFAULTSAVEQUERY));

    if (pQueryBox->Execute() == RET_YES)
    {
        SmModule *pp = SM_MOD1();
        SmFormat aFmt( pp->GetConfig()->GetStandardFormat() );
        WriteTo( aFmt );
        pp->GetConfig()->SetStandardFormat( aFmt );
    }
    delete pQueryBox;
    return 0;
}


IMPL_LINK( SmDistanceDialog, CheckBoxClickHdl, CheckBox *, pCheckBox )
{
    if (pCheckBox == &aCheckBox1)
    {
        aCheckBox1.Toggle();

        BOOL bChecked = aCheckBox1.IsChecked();
        aFixedText4  .Enable( bChecked );
        aMetricField4.Enable( bChecked );
    }
    return 0;
}


void SmDistanceDialog::SetHelpId(MetricField &rField, ULONG nHelpId)
{
    //! HelpID's die auf diese Weise explizit gesetzt werden, muessen im
    //! util Verzeichnis im File "hidother.src" mit Hilfe von "hidspecial"
    //! definiert werden!

    const XubString aEmptyText;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(aEmptyText.Len() == 0, "Sm: Ooops...");
#endif

    rField.SetHelpId(nHelpId);
    rField.SetHelpText(aEmptyText);

    // since MetricField inherits from SpinField which has a sub Edit field
    // (which is actually the one we modify) we have to set the help-id
    // for it too.
    Edit *pSubEdit = rField.GetSubEdit();
    if (pSubEdit)
    {	pSubEdit->SetHelpId(nHelpId);
        pSubEdit->SetHelpText(aEmptyText);
    }
}


void SmDistanceDialog::SetCategory(USHORT nCategory)
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(/*0 <= nCategory  &&*/  nCategory < NOCATEGORIES,
        "Sm: falsche Kategorienummer in SmDistanceDialog");
#endif

    // array to convert category- and metricfield-number in help ids.
    // 0 is used in case of unused combinations.
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(NOCATEGORIES == 10, "Sm : Array passt nicht zu Anzahl der Kategorien");
#endif
    ULONG __READONLY_DATA  aCatMf2Hid[10][4] =
    {
        { HID_SMA_DEFAULT_DIST,         HID_SMA_LINE_DIST,          HID_SMA_ROOT_DIST, 0 },
        { HID_SMA_SUP_DIST,             HID_SMA_SUB_DIST ,          0, 0 },
        { HID_SMA_NUMERATOR_DIST,       HID_SMA_DENOMINATOR_DIST,   0, 0 },
        { HID_SMA_FRACLINE_EXCWIDTH,    HID_SMA_FRACLINE_LINEWIDTH, 0, 0 },
        { HID_SMA_UPPERLIMIT_DIST,      HID_SMA_LOWERLIMIT_DIST,    0, 0 },
        { HID_SMA_BRACKET_EXCHEIGHT,    HID_SMA_BRACKET_DIST,       0, HID_SMA_BRACKET_EXCHEIGHT2 },
        { HID_SMA_MATRIXROW_DIST,       HID_SMA_MATRIXCOL_DIST,     0, 0 },
        { HID_SMA_ATTRIBUT_DIST,        HID_SMA_INTERATTRIBUT_DIST, 0, 0 },
        { HID_SMA_OPERATOR_EXCHEIGHT,   HID_SMA_OPERATOR_DIST,      0, 0 },
        { HID_SMA_LEFTBORDER_DIST,      HID_SMA_RIGHTBORDER_DIST,   HID_SMA_UPPERBORDER_DIST, HID_SMA_LOWERBORDER_DIST }
    };

    // array to help iterate over the controls
    Window * __READONLY_DATA  aWin[4][2] =
    {
        { &aFixedText1,  &aMetricField1 },
        { &aFixedText2,  &aMetricField2 },
        { &aFixedText3,  &aMetricField3 },
        { &aFixedText4,  &aMetricField4 }
    };

    SmCategoryDesc *pCat;

    // merken der (evtl neuen) Einstellungen der aktiven SmCategoryDesc
    // bevor zu der neuen gewechselt wird.
    if (nActiveCategory != CATEGORY_NONE)
    {
        pCat = Categories[nActiveCategory];
        pCat->SetValue(0, (USHORT) aMetricField1.GetValue());
        pCat->SetValue(1, (USHORT) aMetricField2.GetValue());
        pCat->SetValue(2, (USHORT) aMetricField3.GetValue());
        pCat->SetValue(3, (USHORT) aMetricField4.GetValue());

        if (nActiveCategory == 5)
            bScaleAllBrackets = aCheckBox1.IsChecked();

        aMenuButton.GetPopupMenu()->CheckItem(nActiveCategory + 1, FALSE);
    }

    // aktivieren/deaktivieren der zugehoerigen Controls in Abhaengigkeit von der
    // gewaehlten Kategorie.
    BOOL  bActive;
    for (USHORT i = 0;  i < 4;  i++)
    {
        FixedText   *pFT = (FixedText * const)   aWin[i][0];
        MetricField *pMF = (MetricField * const) aWin[i][1];

        // Um feststellen welche Controls aktiv sein sollen wird das
        // vorhandensein einer zugehoerigen HelpID ueberprueft.
        bActive = aCatMf2Hid[nCategory][i] != 0;

        pFT->Show(bActive);
        pFT->Enable(bActive);
        pMF->Show(bActive);
        pMF->Enable(bActive);

        // setzen von Masseinheit und Anzahl der Nachkommastellen
        FieldUnit  eUnit;
        USHORT     nDigits;
        if (nCategory < 9)
        {
            eUnit   = FUNIT_CUSTOM;
            nDigits = 0;
            pMF->SetCustomUnitText( '%' );
        }
        else
        {
            eUnit   = FUNIT_100TH_MM;
            nDigits = 2;
        }
        pMF->SetUnit(eUnit);            //! veraendert den Wert
        pMF->SetDecimalDigits(nDigits);

        if (bActive)
        {
            pCat = Categories[nCategory];
            pFT->SetText(*pCat->GetString(i));

            pMF->SetMin(pCat->GetMinimum(i));
            pMF->SetMax(pCat->GetMaximum(i));
            pMF->SetValue(pCat->GetValue(i));

            SetHelpId(*pMF, aCatMf2Hid[nCategory][i]);
        }
    }
    // nun noch die CheckBox und das zugehoerige MetricField genau dann aktivieren,
    // falls es sich um das Klammer Menu handelt.
    bActive = nCategory == 5;
    aCheckBox1.Show(bActive);
    aCheckBox1.Enable(bActive);
    if (bActive)
    {
        aCheckBox1.Check( bScaleAllBrackets );

        BOOL bChecked = aCheckBox1.IsChecked();
        aFixedText4  .Enable( bChecked );
        aMetricField4.Enable( bChecked );
    }

    aMenuButton.GetPopupMenu()->CheckItem(nCategory + 1, TRUE);
    aFixedLine.SetText(Categories[nCategory]->GetName());

    nActiveCategory = nCategory;

    aMetricField1.GrabFocus();
    Invalidate();
    Update();
}


SmDistanceDialog::SmDistanceDialog(Window *pParent, BOOL bFreeRes)
    : ModalDialog(pParent, SmResId(RID_DISTANCEDIALOG)),
    aFixedText1    (this, SmResId(1)),
    aMetricField1  (this, SmResId(1)),
    aFixedText2    (this, SmResId(2)),
    aMetricField2  (this, SmResId(2)),
    aFixedText3    (this, SmResId(3)),
    aMetricField3  (this, SmResId(3)),
    aCheckBox1     (this, SmResId(1)),
    aFixedText4    (this, SmResId(4)),
    aMetricField4  (this, SmResId(4)),
    aOKButton1     (this, SmResId(1)),
    aCancelButton1 (this, SmResId(1)),
    aMenuButton    (this, SmResId(1)),
    aDefaultButton (this, SmResId(1)),
    aBitmap        (this, SmResId(1)),
    aFixedLine     (this, SmResId(1))
{
    for (USHORT i = 0; i < NOCATEGORIES; i++)
        Categories[i] = new SmCategoryDesc(SmResId(i + 1), i);
    nActiveCategory   = CATEGORY_NONE;
    bScaleAllBrackets = FALSE;

    if (bFreeRes)
        FreeResource();

    ApplyImages();

    // preview like controls should have a 2D look
    aBitmap.SetBorderStyle( WINDOW_BORDER_MONO );

    aMetricField1.SetGetFocusHdl(LINK(this, SmDistanceDialog, GetFocusHdl));
    aMetricField2.SetGetFocusHdl(LINK(this, SmDistanceDialog, GetFocusHdl));
    aMetricField3.SetGetFocusHdl(LINK(this, SmDistanceDialog, GetFocusHdl));
    aMetricField4.SetGetFocusHdl(LINK(this, SmDistanceDialog, GetFocusHdl));
    aCheckBox1.SetClickHdl(LINK(this, SmDistanceDialog, CheckBoxClickHdl));

    aMenuButton.GetPopupMenu()->SetSelectHdl(LINK(this, SmDistanceDialog, MenuSelectHdl));

    aDefaultButton.SetClickHdl(LINK(this, SmDistanceDialog, DefaultButtonClickHdl));
}


SmDistanceDialog::~SmDistanceDialog()
{
    for (int i = 0; i < NOCATEGORIES; i++)
        DELETEZ(Categories[i]);
}

void SmDistanceDialog::ApplyImages()
{
    BOOL bHighContrast = GetDisplayBackground().GetColor().IsDark() != 0;
    for (int i = 0;  i < NOCATEGORIES;  ++i)
    {
        SmCategoryDesc *pCat = Categories[i];
        if (pCat)
            pCat->SetHighContrast( bHighContrast );
    }
}

void SmDistanceDialog::DataChanged( const DataChangedEvent &rEvt )
{
    if ( (rEvt.GetType() == DATACHANGED_SETTINGS) && (rEvt.GetFlags() & SETTINGS_STYLE) )
            ApplyImages();

    ModalDialog::DataChanged( rEvt );
}

void SmDistanceDialog::ReadFrom(const SmFormat &rFormat)
{
    Categories[0]->SetValue(0, rFormat.GetDistance(DIS_HORIZONTAL));
    Categories[0]->SetValue(1, rFormat.GetDistance(DIS_VERTICAL));
    Categories[0]->SetValue(2, rFormat.GetDistance(DIS_ROOT));
    Categories[1]->SetValue(0, rFormat.GetDistance(DIS_SUPERSCRIPT));
    Categories[1]->SetValue(1, rFormat.GetDistance(DIS_SUBSCRIPT));
    Categories[2]->SetValue(0, rFormat.GetDistance(DIS_NUMERATOR));
    Categories[2]->SetValue(1, rFormat.GetDistance(DIS_DENOMINATOR));
    Categories[3]->SetValue(0, rFormat.GetDistance(DIS_FRACTION));
    Categories[3]->SetValue(1, rFormat.GetDistance(DIS_STROKEWIDTH));
    Categories[4]->SetValue(0, rFormat.GetDistance(DIS_UPPERLIMIT));
    Categories[4]->SetValue(1, rFormat.GetDistance(DIS_LOWERLIMIT));
    Categories[5]->SetValue(0, rFormat.GetDistance(DIS_BRACKETSIZE));
    Categories[5]->SetValue(1, rFormat.GetDistance(DIS_BRACKETSPACE));
    Categories[5]->SetValue(3, rFormat.GetDistance(DIS_NORMALBRACKETSIZE));
    Categories[6]->SetValue(0, rFormat.GetDistance(DIS_MATRIXROW));
    Categories[6]->SetValue(1, rFormat.GetDistance(DIS_MATRIXCOL));
    Categories[7]->SetValue(0, rFormat.GetDistance(DIS_ORNAMENTSIZE));
    Categories[7]->SetValue(1, rFormat.GetDistance(DIS_ORNAMENTSPACE));
    Categories[8]->SetValue(0, rFormat.GetDistance(DIS_OPERATORSIZE));
    Categories[8]->SetValue(1, rFormat.GetDistance(DIS_OPERATORSPACE));
    Categories[9]->SetValue(0, rFormat.GetDistance(DIS_LEFTSPACE));
    Categories[9]->SetValue(1, rFormat.GetDistance(DIS_RIGHTSPACE));
    Categories[9]->SetValue(2, rFormat.GetDistance(DIS_TOPSPACE));
    Categories[9]->SetValue(3, rFormat.GetDistance(DIS_BOTTOMSPACE));

    bScaleAllBrackets = rFormat.IsScaleNormalBrackets();

    // force update (even of category 0) by setting nActiveCategory to a
    // non-existent category number
    nActiveCategory = CATEGORY_NONE;
    SetCategory(0);
}


void SmDistanceDialog::WriteTo(SmFormat &rFormat) /*const*/
{
    // hmm... koennen die tatsaechlich unterschiedlich sein?
    // wenn nicht kann oben naemlich das const stehen!
    SetCategory(nActiveCategory);

    rFormat.SetDistance( DIS_HORIZONTAL,    	Categories[0]->GetValue(0) );
    rFormat.SetDistance( DIS_VERTICAL,  		Categories[0]->GetValue(1) );
    rFormat.SetDistance( DIS_ROOT,  			Categories[0]->GetValue(2) );
    rFormat.SetDistance( DIS_SUPERSCRIPT,   	Categories[1]->GetValue(0) );
    rFormat.SetDistance( DIS_SUBSCRIPT,     	Categories[1]->GetValue(1) );
    rFormat.SetDistance( DIS_NUMERATOR,     	Categories[2]->GetValue(0) );
    rFormat.SetDistance( DIS_DENOMINATOR,   	Categories[2]->GetValue(1) );
    rFormat.SetDistance( DIS_FRACTION,  		Categories[3]->GetValue(0) );
    rFormat.SetDistance( DIS_STROKEWIDTH,   	Categories[3]->GetValue(1) );
    rFormat.SetDistance( DIS_UPPERLIMIT,    	Categories[4]->GetValue(0) );
    rFormat.SetDistance( DIS_LOWERLIMIT,    	Categories[4]->GetValue(1) );
    rFormat.SetDistance( DIS_BRACKETSIZE,   	Categories[5]->GetValue(0) );
    rFormat.SetDistance( DIS_BRACKETSPACE,  	Categories[5]->GetValue(1) );
    rFormat.SetDistance( DIS_MATRIXROW,     	Categories[6]->GetValue(0) );
    rFormat.SetDistance( DIS_MATRIXCOL,     	Categories[6]->GetValue(1) );
    rFormat.SetDistance( DIS_ORNAMENTSIZE,  	Categories[7]->GetValue(0) );
    rFormat.SetDistance( DIS_ORNAMENTSPACE, 	Categories[7]->GetValue(1) );
    rFormat.SetDistance( DIS_OPERATORSIZE,  	Categories[8]->GetValue(0) );
    rFormat.SetDistance( DIS_OPERATORSPACE, 	Categories[8]->GetValue(1) );
    rFormat.SetDistance( DIS_LEFTSPACE,     	Categories[9]->GetValue(0) );
    rFormat.SetDistance( DIS_RIGHTSPACE,    	Categories[9]->GetValue(1) );
    rFormat.SetDistance( DIS_TOPSPACE,  		Categories[9]->GetValue(2) );
    rFormat.SetDistance( DIS_BOTTOMSPACE,   	Categories[9]->GetValue(3) );
    rFormat.SetDistance( DIS_NORMALBRACKETSIZE, Categories[5]->GetValue(3) );

    rFormat.SetScaleNormalBrackets( bScaleAllBrackets );

    rFormat.RequestApplyChanges();
}


/**************************************************************************/


IMPL_LINK( SmAlignDialog, DefaultButtonClickHdl, Button *, EMPTYARG /*pButton*/ )
{
   QueryBox *pQueryBox = new QueryBox(this, SmResId(RID_DEFAULTSAVEQUERY));

    if (pQueryBox->Execute() == RET_YES)
    {
        SmModule *pp = SM_MOD1();
        SmFormat aFmt( pp->GetConfig()->GetStandardFormat() );
        WriteTo( aFmt );
        pp->GetConfig()->SetStandardFormat( aFmt );
    }

    delete pQueryBox;
    return 0;
}


SmAlignDialog::SmAlignDialog(Window * pParent, BOOL bFreeRes)
    : ModalDialog(pParent, SmResId(RID_ALIGNDIALOG)),
    aLeft		   (this, SmResId(1)),
    aCenter 	   (this, SmResId(2)),
    aRight		   (this, SmResId(3)),
    aFixedLine1    (this, SmResId(1)),
    aOKButton1	   (this, SmResId(1)),
    aCancelButton1 (this, SmResId(1)),
    aDefaultButton (this, SmResId(1))
{
    if (bFreeRes)
        FreeResource();

    aDefaultButton.SetClickHdl(LINK(this, SmAlignDialog, DefaultButtonClickHdl));
}


void SmAlignDialog::ReadFrom(const SmFormat &rFormat)
{
    switch (rFormat.GetHorAlign())
    {
        case AlignLeft:
            aLeft  .Check(TRUE);
            aCenter.Check(FALSE);
            aRight .Check(FALSE);
            break;

        case AlignCenter:
            aLeft  .Check(FALSE);
            aCenter.Check(TRUE);
            aRight .Check(FALSE);
            break;

        case AlignRight:
            aLeft  .Check(FALSE);
            aCenter.Check(FALSE);
            aRight .Check(TRUE);
            break;
    }
}


void SmAlignDialog::WriteTo(SmFormat &rFormat) const
{
    if (aLeft.IsChecked())
        rFormat.SetHorAlign(AlignLeft);
    else if (aRight.IsChecked())
        rFormat.SetHorAlign(AlignRight);
    else
        rFormat.SetHorAlign(AlignCenter);

    rFormat.RequestApplyChanges();
}


/**************************************************************************/


void SmShowSymbolSet::Paint(const Rectangle&)
{
    Push(PUSH_MAPMODE);

    // MapUnit einstellen fuer die 'nLen' berechnet wurde
    SetMapMode(MapMode(MAP_PIXEL));

    USHORT v        = sal::static_int_cast< USHORT >((aVScrollBar.GetThumbPos() * nColumns));
    USHORT nSymbols = aSymbolSet.GetCount();

    Color aTxtColor( GetTextColor() );
    for (USHORT i = v; i < nSymbols ; i++)
    {
        SmSym 	 aSymbol (aSymbolSet.GetSymbol(i));
        Font     aFont	 (aSymbol.GetFace());
        aFont.SetAlign(ALIGN_TOP);

        // etwas kleinere FontSize nehmen (als nLen) um etwas Luft zu haben
        // (hoffentlich auch genug fuer links und rechts!)
        aFont.SetSize(Size(0, nLen - (nLen / 3)));
        SetFont(aFont);
        // keep text color
        SetTextColor( aTxtColor );

        int   nIV 	= i - v;
        Size  aSize(GetTextWidth(aSymbol.GetCharacter()), GetTextHeight());

        DrawText(Point((nIV % nColumns) * nLen + (nLen - aSize.Width()) / 2,
                       (nIV / nColumns) * nLen + (nLen - aSize.Height()) / 2),
                 aSymbol.GetCharacter());
    }

    if (nSelectSymbol != SYMBOL_NONE)
    {
        Invert(Rectangle(Point(((nSelectSymbol - v) % nColumns) * nLen,
                                 ((nSelectSymbol - v) / nColumns) * nLen),
                           Size(nLen, nLen)));
    }

    Pop();
}


void SmShowSymbolSet::MouseButtonDown(const MouseEvent& rMEvt)
{
    GrabFocus();

    if (rMEvt.IsLeft() && Rectangle(Point(0, 0), aOutputSize).IsInside(rMEvt.GetPosPixel()))
    {
        long nPos = (rMEvt.GetPosPixel().Y() / nLen) * nColumns + (rMEvt.GetPosPixel().X() / nLen) +
                      aVScrollBar.GetThumbPos() * nColumns;
        SelectSymbol( sal::static_int_cast< USHORT >(nPos) );

        aSelectHdlLink.Call(this);

        if (rMEvt.GetClicks() > 1) aDblClickHdlLink.Call(this);
    }
    else Control::MouseButtonDown (rMEvt);
}


void SmShowSymbolSet::KeyInput(const KeyEvent& rKEvt)
{
    USHORT n = nSelectSymbol;

    if (n != SYMBOL_NONE)
    {
        switch (rKEvt.GetKeyCode().GetCode())
        {
            case KEY_DOWN:      n = n + nColumns;   break;
            case KEY_UP:        n = n - nColumns;   break;
            case KEY_LEFT:		n -= 1;	break;
            case KEY_RIGHT:		n += 1;	break;
            case KEY_HOME:		n  = 0;	break;
            case KEY_END:		n  = aSymbolSet.GetCount() - 1;	break;
            case KEY_PAGEUP:	n -= nColumns * nRows;	break;
            case KEY_PAGEDOWN:	n += nColumns * nRows;	break;

            default:
                Control::KeyInput(rKEvt);
                return;
        }
    }
    else
        n = 0;

    if (n >= aSymbolSet.GetCount())
        n = nSelectSymbol;

    // adjust scrollbar
    if ((n < (USHORT) (aVScrollBar.GetThumbPos() * nColumns)) ||
        (n >= (USHORT) ((aVScrollBar.GetThumbPos() + nRows) * nColumns)))
    {
        aVScrollBar.SetThumbPos(n / nColumns);
        Invalidate();
        Update();
    }

    SelectSymbol(n);
    aSelectHdlLink.Call(this);
}


SmShowSymbolSet::SmShowSymbolSet(Window *pParent, const ResId& rResId) :
    Control(pParent, rResId),
    aVScrollBar(this, WinBits(WB_VSCROLL))
{
    nSelectSymbol = SYMBOL_NONE;

    aOutputSize = GetOutputSizePixel();
    long nScrollBarWidth = aVScrollBar.GetSizePixel().Width(),
         nUseableWidth   = aOutputSize.Width() - nScrollBarWidth;

    // Hoehe von 16pt in Pixeln (passend zu 'aOutputSize')
    nLen = (USHORT) LogicToPixel(Size(0, 16), MapMode(MAP_POINT)).Height();

    nColumns = sal::static_int_cast< USHORT >(nUseableWidth / nLen);
    if (nColumns > 2  && nColumns % 2 != 0)
        nColumns--;
    nRows    = sal::static_int_cast< USHORT >(aOutputSize.Height() / nLen);
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(nColumns > 0, "Sm : keine Spalten");
    DBG_ASSERT(nRows > 0, "Sm : keine Zeilen");
#endif

    // genau passend machen
    aOutputSize.Width()	 = nColumns * nLen;
    aOutputSize.Height() = nRows * nLen;

    aVScrollBar.SetPosSizePixel(Point(aOutputSize.Width() + 1, -1),
                                Size(nScrollBarWidth, aOutputSize.Height() + 2));
    aVScrollBar.Enable(FALSE);
    aVScrollBar.Show();
    aVScrollBar.SetScrollHdl(LINK(this, SmShowSymbolSet, ScrollHdl));

    Size WindowSize (aOutputSize);
    WindowSize.Width() += nScrollBarWidth;
    SetOutputSizePixel(WindowSize);

}


void SmShowSymbolSet::SetSymbolSet(const SmSymSet& rSymbolSet)
{
    aSymbolSet = rSymbolSet;

    if (aSymbolSet.GetCount() > (nColumns * nRows))
    {
        aVScrollBar.SetRange(Range(0, ((aSymbolSet.GetCount() + (nColumns - 1)) / nColumns) - nRows));
        aVScrollBar.Enable(TRUE);
    }
    else
    {
        aVScrollBar.SetRange(Range(0,0));
        aVScrollBar.Enable (FALSE);
    }

    Invalidate();
}


void SmShowSymbolSet::SelectSymbol(USHORT nSymbol)
{
    int v = (int) (aVScrollBar.GetThumbPos() * nColumns);

    if (nSelectSymbol != SYMBOL_NONE)
        Invalidate(Rectangle(Point(((nSelectSymbol - v) % nColumns) * nLen,
                                   ((nSelectSymbol - v) / nColumns) * nLen),
                             Size(nLen, nLen)));

    if (nSymbol < aSymbolSet.GetCount())
        nSelectSymbol = nSymbol;

    if (aSymbolSet.GetCount() == 0)
        nSelectSymbol = SYMBOL_NONE;

    if (nSelectSymbol != SYMBOL_NONE)
        Invalidate(Rectangle(Point(((nSelectSymbol - v) % nColumns) * nLen,
                                   ((nSelectSymbol - v) / nColumns) * nLen),
                             Size(nLen, nLen)));

    Update();
}


IMPL_LINK( SmShowSymbolSet, ScrollHdl, ScrollBar*, EMPTYARG /*pScrollBar*/)
{
    Invalidate();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void SmShowSymbol::Paint(const Rectangle &rRect)
{
    Control::Paint( rRect );

    const XubString &rText = GetText();
    Size            aTextSize(GetTextWidth(rText), GetTextHeight());

    DrawText(Point((GetOutputSize().Width()  - aTextSize.Width())  / 2,
                   (GetOutputSize().Height() - aTextSize.Height()) / 2), rText);
}


void SmShowSymbol::MouseButtonDown(const MouseEvent& rMEvt)
{
    if (rMEvt.GetClicks() > 1)
        aDblClickHdlLink.Call(this);
    else
        Control::MouseButtonDown (rMEvt);
}


void SmShowSymbol::SetSymbol(const SmSym *pSymbol)
{
    if (pSymbol)
    {
        Color aTxtColor( GetTextColor() );

        Font aFont (pSymbol->GetFace());
        aFont.SetSize(Size(0, GetOutputSize().Height() - GetOutputSize().Height() / 3));
        aFont.SetAlign(ALIGN_TOP);
        SetFont(aFont);

        // keep old text color (font may have different color set)
        SetTextColor(aTxtColor);

        SetText(XubString(pSymbol->GetCharacter()));
    }

    // 'Invalidate' fuellt den background mit der background-Farbe.
    // Falls der NULL pointer uebergeben wurde reicht dies also zum loeschen
    // der Anzeige
    Invalidate();
}


////////////////////////////////////////////////////////////////////////////////

void SmSymbolDialog::FillSymbolSets(BOOL bDeleteText)
    // fuellt die Eintraege der moeglichen 'SymbolsSet's im Dialog mit den
    // aktuellen Werten des SymbolSet Managers, selektiert aber keinen.
{
    aSymbolSets.Clear();
    if (bDeleteText)
        aSymbolSets.SetNoSelection();

    USHORT nNumSymSets = rSymSetMgr.GetSymbolSetCount();
    for (USHORT i = 0;  i < nNumSymSets;  i++)
        aSymbolSets.InsertEntry(rSymSetMgr.GetSymbolSet(i)->GetName());
}


IMPL_LINK( SmSymbolDialog, SymbolSetChangeHdl, ListBox *, EMPTYARG pListBox )
{
    (void) pListBox;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pListBox == &aSymbolSets, "Sm : falsches Argument");
#endif

    SelectSymbolSet(aSymbolSets.GetSelectEntry());
    return 0;
}


IMPL_LINK( SmSymbolDialog, SymbolChangeHdl, SmShowSymbolSet *, EMPTYARG pShowSymbolSet )
{
    (void) pShowSymbolSet;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pShowSymbolSet == &aSymbolSetDisplay, "Sm : falsches Argument");
#endif

    SelectSymbol(aSymbolSetDisplay.GetSelectSymbol());
    return 0;
}

IMPL_LINK( SmSymbolDialog, EditClickHdl, Button *, EMPTYARG pButton )
{
    (void) pButton;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pButton == &aEditBtn, "Sm : falsches Argument");
#endif

    SmSymDefineDialog *pDialog = new SmSymDefineDialog(this, pFontListDev, rSymSetMgr);

    // aktuelles Symbol und SymbolSet am neuen Dialog setzen
    const XubString  aSymSetName (aSymbolSets.GetSelectEntry()),
                    aSymName    (aSymbolName.GetText());
    pDialog->SelectOldSymbolSet(aSymSetName);
    pDialog->SelectOldSymbol(aSymName);
    pDialog->SelectSymbolSet(aSymSetName);
    pDialog->SelectSymbol(aSymName);

    // altes SymbolSet merken
    XubString  aOldSymbolSet (aSymbolSets.GetSelectEntry());

    USHORT nSymPos = GetSelectedSymbol();

    // Dialog an evtl geaenderte Daten des SymbolSet Manager anpassen
    if (pDialog->Execute() == RET_OK  &&  rSymSetMgr.IsModified())
    {
        rSymSetMgr.Save();
        FillSymbolSets();
    }

    // wenn das alte SymbolSet nicht mehr existiert zum ersten gehen
    // (soweit eines vorhanden ist)
    if (!SelectSymbolSet(aOldSymbolSet)  &&  aSymbolSets.GetEntryCount() > 0)
        SelectSymbolSet(aSymbolSets.GetEntry(0));

    SelectSymbol( nSymPos );

    delete pDialog;
    return 0;
}


IMPL_LINK( SmSymbolDialog, SymbolDblClickHdl, SmShowSymbolSet *, EMPTYARG pShowSymbolSet )
{
    (void) pShowSymbolSet;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pShowSymbolSet == &aSymbolSetDisplay, "Sm : falsches Argument");
#endif

    GetClickHdl(&aGetBtn);
    EndDialog(RET_OK);
    return 0;
}


IMPL_LINK( SmSymbolDialog, GetClickHdl, Button *, EMPTYARG pButton )
{
    (void) pButton;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pButton == &aGetBtn, "Sm : falscher Button");
#endif

    const SmSym *pSym = GetSymbol();
    if (pSym)
    {
        XubString	aText ('%');
        aText += pSym->GetName();

        rViewSh.GetViewFrame()->GetDispatcher()->Execute(
                SID_INSERTTEXT, SFX_CALLMODE_STANDARD,
                new SfxStringItem(SID_INSERTTEXT, aText), 0L);
    }

    return 0;
}


IMPL_LINK_INLINE_START( SmSymbolDialog, CloseClickHdl, Button *, EMPTYARG pButton )
{
    (void) pButton;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pButton == &aCloseBtn, "Sm : falscher Button");
#endif

    EndDialog(TRUE);
    return 0;
}
IMPL_LINK_INLINE_END( SmSymbolDialog, CloseClickHdl, Button *, pButton )


SmSymbolDialog::SmSymbolDialog(Window *pParent, OutputDevice *pFntListDevice,
        SmSymSetManager &rMgr, SmViewShell &rViewShell, BOOL bFreeRes) :
    ModalDialog			(pParent, SmResId(RID_SYMBOLDIALOG)),
    aSymbolSetText      (this, SmResId(1)),
    aSymbolSets         (this, SmResId(1)),
    aSymbolSetDisplay   (this, SmResId(1)),
    aSymbolName         (this, SmResId(2)),
    aSymbolDisplay      (this, SmResId(2)),
    aGetBtn             (this, SmResId(2)),
    aCloseBtn           (this, SmResId(3)),
    aEditBtn            (this, SmResId(1)),
    rViewSh             (rViewShell),
    rSymSetMgr          (rMgr),
    pFontListDev        (pFntListDevice)
{
    if (bFreeRes)
        FreeResource();

    pSymSet = NULL;
    FillSymbolSets();
    if (aSymbolSets.GetEntryCount() > 0)
        SelectSymbolSet(aSymbolSets.GetEntry(0));

    InitColor_Impl();

    // preview like controls should have a 2D look
    aSymbolDisplay.SetBorderStyle( WINDOW_BORDER_MONO );

    aSymbolSets		 .SetSelectHdl	(LINK(this, SmSymbolDialog, SymbolSetChangeHdl));
    aSymbolSetDisplay.SetSelectHdl  (LINK(this, SmSymbolDialog, SymbolChangeHdl));
    aSymbolSetDisplay.SetDblClickHdl(LINK(this, SmSymbolDialog, SymbolDblClickHdl));
    aSymbolDisplay	 .SetDblClickHdl(LINK(this, SmSymbolDialog, SymbolDblClickHdl));
    aCloseBtn		 .SetClickHdl   (LINK(this, SmSymbolDialog, CloseClickHdl));
    aEditBtn		 .SetClickHdl   (LINK(this, SmSymbolDialog, EditClickHdl));
    aGetBtn			 .SetClickHdl   (LINK(this, SmSymbolDialog, GetClickHdl));
}


SmSymbolDialog::~SmSymbolDialog()
{
}


void SmSymbolDialog::InitColor_Impl()
{
#if OSL_DEBUG_LEVEL > 1
    Color aBC( GetDisplayBackground().GetColor() );
#endif
    ColorData   nBgCol  = COL_WHITE,
                nTxtCol = COL_BLACK;
    if (GetDisplayBackground().GetColor().IsDark())
    {
        const StyleSettings &rS = GetSettings().GetStyleSettings();
        nBgCol  = rS.GetFieldColor().GetColor();
        nTxtCol = rS.GetFieldTextColor().GetColor();
    }

    Color aTmpColor( nBgCol );
    Wallpaper aWall( aTmpColor );
    Color aTxtColor( nTxtCol );
    aSymbolDisplay   .SetBackground( aWall );
    aSymbolDisplay   .SetTextColor( aTxtColor );
    aSymbolSetDisplay.SetBackground( aWall );
    aSymbolSetDisplay.SetTextColor( aTxtColor );
}


void SmSymbolDialog::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( rDCEvt.GetType() == DATACHANGED_SETTINGS  &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            InitColor_Impl();

    ModalDialog::DataChanged( rDCEvt );
}


BOOL SmSymbolDialog::SelectSymbolSet(const XubString &rSymbolSetName)
{
    BOOL	bRet = FALSE;
    USHORT 	nPos = aSymbolSets.GetEntryPos(rSymbolSetName);

    pSymSet = NULL;
    if (nPos != LISTBOX_ENTRY_NOTFOUND)
    {
        aSymbolSets.SelectEntryPos(nPos);
        USHORT nSymbolSetNo = rSymSetMgr.GetSymbolSetPos(aSymbolSets.GetSelectEntry());
        pSymSet = rSymSetMgr.GetSymbolSet(nSymbolSetNo);
#if OSL_DEBUG_LEVEL > 1
        DBG_ASSERT(pSymSet, "Sm : NULL pointer");
#endif

        aSymbolSetDisplay.SetSymbolSet(*pSymSet);
        if (pSymSet->GetCount() > 0)
            SelectSymbol(0);

        bRet = TRUE;
    }
    else
        aSymbolSets.SetNoSelection();

    return bRet;
}


void SmSymbolDialog::SelectSymbol(USHORT nSymbolNo)
{
    const SmSym *pSym = NULL;
    if (pSymSet  &&  nSymbolNo < pSymSet->GetCount())
        pSym = &pSymSet->GetSymbol(nSymbolNo);

    aSymbolSetDisplay.SelectSymbol(nSymbolNo);
    aSymbolDisplay.SetSymbol(pSym);
    aSymbolName.SetText(pSym ? pSym->GetName() : XubString());
}


const SmSym * SmSymbolDialog::GetSymbol() const
{
    USHORT nSymbolNo = aSymbolSetDisplay.GetSelectSymbol();
    return pSymSet == NULL ? NULL : &pSymSet->GetSymbol(nSymbolNo);
}


////////////////////////////////////////////////////////////////////////////////


void SmShowChar::Paint(const Rectangle &rRect)
{
    Control::Paint( rRect );

    XubString Text (GetText ());
    if (Text.Len() > 0)
    {
        Size aTextSize(GetTextWidth(Text), GetTextHeight());

        DrawText(Point((GetOutputSize().Width()  - aTextSize.Width())  / 2,
                       (GetOutputSize().Height() - aTextSize.Height()) / 2), Text);
    }
}


void SmShowChar::SetChar(xub_Unicode aChar)
{
    SetText(XubString(aChar));
    Invalidate();
}


void SmShowChar::SetFont(const Font &rFont)
{
    Color aTxtColor( GetTextColor() );

    Font  aFont (rFont);
    Size  aSize (Size(0, GetOutputSize().Height() - GetOutputSize().Height() / 3));

    aFont.SetSize(aSize);
    aFont.SetAlign(ALIGN_TOP);
    aFont.SetTransparent(TRUE);
    Control::SetFont(aFont);

    // keep text color (new font may have different one)
    SetTextColor( aTxtColor );

    Invalidate();
}

////////////////////////////////////////////////////////////////////////////////

void SmSymDefineDialog::FillSymbols(ComboBox &rComboBox, BOOL bDeleteText)
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(&rComboBox == &aOldSymbols  ||  &rComboBox == &aSymbols,
        "Sm : falsche ComboBox");
#endif

    rComboBox.Clear();
    if (bDeleteText)
        rComboBox.SetText(XubString());

    ComboBox &rSymbolSets = &rComboBox == &aOldSymbols ?
                                    aOldSymbolSets : aSymbolSets;
    const SmSymSet *pSymSet = GetSymbolSet(rSymbolSets);
    if (pSymSet)
    {   USHORT nNumSymbols = pSymSet->GetCount();
        for (USHORT i = 0;  i < nNumSymbols;  i++)
            rComboBox.InsertEntry(pSymSet->GetSymbol(i).GetName());
    }

}


void SmSymDefineDialog::FillSymbolSets(ComboBox &rComboBox, BOOL bDeleteText)
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(&rComboBox == &aOldSymbolSets  ||  &rComboBox == &aSymbolSets,
        "Sm : falsche ComboBox");
#endif

    rComboBox.Clear();
    if (bDeleteText)
        rComboBox.SetText(XubString());

    USHORT nNumSymSets = aSymSetMgrCopy.GetSymbolSetCount();
    for (USHORT i = 0;  i < nNumSymSets;  i++)
        rComboBox.InsertEntry(aSymSetMgrCopy.GetSymbolSet(i)->GetName());
}


void SmSymDefineDialog::FillFonts(BOOL bDelete)
{
    aFonts.Clear();
    if (bDelete)
        aFonts.SetNoSelection();

    // alle Fonts der 'FontList' in die Fontliste aufnehmen
    // von denen mit gleichen Namen jedoch nur einen (denn der Style wird
    // ueber die 'FontStyleBox' gewaehlt und nicht auch noch hier)
    if (pFontList)
    {
        USHORT  nCount = pFontList->GetFontNameCount();
        for (USHORT i = 0;  i < nCount;  i++)
            aFonts.InsertEntry( pFontList->GetFontName(i).GetName() );
    }
}


void SmSymDefineDialog::FillStyles(BOOL bDeleteText)
{
    aStyles.Clear();
    if (bDeleteText)
        aStyles.SetText(XubString());

    XubString aText (aFonts.GetSelectEntry());
    if (aText.Len() != 0)
    {
        //aStyles.Fill(aText, &aFontList);
        // eigene StyleName's verwenden
        const SmFontStyles &rStyles = GetFontStyles();
        for (USHORT i = 0;  i < rStyles.GetCount();  i++)
            aStyles.InsertEntry( rStyles.GetStyleName(i) );

#if OSL_DEBUG_LEVEL > 1
        DBG_ASSERT(aStyles.GetEntryCount() > 0, "Sm : keine Styles vorhanden");
#endif
        aStyles.SetText( aStyles.GetEntry(0) );
    }
}


SmSymSet * SmSymDefineDialog::GetSymbolSet(const ComboBox &rComboBox)
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(&rComboBox == &aOldSymbolSets  ||  &rComboBox == &aSymbolSets,
        "Sm : falsche ComboBox");
#endif
    USHORT nSymbolSetNo = aSymSetMgrCopy.GetSymbolSetPos(rComboBox.GetText());

    return nSymbolSetNo == SYMBOLSET_NONE ?
        NULL : aSymSetMgrCopy.GetSymbolSet(nSymbolSetNo);
}


SmSym * SmSymDefineDialog::GetSymbol(const ComboBox &rComboBox)
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(&rComboBox == &aOldSymbols  ||  &rComboBox == &aSymbols,
        "Sm : falsche ComboBox");
#endif
    return aSymSetMgrCopy.GetSymbolByName(rComboBox.GetText());
}


IMPL_LINK( SmSymDefineDialog, OldSymbolChangeHdl, ComboBox *, EMPTYARG pComboBox )
{
    (void) pComboBox;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pComboBox == &aOldSymbols, "Sm : falsches Argument");
#endif
    SelectSymbol(aOldSymbols, aOldSymbols.GetText(), FALSE);
    return 0;
}


IMPL_LINK( SmSymDefineDialog, OldSymbolSetChangeHdl, ComboBox *, EMPTYARG pComboBox )
{
    (void) pComboBox;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pComboBox == &aOldSymbolSets, "Sm : falsches Argument");
#endif
    SelectSymbolSet(aOldSymbolSets, aOldSymbolSets.GetText(), FALSE);
    return 0;
}


IMPL_LINK( SmSymDefineDialog, ModifyHdl, ComboBox *, pComboBox )
{
    // merken der Cursorposition zum wiederherstellen derselben
    Selection  aSelection (pComboBox->GetSelection());

    if (pComboBox == &aSymbols)
        SelectSymbol(aSymbols, aSymbols.GetText(), FALSE);
    else if (pComboBox == &aSymbolSets)
        SelectSymbolSet(aSymbolSets, aSymbolSets.GetText(), FALSE);
    else if (pComboBox == &aOldSymbols)
        // nur Namen aus der Liste erlauben
        SelectSymbol(aOldSymbols, aOldSymbols.GetText(), TRUE);
    else if (pComboBox == &aOldSymbolSets)
        // nur Namen aus der Liste erlauben
        SelectSymbolSet(aOldSymbolSets, aOldSymbolSets.GetText(), TRUE);
    else if (pComboBox == &aStyles)
        // nur Namen aus der Liste erlauben (ist hier eh immer der Fall)
        SelectStyle(aStyles.GetText(), TRUE);
    else
    {
#if OSL_DEBUG_LEVEL > 1
        DBG_ASSERT(0, "Sm : falsche ComboBox Argument");
#endif
    }

    pComboBox->SetSelection(aSelection);

    UpdateButtons();

    return 0;
}


IMPL_LINK( SmSymDefineDialog, FontChangeHdl, ListBox *, EMPTYARG pListBox )
{
    (void) pListBox;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pListBox == &aFonts, "Sm : falsches Argument");
#endif

    SelectFont(aFonts.GetSelectEntry());
    return 0;
}


IMPL_LINK( SmSymDefineDialog, SubsetChangeHdl, ListBox *, EMPTYARG pListBox )
{
    (void) pListBox;
    USHORT nPos = aFontsSubsetLB.GetSelectEntryPos();
    if (LISTBOX_ENTRY_NOTFOUND != nPos)
    {
        const Subset* pSubset = reinterpret_cast<const Subset*> (aFontsSubsetLB.GetEntryData( nPos ));
        if (pSubset)
        {
            aCharsetDisplay.SelectCharacter( pSubset->GetRangeMin() );
        }
    }
    return 0;
}


IMPL_LINK( SmSymDefineDialog, StyleChangeHdl, ComboBox *, EMPTYARG pComboBox )
{
    (void) pComboBox;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pComboBox == &aStyles, "Sm : falsches Argument");
#endif

    SelectStyle(aStyles.GetText());
    return 0;
}


IMPL_LINK( SmSymDefineDialog, CharHighlightHdl, Control *, EMPTYARG )
{
    sal_UCS4 cChar = aCharsetDisplay.GetSelectCharacter();

#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT( pSubsetMap, "SubsetMap missing" )
#endif
    if (pSubsetMap)
    {
        const Subset* pSubset = pSubsetMap->GetSubsetByUnicode( cChar );
        if (pSubset)
            aFontsSubsetLB.SelectEntry( pSubset->GetName() );
        else
            aFontsSubsetLB.SetNoSelection();
    }

    // TO_DO_UCS4 (#i74049): get rid of cast without loosing UCS4 functionality
    aSymbolDisplay.SetChar( sal::static_int_cast< sal_Unicode >(cChar) );
    UpdateButtons();
    return 0;
}


IMPL_LINK( SmSymDefineDialog, AddClickHdl, Button *, EMPTYARG pButton )
{
    (void) pButton;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pButton == &aAddBtn, "Sm : falsches Argument");
    DBG_ASSERT(aAddBtn.IsEnabled(), "Sm : Voraussetzungen erfuellt ??");
#endif

    SmSymSet *pSymSet = GetSymbolSet(aSymbolSets);

    // SymbolSet einfuegen falls es noch nicht existiert
    if (!pSymSet)
    {
        pSymSet = new SmSymSet(aSymbolSets.GetText());
        aSymSetMgrCopy.AddSymbolSet(pSymSet);
        FillSymbolSets(aOldSymbolSets, FALSE);
        FillSymbolSets(aSymbolSets,    FALSE);
    }
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pSymSet, "Sm : NULL pointer");
#endif

    // Symbol ins SymbolSet einfuegen
    // TO_DO_UCS4 (#i74049): get rid of cast without loosing UCS4 functionality
    SmSym *pSym = new SmSym(aSymbols.GetText(), aCharsetDisplay.GetFont(),
                            sal::static_int_cast< sal_Unicode >(aCharsetDisplay.GetSelectCharacter()),
                            aSymbolSets.GetText());
    pSymSet->AddSymbol(pSym);

    // update der Hash Tabelle erzwingen (damit aAddBtn disabled wird).
    // (wird spaeter nach Ueberarbeitung von symbol.cxx ueberfluessig werden).
    aSymSetMgrCopy.ChangeSymbolSet((SmSymSet *)1);

    // Symbolliste aktualiseren
    FillSymbols(aOldSymbols ,FALSE);
    FillSymbols(aSymbols    ,FALSE);

    UpdateButtons();

    return 0;
}


IMPL_LINK( SmSymDefineDialog, ChangeClickHdl, Button *, EMPTYARG pButton )
{
    (void) pButton;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pButton == &aChangeBtn, "Sm : falsches Argument");
    DBG_ASSERT(aChangeBtn.IsEnabled(), "Sm : Voraussetzungen erfuellt ??");
#endif

    // finden des SymbolSets zum alten Symbol
    SmSymSet *pOldSymSet = GetSymbolSet(aOldSymbolSets);
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pOldSymSet, "Sm : NULL pointer");
#endif

    // suchen des neuen SymbolSets
    SmSymSet *pNewSymSet = GetSymbolSet(aSymbolSets);
    // SymbolSet einfuegen falls es noch nicht existiert
    if (!pNewSymSet)
    {
        pNewSymSet = new SmSymSet(aSymbolSets.GetText());
        aSymSetMgrCopy.AddSymbolSet(pNewSymSet);
        FillSymbolSets(aOldSymbolSets, FALSE);
        FillSymbolSets(aSymbolSets,    FALSE);
    }

    // das (alte) Symbol besorgen
    USHORT nSymbol = pOldSymSet->GetSymbolPos(aOldSymbols.GetText());
    SmSym *pSym    = (SmSym *) &pOldSymSet->GetSymbol(nSymbol);
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT( SYMBOL_NONE != nSymbol, "symbol not found" );
    DBG_ASSERT(pSym, "Sm : NULL pointer");
#endif

    // apply changes
    pSym->SetName( aSymbols.GetText() );
    //! get font from symbol-display since charset-display does not keep
    //! the bold attribut.
    pSym->SetFace( aSymbolDisplay.GetFont() );
    // TO_DO_UCS4 (#i74049): get rid of cast without loosing UCS4 functionality
    pSym->SetCharacter( sal::static_int_cast< sal_Unicode >(aCharsetDisplay.GetSelectCharacter()) );

    // das SymbolSet wechseln wenn noetig
    if (pOldSymSet != pNewSymSet)
    {
        pNewSymSet->AddSymbol( new SmSym( *pSym ) );
        pOldSymSet->DeleteSymbol(nSymbol);

        //
        // update controls
        //
        // actualize symbol-lists in the dialog
        String  aTmpOldSymbolName( pOrigSymbol->GetName() );
        aOldSymbols.SetText( String() );
        aOldSymbols.RemoveEntry( aTmpOldSymbolName );
        if (aSymbolSets.GetText() == aOldSymbolSets.GetText())
            aSymbols.RemoveEntry( aTmpOldSymbolName );
        // clear display for original symbol
        SetOrigSymbol(NULL, XubString());
    }

    //!! den SymbolSet Manger dazu zwingen seinen HashTable zu aktualisieren,
    //!! um mit moeglich neuen bzw fehlenden alten Symbol Namen konform zu sein.
    aSymSetMgrCopy.ChangeSymbolSet((SmSymSet *)1);

    UpdateButtons();

    return 0;
}


IMPL_LINK( SmSymDefineDialog, DeleteClickHdl, Button *, EMPTYARG pButton )
{
    (void) pButton;
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(pButton == &aDeleteBtn, "Sm : falsches Argument");
    DBG_ASSERT(aDeleteBtn.IsEnabled(), "Sm : Voraussetzungen erfuellt ??");
#endif

    if (pOrigSymbol)
    {
        // loeschen des Symbols:
        //
        // zugehoeriges SymbolSet finden
        SmSymSet *pSymSet = GetSymbolSet(aOldSymbolSets);
        // finden des Index
        XubString  aTmpOldSymbolName (pOrigSymbol->GetName());
        USHORT    nSymbolNo   = pSymSet->GetSymbolPos(aTmpOldSymbolName);
        // Bezuege auf das Symbols loeschen
        SetOrigSymbol(NULL, XubString());
        // und weg mit dem Symbol
        pSymSet->DeleteSymbol(nSymbolNo);
#if OSL_DEBUG_LEVEL > 1
        DBG_ASSERT(pSymSet, "Sm : NULL pointer");
        DBG_ASSERT(nSymbolNo != SYMBOL_NONE, "Sm : kein Symbol");
#endif

        //!! den SymbolSet Manger dazu zwingen seinen HashTable zu aktualisieren,
        //!! was er naemlich nicht tut, wenn in einem seiner SymbolSets geaendert/
        //!! geloescht wird, was wiederum zu einem Absturz fuehren kann (wenn er
        //!! ueber ein nicht mehr existentes aber nicht entferntes Symbol iteriert).
        aSymSetMgrCopy.ChangeSymbolSet((SmSymSet *)1);

        // aktualisieren der Symboleintraege des Dialogs
        aOldSymbols.SetText(XubString());
        aOldSymbols.RemoveEntry(aTmpOldSymbolName);
        if (aSymbolSets.GetText() == aOldSymbolSets.GetText())
            aSymbols.RemoveEntry(aTmpOldSymbolName);
    }

    UpdateButtons();

    return 0;
}


void SmSymDefineDialog::UpdateButtons()
{
    BOOL  bAdd    = FALSE,
          bChange = FALSE,
          bDelete = FALSE,
          bEqual;
    XubString aTmpSymbolName    (aSymbols.GetText()),
              aTmpSymbolSetName (aSymbolSets.GetText());

    if (aTmpSymbolName.Len() > 0  &&  aTmpSymbolSetName.Len() > 0)
    {
        // alle Einstellungen gleich?
        //! (Font-, Style- und SymbolSet Name werden nicht case sensitiv verglichen)
        bEqual = pOrigSymbol
                    && aTmpSymbolSetName.EqualsIgnoreCaseAscii(aOldSymbolSetName.GetText())
                    && aTmpSymbolName.Equals(pOrigSymbol->GetName())
                    && aFonts.GetSelectEntry().EqualsIgnoreCaseAscii(
                            pOrigSymbol->GetFace().GetName())
                    && aStyles.GetText().EqualsIgnoreCaseAscii(
                            GetFontStyles().GetStyleName(pOrigSymbol->GetFace()))
                    && aCharsetDisplay.GetSelectCharacter() == pOrigSymbol->GetCharacter();

        // hinzufuegen nur wenn es noch kein Symbol desgleichen Namens gibt
        bAdd    = aSymSetMgrCopy.GetSymbolByName(aTmpSymbolName) == NULL;

        // loeschen nur wenn alle Einstellungen gleich sind
        bDelete = pOrigSymbol != NULL;

        // aendern wenn bei gleichem Namen mindestens eine Einstellung anders ist
        // oder wenn es noch kein Symbol des neuen Namens gibt (wuerde implizites
        // loeschen des bereits vorhandenen Symbols erfordern)
        BOOL  bEqualName = pOrigSymbol && aTmpSymbolName == pOrigSymbol->GetName();
        bChange = pOrigSymbol && (bEqualName && !bEqual || !bEqualName && bAdd);
    }

    aAddBtn   .Enable(bAdd);
    aChangeBtn.Enable(bChange);
    aDeleteBtn.Enable(bDelete);
}


SmSymDefineDialog::SmSymDefineDialog(Window * pParent,
        OutputDevice *pFntListDevice, SmSymSetManager &rMgr, BOOL bFreeRes) :
    ModalDialog			(pParent, SmResId(RID_SYMDEFINEDIALOG)),
    aOldSymbolText      (this, SmResId(1)),
    aOldSymbols         (this, SmResId(1)),
    aOldSymbolSetText   (this, SmResId(2)),
    aOldSymbolSets      (this, SmResId(2)),
    aCharsetDisplay     (this, SmResId(1)),
    aSymbolText         (this, SmResId(9)),
    aSymbols            (this, SmResId(4)),
    aSymbolSetText      (this, SmResId(10)),
    aSymbolSets         (this, SmResId(5)),
    aFontText           (this, SmResId(3)),
    aFonts              (this, SmResId(1)),
    aFontsSubsetFT      (this, SmResId( FT_FONTS_SUBSET )),
    aFontsSubsetLB      (this, SmResId( LB_FONTS_SUBSET )),
    aStyleText          (this, SmResId(4)),
    aStyles             (this, SmResId(3)),
    aOldSymbolName      (this, SmResId(7)),
    aOldSymbolDisplay   (this, SmResId(3)),
    aOldSymbolSetName   (this, SmResId(8)),
    aSymbolName         (this, SmResId(5)),
    aSymbolDisplay      (this, SmResId(2)),
    aSymbolSetName      (this, SmResId(6)),
    aOkBtn              (this, SmResId(1)),
    aCancelBtn          (this, SmResId(1)),
    aAddBtn             (this, SmResId(1)),
    aChangeBtn          (this, SmResId(2)),
    aDeleteBtn          (this, SmResId(3)),
    aRightArrow         (this, SmResId(1)),
    aRigthArrow_Im      (SmResId(1)),
    aRigthArrow_Im_HC   (SmResId(2)),   // hi-contrast version
    rSymSetMgr          (rMgr),
    pSubsetMap          (NULL),
    pFontList           (NULL)
{
    if (bFreeRes)
        FreeResource();

    pFontList = new FontList( pFntListDevice );

    pOrigSymbol = 0;

    // make autocompletion for symbols case-sensitive
    aOldSymbols.EnableAutocomplete(TRUE, TRUE);
    aSymbols   .EnableAutocomplete(TRUE, TRUE);

    FillFonts();
    if (aFonts.GetEntryCount() > 0)
        SelectFont(aFonts.GetEntry(0));

    InitColor_Impl();

    SetSymbolSetManager(rSymSetMgr);

    aOldSymbols	   .SetSelectHdl(LINK(this, SmSymDefineDialog, OldSymbolChangeHdl));
    aOldSymbolSets .SetSelectHdl(LINK(this, SmSymDefineDialog, OldSymbolSetChangeHdl));
    aSymbolSets    .SetModifyHdl(LINK(this, SmSymDefineDialog, ModifyHdl));
    aOldSymbolSets .SetModifyHdl(LINK(this, SmSymDefineDialog, ModifyHdl));
    aSymbols       .SetModifyHdl(LINK(this, SmSymDefineDialog, ModifyHdl));
    aOldSymbols    .SetModifyHdl(LINK(this, SmSymDefineDialog, ModifyHdl));
    aStyles		   .SetModifyHdl(LINK(this, SmSymDefineDialog, ModifyHdl));
    aFonts         .SetSelectHdl(LINK(this, SmSymDefineDialog, FontChangeHdl));
    aFontsSubsetLB .SetSelectHdl(LINK(this, SmSymDefineDialog, SubsetChangeHdl));
    aStyles        .SetSelectHdl(LINK(this, SmSymDefineDialog, StyleChangeHdl));
    aAddBtn        .SetClickHdl (LINK(this, SmSymDefineDialog, AddClickHdl));
    aChangeBtn     .SetClickHdl (LINK(this, SmSymDefineDialog, ChangeClickHdl));
    aDeleteBtn     .SetClickHdl (LINK(this, SmSymDefineDialog, DeleteClickHdl));
    aCharsetDisplay.SetHighlightHdl( LINK( this, SmSymDefineDialog, CharHighlightHdl ) );

    // preview like controls should have a 2D look
    aOldSymbolDisplay.SetBorderStyle( WINDOW_BORDER_MONO );
    aSymbolDisplay   .SetBorderStyle( WINDOW_BORDER_MONO );
}


SmSymDefineDialog::~SmSymDefineDialog()
{
    delete pSubsetMap;
    delete pOrigSymbol;
}

void SmSymDefineDialog::InitColor_Impl()
{
#if OSL_DEBUG_LEVEL > 1
    Color aBC( GetDisplayBackground().GetColor() );
#endif
    ColorData   nBgCol  = COL_WHITE,
                nTxtCol = COL_BLACK;
    BOOL bHighContrast = GetDisplayBackground().GetColor().IsDark() != 0;
    if (bHighContrast)
    {
        const StyleSettings &rS = GetSettings().GetStyleSettings();
        nBgCol  = rS.GetFieldColor().GetColor();
        nTxtCol = rS.GetFieldTextColor().GetColor();
    }

    Color aTmpColor( nBgCol );
    Wallpaper aWall( aTmpColor );
    Color aTxtColor( nTxtCol );
    aCharsetDisplay  .SetBackground( aWall );
    aCharsetDisplay  .SetTextColor( aTxtColor );
    aOldSymbolDisplay.SetBackground( aWall );
    aOldSymbolDisplay.SetTextColor( aTxtColor );
    aSymbolDisplay   .SetBackground( aWall );
    aSymbolDisplay   .SetTextColor( aTxtColor );
    
    const Image &rArrowRight = bHighContrast ? aRigthArrow_Im_HC : aRigthArrow_Im;
    aRightArrow.SetImage( rArrowRight );
}


void SmSymDefineDialog::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( rDCEvt.GetType() == DATACHANGED_SETTINGS  &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            InitColor_Impl();

    ModalDialog::DataChanged( rDCEvt );
}


short SmSymDefineDialog::Execute()
{
    short nResult = ModalDialog::Execute();

    // Aenderungen uebernehmen falls Dialog mit OK beendet wurde
    if (aSymSetMgrCopy.IsModified()  &&  nResult == RET_OK)
    {
        // leere SymbolSets aus dem Ergebnis entfernen.
        // Dabei von hinten durch das array iterieren, da beim loeschen die
        // Elemente aufruecken.
        USHORT  nSymbolSets = aSymSetMgrCopy.GetSymbolSetCount();
        for (USHORT i = 0;  i < nSymbolSets;  i++)
        {
            USHORT nIdx = nSymbolSets - 1 - i;
            if (aSymSetMgrCopy.GetSymbolSet(nIdx)->GetCount() == 0)
                aSymSetMgrCopy.DeleteSymbolSet(nIdx);
        }



        rSymSetMgr = aSymSetMgrCopy;
#ifdef DEBUG
//        USHORT nS = rSymSetMgr.GetSymbolSetCount();
#endif
    }

    return nResult;
}


void SmSymDefineDialog::SetSymbolSetManager(const SmSymSetManager &rMgr)
{
    aSymSetMgrCopy = rMgr;
#ifdef DEBUG
//        USHORT nS = aSymSetMgrCopy.GetSymbolSetCount();
#endif

    // Das modified Flag der Kopie auf FALSE setzen, damit man spaeter damit
    // testen kann ob sich was geaendert hat.
    aSymSetMgrCopy.SetModified(FALSE);

    FillSymbolSets(aOldSymbolSets);
    if (aOldSymbolSets.GetEntryCount() > 0)
        SelectSymbolSet(aOldSymbolSets.GetEntry(0));
    FillSymbolSets(aSymbolSets);
    if (aSymbolSets.GetEntryCount() > 0)
        SelectSymbolSet(aSymbolSets.GetEntry(0));
    FillSymbols(aOldSymbols);
    if (aOldSymbols.GetEntryCount() > 0)
        SelectSymbol(aOldSymbols.GetEntry(0));
    FillSymbols(aSymbols);
    if (aSymbols.GetEntryCount() > 0)
        SelectSymbol(aSymbols.GetEntry(0));

    UpdateButtons();
}


BOOL SmSymDefineDialog::SelectSymbolSet(ComboBox &rComboBox,
        const XubString &rSymbolSetName, BOOL bDeleteText)
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(&rComboBox == &aOldSymbolSets  ||  &rComboBox == &aSymbolSets,
        "Sm : falsche ComboBox");
#endif

    // 'Normalisieren' des SymbolNamens (ohne leading und trailing Leerzeichen)
    XubString  aNormName (rSymbolSetName);
    aNormName.EraseLeadingChars(' ');
    aNormName.EraseTrailingChars(' ');
    // und evtl Abweichungen in der Eingabe beseitigen
    rComboBox.SetText(aNormName);

    BOOL   bRet = FALSE;
    USHORT nPos = rComboBox.GetEntryPos(aNormName);

    if (nPos != COMBOBOX_ENTRY_NOTFOUND)
    {
        rComboBox.SetText(rComboBox.GetEntry(nPos));
        bRet = TRUE;
    }
    else if (bDeleteText)
        rComboBox.SetText(XubString());

    BOOL  bIsOld = &rComboBox == &aOldSymbolSets;

    // setzen des SymbolSet Namens an der zugehoerigen Darstellung
    FixedText &rFT = bIsOld ? aOldSymbolSetName : aSymbolSetName;
    rFT.SetText(rComboBox.GetText());

    // setzen der zum SymbolSet gehoerenden Symbol Namen an der zugehoerigen
    // Auswahbox
    ComboBox  &rCB = bIsOld ? aOldSymbols : aSymbols;
    FillSymbols(rCB, FALSE);

    // bei Wechsel des SymbolSets fuer das alte Zeichen ein gueltiges
    // Symbol bzw keins zur Anzeige bringen
    if (bIsOld)
    {
        XubString  aTmpOldSymbolName;
        if (aOldSymbols.GetEntryCount() > 0)
            aTmpOldSymbolName = aOldSymbols.GetEntry(0);
        SelectSymbol(aOldSymbols, aTmpOldSymbolName, TRUE);
    }

    UpdateButtons();

    return bRet;
}


void SmSymDefineDialog::SetOrigSymbol(const SmSym *pSymbol,
                                      const XubString &rSymbolSetName)
{
    // clear old symbol
    delete pOrigSymbol;
    pOrigSymbol = 0;

    XubString   aSymName,
                aSymSetName;
    if (pSymbol)
    {
        // set new symbol
        pOrigSymbol = new SmSym( *pSymbol );

        aSymName    = pSymbol->GetName();
        aSymSetName = rSymbolSetName;
        aOldSymbolDisplay.SetFont(pSymbol->GetFace());
        aOldSymbolDisplay.SetChar(pSymbol->GetCharacter());
    }
    else
    {   // loeschen des angezeigten Symbols
        aOldSymbolDisplay.SetText(XubString());
        aOldSymbolDisplay.Invalidate();
    }
    aOldSymbolName   .SetText(aSymName);
    aOldSymbolSetName.SetText(aSymSetName);
}


BOOL SmSymDefineDialog::SelectSymbol(ComboBox &rComboBox,
        const XubString &rSymbolName, BOOL bDeleteText)
{
#if OSL_DEBUG_LEVEL > 1
    DBG_ASSERT(&rComboBox == &aOldSymbols  ||  &rComboBox == &aSymbols,
        "Sm : falsche ComboBox");
#endif

    // 'Normalisieren' des SymbolNamens (ohne Leerzeichen)
    XubString  aNormName (rSymbolName);
    aNormName.EraseAllChars(' ');
    // und evtl Abweichungen in der Eingabe beseitigen
    rComboBox.SetText(aNormName);

    BOOL   bRet = FALSE;
    USHORT nPos = rComboBox.GetEntryPos(aNormName);

    BOOL  bIsOld = &rComboBox == &aOldSymbols;

    if (nPos != COMBOBOX_ENTRY_NOTFOUND)
    {
        rComboBox.SetText(rComboBox.GetEntry(nPos));

        if (!bIsOld)
        {
            const SmSym *pSymbol = GetSymbol(aSymbols);
            if (pSymbol)
            {
                // Font und Style entsprechend waehlen
                const Font &rFont = pSymbol->GetFace();
                SelectFont(rFont.GetName(), FALSE);
                SelectStyle(GetFontStyles().GetStyleName(rFont), FALSE);

                // da das setzen des Fonts ueber den Style Namen des SymbolsFonts nicht
                // so gut klappt (er kann zB leer sein obwohl der Font selbst 'bold' und
                // 'italic' ist!). Setzen wir hier den Font wie er zum Symbol gehoert
                // zu Fuss.
                aCharsetDisplay.SetFont(rFont);
                aSymbolDisplay.SetFont(rFont);

                // das zugehoerige Zeichen auswaehlen
                SelectChar(pSymbol->GetCharacter());
            }
        }

        bRet = TRUE;
    }
    else if (bDeleteText)
        rComboBox.SetText(XubString());

    if (bIsOld)
    {
        // bei Wechsel des alten Symbols nur vorhandene anzeigen sonst keins
        const SmSym *pOldSymbol = NULL;
        XubString     aTMpOldSymbolSetName;
        if (nPos != COMBOBOX_ENTRY_NOTFOUND)
        {
            pOldSymbol        = aSymSetMgrCopy.GetSymbolByName(aNormName);
            aTMpOldSymbolSetName = aOldSymbolSets.GetText();
        }
        SetOrigSymbol(pOldSymbol, aTMpOldSymbolSetName);
    }
    else
        aSymbolName.SetText(rComboBox.GetText());

    UpdateButtons();

    return bRet;
}


void SmSymDefineDialog::SetFont(const XubString &rFontName, const XubString &rStyleName)
{
    // Font (FontInfo) passend zu Namen und Style holen
    FontInfo aFI;
    if (pFontList)
        aFI = pFontList->Get(rFontName, WEIGHT_NORMAL, ITALIC_NONE);
    SetFontStyle(rStyleName, aFI);

    aCharsetDisplay.SetFont(aFI);
    aSymbolDisplay.SetFont(aFI);

    // update subset listbox for new font's unicode subsets
    FontCharMap aFontCharMap;
    aCharsetDisplay.GetFontCharMap( aFontCharMap );
    if (pSubsetMap)
        delete pSubsetMap;
    pSubsetMap = new SubsetMap( &aFontCharMap );
    //
    aFontsSubsetLB.Clear();
    bool bFirst = true;
    const Subset* pSubset;
    while( NULL != (pSubset = pSubsetMap->GetNextSubset( bFirst )) )
    {
        USHORT nPos = aFontsSubsetLB.InsertEntry( pSubset->GetName());
        aFontsSubsetLB.SetEntryData( nPos, (void *) pSubset );
        // subset must live at least as long as the selected font !!!
        if( bFirst )
        aFontsSubsetLB.SelectEntryPos( nPos );
        bFirst = false;
    }
    if( bFirst )
        aFontsSubsetLB.SetNoSelection();
    aFontsSubsetLB.Enable( !bFirst );
}


BOOL SmSymDefineDialog::SelectFont(const XubString &rFontName, BOOL bApplyFont)
{
    BOOL   bRet = FALSE;
    USHORT nPos = aFonts.GetEntryPos(rFontName);

    if (nPos != LISTBOX_ENTRY_NOTFOUND)
    {
        aFonts.SelectEntryPos(nPos);
        if (aStyles.GetEntryCount() > 0)
            SelectStyle(aStyles.GetEntry(0));
        if (bApplyFont)
            SetFont(aFonts.GetSelectEntry(), aStyles.GetText());
        bRet = TRUE;
    }
    else
        aFonts.SetNoSelection();
    FillStyles();

    UpdateButtons();

    return bRet;
}


BOOL SmSymDefineDialog::SelectStyle(const XubString &rStyleName, BOOL bApplyFont)
{
    BOOL   bRet = FALSE;
    USHORT nPos = aStyles.GetEntryPos(rStyleName);

    // falls der Style nicht zur Auswahl steht nehmen wir den erst moeglichen
    // (sofern vorhanden)
    if (nPos == COMBOBOX_ENTRY_NOTFOUND  &&  aStyles.GetEntryCount() > 0)
        nPos = 0;

    if (nPos != COMBOBOX_ENTRY_NOTFOUND)
    {
        aStyles.SetText(aStyles.GetEntry(nPos));
        if (bApplyFont)
            SetFont(aFonts.GetSelectEntry(), aStyles.GetText());
        bRet = TRUE;
    }
    else
        aStyles.SetText(XubString());

    UpdateButtons();

    return bRet;
}


void SmSymDefineDialog::SelectChar(xub_Unicode cChar)
{
    aCharsetDisplay.SelectCharacter( cChar );
    aSymbolDisplay.SetChar( cChar );

    UpdateButtons();
}


/**************************************************************************/

