/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: document.hxx,v $
 *
 *  $Revision: 1.33 $
 *
 *  last change: $Author: obo $ $Date: 2008-02-26 14:43:27 $
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
#ifndef DOCUMENT_HXX
#define DOCUMENT_HXX

#define SMDLL	1

#ifndef _SOT_STORAGE_HXX
#include <sot/storage.hxx>
#endif
#ifndef _SOT_SOTREF_HXX //autogen
#include <sot/sotref.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <svtools/lstner.hxx>
#endif
#ifndef _SFX_OBJFAC_HXX //autogen
#include <sfx2/docfac.hxx>
#endif
#ifndef _SV_VIRDEV_HXX //autogen
#include <vcl/virdev.hxx>
#endif

#ifndef _FORMAT_HXX
#include "format.hxx"
#endif
#ifndef PARSE_HXX
#include "parse.hxx"
#endif
#ifndef SMMOD_HXX
#include "smmod.hxx"
#endif

#include <vcl/jobset.hxx>

class SmNode;
class SfxMenuBarManager;
class SfxPrinter;
class Printer;

#define HINT_DATACHANGED	1004

#define SM30BIDENT	 ((ULONG)0x534D3033L)
#define SM30IDENT	 ((ULONG)0x30334d53L)
#define SM304AIDENT  ((ULONG)0x34303330L)
#define SM30VERSION  ((ULONG)0x00010000L)
#define SM50VERSION	 ((ULONG)0x00010001L)	//Unterschied zur SM30VERSION ist
                                            //der neue Border im Format.

#define FRMIDENT	((ULONG)0x03031963L)
#define FRMVERSION	((ULONG)0x00010001L)

#define STAROFFICE_XML	"StarOffice XML (Math)"
#define MATHML_XML		"MathML XML (Math)"

/* Zugriff auf den Drucker sollte ausschliesslich ueber diese Klasse erfolgen
 * ==========================================================================
 *
 * Der Drucker kann dem Dokument oder auch dem OLE-Container gehoeren. Wenn
 * das Dokument also eine OLE-Dokument ist, so gehoert der Drucker auch
 * grundsaetzlich dem Container. Der Container arbeitet aber eventuell mit
 * einer anderen MapUnit als der Server. Der Drucker wird bezueglich des MapMode
 * im Konstruktor entsprechend eingestellt und im Destruktor wieder restauriert.
 * Das bedingt natuerlich, das diese Klasse immer nur kurze Zeit existieren darf
 * (etwa waehrend des Paints).
 * Die Kontrolle darueber ob der Drucker selbst angelegt, vom Server besorgt
 * oder dann auch NULL ist, uebernimmt die DocShell in der Methode GetPrt(),
 * fuer die der Access auch Friend der DocShell ist.
*/

class SmDocShell;
class EditEngine;

class SmPrinterAccess
{
    Printer* pPrinter;
    OutputDevice* pRefDev;
public:
    SmPrinterAccess( SmDocShell &rDocShell );
    ~SmPrinterAccess();
    Printer* GetPrinter()  { return pPrinter; }
    OutputDevice* GetRefDev()  { return pRefDev; }
};


////////////////////////////////////////////////////////////

void SetEditEngineDefaultFonts(
        EditEngine &rEditEngine,
        SfxItemPool &rEditEngineItemPool );

////////////////////////////////////////////////////////////

class SmDocShell : public SfxObjectShell, public SfxListener
{
    friend class SmPrinterAccess;
    friend class SmModel;

    String				aText;
    SmFormat   			aFormat;
    SmParser	  		aInterpreter;
    String              aAccText;
    SmNode		  	   *pTree;
    SfxMenuBarManager  *pMenuMgr;
    SfxItemPool		   *pEditEngineItemPool;
    EditEngine		   *pEditEngine;
    SfxPrinter		   *pPrinter;		//Siehe Kommentar zum SmPrinter Access!
    Printer			   *pTmpPrinter;	//ebenfalls
    long				nLeftBorder,
                        nRightBorder,
                        nTopBorder,
                        nBottomBorder;
    USHORT				nModifyCount;
    BOOL				bIsFormulaArranged;



    virtual void SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType,
                        const SfxHint& rHint, const TypeId& rHintType);

    BOOL        WriteAsMathType3( SfxMedium& );

    virtual void		Draw(OutputDevice *pDevice,
                             const JobSetup & rSetup,
                             USHORT nAspect = ASPECT_CONTENT);

    virtual void		FillClass(SvGlobalName* pClassName,
                                  sal_uInt32*  pFormat,
                                  String* pAppName,
                                  String* pFullTypeName,
                                  String* pShortTypeName,
                                  sal_Int32 nFileFormat ) const;

    virtual BOOL		SetData( const String& rData );
    virtual ULONG		GetMiscStatus() const;
    virtual void		OnDocumentPrinterChanged( Printer * );
    virtual sal_Bool    InitNew( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage );
    virtual BOOL        Load( SfxMedium& rMedium );
            void		ImplSave(  SvStorageStreamRef xStrm  );
    virtual BOOL		Save();
    virtual BOOL        SaveAs( SfxMedium& rMedium );
    virtual BOOL        ConvertTo( SfxMedium &rMedium );
    virtual BOOL        SaveCompleted( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage );

    Printer             *GetPrt();
    OutputDevice*       GetRefDev();

    BOOL				IsFormulaArranged() const { return bIsFormulaArranged; }
    void				SetFormulaArranged(BOOL bVal) { bIsFormulaArranged = bVal; }

    virtual BOOL		ConvertFrom(SfxMedium &rMedium);

public:
    TYPEINFO();
    SFX_DECL_INTERFACE(SFX_INTERFACE_SMA_START+1)
    using SotObject::GetInterface;
    SFX_DECL_OBJECTFACTORY();

                SmDocShell(SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED);
    virtual		~SmDocShell();

    void		LoadSymbols();
    void 		SaveSymbols();

    void        ArrangeFormula();

    //Zugriff fuer die View. Diese Zugriffe sind nur fuer den nicht OLE-Fall!
    //und fuer die Kommunikation mit dem SFX!
    //Alle internen Verwendungen des Printers sollten ausschlieslich uber
    //den SmPrinterAccess funktionieren.
    BOOL		HasPrinter()	{ return 0 != pPrinter; }
    SfxPrinter *GetPrinter()	{ GetPrt(); return pPrinter; }
    void	    SetPrinter( SfxPrinter * );

    const String GetComment() const;

    // to replace chars that can not be saved with the document...
    sal_Bool    ReplaceBadChars();

    void        UpdateText();
    void		SetText(const String& rBuffer);
    String& 	GetText() { return (aText); }
    void		SetFormat(SmFormat& rFormat);
    const SmFormat&  GetFormat() { return (aFormat); }

    void			Parse();
    SmParser &		GetParser() { return aInterpreter; }
    const SmNode *	GetFormulaTree() const 	{ return pTree; }
    void 			SetFormulaTree(SmNode *&rTree) { pTree = rTree; }

    String          GetAccessibleText();

    EditEngine & 	GetEditEngine();
    SfxItemPool &	GetEditEngineItemPool();

    void		Draw(OutputDevice &rDev, Point &rPosition);
    Size		GetSize();

    void        Repaint();

    virtual 	SfxUndoManager *GetUndoManager ();

    virtual     SfxItemPool& GetPool() const;

    void		Execute( SfxRequest& rReq );
    void		GetState(SfxItemSet &);

    virtual void SetVisArea (const Rectangle & rVisArea);
    virtual void SetModified(BOOL bModified);
};


#endif

