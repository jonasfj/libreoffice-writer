/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fesh.hxx,v $
 *
 *  $Revision: 1.54 $
 *
 *  last change: $Author: hr $ $Date: 2006-08-14 15:22:05 $
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
#ifndef _FESH_HXX
#define _FESH_HXX

#ifndef _COM_SUN_STAR_EMBED_XCLASSIFIEDOBJECT_HPP_
#include <com/sun/star/embed/XClassifiedObject.hpp>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _EDITSH_HXX
#include <editsh.hxx>
#endif
#ifndef _ORNTENUM_HXX
#include <orntenum.hxx>
#endif
#ifndef _FLYENUM_HXX
#include <flyenum.hxx>
#endif

// OD 25.06.2003 #108784#
#ifndef _SVDTYPES_HXX
#include <svx/svdtypes.hxx>
#endif

#include <svtools/embedhlp.hxx>

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

class SwFlyFrm;
class SwTabCols;
class SvxBrushItem;
class SvxFrameDirectionItem;
class SwTableAutoFmt;
class SwFrm;
class SwFmtFrmSize;
class SwFmtRowSplit;
class SvxBorderLine;
class SdrObject;
class Color;
class Outliner;
class SotDataObject;
class SdrViewUserMarker;
class SwFrmFmt;
struct SwSortOptions;
class SdrMarkList;

enum FrmType
{
    //Fuer GetFrmType() und GetSelFrmType(). Der Return-Wert ist eine
    //Veroderung.
    FRMTYPE_NONE	= 0,
    FRMTYPE_PAGE	= 1,
    FRMTYPE_HEADER	= 2,
    FRMTYPE_FOOTER	= 4,
    FRMTYPE_BODY	= 8,
    FRMTYPE_COLUMN	= 16,
    FRMTYPE_TABLE	= 32,
    FRMTYPE_FLY_FREE	= 64,
    FRMTYPE_FLY_ATCNT	= 128,
    FRMTYPE_FLY_INCNT	= 256,
    FRMTYPE_FOOTNOTE	= 512,
    FRMTYPE_FTNPAGE		= 1024,
    FRMTYPE_FLY_ANY		= 2048,
    FRMTYPE_DRAWOBJ		= 4096,
    FRMTYPE_COLSECT		= 8192,
    FRMTYPE_COLSECTOUTTAB = 16384
};

#define FRMTYPE_ANYCOLSECT ( FRMTYPE_COLSECT | FRMTYPE_COLSECTOUTTAB )

enum GotoObjType
{
    DRAW_CONTROL = 1,
    DRAW_SIMPLE = 2,
    DRAW_ANY = 3,
    FLY_FRM = 4,
    FLY_GRF = 8,
    FLY_OLE = 16,
    FLY_ANY = 28,
    GOTO_ANY = 31
};

enum FlyProtectType
{
     FLYPROTECT_CONTENT		= 1,		// kann verodert werden!
     FLYPROTECT_SIZE		= 2,
     FLYPROTECT_POS			= 4,
     FLYPROTECT_PARENT 		= 8,		// nur Parents untersuchen
     FLYPROTECT_FIXED		= 16		// nur nicht aufhebbarer Schutz
                                        // z.B. durch OLE-Server, gilt auch
                                        // fuer Dialog
};

enum ObjCntType		//Fuer das Ermitteln des Cntnts per Positon (D&D)
{
    OBJCNT_NONE,
    OBJCNT_FLY,
    OBJCNT_GRF,
    OBJCNT_OLE,
    OBJCNT_SIMPLE,
    OBJCNT_CONTROL,
    OBJCNT_URLBUTTON,

    OBJCNT_GROUPOBJ,
    OBJCNT_DONTCARE		// nicht bestimmbar - unterschiedliche Objecte selektiert
};

//fuer GetAnyCurRect
enum CurRectType
{
    RECT_PAGE,					//Rect der aktuellen Seite.
    RECT_PAGE_CALC,				//... Seite wird ggf. Formatiert
    RECT_PAGE_PRT,              //Rect der aktuellen PrtArea der Seite
    RECT_FRM,                   //Rect des aktuellen Rahmen
    RECT_FLY_EMBEDDED,          //Rect des aktuellen FlyFrm
    RECT_FLY_PRT_EMBEDDED,      //Rect der PrtArea des FlyFrm
    RECT_SECTION,				//Rect des aktuellen Bereichs
    RECT_OUTTABSECTION,			//Rect des aktuellen Bereichs,
                                // aber ausserhalb der Tabelle
    RECT_SECTION_PRT,  			//Rect der aktuellen PrtArea des Bereichs
    RECT_OUTTABSECTION_PRT, 	//Rect der aktuellen PrtArea des Bereichs,
                                // aber ausserhalb der Tabelle
    RECT_HEADERFOOTER,     		//Rect des aktuellen Headers/Footer
    RECT_HEADERFOOTER_PRT 		//Rect der PrtArea des aktuellen Headers/Footers
};

struct SwGetCurColNumPara
{
    const SwFrmFmt* pFrmFmt;
    const SwRect* pPrtRect, *pFrmRect;
    SwGetCurColNumPara() : pFrmFmt( 0 ), pPrtRect( 0 ), pFrmRect( 0 ) {}
};

#define SW_PASTESDR_INSERT		1
#define SW_PASTESDR_REPLACE		2
#define SW_PASTESDR_SETATTR		3

#define SW_ADD_SELECT   1
#define SW_ENTER_GROUP  2
#define SW_LEAVE_FRAME  4

#define SW_MOVE_UP      0
#define SW_MOVE_DOWN    1
#define SW_MOVE_LEFT    2
#define SW_MOVE_RIGHT   3

#define SW_TABCOL_NONE          0
#define SW_TABCOL_HORI          1
#define SW_TABCOL_VERT          2
#define SW_TABROW_HORI          3
#define SW_TABROW_VERT          4
// --> FME 2004-07-30 #i32329# Enhanced table selection
#define SW_TABSEL_HORI          5
#define SW_TABSEL_HORI_RTL      6
#define SW_TABROWSEL_HORI       7
#define SW_TABROWSEL_HORI_RTL   8
#define SW_TABCOLSEL_HORI       9
#define SW_TABSEL_VERT          10
#define SW_TABROWSEL_VERT       11
#define SW_TABCOLSEL_VERT       12
// <--


class SW_DLLPUBLIC SwFEShell : public SwEditShell
{
    SdrViewUserMarker *pChainFrom,
                      *pChainTo;
    BOOL bCheckForOLEInCaption;

    SW_DLLPRIVATE SwFlyFrm *FindFlyFrm() const;
    SW_DLLPRIVATE SwFlyFrm *FindFlyFrm( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&  ) const;

    //Actions fuer alle Shells beenden und ChangeLink rufen.
    SW_DLLPRIVATE void EndAllActionAndCall();

    SW_DLLPRIVATE void Scroll( const Point &rPt );

    // OD 25.06.2003 #108784# - correct type of 1st parameter
    SW_DLLPRIVATE void ChangeOpaque( SdrLayerID nLayerId );

    // Used for mouse operations on a table:
    // Returns a cell frame that is 'close' to rPt.
    SW_DLLPRIVATE const SwFrm *GetBox( const Point &rPt, bool* pbRow = 0, bool* pbCol = 0 ) const;

    //0 == in keiner Spalte
    SW_DLLPRIVATE USHORT _GetCurColNum( const SwFrm *pFrm,
                          SwGetCurColNumPara* pPara ) const;

    SW_DLLPRIVATE void _GetTabCols( SwTabCols &rToFill, const SwFrm *pBox ) const;
    SW_DLLPRIVATE void _GetTabRows( SwTabCols &rToFill, const SwFrm *pBox ) const;

    SW_DLLPRIVATE BOOL ImpEndCreate();

    SW_DLLPRIVATE ObjCntType GetObjCntType( const SdrObject& rObj ) const;

    // Methoden fuers kopieren von DrawObjecten
    SW_DLLPRIVATE BOOL CopyDrawSel( SwFEShell* pDestShell, const Point& rSttPt,
                                const Point& rInsPt, BOOL bIsMove,
                                BOOL bSelectInsert );

    // get list of marked SdrObjects;
    // helper method for GetSelFrmType, IsSelContainsControl
    SW_DLLPRIVATE const SdrMarkList* _GetMarkList() const;

    SW_DLLPRIVATE BOOL CheckHeadline( bool bRepeat ) const;

public:
    TYPEINFO();
    SwFEShell( SwDoc& rDoc, Window *pWin,
               SwRootFrm *pMaster = 0, const SwViewOption *pOpt = 0 );
    SwFEShell( SwEditShell& rShell, Window *pWin );
    virtual ~SwFEShell();

    // Copy und Paste Methoden fuer das interne Clipboard
    BOOL Copy( SwDoc* pClpDoc, const String* pNewClpTxt = 0 );
    BOOL Paste( SwDoc* pClpDoc, BOOL bIncludingPageFrames = sal_False);
    //paste some pages into another doc - used in mailmerge
    BOOL PastePages( SwFEShell& rToFill, USHORT nStartPage, USHORT nEndPage);
    // Copy-Methode fuer Drag&Drop
    BOOL Copy( SwFEShell*, const Point& rSttPt, const Point& rInsPt,
                BOOL bIsMove = FALSE, BOOL bSelectInsert = TRUE );

    void SelectFlyFrm( SwFlyFrm& rFrm, BOOL bNew = FALSE );

    // befindet sich der selektierte Rahmen innerhalb eines anderen?
    const SwFrmFmt* IsFlyInFly();

//SS fuer DrawObjekte und Rahmen-----------------------------

    //Wenn ein Objekt angegeben wurde, so wird genau diese Markiert (anstatt
    //ueber die Position zu suchen.
    BOOL SelectObj( const Point& rSelPt, BYTE nFlag = 0, SdrObject *pObj = 0 );
    void DelSelectedObj();

    //Selektion nach oben unten bewegen (Z-Order).
    //TRUE	== ganz nach oben/unten
    //FALSE == einen ueberholen
    void SelectionToTop   ( BOOL bTop = TRUE );
    void SelectionToBottom( BOOL bBottom = TRUE );

    short GetLayerId() const;	//1 Heaven, 0 Hell, -1 Uneindeutig
    void  SelectionToHeaven();	//Ueber dem Dokument
    void  SelectionToHell();	//Unter dem Dokument

    // folgende zwei Methoden returnen den enum SdrHdlKind, um sich ein
    // includen von SVDRAW.HXX zu ersparen als int deklariert.
    int IsObjSelectable( const Point& rPt );
    int IsInsideSelectedObj( const Point& rPt );

    // #107513#
    // Test if there is a draw object at that position and if it should be selected.
    // The 'should' is aimed at Writer text fly frames which may be in front of
    // the draw object.
    sal_Bool ShouldObjectBeSelected(const Point& rPt);

    sal_Bool MoveAnchor( USHORT nDir );

    // Returns if Upper of frame at current position is section frame
    // Currently only used by the rules. To be replaced by something more
    // sophisticated one day.
    bool IsDirectlyInSection() const;

    //Returnwerte siehe oben FrmType.
    //pPt: Crsr bzw. DocPos; bStopAtFly: Bei Flys anhalten oder ueber den Anchor weitergehen
    // Obgleich (0,TRUE) eine Art Standard ist, sind die Parameter nicht defaultet, damit
    // bei jeder Benutzung insbesondere das bStopAtFly bewusst genutzt wird.
    USHORT GetFrmType( const Point *pPt, BOOL bStopAtFly ) const;
    USHORT GetSelFrmType() const;				//Selektion (Drawing)

    /** #108784# check whether selected frame contains a control;
     * companion method to GetSelFrmType, used for preventing
     * drag&drop of controls into header */
    bool IsSelContainsControl() const;

    ObjCntType GetObjCntType( const Point &rPt, SdrObject *&rpObj ) const;
    ObjCntType GetObjCntTypeOfSelection( SdrObject** ppObj = 0 ) const;

    //Zum Anpassen der PosAttr bei Ankerwechseln.
    SwRect  GetObjRect() const;
    //Zum Verschieben von Flys mit der Tastatur
    SwRect  GetFlyRect() const;
    // OD 18.09.2003 #i17567#, #108749#, #110354# - adjustments to allow
    //          negative vertical positions for fly frames anchored
    //          to paragraph or to character.
    // OD 02.10.2003 #i18732# - adjustments for new option 'FollowTextFlow'
    // OD 12.11.2003 #i22341# - adjustments for new vertical alignment at top of line
    void CalcBoundRect( SwRect& _orRect,
                        const RndStdIds _nAnchorId,
                        const SwRelationOrient _eHoriRelOrient = FRAME,
                        const SwRelationOrient _eVertRelOrient = FRAME,
                        const SwPosition* _pToCharCntntPos = NULL,
                        const bool _bFollowTextFlow = false,
                        bool _bMirror = false,
                        Point* _opRef = NULL,
                        Size* _opPercent = NULL ) const;

    // Groesse von Drawobjekten setzen
    void SetObjRect( const SwRect& rRect );

    long BeginDrag( const Point *pPt, BOOL bProp );
    long Drag	  ( const Point *pPt, BOOL bProp );
    long EndDrag  ( const Point *pPt, BOOL bProp );
    void BreakDrag();

    //Methoden fuer die Statuszeile.
    Point GetAnchorObjDiff() const;	//Abstand zum Anker
    Point GetObjAbsPos()	 const; //Absolute Position
    Size  GetObjSize()	     const;	//Groesse, ggf. die umschliessende

    //SS fuer die BriefUmschlaege: hole alle Seitengebundenen Objekte
    //und setze diese auf eine neue Seite.
    void GetPageObjs( SvPtrarr& rFillArr );
    void SetPageObjsNewPage( SvPtrarr& rFillArr, int nOffset = 1 );

    // zeige die aktuelle Selektion an ( ggfs. den Rahmen/DrawObject)
    virtual void MakeSelVisible();

    // returne das FrmFmt von dem evt. unter dem Point stehenden Object.
    // Das Object wird nicht selektiert!
    const SwFrmFmt* GetFmtFromObj( const Point& rPt, SwRect** pRectToFill = 0 ) const;
    // returns a format too, if the point is over the text of any fly
    const SwFrmFmt* GetFmtFromAnyObj( const Point& rPt ) const;

    //Welcher Schutz ist am selektierten Objekt gesetzt?
    BYTE IsSelObjProtected( FlyProtectType eType ) const;

    //Liefert neben der Grafik in rName bei gelinkten Grafiken den Namen mit
    //Pfad und sonst den Grafiknamen. rbLink ist TRU bei gelinkten Grafiken.
    const Graphic *GetGrfAtPos( const Point &rDocPos,
                                String &rName, BOOL &rbLink ) const;

//SS fuer Rahmen --------------------------------------------

    BOOL IsFrmSelected() const;
    BOOL GetFlyFrmAttr( SfxItemSet &rSet ) const;
    BOOL SetFlyFrmAttr( SfxItemSet &rSet );
    BOOL ResetFlyFrmAttr( USHORT nWhich, const SfxItemSet* pSet = 0 );
    const SwFrmFmt *NewFlyFrm( const SfxItemSet &rSet, BOOL bAnchValid = FALSE,
                         SwFrmFmt *pParent = 0 );
    void SetFlyPos( const Point &rAbsPos);
    Point FindAnchorPos( const Point &rAbsPos, BOOL bMoveIt = FALSE );
    // determines whether a frame or its environment is vertically formatted and right-to-left
    BOOL IsFrmVertical(BOOL bEnvironment, BOOL& bRightToLeft) const;

    SwFrmFmt* GetCurFrmFmt() const;	//Wenn Rahmen, dann Rahmenvorlage, sonst 0
    void SetFrmFmt( SwFrmFmt *pFmt, BOOL bKeepOrient = FALSE, Point* pDocPos = 0 );	//Wenn Rahmen, dann Rahmenvorlage setzen
    const SwFlyFrm *GetCurrFlyFrm() const { return FindFlyFrm(); }

    // finde/loeschen den Fly, in dem der Cursor steht
    SwFrmFmt* WizzardGetFly();

    //Selebstaendiges selektieren von Flys
    BOOL GotoNextFly(GotoObjType eType = FLY_ANY)
                                { return GotoObj( TRUE, eType ); }
    BOOL GotoPrevFly(GotoObjType eType = FLY_ANY)
                                { return GotoObj( FALSE, eType); }

    //iterieren ueber Flys - fuer Basic-Collections
    USHORT GetFlyCount(FlyCntType eType = FLYCNTTYPE_ALL) const;
    const SwFrmFmt* GetFlyNum(USHORT nIdx, FlyCntType eType = FLYCNTTYPE_ALL) const;

    //Wenn ein fly selectiert ist, zieht er den Crsr in den ersten CntntFrm
    const SwFrmFmt* SelFlyGrabCrsr();

    //Get FlyFrameFormat; fuer UI Macro Anbindung an Flys
    const SwFrmFmt* GetFlyFrmFmt() const;
          SwFrmFmt* GetFlyFrmFmt();

    //OLE, Server fordert neue Groesse an, die gewuenschten Werte werden
    //als Rahmenattribute eingestellt. Wenn die Werte nicht erlaubt sind,
    //so wird von der Formatierung geclippt und eine Scalierung eingestellt.
    //siehe CalcAndSetScale().
    void RequestObjectResize( const SwRect &rRect, const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& );

    //Der Client fuer das OleObject muss bezueglich der Scalierung auf dem
    //neuesten Stand gehalten werden. Impl in der WrtShell.
    //Wird ein Pointer auf eine Size uebergeben, so ist diese die aktuelle
    //Core-Groesse des Objectes. Anderfalls wird die Groesse per GetCurFlyRect()
    //besorgt.
    virtual void CalcAndSetScale( svt::EmbeddedObjectRef& xObj,
                                  const SwRect *pFlyPrtRect = 0,
                                  const SwRect *pFlyFrmRect = 0 ) = 0;

    //Objekte mit ActivateWhenVisible werden beim Paint Connected.
    //gerufen von notxtfrm::Paint, impl in wrtsh
    virtual void ConnectObj( svt::EmbeddedObjectRef&,
                             const SwRect &rPrt,
                             const SwRect &rFrm ) = 0;

    //Sichbaren Bereich auf das Object setzen, wenn es noch nicht sichtbar ist.
    void MakeObjVisible( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& ) const;

    // check resize of OLE-Object
    BOOL IsCheckForOLEInCaption() const 		{ return bCheckForOLEInCaption; }
    void SetCheckForOLEInCaption( BOOL bFlag )	{ bCheckForOLEInCaption = bFlag; }

    // setze am selektierten FlyFrame einen Namen
    void SetFlyName( const String& rName );
    const String& GetFlyName() const;

    // erezeuge eindeutige Namen fuer Rahmen
    String GetUniqueGrfName() const;
    String GetUniqueOLEName() const;
    String GetUniqueFrameName() const;

    // springe zum benannten Rahmen (Grafik/OLE)
    BOOL GotoFly( const String& rName, FlyCntType eType = FLYCNTTYPE_ALL,
                    BOOL bSelFrame = TRUE );
    // steht an der Position eine Grafik mit einer URL ?
    const SwFrmFmt* IsURLGrfAtPos( const Point& rPt, String* pURL = 0,
                                    String *pTargetFrameName = 0,
                                    String *pURLDescription = 0 ) const;

    //Fuer das Chain wird immer der durch das Format spezifizierte Fly
    //mit dem durch den Point getroffenen verbunden.
    //In rRect wird das Rect des Flys geliefert (fuer Highlight desselben)
    int Chainable( SwRect &rRect, const SwFrmFmt &rSource, const Point &rPt ) const;
    int Chain( SwFrmFmt &rSource, const Point &rPt );
    int Chain( SwFrmFmt &rSource, const SwFrmFmt &rDest );
    void Unchain( SwFrmFmt &rFmt );
    void HideChainMarker();
    void SetChainMarker();

    Size GetGraphicDefaultSize() const;

//SS fuer DrawObjekte ---------------------

    //Temporaer um Bug zu umgehen.
    void CheckUnboundObjects();

    //Achtung: Uneindeutikeiten bei Mehrfachselektionen.
    BOOL GetObjAttr( SfxItemSet &rSet ) const;
    BOOL SetObjAttr( const SfxItemSet &rSet );

    BOOL GotoObj( BOOL bNext, GotoObjType eType = DRAW_ANY);

    //Setzen vom DragMode (z.B. Rotate), tut nix bei Rahmenselektion.
    void SetDragMode( UINT16 eSdrDragMode );

    USHORT IsObjSelected() const;	//Liefert gleich die Anzahl der Objekte,
                                    //zaehlt aber nicht die Objekte in Gruppen.
    sal_Bool IsObjSelected( const SdrObject& rObj ) const;

    void EndTextEdit();				//Loescht ggf. das Objekt.

    //Ankertyp des selektierten Objektes, -1 bei Uneindeutigkeit oder
    //Rahmenselektion; FLY_PAGE bzw. FLY_AT_CNTNT aus frmatr.hxx sonst.
    short GetAnchorId() const;

    //Erzeugen von DrawObjekten, beim Begin wird der Objekttyp mitgegeben.
    //Beim End kann ein Cmd mitgegeben werden, hier ist ggf.
    //SDRCREATE_RESTRAINTEND fuer Ende oder SDRCREATE_NEXTPOINT fuer ein
    //Polygon relevant. Nach dem RESTRAINTEND ist das Objekt erzeugt und
    //selektiert.
    //Mit BreakCreate wird der Vorgang abgebrochen, dann ist kein Objekt
    //mehr selektiert.
    BOOL BeginCreate( UINT16 eSdrObjectKind, const Point &rPos );
    BOOL BeginCreate( UINT16 eSdrObjectKind, UINT32 eObjInventor, const Point &);
    void MoveCreate ( const Point &rPos );
    BOOL EndCreate	( UINT16 eSdrCreateCmd );
    void BreakCreate();
    BOOL IsDrawCreate() const;
    void CreateDefaultShape(UINT16 eSdrObjectKind, const Rectangle& rRect, USHORT nSlotId);

    // Funktionen f�r Rubberbox, um Draw-Objekte zu selektieren
    BOOL BeginMark( const Point &rPos );
    void MoveMark ( const Point &rPos );
    BOOL EndMark  ();
    void BreakMark();

    //Gruppe erzeugen, aufloesen, nix bei Rahmenselektion.
    BOOL IsGroupSelected();		//Kann auch eine Mischselektion sein!
    void GroupSelection();		//Hinterher ist die Gruppe selektiert.
    void UnGroupSelection();	//Die Einzelobjekte sind Selektiert
                                //Es koennen noch immer Gruppen dabei sein.

    // OD 27.06.2003 #108784# - change return type.
    bool IsGroupAllowed() const;

    void MirrorSelection( BOOL bHorizontal );	//Bei FALSE Vertikal

    //frmatr.hxx. Hier kein enum wg. Abhaengigkeiten
    //Der BOOL ist nur fuer internen Gebrauch! Anker wird nur - anhand der
    //aktuellen Dokumentposition - neu gesetzt aber nicht umgesetzt.
    void ChgAnchor( int eAnchorId, BOOL bSameOnly = FALSE,
                                   BOOL bPosCorr = TRUE );

    BOOL SetDrawingAttr( SfxItemSet &rSet );

    // hole die selectierten DrawObj als Grafik (MetaFile/Bitmap)
    // Return-Wert besagt ob konvertiert wurde!!
    BOOL GetDrawObjGraphic( ULONG nFmt, Graphic& rGrf ) const;

    void Paste( SvStream& rStm, USHORT nAction, const Point* pPt = 0 );
    BOOL Paste( const Graphic &rGrf );
    BOOL Paste( SotDataObject& rObj, const Point& rPt );

    BOOL IsAlignPossible() const;
    void SetCalcFieldValueHdl(Outliner* pOutliner);

        //Einfuegen eines DrawObjectes. Das Object muss bereits im DrawModel
        // angemeldet sein.
    void Insert( SdrObject& rDrawObj, const SfxItemSet* pFlyAttrSet = 0,
                SwFrmFmt* = 0, const Point* = 0 );

    BOOL ReplaceSdrObj( const String& rGrfName, const String& rFltName,
                        const Graphic* pGrf = 0 );

//------------------------------------------

    //Auskunft ueber naechstliegenden Inhalt zum uebergebenen Point
    Point GetCntntPos( const Point& rPoint, BOOL bNext ) const;

    Point GetContentFromPos( const Point &rPoint, int offset ) const;

    //convert document position into position relative to the current page
    Point GetRelativePagePosition(const Point& rDocPos);

    //Layout-Selektion Hiden/Zeigen und aufruf an die CrsrSh weiterreichen.
    void ShLooseFcs();
    void ShGetFcs( BOOL bUpdate = TRUE );

    //PageDescriptor-Schnittstelle
    void   ChgCurPageDesc( const SwPageDesc& );
    USHORT GetCurPageDesc( const BOOL bCalcFrm = TRUE ) const;
    USHORT GetMousePageDesc( const Point &rPt ) const;
    USHORT GetPageDescCnt() const;
    SwPageDesc* FindPageDescByName( const String& rName,
                                    BOOL bGetFromPool = FALSE,
                                    USHORT* pPos = 0 );

    const SwPageDesc& GetPageDesc( USHORT i ) const;
    void  ChgPageDesc( USHORT i, const SwPageDesc& );
    // if inside all selection only one PageDesc, return this.
    // Otherwise return 0 pointer
    const SwPageDesc* GetSelectedPageDescs() const;

    const SwRect& GetAnyCurRect( CurRectType eType,
                                 const Point* pPt = 0,
                                 const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& =
                                 ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >() ) const;

    //Seitennummer der Seite in der der Point liegt, 0 wenn keine
    //getroffen ist.
    USHORT GetPageNumber( const Point &rPoint ) const;
    BOOL GetPageNumber( long nYPos, BOOL bAtCrsrPos, USHORT& rPhyNum, USHORT& rVirtNum, String &rDisplay ) const;


    void Insert(const String& rGrfName,
                const String& rFltName,
                const Graphic* pGraphic = 0,
                const SfxItemSet* pFlyAttrSet = 0,
                const SfxItemSet* pGrfAttrSet = 0,
                SwFrmFmt* = 0 );

    void InsertObject( const svt::EmbeddedObjectRef&,
                const SfxItemSet* pFlyAttrSet = 0,
                const SfxItemSet* pGrfAttrSet = 0,
                SwFrmFmt* = 0 );
    BOOL 	FinishOLEObj();								// Server wird beendet

    //Attribute der Tabelle besorgen/setzen.
    void GetTblAttr( SfxItemSet & ) const;
    void SetTblAttr( const SfxItemSet & );

    //Tabelle vollstaendig selektiert?
    BOOL HasWholeTabSelection() const;
    //Ist der Inhalt einer Tabellenzelle oder mindestens eine Tabellenzelle
    //vollstaendig selektiert ist
    BOOL HasBoxSelection() const;

    BOOL InsertRow( USHORT nCnt, BOOL bBehind );
    BOOL InsertCol( USHORT nCnt, BOOL bBehind );  // 0 == am Ende
    BOOL DeleteCol();
    BOOL DeleteRow();

    BOOL DeleteTblSel();		//Aktuelle Selektion, ggf. die ganze Tabelle.

    USHORT MergeTab();			//Merged selektierte Tabellenteile zusammen
                                //Fehler ueber enum zurueck
    // Zelle Vertikal oder Horizontal splitten.
    BOOL SplitTab( BOOL nVert = TRUE, USHORT nCnt = 1, BOOL bSameHeight = FALSE );
    BOOL Sort(const SwSortOptions&);	//Sortieren.

    void SetRowHeight( const SwFmtFrmSize &rSz );
    //Der Pointer muss vom Aufrufer zerstoert werden wenn != 0
    void GetRowHeight( SwFmtFrmSize *&rpSz ) const;

    void SetRowSplit( const SwFmtRowSplit &rSz );
    void GetRowSplit( SwFmtRowSplit *&rpSz ) const;

    void   SetBoxAlign( USHORT nOrient );
    USHORT GetBoxAlign() const;			//USHRT_MAX fuer uneindeutig!

    //Ausgleichen der Zeilenhoehen. Mit bTstOnly festellen ob mehr als eine
    //Zeile markiert ist.
    BOOL BalanceRowHeight( BOOL bTstOnly );

    void SetTabBorders( const SfxItemSet& rSet );
    void GetTabBorders( 	  SfxItemSet& rSet) const;
    void SetTabLineStyle(const Color* pColor, BOOL bSetLine = FALSE, const SvxBorderLine* pBorderLine = NULL);

    void SetTabBackground( const SvxBrushItem &rNew );
    void GetTabBackground( SvxBrushItem &rToFill ) const;

    void SetBoxBackground( const SvxBrushItem &rNew );
    BOOL GetBoxBackground( SvxBrushItem &rToFill ) const; //FALSE uneindeutig

    void SetBoxDirection( const SvxFrameDirectionItem& rNew );
    BOOL GetBoxDirection( SvxFrameDirectionItem& rToFill ) const; //FALSE uneindeutig

    void SetRowBackground( const SvxBrushItem &rNew );
    BOOL GetRowBackground( SvxBrushItem &rToFill ) const; //FALSE uneindeutig

    BYTE WhichMouseTabCol( const Point &rPt ) const;
    void GetTabCols( SwTabCols &rToFill ) const; //Spalten- und Randinfo.
    void SetTabCols( const SwTabCols &rNew, BOOL bCurRowOnly = TRUE );
    void GetMouseTabCols( SwTabCols &rToFill, const Point &rPt ) const;
    void SetMouseTabCols( const SwTabCols &rNew, BOOL bCurRowOnly,
                          const Point &rPt );

    // --> FME 2004-07-30 #i20126# Enhanced table selection
    // pEnd will be used during MouseMove
    bool SelTblRowCol( const Point& rPt, const Point* pEnd = 0 );
    // <--

    // #i24134# adjustment of table rows via Ruler
    void GetTabRows( SwTabCols &rToFill ) const;
    void SetTabRows( const SwTabCols &rNew, BOOL bCurColOnly );
    void GetMouseTabRows( SwTabCols &rToFill, const Point &rPt ) const;
    void SetMouseTabRows( const SwTabCols &rNew, BOOL bCurColOnly, const Point &rPt );

    void ProtectCells();    //Falls eine Tabselektion besteht, wird sie ver-
                            // nichtet, wenn der Cursor nicht in Readonly darf
    void UnProtectCells();	// auf die Tabellenselektin
    void UnProtectTbls();	//bei allen Tabellen in der Selektion den Schutz aufheben
    BOOL HasTblAnyProtection( const String* pTblName = 0,
                                BOOL* pFullTblProtection = 0 );
    BOOL CanUnProtectCells() const;

    USHORT GetRowsToRepeat() const;
    void SetRowsToRepeat( USHORT nNumOfRows );
    USHORT GetVirtPageNum( const BOOL bCalcFrm = TRUE );
    //returns the number of table rows currently selected
    //if the selection start at the top of the table
    USHORT    GetRowSelectionFromTop() const;

    BOOL IsInRepeatedHeadline() const { return CheckHeadline( true ); }
    BOOL IsInHeadline() const { return CheckHeadline( false ); }

    //Stellt die Breiten der Zellen so ein, dass der Inhalt moeglichst
    //nicht umgebrochen werden muss.
    //bBalance sorgt fuer einen Ausgleich der markierten Spalten.
    void AdjustCellWidth( BOOL bBalance = FALSE );
    //Nicht erlaubt, wenn nur  leere Zellen selektiert sind.
    BOOL IsAdjustCellWidthAllowed( BOOL bBalance = FALSE ) const;

    //Ausgleich der Zellenbreiten, mit bTstOnly feststellen, ob mehr als
    //eine Zelle markiert ist.
    BOOL BalanceCellWidth( BOOL bTstOnly );

        // AutoFormat fuer die Tabelle/TabellenSelection
    BOOL SetTableAutoFmt( const SwTableAutoFmt& rNew );
        // Erfrage wie attributiert ist
    BOOL GetTableAutoFmt( SwTableAutoFmt& rGet );
        // aender eine  Zellenbreite/-Hoehe/Spaltenbreite/Zeilenhoehe
    BOOL SetColRowWidthHeight( USHORT eType, USHORT nDiff = 283 );
        // Autosumme
    BOOL GetAutoSum( String& rFml ) const;

    //Phy:	Tatsaechliche Seitenanzahl.
    //Virt: Vom User evtl. gesetzten Offset mit einbeziehen.
    USHORT	GetPhyPageNum();

    // Setzt an der aktuellen Postion einen neuen Page Offset
    void SetNewPageOffset( USHORT nOffset );
    void SetPageOffset( USHORT nOffset );	//Aendert den letzten Page Offset
    USHORT GetPageOffset() const;			//Liefert den letzten Page Offset

    //SS fuer Beschriftungen
    void InsertLabel( const SwLabelType eType, const String &rTxt, const String& rSeparator,
                      const BOOL bBefore, const USHORT nId,
                      const String& rCharacterStyle,
                      const BOOL bCpyBrd = TRUE );

    //Das Lineal will auch noch etwas von uns wissen.
    USHORT GetCurColNum( SwGetCurColNumPara* pPara = 0 ) const;	//0 == in keiner Spalte
    USHORT GetCurMouseColNum( const Point &rPt,
                            SwGetCurColNumPara* pPara = 0 ) const;
    USHORT GetCurTabColNum() const;		//0 == in keiner Tabelle
    USHORT GetCurMouseTabColNum( const Point &rPt ) const;
    USHORT GetCurOutColNum( SwGetCurColNumPara* pPara = 0 ) const;	// aktuelle aeussere Spalte

    BOOL IsTableRightToLeft() const;
    BOOL IsMouseTableRightToLeft( const Point &rPt ) const;
    BOOL IsTableVertical() const;

    BOOL IsLastCellInRow() const;
    // Die Breite des aktuellen Bereichs fuer Spaltendialog
    long GetSectionWidth( SwFmt& rFmt ) const;

    void GetConnectableFrmFmts
    (SwFrmFmt & rFmt, const String & rReference, BOOL bSuccessors,
     ::std::vector< String > & aPrevPageVec,
     ::std::vector< String > & aThisPageVec,
     ::std::vector< String > & aNextPageVec,
     ::std::vector< String > & aRestVec);

    /** SwFEShell::GetShapeBackgrd

        OD 02.09.2002 for #102450#:
        method determines background color of the page the selected drawing
        object is on and returns this color.
        If no color is found, because no drawing object is selected or ...,
        color COL_BLACK (default color on constructing object of class Color)
        is returned.

        @author OD

        @returns an object of class Color
    */
    const Color GetShapeBackgrd() const;

    /** Is default horizontal text direction for selected drawing object right-to-left

        OD 09.12.2002 #103045#
        Because drawing objects only painted for each page only, the default
        horizontal text direction of a drawing object is given by the corresponding
        page property.

        @author OD

        @returns boolean, indicating, if the horizontal text direction of the
        page, the selected drawing object is on, is right-to-left.
    */
    const bool IsShapeDefaultHoriTextDirR2L() const;

    void ParkCursorInTab();

    // -> #i23726#
    SwTxtNode * GetNumRuleNodeAtPos(const Point &rPot);
    BOOL IsNumLabel( const Point &rPt, int nMaxOffset = -1 );
    // <- #i23726#
    // --> OD 2005-02-21 #i42921#
    bool IsVerticalModeAtNdAndPos( const SwTxtNode& _rTxtNode,
                                   const Point& _rDocPos ) const;
    // <--

};

#endif
