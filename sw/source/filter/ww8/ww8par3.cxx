/*************************************************************************
 *
 *  $RCSfile: ww8par3.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: cmc $ $Date: 2002-03-01 09:30:56 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

#ifndef _SFXITEMITER_HXX //autogen
#include <svtools/itemiter.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>    
#endif
#ifndef _SV_OUTDEV_HXX
#include <vcl/outdev.hxx>
#endif

#ifndef _TOOLKIT_UNOHLP_HXX
#include <toolkit/helper/vclunohelper.hxx>
#endif

#ifndef _COM_SUN_STAR_FORM_XFORMSSUPPLIER_HPP_
#include <com/sun/star/form/XFormsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORM_HPP_
#include <com/sun/star/form/XForm.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XIMAGEPRODUCERSUPPLIER_HPP_
#include <com/sun/star/form/XImageProducerSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMCONTROLLER_HPP_
#include <com/sun/star/form/XFormController.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMCOMPONENT_HPP_
#include <com/sun/star/form/XFormComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMCONTROLLERLISTENER_HPP_
#include <com/sun/star/form/XFormControllerListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XCONNECTABLESHAPE_HPP_
#include <com/sun/star/drawing/XConnectableShape.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XCONNECTORSHAPE_HPP_
#include <com/sun/star/drawing/XConnectorShape.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XCONTROLSHAPE_HPP_
#include <com/sun/star/drawing/XControlShape.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEALIGNER_HPP_
#include <com/sun/star/drawing/XShapeAligner.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEGROUP_HPP_
#include <com/sun/star/drawing/XShapeGroup.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEDESCRIPTOR_HPP_
#include <com/sun/star/drawing/XShapeDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XUNIVERSALSHAPEDESCRIPTOR_HPP_
#include <com/sun/star/drawing/XUniversalShapeDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEMIRROR_HPP_
#include <com/sun/star/drawing/XShapeMirror.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEARRANGER_HPP_
#include <com/sun/star/drawing/XShapeArranger.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGE_HPP_
#include <com/sun/star/drawing/XDrawPage.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPES_HPP_
#include <com/sun/star/drawing/XShapes.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_VERTORIENTATION_HPP_
#include <com/sun/star/text/VertOrientation.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TEXTCONTENTANCHORTYPE_HPP_
#include <com/sun/star/text/TextContentAnchorType.hpp>
#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVX_FONTITEM_HXX
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_LRSPITEM_HXX //autogen
#include <svx/lrspitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX
#include <svx/colritem.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX
#include <svx/wghtitem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX
#include <svx/crsditem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX
#include <svx/udlnitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX
#include <svx/postitem.hxx>
#endif
#ifndef _MSOCXIMEX_HXX
#include <svx/msocximex.hxx>
#endif

#ifndef _ERRHDL_HXX //autogen
#include <errhdl.hxx>
#endif
#ifndef _UNOOBJ_HXX //autogen
#include <unoobj.hxx>
#endif
#ifndef _DOC_HXX //autogen
#include <doc.hxx>
#endif
#ifndef _DOCARY_HXX
#include <docary.hxx>
#endif
#ifndef _SWDOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _NUMRULE_HXX //autogen
#include <numrule.hxx>
#endif
#ifndef _PARATR_HXX //autogen
#include <paratr.hxx>
#endif
#ifndef _CHARATR_HXX //autogen
#include <charatr.hxx>
#endif
#ifndef _CHARFMT_HXX //autogen
#include <charfmt.hxx>
#endif
#ifndef _NDTXT_HXX //autogen
#include <ndtxt.hxx>
#endif

#ifndef _WW8PAR_HXX
#include "ww8par.hxx"
#endif
#ifndef _WW8PAR2_HXX
#include "ww8par2.hxx"	// wg. Listen-Attributen in Styles
#endif

#ifndef C2U
#define C2U(s) rtl::OUString::createFromAscii(s)
#endif

#define WW8_DFLT_EDIT_WIDTH 2750
#define WW8_DFLT_EDIT_HEIGHT 500
#define WW8_DFLT_CHECKBOX_WIDTH 320
#define WW8_DFLT_CHECKBOX_HEIGHT 320

static sal_Char sWW8_form[] = "WW-Standard";
static sal_Char sWW8_edit[] = "TextBox";
static sal_Char sWW8_checkbox[] = "CheckBox";

using namespace com::sun::star; 
//-----------------------------------------
//			  UNO-Controls
//-----------------------------------------

void SwWW8ImplReader::BuildInputField(sal_uInt16 nType)
{
    if( nIniFlags & WW8FL_NO_VCCONTROLS )
    {
#ifdef DEBUG
        rDoc.Insert( *pPaM, sal_Unicode ('#') );
        static long l;
        l++;
#endif
        return;
    }

    if( !pDrawModel )				// MIB: Braucht man das?
        GrafikCtor();
    if( !pFormImpl )
        pFormImpl = new SwMSConvertControls(rDoc.GetDocShell(),pPaM);

    const uno::Reference< lang::XMultiServiceFactory > & rServiceFactory =
        pFormImpl->GetServiceFactory();
    if(!rServiceFactory.is())
        return;

    rtl::OUString sType;
    switch( (SwWw8ControlType)nType )
    {
        case WW8_CT_CHECKBOX:
            sType = C2U("CheckBox");
            break;
    //	case WW8_CT_EDIT:
        default:
            // see change in ../html/htmlform.cxx as of 1999/11/25 11:20:16
            // made by O.Specht
            sType = C2U("TextField");
            break;
    }

    rtl::OUString sServiceName(C2U("com.sun.star.form.component."));
    sServiceName += sType;
    uno::Reference< uno::XInterface >  xCreate = 
        rServiceFactory->createInstance( sServiceName );
    if( !xCreate.is() )
        return;

    uno::Reference< form::XFormComponent > xFComp( xCreate, uno::UNO_QUERY );
    if( !xFComp.is() )
        return;

    awt::Size aSz;
    rtl::OUString sName;
    sal_uInt16 nControl;
    switch( (SwWw8ControlType)nType )
    {
        case WW8_CT_CHECKBOX:
            aSz.Width = WW8_DFLT_CHECKBOX_WIDTH;
            aSz.Height = WW8_DFLT_CHECKBOX_HEIGHT;
            sName = C2U(sWW8_checkbox);
            nControl = pFormImpl->GetCheckboxNum();
            break;
//      case WW8_CT_EDIT:
        default:
            aSz.Width = WW8_DFLT_EDIT_WIDTH; // etwas 20 Zeichen
            aSz.Height = WW8_DFLT_EDIT_HEIGHT;
            sName = C2U(sWW8_edit);
            nControl = pFormImpl->GetEditNum();
            break;
    }

    uno::Reference< beans::XPropertySet > xPropSet( xCreate, uno::UNO_QUERY );

    sName += rtl::OUString::valueOf(nControl);
    uno::Any aTmp;
    aTmp <<= sName;
    xPropSet->setPropertyValue(C2U("Name"), aTmp );

    pFormImpl->InsertControl(xFComp, aSz, 0,FALSE);
}

//cmc, OCX i.e. word 97 form controls
eF_ResT SwWW8ImplReader::Read_F_OCX( WW8FieldDesc*, String& )
{
    if( bObj && nPicLocFc )
        nObjLocFc = nPicLocFc;
    bEmbeddObj = TRUE;
    return FLD_TEXT;
}

eF_ResT SwWW8ImplReader::Read_F_FormTextBox( WW8FieldDesc* pF, String& rStr )
{
    if (0x01 == rStr.GetChar( pF->nLCode-1 ))
    {
        WW8FormulaEditBox aFormula(*this);
        if (ImportFormulaControl(aFormula,pF->nSCode+pF->nLCode-1, WW8_CT_EDIT))
        {
            if( !pFormImpl )
                pFormImpl = new SwMSConvertControls(rDoc.GetDocShell(),pPaM);
            /* #80205#
            Here we have a small complication. This formula control contains
            the default text that is displayed if you edit the form field in
            the "default text" area. But MSOffice does not display that
            information, instead it display the result of the field,
            MSOffice just uses the default text of the control as its
            initial value for the displayed default text. So we will swap in
            the field result into the formula here in place of the default
            text.
            */
            aFormula.sDefault = GetFieldResult(pF);

            /* #80205#
            And also a blank TextBox is indicated by 5 of these
            placeholder chars, convert a field result of this stuff
            into an empty string.
            */
            static sal_Unicode __READONLY_DATA aFormTextBoxBlank[] =
            {
                0x2002,0x2002,0x2002,0x2002,0x2002
            };
            if (aFormula.sDefault == aFormTextBoxBlank)
                aFormula.sDefault.Erase();

            if (pFormImpl->InsertFormula(aFormula))
                return FLD_OK;
        }
    }
    ASSERT(0,"New Formula Code Failed!\n");
    BuildInputField(WW8_CT_EDIT);
    return FLD_OK;

}

eF_ResT SwWW8ImplReader::Read_F_FormCheckBox( WW8FieldDesc* pF, String& rStr )
{
    if (0x01 == rStr.GetChar( pF->nLCode-1 ))
    {
        WW8FormulaCheckBox aFormula(*this);
        if (ImportFormulaControl(aFormula,pF->nSCode+pF->nLCode-1,
            WW8_CT_CHECKBOX))
        {
            if( !pFormImpl )
                pFormImpl = new SwMSConvertControls(rDoc.GetDocShell(),pPaM);
            if (pFormImpl->InsertFormula(aFormula))
                return FLD_OK;
        }
    }
    ASSERT(!this,"New Formula Code Failed!\n");
    BuildInputField(WW8_CT_CHECKBOX);
    return FLD_OK;
}

eF_ResT SwWW8ImplReader::Read_F_FormListBox( WW8FieldDesc*, String& )
{
    return FLD_TAGIGN;
}


void SwWW8ImplReader::DeleteFormImpl()
{
    delete pFormImpl, pFormImpl = 0;
}

//----------------------------------------------------------------------------
//			WW8ListManager			oeffentliche Methoden stehen ganz am Ende
//------------------------- ============ --------------- ============ --------



// Hilfs-Deklarationen ///////////////////////////////////////////////////////
//
// Style Id's for each level
typedef sal_uInt16 WW8aIdSty[    nWW8MaxListLevel];
// Zeichenattribute aus GrpprlChpx
typedef SfxItemSet* WW8aISet[nWW8MaxListLevel];
// Zeichen Style Pointer
typedef SwCharFmt* WW8aCFmt[ nWW8MaxListLevel];

struct WW8LST	// nur DIE Eintraege, die WIR benoetigen!
{
    WW8aIdSty	aIdSty;		// Style Id's for each level,
                            //   nIStDNil if no style linked
    sal_uInt32		nIdLst;		// Unique List ID
    sal_uInt32		nTplC;		// Unique template code - Was ist das bloss?
    sal_Bool		bSimpleList:1;	// Flag: Liste hat nur EINEN Level
    sal_Bool		bRestartHdn:1;	// WW6-Kompatibilitaets-Flag:
                                                        //   true if the list should start numbering over
};														//   at the beginning of each section

struct WW8LFO	// nur DIE Eintraege, die WIR benoetigen!
{
    SwNumRule*	    pNumRule;	// Parent NumRule
    sal_uInt32		nIdLst;		// Unique List ID
    sal_uInt8		nLfoLvl;	// count of levels whose format is overridden
    sal_Bool		bSimpleList;
};

#define nIStDNil 0x0FFF	// d.h. KEIN Style ist an den Level gelinkt
#define nSizeOfLST 0x1C

struct WW8LVL	// nur DIE Eintraege, die WIR benoetigen!
{
    long	nStartAt;		// start at value for this value
    long	nV6DxaSpace;// Ver6-Compatible: min Space between Num anf ::com::sun::star::text::Paragraph
    long	nV6Indent;	// Ver6-Compatible: Breite des Prefix Textes; ggfs. zur
                        // Definition d. Erstzl.einzug nutzen!
    // Absatzattribute aus GrpprlPapx
    sal_uInt16	nDxaLeft;				// linker Einzug
    short	nDxaLeft1;			// Erstzeilen-Einzug

    sal_uInt8	nNFC;				// number format code
    sal_uInt8	aOfsNumsXCH[nWW8MaxListLevel];// Offset der Feldkodes im Num-X-String
    sal_uInt8	nLenGrpprlChpx;	// length, in bytes, of the LVL's grpprlChpx
    sal_uInt8	nLenGrpprlPapx;	// length, in bytes, of the LVL's grpprlPapx
    sal_uInt8	nAlign:	2;	// alignment (left, right, centered) of the number
    sal_Bool	bLegal:	1;	// egal
    sal_Bool	bNoRest:1;	// egal
    sal_Bool	bV6Prev:1;	// Ver6-Compatible: number will include previous levels
    sal_Bool	bV6PrSp:1;	// Ver6-Compatible: egal
    sal_Bool	bV6:		1;	// falls sal_True, beachte die V6-Compatible Eintraege!
    sal_uInt8	bDummy:	1;	// (macht das Byte voll)

};

struct WW8LFOLVL
{
    long nStartAt;			// start-at value if bFormat==false and bStartAt == true
                                            // (if bFormat==true, the start-at is stored in the LVL)
    sal_uInt8 nLevel;				// the level to be overridden
    // dieses Byte ist _absichtlich_ nicht in das folgende Byte hineingepackt	!!
    // (siehe Kommentar unten bei struct WW8LFOInfo)

    sal_Bool bStartAt :1;		// true if the start-at value is overridden
    sal_Bool bFormat	:1;		// true if the formatting is overriden
};



// in den ListenInfos zu speichernde Daten ///////////////////////////////////
//
struct WW8LSTInfo	// sortiert nach nIdLst (in WW8 verwendete Listen-Id)
{
    WW8aIdSty	aIdSty;			 // Style Id's for each level
    WW8aISet	aItemSet;		 // Zeichenattribute aus GrpprlChpx
    WW8aCFmt	aCharFmt;		 // Zeichen Style Pointer

    SwNumRule*	pNumRule;		 // Zeiger auf entsprechende Listenvorlage im Writer
    sal_uInt32		nIdLst;			 // WW8Id dieser Liste
    sal_uInt16		nStreamPos;	 // Reihenfolge des Auftretens des LST im PLCF LST
                                                     // siehe auch "WW8ListManager::GetLSTByStreamPos()"
    sal_Bool		bSimpleList:1;// Flag, ob diese NumRule nur einen Level verwendet
    sal_Bool		bUsedInDoc :1;// Flag, ob diese NumRule im Doc verwendet wird,
                                                     //   oder beim Reader-Ende geloescht werden sollte

    // Hilfs Ctor, benoetigt innerhalb von WW8ListManager::GetLSTByListId()
    WW8LSTInfo(sal_uInt32 nIdLst_): nIdLst( nIdLst_){}

    // normaler Ctor
    WW8LSTInfo(SwNumRule* pNumRule_, WW8LST& aLST, sal_uInt16 nStreamPos_):
        nIdLst(			aLST.nIdLst 		),
        nStreamPos( nStreamPos_			),
        pNumRule(		pNumRule_				),
        bSimpleList(aLST.bSimpleList),
        bUsedInDoc(	0 )
    {
        memcpy( aIdSty, aLST.aIdSty, sizeof( aIdSty   ));
        memset(&aItemSet, 0,  sizeof( aItemSet ));
        memset(&aCharFmt, 0,  sizeof( aCharFmt ));
    }

    sal_Bool operator==( const WW8LSTInfo& rEntry ) const
    {   return nIdLst == rEntry.nIdLst; }
    sal_Bool operator<( const WW8LSTInfo& rEntry ) const
    {   return nIdLst < rEntry.nIdLst;  }
};

SV_IMPL_OP_PTRARR_SORT(WW8LSTInfos, WW8LSTInfo_Ptr)


// in den ListenFormatOverrideInfos zu speichernde Daten /////////////////////
//
struct WW8LFOInfo	// unsortiert, d.h. Reihenfolge genau wie im WW8 Stream
{
    SwNumRule* pNumRule;		 // Zeiger auf entsprechende Listenvorlage im Writer
                                                     // entweder: Liste in LSTInfos oder eigene Liste
                                                     // (im Ctor erstmal die aus den LSTInfos merken)

    sal_uInt32	nIdLst;			 // WW8-Id der betreffenden Liste
    sal_uInt8	nLfoLvl;			 // count of levels whose format is overridden
    // Ja, ich natuerlich koennten wir nLfoLvl (mittels :4) noch in das folgende
    // Byte mit hineinpacken, doch waere das eine ziemliche Fehlerquelle,
    // an dem Tag, wo MS ihr Listenformat auf mehr als 15 Level aufbohren.

    sal_Bool	bOverride	:1;// Flag, ob die NumRule nicht in pLSTInfos steht,
                                                     //   sondern fuer pLFOInfos NEU angelegt wurde
    sal_Bool	bSimpleList:1;// Flag, ob diese NumRule nur einen Level verwendet
    sal_Bool	bUsedInDoc	:1;// Flag, ob diese NumRule im Doc verwendet wird,
                                                     //   oder beim Reader-Ende geloescht werden sollte
    sal_Bool	bLSTbUIDSet	:1;// Flag, ob bUsedInDoc in pLSTInfos gesetzt wurde,
                                                     //   und nicht nochmals gesetzt zu werden braucht
    WW8LFOInfo( const WW8LFO& rLFO ):
        nIdLst(			rLFO.nIdLst				),
        pNumRule(		rLFO.pNumRule			),// hier bloss die Parent NumRule
        nLfoLvl(		rLFO.nLfoLvl			),
        bOverride(	rLFO.nLfoLvl ? sal_True : sal_False	),
        bSimpleList(rLFO.bSimpleList			),
        bUsedInDoc(				0				),
        bLSTbUIDSet(			0				){}
};

SV_IMPL_PTRARR( WW8LFOInfos, WW8LFOInfo_Ptr );


// Hilfs-Methoden ////////////////////////////////////////////////////////////
//

// finden der Sprm-Parameter-Daten, falls Sprm im Grpprl enthalten
sal_uInt8* WW8ListManager::GrpprlHasSprm(sal_uInt16 nId, sal_uInt8& rSprms, 
    sal_uInt8 nLen)
{
    sal_uInt8* pSprms = &rSprms;
    for(USHORT i=0; i<nLen; )
    {
        sal_uInt8 nDelta;
        sal_uInt16 nAktId = WW8GetSprmId(rFib.nVersion, pSprms, &nDelta );
        if( nAktId == nId ) // Sprm found
            return pSprms + 1 + nDelta + WW8SprmDataOfs( nId );
                                                // gib Zeiger auf Daten
        USHORT x = WW8GetSprmSize(rFib.nVersion, pSprms, &nAktId );
        i += x;
        pSprms += x;
    }
    return 0;							// Sprm not found
}

// Zugriff ueber die List-Id des LST Eintrags
WW8LSTInfo* WW8ListManager::GetLSTByListId( sal_uInt32 nIdLst ) const
{
    WW8LSTInfo aEntry( nIdLst );
    sal_uInt16 nPos;
    if( !pLSTInfos->Seek_Entry( &aEntry, &nPos ) ) return 0;
    return pLSTInfos->GetObject( nPos );
}

// Zugriff ueber Reihenfolge der LST Eintraege im PLCF LST (Null basiert)
WW8LSTInfo* WW8ListManager::GetLSTByStreamPos( sal_uInt16 nStreamPos ) const
{
    if( !pLSTInfos ) return 0;
    WW8LSTInfo* pEntry;
    for(sal_uInt16 nPos = nLSTInfos; nPos; )
    {
        pEntry = pLSTInfos->GetObject( --nPos );
        if( pEntry && ( pEntry->nStreamPos == nStreamPos )) return pEntry;
    }
    return 0;
}

sal_Bool WW8ListManager::ReadLVL(SwNumFmt& rNumFmt, SfxItemSet*& rpItemSet, 
    sal_uInt16 nLevelStyle, sal_Bool bSetStartNo )
{
    sal_uInt8	    aBits1;
    sal_uInt8		nUpperLevel = 0;	// akt. Anzeigetiefe fuer den Writer
    sal_uInt16		nStartNo    = 0;	// Start-Nr. fuer den Writer
    SvxExtNumType	eType;				// Writer-Num-Typ
    SvxAdjust		eAdj;				// Ausrichtung (Links/rechts/zent.)
    sal_Unicode		cBullet;
    String			sPrefix;
    String			sPostfix;
    WW8LVL			aLVL;
    //
    // 1. LVLF einlesen
    //
    memset(&aLVL, 0, sizeof( aLVL ));
    rSt >> aLVL.nStartAt;
    rSt >> aLVL.nNFC;
    rSt >> aBits1;
    if( 0 != rSt.GetError() ) return sal_False;
    aLVL.nAlign = (aBits1 & 0x03);
    if( aBits1 & 0x10 ) aLVL.bV6Prev	= sal_True;
    if( aBits1 & 0x20 ) aLVL.bV6PrSp	= sal_True;
    if( aBits1 & 0x40 ) aLVL.bV6		= sal_True;
    sal_Bool bLVLOkB = sal_True;
    for(sal_uInt8 nLevelB = 0; nLevelB < nWW8MaxListLevel; nLevelB++)
    {
        rSt >> aLVL.aOfsNumsXCH[ nLevelB ];
        if( 0 != rSt.GetError() )
        {
            bLVLOkB = sal_False;
            break;
        }
        if( !nUpperLevel && !aLVL.aOfsNumsXCH[ nLevelB ] )
        {
            nUpperLevel = nLevelB;
        }
    }
    if( !bLVLOkB ) 
        return sal_False;

    // falls kein NULL als Terminierungs-Char kam,
    // ist die Liste voller Indices, d.h. alle Plaetze sind besetzt,
    // also sind alle Level anzuzeigen
    if( !nUpperLevel )
        nUpperLevel = nWW8MaxListLevel;


    rSt.SeekRel( 1 );
    rSt	>> aLVL.nV6DxaSpace;
    rSt >> aLVL.nV6Indent;
    rSt >> aLVL.nLenGrpprlChpx;
    rSt >> aLVL.nLenGrpprlPapx;
    rSt.SeekRel( 2 );
    if( 0 != rSt.GetError()) return sal_False;

    //
    // 2. ggfs. PAPx einlesen und nach Einzug-Werten suchen
    //
    if( aLVL.nLenGrpprlPapx )
    {
        sal_uInt8 aGrpprlPapx[ 255 ];
        if(aLVL.nLenGrpprlPapx != rSt.Read(&aGrpprlPapx,aLVL.nLenGrpprlPapx)) 
            return sal_False;
        // "sprmPDxaLeft"  pap.dxaLeft;dxa;word;
        sal_uInt8* pSprm;
        if( (pSprm = GrpprlHasSprm(0x840F,aGrpprlPapx[0],aLVL.nLenGrpprlPapx)) )
        {
            short nDxaLeft = SVBT16ToShort( pSprm );
            aLVL.nDxaLeft = (0 < nDxaLeft) ? (sal_uInt16)nDxaLeft
                            : (sal_uInt16)(-nDxaLeft);
        }
        // "sprmPDxaLeft1" pap.dxaLeft1;dxa;word;
        if( (pSprm = GrpprlHasSprm(0x8411,aGrpprlPapx[0],aLVL.nLenGrpprlPapx)) )
            aLVL.nDxaLeft1 = SVBT16ToShort(  pSprm );
    }
    //
    // 3. ggfs. CHPx einlesen und
    //
    if( aLVL.nLenGrpprlChpx )
    {
        sal_uInt8 aGrpprlChpx[ 255 ];
        memset(&aGrpprlChpx, 0, sizeof( aGrpprlChpx ));
        if(aLVL.nLenGrpprlChpx != rSt.Read(&aGrpprlChpx, aLVL.nLenGrpprlChpx)) 
            return sal_False;
        // neues ItemSet fuer die Zeichenattribute anlegen
        rpItemSet = new SfxItemSet( rDoc.GetAttrPool(), RES_CHRATR_BEGIN,
            RES_CHRATR_END - 1 );

        // Reader-ItemSet-Pointer darauf zeigen lassen
        rReader.SetAktItemSet( rpItemSet );
        // Reader-Style auf den Style dieses Levels setzen
        sal_uInt16 nOldColl = rReader.GetNAktColl();
        sal_uInt16 nNewColl = nLevelStyle;
        if( nIStDNil == nNewColl )
            nNewColl = 0;
        rReader.SetNAktColl( nNewColl );

        // Nun den GrpprlChpx einfach durchnudeln: die Read_xy() Methoden
        // in WW8PAR6.CXX rufen ganz normal ihr NewAttr() oder GetFmtAttr()
        // und diese merken am besetzten Reader-ItemSet-Pointer, dass dieser
        // spezielle ItemSet relevant ist - und nicht ein Stack oder Style!
        short nLen		= aLVL.nLenGrpprlChpx;
        sal_uInt8* pSprms1	= &aGrpprlChpx[0];
        while( 0 < nLen )
        {
            sal_uInt16 nL1 = rReader.ImportSprm( pSprms1 );
            nLen      -= nL1;
            pSprms1   += nL1;
        }
        // Reader-ItemSet-Pointer und Reader-Style zuruecksetzen
        rReader.SetAktItemSet( 0 );
        rReader.SetNAktColl( nOldColl );
    }
    //
    // 4. den Nummerierungsstring einlesen: ergibt Prefix und Postfix
    //
    String sNumString( WW8Read_xstz(rSt, 0, FALSE) );

    //
    // 5. gelesene Werte in Writer Syntax umwandeln
    //
    if( 0 <= aLVL.nStartAt )
        nStartNo = (sal_uInt16)aLVL.nStartAt;

    switch( aLVL.nNFC )
    {
        case   0: 
            eType = SVX_NUM_ARABIC;				
            break;
        case   1: 
            eType = SVX_NUM_ROMAN_UPPER;			
            break;
        case   2: 
            eType = SVX_NUM_ROMAN_LOWER;			
            break;
        case   3: 
            eType = SVX_NUM_CHARS_UPPER_LETTER_N;	
            break;
        case   4: 
            eType = SVX_NUM_CHARS_LOWER_LETTER_N;	
            break;
        case   5: 
            // eigentlich: ORDINAL
            eType = SVX_NUM_ARABIC;				
            break;
        case  23: 
            eType = SVX_NUM_CHAR_SPECIAL;			
            break;
        case 255: 
            eType = SVX_NUM_NUMBER_NONE;			
            break;
         default: 
            // take default
            eType = SVX_NUM_ARABIC;				
            break;
    }

    if( SVX_NUM_CHAR_SPECIAL == eType )
    {
        cBullet = sNumString.Len() ? sNumString.GetChar( 0 ) : 0x2190;

        if( !cBullet )  // unsave control code?
            cBullet = 0x2190;

        sPrefix  = aEmptyStr;
        sPostfix = aEmptyStr;
    }
    else
    {
        /*
        #83154#, #82192#, ##173##
        Our aOfsNumsXCH seems generally to be an array that contains the
        offset into sNumString of locations where the numbers should be
        filled in, so if the first "fill in a number" slot is greater than
        1 there is a "prefix" before the number
        */
        if(aLVL.aOfsNumsXCH[0] <= 1)
            sPrefix = aEmptyStr;
        else
            sPrefix = sNumString.Copy(0,aLVL.aOfsNumsXCH[0]-1);

        if(nUpperLevel && (sNumString.Len() > aLVL.aOfsNumsXCH[nUpperLevel-1]))
            sPostfix = sNumString.Copy(aLVL.aOfsNumsXCH[nUpperLevel-1]);
        else
            sPostfix.Erase();
    }

    switch( aLVL.nAlign )
    {
        case 0: 
            eAdj = SVX_ADJUST_LEFT;		
            break;
        case 1: 
            eAdj = SVX_ADJUST_CENTER;	
            break;
        case 2: 
            eAdj = SVX_ADJUST_RIGHT;	
            break;
        case 3: 
            // Writer here cannot do block justification
            eAdj = SVX_ADJUST_LEFT;		
            break;
         default:
            // undefied value
            ASSERT( !this, "Value of aLVL.nAlign is not supported" );
            // take default
            eAdj = SVX_ADJUST_LEFT;
            break;
    }

    // 6. entsprechendes NumFmt konfigurieren
    if( bSetStartNo )
        rNumFmt.SetStart( nStartNo );
    rNumFmt.SetNumberingType(eType);
    rNumFmt.SetNumAdjust( eAdj );

    if( SVX_NUM_CHAR_SPECIAL == eType )
    {
        // first character of the Prefix-Text is the Bullet
        rNumFmt.SetBulletChar( cBullet );
        // Don't forget: unten, nach dem Bauen eventueller Styles auch noch
        // SetBulletFont() rufen !!!
    }
    else
    {
        // reminder: Garnix ist default Prefix
        if( sPrefix.Len() )
            rNumFmt.SetPrefix( sPrefix );
        // reminder: Point is default Postfix
        rNumFmt.SetSuffix( sPostfix );
        rNumFmt.SetIncludeUpperLevels( nUpperLevel );
    }

    rNumFmt.SetAbsLSpace( aLVL.nDxaLeft );

    if( 0 < aLVL.nDxaLeft1 )
        aLVL.nDxaLeft1 = aLVL.nDxaLeft1 * -1;
    rNumFmt.SetFirstLineOffset( aLVL.nDxaLeft1 );

    return sal_True;
}

void WW8ListManager::AdjustLVL( sal_uInt8 nLevel, SwNumRule& rNumRule, 
    WW8aISet& rListItemSet, WW8aCFmt& rCharFmt, sal_Bool& bNewCharFmtCreated,
    String sPrefix )
{
    bNewCharFmtCreated = sal_False;
    SfxItemSet* pThisLevelItemSet;
    SfxItemSet* pLowerLevelItemSet;
    sal_uInt8        nIdenticalItemSetLevel;
    const SfxPoolItem* pItem;

    SwNumFmt aNumFmt  = rNumRule.Get( nLevel );

    pThisLevelItemSet = rListItemSet[ nLevel ];

    if( pThisLevelItemSet && pThisLevelItemSet->Count())
    {
        nIdenticalItemSetLevel = nWW8MaxListLevel;
        SfxItemIter aIter( *pThisLevelItemSet );
        for( sal_uInt8 nLowerLevel = 0; nLowerLevel < nLevel; nLowerLevel++)
        {
            pLowerLevelItemSet = rListItemSet[ nLowerLevel ];
            if(     pLowerLevelItemSet
                && (pLowerLevelItemSet->Count() == pThisLevelItemSet->Count()) )
            {
                nIdenticalItemSetLevel = nLowerLevel;
                sal_uInt16 nWhich = aIter.GetCurItem()->Which();
                while( sal_True )
                {
                    if(	 // ggfs. passenden pItem im pLowerLevelItemSet finden
                         (SFX_ITEM_SET != pLowerLevelItemSet->GetItemState(
                                            nWhich, sal_False, &pItem ) )
                        || // virtuellen "!=" Operator anwenden
                         (*pItem != *aIter.GetCurItem() ) )
                    // falls kein Item mit gleicher nWhich gefunden oder Werte
                    // der Items ungleich, Ungleichheit merken und abbrechen!
                    {
                        nIdenticalItemSetLevel = nWW8MaxListLevel;
                        break;
                    }
                    if( aIter.IsAtEnd() )
                        break;
                    nWhich = aIter.NextItem()->Which();
                }

                if( nIdenticalItemSetLevel != nWW8MaxListLevel )
                    break;
            }
        }

        SwCharFmt* pFmt;
        if( nWW8MaxListLevel == nIdenticalItemSetLevel )
        {
            // Style definieren
            String aName( sPrefix.Len() ? sPrefix : rNumRule.GetName() );
            (aName += 'z') += String::CreateFromInt32( nLevel );

            // const Wegcasten
            pFmt = rDoc.MakeCharFmt(aName, (SwCharFmt*)rDoc.GetDfltCharFmt());
            bNewCharFmtCreated = sal_True;
            // Attribute reinsetzen
            pFmt->SetAttr( *pThisLevelItemSet );
        }
        else
        {
            // passenden Style hier anhaengen
            pFmt = rCharFmt[ nIdenticalItemSetLevel ];
        }

        // merken
        rCharFmt[ nLevel ] = pFmt;

        //
        // Style an das NumFormat haengen
        //
        aNumFmt.SetCharFmt( pFmt );
    }
    //
    // ggfs. Bullet Font an das NumFormat haengen
    //
    if( SVX_NUM_CHAR_SPECIAL == aNumFmt.GetNumberingType() )
    {
        SwCharFmt* pFmt = aNumFmt.GetCharFmt();
        Font aFont;
        if(	!pFmt )
            aFont = SwNumRule::GetDefBulletFont();
        else
        {
            const SvxFontItem& rFontItem = pFmt->GetFont();
            aFont.SetFamily(    rFontItem.GetFamily()     );
            aFont.SetName(      rFontItem.GetFamilyName() );
            aFont.SetStyleName( rFontItem.GetStyleName()  );
            aFont.SetPitch(     rFontItem.GetPitch()      );
            aFont.SetCharSet(   rFontItem.GetCharSet()    );
        }
        aNumFmt.SetBulletFont( &aFont );
    }
    //
    // und wieder rein in die NumRule
    //
    rNumRule.Set(nLevel, aNumFmt);
}


sal_Bool WW8ListManager::LFOequaltoLST(WW8LFOInfo& rLFOInfo)
{
    sal_Bool bRes = sal_False;
    WW8LSTInfo* pLSTInfo = GetLSTByListId( rLFOInfo.nIdLst );
    if(    pLSTInfo
        && pLSTInfo->pNumRule
        && rLFOInfo.pNumRule
        && (rLFOInfo.nLfoLvl <= (pLSTInfo->bSimpleList ? 1 : nWW8MaxListLevel)))
    {
        const SwNumRule& rLSTRule = *pLSTInfo->pNumRule;
        const SwNumRule& rLFORule = *rLFOInfo.pNumRule;
        bRes = sal_True;
        for(sal_uInt16 nLevel = 0; bRes && (nLevel < rLFOInfo.nLfoLvl);++nLevel)
        {
            const SwNumFmt& rLSTNumFmt = rLSTRule.Get( nLevel );
            const SwNumFmt& rLFONumFmt = rLFORule.Get( nLevel );
            const SwCharFmt* pLSTCharFmt = rLSTNumFmt.GetCharFmt();
            const SwCharFmt* pLFOCharFmt = rLFONumFmt.GetCharFmt();
            if(	pLSTCharFmt && pLFOCharFmt )
            {
                // erst Char-Styles inhaltlich vergleichen,
                // ( siehe: ...\svtools\source\items\itemset.cxx )
                // falls identische Einstellungen, kurz den LFO-Style umhaengen,
                // damit '( rLSTNumFmt == rLFONumFmt )' funktioniert
                if( pLSTCharFmt->GetAttrSet() == pLFOCharFmt->GetAttrSet() )
                    ((SwNumFmt&)rLFONumFmt).SetCharFmt((SwCharFmt*)pLSTCharFmt);
                else
                {
                    bRes = sal_False;
                    break;
                }
            }
            if(		(    (0 == pLSTCharFmt)
                      != (0 == pLFOCharFmt)
                    )
                ||	!(rLSTNumFmt == rLFONumFmt) )
                bRes = sal_False;
            // vermurksten LFO-Style wieder zuruecksetzen
            if( pLFOCharFmt )
                ((SwNumFmt&)rLFONumFmt).SetCharFmt( (SwCharFmt*)pLFOCharFmt );
        }
    }
    return bRes;
}

SwNumRule* WW8ListManager::CreateNextRule(BOOL bSimple)
{
    // wird erstmal zur Bildung des Style Namens genommen
    String sPrefix(CREATE_CONST_ASC("WW8Num"));
    sPrefix += String::CreateFromInt32( nUniqueList++ );
    sal_uInt16 nRul =
        rDoc.MakeNumRule(rDoc.GetUniqueNumRuleName( &sPrefix ));
    SwNumRule* pMyNumRule = rDoc.GetNumRuleTbl()[nRul];
    pMyNumRule->SetAutoRule( sal_False );
    pMyNumRule->SetContinusNum( bSimple );
    return pMyNumRule;
}

// oeffentliche Methoden /////////////////////////////////////////////////////
//
WW8ListManager::WW8ListManager(SvStream& rSt_, SwWW8ImplReader& rReader_)
    : pLSTInfos( 0 ), pLFOInfos( 0 ), nLSTInfos( 0 ), nUniqueList(1), 
    nLFOInfos( 0 ), rSt( rSt_ ), rReader( rReader_ ), rDoc( rReader.GetDoc() ),
    rFib( rReader.GetFib() )
{
    // LST und LFO gibts erst ab WW8
    if(    ( 8 > rFib.nVersion ) 					
            || ( rFib.fcPlcfLst == rFib.fcPlfLfo )
            || ( !rFib.lcbPlcfLst )
            || ( !rFib.lcbPlfLfo ) ) return; // offensichtlich keine Listen da

    // Arrays anlegen
    pLSTInfos = new WW8LSTInfos;
    pLFOInfos = new WW8LFOInfos;
    sal_uInt16 nList;
    sal_uInt16 nLfo;
    sal_Bool   bLVLOk=sal_True;
    sal_uInt8  aBits1;
    String sPostfix;

    long nOriginalPos = rSt.Tell();
    //
    // 1. PLCF LST auslesen und die Listen Vorlagen im Writer anlegen
    //
    rSt.Seek( rFib.fcPlcfLst );
    sal_uInt16 nListCount;
    rSt >> nListCount;
    sal_Bool bOk = 0 < nListCount;
    if( bOk )
    {
        WW8LST aLST;
        //
        // 1.1 alle LST einlesen
        //
        for(nList = 0; nList < nListCount; nList++)
        {
            bOk = sal_False;
            memset(&aLST, 0, sizeof( aLST ));
            sal_uInt16 nLevel;
            //
            // 1.1.1 Daten einlesen
            //
            rSt >> aLST.nIdLst;
            rSt >> aLST.nTplC;
            for(nLevel = 0; nLevel < nWW8MaxListLevel; nLevel++)
            {
                rSt >> aLST.aIdSty[ nLevel ];
            }

         
            rSt >> aBits1;

            rSt.SeekRel( 1 );

            if( 0 != rSt.GetError() ) break;

            if( aBits1 & 0x01 ) 
                aLST.bSimpleList = sal_True;
            if( aBits1 & 0x02 ) 
                aLST.bRestartHdn = sal_True;

            // 1.1.2 new NumRule inserted in Doc and  WW8LSTInfo marked

            /*
            #i1869#
            In word 2000 microsoft got rid of creating new "simple lists" with
            only 1 level, all new lists are created with 9 levels. To hack it
            so that the list types formerly known as simple lists still have
            their own tab page to themselves one of the reserved bits is used
            to show that a given list is to be in the simple list tabpage.
            This has now nothing to do with the actual number of list level a
            list has, only how many will be shown in the user interface.

            i.e. create a simple list in 2000 and open it in 97 and 97 will
            claim (correctly) that it is an outline list. We can set our
            continous flag in these lists to store this information.
            */
            SwNumRule* pMyNumRule = CreateNextRule(
                aLST.bSimpleList || (aBits1 & 0x10));

            WW8LSTInfo* pLSTInfo = new WW8LSTInfo(pMyNumRule, aLST, nList);
            pLSTInfos->Insert( pLSTInfo );
            nLSTInfos++;
            bOk = sal_True;
        }
    }

    if( bOk )
    {
        //
        // 1.2 alle LVL aller aLST einlesen
        //
        sal_uInt8 nLevel;
        for(nList = 0; nList < nLSTInfos; nList++)
        {
            bOk = sal_False;
            WW8LSTInfo* pListInfo = GetLSTByStreamPos( nList );
            if( !pListInfo || !pListInfo->pNumRule ) break;
            SwNumRule& rMyNumRule = *pListInfo->pNumRule;
            //
            // 1.2.1 betreffende(n) LVL(s) fuer diese aLST einlesen
            //
            sal_uInt16 nLvlCount=pListInfo->bSimpleList ? 1 : nWW8MaxListLevel;
            for(nLevel = 0; nLevel < nLvlCount; nLevel++)
            {
                SwNumFmt aNumFmt( rMyNumRule.Get( nLevel ) );
                // LVLF einlesen
                bLVLOk = ReadLVL( aNumFmt, pListInfo->aItemSet[nLevel],
                    pListInfo->aIdSty[nLevel], sal_True );
                if( !bLVLOk ) 
                    break;
                // und in die rMyNumRule aufnehmen
                rMyNumRule.Set( nLevel, aNumFmt );
            }
            if( !bLVLOk ) 
                break;
            //
            // 1.2.2 die ItemPools mit den CHPx Einstellungen der verschiedenen
            //       Level miteinander vergleichen und ggfs. Style(s) erzeugen
            //
            sal_Bool bDummy;
            for(nLevel = 0; nLevel < nLvlCount; nLevel++)
                AdjustLVL( nLevel, rMyNumRule, pListInfo->aItemSet,
                                               pListInfo->aCharFmt, bDummy );
            //
            // 1.2.3 ItemPools leeren und loeschen
            //
            for(nLevel = 0; nLevel < nLvlCount; nLevel++)
                delete pListInfo->aItemSet[ nLevel ];
            bOk = sal_True;
        }
    }
    if( !bOk )
    {
        // Fehler aufgetreten - LSTInfos abraeumen !!!

        ;
    }
    //
    // 2. PLF LFO auslesen und speichern
    //
    long nLfoCount;
    if( bOk )
    {
        nLfoCount = 0;
        rSt.Seek( rFib.fcPlfLfo );
        rSt >> nLfoCount;
        if( 0 >= nLfoCount ) 
            bOk = sal_False;
    }
    if( bOk )
    {
        WW8LFO aLFO;
        //
        // 2.1 alle LFO einlesen
        //
        for(nLfo = 0; nLfo < nLfoCount; nLfo++)
        {
            bOk = sal_False;
            memset(&aLFO, 0, sizeof( aLFO ));
            rSt >> aLFO.nIdLst;
            rSt.SeekRel( 8 );
            rSt >> aLFO.nLfoLvl;
            rSt.SeekRel( 3 );
            // soviele Overrides existieren
            if( (nWW8MaxListLevel < aLFO.nLfoLvl) && (0 != rSt.GetError()) ) 
                break;

            // die Parent NumRule der entsprechenden Liste ermitteln
            WW8LSTInfo* pParentListInfo = GetLSTByListId( aLFO.nIdLst );
            if( !pParentListInfo ) break;
            // hier, im ersten Schritt, erst mal diese NumRule festhalten
            aLFO.pNumRule = pParentListInfo->pNumRule;
            if( !aLFO.pNumRule ) break;
            // hat die Liste mehrere Level ?
            aLFO.bSimpleList = pParentListInfo->bSimpleList;
            // und rein ins Merk-Array mit dem Teil
            WW8LFOInfo* pLFOInfo = new WW8LFOInfo( aLFO );
            pLFOInfos->Insert(pLFOInfo, pLFOInfos->Count());
            nLFOInfos++;
            bOk = sal_True;
        }
    }
    if( bOk )
    {
        //
        // 2.2 fuer alle LFO die zugehoerigen LFOLVL einlesen
        //
        WW8LFOLVL aLFOLVL;
        for(nLfo = 0; nLfo < nLFOInfos; nLfo++)
        {
            bOk = sal_False;
            WW8LFOInfo* pLFOInfo = pLFOInfos->GetObject( nLfo );
            if( !pLFOInfo ) 
                break;
            // stehen hierfuer ueberhaupt LFOLVL an ?
            if( pLFOInfo->bOverride )	
            {
                WW8LSTInfo* pParentListInfo = GetLSTByListId(pLFOInfo->nIdLst);
                if (!pParentListInfo) 
                    break;
                //
                // 2.2.1 eine neue NumRule fuer diese Liste anlegen
                //
                SwNumRule* pParentNumRule =	pLFOInfo->pNumRule;
                if( !pParentNumRule )
                    break;
                // Nauemsprefix aufbauen: fuer NumRule-Name (eventuell)
                // und (falls vorhanden) fuer Style-Name (dann auf jeden Fall)
                String sPrefix(CREATE_CONST_ASC( "WW8NumSt" ));
                sPrefix += String::CreateFromInt32( nLfo + 1 );
                // jetzt dem pNumRule seinen RICHTIGEN Wert zuweisen !!!
                // (bis dahin war hier die Parent NumRule vermerkt )
                //
                // Dazu erst mal nachsehen, ob ein Style diesen LFO
                // referenziert:
                if( USHRT_MAX > rReader.StyleUsingLFO( nLfo ) )
                {
                    sal_uInt16 nRul = rDoc.MakeNumRule(
                        rDoc.GetUniqueNumRuleName( &sPrefix ), pParentNumRule);
                    pLFOInfo->pNumRule = rDoc.GetNumRuleTbl()[ nRul ];
                    pLFOInfo->pNumRule->SetAutoRule( sal_False );
                }
                else
                {
                    sal_uInt16 nRul = rDoc.MakeNumRule(
                        rDoc.GetUniqueNumRuleName(), pParentNumRule);
                    pLFOInfo->pNumRule = rDoc.GetNumRuleTbl()[ nRul ];
                    pLFOInfo->pNumRule->SetAutoRule( sal_True );	// = default
                }
                //
                // 2.2.2 alle LFOLVL (und ggfs. LVL) fuer die neue NumRule
                // einlesen
                //
                WW8aISet aItemSet;		 // Zeichenattribute aus GrpprlChpx
                WW8aCFmt aCharFmt;		 // Zeichen Style Pointer
                memset(&aItemSet, 0,  sizeof( aItemSet ));
                memset(&aCharFmt, 0,  sizeof( aCharFmt ));

                for(sal_uInt8 nLevel = 0; nLevel < pLFOInfo->nLfoLvl; nLevel++)
                {
                    bLVLOk = sal_False;
                    memset(&aLFOLVL, 0, sizeof( aLFOLVL ));

                    //
                    // 2.2.2.0 fuehrende 0xFF ueberspringen
                    //
                    sal_uInt8 n1;
                    do
                    {
                        rSt >> n1;
                        if( 0 != rSt.GetError() ) 
                            break;
                    }
                    while( 0xFF == n1 );
                    rSt.SeekRel( -1 );

                    //
                    // 2.2.2.1 den LFOLVL einlesen
                    //
                    rSt >> aLFOLVL.nStartAt;
                    rSt >> aBits1;
                    rSt.SeekRel( 3 );
                    if( 0 != rSt.GetError() ) 
                        break;

                    // beachte: Die Witzbolde bei MS quetschen die
                    // Override-Level-Nummer in vier Bits hinein, damit sie
                    // wieder einen Grund haben, ihr Dateiformat zu aendern,
                    // falls ihnen einfaellt, dass sie eigentlich doch gerne
                    // bis zu 16 Listen-Level haetten.  Wir tun das *nicht*
                    // (siehe Kommentar oben bei "struct
                    // WW8LFOInfo")
                    aLFOLVL.nLevel = aBits1 & 0x0F;
                    if( (0xFF > aBits1) && (0 <= aLFOLVL.nLevel)
                        && (nWW8MaxListLevel > aLFOLVL.nLevel) )
                    {
                        if( aBits1 & 0x10 )
                            aLFOLVL.bStartAt = sal_True;
                        //
                        // 2.2.2.2 eventuell auch den zugehoerigen LVL einlesen
                        //
                        SwNumFmt aNumFmt( 
                            pLFOInfo->pNumRule->Get( aLFOLVL.nLevel ) );
                        if( aBits1 & 0x20 )
                        {
                            aLFOLVL.bFormat = sal_True;
                            // falls bStartup sal_True, hier den Startup-Level
                            // durch den im LVL vermerkten ersetzen LVLF
                            // einlesen
                            bLVLOk= ReadLVL(aNumFmt, aItemSet[nLevel], 
                                pParentListInfo->aIdSty[nLevel], 
                                aLFOLVL.bStartAt );

                            if( !bLVLOk ) 
                                break;
                        }
                        else if( aLFOLVL.bStartAt )
                            aNumFmt.SetStart( (sal_uInt16)aLFOLVL.nStartAt );
                        //
                        // 2.2.2.3 das NumFmt in die NumRule aufnehmen
                        //
                        pLFOInfo->pNumRule->Set( nLevel, aNumFmt );
                    }
                    bLVLOk = sal_True;
                }
                if( !bLVLOk ) 
                    break;
                //
                // 2.2.3 die LVL der neuen NumRule anpassen
                //
                sal_uInt16 aFlagsNewCharFmt = 0;
                sal_Bool bNewCharFmtCreated = sal_False;
                for(nLevel = 0; nLevel < pLFOInfo->nLfoLvl; nLevel++)
                {
                    AdjustLVL( nLevel, *pLFOInfo->pNumRule, aItemSet, aCharFmt,
                        bNewCharFmtCreated, sPrefix );
                    if( bNewCharFmtCreated )
                        aFlagsNewCharFmt += (1 << nLevel);
                }
                //
                // 2.2.4 ItemPools leeren und loeschen
                //
                for(nLevel = 0; nLevel < pLFOInfo->nLfoLvl; nLevel++)
                    delete aItemSet[ nLevel ];
                //
                // 2.2.5 falls diese NumRule identische Einstellungen und
                //       Formatierungen hat wie ihre Parent-Rule, loeschen
                //       wir sie wieder und nehmen stattdessen die alte.
                //
                if( LFOequaltoLST( *pLFOInfo ) )
                {
                    // 2.2.5.1 ggfs die an den einzelnen Leveln haengenden
                    //         Char-Formate freigeben und loeschen
                    //
                    for(nLevel = 0; nLevel < pLFOInfo->nLfoLvl; nLevel++)
                        if( aFlagsNewCharFmt & (1 << nLevel) )
                        {
                            rDoc.DelCharFmt(
                                pLFOInfo->pNumRule->Get(nLevel ).GetCharFmt());
                        }
                    //
                    // 2.2.5.2 jetzt die NumRule freigeben und die alte nehmen
                    //
                    rDoc.DelNumRule( pLFOInfo->pNumRule->GetName() );
                    pLFOInfo->pNumRule = pParentNumRule;
                }
                bOk = sal_True;
            }
        }
    }
    if( !bOk )
    {
        // Fehler aufgetreten - LSTInfos und LFOInfos abraeumen !!!
        ;
    }
    // und schon sind wir fertig!
    rSt.Seek( nOriginalPos );
}

WW8ListManager::~WW8ListManager()
{
    /*
    named lists remain in doc!!!
    unnamed lists are deleted when unused 
    pLSTInfos and pLFOInfos are in any case destructed
    */
    sal_uInt16 nInfo;
    if( pLSTInfos )
    {
        for(nInfo = pLSTInfos->Count(); nInfo; )
        {
            WW8LSTInfo& rActInfo = *pLSTInfos->GetObject( --nInfo );
            if (rActInfo.pNumRule && !rActInfo.bUsedInDoc
                    && rActInfo.pNumRule->IsAutoRule())
            {
                rDoc.DelNumRule( rActInfo.pNumRule->GetName() );
            }
        }
        delete pLSTInfos;
    }
    if( pLFOInfos )
    {
        for(nInfo = pLFOInfos->Count(); nInfo; )
        {
            WW8LFOInfo& rActInfo = *pLFOInfos->GetObject( --nInfo );
            if (rActInfo.bOverride &&  rActInfo.pNumRule
                && !rActInfo.bUsedInDoc && rActInfo.pNumRule->IsAutoRule())
            {
                rDoc.DelNumRule( rActInfo.pNumRule->GetName() );
            }
        }
        delete pLFOInfos;
    }
}

SwNumRule* WW8ListManager::GetNumRuleForActivation(sal_uInt16 nLFOPosition, 
    sal_uInt8 nLevel) const
{
    if( nLFOInfos <= nLFOPosition ) 
        return 0;

    WW8LFOInfo* pLFOInfo = pLFOInfos->GetObject( nLFOPosition );

    if( !pLFOInfo )
        return 0;

    pLFOInfo->bUsedInDoc = sal_True;

    if( !pLFOInfo->pNumRule )
        return 0;

    /*
    #i1869#
    If this list has had its bits set in word 2000 to pretend that it is a
    simple list from the point of view of the user, then it is almost
    certainly a simple continous list, and we will try to keep it like that.
    Otherwise when we save again it will be shown as the true outline list
    that it is, confusing the user that just wanted what they thought was a
    simple list. On the otherhand it is possible that some of the other levels
    were used by the user, in which case we will not pretend anymore that it
    is a simple list. Something that word 2000 does anyway, that 97 didn't, to
    my bewilderment.
    */
    if (nLevel && pLFOInfo->pNumRule->IsContinusNum())
        pLFOInfo->pNumRule->SetContinusNum(FALSE);

    if( (!pLFOInfo->bOverride) && (!pLFOInfo->bLSTbUIDSet) )
    {
        WW8LSTInfo* pParentListInfo = GetLSTByListId( pLFOInfo->nIdLst );
        if( pParentListInfo )
            pParentListInfo->bUsedInDoc = sal_True;
        pLFOInfo->bLSTbUIDSet = sal_True;
    }

    return pLFOInfo->pNumRule;
}



sal_Bool WW8ListManager::IsSimpleList(sal_uInt16 nLFOPosition) const
{
    if (nLFOInfos <= nLFOPosition) 
        return sal_False;

    WW8LFOInfo* pLFOInfo = pLFOInfos->GetObject( nLFOPosition );

    if( !pLFOInfo )
        return sal_False;
    return pLFOInfo->bSimpleList;
}




//----------------------------------------------------------------------------
//			SwWW8ImplReader:  anhaengen einer Liste an einen Style oder Absatz
//----------------------------------------------------------------------------


sal_Bool SwWW8ImplReader::SetTxtFmtCollAndListLevel(const SwPaM& rRg,
    SwWW8StyInf& rStyleInfo)
{
    sal_Bool bRes = sal_True;
    if( rStyleInfo.pFmt && rStyleInfo.bColl )
    {
        bRes = rDoc.SetTxtFmtColl(rRg, (SwTxtFmtColl*)rStyleInfo.pFmt);
        SwTxtNode* pTxtNode = pPaM->GetNode()->GetTxtNode();
        ASSERT( pTxtNode, "No Text-Node at PaM-Position" );
        if( !IsInvalidOrToBeMergedTabCell() )
            pTxtNode->SwCntntNode::ResetAttr( RES_PARATR_NUMRULE );

        if( !rStyleInfo.pOutlineNumrule )
        {
            if(    (USHRT_MAX        > rStyleInfo.nLFOIndex )
                && (nWW8MaxListLevel > rStyleInfo.nListLevel) )
                RegisterNumFmtOnTxtNode( rStyleInfo.nLFOIndex,
                                         rStyleInfo.nListLevel, sal_False);
            else
                pTxtNode->UpdateNum( SwNodeNum(NO_NUMBERING) );

            if( rStyleInfo.bHasStyNumRule && pTxtNode )
            {
                pTxtNode->SetNumLSpace( sal_False );
            }
        }
    }
    return bRes;
}

SwNumRule* SwWW8ImplReader::SyncStyleIndentWithList( SwWW8StyInf &rStyle, 
    SwNumRule* pRule, BYTE nLevel)
{
    /*
    #i1886#
    When a style has a numbering rule attached to it in word, the numbering
    rule contains indentation information for the style. The style itself also
    has indentation info, generally setting one in word sets the indentation
    in the list formatting for the level in the list to be the same, so the
    two remain in sync. But it is possible for them to fall out of sync. In
    which case the styles indentation is the real indentation for the level of
    the list that is attached to it. The solution is to create a new list
    based upon the one registered on the style and change the formatting of
    the level that is effectively overridden by the styles formatting info.

    Once the styles numbering is correctly setup then conceptionally setting a
    numbering style on anything in word removes all its original indentation
    information and the lists is used instead, so if we have a style which has
    a outline list formatting and it happens to be based on a style that has
    indentation, then that inherited indentation is stripped from the style.
    */
    const SfxPoolItem *pItem;
    if (SFX_ITEM_SET == rStyle.pFmt->GetItemState(RES_LR_SPACE,FALSE,&pItem))
    {
        const SvxLRSpaceItem *pLR = (const SvxLRSpaceItem *)pItem;
        const SwNumFmt& rRule = pRule->Get( nLevel );

        BOOL bRequired = FALSE;
        if ( rRule.GetAbsLSpace() != pLR->GetTxtLeft() )
            bRequired = TRUE;
        else if(pLR->GetTxtFirstLineOfst() && (pLR->GetTxtFirstLineOfst() != 0))
        {
            if (rRule.GetFirstLineOffset() != pLR->GetTxtFirstLineOfst())
                bRequired = TRUE;
        }

        if (bRequired)
        {
            //new list required with these settings.
            SwNumRule *pNewRule = pLstManager->CreateNextRule(
                pRule->IsContinusNum());

            for (int i=0;i<MAXLEVEL;i++)
            {
                const SwNumFmt& rSubRule = pRule->Get(i);
                pNewRule->Set( i, rSubRule );
            }

            SwNumFmt aRule = pRule->Get(nLevel);
            aRule.SetAbsLSpace( pLR->GetTxtLeft() );
            if (pLR->GetTxtFirstLineOfst() != 1)
                aRule.SetFirstLineOffset( pLR->GetTxtFirstLineOfst() );
            pNewRule->Set( nLevel, aRule );
            pRule = pNewRule;
        }
    }
    SvxLRSpaceItem aLR;
    rStyle.pFmt->SetAttr( aLR );
    return pRule;
}

void SwWW8ImplReader::RegisterNumFmtOnStyle(sal_uInt16 nStyle, 
    sal_uInt16 nActLFO, sal_uInt8 nActLevel)
{
    SwWW8StyInf &rStyleInf = pCollA[ nStyle ];
    if( rStyleInf.bValid )
    {
        // Phase 1: Nummerierungsattribute beim Einlesen einer StyleDef
        if( pAktColl )
        {
            // jetzt nur die Parameter vermerken: die tatsaechliche Liste wird
            // spaeter drangehaengt, wenn die Listendefinitionen gelesen sind...
            if( (USHRT_MAX > nActLFO) && (nWW8MaxListLevel > nActLevel))
            {
                rStyleInf.nLFOIndex  = nActLFO;
                rStyleInf.nListLevel = nActLevel;
            }
        }
        else
        // Phase 2: aktualisieren der StyleDef nach einlesen aller Listen
        {
            sal_uInt16 nLFO;
            sal_uInt8   nLevel;
            if( (USHRT_MAX > nActLFO) && (nWW8MaxListLevel > nActLevel))
            {
                // Plan A: die Werte fuer Listen- und Level-Nummer wurden
                // uebergeben
                nLFO   = nActLFO;
                nLevel = nActLevel;
            }
            else
            {
                // Plan B: die vorhin in Phase 1 gespeicherten Werte sind zu
                // nehmen
                nLFO   = rStyleInf.nLFOIndex;
                nLevel = rStyleInf.nListLevel;
            }
            if( (USHRT_MAX > nLFO) && (nWW8MaxListLevel > nLevel))
            {
                SwNumRule* pNmRule = 
                    pLstManager->GetNumRuleForActivation(nLFO,nLevel);

                if( pNmRule )
                {
                    pNmRule = SyncStyleIndentWithList(rStyleInf,pNmRule,nLevel);
                    if( MAXLEVEL > rStyleInf.nOutlineLevel )
                        rStyleInf.pOutlineNumrule = pNmRule;
                    else
                    {
                        rStyleInf.pFmt->SetAttr( 
                            SwNumRuleItem( pNmRule->GetName() ) );
                        rStyleInf.bHasStyNumRule = sal_True;
                    }
                }
            }
            else
            {
                //inherit numbering from base if not explicitly set for this
                //style
                if (rStyleInf.nBase < nStyle)
                {
                    rStyleInf.pOutlineNumrule =
                        pCollA[rStyleInf.nBase].pOutlineNumrule;
                    const SfxPoolItem* pItem;
                    if (pCollA[rStyleInf.nBase].pFmt && 
                    SFX_ITEM_SET == pCollA[rStyleInf.nBase].pFmt->
                        GetItemState(RES_PARATR_NUMRULE,FALSE,&pItem))
                    {
                        const SwNumRuleItem *pRule=(const SwNumRuleItem *)pItem;
                        rStyleInf.pFmt->SetAttr(*pRule);
                    }
                    rStyleInf.bHasStyNumRule = 
                        pCollA[rStyleInf.nBase].bHasStyNumRule;
                    rStyleInf.nLFOIndex = pCollA[rStyleInf.nBase].nLFOIndex;
                    rStyleInf.nListLevel = pCollA[rStyleInf.nBase].nListLevel;
                }
            }
        }
    }
}

void SwWW8ImplReader::RegisterNumFmtOnTxtNode(sal_uInt16 nActLFO, 
    sal_uInt8 nActLevel, sal_Bool bSetAttr)
{
    // beachte: die Methode haengt die NumRule an den Text Node, falls
    // bSetAttr (dann muessen natuerlich vorher die Listen gelesen sein)
    // stellt sie NUR den Level ein, im Vertrauen darauf, dass am STYLE eine
    // NumRule haengt - dies wird NICHT ueberprueft !!!

    if (pLstManager) // sind die Listendeklarationen gelesen?
    {
        const SwNumRule* pRule = bSetAttr ? 
            pLstManager->GetNumRuleForActivation( nActLFO, nActLevel) : 0;

        if( pRule || !bSetAttr )
        {
            SwTxtNode* pTxtNode = pPaM->GetNode()->GetTxtNode();
            ASSERT( pTxtNode, "Kein Text-Node an PaM-Position" );

            if (bSetAttr)
            {
                pTxtNode->SwCntntNode::SetAttr(SwNumRuleItem(pRule->GetName()));

                SvxLRSpaceItem aLR(*(SvxLRSpaceItem*)GetFmtAttr(RES_LR_SPACE));
                if( 1 < aLR.GetTxtFirstLineOfst() )
                {
                    aLR.SetTxtFirstLineOfst( 1 );
                    pTxtNode->SwCntntNode::SetAttr(aLR);
                }
            }

            pTxtNode->SetNumLSpace( bSetAttr );

            pTxtNode->UpdateNum( SwNodeNum( nActLevel ) );
        }
    }
}

void SwWW8ImplReader::RegisterNumFmt(sal_uInt16 nActLFO, sal_uInt8 nActLevel)
{
    // sind wir erst beim Einlesen der StyleDef ?
    if( pAktColl )
        RegisterNumFmtOnStyle( nAktColl , nActLFO, nActLevel);
    else
        RegisterNumFmtOnTxtNode(nActLFO, nActLevel);
}

void SwWW8ImplReader::Read_ListLevel(sal_uInt16, const sal_uInt8* pData,
    short nLen)
{
    if( nLen < 0 )
    {
        // aktuelle Liste ist hier zu Ende, was ist zu tun ???
        nListLevel = nWW8MaxListLevel;
        if (pStyles && !bVer67)
            pStyles->nWwNumLevel = 0;
    }
    else
    {
        // Sicherheitspruefung auf NIL Pointer
        if( !pData ) return;
        // die Streamdaten sind hier Null basiert, so wie wir es brauchen
        nListLevel = *pData;

        if (pStyles && !bVer67)
        {
            /*
            #94672#
            if this is the case, then if the numbering is actually stored in
            winword 6 format, and its likely that sprmPIlvl has been abused
            to set the ww6 list level information which we will need when we
            reach the true ww6 list def.  So set it now
            */
            pStyles->nWwNumLevel = nListLevel;
        }

        if( nWW8MaxListLevel <= nListLevel )
            nListLevel = nWW8MaxListLevel;
        else
        if( (USHRT_MAX > nLFOPosition) && (nWW8MaxListLevel > nListLevel))
        {
            RegisterNumFmt(nLFOPosition, nListLevel);
            nLFOPosition = USHRT_MAX;
            nListLevel   = nWW8MaxListLevel;
        }
    }
}

void SwWW8ImplReader::Read_LFOPosition(sal_uInt16, const sal_uInt8* pData,
    short nLen)
{
    if( nLen < 0 )
    {
        // aktueller Level ist hier zu Ende, was ist zu tun ???
        nLFOPosition = USHRT_MAX;
        nListLevel   = nWW8MaxListLevel;
    }
    else
    {
        // Sicherheitspruefung auf NIL Pointer
        if( !pData ) 
            return;
        short nData = SVBT16ToShort( pData );
        if( 0 >= nData )
        {
            // expliziet die Numerierung abschalten
            // sind wir erst beim Einlesen der StyleDef ?
            if( pAktColl )
                pAktColl->SetAttr( *GetDfltAttr( RES_PARATR_NUMRULE ) );
            else
            {
                SwTxtNode* pTxtNode = pPaM->GetNode()->GetTxtNode();
                if( pTxtNode )
                {
                    pTxtNode->SwCntntNode::SetAttr(
                                *GetDfltAttr( RES_PARATR_NUMRULE ) );
                    pTxtNode->UpdateNum( SwNodeNum( NO_NUMBERING ) );
                }
            }

            /*
            #94672#
            If you have a paragraph in word with left and/or hanging indent
            and remove its numbering, then the indentation appears to get
            reset, but not back to the base style, instead its goes to a blank
            setting.
            Unless its a broken ww6 list in 97 in which case more hackery is
            required, some more details about that in
            ww8par6.cxx#SwWW8ImplReader::Read_LR
            */
            SvxLRSpaceItem aLR;
            if (pCollA[nAktColl].bHasBrokenWW6List)
            {
                const SvxLRSpaceItem &rLR = (const SvxLRSpaceItem&) 
                    pCollA[nAktColl].pFmt->GetAttr( RES_LR_SPACE );
                aLR.SetTxtFirstLineOfst(-rLR.GetTxtFirstLineOfst());
            }
            NewAttr( aLR );

            nLFOPosition = USHRT_MAX;
        }
        else
        {
            nLFOPosition = (sal_uInt16)nData-1;
            /*
            #94672#
            If we are a ww8+ style with ww7- style lists then there is a
            bizarre broken word bug where when the list is removed from a para
            the ww6 list first line indent still affects the first line
            indentation.  Setting this flag will allow us to recover from this
            braindeadness
            */
            if( pAktColl && (nLFOPosition == 2047-1) )
                pCollA[nAktColl].bHasBrokenWW6List = TRUE;

            // die Streamdaten sind hier 1 basiert, wir ziehen EINS ab
            if ( (USHRT_MAX > nLFOPosition) && (nLFOPosition != 2047-1) )
            {
                /*
                    traurig: WW8 speichert manchmal keine Level-Attribute ab,
                    auch wenn die Liste *nicht* bSimpleList hat.

                    daher: nach Auftreten von sprmPIlfo sofort die Liste
                    anklemmen, auch wenn kein sprmPIlvl vorher kam!

                    schoener waere gewesen:
                    if(    (nWW8MaxListLevel == nListLevel) && pLstManager
                        && pLstManager->IsSimpleList(	nLFOPosition ) )
                            nListLevel = 0;
                */
                if(nWW8MaxListLevel == nListLevel)
                    nListLevel = 0;
                if(nWW8MaxListLevel > nListLevel)
                {
                    RegisterNumFmt(nLFOPosition, nListLevel);
                    nLFOPosition = USHRT_MAX;
                    nListLevel   = nWW8MaxListLevel;
                }
            }
        }
    }
}

// -------------------------------------------------------------------
// ------------------------- Reading Controls ------------------------
// -------------------------------------------------------------------

BOOL SwWW8ImplReader::ImportFormulaControl(WW8FormulaControl &aFormula,
    WW8_CP nStart, SwWw8ControlType nWhich )
{
    BOOL bRet=FALSE;
    /*
     * Save the reader state and process the sprms for this anchor cp.
     * Doing so will set the nPicLocFc to the offset to find the hypertext
     * data in the data stream.
     */
    WW8_CP nEndCp = nStart+1; //Only interested in the single 0x01 character

    WW8ReaderSave aSave(this,nStart);

    WW8PLCFManResult aRes;
    nStart = pPlcxMan->Where();
    while(nStart <= nEndCp)
    {
        if ( pPlcxMan->Get(&aRes)
            && aRes.pMemPos && aRes.nSprmId )
        {
            //only interested in sprms which would set nPicLocFc
            if ( (68 == aRes.nSprmId) || (0x6A03 == aRes.nSprmId) )
            {
                Read_PicLoc( aRes.nSprmId, aRes.pMemPos+ 1+
                (8 > pWwFib->nVersion ? 0 : 1) + WW8SprmDataOfs(aRes.nSprmId),
                4);
                break;
            }
        }
        (*pPlcxMan)++;
        nStart = pPlcxMan->Where();
    }
    ULONG nOffset = nPicLocFc;
    aSave.Restore(this);

    ULONG nOldPos = pDataStream->Tell();
    WW8_PIC aPic;
    pDataStream->Seek( nOffset);
    PicRead( pDataStream, &aPic, bVer67);

    if((aPic.lcb > 0x3A) && !pDataStream->GetError() )
    {
        pDataStream->Seek( nPicLocFc + aPic.cbHeader );
        aFormula.FormulaRead(nWhich,pDataStream);
        bRet=TRUE;
    }

    /*
     There is a problem with aPic, the WW8_PIC is always used even though it
     is too big for the WW95 files, it needs to be modified to check the
     version C.
     */
    pDataStream->Seek( nOldPos );
    return(bRet);
}

BOOL SwMSConvertControls::InsertFormula(WW8FormulaControl &rFormula,
    uno::Reference <drawing::XShape> *pShapeRef)
{
    BOOL bRet=FALSE;

    awt::Size aSz;
    uno::Reference< form::XFormComponent> xFComp;
    uno::Reference < drawing::XShape> xShape;

    const uno::Reference< lang::XMultiServiceFactory > & rServiceFactory =
        GetServiceFactory();

    if(!rServiceFactory.is())
        return(FALSE);
    if (bRet = rFormula.Import(rServiceFactory, xFComp, aSz))
        bRet = InsertControl( xFComp, aSz, pShapeRef,FALSE );
    return bRet;
}

void WW8FormulaControl::FormulaRead(SwWw8ControlType nWhich,
    SvStream *pDataStream)
{
    UINT8 nField;
    UINT8 nTest;

    int nType=0;
    pDataStream->Read(&nTest, 1);
    if (nTest == 0xFF) //Guesswork time, difference between 97 and 95 ?
    {
        pDataStream->SeekRel(4);
        nType=1;
    }
    *pDataStream >> nField;
    fToolTip = nField & 0x01;
    fNoMark = (nField & 0x02)>>1;
    fUseSize = (nField & 0x04)>>2;
    fNumbersOnly= (nField & 0x08)>>3;
    fDateOnly = (nField & 0x10)>>4;
    fUnused = (nField & 0xE0)>>5;
    *pDataStream >> nSize;

    *pDataStream >> hpsCheckBox;
    if (nType == 0)
        pDataStream->SeekRel(2); //Guess

    rtl_TextEncoding eEnc = rRdr.eStructCharSet;
    sTitle = !nType ? WW8ReadPString( *pDataStream, eEnc, TRUE )
                    : WW8Read_xstz(   *pDataStream, 0,    TRUE );

    if (nWhich == WW8_CT_CHECKBOX)
        *pDataStream >> nChecked;
    else
        sDefault = !nType ? WW8ReadPString( *pDataStream, eEnc, TRUE )
                          : WW8Read_xstz(   *pDataStream, 0,    TRUE );

    sFormatting = !nType ? WW8ReadPString( *pDataStream, eEnc, TRUE )
                           : WW8Read_xstz(   *pDataStream, 0,    TRUE );

    sHelp = !nType ? WW8ReadPString( *pDataStream, eEnc, TRUE )
                     : WW8Read_xstz(   *pDataStream, 0,    TRUE );

    if( fToolTip )
        sToolTip = !nType ? WW8ReadPString( *pDataStream, eEnc, TRUE )
                               : WW8Read_xstz(   *pDataStream, 0,    TRUE );
}

WW8FormulaCheckBox::WW8FormulaCheckBox(SwWW8ImplReader &rR)
    : WW8FormulaControl( CREATE_CONST_ASC( "CheckBox" ), rR)
{
}

BOOL WW8FormulaCheckBox::Import(const uno::Reference <
    lang::XMultiServiceFactory> &rServiceFactory,
    uno::Reference <form::XFormComponent> &rFComp,awt::Size &rSz )
{
    uno::Reference< uno::XInterface > xCreate = rServiceFactory->createInstance(
        C2U("com.sun.star.form.component.CheckBox"));
    if( !xCreate.is() )
        return FALSE;

    rFComp = uno::Reference< form::XFormComponent >( xCreate, uno::UNO_QUERY );
    if( !rFComp.is() )
        return FALSE;

    uno::Reference< beans::XPropertySet > xPropSet( xCreate, uno::UNO_QUERY );

    rSz.Width = 16 * hpsCheckBox;
    rSz.Height = 16 * hpsCheckBox;

    uno::Any aTmp;
    if (sTitle.Len())
        aTmp <<= rtl::OUString(sTitle);
    else
        aTmp <<= rtl::OUString(sName);
    xPropSet->setPropertyValue(C2U("Name"), aTmp );

    aTmp <<= (sal_Int16)nChecked;
    xPropSet->setPropertyValue(C2U("DefaultState"), aTmp);

    if( sToolTip.Len() )
    {
        aTmp <<= rtl::OUString(sToolTip);
        xPropSet->setPropertyValue(C2U("HelpText"), aTmp );
    }

    return TRUE;

}

void WW8FormulaControl::SetOthersFromDoc(awt::Size &rSz,
    uno::Reference< beans::XPropertySet >& rPropSet)
{
    const struct CtrlFontMapEntry
    {
        USHORT nWhichId;
        const sal_Char* pPropNm;
    }
    aMapTable [] =
    {
        {RES_CHRATR_COLOR,		"TextColor"},
        {RES_CHRATR_COLOR,		"TextColor"},
        {RES_CHRATR_FONT,       "FontName"},
        {RES_CHRATR_FONTSIZE,   "FontHeight"},
        {RES_CHRATR_WEIGHT,     "FontWeight"},
        {RES_CHRATR_UNDERLINE,  "FontUnderline"},
        {RES_CHRATR_CROSSEDOUT, "FontStrikeout"},
        {RES_CHRATR_POSTURE,    "FontSlant"},
        {0,						0}
    };

    USHORT nDefSize;
    Font aFont;
    uno::Reference< beans::XPropertySetInfo > xPropSetInfo = 
        rPropSet->getPropertySetInfo();

    if (nSize)
        nDefSize = nSize;
    else if( sDefault.Len() )
        nDefSize = sDefault.Len();
    else
        nDefSize = 18;           //no chars, ms actually uses
                                //5 0x96 chars (-) as its default, but thats
                                //the default for an empty text area, which
                                //stretchs in size as text is entered so...

    uno::Any aTmp;
    for( const CtrlFontMapEntry* pMap = aMapTable; pMap->nWhichId; ++pMap )
    {
        BOOL bSet = TRUE;
        const SfxPoolItem* pItem = rRdr.GetFmtAttr( pMap->nWhichId );

        switch ( pMap->nWhichId )
        {
        case RES_CHRATR_COLOR:
            aTmp <<= (sal_Int32)((SvxColorItem*)pItem)->GetValue().GetRGBColor();
            aFont.SetColor( ((SvxColorItem*)pItem)->GetValue() );
            break;

        case RES_CHRATR_FONT:
            {
                const SvxFontItem *pFontItem = (SvxFontItem *)pItem;
                rtl::OUString sNm;
                if (xPropSetInfo->hasPropertyByName(sNm = C2U("FontStyleName")))
                {
                    aTmp <<= rtl::OUString(pFontItem->GetStyleName());
                    rPropSet->setPropertyValue( sNm, aTmp );
                }
                if (xPropSetInfo->hasPropertyByName(sNm = C2U("FontFamily")))
                {
                    aTmp <<= (sal_Int16)pFontItem->GetFamily();
                    rPropSet->setPropertyValue( sNm, aTmp );
                }
                if( xPropSetInfo->hasPropertyByName(sNm = C2U("FontCharset")))
                {
                    aTmp <<= (sal_Int16)pFontItem->GetCharSet();
                    rPropSet->setPropertyValue( sNm, aTmp );
                }
                if( xPropSetInfo->hasPropertyByName(sNm = C2U("FontPitch")))
                {
                    aTmp <<= (sal_Int16)pFontItem->GetPitch();
                    rPropSet->setPropertyValue( sNm, aTmp );
                }

                aTmp <<= rtl::OUString(pFontItem->GetFamilyName());
                aFont.SetName( pFontItem->GetFamilyName() );
                aFont.SetStyleName( pFontItem->GetStyleName() );
                aFont.SetFamily( pFontItem->GetFamily() );
                aFont.SetCharSet( pFontItem->GetCharSet() );
                aFont.SetPitch( pFontItem->GetPitch() );
            }
            break;
        case RES_CHRATR_FONTSIZE:
            {
                Size aSize( aFont.GetSize().Width(),
                            ((SvxFontHeightItem*)pItem)->GetHeight() );
                aTmp <<= ((float)aSize.Height()) / 20. ;

                aFont.SetSize( OutputDevice::LogicToLogic( aSize, MAP_TWIP, 
                    MAP_100TH_MM ) );
            }
            break;
        case RES_CHRATR_WEIGHT:
            aTmp <<= (float)VCLUnoHelper::ConvertFontWeight(
                ((SvxWeightItem*)pItem)->GetWeight() );
            aFont.SetWeight( ((SvxWeightItem*)pItem)->GetWeight() );
            break;
        case RES_CHRATR_UNDERLINE:
            aTmp <<= (sal_Int16)( ((SvxUnderlineItem*)pItem)->GetUnderline() );
            aFont.SetUnderline( ((SvxUnderlineItem*)pItem)->GetUnderline() );
            break;
        case RES_CHRATR_CROSSEDOUT:
            aTmp <<= (sal_Int16)( ((SvxCrossedOutItem*)pItem)->GetStrikeout() );
            aFont.SetStrikeout( ((SvxCrossedOutItem*)pItem)->GetStrikeout() );
            break;
        case RES_CHRATR_POSTURE:
            aTmp <<= (sal_Int16)( ((SvxPostureItem*)pItem)->GetPosture() );
            aFont.SetItalic( ((SvxPostureItem*)pItem)->GetPosture() );
            break;
        default:
            bSet = FALSE;
            break;
        }

        rtl::OUString sNm;
        if( bSet && xPropSetInfo->hasPropertyByName( sNm = C2U(pMap->pPropNm)) )
        {
            rPropSet->setPropertyValue(sNm, aTmp);
        }
    }
    // now calculate the size of the control
    String sExpText;
    sExpText.Fill( nDefSize, 'M' );
    OutputDevice* pOut = Application::GetDefaultDevice();
    pOut->Push( PUSH_FONT | PUSH_MAPMODE );
    pOut->SetMapMode( MapMode( MAP_100TH_MM ));
    pOut->SetFont( aFont );
    rSz.Width  = pOut->GetTextWidth( sExpText );
    rSz.Height = pOut->GetTextHeight();

    pOut->Pop();
}

WW8FormulaEditBox::WW8FormulaEditBox(SwWW8ImplReader &rR)
    : WW8FormulaControl( CREATE_CONST_ASC( "TextField" ) ,rR)
{
}

BOOL WW8FormulaEditBox::Import(const uno::Reference<
    lang::XMultiServiceFactory >& rServiceFactory,
    uno::Reference< form::XFormComponent >& rFComp, awt::Size &rSz)
{
    uno::Reference< uno::XInterface > xCreate = rServiceFactory->createInstance(
        C2U("com.sun.star.form.component.TextField"));

    if( !xCreate.is() )
        return FALSE;

    rFComp = uno::Reference< form::XFormComponent >( xCreate, uno::UNO_QUERY );
    if( !rFComp.is() )
        return FALSE;

    uno::Reference< beans::XPropertySet > xPropSet( xCreate, uno::UNO_QUERY );

    uno::Any aTmp;
    if( sTitle.Len() )
        aTmp <<= rtl::OUString(sTitle);
    else
        aTmp <<= rtl::OUString(sName);
    xPropSet->setPropertyValue(C2U("Name"), aTmp);

    BOOL bTemp = FALSE;
    aTmp.setValue(&bTemp, ::getBooleanCppuType());
    xPropSet->setPropertyValue(C2U("MultiLine"), aTmp);

    aTmp <<= (sal_Int16)0;	//No Border
    xPropSet->setPropertyValue(C2U("Border"), aTmp);

    if( sDefault.Len() )
    {
        aTmp <<= rtl::OUString(sDefault);
        xPropSet->setPropertyValue(C2U("DefaultText"), aTmp);
    }

    rSz.Width = 300;
    rSz.Height = 200;

    aTmp <<= (sal_Int32)(0x00C0C0C0);
    xPropSet->setPropertyValue(C2U("BackgroundColor"), aTmp);

    SetOthersFromDoc(rSz,xPropSet);

    aTmp <<= (sal_Int16)nSize;
    xPropSet->setPropertyValue(C2U("MaxTextLen"), aTmp);

    if( sToolTip.Len() )
    {
        aTmp <<= rtl::OUString(sToolTip);
        xPropSet->setPropertyValue(C2U("HelpText"), aTmp );
    }

    return TRUE;
}


BOOL SwMSConvertControls::InsertControl(
    const uno::Reference< form::XFormComponent > & rFComp,
    const awt::Size& rSize,
    uno::Reference< drawing::XShape > *pShape,
    BOOL bFloatingCtrl)
{
    uno::Reference< drawing::XShape >  xShape;

    const uno::Reference< container::XIndexContainer > &rComps = GetFormComps();
    uno::Any aTmp( &rFComp, ::getCppuType((const uno::Reference<
        form::XFormComponent >*)0) );
    rComps->insertByIndex( rComps->getCount(), aTmp );

    const uno::Reference< lang::XMultiServiceFactory > &rServiceFactory =
        GetServiceFactory();
    if( !rServiceFactory.is() )
        return FALSE;

    uno::Reference< uno::XInterface > xCreate = rServiceFactory->createInstance(
        C2U("com.sun.star.drawing.ControlShape"));
    if( !xCreate.is() )
        return FALSE;

    xShape = uno::Reference< drawing::XShape >(xCreate, uno::UNO_QUERY);

    DBG_ASSERT(xShape.is(), "XShape nicht erhalten")
    xShape->setSize(rSize);

    uno::Reference< beans::XPropertySet >  xShapePropSet(
        xCreate, uno::UNO_QUERY );

    //I lay a small bet that this will change to
    //INT16 nTemp=TextContentAnchorType::AS_CHARACTER;
    INT16 nTemp;
    if (bFloatingCtrl)
        nTemp= text::TextContentAnchorType_AT_PARAGRAPH;
    else
        nTemp= text::TextContentAnchorType_AS_CHARACTER;

    aTmp <<= nTemp;
    xShapePropSet->setPropertyValue(C2U("AnchorType"), aTmp );

    nTemp= text::VertOrientation::TOP;
    aTmp <<= nTemp;
    xShapePropSet->setPropertyValue(C2U("VertOrient"), aTmp );

    uno::Reference< text::XText >  xDummyTxtRef;
    uno::Reference< text::XTextRange >  xTxtRg =
        new SwXTextRange( *pPaM, xDummyTxtRef );

    aTmp.setValue(&xTxtRg,::getCppuType((
        uno::Reference< text::XTextRange >*)0));
    xShapePropSet->setPropertyValue(C2U("TextRange"), aTmp );

    GetShapes()->add( xShape );

    // Das Control-Model am Control-Shape setzen
    uno::Reference< drawing::XControlShape >  xControlShape( xShape,
        uno::UNO_QUERY );
    uno::Reference< awt::XControlModel >  xControlModel( rFComp,
        uno::UNO_QUERY );
    xControlShape->setControl( xControlModel );

    if (pShape)
        *pShape = xShape;

    return TRUE;
}

