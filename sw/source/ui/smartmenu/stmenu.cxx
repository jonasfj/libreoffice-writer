/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: stmenu.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-09-27 12:32:10 $
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
 *    Initial Contributer was Fabalabs Software GmbH, Jakob Lechner
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

// SMARTTAGS

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifndef _STMENU_HXX
#include <stmenu.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif

#include <SwSmartTagMgr.hxx>

#ifndef _STMENU_HRC
#include <stmenu.hrc>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif

#define C2U(cChar) rtl::OUString::createFromAscii(cChar)

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

SwSmartTagPopup::SwSmartTagPopup( SwView* pSwView,
                                  Sequence< rtl::OUString >& rSmartTagTypes,
                                  Sequence< Reference< container::XStringKeyMap > >& rStringKeyMaps,
                                  Reference< text::XTextRange > xTextRange ) :
    PopupMenu( SW_RES(MN_SMARTTAG_POPUP) ),
    mpSwView ( pSwView ),
    mxTextRange( xTextRange )
{
    //CreateAutoMnemonics();

    Reference <frame::XController> xController = mpSwView->GetController();
    const lang::Locale aLocale( SW_BREAKITER()->GetLocale( (LanguageType)GetAppLanguage() ) );

    USHORT nMenuPos = 0;
    USHORT nSubMenuPos = 0;
    USHORT nMenuId = 1;
    USHORT nSubMenuId = MN_ST_INSERT_START;

    const rtl::OUString aRangeText = mxTextRange->getString();

    SmartTagMgr& rSmartTagMgr = SwSmartTagMgr::Get();
    const rtl::OUString aApplicationName( rSmartTagMgr.GetApplicationName() );

    Sequence < Sequence< Reference< smarttags::XSmartTagAction > > > aActionComponentsSequence;
    Sequence < Sequence< sal_Int32 > > aActionIndicesSequence;

    rSmartTagMgr.GetActionSequences( rSmartTagTypes,
                                     aActionComponentsSequence,
                                     aActionIndicesSequence );

    InsertSeparator(0);

    for ( USHORT j = 0; j < aActionComponentsSequence.getLength(); ++j )
    {
        Reference< container::XStringKeyMap > xSmartTagProperties = rStringKeyMaps[j];

        // Get all actions references associated with the current smart tag type:
        const Sequence< Reference< smarttags::XSmartTagAction > >& rActionComponents = aActionComponentsSequence[j];
        const Sequence< sal_Int32 >& rActionIndices = aActionIndicesSequence[j];

        if ( 0 == rActionComponents.getLength() || 0 == rActionIndices.getLength() )
            continue;

        // Ask first entry for the smart tag type caption:
        Reference< smarttags::XSmartTagAction > xAction = rActionComponents[0];

        if ( !xAction.is() )
            continue;

        const sal_Int32 nSmartTagIndex = rActionIndices[0];
        const rtl::OUString aSmartTagType = xAction->getSmartTagName( nSmartTagIndex );
        const rtl::OUString aSmartTagCaption = xAction->getSmartTagCaption( nSmartTagIndex, aLocale );

        // no sub-menues if there's only one smart tag type listed:
        PopupMenu* pSbMenu = this;
        if ( 1 < aActionComponentsSequence.getLength() )
        {
            InsertItem( nMenuId, aSmartTagCaption, 0, nMenuPos++);
            pSbMenu = new PopupMenu;
            SetPopupMenu( nMenuId++, pSbMenu );
        }

        // sub-menu starts with smart tag caption and separator
        const rtl::OUString aSmartTagCaption2 = aSmartTagCaption + C2U(": ") + aRangeText;
        nSubMenuPos = 0;
        pSbMenu->InsertItem( nMenuId++, aSmartTagCaption2, MIB_NOSELECT, nSubMenuPos++ );
        pSbMenu->InsertSeparator( nSubMenuPos++ );

        // Add subitem for every action reference for the current smart tag type:
        for ( USHORT i = 0; i < rActionComponents.getLength(); ++i )
        {
            xAction = rActionComponents[i];

            for ( sal_Int32 k = 0; k < xAction->getActionCount( aSmartTagType, xController ); ++k )
            {
                const sal_uInt32 nActionID = xAction->getActionID( aSmartTagType, k, xController  );
                rtl::OUString aActionCaption = xAction->getActionCaptionFromID( nActionID,
                                                                                aApplicationName,
                                                                                aLocale,
                                                                                xSmartTagProperties,
                                                                                aRangeText,
                                                                                rtl::OUString(),
                                                                                xController,
                                                                                mxTextRange );

                pSbMenu->InsertItem( nSubMenuId++, aActionCaption, 0, nSubMenuPos++ );
                InvokeAction aEntry( xAction, xSmartTagProperties, nActionID );
                maInvokeActions.push_back( aEntry );
            }
        }
    }
}

/** Function: Execute

   executes actions by calling the invoke function of the appropriate
   smarttag library.

*/
sal_uInt16 SwSmartTagPopup::Execute( const Rectangle& rWordPos, Window* pWin )
{
    sal_uInt16 nId = PopupMenu::Execute(pWin, pWin->LogicToPixel(rWordPos));

    if ( nId == MN_SMARTTAG_OPTIONS )
    {
        SfxBoolItem aBool(SID_OPEN_SMARTTAGOPTIONS, TRUE);
        mpSwView->GetViewFrame()->GetDispatcher()->Execute( SID_AUTO_CORRECT_DLG, SFX_CALLMODE_ASYNCHRON, &aBool, 0L );
    }

    if ( nId < MN_ST_INSERT_START) return nId;
    nId -= MN_ST_INSERT_START;

    // compute smarttag lib index and action index
    if ( nId < maInvokeActions.size() )
    {
        Reference< smarttags::XSmartTagAction > xSmartTagAction = maInvokeActions[ nId ].mxAction;

        // execute action
        if ( xSmartTagAction.is() )
        {
            SmartTagMgr& rSmartTagMgr = SwSmartTagMgr::Get();

            xSmartTagAction->invokeAction( maInvokeActions[ nId ].mnActionID,
                                           rSmartTagMgr.GetApplicationName(),
                                           mpSwView->GetController(),
                                           mxTextRange,
                                           maInvokeActions[ nId ].mxSmartTagProperties,
                                           mxTextRange->getString(),
                                           rtl::OUString(),
                                           SW_BREAKITER()->GetLocale( (LanguageType)GetAppLanguage() )  );
        }
    }

    return nId;
}
