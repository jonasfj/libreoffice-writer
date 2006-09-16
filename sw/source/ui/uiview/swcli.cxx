/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: swcli.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 23:23:17 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"

#ifndef _COM_SUN_STAR_EMBED_NOVISUALAREASIZEEXCEPTION_HPP_
#include <com/sun/star/embed/NoVisualAreaSizeException.hpp>
#endif


#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#ifndef _EDTWIN_HXX
#include <edtwin.hxx>
#endif
#ifndef _SWCLI_HXX
#include <swcli.hxx>
#endif

#include <toolkit/helper/vclunohelper.hxx>

using namespace com::sun::star;

SwOleClient::SwOleClient( SwView *pView, SwEditWin *pWin, const svt::EmbeddedObjectRef& xObj ) :
    SfxInPlaceClient( pView, pWin, xObj.GetViewAspect() ), bInDoVerb( FALSE ),
    bOldCheckForOLEInCaption( pView->GetWrtShell().IsCheckForOLEInCaption() )
{
    SetObject( xObj.GetObject() );
}

void SwOleClient::RequestNewObjectArea( Rectangle& aLogRect )
{
    //Der Server moechte die Clientgrosse verandern.
    //Wir stecken die Wunschgroesse in die Core. Die Attribute des Rahmens
    //werden auf den Wunschwert eingestellt. Dieser Wert wird also auch an
    //den InPlaceClient weitergegeben.
    //Die Core aktzeptiert bzw. formatiert die eingestellten Werte nicht
    //zwangslaeufig. Wenn der Ole-Frm formatiert wurde wird das CalcAndSetScale()
    //der WrtShell gerufen. Dort wird ggf. die Scalierung des SwOleClient
    //eingestellt.

    SwWrtShell &rSh  = ((SwView*)GetViewShell())->GetWrtShell();

    // Falls der Server nicht mit dem Ma�stab des Containers syncronisiert
    // ist, wird durch das Setzen und Abfragen der VisArea die Server
    // Einstellung ermittelt.
    // Niemals Koordinatentransformationen mit dem Rectangle vornehmen!!!

    /*
    Window *pWin = rSh.GetWin();
    Rectangle aLogRect( PixelObjVisAreaToLogic( rObjRect ) );
    if ( pEnv->GetObjAreaPixel().GetSize() != rObjRect.GetSize() )
        // sichtbaren Ausschnitt setzen und abfragen
        aLogRect = pIPObj->SetGetVisArea( aLogRect );

    Size aBla( aLogRect.GetSize() );
    aBla.Width() = Fraction( aBla.Width()  ) * GetEnv()->GetScaleWidth();
    aBla.Height()= Fraction( aBla.Height() ) * GetEnv()->GetScaleHeight();
    aLogRect.SetSize( aBla );

    const MapMode aTmp( pIPObj->GetMapUnit() );
    aLogRect.SetSize( pWin->LogicToLogic( aLogRect.GetSize(), aTmp, MAP_TWIP ) );

    //#52207# Hat sich die Position wirklich geaendert (Umrechnungsfehler vermeiden)?
    if ( GetObjAreaPixel().TopLeft() != rObjRect.TopLeft() )
        aLogRect.SetPos ( pWin->PixelToLogic( rObjRect.TopLeft()));
    else
        aLogRect.SetPos( Point( LONG_MIN, LONG_MIN ) );*/

    if ( aLogRect.GetSize() != GetScaledObjArea().GetSize() )
    {
        // size has changed, so first change visual area of the object before we resize its view
        // without this the object always would be scaled - now it has the choice

        // TODO/LEAN: getMapUnit can switch object to running state
        MapMode aObjectMap( VCLUnoHelper::UnoEmbed2VCLMapUnit( GetObject()->getMapUnit( GetAspect() ) ) );
        MapMode aClientMap( GetEditWin()->GetMapMode().GetMapUnit() );

        Size aNewObjSize( Fraction( aLogRect.GetWidth() ) / GetScaleWidth(),
                          Fraction( aLogRect.GetHeight() ) / GetScaleHeight() );

        // convert to logical coordinates of the embedded object
        Size aNewSize = GetEditWin()->LogicToLogic( aNewObjSize, &aClientMap, &aObjectMap );
        GetObject()->setVisualAreaSize( GetAspect(), awt::Size( aNewSize.Width(), aNewSize.Height() ) );
    }

    rSh.StartAllAction();
    rSh.RequestObjectResize( SwRect( aLogRect ), GetObject());
    rSh.EndAllAction();

    SwRect aFrm( rSh.GetAnyCurRect( RECT_FLY_EMBEDDED,     0, GetObject() )),
           aPrt( rSh.GetAnyCurRect( RECT_FLY_PRT_EMBEDDED, 0, GetObject() ));
    aLogRect.SetPos( aPrt.Pos() + aFrm.Pos() );
    aLogRect.SetSize( aPrt.SSize() );
}

void SwOleClient::ObjectAreaChanged()
{
    SwWrtShell &rSh  = ((SwView*)GetViewShell())->GetWrtShell();
    SwRect aFrm( rSh.GetAnyCurRect( RECT_FLY_EMBEDDED,     0, GetObject() )),
           aPrt( rSh.GetAnyCurRect( RECT_FLY_PRT_EMBEDDED, 0, GetObject() ));
    if ( !aFrm.IsOver( rSh.VisArea() ) )
        rSh.MakeVisible( aFrm );
}

void SwOleClient::ViewChanged()
{
    if ( bInDoVerb )
        return;

    SwWrtShell &rSh  = ((SwView*)GetViewShell())->GetWrtShell();
    Window 	   *pWin = rSh.GetWin();

    //Einstellen der Groesse des Objektes in der Core. Die Scalierung muss
    //beruecksichtigt werden. Rueckwirkung auf das Objekt werden von
    //CalcAndSetScale() der WrtShell beruecksichtig, wenn die Groesse/Pos des
    //Rahmens in der Core sich veraendert.

    // TODO/LEAN: getMapUnit can switch object to running state
    awt::Size aSz;
    try
    {
        aSz = GetObject()->getVisualAreaSize( GetAspect() );
    }
    catch( embed::NoVisualAreaSizeException& )
    {
        // Nothing will be done
    }
    
    Size aVisSize( aSz.Width, aSz.Height );

    // Bug 24833: solange keine vernuenftige Size vom Object kommt,
    // 				kann nichts skaliert werden
    if( !aVisSize.Width() || !aVisSize.Height() )
        return;

    // first convert to TWIPS before scaling, because scaling factors are calculated for
    // the TWIPS mapping and so they will produce the best results if applied to TWIPS based
    // coordinates
    const MapMode aMyMap ( MAP_TWIP );
    const MapMode aObjMap( VCLUnoHelper::UnoEmbed2VCLMapUnit( GetObject()->getMapUnit( GetAspect() ) ) );
    aVisSize = OutputDevice::LogicToLogic( aVisSize, aObjMap, aMyMap );

    aVisSize.Width() = Fraction( aVisSize.Width()  ) * GetScaleWidth();
    aVisSize.Height()= Fraction( aVisSize.Height() ) * GetScaleHeight();

    SwRect aRect( Point( LONG_MIN, LONG_MIN ), aVisSize );
    rSh.LockView( TRUE );	//Scrollen im EndAction verhindern
    rSh.StartAllAction();
    rSh.RequestObjectResize( aRect, GetObject() );
    rSh.EndAllAction();
    rSh.LockView( FALSE );
}

void SwOleClient::MakeVisible()
{
    const SwWrtShell &rSh  = ((SwView*)GetViewShell())->GetWrtShell();
    rSh.MakeObjVisible( GetObject() );
}
