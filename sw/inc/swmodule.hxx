/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: swmodule.hxx,v $
 *
 *  $Revision: 1.31 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 13:14:42 $
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
#ifndef _SWMODULE_HXX
#define _SWMODULE_HXX
#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _VCL_FLDUNIT_HXX
#include <vcl/fldunit.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <svtools/lstner.hxx>
#endif
#ifndef _SFXMODULE_HXX //autogen
#include <sfx2/module.hxx>
#endif

#include <tools/shl.hxx>

#ifndef _SFXLSTNER_HXX //autogen
#include <svtools/lstner.hxx>
#endif
//#ifndef SW_SWDLL_HXX
//#include <swdll.hxx>
//#endif
#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _SHELLID_HXX
#include "shellid.hxx"
#endif

#ifndef _COM_SUN_STAR_LINGUISTIC2_XLINGUSERVICEEVENTLISTENER_HPP_
#include <com/sun/star/linguistic2/XLinguServiceEventListener.hpp>
#endif
#ifndef _COM_SUN_STAR_LINGUISTIC2_XLANGUAGEGUESSING_HPP_
#include <com/sun/star/linguistic2/XLanguageGuessing.hpp>
#endif

class SvStringsDtor;
class Color;
class SfxItemSet;
class SfxRequest;
class SfxErrorHandler;
class SwDBConfig;
class SwModuleOptions;
class SwMasterUsrPref;
class SwViewOption;
class SwView;
class SwWrtShell;
class SwPrintOptions;
class SwChapterNumRules;
class SwStdFontConfig;
class SwNavigationConfig;
class SwTransferable;
class SwToolbarConfigItem;
class SwAttrPool;
namespace svtools{ class ColorConfig;}
class SvtAccessibilityOptions;
class SvtCTLOptions;
class SvtUserOptions;
class SvtUndoOptions;

struct SwDBData;
#define VIEWOPT_DEST_VIEW 		0
#define VIEWOPT_DEST_TEXT		1
#define VIEWOPT_DEST_WEB    	2
#define VIEWOPT_DEST_VIEW_ONLY 	3 //ViewOptions werden nur an der ::com::sun::star::sdbcx::View, nicht an der Appl. gesetzt

namespace com{ namespace sun{ namespace star{ namespace scanner{
    class XScannerManager;
}}}}

class SwModule: public SfxModule, public SfxListener
{
    String				sActAuthor;

    // ConfigItems
    SwModuleOptions*    pModuleConfig;
    SwMasterUsrPref*	pUsrPref;
    SwMasterUsrPref*	pWebUsrPref;
    SwPrintOptions* 	pPrtOpt;
    SwPrintOptions* 	pWebPrtOpt;
    SwChapterNumRules*	pChapterNumRules;
    SwStdFontConfig*	pStdFontConfig;
    SwNavigationConfig*	pNavigationConfig;
    SwToolbarConfigItem*pToolbarConfig;		//fuer gestackte Toolbars, welche
    SwToolbarConfigItem*pWebToolbarConfig;	//war sichtbar?
    SwDBConfig*			pDBConfig;
    svtools::ColorConfig*   pColorConfig;
    SvtAccessibilityOptions* pAccessibilityOptions;
    SvtCTLOptions*      pCTLOptions;
    SvtUserOptions*     pUserOptions;
    SvtUndoOptions*     pUndoOptions;

    SfxErrorHandler* 	pErrorHdl;

    SwAttrPool			*pAttrPool;

    // Die aktuelle View wird hier gehalten um nicht ueber
    // GetActiveView arbeiten zu muessen
    // Die View ist solange gueltig bis Sie im Activate
    // zerstoert oder ausgetauscht wird
    SwView* 			pView;

    // Liste aller Redline-Autoren
    SvStringsDtor*		pAuthorNames;

    // DictionaryList listener to trigger spellchecking or hyphenation
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XLinguServiceEventListener > xLngSvcEvtListener;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::scanner::XScannerManager >	m_xScannerManager;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XLanguageGuessing >  m_xLanguageGuesser;

    sal_Bool				bAuthorInitialised : 1;
    sal_Bool				bEmbeddedLoadSave : 1;

    // Hint abfangen fuer DocInfo
    virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

protected:
    // Briefumschlaege, Etiketten
    void                InsertEnv(SfxRequest&);
    void				InsertLab(SfxRequest&, sal_Bool bLabel);

public:
    // public Data - used for internal Clipboard / Drag & Drop / XSelection
    SwTransferable	*pClipboard, *pDragDrop, *pXSelection;


    TYPEINFO();
    SFX_DECL_INTERFACE(SW_INTERFACE_MODULE);

    // dieser Ctor nur fuer SW-Dll
    SwModule( SfxObjectFactory* pFact,
                SfxObjectFactory* pWebFact,
                    SfxObjectFactory* pGlobalFact );

    ~SwModule();

    // View setzen nur fuer internen Gebrauch,
    // aus techn. Gruenden public
    //
    inline	void		SetView(SwView* pVw) { pView = pVw; }
    inline	SwView*		GetView() { return pView; }

    //Die Handler fuer die Slots
    void				StateOther(SfxItemSet &);	// andere

    void                ExecOther(SfxRequest &);    // Felder, Formel ..

    // Benutzereinstellungen modifizieren
    SW_DLLPUBLIC const SwMasterUsrPref *GetUsrPref(sal_Bool bWeb) const;
    const SwViewOption*	GetViewOption(sal_Bool bWeb);
    void				ApplyUsrPref(const SwViewOption &, SwView*,
                                     sal_uInt16 nDest = VIEWOPT_DEST_VIEW );
    void ApplyUserMetric( FieldUnit eMetric, BOOL bWeb );
    SW_DLLPUBLIC void ApplyFldUpdateFlags(sal_Int32 nFldFlags);
    SW_DLLPUBLIC void ApplyLinkMode(sal_Int32 nNewLinkMode);

    // ConfigItems erzeugen
    SwModuleOptions*    GetModuleConfig()		{ return pModuleConfig;}
    SW_DLLPUBLIC SwPrintOptions* 	GetPrtOptions(sal_Bool bWeb);
    SW_DLLPUBLIC SwChapterNumRules*	GetChapterNumRules();
    SwStdFontConfig*	GetStdFontConfig()		{ return pStdFontConfig; }
    SwNavigationConfig* GetNavigationConfig();
    SwToolbarConfigItem*GetToolbarConfig()		{ return pToolbarConfig;	}
    SwToolbarConfigItem*GetWebToolbarConfig()   { return pWebToolbarConfig; }
    SW_DLLPUBLIC SwDBConfig*			GetDBConfig();
    svtools::ColorConfig&   GetColorConfig();
    SW_DLLPUBLIC SvtAccessibilityOptions&    GetAccessibilityOptions();
    SW_DLLPUBLIC SvtCTLOptions&      GetCTLOptions();
    SvtUserOptions&     GetUserOptions();
    SvtUndoOptions&     GetUndoOptions();

    // Ueber Sichten iterieren
    static SwView* 		GetFirstView();
    static SwView* 		GetNextView(SwView*);

    sal_Bool IsEmbeddedLoadSave() const 		{ return bEmbeddedLoadSave; }
    void SetEmbeddedLoadSave( sal_Bool bFlag )	{ bEmbeddedLoadSave = bFlag; }

    void ShowDBObj( SwView& rView, const SwDBData& rData, BOOL bOnlyIfAvailable = FALSE);

    // Tabellenmodi
    sal_Bool			IsInsTblFormatNum(sal_Bool bHTML) const;
    sal_Bool 			IsInsTblChangeNumFormat(sal_Bool bHTML) const;
    sal_Bool			IsInsTblAlignNum(sal_Bool bHTML) const;

    // Redlining
    sal_uInt16			GetRedlineAuthor();
    const String&		GetRedlineAuthor(sal_uInt16 nPos);
    sal_uInt16			InsertRedlineAuthor(const String& rAuthor);

    void				GetInsertAuthorAttr(sal_uInt16 nAuthor, SfxItemSet &rSet);
    void				GetDeletedAuthorAttr(sal_uInt16 nAuthor, SfxItemSet &rSet);
    void				GetFormatAuthorAttr(sal_uInt16 nAuthor, SfxItemSet &rSet);

    sal_uInt16				GetRedlineMarkPos();
    const Color&			GetRedlineMarkColor();

    // returne den definierten DocStat - WordDelimiter
    const String&		GetDocStatWordDelim() const;

    // Durchreichen der Metric von der ModuleConfig (fuer HTML-Export)
    sal_uInt16 GetMetric( sal_Bool bWeb ) const;

    // Update-Stati durchreichen
    sal_uInt16 GetLinkUpdMode( sal_Bool bWeb ) const;
    sal_uInt16 GetFldUpdateFlags( sal_Bool bWeb ) const;

    //virtuelle Methoden fuer den Optionendialog
    virtual SfxItemSet*	 CreateItemSet( sal_uInt16 nId );
    virtual void		 ApplyItemSet( sal_uInt16 nId, const SfxItemSet& rSet );
    virtual	SfxTabPage*	 CreateTabPage( sal_uInt16 nId, Window* pParent, const SfxItemSet& rSet );

    //hier wird der Pool angelegt und an der SfxShell gesetzt
    void	InitAttrPool();
    //Pool loeschen bevor es zu spaet ist
    void	RemoveAttrPool();

    // Invalidiert ggf. OnlineSpell-WrongListen
    void	CheckSpellChanges( sal_Bool bOnlineSpelling,
                    sal_Bool bIsSpellWrongAgain, sal_Bool bIsSpellAllAgain, sal_Bool bSmartTags );

    inline ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XLinguServiceEventListener >
            GetLngSvcEvtListener();
    inline void SetLngSvcEvtListener( ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XLinguServiceEventListener > & xLstnr);
    void    CreateLngSvcEvtListener();

    ::com::sun::star::uno::Reference<
        ::com::sun::star::scanner::XScannerManager >
            GetScannerManager();

    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XLanguageGuessing >
            GetLanguageGuesser();
};


inline ::com::sun::star::uno::Reference<
    ::com::sun::star::linguistic2::XLinguServiceEventListener >
        SwModule::GetLngSvcEvtListener()
{
    return xLngSvcEvtListener;
}

inline void SwModule::SetLngSvcEvtListener(
    ::com::sun::star::uno::Reference<
       ::com::sun::star::linguistic2::XLinguServiceEventListener > & xLstnr)
{
    xLngSvcEvtListener = xLstnr;
}


/*-----------------08.07.97 10.33-------------------
    Zugriff auf das SwModule, die ::com::sun::star::sdbcx::View und die Shell
--------------------------------------------------*/

#define SW_MOD() ( *(SwModule**) GetAppData(SHL_WRITER))

SW_DLLPUBLIC SwView* 	GetActiveView();
SW_DLLPUBLIC SwWrtShell* GetActiveWrtShell();

#endif
