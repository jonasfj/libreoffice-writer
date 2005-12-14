/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: docsh.hxx,v $
 *
 *  $Revision: 1.32 $
 *
 *  last change: $Author: rt $ $Date: 2005-12-14 14:46:08 $
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
#ifndef _SWDOCSH_HXX
#define _SWDOCSH_HXX

#ifndef _TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _SFX_OBJFAC_HXX //autogen
#include <sfx2/docfac.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif

#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef SW_SWDLL_HXX
#include <swdll.hxx>
#endif
#ifndef _SHELLID_HXX
#include <shellid.hxx>
#endif

#include <svtools/lstner.hxx>

class	SwDoc;
class   SfxDocumentInfoDialog;
class	SfxStyleSheetBasePool;
class	FontList;
class	SwView;
class	SwWrtShell;
class	SwFEShell;
class	Reader;
class	SwReader;
class	SwCrsrShell;
class 	SwSrcView;
class	PushButton;
class	FixedText;
class 	SwPaM;
class	SwgReaderOption;

class SW_DLLPUBLIC SwDocShell: public SfxObjectShell, public SfxListener
{
    SwDoc*					pDoc;			// Document
    SfxStyleSheetBasePool*	pBasePool;		// Durchreiche fuer Formate
    FontList*				pFontList;		// aktuelle FontListe

    // Nix geht ohne die WrtShell (historische Gruende)
    // RuekwaertsPointer auf die View (historische Gruende)
    // Dieser gilt solange bis im Activate ein neuer gesetzt wird
    // oder dieser im Dtor der View geloescht wird
    //
    SwView* 				pView;
    SwWrtShell* 			pWrtShell;

    Timer					aFinishedTimer;	// Timer fuers ueberpriefen der
                                            // Grafik-Links. Sind alle da,
                                            // dann ist Doc voll. geladen

    //SvPersistRef            xOLEChildList;  // fuers RemoveOLEObjects
    comphelper::EmbeddedObjectContainer*    pOLEChildList;
    sal_Int16               nUpdateDocMode; // contains the com::sun::star::document::UpdateDocMode
    bool                    bInUpdateFontList; //prevent nested calls of UpdateFontList
    // Methoden fuer den Zugriff aufs Doc
    SW_DLLPRIVATE void					AddLink();
    SW_DLLPRIVATE void					RemoveLink();

    // Hint abfangen fuer DocInfo
    SW_DLLPRIVATE virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    // FileIO
    SW_DLLPRIVATE virtual sal_Bool InitNew( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage );
    SW_DLLPRIVATE virtual sal_Bool Load( SfxMedium& rMedium );
    SW_DLLPRIVATE virtual sal_Bool LoadFrom( SfxMedium& rMedium );
    SW_DLLPRIVATE virtual sal_Bool            ConvertFrom( SfxMedium &rMedium );
    SW_DLLPRIVATE virtual sal_Bool            ConvertTo( SfxMedium &rMedium );
    SW_DLLPRIVATE virtual sal_Bool SaveAs( SfxMedium& rMedium );
    SW_DLLPRIVATE virtual sal_Bool SaveCompleted( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage );

    SW_DLLPRIVATE virtual USHORT			PrepareClose( BOOL bUI = TRUE, BOOL bForBrowsing = FALSE );

    // DocInfo dem Doc melden
    //
    SW_DLLPRIVATE virtual SfxDocumentInfoDialog* CreateDocumentInfoDialog(
                                    Window *pParent, const SfxItemSet &);
    // OLE-Geraffel
    SW_DLLPRIVATE virtual void			Draw( OutputDevice*, const JobSetup&, USHORT);

    // Methoden fuer StyleSheets
    SW_DLLPRIVATE USHORT					Edit( const String &rName, const String& rParent, USHORT nFamily,
                                    USHORT nMask, BOOL bNew,
                                    BOOL bColumn = FALSE,
                                    SwWrtShell* pActShell = 0,
                                    BOOL bBasic = FALSE );
    SW_DLLPRIVATE USHORT					Delete(const String &rName, USHORT nFamily);
    SW_DLLPRIVATE USHORT					ApplyStyles(const String &rName, USHORT nFamily, SwWrtShell* pShell = 0,
                                        USHORT nMode = 0 );
    SW_DLLPRIVATE USHORT					DoWaterCan( const String &rName, USHORT nFamily);
    SW_DLLPRIVATE USHORT					UpdateStyle(const String &rName, USHORT nFamily, SwWrtShell* pShell = 0);
    SW_DLLPRIVATE USHORT					MakeByExample(const String &rName,
                                            USHORT nFamily, USHORT nMask, SwWrtShell* pShell = 0);

    SW_DLLPRIVATE void					InitDraw();
    SW_DLLPRIVATE void					SubInitNew();   // fuer InitNew und HtmlSourceModus
    inline void 			SetWrtShell(SwWrtShell* pShell)
                                { pWrtShell = pShell; }

    SW_DLLPRIVATE void 					RemoveOLEObjects();
    SW_DLLPRIVATE void					CalcLayoutForOLEObjects();

    SW_DLLPRIVATE void                    Init_Impl();
    SW_DLLPRIVATE DECL_STATIC_LINK( SwDocShell, IsLoadFinished, void* );

public:

    // aber selbst implementieren
    SFX_DECL_INTERFACE(SW_DOCSHELL);
    SFX_DECL_OBJECTFACTORY();
    TYPEINFO();

    static SfxInterface *_GetInterface() { return _GetInterfaceImpl(); }

    //Das Doc wird fuer SO-Datenaustausch benoetigt!
    SwDocShell(SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED);
    SwDocShell( SwDoc *pDoc, SfxObjectCreateMode eMode = SFX_CREATE_MODE_STANDARD );
    ~SwDocShell();

    // OLE 2.0-Benachrichtigung
    DECL_LINK( Ole2ModifiedHdl, void * );

    // OLE-Geraffel
    virtual void	  SetVisArea( const Rectangle &rRect );
    virtual Rectangle GetVisArea( USHORT nAspect ) const;
    virtual Printer  *GetDocumentPrinter();
    virtual OutputDevice* GetDocumentRefDev();
    virtual void	  OnDocumentPrinterChanged( Printer * pNewPrinter );
    virtual ULONG	  GetMiscStatus() const;

    virtual void			PrepareReload();
    virtual void			SetModified( BOOL = TRUE );

    // Dispatcher
    void					Execute(SfxRequest &);
    void					ExecStyleSheet(SfxRequest&);
    void					ExecDB(SfxRequest&);

    void					GetState(SfxItemSet &);
    void					StateAlways(SfxItemSet &);
    void					StateStyleSheet(SfxItemSet&, SwWrtShell* pSh = 0 );

    // Doc rausreichen aber VORSICHT
    inline SwDoc*			GetDoc() { return pDoc; }
    void					UpdateFontList();
    void					UpdateChildWindows();

    // DocumentInfo neu setzen
    BOOL					SetDocumentInfo(const SfxDocumentInfo& rInfo);

    // globaler IO
    virtual BOOL			Save();

    // fuer VorlagenPI
    virtual SfxStyleSheetBasePool*	GetStyleSheetPool();

    // Fuer Organizer
    virtual BOOL Insert(SfxObjectShell &rSource,
                        USHORT	nSourceIdx1,
                        USHORT	nSourceIdx2,
                        USHORT	nSourceIdx3,
                        USHORT& nIdx1,
                        USHORT& nIdx2,
                        USHORT& nIdx3,
                        USHORT& nRemovedIdx);

    virtual BOOL Remove(USHORT nIdx1,
                        USHORT nIdx2 = INDEX_IGNORE,
                        USHORT nIdx3 = INDEX_IGNORE);

    virtual Bitmap 		GetStyleFamilyBitmap( SfxStyleFamily eFamily, BmpColorMode eColorMode );

    // View setzen fuer Aktionen ueber Shell
    void 		  SetView(SwView* pVw);
    const SwView *GetView() const { return pView; }
    SwView       *GetView()       { return pView; }

    // Zugriff auf die zur SwView gehoerige SwWrtShell
          SwWrtShell *GetWrtShell() 	  { return pWrtShell; }
    const SwWrtShell *GetWrtShell() const { return pWrtShell; }

    // fuer die Core - die kennt die DocShell aber keine WrtShell!
          SwFEShell *GetFEShell();
    const SwFEShell *GetFEShell() const
                { return ((SwDocShell*)this)->GetFEShell(); }


    // Fuer Einfuegen Dokument
    Reader* StartConvertFrom(SfxMedium& rMedium, SwReader** ppRdr,
                            SwCrsrShell* pCrsrSh = 0, SwPaM* pPaM = 0);

    virtual long DdeGetData( const String& rItem, const String& rMimeType,
                             ::com::sun::star::uno::Any & rValue );
    virtual long DdeSetData( const String& rItem, const String& rMimeType,
                                const ::com::sun::star::uno::Any & rValue );
    virtual ::sfx2::SvLinkSource* DdeCreateLinkSource( const String& rItem );
    virtual void FillClass( SvGlobalName * pClassName,
                                   sal_uInt32 * pClipFormat,
                                   String * pAppName,
                                   String * pLongUserName,
                                   String * pUserName,
                                   sal_Int32 nFileFormat ) const;

    virtual void LoadStyles( SfxObjectShell& rSource );

    void _LoadStyles( SfxObjectShell& rSource, BOOL bPreserveCurrentDocument );

    // Seitenvorlagedialog anzeigen, ggf. auf Spaltenpage
    void FormatPage( const String& rPage,
                        BOOL bColumn = FALSE,
                        SwWrtShell* 	pActShell = 0 );

    // Timer starten fuers ueberpruefen der Grafik-Links. Sind alle
    // vollstaendig geladen, dann ist das Doc fertig
    void StartLoadFinishedTimer();

    // eine Uebertragung wird abgebrochen (wird aus dem SFX gerufen)
    virtual void CancelTransfers();

    // Doc aus Html-Source neu laden
    void	ReloadFromHtml( const String& rStreamName, SwSrcView* pSrcView );

    sal_Int16   GetUpdateDocMode() const {return nUpdateDocMode;}

    //Activate wait cursor for all windows of this document
    //Optionally all dispatcher could be Locked
    //Usually locking should be done using the class: SwWaitObject!
    void EnterWait( BOOL bLockDispatcher );
    void LeaveWait( BOOL bLockDispatcher );

    void ToggleBrowserMode(BOOL bOn, SwView* pView = 0);

    ULONG LoadStylesFromFile( const String& rURL, SwgReaderOption& rOpt,
                                BOOL bUnoCall );
    void InvalidateModel();
    void ReactivateModel();

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >	GetEventNames();

    // --> FME 2004-08-05 #i20883# Digital Signatures and Encryption
    virtual sal_uInt16 GetHiddenInformationState( sal_uInt16 nStates );
    // <--

    // --> FME 2005-02-25 #i42634# Overwrites SfxObjectShell::UpdateLinks
    // This new function is necessary to trigger update of links in docs
    // read by the binary filter:
    virtual void UpdateLinks();
    // <--
};

#endif
