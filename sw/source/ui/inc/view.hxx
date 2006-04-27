/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: view.hxx,v $
 *
 *  $Revision: 1.41 $
 *
 *  last change: $Author: kz $ $Date: 2006-04-27 09:50:04 $
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
#ifndef _SWVIEW_HXX
#define _SWVIEW_HXX

#ifndef _TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _VIEWFAC_HXX //autogen
#include <sfx2/viewfac.hxx>
#endif
#ifndef _SFXVIEWSH_HXX //autogen
#include <sfx2/viewsh.hxx>
#endif
#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>   // SfxObjectShellRef <-> SV_DECL_REF(SfxObjectShell)
#endif
#ifndef _SVX_SVXENUM_HXX //autogen
#include <svx/svxenum.hxx>
#endif
#ifndef _SVX_ZOOMITEM_HXX //autogen
#include <svx/zoomitem.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _SHELLID_HXX
#include <shellid.hxx>
#endif

class SwBaseShell;
class Button;
class ImageButton;
class SwTxtFmtColl;
class SwPageDesc;
class SwFrmFmt;
class SwCharFmt;
class SwNumRule;
class SwViewOption;
class SwGlossaryHdl;
class SwDrawBase;
class SvxRuler;
class SvxLRSpaceItem;
class SwDocShell;
class SwScrollbar;
class PrintDialog;
class SvxVCBrowser;
class SvBorder;
class Ruler;
class OutlinerView;
class SvxSearchItem;
class SearchAttrItemList;
class SvxSearchDialog;
class SdrView;
class Dialog;
class SdrObject;
class SdrPageView;
class SwNaviImageButton;
class SwHlpImageButton;
class Outliner;
class SwView;
class SwEditWin;
class SwWrtShell;
class SwView_Impl;
class XSelectionObject;
struct SwSearchOptions;
class FmFormShell;
class CommandEvent;
class InsCaptionOpt;
class SvGlobalName;
class SvtAccessibilityOptions;
class SwPrtOptions;
class SwTransferable;
class SwMailMergeConfigItem;
class SwTxtNode; // #i23726#
struct SwPrintData;
class SwFormatClipboard;
struct SwConversionArgs;
class Graphic;
class GraphicFilter;

namespace com{ namespace sun { namespace star {
    namespace view{ class XSelectionSupplier; }
}}}

const long nLeftOfst = -370;
const long nScrollX	 =	 30;
const long nScrollY	 =	 30;

#define MINZOOM 20
#define MAXZOOM 600

#define MAX_MARKS 5

#define CHILDWIN_LABEL      1
#define CHILDWIN_MAILMERGE  2

enum ShellModes
{
    SEL_TEXT,
    SEL_FRAME,
    SEL_GRAPHIC,
    SEL_OBJECT,
    SEL_DRAW,
    SEL_DRAW_CTRL,
    SEL_DRAW_FORM,
    SEL_DRAWTEXT,
    SEL_BEZIER,
    SEL_LIST_TEXT,
    SEL_TABLE_TEXT,
    SEL_TABLE_LIST_TEXT,
    SEL_MEDIA,
    SEL_EXTRUDED_CUSTOMSHAPE,
    SEL_FONTWORK
};

/*--------------------------------------------------------------------
    Beschreibung:	Anwendung einer Vorlage
 --------------------------------------------------------------------*/

struct SwApplyTemplate
{
    union
    {
        SwTxtFmtColl* pTxtColl;
        SwPageDesc*   pPageDesc;
        SwFrmFmt*	  pFrmFmt;
        SwCharFmt*	  pCharFmt;
        SwNumRule*	  pNumRule;
    } aColl;

    int eType;
    USHORT nColor;
    SwFormatClipboard* pFormatClipboard;
    BOOL bUndo;

    SwApplyTemplate() :
        eType(0),
        nColor(0),
        pFormatClipboard(0),
        bUndo(FALSE)
    {
        aColl.pTxtColl = 0;
    }
};


/*--------------------------------------------------------------------
    Beschreibung:	Sicht auf ein Dokument
 --------------------------------------------------------------------*/

class SW_DLLPUBLIC SwView: public SfxViewShell
{
    //Messehack (MA,MBA)
    friend void lcl_SelectShellForDrop( SwView &rView );

    friend class TestTemplateItem;
    friend class SwHHCWrapper;
    friend class SwHyphWrapper;
    friend class SwView_Impl;
    friend class SwClipboardChangeListener;

    //suchen & ersetzen
    static SvxSearchDialog *pSrchDlg;
    static SvxSearchItem   *pSrchItem;

    static USHORT			nInsertCtrlState;
    static USHORT			nWebInsertCtrlState;
    static USHORT			nInsertObjectCtrlState;
    static USHORT			nInsertFieldCtrlState;
    static USHORT			nMoveType; // fuer Buttons unter dem Scrollbar (viewmdi)
    static BYTE				nActMark; // aktuelle Sprungmarke fuer unbenannte Merker

    static BOOL             bExtra;
    static BOOL             bFound;
    static BOOL				bJustOpened;

    static SearchAttrItemList* pSrchList;
    static SearchAttrItemList* pReplList;


    Timer				aTimer;			//Fuer verzoegerte ChgLnks waehrend
                                        //einer Aktion
    String				aPageStr;		//Statusanzeige, aktuelle Seite
    String 				sSwViewData,
    //and the new cursor position if the user double click in the PagePreView
                        sNewCrsrPos;
    // to support keyboard the number of the page to go to can be set too
    USHORT              nNewPage;

    Point				aTabColFromDocPos;	//Verschieben von Tabellenspalten aus
                                            //aus dem Dokument heraus.
    SwTxtNode           * pNumRuleNodeFromDoc; // Moving indent of numrule #i23726#

    Size				aDocSz;			// aktuelle Dokumentgroesse
    Rectangle			aVisArea;		// sichtbarer Bereich

    SwEditWin			*pEditWin;
    SwWrtShell			*pWrtShell;

    SfxShell			*pShell;		//aktuelle SubShell auf dem Dispatcher
    FmFormShell			*pFormShell;	// DB-FormShell

    SwView_Impl			*pViewImpl;		// Impl-Daten fuer UNO + Basic


    SwScrollbar	   		*pHScrollbar,	// MDI Bedienelemente
                        *pVScrollbar;

    Window		   		*pScrollFill;	// Dummy-Window zum F�llen der rechten
                                        // unteren Ecke, wenn beide Scrollbars
                                        // aktiv sind

    SvxRuler            *pHRuler,
                        *pVRuler;
    ImageButton	   		*pTogglePageBtn;

    SwHlpImageButton 	*pPageUpBtn,
                        *pPageDownBtn;

    SwNaviImageButton 	*pNaviBtn;
    SwGlossaryHdl  		*pGlosHdl;			// Henkel Textbausteine
    SwDrawBase			*pDrawActual;

    const SwFrmFmt		*pLastTableFormat;

    SwFormatClipboard   *pFormatClipboard; //holds data for format paintbrush

    int					nSelectionType;

    static const int MASTERENUMCOMMANDS = 6;

    String			aCurrShapeEnumCommand[ MASTERENUMCOMMANDS ];

    // Aktueller Drawmode
    USHORT			nDrawSfxId;
    String          sDrawCustom; //some drawing types are marked with strings!
    USHORT			nFormSfxId;
    USHORT			nLastPasteDestination;

    // save the border distance status from SwView::StateTabWin to re-use it in SwView::ExecTabWin()
    USHORT          nLeftBorderDistance;
    USHORT          nRightBorderDistance;

    BOOL			bCenterCrsr : 1,
                    bTopCrsr : 1,
                    bAllwaysShowSel : 1,
                    bTabColFromDoc : 1,
                    bNumIndentFromDoc : 1, // #i23726#
                    bTabRowFromDoc : 1,
                    bSetTabColFromDoc : 1 ,
                    bSetTabRowFromDoc : 1,
                    bAttrChgNotified : 1,
                    bAttrChgNotifiedWithRegistrations : 1,
                    bVerbsActive : 1,
                    bDrawRotate : 1,
                    bDrawSelMode : 1,
                    bShowAtResize : 1,
                    bInOuterResizePixel : 1,
                    bInInnerResizePixel : 1,
                    bIsApi : 1,
                    bPasteState : 1,
                    bPasteSpecialState : 1,
                    bInMailMerge : 1,
                    bInDtor : 1, //detect destructor to prevent creating of sub shells while closing
                    bOldShellWasPagePreView : 1,
                    bMakeSelectionVisible : 1 // #b6330459# transport the bookmark selection
                    ;

    // Methoden fuers Suchen
    // Suchkontext setzen
    SW_DLLPRIVATE BOOL   			SearchAndWrap(BOOL bApi = FALSE);
    SW_DLLPRIVATE BOOL 			SearchAll(USHORT* pFound = 0);
    SW_DLLPRIVATE ULONG			FUNC_Search( const SwSearchOptions& rOptions );
    SW_DLLPRIVATE void 			Replace();

    inline BOOL		IsDocumentBorder();

    SW_DLLPRIVATE BOOL			IsTextTool() const;

    // Bedienelemente verwalten anlegen
    SW_DLLPRIVATE void			CreateBtns();
    SW_DLLPRIVATE DECL_LINK( BtnPage, Button * );

    SW_DLLPRIVATE DECL_LINK( TimeoutHdl, Timer * );
    SW_DLLPRIVATE DECL_LINK( UpdatePercentHdl, GraphicFilter* );


    inline long		GetXScroll() const;
    inline long		GetYScroll() const;
    SW_DLLPRIVATE  Point	AlignToPixel(const Point& rPt) const;
    SW_DLLPRIVATE 	void		CalcPt(	Point* pPt,const Rectangle& rRect,
                            USHORT nRangeX = USHRT_MAX,
                            USHORT nRangeY = USHRT_MAX);

    SW_DLLPRIVATE BOOL			GetPageScrollUpOffset(SwTwips& rOff) const;
    SW_DLLPRIVATE BOOL			GetPageScrollDownOffset(SwTwips& rOff) const;

    // Scrollbar Movements
    SW_DLLPRIVATE long			PageUp();
    SW_DLLPRIVATE long			PageDown();
    SW_DLLPRIVATE long			PageUpCrsr(BOOL bSelect);
    SW_DLLPRIVATE long			PageDownCrsr(BOOL bSelect);
    SW_DLLPRIVATE long			PhyPageUp();
    SW_DLLPRIVATE long			PhyPageDown();

    SW_DLLPRIVATE int				_CreateScrollbar( int bHori );
    SW_DLLPRIVATE int				_KillScrollbar( int bHori );
    SW_DLLPRIVATE DECL_LINK( ScrollHdl, SwScrollbar * );
    SW_DLLPRIVATE DECL_LINK( EndScrollHdl, SwScrollbar * );
    SW_DLLPRIVATE BOOL			UpdateScrollbars();
    SW_DLLPRIVATE void			CalcVisArea( const Size &rPixelSz );

    SW_DLLPRIVATE void            CreatePageButtons(BOOL bShow);

    // Linguistik-Funktionen
    SW_DLLPRIVATE void			HyphenateDocument();
    SW_DLLPRIVATE BOOL			IsDrawTextHyphenate();
    SW_DLLPRIVATE void			HyphenateDrawText();
    SW_DLLPRIVATE void			StartThesaurus();

    // text conversion
    SW_DLLPRIVATE void          StartTextConversion( LanguageType nSourceLang, LanguageType nTargetLang, const Font *pTargetFont, sal_Int32 nOptions, sal_Bool bIsInteractive );

    // used for spell checking and text conversion
    SW_DLLPRIVATE void          SpellStart( SvxSpellArea eSpell, BOOL bStartDone,
                                        BOOL bEndDone, SwConversionArgs *pConvArgs = 0 );
    SW_DLLPRIVATE void          SpellEnd( SwConversionArgs *pConvArgs = 0 );

    SW_DLLPRIVATE void          HyphStart( SvxSpellArea eSpell );
    SW_DLLPRIVATE BOOL			CheckSpecialCntnt();
    SW_DLLPRIVATE void			SpellKontext(BOOL bOn = TRUE)
                            { bCenterCrsr = bOn; bAllwaysShowSel = bOn; }

    // Methoden fuers Printing
    SW_DLLPRIVATE virtual	SfxPrinter* 	GetPrinter( BOOL bCreate = FALSE );
    SW_DLLPRIVATE virtual PrintDialog*	CreatePrintDialog( Window* pParent );
            SfxTabPage* 	CreatePrintOptionsPage( Window* pParent,
                                                    const SfxItemSet& rSet);
    // fuer Readonly-Umschaltung
    SW_DLLPRIVATE virtual void	Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
    SW_DLLPRIVATE void			_CheckReadonlyState();
    SW_DLLPRIVATE void			_CheckReadonlySelection();

    // Methode fuer PageDesc drehen
    SW_DLLPRIVATE void			SwapPageMargin(const SwPageDesc&, SvxLRSpaceItem& rLR);

    SW_DLLPRIVATE void			_SetZoom( const Size &rEditSz,
                              SvxZoomType eZoomType,
                              short nFactor = 100,
                              BOOL bViewOnly = FALSE);
    SW_DLLPRIVATE void			CalcAndSetBorderPixel( SvBorder &rToFill, FASTBOOL bInner );

    SW_DLLPRIVATE void		    ShowAtResize();

    SW_DLLPRIVATE virtual void	Move();

    SW_DLLPRIVATE BOOL          InsertGraphicDlg( SfxRequest& );

protected:

    SwView_Impl*	GetViewImpl() {return pViewImpl;}

    void ImpSetVerb( int nSelType );

    int				GetSelectionType() const { return nSelectionType; }
    void			SetSelectionType(int nSet) { nSelectionType = nSet;}

    // fuer die SwWebView
    void			SetShell( SfxShell* pS ) 			{ pShell = pS; }
    void 			SetFormShell( FmFormShell* pSh )	{ pFormShell = pSh; }

    virtual void	SelectShell();

    virtual void	Activate(BOOL);
    virtual void 	Deactivate(BOOL);
    virtual void 	InnerResizePixel( const Point &rOfs, const Size &rSize );
    virtual void 	OuterResizePixel( const Point &rOfs, const Size &rSize );
    virtual Size	GetOptimalSizePixel() const;

    void			SetImageButtonColor(Color& rColor);

    const SwFrmFmt* GetLastTblFrmFmt() const {return pLastTableFormat;}
    void 			SetLastTblFrmFmt(const SwFrmFmt* pSet) {pLastTableFormat = pSet;}

    // form letter execution
    void    GenerateFormLetter(BOOL bUseCurrentDocument);

public:

    SFX_DECL_VIEWFACTORY(SwView);
    SFX_DECL_INTERFACE(SW_VIEWSHELL);
    TYPEINFO();

    SfxDispatcher 	&GetDispatcher();

    void                    GotFocus() const;
    virtual SdrView*		GetDrawView() const;
    virtual	BOOL			HasUIFeature( sal_uInt32 nFeature );
    virtual	void			ShowCursor( FASTBOOL bOn = TRUE );
    virtual ErrCode			DoVerb( long nVerb );

    virtual USHORT			SetPrinter(	SfxPrinter* pNew,
                                        USHORT nDiff = SFX_PRINTER_ALL);
    virtual ErrCode			DoPrint( SfxPrinter *pPrinter, PrintDialog *pPrintDialog,
                                     BOOL bSilent );
    ShellModes				GetShellMode();

    com::sun::star::view::XSelectionSupplier*		GetUNOObject();

    String					GetSelectionTextParam( BOOL bCompleteWords,
                                                   BOOL bEraseTrail );
    virtual BOOL			HasSelection( BOOL  bText ) const;
    virtual String			GetSelectionText( BOOL bCompleteWords = FALSE );
    virtual USHORT			PrepareClose( BOOL bUI = TRUE, BOOL bForBrowsing = FALSE );
    virtual	void			MarginChanged();

    // Shell sofort wechseln ->  fuer GetSelectionObject
    void		StopShellTimer();

    inline SwWrtShell&		GetWrtShell   () const { return *pWrtShell; }
    inline SwWrtShell*		GetWrtShellPtr() const { return  pWrtShell; }

    inline 		 SwEditWin &GetEditWin() 	    { return *pEditWin; }
    inline const SwEditWin &GetEditWin () const { return *pEditWin; }

#if defined WIN || defined WNT || defined UNX
    void ScannerEventHdl( const ::com::sun::star::lang::EventObject& rEventObject );
#endif

    //Handler fuer Textbausteine an die Textshell rausreichen, gfs. anlegen
    SwGlossaryHdl*			GetGlosHdl();

    inline const Rectangle&	GetVisArea() const { return aVisArea; }

    BOOL			IsScroll(const Rectangle& rRect) const;
    void			Scroll(	const Rectangle& rRect,
                            USHORT nRangeX = USHRT_MAX,
                            USHORT nRangeY = USHRT_MAX);

    long		SetVScrollMax(long lMax);
    long		SetHScrollMax(long lMax);

    DECL_LINK( SpellError, void * );
    BOOL 			ExecSpellPopup( const Point& rPt );
    BOOL			ExecDrwTxtSpellPopup(const Point& rPt);

    void			SetTabColFromDocPos( const Point &rPt ) { aTabColFromDocPos = rPt; }
    void			SetTabColFromDoc( BOOL b ) { bTabColFromDoc = b; }
    BOOL            IsTabColFromDoc() const    { return bTabColFromDoc; }
    void            SetTabRowFromDoc( BOOL b ) { bTabRowFromDoc = b; }
    BOOL            IsTabRowFromDoc() const    { return bTabRowFromDoc; }

    // -> #i23726#
    void            SetNumRuleNodeFromDoc( SwTxtNode * pNumRuleNode )
                    { pNumRuleNodeFromDoc = pNumRuleNode; }
    void            SetNumIndentFromDoc(BOOL b) { bNumIndentFromDoc = b; }
    BOOL            IsNumIndentFromDoc() const { return NULL != pNumRuleNodeFromDoc; }
    // <- #i23726#

    void	DocSzChgd( const Size& rNewSize );
    const 	Size&	GetDocSz() const { return aDocSz; }
    virtual void	SetVisArea( const Rectangle&, BOOL bUpdateScrollbar = TRUE);
            void	SetVisArea( const Point&, BOOL bUpdateScrollbar = TRUE);
            void	CheckVisArea();

    static Dialog* 	GetSearchDialog();

    static USHORT	GetMoveType();
    static void		SetMoveType(USHORT nSet);
    void 			MoveNavigation(BOOL bNext);
    static void		SetActMark(BYTE nSet);

    BOOL 			HandleWheelCommands( const CommandEvent& );

    // Rahmen einfuegen
    void			InsFrmMode(USHORT nCols);

    void			SetZoom( SvxZoomType eZoomType, short nFactor = 100, BOOL bViewOnly = FALSE);
    virtual void	SetZoomFactor( const Fraction &rX, const Fraction & );

    void            ShowHScrollbar(sal_Bool bShow);
    sal_Bool        IsHScrollbarVisible()const;

    void            ShowVScrollbar(sal_Bool bShow);
    sal_Bool        IsVScrollbarVisible()const;

    int             CreateVLineal();
    int             KillVLineal();
    int             CreateTab();
    int             KillTab();

    int             StatVLineal() const { return ((Window*)pVRuler)->IsVisible(); }
    void			ChangeVLinealMetric(FieldUnit eUnit);
    BOOL            GetVLinealMetric(FieldUnit& rToFill) const;

    int             StatTab() const { return ((Window*)pHRuler)->IsVisible(); }
    SvxRuler&       GetHLineal()    { return *pHRuler; }
    SvxRuler&       GetVLineal()    { return *pVRuler; }
    void			InvalidateRulerPos();
    void			ChangeTabMetric(FieldUnit eUnit);
    BOOL            GetHLinealMetric(FieldUnit& rToFill) const;

        // Handler
    void			Execute(SfxRequest&);
    void			ExecPageMove(SfxRequest&);
    void			ExecStyle(SfxRequest&);
    void			ExecLingu(SfxRequest&);
    void			ExecDataBase(SfxRequest&);
    void			ExecDlg(SfxRequest&);
    void			ExecDlgExt(SfxRequest&);
    void			ExecDBDlg(SfxRequest &);
    void			ExecColl(SfxRequest&);
    void			ExecutePrint(SfxRequest&);
    void 			ExecDraw(SfxRequest&);
    void 			ExecTabWin(SfxRequest&);
    void			ExecuteStatusLine(SfxRequest&);
    DECL_LINK( ExecRulerClick, Ruler * );
    void 			ExecSearch(SfxRequest&, BOOL bNoMessage = FALSE);
    void            ExecViewOptions(SfxRequest &);

    void            StateViewOptions(SfxItemSet &);
    void            StateSearch(SfxItemSet &);
    void			GetState(SfxItemSet&);
    void			StateStatusLine(SfxItemSet&);


    // Funktionen fuer Drawing
    void			SetDrawFuncPtr(SwDrawBase* pFuncPtr);
    inline SwDrawBase* GetDrawFuncPtr(BOOL bBuf = FALSE) const	{ return pDrawActual; }
    void 			GetDrawState(SfxItemSet &rSet);
    void			ExitDraw();
    inline BOOL		IsDrawRotate()		{ return bDrawRotate; }
    inline void		FlipDrawRotate()	{ bDrawRotate = !bDrawRotate; }
    inline BOOL		IsDrawSelMode() 	{ return bDrawSelMode; }
    void			SetSelDrawSlot();
    inline void		FlipDrawSelMode()	{ bDrawSelMode = !bDrawSelMode; }
    void			NoRotate();		// Rotate-Mode abschalten
    BOOL			EnterDrawTextMode(const Point& aDocPos);
    void            LeaveDrawCreate()   { nDrawSfxId = nFormSfxId = USHRT_MAX; sDrawCustom.Erase();}
    BOOL			IsDrawMode()		{ return (nDrawSfxId != USHRT_MAX || nFormSfxId != USHRT_MAX); }
    BOOL			IsFormMode() const;
    BOOL			IsBezierEditMode();
    BOOL			AreOnlyFormsSelected() const;
    BOOL			HasDrwObj(SdrObject *pSdrObj) const;
    BOOL			HasOnlyObj(SdrObject *pSdrObj, UINT32 eObjInventor) const;
    BOOL			BeginTextEdit(	SdrObject* pObj, SdrPageView* pPV=NULL,
                                    Window* pWin=NULL, BOOL bIsNewObj=FALSE);

    void			StateTabWin(SfxItemSet&);

    // Attribute haben sich geaendert
    DECL_LINK( AttrChangedNotify, SwWrtShell * );

    // form control has been activated
    DECL_LINK( FormControlActivated, FmFormShell* );

    // Links bearbeiten
    void			EditLinkDlg();
    void			AutoCaption(const USHORT nType, const SvGlobalName *pOleId = 0);
    void			InsertCaption(const InsCaptionOpt *pOpt);

    // Async Aufruf durch Core
    void        UpdatePageNums(USHORT nPhyNum, USHORT nVirtNum, const String& rPgStr);

    String 		GetPageStr(USHORT nPhyNum, USHORT nVirtNum, const String& rPgStr);

    // Shell rausreichen
                 SfxShell		*GetCurShell()	{ return pShell; }
                 SwDocShell		*GetDocShell();
    inline const SwDocShell		*GetDocShell() const;
    inline		 FmFormShell	*GetFormShell()	{ return pFormShell; }

    //damit in DToren der SubShells ggf. pShell zurueckgesetzt werden kann
    void ResetSubShell() 	{ pShell = 0; }

    virtual void	WriteUserData(String &, BOOL bBrowse = FALSE );
    virtual void	ReadUserData(const String &, BOOL bBrowse = FALSE );
    virtual void    ReadUserDataSequence ( const com::sun::star::uno::Sequence < com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse );
    virtual void    WriteUserDataSequence ( com::sun::star::uno::Sequence < com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse );

    void SetCrsrAtTop( BOOL bFlag, BOOL bCenter = FALSE )
        { bTopCrsr = bFlag, bCenterCrsr = bCenter; }
    BOOL IsCrsrAtTop() const 					{ return bTopCrsr; }
    BOOL IsCrsrAtCenter() const 				{ return bCenterCrsr; }

    BOOL JumpToSwMark( const String& rMark );

    long InsertDoc( USHORT nSlotId, const String& rFileName,
                    const String& rFilterName, INT16 nVersion = 0 );

    // status methods for clipboard.
    // Status changes now notified from the clipboard.
    BOOL IsPasteAllowed();
    BOOL IsPasteSpecialAllowed();

    // Enable mail merge - mail merge field dialog enabled
    void EnableMailMerge(BOOL bEnable = TRUE);
    //apply Accessiblity options
    void ApplyAccessiblityOptions(SvtAccessibilityOptions& rAccessibilityOptions);

    // get print options (SwPrtOptions)
    static void MakeOptions( PrintDialog* pDlg, SwPrtOptions& rOpts,
            BOOL* pPrtProspect, BOOL bWeb, SfxPrinter* pPrt, SwPrintData* pData );

    SwView(SfxViewFrame* pFrame, SfxViewShell*);
    ~SwView();

    void NotifyDBChanged();

    SfxObjectShellRef & GetTmpSelectionDoc();
    SfxObjectShellRef & GetOrCreateTmpSelectionDoc();

    void        AddTransferable(SwTransferable& rTransferable);

    // store MailMerge data while "Back to Mail Merge Wizard" FloatingWindow is active
    // or to support printing
    void   SetMailMergeConfigItem(SwMailMergeConfigItem*  pConfigItem, sal_uInt16 nRestart, sal_Bool bIsSource);
    SwMailMergeConfigItem*  GetMailMergeConfigItem();
    sal_uInt16              GetMailMergeRestartPage() const;
    sal_Bool                IsMailMergeSourceView() const;

    void ExecFormatPaintbrush(SfxRequest &);
    void StateFormatPaintbrush(SfxItemSet &);

    //public fuer D&D
    int     InsertGraphic( const String &rPath, const String &rFilter,
                            BOOL bLink = TRUE, GraphicFilter *pFlt = 0,
                            Graphic* pPreviewGrf = 0,
                            BOOL bRule = FALSE );
};

// ----------------- inline Methoden ----------------------

inline long SwView::GetXScroll() const
{
    return aVisArea.GetWidth() * nScrollX / 100L;
}

inline long SwView::GetYScroll() const
{
    return aVisArea.GetHeight() * nScrollY / 100L;
}

inline const SwDocShell	*SwView::GetDocShell() const
{
    return ((SwView*)this)->GetDocShell();
}

//***********************************************************

SfxTabPage* CreatePrintOptionsPage(	Window *pParent,
                                    const SfxItemSet &rOptions,
                                    BOOL bPreview);

#endif
