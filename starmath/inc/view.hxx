/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: view.hxx,v $
 * $Revision: 1.24 $
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
#ifndef VIEW_HXX
#define VIEW_HXX

#include <sfx2/dockwin.hxx>
#include <sfx2/viewsh.hxx>
#include <svtools/scrwin.hxx>
#include <sfx2/ctrlitem.hxx>
#include <sfx2/shell.hxx>
#include <sfx2/viewfac.hxx>
#include <sfx2/viewfrm.hxx>
#include <svtools/colorcfg.hxx>
#include "edit.hxx"
#include "node.hxx"
#include "accessibility.hxx"

class Menu;
class DataChangedEvent;
class SmClipboardChangeListener;
class SmDocShell;
class SmViewShell;
class SmPrintUIOptions;

/**************************************************************************/

class SmGraphicWindow : public ScrollableWindow
{
    Point			aFormulaDrawPos;
    Rectangle		aCursorRect;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible >  xAccessible;
    SmGraphicAccessible *                                       pAccessible;

    SmViewShell    *pViewShell;
    USHORT			nZoom;
    short			nModifyCount;
    BOOL			bIsCursorVisible;

protected:
    void		SetFormulaDrawPos(const Point &rPos) { aFormulaDrawPos = rPos; }
    void		SetIsCursorVisible(BOOL bVis) { bIsCursorVisible = bVis; }
    using   Window::SetCursor;
    void        SetCursor(const SmNode *pNode);
    void 		SetCursor(const Rectangle &rRect);

    virtual void DataChanged( const DataChangedEvent& );
    virtual void Paint(const Rectangle&);
    virtual void KeyInput(const KeyEvent& rKEvt);
    virtual void Command(const CommandEvent& rCEvt);
    virtual void StateChanged( StateChangedType eChanged );
    DECL_LINK(MenuSelectHdl, Menu *);

public:
    SmGraphicWindow(SmViewShell* pShell);
    ~SmGraphicWindow();

    // Window
    virtual void    MouseButtonDown(const MouseEvent &rMEvt);
    virtual void    GetFocus();
    virtual void    LoseFocus();

    SmViewShell *   GetView()   { return pViewShell; }

    using   Window::SetZoom;
    void   SetZoom(USHORT Factor);
    using   Window::GetZoom;
    USHORT GetZoom() const { return nZoom; }

    const Point &   GetFormulaDrawPos() const { return aFormulaDrawPos; }

    void ZoomToFitInWindow();
    using   ScrollableWindow::SetTotalSize;
    void SetTotalSize();

    BOOL IsCursorVisible() const { return bIsCursorVisible; }
    void ShowCursor(BOOL bShow);
    const SmNode * SetCursorPos(USHORT nRow, USHORT nCol);

    void ApplyColorConfigValues( const svtools::ColorConfig &rColorCfg );

    // for Accessibility
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > CreateAccessible();

    using   Window::GetAccessible;
    SmGraphicAccessible *   GetAccessible_Impl()  { return pAccessible; }
};

/**************************************************************************/

class SmGraphicController: public SfxControllerItem
{
protected:
    SmGraphicWindow &rGraphic;
public:
    SmGraphicController(SmGraphicWindow &, USHORT, SfxBindings & );
    virtual void StateChanged(USHORT			 nSID,
                              SfxItemState		 eState,
                              const SfxPoolItem* pState);
};

/**************************************************************************/

class SmEditController: public SfxControllerItem
{
protected:
    SmEditWindow &rEdit;

public:
    SmEditController(SmEditWindow &, USHORT, SfxBindings  & );
#if OSL_DEBUG_LEVEL > 1
    virtual ~SmEditController();
#endif

    virtual void StateChanged(USHORT			 nSID,
                              SfxItemState		 eState,
                              const SfxPoolItem* pState);
};

/**************************************************************************/

class SmCmdBoxWindow : public SfxDockingWindow
{
    SmEditWindow		aEdit;
    SmEditController	aController;
    BOOL                bExiting;
    
    Timer               aInitialFocusTimer;
    
    DECL_LINK(InitialFocusTimerHdl, Timer *);

protected :

    // Window
    virtual void    GetFocus();
    virtual void Resize();
    virtual void Paint(const Rectangle& rRect);
    virtual void StateChanged( StateChangedType nStateChange );

    virtual Size CalcDockingSize(SfxChildAlignment eAlign);
    virtual SfxChildAlignment CheckAlignment(SfxChildAlignment eActual,
                                             SfxChildAlignment eWish);

    virtual void	ToggleFloatingMode();

public:
    SmCmdBoxWindow(SfxBindings	  *pBindings,
                   SfxChildWindow *pChildWindow,
                   Window		  *pParent);

    virtual ~SmCmdBoxWindow ();

    void AdjustPosition();

    SmEditWindow *GetEditWindow() { return (&aEdit); }
    SmViewShell  *GetView();
};

/**************************************************************************/

class SmCmdBoxWrapper : public SfxChildWindow
{
    SFX_DECL_CHILDWINDOW(SmCmdBoxWrapper);

protected:
    SmCmdBoxWrapper(Window			*pParentWindow,
                    USHORT			 nId,
                    SfxBindings 	*pBindings,
                    SfxChildWinInfo *pInfo);

#if OSL_DEBUG_LEVEL > 1
    virtual ~SmCmdBoxWrapper();
#endif

public:

    SmEditWindow *GetEditWindow()
    {
        return (((SmCmdBoxWindow *)pWindow)->GetEditWindow());
    }

};

/**************************************************************************/

namespace sfx2 { class FileDialogHelper; }
struct SmViewShell_Impl;

class SmViewShell: public SfxViewShell
{
    // for handling the PasteClipboardState
    friend class SmClipboardChangeListener;

    SmGraphicWindow 	aGraphic;
    SmGraphicController aGraphicController;
    String				StatusText;

    ::com::sun::star::uno:: Reference <
            ::com::sun::star::lang:: XEventListener > xClipEvtLstnr;
    SmClipboardChangeListener*	pClipEvtLstnr;
    SmViewShell_Impl*   pImpl;
    BOOL				bPasteState;

    void AddRemoveClipboardListener( BOOL bAdd );

    DECL_LINK( DialogClosedHdl, sfx2::FileDialogHelper* );

protected:

    Size GetTextLineSize(OutputDevice& rDevice,
                         const String& rLine);
    Size GetTextSize(OutputDevice& rDevice,
                     const String& rText,
                     long		   MaxWidth);
    void DrawTextLine(OutputDevice& rDevice,
                      const Point&	rPosition,
                      const String& rLine);
    void DrawText(OutputDevice& rDevice,
                  const Point&	rPosition,
                  const String& rText,
                  USHORT		MaxWidth);

    virtual USHORT Print(SfxProgress &rProgress, BOOL bIsAPI, PrintDialog *pPrintDialog = 0);
    virtual SfxPrinter *GetPrinter(BOOL bCreate = FALSE);
    virtual USHORT SetPrinter(SfxPrinter *pNewPrinter,
                              USHORT     nDiffFlags = SFX_PRINTER_ALL, bool bIsAPI=false);

    BOOL        Insert( SfxMedium& rMedium );
    BOOL        InsertFrom(SfxMedium &rMedium);

    virtual SfxTabPage *CreatePrintOptionsPage(Window			*pParent,
                                               const SfxItemSet &rOptions);
    virtual void Deactivate(BOOL IsMDIActivate);
    virtual void Activate(BOOL IsMDIActivate);
    virtual Size GetOptimalSizePixel() const;
    virtual void AdjustPosSizePixel(const Point &rPos, const Size &rSize);
    virtual void InnerResizePixel(const Point &rOfs, const Size  &rSize);
    virtual void OuterResizePixel(const Point &rOfs, const Size  &rSize);
    virtual void QueryObjAreaPixel( Rectangle& rRect ) const;
    virtual void SetZoomFactor( const Fraction &rX, const Fraction &rY );

public:
    TYPEINFO();

    SmViewShell(SfxViewFrame *pFrame, SfxViewShell *pOldSh);
    ~SmViewShell();

    SmDocShell * GetDoc()
    {
        return (SmDocShell *) GetViewFrame()->GetObjectShell();
    }

    SmEditWindow * GetEditWindow();
          SmGraphicWindow & GetGraphicWindow() 		 { return aGraphic; }
    const SmGraphicWindow & GetGraphicWindow() const { return aGraphic; }

    void		SetStatusText(const String& Text);

    void		ShowError( const SmErrorDesc *pErrorDesc );
    void		NextError();
    void		PrevError();

    SFX_DECL_INTERFACE(SFX_INTERFACE_SMA_START+2)
    SFX_DECL_VIEWFACTORY(SmViewShell);

    virtual void Execute( SfxRequest& rReq );
    virtual void GetState(SfxItemSet &);

    void Impl_Print( OutputDevice &rOutDev, const SmPrintUIOptions &rPrintUIOptions,
            Rectangle aOutRect, Point aZeroPoint );
};

#endif

