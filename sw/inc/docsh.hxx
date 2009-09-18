/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: docsh.hxx,v $
 * $Revision: 1.48.72.2 $
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
#ifndef _SWDOCSH_HXX
#define _SWDOCSH_HXX

#include <rtl/ref.hxx>
#include <com/sun/star/frame/XController.hpp>
#include <vcl/timer.hxx>
#include <sfx2/docfac.hxx>
#include <sfx2/objsh.hxx>
#include "swdllapi.h"
#include <swdll.hxx>
#include <shellid.hxx>

#include <svtools/lstner.hxx>
#include <svtools/embedhlp.hxx>

class SwDoc;
class SfxDocumentInfoDialog;
class SfxStyleSheetBasePool;
class SfxInPlaceClient;
class FontList;
class SwView;
class SwWrtShell;
class SwFEShell;
class Reader;
class SwReader;
class SwCrsrShell;
class SwSrcView;
class SwPaM;
class SwgReaderOption;
class SwOLEObj;
class IDocumentDeviceAccess;
class IDocumentSettingAccess;
class IDocumentTimerAccess;
class IDocumentChartDataProviderAccess;


class SW_DLLPUBLIC SwDocShell: public SfxObjectShell, public SfxListener
{
    SwDoc*					pDoc;			// Document
    rtl::Reference< SfxStyleSheetBasePool >	mxBasePool;		// Durchreiche fuer Formate
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
    bool                    bIsATemplate; //prevent nested calls of UpdateFontList
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

    SW_DLLPRIVATE void 					RemoveOLEObjects();
    SW_DLLPRIVATE void					CalcLayoutForOLEObjects();

    SW_DLLPRIVATE void                    Init_Impl();
    SW_DLLPRIVATE DECL_STATIC_LINK( SwDocShell, IsLoadFinished, void* );


    using SfxObjectShell::GetVisArea;
    using SfxObjectShell::GetStyleFamilyBitmap;

protected:
    /// override to update text fields
    virtual void                DoFlushDocInfo();

public:
    using SotObject::GetInterface;

    // aber selbst implementieren
    SFX_DECL_INTERFACE(SW_DOCSHELL)
    SFX_DECL_OBJECTFACTORY()
    TYPEINFO();

    static SfxInterface *_GetInterface() { return GetStaticInterface(); }

    //Das Doc wird fuer SO-Datenaustausch benoetigt!
    SwDocShell( SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED, sal_Bool _bScriptingSupport = sal_True );
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
    inline SwDoc*                   GetDoc() { return pDoc; }
    IDocumentDeviceAccess*          getIDocumentDeviceAccess();
    const IDocumentSettingAccess*   getIDocumentSettingAccess() const;
    IDocumentChartDataProviderAccess*       getIDocumentChartDataProviderAccess();


    void                    UpdateFontList();
    void					UpdateChildWindows();

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
                                   sal_Int32 nFileFormat,
                                   sal_Bool bTemplate = sal_False ) const;

    virtual void LoadStyles( SfxObjectShell& rSource );

    void _LoadStyles( SfxObjectShell& rSource, BOOL bPreserveCurrentDocument );

    // Seitenvorlagedialog anzeigen, ggf. auf Spaltenpage
    void FormatPage( const String& rPage,
                        BOOL bColumn = FALSE,
                        SwWrtShell* 	pActShell = 0 );

    // --> OD 2006-11-07 #i59688#
    // linked graphics are now loaded on demand.
    // Thus, loading of linked graphics no longer needed and necessary for
    // the load of document being finished.
//    // Timer starten fuers ueberpruefen der Grafik-Links. Sind alle
//    // vollstaendig geladen, dann ist das Doc fertig
//    void StartLoadFinishedTimer();
    void LoadingFinished();
    // <--

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

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >
                                GetController();

    SfxInPlaceClient* GetIPClient( const ::svt::EmbeddedObjectRef& xObjRef );
    SW_DLLPRIVATE sal_Bool IsTemplate() { return bIsATemplate; }
    SW_DLLPRIVATE void SetIsTemplate( bool bValue ) { bIsATemplate = bValue; }

    virtual const ::sfx2::IXmlIdRegistry* GetXmlIdRegistry() const;
};

class Graphic;
//implemented in source/ui/docvw/romenu.cxx
String ExportGraphic( const Graphic &rGraphic, const String &rGrfName );

#endif
