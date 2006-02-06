/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: basesh.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2006-02-06 17:22:58 $
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
#ifndef _SWBASESH_HXX
#define _SWBASESH_HXX


#ifndef _SHELLID_HXX
#include <shellid.hxx>
#endif

#define _SVSTDARR_USHORTSSORT
#define _SVSTDARR_USHORTS

#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _SV_TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _SFXMODULE_HXX //autogen
#include <sfx2/module.hxx>
#endif
#ifndef _SFX_SHELL_HXX //autogen
#include <sfx2/shell.hxx>
#endif
#include <svtools/svstdarr.hxx>

class SwWrtShell;
class SwCrsrShell;
class SwView;
class GraphicFilter;
class SwFlyFrmAttrMgr;
class SfxItemSet;
class Graphic;

struct DBTextStruct_Impl;
class SwBaseShell: public SfxShell
{
    SwView      &rView;

    // DragModus
    static USHORT nFrameMode;

    // Bug 75078 - if in GetState the asynch call of GetGraphic returns
    //				synch, the set the state directly into the itemset
    SfxItemSet* 		pGetStateSet;

    //Update-Timer fuer Graphic
    SvUShortsSort aGrfUpdateSlots;

    DECL_LINK( GraphicArrivedHdl, SwCrsrShell* );

protected:
    SwWrtShell&			GetShell();
    SwWrtShell*			GetShellPtr();

    inline SwView&		GetView()						{ return rView; }
    inline void			SetGetStateSet( SfxItemSet* p )	{ pGetStateSet = p; }
    inline BOOL			AddGrfUpdateSlot( USHORT nSlot ){ return aGrfUpdateSlots.Insert( nSlot ); }

    DECL_STATIC_LINK( 	SwBaseShell, InsertDBTextHdl, DBTextStruct_Impl* );

    void 				InsertURLButton( const String& rURL, const String& rTarget, const String& rTxt );
    void 				InsertTable( SfxRequest& _rRequest );

public:
    SwBaseShell(SwView &rShell);
    virtual     ~SwBaseShell();
    SFX_DECL_INTERFACE(SW_BASESHELL);
    TYPEINFO();

    void        ExecDelete(SfxRequest &);

    void        ExecClpbrd(SfxRequest &);
    void        StateClpbrd(SfxItemSet &);

    void        ExecUndo(SfxRequest &);
    void        StateUndo(SfxItemSet &);

    void        Execute(SfxRequest &);
    void        GetState(SfxItemSet &);
    void        StateStyle(SfxItemSet &);

    void		ExecuteGallery(SfxRequest&);
    void		GetGalleryState(SfxItemSet&);

    void        ExecDlg(SfxRequest &);

    void        StateStatusLine(SfxItemSet &rSet);
    void        ExecTxtCtrl(SfxRequest& rReq);
    void        GetTxtFontCtrlState(SfxItemSet& rSet);
    void        GetTxtCtrlState(SfxItemSet& rSet);
    void 		GetBorderState(SfxItemSet &rSet);
    void        GetBckColState(SfxItemSet &rSet);

    void        ExecBckCol(SfxRequest& rReq);
    void		SetWrapMode( USHORT nSlot );

    void		StateDisableItems(SfxItemSet &);

    void		EditRegionDialog(SfxRequest& rReq);
    void		InsertRegionDialog(SfxRequest& rReq);

    void		ExecField(SfxRequest& rReq);

    static void    SetFrmMode( USHORT nMode, SwWrtShell *pShell );  //Mit Update!
    static void   _SetFrmMode( USHORT nMode )   { nFrameMode = nMode; }
    static USHORT  GetFrmMode()                 { return nFrameMode;  }

};


#endif
