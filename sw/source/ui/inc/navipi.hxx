/*************************************************************************
 *
 *  $RCSfile: navipi.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: obo $ $Date: 2004-08-12 13:05:45 $
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
#ifndef _NAVIPI_HXX
#define _NAVIPI_HXX

// INCLUDE ---------------------------------------------------------------

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <svtools/lstner.hxx>
#endif
#ifndef _TRANSFER_HXX
#include <svtools/transfer.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX //autogen
#include <sfx2/childwin.hxx>
#endif
#ifndef _SFXCTRLITEM_HXX //autogen
#include <sfx2/ctrlitem.hxx>
#endif
#ifndef _SFXTBXCTRL_HXX
#include <sfx2/tbxctrl.hxx>
#endif

#ifndef _CONTTREE_HXX
#include <conttree.hxx>
#endif
#ifndef _POPBOX_HXX
#include <popbox.hxx>
#endif

class SwWrtShell;
class SwNavigationPI;
class SwNavigationChild;
class SfxBindings;
class NumEditAction;
class SwView;
class SwNavigationConfig;
class SfxObjectShellLock;
class SfxChildWindowContext;

//-----------------------------------------------------------------------
class SwNavigationPI;
class SwNavHelpToolBox : public SwHelpToolBox
{
    virtual void 	MouseButtonDown(const MouseEvent &rEvt);
    virtual void	RequestHelp( const HelpEvent& rHEvt );
    public:
        SwNavHelpToolBox(SwNavigationPI* pParent, const ResId &rResId);
};


// CLASS -----------------------------------------------------------------
class SwNavigationPI : public Window,
                        public SfxControllerItem, public SfxListener
{
    friend class SwNavigationChild;
    friend class SwContentTree;
    friend class SwGlobalTree;

    // --------- members -----------------------------
    SwNavHelpToolBox   	aContentToolBox;
    SwHelpToolBox   	aGlobalToolBox;
    ImageList			aContentImageList;
    ImageList           aContentImageListH;
    SwContentTree		aContentTree;
    SwGlobalTree        aGlobalTree;
    ListBox				aDocListBox;
    Timer 				aPageChgTimer;
    String				sContentFileName;
    String				aContextArr[3];
    String 				aStatusArr[4];
    Point 				aBoxBottomLeft; // Pos., wenn Box unten ist

    SfxObjectShellLock	*pxObjectShell;
    SwView				*pContentView;
    SwWrtShell			*pContentWrtShell;
    SwView 				*pActContView;
    SwView				*pCreateView;
    SfxPopupWindow      *pPopupWindow;
    SfxPopupWindow      *pFloatingWindow;

    SfxChildWindowContext* pContextWin;

    SwNavigationConfig	*pConfig;
    SfxBindings 		&rBindings;

    long 	nDocLBIniHeight;
    long	nWishWidth;
    USHORT 	nActMark;
    USHORT 	nAutoMarkIdx;
    USHORT 	nRegionMode; // 0 - URL, 1 - Bereich mit Link 2 - B. ohne Link
    short  	nZoomIn;
    short  	nZoomOutInit;
    short  	nZoomOut;

    BOOL 	bSmallMode : 1;
    BOOL 	bIsZoomedIn : 1;
    BOOL	bPageCtrlsVisible : 1;
    BOOL	bGlobalMode : 1;

    // --------- methods -----------------------------
    BOOL _IsZoomedIn() const {return bIsZoomedIn;}
    void _ZoomOut();
    void _ZoomIn();

    void FillBox();
    void MakeMark();

    DECL_LINK( DocListBoxSelectHdl, ListBox * );
    DECL_LINK( ToolBoxSelectHdl, ToolBox * );
    DECL_LINK( ToolBoxClickHdl, ToolBox * );
    DECL_LINK( ToolBoxDropdownClickHdl, ToolBox* );
    DECL_LINK( EditAction, NumEditAction * );
    DECL_LINK( EditGetFocus, NumEditAction * );
    DECL_LINK( DoneLink, SfxPoolItem * );
    DECL_LINK( MenuSelectHdl, Menu * );
    DECL_LINK( ChangePageHdl, Timer* );
    DECL_LINK( PageEditModifyHdl, Edit* );
    DECL_LINK( PopupModeEndHdl, void * );
    DECL_LINK( ClosePopupWindow, SfxPopupWindow * );
    void UsePage(SwWrtShell *);

    void MakeVisible();
    void InitImageList();
    virtual SfxChildAlignment
                    CheckAlignment(SfxChildAlignment,SfxChildAlignment);
    void SetPopupWindow( SfxPopupWindow* );

protected:

    virtual 		BOOL Close();
    virtual 		void Resize();
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );


    // zum App-Ende rechtzeitig ObjectShellLock loslassen
    virtual void	Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    NumEditAction&	GetPageEdit();
    BOOL			ToggleTree();
    void			SetGlobalMode(BOOL bSet) {bGlobalMode = bSet;}

public:
    SwNavigationPI(SfxBindings*, SfxChildWindowContext*, Window*);
    ~SwNavigationPI();

    void 			GotoPage();	// Seite anspringen; bindbare Funktion

    void        	Update() { FillBox(); }
    void			UpdateListBox();
    void        	MoveOutline(USHORT nSource, USHORT nTarget, BOOL bWithCilds);
    virtual void    StateChanged( USHORT nSID, SfxItemState eState,
                                            const SfxPoolItem* pState );

    static String 	CreateDropFileName( TransferableDataHelper& rData );
    static void 	CleanEntry( String& rEntry );

    USHORT			GetRegionDropMode() const {return nRegionMode;}
    void			SetRegionDropMode(USHORT nNewMode);

    sal_Int8 		AcceptDrop( const AcceptDropEvent& rEvt );
    sal_Int8 		ExecuteDrop( const ExecuteDropEvent& rEvt );

    BOOL 			IsGlobalDoc() const;
    BOOL			IsGlobalMode() const {return	bGlobalMode;}

    SwView*         GetCreateView() const;
    void            CreateNavigationTool(const Rectangle& rRect, BOOL bSetFocus);
};

class SwNavigationChild : public SfxChildWindowContext
{
public:
    SwNavigationChild( Window* ,
                        USHORT nId,
                        SfxBindings*,
                        SfxChildWinInfo*  );

    SFX_DECL_CHILDWINDOW_CONTEXT( SwNavigationChild )
};

#endif
