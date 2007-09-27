/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: redlndlg.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:08:31 $
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
#ifndef _SWREDLNDLG_HXX
#define _SWREDLNDLG_HXX
#ifndef INCLUDED_SWDLLAPI_H
#include "swdllapi.h"
#endif
#ifndef _CHLDWRAP_HXX
#include "chldwrap.hxx"
#endif
#ifndef _REDLINE_HXX
#include <redline.hxx>
#endif
#ifndef _DATETIME_HXX //autogen
#include <tools/datetime.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVSTDARR_HXX
#include <svtools/svstdarr.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _BASEDLGS_HXX //autogen
#include <sfx2/basedlgs.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX //autogen
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif
#ifndef _SVX_CTREDLIN_HXX //autogen
#include <svx/ctredlin.hxx>
#endif
#ifndef _SVX_POSTATTR_HXX //autogen
#include <svx/postattr.hxx>
#endif

class SwChildWinWrapper;

struct SwRedlineDataChild
{
    const SwRedlineData*		pChild;		// Verweis auf originale gestackte Daten
    const SwRedlineDataChild*	pNext;		// Verweis auf gestackte Daten
    SvLBoxEntry*				pTLBChild;	// zugehoeriger TreeListBox-Eintrag
};

struct SwRedlineDataParent
{
    const SwRedlineData*		pData;		// RedlineDataPtr
    const SwRedlineDataChild*	pNext;		// Verweis auf gestackte Daten
    SvLBoxEntry*				pTLBParent;	// zugehoeriger TreeListBox-Eintrag
    String						sComment;	// Redline-Kommentar

    inline BOOL	operator==( const SwRedlineDataParent& rObj ) const
                        { return (pData && pData->GetSeqNo() == rObj.pData->GetSeqNo()); }
    inline BOOL	operator< ( const SwRedlineDataParent& rObj ) const
                        { return (pData && pData->GetSeqNo() <  rObj.pData->GetSeqNo()); }
};

typedef SwRedlineDataParent* SwRedlineDataParentPtr;
SV_DECL_PTRARR_DEL(SwRedlineDataParentArr, SwRedlineDataParentPtr, 10, 20)


SV_DECL_PTRARR_SORT(SwRedlineDataParentSortArr, SwRedlineDataParentPtr, 10, 20)


typedef SwRedlineDataChild* SwRedlineDataChildPtr;
SV_DECL_PTRARR_DEL(SwRedlineDataChildArr, SwRedlineDataChildPtr, 4, 4)


typedef SvLBoxEntry* SvLBoxEntryPtr;
SV_DECL_PTRARR(SvLBoxEntryArr, SvLBoxEntryPtr, 100, 100)

class SW_DLLPUBLIC SwRedlineAcceptDlg
{
    Dialog*					pParentDlg;
    SwRedlineDataParentArr	aRedlineParents;
    SwRedlineDataChildArr	aRedlineChilds;
    SwRedlineDataParentSortArr aUsedSeqNo;
    SvxAcceptChgCtr			aTabPagesCTRL;
    PopupMenu				aPopup;
    Timer					aDeselectTimer;
    Timer					aSelectTimer;
    String					sInserted;
    String					sDeleted;
    String					sFormated;
    String					sTableChgd;
    String					sFmtCollSet;
    String					sFilterAction;
    String					sAutoFormat;
    Size					aBorderSz;
    SvxTPView*				pTPView;
    SvxRedlinTable*			pTable; // PB 2006/02/02 #i48648 now SvHeaderTabListBox
    Link					aOldSelectHdl;
    Link					aOldDeselectHdl;
    BOOL 					bOnlyFormatedRedlines;
    BOOL					bHasReadonlySel;
    BOOL					bRedlnAutoFmt;

    // prevent update dialog data during longer operations (cf #102657#)
    bool                    bInhibitActivate;

    SW_DLLPRIVATE DECL_LINK( AcceptHdl,		void* );
    SW_DLLPRIVATE DECL_LINK( AcceptAllHdl,	void* );
    SW_DLLPRIVATE DECL_LINK( RejectHdl,		void* );
    SW_DLLPRIVATE DECL_LINK( RejectAllHdl,	void* );
    SW_DLLPRIVATE DECL_LINK( UndoHdl,			void* );
    SW_DLLPRIVATE DECL_LINK( DeselectHdl,		void* );
    SW_DLLPRIVATE DECL_LINK( SelectHdl,		void* );
    SW_DLLPRIVATE DECL_LINK( GotoHdl,			void* );
    SW_DLLPRIVATE DECL_LINK( CommandHdl,		void* );

    SW_DLLPRIVATE USHORT			CalcDiff(USHORT nStart, BOOL bChild);
    SW_DLLPRIVATE void			InsertChilds(SwRedlineDataParent *pParent, const SwRedline& rRedln, const USHORT nAutoFmt);
    SW_DLLPRIVATE void			InsertParents(USHORT nStart, USHORT nEnd = USHRT_MAX);
    SW_DLLPRIVATE void			RemoveParents(USHORT nStart, USHORT nEnd);
    SW_DLLPRIVATE void			InitAuthors();

    SW_DLLPRIVATE String			GetRedlineText(const SwRedline& rRedln, DateTime &rDateTime, USHORT nStack = 0);
    SW_DLLPRIVATE const String&	GetActionText(const SwRedline& rRedln, USHORT nStack = 0);
    SW_DLLPRIVATE USHORT			GetRedlinePos( const SvLBoxEntry& rEntry) const;

public:
    SwRedlineAcceptDlg(Dialog *pParent, BOOL bAutoFmt = FALSE);
    virtual ~SwRedlineAcceptDlg();

    DECL_LINK( FilterChangedHdl, void *pDummy = 0 );

    inline SvxAcceptChgCtr* GetChgCtrl()		{ return &aTabPagesCTRL; }
    inline BOOL		HasRedlineAutoFmt() const 	{ return bRedlnAutoFmt; }

    void			Init(USHORT nStart = 0);
    void			CallAcceptReject( BOOL bSelect, BOOL bAccept );

    void			Initialize(const String &rExtraData);
    void			FillInfo(String &rExtraData) const;

    virtual void	Activate();
    virtual void	Resize();
};

/*------------------------------------------------------------------------
    Beschreibung:
------------------------------------------------------------------------*/

class SwModelessRedlineAcceptDlg : public SfxModelessDialog
{
    SwRedlineAcceptDlg*		pImplDlg;
    SwChildWinWrapper*		pChildWin;

    virtual void	Resize();

public:
    SwModelessRedlineAcceptDlg(SfxBindings*, SwChildWinWrapper*, Window *pParent);
    ~SwModelessRedlineAcceptDlg();

    virtual void	Activate();
    virtual void	FillInfo(SfxChildWinInfo&) const;
    void			Initialize (SfxChildWinInfo* pInfo);
};


/*------------------------------------------------------------------------
    Beschreibung:
------------------------------------------------------------------------*/

class SwRedlineAcceptChild : public SwChildWinWrapper
{
public:
    SwRedlineAcceptChild(	Window* ,
                            USHORT nId,
                            SfxBindings*,
                            SfxChildWinInfo*  );

    SFX_DECL_CHILDWINDOW( SwRedlineAcceptChild );

    virtual BOOL	ReInitDlg(SwDocShell *pDocSh);
};


#endif


