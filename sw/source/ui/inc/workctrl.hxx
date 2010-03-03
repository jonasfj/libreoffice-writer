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
#ifndef _WORKCTRL_HXX
#define _WORKCTRL_HXX

#include <sfx2/tbxctrl.hxx>
#include <vcl/toolbox.hxx>
#include <svtools/stdctrl.hxx>
#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/imagebtn.hxx>
#endif

class PopupMenu;
class SwView;

// doppelter Eintrag! hrc und hxx
// diese Ids bestimmen, was die Buttons unter dem Scrollbar tun
#define NID_START   20000
#define NID_NEXT 	20000
#define NID_PREV	20001
#define NID_TBL     20002
#define NID_FRM     20003
#define NID_PGE     20004
#define NID_DRW     20005
#define NID_CTRL    20006
#define NID_REG     20007
#define NID_BKM     20008
#define NID_GRF    	20009
#define NID_OLE    	20010
#define NID_OUTL   	20011
#define NID_SEL    	20012
#define NID_FTN   	20013
#define NID_MARK	20014
#define NID_POSTIT  20015
#define NID_SRCH_REP 20016
#define NID_INDEX_ENTRY  20017
#define NID_TABLE_FORMULA	20018
#define NID_TABLE_FORMULA_ERROR		20019
#define NID_COUNT  20



//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class SwTbxInsertCtrl : public SfxToolBoxControl
{
    USHORT 					nLastSlotId;

    using SfxToolBoxControl::Select;
    virtual void			Select( BOOL bMod1 = FALSE );

public:
    SFX_DECL_TOOLBOX_CONTROL();

    SwTbxInsertCtrl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~SwTbxInsertCtrl();

    virtual SfxPopupWindowType	GetPopupWindowType() const;
    virtual SfxPopupWindow* 	CreatePopupWindow();
    virtual void				StateChanged( USHORT nSID,
                                              SfxItemState eState,
                                              const SfxPoolItem* pState );

    virtual void SAL_CALL update() throw (::com::sun::star::uno::RuntimeException);
};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class SwTbxAutoTextCtrl : public SfxToolBoxControl
{
    PopupMenu*				pPopup;
    SwView* 				pView;


    void					DelPopup();
public:
    SFX_DECL_TOOLBOX_CONTROL();

    SwTbxAutoTextCtrl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~SwTbxAutoTextCtrl();

    virtual SfxPopupWindowType	GetPopupWindowType() const;
    virtual SfxPopupWindow* 	CreatePopupWindow();
    virtual void				StateChanged( USHORT nSID,
                                              SfxItemState eState,
                                              const SfxPoolItem* pState );

    DECL_LINK(PopupHdl, PopupMenu*);
};


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class SwScrollNaviToolBox : public ToolBox
{
    virtual void	MouseButtonUp( const MouseEvent& rMEvt );
    virtual void	RequestHelp( const HelpEvent& rHEvt );

    public:
        SwScrollNaviToolBox(Window* pParent, WinBits nWinStyle ) :
            ToolBox(pParent, nWinStyle ) {}
};

class SwScrollNaviPopup : public SfxPopupWindow
{
    SwScrollNaviToolBox    aToolBox;
    FixedLine       aSeparator;
    FixedInfo       aInfoField;
    ImageList		aIList;
    ImageList       aIListH;

    String			sQuickHelp[2 * NID_COUNT];

    USHORT			nFwdId;
    USHORT			nBackId;

    void            ApplyImageList();
    
    using Window::GetQuickHelpText;

protected:
        DECL_LINK(SelectHdl, ToolBox*);
        virtual void        DataChanged( const DataChangedEvent& rDCEvt );

public:
        SwScrollNaviPopup( USHORT nId, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
        ~SwScrollNaviPopup();

    static String			GetQuickHelpText(BOOL bNext);
    
    virtual SfxPopupWindow* Clone() const;
    void                GrabFocus(){aToolBox.GrabFocus();}
};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

class SwNaviImageButton : public ImageButton
{
        SwScrollNaviPopup* 	pPopup;
        Image				aImage;
        Image               aImageH;
        String              sQuickText;
        SfxPopupWindow*     pPopupWindow;
        SfxPopupWindow*     pFloatingWindow;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > m_xFrame;

    protected:
        DECL_LINK( PopupModeEndHdl, void * );
        DECL_LINK( ClosePopupWindow, SfxPopupWindow * );
        
        virtual void    Click();
        virtual void    DataChanged( const DataChangedEvent& rDCEvt );
        void            SetPopupWindow( SfxPopupWindow* pWindow );

    public:
        SwNaviImageButton(Window* pParent, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame );
};

//----------------------------------------------------------------------------
//	Die ImageButtons muessen sich bei Bedarf den HelpText selbst setzen
//----------------------------------------------------------------------------

class SwHlpImageButton : public ImageButton
{
    BOOL		bUp;
    public:
        SwHlpImageButton(Window* pParent, const ResId& rResId, BOOL bUpBtn) :
            ImageButton(pParent, rResId), bUp(bUpBtn){}

    virtual void	RequestHelp( const HelpEvent& rHEvt );

};
/* -----------------26.11.2002 09:25-----------------
 * 
 * --------------------------------------------------*/
class SwPreviewZoomControl : public SfxToolBoxControl
{
public:
    SFX_DECL_TOOLBOX_CONTROL();

    SwPreviewZoomControl( USHORT nSlotId, USHORT nId, ToolBox& rTbx );
    ~SwPreviewZoomControl();

    virtual void            StateChanged( USHORT nSID,
                                              SfxItemState eState,
                                              const SfxPoolItem* pState );

    virtual Window*         CreateItemWindow( Window *pParent );
};
#endif



