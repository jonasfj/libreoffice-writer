/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: htmlform.cxx,v $
 * $Revision: 1.27.94.1 $
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


#include <hintids.hxx>
#include <vcl/svapp.hxx>
#ifndef _WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#include <vcl/imgcons.hxx>

#ifndef _TOOLKIT_UNOHLP_HXX
#include <toolkit/helper/vclunohelper.hxx>
#endif
#include <svtools/htmlkywd.hxx>
#include <svtools/htmltokn.h>
#include <svl/urihelper.hxx>
#include <sfx2/request.hxx>
#include <sfx2/docfile.hxx>
#ifndef _SFXAPP_HXX
#include <sfx2/viewfrm.hxx>
#endif
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/brshitem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/crsditem.hxx>
#include <svx/svdouno.hxx>
#include <svx/fmglob.hxx>
#include <com/sun/star/form/ListSourceType.hpp>
#include <com/sun/star/form/FormButtonType.hpp>
#include <com/sun/star/form/FormSubmitEncoding.hpp>
#include <com/sun/star/form/FormSubmitMethod.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/script/XEventAttacherManager.hpp>
#include <com/sun/star/text/WrapTextMode.hpp>
#include <com/sun/star/text/HoriOrientation.hpp>
#include <com/sun/star/text/VertOrientation.hpp>
#include <com/sun/star/text/TextContentAnchorType.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/drawing/XControlShape.hpp>
#include <com/sun/star/awt/XTextLayoutConstrains.hpp>
#include <com/sun/star/awt/XLayoutConstrains.hpp>
#include <doc.hxx>
#include <pam.hxx>
#include <swtable.hxx>
#include <fmtanchr.hxx>
#include <htmltbl.hxx>
#include <docsh.hxx>
#include <viewsh.hxx>
#include <unodraw.hxx>
#include <unoobj.hxx>
#include "dcontact.hxx"

#include "swcss1.hxx"
#include "swhtml.hxx"
#include "htmlform.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::form;
using ::rtl::OUString;

const sal_uInt16 TABINDEX_MIN = 0;
const sal_uInt16 TABINDEX_MAX = 32767;

static HTMLOptionEnum __FAR_DATA aHTMLFormMethodTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_METHOD_get,		FormSubmitMethod_GET	},
    { OOO_STRING_SVTOOLS_HTML_METHOD_post,	FormSubmitMethod_POST	},
    { 0,					0						}
};

static HTMLOptionEnum __FAR_DATA aHTMLFormEncTypeTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_ET_url,			FormSubmitEncoding_URL			},
    { OOO_STRING_SVTOOLS_HTML_ET_multipart,	FormSubmitEncoding_MULTIPART	},
    { OOO_STRING_SVTOOLS_HTML_ET_text,		FormSubmitEncoding_TEXT			},
    { 0,					0								}
};

enum HTMLWordWrapMode { HTML_WM_OFF, HTML_WM_HARD, HTML_WM_SOFT };

static HTMLOptionEnum __FAR_DATA aHTMLTextAreaWrapTable[] =
{
    { OOO_STRING_SVTOOLS_HTML_WW_off,		HTML_WM_OFF	},
    { OOO_STRING_SVTOOLS_HTML_WW_hard,	HTML_WM_HARD	},
    { OOO_STRING_SVTOOLS_HTML_WW_soft,	HTML_WM_SOFT	},
    { OOO_STRING_SVTOOLS_HTML_WW_physical,HTML_WM_HARD	},
    { OOO_STRING_SVTOOLS_HTML_WW_virtual,	HTML_WM_SOFT	},
    { 0,				0			   	}
};

HTMLEventType __FAR_DATA aEventTypeTable[] =
{
    HTML_ET_ONSUBMITFORM,
    HTML_ET_ONRESETFORM,
    HTML_ET_ONGETFOCUS,
    HTML_ET_ONLOSEFOCUS,
    HTML_ET_ONCLICK,
    HTML_ET_ONCLICK_ITEM,
    HTML_ET_ONCHANGE,
    HTML_ET_ONSELECT,
    HTML_ET_END
};

const sal_Char * __FAR_DATA aEventListenerTable[] =
{
    "XSubmitListener",
    "XResetListener",
    "XFocusListener",
    "XFocusListener",
    "XApproveActionListener",
    "XItemListener",
    "XChangeListener",
    ""
};

const sal_Char * __FAR_DATA aEventMethodTable[] =
{
    "approveSubmit",
    "approveReset",
    "focusGained",
    "focusLost",
    "approveAction",
    "itemStateChanged",
    "changed",
    ""
};

const sal_Char * __FAR_DATA aEventSDOptionTable[] =
{
    OOO_STRING_SVTOOLS_HTML_O_SDonsubmit,
    OOO_STRING_SVTOOLS_HTML_O_SDonreset,
    OOO_STRING_SVTOOLS_HTML_O_SDonfocus,
    OOO_STRING_SVTOOLS_HTML_O_SDonblur,
    OOO_STRING_SVTOOLS_HTML_O_SDonclick,
    OOO_STRING_SVTOOLS_HTML_O_SDonclick,
    OOO_STRING_SVTOOLS_HTML_O_SDonchange,
    0
};

const sal_Char * __FAR_DATA aEventOptionTable[] =
{
    OOO_STRING_SVTOOLS_HTML_O_onsubmit,
    OOO_STRING_SVTOOLS_HTML_O_onreset,
    OOO_STRING_SVTOOLS_HTML_O_onfocus,
    OOO_STRING_SVTOOLS_HTML_O_onblur,
    OOO_STRING_SVTOOLS_HTML_O_onclick,
    OOO_STRING_SVTOOLS_HTML_O_onclick,
    OOO_STRING_SVTOOLS_HTML_O_onchange,
    0
};

/*  */

class SwHTMLForm_Impl
{
    SwDocShell 					*pDocSh;

    SvKeyValueIterator			*pHeaderAttrs;

    // gecachte Interfaces
    uno::Reference< drawing::XDrawPage >  			xDrawPage;
    uno::Reference< container::XIndexContainer > 	xForms;
    uno::Reference< drawing::XShapes >   			xShapes;
    uno::Reference< XMultiServiceFactory > 			xServiceFactory;

    uno::Reference< script::XEventAttacherManager > 	xControlEventManager;
    uno::Reference< script::XEventAttacherManager > 	xFormEventManager;

    // Kontext-Informationen
    uno::Reference< container::XIndexContainer > 	xFormComps;
    uno::Reference< beans::XPropertySet > 			xFCompPropSet;
    uno::Reference< drawing::XShape > 				xShape;

    String						sText;
    SvStringsDtor				aStringList;
    SvStringsDtor				aValueList;
    SvUShorts					aSelectedList;

public:

    SwHTMLForm_Impl( SwDocShell *pDSh ) :
        pDocSh( pDSh ),
        pHeaderAttrs( pDSh ? pDSh->GetHeaderAttributes() : 0 )
    {
        ASSERT( pDocSh, "Keine DocShell, keine Controls" );
    }

    const uno::Reference< XMultiServiceFactory >& GetServiceFactory();
    const uno::Reference< drawing::XDrawPage >& GetDrawPage();
    const uno::Reference< drawing::XShapes >& GetShapes();
    const uno::Reference< script::XEventAttacherManager >& GetControlEventManager();
    const uno::Reference< script::XEventAttacherManager >& GetFormEventManager();
    const uno::Reference< container::XIndexContainer >& GetForms();

    const uno::Reference< container::XIndexContainer >& GetFormComps() const
    {
        return xFormComps;
    }

    void SetFormComps( const uno::Reference< container::XIndexContainer >& r )
    {
        xFormComps = r;
    }

    void ReleaseFormComps() { xFormComps = 0; xControlEventManager = 0; }

    const uno::Reference< beans::XPropertySet >& GetFCompPropSet() const
    {
        return xFCompPropSet;
    }

    void SetFCompPropSet( const uno::Reference< beans::XPropertySet >& r )
    {
        xFCompPropSet = r;
    }

    void ReleaseFCompPropSet() { xFCompPropSet = 0; }

    const uno::Reference< drawing::XShape >& GetShape() const { return xShape; }
    void SetShape( const uno::Reference< drawing::XShape >& r ) { xShape = r; }
    void ReleaseShape() { xShape = 0; }

    String& GetText() { return sText; }
    void EraseText() { sText = aEmptyStr; }

    SvStringsDtor& GetStringList() { return aStringList; }
    void EraseStringList()
    {
        aStringList.DeleteAndDestroy( 0, aStringList.Count() );
    }

    SvStringsDtor& GetValueList() { return aValueList; }
    void EraseValueList()
    {
        aValueList.DeleteAndDestroy( 0, aValueList.Count() );
    }

    SvUShorts& GetSelectedList() { return aSelectedList; }
    void EraseSelectedList()
    {
        aSelectedList.Remove( 0, aSelectedList.Count() );
    }

    SvKeyValueIterator *GetHeaderAttrs() const { return pHeaderAttrs; }
};

const uno::Reference< XMultiServiceFactory >& SwHTMLForm_Impl::GetServiceFactory()
{
    if( !xServiceFactory.is() && pDocSh )
    {
        xServiceFactory =
            uno::Reference< XMultiServiceFactory >( pDocSh->GetBaseModel(),
                                               UNO_QUERY );
        ASSERT( xServiceFactory.is(),
                "XServiceFactory nicht vom Model erhalten" );
    }
    return xServiceFactory;
}


const uno::Reference< drawing::XDrawPage >& SwHTMLForm_Impl::GetDrawPage()
{
    if( !xDrawPage.is() && pDocSh )
    {
        uno::Reference< drawing::XDrawPageSupplier > xTxtDoc( pDocSh->GetBaseModel(),
                                                         UNO_QUERY );
        ASSERT( xTxtDoc.is(),
                "drawing::XDrawPageSupplier nicht vom XModel erhalten" );
        xDrawPage = xTxtDoc->getDrawPage();
        ASSERT( xDrawPage.is(), "drawing::XDrawPage nicht erhalten" );
    }
    return xDrawPage;
}

const uno::Reference< container::XIndexContainer >& SwHTMLForm_Impl::GetForms()
{
    if( !xForms.is() )
    {
        GetDrawPage();
        if( xDrawPage.is() )
        {
            uno::Reference< XFormsSupplier > xFormsSupplier( xDrawPage, UNO_QUERY );
            ASSERT( xFormsSupplier.is(),
                    "XFormsSupplier nicht vom drawing::XDrawPage erhalten" );

            uno::Reference< container::XNameContainer > xNameCont =
                xFormsSupplier->getForms();
            xForms = uno::Reference< container::XIndexContainer >( xNameCont,
                                                              UNO_QUERY );

            ASSERT( xForms.is(), "XForms nicht erhalten" );
        }
    }
    return xForms;
}


const uno::Reference< drawing::XShapes > & SwHTMLForm_Impl::GetShapes()
{
    if( !xShapes.is() )
    {
        GetDrawPage();
        if( xDrawPage.is() )
        {
            xShapes = uno::Reference< drawing::XShapes >( xDrawPage, UNO_QUERY );
            ASSERT( xShapes.is(),
                    "XShapes nicht vom drawing::XDrawPage erhalten" );
        }
    }
    return xShapes;
}

const uno::Reference< script::XEventAttacherManager >&
                                    SwHTMLForm_Impl::GetControlEventManager()
{
    if( !xControlEventManager.is() && xFormComps.is() )
    {
        xControlEventManager =
            uno::Reference< script::XEventAttacherManager >( xFormComps, UNO_QUERY );
        ASSERT( xControlEventManager.is(),
    "uno::Reference< XEventAttacherManager > nicht von xFormComps erhalten" );
    }

    return xControlEventManager;
}

const uno::Reference< script::XEventAttacherManager >&
    SwHTMLForm_Impl::GetFormEventManager()
{
    if( !xFormEventManager.is() )
    {
        GetForms();
        if( xForms.is() )
        {
            xFormEventManager =
                uno::Reference< script::XEventAttacherManager >( xForms, UNO_QUERY );
            ASSERT( xFormEventManager.is(),
        "uno::Reference< XEventAttacherManager > nicht von xForms erhalten" );
        }
    }

    return xFormEventManager;
}

class SwHTMLImageWatcher :
    public cppu::WeakImplHelper2< awt::XImageConsumer, XEventListener >
{
    uno::Reference< drawing::XShape >  		xShape;		// das control
    uno::Reference< XImageProducerSupplier >	xSrc;
    uno::Reference< awt::XImageConsumer > 	xThis;		// man selbst
    sal_Bool 							bSetWidth;
    sal_Bool							bSetHeight;

    void clear();

public:

    SwHTMLImageWatcher( const uno::Reference< drawing::XShape > & rShape,
                        sal_Bool bWidth, sal_Bool bHeight );
    ~SwHTMLImageWatcher();

    // startProduction darf nicht im Konstruktor gerufen werden, weil
    // wir und ggf. selbst zerstoeren ... Deshlab eine eigene Methode.
    void start() { xSrc->getImageProducer()->startProduction(); }

    // UNO Anbindung

    // XImageConsumer
    virtual void SAL_CALL init( sal_Int32 Width, sal_Int32 Height)
        throw( uno::RuntimeException );
    virtual void SAL_CALL setColorModel(
            sal_Int16 BitCount, const uno::Sequence< sal_Int32 >& RGBAPal,
            sal_Int32 RedMask, sal_Int32 GreenMask, sal_Int32 BlueMask,
            sal_Int32 AlphaMask)
        throw( uno::RuntimeException );
    virtual void SAL_CALL setPixelsByBytes(
            sal_Int32 X, sal_Int32 Y, sal_Int32 Width, sal_Int32 Height,
            const uno::Sequence< sal_Int8 >& ProducerData,
            sal_Int32 Offset, sal_Int32 Scansize)
        throw( uno::RuntimeException );
    virtual void SAL_CALL setPixelsByLongs(
            sal_Int32 X, sal_Int32 Y, sal_Int32 Width, sal_Int32 Height,
            const uno::Sequence< sal_Int32 >& ProducerData,
            sal_Int32 Offset, sal_Int32 Scansize)
        throw( uno::RuntimeException );
    virtual void SAL_CALL complete(
            sal_Int32 Status,
            const uno::Reference< awt::XImageProducer > & Producer)
        throw( uno::RuntimeException );

    // XEventListener
    virtual void SAL_CALL disposing( const EventObject& Source ) throw ( uno::RuntimeException);
};

SwHTMLImageWatcher::SwHTMLImageWatcher(
        const uno::Reference< drawing::XShape >& rShape,
        sal_Bool bWidth, sal_Bool bHeight ) :
    xShape( rShape ),
    bSetWidth( bWidth ), bSetHeight( bHeight )
{
    // Die Quelle des Images merken
    uno::Reference< drawing::XControlShape > xControlShape( xShape, UNO_QUERY );
    uno::Reference< awt::XControlModel > xControlModel(
            xControlShape->getControl() );
    xSrc = uno::Reference< XImageProducerSupplier >( xControlModel, UNO_QUERY );
    ASSERT( xSrc.is(), "Kein XImageProducerSupplier" );

    // Als Event-Listener am Shape anmelden, damit wir es beim dispose
    // loslassen ko�nnen ...
    uno::Reference< XEventListener > xEvtLstnr = (XEventListener *)this;
    uno::Reference< XComponent > xComp( xShape, UNO_QUERY );
    xComp->addEventListener( xEvtLstnr );

    // Zum Schluss halten wir noch eine Referenz auf uns selbst, damit
    // wir am Leben bleiben ... (eigentlich sollte das nicht neotig sein,
    // weil wir ja noch an diversen anderen Stellen angemeldet sind)
    xThis = (awt::XImageConsumer *)this;

    // und am ImageProducer anmelden, um die Groesse zu erehalten ...
    xSrc->getImageProducer()->addConsumer( xThis );
}

SwHTMLImageWatcher::~SwHTMLImageWatcher()
{
}

void SwHTMLImageWatcher::clear()
{
    // Am Shape als Event-Listener abmelden
    uno::Reference< XEventListener > xEvtLstnr = (XEventListener *)this;
    uno::Reference< XComponent > xComp( xShape, UNO_QUERY );
    xComp->removeEventListener( xEvtLstnr );

    // Am ImageProducer abmelden
    uno::Reference<awt::XImageProducer> xProd = xSrc->getImageProducer();
    if( xProd.is() )
        xProd->removeConsumer( xThis );
}

//------------------------------------------------------------------------------

void SwHTMLImageWatcher::init( sal_Int32 Width, sal_Int32 Height )
    throw( uno::RuntimeException )
{
    ASSERT( bSetWidth || bSetHeight,
            "Breite oder Hoehe muss angepasst werden" );

    // Wenn keine Breite oder Hoehe angegeben ist, ist das das init von
    // der leeren Grafik, die angezeigt wird, bevor der Stream einer
    // asynchron anzuzeigenden Grfik verfuegbar ist.
    if( !Width && !Height )
        return;

    awt::Size aNewSz;
    aNewSz.Width = Width;
    aNewSz.Height = Height;
    if( Application::GetDefaultDevice() )
    {
        Size aTmp(aNewSz.Width, aNewSz.Height);
        aTmp = Application::GetDefaultDevice()
                    ->PixelToLogic( aTmp, MapMode( MAP_100TH_MM ) );
        aNewSz.Width = aTmp.Width();
        aNewSz.Height = aTmp.Height();
    }

    if( !bSetWidth || !bSetHeight )
    {
        awt::Size aSz( xShape->getSize() );
        if( bSetWidth && aNewSz.Height )
        {
            aNewSz.Width *= aSz.Height;
            aNewSz.Width /= aNewSz.Height;
            aNewSz.Height = aSz.Height;
        }
        if( bSetHeight && aNewSz.Width )
        {
            aNewSz.Height *= aSz.Width;
            aNewSz.Height /= aNewSz.Width;
            aNewSz.Width = aSz.Width;
        }
    }
    if( aNewSz.Width < MINFLY )
        aNewSz.Width = MINFLY;
    if( aNewSz.Height < MINFLY )
        aNewSz.Height = MINFLY;

    xShape->setSize( aNewSz );
    if( bSetWidth )
    {
        // Wenn das Control in einer Tabelle verankert ist, muesen
        // die Tabellen-Spalten neu berechnet werden

        // Um an den SwXShape* zu gelangen, brauchen wir ein Interface,
        // das auch vom SwXShape implementiert wird.

        uno::Reference< beans::XPropertySet > xPropSet( xShape, UNO_QUERY );
        uno::Reference< XUnoTunnel> xTunnel( xPropSet, UNO_QUERY );
                SwXShape *pSwShape = xTunnel.is() ?
                    reinterpret_cast< SwXShape * >( sal::static_int_cast< sal_IntPtr>(
                    xTunnel->getSomething(SwXShape::getUnoTunnelId()) ))
                : 0;

        ASSERT( pSwShape, "Wo ist das SW-Shape?" );
        if( pSwShape )
        {
            SwFrmFmt *pFrmFmt = pSwShape->GetFrmFmt();

            const SwDoc *pDoc = pFrmFmt->GetDoc();
            const SwPosition* pAPos = pFrmFmt->GetAnchor().GetCntntAnchor();
            SwNode *pANd;
            SwTableNode *pTblNd;
            if( pAPos &&
                0 != (pANd = pDoc->GetNodes()[pAPos->nNode]) &&
                0 != (pTblNd = pANd->FindTableNode()) )
            {
                sal_Bool bLastGrf = !pTblNd->GetTable().DecGrfsThatResize();
                SwHTMLTableLayout *pLayout =
                    pTblNd->GetTable().GetHTMLTableLayout();
                if( pLayout )
                {
                    sal_uInt16 nBrowseWidth =
                        pLayout->GetBrowseWidthByTable( *pDoc );

                    if( nBrowseWidth )
                        pLayout->Resize( nBrowseWidth, sal_True, sal_True,
                                         bLastGrf ? HTMLTABLE_RESIZE_NOW
                                                  : 500 );
                }
            }
        }
    }

    // uns selbst abmelden und loeschen
    clear();
    uno::Reference< awt::XImageConsumer >  xTmp = (awt::XImageConsumer*)this;
    xThis = 0;
}

void SwHTMLImageWatcher::setColorModel(
        sal_Int16, const Sequence< sal_Int32 >&, sal_Int32, sal_Int32,
        sal_Int32, sal_Int32 )
    throw( uno::RuntimeException )
{
}

void SwHTMLImageWatcher::setPixelsByBytes(
        sal_Int32, sal_Int32, sal_Int32, sal_Int32,
        const Sequence< sal_Int8 >&, sal_Int32, sal_Int32 )
    throw( uno::RuntimeException )
{
}


void SwHTMLImageWatcher::setPixelsByLongs(
        sal_Int32, sal_Int32, sal_Int32, sal_Int32,
        const Sequence< sal_Int32 >&, sal_Int32, sal_Int32 )
    throw( uno::RuntimeException )
{
}


void SwHTMLImageWatcher::complete( sal_Int32 Status,
        const uno::Reference< awt::XImageProducer >& )
    throw( uno::RuntimeException )
{
    if( IMAGEERROR == Status || IMAGEABORTED == Status )
    {
        // uns selbst abmelden und loeschen
        clear();
        uno::Reference< awt::XImageConsumer > xTmp = (awt::XImageConsumer*)this;
        xThis = 0;
    }
}

void SwHTMLImageWatcher::disposing(const lang::EventObject& evt) throw ( uno::RuntimeException)
{
    uno::Reference< awt::XImageConsumer > xTmp;

    // Wenn das Shape verschwindet soll muessen wir es loslassen
    uno::Reference< drawing::XShape > xTmpShape;
    if( evt.Source == xShape )
    {
        clear();
        xTmp = (awt::XImageConsumer*)this;
        xThis = 0;
    }
}

void SwHTMLParser::DeleteFormImpl()
{
    delete pFormImpl;
    pFormImpl = 0;
}

static void lcl_html_setFixedFontProperty(
        const uno::Reference< beans::XPropertySet >& rPropSet )
{
    Font aFixedFont( OutputDevice::GetDefaultFont(
                                    DEFAULTFONT_FIXED, LANGUAGE_ENGLISH_US,
                                    DEFAULTFONT_FLAGS_ONLYONE )  );
    Any aTmp;
    aTmp <<= OUString( aFixedFont.GetName() );
    rPropSet->setPropertyValue( OUString::createFromAscii("FontName"), aTmp );

    aTmp <<= OUString( aFixedFont.GetStyleName() );
    rPropSet->setPropertyValue( OUString::createFromAscii("FontStyleName"),
                                aTmp );

    aTmp <<= (sal_Int16) aFixedFont.GetFamily();
    rPropSet->setPropertyValue( OUString::createFromAscii("FontFamily"), aTmp );

    aTmp <<= (sal_Int16) aFixedFont.GetCharSet();
    rPropSet->setPropertyValue( OUString::createFromAscii("FontCharset"),
                                aTmp );

    aTmp <<= (sal_Int16) aFixedFont.GetPitch();
    rPropSet->setPropertyValue( OUString::createFromAscii("FontPitch"), aTmp );

    float fVal(10.);
    aTmp.setValue( &fVal, ::getCppuType(&fVal ));
    rPropSet->setPropertyValue( OUString::createFromAscii("FontHeight"), aTmp );
}

class SwHTMLFormPendingStackData_Impl: public SwPendingStackData
{
    uno::Reference< drawing::XShape >  	xShape;
    Size			aTextSz;
    sal_Bool		bMinWidth;
    sal_Bool		bMinHeight;

public:

    SwHTMLFormPendingStackData_Impl(
            const uno::Reference< drawing::XShape > & rShape, const Size& rTextSz,
            sal_Bool bMinW, sal_Bool bMinH ) :
        xShape( rShape ),
        aTextSz( rTextSz ),
        bMinWidth( bMinW ),
        bMinHeight( bMinH )
    {}

    const uno::Reference< drawing::XShape >& GetShape() const { return xShape; }
    const Size& GetTextSize() const { return aTextSz; }
    sal_Bool IsMinWidth() const { return bMinWidth; }
    sal_Bool IsMinHeight() const { return bMinHeight; }
};

void SwHTMLParser::SetPendingControlSize( int nToken )
{
    ASSERT( pPendStack, "Wo ist der Pending Stack?" );
    SwHTMLFormPendingStackData_Impl *pData =
        (SwHTMLFormPendingStackData_Impl *)pPendStack->pData;

    SwPendingStack* pTmp = pPendStack->pNext;
    delete pPendStack;
    pPendStack = pTmp;
    ASSERT( !pPendStack, "Wo kommt der Pending-Stack her?" );

    SetControlSize( pData->GetShape(), pData->GetTextSize(),
                    pData->IsMinWidth(), pData->IsMinHeight(),
                    nToken );
    delete pData;
}

void SwHTMLParser::SetControlSize( const uno::Reference< drawing::XShape >& rShape,
                                   const Size& rTextSz,
                                   sal_Bool bMinWidth,
                                   sal_Bool bMinHeight,
                                   int nToken )
{
    nToken = 0;
    if( !rTextSz.Width() && !rTextSz.Height() && !bMinWidth  && !bMinHeight )
        return;

    // Um an den SwXShape* zu gelangen, brauchen wir ein Interface,
    // das auch vom SwXShape implementiert wird.
    uno::Reference< beans::XPropertySet > xPropSet( rShape, UNO_QUERY );

    ViewShell *pVSh;
    pDoc->GetEditShell( &pVSh );
    if( !pVSh && !nEventId )
    {
        // If there is no view shell by now and the doc shell is an internal
        // one, no view shell will be created. That for, we have to do that of
        // our own. This happens if a linked section is inserted or refreshed.
        SwDocShell *pDocSh = pDoc->GetDocShell();
        if( pDocSh )
        {
            if ( pDocSh->GetMedium() )
            {
                // if there is no hidden property in the MediaDescriptor it should be removed after loading
                SFX_ITEMSET_ARG( pDocSh->GetMedium()->GetItemSet(), pHiddenItem, SfxBoolItem, SID_HIDDEN, sal_False );
                bRemoveHidden = ( pHiddenItem == NULL || !pHiddenItem->GetValue() );
            }
                
            pTempViewFrame = SfxViewFrame::CreateViewFrame( *pDocSh, 0, sal_True );
            CallStartAction();
            pDoc->GetEditShell( &pVSh );
        }
    }

    uno::Reference< XUnoTunnel> xTunnel( xPropSet, UNO_QUERY );
    SwXShape *pSwShape = xTunnel.is() ?
        reinterpret_cast< SwXShape *>( sal::static_int_cast< sal_IntPtr >(
            xTunnel->getSomething(SwXShape::getUnoTunnelId()) ))
        : 0;

    ASSERT( pSwShape, "Wo ist das SW-Shape?" );

    // es muss ein Draw-Format sein
    SwFrmFmt *pFrmFmt = pSwShape->GetFrmFmt();
    ASSERT( RES_DRAWFRMFMT == pFrmFmt->Which(), "Kein DrawFrmFmt" );

    // Schauen, ob es ein SdrObject dafuer gibt
    const SdrObject *pObj = pFrmFmt->FindSdrObject();
    ASSERT( pObj, "SdrObject nicht gefunden" );
    ASSERT( FmFormInventor == pObj->GetObjInventor(), "falscher Inventor" );

    const SdrView* pDrawView = pVSh ? pVSh->GetDrawView() : 0;

    SdrUnoObj *pFormObj = PTR_CAST( SdrUnoObj, pObj );
    uno::Reference< awt::XControl > xControl;
    if ( pDrawView && pVSh->GetWin() )
        xControl = pFormObj->GetUnoControl( *pDrawView, *pVSh->GetWin() );

    awt::Size aSz( rShape->getSize() );
    awt::Size aNewSz( 0, 0 );

    // #i71248# ensure we got a XControl before apllying corrections
    if(xControl.is())
    {
        if( bMinWidth || bMinHeight )
        {
            uno::Reference< awt::XLayoutConstrains > xLC( xControl, UNO_QUERY );
            awt::Size aTmpSz( xLC->getPreferredSize() );
            if( bMinWidth )
                aNewSz.Width = aTmpSz.Width;
            if( bMinHeight )
                aNewSz.Height = aTmpSz.Height;
        }
        if( rTextSz.Width() || rTextSz.Height())
        {
            uno::Reference< awt::XTextLayoutConstrains > xLC( xControl, UNO_QUERY );
            ASSERT( xLC.is(), "kein XTextLayoutConstrains" );
            if( xLC.is() )
            {
                awt::Size aTmpSz( rTextSz.Width(), rTextSz.Height() );
                if( -1 == rTextSz.Width() )
                {
                    aTmpSz.Width = 0;
                    aTmpSz.Height = nSelectEntryCnt;
                }
                aTmpSz = xLC->getMinimumSize( static_cast< sal_Int16 >(aTmpSz.Width), static_cast< sal_Int16 >(aTmpSz.Height) );
                if( rTextSz.Width() )
                    aNewSz.Width = aTmpSz.Width;
                if( rTextSz.Height() )
                    aNewSz.Height = aTmpSz.Height;
            }
        }
    }

    if( Application::GetDefaultDevice() )
    {
        Size aTmpSz( aNewSz.Width, aNewSz.Height );
        aTmpSz = Application::GetDefaultDevice()
                        ->PixelToLogic( aTmpSz, MapMode( MAP_100TH_MM ) );
        aNewSz.Width  = aTmpSz.Width();
        aNewSz.Height = aTmpSz.Height();
    }
    if( aNewSz.Width )
    {
        if( aNewSz.Width < MINLAY )
            aNewSz.Width = MINLAY;
        aSz.Width = aNewSz.Width;
    }
    if( aNewSz.Height )
    {
        if( aNewSz.Height < MINLAY )
            aNewSz.Height = MINLAY;
        aSz.Height = aNewSz.Height;
    }

    rShape->setSize( aSz );
}

static void lcl_html_setEvents(
        const uno::Reference< script::XEventAttacherManager > & rEvtMn,
        sal_uInt32 nPos, const SvxMacroTableDtor& rMacroTbl,
        const SvStringsDtor& rUnoMacroTbl,
        const SvStringsDtor& rUnoMacroParamTbl,
        const String& rType )
{
    // Erstmal muss die Anzahl der Events ermittelt werden ...
    sal_Int32 nEvents = 0;
    sal_uInt16 i;

    for( i = 0; HTML_ET_END != aEventTypeTable[i]; i++ )
    {
        const SvxMacro *pMacro = rMacroTbl.Get( aEventTypeTable[i] );
        // Solange nicht alle Events implementiert sind, enthaelt die
        // Tabelle auch Leerstrings!
        if( pMacro && aEventListenerTable[i] )
            nEvents++;
    }
    for( i=0; i< rUnoMacroTbl.Count(); i++ )
    {
        const String& rStr = *rUnoMacroTbl[i];
        xub_StrLen nIndex = 0;
        if( !rStr.GetToken( 0, '-', nIndex ).Len() || STRING_NOTFOUND == nIndex )
            continue;
        if( !rStr.GetToken( 0, '-', nIndex ).Len() || STRING_NOTFOUND == nIndex )
            continue;
        if( nIndex < rStr.Len() )
            nEvents++;
    }

    if( 0==nEvents )
        return;

    Sequence<script::ScriptEventDescriptor> aDescs( nEvents );
    script::ScriptEventDescriptor* pDescs = aDescs.getArray();
    sal_Int32 nEvent = 0;

    for( i=0; HTML_ET_END != aEventTypeTable[i]; i++ )
    {
        const SvxMacro *pMacro = rMacroTbl.Get( aEventTypeTable[i] );
        if( pMacro && aEventListenerTable[i] )
        {
            script::ScriptEventDescriptor& rDesc = pDescs[nEvent++];
            rDesc.ListenerType =
                    OUString::createFromAscii(aEventListenerTable[i]);
            rDesc.EventMethod = OUString::createFromAscii(aEventMethodTable[i]);
            rDesc.ScriptType = pMacro->GetLanguage();
            rDesc.ScriptCode = pMacro->GetMacName();
        }
    }

    for( i=0; i< rUnoMacroTbl.Count(); i++ )
    {
        const String& rStr = *rUnoMacroTbl[i];
        xub_StrLen nIndex = 0;
        String sListener( rStr.GetToken( 0, '-', nIndex ) );
        if( !sListener.Len() || STRING_NOTFOUND == nIndex )
            continue;

        String sMethod( rStr.GetToken( 0, '-', nIndex ) );
        if( !sMethod.Len() || STRING_NOTFOUND == nIndex )
            continue;

        String sCode( rStr.Copy( nIndex ) );
        if( !sCode.Len() )
            continue;

        script::ScriptEventDescriptor& rDesc = pDescs[nEvent++];
        rDesc.ListenerType = sListener;
        rDesc.EventMethod = sMethod;
        rDesc.ScriptType = rType;
        rDesc.ScriptCode = sCode;
        rDesc.AddListenerParam = OUString();

        if( rUnoMacroParamTbl.Count() )
        {
            String sSearch( sListener );
            sSearch += '-';
            sSearch += sMethod;
            sSearch += '-';
            xub_StrLen nLen = sSearch.Len();
            for( sal_uInt16 j=0; j < rUnoMacroParamTbl.Count(); j++ )
            {
                const String& rParam = *rUnoMacroParamTbl[j];
                if( rParam.CompareTo( sSearch, nLen ) == COMPARE_EQUAL &&
                    rParam.Len() > nLen )
                {
                    rDesc.AddListenerParam = rParam.Copy(nLen);
                    break;
                }
            }
        }
    }
    rEvtMn->registerScriptEvents( nPos, aDescs );
}

static void lcl_html_getEvents( const String& rOption, const String& rValue,
                                SvStringsDtor& rUnoMacroTbl,
                                SvStringsDtor& rUnoMacroParamTbl )
{
    if( rOption.CompareIgnoreCaseToAscii( OOO_STRING_SVTOOLS_HTML_O_sdevent,
                            sizeof(OOO_STRING_SVTOOLS_HTML_O_sdevent)-1 ) == COMPARE_EQUAL )
    {
        String *pEvent = new String( rOption.Copy(sizeof(OOO_STRING_SVTOOLS_HTML_O_sdevent)-1) );
        *pEvent += '-';
        *pEvent += rValue;
        rUnoMacroTbl.Insert( pEvent, rUnoMacroTbl.Count() );
    }
    else if( rOption.CompareIgnoreCaseToAscii( OOO_STRING_SVTOOLS_HTML_O_sdaddparam,
                            sizeof(OOO_STRING_SVTOOLS_HTML_O_sdaddparam)-1 ) == COMPARE_EQUAL )
    {
        String *pParam =
                    new String( rOption.Copy( sizeof(OOO_STRING_SVTOOLS_HTML_O_sdaddparam)-1 ) );
        *pParam += '-';
        *pParam += rValue;
        rUnoMacroParamTbl.Insert( pParam, rUnoMacroParamTbl.Count() );
    }
}

uno::Reference< drawing::XShape > SwHTMLParser::InsertControl(
        const uno::Reference< XFormComponent > & rFComp,
        const uno::Reference< beans::XPropertySet > & rFCompPropSet,
        const Size& rSize, sal_Int16 eVertOri, sal_Int16 eHoriOri,
        SfxItemSet& rCSS1ItemSet, SvxCSS1PropertyInfo& rCSS1PropInfo,
        const SvxMacroTableDtor& rMacroTbl, const SvStringsDtor& rUnoMacroTbl,
        const SvStringsDtor& rUnoMacroParamTbl, sal_Bool bSetFCompPropSet,
        sal_Bool bHidden )
{
    uno::Reference< drawing::XShape >  xShape;

    const uno::Reference< container::XIndexContainer > & rFormComps =
        pFormImpl->GetFormComps();
    Any aAny( &rFComp, ::getCppuType( (uno::Reference< XFormComponent>*)0 ) );
    rFormComps->insertByIndex( rFormComps->getCount(), aAny );

    if( !bHidden )
    {
        Any aTmp;
        sal_uInt16 nLeftSpace = 0, nRightSpace = 0,
                      nUpperSpace = 0, nLowerSpace = 0;

        const uno::Reference< XMultiServiceFactory > & rServiceFactory =
            pFormImpl->GetServiceFactory();
        if( !rServiceFactory.is() )
            return xShape;

        uno::Reference< XInterface > xCreate =
            rServiceFactory ->createInstance(
                OUString::createFromAscii("com.sun.star.drawing.ControlShape"));
        if( !xCreate.is() )
            return xShape;

        xShape = uno::Reference< drawing::XShape >( xCreate, UNO_QUERY );

        DBG_ASSERT( xShape.is(), "XShape nicht erhalten" );
        awt::Size aTmpSz;
        aTmpSz.Width  = rSize.Width();
        aTmpSz.Height = rSize.Height();
        xShape->setSize( aTmpSz );

        uno::Reference< beans::XPropertySet > xShapePropSet( xCreate, UNO_QUERY );

        // linken/rechten Rand setzen
        const SfxPoolItem *pItem;
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_LR_SPACE, sal_True,
                                                     &pItem ) )
        {
            // Ggf. den Erstzeilen-Einzug noch plaetten
            const SvxLRSpaceItem *pLRItem = (const SvxLRSpaceItem *)pItem;
            SvxLRSpaceItem aLRItem( *pLRItem );
            aLRItem.SetTxtFirstLineOfst( 0 );
            if( rCSS1PropInfo.bLeftMargin )
            {
                nLeftSpace = static_cast< sal_uInt16 >(TWIP_TO_MM100( aLRItem.GetLeft() ));
                rCSS1PropInfo.bLeftMargin = sal_False;
            }
            if( rCSS1PropInfo.bRightMargin )
            {
                nRightSpace = static_cast< sal_uInt16 >(TWIP_TO_MM100( aLRItem.GetRight() ));
                rCSS1PropInfo.bRightMargin = sal_False;
            }
            rCSS1ItemSet.ClearItem( RES_LR_SPACE );
        }
        if( nLeftSpace || nRightSpace )
        {
            Any aAny2;
            aAny2 <<= (sal_Int32)nLeftSpace;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "LeftMargin" ), aAny2 );

            aAny2 <<= (sal_Int32)nRightSpace;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "RightMargin" ), aAny2 );
        }

        // oberen/unteren Rand setzen
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_UL_SPACE, sal_True,
                                                     &pItem ) )
        {
            // Ggf. den Erstzeilen-Einzug noch plaetten
            const SvxULSpaceItem *pULItem = (const SvxULSpaceItem *)pItem;
            if( rCSS1PropInfo.bTopMargin )
            {
                nUpperSpace = TWIP_TO_MM100_UNSIGNED( pULItem->GetUpper() );
                rCSS1PropInfo.bTopMargin = sal_False;
            }
            if( rCSS1PropInfo.bBottomMargin )
            {
                nLowerSpace = TWIP_TO_MM100_UNSIGNED( pULItem->GetLower() );
                rCSS1PropInfo.bBottomMargin = sal_False;
            }

            rCSS1ItemSet.ClearItem( RES_UL_SPACE );
        }
        if( nUpperSpace || nLowerSpace )
        {
            uno::Any aAny2;
            aAny2 <<= (sal_Int32)nUpperSpace;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "TopMargin" ), aAny2 );

            aAny2 <<= (sal_Int32)nLowerSpace;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "BottomMargin" ), aAny2 );
        }

        uno::Reference< beans::XPropertySetInfo > xPropSetInfo =
            rFCompPropSet->getPropertySetInfo();
        OUString sPropName = OUString::createFromAscii( "BackgroundColor" );
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_BACKGROUND, sal_True,
                                                     &pItem ) &&
            xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            const Color &rColor = ((const SvxBrushItem *)pItem)->GetColor();
            /// OD 02.09.2002 #99657#
            /// copy color, if color is not "no fill"/"auto fill"
            if( rColor.GetColor() != COL_TRANSPARENT )
            {
                /// OD 02.09.2002 #99657#
                /// copy complete color with transparency
                aTmp <<= static_cast<sal_Int32>(rColor.GetColor());
                rFCompPropSet->setPropertyValue( sPropName, aTmp );
            }

        }

        sPropName = OUString::createFromAscii( "TextColor" );
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_CHRATR_COLOR, sal_True,
                                                     &pItem ) &&
            xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp <<= (sal_Int32)((const SvxColorItem *)pItem)->GetValue()
                                                         .GetRGBColor();
            rFCompPropSet->setPropertyValue( sPropName, aTmp );
        }

        sPropName = OUString::createFromAscii( "FontHeight" );
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_CHRATR_FONTSIZE,
                                                     sal_True, &pItem ) &&
            xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            float fVal = static_cast< float >(
                (((SvxFontHeightItem *)pItem)->GetHeight()) / 20.0 );
            aTmp.setValue( &fVal, ::getCppuType(&fVal));
            rFCompPropSet->setPropertyValue( sPropName, aTmp );
        }

        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_CHRATR_FONT, sal_True,
                                                     &pItem ) )
        {
            const SvxFontItem *pFontItem = (SvxFontItem *)pItem;
            sPropName = OUString::createFromAscii( "FontName" );
            if( xPropSetInfo->hasPropertyByName( sPropName ) )
            {
                aTmp <<= OUString( pFontItem->GetFamilyName() );
                rFCompPropSet->setPropertyValue( sPropName, aTmp );
            }
            sPropName = OUString::createFromAscii( "FontStyleName" );
            if( xPropSetInfo->hasPropertyByName( sPropName ) )
            {
                aTmp <<= OUString( pFontItem->GetStyleName() );
                rFCompPropSet->setPropertyValue( sPropName, aTmp );
            }
            sPropName = OUString::createFromAscii( "FontFamily" );
            if( xPropSetInfo->hasPropertyByName( sPropName ) )
            {
                aTmp <<= (sal_Int16)pFontItem->GetFamily() ;
                rFCompPropSet->setPropertyValue( sPropName, aTmp );
            }
            sPropName = OUString::createFromAscii( "FontCharset" );
            if( xPropSetInfo->hasPropertyByName( sPropName ) )
            {
                aTmp <<= (sal_Int16)pFontItem->GetCharSet() ;
                rFCompPropSet->setPropertyValue( sPropName, aTmp );
            }
            sPropName = OUString::createFromAscii( "FontPitch" );
            if( xPropSetInfo->hasPropertyByName( sPropName ) )
            {
                aTmp <<= (sal_Int16)pFontItem->GetPitch() ;
                rFCompPropSet->setPropertyValue( sPropName, aTmp );
            }
        }

        sPropName = OUString::createFromAscii( "FontWeight" );
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_CHRATR_WEIGHT,
                                                     sal_True, &pItem ) &&
            xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            float fVal = VCLUnoHelper::ConvertFontWeight(
                    ((SvxWeightItem *)pItem)->GetWeight() );
            aTmp.setValue( &fVal, ::getCppuType(&fVal));
            rFCompPropSet->setPropertyValue( sPropName, aTmp );
        }

        sPropName = OUString::createFromAscii( "FontSlant" );
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_CHRATR_POSTURE,
                                                     sal_True, &pItem ) &&
            xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp <<= (sal_Int16)((SvxPostureItem *)pItem)->GetPosture();
            rFCompPropSet->setPropertyValue( sPropName, aTmp );
        }

        sPropName = OUString::createFromAscii( "FontUnderline" );
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_CHRATR_UNDERLINE,
                                                     sal_True, &pItem ) &&
            xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp <<= (sal_Int16)((SvxUnderlineItem *)pItem)->GetLineStyle();
            rFCompPropSet->setPropertyValue( sPropName, aTmp );
        }

        sPropName = OUString::createFromAscii( "FontStrikeout" );
        if( SFX_ITEM_SET==rCSS1ItemSet.GetItemState( RES_CHRATR_CROSSEDOUT,
                                                     sal_True, &pItem ) &&
            xPropSetInfo->hasPropertyByName( sPropName ) )
        {
            aTmp <<= (sal_Int16)((SvxCrossedOutItem *)pItem)->GetStrikeout();
            rFCompPropSet->setPropertyValue( sPropName, aTmp );
        }

        uno::Reference< text::XTextRange >  xTxtRg;
        sal_Int16 nAnchorType = text::TextContentAnchorType_AS_CHARACTER;
        sal_Bool bSetPos = sal_False, bSetSurround = sal_False;
        sal_Int32 nXPos = 0, nYPos = 0;
        sal_Int16 nSurround = text::WrapTextMode_NONE;
        if( SVX_CSS1_POS_ABSOLUTE == rCSS1PropInfo.ePosition &&
            SVX_CSS1_LTYPE_TWIP == rCSS1PropInfo.eLeftType &&
            SVX_CSS1_LTYPE_TWIP	== rCSS1PropInfo.eTopType )
        {
            const SwStartNode *pFlySttNd =
                pDoc->GetNodes()[pPam->GetPoint()->nNode]->FindFlyStartNode();

            if( pFlySttNd )
            {
                nAnchorType = text::TextContentAnchorType_AT_FRAME;
                SwPaM aPaM( *pFlySttNd );

                uno::Reference< text::XText >  xDummyTxtRef; // unsauber, aber laut OS geht das ...
                xTxtRg = new SwXTextRange( aPaM, xDummyTxtRef );
            }
            else
            {
                nAnchorType = text::TextContentAnchorType_AT_PAGE;
            }
            nXPos = TWIP_TO_MM100( rCSS1PropInfo.nLeft ) + nLeftSpace;
            nYPos = TWIP_TO_MM100( rCSS1PropInfo.nTop ) + nUpperSpace;
            bSetPos = sal_True;

            nSurround = text::WrapTextMode_THROUGHT;
            bSetSurround = sal_True;
        }
        else if( SVX_ADJUST_LEFT == rCSS1PropInfo.eFloat ||
                 text::HoriOrientation::LEFT == eHoriOri )
        {
            nAnchorType = text::TextContentAnchorType_AT_PARAGRAPH;
            nXPos = nLeftSpace;
            nYPos = nUpperSpace;
            bSetPos = sal_True;
            nSurround = text::WrapTextMode_RIGHT;
            bSetSurround = sal_True;
        }
        else if( text::VertOrientation::NONE != eVertOri )
        {
            sal_Int16 nVertOri = text::VertOrientation::NONE;
            switch( eVertOri )
            {
            case text::VertOrientation::NONE:
                nVertOri = text::VertOrientation::NONE;
                break;
            case text::VertOrientation::TOP:
                nVertOri = text::VertOrientation::TOP;
                break;
            case text::VertOrientation::CENTER:
                nVertOri = text::VertOrientation::CENTER;
                break;
            case text::VertOrientation::BOTTOM:
                nVertOri = text::VertOrientation::BOTTOM;
                break;
            case text::VertOrientation::CHAR_TOP:
                nVertOri = text::VertOrientation::CHAR_TOP;
                break;
            case text::VertOrientation::CHAR_CENTER:
                nVertOri = text::VertOrientation::CHAR_CENTER;
                break;
            case text::VertOrientation::CHAR_BOTTOM:
                nVertOri = text::VertOrientation::CHAR_BOTTOM;
                break;
            case text::VertOrientation::LINE_TOP:
                nVertOri = text::VertOrientation::LINE_TOP;
                break;
            case text::VertOrientation::LINE_CENTER:
                nVertOri = text::VertOrientation::LINE_CENTER;
                break;
            case text::VertOrientation::LINE_BOTTOM:
                nVertOri = text::VertOrientation::LINE_BOTTOM;
                break;
            }
            aTmp <<= (sal_Int16)nVertOri ;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "VertOrient" ), aTmp );
        }

        aTmp <<= (sal_Int16)nAnchorType ;
        xShapePropSet->setPropertyValue(
                OUString::createFromAscii( "AnchorType" ), aTmp );

        if( text::TextContentAnchorType_AT_PAGE == nAnchorType )
        {
            aTmp <<= (sal_Int16) 1 ;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "AnchorPageNo" ), aTmp );
        }
        else
        {
            if( !xTxtRg.is() )
            {
                uno::Reference< text::XText >  xDummyTxtRef; // unsauber, aber laut OS geht das ...
                xTxtRg = new SwXTextRange( *pPam, xDummyTxtRef );
            }

            aTmp.setValue( &xTxtRg,
                           ::getCppuType((uno::Reference< text::XTextRange>*)0));
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "TextRange" ), aTmp );
        }

        if( bSetPos )
        {
            aTmp <<= (sal_Int16)text::HoriOrientation::NONE;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "HoriOrient" ), aTmp );
            aTmp <<= (sal_Int32)nXPos ;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "HoriOrientPosition" ), aTmp );

            aTmp <<= (sal_Int16)text::VertOrientation::NONE;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "VertOrient" ), aTmp );
            aTmp <<= (sal_Int32)nYPos ;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "VertOrientPosition" ), aTmp );
        }
        if( bSetSurround )
        {
            aTmp <<= (sal_Int16)nSurround ;
            xShapePropSet->setPropertyValue(
                    OUString::createFromAscii( "Surround" ), aTmp );
        }

        pFormImpl->GetShapes()->add(xShape);

        // Das Control-Model am Control-Shape setzen
        uno::Reference< drawing::XControlShape > xControlShape( xShape, UNO_QUERY );
        uno::Reference< awt::XControlModel >  xControlModel( rFComp, UNO_QUERY );
        xControlShape->setControl( xControlModel );
    }

    // Da beim Einfuegen der Controls der Fokus gesetzt wird, werden
    // auch schon Fokus-Events verschickt. Damit die nicht evtl. schon
    // vorhendene JavaSCript-Eents rufen, werden die Events nachtraeglich
    // gesetzt.
    if( rMacroTbl.Count() || rUnoMacroTbl.Count() )
    {
        lcl_html_setEvents( pFormImpl->GetControlEventManager(),
                            rFormComps->getCount() - 1,
                            rMacroTbl, rUnoMacroTbl, rUnoMacroParamTbl,
                            GetScriptTypeString(pFormImpl->GetHeaderAttrs()) );
    }

    if( bSetFCompPropSet )
    {
        pFormImpl->SetFCompPropSet( rFCompPropSet );
    }

    return xShape;
}

void SwHTMLParser::NewForm( sal_Bool bAppend )
{
    // Gibt es schon eine Form?
    if( pFormImpl && pFormImpl->GetFormComps().is() )
        return;

    if( bAppend )
    {
        if( pPam->GetPoint()->nContent.GetIndex() )
            AppendTxtNode( AM_SPACE );
        else
            AddParSpace();
    }

    if( !pFormImpl )
        pFormImpl = new SwHTMLForm_Impl( pDoc->GetDocShell() );

    String aAction( sBaseURL );
    String sName, sTarget;
    sal_uInt16 nEncType = FormSubmitEncoding_URL;
    sal_uInt16 nMethod = FormSubmitMethod_GET;
    SvxMacroTableDtor aMacroTbl;
    SvStringsDtor aUnoMacroTbl;
    SvStringsDtor aUnoMacroParamTbl;
    SvKeyValueIterator *pHeaderAttrs = pFormImpl->GetHeaderAttrs();
    ScriptType eDfltScriptType = GetScriptType( pHeaderAttrs );
    const String& rDfltScriptType = GetScriptTypeString( pHeaderAttrs );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( sal_uInt16 i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        ScriptType eScriptType2 = eDfltScriptType;
        sal_uInt16 nEvent;
        sal_Bool bSetEvent = sal_False;

        switch( pOption->GetToken() )
        {
        case HTML_O_ACTION:
            aAction = pOption->GetString();
            break;
        case HTML_O_METHOD:
            nMethod = pOption->GetEnum( aHTMLFormMethodTable, nMethod );
            break;
        case HTML_O_ENCTYPE:
            nEncType = pOption->GetEnum( aHTMLFormEncTypeTable, nEncType );
            break;
        case HTML_O_TARGET:
            sTarget = pOption->GetString();
            break;
        case HTML_O_NAME:
            sName = pOption->GetString();
            break;

        case HTML_O_SDONSUBMIT:
            eScriptType2 = STARBASIC;
        case HTML_O_ONSUBMIT:
            nEvent = HTML_ET_ONSUBMITFORM;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONRESET:
            eScriptType2 = STARBASIC;
        case HTML_O_ONRESET:
            nEvent = HTML_ET_ONRESETFORM;
            bSetEvent = sal_True;
            break;

        default:
            lcl_html_getEvents( pOption->GetTokenString(),
                                pOption->GetString(),
                                aUnoMacroTbl, aUnoMacroParamTbl );
            break;
        }

        if( bSetEvent )
        {
            String sEvent( pOption->GetString() );
            if( sEvent.Len() )
            {
                sEvent.ConvertLineEnd();
                String aScriptType2;
                if( EXTENDED_STYPE==eScriptType2 )
                    aScriptType2 = rDfltScriptType;
                aMacroTbl.Insert( nEvent, new SvxMacro( sEvent, aScriptType2,
                                  eScriptType2 ) );
            }
        }
    }

    const uno::Reference< XMultiServiceFactory > & rSrvcMgr =
        pFormImpl->GetServiceFactory();
    if( !rSrvcMgr.is() )
        return;

    uno::Reference< XInterface > xInt = rSrvcMgr->createInstance(
            OUString::createFromAscii( "com.sun.star.form.component.Form" ) );
    if( !xInt.is() )
        return;

    uno::Reference< XForm >  xForm( xInt, UNO_QUERY );
    DBG_ASSERT( xForm.is(), "keine Form?" );

    uno::Reference< container::XIndexContainer > xFormComps( xForm, UNO_QUERY );
    pFormImpl->SetFormComps( xFormComps );

    uno::Reference< beans::XPropertySet > xFormPropSet( xForm, UNO_QUERY );

    Any aTmp;
    aTmp <<= OUString(sName);
    xFormPropSet->setPropertyValue( OUString::createFromAscii( "Name" ), aTmp );

    if( aAction.Len() )
    {
        aAction = URIHelper::SmartRel2Abs(INetURLObject(sBaseURL), aAction, Link(), false);
    }
    else
    {
        // Bei leerer URL das Directory nehmen
        INetURLObject aURLObj( aPathToFile );
        aAction = aURLObj.GetPartBeforeLastName();
    }
    aTmp <<= OUString(aAction);
    xFormPropSet->setPropertyValue( OUString::createFromAscii( "TargetURL" ),
                                    aTmp );

    FormSubmitMethod eMethod = (FormSubmitMethod)nMethod;
    aTmp.setValue( &eMethod, ::getCppuType((const FormSubmitMethod*)0) );
    xFormPropSet->setPropertyValue( OUString::createFromAscii( "SubmitMethod" ),
                                    aTmp );

     FormSubmitEncoding eEncType = (FormSubmitEncoding)nEncType;
    aTmp.setValue( &eEncType, ::getCppuType((const FormSubmitEncoding*)0) );
    xFormPropSet->setPropertyValue(
            OUString::createFromAscii( "SubmitEncoding" ), aTmp );

    if( sTarget.Len() )
    {
        aTmp <<= OUString(sTarget);
        xFormPropSet->setPropertyValue(
                OUString::createFromAscii( "TargetFrame" ), aTmp );
    }

    const uno::Reference< container::XIndexContainer > & rForms =
        pFormImpl->GetForms();
    Any aAny( &xForm, ::getCppuType((uno::Reference< XForm>*)0) );
    rForms->insertByIndex( rForms->getCount(), aAny );
    if( aMacroTbl.Count() )
        lcl_html_setEvents( pFormImpl->GetFormEventManager(),
                            rForms->getCount() - 1,
                            aMacroTbl, aUnoMacroTbl, aUnoMacroParamTbl,
                            rDfltScriptType );
}

void SwHTMLParser::EndForm( sal_Bool bAppend )
{
    if( pFormImpl && pFormImpl->GetFormComps().is() )
    {
        if( bAppend )
        {
            if( pPam->GetPoint()->nContent.GetIndex() )
                AppendTxtNode( AM_SPACE );
            else
                AddParSpace();
        }

        pFormImpl->ReleaseFormComps();
    }
}

void SwHTMLParser::InsertInput()
{
    if( pPendStack )
    {
        SetPendingControlSize( HTML_INPUT );
        return;
    }

    if( !pFormImpl || !pFormImpl->GetFormComps().is() )
        return;

    String sImgSrc, aId, aClass, aStyle, sText;
    String sName;
    SvxMacroTableDtor aMacroTbl;
    SvStringsDtor aUnoMacroTbl;
    SvStringsDtor aUnoMacroParamTbl;
    sal_uInt16 nSize = 0;
    sal_Int16 nMaxLen = 0;
    sal_Int16 nChecked = STATE_NOCHECK;
    sal_Int32 nTabIndex = TABINDEX_MAX + 1;
    HTMLInputType eType = HTML_IT_TEXT;
    sal_Bool bDisabled = sal_False, bValue = sal_False;
    sal_Bool bSetGrfWidth = sal_False, bSetGrfHeight = sal_False;
    sal_Bool bHidden = sal_False;
    long nWidth=0, nHeight=0;
    sal_Int16 eVertOri = text::VertOrientation::TOP;
    sal_Int16 eHoriOri = text::HoriOrientation::NONE;
    SvKeyValueIterator *pHeaderAttrs = pFormImpl->GetHeaderAttrs();
    ScriptType eDfltScriptType = GetScriptType( pHeaderAttrs );
    const String& rDfltScriptType = GetScriptTypeString( pHeaderAttrs );

    sal_uInt16 nKeepCRLFToken = HTML_O_VALUE;
    const HTMLOptions *pHTMLOptions = GetOptions( &nKeepCRLFToken );
    for( sal_uInt16 i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        ScriptType eScriptType2 = eDfltScriptType;
        sal_uInt16 nEvent;
        sal_Bool bSetEvent = sal_False;

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
        case HTML_O_TYPE:
            eType = pOption->GetInputType();
            break;
        case HTML_O_NAME:
            sName = pOption->GetString();
            break;
        case HTML_O_VALUE:
            sText = pOption->GetString();
            bValue = sal_True;
            break;
        case HTML_O_CHECKED:
            nChecked = STATE_CHECK;
            break;
        case HTML_O_DISABLED:
            bDisabled = sal_True;
            break;
        case HTML_O_MAXLENGTH:
            nMaxLen = (sal_Int16)pOption->GetNumber();
            break;
        case HTML_O_SIZE:
            nSize = (sal_uInt16)pOption->GetNumber();
            break;
        case HTML_O_SRC:
            sImgSrc = pOption->GetString();
            break;
        case HTML_O_WIDTH:
            // erstmal nur als Pixelwerte merken!
            nWidth = pOption->GetNumber();
            break;
        case HTML_O_HEIGHT:
            // erstmal nur als Pixelwerte merken!
            nHeight = pOption->GetNumber();
            break;
        case HTML_O_ALIGN:
            eVertOri =
                pOption->GetEnum( aHTMLImgVAlignTable, eVertOri );
            eHoriOri =
                pOption->GetEnum( aHTMLImgHAlignTable, eHoriOri );
            break;
        case HTML_O_TABINDEX:
            // erstmal nur als Pixelwerte merken!
            nTabIndex = pOption->GetNumber();
            break;

        case HTML_O_SDONFOCUS:
            eScriptType2 = STARBASIC;
        case HTML_O_ONFOCUS:
            nEvent = HTML_ET_ONGETFOCUS;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONBLUR:				// eigtl. nur EDIT
            eScriptType2 = STARBASIC;
        case HTML_O_ONBLUR:
            nEvent = HTML_ET_ONLOSEFOCUS;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONCLICK:
            eScriptType2 = STARBASIC;
        case HTML_O_ONCLICK:
            nEvent = HTML_ET_ONCLICK;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONCHANGE:				// eigtl. nur EDIT
            eScriptType2 = STARBASIC;
        case HTML_O_ONCHANGE:
            nEvent = HTML_ET_ONCHANGE;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONSELECT:				// eigtl. nur EDIT
            eScriptType2 = STARBASIC;
        case HTML_O_ONSELECT:
            nEvent = HTML_ET_ONSELECT;
            bSetEvent = sal_True;
            break;

        default:
            lcl_html_getEvents( pOption->GetTokenString(),
                                pOption->GetString(),
                                aUnoMacroTbl, aUnoMacroParamTbl );
            break;
        }

        if( bSetEvent )
        {
            String sEvent( pOption->GetString() );
            if( sEvent.Len() )
            {
                sEvent.ConvertLineEnd();
                String aScriptType2;
                if( EXTENDED_STYPE==eScriptType2 )
                    aScriptType2 = rDfltScriptType;
                aMacroTbl.Insert( nEvent, new SvxMacro( sEvent, aScriptType2,
                                  eScriptType2 ) );
            }
        }
    }

    if( HTML_IT_IMAGE==eType )
    {
        // Image-Controls ohne Image-URL werden ignoriert (wie bei MS)
        if( !sImgSrc.Len() )
            return;
    }
    else
    {
        // ALIGN fuer alle Controls auszuwerten ist keine so gute Idee,
        // solange Absatz-gebundene Controls die Hoehe von Tabellen-Zellen
        // nicht beeinflussen
        // (#64110#, http://www.telekom.de/katalog-online/onlineshop.html)
        eVertOri = text::VertOrientation::TOP;
        eHoriOri = text::HoriOrientation::NONE;
    }

    // Defaults entsprechen HTML_IT_TEXT
    const sal_Char *pType = "TextField";
    sal_Bool bKeepCRLFInValue = sal_False;
    switch( eType )
    {
    case HTML_IT_CHECKBOX:
        pType = "CheckBox";
        bKeepCRLFInValue = sal_True;
        break;

    case HTML_IT_RADIO:
        pType = "RadioButton";
        bKeepCRLFInValue = sal_True;
        break;

    case HTML_IT_PASSWORD:
        bKeepCRLFInValue = sal_True;
        break;

    case HTML_IT_BUTTON:
        bKeepCRLFInValue = sal_True;
    case HTML_IT_SUBMIT:
    case HTML_IT_RESET:
        pType = "CommandButton";
        break;

    case HTML_IT_IMAGE:
        pType = "ImageButton";
        break;

    case HTML_IT_FILE:
        pType = "FileControl";
        break;

    case HTML_IT_HIDDEN:
        pType = "HiddenControl";
        bKeepCRLFInValue = sal_True;
        break;
    default:
        ;
    }

    // Fuer ein par Controls mussen CR/LF noch aus dem VALUE
    // geloescht werden.
    if( !bKeepCRLFInValue )
    {
        sText.EraseAllChars( _CR );
        sText.EraseAllChars( _LF );
    }

    const uno::Reference< XMultiServiceFactory > & rServiceFactory =
        pFormImpl->GetServiceFactory();
    if( !rServiceFactory.is() )
        return;

    String sServiceName(
            OUString::createFromAscii("com.sun.star.form.component.") );
    sServiceName.AppendAscii( pType );
    uno::Reference< XInterface > xInt =
        rServiceFactory->createInstance( sServiceName );
    if( !xInt.is() )
        return;

    uno::Reference< XFormComponent > xFComp( xInt, UNO_QUERY );
    if( !xFComp.is() )
        return;

    uno::Reference< beans::XPropertySet > xPropSet( xFComp, UNO_QUERY );

    Any aTmp;
    aTmp <<= OUString(sName);
    xPropSet->setPropertyValue( OUString::createFromAscii( "Name" ), aTmp );

    if( HTML_IT_HIDDEN != eType  )
    {
        if( nTabIndex >= TABINDEX_MIN && nTabIndex <= TABINDEX_MAX  )
        {
            aTmp <<= (sal_Int16) (sal_Int16)nTabIndex ;
            xPropSet->setPropertyValue( OUString::createFromAscii( "TabIndex" ), aTmp );
        }

        if( bDisabled )
        {
            BOOL bFalse = sal_False;
            aTmp.setValue(&bFalse, ::getBooleanCppuType()  );
            xPropSet->setPropertyValue( OUString::createFromAscii( "Enabled" ), aTmp );
        }
    }

    aTmp <<= OUString(sText);

    Size aSz( 0, 0 );		// defaults
    Size aTextSz( 0, 0 );	// Text-Size
    sal_Bool bMinWidth = sal_False, bMinHeight = sal_False;
    sal_Bool bUseSize = sal_False;
    switch( eType )
    {
    case HTML_IT_CHECKBOX:
    case HTML_IT_RADIO:
        {
            if( !bValue )
                aTmp <<= OUString::createFromAscii( OOO_STRING_SVTOOLS_HTML_on );
            xPropSet->setPropertyValue( OUString::createFromAscii( "RefValue" ),
                                        aTmp );
            aTmp <<= OUString();
            xPropSet->setPropertyValue( OUString::createFromAscii( "Label" ),
                                        aTmp );
            // #53559#: Beim RadioButton darf die DefaultChecked-Property
            // erst gesetzt werden, wenn das Control angelegt und ein
            // activateTabOrder gerufen wurde, weil es sonst noch zu der
            // vorhergehenden Gruppe gehoert.
            if( HTML_IT_CHECKBOX == eType )
            {
                aTmp <<= (sal_Int16) nChecked ;
                xPropSet->setPropertyValue(
                        OUString::createFromAscii( "DefaultState" ), aTmp );
            }

            SvxMacro *pMacro = aMacroTbl.Get( HTML_ET_ONCLICK );
            if( pMacro )
            {
                aMacroTbl.Remove( HTML_ET_ONCLICK );
                aMacroTbl.Insert( HTML_ET_ONCLICK_ITEM, pMacro );
            }
            // SIZE auszuwerten duerfte hier keinen Sinn machen???
            bMinWidth = bMinHeight = sal_True;
        }
        break;

    case HTML_IT_IMAGE:
        {
            // SIZE = WIDTH
            aSz.Width() = nSize ? nSize : nWidth;
            aSz.Width() = nWidth;
            aSz.Height() = nHeight;
            if( (aSz.Width() || aSz.Height()) && Application::GetDefaultDevice() )
            {
                aSz = Application::GetDefaultDevice()
                    ->PixelToLogic( aSz, MapMode( MAP_100TH_MM ) );
            }
             FormButtonType eButtonType = FormButtonType_SUBMIT;
            aTmp.setValue( &eButtonType,
                           ::getCppuType((const FormButtonType*)0));
            xPropSet->setPropertyValue(
                    OUString::createFromAscii( "ButtonType" ), aTmp );

            aTmp <<= (sal_Int16) 0  ;
            xPropSet->setPropertyValue( OUString::createFromAscii( "Border" ),
                                        aTmp );
        }
        break;

    case HTML_IT_BUTTON:
    case HTML_IT_SUBMIT:
    case HTML_IT_RESET:
        {
             FormButtonType eButtonType;
            switch( eType )
            {
            case HTML_IT_BUTTON:
                eButtonType = FormButtonType_PUSH;
                break;
            case HTML_IT_SUBMIT:
                eButtonType = FormButtonType_SUBMIT;
                if( !sText.Len() )
                    sText.AssignAscii( OOO_STRING_SVTOOLS_HTML_IT_submit );
                break;
            case HTML_IT_RESET:
                eButtonType = FormButtonType_RESET;
                if( !sText.Len() )
                    sText.AssignAscii( OOO_STRING_SVTOOLS_HTML_IT_reset );
                break;
            default:
                ;
            }
            aTmp <<= OUString(sText);
            xPropSet->setPropertyValue( OUString::createFromAscii( "Label" ),
                                        aTmp );

            aTmp.setValue( &eButtonType,
                           ::getCppuType((const FormButtonType*)0));
            xPropSet->setPropertyValue(
                            OUString::createFromAscii( "ButtonType" ), aTmp );

            bMinWidth = bMinHeight = sal_True;
            bUseSize = sal_True;
        }
        break;

    case HTML_IT_PASSWORD:
    case HTML_IT_TEXT:
    case HTML_IT_FILE:
        if( HTML_IT_FILE != eType )
        {
        // Beim File-Control wird der VALUE aus Sicherheitsgruenden ignoriert.
            xPropSet->setPropertyValue(
                    OUString::createFromAscii( "DefaultText" ), aTmp );
            if( nMaxLen != 0 )
            {
                aTmp <<= (sal_Int16) nMaxLen ;
                xPropSet->setPropertyValue(
                        OUString::createFromAscii( "MaxTextLen" ), aTmp );
            }
        }

        if( HTML_IT_PASSWORD == eType )
        {
            aTmp <<= (sal_Int16)'*' ;
            xPropSet->setPropertyValue( OUString::createFromAscii( "EchoChar" ),
                                        aTmp );
        }

        lcl_html_setFixedFontProperty( xPropSet );

        if( !nSize )
            nSize = 20;
        aTextSz.Width() = nSize;
        bMinHeight = sal_True;
        break;

    case HTML_IT_HIDDEN:
        xPropSet->setPropertyValue( OUString::createFromAscii( "HiddenValue" ),
                                    aTmp );
        bHidden = sal_True;
        break;
    default:
        ;
    }

    if( bUseSize && nSize>0 )
    {
        if( Application::GetDefaultDevice() )
        {
            Size aNewSz( nSize, 0 );
            aNewSz = Application::GetDefaultDevice()
                        ->PixelToLogic( aNewSz, MapMode( MAP_100TH_MM ) );
            aSz.Width() = aNewSz.Width();
            ASSERT( !aTextSz.Width(), "Text-Breite ist gegeben" );
            bMinWidth = sal_False;
        }
    }

    SfxItemSet aCSS1ItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aCSS1PropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
    {
        ParseStyleOptions( aStyle, aId, aClass, aCSS1ItemSet, aCSS1PropInfo );
        if( aId.Len() )
            InsertBookmark( aId );
    }

    if( SVX_CSS1_LTYPE_TWIP== aCSS1PropInfo.eWidthType )
    {
        aSz.Width() = TWIP_TO_MM100( aCSS1PropInfo.nWidth );
        aTextSz.Width() = 0;
        bMinWidth = sal_False;
    }
    if( SVX_CSS1_LTYPE_TWIP== aCSS1PropInfo.eHeightType )
    {
        aSz.Height() = TWIP_TO_MM100( aCSS1PropInfo.nHeight );
        aTextSz.Height() = 0;
        bMinHeight = sal_False;
    }

    // Beim Image-Button bei nicht gegebern Groesse einen sinnvollen Default
    // setzen
    if( HTML_IT_IMAGE== eType )
    {
        if( !aSz.Width() )
        {
            aSz.Width() = HTML_DFLT_IMG_WIDTH;
            bSetGrfWidth = sal_True;
            if( pTable != 0 )
                IncGrfsThatResizeTable();
        }
        if( !aSz.Height() )
        {
            aSz.Height() = HTML_DFLT_IMG_HEIGHT;
            bSetGrfHeight = sal_True;
        }
    }
    if( aSz.Width() < MINFLY )
        aSz.Width() = MINFLY;
    if( aSz.Height() < MINFLY )
        aSz.Height() = MINFLY;

    uno::Reference< drawing::XShape > xShape = InsertControl(
                                             xFComp, xPropSet, aSz,
                                             eVertOri, eHoriOri,
                                             aCSS1ItemSet, aCSS1PropInfo,
                                             aMacroTbl, aUnoMacroTbl,
                                             aUnoMacroParamTbl, sal_False,
                                             bHidden );
    if( aTextSz.Width() || aTextSz.Height() || bMinWidth || bMinHeight )
    {
        ASSERT( !(bSetGrfWidth || bSetGrfHeight), "Grafikgroesse anpassen???" );
        SetControlSize( xShape, aTextSz, bMinWidth, bMinHeight, HTML_INPUT );
    }

    if( HTML_IT_RADIO == eType )
    {
        aTmp <<= (sal_Int16) nChecked ;
        xPropSet->setPropertyValue( OUString::createFromAscii( "DefaultState" ), aTmp );
    }

    if( HTML_IT_IMAGE == eType )
    {
        // Die URL erst nach dem Einfuegen setzen, weil sich der
        // Download der Grafik erst dann am XModel anmelden kann,
        // wenn das Control eingefuegt ist.
        aTmp <<= OUString( URIHelper::SmartRel2Abs(INetURLObject(sBaseURL), sImgSrc, Link(), false));
        xPropSet->setPropertyValue( OUString::createFromAscii( "ImageURL" ),
                                    aTmp );
    }

    if( bSetGrfWidth || bSetGrfHeight )
    {
        SwHTMLImageWatcher* pWatcher =
            new SwHTMLImageWatcher( xShape, bSetGrfWidth, bSetGrfHeight );
        uno::Reference< awt::XImageConsumer > xCons = pWatcher;
        pWatcher->start();
    }
}

void SwHTMLParser::NewTextArea()
{
    if( pPendStack )
    {
        SetPendingControlSize( HTML_TEXTAREA_ON );
        return;
    }

    ASSERT( !bTextArea, "TextArea in TextArea???" );
    ASSERT( !pFormImpl || !pFormImpl->GetFCompPropSet().is(),
            "TextArea in Control???" );

    if( !pFormImpl || !pFormImpl->GetFormComps().is() )
    {
        // Spezialbehandlung fuer TextArea auch untem im Parser beenden
        FinishTextArea();
        return;
    }

    String aId, aClass, aStyle;
    String sName;
    sal_Int32 nTabIndex = TABINDEX_MAX + 1;
    SvxMacroTableDtor aMacroTbl;
    SvStringsDtor aUnoMacroTbl;
    SvStringsDtor aUnoMacroParamTbl;
    sal_uInt16 nRows = 0, nCols = 0;
    sal_uInt16 nWrap = HTML_WM_OFF;
    sal_Bool bDisabled = sal_False;
    SvKeyValueIterator *pHeaderAttrs = pFormImpl->GetHeaderAttrs();
    ScriptType eDfltScriptType = GetScriptType( pHeaderAttrs );
    const String& rDfltScriptType = GetScriptTypeString( pHeaderAttrs );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( sal_uInt16 i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        ScriptType eScriptType2 = eDfltScriptType;
        sal_uInt16 nEvent;
        sal_Bool bSetEvent = sal_False;

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
            sName = pOption->GetString();
            break;
        case HTML_O_DISABLED:
            bDisabled = sal_True;
            break;
        case HTML_O_ROWS:
            nRows = (sal_uInt16)pOption->GetNumber();
            break;
        case HTML_O_COLS:
            nCols = (sal_uInt16)pOption->GetNumber();
            break;
        case HTML_O_WRAP:
            nWrap = pOption->GetEnum( aHTMLTextAreaWrapTable, nWrap );
            break;

        case HTML_O_TABINDEX:
            nTabIndex = pOption->GetSNumber();
            break;

        case HTML_O_SDONFOCUS:
            eScriptType2 = STARBASIC;
        case HTML_O_ONFOCUS:
            nEvent = HTML_ET_ONGETFOCUS;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONBLUR:
            eScriptType2 = STARBASIC;
        case HTML_O_ONBLUR:
            nEvent = HTML_ET_ONLOSEFOCUS;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONCLICK:
            eScriptType2 = STARBASIC;
        case HTML_O_ONCLICK:
            nEvent = HTML_ET_ONCLICK;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONCHANGE:
            eScriptType2 = STARBASIC;
        case HTML_O_ONCHANGE:
            nEvent = HTML_ET_ONCHANGE;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONSELECT:
            eScriptType2 = STARBASIC;
        case HTML_O_ONSELECT:
            nEvent = HTML_ET_ONSELECT;
            bSetEvent = sal_True;
            break;

        default:
            lcl_html_getEvents( pOption->GetTokenString(),
                                pOption->GetString(),
                                aUnoMacroTbl, aUnoMacroParamTbl );
            break;
        }

        if( bSetEvent )
        {
            String sEvent( pOption->GetString() );
            if( sEvent.Len() )
            {
                sEvent.ConvertLineEnd();
                if( EXTENDED_STYPE==eScriptType2 )
                    aScriptType = rDfltScriptType;
                aMacroTbl.Insert( nEvent, new SvxMacro( sEvent, aScriptType,
                                  eScriptType2 ) );
            }
        }
    }


    const uno::Reference< lang::XMultiServiceFactory > & rSrvcMgr =
        pFormImpl->GetServiceFactory();
    if( !rSrvcMgr.is() )
    {
        FinishTextArea();
        return;
    }
    uno::Reference< uno::XInterface >  xInt = rSrvcMgr->createInstance(
        OUString::createFromAscii( "com.sun.star.form.component.TextField" ) );
    if( !xInt.is() )
    {
        FinishTextArea();
        return;
    }

    uno::Reference< XFormComponent > xFComp( xInt, UNO_QUERY );
    DBG_ASSERT( xFComp.is(), "keine FormComponent?" );

    uno::Reference< beans::XPropertySet > xPropSet( xFComp, UNO_QUERY );

    Any aTmp;
    aTmp <<= OUString(sName);
    xPropSet->setPropertyValue( OUString::createFromAscii( "Name" ), aTmp );

    BOOL bTrue = sal_True;
    aTmp.setValue( &bTrue, ::getBooleanCppuType() );
    xPropSet->setPropertyValue( OUString::createFromAscii( "MultiLine" ),
                                aTmp );
    xPropSet->setPropertyValue( OUString::createFromAscii( "VScroll" ), aTmp );
    if( HTML_WM_OFF == nWrap )
        xPropSet->setPropertyValue( OUString::createFromAscii( "HScroll" ),
                                    aTmp );
    if( HTML_WM_HARD == nWrap )
        xPropSet->setPropertyValue(
                OUString::createFromAscii( "HardLineBreaks" ), aTmp );

    if( nTabIndex >= TABINDEX_MIN && nTabIndex <= TABINDEX_MAX  )
    {
        aTmp <<= (sal_Int16)nTabIndex ;
        xPropSet->setPropertyValue( OUString::createFromAscii( "TabIndex" ),
                                    aTmp );
    }

    lcl_html_setFixedFontProperty( xPropSet );

    if( bDisabled )
    {
        BOOL bFalse = sal_False;
        aTmp.setValue( &bFalse, ::getBooleanCppuType() );
        xPropSet->setPropertyValue( OUString::createFromAscii( "Enabled" ),
                                    aTmp );
    }

    ASSERT( !pFormImpl->GetText().Len(), "Text ist nicht leer!" );

    if( !nCols )
        nCols = 20;
    if( !nRows )
        nRows = 1;

    Size aTextSz( nCols, nRows );

    SfxItemSet aCSS1ItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aCSS1PropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
    {
        ParseStyleOptions( aStyle, aId, aClass, aCSS1ItemSet, aCSS1PropInfo );
        if( aId.Len() )
            InsertBookmark( aId );
    }

    Size aSz( MINFLY, MINFLY );
    if( SVX_CSS1_LTYPE_TWIP== aCSS1PropInfo.eWidthType )
    {
        aSz.Width() = TWIP_TO_MM100( aCSS1PropInfo.nWidth );
        aTextSz.Width() = 0;
    }
    if( SVX_CSS1_LTYPE_TWIP== aCSS1PropInfo.eHeightType )
    {
        aSz.Height() = TWIP_TO_MM100( aCSS1PropInfo.nHeight );
        aTextSz.Height() = 0;
    }
    if( aSz.Width() < MINFLY )
        aSz.Width() = MINFLY;
    if( aSz.Height() < MINFLY )
        aSz.Height() = MINFLY;

    uno::Reference< drawing::XShape > xShape = InsertControl( xFComp, xPropSet, aSz,
                                      text::VertOrientation::TOP, text::HoriOrientation::NONE,
                                      aCSS1ItemSet, aCSS1PropInfo,
                                      aMacroTbl, aUnoMacroTbl,
                                      aUnoMacroParamTbl );
    if( aTextSz.Width() || aTextSz.Height() )
        SetControlSize( xShape, aTextSz, sal_False, sal_False,
                        HTML_TEXTAREA_ON );

    // einen neuen Kontext anlegen
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( HTML_TEXTAREA_ON );

    // und PRE/Listing/XMP voruebergehend aussetzen
    SplitPREListingXMP( pCntxt );
    PushContext( pCntxt );

    bTextArea = sal_True;
    bTAIgnoreNewPara = sal_True;
}

void SwHTMLParser::EndTextArea()
{
    ASSERT( bTextArea, "keine TextArea oder falscher Typ" );
    ASSERT( pFormImpl && pFormImpl->GetFCompPropSet().is(),
            "TextArea fehlt" );

    const uno::Reference< beans::XPropertySet > & rPropSet =
        pFormImpl->GetFCompPropSet();

    Any aTmp;
    aTmp <<= OUString(pFormImpl->GetText());
    rPropSet->setPropertyValue( OUString::createFromAscii( "DefaultText" ),
                                aTmp );
    pFormImpl->EraseText();

    pFormImpl->ReleaseFCompPropSet();

    // den Kontext holen
    _HTMLAttrContext *pCntxt = PopContext( HTML_TEXTAREA_ON );
    if( pCntxt )
    {
        // und ggf. die Attribute beenden
        EndContext( pCntxt );
        delete pCntxt;
    }

    bTextArea = sal_False;
}


void SwHTMLParser::InsertTextAreaText( sal_uInt16 nToken )
{
    ASSERT( bTextArea, "keine TextArea oder falscher Typ" );
    ASSERT( pFormImpl && pFormImpl->GetFCompPropSet().is(),
            "TextArea fehlt" );

    String& rText = pFormImpl->GetText();
    switch( nToken)
    {
    case HTML_TEXTTOKEN:
        rText += aToken;
        break;
    case HTML_NEWPARA:
        if( !bTAIgnoreNewPara )
            rText += '\n';    // das ist hier richtig!!!
        break;
    default:
        rText += '<';
        rText += sSaveToken;
        if( aToken.Len() )
        {
            rText += ' ';
            rText += aToken;
        }
        rText += '>';
    }

    bTAIgnoreNewPara = sal_False;
}

void SwHTMLParser::NewSelect()
{
    if( pPendStack )
    {
        SetPendingControlSize( HTML_SELECT_ON );
        return;
    }

    ASSERT( !bSelect, "Select in Select???" );
    ASSERT( !pFormImpl || !pFormImpl->GetFCompPropSet().is(),
            "Select in Control???" );

    if( !pFormImpl || !pFormImpl->GetFormComps().is() )
        return;

    String aId, aClass, aStyle;
    String sName;
    sal_Int32 nTabIndex = TABINDEX_MAX + 1;
    SvxMacroTableDtor aMacroTbl;
    SvStringsDtor aUnoMacroTbl;
    SvStringsDtor aUnoMacroParamTbl;
    sal_Bool bMultiple = sal_False;
    sal_Bool bDisabled = sal_False;
    nSelectEntryCnt = 1;
    SvKeyValueIterator *pHeaderAttrs = pFormImpl->GetHeaderAttrs();
    ScriptType eDfltScriptType = GetScriptType( pHeaderAttrs );
    const String& rDfltScriptType = GetScriptTypeString( pHeaderAttrs );

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( sal_uInt16 i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[--i];
        ScriptType eScriptType2 = eDfltScriptType;
        sal_uInt16 nEvent;
        sal_Bool bSetEvent = sal_False;

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
            sName = pOption->GetString();
            break;
        case HTML_O_MULTIPLE:
            bMultiple = sal_True;
            break;
        case HTML_O_DISABLED:
            bDisabled = sal_True;
            break;
        case HTML_O_SIZE:
            nSelectEntryCnt = (sal_uInt16)pOption->GetNumber();
            break;

        case HTML_O_TABINDEX:
            nTabIndex = pOption->GetSNumber();
            break;

        case HTML_O_SDONFOCUS:
            eScriptType2 = STARBASIC;
        case HTML_O_ONFOCUS:
            nEvent = HTML_ET_ONGETFOCUS;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONBLUR:
            eScriptType2 = STARBASIC;
        case HTML_O_ONBLUR:
            nEvent = HTML_ET_ONLOSEFOCUS;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONCLICK:
            eScriptType2 = STARBASIC;
        case HTML_O_ONCLICK:
            nEvent = HTML_ET_ONCLICK;
            bSetEvent = sal_True;
            break;

        case HTML_O_SDONCHANGE:
            eScriptType2 = STARBASIC;
        case HTML_O_ONCHANGE:
            nEvent = HTML_ET_ONCHANGE;
            bSetEvent = sal_True;
            break;

        default:
            lcl_html_getEvents( pOption->GetTokenString(),
                                pOption->GetString(),
                                aUnoMacroTbl, aUnoMacroParamTbl );
            break;
        }

        if( bSetEvent )
        {
            String sEvent( pOption->GetString() );
            if( sEvent.Len() )
            {
                sEvent.ConvertLineEnd();
                if( EXTENDED_STYPE==eScriptType2 )
                    aScriptType = rDfltScriptType;
                aMacroTbl.Insert( nEvent, new SvxMacro( sEvent, aScriptType,
                                  eScriptType2 ) );
            }
        }
    }

    const uno::Reference< lang::XMultiServiceFactory > & rSrvcMgr =
        pFormImpl->GetServiceFactory();
    if( !rSrvcMgr.is() )
    {
        FinishTextArea();
        return;
    }
    uno::Reference< uno::XInterface >  xInt = rSrvcMgr->createInstance(
        OUString::createFromAscii( "com.sun.star.form.component.ListBox" ) );
    if( !xInt.is() )
    {
        FinishTextArea();
        return;
    }

    uno::Reference< XFormComponent > xFComp( xInt, UNO_QUERY );
    DBG_ASSERT(xFComp.is(), "keine FormComponent?");

    uno::Reference< beans::XPropertySet >  xPropSet( xFComp, UNO_QUERY );

    Any aTmp;
    aTmp <<= OUString(sName);
    xPropSet->setPropertyValue( OUString::createFromAscii( "Name" ), aTmp );

    if( nTabIndex >= TABINDEX_MIN && nTabIndex <= TABINDEX_MAX  )
    {
        aTmp <<= (sal_Int16)nTabIndex ;
        xPropSet->setPropertyValue( OUString::createFromAscii( "TabIndex" ),
                                    aTmp );
    }

    if( bDisabled )
    {
        BOOL bFalse = sal_False;
        aTmp.setValue( &bFalse, ::getBooleanCppuType() );
        xPropSet->setPropertyValue( OUString::createFromAscii( "Enabled" ),
                                    aTmp );
    }

    Size aTextSz( 0, 0 );
    sal_Bool bMinWidth = sal_True, bMinHeight = sal_True;
    if( !bMultiple && 1==nSelectEntryCnt )
    {
        BOOL bTrue = sal_True;
        aTmp.setValue( &bTrue, ::getBooleanCppuType() );
        xPropSet->setPropertyValue( OUString::createFromAscii( "Dropdown" ),
                                    aTmp );
    }
    else
    {
        if( nSelectEntryCnt <= 1 )		// 4 Zeilen als default
            nSelectEntryCnt = 4;

        if( bMultiple )
        {
            BOOL bTrue = sal_True;
            aTmp.setValue( &bTrue, ::getBooleanCppuType() );
            xPropSet->setPropertyValue(
                    OUString::createFromAscii( "MultiSelection" ), aTmp );
        }
        aTextSz.Height() = nSelectEntryCnt;
        bMinHeight = sal_False;
    }

    SfxItemSet aCSS1ItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
    SvxCSS1PropertyInfo aCSS1PropInfo;
    if( HasStyleOptions( aStyle, aId, aClass ) )
    {
        ParseStyleOptions( aStyle, aId, aClass, aCSS1ItemSet, aCSS1PropInfo );
        if( aId.Len() )
            InsertBookmark( aId );
    }

    Size aSz( MINFLY, MINFLY );
    bFixSelectWidth = bFixSelectHeight = sal_True;
    if( SVX_CSS1_LTYPE_TWIP== aCSS1PropInfo.eWidthType )
    {
        aSz.Width() = TWIP_TO_MM100( aCSS1PropInfo.nWidth );
        bFixSelectWidth = sal_False;
        bMinWidth = sal_False;
    }
    if( SVX_CSS1_LTYPE_TWIP== aCSS1PropInfo.eHeightType )
    {
        aSz.Height() = TWIP_TO_MM100( aCSS1PropInfo.nHeight );
        aTextSz.Height() = sal_False;
        bMinHeight = sal_False;
    }
    if( aSz.Width() < MINFLY )
        aSz.Width() = MINFLY;
    if( aSz.Height() < MINFLY )
        aSz.Height() = MINFLY;

    uno::Reference< drawing::XShape >  xShape = InsertControl( xFComp, xPropSet, aSz,
                                      text::VertOrientation::TOP, text::HoriOrientation::NONE,
                                      aCSS1ItemSet, aCSS1PropInfo,
                                      aMacroTbl, aUnoMacroTbl,
                                      aUnoMacroParamTbl );
    if( bFixSelectWidth )
        pFormImpl->SetShape( xShape );
    if( aTextSz.Height() || bMinWidth || bMinHeight )
        SetControlSize( xShape, aTextSz, bMinWidth, bMinHeight,
                        HTML_SELECT_ON );

    // einen neuen Kontext anlegen
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( HTML_SELECT_ON );

    // und PRE/Listing/XMP voruebergehend aussetzen
    SplitPREListingXMP( pCntxt );
    PushContext( pCntxt );

    bSelect = sal_True;
}

void SwHTMLParser::EndSelect()
{
    if( pPendStack )
    {
        SetPendingControlSize( HTML_SELECT_OFF );
        return;
    }

    ASSERT( bSelect, "keine Select" );
    ASSERT( pFormImpl && pFormImpl->GetFCompPropSet().is(),
            "kein Select-Control" );

    const uno::Reference< beans::XPropertySet > & rPropSet =
        pFormImpl->GetFCompPropSet();

    // die Groesse anpassen
    Size aNewSz( MINFLY, MINFLY );

    sal_uInt16 nEntryCnt = pFormImpl->GetStringList().Count();
    if( nEntryCnt )
    {
        Sequence<OUString> aList( (sal_Int32)nEntryCnt );
        Sequence<OUString> aValueList( (sal_Int32)nEntryCnt );
        OUString *pStrings = aList.getArray();
        OUString *pValues = aValueList.getArray();
        sal_uInt16 i;

        for( i = 0; i < nEntryCnt; i++ )
        {
            String sText( *pFormImpl->GetStringList()[i] );
            sText.EraseTrailingChars();
            pStrings[i] = sText;

            sText = *pFormImpl->GetValueList()[i];
            pValues[i] = sText;
        }

        Any aAny( &aList, ::getCppuType((uno::Sequence<OUString>*)0) );

        rPropSet->setPropertyValue(
                OUString::createFromAscii( "StringItemList" ), aAny );

        aAny <<= ListSourceType_VALUELIST;
        rPropSet->setPropertyValue(
                OUString::createFromAscii( "ListSourceType" ), aAny );

        aAny.setValue( &aValueList, ::getCppuType((uno::Sequence<OUString>*)0) );

        rPropSet->setPropertyValue( OUString::createFromAscii( "ListSource" ),
                                    aAny );

        sal_uInt16 nSelCnt = pFormImpl->GetSelectedList().Count();
        if( !nSelCnt && 1 == nSelectEntryCnt && nEntryCnt )
        {
            // In einer DropDown-Listbox sollte immer ein Eintrag selektiert
            // sein.
            pFormImpl->GetSelectedList().Insert( (sal_uInt16)0, (sal_uInt16)0 );
            nSelCnt = 1;
        }
        Sequence<sal_Int16> aSelList( (sal_Int32)nSelCnt );
        sal_Int16 *pSels = aSelList.getArray();
        for( i=0; i<nSelCnt; i++ )
        {
            pSels[i] = (sal_Int16)pFormImpl->GetSelectedList()[i];
        }
        aAny.setValue( &aSelList,
                       ::getCppuType((uno::Sequence<sal_Int16>*)0) );

        rPropSet->setPropertyValue(
                OUString::createFromAscii( "DefaultSelection" ), aAny );

        pFormImpl->EraseStringList();
        pFormImpl->EraseValueList();
    }

    pFormImpl->EraseSelectedList();

    if( bFixSelectWidth )
    {
        ASSERT( pFormImpl->GetShape().is(), "Kein Shape gemerkt" );
        Size aTextSz( -1, 0 );
        SetControlSize( pFormImpl->GetShape(), aTextSz, sal_False, sal_False,
                        HTML_SELECT_OFF );
    }

    pFormImpl->ReleaseFCompPropSet();

    // den Kontext holen
    _HTMLAttrContext *pCntxt = PopContext( HTML_SELECT_ON );
    if( pCntxt )
    {
        // und ggf. die Attribute beenden
        EndContext( pCntxt );
        delete pCntxt;
    }

    bSelect = sal_False;
}

void SwHTMLParser::InsertSelectOption()
{
    ASSERT( bSelect, "keine Select" );
    ASSERT( pFormImpl && pFormImpl->GetFCompPropSet().is(),
            "kein Select-Control" );

    bLBEntrySelected = sal_False;
    String aValue;

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( sal_uInt16 i = pHTMLOptions->Count(); i; )
    {
        HTMLOption *pOption = (*pHTMLOptions)[--i];
        switch( pOption->GetToken() )
        {
        case HTML_O_ID:
            // erstmal weglassen!!!
            break;
        case HTML_O_SELECTED:
            bLBEntrySelected = sal_True;
            break;
        case HTML_O_VALUE:
            aValue = pOption->GetString();
            if( !aValue.Len() )
                aValue.AssignAscii( "$$$empty$$$" );
            break;
        }
    }

    sal_uInt16 nEntryCnt = pFormImpl->GetStringList().Count();
    pFormImpl->GetStringList().Insert( new String( aEmptyStr ), nEntryCnt );
    pFormImpl->GetValueList().Insert( new String( aValue ), nEntryCnt );
    if( bLBEntrySelected )
        pFormImpl->GetSelectedList().Insert( nEntryCnt,
                                pFormImpl->GetSelectedList().Count() );
}

void SwHTMLParser::InsertSelectText()
{
    ASSERT( bSelect, "keine Select" );
    ASSERT( pFormImpl && pFormImpl->GetFCompPropSet().is(),
            "kein Select-Control" );

    sal_uInt16 nEntryCnt = pFormImpl->GetStringList().Count();
    if( nEntryCnt )
    {
        String& rText = *pFormImpl->GetStringList()[nEntryCnt-1];

        if( aToken.Len() && ' '==aToken.GetChar( 0 ) )
        {
            xub_StrLen nLen = rText.Len();
            if( !nLen || ' '==rText.GetChar( nLen-1 ))
                aToken.Erase( 0, 1 );
        }
        if( aToken.Len() )
            rText += aToken;
    }
}

