/*************************************************************************
 *
 *  $RCSfile: navipi.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: os $ $Date: 2000-09-21 13:40:18 $
 *
 *  Copyright according the GNU Public License.
 *
 ************************************************************************/
#ifndef _NAVIPI_HXX
#define _NAVIPI_HXX

// INCLUDE ---------------------------------------------------------------

#ifndef _SFXCTRLITEM_HXX //autogen
#include <sfx2/ctrlitem.hxx>
#endif

#ifndef _SFXLSTNER_HXX //autogen
#include <svtools/lstner.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _SFX_CHILDWIN_HXX //autogen
#include <sfx2/childwin.hxx>
#endif

#include "conttree.hxx"
#include "popbox.hxx"

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
class SwNavigationPI: public Window,
                            public SfxControllerItem, public SfxListener
{
    friend class SwNavigationChild;
    friend class SwContentTree;
    friend class SwGlobalTree;

private:

    SwNavHelpToolBox   	aContentToolBox;
    SwHelpToolBox   	aGlobalToolBox;
    ImageList			aContentImageList;
    SwContentTree		aContentTree;
    SwGlobalTree        aGlobalTree;
    ListBox				aDocListBox;

    Timer 				aPageChgTimer;

    SfxObjectShellLock* pxObjectShell;
    SwView*				pContentView;
    SwWrtShell*			pContentWrtShell;
    SwView* 			pActContView;
    SwView*				pCreateView;

    SfxChildWindowContext* pContextWin;

    SwNavigationConfig*	pConfig;

    String				sContentFileName;
    String				aContextArr[3];
    String 				aStatusArr[4];

    Point 	aBoxBottomLeft; // Pos., wenn Box unten ist
    long 	nDocLBIniHeight;
    long	nWishWidth;
    USHORT 	nActMark;
    USHORT 	nAutoMarkIdx;
    USHORT 	nRegionMode; // 0 - URL, 1 - Bereich mit Link 2 - B. ohne Link
    short  	nZoomIn;
    short  	nZoomOutInit;
    short  	nZoomOut;

    BOOL 	bSmallMode :	1;
    BOOL 	bIsZoomedIn : 	1;
    BOOL	bPageCtrlsVisible : 1;
    BOOL	bGlobalMode : 1;

    SfxBindings &rBindings;

    BOOL _IsZoomedIn() const {return bIsZoomedIn;}
    void _ZoomOut();
    void _ZoomIn();

    void FillBox();
    void MakeMark();

    DECL_LINK( DocListBoxSelectHdl, ListBox * );
    DECL_LINK( ToolBoxSelectHdl, ToolBox * );
    DECL_LINK( ToolBoxClickHdl, ToolBox * );
    DECL_LINK( EditAction, NumEditAction * );
    DECL_LINK( EditGetFocus, NumEditAction * );
    DECL_LINK( DoneLink, SfxPoolItem * );
    DECL_LINK( MenuSelectHdl, Menu * );
    DECL_LINK( ReadOnlyHdl, void * );
    DECL_LINK( ChangePageHdl, Timer* );
    DECL_LINK( PageEditModifyHdl, Edit* );
    void UsePage(SwWrtShell *);

    void MakeVisible();

    virtual SfxChildAlignment
                    CheckAlignment(SfxChildAlignment,SfxChildAlignment);

protected:

    virtual 		BOOL Close();
    virtual 		void Resize();

    virtual BOOL	Drop( const DropEvent& rEvt);
    virtual BOOL	QueryDrop( DropEvent& rEvt);

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

    static String	CreateDropFileName(USHORT nItem);
    static void 	CleanEntry( String& rEntry );

    USHORT			GetRegionDropMode() const {return nRegionMode;}
    void			SetRegionDropMode(USHORT nNewMode);
    BOOL			IsInDrag() const;

    BOOL 			IsGlobalDoc() const;
    BOOL			IsGlobalMode() const {return	bGlobalMode;}

    SwView*			GetCreateView() const {return pCreateView;}
};

class SwNavigationChild : public SfxChildWindowContext
{
protected:
    virtual BOOL	Drop( const DropEvent& rEvt);
    virtual BOOL	QueryDrop( DropEvent& rEvt);
public:
    SwNavigationChild( Window* ,
                        USHORT nId,
                        SfxBindings*,
                        SfxChildWinInfo*  );

    SFX_DECL_CHILDWINDOW_CONTEXT( SwNavigationChild )

};
#endif
