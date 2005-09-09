/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pview.hxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:58:44 $
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
#ifndef _SWPVIEW_HXX
#define _SWPVIEW_HXX

#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _VIEWFAC_HXX //autogen
#include <sfx2/viewfac.hxx>
#endif
#ifndef _SFXVIEWSH_HXX //autogen
#include <sfx2/viewsh.hxx>
#endif
#ifndef _SVX_ZOOMITEM_HXX
#include <svx/zoomitem.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _SHELLID_HXX
#include "shellid.hxx"
#endif

class SwViewOption;
class SwDocShell;
class SwScrollbar;
class ViewShell;
class SwPagePreView;
class ImageButton;
class Button;
class SwRect;
class DataChangedEvent;
class CommandEvent;
class SvtAccessibilityOptions;
// OD 12.12.2002 #103492#
class SwPagePreviewLayout;

// OD 24.03.2003 #108282# - delete member <mnVirtPage> and its accessor.
class SwPagePreViewWin : public Window
{
    ViewShell*          mpViewShell;
    USHORT              mnSttPage;
    BYTE                mnRow, mnCol;
    Size                maPxWinSize;
    Fraction            maScale;
    SwPagePreView&      mrView;
    // OD 02.12.2002 #103492#
    bool                mbCalcScaleForPreviewLayout;
    Rectangle           maPaintedPreviewDocRect;
    // OD 12.12.2002 #103492#
    SwPagePreviewLayout* mpPgPrevwLayout;

    void SetPagePreview( BYTE nRow, BYTE nCol );

public:
    SwPagePreViewWin( Window* pParent, SwPagePreView& rView );
    ~SwPagePreViewWin();

    //Ruft ViewShell::Paint
    virtual void Paint( const Rectangle& rRect );
    virtual void KeyInput( const KeyEvent & );
    virtual void Command( const CommandEvent& rCEvt );
    virtual void MouseButtonDown(const MouseEvent& rMEvt);
    virtual void DataChanged( const DataChangedEvent& );

    void SetViewShell( ViewShell* pShell );

    ViewShell* GetViewShell() const { return mpViewShell; }

    BYTE    GetRow() const      { return mnRow; }
    void    SetRow( BYTE n )    { if( n ) mnRow = n; }

    BYTE    GetCol() const      { return mnCol; }
    void    SetCol( BYTE n )    { if( n ) mnCol = n; }

    USHORT  GetSttPage() const      { return mnSttPage; }
    void    SetSttPage( USHORT n )  { mnSttPage = n; }

    /** get selected page number of document preview

        OD 13.12.2002 #103492#

        @author OD

        @return selected page number
    */
    sal_uInt16 SelectedPage() const;

    /** set selected page number in document preview

        OD 13.12.2002 #103492#

        @author OD

        @param _nSelectedPageNum
        input parameter - physical page number of page that will be the selected one.
    */
    void SetSelectedPage( sal_uInt16 _nSelectedPageNum );

    //JP 19.08.98: bei Einspaltigkeit gibt es keine 0. Seite!
    USHORT  GetDefSttPage() const   { return 1 == mnCol ? 1 : 0; }

    void CalcWish( BYTE nNewRow, BYTE nNewCol );

    const Size& GetWinSize() const  { return maPxWinSize; }
    void SetWinSize( const Size& rNewSize );

    // OD 18.12.2002 #103492# - add <MV_SELPAGE>, <MV_SCROLL>
    enum MoveMode{ MV_CALC, MV_PAGE_UP, MV_PAGE_DOWN, MV_DOC_STT, MV_DOC_END,
                   MV_SELPAGE, MV_SCROLL, MV_NEWWINSIZE };
    int MovePage( int eMoveMode );

    // erzeuge den String fuer die StatusLeiste
    void GetStatusStr( String& rStr, USHORT nPageCount ) const;

    void RepaintCoreRect( const SwRect& rRect );

    /** method to adjust preview to a new zoom factor

        OD 02.12.2002 #103492#
        paint of preview is prepared for a new zoom factor
        OD 24.09.2003 #i19975# - zoom type has also been considered.
        Thus, add new parameter <_eZoomType>

        @author OD
    */
    void AdjustPreviewToNewZoom( const sal_uInt16 _nZoomFactor,
                                 const SvxZoomType _eZoomType );

    const Rectangle& GetPaintedPreviewDocRect() const
    {
        return maPaintedPreviewDocRect;
    }

    void Scroll(long nXMove, long nYMove);

    /** method to enable/disable book preview

        OD 2004-03-05 #i18143#

        @author OD

        @param _bBookPreview
        input parameter - boolean indicating, if book preview mode has to
        switch on <TRUE> or of <FALSE>

        @return boolean indicating, if book preview mode has changed.
    */
    bool SetBookPreviewMode( const bool _bBookPreview );

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible>
                    CreateAccessible();
};


/*--------------------------------------------------------------------
    Beschreibung:   Sicht auf ein Dokument
 --------------------------------------------------------------------*/

class SW_DLLPUBLIC SwPagePreView: public SfxViewShell
{
    // ViewWindow und Henkel zur Core
    // aktuelle Dispatcher-Shell
    SwPagePreViewWin        aViewWin;
    //viewdata of the previous SwView and the new crsrposition
    String 					sSwViewData,
    //and the new cursor position if the user double click in the PagePreView
                            sNewCrsrPos;
    // to support keyboard the number of the page to go to can be set too
    USHORT                  nNewPage;
    // Sichtbarer Bereich
    String                  sPageStr;
    Size                    aDocSz;
    Rectangle               aVisArea;

    // MDI Bedienelemente
    SwScrollbar             *pHScrollbar;
    SwScrollbar             *pVScrollbar;
    ImageButton             *pPageUpBtn,
                            *pPageDownBtn;
    // Dummy-Window zum F�llen der rechten unteren Ecke, wenn beide Scrollbars
    // aktiv sind
    Window					*pScrollFill;

    USHORT                  mnPageCount;
    BOOL                    bNormalPrint;

    // OD 09.01.2003 #106334#
    // new members to reset design mode at draw view for form shell on switching
    // back from writer page preview to normal view.
    sal_Bool                mbResetFormDesignMode:1;
    sal_Bool                mbFormDesignModeToReset:1;

    SW_DLLPRIVATE void            Init(const SwViewOption* = 0);
    SW_DLLPRIVATE Point           AlignToPixel(const Point& rPt) const;

    SW_DLLPRIVATE int             _CreateScrollbar( int bHori );
    SW_DLLPRIVATE DECL_LINK( ScrollHdl, SwScrollbar * );
    SW_DLLPRIVATE DECL_LINK( EndScrollHdl, SwScrollbar * );
    SW_DLLPRIVATE DECL_LINK( BtnPage, Button * );
    SW_DLLPRIVATE int             ChgPage( int eMvMode, int bUpdateScrollbar = TRUE );


    SW_DLLPRIVATE virtual USHORT          Print( SfxProgress &rProgress,
                                   PrintDialog *pPrintDialog = 0 );
    SW_DLLPRIVATE virtual SfxPrinter*     GetPrinter( BOOL bCreate = FALSE );
    SW_DLLPRIVATE virtual USHORT          SetPrinter( SfxPrinter *pNewPrinter, USHORT nDiffFlags = SFX_PRINTER_ALL );
    SW_DLLPRIVATE virtual SfxTabPage*     CreatePrintOptionsPage( Window *pParent,
                                                const SfxItemSet &rOptions );
    SW_DLLPRIVATE virtual PrintDialog*    CreatePrintDialog( Window *pParent );

    SW_DLLPRIVATE void CalcAndSetBorderPixel( SvBorder &rToFill, FASTBOOL bInner );

    /** help method to execute SfxRequest FN_PAGE_UP and FN_PAGE_DOWN

        OD 04.03.2003 #107369#

        @param _bPgUp
        input parameter - boolean that indicates, if FN_PAGE_UP or FN_PAGE_DOWN
        has to be executed.

        @param _pReq
        optional input parameter - pointer to the <SfxRequest> instance, if existing.

        @author OD
    */
    SW_DLLPRIVATE void _ExecPgUpAndPgDown( const bool  _bPgUp,
                             SfxRequest* _pReq = 0 );

protected:
    virtual void    InnerResizePixel( const Point &rOfs, const Size &rSize );
    virtual void    OuterResizePixel( const Point &rOfs, const Size &rSize );
    virtual Size	GetOptimalSizePixel() const;

    void         SetZoom(SvxZoomType eSet, USHORT nFactor);

public:
    SFX_DECL_VIEWFACTORY(SwPagePreView);
    SFX_DECL_INTERFACE(SW_PAGEPREVIEW);
    TYPEINFO();

    inline Window*          GetFrameWindow() const { return &(GetViewFrame())->GetWindow(); }
    inline ViewShell*       GetViewShell() const { return aViewWin.GetViewShell(); }
    inline const Rectangle& GetVisArea() const { return aVisArea; }
    inline void             GrabFocusViewWin() { aViewWin.GrabFocus(); }
    inline void             RepaintCoreRect( const SwRect& rRect )
                                { aViewWin.RepaintCoreRect( rRect ); }

    void            DocSzChgd(const Size& rNewSize);
    const Size&     GetDocSz() const { return aDocSz; }

    virtual void    SetVisArea( const Rectangle&, BOOL bUpdateScrollbar = TRUE);

    inline void     AdjustEditWin();

    void            ScrollViewSzChg();
    void            ScrollDocSzChg();
    void            ShowHScrollbar(sal_Bool bShow);

    void            ShowVScrollbar(sal_Bool bShow);

    USHORT          GetPageCount() const        { return mnPageCount; }

    BOOL 			HandleWheelCommands( const CommandEvent& );

    const String&	GetPrevSwViewData() const		{ return sSwViewData; }
    void 			SetNewCrsrPos( const String& rStr ) { sNewCrsrPos = rStr; }
    const String&	GetNewCrsrPos() const			{ return sNewCrsrPos; }

    USHORT          GetNewPage() const {return nNewPage;}
    void            SetNewPage(USHORT nSet)  {nNewPage = nSet;}

        // Handler
    void            Execute(SfxRequest&);
    void            GetState(SfxItemSet&);
    void            StateUndo(SfxItemSet&);

    SwDocShell*     GetDocShell();

    //apply Accessiblity options
    void ApplyAccessiblityOptions(SvtAccessibilityOptions& rAccessibilityOptions);

    // OD 09.01.2003 #106334# - inline method to request values of new members
    // <mbResetFormDesignMode> and <mbFormDesignModeToReset>
    inline sal_Bool ResetFormDesignMode() const
    {
        return mbResetFormDesignMode;
    }

    inline sal_Bool FormDesignModeToReset() const
    {
        return mbFormDesignModeToReset;
    }

    /** adjust position of vertical scrollbar

        OD 19.02.2003 #107369
        Currently used, if the complete preview layout rows fit into to the given
        window, if a new page is selected and this page is visible.

        @author OD

        @param _nNewThumbPos
        input parameter - new position, which will be assigned to the vertical
        scrollbar.
    */
    void SetVScrollbarThumbPos( const sal_uInt16 _nNewThumbPos );

    SwPagePreView( SfxViewFrame* pFrame, SfxViewShell* );
    ~SwPagePreView();
};

// ----------------- inline Methoden ----------------------


inline void SwPagePreView::AdjustEditWin()
{
    OuterResizePixel( Point(), GetFrameWindow()->GetOutputSizePixel() );
}

#endif
