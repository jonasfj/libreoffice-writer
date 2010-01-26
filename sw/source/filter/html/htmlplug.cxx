/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlplug.cxx,v $
 * $Revision: 1.29 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
#include <com/sun/star/embed/EmbedStates.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include "hintids.hxx"
#include <svl/urihelper.hxx>
#define _SVSTDARR_ULONGS
#include <svl/svstdarr.hxx>
#include <vcl/svapp.hxx>
#include <sfx2/frmhtml.hxx>
#include <sfx2/frmhtmlw.hxx>
#include <vcl/wrkwin.hxx>
#include <sot/storage.hxx>
#include <svx/xoutbmp.hxx>
#include <svx/ulspitem.hxx>
#include <svx/lrspitem.hxx>
#include <svtools/htmlkywd.hxx>
#include <svtools/htmltokn.h>
#include <SwAppletImpl.hxx>
#include <fmtornt.hxx>
#include <fmtfsize.hxx>
#include <fmtsrnd.hxx>
#include <fmtanchr.hxx>
#include <fmtcntnt.hxx>
#include <frmfmt.hxx>

#include <svl/ownlist.hxx>
#include "pam.hxx"
#include "doc.hxx"
#include "ndtxt.hxx"
#include "swerror.h"
#include "ndole.hxx"
#include "swtable.hxx"
#include "swhtml.hxx"
#include "wrthtml.hxx"
#include "htmlfly.hxx"
#include "swcss1.hxx"
#include <com/sun/star/embed/XClassifiedObject.hpp>
#include <com/sun/star/embed/EmbedStates.hpp>
#include <comphelper/embeddedobjectcontainer.hxx>
#include <sot/clsids.hxx>

using namespace com::sun::star;

#define HTML_DFLT_EMBED_WIDTH ((MM50*5)/2)
#define HTML_DFLT_EMBED_HEIGHT ((MM50*5)/2)

#define HTML_DFLT_APPLET_WIDTH ((MM50*5)/2)
#define HTML_DFLT_APPLET_HEIGHT ((MM50*5)/2)

namespace {

static char const sHTML_O_Hidden_False[] = "FALSE";

}

const ULONG HTML_FRMOPTS_EMBED_ALL		=
    HTML_FRMOPT_ALT |
    HTML_FRMOPT_SIZE |
    HTML_FRMOPT_NAME;
const ULONG HTML_FRMOPTS_EMBED_CNTNR	=
    HTML_FRMOPTS_EMBED_ALL |
    HTML_FRMOPT_ABSSIZE;
const ULONG HTML_FRMOPTS_EMBED			=
    HTML_FRMOPTS_EMBED_ALL |
    HTML_FRMOPT_ALIGN |
    HTML_FRMOPT_SPACE |
    HTML_FRMOPT_BRCLEAR |
    HTML_FRMOPT_NAME;
const ULONG HTML_FRMOPTS_HIDDEN_EMBED	=
    HTML_FRMOPT_ALT |
    HTML_FRMOPT_NAME;

const ULONG HTML_FRMOPTS_APPLET_ALL		=
    HTML_FRMOPT_ALT |
    HTML_FRMOPT_SIZE;
const ULONG HTML_FRMOPTS_APPLET_CNTNR	=
    HTML_FRMOPTS_APPLET_ALL |
    HTML_FRMOPT_ABSSIZE;
const ULONG HTML_FRMOPTS_APPLET			=
    HTML_FRMOPTS_APPLET_ALL |
    HTML_FRMOPT_ALIGN |
    HTML_FRMOPT_SPACE |
    HTML_FRMOPT_BRCLEAR;

const ULONG HTML_FRMOPTS_IFRAME_ALL		=
    HTML_FRMOPT_ALT |
    HTML_FRMOPT_SIZE;
const ULONG HTML_FRMOPTS_IFRAME_CNTNR	=
    HTML_FRMOPTS_IFRAME_ALL |
    HTML_FRMOPT_ABSSIZE;
const ULONG HTML_FRMOPTS_IFRAME			=
    HTML_FRMOPTS_IFRAME_ALL |
    HTML_FRMOPT_ALIGN |
    HTML_FRMOPT_SPACE |
    HTML_FRMOPT_BORDER |
    HTML_FRMOPT_BRCLEAR;

const ULONG HTML_FRMOPTS_OLE_CSS1		=
    HTML_FRMOPT_S_ALIGN |
    HTML_FRMOPT_S_SPACE;

/*  */

void SwHTMLParser::SetFixSize( const Size& rPixSize,
                               const Size& rTwipDfltSize,
                               BOOL bPrcWidth, BOOL bPrcHeight,
                               SfxItemSet& /*rCSS1ItemSet*/,
                               SvxCSS1PropertyInfo& rCSS1PropInfo,
                               SfxItemSet& rFlyItemSet )
{
    // absolulte Groessenangaben in Twip umrechnen
    BYTE nPrcWidth = 0, nPrcHeight = 0;
    Size aTwipSz( bPrcWidth || USHRT_MAX==rPixSize.Width() ? 0 : rPixSize.Width(),
                  bPrcHeight || USHRT_MAX==rPixSize.Height() ? 0 : rPixSize.Height() );
    if( (aTwipSz.Width() || aTwipSz.Height()) && Application::GetDefaultDevice() )
    {
        aTwipSz =
            Application::GetDefaultDevice()->PixelToLogic( aTwipSz,
                                                MapMode(MAP_TWIP) );
    }

    // die Breite bearbeiten
    if( SVX_CSS1_LTYPE_PERCENTAGE == rCSS1PropInfo.eWidthType )
    {
        nPrcWidth = (BYTE)rCSS1PropInfo.nWidth;
        aTwipSz.Width() = rTwipDfltSize.Width();
    }
    else if( SVX_CSS1_LTYPE_TWIP== rCSS1PropInfo.eWidthType )
    {
        aTwipSz.Width() = rCSS1PropInfo.nWidth;
    }
    else if( bPrcWidth && rPixSize.Width() )
    {
        nPrcWidth = (BYTE)rPixSize.Width();
        if( nPrcWidth > 100 )
            nPrcWidth = 100;

        aTwipSz.Width() = rTwipDfltSize.Width();
    }
    else if( USHRT_MAX==rPixSize.Width() )
    {
        aTwipSz.Width() = rTwipDfltSize.Width();
    }
    if( aTwipSz.Width() < MINFLY )
    {
        aTwipSz.Width() = MINFLY;
    }

    // Hoehe bearbeiten
    if( SVX_CSS1_LTYPE_PERCENTAGE == rCSS1PropInfo.eHeightType )
    {
        nPrcHeight = (BYTE)rCSS1PropInfo.nHeight;
        aTwipSz.Height() = rTwipDfltSize.Height();
    }
    else if( SVX_CSS1_LTYPE_TWIP== rCSS1PropInfo.eHeightType )
    {
        aTwipSz.Height() = rCSS1PropInfo.nHeight;
    }
    else if( bPrcHeight && rPixSize.Height() )
    {
        nPrcHeight = (BYTE)rPixSize.Height();
        if( nPrcHeight > 100 )
            nPrcHeight = 100;

        aTwipSz.Height() = rTwipDfltSize.Height();
    }
    else if( USHRT_MAX==rPixSize.Height() )
    {
        aTwipSz.Height() = rTwipDfltSize.Height();
    }
    if( aTwipSz.Height() < MINFLY )
    {
        aTwipSz.Height() = MINFLY;
    }

    // Size setzen
    SwFmtFrmSize aFrmSize( ATT_FIX_SIZE, aTwipSz.Width(), aTwipSz.Height() );
    aFrmSize.SetWidthPercent( nPrcWidth );
    aFrmSize.SetHeightPercent( nPrcHeight );
    rFlyItemSet.Put( aFrmSize );
}

void SwHTMLParser::SetSpace( const Size& rPixSpace,
                             SfxItemSet& rCSS1ItemSet,
                             SvxCSS1PropertyInfo& rCSS1PropInfo,
                             SfxItemSet& rFlyItemSet )
{
    sal_Int32 nLeftSpace = 0, nRightSpace = 0;
    sal_uInt16 nUpperSpace = 0, nLowerSpace = 0;
    if( (rPixSpace.Width() || rPixSpace.Height()) && Application::GetDefaultDevice() )
    {
        Size aTwipSpc( rPixSpace.Width(), rPixSpace.Height() );
        aTwipSpc =
            Application::GetDefaultDevice()->PixelToLogic( aTwipSpc,
                                                MapMode(MAP_TWIP) );
        nLeftSpace = nRightSpace = aTwipSpc.Width();
        nUpperSpace = nLowerSpace = (USHORT)aTwipSpc.Height();
    }

    // linken/rechten Rand setzen
    const SfxPoolItem *pItem;
    if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_LR_SPACE, TRUE, &pItem ) )
    {
        // Ggf. den Erstzeilen-Einzug noch plaetten
        const SvxLRSpaceItem *pLRItem = (const SvxLRSpaceItem *)pItem;
        SvxLRSpaceItem aLRItem( *pLRItem );
        aLRItem.SetTxtFirstLineOfst( 0 );
        if( rCSS1PropInfo.bLeftMargin )
        {
            nLeftSpace = aLRItem.GetLeft();
            rCSS1PropInfo.bLeftMargin = FALSE;
        }
        if( rCSS1PropInfo.bRightMargin )
        {
            nRightSpace = aLRItem.GetRight();
            rCSS1PropInfo.bRightMargin = FALSE;
        }
        rCSS1ItemSet.ClearItem( RES_LR_SPACE );
    }
    if( nLeftSpace > 0 || nRightSpace > 0 )
    {
        SvxLRSpaceItem aLRItem( RES_LR_SPACE );
        aLRItem.SetLeft( nLeftSpace > 0 ? nLeftSpace : 0 );
        aLRItem.SetRight( nRightSpace > 0 ? nRightSpace : 0 );
        rFlyItemSet.Put( aLRItem );
        if( nLeftSpace )
        {
            const SwFmtHoriOrient& rHoriOri =
                (const SwFmtHoriOrient&)rFlyItemSet.Get( RES_HORI_ORIENT );
            if( text::HoriOrientation::NONE == rHoriOri.GetHoriOrient() )
            {
                SwFmtHoriOrient aHoriOri( rHoriOri );
                aHoriOri.SetPos( aHoriOri.GetPos() + nLeftSpace );
                rFlyItemSet.Put( aHoriOri );
            }
        }
    }

    // oberen/unteren Rand setzen
    if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_UL_SPACE, TRUE, &pItem ) )
    {
        // Ggf. den Erstzeilen-Einzug noch plaetten
        const SvxULSpaceItem *pULItem = (const SvxULSpaceItem *)pItem;
        if( rCSS1PropInfo.bTopMargin )
        {
            nUpperSpace = pULItem->GetUpper();
            rCSS1PropInfo.bTopMargin = FALSE;
        }
        if( rCSS1PropInfo.bBottomMargin )
        {
            nLowerSpace = pULItem->GetLower();
            rCSS1PropInfo.bBottomMargin = FALSE;
        }
        rCSS1ItemSet.ClearItem( RES_UL_SPACE );
    }
    if( nUpperSpace || nLowerSpace )
    {
        SvxULSpaceItem aULItem( RES_UL_SPACE );
        aULItem.SetUpper( nUpperSpace );
        aULItem.SetLower( nLowerSpace );
        rFlyItemSet.Put( aULItem );
        if( nUpperSpace )
        {
            const SwFmtVertOrient& rVertOri =
                (const SwFmtVertOrient&)rFlyItemSet.Get( RES_VERT_ORIENT );
            if( text::VertOrientation::NONE == rVertOri.GetVertOrient() )
            {
                SwFmtVertOrient aVertOri( rVertOri );
                aVertOri.SetPos( aVertOri.GetPos() + nUpperSpace );
                rFlyItemSet.Put( aVertOri );
            }
        }
    }
}

/*  */

void SwHTMLParser::InsertEmbed()
{
    String aURL, aType, aName, aAlt, aId, aStyle, aClass;
    Size aSize( USHRT_MAX, USHRT_MAX );
    Size aSpace( USHRT_MAX, USHRT_MAX );
    BOOL bPrcWidth = FALSE, bPrcHeight = FALSE, bHidden = FALSE;
    sal_Int16 eVertOri = text::VertOrientation::NONE;
    sal_Int16 eHoriOri = text::HoriOrientation::NONE;
    SvCommandList aCmdLst;
    const HTMLOptions *pHTMLOptions = GetOptions();

    // Die Optionen werden vorwaerts gelesen, weil die Plugins sie in
    // dieser Reihenfolge erwarten. Trotzdem darf immer nur der erste
    // Wert einer Option beruecksichtigt werden.
    USHORT nArrLen = pHTMLOptions->Count();
    for( USHORT i=0; i<nArrLen; i++ )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[i];
        switch( pOption->GetToken() )
        {
        case HTML_O_ID:
            aId = pOption->GetString();
            break;
        case HTML_O_STYLE:
            aStyle = pOption->GetString();
            break;
        case HTML_O_CLASS:
            aClass = pOption->GetString();
            break;
        case HTML_O_NAME:
            aName = pOption->GetString();
            break;
        case HTML_O_SRC:
            if( !aURL.Len() )
                aURL = pOption->GetString();
            break;
        case HTML_O_ALT:
            aAlt = pOption->GetString();
            break;
        case HTML_O_TYPE:
            if( !aType.Len() )
                aType = pOption->GetString();
            break;
        case HTML_O_ALIGN:
            if( eVertOri==text::VertOrientation::NONE && eHoriOri==text::HoriOrientation::NONE )
            {
                eVertOri = pOption->GetEnum( aHTMLImgVAlignTable, eVertOri );
                eHoriOri = pOption->GetEnum( aHTMLImgHAlignTable, eHoriOri );
            }
            break;
        case HTML_O_WIDTH:
            if( USHRT_MAX==aSize.Width() )
            {
                bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
                aSize.Width() = (long)pOption->GetNumber();
            }
            break;
        case HTML_O_HEIGHT:
            if( USHRT_MAX==aSize.Height() )
            {
                bPrcHeight = (pOption->GetString().Search('%') != STRING_NOTFOUND);
                aSize.Height() = (long)pOption->GetNumber();
            }
            break;
        case HTML_O_HSPACE:
            if( USHRT_MAX==aSpace.Width() )
                aSpace.Width() = (long)pOption->GetNumber();
            break;
        case HTML_O_VSPACE:
            if( USHRT_MAX==aSpace.Height() )
                aSpace.Height() = (long)pOption->GetNumber();
            break;
        case HTML_O_UNKNOWN:
            if( pOption->GetTokenString().EqualsIgnoreCaseAscii( OOO_STRING_SW_HTML_O_Hidden ) )
                bHidden =
                    !pOption->GetString().EqualsIgnoreCaseAscii( sHTML_O_Hidden_False );
            break;
        }

        // Es werden alle Parameter an das Plugin weitergereicht
        aCmdLst.Append( pOption->GetTokenString(), pOption->GetString() );
    }

    SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aPropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
        ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo );

    // Die Default-Werte umsetzen (ausser Hoehe/Breite, das macht schon
    // SetFrmSize() fuer uns)
    if( eVertOri==text::VertOrientation::NONE && eHoriOri==text::HoriOrientation::NONE )
        eVertOri = text::VertOrientation::TOP;
    if( USHRT_MAX==aSpace.Width() )
        aSpace.Width() = 0;
    if( USHRT_MAX==aSpace.Height() )
        aSpace.Height() = 0;
    if( bHidden )
    {
        // Size (0,0) wird in SetFrmSize auf (MINFLY, MINFLY) umgebogen
        aSize.Width() = 0; aSize.Height() = 0;
        aSpace.Width() = 0;	aSpace.Height() = 0;
        bPrcWidth = bPrcHeight = FALSE;
    }

    // die URL aufbereiten
    INetURLObject aURLObj;
    bool bHasURL = aURL.Len() &&
                   aURLObj.SetURL(
                       URIHelper::SmartRel2Abs(
                           INetURLObject(sBaseURL), aURL,
                           URIHelper::GetMaybeFileHdl()) );

    // #109761# do not insert plugin if it has neither URL nor type
    bool bHasType = aType.Len() != 0;
    if( !bHasURL && !bHasType )
        return;

    // das Plugin anlegen
    comphelper::EmbeddedObjectContainer aCnt;
    ::rtl::OUString aObjName;
    uno::Reference < embed::XEmbeddedObject > xObj = aCnt.CreateEmbeddedObject( SvGlobalName( SO3_PLUGIN_CLASSID ).GetByteSequence(), aObjName );
    if ( svt::EmbeddedObjectRef::TryRunningState( xObj ) )
    {
        uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
        if ( xSet.is() )
        {
            if( bHasURL )
                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginURL"),
                    uno::makeAny( ::rtl::OUString( aURL ) ) );
            if( bHasType )
                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginMimeType"),
                    uno::makeAny( ::rtl::OUString( aType ) ) );

            uno::Sequence < beans::PropertyValue > aProps;
            aCmdLst.FillSequence( aProps );
            xSet->setPropertyValue( ::rtl::OUString::createFromAscii("PluginCommands"), uno::makeAny( aProps ) );

            // TODO/LATER: EnableSetModified?!
            //pPlugin->EnableSetModified( TRUE );
        }
    }

    SfxItemSet aFrmSet( pDoc->GetAttrPool(),
                        RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
    if( !IsNewDoc() )
        Reader::ResetFrmFmtAttrs( aFrmSet );

    // den Anker setzen
    if( !bHidden )
    {
        SetAnchorAndAdjustment( eVertOri, eHoriOri, aItemSet, aPropInfo, aFrmSet );
    }
    else
    {
        SwFmtAnchor aAnchor( FLY_AT_PARA );
        aAnchor.SetAnchor( pPam->GetPoint() );
        aFrmSet.Put( aAnchor );
        aFrmSet.Put( SwFmtHoriOrient( 0, text::HoriOrientation::LEFT, text::RelOrientation::FRAME) );
        aFrmSet.Put( SwFmtSurround( SURROUND_THROUGHT ) );
        aFrmSet.Put( SwFmtVertOrient( 0, text::VertOrientation::TOP, text::RelOrientation::PRINT_AREA ) );
    }

    // und noch die Groesse des Rahmens
    Size aDfltSz( HTML_DFLT_EMBED_WIDTH, HTML_DFLT_EMBED_HEIGHT );
    SetFixSize( aSize, aDfltSz, bPrcWidth, bPrcHeight, aItemSet, aPropInfo,
                aFrmSet );
    SetSpace( aSpace, aItemSet, aPropInfo, aFrmSet );

    // und in das Dok einfuegen
    SwFrmFmt* pFlyFmt =
        pDoc->Insert( *pPam, ::svt::EmbeddedObjectRef( xObj, embed::Aspects::MSOLE_CONTENT ), &aFrmSet, NULL, NULL );

    // Namen am FrmFmt setzen
    if( aName.Len() )
        pFlyFmt->SetName( aName );

    // den alternativen Text setzen
    SwNoTxtNode *pNoTxtNd =
        pDoc->GetNodes()[ pFlyFmt->GetCntnt().GetCntntIdx()
                          ->GetIndex()+1 ]->GetNoTxtNode();
    pNoTxtNd->SetTitle( aAlt );

    // Ggf Frames anlegen und auto-geb. Rahmen registrieren
    if( !bHidden )
    {
        // HIDDEN-Plugins sollen absatzgebunden bleiben. Da RegisterFlyFrm
        // absatzgebundene Rahmen mit DUrchlauf in am Zeichen gebundene
        // Rahmen umwandelt, muessen die Frames hier von Hand angelegt werden.
        RegisterFlyFrm( pFlyFmt );
    }
}

/*  */

#ifdef SOLAR_JAVA
void SwHTMLParser::NewObject()
{
    String aClassID, aName, aStandBy, aId, aStyle, aClass;
    Size aSize( USHRT_MAX, USHRT_MAX );
    Size aSpace( 0, 0 );
    sal_Int16 eVertOri = text::VertOrientation::TOP;
    sal_Int16 eHoriOri = text::HoriOrientation::NONE;

    sal_Bool bPrcWidth = sal_False, bPrcHeight = sal_False,
             bDeclare = sal_False;
    // Eine neue Command-List anlegen
    if( pAppletImpl )
        delete pAppletImpl;
    pAppletImpl = new SwApplet_Impl( pDoc->GetAttrPool(),
                                     RES_FRMATR_BEGIN, RES_FRMATR_END-1 );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( USHORT i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        switch( pOption->GetToken() )
        {
        case HTML_O_ID:
            aId = pOption->GetString();
            break;
        case HTML_O_STYLE:
            aStyle = pOption->GetString();
            break;
        case HTML_O_CLASS:
            aClass = pOption->GetString();
            break;
        case HTML_O_DECLARE:
            bDeclare = sal_True;
            break;
        case HTML_O_CLASSID:
            aClassID = pOption->GetString();
            break;
        case HTML_O_CODEBASE:
            break;
        case HTML_O_DATA:
            break;
        case HTML_O_TYPE:
            break;
        case HTML_O_CODETYPE:
            break;
        case HTML_O_ARCHIVE:
        case HTML_O_UNKNOWN:
            break;
        case HTML_O_STANDBY:
            aStandBy = pOption->GetString();
            break;
        case HTML_O_WIDTH:
            bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
            aSize.Width() = (long)pOption->GetNumber();
            break;
        case HTML_O_HEIGHT:
            bPrcHeight = (pOption->GetString().Search('%') != STRING_NOTFOUND);
            aSize.Height() = (long)pOption->GetNumber();
            break;
        case HTML_O_ALIGN:
            eVertOri = pOption->GetEnum( aHTMLImgVAlignTable, eVertOri );
            eHoriOri = pOption->GetEnum( aHTMLImgHAlignTable, eHoriOri );
            break;
        case HTML_O_USEMAP:
            break;
        case HTML_O_NAME:
            aName = pOption->GetString();
            break;
        case HTML_O_HSPACE:
            aSpace.Width() = (long)pOption->GetNumber();
            break;
        case HTML_O_VSPACE:
            aSpace.Height() = (long)pOption->GetNumber();
            break;
        case HTML_O_BORDER:
            break;

        case HTML_O_SDONCLICK:
        case HTML_O_ONCLICK:
        case HTML_O_SDONMOUSEOVER:
        case HTML_O_ONMOUSEOVER:
        case HTML_O_SDONMOUSEOUT:
        case HTML_O_ONMOUSEOUT:
            break;
        }
        // Es werden alle Parameter auch an das Applet weitergereicht
        pAppletImpl->AppendParam( pOption->GetTokenString(),
                                  pOption->GetString() );

    }

    // Objects that are declared only are not evaluated. Moreover, only
    // Java applets are supported.
    sal_Bool bIsApplet = sal_False;;

    if( !bDeclare && aClassID.Len() == 42 &&
        aClassID.EqualsAscii( "clsid:", 0, 6 ) )
    {
        aClassID.Erase( 0, 6 );
        SvGlobalName aCID;
        if( aCID.MakeId( aClassID ) )
        {
            SvGlobalName aJavaCID( 0x8AD9C840UL, 0x044EU, 0x11D1U, 0xB3U, 0xE9U,
                                   0x00U, 0x80U, 0x5FU, 0x49U, 0x9DU, 0x93U );

            bIsApplet = aJavaCID == aCID;
        }
    }

    if( !bIsApplet )
    {
        delete pAppletImpl;
        pAppletImpl = 0;
        return;
    }

    pAppletImpl->SetAltText( aStandBy );

    SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aPropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
        ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo );

    SfxItemSet& rFrmSet = pAppletImpl->GetItemSet();
    if( !IsNewDoc() )
        Reader::ResetFrmFmtAttrs( rFrmSet );

    // den Anker und die Ausrichtung setzen
    SetAnchorAndAdjustment( eVertOri, eHoriOri, aItemSet, aPropInfo, rFrmSet );

    // und noch die Groesse des Rahmens
    Size aDfltSz( HTML_DFLT_APPLET_WIDTH, HTML_DFLT_APPLET_HEIGHT );
    SetFixSize( aSize, aDfltSz, bPrcWidth, bPrcHeight, aItemSet, aPropInfo,
                rFrmSet );
    SetSpace( aSpace, aItemSet, aPropInfo, rFrmSet );
}
#endif

void SwHTMLParser::EndObject()
{
#ifdef SOLAR_JAVA
    if( !pAppletImpl )
        return;
    if( pAppletImpl->CreateApplet( sBaseURL ) )
    {
        pAppletImpl->FinishApplet();

        // und in das Dok einfuegen
        SwFrmFmt* pFlyFmt =
            pDoc->Insert( *pPam,
                    ::svt::EmbeddedObjectRef( pAppletImpl->GetApplet(), embed::Aspects::MSOLE_CONTENT ),
                    &pAppletImpl->GetItemSet(),
                    NULL,
                    NULL );

        // den alternativen Namen setzen
        SwNoTxtNode *pNoTxtNd =
            pDoc->GetNodes()[ pFlyFmt->GetCntnt().GetCntntIdx()
                              ->GetIndex()+1 ]->GetNoTxtNode();
        pNoTxtNd->SetTitle( pAppletImpl->GetAltText() );

        // Ggf Frames anlegen und auto-geb. Rahmen registrieren
        RegisterFlyFrm( pFlyFmt );

        delete pAppletImpl;
        pAppletImpl = 0;
    }
#endif
}

#ifdef SOLAR_JAVA
void SwHTMLParser::InsertApplet()
{
    String aCodeBase, aCode, aName, aAlt, aId, aStyle, aClass;
    Size aSize( USHRT_MAX, USHRT_MAX );
    Size aSpace( 0, 0 );
    BOOL bPrcWidth = FALSE, bPrcHeight = FALSE, bMayScript = FALSE;
    sal_Int16 eVertOri = text::VertOrientation::TOP;
    sal_Int16 eHoriOri = text::HoriOrientation::NONE;

    // Eine neue Command-List anlegen
    if( pAppletImpl )
        delete pAppletImpl;
    pAppletImpl = new SwApplet_Impl( pDoc->GetAttrPool(), RES_FRMATR_BEGIN, RES_FRMATR_END-1 );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( USHORT i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        switch( pOption->GetToken() )
        {
        case HTML_O_ID:
            aId = pOption->GetString();
            break;
        case HTML_O_STYLE:
            aStyle = pOption->GetString();
            break;
        case HTML_O_CLASS:
            aClass = pOption->GetString();
            break;
        case HTML_O_CODEBASE:
            aCodeBase = pOption->GetString();
            break;
        case HTML_O_CODE:
            aCode = pOption->GetString();
            break;
        case HTML_O_NAME:
            aName = pOption->GetString();
            break;
        case HTML_O_ALT:
            aAlt = pOption->GetString();
            break;
        case HTML_O_ALIGN:
            eVertOri = pOption->GetEnum( aHTMLImgVAlignTable, eVertOri );
            eHoriOri = pOption->GetEnum( aHTMLImgHAlignTable, eHoriOri );
            break;
        case HTML_O_WIDTH:
            bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
            aSize.Width() = (long)pOption->GetNumber();
            break;
        case HTML_O_HEIGHT:
            bPrcHeight = (pOption->GetString().Search('%') != STRING_NOTFOUND);
            aSize.Height() = (long)pOption->GetNumber();
            break;
        case HTML_O_HSPACE:
            aSpace.Width() = (long)pOption->GetNumber();
            break;
        case HTML_O_VSPACE:
            aSpace.Height() = (long)pOption->GetNumber();
            break;
        case HTML_O_MAYSCRIPT:
            bMayScript = TRUE;
            break;
        }

        // Es werden alle Parameter auch an das Applet weitergereicht
        pAppletImpl->AppendParam( pOption->GetTokenString(),
                                  pOption->GetString() );
    }

    if( !aCode.Len() )
    {
        delete pAppletImpl;
        pAppletImpl = 0;
        return;
    }

    if ( aCodeBase.Len() )
        aCodeBase = INetURLObject::GetAbsURL( sBaseURL, aCodeBase );
    pAppletImpl->CreateApplet( aCode, aName, bMayScript, aCodeBase, sBaseURL );//, aAlt );
    pAppletImpl->SetAltText( aAlt );

    SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aPropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
        ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo );

    SfxItemSet& rFrmSet = pAppletImpl->GetItemSet();
    if( !IsNewDoc() )
        Reader::ResetFrmFmtAttrs( rFrmSet );

    // den Anker und die Ausrichtung setzen
    SetAnchorAndAdjustment( eVertOri, eHoriOri, aItemSet, aPropInfo, rFrmSet );

    // und noch die Groesse des Rahmens
    Size aDfltSz( HTML_DFLT_APPLET_WIDTH, HTML_DFLT_APPLET_HEIGHT );
    SetFixSize( aSize, aDfltSz, bPrcWidth, bPrcHeight, aItemSet, aPropInfo,
                rFrmSet );
    SetSpace( aSpace, aItemSet, aPropInfo, rFrmSet );
}
#endif

void SwHTMLParser::EndApplet()
{
#ifdef SOLAR_JAVA
    if( !pAppletImpl )
        return;

    pAppletImpl->FinishApplet();

    // und in das Dok einfuegen
    SwFrmFmt* pFlyFmt =
        pDoc->Insert( *pPam,
                    ::svt::EmbeddedObjectRef( pAppletImpl->GetApplet(), embed::Aspects::MSOLE_CONTENT ),
                    &pAppletImpl->GetItemSet(),
                    NULL,
                    NULL );

    // den alternativen Namen setzen
    SwNoTxtNode *pNoTxtNd =
        pDoc->GetNodes()[ pFlyFmt->GetCntnt().GetCntntIdx()
                          ->GetIndex()+1 ]->GetNoTxtNode();
    pNoTxtNd->SetTitle( pAppletImpl->GetAltText() );

    // Ggf Frames anlegen und auto-geb. Rahmen registrieren
    RegisterFlyFrm( pFlyFmt );

    delete pAppletImpl;
    pAppletImpl = 0;
#endif
}

void SwHTMLParser::InsertParam()
{
#ifdef SOLAR_JAVA
    if( !pAppletImpl )
        return;

    String aName, aValue;

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( USHORT i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        switch( pOption->GetToken() )
        {
        case HTML_O_NAME:
            aName = pOption->GetString();
            break;
        case HTML_O_VALUE:
            aValue = pOption->GetString();
            break;
        }
    }

    if( !aName.Len() )
        return;

    pAppletImpl->AppendParam( aName, aValue );
#endif
}


/*  */

void SwHTMLParser::InsertFloatingFrame()
{
    String aAlt, aId, aStyle, aClass;
    Size aSize( USHRT_MAX, USHRT_MAX );
    Size aSpace( 0, 0 );
    BOOL bPrcWidth = FALSE, bPrcHeight = FALSE;
    sal_Int16 eVertOri = text::VertOrientation::TOP;
    sal_Int16 eHoriOri = text::HoriOrientation::NONE;

    const HTMLOptions *pHTMLOptions = GetOptions();

    // Erstmal die Optionen f?r das Writer-Frame-Format holen
    USHORT nArrLen = pHTMLOptions->Count();
    for ( USHORT i=0; i<nArrLen; i++ )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[i];
        switch( pOption->GetToken() )
        {
        case HTML_O_ID:
            aId = pOption->GetString();
            break;
        case HTML_O_STYLE:
            aStyle = pOption->GetString();
            break;
        case HTML_O_CLASS:
            aClass = pOption->GetString();
            break;
        case HTML_O_ALT:
            aAlt = pOption->GetString();
            break;
        case HTML_O_ALIGN:
            eVertOri = pOption->GetEnum( aHTMLImgVAlignTable, eVertOri );
            eHoriOri = pOption->GetEnum( aHTMLImgHAlignTable, eHoriOri );
            break;
        case HTML_O_WIDTH:
            bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
            aSize.Width() = (long)pOption->GetNumber();
            break;
        case HTML_O_HEIGHT:
            bPrcHeight = (pOption->GetString().Search('%') != STRING_NOTFOUND);
            aSize.Height() = (long)pOption->GetNumber();
            break;
        case HTML_O_HSPACE:
            aSpace.Width() = (long)pOption->GetNumber();
            break;
        case HTML_O_VSPACE:
            aSpace.Height() = (long)pOption->GetNumber();
            break;
        }
    }

    // und jetzt die fuer den SfxFrame
    SfxFrameDescriptor aFrameDesc;

    SfxFrameHTMLParser::ParseFrameOptions( &aFrameDesc, pHTMLOptions, sBaseURL );

    // den Floating-Frame anlegen
    comphelper::EmbeddedObjectContainer aCnt;
    ::rtl::OUString aObjName;
    uno::Reference < embed::XEmbeddedObject > xObj = aCnt.CreateEmbeddedObject( SvGlobalName( SO3_IFRAME_CLASSID ).GetByteSequence(), aObjName );

    //pFrame->EnableSetModified( FALSE );
    try
    {
        // TODO/MBA: testing
        if ( svt::EmbeddedObjectRef::TryRunningState( xObj ) )
        {
            uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
            if ( xSet.is() )
            {
                ::rtl::OUString aName = aFrameDesc.GetName();
                ScrollingMode eScroll = aFrameDesc.GetScrollingMode();
                sal_Bool bHasBorder = aFrameDesc.HasFrameBorder();
                Size aMargin = aFrameDesc.GetMargin();

                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameURL"), uno::makeAny( ::rtl::OUString( aFrameDesc.GetURL().GetMainURL( INetURLObject::NO_DECODE ) ) ) );
                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameName"), uno::makeAny( aName ) );

                if ( eScroll == ScrollingAuto )
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsAutoScroll"),
                        uno::makeAny( sal_True ) );
                else
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsScrollingMode"),
                        uno::makeAny( (sal_Bool) ( eScroll == ScrollingYes) ) );

                //if ( aFrmDescr.IsFrameBorderSet() )
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsBorder"),
                        uno::makeAny( bHasBorder ) );
                /*else
                    xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameIsAutoBorder"),
                        uno::makeAny( sal_True ) );*/

                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameMarginWidth"),
                    uno::makeAny( sal_Int32( aMargin.Width() ) ) );

                xSet->setPropertyValue( ::rtl::OUString::createFromAscii("FrameMarginHeight"),
                    uno::makeAny( sal_Int32( aMargin.Height() ) ) );
            }
        }
    }
    catch ( uno::Exception& )
    {
    }

    //pFrame->EnableSetModified( TRUE );

    SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aPropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
        ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo );

    // den Itemset holen
    SfxItemSet aFrmSet( pDoc->GetAttrPool(),
                        RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
    if( !IsNewDoc() )
        Reader::ResetFrmFmtAttrs( aFrmSet );

    // den Anker und die Ausrichtung setzen
    SetAnchorAndAdjustment( eVertOri, eHoriOri, aItemSet, aPropInfo, aFrmSet );

    // und noch die Groesse des Rahmens
    Size aDfltSz( HTML_DFLT_APPLET_WIDTH, HTML_DFLT_APPLET_HEIGHT );
    SetFixSize( aSize, aDfltSz, bPrcWidth, bPrcHeight, aItemSet, aPropInfo,
                aFrmSet );
    SetSpace( aSpace, aItemSet, aPropInfo, aFrmSet );

    // und in das Dok einfuegen
    SwFrmFmt* pFlyFmt =
        pDoc->Insert( *pPam, ::svt::EmbeddedObjectRef( xObj, embed::Aspects::MSOLE_CONTENT ), &aFrmSet, NULL, NULL );

    // den alternativen Namen setzen
    SwNoTxtNode *pNoTxtNd =
        pDoc->GetNodes()[ pFlyFmt->GetCntnt().GetCntntIdx()
                          ->GetIndex()+1 ]->GetNoTxtNode();
    pNoTxtNd->SetTitle( aAlt );

    // Ggf Frames anlegen und auto-geb. Rahmen registrieren
    RegisterFlyFrm( pFlyFmt );

    bInFloatingFrame = TRUE;
}

/*  */

/*
#define SWHTML_OPTTYPE_IGNORE 0
#define SWHTML_OPTTYPE_TAG 1
#define SWHTML_OPTTYPE_PARAM 2


static USHORT GetOptionType( const String& rName, BOOL bApplet )
{
    USHORT nType = bApplet ? SWHTML_OPTTYPE_PARAM : SWHTML_OPTTYPE_TAG;

    switch( rName.GetChar(0) )
    {
    case 'A':
    case 'a':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_align ) ||
            rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_alt ) )
            nType = SWHTML_OPTTYPE_IGNORE;
        else if( bApplet &&
                 (rName.EqualsIgnoreCaseAscii( sHTML_O_archive ) ||
                 rName.EqualsIgnoreCaseAscii( sHTML_O_Archives )) )
            nType = SWHTML_OPTTYPE_TAG;
        break;
    case 'C':
    case 'c':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_class ) ||
            (bApplet && (rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_code ) ||
                         rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_codebase ))) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'H':
    case 'h':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_height ) ||
            rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_hspace ) ||
            (!bApplet && rName.EqualsIgnoreCaseAscii( OOO_STRING_SW_HTML_O_Hidden )) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'I':
    case 'i':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_id ) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'M':
    case 'm':
        if( bApplet && rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_mayscript ) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'N':
    case 'n':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_name ) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'O':
    case 'o':
        if( bApplet && rName.EqualsIgnoreCaseAscii( sHTML_O_Object ) )
            nType = SWHTML_OPTTYPE_TAG;
        break;
    case 'S':
    case 's':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_style ) ||
            (!bApplet && rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_src )) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'T':
    case 't':
        if( !bApplet && rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_type ) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'V':
    case 'v':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_vspace ) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    case 'W':
    case 'w':
        if( rName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_O_width ) )
            nType = SWHTML_OPTTYPE_IGNORE;
        break;
    }

    return nType;
}
*/

USHORT SwHTMLWriter::GuessOLENodeFrmType( const SwNode& rNode )
{
    SwOLEObj& rObj = ((SwOLENode*)rNode.GetOLENode())->GetOLEObj();

    SwHTMLFrmType eType = HTML_FRMTYPE_OLE;

    uno::Reference < embed::XClassifiedObject > xClass ( rObj.GetOleRef(), uno::UNO_QUERY );
    SvGlobalName aClass( xClass->getClassID() );
    if( aClass == SvGlobalName( SO3_PLUGIN_CLASSID ) )
    {
        eType = HTML_FRMTYPE_PLUGIN;
    }
    else if( aClass == SvGlobalName( SO3_IFRAME_CLASSID ) )
    {
        eType = HTML_FRMTYPE_IFRAME;
    }
#ifdef SOLAR_JAVA
    else if( aClass == SvGlobalName( SO3_APPLET_CLASSID ) )
    {
        eType = HTML_FRMTYPE_APPLET;
    }
#endif

    return static_cast< USHORT >(eType);
}

Writer& OutHTML_FrmFmtOLENode( Writer& rWrt, const SwFrmFmt& rFrmFmt,
                               BOOL bInCntnr )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;

    const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
    ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex()+1;
    SwOLENode *pOLENd = rHTMLWrt.pDoc->GetNodes()[ nStt ]->GetOLENode();

    ASSERT( pOLENd, "OLE-Node erwartet" );
    if( !pOLENd )
        return rWrt;

    SwOLEObj &rObj = pOLENd->GetOLEObj();

    uno::Reference < embed::XEmbeddedObject > xObj( rObj.GetOleRef() );
    if ( !svt::EmbeddedObjectRef::TryRunningState( xObj ) )
        return rWrt;

    uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
    BOOL bHiddenEmbed = FALSE;

    if( !xSet.is() )
    {
        DBG_ERROR("Unknown Object" );
        return rWrt;
    }

    ByteString aEndTags;
    ULONG nFrmOpts;

    // wenn meoglich vor dem "Objekt" einen Zeilen-Umbruch ausgeben
    if( rHTMLWrt.bLFPossible )
        rHTMLWrt.OutNewLine( TRUE );

    if( rFrmFmt.GetName().Len() )
        rHTMLWrt.OutImplicitMark( rFrmFmt.GetName(),
                                  pMarkToOLE );
    uno::Any aAny;
    SvGlobalName aGlobName( xObj->getClassID() );
    ByteString sOut('<');
    if( aGlobName == SvGlobalName( SO3_PLUGIN_CLASSID ) )
    {
        // erstmal das Plug-spezifische
        sOut += OOO_STRING_SVTOOLS_HTML_embed;

        ::rtl::OUString aStr;
        String aURL;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("PluginURL" ) );
        if( (aAny >>= aStr) && aStr.getLength() )
        {
            aURL = URIHelper::simpleNormalizedMakeRelative( rWrt.GetBaseURL(),
                      aStr);
        }

        if( aURL.Len() )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_src) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rWrt.Strm(), aURL, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            sOut = '\"';
        }

        ::rtl::OUString aType;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("PluginMimeType" ) );
        if( (aAny >>= aType) && aType.getLength() )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_type) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rWrt.Strm(), aType, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            sOut = '\"';
        }

        if ((FLY_AT_PARA == rFrmFmt.GetAnchor().GetAnchorId()) &&
            SURROUND_THROUGHT == rFrmFmt.GetSurround().GetSurround() )
        {
            // Das Plugin ist HIDDEN
            (sOut += ' ') += OOO_STRING_SW_HTML_O_Hidden;
            nFrmOpts = HTML_FRMOPTS_HIDDEN_EMBED;
            bHiddenEmbed = TRUE;
        }
        else
        {
            nFrmOpts = bInCntnr ? HTML_FRMOPTS_EMBED_CNTNR
                                : HTML_FRMOPTS_EMBED;
        }
    }
    else if( aGlobName == SvGlobalName( SO3_APPLET_CLASSID ) )
    {
        // oder das Applet-Spezifische

        sOut += OOO_STRING_SVTOOLS_HTML_applet;

        // CODEBASE
        ::rtl::OUString aCd;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("AppletCodeBase" ) );
        if( (aAny >>= aCd) && aCd.getLength() )
        {
            String sCodeBase( URIHelper::simpleNormalizedMakeRelative(rWrt.GetBaseURL(), aCd) );
            if( sCodeBase.Len() )
            {
                ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_codebase) += "=\"";
                rWrt.Strm() << sOut.GetBuffer();
                HTMLOutFuncs::Out_String( rWrt.Strm(), sCodeBase, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
                sOut = '\"';
            }
        }

        // CODE
        ::rtl::OUString aClass;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("AppletCode" ) );
        aAny >>= aClass;
        ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_code) += "=\"";
        rWrt.Strm() << sOut.GetBuffer();
        HTMLOutFuncs::Out_String( rWrt.Strm(), aClass, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
        sOut = '\"';

        // NAME
        ::rtl::OUString aAppletName;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("AppletName" ) );
        aAny >>= aAppletName;
        if( aAppletName.getLength() )
        {
            ((sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_name) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rWrt.Strm(), aAppletName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            sOut = '\"';
        }

        sal_Bool bScript = sal_False;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("AppletIsScript" ) );
        aAny >>= bScript;
        if( bScript )
            (sOut += ' ') += OOO_STRING_SVTOOLS_HTML_O_mayscript;

        nFrmOpts = bInCntnr ? HTML_FRMOPTS_APPLET_CNTNR
                            : HTML_FRMOPTS_APPLET;
    }
    else
    {
        // oder das Flating-Frame spezifische

        sOut += OOO_STRING_SVTOOLS_HTML_iframe;
        rWrt.Strm() << sOut.GetBuffer();

        SfxFrameHTMLWriter::Out_FrameDescriptor( rWrt.Strm(), rWrt.GetBaseURL(),
                                        xSet,
                                        rHTMLWrt.eDestEnc,
                                        &rHTMLWrt.aNonConvertableCharacters );
        sOut.Erase();

        nFrmOpts = bInCntnr ? HTML_FRMOPTS_IFRAME_CNTNR
                            : HTML_FRMOPTS_IFRAME;
    }

    rWrt.Strm() << sOut.GetBuffer();

    // ALT, WIDTH, HEIGHT, HSPACE, VSPACE, ALIGN
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_ABS_POS_FLY ) && !bHiddenEmbed )
        nFrmOpts |= HTML_FRMOPTS_OLE_CSS1;
    rHTMLWrt.OutFrmFmtOptions( rFrmFmt, pOLENd->GetTitle(),
                               aEndTags, nFrmOpts );
    if( rHTMLWrt.IsHTMLMode( HTMLMODE_ABS_POS_FLY ) && !bHiddenEmbed )
        rHTMLWrt.OutCSS1_FrmFmtOptions( rFrmFmt, nFrmOpts );

    if( aGlobName == SvGlobalName( SO3_APPLET_CLASSID ) )
    {
        // fuer Applets die Parameter als eigene Tags ausgeben
        // und ein </APPLET> schreiben

        uno::Sequence < beans::PropertyValue > aProps;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("AppletCommands" ) );
        aAny >>= aProps;

        SvCommandList aCommands;
        aCommands.FillFromSequence( aProps );
        SvULongs aParams;
        ULONG i = aCommands.Count();
        while( i > 0 )
        {
            const SvCommand& rCommand = aCommands[ --i ];
            const String& rName = rCommand.GetCommand();
            USHORT nType = SwApplet_Impl::GetOptionType( rName, TRUE );
            if( SWHTML_OPTTYPE_TAG == nType )
            {
                const String& rValue = rCommand.GetArgument();
                rWrt.Strm() << ' ';
                HTMLOutFuncs::Out_String( rWrt.Strm(), rName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
                rWrt.Strm() << "=\"";
                HTMLOutFuncs::Out_String( rWrt.Strm(), rValue, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters ) << '\"';
            }
            else if( SWHTML_OPTTYPE_PARAM == nType )
            {
                aParams.Insert( i, aParams.Count() );
            }
        }

        rHTMLWrt.Strm() << '>';

        rHTMLWrt.IncIndentLevel(); // Inhalt von Applet einruecken

        USHORT ii = aParams.Count();
        while( ii > 0  )
        {
            const SvCommand& rCommand = aCommands[ aParams[--ii] ];
            const String& rName = rCommand.GetCommand();
            const String& rValue = rCommand.GetArgument();
            rHTMLWrt.OutNewLine();
            ((((sOut = '<') += OOO_STRING_SVTOOLS_HTML_param) += ' ') += OOO_STRING_SVTOOLS_HTML_O_name)
                += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rWrt.Strm(), rName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
            ((sOut = "\" ") += OOO_STRING_SVTOOLS_HTML_O_value) += "=\"";
            rWrt.Strm() << sOut.GetBuffer();
            HTMLOutFuncs::Out_String( rWrt.Strm(), rValue, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters ) << "\">";
        }

        rHTMLWrt.DecIndentLevel(); // Inhalt von Applet einruecken
        if( aCommands.Count() )
            rHTMLWrt.OutNewLine();
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_applet, FALSE );
    }
    else
    if( aGlobName == SvGlobalName( SO3_PLUGIN_CLASSID ) )
    {
        // fuer Plugins die Paramater als Optionen schreiben

        uno::Sequence < beans::PropertyValue > aProps;
        aAny = xSet->getPropertyValue( ::rtl::OUString::createFromAscii("PluginCommands" ) );
        aAny >>= aProps;

        SvCommandList aCommands;
        aCommands.FillFromSequence( aProps );
        for( ULONG i=0; i<aCommands.Count(); i++ )
        {
            const SvCommand& rCommand = aCommands[ i ];
            const String& rName = rCommand.GetCommand();

            if( SwApplet_Impl::GetOptionType( rName, FALSE ) == SWHTML_OPTTYPE_TAG )
            {
                const String& rValue = rCommand.GetArgument();
                rWrt.Strm() << ' ';
                HTMLOutFuncs::Out_String( rWrt.Strm(), rName, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters );
                rWrt.Strm() << "=\"";
                HTMLOutFuncs::Out_String( rWrt.Strm(), rValue, rHTMLWrt.eDestEnc, &rHTMLWrt.aNonConvertableCharacters ) << '\"';
            }
        }
        rHTMLWrt.Strm() << '>';
    }
    else
    {
        // und fuer Floating-Frames einfach noch ein </IFRAME>
        // ausgeben

        rHTMLWrt.Strm() << '>';
        HTMLOutFuncs::Out_AsciiTag( rWrt.Strm(), OOO_STRING_SVTOOLS_HTML_iframe, FALSE );
    }

    if( aEndTags.Len() )
        rWrt.Strm() << aEndTags.GetBuffer();

    return rWrt;
}

Writer& OutHTML_FrmFmtOLENodeGrf( Writer& rWrt, const SwFrmFmt& rFrmFmt,
                                  BOOL bInCntnr )
{
    SwHTMLWriter& rHTMLWrt = (SwHTMLWriter&)rWrt;

    const SwFmtCntnt& rFlyCntnt = rFrmFmt.GetCntnt();
    ULONG nStt = rFlyCntnt.GetCntntIdx()->GetIndex()+1;
    SwOLENode *pOLENd = rHTMLWrt.pDoc->GetNodes()[ nStt ]->GetOLENode();

    ASSERT( pOLENd, "OLE-Node erwartet" );
    if( !pOLENd )
        return rWrt;

    // Inhalt des Nodes als Grafik speichern
    //uno::Reference < embed::XEmbeddedObject > xObj = pOLENd->GetOLEObj().GetOleRef();
    //GDIMetaFile aPic;
    //if( xObj.is() && xRef->GetGDIMetaFile( aPic ).GetActionCount() )
    {
        //Graphic aGrf( aPic );
        Graphic aGrf( *pOLENd->GetGraphic() );
        String aGrfNm;
        const String* pTempFileName = rHTMLWrt.GetOrigFileName();
        if(pTempFileName)
            aGrfNm = *pTempFileName;

        USHORT nErr = XOutBitmap::WriteGraphic( aGrf, aGrfNm,
                                    String::CreateFromAscii("JPG"),
                                    (XOUTBMP_USE_GIF_IF_POSSIBLE |
                                     XOUTBMP_USE_NATIVE_IF_POSSIBLE) );
        if( nErr )     			// fehlerhaft, da ist nichts auszugeben
        {
            rHTMLWrt.nWarn = WARN_SWG_POOR_LOAD | WARN_SW_WRITE_BASE;
            return rWrt;
        }
        aGrfNm = URIHelper::SmartRel2Abs(
            INetURLObject(rWrt.GetBaseURL()), aGrfNm,
            URIHelper::GetMaybeFileHdl() );
        ULONG nFlags = bInCntnr ? HTML_FRMOPTS_GENIMG_CNTNR
                                  : HTML_FRMOPTS_GENIMG;
        OutHTML_Image( rWrt, rFrmFmt, aGrfNm,
                       pOLENd->GetTitle(), pOLENd->GetTwipSize(),
                       nFlags, pMarkToOLE );
    }

    return rWrt;
}


