/*************************************************************************
 *
 *  $RCSfile: viewsh.hxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: tl $ $Date: 2002-09-06 05:55:16 $
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
#ifndef _VIEWSH_HXX
#define _VIEWSH_HXX

#ifndef _RTTI_HXX //autogen
#include <tools/rtti.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _RING_HXX
#include <ring.hxx>
#endif
#ifndef _SWRECT_HXX
#include <swrect.hxx>
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifdef ACCESSIBLE_LAYOUT
namespace drafts {
    namespace com { namespace sun { namespace star { namespace accessibility {
           class XAccessible; } } } } }
#endif

class SwDoc;
class SfxPrinter;
class SfxProgress;
class SwRootFrm;
class SwNodes;
class SdrView;
class SfxItemPool;
class SfxViewShell;
class SwViewOption;
class SwViewImp;
class SwPrtOptions;
class SwPagePreViewPrtData;
class Window;
class OutputDevice;
class SwLayIdle;
struct ShellResource;
class SwRegionRects;
class SwFrm;
struct SwPrintData;
class SvtAccessibilityOptions;
class Fraction;

//JP 19.07.98: - Bug 52312
// define fuer Flags, die im CTOR oder den darunter liegenden Schichten
// benoetigt werden.
// Zur Zeit wird fuer die DrawPage das PreView Flag benoetigt
#define VSHELLFLAG_ISPREVIEW 			((long)0x1)

class ViewShell : public Ring
{
    friend void SetOutDev( ViewShell *pSh, OutputDevice *pOut );
    friend void SetOutDevAndWin( ViewShell *pSh, OutputDevice *pOut,
                                 Window *pWin, sal_uInt16 nZoom );

    friend class SwViewImp;
    friend class SwLayIdle;

    //Umsetzen der SwVisArea, damit vor dem Drucken sauber formatiert
    //werden kann.
    friend void SetSwVisArea( ViewShell *pSh, const SwRect &, BOOL bPDFExport = FALSE );

    static sal_Bool bLstAct;			// sal_True wenn Das EndAction der letzten Shell
                                    // laeuft; also die EndActions der
                                    // anderen Shells auf das Dokument
                                    // abgearbeitet sind.

    Point		  aPrtOffst;		 //Ofst fuer den Printer,
                                     //nicht bedruckbarer Rand.
     Size		  aBrowseBorder;	//Rand fuer Framedokumente
    SwRect 		  aInvalidRect;

    SfxViewShell *pSfxViewShell;
    SwViewImp 	 *pImp;				//Core-Interna der ViewShell.
                                    //Der Pointer ist niemals 0.

    Window		 *pWin;				 //Ist 0, wenn gedruckt wird.
    OutputDevice *pOut;				 //Fuer alle Ausgaben. Kann sein:
                                     //Window, Printer, VirtDev, ...
    OutputDevice *pRef;				 //Formatierreferenzdevice, soll zum
    // Formatieren benutzt werden, wenn gesetzt (Prospekt+Seitenvorschaudruck)

    SwViewOption *pOpt;


    sal_Bool  bDocSizeChgd	   :1;	//Fuer DocChgNotify(): Neue DocGroesse bei
                                //EndAction an das DocMDI melden.
    sal_Bool  bPaintWorks	   :1;	//Normal Painten wenn sal_True,
                                //Paint merken wenn sal_False
    sal_Bool  bPaintInProgress :1;	//Kein zweifaches Paint durchlassen.
    sal_Bool  bViewLocked	   :1;	//Lockt den sichtbaren Bereich,
                                //MakeVisible laeuft dann in's leere.
    sal_Bool  bInEndAction	   :1;  //Fiese unstaende vermeiden, siehe viewsh.cxx
    sal_Bool  bPreView		   :1;	//Ist sal_True wenns eine PreView-ViewShell ist.
    sal_Bool  bFrameView	   :1;  //sal_True wenn es ein (HTML-)Frame ist.
    sal_Bool  bEnableSmooth    :1;	//Disable des SmoothScroll z.B. fuer
                                //Drag der Scrollbars.
    sal_Bool  bEndActionByVirDev:1;	//Paints aus der EndAction immer ueber virtuelles

                                //Device (etwa beim Browsen)

    //Initialisierung, wird von den verschiedenen Konstruktoren gerufen.
    void Init( const SwViewOption *pNewOpt );

    inline void ResetInvalidRect();

    void SetSubsLines();

    void Reformat();			//Invalidert das ges. Layout (ApplyViewOption)

    void PaintDesktop( const SwRect & );		// sammeln der Werte fuers
                                                // Malen der Wiese und rufen
    // PaintDesktop gesplittet, dieser Teil wird auch von PreViewPage benutzt
    void _PaintDesktop( const SwRegionRects &rRegion );

    sal_Bool CheckInvalidForPaint( const SwRect & );//Direkt Paint oder lieber
                                                //eine Aktion ausloesen.

    void Scroll();	//Scrollen wenn sich aus der LayAction Scrollmoeglichkeiten
                    //ergaben.

    void PrepareForPrint( const SwPrtOptions &rOptions );

    void ImplApplyViewOptions( const SwViewOption &rOpt );

protected:
    static ShellResource*	pShellRes;		// Resourcen fuer die Shell
    static Window*			pCareWindow;	// diesem Fenster ausweichen

    SwRect					aVisArea;		//Die moderne Ausfuerung der VisArea
    SwDoc					*pDoc;			//Das Dokument, niemals 0

    sal_uInt16 nStartAction; //ist != 0 wenn mindestens eine ::com::sun::star::chaos::Action laeuft
    sal_uInt16 nLockPaint;	 //ist != 0 wenn das Paint gelocked ist.

public:
    TYPEINFO();

          SwViewImp *Imp() { return pImp; }
    const SwViewImp *Imp() const { return pImp; }

    const SwNodes& GetNodes() const;

    SfxPrinter*		GetPrt( sal_Bool bCreate = sal_False ) const;

    //Nach Druckerwechsel, vom Doc
    //pPDFOut != NULL is used for PDF export.
    void            InitPrt( SfxPrinter * , OutputDevice *pPDFOut = NULL );

    SwPrintData*    GetPrintData() const;
    void            SetPrintData(SwPrintData& rPrtData);

    //Klammerung von zusammengehoerenden Aktionen.
    inline void StartAction();
           void ImplStartAction();
    inline void EndAction( const sal_Bool bIdleEnd = sal_False );
           void ImplEndAction( const sal_Bool bIdleEnd = sal_False );
    sal_uInt16 ActionCount() const { return nStartAction; }
    sal_Bool ActionPend() const { return nStartAction != 0; }
    sal_Bool IsInEndAction() const { return bInEndAction; }

    void SetEndActionByVirDev( sal_Bool b )	{ bEndActionByVirDev = b; }
    sal_Bool IsEndActionByVirDev()			{ return bEndActionByVirDev; }

    // 	Per UNO wird am RootFrame fuer alle shells der ActionCount kurzfristig
    //  auf Null gesetzt und wieder restauriert
    void	SetRestoreActions(sal_uInt16 nSet);
    sal_uInt16 	GetRestoreActions() const;

    inline sal_Bool HasInvalidRect() const { return aInvalidRect.HasArea(); }
    void ChgHyphenation() {	Reformat(); }

    //Methoden fuer Paint- und Scrollrects, die auf allen Shells im
    //Ring arbeiten.
    sal_Bool AddPaintRect( const SwRect &rRect );
    void AddScrollRect( const SwFrm *pFrm, const SwRect &rRect, long nOffs );
    void SetNoNextScroll();

    void InvalidateWindows( const SwRect &rRect );
    virtual void Paint(const Rectangle &rRect);
    sal_Bool IsPaintInProgress() const { return bPaintInProgress; }

    //Benachrichtung, dass sich der sichtbare Bereich geaendert hat.
    //VisArea wird neu gesetzt, anschliessend wird gescrollt.
    //Das uebergebene Rect liegt auf Pixelgrenzen,
    //um Pixelfehler beim Scrollen zu vermeiden.
    virtual void VisPortChgd( const SwRect & );
    sal_Bool SmoothScroll( long lXDiff, long lYDiff, const Rectangle* );//Browser
    void EnableSmooth( sal_Bool b ) { bEnableSmooth = b; }

    const SwRect &VisArea() const { return aVisArea; }
        //Es wird, wenn notwendig, soweit gescrollt, dass das
        //uebergebene Rect im sichtbaren Ausschnitt liegt.
    void MakeVisible( const SwRect & );

    //Bei naechster Gelegenheit die neue Dokuemntgroesse an das UI weiterreichen.
    void SizeChgNotify(const Size &);
    void UISizeNotify();			//Das weiterreichen der aktuellen groesse.

    Point GetPagePos( sal_uInt16 nPageNum ) const;

    sal_uInt16 GetNumPages();	//Anzahl der aktuellen Seiten Layout erfragen.
    sal_Bool   IsDummyPage( USHORT nPageNum ) const;  // An empty page?

    //Invalidierung der ersten Sichtbaren Seite fuer alle Shells im Ring.
    void SetFirstVisPageInvalid();

    SwRootFrm	*GetLayout() const;
    sal_Bool		 IsNewLayout() const; //Wurde das Layout geladen oder neu
                                      //erzeugt?

     Size GetDocSize() const;// erfrage die Groesse des Dokuments

    void CalcLayout();	//Durchformatierung des Layouts erzwingen.

    inline SwDoc *GetDoc()	const { return pDoc; }	//niemals 0.

    //'Drei' OutputDevices sind relevant:
    //Der Drucker (am Dokument). Er bestimmt immer die FontMetriken.
    //Das Window, gibt es nur wenn nicht gerade gedruckt wird.
    //Das OutputDevice. Auf diesem Device finden grunds�tzlich alle
    //Ausgaben statt. Der Pointer zeigt auf das Window, den Drucker oder
    //auch mal ein VirtualDevice (z.B. Refresh der Scrollrects).
    inline OutputDevice *GetOut()	 const { return pOut; }
    inline Window		*GetWin()	 const { return pWin; }

    inline OutputDevice *GetReferenzDevice() const { return pRef; }
    inline void SetReferenzDevice( OutputDevice* pNew ) { pRef = pNew; }
    static inline sal_Bool IsLstEndAction() { return ViewShell::bLstAct; }

    // Setzt Drucker fuer ALLE Sichten im Ring; einschl. Invalidierungen
    void SetPrt(SfxPrinter *);

    //Andern alle PageDescriptoren
    void   ChgAllPageOrientation( sal_uInt16 eOri );
    void   ChgAllPageSize( Size &rSz );

    //Druckauftrag abwickeln.
    // pPDFOut != Null is: do PDF Export (no printing!)
    sal_Bool Prt( SwPrtOptions& rOptions, SfxProgress& rProgress,
                  OutputDevice *pPDFOut = NULL );
    //"Drucken" fuer OLE 2.0
    static void PrtOle2( SwDoc *pDoc, const SwViewOption *pOpt,
                         OutputDevice* pOleOut, const Rectangle& rRect );

    //Wird intern fuer die Shell gerufen die Druckt. Formatiert die Seiten.
    void CalcPagesForPrint( sal_uInt16 nMax, SfxProgress* pProgress = 0,
        const String* pStr = NULL, sal_uInt32 nMergeAct = 0, sal_uInt32 nMergeCnt = 0 );

    //All about fields.
    void UpdateFlds(sal_Bool bCloseDB = sal_False);
    sal_Bool IsAnyFieldInDoc() const;
    // update all charts, for that exists any table
    void UpdateAllCharts();
    sal_Bool HasCharts() const;

    // Sollen Absatzabstaende addiert oder maximiert werden?
    sal_Bool IsParaSpaceMax() const;
    sal_Bool IsParaSpaceMaxAtPages() const;
    void SetParaSpaceMax( sal_Bool bNew, sal_Bool bAtPages );

    // compatible behaviour of tabs
    sal_Bool IsTabCompat() const;
    void SetTabCompat( sal_Bool bNew );

    //Ruft den Idle-Formatierer des Layouts
    void LayoutIdle();

    inline const SwViewOption *GetViewOptions() const { return pOpt; }
           void  ApplyViewOptions( const SwViewOption &rOpt );
           void  SetUIOptions( const SwViewOption &rOpt );
           void  SetReadonlyOption(sal_Bool bSet);   // Readonly-Bit d. ViewOptions setzen
           void  SetPrtFormatOption(sal_Bool bSet);  // PrtFormat-Bit d. ViewOptions setzen
           void  SetReadonlySelectionOption(sal_Bool bSet);//change the selection mode in readonly docs

    static void 		  SetShellRes( ShellResource* pRes ) { pShellRes = pRes; }
    static ShellResource* GetShellRes() { return pShellRes; }

    static void 		  SetCareWin( Window* pNew ) { pCareWindow = pNew; }
    static Window* 		  GetCareWin(ViewShell& rVSh)
                        { return pCareWindow ? pCareWindow : CareChildWin(rVSh); }
    static Window* 		  CareChildWin(ViewShell& rVSh);

    inline SfxViewShell   *GetSfxViewShell() { return pSfxViewShell; }
    inline void 		  SetSfxViewShell(SfxViewShell *pNew) { pSfxViewShell = pNew; }

    // Selektion der Draw ::com::sun::star::script::Engine geaendert
    virtual void DrawSelChanged(SdrView*);

    // SS fuer Seitenvorschau anzeigen
    void PreViewPage( const Rectangle& rRect, sal_uInt16 nRowCol,
                      sal_uInt16 nSttPage, const Size& rPageSize,
                      sal_uInt16 nSelectedPage );
    void RepaintCoreRect( const SwRect& rRect, sal_uInt16 nRowCol,
                            sal_uInt16 nSttPage, const Size& rPageSize );
    // und jetzt mal auf den Drucker
    void PrintPreViewPage( SwPrtOptions& rOptions, sal_uInt16 nRowCol,
                           SfxProgress& rProgress,
                           const SwPagePreViewPrtData* = 0 );
     Size GetPagePreViewPrtMaxSize() const;
    //	errechnen & MapMode setzen
    sal_uInt16 CalcPreViewPage( const Size& rWinWidth, sal_uInt16& nRowCol,
                            sal_uInt16 nSttPage, Size& rPageSize,
                            sal_uInt16& rVirtPageNo, sal_uInt16 nAccSelPage );
    sal_Bool IsPreViewDocPos( Point& rDocPt, sal_uInt16 nRowCol, sal_uInt16 nSttPage,
                            const Size& rMaxSize );

    // Prospekt-Format drucken
    void PrintProspect( SwPrtOptions&, SfxProgress& );

    sal_Bool IsViewLocked() const { return bViewLocked; }
    void LockView( sal_Bool b )	  { bViewLocked = b;	}

    inline void LockPaint();
           void ImplLockPaint();
    inline void UnlockPaint( sal_Bool bVirDev = sal_False );
           void ImplUnlockPaint( sal_Bool bVirDev );
           sal_Bool IsPaintLocked() const { return nLockPaint != 0; }

    // Abfragen/Erzeugen DrawView + PageView
    const sal_Bool HasDrawView() const;
    void MakeDrawView();

    //DrawView darf u.U. am UI benutzt werden.
          SdrView *GetDrawView();
    const SdrView *GetDrawView() const { return ((ViewShell*)this)->GetDrawView(); }

    //sorge dafuer, das auf jedenfall die MarkListe aktuell ist (Bug 57153)
    SdrView *GetDrawViewWithValidMarkList();

    // erfrage den Attribut Pool
    inline const SfxItemPool& GetAttrPool() const;
                 SfxItemPool& GetAttrPool();

    sal_Bool IsPreView() const { return bPreView; }

    sal_Bool IsFrameView()  const { return bFrameView; }
    void SetFrameView( const Size& rBrowseBorder )
        { bFrameView = sal_True; aBrowseBorder = rBrowseBorder; }

    //Nimmt die notwendigen Invalidierungen vor,
    //wenn sich der BrowdseModus aendert, bBrowseChgd == sal_True
    //oder, im BrowseModus, wenn sich die Groessenverhaeltnisse
    //aendern (bBrowseChgd == sal_False)
    void CheckBrowseView( FASTBOOL bBrowseChgd );

    //Damit in der UI nicht ueberall das dochxx includet werden muss
    sal_Bool IsBrowseMode() const;

    sal_Bool IsHeadInBrowse() const;
    void SetHeadInBrowse( sal_Bool bOn = sal_True );
    sal_Bool IsFootInBrowse() const;
    void SetFootInBrowse( sal_Bool bOn = sal_True );

    const Size& GetBrowseBorder() const{ return aBrowseBorder; }
    void SetBrowseBorder( const Size& rNew );

#ifdef ACCESSIBLE_LAYOUT
    ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible > CreateAccessible();

    ::com::sun::star::uno::Reference<
        ::drafts::com::sun::star::accessibility::XAccessible >
            CreateAccessiblePreview( sal_uInt8 nRow,
                                     sal_uInt8 nColumn,
                                     sal_uInt16 nStartPage,
                                     const Size& rPageSize,
                                     const Point& rFreePoint,
                                     const Fraction& rScale,
                                       sal_uInt16 nSelectedPage );

    Point GetPreviewFreePix() const;

    void ShowPreViewSelection( sal_uInt16 nSelPage );
    void InvalidateAccessibleFocus();
#endif

    //apply Accessiblity options
    void ApplyAccessiblityOptions(SvtAccessibilityOptions& rAccessibilityOptions);

    ViewShell( ViewShell&, Window *pWin = 0, OutputDevice *pOut = 0,
                long nFlags = 0 );
    ViewShell( SwDoc& rDoc, Window *pWin,
               const SwViewOption *pOpt = 0, OutputDevice *pOut = 0,
               long nFlags = 0 );
    virtual ~ViewShell();

};

//---- class CurrShell verwaltet den globalen ShellPointer -------------------

class CurrShell
{
public:
    ViewShell *pPrev;
    SwRootFrm *pRoot;

    CurrShell( ViewShell *pNew );
    ~CurrShell();
};

inline void ViewShell::ResetInvalidRect()
{
    aInvalidRect.Clear();
}
inline void ViewShell::StartAction()
{
    if ( !nStartAction++ )
        ImplStartAction();
}
inline void ViewShell::EndAction( const sal_Bool bIdleEnd )
{
    if( 0 == (nStartAction - 1) )
        ImplEndAction( bIdleEnd );
    --nStartAction;
}

inline void ViewShell::LockPaint()
{
    if ( !nLockPaint++ )
        ImplLockPaint();
}
inline void ViewShell::UnlockPaint( sal_Bool bVirDev )
{
    if ( 0 == --nLockPaint )
        ImplUnlockPaint( bVirDev );
}
inline const SfxItemPool& ViewShell::GetAttrPool() const
{
    return ((ViewShell*)this)->GetAttrPool();
}



#endif //_VIEWSH_HXX
